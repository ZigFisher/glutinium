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

HI_U8 gu8SensorImageMode = SENSOR_2M_1080p30_MODE;
WDR_MODE_E genSensorMode = WDR_MODE_NONE;

static HI_U32 gu32FullLinesStd = FRAME_LINES_2M_1080p;
static HI_U32 gu32FullLines = FRAME_LINES_2M_1080p; 
static HI_BOOL bInit = HI_FALSE;
HI_BOOL bSensorInit = HI_FALSE; 

ISP_SNS_REGS_INFO_S g_stSnsRegsInfo = {0};
ISP_SNS_REGS_INFO_S g_stPreSnsRegsInfo = {0};

#if 0
/* Piris attr */
static ISP_PIRIS_ATTR_S gstPirisAttr=
{
    0,      // bStepFNOTableChange
    1,      // bZeroIsMax
    94,     // u16TotalStep
    62,     // u16StepCount
    /* Step-F number mapping table. Must be from small to large. F1.0 is 1024 and F32.0 is 1 */
    {30,35,40,45,50,56,61,67,73,79,85,92,98,105,112,120,127,135,143,150,158,166,174,183,191,200,208,217,225,234,243,252,261,270,279,289,298,307,316,325,335,344,353,362,372,381,390,399,408,417,426,435,444,453,462,470,478,486,493,500,506,512},
    ISP_IRIS_F_NO_1_4, // enMaxIrisFNOTarget
    ISP_IRIS_F_NO_5_6,  // enMinIrisFNOTarget
    0,                 // bFNOExValid
    512,              // u32MaxIrisFNOTarget  
    30                  // u32MinIrisFNOTarget
};
#endif

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
    pstAeSnsDft->stIntTimeAccu.f32Offset = 0.33;

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

    //memcpy(&pstAeSnsDft->stPirisAttr, &gstPirisAttr, sizeof(ISP_PIRIS_ATTR_S));
    pstAeSnsDft->enMaxIrisFNO = ISP_IRIS_F_NO_1_4;
    pstAeSnsDft->enMinIrisFNO = ISP_IRIS_F_NO_5_6;

    pstAeSnsDft->bAERouteExValid = HI_FALSE;
    pstAeSnsDft->stAERouteAttr.u32TotalNum = 0;
    pstAeSnsDft->stAERouteAttrEx.u32TotalNum = 0;

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
        }
        else if (25 == f32Fps)
        {
            gu32FullLinesStd = FRAME_LINES_2M_1080p * 30 /f32Fps;
        }
        else
        {
            printf("Not support Fps: %f\n", f32Fps);
            return;
        }

        pstAeSnsDft->u32MaxIntTime = gu32FullLinesStd*16/17;
    }
    else
    {
        if (SENSOR_2M_1080p30_MODE == gu8SensorImageMode)
        {
            if ((f32Fps <= 30) && (f32Fps >= 0.51))
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
            if ((f32Fps <= 60) && (f32Fps >= 1.03))
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
    u32FullLines = (u32FullLines > FULL_LINES_MAX) ? FULL_LINES_MAX : u32FullLines;
    gu32FullLines = u32FullLines;
    pstAeSnsDft->u32FullLines = gu32FullLines;
  
    if (WDR_MODE_NONE == genSensorMode)
    {
        g_stSnsRegsInfo.astI2cData[3].u32Data = gu32FullLines;
    }
    else
    {
       pstAeSnsDft->u32FullLines = gu32FullLinesStd;
       return;
    }
  
    pstAeSnsDft->u32MaxIntTime = gu32FullLines - 2;
    
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
    switch (genSensorMode)
    {
        default:
        case WDR_MODE_NONE:
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
        break;
        
        case WDR_MODE_BUILT_IN:
            pstAwbSnsDft->u16WbRefTemp = g_AwbDft[1].u16WbRefTemp;
            for(i= 0; i < 4; i++)
            {
                pstAwbSnsDft->au16GainOffset[i] = g_AwbDft[1].au16GainOffset[i];
            }
           
            for(i= 0; i < 6; i++)
            {
                pstAwbSnsDft->as32WbPara[i] = g_AwbDft[1].as32WbPara[i];
            }

            memcpy(&pstAwbSnsDft->stCcm, &g_AwbDft[1].stCcm, sizeof(AWB_CCM_S));            
            memcpy(&pstAwbSnsDft->stAgcTbl, &g_AwbDft[1].stAgcTbl, sizeof(AWB_AGC_TABLE_S));
        break;
        
    }
    return 0;
}

#else

static AWB_CCM_S g_stAwbCcm =
{
    5050,
    {
        0x01EA, 0x80AF, 0x803B,
        0x805C, 0x0174, 0x8018,
        0x8001, 0x809A, 0x019B,
    },
        
    3932,
    {
        0x01B5, 0x805F, 0x8056,
        0x807B, 0x018F, 0x8014,
        0x8011, 0x8097, 0x01A8,
    },

    2658,
    {
        0x01FF, 0x8087, 0x8078,
        0x807E, 0x019F, 0x8021,
        0x0002, 0x8134, 0x0232,
    }
};

static AWB_CCM_S g_stAwbCcmWDR =
{
    5050,
    {
        0x01B3, 0x80AB, 0x8008,
        0x805C, 0x0142, 0x001A,
        0x803D, 0x810C, 0x0249,
    },
        
    3932,
    {
        0x0130, 0x8031, 0x0001,
        0x8041, 0x010C, 0x0035,
        0x8021, 0x8095, 0x01B6,
    },

    2658,
    {
        0x012E, 0x8024, 0x800A,
        0x8060, 0x0138, 0x0028,
        0x8053, 0x80F5, 0x0248,
    }
};

static AWB_AGC_TABLE_S g_stAwbAgcTableLin =
{
    /* bvalid */
    1,

    /* saturation */ 
    {0x80,0x80,0x72,0x68,0x60,0x58,0x50,0x48,0x40,0x38,0x38,0x38,0x38,0x38,0x38,0x38}
};

static AWB_AGC_TABLE_S g_stAwbAgcTableWDR =
{
    /* bvalid */
    1,

    /* saturation */
    {0x80,0x78,0x72,0x68,0x60,0x58,0x50,0x48,0x40,0x38,0x38,0x38,0x38,0x38,0x38,0x38}
};


static HI_S32 cmos_get_awb_default(AWB_SENSOR_DEFAULT_S *pstAwbSnsDft)
{
    if (HI_NULL == pstAwbSnsDft)
    {
        printf("null pointer when get awb default value!\n");
        return -1;
    }

    memset(pstAwbSnsDft, 0, sizeof(AWB_SENSOR_DEFAULT_S));

    pstAwbSnsDft->u16WbRefTemp = 5000;

    pstAwbSnsDft->au16GainOffset[0] = 0x182;
    pstAwbSnsDft->au16GainOffset[1] = 0x100;
    pstAwbSnsDft->au16GainOffset[2] = 0x100;
    pstAwbSnsDft->au16GainOffset[3] = 0x182;

    pstAwbSnsDft->as32WbPara[0] = -0x0069;
    pstAwbSnsDft->as32WbPara[1] = 0x0169;
    pstAwbSnsDft->as32WbPara[2] = 0x0;
    pstAwbSnsDft->as32WbPara[3] = 0x3247E;
    pstAwbSnsDft->as32WbPara[4] = 0x80;
    pstAwbSnsDft->as32WbPara[5] = -0x25D51;
    
    
    switch (genSensorMode)
    {
        default:
        case WDR_MODE_NONE:
            
            memcpy(&pstAwbSnsDft->stCcm, &g_stAwbCcm, sizeof(AWB_CCM_S));
            memcpy(&pstAwbSnsDft->stAgcTbl, &g_stAwbAgcTableLin, sizeof(AWB_AGC_TABLE_S));
        break;

        case WDR_MODE_BUILT_IN:
            
            memcpy(&pstAwbSnsDft->stCcm, &g_stAwbCcmWDR, sizeof(AWB_CCM_S));
            memcpy(&pstAwbSnsDft->stAgcTbl, &g_stAwbAgcTableWDR, sizeof(AWB_AGC_TABLE_S));
        break;
    }
    
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
            memcpy(&pstDef->stNoiseTbl, &g_IspDft[0].stNoiseTbl, sizeof(ISP_CMOS_NOISE_TABLE_S));            
            memcpy(&pstDef->stDemosaic, &g_IspDft[0].stDemosaic, sizeof(ISP_CMOS_DEMOSAIC_S));
            memcpy(&pstDef->stRgbSharpen, &g_IspDft[0].stRgbSharpen, sizeof(ISP_CMOS_RGBSHARPEN_S));
            memcpy(&pstDef->stGamma, &g_IspDft[0].stGamma, sizeof(ISP_CMOS_GAMMA_S));
            memcpy(&pstDef->stGe, &g_IspDft[0].stGe, sizeof(ISP_CMOS_GE_S));            
        break;
        case WDR_MODE_BUILT_IN:            
            memcpy(&pstDef->stDrc, &g_IspDft[1].stDrc, sizeof(ISP_CMOS_DRC_S));

            memcpy(&pstDef->stNoiseTbl, &g_IspDft[1].stNoiseTbl, sizeof(ISP_CMOS_NOISE_TABLE_S));            
            memcpy(&pstDef->stDemosaic, &g_IspDft[1].stDemosaic, sizeof(ISP_CMOS_DEMOSAIC_S));
            memcpy(&pstDef->stRgbSharpen, &g_IspDft[1].stRgbSharpen, sizeof(ISP_CMOS_RGBSHARPEN_S));            
            memcpy(&pstDef->stGamma, &g_IspDft[1].stGamma, sizeof(ISP_CMOS_GAMMA_S));
            memcpy(&pstDef->stGammafe, &g_IspDft[1].stGammafe, sizeof(ISP_CMOS_GAMMAFE_S));
            memcpy(&pstDef->stGe, &g_IspDft[1].stGe, sizeof(ISP_CMOS_GE_S));
        break;

    }

    pstDef->stSensorMaxResolution.u32MaxWidth  = 1920;
    pstDef->stSensorMaxResolution.u32MaxHeight = 1080;

    return 0;
}

#else


#define  DMNR_CALIB_CARVE_NUM_AR0230  (12)
static HI_FLOAT g_coef_calib_AR0230[DMNR_CALIB_CARVE_NUM_AR0230][HI_ISP_NR_CALIB_COEF_COL] = 
{
    {117.000000f, 2.068186f, 0.038048f, 8.539988f, }, 
    {234.000000f, 2.369216f, 0.038854f, 8.647237f, }, 
    {338.000000f, 2.528917f, 0.038449f, 8.986108f, }, 
    {507.000000f, 2.705008f, 0.039314f, 9.173982f, }, 
    {788.000000f, 2.896526f, 0.042165f, 9.135517f, }, 
    {945.000000f, 2.975432f, 0.042747f, 9.325232f, }, 
    {1182.000000f, 3.072618f, 0.044354f, 9.478379f, }, 
    {1583.000000f, 3.199481f, 0.046800f, 9.752023f, }, 
    {3135.000000f, 3.496238f, 0.053930f, 11.135441f, }, 
    {6201.000000f, 3.792462f, 0.063879f, 14.289218f, }, 
    {9014.000000f, 3.954918f, 0.070638f, 17.333263f, }, 
    {13598.000000f, 4.133475f, 0.078309f, 22.591230f, }, 
};

static HI_FLOAT g_coef_calib_AR0230WDR[DMNR_CALIB_CARVE_NUM_AR0230][HI_ISP_NR_CALIB_COEF_COL] = 
{
    {303.000000f, 2.481443f, 0.024227f, 13.260330f, }, 
    {389.000000f, 2.589950f, 0.026167f, 12.662060f, }, 
    {880.000000f, 2.944483f, 0.024233f, 14.136049f, }, 
    {1200.000000f, 3.079181f, 0.023359f, 14.992138f, }, 
    {1594.000000f, 3.202488f, 0.020216f, 18.146648f, }, 
    {2711.000000f, 3.433130f, 0.000000f, 27.000000f, }, 
    {5422.000000f, 3.734159f, 0.000000f, 26.000000f, }, 
    {6786.000000f, 3.831614f, 0.000000f, 26.000000f, }, 
    {9072.000000f, 3.957703f, 0.000000f, 25.000000f, }, 
    {9072.000000f, 3.957703f, 0.000000f, 25.000000f, }, 
    {9072.000000f, 3.957703f, 0.000000f, 25.000000f, }, 
    {9072.000000f, 3.957703f, 0.000000f, 25.000000f, }, 
};

static ISP_NR_ISO_PARA_TABLE_S g_stNrIsoParaTab[HI_ISP_NR_ISO_LEVEL_MAX] = 
{
     //u16Threshold//u8varStrength//u8fixStrength//u8LowFreqSlope	
       {1750,       256-224,             256-256,            0 },  //100    //                       //                                                
       {1750,       256-224,             256-256,            0 },  //200    // ISO //Thr//var//fix   // ISO //u8LowFreqSlope
       {1750,       256-224,             256-256,            0 },  //400    //{400,  1400, 80,256},  //{400 , 0  }
       {1750,       256-224,             256-256,            8 },  //800    //{800,  1400, 72,256},  //{600 , 2  }
       {1750,       256-208,             256-256,            6 },  //1600   //{1600, 1400, 64,256},  //{800 , 8  }
       {1750,       256-208,             256-256,            0 },  //3200   //{3200, 1400, 48,230},  //{1000, 12 }
       {1750,       256-208,             256-256,            0 },  //6400   //{6400, 1400, 48,210},  //{1600, 6  }
       {1750,       256-204,             256-256,            0 },  //12800  //{12000,1400, 32,180},  //{2400, 0  }
       {1625,       256-204,             256-256,            0 },  //25600  //{36000,1300, 48,160},  //           
       {1375,       256-204,             256-256,            0 },  //51200  //{64000,1100, 40,140},  //           
       {1250,       256-204,             256-256,            0 },  //102400 //{82000,1000, 30,128},  //           
       {1250,       256-192,             256-256,            0 },  //204800 //                       //
       {1250,       256-192,             256-256,            0 },  //409600 //                       //
       {1250,       256-192,             256-256,            0 },  //819200 //                       //
       {1250,       256-192,             256-256,            0 },  //1638400//                       //
       {1250,       256-192,             256-256,            0 },  //3276800//                       //
};

static ISP_NR_ISO_PARA_TABLE_S g_stNrIsoParaTabWDR[HI_ISP_NR_ISO_LEVEL_MAX] = 
{
     //u16Threshold//u8varStrength//u8fixStrength//u8LowFreqSlope	
       {1500,       256-240,            256-256,            0 },  //100    //                      //                                                
       {1500,       256-240,            256-256,            0 },  //200    // ISO //Thr//var//fix  // ISO //u8LowFreqSlope
       {1500,       256-240,            256-256,            0 },  //400    //{400,  1200,240,256}, //{400 , 0  }
       {1500,       256-220,            256-256,            4 },  //800    //{800,  1200,220,256}, //{600 , 2  }
       {1500,       256-200,            256-256,            4 },  //1600   //{1600, 1200,200,256}, //{800 , 4  }
       {1500,       256-200,            256-256,            0 },  //3200   //{3200, 1200,200,256}, //{1000, 6  }
       {1500,       256-200,            256-256,            0 },  //6400   //{6400, 1200,200,256}, //{1600, 4  }
       {1500,       256-200,            256-256,            0 },  //12800  //{12000,1200,200,256}, //{2400, 0  }
       {1500,       256-180,            256-256,            0 },  //25600  //{36000,1200,180,256}, //           
       {1375,       256-180,            256-256,            0 },  //51200  //{64000,1100,180,256}, //           
       {1375,       256-180,            256-256,            0 },  //102400 //{82000,1100,180,256}, //           
       {1375,       256-180,            256-256,            0 },  //204800 //                      //
       {1375,       256-180,            256-256,            0 },  //409600 //                      //
       {1375,       256-180,            256-256,            0 },  //819200 //                      //
       {1375,       256-180,            256-256,            0 },  //1638400//                      //
       {1375,       256-180,            256-256,            0 },  //3276800//                      //
};



static ISP_CMOS_DEMOSAIC_S g_stIspDemosaicLin =
{
	/*For Demosaic*/
	1, /*bEnable*/			
	12,/*u16VhLimit*/	
	8,/*u16VhOffset*/
	48,   /*u16VhSlope*/
	/*False Color*/
	1,    /*bFcrEnable*/
	{24,24,24,24,24,24,24,24,10, 0, 0, 0, 0, 0, 0, 0},    /*au8FcrStrength[ISP_AUTO_ISO_STENGTH_NUM]*/
	{24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,24},    /*au8FcrThreshold[ISP_AUTO_ISO_STENGTH_NUM]*/	
	/*For Ahd*/
	400, /*u16UuSlope*/	
	{512,512,512,512,512,512,512,  400,  0,0,0,0,0,0,0,0}    /*au16NpOffset[ISP_AUTO_ISO_STENGTH_NUM]*/	
};


static ISP_CMOS_DEMOSAIC_S g_stIspDemosaicWDR =
{
	/*For Demosaic*/
	1, /*bEnable*/			
	24,/*u16VhLimit*/	
	16,/*u16VhOffset*/
	48,   /*u16VhSlope*/
	/*False Color*/
	1,    /*bFcrEnable*/
	{12,12,12,12,12,12,12,12, 5, 0, 0, 0, 0, 0, 0, 0},    /*au8FcrStrength[ISP_AUTO_ISO_STENGTH_NUM]*/
	{24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,24},    /*au8FcrThreshold[ISP_AUTO_ISO_STENGTH_NUM]*/	
	/*For Ahd*/
	200, /*u16UuSlope*/	
	{512,512,512,512,512,512,512,400,0,0,0,0,0,0,0,0}    /*au16NpOffset[ISP_AUTO_ISO_STENGTH_NUM]*/
};

static ISP_CMOS_GE_S g_stIspGeLin =
{
	/*For GE*/
	0,    /*bEnable*/			
	7,    /*u8Slope*/	
	7,    /*u8Sensitivity*/
	4096, /*u16Threshold*/
	4096, /*u16SensiThreshold*/	
	{1024,1024,1024,2048,2048,2048,2048,2048,2048,2048,2048,2048,2048,2048,2048,2048}    /*au16Strength[ISP_AUTO_ISO_STENGTH_NUM]*/	
};

static ISP_CMOS_GE_S g_stIspGeWDR =
{
	/*For GE*/
	0,    /*bEnable*/			
	7,    /*u8Slope*/	
	7,    /*u8Sensitivity*/
	4096, /*u16Threshold*/
	4096, /*u16SensiThreshold*/	
	{1024,1024,1024,2048,2048,2048,2048,2048,2048,2048,2048,2048,2048,2048,2048,2048}    /*au16Strength[ISP_AUTO_ISO_STENGTH_NUM]*/	
};

static ISP_CMOS_GAMMAFE_S g_stGammafe = 
{
	/* bvalid */
	1,
	
	/* gamma_fe0 */
	{	   
		//10,8//
		
		/*0	 ,	43008,	46130,	49152,	51200,	52224,	52717,	53210,	53703,	54196,	54689,	55182,	55675,	56168,	56661,	57154,	57647,	58140,	58633,	59127,	59620,	60113,	60606,	61099,	61592,	62085,	62578,	63071,	63564,	64057,	64550,	65043,	65535*/

		//14,12
		0,43008,46130,49152,51200,52224,52717,53210,53703,54196,54689,55182,55675,56168,56661,57154,57647,58140,58633,59127,59620,60113,60606,61099,61592,62085,62578,63071,63564,64057,64550,65043,65535
	},

	/* gamma_fe1 */
	{
		//16,14	
         1,    68,   118,   163,   205,   244,   282,   318,   353,   388,   421,   454,   485,   517,   547,   577,
       607,   636,   665,   693,   721,   748,   776,   802,   829,   855,   881,   907,   932,   957,   982,  1007,
      1031,  1055,  1079,  1103,  1127,  1150,  1173,  1196,  1219,  1241,  1264,  1286,  1309,  1330,  1352,  1374,
      1395,  1417,  1438,  1459,  1480,  1500,  1521,  1542,  1562,  1582,  1603,  1622,  1642,  1662,  1682,  1702,
      1721,  1740,  1760,  1779,  1798,  1817,  1836,  1854,  1873,  1892,  1910,  1928,  1947,  1965,  1983,  2001,
      2019,  2037,  2054,  2072,  2090,  2107,  2125,  2142,  2159,  2177,  2194,  2211,  2228,  2245,  2262,  2278,
      2295,  2312,  2328,  2345,  2361,  2378,  2394,  2410,  2427,  2443,  2459,  2475,  2491,  2507,  2523,  2538,
      2554,  2570,  2585,  2601,  2616,  2632,  2647,  2663,  2678,  2693,  2708,  2723,  2739,  2754,  2769,  2784,
      2799,  2813,  2828,  2843,  2858,  2872,  2887,  2902,  2916,  2931,  2945,  2960,  2974,  2988,  3003,  3017,
      3031,  3045,  3059,  3073,  3087,  3102,  3115,  3129,  3143,  3157,  3171,  3185,  3199,  3212,  3226,  3240,
      3253,  3267,  3280,  3294,  3307,  3321,  3334,  3348,  3361,  3542,  3718,  3889,  4057,  4220,  4381,  4539,
      4694,  4847,  4999,  5149,  5297,  5448,  5599,  5749,  5899,  6048,  6196,  6344,  6492,  6639,  6787,  6934,
      7082,  7300,  7518,  7737,  7957,  8177,  8398,  8620,  8844,  9293,  9747, 10205, 10667, 11640, 12629, 13633,
     14648, 15674, 16708, 17749, 18796, 19846, 20900, 21956, 23014, 24074, 25135, 26196, 27259, 28321, 29384, 30447,
     31510, 32573, 33637, 34700, 35764, 36827, 37888, 38950, 40013, 41077, 42140, 43204, 44267, 45330, 46394, 47457,
     48521, 49584, 50648, 51711, 52775, 53838, 54902, 55965, 57028, 58092, 59155, 60219, 61282, 62346, 63409, 64475,
     65535
	}
};

static ISP_CMOS_RGBSHARPEN_S g_stIspRgbSharpen =
{      
  //{100,	200,	400,	800,	1600,	3200,	6400,	12800,	25600,	51200,	102400,	204800,	409600,	819200,	1638400,	3276800}; //ISO
	{0,		0,		0,		0,		1,		1,		1,		1,		1,		1,		1,		1,		1,		1,		1,			1},/* bEnLowLumaShoot */
    {40,	40,		35, 	35, 	35, 	30,  	30,  	25,    	21,    	15,    	12,    	12,    	12,    	12,    	12,    		12},/*SharpenUD*/
    {30,	30,		32,	 	32, 	35, 	35,  	37, 	37, 	40, 	45, 	50, 	50, 	50, 	50, 	50, 		50},/*SharpenD*/
    {10,    10,  	12,  	14, 	16, 	18,  	20,  	22,    	24,    	26,    	28,    	30,    	30,    	30,    	30,    		30},/*TextureNoiseThd*/
    {10,    10,  	12,  	14, 	16, 	18,  	20,  	22,    	24,    	26,    	28,    	30,    	30,    	30,    	30,    		30},/*EdgeNoiseThd*/
    {  150, 140,  	120,  	110,  	110,  	60,    	40,    	30,    	20,    	10,    	0,    	0,    	0,    	0,    	0,    		0},/*overshoot*/
    {  160, 160,  	160,  	160, 	200, 	200,  	200,  	200,   	200,  	220,  	255,  	255,   	255,  	255,  	255,  		255},/*undershoot*/
};

static ISP_CMOS_RGBSHARPEN_S g_stIspRgbSharpenWDR =
{      
  //{100,	200,	400,	800,	1600,	3200,	6400,	12800,	25600,	51200,	102400,	204800,	409600,	819200,	1638400,	3276800}; //ISO
	{  0,	 0,		 0,		 0,		 1,		 1,		 1,		 1,		 1,		 1,		 1,		 1,		 1,		 1,		1,			1},/* bEnLowLumaShoot */
    { 55,	55,		55, 	55, 	50, 	45,  	40,  	40,    	40,    	40,    	43,    	50,    	50,    	50,    	50,    		50},/*SharpenUD*/
    { 50,	50,		50, 	50, 	50, 	40,  	40, 	45, 	45, 	45, 	40, 	40, 	40, 	40, 	40, 		40},/*SharpenD*/
    {10,    10,  	12,  	14, 	16, 	18,  	20,  	22,    	24,    	26,    	28,    	30,    	30,    	30,    	30,    		30},/*TextureNoiseThd*/
    {10,    10,  	12,  	14, 	16, 	14,  	12,  	8,    	4,    	8,    	12,    	16,    	24,    	30,    	30,    		30},/*EdgeNoiseThd*/
    { 70,   70,  	70,   	60,   	60,     60,     60,     70,    	70,    	80,    	80,    	80,    	80,    	80,    	80,    		80},/*overshoot*/
    {170,  170,    170,  	160, 	160,   160,    160,    150,    160,    170,    200,    200,  	200,  	200,  	200,  		200},/*undershoot*/
};

static ISP_CMOS_UVNR_S g_stIspUVNR = 
{
  //{100,	200,	400,	800,	1600,	3200,	6400,	12800,	25600,	51200,	102400,	204800,	409600,	819200,	1638400,	3276800}; //ISO
	{1,		2,		4,		6,		8,		10,		12,		14,		16,		18,		20,		22,		22,		22,		22,			22},      /*u8UvnrThreshold*/
 	{0,		0,		0,		0,		0,		0,		0,		0,		1,		1,		2,		2,		2,		2,		2,			2},       /*ColorCast*/
 	{0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,			0}  /*u8UvnrStrength*/
};

static ISP_CMOS_DPC_S g_stCmosDpc = 
{
	//0,/*IR_channel*/
	//0,/*IR_position*/
	{45,90,152,202,220,224,224,224,224,224,224,224,224,224,224,224},/*au16Strength[16]*/
	{0,0,0,0,0,0,0,0,0,0x23,0x80,0xD0,0xF0,0xF0,0xF0,0xF0},/*au16BlendRatio[16]*/
};

static ISP_CMOS_DPC_S g_stCmosDpcWDR = 
{
	//0,/*IR_channel*/
	//0,/*IR_position*/
	{45,90,152,202,220,224,224,224,224,224,224,224,224,224,224,224},/*au16Strength[16]*/
	{0,0,0,0,0,0,0,0,0,0x23,0x80,0xD0,0xF0,0xF0,0xF0,0xF0},/*au16BlendRatio[16]*/
};

static ISP_CMOS_COMPANDER_S g_stCmosCompander =
{
	12,
	16,
	32,
	512,
	80,
	2048,
	108,
	16384,
	124,
	32768,
	129,
	32768
};

static ISP_CMOS_DRC_S g_stIspDrcLin =
{
    0,
    10,
    0,
    2,
    180,
    54,
    0,
    0,
    0,
    {1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024}
};

static ISP_CMOS_DRC_S g_stIspDrcWDR =
{
    1,
    10,
    0,
    2,
    192,
    54,
    0,
    0,
    0,
    {900,900,900,900,900,900,900,900,900,900,900,900,900,900,900,900,890,880,870,860,850,840,830,820,810,800,790,780,770,760,750,740,720}
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
        default:
        case WDR_MODE_NONE:
            
            memcpy(&pstDef->stDrc, &g_stIspDrcLin, sizeof(ISP_CMOS_DRC_S));
            memcpy(&pstDef->stDemosaic, &g_stIspDemosaicLin, sizeof(ISP_CMOS_DEMOSAIC_S));
            memcpy(&pstDef->stGe, &g_stIspGeLin, sizeof(ISP_CMOS_GE_S));			
			
			pstDef->stNoiseTbl.stNrCaliPara.u8CalicoefRow = DMNR_CALIB_CARVE_NUM_AR0230;
			pstDef->stNoiseTbl.stNrCaliPara.pCalibcoef    = (HI_FLOAT (*)[4])g_coef_calib_AR0230;
			memcpy(&pstDef->stNoiseTbl.stIsoParaTable[0], &g_stNrIsoParaTab[0],sizeof(ISP_NR_ISO_PARA_TABLE_S)*HI_ISP_NR_ISO_LEVEL_MAX);
			

			memcpy(&pstDef->stRgbSharpen, &g_stIspRgbSharpen, sizeof(ISP_CMOS_RGBSHARPEN_S));
			memcpy(&pstDef->stUvnr,       &g_stIspUVNR,       sizeof(ISP_CMOS_UVNR_S));
			memcpy(&pstDef->stDpc,       &g_stCmosDpc,       sizeof(ISP_CMOS_DPC_S));
            
        break;
        case WDR_MODE_BUILT_IN:
            
            memcpy(&pstDef->stDrc, &g_stIspDrcWDR, sizeof(ISP_CMOS_DRC_S));
            memcpy(&pstDef->stDemosaic, &g_stIspDemosaicWDR, sizeof(ISP_CMOS_DEMOSAIC_S));	
            memcpy(&pstDef->stGe, &g_stIspGeWDR, sizeof(ISP_CMOS_GE_S));

			pstDef->stNoiseTbl.stNrCaliPara.u8CalicoefRow = DMNR_CALIB_CARVE_NUM_AR0230;
			pstDef->stNoiseTbl.stNrCaliPara.pCalibcoef    = (HI_FLOAT (*)[4])g_coef_calib_AR0230WDR;
			memcpy(&pstDef->stNoiseTbl.stIsoParaTable[0], &g_stNrIsoParaTabWDR[0],sizeof(ISP_NR_ISO_PARA_TABLE_S)*HI_ISP_NR_ISO_LEVEL_MAX);
			
            memcpy(&pstDef->stGammafe, &g_stGammafe, sizeof(ISP_CMOS_GAMMAFE_S));
         
			memcpy(&pstDef->stRgbSharpen, &g_stIspRgbSharpenWDR, sizeof(ISP_CMOS_RGBSHARPEN_S));
			memcpy(&pstDef->stUvnr,       &g_stIspUVNR,       sizeof(ISP_CMOS_UVNR_S));
			memcpy(&pstDef->stDpc,       &g_stCmosDpcWDR,       sizeof(ISP_CMOS_DPC_S));
			memcpy(&pstDef->stCompander,  &g_stCmosCompander, sizeof(ISP_CMOS_COMPANDER_S));
			
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

    switch (genSensorMode)
    {
        default :
        case WDR_MODE_NONE :
            for (i=0; i<4; i++)
            {
                pstBlackLevel->au16BlackLevel[i] = 0xA8;
            }
            break;
        case WDR_MODE_BUILT_IN :
            for (i=0; i<4; i++)
            {
                pstBlackLevel->au16BlackLevel[i] = 0xA8;
            }
            break;
    }

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
            g_stSnsRegsInfo.astI2cData[8].u32Data = 0x480;
            g_stSnsRegsInfo.astI2cData[9].u8DelayFrmNum = 0;
            g_stSnsRegsInfo.astI2cData[9].u32RegAddr = 0x3098;
            g_stSnsRegsInfo.astI2cData[9].u32Data = 0x480;
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
