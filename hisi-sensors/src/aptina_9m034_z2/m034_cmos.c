#if !defined(__M034_CMOS_H_)
#define __M034_CMOS_H_

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

#define M034_ID 9034

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

#define EXPOSURE_TIME  (0x3012)
#define ANALOG_GAIN    (0x30B0)
#define DIGITAL_GAIN   (0x305E)
#define FRAME_LINES    (0x300A)
#define LINE_LEN_PCK   (0x300C)

#define SENSOR_720P_30FPS_MODE  (1) 
#define SENSOR_960P_30FPS_MODE  (2) 

#define INCREASE_LINES (0) /* make real fps less than stand fps because NVR require*/
#define FRAME_LINES_720P  (750+INCREASE_LINES)
#define FRAME_LINES_960P  (990+INCREASE_LINES)
#define LINE_LENGTH_PCK_720P_30FPS   (3300) 
#define LINE_LENGTH_PCK_960P_30FPS   (2500) 

HI_U8 gu8SensorImageMode = SENSOR_720P_30FPS_MODE;
WDR_MODE_E genSensorMode = WDR_MODE_NONE;

static HI_U32 gu32FullLinesStd = FRAME_LINES_720P;
static HI_U32 gu32FullLines = FRAME_LINES_720P; 
static HI_U32 gu32LineLength = LINE_LENGTH_PCK_720P_30FPS;
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
#define CMOS_CFG_INI "9m034_cfg.ini"
static char pcName[PATHLEN_MAX] = "configs/9m034_cfg.ini";


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
    pstAeSnsDft->stAgainAccu.f32Accuracy = 6;

    pstAeSnsDft->stDgainAccu.enAccuType = AE_ACCURACY_LINEAR;
    pstAeSnsDft->stDgainAccu.f32Accuracy = 0.03125;
    
    pstAeSnsDft->u32ISPDgainShift = 8;
    pstAeSnsDft->u32MinISPDgainTarget = 1 << pstAeSnsDft->u32ISPDgainShift;
    pstAeSnsDft->u32MaxISPDgainTarget = 32 << pstAeSnsDft->u32ISPDgainShift;

    pstAeSnsDft->enMaxIrisFNO = ISP_IRIS_F_NO_1_0;
    pstAeSnsDft->enMinIrisFNO = ISP_IRIS_F_NO_32_0;

    pstAeSnsDft->bAERouteExValid = HI_FALSE;
    pstAeSnsDft->stAERouteAttr.u32TotalNum = 0;
    pstAeSnsDft->stAERouteAttrEx.u32TotalNum = 0;
    
    switch(genSensorMode)
    {
        default:
        case WDR_MODE_NONE:   /*linear mode*/
            pstAeSnsDft->au8HistThresh[0] = 0xd;
            pstAeSnsDft->au8HistThresh[1] = 0x28;
            pstAeSnsDft->au8HistThresh[2] = 0x60;
            pstAeSnsDft->au8HistThresh[3] = 0x80;
            
            pstAeSnsDft->u8AeCompensation = g_AeDft[0].u8AeCompensation;
            
            pstAeSnsDft->u32MaxIntTime = gu32FullLinesStd - 2;
            pstAeSnsDft->u32MinIntTime = 2;
            pstAeSnsDft->u32MaxIntTimeTarget = g_AeDft[0].u32MaxIntTimeTarget;
            pstAeSnsDft->u32MinIntTimeTarget = g_AeDft[0].u32MinIntTimeTarget;
            
            pstAeSnsDft->u32MaxAgain = 8192;  
            pstAeSnsDft->u32MinAgain = 1024;
            pstAeSnsDft->u32MaxAgainTarget = g_AeDft[0].u32MaxAgainTarget;
            pstAeSnsDft->u32MinAgainTarget = g_AeDft[0].u32MinAgainTarget;
            
            pstAeSnsDft->u32MaxDgain = 255;  
            pstAeSnsDft->u32MinDgain = 32;
            pstAeSnsDft->u32MaxDgainTarget = g_AeDft[0].u32MaxDgainTarget;
            pstAeSnsDft->u32MinDgainTarget = g_AeDft[0].u32MinDgainTarget;
          
            pstAeSnsDft->u32ISPDgainShift = g_AeDft[0].u32ISPDgainShift;
            pstAeSnsDft->u32MinISPDgainTarget = g_AeDft[0].u32MinISPDgainTarget;
            pstAeSnsDft->u32MaxISPDgainTarget = g_AeDft[0].u32MaxISPDgainTarget;    
        break;
        
        case WDR_MODE_BUILT_IN:
            pstAeSnsDft->au8HistThresh[0] = 0x20;
            pstAeSnsDft->au8HistThresh[1] = 0x40;
            pstAeSnsDft->au8HistThresh[2] = 0x60;
            pstAeSnsDft->au8HistThresh[3] = 0x80;
            
            pstAeSnsDft->u8AeCompensation = g_AeDft[1].u8AeCompensation;
            
            pstAeSnsDft->u32MaxIntTime = 675;
            pstAeSnsDft->u32MinIntTime = 128;
            pstAeSnsDft->u32MaxIntTimeTarget = g_AeDft[1].u32MaxIntTimeTarget;
            pstAeSnsDft->u32MinIntTimeTarget = g_AeDft[1].u32MinIntTimeTarget;
            
            pstAeSnsDft->u32MaxAgain = 8192;  
            pstAeSnsDft->u32MinAgain = 1024;
            pstAeSnsDft->u32MaxAgainTarget = g_AeDft[1].u32MaxAgainTarget;
            pstAeSnsDft->u32MinAgainTarget = g_AeDft[1].u32MinAgainTarget;
            
            pstAeSnsDft->u32MaxDgain = 255;  
            pstAeSnsDft->u32MinDgain = 32;
            pstAeSnsDft->u32MaxDgainTarget = g_AeDft[1].u32MaxDgainTarget;
            pstAeSnsDft->u32MinDgainTarget = g_AeDft[1].u32MinDgainTarget;
          
            pstAeSnsDft->u32ISPDgainShift = g_AeDft[1].u32ISPDgainShift;
            pstAeSnsDft->u32MinISPDgainTarget = g_AeDft[1].u32MinISPDgainTarget;
            pstAeSnsDft->u32MaxISPDgainTarget = g_AeDft[1].u32MaxISPDgainTarget;    
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
    pstAeSnsDft->stIntTimeAccu.f32Offset = 0;

    pstAeSnsDft->stAgainAccu.enAccuType = AE_ACCURACY_TABLE;
    pstAeSnsDft->stAgainAccu.f32Accuracy = 6;

    pstAeSnsDft->stDgainAccu.enAccuType = AE_ACCURACY_LINEAR;
    pstAeSnsDft->stDgainAccu.f32Accuracy = 0.03125;
    
    pstAeSnsDft->u32ISPDgainShift = 8;
    pstAeSnsDft->u32MinISPDgainTarget = 1 << pstAeSnsDft->u32ISPDgainShift;

    //memcpy(&pstAeSnsDft->stPirisAttr, &gstPirisAttr, sizeof(ISP_PIRIS_ATTR_S));
    pstAeSnsDft->enMaxIrisFNO = ISP_IRIS_F_NO_1_4;
    pstAeSnsDft->enMinIrisFNO = ISP_IRIS_F_NO_5_6;

    pstAeSnsDft->bAERouteExValid = HI_FALSE;
    pstAeSnsDft->stAERouteAttr.u32TotalNum = 0;
    pstAeSnsDft->stAERouteAttrEx.u32TotalNum = 0;

    switch(genSensorMode)
    {
        default:
        case WDR_MODE_NONE:   /*linear mode*/
            pstAeSnsDft->au8HistThresh[0] = 0xd;
            pstAeSnsDft->au8HistThresh[1] = 0x28;
            pstAeSnsDft->au8HistThresh[2] = 0x60;
            pstAeSnsDft->au8HistThresh[3] = 0x80;
            
            pstAeSnsDft->u8AeCompensation = 0x40;
            
            pstAeSnsDft->u32MaxIntTime = gu32FullLinesStd - 2;
            pstAeSnsDft->u32MinIntTime = 2;    
            pstAeSnsDft->u32MaxIntTimeTarget = 65535;
            pstAeSnsDft->u32MinIntTimeTarget = 2;

            pstAeSnsDft->u32MaxAgain = 8192;  /* (8mult)/0.0078125 = 1024 */
            pstAeSnsDft->u32MinAgain = 1024;
            pstAeSnsDft->u32MaxAgainTarget = pstAeSnsDft->u32MaxAgain;
            pstAeSnsDft->u32MinAgainTarget = pstAeSnsDft->u32MinAgain;
            
            pstAeSnsDft->u32MaxDgain = 255; 
            pstAeSnsDft->u32MinDgain = 32;
            pstAeSnsDft->u32MaxDgainTarget = pstAeSnsDft->u32MaxDgain;
            pstAeSnsDft->u32MinDgainTarget = pstAeSnsDft->u32MinDgain; 
            
            pstAeSnsDft->u32MaxISPDgainTarget = 4 << pstAeSnsDft->u32ISPDgainShift;   
        break;
        
        case WDR_MODE_BUILT_IN:
            pstAeSnsDft->au8HistThresh[0] = 0x20;
            pstAeSnsDft->au8HistThresh[1] = 0x40;
            pstAeSnsDft->au8HistThresh[2] = 0x60;
            pstAeSnsDft->au8HistThresh[3] = 0x80;
            
            pstAeSnsDft->u8AeCompensation = 0x40;
            
            pstAeSnsDft->u32MaxIntTime = 675;
            pstAeSnsDft->u32MinIntTime = 128;    
            pstAeSnsDft->u32MaxIntTimeTarget = 65535;
            pstAeSnsDft->u32MinIntTimeTarget = 2;

            pstAeSnsDft->u32MaxAgain = 8192;  
            pstAeSnsDft->u32MinAgain = 1024;
            pstAeSnsDft->u32MaxAgainTarget = pstAeSnsDft->u32MaxAgain;
            pstAeSnsDft->u32MinAgainTarget = pstAeSnsDft->u32MinAgain;
            
            pstAeSnsDft->u32MaxDgain = 255; 
            pstAeSnsDft->u32MinDgain = 32;
            pstAeSnsDft->u32MaxDgainTarget = pstAeSnsDft->u32MaxDgain;
            pstAeSnsDft->u32MinDgainTarget = pstAeSnsDft->u32MinDgain;
            
            pstAeSnsDft->u32MaxISPDgainTarget = 32 << pstAeSnsDft->u32ISPDgainShift;   
        break;
    }
    return 0;
}

#endif

/* the function of sensor set fps */
static HI_VOID cmos_fps_set(HI_FLOAT f32Fps, AE_SENSOR_DEFAULT_S *pstAeSnsDft)
{
    switch (genSensorMode)
    {
        default :
        case WDR_MODE_NONE :
            if (SENSOR_720P_30FPS_MODE == gu8SensorImageMode)
            {
                if ((f32Fps <= 30) && (f32Fps >= 1.51))
                {
                    gu32LineLength = (LINE_LENGTH_PCK_720P_30FPS * 30) / f32Fps; 
                }
                else
                {
                    printf("Not support Fps: %f\n", f32Fps);
                    return;
                }
                gu32FullLinesStd = FRAME_LINES_720P;
            }
            else if (SENSOR_960P_30FPS_MODE == gu8SensorImageMode)
            {
                if ((f32Fps <= 30) && (f32Fps >= 1.14))
                {
                    gu32LineLength = (LINE_LENGTH_PCK_960P_30FPS * 30) / f32Fps; 
                }
                else
                {
                    printf("Not support Fps: %f\n", f32Fps);
                    return;
                }
                gu32FullLinesStd = FRAME_LINES_960P;  
            }
            else
            {
                printf("Not support! gu8SensorImageMode:%d, f32Fps:%f\n", gu8SensorImageMode, f32Fps);
                return;
            }

            pstAeSnsDft->u32MaxIntTime = gu32FullLinesStd - 2;
            break;
        case WDR_MODE_BUILT_IN :
            if (SENSOR_720P_30FPS_MODE == gu8SensorImageMode)
            {
                if ((f32Fps == 30) || (f32Fps == 25))
                {
                    gu32LineLength = (LINE_LENGTH_PCK_720P_30FPS * 30) / f32Fps; 
                }
                else
                {
                    printf("Not support Fps: %f\n", f32Fps);
                    return;
                }
                gu32FullLinesStd = FRAME_LINES_720P;
                
            }
            else if (SENSOR_960P_30FPS_MODE == gu8SensorImageMode)
            {
                if ((f32Fps == 30) && (f32Fps == 25))
                {
                    gu32LineLength = (LINE_LENGTH_PCK_960P_30FPS * 30) / f32Fps; 
                }
                else
                {
                    printf("Not support Fps: %f\n", f32Fps);
                    return;
                }
                gu32FullLinesStd = FRAME_LINES_960P;  
            }
            else
            {
                printf("Not support! gu8SensorImageMode:%d, f32Fps:%f\n", gu8SensorImageMode, f32Fps);
                return;
            }

            pstAeSnsDft->u32MaxIntTime = 675;
            break;
    }
    

    gu32LineLength = gu32LineLength > FULL_LINES_MAX ? FULL_LINES_MAX : gu32LineLength;

    g_stSnsRegsInfo.astI2cData[3].u32Data = gu32FullLinesStd;
    g_stSnsRegsInfo.astI2cData[4].u32Data = gu32LineLength;
    
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

static HI_U32 analog_gain_table[4] =
{
     1024, 2048, 4096, 8192
};

static HI_VOID cmos_again_calc_table(HI_U32 *pu32AgainLin, HI_U32 *pu32AgainDb)
{
    int i;

    if((HI_NULL == pu32AgainLin) ||(HI_NULL == pu32AgainDb))
    {
        printf("null pointer when get ae sensor gain info  value!\n");
        return;
    }

    if (*pu32AgainLin >= analog_gain_table[3])
    {
         *pu32AgainLin = analog_gain_table[3];
         *pu32AgainDb = 3;
         return ;
    }
    
    for (i = 1; i < 4; i++)
    {
        if (*pu32AgainLin < analog_gain_table[i])
        {
            *pu32AgainLin = analog_gain_table[i - 1];
            *pu32AgainDb = i - 1;
            break;
        }
    }

    return;
}


static HI_VOID cmos_gains_update(HI_U32 u32Again, HI_U32 u32Dgain)
{
    switch(u32Again)
    {
        case 0:
            g_stSnsRegsInfo.astI2cData[1].u32Data = 0x1300;
            break;
        case 1:
            g_stSnsRegsInfo.astI2cData[1].u32Data = 0x1310;
            break;
        case 2:
            g_stSnsRegsInfo.astI2cData[1].u32Data = 0x1320;
            break;
        case 3:
            g_stSnsRegsInfo.astI2cData[1].u32Data = 0x1330;
            break;
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
    5120,
    {
       0x01F1,0x80B0,0x8041,
       0x803E,0x0159,0x801B,
       0x0010,0x8094,0x0184,
    },

    3633,
    {
        0x01CE,0x8086,0x8048,
        0x805A,0x0168,0x800E,
        0x0002,0x80A2,0x01A0,
    },

    2449,
    {
        0x020B,0x80A9,0x8062,
        0x8051,0x0169,0x8018,
        0x0017,0x814B,0x0234,
    }

};

static AWB_AGC_TABLE_S g_stAwbAgcTableLin =
{
    /* bvalid */
    1,

    /* saturation */
    {0x80,0x80,0x7C,0x76,0x58,0x4c,0x40,0x38,0x38,0x38,0x38,0x38,0x38,0x38,0x38,0x38}
};

static AWB_AGC_TABLE_S g_stAwbAgcTableWDR =
{
    /* bvalid */
    1,

    /* saturation */
    {0x80,0x80,0x7c,0x76,0x70,0x64,0x54,0x48,0x40,0x38,0x38,0x38,0x38,0x38,0x38,0x38}
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

    pstAwbSnsDft->au16GainOffset[0] = 0x1A5;
    pstAwbSnsDft->au16GainOffset[1] = 0x100;
    pstAwbSnsDft->au16GainOffset[2] = 0x100;
    pstAwbSnsDft->au16GainOffset[3] = 0x187;

    pstAwbSnsDft->as32WbPara[0] = 0x0042;
    pstAwbSnsDft->as32WbPara[1] = 0x0013;
    pstAwbSnsDft->as32WbPara[2] = -0x00AB;
    pstAwbSnsDft->as32WbPara[3] = 0x3318B;
    pstAwbSnsDft->as32WbPara[4] = 0x80;
    pstAwbSnsDft->as32WbPara[5] = -0x27CEA;

    memcpy(&pstAwbSnsDft->stCcm, &g_stAwbCcm, sizeof(AWB_CCM_S));
    
    switch (genSensorMode)
    {
        default:
        case WDR_MODE_NONE:
            memcpy(&pstAwbSnsDft->stAgcTbl, &g_stAwbAgcTableLin, sizeof(AWB_AGC_TABLE_S));
        break;

        case WDR_MODE_BUILT_IN:
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
        
        case WDR_MODE_2To1_FRAME:       
            memcpy(&pstDef->stDrc, &g_IspDft[1].stDrc, sizeof(ISP_CMOS_DRC_S));
            memcpy(&pstDef->stNoiseTbl, &g_IspDft[1].stNoiseTbl, sizeof(ISP_CMOS_NOISE_TABLE_S));            
            memcpy(&pstDef->stDemosaic, &g_IspDft[1].stDemosaic, sizeof(ISP_CMOS_DEMOSAIC_S));
            memcpy(&pstDef->stRgbSharpen, &g_IspDft[1].stRgbSharpen, sizeof(ISP_CMOS_RGBSHARPEN_S));            
            memcpy(&pstDef->stGamma, &g_IspDft[1].stGamma, sizeof(ISP_CMOS_GAMMA_S));
            memcpy(&pstDef->stGammafe, &g_IspDft[1].stGammafe, sizeof(ISP_CMOS_GAMMAFE_S));
            memcpy(&pstDef->stGe, &g_IspDft[1].stGe, sizeof(ISP_CMOS_GE_S));
        break;
    }
    pstDef->stSensorMaxResolution.u32MaxWidth  = 1280;
    pstDef->stSensorMaxResolution.u32MaxHeight = 720;

    return 0;
}

#else



#define  DMNR_CALIB_CARVE_NUM_9M034  (13)
static HI_FLOAT g_coef_calib_9m034[DMNR_CALIB_CARVE_NUM_9M034][HI_ISP_NR_CALIB_COEF_COL] = 
{
    {100.000000f,   2.000000f, 0.035900f, 9.163901f}, 
    {611.000000f,   2.786041f, 0.039199f, 9.662953f}, 
    {1104.000000f,  3.042969f, 0.042015f, 10.098933f}, 
    {2317.000000f,  3.364926f, 0.046898f, 11.608693f}, 
    {4625.000000f,  3.665112f, 0.053912f, 14.938090f}, 
    {5923.000000f,  3.772542f, 0.056674f, 17.071514f}, 
    {8068.000000f,  3.906766f, 0.061652f, 20.065725f}, 
    {9985.000000f,  3.999348f, 0.063942f, 22.888287f}, 
    {20395.000000f, 4.309524f, 0.062935f, 41.496876f}, 
    {40067.000000f, 4.602787f, 0.053383f, 77.137268f}, 
    {49630.000000f, 4.695744f, 0.030656f, 104.676781f},
    {60637.000000f, 4.782738f, 0.000000f, 138.000000f}, 
    {89225.000000f, 4.950487f, 0.000000f, 171.000000f}, 
};

static HI_FLOAT g_coef_calib_9m034WDR[DMNR_CALIB_CARVE_NUM_9M034][HI_ISP_NR_CALIB_COEF_COL] = 
{
    {2785.000000, 3.444825, 0.061053, 3.400960, }, 
    {4517.000000, 3.654850, 0.041061, 9.879556, }, 
    {9736.000000, 3.988381, 0.049405, 8.447605, }, 
    {24105.000000, 4.382107, 0.065567, 2.961599, }, 
    {49705.000000, 4.696400, 0.051170, 6.569161, }, 
    {75952.000000, 4.880539, 0.036218, 11.878284, }, 
    {82899.000000, 4.918549, 0.034525, 12.647687, },
    {82899.000000, 4.918549, 0.034525, 12.647687, },
    {82899.000000, 4.918549, 0.034525, 12.647687, },
    {82899.000000, 4.918549, 0.034525, 12.647687, },
    {82899.000000, 4.918549, 0.034525, 12.647687, },
    {82899.000000, 4.918549, 0.034525, 12.647687, },
    {82899.000000, 4.918549, 0.034525, 12.647687, },
   
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
	{512,512,512,512,512,512,512,400,0,0,0,0,0,0,0,0}    /*au16NpOffset[ISP_AUTO_ISO_STENGTH_NUM]*/	
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
	400, /*u16UuSlope*/	
	{512,512,512,512,512,512,512,  400,  0,0,0,0,0,0,0,0}    /*au16NpOffset[ISP_AUTO_ISO_STENGTH_NUM]*/
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
		//15,13//																																																																																																																																																																																																																																																													
            1,   105,   175,   237,   293,   346,   396,   444,   489,   533,   576,   618,   658,   698,   736,   774,
          811,   847,   883,   918,   952,   986,  1019,  1052,  1085,  1116,  1148,  1179,  1210,  1240,  1270,  1300,
         1329,  1358,  1386,  1415,  1443,  1470,  1498,  1525,  1552,  1578,  1605,  1631,  1657,  1683,  1708,  1734,
         1759,  1783,  1808,  1832,  1857,  1881,  1905,  1928,  1952,  1975,  1998,  2021,  2044,  2067,  2089,  2112,
         2134,  2156,  2178,  2200,  2221,  2243,  2264,  2285,  2307,  2327,  2348,  2369,  2390,  2410,  2430,  2451,
         2471,  2491,  2510,  2530,  2550,  2569,  2589,  2608,  2627,  2646,  2665,  2684,  2703,  2722,  2740,  2759,
         2777,  2796,  2814,  2832,  2850,  2868,  2886,  2904,  2921,  2939,  2957,  2974,  2992,  3009,  3026,  3043,
         3060,  3077,  3094,  3111,  3128,  3144,  3161,  3178,  3194,  3211,  3227,  3243,  3260,  3276,  3292,  3308,
         3324,  3340,  3356,  3371,  3387,  3403,  3418,  3434,  3449,  3465,  3480,  3495,  3510,  3526,  3541,  3556,
         3571,  3586,  3601,  3616,  3630,  3645,  3660,  3675,  3689,  3704,  3718,  3733,  3747,  3761,  3776,  3790,
         3804,  3818,  3833,  3847,  3861,  3875,  3889,  3903,  3916,  4103,  4283,  4457,  4625,  4789,  4948,  5103,
         5256,  5405,  5552,  5696,  5839,  5983,  6127,  6270,  6411,  6552,  6692,  6831,  6970,  7109,  7247,  7385,
         7523,  7727,  7932,  8137,  8343,  8550,  8757,  8966,  9176,  9600, 10029, 10464, 10905, 11836, 12789, 13762,
        14752, 15756, 16773, 17799, 18834, 19875, 20922, 21973, 23027, 24083, 25141, 26201, 27262, 28323, 29385, 30448,
        31511, 32574, 33637, 34700, 35764, 36827, 37888, 38950, 40013, 41077, 42140, 43204, 44267, 45330, 46394, 47457,
        48521, 49584, 50648, 51711, 52775, 53838, 54902, 55965, 57028, 58092, 59155, 60219, 61282, 62346, 63409, 64475,
        65535
	}
};

static ISP_CMOS_RGBSHARPEN_S g_stIspRgbSharpen =
{      
  //{100,	200,	400,	800,	1600,	3200,	6400,	12800,	25600,	51200,	102400,	204800,	409600,	819200,	1638400,	3276800}; //ISO
	{0,		0,		0,		0,		1,		1,		1,		1,		1,		1,		1,		1,		1,		1,		1,			1},/* bEnLowLumaShoot */
    {35,	30,		30, 	30, 	30, 	30,  	30,  	25,    	21,    	15,    	12,    	12,    	12,    	12,    	12,    		12},/*SharpenUD*/
    {25,	25,		28,	 	28, 	30, 	30,  	30, 	35, 	40, 	40, 	40, 	50, 	50, 	50, 	50, 		50},/*SharpenD*/
    {10,    10,  	12,  	14, 	16, 	18,  	20,  	22,    	24,    	26,    	28,    	30,    	30,    	30,    	30,    		30},/*TextureNoiseThd*/
    {10,    10,  	12,  	14, 	16, 	18,  	20,  	22,    	24,    	26,    	28,    	30,    	30,    	30,    	30,    		30},/*EdgeNoiseThd*/
    {  150, 130,  	120,  	110,  	110,  	60,    	40,    	30,    	20,    	10,    	0,    	0,    	0,    	0,    	0,    		0},/*overshoot*/
    {  160, 160,  	160,  	160, 	200, 	200,  	200,  	200,   	200,  	220,  	255,  	255,   	255,  	255,  	255,  		255},/*undershoot*/
};

static ISP_CMOS_RGBSHARPEN_S g_stIspRgbSharpenWDR =
{      
  //{100,	200,	400,	800,	1600,	3200,	6400,	12800,	25600,	51200,	102400,	204800,	409600,	819200,	1638400,	3276800}; //ISO
	{  0,	 0,		 0,		 0,		 1,		 1,		 1,		 1,		 1,		 1,		 1,		 1,		 1,		 1,		1,			1},/* bEnLowLumaShoot */
    { 40,	45,		50, 	55, 	50, 	45,  	40,  	40,    	40,    	40,    	43,    	50,    	50,    	50,    	50,    		50},/*SharpenUD*/
    { 35,	40,		45, 	45, 	50, 	40,  	40, 	45, 	45, 	45, 	40, 	40, 	40, 	40, 	40, 		40},/*SharpenD*/
    {10,    10,  	12,  	14, 	16, 	18,  	20,  	22,    	24,    	26,    	28,    	30,    	30,    	30,    	30,    		30},/*TextureNoiseThd*/
    {10,    10,  	12,  	14, 	16, 	14,  	12,  	8,    	4,    	8,    	12,    	16,    	24,    	30,    	30,    		30},/*EdgeNoiseThd*/
    { 50,   50,  	60,   	60,   	60,     60,     60,     70,    	70,    	80,    	80,    	80,    	80,    	80,    	80,    		80},/*overshoot*/
    {150,  150,    150,  	140, 	140,   140,    140,    150,    160,    170,    200,    200,  	200,  	200,  	200,  		200},/*undershoot*/
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
	{70,130,244,248,250,252,252,252,252,252,252,252,252,252,252,252},/*au16Strength[16]*/
	{0,0,0,0,0,0,0,0,0,0x23,0x80,0xD0,0xF0,0xF0,0xF0,0xF0},/*au16BlendRatio[16]*/
};

static ISP_CMOS_DPC_S g_stCmosDpcWDR = 
{
	//0,/*IR_channel*/
	//0,/*IR_position*/
	{70,130,244,248,250,252,252,252,252,252,252,252,252,252,252,252},/*au16Strength[16]*/
	{0,0,0,0,0,0,0,0,0,0x23,0x80,0xD0,0xF0,0xF0,0xF0,0xF0},/*au16BlendRatio[16]*/
};

static ISP_CMOS_COMPANDER_S g_stCmosCompander =
{
	12,
	20,
	64,
	64,
	95,
	2048,
	125,
	32768,
	125,
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
    192,
    60,
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
    1,
    192,
    60,
    0,
    0,
    0,
    {1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024}
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
			
			pstDef->stNoiseTbl.stNrCaliPara.u8CalicoefRow = DMNR_CALIB_CARVE_NUM_9M034;
			pstDef->stNoiseTbl.stNrCaliPara.pCalibcoef    = (HI_FLOAT (*)[4])g_coef_calib_9m034;
			memcpy(&pstDef->stNoiseTbl.stIsoParaTable[0], &g_stNrIsoParaTab[0],sizeof(ISP_NR_ISO_PARA_TABLE_S)*HI_ISP_NR_ISO_LEVEL_MAX);
			

			memcpy(&pstDef->stRgbSharpen, &g_stIspRgbSharpen, sizeof(ISP_CMOS_RGBSHARPEN_S));
			memcpy(&pstDef->stUvnr,       &g_stIspUVNR,       sizeof(ISP_CMOS_UVNR_S));
			memcpy(&pstDef->stDpc,       &g_stCmosDpc,       sizeof(ISP_CMOS_DPC_S));
        break;
        case WDR_MODE_BUILT_IN:

            memcpy(&pstDef->stDrc, &g_stIspDrcWDR, sizeof(ISP_CMOS_DRC_S));
            memcpy(&pstDef->stDemosaic, &g_stIspDemosaicWDR, sizeof(ISP_CMOS_DEMOSAIC_S));
            memcpy(&pstDef->stGe, &g_stIspGeWDR, sizeof(ISP_CMOS_GE_S));			

			pstDef->stNoiseTbl.stNrCaliPara.u8CalicoefRow = DMNR_CALIB_CARVE_NUM_9M034;
			pstDef->stNoiseTbl.stNrCaliPara.pCalibcoef    = (HI_FLOAT (*)[4])g_coef_calib_9m034WDR;
			memcpy(&pstDef->stNoiseTbl.stIsoParaTable[0], &g_stNrIsoParaTabWDR[0],sizeof(ISP_NR_ISO_PARA_TABLE_S)*HI_ISP_NR_ISO_LEVEL_MAX);
			
            memcpy(&pstDef->stGammafe, &g_stGammafe, sizeof(ISP_CMOS_GAMMAFE_S));
         
			memcpy(&pstDef->stRgbSharpen, &g_stIspRgbSharpenWDR, sizeof(ISP_CMOS_RGBSHARPEN_S));
			memcpy(&pstDef->stUvnr,       &g_stIspUVNR,       sizeof(ISP_CMOS_UVNR_S));
			memcpy(&pstDef->stDpc,       &g_stCmosDpcWDR,       sizeof(ISP_CMOS_DPC_S));
			memcpy(&pstDef->stCompander,  &g_stCmosCompander, sizeof(ISP_CMOS_COMPANDER_S));
			
        break;
    }
    pstDef->stSensorMaxResolution.u32MaxWidth  = 1280;
    pstDef->stSensorMaxResolution.u32MaxHeight = 720;

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
                pstBlackLevel->au16BlackLevel[i] = 0xC0;
            }
            break;
    }

    return 0;    
}


HI_VOID cmos_set_pixel_detect(HI_BOOL bEnable)
{
    HI_U32 u32FullLines_5Fps = FRAME_LINES_720P;
    
    if (SENSOR_720P_30FPS_MODE == gu8SensorImageMode)
    {
        u32FullLines_5Fps = LINE_LENGTH_PCK_720P_30FPS * 30 / 5;
    } 
    else if(SENSOR_960P_30FPS_MODE == gu8SensorImageMode)
    {
        u32FullLines_5Fps = LINE_LENGTH_PCK_960P_30FPS * 30 / 5;
    }
    else
    {
        return;
    }

    u32FullLines_5Fps = (u32FullLines_5Fps > FULL_LINES_MAX) ? FULL_LINES_MAX : u32FullLines_5Fps;
    
    if (bEnable) /* setup for ISP pixel calibration mode */
    {
        sensor_write_register(LINE_LEN_PCK, u32FullLines_5Fps);  /* 5fps */
        sensor_write_register(EXPOSURE_TIME, 0x118);     /* max exposure time */ 
        sensor_write_register(ANALOG_GAIN, 0x1300);      /* AG, Context A */
        sensor_write_register(DIGITAL_GAIN, 0x0020);     /* DG, Context A */
    }
    else /* setup for ISP 'normal mode' */
    {
        if(SENSOR_720P_30FPS_MODE == gu8SensorImageMode)
        {
            sensor_write_register(LINE_LEN_PCK, LINE_LENGTH_PCK_720P_30FPS);   
        }
        else if(SENSOR_960P_30FPS_MODE == gu8SensorImageMode)
        {
            sensor_write_register(LINE_LEN_PCK, LINE_LENGTH_PCK_960P_30FPS);   
        }
        else
        {
        }
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

            /* Disable DCG */
            sensor_write_register(0x3100, 0x0000);

            /* Disable 1.25x analog gain */
            sensor_write_register(0x3EE4, 0xD208);
            
            printf("linear mode\n");
        break;

        case WDR_MODE_BUILT_IN:
            genSensorMode = WDR_MODE_BUILT_IN;

            /* Disable DCG */
            sensor_write_register(0x3100, 0x001A);

            /* Disable 1.25x analog gain */
            sensor_write_register(0x3EE4, 0xD208);
        
            printf("WDR mode\n");
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

    if((pstSensorImageMode->u16Width <= 1280)&&(pstSensorImageMode->u16Height <= 720))
    {
        if (pstSensorImageMode->f32Fps <= 30)
        {
            u8SensorImageMode = SENSOR_720P_30FPS_MODE;
        }
        else
        {
            printf("Not support! Width:%d, Height:%d, Fps:%f\n", 
                pstSensorImageMode->u16Width, 
                pstSensorImageMode->u16Height,
                pstSensorImageMode->f32Fps);

            return -1;
        }
    } 
    else if((pstSensorImageMode->u16Width <= 1280)&&(pstSensorImageMode->u16Height <= 960))
    {
        if (pstSensorImageMode->f32Fps <= 30)
        {
            u8SensorImageMode = SENSOR_960P_30FPS_MODE;
        }
        else
        {
            printf("Not support! Width:%d, Height:%d, Fps:%f\n", 
                pstSensorImageMode->u16Width, 
                pstSensorImageMode->u16Height,
                pstSensorImageMode->f32Fps);

            return -1;
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
        g_stSnsRegsInfo.u32RegNum = 5;
             
        for (i=0; i<g_stSnsRegsInfo.u32RegNum; i++)
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
        g_stSnsRegsInfo.astI2cData[4].u8DelayFrmNum = 0;
        g_stSnsRegsInfo.astI2cData[4].u32RegAddr = LINE_LEN_PCK;
        
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
    gu8SensorImageMode = SENSOR_720P_30FPS_MODE;
    genSensorMode = WDR_MODE_NONE; 
    gu32FullLinesStd = FRAME_LINES_720P;
    gu32FullLines = FRAME_LINES_720P; 
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
    s32Ret = HI_MPI_ISP_SensorRegCallBack(IspDev, M034_ID, &stIspRegister);
    if (s32Ret)
    {
        printf("sensor register callback function failed!\n");
        return s32Ret;
    }
    
    stLib.s32Id = 0;
    strncpy(stLib.acLibName, HI_AE_LIB_NAME, sizeof(HI_AE_LIB_NAME));
    cmos_init_ae_exp_function(&stAeRegister.stSnsExp);
    s32Ret = HI_MPI_AE_SensorRegCallBack(IspDev, &stLib, M034_ID, &stAeRegister);
    if (s32Ret)
    {
        printf("sensor register callback function to ae lib failed!\n");
        return s32Ret;
    }

    stLib.s32Id = 0;
    strncpy(stLib.acLibName, HI_AWB_LIB_NAME, sizeof(HI_AWB_LIB_NAME));
    cmos_init_awb_exp_function(&stAwbRegister.stSnsExp);
    s32Ret = HI_MPI_AWB_SensorRegCallBack(IspDev, &stLib, M034_ID, &stAwbRegister);
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

    s32Ret = HI_MPI_ISP_SensorUnRegCallBack(IspDev, M034_ID);
    if (s32Ret)
    {
        printf("sensor unregister callback function failed!\n");
        return s32Ret;
    }
    
    stLib.s32Id = 0;
    strncpy(stLib.acLibName, HI_AE_LIB_NAME, sizeof(HI_AE_LIB_NAME));
    s32Ret = HI_MPI_AE_SensorUnRegCallBack(IspDev, &stLib, M034_ID);
    if (s32Ret)
    {
        printf("sensor unregister callback function to ae lib failed!\n");
        return s32Ret;
    }

    stLib.s32Id = 0;
    strncpy(stLib.acLibName, HI_AWB_LIB_NAME, sizeof(HI_AWB_LIB_NAME));
    s32Ret = HI_MPI_AWB_SensorUnRegCallBack(IspDev, &stLib, M034_ID);
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
