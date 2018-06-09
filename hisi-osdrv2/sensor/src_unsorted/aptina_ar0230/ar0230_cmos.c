#if !defined(__AR0230_CMOS_H_)
#define __AR0230_CMOS_H_

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "hi_comm_sns.h"
#include "hi_comm_video.h"
#include "hi_sns_ctrl.h"
#include "mpi_isp.h"
#include "mpi_ae.h"
#include "mpi_awb.h"
#include "mpi_af.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* End of #ifdef __cplusplus */

#define AR0230_ID 230

/* To change the mode of config. ifndef INIFILE_CONFIG_MODE, quick config mode.*/
/* else, cmos_cfg.ini file config mode*/
#ifdef INIFILE_CONFIG_MODE

extern AE_SENSOR_DEFAULT_S  g_AeDft[];
extern AWB_SENSOR_DEFAULT_S g_AwbDft[];
extern ISP_CMOS_DEFAULT_S   g_IspDft[];
extern HI_S32 Cmos_LoadINIPara(const HI_CHAR *pcName);
#else

#endif

/****************************************************************************
 * local variables                                                            *
 ****************************************************************************/

extern const unsigned int sensor_i2c_addr;
extern unsigned int sensor_addr_byte;
extern unsigned int sensor_data_byte;

#define FULL_LINES_MAX  (0xFFFF)

#define ANALOG_GAIN    (0x3060)
#define DIGITAL_GAIN   (0x305E)
#define FRAME_LINES    (0x300A)
#define EXPOSURE_TIME  (0x3012)
#define LINE_LEN_PCK   (0x300C)

#define SENSOR_2M_1080p30_MODE  (1) 
#define SENSOR_2M_1080p60_MODE  (2)

#define INCREASE_LINES (1) /* make real fps less than stand fps because NVR require*/
#define FRAME_LINES_2M_1080p  (1125+INCREASE_LINES)


// Max integration time for HDR mode: T1 max = min ( 70* 16, FLL*16/17), when ratio=16;
// we use a constant full lines in build-in wdr mode
#define LONG_EXP_SHT_CLIP     (FRAME_LINES_2M_1080p*16/17)
#define LONG_EXP_SHT_CLIP_PAL (FRAME_LINES_2M_1080p*30/25*16/17)

HI_U8 gu8SensorImageMode = SENSOR_2M_1080p30_MODE;
WDR_MODE_E genSensorMode = WDR_MODE_NONE;

static HI_U32 gu32FullLinesStd = FRAME_LINES_2M_1080p;
static HI_U32 gu32FullLines = FRAME_LINES_2M_1080p; 
static HI_BOOL bInit = HI_FALSE;
HI_BOOL bSensorInit = HI_FALSE; 

ISP_SNS_REGS_INFO_S g_stSnsRegsInfo = {0};
ISP_SNS_REGS_INFO_S g_stPreSnsRegsInfo = {0};

#define PATHLEN_MAX 256
#define CMOS_CFG_INI "ar0230_cfg.ini"
static char pcName[PATHLEN_MAX] = "configs/ar0230_cfg.ini";


/* AE default parameter and function */
#ifdef INIFILE_CONFIG_MODE
static HI_S32 cmos_get_ae_default(AE_SENSOR_DEFAULT_S *pstAeSnsDft)
{
    if (HI_NULL == pstAeSnsDft)
    {
        printf("null pointer when get ae default value!\n");
        return -1;
    }
    
    pstAeSnsDft->u32LinesPer500ms = gu32FullLinesStd*30/2;
    pstAeSnsDft->u32FullLinesStd = gu32FullLinesStd;
    pstAeSnsDft->u32FlickerFreq = 0;
    pstAeSnsDft->u32FullLinesMax = FULL_LINES_MAX;
    
    pstAeSnsDft->stIntTimeAccu.enAccuType = AE_ACCURACY_LINEAR;
    pstAeSnsDft->stIntTimeAccu.f32Accuracy = 1;
    pstAeSnsDft->stIntTimeAccu.f32Offset = 0;

    pstAeSnsDft->stAgainAccu.enAccuType = AE_ACCURACY_TABLE;
    pstAeSnsDft->stAgainAccu.f32Accuracy = 0.0078125;

    pstAeSnsDft->stDgainAccu.enAccuType = AE_ACCURACY_LINEAR;
    pstAeSnsDft->stDgainAccu.f32Accuracy = 0.0078125; 
    
    switch(genSensorMode)
    {
        case WDR_MODE_NONE:   /*linear mode*/
        {
            pstAeSnsDft->au8HistThresh[0] = 0xd;
            pstAeSnsDft->au8HistThresh[1] = 0x28;
            pstAeSnsDft->au8HistThresh[2] = 0x60;
            pstAeSnsDft->au8HistThresh[3] = 0x80;
            
            pstAeSnsDft->u8AeCompensation = g_AeDft[0].u8AeCompensation;
            
            pstAeSnsDft->u32MaxIntTime = gu32FullLinesStd - 2;
            pstAeSnsDft->u32MinIntTime = 2;
            pstAeSnsDft->u32MaxIntTimeTarget = g_AeDft[0].u32MaxIntTimeTarget;
            pstAeSnsDft->u32MinIntTimeTarget = g_AeDft[0].u32MinIntTimeTarget;
    
            pstAeSnsDft->u32MaxAgain = 12288;  
            pstAeSnsDft->u32MinAgain = 1024;
            pstAeSnsDft->u32MaxAgainTarget = g_AeDft[0].u32MaxAgainTarget;
            pstAeSnsDft->u32MinAgainTarget = g_AeDft[0].u32MinAgainTarget;
            
            pstAeSnsDft->u32MaxDgain = 2046;  
            pstAeSnsDft->u32MinDgain = 128;
            pstAeSnsDft->u32MaxDgainTarget = g_AeDft[0].u32MaxDgainTarget;
            pstAeSnsDft->u32MinDgainTarget = g_AeDft[0].u32MinDgainTarget;
          
            pstAeSnsDft->u32ISPDgainShift = g_AeDft[0].u32ISPDgainShift;
            pstAeSnsDft->u32MinISPDgainTarget = g_AeDft[0].u32MinISPDgainTarget;
            pstAeSnsDft->u32MaxISPDgainTarget = g_AeDft[0].u32MaxISPDgainTarget;    
            break;  
        }
        case WDR_MODE_BUILT_IN:
        {
            pstAeSnsDft->au8HistThresh[0] = 0xC;
            pstAeSnsDft->au8HistThresh[1] = 0x18;
            pstAeSnsDft->au8HistThresh[2] = 0x60;
            pstAeSnsDft->au8HistThresh[3] = 0x80;
           
            pstAeSnsDft->u8AeCompensation = g_AeDft[1].u8AeCompensation;
            
            pstAeSnsDft->u32MaxIntTime = LONG_EXP_SHT_CLIP;  
            pstAeSnsDft->u32MinIntTime = 32;
            pstAeSnsDft->u32MaxIntTimeTarget = g_AeDft[1].u32MaxIntTimeTarget;
            pstAeSnsDft->u32MinIntTimeTarget = g_AeDft[1].u32MinIntTimeTarget;
            
            pstAeSnsDft->u32MaxAgain = 2048; 
            pstAeSnsDft->u32MinAgain = 1024;
            pstAeSnsDft->u32MaxAgainTarget = g_AeDft[1].u32MaxAgainTarget;
            pstAeSnsDft->u32MinAgainTarget = g_AeDft[1].u32MinAgainTarget;
            
            pstAeSnsDft->u32MaxDgain = 384;
            pstAeSnsDft->u32MinDgain = 128;
            pstAeSnsDft->u32MaxDgainTarget = g_AeDft[1].u32MaxDgainTarget;
            pstAeSnsDft->u32MinDgainTarget = g_AeDft[1].u32MinDgainTarget;

            pstAeSnsDft->u32ISPDgainShift = g_AeDft[1].u32ISPDgainShift;
            pstAeSnsDft->u32MinISPDgainTarget = g_AeDft[1].u32MinISPDgainTarget;
            pstAeSnsDft->u32MaxISPDgainTarget = g_AeDft[1].u32MaxISPDgainTarget;

            break;
        }
        
        default:
            break;
    }
    return 0;
}

#else

static HI_S32 cmos_get_ae_default(AE_SENSOR_DEFAULT_S *pstAeSnsDft)
{
    if (HI_NULL == pstAeSnsDft)
    {
        printf("null pointer when get ae default value!\n");
        return -1;
    }
    
    pstAeSnsDft->u32LinesPer500ms = gu32FullLinesStd*30/2;
    pstAeSnsDft->u32FullLinesStd = gu32FullLinesStd;
    pstAeSnsDft->u32FlickerFreq = 0;
    pstAeSnsDft->u32FullLinesMax = FULL_LINES_MAX;

    pstAeSnsDft->stIntTimeAccu.enAccuType = AE_ACCURACY_LINEAR;
    pstAeSnsDft->stIntTimeAccu.f32Accuracy = 1;
    pstAeSnsDft->stIntTimeAccu.f32Offset = 0.33;

    pstAeSnsDft->stAgainAccu.enAccuType = AE_ACCURACY_TABLE;
    pstAeSnsDft->stAgainAccu.f32Accuracy = 0.0078125;

    pstAeSnsDft->stDgainAccu.enAccuType = AE_ACCURACY_LINEAR;
    pstAeSnsDft->stDgainAccu.f32Accuracy = 0.0078125;    

    pstAeSnsDft->u32ISPDgainShift = 8;
    pstAeSnsDft->u32MinISPDgainTarget = 1 << pstAeSnsDft->u32ISPDgainShift;
    pstAeSnsDft->u32MaxISPDgainTarget = 4 << pstAeSnsDft->u32ISPDgainShift;    

    switch(genSensorMode)
    {
        case WDR_MODE_NONE:   /*linear mode*/
        {
            pstAeSnsDft->au8HistThresh[0] = 0xd;
            pstAeSnsDft->au8HistThresh[1] = 0x28;
            pstAeSnsDft->au8HistThresh[2] = 0x60;
            pstAeSnsDft->au8HistThresh[3] = 0x80;
            
            pstAeSnsDft->u8AeCompensation = 0x38;
            
            pstAeSnsDft->u32MaxIntTime = gu32FullLinesStd - 2;
            pstAeSnsDft->u32MinIntTime = 2;    
            pstAeSnsDft->u32MaxIntTimeTarget = 65535;
            pstAeSnsDft->u32MinIntTimeTarget = 2;

            pstAeSnsDft->u32MaxAgain = 29030;  
            pstAeSnsDft->u32MinAgain = 1024;
            pstAeSnsDft->u32MaxAgainTarget = pstAeSnsDft->u32MaxAgain;
            pstAeSnsDft->u32MinAgainTarget = pstAeSnsDft->u32MinAgain;
            
            pstAeSnsDft->u32MaxDgain = 2046; 
            pstAeSnsDft->u32MinDgain = 128;
            pstAeSnsDft->u32MaxDgainTarget = pstAeSnsDft->u32MaxDgain;
            pstAeSnsDft->u32MinDgainTarget = pstAeSnsDft->u32MinDgain;            
            break; 
        }
        case WDR_MODE_BUILT_IN:
        {
            pstAeSnsDft->au8HistThresh[0] = 0xC;
            pstAeSnsDft->au8HistThresh[1] = 0x18;
            pstAeSnsDft->au8HistThresh[2] = 0x60;
            pstAeSnsDft->au8HistThresh[3] = 0x80;
           
            pstAeSnsDft->u8AeCompensation = 0x38;
            
            pstAeSnsDft->u32MaxIntTime = LONG_EXP_SHT_CLIP;  
            pstAeSnsDft->u32MinIntTime = 32;
            pstAeSnsDft->u32MaxIntTimeTarget = 65535;
            pstAeSnsDft->u32MinIntTimeTarget = 2;
            
            pstAeSnsDft->u32MaxAgain = 4096; 
            pstAeSnsDft->u32MinAgain = 1024;
            pstAeSnsDft->u32MaxAgainTarget = 8192;
            pstAeSnsDft->u32MinAgainTarget = pstAeSnsDft->u32MinAgain;
            
            pstAeSnsDft->u32MaxDgain = 384;
            pstAeSnsDft->u32MinDgain = 128;
            pstAeSnsDft->u32MaxDgainTarget = 384;
            pstAeSnsDft->u32MinDgainTarget = pstAeSnsDft->u32MinDgain;
            pstAeSnsDft->u32MaxISPDgainTarget = 8 << pstAeSnsDft->u32ISPDgainShift;

            break;
        }
        
        default:
            break;
    }
    
    return 0;
}

#endif

/* the function of sensor set fps */
static HI_VOID cmos_fps_set(HI_FLOAT f32Fps, AE_SENSOR_DEFAULT_S *pstAeSnsDft)
{    
    if (WDR_MODE_BUILT_IN == genSensorMode)
    {
        if (30 == f32Fps)
        {
            gu32FullLinesStd = FRAME_LINES_2M_1080p;
            pstAeSnsDft->u32MaxIntTime = LONG_EXP_SHT_CLIP;
        }
        else if (25 == f32Fps)
        {
            gu32FullLinesStd = FRAME_LINES_2M_1080p * 30 /f32Fps;
            pstAeSnsDft->u32MaxIntTime = LONG_EXP_SHT_CLIP_PAL;
        }
        else
        {
            printf("Not support Fps: %f\n", f32Fps);
            return;
        }
    }
    else
    {
        if (SENSOR_2M_1080p30_MODE == gu8SensorImageMode)
        {
            if ((f32Fps <= 30) && (f32Fps >= 0.5))
            {
                gu32FullLinesStd = (FRAME_LINES_2M_1080p * 30) / f32Fps; 
            }
            else
            {
                printf("Not support Fps: %f\n", f32Fps);
                return;
            }        
        }
        else if (SENSOR_2M_1080p60_MODE == gu8SensorImageMode)
        {
            if ((f32Fps <= 60) && (f32Fps >= 0.5))
            {
                gu32FullLinesStd = (FRAME_LINES_2M_1080p * 60) / f32Fps;
            }
            else
            {
                printf("Not support Fps: %f\n", f32Fps);
                return;
            }
        }

        pstAeSnsDft->u32MaxIntTime = gu32FullLinesStd - 2;
    }    
   
   
    gu32FullLinesStd = gu32FullLinesStd > FULL_LINES_MAX ? FULL_LINES_MAX : gu32FullLinesStd;
    g_stSnsRegsInfo.astI2cData[3].u32Data = gu32FullLinesStd;
        
    pstAeSnsDft->f32Fps = f32Fps;
    pstAeSnsDft->u32LinesPer500ms = gu32FullLinesStd * f32Fps / 2;
    pstAeSnsDft->u32FullLinesStd = gu32FullLinesStd;
    gu32FullLines = gu32FullLinesStd;
    pstAeSnsDft->u32FullLines = gu32FullLines;

    return;
}
static HI_VOID cmos_slow_framerate_set(HI_U32 u32FullLines,
    AE_SENSOR_DEFAULT_S *pstAeSnsDft)
{
    if (WDR_MODE_NONE == genSensorMode)
    {
        u32FullLines = (u32FullLines > FULL_LINES_MAX) ? FULL_LINES_MAX : u32FullLines;
        gu32FullLines = u32FullLines;
        pstAeSnsDft->u32FullLines = gu32FullLines;

        g_stSnsRegsInfo.astI2cData[3].u32Data = gu32FullLines;

        pstAeSnsDft->u32MaxIntTime = gu32FullLines - 2;
    }    
    else if(WDR_MODE_BUILT_IN == genSensorMode)
    {
        pstAeSnsDft->u32FullLines = gu32FullLinesStd;

        return; 
    }
    else
    {}
    
    return;
}

/* while isp notify ae to update sensor regs, ae call these funcs. */
static HI_VOID cmos_inttime_update(HI_U32 u32IntTime)
{
    g_stSnsRegsInfo.astI2cData[0].u32Data = u32IntTime;

    return;
}

static  HI_U32  again_table[51] = 
{ 
    //here 1024 means x1.52
    1024,1075,1132,1195,1265,1344,1434,1536,1654,1792,1814,1873,1935,2002,2074,
    2150,2233,2322,2419,2524,2639,2765,2903,3056,3226,3415,3629,3871,4147,4466,
    4838,5278,5806,6451,7258,7741,8294,8932,9677,10557,11612,12902,14515,15483,16589,
    17865,19354,21113,23224,25805,29030
};

static HI_VOID cmos_again_calc_table(HI_U32 *pu32AgainLin, HI_U32 *pu32AgainDb)
{
    int i;
    HI_U32 u32InTimes;

    if(!pu32AgainDb || !pu32AgainLin)
    {
        printf("null pointer when get ae sensor gain info  value!\n");
        return;
    }
    
    u32InTimes = *pu32AgainLin;
   
    if (u32InTimes >= again_table[50])
    {
         *pu32AgainLin = again_table[50];
         *pu32AgainDb = 50;
         return ;
    }
    
    for(i = 1; i < 51; i++)
    {
        if(u32InTimes < again_table[i])
        {
            *pu32AgainLin = again_table[i - 1];
            *pu32AgainDb = i - 1;
            break;
        }

    }
          
    return;    
}

static HI_VOID cmos_gains_update(HI_U32 u32Again, HI_U32 u32Dgain)
{
    static HI_BOOL bHCG = HI_FALSE;

    if(u32Again < 10)
    {
        if(HI_TRUE == bHCG)
        {
            g_stSnsRegsInfo.astI2cData[4].u32Data = 0x0000;
            g_stSnsRegsInfo.astI2cData[5].u32Data = 0x0B08;
            g_stSnsRegsInfo.astI2cData[6].u32Data = 0x1E13;
            g_stSnsRegsInfo.astI2cData[7].u32Data = 0x0080;
            
            if (WDR_MODE_NONE != genSensorMode)
            {
                g_stSnsRegsInfo.astI2cData[8].u32Data = 0x0480;
                g_stSnsRegsInfo.astI2cData[9].u32Data = 0x0480;
            }
            else
            {           
                g_stSnsRegsInfo.astI2cData[8].u32Data =  0x0080;
                g_stSnsRegsInfo.astI2cData[9].u32Data =  0x0080;
                g_stSnsRegsInfo.astI2cData[10].u32Data =  0x0080;
                g_stSnsRegsInfo.astI2cData[11].u32Data = 0x0080;
            } 
            bHCG = HI_FALSE;
        }
        
        if(u32Again < 5)
        {
            g_stSnsRegsInfo.astI2cData[1].u32Data = u32Again + 0xb;
        }
        else
        {
            //0x10 + (again - 5) * 2
            g_stSnsRegsInfo.astI2cData[1].u32Data = u32Again * 2 + 0x6;
        }
    }
    else
    {
        if(HI_FALSE == bHCG)
        {
            g_stSnsRegsInfo.astI2cData[4].u32Data = 0x0004;
            g_stSnsRegsInfo.astI2cData[5].u32Data = 0x1C0E;
            g_stSnsRegsInfo.astI2cData[6].u32Data = 0x4E39;
            g_stSnsRegsInfo.astI2cData[7].u32Data = 0x00B0;
            
            if (WDR_MODE_NONE != genSensorMode)
            {
                g_stSnsRegsInfo.astI2cData[8].u32Data = 0x0780;
                g_stSnsRegsInfo.astI2cData[9].u32Data = 0x0780;
            }
            else
            {           
                g_stSnsRegsInfo.astI2cData[8].u32Data =  0xFF80;
                g_stSnsRegsInfo.astI2cData[9].u32Data =  0xFF80;
                g_stSnsRegsInfo.astI2cData[10].u32Data =  0xFF80;
                g_stSnsRegsInfo.astI2cData[11].u32Data = 0xFF80;
            }
            
            bHCG = HI_TRUE;
        }
        
        if(u32Again < 26)
        {
            g_stSnsRegsInfo.astI2cData[1].u32Data = u32Again - 10;
        }
        else
        {
            //0x10 + (again - 26) * 2
            g_stSnsRegsInfo.astI2cData[1].u32Data = u32Again * 2 - 36;
        }
    }
    
    g_stSnsRegsInfo.astI2cData[2].u32Data = u32Dgain;
    

    return;
}

HI_S32 cmos_init_ae_exp_function(AE_SENSOR_EXP_FUNC_S *pstExpFuncs)
{
    memset(pstExpFuncs, 0, sizeof(AE_SENSOR_EXP_FUNC_S));

    pstExpFuncs->pfn_cmos_get_ae_default    = cmos_get_ae_default;
    pstExpFuncs->pfn_cmos_fps_set           = cmos_fps_set;
    pstExpFuncs->pfn_cmos_slow_framerate_set= cmos_slow_framerate_set;    
    pstExpFuncs->pfn_cmos_inttime_update    = cmos_inttime_update;
    pstExpFuncs->pfn_cmos_gains_update      = cmos_gains_update;
    pstExpFuncs->pfn_cmos_again_calc_table  = cmos_again_calc_table;
 

    return 0;
}


/* AWB default parameter and function */
#ifdef INIFILE_CONFIG_MODE

static HI_S32 cmos_get_awb_default(AWB_SENSOR_DEFAULT_S *pstAwbSnsDft)
{
    HI_U8 i;
    
    if (HI_NULL == pstAwbSnsDft)
    {
        printf("null pointer when get awb default value!\n");
        return -1;
    }

    memset(pstAwbSnsDft, 0, sizeof(AWB_SENSOR_DEFAULT_S));
    pstAwbSnsDft->u16WbRefTemp = g_AwbDft[0].u16WbRefTemp;

    for(i= 0; i < 4; i++)
    {
        pstAwbSnsDft->au16GainOffset[i] = g_AwbDft[0].au16GainOffset[i];
    }
   
    for(i= 0; i < 6; i++)
    {
        pstAwbSnsDft->as32WbPara[i] = g_AwbDft[0].as32WbPara[i];
    }


    memcpy(&pstAwbSnsDft->stCcm, &g_AwbDft[0].stCcm, sizeof(AWB_CCM_S));
    memcpy(&pstAwbSnsDft->stAgcTbl, &g_AwbDft[0].stAgcTbl, sizeof(AWB_AGC_TABLE_S));

    return 0;
}

#else

// Calibration results for Static WB

#define CALIBRATE_STATIC_WB_R_GAIN 0x18D 
#define CALIBRATE_STATIC_WB_GR_GAIN 0x100 
#define CALIBRATE_STATIC_WB_GB_GAIN 0x100 
#define CALIBRATE_STATIC_WB_B_GAIN 0x1AD 
/* Calibration results for Auto WB Planck */
#define CALIBRATE_AWB_P1 0x0058  
#define CALIBRATE_AWB_P2 -0x000A 
#define CALIBRATE_AWB_Q1 -0x00B2 
#define CALIBRATE_AWB_A1 0x3404C  
#define CALIBRATE_AWB_B1 0x0080  
#define CALIBRATE_AWB_C1 -0x288A4 

//CCM
#define MT_ABSOLUTE_LS_H_CALIBRATE_CCM_LINEAR_R_R 0x0219
#define MT_ABSOLUTE_LS_H_CALIBRATE_CCM_LINEAR_R_G 0x80C8
#define MT_ABSOLUTE_LS_H_CALIBRATE_CCM_LINEAR_R_B 0x8051
#define MT_ABSOLUTE_LS_H_CALIBRATE_CCM_LINEAR_G_R 0x8055
#define MT_ABSOLUTE_LS_H_CALIBRATE_CCM_LINEAR_G_G 0x018C
#define MT_ABSOLUTE_LS_H_CALIBRATE_CCM_LINEAR_G_B 0x8037
#define MT_ABSOLUTE_LS_H_CALIBRATE_CCM_LINEAR_B_R 0x0006
#define MT_ABSOLUTE_LS_H_CALIBRATE_CCM_LINEAR_B_G 0x80C0
#define MT_ABSOLUTE_LS_H_CALIBRATE_CCM_LINEAR_B_B 0x01BA

#define MT_ABSOLUTE_LS_M_CALIBRATE_CCM_LINEAR_R_R 0x0204
#define MT_ABSOLUTE_LS_M_CALIBRATE_CCM_LINEAR_R_G 0x80A0
#define MT_ABSOLUTE_LS_M_CALIBRATE_CCM_LINEAR_R_B 0x8064
#define MT_ABSOLUTE_LS_M_CALIBRATE_CCM_LINEAR_G_R 0x807A
#define MT_ABSOLUTE_LS_M_CALIBRATE_CCM_LINEAR_G_G 0x01A7
#define MT_ABSOLUTE_LS_M_CALIBRATE_CCM_LINEAR_G_B 0x802D
#define MT_ABSOLUTE_LS_M_CALIBRATE_CCM_LINEAR_B_R 0x800F
#define MT_ABSOLUTE_LS_M_CALIBRATE_CCM_LINEAR_B_G 0x80C9
#define MT_ABSOLUTE_LS_M_CALIBRATE_CCM_LINEAR_B_B 0x01D8

#define MT_ABSOLUTE_LS_L_CALIBRATE_CCM_LINEAR_R_R 0x0210 
#define MT_ABSOLUTE_LS_L_CALIBRATE_CCM_LINEAR_R_G 0x8092
#define MT_ABSOLUTE_LS_L_CALIBRATE_CCM_LINEAR_R_B 0x807E
#define MT_ABSOLUTE_LS_L_CALIBRATE_CCM_LINEAR_G_R 0x805F
#define MT_ABSOLUTE_LS_L_CALIBRATE_CCM_LINEAR_G_G 0x0190
#define MT_ABSOLUTE_LS_L_CALIBRATE_CCM_LINEAR_G_B 0x8031
#define MT_ABSOLUTE_LS_L_CALIBRATE_CCM_LINEAR_B_R 0x8018
#define MT_ABSOLUTE_LS_L_CALIBRATE_CCM_LINEAR_B_G 0x815D
#define MT_ABSOLUTE_LS_L_CALIBRATE_CCM_LINEAR_B_B 0x0275

static AWB_CCM_S g_stAwbCcm =
{
 
   4850,
   {
      MT_ABSOLUTE_LS_H_CALIBRATE_CCM_LINEAR_R_R, MT_ABSOLUTE_LS_H_CALIBRATE_CCM_LINEAR_R_G, MT_ABSOLUTE_LS_H_CALIBRATE_CCM_LINEAR_R_B,   
      MT_ABSOLUTE_LS_H_CALIBRATE_CCM_LINEAR_G_R, MT_ABSOLUTE_LS_H_CALIBRATE_CCM_LINEAR_G_G, MT_ABSOLUTE_LS_H_CALIBRATE_CCM_LINEAR_G_B,   
      MT_ABSOLUTE_LS_H_CALIBRATE_CCM_LINEAR_B_R, MT_ABSOLUTE_LS_H_CALIBRATE_CCM_LINEAR_B_G, MT_ABSOLUTE_LS_H_CALIBRATE_CCM_LINEAR_B_B    
   },
   3850,
   {
      MT_ABSOLUTE_LS_M_CALIBRATE_CCM_LINEAR_R_R, MT_ABSOLUTE_LS_M_CALIBRATE_CCM_LINEAR_R_G, MT_ABSOLUTE_LS_M_CALIBRATE_CCM_LINEAR_R_B,   
      MT_ABSOLUTE_LS_M_CALIBRATE_CCM_LINEAR_G_R, MT_ABSOLUTE_LS_M_CALIBRATE_CCM_LINEAR_G_G, MT_ABSOLUTE_LS_M_CALIBRATE_CCM_LINEAR_G_B,   
      MT_ABSOLUTE_LS_M_CALIBRATE_CCM_LINEAR_B_R, MT_ABSOLUTE_LS_M_CALIBRATE_CCM_LINEAR_B_G, MT_ABSOLUTE_LS_M_CALIBRATE_CCM_LINEAR_B_B    
   }, 
   
   2450,
   {     
      MT_ABSOLUTE_LS_L_CALIBRATE_CCM_LINEAR_R_R, MT_ABSOLUTE_LS_L_CALIBRATE_CCM_LINEAR_R_G, MT_ABSOLUTE_LS_L_CALIBRATE_CCM_LINEAR_R_B,   
      MT_ABSOLUTE_LS_L_CALIBRATE_CCM_LINEAR_G_R, MT_ABSOLUTE_LS_L_CALIBRATE_CCM_LINEAR_G_G, MT_ABSOLUTE_LS_L_CALIBRATE_CCM_LINEAR_G_B,   
      MT_ABSOLUTE_LS_L_CALIBRATE_CCM_LINEAR_B_R, MT_ABSOLUTE_LS_L_CALIBRATE_CCM_LINEAR_B_G, MT_ABSOLUTE_LS_L_CALIBRATE_CCM_LINEAR_B_B   
   }                 
};

static AWB_AGC_TABLE_S g_stAwbAgcTable =
{
    /* bvalid */
    1,

    /* saturation */ 
    {0x80,0x80,0x7e,0x72,0x68,0x60,0x58,0x50,0x48,0x40,0x38,0x38,0x38,0x38,0x38,0x38}
};

static HI_S32 cmos_get_awb_default(AWB_SENSOR_DEFAULT_S *pstAwbSnsDft)
{
    if (HI_NULL == pstAwbSnsDft)
    {
        printf("null pointer when get awb default value!\n");
        return -1;
    }

    memset(pstAwbSnsDft, 0, sizeof(AWB_SENSOR_DEFAULT_S));

    pstAwbSnsDft->u16WbRefTemp = 4900;

    pstAwbSnsDft->au16GainOffset[0] = 0x18F;
    pstAwbSnsDft->au16GainOffset[1] = 0x100;
    pstAwbSnsDft->au16GainOffset[2] = 0x100;
    pstAwbSnsDft->au16GainOffset[3] = 0x195;

    pstAwbSnsDft->as32WbPara[0] = CALIBRATE_AWB_P1;
    pstAwbSnsDft->as32WbPara[1] = CALIBRATE_AWB_P2;
    pstAwbSnsDft->as32WbPara[2] = CALIBRATE_AWB_Q1;
    pstAwbSnsDft->as32WbPara[3] = CALIBRATE_AWB_A1;
    pstAwbSnsDft->as32WbPara[4] = CALIBRATE_AWB_B1;
    pstAwbSnsDft->as32WbPara[5] = CALIBRATE_AWB_C1;
    
    memcpy(&pstAwbSnsDft->stCcm, &g_stAwbCcm, sizeof(AWB_CCM_S));
    memcpy(&pstAwbSnsDft->stAgcTbl, &g_stAwbAgcTable, sizeof(AWB_AGC_TABLE_S));
    
    return 0;
}

#endif


HI_S32 cmos_init_awb_exp_function(AWB_SENSOR_EXP_FUNC_S *pstExpFuncs)
{
    memset(pstExpFuncs, 0, sizeof(AWB_SENSOR_EXP_FUNC_S));

    pstExpFuncs->pfn_cmos_get_awb_default = cmos_get_awb_default;

    return 0;
}


/* ISP default parameter and function */
#ifdef INIFILE_CONFIG_MODE

HI_U32 cmos_get_isp_default(ISP_CMOS_DEFAULT_S *pstDef)
{   
    if (HI_NULL == pstDef)
    {
        printf("null pointer when get isp default value!\n");
        return -1;
    }

    memset(pstDef, 0, sizeof(ISP_CMOS_DEFAULT_S));
    
    switch (genSensorMode)
    {
        default:
        case WDR_MODE_NONE:           
            memcpy(&pstDef->stDrc, &g_IspDft[0].stDrc, sizeof(ISP_CMOS_DRC_S));
            memcpy(&pstDef->stAgcTbl, &g_IspDft[0].stAgcTbl, sizeof(ISP_CMOS_AGC_TABLE_S));
            memcpy(&pstDef->stNoiseTbl, &g_IspDft[0].stNoiseTbl, sizeof(ISP_CMOS_NOISE_TABLE_S));            
            memcpy(&pstDef->stDemosaic, &g_IspDft[0].stDemosaic, sizeof(ISP_CMOS_DEMOSAIC_S));
            memcpy(&pstDef->stRgbSharpen, &g_IspDft[0].stRgbSharpen, sizeof(ISP_CMOS_RGBSHARPEN_S));
            memcpy(&pstDef->stGamma, &g_IspDft[0].stGamma, sizeof(ISP_CMOS_GAMMA_S));
        break;
        case WDR_MODE_BUILT_IN:            
            memcpy(&pstDef->stDrc, &g_IspDft[1].stDrc, sizeof(ISP_CMOS_DRC_S));
            memcpy(&pstDef->stAgcTbl, &g_IspDft[1].stAgcTbl, sizeof(ISP_CMOS_AGC_TABLE_S));
            memcpy(&pstDef->stNoiseTbl, &g_IspDft[1].stNoiseTbl, sizeof(ISP_CMOS_NOISE_TABLE_S));            
            memcpy(&pstDef->stDemosaic, &g_IspDft[1].stDemosaic, sizeof(ISP_CMOS_DEMOSAIC_S));
            memcpy(&pstDef->stRgbSharpen, &g_IspDft[1].stRgbSharpen, sizeof(ISP_CMOS_RGBSHARPEN_S));            
            memcpy(&pstDef->stGamma, &g_IspDft[1].stGamma, sizeof(ISP_CMOS_GAMMA_S));
            memcpy(&pstDef->stGammafe, &g_IspDft[1].stGammafe, sizeof(ISP_CMOS_GAMMAFE_S));
        break;

    }

    pstDef->stSensorMaxResolution.u32MaxWidth  = 1920;
    pstDef->stSensorMaxResolution.u32MaxHeight = 1080;

    return 0;
}

#else

static ISP_CMOS_AGC_TABLE_S g_stIspAgcTable =
{
    /* bvalid */
    1,
    
    /* 100, 200, 400, 800, 1600, 3200, 6400, 12800, 25600, 51200, 102400, 204800, 409600, 819200, 1638400, 3276800 */

    /* sharpen_alt_d */
    {0x3c,0x3c,0x3a,0x36,0x32,0x30,0x2a,0x26,0x22,0x20,0x20,0x20,0x20,0x20,0x20,0x20},
        
    /* sharpen_alt_ud */
    {0x6a,0x6a,0x68,0x60,0x58,0x50,0x40,0x30,0x20,0x10,0x10,0x10,0x10,0x10,0x10,0x10},    
        
    /* snr_thresh Max=0x54 */
    {0x08,0x0a,0x0c,0x10,0x16,0x1b,0x22,0x28,0x2a,0x3a,0x3a,0x3a,0x3a,0x3a,0x3a,0x3a},
        
    /* demosaic_lum_thresh */
    {0x50,0x50,0x4e,0x49,0x45,0x45,0x40,0x3a,0x30,0x20,0x20,0x20,0x20,0x20,0x20,0x20},
        
    /* demosaic_np_offset */
    {0x00,0x0a,0x12,0x1a,0x20,0x28,0x30,0x32,0x34,0x38,0x38,0x38,0x38,0x38,0x38,0x38},
        
    /* ge_strength */
    {0x55,0x55,0x55,0x55,0x55,0x55,0x37,0x37,0x37,0x35,0x35,0x35,0x35,0x35,0x35,0x35},

    /* rgbsharp_strength */
    {0x86,0x86,0x84,0x7a,0x60,0x50,0x40,0x30,0x20,0x10,0x10,0x10,0x10,0x10,0x10,0x10}
};

static ISP_CMOS_AGC_TABLE_S g_stIspAgcTableBuiltInWDR =
{
    /* bvalid */
    1,

    /* sharpen_alt_d */
    {0x40,0x40,0x40,0x40,0x38,0x30,0x28,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20},
    
    /* sharpen_alt_ud */
    {0x60,0x60,0x60,0x60,0x50,0x40,0x30,0x20,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10},
        
    /* snr_thresh */
    {0x8,0xC,0x10,0x14,0x18,0x20,0x28,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30},
        
    /* demosaic_lum_thresh */
    {0x50,0x50,0x40,0x40,0x30,0x30,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20},
        
    /* demosaic_np_offset */
    {0x0,0xa,0x12,0x1a,0x20,0x28,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30},
        
    /* ge_strength */
    {0x55,0x55,0x55,0x55,0x55,0x55,0x37,0x37,0x37,0x37,0x37,0x37,0x37,0x37,0x37,0x37},

    /* RGBsharpen_strength */
    {0x60,0x60,0x60,0x60,0x50,0x40,0x30,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20}
};

static ISP_CMOS_NOISE_TABLE_S g_stIspNoiseTable =
{
    /* bvalid */
    1,
    
    /* nosie_profile_weight_lut */
    {0x0, 0x0,  0x0,  0x0,  0x0,  0x3,  0xC,  0x11, 0x15, 0x17, 0x1A, 0x1B, 0x1D, 0x1E, 0x1F, 0x21, 
	0x22, 0x23, 0x23, 0x24, 0x25, 0x26, 0x26, 0x27, 0x27, 0x28, 0x29, 0x29, 0x2A, 0x2A, 0x2A, 0x2B, 
	0x2B, 0x2C, 0x2C, 0x2C, 0x2D, 0x2D, 0x2E, 0x2E, 0x2E, 0x2E, 0x2F, 0x2F, 0x2F, 0x30, 0x30, 0x30, 
	0x30, 0x31, 0x31, 0x31, 0x31, 0x32, 0x32, 0x32, 0x32, 0x33, 0x33, 0x33, 0x33, 0x33, 0x34, 0x34, 
	0x34, 0x34, 0x34, 0x35, 0x35, 0x35, 0x35, 0x35, 0x35, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x37, 
	0x37, 0x37, 0x37, 0x37, 0x37, 0x38, 0x38, 0x38, 0x38, 0x38, 0x38, 0x38, 0x38, 0x39, 0x39, 0x39, 
	0x39, 0x39, 0x39, 0x39, 0x39, 0x3A, 0x3A, 0x3A, 0x3A, 0x3A, 0x3A, 0x3A, 0x3A, 0x3B, 0x3B, 0x3B, 
	0x3B, 0x3B, 0x3B, 0x3B, 0x3B, 0x3B, 0x3B, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C }, 
    
    /* demosaic_weight_lut */
    {0x3, 0xC,  0x11, 0x15, 0x17, 0x1A, 0x1B, 0x1D, 0x1E, 0x1F, 0x21, 0x22, 0x23, 0x23, 0x24, 0x25, 
    0x26, 0x26, 0x27, 0x27, 0x28, 0x29, 0x29, 0x2A, 0x2A, 0x2A, 0x2B, 0x2B, 0x2C, 0x2C, 0x2C, 0x2D, 
    0x2D, 0x2E, 0x2E, 0x2E, 0x2E, 0x2F, 0x2F, 0x2F, 0x30, 0x30, 0x30, 0x30, 0x31, 0x31, 0x31, 0x31, 
    0x32, 0x32, 0x32, 0x32, 0x33, 0x33, 0x33, 0x33, 0x33, 0x34, 0x34, 0x34, 0x34, 0x34, 0x35, 0x35, 
    0x35, 0x35, 0x35, 0x35, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x37, 0x37, 0x37, 0x37, 0x37, 0x37, 
    0x38, 0x38, 0x38, 0x38, 0x38, 0x38, 0x38, 0x38, 0x39, 0x39, 0x39, 0x39, 0x39, 0x39, 0x39, 0x39, 
    0x3A, 0x3A, 0x3A, 0x3A, 0x3A, 0x3A, 0x3A, 0x3A, 0x3B, 0x3B, 0x3B, 0x3B, 0x3B, 0x3B, 0x3B, 0x3B, 
    0x3B, 0x3B, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C}
};

static ISP_CMOS_DEMOSAIC_S g_stIspDemosaic =
{
    /* bvalid */
    1,
    
    /*vh_slope*/
    0xac,

    /*aa_slope*/
    0xaa,

    /*va_slope*/
    0xa8,

    /*uu_slope*/
    0xa0,

    /*sat_slope*/
    0x5d,

    /*ac_slope*/
    0xa0,
    
    /*fc_slope*/
    0x8a,

    /*vh_thresh*/
    0x00,

    /*aa_thresh*/
    0x00,

    /*va_thresh*/
    0x00,

    /*uu_thresh*/
    0x08,

    /*sat_thresh*/
    0x00,

    /*ac_thresh*/
    0x1b3
};

static ISP_CMOS_DEMOSAIC_S g_stIspDemosaicBuiltInWDR =
{
    /* bvalid */
    1,
    
    /*vh_slope*/
    0xdc,

    /*aa_slope*/
    0xc8,

    /*va_slope*/
    0xb9,

    /*uu_slope*/
    0xa8,

    /*sat_slope*/
    0x5d,

    /*ac_slope*/
    0xa0,
    
    /*fc_slope*/
    0x80,

    /*vh_thresh*/
    0x00,

    /*aa_thresh*/
    0x00,

    /*va_thresh*/
    0x00,

    /*uu_thresh*/
    0x08,

    /*sat_thresh*/
    0x00,

    /*ac_thresh*/
    0x1b3
};

static ISP_CMOS_RGBSHARPEN_S g_stIspRgbSharpen =
{   
    /* bvalid */   
    1,   
    
    /*lut_core*/   
    192,  
    
    /*lut_strength*/  
    127, 
    
    /*lut_magnitude*/   
    6      
};

static ISP_CMOS_GAMMA_S g_stIspGamma =
{
    /* bvalid */
    1,
    
#if 1 /* Normal mode */   
    {  0, 180, 320, 426, 516, 590, 660, 730, 786, 844, 896, 946, 994, 1040, 1090, 1130, 1170, 1210, 1248,
    1296, 1336, 1372, 1416, 1452, 1486, 1516, 1546, 1580, 1616, 1652, 1678, 1714, 1742, 1776, 1798, 1830,
    1862, 1886, 1912, 1940, 1968, 1992, 2010, 2038, 2062, 2090, 2114, 2134, 2158, 2178, 2202, 2222, 2246,
    2266, 2282, 2300, 2324, 2344, 2360, 2372, 2390, 2406, 2422, 2438, 2458, 2478, 2494, 2510, 2526, 2546,
    2562, 2582, 2598, 2614, 2630, 2648, 2660, 2670, 2682, 2698, 2710, 2724, 2736, 2752, 2764, 2780, 2792,
    2808, 2820, 2836, 2848, 2864, 2876, 2888, 2896, 2908, 2920, 2928, 2940, 2948, 2960, 2972, 2984, 2992,
    3004, 3014, 3028, 3036, 3048, 3056, 3068, 3080, 3088, 3100, 3110, 3120, 3128, 3140, 3148, 3160, 3168,
    3174, 3182, 3190, 3202, 3210, 3218, 3228, 3240, 3256, 3266, 3276, 3288, 3300, 3306, 3318, 3326, 3334,
    3342, 3350, 3360, 3370, 3378, 3386, 3394, 3398, 3406, 3414, 3422, 3426, 3436, 3444, 3454, 3466, 3476,
    3486, 3498, 3502, 3510, 3518, 3526, 3530, 3538, 3546, 3554, 3558, 3564, 3570, 3574, 3582, 3590, 3598,
    3604, 3610, 3618, 3628, 3634, 3640, 3644, 3652, 3656, 3664, 3670, 3678, 3688, 3696, 3700, 3708, 3712,
    3716, 3722, 3730, 3736, 3740, 3748, 3752, 3756, 3760, 3766, 3774, 3778, 3786, 3790, 3800, 3808, 3812,
    3816, 3824, 3830, 3832, 3842, 3846, 3850, 3854, 3858, 3862, 3864, 3870, 3874, 3878, 3882, 3888, 3894,
    3900, 3908, 3912, 3918, 3924, 3928, 3934, 3940, 3946, 3952, 3958, 3966, 3974, 3978, 3982, 3986, 3990,
    3994, 4002, 4006, 4010, 4018, 4022, 4032, 4038, 4046, 4050, 4056, 4062, 4072, 4076, 4084, 4090, 4095}
#else  /* Infrared or Spotlight mode */
    {  0, 120, 220, 320, 416, 512, 592, 664, 736, 808, 880, 944, 1004, 1062, 1124, 1174,
    1226, 1276, 1328, 1380, 1432, 1472, 1516, 1556, 1596, 1636, 1680, 1720, 1756, 1792,
    1828, 1864, 1896, 1932, 1968, 2004, 2032, 2056, 2082, 2110, 2138, 2162, 2190, 2218,
    2242, 2270, 2294, 2314, 2338, 2358, 2382, 2402, 2426, 2446, 2470, 2490, 2514, 2534,
    2550, 2570, 2586, 2606, 2622, 2638, 2658, 2674, 2694, 2710, 2726, 2746, 2762, 2782,
    2798, 2814, 2826, 2842, 2854, 2870, 2882, 2898, 2910, 2924, 2936, 2952, 2964, 2980,
    2992, 3008, 3020, 3036, 3048, 3064, 3076, 3088, 3096, 3108, 3120, 3128, 3140, 3152,
    3160, 3172, 3184, 3192, 3204, 3216, 3224, 3236, 3248, 3256, 3268, 3280, 3288, 3300,
    3312, 3320, 3332, 3340, 3348, 3360, 3368, 3374, 3382, 3390, 3402, 3410, 3418, 3426,
    3434, 3446, 3454, 3462, 3470, 3478, 3486, 3498, 3506, 3514, 3522, 3530, 3542, 3550,
    3558, 3566, 3574, 3578, 3586, 3594, 3602, 3606, 3614, 3622, 3630, 3634, 3642, 3650,
    3658, 3662, 3670, 3678, 3686, 3690, 3698, 3706, 3710, 3718, 3722, 3726, 3734, 3738,
    3742, 3750, 3754, 3760, 3764, 3768, 3776, 3780, 3784, 3792, 3796, 3800, 3804, 3812,
    3816, 3820, 3824, 3832, 3836, 3840, 3844, 3848, 3856, 3860, 3864, 3868, 3872, 3876,
    3880, 3884, 3892, 3896, 3900, 3904, 3908, 3912, 3916, 3920, 3924, 3928, 3932, 3936,
    3940, 3944, 3948, 3952, 3956, 3960, 3964, 3968, 3972, 3972, 3976, 3980, 3984, 3988,
    3992, 3996, 4000, 4004, 4008, 4012, 4016, 4020, 4024, 4028, 4032, 4032, 4036, 4040,
    4044, 4048, 4052, 4056, 4056, 4060, 4064, 4068, 4072, 4072, 4076, 4080, 4084, 4086,
    4088, 4092, 4095} 
#endif
};



static ISP_CMOS_GAMMA_S g_stIspGammaBuiltInWDR =
{


    /* bvalid */
    1,
    {
        0,11,22,33,45,57,70,82,95,109,122,136,151,165,180,195,211,226,241,256,272,288,304,321,337,353,370,387,404,422,439,457,476,494,512,531,549,567,585,603,621,639,658,676,694,713,731,750,769,793,816,839,863,887,910,933,958,983,1008,1033,1059,1084,1109,1133,1157,1180,1204,1228,1252,1277,1301,1326,1348,1366,1387,1410,1432,1453,1475,1496,1518,1542,1565,1586,1607,1628,1648,1669,1689,1711,1730,1751,1771,1793,1815,1835,1854,1871,1889,1908,1928,1947,1965,1983,2003,2023,2040,2060,2079,2096,2113,2132,2149,2167,2184,2203,2220,2238,2257,2275,2293,2310,2326,2344,2359,2377,2392,2408,2426,2442,2460,2477,2492,2510,2527,2545,2561,2577,2592,2608,2623,2638,2654,2669,2685,2700,2716,2732,2748,2764,2779,2796,2811,2827,2842,2855,2870,2884,2898,2913,2926,2941,2955,2969,2985,2998,3014,3027,3042,3057,3071,3086,3100,3114,3127,3142,3155,3168,3182,3196,3209,3222,3237,3250,3264,3277,3292,3305,3319,3332,3347,3360,3374,3385,3398,3411,3424,3437,3448,3462,3475,3487,3501,3513,3526,3537,3550,3561,3573,3585,3596,3609,3621,3632,3644,3655,3666,3678,3689,3701,3712,3723,3735,3746,3757,3767,3779,3791,3802,3812,3823,3834,3845,3855,3866,3877,3888,3899,3907,3919,3929,3940,3951,3962,3973,3984,3994,4003,4014,4025,4034,4045,4056,4065,4076,4085,4095
    }
};

static ISP_CMOS_GAMMAFE_S g_stGammafeBuiltInWDR = 
{
    /* bvalid */
    1,

    /* gamma_fe0 */
    {
        0, 2048, 34816, 35840, 36864, 37888, 38912, 39936, 40960, 41984, 43008, 44032, 45056, 46080, 47104, 48128, 49152, 50176, 51200, 52224, 53248, 54272, 55296, 56320, 57344, 58368, 59392, 60416, 61440, 62464, 63488, 64512, 65535
    },

    /* gamma_fe1 */
    {
        0, 156, 320, 495, 682, 885, 1108, 1358, 1650, 1671, 1691, 1712, 1733, 1754, 1776, 1798, 1821, 1844, 1867, 1891, 1915, 1939, 1965, 1990, 2017, 2044, 2071, 2100, 2129, 2159, 2190, 2222, 2255, 2290, 2326, 2363, 2403, 2445, 2489, 2537, 2589, 2647, 2712, 2790, 2891, 3135, 3379, 3480, 3558, 3623, 3681, 3733, 3781, 3825, 3867, 3907, 3944, 3980, 4015, 4048, 4080, 4111, 4141, 4170, 4199, 4226, 4253, 4280, 4305, 4331, 4355, 4379, 4403, 4426, 4449, 4472, 4494, 4516, 4537, 4558, 4579, 4599, 4620, 4639, 4659, 4679, 4698, 4717, 4735, 4754, 4772, 4790, 4808, 4826, 4843, 4861, 4878, 4895, 4912, 4928, 4945, 4961, 4978, 4994, 5010, 5025, 5041, 5057, 5072, 5087, 5103, 5118, 5133, 5148, 5162, 5177, 5191, 5206, 5220, 5234, 5249, 5263, 5277, 5290, 5304, 5318, 5332, 5345, 5358, 5372, 5385, 5398, 5411, 5424, 5437, 5450, 5463, 5842, 6174, 6472, 6747, 7002, 7241, 7467, 7681, 7886, 8083, 8272, 8454, 8630, 8801, 8967, 9128, 9285, 9438, 9588, 9734, 9877, 10016, 10154, 10288, 10550, 10803, 11043, 11281, 11512, 11736, 11955, 12168, 12377, 12581, 12781, 12976, 13168, 13356, 13541, 13723, 13901, 14077, 14250, 14420, 14588, 14753, 14916, 15076, 15235, 15392, 15546, 15699, 15850, 15999, 16146, 16292, 16436, 16578, 16719, 16859, 16997, 17134, 17269, 17404, 17537, 17668, 17799, 17928, 18057, 18184, 18310, 18435, 20282, 21977, 23531, 24974, 26328, 27608, 28824, 29984, 31097, 32167, 33199, 34196, 35163, 36101, 37014, 37901, 38768, 39613, 40440, 41249, 42040, 42816, 43577, 44325, 45058, 45779, 46488, 47186, 48533, 49856, 51144, 52398, 53620, 54815, 55981, 57123, 58240, 59336, 60410, 61466, 62502, 63520, 64522, 65507, 65535, 65535, 65535, 65535
    }
};


HI_U32 cmos_get_isp_default(ISP_CMOS_DEFAULT_S *pstDef)
{   
    if (HI_NULL == pstDef)
    {
        printf("null pointer when get isp default value!\n");
        return -1;
    }

    memset(pstDef, 0, sizeof(ISP_CMOS_DEFAULT_S));

    switch (genSensorMode)
    {
        case WDR_MODE_NONE:
        {
            pstDef->stDrc.bEnable               = HI_FALSE;
            pstDef->stDrc.u32BlackLevel         = 0x00;
            pstDef->stDrc.u32WhiteLevel         = 0x4FF; 
            pstDef->stDrc.u32SlopeMax           = 0x30;
            pstDef->stDrc.u32SlopeMin           = 0x00;
            pstDef->stDrc.u32VarianceSpace      = 0x04;
            pstDef->stDrc.u32VarianceIntensity  = 0x01;
            pstDef->stDrc.u32Asymmetry          = 0x14;
            pstDef->stDrc.u32BrightEnhance      = 0xC8;

            memcpy(&pstDef->stNoiseTbl, &g_stIspNoiseTable, sizeof(ISP_CMOS_NOISE_TABLE_S));            
            memcpy(&pstDef->stAgcTbl, &g_stIspAgcTable, sizeof(ISP_CMOS_AGC_TABLE_S));
            memcpy(&pstDef->stDemosaic, &g_stIspDemosaic, sizeof(ISP_CMOS_DEMOSAIC_S));
            memcpy(&pstDef->stGamma, &g_stIspGamma, sizeof(ISP_CMOS_GAMMA_S));
            memcpy(&pstDef->stRgbSharpen, &g_stIspRgbSharpen, sizeof(ISP_CMOS_RGBSHARPEN_S));
            break;
        }
        
        case WDR_MODE_BUILT_IN:
        {
            pstDef->stDrc.bEnable               = HI_TRUE;
            pstDef->stDrc.u32BlackLevel         = 0x00;
            pstDef->stDrc.u32WhiteLevel         = 0xFFF; 
            pstDef->stDrc.u32SlopeMax           = 0x38;
            pstDef->stDrc.u32SlopeMin           = 0xC0;
            pstDef->stDrc.u32VarianceSpace      = 0x0A;
            pstDef->stDrc.u32VarianceIntensity  = 0x04;
            pstDef->stDrc.u32Asymmetry          = 0x14;
            pstDef->stDrc.u32BrightEnhance      = 0xC8;

            memcpy(&pstDef->stNoiseTbl, &g_stIspNoiseTable, sizeof(ISP_CMOS_NOISE_TABLE_S));
            memcpy(&pstDef->stAgcTbl, &g_stIspAgcTableBuiltInWDR, sizeof(ISP_CMOS_AGC_TABLE_S));
            memcpy(&pstDef->stDemosaic, &g_stIspDemosaicBuiltInWDR, sizeof(ISP_CMOS_DEMOSAIC_S));
            memcpy(&pstDef->stGamma, &g_stIspGammaBuiltInWDR, sizeof(ISP_CMOS_GAMMA_S));
            memcpy(&pstDef->stGammafe, &g_stGammafeBuiltInWDR, sizeof(ISP_CMOS_GAMMAFE_S));
            memcpy(&pstDef->stRgbSharpen, &g_stIspRgbSharpen, sizeof(ISP_CMOS_RGBSHARPEN_S));
            break;
        }
        
        default:
            break;
    }

    pstDef->stSensorMaxResolution.u32MaxWidth  = 1920;
    pstDef->stSensorMaxResolution.u32MaxHeight = 1080;

    return 0;
}

#endif

HI_U32 cmos_get_isp_black_level(ISP_CMOS_BLACK_LEVEL_S *pstBlackLevel)
{
    HI_S32  i;
    
    if (HI_NULL == pstBlackLevel)
    {
        printf("null pointer when get isp black level value!\n");
        return -1;
    }

    /* Don't need to update black level when iso change */
    pstBlackLevel->bUpdate = HI_FALSE;

    if (WDR_MODE_NONE == genSensorMode)
    {
        for (i=0; i<4; i++)
        {
            pstBlackLevel->au16BlackLevel[i] = 0xA8;
        }
    }
    else if (WDR_MODE_BUILT_IN == genSensorMode)
    {
        for (i=0; i<4; i++)
        {
            pstBlackLevel->au16BlackLevel[i] = 0xC3;
        }
    }
    else
    {}

    return 0;    
}

HI_VOID cmos_set_pixel_detect(HI_BOOL bEnable)
{
    
    HI_U32 u32FullLines_5Fps = FRAME_LINES_2M_1080p * 30 / 5;
    HI_U32 u32MaxExpTime_5Fps = u32FullLines_5Fps - 2;
    
    if (WDR_MODE_BUILT_IN == genSensorMode)
    {
        return;
    }

    u32FullLines_5Fps = (u32FullLines_5Fps > FULL_LINES_MAX) ? FULL_LINES_MAX : u32FullLines_5Fps;
    u32MaxExpTime_5Fps = u32FullLines_5Fps - 2; 
    
    if (bEnable) /* setup for ISP pixel calibration mode */
    {
        sensor_write_register(FRAME_LINES, u32FullLines_5Fps); /* 5fps */
        sensor_write_register(EXPOSURE_TIME, u32MaxExpTime_5Fps); /* max exposure time */
        sensor_write_register(ANALOG_GAIN, 0x0);      /* AG, Context A */
        sensor_write_register(DIGITAL_GAIN, 0x0080);  /* DG, Context A */
    }
    else /* setup for ISP 'normal mode' */
    {
        gu32FullLinesStd = (gu32FullLinesStd > FULL_LINES_MAX) ? FULL_LINES_MAX : gu32FullLinesStd;
        sensor_write_register(FRAME_LINES, gu32FullLinesStd);    /* 30fps */
        bInit = HI_FALSE;
    }

    return;
}

HI_VOID cmos_set_wdr_mode(HI_U8 u8Mode)
{
    bInit = HI_FALSE;
    
    switch(u8Mode)
    {
        case WDR_MODE_NONE:
            genSensorMode = WDR_MODE_NONE;
        break;

        case WDR_MODE_BUILT_IN:
            genSensorMode = WDR_MODE_BUILT_IN;
        break;

        default:
            printf("NOT support this mode!\n");
            return;
        break;
    }
    return;
}    


static HI_S32 cmos_set_image_mode(ISP_CMOS_SENSOR_IMAGE_MODE_S *pstSensorImageMode)
{
    HI_U8 u8SensorImageMode = gu8SensorImageMode;
    
    bInit = HI_FALSE;    
        
    if (HI_NULL == pstSensorImageMode )
    {
        printf("null pointer when set image mode\n");
        return -1;
    }

    if((pstSensorImageMode->u16Width <= 1920)&&(pstSensorImageMode->u16Height <= 1080))
    {
        if (pstSensorImageMode->f32Fps <= 30)
        {
            u8SensorImageMode = SENSOR_2M_1080p30_MODE;
        }
        else
        {
            u8SensorImageMode = SENSOR_2M_1080p60_MODE;
        }
    } 
    else
    {
        printf("Not support! Width:%d, Height:%d, Fps:%f\n", 
            pstSensorImageMode->u16Width, 
            pstSensorImageMode->u16Height,
            pstSensorImageMode->f32Fps);
    }

    /* Sensor first init */
    if (HI_FALSE == bSensorInit)
    {
        gu8SensorImageMode = u8SensorImageMode;
        
        return 0;
    }

    /* Switch SensorImageMode */
    if (u8SensorImageMode == gu8SensorImageMode)
    {
        /* Don't need to switch SensorImageMode */
        return -1;
    }
    
    gu8SensorImageMode = u8SensorImageMode;

    return 0;
    
}

HI_U32 cmos_get_sns_regs_info(ISP_SNS_REGS_INFO_S *pstSnsRegsInfo)
{

    HI_S32 i;

    if (HI_FALSE == bInit)
    {
        g_stSnsRegsInfo.enSnsType = ISP_SNS_I2C_TYPE;
        g_stSnsRegsInfo.u8Cfg2ValidDelayMax = 2;
        g_stSnsRegsInfo.u32RegNum = 12;
        for (i=0; i < g_stSnsRegsInfo.u32RegNum; i++)
        {
            g_stSnsRegsInfo.astI2cData[i].bUpdate = HI_TRUE;
            g_stSnsRegsInfo.astI2cData[i].u8DevAddr = sensor_i2c_addr;
            g_stSnsRegsInfo.astI2cData[i].u32AddrByteNum = sensor_addr_byte;
            g_stSnsRegsInfo.astI2cData[i].u32DataByteNum = sensor_data_byte;
        }
        g_stSnsRegsInfo.astI2cData[0].u8DelayFrmNum = 0;
        g_stSnsRegsInfo.astI2cData[0].u32RegAddr = EXPOSURE_TIME;
        g_stSnsRegsInfo.astI2cData[1].u8DelayFrmNum = 0;
        g_stSnsRegsInfo.astI2cData[1].u32RegAddr = ANALOG_GAIN;
        g_stSnsRegsInfo.astI2cData[2].u8DelayFrmNum = 0;
        g_stSnsRegsInfo.astI2cData[2].u32RegAddr = DIGITAL_GAIN;
        g_stSnsRegsInfo.astI2cData[3].u8DelayFrmNum = 0;
        g_stSnsRegsInfo.astI2cData[3].u32RegAddr = FRAME_LINES;
        
        // related registers in DCG mode
        g_stSnsRegsInfo.astI2cData[4].u8DelayFrmNum = 0;
        g_stSnsRegsInfo.astI2cData[4].u32RegAddr = 0x3100;
        g_stSnsRegsInfo.astI2cData[5].u8DelayFrmNum = 0;
        g_stSnsRegsInfo.astI2cData[5].u32RegAddr = 0x3206;
        g_stSnsRegsInfo.astI2cData[6].u8DelayFrmNum = 0;
        g_stSnsRegsInfo.astI2cData[6].u32RegAddr = 0x3208;
        g_stSnsRegsInfo.astI2cData[7].u8DelayFrmNum = 0;
        g_stSnsRegsInfo.astI2cData[7].u32RegAddr = 0x3202;
        if (WDR_MODE_NONE != genSensorMode)
        {
            g_stSnsRegsInfo.astI2cData[8].u8DelayFrmNum = 0;
            g_stSnsRegsInfo.astI2cData[8].u32RegAddr = 0x3096;
            g_stSnsRegsInfo.astI2cData[9].u8DelayFrmNum = 0;
            g_stSnsRegsInfo.astI2cData[9].u32RegAddr = 0x3098;
            g_stSnsRegsInfo.u32RegNum = 10;
        }
        else
        {
            g_stSnsRegsInfo.astI2cData[8].u8DelayFrmNum = 0;
            g_stSnsRegsInfo.astI2cData[8].u32RegAddr = 0x3176;
            g_stSnsRegsInfo.astI2cData[9].u8DelayFrmNum = 0;
            g_stSnsRegsInfo.astI2cData[9].u32RegAddr = 0x3178;
            g_stSnsRegsInfo.astI2cData[10].u8DelayFrmNum = 0;
            g_stSnsRegsInfo.astI2cData[10].u32RegAddr = 0x317A;
            g_stSnsRegsInfo.astI2cData[11].u8DelayFrmNum = 0;
            g_stSnsRegsInfo.astI2cData[11].u32RegAddr = 0x317C;
        }        
        
        

        bInit = HI_TRUE;
    }
    else
    {
        for (i=0; i<g_stSnsRegsInfo.u32RegNum; i++)
        {
            if (g_stSnsRegsInfo.astI2cData[i].u32Data == g_stPreSnsRegsInfo.astI2cData[i].u32Data)
            {
                g_stSnsRegsInfo.astI2cData[i].bUpdate = HI_FALSE;
            }
            else
            {
                g_stSnsRegsInfo.astI2cData[i].bUpdate = HI_TRUE;
            }
        }
    }
    
    if (HI_NULL == pstSnsRegsInfo)
    {
        printf("null pointer when get sns reg info!\n");
        return -1;
    }

    memcpy(pstSnsRegsInfo, &g_stSnsRegsInfo, sizeof(ISP_SNS_REGS_INFO_S)); 
    memcpy(&g_stPreSnsRegsInfo, &g_stSnsRegsInfo, sizeof(ISP_SNS_REGS_INFO_S)); 

    return 0;
}

int  sensor_set_inifile_path(const char *pcPath)
{
    memset(pcName, 0, sizeof(pcName));
        
    if (HI_NULL == pcPath)
    {        
        strncat(pcName, "configs/", strlen("configs/"));
        strncat(pcName, CMOS_CFG_INI, sizeof(CMOS_CFG_INI));
    }
    else
    {
        if(strlen(pcPath) > (PATHLEN_MAX - 30))
        {
            printf("Set inifile path is larger PATHLEN_MAX!\n");
            return -1;        
        }
        
        strncat(pcName, pcPath, strlen(pcPath));
        strncat(pcName, CMOS_CFG_INI, sizeof(CMOS_CFG_INI));
    }
    
    return 0;
}

HI_VOID sensor_global_init()
{   
    gu8SensorImageMode = SENSOR_2M_1080p30_MODE;
    genSensorMode = WDR_MODE_NONE;       
    gu32FullLinesStd = FRAME_LINES_2M_1080p;
    gu32FullLines = FRAME_LINES_2M_1080p; 
    bInit = HI_FALSE;
    bSensorInit = HI_FALSE; 
    
    memset(&g_stSnsRegsInfo, 0, sizeof(ISP_SNS_REGS_INFO_S));
    memset(&g_stPreSnsRegsInfo, 0, sizeof(ISP_SNS_REGS_INFO_S));
    
#ifdef INIFILE_CONFIG_MODE 
    HI_S32 s32Ret = HI_SUCCESS;
    s32Ret = Cmos_LoadINIPara(pcName);
    if (HI_SUCCESS != s32Ret)
    {
        printf("Cmos_LoadINIPara failed!!!!!!\n");
    }
#else

#endif

}

HI_S32 cmos_init_sensor_exp_function(ISP_SENSOR_EXP_FUNC_S *pstSensorExpFunc)
{
    memset(pstSensorExpFunc, 0, sizeof(ISP_SENSOR_EXP_FUNC_S));

    pstSensorExpFunc->pfn_cmos_sensor_init = sensor_init;
    pstSensorExpFunc->pfn_cmos_sensor_exit = sensor_exit;
    pstSensorExpFunc->pfn_cmos_sensor_global_init = sensor_global_init;
    pstSensorExpFunc->pfn_cmos_set_image_mode = cmos_set_image_mode;
    pstSensorExpFunc->pfn_cmos_set_wdr_mode = cmos_set_wdr_mode;
    
    pstSensorExpFunc->pfn_cmos_get_isp_default = cmos_get_isp_default;
    pstSensorExpFunc->pfn_cmos_get_isp_black_level = cmos_get_isp_black_level;
    pstSensorExpFunc->pfn_cmos_set_pixel_detect = cmos_set_pixel_detect;
    pstSensorExpFunc->pfn_cmos_get_sns_reg_info = cmos_get_sns_regs_info;

    return 0;
}

/****************************************************************************
 * callback structure                                                       *
 ****************************************************************************/
 
int sensor_register_callback(void)
{
    ISP_DEV IspDev = 0;
    HI_S32 s32Ret;
    ALG_LIB_S stLib;
    ISP_SENSOR_REGISTER_S stIspRegister;
    AE_SENSOR_REGISTER_S  stAeRegister;
    AWB_SENSOR_REGISTER_S stAwbRegister;

    cmos_init_sensor_exp_function(&stIspRegister.stSnsExp);
    s32Ret = HI_MPI_ISP_SensorRegCallBack(IspDev, AR0230_ID, &stIspRegister);
    if (s32Ret)
    {
        printf("sensor register callback function failed!\n");
        return s32Ret;
    }
    
    stLib.s32Id = 0;
    strncpy(stLib.acLibName, HI_AE_LIB_NAME, sizeof(HI_AE_LIB_NAME));
    cmos_init_ae_exp_function(&stAeRegister.stSnsExp);
    s32Ret = HI_MPI_AE_SensorRegCallBack(IspDev, &stLib, AR0230_ID, &stAeRegister);
    if (s32Ret)
    {
        printf("sensor register callback function to ae lib failed!\n");
        return s32Ret;
    }

    stLib.s32Id = 0;
    strncpy(stLib.acLibName, HI_AWB_LIB_NAME, sizeof(HI_AWB_LIB_NAME));
    cmos_init_awb_exp_function(&stAwbRegister.stSnsExp);
    s32Ret = HI_MPI_AWB_SensorRegCallBack(IspDev, &stLib, AR0230_ID, &stAwbRegister);
    if (s32Ret)
    {
        printf("sensor register callback function to awb lib failed!\n");
        return s32Ret;
    }
    
    return 0;
}

int sensor_unregister_callback(void)
{
    ISP_DEV IspDev = 0;
    HI_S32 s32Ret;
    ALG_LIB_S stLib;

    s32Ret = HI_MPI_ISP_SensorUnRegCallBack(IspDev, AR0230_ID);
    if (s32Ret)
    {
        printf("sensor unregister callback function failed!\n");
        return s32Ret;
    }
    
    stLib.s32Id = 0;
    strncpy(stLib.acLibName, HI_AE_LIB_NAME, sizeof(HI_AE_LIB_NAME));
    s32Ret = HI_MPI_AE_SensorUnRegCallBack(IspDev, &stLib, AR0230_ID);
    if (s32Ret)
    {
        printf("sensor unregister callback function to ae lib failed!\n");
        return s32Ret;
    }

    stLib.s32Id = 0;
    strncpy(stLib.acLibName, HI_AWB_LIB_NAME, sizeof(HI_AWB_LIB_NAME));
    s32Ret = HI_MPI_AWB_SensorUnRegCallBack(IspDev, &stLib, AR0230_ID);
    if (s32Ret)
    {
        printf("sensor unregister callback function to awb lib failed!\n");
        return s32Ret;
    }
    
    return 0;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif 
