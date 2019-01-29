#if !defined(__AR0237_CMOS_H_)
#define __AR0237_CMOS_H_

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

#define AR0237_ID 237

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

#define ANALOG_GAIN      (0x3060)
#define DIGITAL_GAIN     (0x305E)
#define FRAME_LINES      (0x300A)
#define EXPOSURE_TIME_L  (0x3012)
#define EXPOSURE_TIME_S  (0x3212)
#define LINE_LEN_PCK     (0x300C)

#define SENSOR_2M_1080p30_MODE  (1) 
#define SENSOR_2M_1080p60_MODE  (2)

#define INCREASE_LINES (1) /* make real fps less than stand fps because NVR require*/
#define FRAME_LINES_2M_LIN  (0x454+INCREASE_LINES)
#define FRAME_LINES_2M_WDR  (0x4C4+INCREASE_LINES)


// Max integration time for HDR mode: T1 max = min ( 70* 16, FLL*16/17), when ratio=16;
// we use a constant full lines in build-in wdr mode
#define LONG_EXP_SHT_CLIP     (FRAME_LINES_2M_LIN*16/17)

HI_U8 gu8SensorImageMode = SENSOR_2M_1080p30_MODE;
WDR_MODE_E genSensorMode = WDR_MODE_NONE;

static HI_U32 gu32FullLinesStd = FRAME_LINES_2M_LIN;
static HI_U32 gu32FullLines = FRAME_LINES_2M_LIN; 
static HI_BOOL bInit = HI_FALSE;
HI_BOOL bSensorInit = HI_FALSE; 

ISP_SNS_REGS_INFO_S g_stSnsRegsInfo = {0};
ISP_SNS_REGS_INFO_S g_stPreSnsRegsInfo = {0};

#define PATHLEN_MAX 256
#define CMOS_CFG_INI "AR0237_cfg.ini"
static char pcName[PATHLEN_MAX] = "configs/AR0237_cfg.ini";


/* AE default parameter and function */
#ifdef INIFILE_CONFIG_MODE
static HI_S32 cmos_get_ae_default(AE_SENSOR_DEFAULT_S *pstAeSnsDft)
{
    if (HI_NULL == pstAeSnsDft)
    {
        printf("null pointer when get ae default value!\n");
        return -1;
    }
    
    gu32FullLinesStd = (WDR_MODE_NONE == genSensorMode)? FRAME_LINES_2M_LIN : \
                                                         FRAME_LINES_2M_WDR;
        
    pstAeSnsDft->u32LinesPer500ms = gu32FullLinesStd*30/2;
    pstAeSnsDft->u32FullLinesStd = gu32FullLinesStd;
    pstAeSnsDft->u32FlickerFreq = 0;

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
    
            pstAeSnsDft->u32MaxAgain = 29030;  
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
        case WDR_MODE_2To1_LINE:
        {
            pstAeSnsDft->au8HistThresh[0] = 0xC;
            pstAeSnsDft->au8HistThresh[1] = 0x18;
            pstAeSnsDft->au8HistThresh[2] = 0x60;
            pstAeSnsDft->au8HistThresh[3] = 0x80;
           
            pstAeSnsDft->u8AeCompensation = g_AeDft[1].u8AeCompensation;
            
            pstAeSnsDft->u32MaxIntTime = gu32FullLinesStd - 2;;  
            pstAeSnsDft->u32MinIntTime = 8;
            pstAeSnsDft->u32MaxIntTimeTarget = g_AeDft[1].u32MaxIntTimeTarget;
            pstAeSnsDft->u32MinIntTimeTarget = g_AeDft[1].u32MinIntTimeTarget;
            
            pstAeSnsDft->u32MaxAgain = 16384; 
            pstAeSnsDft->u32MinAgain = 1024;
            pstAeSnsDft->u32MaxAgainTarget = g_AeDft[1].u32MaxAgainTarget;
            pstAeSnsDft->u32MinAgainTarget = g_AeDft[1].u32MinAgainTarget;
            
            pstAeSnsDft->u32MaxDgain = 128;
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
    
    gu32FullLinesStd = (WDR_MODE_NONE == genSensorMode)? FRAME_LINES_2M_LIN : \
                                                         FRAME_LINES_2M_WDR;
            
    
    pstAeSnsDft->u32LinesPer500ms = gu32FullLinesStd*30/2;
    pstAeSnsDft->u32FullLinesStd = gu32FullLinesStd;
    pstAeSnsDft->u32FlickerFreq = 0;

    pstAeSnsDft->stIntTimeAccu.enAccuType = AE_ACCURACY_LINEAR;
    pstAeSnsDft->stIntTimeAccu.f32Accuracy = 1;
    pstAeSnsDft->stIntTimeAccu.f32Offset = 0;

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
            
            pstAeSnsDft->u8AeCompensation = 0x32;
            
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
        case WDR_MODE_2To1_LINE:
        {
            pstAeSnsDft->au8HistThresh[0] = 0xC;
            pstAeSnsDft->au8HistThresh[1] = 0x18;
            pstAeSnsDft->au8HistThresh[2] = 0x60;
            pstAeSnsDft->au8HistThresh[3] = 0x80;
            pstAeSnsDft->u8AeCompensation = 0x32;
            
            pstAeSnsDft->u32MaxIntTime = gu32FullLinesStd - 2;;  
            pstAeSnsDft->u32MinIntTime = 8;
            pstAeSnsDft->u32MaxIntTimeTarget = 65535;
            pstAeSnsDft->u32MinIntTimeTarget = 2;
            
            pstAeSnsDft->u32MaxAgain = 16384; 
            pstAeSnsDft->u32MinAgain = 1024;
            pstAeSnsDft->u32MaxAgainTarget = 29030;
            pstAeSnsDft->u32MinAgainTarget = pstAeSnsDft->u32MinAgain;
            
            pstAeSnsDft->u32MaxDgain = 128;
            pstAeSnsDft->u32MinDgain = 128;
            pstAeSnsDft->u32MaxDgainTarget = pstAeSnsDft->u32MaxDgain;
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
    if (WDR_MODE_2To1_LINE == genSensorMode)
    {
        if (30 == f32Fps)
        {
            gu32FullLinesStd = FRAME_LINES_2M_WDR;
        }
        else if ((f32Fps < 30) && (f32Fps >= 0.5))
        {
            gu32FullLinesStd = FRAME_LINES_2M_WDR * 30 / f32Fps;
        }
        else
        {
            printf("Not support Fps: %f\n", f32Fps);
            return;
        }
        pstAeSnsDft->u32MaxIntTime = gu32FullLinesStd - 2;
    }
    else
    {
        if (SENSOR_2M_1080p30_MODE == gu8SensorImageMode)
        {
            if ((f32Fps <= 30) && (f32Fps >= 0.5))
            {
                gu32FullLinesStd = (FRAME_LINES_2M_LIN * 30) / f32Fps; 
            }
            else
            {
                printf("Not support Fps: %f\n", f32Fps);
                return;
            } 
        }
        pstAeSnsDft->u32MaxIntTime = gu32FullLinesStd - 2;
    }    
   
    gu32FullLinesStd = gu32FullLinesStd > 0xFFFF ? 0xFFFF : gu32FullLinesStd;
    g_stSnsRegsInfo.astI2cData[4].u32Data = gu32FullLinesStd;
        
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
    u32FullLines = (u32FullLines > 0xFFFF) ? 0xFFFF : u32FullLines;
    gu32FullLines = u32FullLines;
    pstAeSnsDft->u32FullLines = gu32FullLines;
    
    g_stSnsRegsInfo.astI2cData[4].u32Data = gu32FullLines;

    pstAeSnsDft->u32MaxIntTime = gu32FullLines - 2;
    
    return;
}

/* while isp notify ae to update sensor regs, ae call these funcs. */
static HI_VOID cmos_inttime_update(HI_U32 u32IntTime)
{
    static HI_BOOL bFirst = HI_TRUE;
    
    if (WDR_MODE_2To1_LINE == genSensorMode)
    {
        if (bFirst) /* short exposure */
        {
            g_stSnsRegsInfo.astI2cData[0].u32Data = u32IntTime;
            bFirst = HI_FALSE;
        }
        else /* long exposure */
        {
            g_stSnsRegsInfo.astI2cData[1].u32Data = u32IntTime;
            bFirst = HI_TRUE;
        }
    }
    else
    {
        g_stSnsRegsInfo.astI2cData[1].u32Data = u32IntTime;
        bFirst = HI_TRUE;
    }

    return;    
}

// TODO:: Again of AR0237 could not be 0x40?
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
            g_stSnsRegsInfo.astI2cData[5].u32Data = 0x0000;
            bHCG = HI_FALSE;
        }
        
        if(u32Again < 5)
        {
            g_stSnsRegsInfo.astI2cData[2].u32Data = u32Again + 0xb;
        }
        else
        {
            //0x10 + (again - 5) * 2
            g_stSnsRegsInfo.astI2cData[2].u32Data = u32Again * 2 + 0x6;
        }
    }
    else
    {
        if(HI_FALSE == bHCG)
        {
            g_stSnsRegsInfo.astI2cData[5].u32Data = 0x0004;        
            bHCG = HI_TRUE;
        }
        
        if(u32Again < 26)
        {
            g_stSnsRegsInfo.astI2cData[2].u32Data = u32Again - 10;
        }
        else
        {
            //0x10 + (again - 26) * 2
            g_stSnsRegsInfo.astI2cData[2].u32Data = u32Again * 2 - 36;
        }
    }
    
    g_stSnsRegsInfo.astI2cData[3].u32Data = u32Dgain;
    

    return;
}

/* Only used in WDR_MODE_2To1_LINE mode */

/* [T2 max = FLL - y_output ¡§C 16] The y_output is the output rows calculated by...
    ...(Y_ADDR_END - Y_ADDR_START +1).*/

#define Y_OUTPUT 1080
static HI_VOID cmos_get_inttime_max(HI_U32 u32Ratio, HI_U32 *pu32IntTimeMax)
{
    HI_U32 u32T2Max;
    
    if(HI_NULL == pu32IntTimeMax)
    {
        printf("null pointer when get ae sensor IntTimeMax value!\n");
        return;
    }
    
    if ((WDR_MODE_2To1_LINE == genSensorMode))
    {
        /* Short + Long < 1V - 2; 
           Ratio = Long * 0x40 / Short */

        u32T2Max = gu32FullLines - 16 - Y_OUTPUT;
        *pu32IntTimeMax = (gu32FullLines - 30) * 0x40 / (u32Ratio + 0x40);

        if ( *pu32IntTimeMax > u32T2Max )
        {
            *pu32IntTimeMax = u32T2Max;
        }        
    }
    
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
    pstExpFuncs->pfn_cmos_dgain_calc_table  = NULL;
    pstExpFuncs->pfn_cmos_get_inttime_max   = cmos_get_inttime_max;
 
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

    pstAwbSnsDft->au16GainOffset[0] = CALIBRATE_STATIC_WB_R_GAIN;
    pstAwbSnsDft->au16GainOffset[1] = CALIBRATE_STATIC_WB_GR_GAIN;
    pstAwbSnsDft->au16GainOffset[2] = CALIBRATE_STATIC_WB_GB_GAIN;
    pstAwbSnsDft->au16GainOffset[3] = CALIBRATE_STATIC_WB_B_GAIN;

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
        case WDR_MODE_2To1_LINE:            
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

static ISP_CMOS_NOISE_TABLE_S g_stIspNoiseTable_FSWDR =
{
    /* bvalid */
    1,

    /* nosie_profile_weight_lut */
    {0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 
        0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 
        0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 
        0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
        0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 
        0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x2D},   

    /* demosaic_weight_lut */
    {0x3, 0xA, 0xF, 0x12, 0x14, 0x17, 0x18, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 0x20, 0x21, 0x22, 0x22, 0x23, 0x24, 0x24, 
        0x25, 0x25, 0x26, 0x26, 0x27, 0x27, 0x28, 0x28, 0x28, 0x29, 0x29, 0x2A, 0x2A, 0x2A, 0x2B, 0x2B, 0x2B, 0x2C, 0x2C, 0x2C, 
        0x2C, 0x2D, 0x2D, 0x2D, 0x2D, 0x2E, 0x2E, 0x2E, 0x2E, 0x2F, 0x2F, 0x2F, 0x2F, 0x30, 0x30, 0x30, 0x30, 0x30, 0x31, 0x31,
        0x31, 0x31, 0x31, 0x32, 0x32, 0x32, 0x32, 0x32, 0x32, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x34, 0x34, 0x34, 0x34, 0x34, 
        0x34, 0x34, 0x35, 0x35, 0x35, 0x35, 0x35, 0x35, 0x35, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x37, 0x37, 
        0x37, 0x37, 0x37, 0x37, 0x37, 0x37, 0x37, 0x38, 0x38, 0x38, 0x38, 0x38, 0x38, 0x38, 0x38, 0x38, 0x38, 0x39, 0x39, 
        0x39, 0x39, 0x39, 0x39, 0x39, 0x39, 0x39, 0x39, 0x39}        
    
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



static ISP_CMOS_GAMMA_S g_stIspGammaFSWDR =
{
    /* bvalid */
    1,
    
#if 0    
    {
        0,   1,   2,   4,   8,  12,  17,  23,  30,  38,  47,  57,  68,  79,  92, 105, 120, 133, 147, 161,
        176, 192, 209, 226, 243, 260, 278, 296, 315, 333, 351, 370, 390, 410, 431, 453, 474, 494, 515,
        536, 558, 580, 602, 623, 644, 665, 686, 708, 730, 751, 773, 795, 818, 840, 862, 884, 907, 929,
        951, 974, 998,1024,1051,1073,1096,1117,1139,1159,1181,1202,1223,1243,1261,1275,1293,1313,
        1332,1351,1371,1389,1408,1427,1446,1464,1482,1499,1516,1533,1549,1567,1583,1600,1616,1633,
        1650,1667,1683,1700,1716,1732,1749,1766,1782,1798,1815,1831,1847,1863,1880,1896,1912,1928,
        1945,1961,1977,1993,2009,2025,2041,2057,2073,2089,2104,2121,2137,2153,2168,2184,2200,2216,
        2231,2248,2263,2278,2294,2310,2326,2341,2357,2373,2388,2403,2419,2434,2450,2466,2481,2496,
        2512,2527,2543,2558,2573,2589,2604,2619,2635,2650,2665,2680,2696,2711,2726,2741,2757,2771,
        2787,2801,2817,2832,2847,2862,2877,2892,2907,2922,2937,2952,2967,2982,2997,3012,3027,3041,
        3057,3071,3086,3101,3116,3130,3145,3160,3175,3190,3204,3219,3234,3248,3263,3278,3293,3307,
        3322,3337,3351,3365,3380,3394,3409,3424,3438,3453,3468,3482,3497,3511,3525,3540,3554,3569,
        3584,3598,3612,3626,3641,3655,3670,3684,3699,3713,3727,3742,3756,3770,3784,3799,3813,3827,
        3841,3856,3870,3884,3898,3912,3927,3941,3955,3969,3983,3997,4011,4026,4039,4054,4068,4082,
        4095
    }
#else  /*higher  contrast*/
    {
        0,1,2,4,8,12,17,23,30,38,47,57,68,79,92,105,120,133,147,161,176,192,209,226,243,260,278,296,
        317,340,365,390,416,440,466,491,517,538,561,584,607,631,656,680,705,730,756,784,812,835,
        858,882,908,934,958,982,1008,1036,1064,1092,1119,1143,1167,1192,1218,1243,1269,1294,1320,
        1346,1372,1398,1424,1450,1476,1502,1528,1554,1580,1607,1633,1658,1684,1710,1735,1761,1786,
        1811,1836,1860,1884,1908,1932,1956,1979,2002,2024,2046,2068,2090,2112,2133,2154,2175,2196,
        2217,2237,2258,2278,2298,2318,2337,2357,2376,2395,2414,2433,2451,2469,2488,2505,2523,2541,
        2558,2575,2592,2609,2626,2642,2658,2674,2690,2705,2720,2735,2750,2765,2779,2793,2807,2821,
        2835,2848,2861,2874,2887,2900,2913,2925,2937,2950,2962,2974,2986,2998,3009,3021,3033,3044,
        3056,3067,3078,3088,3099,3109,3119,3129,3139,3148,3158,3168,3177,3187,3197,3207,3217,3227,
        3238,3248,3259,3270,3281,3292,3303,3313,3324,3335,3346,3357,3368,3379,3389,3400,3410,3421,
        3431,3441,3451,3461,3471,3481,3491,3501,3511,3521,3531,3541,3552,3562,3572,3583,3593,3604,
        3615,3625,3636,3646,3657,3668,3679,3689,3700,3711,3721,3732,3743,3753,3764,3774,3784,3795,
        3805,3816,3826,3837,3847,3858,3869,3880,3891,3902,3913,3925,3937,3949,3961,3973,3985,3997,
        4009,4022,4034,4046,4058,4071,4083,4095
    }
#endif
};

static ISP_CMOS_GAMMAFE_S g_stGammafeFSWDR = 
{
    /* bvalid */
    1,

    /* gamma_fe0 */
    {
        0, 38406, 39281, 40156, 41031, 41907, 42782, 43657, 44532, 45407, 46282, 47158, 48033, 48908, 49783, 50658, 51533, 52409, 53284, 54159, 55034, 55909, 56784, 57660, 58535, 59410, 60285, 61160, 62035, 62911, 63786, 64661, 65535
    },

    /* gamma_fe1 */
    {
        0, 72, 145, 218, 293, 369, 446, 524, 604, 685, 767, 851, 937, 1024, 1113, 1204, 1297, 1391, 1489, 1590, 1692, 1798, 1907, 2020, 2136, 2258, 2383, 2515, 2652, 2798, 2952, 3116, 3295, 3490, 3708, 3961, 4272, 4721, 5954, 6407, 6719, 6972, 7190, 7386, 7564, 7729, 7884, 8029, 8167, 8298, 8424, 8545, 8662, 8774, 8883, 8990, 9092, 9192, 9289, 9385, 9478, 9569, 9658, 9745, 9831, 9915, 9997, 10078, 10158, 10236, 10313, 10389, 10463, 10538, 10610, 10682, 10752, 10823, 10891, 10959, 11026, 11094, 11159, 11224, 11289, 11352, 11415, 11477, 11539, 11600, 11660, 11720, 11780, 11838, 11897, 11954, 12012, 12069, 12125, 12181, 12236, 12291, 12346, 12399, 12453, 12507, 12559, 12612, 12664, 12716, 12768, 12818, 12869, 12919, 12970, 13020, 13069, 13118, 13166, 13215, 13263, 13311, 13358, 13405, 13453, 13500, 13546, 13592, 13638, 13684, 13730, 13775, 13820, 13864, 13909, 13953, 13997, 14041, 14085, 14128, 14172, 14214, 14257, 14299, 14342, 14384, 14426, 14468, 14509, 14551, 14592, 16213, 17654, 18942, 20118, 21208, 22227, 23189, 24101, 24971, 25804, 26603, 27373, 28118, 28838, 29538, 30219, 30881, 31527, 32156, 32772, 33375, 33964, 34541, 35107, 35663, 36208, 36745, 37272, 37790, 38301, 38803, 39298, 39785, 40267, 40741, 41210, 41672, 42128, 42580, 43026, 43466, 43901, 44332, 44757, 45179, 45596, 46008, 46417, 46821, 47222, 47619, 48011, 48400, 48785, 49168, 49547, 49924, 50296, 50666, 51033, 51397, 51758, 52116, 52472, 52825, 53175, 53522, 53868, 54211, 54551, 54889, 55225, 55558, 55889, 56218, 56545, 56870, 57193, 57514, 57833, 58150, 58465, 58778, 59090, 59399, 59708, 60014, 60318, 60621, 60922, 61222, 61520, 61816, 62111, 62403, 62695, 62985, 63275, 63562, 63848, 64132, 64416, 64698, 64978, 65258, 65535
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
        
        case WDR_MODE_2To1_LINE: 
            pstDef->stDrc.bEnable               = HI_TRUE;
            pstDef->stDrc.u32BlackLevel         = 0x00;
            pstDef->stDrc.u32WhiteLevel         = 0xFFF; 
            pstDef->stDrc.u32SlopeMax           = 0x38;
            pstDef->stDrc.u32SlopeMin           = 0xC0;
            pstDef->stDrc.u32VarianceSpace      = 0x0A;
            pstDef->stDrc.u32VarianceIntensity  = 0x04;
            pstDef->stDrc.u32Asymmetry          = 0x14;
            pstDef->stDrc.u32BrightEnhance      = 0xC8;

            memcpy(&pstDef->stNoiseTbl, &g_stIspNoiseTable_FSWDR, sizeof(ISP_CMOS_NOISE_TABLE_S));            
            memcpy(&pstDef->stAgcTbl, &g_stIspAgcTable, sizeof(ISP_CMOS_AGC_TABLE_S));
            memcpy(&pstDef->stDemosaic, &g_stIspDemosaic, sizeof(ISP_CMOS_DEMOSAIC_S));
            memcpy(&pstDef->stRgbSharpen, &g_stIspRgbSharpen, sizeof(ISP_CMOS_RGBSHARPEN_S));        
            memcpy(&pstDef->stGamma, &g_stIspGammaFSWDR, sizeof(ISP_CMOS_GAMMA_S));
            memcpy(&pstDef->stGammafe, &g_stGammafeFSWDR, sizeof(ISP_CMOS_GAMMAFE_S));
        break;
        
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
    HI_U16 u16Bl;
    
    if (HI_NULL == pstBlackLevel)
    {
        printf("null pointer when get isp black level value!\n");
        return -1;
    }

    /* Don't need to update black level when iso change */
    pstBlackLevel->bUpdate = HI_FALSE;

    u16Bl = (WDR_MODE_NONE == genSensorMode)? 0xA8 : 0xA6;

    for (i=0; i<4; i++)
    {
        pstBlackLevel->au16BlackLevel[i] = u16Bl;
    }

    return 0;    
}

HI_VOID cmos_set_pixel_detect(HI_BOOL bEnable)
{
    
    HI_U32 u32FullLines_5Fps = FRAME_LINES_2M_LIN * 30 / 5;
    HI_U32 u32MaxExpTime_5Fps = u32FullLines_5Fps - 2;
    
    if (WDR_MODE_BUILT_IN == genSensorMode)
    {
        return;
    }

    u32FullLines_5Fps = (u32FullLines_5Fps > 0xFFFF) ? 0xFFFF : u32FullLines_5Fps;
    u32MaxExpTime_5Fps = u32FullLines_5Fps - 2; 
    
    if (bEnable) /* setup for ISP pixel calibration mode */
    {
        sensor_write_register(FRAME_LINES, u32FullLines_5Fps); /* 5fps */
        sensor_write_register(EXPOSURE_TIME_L, u32MaxExpTime_5Fps); /* max exposure time */
        sensor_write_register(ANALOG_GAIN, 0x0);      /* AG, Context A */
        sensor_write_register(DIGITAL_GAIN, 0x0080);  /* DG, Context A */
    }
    else /* setup for ISP 'normal mode' */
    {
        gu32FullLinesStd = (gu32FullLinesStd > 0xFFFF) ? 0xFFFF : gu32FullLinesStd;
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

        case WDR_MODE_2To1_LINE:
            genSensorMode = WDR_MODE_2To1_LINE;
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
        g_stSnsRegsInfo.u8Cfg2ValidDelayMax = 3;
        g_stSnsRegsInfo.u32RegNum = 6;
        for (i=0; i < g_stSnsRegsInfo.u32RegNum; i++)
        {
            g_stSnsRegsInfo.astI2cData[i].bUpdate = HI_TRUE;
            g_stSnsRegsInfo.astI2cData[i].u8DevAddr = sensor_i2c_addr;
            g_stSnsRegsInfo.astI2cData[i].u32AddrByteNum = sensor_addr_byte;
            g_stSnsRegsInfo.astI2cData[i].u32DataByteNum = sensor_data_byte;
        }
        g_stSnsRegsInfo.astI2cData[0].u8DelayFrmNum = 1;
        g_stSnsRegsInfo.astI2cData[0].u32RegAddr = EXPOSURE_TIME_S;
        g_stSnsRegsInfo.astI2cData[1].u8DelayFrmNum = 1;
        g_stSnsRegsInfo.astI2cData[1].u32RegAddr = EXPOSURE_TIME_L;
        g_stSnsRegsInfo.astI2cData[2].u8DelayFrmNum = 1;
        g_stSnsRegsInfo.astI2cData[2].u32RegAddr = ANALOG_GAIN;
        g_stSnsRegsInfo.astI2cData[3].u8DelayFrmNum = 1;
        g_stSnsRegsInfo.astI2cData[3].u32RegAddr = DIGITAL_GAIN;
        g_stSnsRegsInfo.astI2cData[4].u8DelayFrmNum = 0;
        g_stSnsRegsInfo.astI2cData[4].u32RegAddr = FRAME_LINES;
        
        // related registers in DCG mode
        g_stSnsRegsInfo.astI2cData[5].u8DelayFrmNum = 1;
        g_stSnsRegsInfo.astI2cData[5].u32RegAddr = 0x3100;
      
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
    gu32FullLinesStd = FRAME_LINES_2M_LIN;
    gu32FullLines = FRAME_LINES_2M_LIN; 
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
    s32Ret = HI_MPI_ISP_SensorRegCallBack(IspDev, AR0237_ID, &stIspRegister);
    if (s32Ret)
    {
        printf("sensor register callback function failed!\n");
        return s32Ret;
    }
    
    stLib.s32Id = 0;
    strncpy(stLib.acLibName, HI_AE_LIB_NAME, sizeof(HI_AE_LIB_NAME));
    cmos_init_ae_exp_function(&stAeRegister.stSnsExp);
    s32Ret = HI_MPI_AE_SensorRegCallBack(IspDev, &stLib, AR0237_ID, &stAeRegister);
    if (s32Ret)
    {
        printf("sensor register callback function to ae lib failed!\n");
        return s32Ret;
    }

    stLib.s32Id = 0;
    strncpy(stLib.acLibName, HI_AWB_LIB_NAME, sizeof(HI_AWB_LIB_NAME));
    cmos_init_awb_exp_function(&stAwbRegister.stSnsExp);
    s32Ret = HI_MPI_AWB_SensorRegCallBack(IspDev, &stLib, AR0237_ID, &stAwbRegister);
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

    s32Ret = HI_MPI_ISP_SensorUnRegCallBack(IspDev, AR0237_ID);
    if (s32Ret)
    {
        printf("sensor unregister callback function failed!\n");
        return s32Ret;
    }
    
    stLib.s32Id = 0;
    strncpy(stLib.acLibName, HI_AE_LIB_NAME, sizeof(HI_AE_LIB_NAME));
    s32Ret = HI_MPI_AE_SensorUnRegCallBack(IspDev, &stLib, AR0237_ID);
    if (s32Ret)
    {
        printf("sensor unregister callback function to ae lib failed!\n");
        return s32Ret;
    }

    stLib.s32Id = 0;
    strncpy(stLib.acLibName, HI_AWB_LIB_NAME, sizeof(HI_AWB_LIB_NAME));
    s32Ret = HI_MPI_AWB_SensorUnRegCallBack(IspDev, &stLib, AR0237_ID);
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
