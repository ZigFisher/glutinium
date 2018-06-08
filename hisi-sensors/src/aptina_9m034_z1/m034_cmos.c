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
	
	pstAeSnsDft->stIntTimeAccu.enAccuType = AE_ACCURACY_LINEAR;
    pstAeSnsDft->stIntTimeAccu.f32Accuracy = 1;
    pstAeSnsDft->stIntTimeAccu.f32Offset = 0;

    pstAeSnsDft->stAgainAccu.enAccuType = AE_ACCURACY_TABLE;
    pstAeSnsDft->stAgainAccu.f32Accuracy = 6;

    pstAeSnsDft->stDgainAccu.enAccuType = AE_ACCURACY_LINEAR;
    pstAeSnsDft->stDgainAccu.f32Accuracy = 0.03125;
    
    pstAeSnsDft->u32ISPDgainShift = 8;
    pstAeSnsDft->u32MinISPDgainTarget = 1 << pstAeSnsDft->u32ISPDgainShift;

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
                if ((f32Fps <= 30) && (f32Fps >= 0.5))
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
                if ((f32Fps <= 30) && (f32Fps >= 0.5))
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
    

    gu32LineLength = gu32LineLength > 0xFFFF ? 0xFFFF : gu32LineLength;

    //g_stSnsRegsInfo.astI2cData[3].u32Data = gu32FullLinesStd;
    //g_stSnsRegsInfo.astI2cData[4].u32Data = gu32LineLength;

    //sensor_write_register(FRAME_LINES, gu32FullLinesStd);
    sensor_write_register(LINE_LEN_PCK, gu32LineLength);

    pstAeSnsDft->f32Fps = f32Fps;
    pstAeSnsDft->u32LinesPer500ms = gu32FullLinesStd * f32Fps / 2;
    pstAeSnsDft->u32FullLinesStd = gu32FullLinesStd;

    return;
}


static HI_VOID cmos_slow_framerate_set(HI_U32 u32FullLines,
    AE_SENSOR_DEFAULT_S *pstAeSnsDft)
{
    u32FullLines = (u32FullLines > 0xFFFF) ? 0xFFFF : u32FullLines;
    gu32FullLines = u32FullLines;

    //g_stSnsRegsInfo.astI2cData[3].u32Data = gu32FullLines;

    sensor_write_register(FRAME_LINES, gu32FullLines);

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
    {0x80,0x80,0x7c,0x76,0x70,0x60,0x50,0x40,0x38,0x38,0x38,0x38,0x38,0x38,0x38,0x38}
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
	1,    /*bEnable*/			
	7,    /*u8Slope*/	
	7,    /*u8Sensitivity*/
	8192, /*u16Threshold*/
	8192, /*u16SensiThreshold*/	
	{1024,1024,1024,2048,2048,2048,2048,2048,2048,2048,2048,2048,2048,2048,2048,2048}    /*au16Strength[ISP_AUTO_ISO_STENGTH_NUM]*/	
};

static ISP_CMOS_GE_S g_stIspGeWDR =
{
	/*For GE*/
	1,    /*bEnable*/			
	7,    /*u8Slope*/	
	7,    /*u8Sensitivity*/
	8192, /*u16Threshold*/
	8192, /*u16SensiThreshold*/	
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
	{0,0,0,0,1,1,2,2,2,3,3,3,3,3,3,3},/*au16Strength[16]*/
	{0,0,0,0,0,0,0,0,0,0x23,0x80,0xD0,0xF0,0xF0,0xF0,0xF0},/*au16BlendRatio[16]*/
};

static ISP_CMOS_DPC_S g_stCmosDpcWDR = 
{
	//0,/*IR_channel*/
	//0,/*IR_position*/
	{0,0,0,0,1,1,2,2,2,3,3,3,3,3,3,3},/*au16Strength[16]*/
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


static ISP_LSC_CABLI_TABLE_S g_stCmosLscTable[HI_ISP_LSC_LIGHT_NUM] = 
{
	//--------------------------------------------
	// single light default
	//--------------------------------------------
	//3000K
	{   
        {
            12966290, 12850780, 12557130, 12237540, 11823770, 11369460, 10990480, 10685190, 10548450, 10835680, 11280550, 11860230, 12462200, 13024830, 13465580, 13903610, 14056030, 
            12937220, 12807990, 12516270, 12198730, 11787540, 11347100, 10959160, 10655590, 10529200, 10825520, 11247560, 11823770, 12408600, 12980880, 13434230, 13853540, 14038930, 
            12836480, 12723260, 12421960, 12109130, 11715740, 11291580, 10917690, 10596880, 10500450, 10754890, 11171350, 11715740, 12315900, 12879460, 13325640, 13754470, 13937200, 
            12695270, 12584510, 12315900, 11995840, 11609660, 11214770, 10845870, 10510020, 10434000, 10675300, 11074860, 11574720, 12185850, 12737310, 13203660, 13608490, 13770880, 
            12543480, 12408600, 12147370, 11823770, 11482590, 11117530, 10754890, 10424570, 10368370, 10567770, 10959160, 11437070, 12033370, 12543480, 13010150, 13387470, 13544600, 
            12355460, 12224580, 11946170, 11668350, 11358270, 11000950, 10626150, 10359070, 10303570, 10452900, 10866290, 11313730, 11860230, 12342250, 12765490, 13158500, 13325640, 
            12109130, 11995840, 11751530, 11459780, 11193020, 10856070, 10490910, 10285200, 10203360, 10387040, 10734880, 11160540, 11668350, 12211640, 12612020, 12951740, 13098750, 
            11872430, 11751530, 11540000, 11302640, 11053640, 10705010, 10359070, 10203360, 10105080, 10285200, 10596880, 11043060, 11540000, 11995840, 12395270, 12695270, 12836480, 
            11692000, 11597990, 11403160, 11193020, 10928030, 10626150, 10312780, 10122810, 10000000, 10221430, 10510020, 10980020, 11414440, 11835900, 12211640, 12489180, 12639650, 
            11739570, 11644800, 11448410, 11225680, 10969580, 10645760, 10331240, 10140600, 10043520, 10266900, 10500450, 10969580, 11414440, 11811670, 12173000, 12475680, 12639650, 
            11896910, 11799590, 11586350, 11335950, 11085490, 10774980, 10415160, 10212390, 10131690, 10285200, 10548450, 11117530, 11586350, 11995840, 12368700, 12681320, 12822220, 
            12083770, 11970950, 11715740, 11471170, 11193020, 10897070, 10510020, 10294380, 10194350, 10340500, 10795140, 11369460, 11751530, 12198730, 12543480, 12879460, 13010150, 
            12276600, 12134590, 11872430, 11586350, 11324830, 11000950, 10626150, 10349780, 10266900, 10415160, 11064240, 11516970, 11958550, 12421960, 12779620, 13083900, 13218790, 
            12408600, 12263550, 12020830, 11703850, 11403160, 11064240, 10695090, 10415160, 10312780, 10490910, 11182170, 11609660, 12109130, 12570810, 12951740, 13233950, 13371960, 
            12543480, 12395270, 12134590, 11811670, 11459780, 11139000, 10754890, 10471870, 10340500, 10596880, 11269530, 11692000, 12237540, 12695270, 13083900, 13371960, 13481310, 
            12612020, 12489180, 12185850, 11884650, 11494020, 11193020, 10805240, 10510020, 10349780, 10655590, 11324830, 11775510, 12315900, 12779620, 13158500, 13481310, 13576470, 
            12625820, 12502710, 12211640, 11909180, 11494020, 11203880, 10835680, 10510020, 10340500, 10675300, 11335950, 11811670, 12342250, 12836480, 13203660, 13528720, 13624560, 
        },  
        {   
            12937060, 12821780, 12560620, 12216980, 11804920, 11369620, 10974580, 10658440, 10502840, 10702480, 11087330, 11624780, 12228520, 12783810, 13254860, 13660340, 13820700, 
            12924150, 12771200, 12524180, 12182500, 11772730, 11339760, 10956010, 10632180, 10485830, 10693640, 11049490, 11572830, 12182500, 12758620, 13200820, 13602940, 13761960, 
            12834490, 12683640, 12416110, 12114130, 11698280, 11280490, 10909860, 10588720, 10435130, 10623460, 10993210, 11470330, 12080220, 12646480, 13080810, 13475550, 13645940, 
            12671230, 12560620, 12298200, 11979650, 11614350, 11202420, 10845900, 10519900, 10401610, 10562810, 10900670, 11349690, 11946490, 12475910, 12937060, 13323040, 13489580, 
            12512080, 12368670, 12125470, 11826480, 11470330, 11125430, 10737980, 10435130, 10351720, 10460420, 10782680, 11202420, 11783440, 12298200, 12733530, 13094030, 13241310, 
            12286530, 12159620, 11935480, 11635220, 11349690, 11002550, 10614750, 10360000, 10294120, 10368290, 10693640, 11087330, 11572830, 12080220, 12487950, 12847220, 13002010, 
            12046510, 11935480, 11698280, 11439930, 11192740, 10845900, 10485830, 10285940, 10212930, 10285940, 10554200, 10928270, 11399650, 11924490, 12309890, 12634150, 12771200, 
            11794170, 11687730, 11470330, 11270670, 11002550, 10676010, 10351720, 10196850, 10093530, 10204890, 10418340, 10818710, 11260870, 11730070, 12080220, 12380500, 12548450, 
            11624780, 11521350, 11339760, 11154180, 10891510, 10580070, 10285940, 10117190, 10000000, 10133020, 10343450, 10746890, 11154180, 11541890, 11924490, 12193970, 12309890, 
            11645680, 11562500, 11389620, 11173430, 10946750, 10606060, 10326950, 10133020, 10038760, 10164840, 10360000, 10746890, 11135000, 11531610, 11869840, 12182500, 12321600, 
            11826480, 11708860, 11500890, 11270670, 11021280, 10737980, 10384920, 10212930, 10117190, 10204890, 10401610, 10882350, 11300170, 11719460, 12080220, 12356870, 12463910, 
            12001850, 11880730, 11666670, 11429830, 11144580, 10845900, 10494330, 10277780, 10164840, 10253370, 10623460, 11068380, 11460180, 11880730, 12240080, 12548450, 12671230, 
            12182500, 12057730, 11826480, 11562500, 11270670, 10956010, 10597380, 10318730, 10229070, 10285940, 10845900, 11231570, 11645680, 12102800, 12463910, 12758620, 12872760, 
            12321600, 12205470, 11979650, 11645680, 11359650, 11021280, 10676010, 10393260, 10277780, 10384920, 10956010, 11319930, 11815690, 12274880, 12646480, 12924150, 13041290, 
            12475910, 12345090, 12080220, 11762030, 11429830, 11096830, 10737980, 10451980, 10302310, 10477350, 11030660, 11409690, 11913520, 12404210, 12796440, 13080810, 13200820, 
            12548450, 12416110, 12136830, 11837290, 11460180, 11154180, 10773710, 10485830, 10318730, 10537020, 11087330, 11470330, 12012990, 12487950, 12885570, 13187370, 13282050, 
            12572820, 12439960, 12159620, 11869840, 11460180, 11163790, 10791670, 10502840, 10318730, 10562810, 11106350, 11511110, 12057730, 12536300, 12924150, 13214290, 13295690, 
        },  
        {   
            12811880, 12673850, 12370940, 12048420, 11626240, 11174440, 10792330, 10460790, 10343720, 10632700, 11050380, 11615800, 12242190, 12824580, 13326470, 13707630, 13913980, 
            12773940, 12624390, 12347330, 12014860, 11584600, 11145560, 10765390, 10443910, 10327220, 10597870, 11022150, 11584600, 12196040, 12786560, 13271790, 13664200, 13869240, 
            12673850, 12538760, 12265400, 11937270, 11522710, 11097770, 10720800, 10410300, 10294350, 10537460, 10947550, 11502220, 12104770, 12698720, 13177190, 13549740, 13736730, 
            12538760, 12406520, 12150240, 11828150, 11451330, 11050380, 10667770, 10352000, 10261700, 10486220, 10883100, 11390840, 12003710, 12563110, 13044350, 13395450, 13535570, 
            12394640, 12277040, 12003710, 11699820, 11350880, 10975400, 10597870, 10286170, 10229250, 10410300, 10774350, 11271780, 11849820, 12394640, 12824580, 13204080, 13381590, 
            12219070, 12093460, 11849820, 11553570, 11242400, 10873950, 10511780, 10237340, 10205050, 10327220, 10711920, 11164800, 11689250, 12196040, 12612090, 12991970, 13150410, 
            12003710, 11893380, 11647160, 11380830, 11116840, 10765390, 10410300, 10188980, 10141070, 10278000, 10589200, 11040960, 11543260, 12082170, 12478300, 12824580, 12940000, 
            11774340, 11657660, 11471630, 11261970, 10984720, 10623970, 10302550, 10149020, 10070040, 10213100, 10503250, 10966100, 11441200, 11937270, 12277040, 12599810, 12761340, 
            11647160, 11553570, 11370830, 11174440, 10910620, 10589200, 10302550, 10117280, 10000000, 10188980, 10477730, 10938290, 11370830, 11774340, 12150240, 12454280, 12575320, 
            11742290, 11647160, 11451330, 11222900, 10994050, 10658980, 10360290, 10164960, 10085740, 10269840, 10503250, 10938290, 11380830, 11817350, 12173090, 12478300, 12624390, 
            11904320, 11806570, 11605380, 11370830, 11116840, 10792330, 10435480, 10261700, 10180960, 10318980, 10571900, 11107300, 11584600, 12014860, 12382780, 12673850, 12799210, 
            12082170, 11959330, 11752950, 11512460, 11242400, 10929050, 10554650, 10335460, 10253570, 10385230, 10819400, 11331000, 11785060, 12219070, 12587550, 12862820, 12965930, 
            12277040, 12173090, 11915290, 11657660, 11360840, 11031540, 10667770, 10401930, 10318980, 10435480, 11069290, 11502220, 11959330, 12430360, 12799210, 13097170, 13231080, 
            12430360, 12323810, 12070900, 11763640, 11471630, 11135970, 10747510, 10477730, 10376900, 10537460, 11193770, 11605380, 12116100, 12587550, 12991970, 13299080, 13437180, 
            12563110, 12466280, 12173090, 11882460, 11522710, 11193770, 10819400, 10537460, 10401930, 10650210, 11291450, 11721010, 12265400, 12748770, 13137060, 13423240, 13549740, 
            12673850, 12550920, 12242190, 11948290, 11574240, 11242400, 10864820, 10580540, 10435480, 10711920, 11350880, 11763640, 12347330, 12837300, 13217570, 13535570, 13635410, 
            12723700, 12575320, 12277040, 11970400, 11584600, 11252170, 10873950, 10606560, 10435480, 10738590, 11380830, 11795810, 12394640, 12888450, 13244630, 13606730, 13707630, 
        }, 
        {  
            11625240, 11558940, 11407130, 11155960, 10915620, 10647990, 10428820, 10270270, 10133330, 10252950, 10393160, 10742050, 11094890, 11450090, 11737450, 11944990, 12015810, 
            11603050, 11537000, 11364490, 11135530, 10896060, 10647990, 10428820, 10235690, 10133330, 10218490, 10375430, 10704230, 11054550, 11428570, 11692310, 11921570, 11992110, 
            11558940, 11471700, 11280150, 11074680, 10837790, 10610820, 10410960, 10218490, 10099670, 10184250, 10340140, 10610820, 11014490, 11343280, 11625240, 11875000, 11944990, 
            11450090, 11385770, 11217710, 11014490, 10761060, 10555560, 10357750, 10184250, 10082920, 10133330, 10305090, 10555560, 10935250, 11238450, 11515150, 11782950, 11851850, 
            11385770, 11301120, 11135530, 10915620, 10723100, 10537260, 10287650, 10150250, 10082920, 10099670, 10235690, 10500860, 10818510, 11115170, 11407130, 11669870, 11760150, 
            11259260, 11176470, 11034480, 10837790, 10685410, 10482760, 10270270, 10150250, 10066230, 10082920, 10235690, 10428820, 10723100, 11014490, 11322160, 11558940, 11625240, 
            11155960, 11074680, 10915620, 10742050, 10610820, 10410960, 10218490, 10116470, 10066230, 10049590, 10167220, 10357750, 10629370, 10954950, 11259260, 11450090, 11515150, 
            11014490, 10935250, 10799290, 10704230, 10519030, 10340140, 10184250, 10116470, 10033000, 10033000, 10116470, 10305090, 10537260, 10857140, 11115170, 11322160, 11364490, 
            10935250, 10876560, 10742050, 10647990, 10500860, 10357750, 10218490, 10099670, 10000000, 10049590, 10099670, 10287650, 10519030, 10761060, 10974730, 11155960, 11217710, 
            10994580, 10915620, 10799290, 10685410, 10573910, 10410960, 10235690, 10116470, 10066230, 10082920, 10116470, 10322580, 10537260, 10761060, 10954950, 11176470, 11238450, 
            11115170, 11034480, 10896060, 10799290, 10666670, 10500860, 10287650, 10235690, 10133330, 10116470, 10133330, 10393160, 10592330, 10857140, 11115170, 11301120, 11385770, 
            11238450, 11155960, 11014490, 10876560, 10742050, 10592330, 10357750, 10252950, 10167220, 10150250, 10270270, 10482760, 10666670, 10974730, 11197050, 11407130, 11471700, 
            11343280, 11280150, 11115170, 10994580, 10818510, 10629370, 10446740, 10270270, 10184250, 10167220, 10393160, 10555560, 10780140, 11115170, 11343280, 11537000, 11603050, 
            11471700, 11364490, 11217710, 11034480, 10876560, 10685410, 10482760, 10322580, 10218490, 10218490, 10464720, 10610820, 10915620, 11176470, 11471700, 11647510, 11692310, 
            11580950, 11471700, 11280150, 11115170, 10896060, 10723100, 10519030, 10340140, 10235690, 10270270, 10500860, 10647990, 10974730, 11259260, 11558940, 11760150, 11805830, 
            11625240, 11515150, 11343280, 11155960, 10915620, 10742050, 10555560, 10375430, 10252950, 10305090, 10537260, 10666670, 11034480, 11343280, 11603050, 11828790, 11875000, 
            11625240, 11537000, 11364490, 11176470, 10954950, 10761060, 10555560, 10393160, 10270270, 10305090, 10555560, 10666670, 11054550, 11364490, 11647510, 11851850, 11921570, 
        },     
    },         
               
	//D50  
    {          
        {      
            13333330, 13201320, 12847970, 12435230, 11940300, 11472280, 11070110, 10801080, 10666670, 10899180, 11396010, 11952190, 12526100, 13071900, 13528750, 13904980, 14117650, 
            13274340, 13129100, 12806830, 12396690, 11916580, 11450380, 11049720, 10762330, 10638300, 10879420, 11363640, 11904760, 12486990, 13015180, 13468010, 13856810, 14051520, 
            13157890, 13015180, 12711860, 12307690, 11846000, 11385200, 10999080, 10714290, 10582010, 10830320, 11299430, 11811020, 12383900, 12889370, 13348160, 13745700, 13921110, 
            13015180, 12861740, 12565440, 12170390, 11741680, 11299430, 10899180, 10628880, 10535560, 10743060, 11204480, 11673150, 12244900, 12752390, 13186810, 13605440, 13777270, 
            12793180, 12644890, 12332990, 11988010, 11583010, 11204480, 10781670, 10517090, 10443860, 10619470, 11059910, 11516310, 12060300, 12539190, 12972970, 13348160, 13528750, 
            12565440, 12422360, 12108980, 11753180, 11406840, 11029410, 10628880, 10407630, 10344830, 10517090, 10919020, 11374410, 11834320, 12295080, 12711860, 13071900, 13230430, 
            12269940, 12121210, 11846000, 11538460, 11214950, 10840110, 10462080, 10309280, 10221460, 10380620, 10752690, 11194030, 11639180, 12121210, 12513030, 12847970, 12987010, 
            11964110, 11822660, 11560690, 11320750, 10999080, 10628880, 10309280, 10178120, 10075570, 10256410, 10582010, 11029410, 11461320, 11916580, 12269940, 12565440, 12738850, 
            11764710, 11650490, 11439470, 11194030, 10869570, 10535560, 10247650, 10084030, 10000000, 10143700, 10434780, 10948900, 11352890, 11730200, 12096770, 12371130, 12500000, 
            11834320, 11730200, 11483250, 11214950, 10938920, 10591350, 10282780, 10109520, 10000000, 10135140, 10425720, 10899180, 11352890, 11741680, 12096770, 12409510, 12578620, 
            11988010, 11869440, 11627910, 11342150, 11049720, 10704730, 10353750, 10195410, 10058680, 10178120, 10471200, 11070110, 11527380, 11940300, 12332990, 12618300, 12765960, 
            12170390, 12048190, 11753180, 11483250, 11173180, 10830320, 10452960, 10282780, 10092510, 10195410, 10704730, 11288810, 11695910, 12145750, 12500000, 12820510, 12958960, 
            12332990, 12207530, 11928430, 11594200, 11278200, 10919020, 10526320, 10291600, 10126580, 10230180, 10948900, 11428570, 11869440, 12320330, 12711860, 13015180, 13143480, 
            12474010, 12345680, 12060300, 11695910, 11363640, 10989010, 10619470, 10371650, 10195410, 10318140, 11059910, 11516310, 12024050, 12500000, 12875540, 13186810, 13333330, 
            12644890, 12486990, 12170390, 11822660, 11428570, 11049720, 10685660, 10407630, 10221460, 10425720, 11142060, 11594200, 12133470, 12631580, 13015180, 13333330, 13468010, 
            12738850, 12578620, 12244900, 11881190, 11483250, 11090570, 10723860, 10452960, 10230180, 10498690, 11183600, 11627910, 12207530, 12711860, 13100440, 13422820, 13544020, 
            12765960, 12618300, 12282500, 11892960, 11505270, 11100830, 10743060, 10480350, 10221460, 10507880, 11204480, 11650490, 12244900, 12738850, 13143480, 13468010, 13590030, 
        },  
        {   
            13258660, 13111780, 12827590, 12411820, 11912170, 11441120, 11052630, 10831950, 10628570, 10796020, 11175970, 11677130, 12236840, 12752200, 13204870, 13619250, 13821660, 
            13218270, 13059180, 12777230, 12376430, 11890410, 11421050, 11024550, 10787080, 10602610, 10778150, 11147260, 11635390, 12191010, 12714840, 13151520, 13548390, 13763210, 
            13125000, 12968130, 12665370, 12259890, 11814880, 11351350, 10978080, 10716050, 10559610, 10724880, 11099740, 11532330, 12100370, 12604070, 13046090, 13422680, 13633510, 
            12955220, 12814960, 12507210, 12111630, 11698110, 11272730, 10895400, 10628570, 10516960, 10654660, 11015230, 11421050, 11977920, 12447420, 12891090, 13245170, 13422680, 
            12727270, 12591880, 12306240, 11955920, 11552800, 11166380, 10787080, 10516960, 10432690, 10533980, 10895400, 11272730, 11772150, 12248350, 12640780, 13020000, 13164810, 
            12483220, 12341230, 12055560, 11740310, 11381120, 11005920, 10619900, 10432690, 10341540, 10424340, 10760330, 11128210, 11552800, 12000000, 12376430, 12727270, 12878340, 
            12179610, 12044400, 11782810, 11491620, 11185570, 10804980, 10457830, 10333330, 10219780, 10308790, 10593980, 10950380, 11341460, 11804170, 12179610, 12507210, 12616280, 
            11836360, 11729730, 11491620, 11262980, 10950380, 10602610, 10300630, 10203760, 10077400, 10179830, 10432690, 10796020, 11175970, 11604280, 11966910, 12225350, 12364670, 
            11656220, 11552800, 11341460, 11128210, 10840970, 10508470, 10251970, 10108700, 10000000, 10061820, 10284360, 10716050, 11080850, 11441120, 11782810, 12044400, 12168220, 
            11729730, 11625000, 11421050, 11156810, 10877190, 10568180, 10276240, 10124420, 10000000, 10077400, 10292490, 10707240, 11080850, 11471370, 11782810, 12077920, 12202440, 
            11890410, 11782810, 11542550, 11282500, 11005920, 10680890, 10349760, 10219780, 10069610, 10100850, 10341540, 10831950, 11253240, 11645800, 12000000, 12283020, 12423660, 
            12089140, 11934010, 11698110, 11421050, 11118700, 10787080, 10432690, 10276240, 10108700, 10116550, 10533980, 11015230, 11391080, 11836360, 12168220, 12459330, 12579710, 
            12236840, 12122910, 11836360, 11522120, 11224140, 10868110, 10516960, 10284360, 10132300, 10163930, 10733720, 11137730, 11573330, 12000000, 12376430, 12677700, 12802360, 
            12411820, 12271440, 11977920, 11645800, 11311900, 10950380, 10585370, 10366240, 10171870, 10219780, 10840970, 11224140, 11719170, 12168220, 12555450, 12865610, 13020000, 
            12519230, 12388200, 12089140, 11761520, 11371180, 11015230, 10663390, 10432690, 10211760, 10325140, 10895400, 11302080, 11825610, 12306240, 12690060, 13006990, 13151520, 
            12628520, 12471260, 12156860, 11825610, 11411040, 11062020, 10698440, 10466240, 10219780, 10366240, 10950380, 11361260, 11901280, 12388200, 12777230, 13098590, 13245170, 
            12653060, 12507210, 12168220, 11857920, 11421050, 11080850, 10724880, 10491540, 10219780, 10391060, 10968830, 11381120, 11934010, 12435530, 12814960, 13138240, 13272170, 
        },  
        {   
            13091640, 12961120, 12633630, 12218040, 11732850, 11235950, 10851420, 10577700, 10458570, 10690790, 11120620, 11690650, 12298960, 12833170, 13306040, 13669820, 13874070, 
            13039120, 12896830, 12584700, 12172280, 11701170, 11216570, 10833330, 10560520, 10441770, 10673230, 11101620, 11648750, 12252590, 12782690, 13238290, 13598330, 13785790, 
            12922470, 12782690, 12487990, 12093020, 11627910, 11158800, 10788380, 10509300, 10400000, 10612240, 11054420, 11576140, 12149530, 12670570, 13118060, 13499480, 13684210, 
            12770140, 12645910, 12345680, 11970530, 11535050, 11111110, 10734930, 10466990, 10366830, 10560520, 10989010, 11453740, 12025900, 12536160, 12961120, 13347020, 13513510, 
            12621360, 12464050, 12160900, 11828940, 11403510, 11026290, 10620910, 10375100, 10292950, 10458570, 10878660, 11333910, 11861310, 12333970, 12757610, 13118060, 13292430, 
            12392750, 12241050, 11959520, 11607140, 11265160, 10906040, 10500810, 10309280, 10244290, 10375100, 10797340, 11216570, 11669660, 12126870, 12524090, 12858560, 13013010, 
            12104280, 11981570, 11711710, 11433600, 11101620, 10717230, 10375100, 10236220, 10156250, 10292950, 10638300, 11073250, 11494250, 11981570, 12357410, 12695310, 12820510, 
            11818180, 11722270, 11473960, 11245680, 10933560, 10569110, 10244290, 10148320, 10054140, 10188090, 10500810, 10933560, 11363640, 11807450, 12160900, 12464050, 12621360, 
            11669660, 11576140, 11393510, 11149230, 10851420, 10526320, 10236220, 10093170, 10000000, 10116730, 10408330, 10887770, 11294530, 11669660, 12025900, 12310610, 12428300, 
            11807450, 11690650, 11463840, 11216570, 10942760, 10594950, 10301110, 10140410, 10054140, 10156250, 10450160, 10906040, 11333910, 11743450, 12081780, 12380950, 12500000, 
            11959520, 11872150, 11638320, 11343800, 11063830, 10743800, 10391690, 10260460, 10116730, 10212100, 10509300, 11082690, 11524820, 11926610, 12310610, 12609120, 12720160, 
            12172280, 12025900, 11775360, 11494250, 11187610, 10842370, 10492330, 10342080, 10164190, 10252370, 10726070, 11274940, 11680140, 12126870, 12487990, 12795280, 12909630, 
            12369170, 12229540, 11937560, 11638320, 11304350, 10933560, 10594950, 10358570, 10228170, 10276680, 10961210, 11413520, 11883000, 12333970, 12720160, 13000000, 13118060, 
            12500000, 12357410, 12081780, 11754070, 11393510, 11016950, 10673230, 10433390, 10276680, 10383390, 11082690, 11504420, 12037040, 12500000, 12896830, 13238290, 13360740, 
            12645910, 12500000, 12206570, 11872150, 11453740, 11082690, 10743800, 10492330, 10301110, 10483870, 11158800, 11576140, 12149530, 12658230, 13026050, 13360740, 13499480, 
            12757610, 12596900, 12275730, 11937560, 11494250, 11130140, 10779440, 10509300, 10309280, 10551950, 11187610, 11648750, 12218040, 12732620, 13091640, 13443640, 13598330, 
            12795280, 12633630, 12310610, 11959520, 11504420, 11158800, 10797340, 10526320, 10301110, 10586320, 11197240, 11690650, 12241050, 12770140, 13144590, 13485480, 13612570, 
        },  
        {   
            11769380, 11699600, 11517510, 11276190, 10983300, 10705240, 10496450, 10367780, 10224520, 10295650, 10459360, 10744100, 11106940, 11384620, 11630650, 11911470, 11935480, 
            11769380, 11676530, 11495150, 11233400, 10942700, 10685920, 10477880, 10349650, 10224520, 10277780, 10440920, 10705240, 11065420, 11341000, 11607840, 11863730, 11935480, 
            11699600, 11607840, 11406550, 11169810, 10902390, 10647480, 10440920, 10313590, 10171820, 10242210, 10385970, 10647480, 11003720, 11254750, 11539960, 11769380, 11863730, 
            11607840, 11495150, 11319310, 11065420, 10842490, 10609320, 10385970, 10277780, 10136990, 10206900, 10349650, 10590340, 10902390, 11169810, 11450680, 11653540, 11746030, 
            11495150, 11384620, 11212120, 10983300, 10763640, 10552590, 10349650, 10206900, 10154370, 10171820, 10313590, 10515100, 10783240, 11065420, 11341000, 11562500, 11630650, 
            11384620, 11276190, 11086140, 10862390, 10705240, 10477880, 10295650, 10206900, 10119660, 10136990, 10277780, 10440920, 10666670, 10942700, 11212120, 11428570, 11495150, 
            11169810, 11106940, 10942700, 10783240, 10609320, 10404220, 10224520, 10171820, 10119660, 10085180, 10206900, 10367780, 10571430, 10862390, 11127820, 11319310, 11384620, 
            11044780, 10962960, 10802920, 10705240, 10515100, 10313590, 10171820, 10136990, 10033900, 10050930, 10154370, 10295650, 10515100, 10763640, 10983300, 11148780, 11233400, 
            10962960, 10882350, 10763640, 10628370, 10477880, 10313590, 10189330, 10102390, 10033900, 10000000, 10068030, 10277780, 10459360, 10666670, 10882350, 11044780, 11086140, 
            11044780, 10942700, 10842490, 10685920, 10552590, 10385970, 10242210, 10136990, 10068030, 10050930, 10085180, 10295650, 10496450, 10666670, 10882350, 11065420, 11127820, 
            11127820, 11065420, 10922510, 10783240, 10628370, 10459360, 10259970, 10224520, 10102390, 10068030, 10119660, 10367780, 10533810, 10783240, 11044780, 11233400, 11297710, 
            11276190, 11190930, 11003720, 10862390, 10724640, 10515100, 10331590, 10259970, 10119660, 10068030, 10224520, 10440920, 10590340, 10862390, 11127820, 11319310, 11362760, 
            11406550, 11319310, 11106940, 10962960, 10783240, 10571430, 10385970, 10277780, 10154370, 10085180, 10331590, 10477880, 10724640, 11003720, 11276190, 11450680, 11495150, 
            11539960, 11428570, 11212120, 11024210, 10822670, 10609320, 10440920, 10313590, 10171820, 10119660, 10367780, 10496450, 10802920, 11127820, 11362760, 11562500, 11630650, 
            11607840, 11517510, 11297710, 11106940, 10862390, 10647480, 10459360, 10349650, 10189330, 10154370, 10404220, 10552590, 10882350, 11190930, 11428570, 11653540, 11722770, 
            11653540, 11539960, 11362760, 11148780, 10902390, 10666670, 10515100, 10385970, 10206900, 10189330, 10422530, 10571430, 10942700, 11233400, 11517510, 11699600, 11769380, 
            11676530, 11562500, 11384620, 11169810, 10922510, 10685920, 10515100, 10385970, 10206900, 10206900, 10422530, 10571430, 10962960, 11254750, 11562500, 11722770, 11792830, 
        },   
    },       
             
	//D65
    {        
        {    
            12674420, 12546760, 12299010, 11961590, 11564990, 11165170, 10859280, 10595380, 10493380, 10647130, 10900000, 11324680, 11752020, 12230010, 12619390, 12937690, 13112780, 
            12619390, 12528740, 12264420, 11928860, 11534390, 11165170, 10832300, 10582520, 10468190, 10608270, 10872820, 11266150, 11720430, 12178770, 12582970, 12880350, 13034380, 
            12546760, 12421650, 12161790, 11847830, 11473680, 11122450, 10805450, 10531400, 10443110, 10569700, 10818860, 11179490, 11642190, 12060860, 12474960, 12785920, 12937690, 
            12439370, 12281690, 12044200, 11736200, 11383810, 11051960, 10765430, 10480770, 10380950, 10493380, 10752160, 11065990, 11503960, 11928860, 12316380, 12674420, 12804700, 
            12264420, 12144850, 11896320, 11595750, 11309990, 10996220, 10660150, 10393330, 10344010, 10418160, 10634150, 10954770, 11368970, 11752020, 12161790, 12474960, 12601160, 
            12094310, 11961590, 11720430, 11443570, 11179490, 10872820, 10544140, 10319530, 10246770, 10331750, 10582520, 10845770, 11193840, 11580350, 11928860, 12264420, 12403980, 
            11880110, 11752020, 11519150, 11280730, 11051960, 10752160, 10418160, 10258820, 10175030, 10258820, 10443110, 10712530, 11037970, 11473680, 11799730, 12094310, 12230010, 
            11626670, 11534390, 11324680, 11122450, 10900000, 10569700, 10319530, 10186920, 10080930, 10175030, 10344010, 10621190, 10954770, 11324680, 11611190, 11880110, 12011020, 
            11488800, 11383810, 11208230, 11037970, 10765430, 10493380, 10246770, 10092590, 10022990, 10069280, 10246770, 10544140, 10845770, 11150900, 11443570, 11704700, 11799730, 
            11564990, 11428570, 11251610, 11024020, 10792080, 10518700, 10270910, 10080930, 10000000, 10069280, 10222740, 10518700, 10832300, 11122450, 11413610, 11689010, 11815720, 
            11626670, 11534390, 11324680, 11108280, 10859280, 10595380, 10307330, 10127760, 10022990, 10092590, 10222740, 10634150, 10941030, 11280730, 11595750, 11863950, 11961590, 
            11783780, 11673360, 11443570, 11208230, 10968550, 10686270, 10356300, 10175030, 10057670, 10092590, 10405730, 10792080, 11094150, 11428570, 11720430, 11994500, 12111110, 
            11928860, 11799730, 11534390, 11295340, 11051960, 10738920, 10418160, 10210770, 10080930, 10139530, 10621190, 10913640, 11222650, 11595750, 11896320, 12161790, 12299010, 
            12060860, 11928860, 11673360, 11383810, 11094150, 10792080, 10493380, 10246770, 10104290, 10186920, 10712530, 10968550, 11354170, 11720430, 12060860, 12316380, 12457140, 
            12161790, 12044200, 11783780, 11473680, 11150900, 10832300, 10531400, 10283020, 10127760, 10283020, 10778740, 11051960, 11458610, 11831750, 12178770, 12457140, 12546760, 
            12247190, 12111110, 11831750, 11549670, 11193840, 10872820, 10569700, 10319530, 10139530, 10331750, 10818860, 11108280, 11503960, 11912570, 12247190, 12528740, 12656020, 
            12299010, 12161790, 11847830, 11549670, 11222650, 10900000, 10569700, 10319530, 10139530, 10331750, 10845770, 11122450, 11534390, 11961590, 12281690, 12564840, 12692870, 
        }, 
        {  
            13184580, 13013010, 12695310, 12322270, 11839710, 11383540, 11045030, 10699590, 10534850, 10638300, 10887770, 11324040, 11839710, 12380950, 12845850, 13251780, 13457560, 
            13144590, 12974050, 12658230, 12287330, 11828940, 11383540, 11016950, 10682000, 10509300, 10612240, 10860480, 11294530, 11818180, 12322270, 12782690, 13184580, 13388260, 
            13013010, 12858560, 12572530, 12183690, 11775360, 11333910, 10951980, 10629600, 10450160, 10551950, 10806320, 11197240, 11722270, 12206570, 12645910, 13078470, 13265310, 
            12871290, 12720160, 12416430, 12070570, 11648750, 11255410, 10887770, 10543390, 10416670, 10492330, 10743800, 11082690, 11576140, 12081780, 12500000, 12909630, 13065330, 
            12658230, 12512030, 12252590, 11926610, 11535050, 11177990, 10788380, 10466990, 10366830, 10400000, 10620910, 10933560, 11433600, 11883000, 12287330, 12658230, 12820510, 
            12428300, 12310610, 12025900, 11722270, 11403510, 11045030, 10647010, 10375100, 10301110, 10309280, 10517800, 10824310, 11226250, 11638320, 12037040, 12404580, 12560390, 
            12172280, 12048190, 11796730, 11524820, 11226250, 10887770, 10517800, 10309280, 10228170, 10220130, 10400000, 10690790, 11054420, 11504420, 11850500, 12195120, 12333970, 
            11883000, 11764710, 11555560, 11324040, 11035650, 10690790, 10366830, 10228170, 10077520, 10124610, 10268560, 10577700, 10906040, 11314190, 11648750, 11937560, 12081780, 
            11701170, 11617520, 11423550, 11206900, 10924370, 10603590, 10317460, 10116730, 10007700, 10038610, 10180110, 10492330, 10806320, 11149230, 11473960, 11754070, 11883000, 
            11754070, 11638320, 11453740, 11216570, 10942760, 10612240, 10317460, 10093170, 10000000, 10046370, 10156250, 10458570, 10779440, 11139670, 11463840, 11743450, 11893870, 
            11893870, 11786040, 11555560, 11304350, 11007620, 10699590, 10350320, 10172140, 10046370, 10046370, 10156250, 10560520, 10915200, 11304350, 11648750, 11948530, 12081780, 
            12048190, 11937560, 11701170, 11433600, 11120620, 10788380, 10433390, 10228170, 10054140, 10046370, 10350320, 10717230, 11045030, 11453740, 11786040, 12081780, 12206570, 
            12218040, 12093020, 11807450, 11524820, 11226250, 10869570, 10509300, 10252370, 10101010, 10093170, 10543390, 10851420, 11216570, 11627910, 12003690, 12287330, 12440190, 
            12392750, 12229540, 11970530, 11627910, 11304350, 10933560, 10577700, 10284810, 10132500, 10156250, 10647010, 10924370, 11353710, 11807450, 12160900, 12476010, 12609120, 
            12512030, 12369170, 12070570, 11743450, 11373580, 10989010, 10647010, 10358570, 10156250, 10252370, 10734930, 11007620, 11484100, 11926610, 12310610, 12621360, 12757610, 
            12596900, 12452110, 12160900, 11818180, 11403510, 11026290, 10673230, 10400000, 10172140, 10292950, 10761590, 11045030, 11555560, 12014790, 12392750, 12695310, 12820510, 
            12633630, 12487990, 12206570, 11839710, 11423550, 11045030, 10690790, 10425020, 10164190, 10309280, 10779440, 11054420, 11576140, 12059370, 12440190, 12732620, 12845850, 
        },  
        {   
            12908370, 12755910, 12449570, 12078290, 11623320, 11153180, 10773070, 10485440, 10326690, 10493930, 10791010, 11230500, 11792540, 12331110, 12793680, 13184130, 13388430, 
            12857140, 12718350, 12401910, 12033430, 11581770, 11143590, 10755190, 10468500, 10310260, 10476960, 10773070, 11201380, 11749770, 12284360, 12743360, 13144020, 13347070, 
            12743360, 12619280, 12307690, 11944700, 11520000, 11095890, 10719600, 10418010, 10277560, 10426390, 10719600, 11134020, 11675680, 12180450, 12619280, 13051360, 13238000, 
            12643900, 12497590, 12191910, 11835620, 11418500, 11048590, 10675450, 10384620, 10253160, 10368000, 10657890, 11039180, 11540520, 12044610, 12485550, 12869910, 13051360, 
            12461540, 12319390, 12033430, 11696750, 11338580, 10973750, 10596890, 10293880, 10228890, 10302070, 10570960, 10927490, 11408450, 11879010, 12272730, 12656250, 12844400, 
            12261120, 12123480, 11824820, 11540520, 11230500, 10872480, 10476960, 10253160, 10164710, 10212770, 10485440, 10809010, 11240240, 11675680, 12078290, 12413790, 12582520, 
            12022260, 11900830, 11633750, 11368420, 11076920, 10746270, 10384620, 10204720, 10101330, 10156740, 10384620, 10701900, 11086400, 11530250, 11922720, 12249530, 12378220, 
            11781820, 11665170, 11448760, 11220780, 10936710, 10596890, 10285710, 10132920, 10000000, 10077760, 10269410, 10614250, 10973750, 11388400, 11728510, 12033430, 12180450, 
            11675680, 11571430, 11358460, 11134020, 10845190, 10545160, 10253160, 10077760, 10000000, 10054310, 10228890, 10562350, 10890760, 11259770, 11602510, 11900830, 12033430, 
            11739130, 11633750, 11418500, 11191710, 10918280, 10588240, 10293880, 10101330, 10007720, 10093460, 10253160, 10579590, 10927490, 11289200, 11633750, 11933700, 12044610, 
            11900830, 11781820, 11540520, 11299040, 11011040, 10693070, 10376300, 10180680, 10077760, 10125000, 10285710, 10701900, 11095890, 11479180, 11824820, 12123480, 12261120, 
            12078290, 11944700, 11675680, 11428570, 11143590, 10818030, 10451610, 10261280, 10117100, 10172680, 10485440, 10899920, 11240240, 11654680, 12000000, 12296020, 12425700, 
            12249530, 12112150, 11857270, 11540520, 11259770, 10899920, 10536580, 10293880, 10172680, 10204720, 10728480, 11039180, 11448760, 11857270, 12237960, 12545980, 12681020, 
            12413790, 12272730, 12000000, 11665170, 11348510, 10973750, 10640390, 10351440, 10220820, 10293880, 10818030, 11143590, 11592130, 12033430, 12413790, 12718350, 12844400, 
            12545980, 12413790, 12112150, 11771120, 11398420, 11029790, 10710740, 10418010, 10245060, 10409640, 10899920, 11220780, 11707320, 12169010, 12570320, 12895520, 13012050, 
            12668620, 12509650, 12180450, 11857270, 11448760, 11086400, 10737370, 10451610, 10285710, 10460050, 10945950, 11289200, 11792540, 12249530, 12643900, 12972970, 13104150, 
            12730850, 12545980, 12214890, 11868130, 11469030, 11114920, 10746270, 10476960, 10302070, 10468500, 10992370, 11308900, 11824820, 12296020, 12668620, 12999000, 13144020, 
        }, 
        {  
            12126980, 11993720, 11753850, 11471470, 11137030, 10852270, 10581720, 10352300, 10200270, 10241290, 10380430, 10700280, 11104650, 11488720, 11826620, 12126980, 12243590, 
            12069510, 11974920, 11735790, 11437130, 11120820, 10821530, 10567080, 10338290, 10186670, 10227580, 10366350, 10655510, 11056440, 11454270, 11790120, 12088610, 12204470, 
            12012580, 11900310, 11664120, 11385990, 11072460, 10790960, 10523420, 10310390, 10159570, 10200270, 10324320, 10581720, 10992810, 11352150, 11699850, 11993720, 12107770, 
            11900310, 11771960, 11558250, 11301770, 11008650, 10745430, 10480110, 10268820, 10159570, 10173100, 10282640, 10508940, 10898720, 11218800, 11575760, 11863350, 11974920, 
            11771960, 11646340, 11437130, 11218800, 10929900, 10700280, 10422920, 10227580, 10146080, 10105820, 10213900, 10437160, 10760560, 11088530, 11437130, 11717790, 11826620, 
            11628610, 11523380, 11318520, 11088530, 10883190, 10640670, 10380430, 10200270, 10132630, 10105820, 10213900, 10380430, 10640670, 10945560, 11285080, 11540790, 11664120, 
            11454270, 11369050, 11169590, 10961260, 10790960, 10537930, 10310390, 10173100, 10105820, 10052630, 10119210, 10296500, 10537930, 10883190, 11202350, 11454270, 11540790, 
            11285080, 11202350, 11024530, 10883190, 10670390, 10451440, 10255030, 10159570, 10039420, 10026250, 10079160, 10241290, 10480110, 10775740, 11040460, 11268440, 11352150, 
            11202350, 11120820, 10977010, 10821530, 10625870, 10422920, 10255030, 10119210, 10039420, 10000000, 10039420, 10227580, 10451440, 10685320, 10945560, 11137030, 11235290, 
            11301770, 11202350, 11040460, 10867710, 10700280, 10494510, 10310390, 10146080, 10065880, 10039420, 10065880, 10227580, 10422920, 10670390, 10945560, 11185940, 11251840, 
            11402980, 11318520, 11153280, 10961260, 10790960, 10581720, 10324320, 10241290, 10119210, 10065880, 10065880, 10310390, 10508940, 10790960, 11088530, 11335310, 11402980, 
            11575760, 11454270, 11235290, 11088530, 10883190, 10655510, 10408720, 10282640, 10132630, 10079160, 10200270, 10394560, 10581720, 10929900, 11218800, 11437130, 11558250, 
            11699850, 11593320, 11385990, 11169590, 10977010, 10700280, 10480110, 10296500, 10173100, 10092470, 10324320, 10465750, 10715290, 11056440, 11369050, 11593320, 11699850, 
            11808350, 11699850, 11506020, 11268440, 11024530, 10760560, 10523420, 10338290, 10213900, 10146080, 10394560, 10508940, 10836880, 11185940, 11488720, 11735790, 11844960, 
            11937500, 11808350, 11575760, 11335310, 11072460, 10806220, 10581720, 10394560, 10213900, 10213900, 10451440, 10581720, 10929900, 11285080, 11610940, 11863350, 11974920, 
            12012580, 11881800, 11646340, 11402980, 11088530, 10836880, 10611110, 10422920, 10227580, 10241290, 10465750, 10625870, 10992810, 11352150, 11681960, 11937500, 12050470, 
            12031500, 11900310, 11664120, 11420030, 11104650, 10852270, 10640670, 10437160, 10227580, 10255030, 10465750, 10640670, 11024530, 11369050, 11699850, 11974920, 12088610, 
        },
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
	memcpy(&pstDef->stLsc.stLscParaTable[0], &g_stCmosLscTable[0], sizeof(ISP_LSC_CABLI_TABLE_S)*HI_ISP_LSC_LIGHT_NUM);
	
    switch (genSensorMode)
    {
        default:
        case WDR_MODE_NONE:

            pstDef->stDrc.bEnable               = HI_FALSE;
            pstDef->stDrc.u8Asymmetry           = 0x02;
            pstDef->stDrc.u8SecondPole          = 0xC0;
            pstDef->stDrc.u8Stretch             = 0x3C;
            pstDef->stDrc.u8LocalMixingBrigtht  = 0x2D;
            pstDef->stDrc.u8LocalMixingDark     = 0x2D;
            pstDef->stDrc.u8LocalMixingThres    = 0x02;
            pstDef->stDrc.u16BrightGainLmt      = 0x7F;
            pstDef->stDrc.u16DarkGainLmtC       = 0x7F;
            pstDef->stDrc.u16DarkGainLmtY       = 0x7F;
            pstDef->stDrc.u8RangeVar            = 0x00;
            pstDef->stDrc.u8SpatialVar          = 0x0A;

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

            pstDef->stDrc.bEnable               = HI_TRUE;
            pstDef->stDrc.u8Asymmetry           = 0x01;
            pstDef->stDrc.u8SecondPole          = 0xC0;
            pstDef->stDrc.u8Stretch             = 0x3C;
            pstDef->stDrc.u8LocalMixingBrigtht  = 0x2D;
            pstDef->stDrc.u8LocalMixingDark     = 0x2D;
            pstDef->stDrc.u8LocalMixingThres    = 0x02;
            pstDef->stDrc.u16BrightGainLmt      = 0x7F;
            pstDef->stDrc.u16DarkGainLmtC       = 0x7F;
            pstDef->stDrc.u16DarkGainLmtY       = 0x7F;
            pstDef->stDrc.u8RangeVar            = 0x00;
            pstDef->stDrc.u8SpatialVar          = 0x0A;

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

    u32FullLines_5Fps = (u32FullLines_5Fps > 0xFFFF) ? 0xFFFF : u32FullLines_5Fps;
    
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
        g_stSnsRegsInfo.u32RegNum = 3;
             
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
        //g_stSnsRegsInfo.astI2cData[3].u8DelayFrmNum = 1;
        //g_stSnsRegsInfo.astI2cData[3].u32RegAddr = FRAME_LINES;
        //g_stSnsRegsInfo.astI2cData[4].u8DelayFrmNum = 1;
        //g_stSnsRegsInfo.astI2cData[4].u32RegAddr = LINE_LEN_PCK;
        
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
