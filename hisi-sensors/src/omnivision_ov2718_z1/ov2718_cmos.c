#if !defined(__OV2718_CMOS_H_)
#define __OV2718_CMOS_H_

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


#define OV2718_ID 2718


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

#define EXPOSURE_ADDR_H       (0x30B6)
#define EXPOSURE_ADDR_L       (0x30B7)
#define AGC_ADDR              (0x30BB)
#define DGC_HCG_ADDR_H        (0x315A)
#define DGC_HCG_ADDR_L        (0x315B)
#define DGC_LCG_ADDR_H        (0x315C)
#define DGC_LCG_ADDR_L        (0x315D)
#define VMAX_ADDR_H           (0x30B2)
#define VMAX_ADDR_L           (0x30B3)

#define INCREASE_LINES (1) /* make real fps less than stand fps because NVR require*/
#define VMAX_1080P30_LINEAR  (1136+INCREASE_LINES)
#define SENSOR_1080P_30FPS_MODE (1)

HI_U8 gu8SensorImageMode = SENSOR_1080P_30FPS_MODE;
WDR_MODE_E genSensorMode = WDR_MODE_NONE;

static HI_U32 gu32FullLinesStd = VMAX_1080P30_LINEAR;
static HI_U32 gu32FullLines = VMAX_1080P30_LINEAR;
static HI_U32 gu32PreFullLines = VMAX_1080P30_LINEAR;

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
    30                 // u32MinIrisFNOTarget
};
#endif

#define PATHLEN_MAX 256
#define CMOS_CFG_INI "ov2718_cfg.ini"
static char pcName[PATHLEN_MAX] = "configs/ov2718_cfg.ini";

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
    pstAeSnsDft->stDgainAccu.f32Accuracy = 0.00390625;

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
            
            pstAeSnsDft->u32MaxAgain = 32382; 
            pstAeSnsDft->u32MinAgain = 1024;
            pstAeSnsDft->u32MaxAgainTarget = g_AeDft[0].u32MaxAgainTarget;
            pstAeSnsDft->u32MinAgainTarget = g_AeDft[0].u32MinAgainTarget;
            
            pstAeSnsDft->u32MaxDgain = 32382;  
            pstAeSnsDft->u32MinDgain = 1024;
            pstAeSnsDft->u32MaxDgainTarget = g_AeDft[0].u32MaxDgainTarget;
            pstAeSnsDft->u32MinDgainTarget = g_AeDft[0].u32MinDgainTarget;
          
            pstAeSnsDft->u32ISPDgainShift = g_AeDft[0].u32ISPDgainShift;
            pstAeSnsDft->u32MinISPDgainTarget = g_AeDft[0].u32MinISPDgainTarget;
            pstAeSnsDft->u32MaxISPDgainTarget = g_AeDft[0].u32MaxISPDgainTarget;    
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
    pstAeSnsDft->stDgainAccu.f32Accuracy = 0.00390625;
    
    pstAeSnsDft->u32ISPDgainShift = 8;
    pstAeSnsDft->u32MinISPDgainTarget = 1 << pstAeSnsDft->u32ISPDgainShift;
    pstAeSnsDft->u32MaxISPDgainTarget = 16 << pstAeSnsDft->u32ISPDgainShift;      
    
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

            
            pstAeSnsDft->u8AeCompensation = 0x40;
            pstAeSnsDft->u32MaxIntTime = gu32FullLinesStd - 4;
            pstAeSnsDft->u32MinIntTime = 2;
            pstAeSnsDft->u32MaxIntTimeTarget = 65535;
            pstAeSnsDft->u32MinIntTimeTarget = 2;
            
            pstAeSnsDft->u32MaxAgain = 8192; 
            pstAeSnsDft->u32MinAgain = 2048;
            pstAeSnsDft->u32MaxAgainTarget = pstAeSnsDft->u32MaxAgain;
            pstAeSnsDft->u32MinAgainTarget = pstAeSnsDft->u32MinAgain;
            
            pstAeSnsDft->u32MaxDgain = 16383;  
            pstAeSnsDft->u32MinDgain = 384;
            pstAeSnsDft->u32MaxDgainTarget = pstAeSnsDft->u32MaxDgain;
            pstAeSnsDft->u32MinDgainTarget = pstAeSnsDft->u32MinDgain;
                       
        break; 
        }
        
        case WDR_MODE_BUILT_IN:
        {
            pstAeSnsDft->au8HistThresh[0] = 0xd;
            pstAeSnsDft->au8HistThresh[1] = 0x28;
            pstAeSnsDft->au8HistThresh[2] = 0x60;
            pstAeSnsDft->au8HistThresh[3] = 0x80;

            
            pstAeSnsDft->u8AeCompensation = 0x40;
            pstAeSnsDft->u32MaxIntTime = gu32FullLinesStd - 4;
            pstAeSnsDft->u32MinIntTime = 2;
            pstAeSnsDft->u32MaxIntTimeTarget = 65535;
            pstAeSnsDft->u32MinIntTimeTarget = 2;
            
            pstAeSnsDft->u32MaxAgain = 8192; 
            pstAeSnsDft->u32MinAgain = 2048;
            pstAeSnsDft->u32MaxAgainTarget = pstAeSnsDft->u32MaxAgain;
            pstAeSnsDft->u32MinAgainTarget = pstAeSnsDft->u32MinAgain;
            
            pstAeSnsDft->u32MaxDgain = 16383;  
            pstAeSnsDft->u32MinDgain = 384;
            pstAeSnsDft->u32MaxDgainTarget = pstAeSnsDft->u32MaxDgain;
            pstAeSnsDft->u32MinDgainTarget = pstAeSnsDft->u32MinDgain;
            pstAeSnsDft->u32InitExposure = 96000;
            
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
    if (WDR_MODE_NONE == genSensorMode)
    {
        if ((f32Fps <= 30) && (f32Fps >= 0.51))
        {
			if (SENSOR_1080P_30FPS_MODE == gu8SensorImageMode)
			{
				gu32FullLinesStd = VMAX_1080P30_LINEAR * 30 / f32Fps;       	
			}
			else
			{
				printf("Not support ImageMode&Fps: %d %f\n", gu8SensorImageMode, f32Fps);
				return;
			}
        }
        else
        {
            printf("Not support Fps: %f\n", f32Fps);
            return;
        }
    }
    else if(WDR_MODE_BUILT_IN == genSensorMode)
    {
        if (f32Fps == 30)
        {
			if (SENSOR_1080P_30FPS_MODE == gu8SensorImageMode)
			{
				gu32FullLinesStd = VMAX_1080P30_LINEAR;       	
			}
			else
			{
				printf("Not support ImageMode&Fps: %d %f\n", gu8SensorImageMode, f32Fps);
				return;
			}
        }
        else if (f32Fps == 25)
        {
			if (SENSOR_1080P_30FPS_MODE == gu8SensorImageMode)
			{
				gu32FullLinesStd = VMAX_1080P30_LINEAR * 30 / f32Fps;       	
			}
			else
			{
				printf("Not support ImageMode&Fps: %d %f\n", gu8SensorImageMode, f32Fps);
				return;
			}
        }
        else
        {
            printf("Not support Fps: %f\n", f32Fps);
            return;
        }    
    }
    else
    {
        printf("Not support! gu8SensorImageMode:%d, f32Fps:%f\n", gu8SensorImageMode, f32Fps);
        return;
        
    }

    gu32FullLinesStd = (gu32FullLinesStd > FULL_LINES_MAX) ? FULL_LINES_MAX : gu32FullLinesStd;
    g_stSnsRegsInfo.astI2cData[7].u32Data = ((gu32FullLinesStd & 0xFF00) >> 8);
    g_stSnsRegsInfo.astI2cData[8].u32Data = (gu32FullLinesStd & 0xFF);        

    pstAeSnsDft->f32Fps = f32Fps;
    pstAeSnsDft->u32LinesPer500ms = gu32FullLinesStd * f32Fps / 2;
    pstAeSnsDft->u32MaxIntTime = gu32FullLinesStd - 4;
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
        g_stSnsRegsInfo.astI2cData[7].u32Data = ((u32FullLines & 0xFF00) >> 8);
        g_stSnsRegsInfo.astI2cData[8].u32Data = (u32FullLines & 0xFF);  
    }
    else
    {
       pstAeSnsDft->u32FullLines = gu32FullLinesStd;
       return;
    }
  
    pstAeSnsDft->u32MaxIntTime = gu32FullLines - 4;
    
    return;
}

/* while isp notify ae to update sensor regs, ae call these funcs. */
static HI_VOID cmos_inttime_update(HI_U32 u32IntTime)
{
    g_stSnsRegsInfo.astI2cData[0].u32Data = ((u32IntTime & 0xFF00) >> 8);
    g_stSnsRegsInfo.astI2cData[1].u32Data = (u32IntTime & 0xFF);
    
    return;
}

static HI_U32 again_table[8]=
{    
    1024, 2048, 4096, 8192, 11264, 22528, 45056, 90112    
};

static HI_VOID cmos_again_calc_table(HI_U32 *pu32AgainLin, HI_U32 *pu32AgainDb)
{
    int i;

    if((HI_NULL == pu32AgainLin) ||(HI_NULL == pu32AgainDb))
    {
        printf("null pointer when get ae sensor gain info  value!\n");
        return;
    }

    if (*pu32AgainLin >= again_table[7])
    {
         *pu32AgainLin = again_table[7];
         *pu32AgainDb = 7;
         return ;
    }
    
    for (i = 1; i < 8; i++)
    {
        if (*pu32AgainLin < again_table[i])
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
    //HI_U32 u32Tmp_Dgain = u32Dgain >> 2;

    if(u32Again < 1)
    {
        u32Again = 0x0;    //LCG 1024
    }
    else if(u32Again < 2)
    {
        u32Again = 0x1;      //LCG 2048
    }
    else if(u32Again < 3)
    {
        u32Again = 0x2;    //LCG 4096
    }
    else if(u32Again < 4)
    {
        u32Again = 0x3;    //LCG 8192
    }
    else if(u32Again < 5)
    {
        u32Again = 0x40;      //HCG 1024
    }
    else if(u32Again < 6)
    {
        u32Again = 0x41;    //HCG 2048
    }
    else if(u32Again < 7)
    {
        u32Again = 0x42;    //HCG 4096
    }
    else
    {        
        u32Again = 0x43;    //HCG 8192
    }
    
    if (WDR_MODE_NONE == genSensorMode)
    {     
        g_stSnsRegsInfo.astI2cData[2].u32Data = u32Again & 0xFF;
        g_stSnsRegsInfo.astI2cData[3].u32Data = ((u32Dgain & 0xFF00) >> 8);
        g_stSnsRegsInfo.astI2cData[4].u32Data = (u32Dgain & 0xFF);
    }
    else if (WDR_MODE_BUILT_IN == genSensorMode)
    {
        g_stSnsRegsInfo.astI2cData[2].u32Data = ((u32Again << 2)|u32Again);
        g_stSnsRegsInfo.astI2cData[3].u32Data = ((u32Dgain & 0xFF00) >> 8);
        g_stSnsRegsInfo.astI2cData[4].u32Data = (u32Dgain & 0xFF);
        g_stSnsRegsInfo.astI2cData[5].u32Data = ((u32Dgain & 0xFF00) >> 8);
        g_stSnsRegsInfo.astI2cData[6].u32Data = (u32Dgain & 0xFF);
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
        
    }
    return 0;
}

#else

static AWB_CCM_S g_stAwbCcm =
{  
    5120,    
    {       
        0x01AE,  0x8072,  0x803C,
        0x8059,  0x01BE,  0x8065,       
        0x0006,  0x808D,  0x0187    
    },  
    
    3633,    
    {       
        0x01C5,  0x8093,  0x8032,       
        0x806A,  0x01AE,  0x8044,       
        0x001A,  0x80B9,  0x019F    
    },
    
    2465,    
    {            
        0x01C2,  0x80B3,  0x800F,        
        0x806C,  0x0180,  0x8014,       
        0x003E,  0x8223,  0x02E5    
    }
};

static AWB_CCM_S g_stAwbCcmWDR =
{  
    5120,    
    {       
        0x0178,  0x8063,  0x8015,
        0x8033,  0x0162,  0x802F,       
        0x801D,  0x80A9,  0x01C6    
    },  
    
    3633,    
    {       
        0x01C5,  0x8093,  0x8032,       
        0x806A,  0x01AE,  0x8044,       
        0x001A,  0x80B9,  0x019F    
    },
    
    2465,    
    {            
        0x01C2,  0x80B3,  0x800F,        
        0x806C,  0x0180,  0x8014,       
        0x003E,  0x8223,  0x02E5    
    }
};

static AWB_AGC_TABLE_S g_stAwbAgcTable =
{
    /* bvalid */
    1,
	
    /*1,  2,  4,  8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384, 32768*/
    /* saturation */   
    {0x80,0x80,0x7b,0x78,0x70,0x68,0x60,0x58,0x50,0x48,0x40,0x38,0x38,0x38,0x38,0x38}
};

static AWB_AGC_TABLE_S g_stAwbAgcTableWDR =
{
    /* bvalid */
    1,
	
    /*1,  2,  4,  8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384, 32768*/
    /* saturation */   
    {0x80,0x80,0x7b,0x78,0x70,0x68,0x60,0x58,0x50,0x48,0x40,0x38,0x38,0x38,0x38,0x38}
};

static HI_S32 cmos_get_awb_default(AWB_SENSOR_DEFAULT_S *pstAwbSnsDft)
{
    if (HI_NULL == pstAwbSnsDft)
    {
        printf("null pointer when get awb default value!\n");
        return -1;
    }

    memset(pstAwbSnsDft, 0, sizeof(AWB_SENSOR_DEFAULT_S));
    pstAwbSnsDft->u16WbRefTemp = 5120;
    pstAwbSnsDft->au16GainOffset[0] = 0x25B;    
    pstAwbSnsDft->au16GainOffset[1] = 0x100;    
    pstAwbSnsDft->au16GainOffset[2] = 0x100;    
    pstAwbSnsDft->au16GainOffset[3] = 0x19A;    
    pstAwbSnsDft->as32WbPara[0] = -28;    
    pstAwbSnsDft->as32WbPara[1] = 284;    
    pstAwbSnsDft->as32WbPara[2] = 0;    
    pstAwbSnsDft->as32WbPara[3] = 163812;    
    pstAwbSnsDft->as32WbPara[4] = 128;    
    pstAwbSnsDft->as32WbPara[5] = -113080;
    
    switch (genSensorMode)
    {
        default:
        case WDR_MODE_NONE:
            
            memcpy(&pstAwbSnsDft->stCcm, &g_stAwbCcm, sizeof(AWB_CCM_S));
            memcpy(&pstAwbSnsDft->stAgcTbl, &g_stAwbAgcTable, sizeof(AWB_AGC_TABLE_S));
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
    }
    pstDef->stSensorMaxResolution.u32MaxWidth  = 1920;
    pstDef->stSensorMaxResolution.u32MaxHeight = 1080;

    return 0;
}

#else


#define DMNR_CALIB_CARVE_NUM_OV2718 11

float g_coef_calib_ov2718[DMNR_CALIB_CARVE_NUM_OV2718][4] = 
{
    {251.000000f, 2.399674f, 0.037960f, 6.017098f, }, 
    {314.000000f, 2.496930f, 0.038185f, 6.077293f, }, 
    {427.000000f, 2.630428f, 0.037967f, 6.196352f, }, 
    {854.000000f, 2.931458f, 0.038547f, 6.405700f, }, 
    {1166.000000f, 3.066699f, 0.039888f, 6.283467f, }, 
    {2333.000000f, 3.367915f, 0.041175f, 6.977968f, }, 
    {5382.000000f, 3.730944f, 0.046696f, 7.188173f, }, 
    {10765.000000f, 4.032014f, 0.053349f, 8.365396f, }, 
    {21531.000000f, 4.333064f, 0.064582f, 10.314905f, }, 
    {45765.000000f, 4.660533f, 0.085307f, 13.978680f, }, 
    {91531.000000f, 4.961568f, 0.108957f, 21.783899f, }, 
};

static ISP_NR_ISO_PARA_TABLE_S g_stNrIsoParaTab[HI_ISP_NR_ISO_LEVEL_MAX] = 
{
     //u16Threshold//u8varStrength//u8fixStrength//u8LowFreqSlope	
       {1500,       160,              0,            0 },  //100    //                      //                                                
       {1500,       160,              0,            0 },  //200    // ISO                  // ISO //u8LowFreqSlope
       {1250,       180,             10,            0 },  //400    //{400,  1200, 96,256}, //{400 , 0  }
       {1250,       180,             10,            0 },  //800    //{800,  1400, 80,256}, //{600 , 2  }
       {1250,       200,             20,            0 },  //1600   //{1600, 1200, 72,256}, //{800 , 8  }
       {1000,       200,             20,            0 },  //3200   //{3200, 1200, 64,256}, //{1000, 12 }
       {1000,       220,             30,            0 },  //6400   //{6400, 1100, 56,256}, //{1600, 6  }
       { 800,       220,             30,            0 },  //12800  //{12000,1100, 48,256}, //{2400, 0  }
       { 800,       255,             40,            0 },  //25600  //{36000,1100, 48,256}, //
       { 600,       255,             40,            0 },  //51200  //{64000,1100, 96,256}, //
       { 600,       255,             50,            0 },  //102400 //{82000,1000,240,256}, //
       { 600,       255,             50,            0 },  //204800 //                           //
       { 600,       255,             60,            0 },  //409600 //                           //
       { 600,       255,             60,            0 },  //819200 //                           //
       { 600,       255,             60,            0 },  //1638400//                           //
       { 600,       255,             60,            0 },  //3276800//                           //
};

static ISP_CMOS_DEMOSAIC_S g_stIspDemosaic =
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
	200, /*u16UuSlope*/	
	{512,512,512,512,512,512,512,400,0,0,0,0,0,0,0,0}    /*au16NpOffset[ISP_AUTO_ISO_STENGTH_NUM]*/
};

static ISP_CMOS_GE_S g_stIspGe =
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
    {50,	45,		43, 	43, 	41, 	41,  	39,  	37,    	35,    	33,    	30,    	30,    	30,    	30,    	30,    		30},/*SharpenUD*/
    {40,	40,		43,	 	43, 	50, 	50,  	55, 	55, 	65, 	65, 	45, 	45, 	45, 	45, 	45, 		45},/*SharpenD*/
    { 10,   10,  	12,  	14, 	16, 	18,  	19,  	20,    	22,    	24,    	26,    	28,    	28,    	28,    	28,    		28},/*TextureNoiseThd*/
    {  0,  	0,  	2,  	2,   	5,   	7,    	12,    	12,    	0,    	0,    	0,    	0,    	0,    	0,    	0,    		0},/*EdgeNoiseThd*/
    {  150, 130,  	120,  	110,  	110,  	90,    	60,    	40,    	20,    	10,    	0,    	0,    	0,    	0,    	0,    		0},/*overshoot*/
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
  //{100,	200,	400,	800,	1600,	3200,	6400,	12800,	25600,	51200,	102400,	204800,	409600,	819200,	1638400,	3276800};
	{1,	    2,      4,      5,      7,      48,     32,     16,     16,     16,     16,     16,     16,     16,     16,     16},      /*UVNRThreshold*/
 	{0,		0,		0,		0,		0,		0,		0,		0,		0,		1,		1,		2,		2,		2,		2,		2},  /*Coring_lutLimit*/
	{0,		0,		0,		16,		34,		34,		34,		34,		34,		34,		34,		34,		34,		34,		34,		34}  /*UVNR_blendRatio*/
};

static ISP_CMOS_DPC_S g_stCmosDpc = 
{
	//1,/*IR_channel*/
	//1,/*IR_position*/
	{70,150,240,248,250,252,252,252,252,252,252,252,252,252,252,252},/*au16Strength[16]*/
	{0,0,0,0,0,0,0,0,0x24,0x80,0x80,0x80,0xE5,0xE5,0xE5,0xE5},/*au16BlendRatio[16]*/
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
	16,
	512,
	24,
	1024,
	80,
	8192,
	128,
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
            memcpy(&pstDef->stDemosaic, &g_stIspDemosaic, sizeof(ISP_CMOS_DEMOSAIC_S));
            memcpy(&pstDef->stRgbSharpen, &g_stIspRgbSharpen, sizeof(ISP_CMOS_RGBSHARPEN_S));
            memcpy(&pstDef->stGe, &g_stIspGe, sizeof(ISP_CMOS_GE_S));
            
			pstDef->stNoiseTbl.stNrCaliPara.u8CalicoefRow = DMNR_CALIB_CARVE_NUM_OV2718;
            pstDef->stNoiseTbl.stNrCaliPara.pCalibcoef    = (HI_FLOAT (*)[4])g_coef_calib_ov2718;
            memcpy(&pstDef->stNoiseTbl.stIsoParaTable[0], &g_stNrIsoParaTab[0],sizeof(ISP_NR_ISO_PARA_TABLE_S)*HI_ISP_NR_ISO_LEVEL_MAX);
                    
            memcpy(&pstDef->stUvnr,      &g_stIspUVNR,       sizeof(ISP_CMOS_UVNR_S));
            memcpy(&pstDef->stDpc,       &g_stCmosDpc,       sizeof(ISP_CMOS_DPC_S));

        break;
        case WDR_MODE_BUILT_IN:
            
            memcpy(&pstDef->stDrc, &g_stIspDrcWDR, sizeof(ISP_CMOS_DRC_S));
            memcpy(&pstDef->stDemosaic, &g_stIspDemosaicWDR, sizeof(ISP_CMOS_DEMOSAIC_S));	
            memcpy(&pstDef->stGe, &g_stIspGeWDR, sizeof(ISP_CMOS_GE_S));

			pstDef->stNoiseTbl.stNrCaliPara.u8CalicoefRow = DMNR_CALIB_CARVE_NUM_OV2718;
			pstDef->stNoiseTbl.stNrCaliPara.pCalibcoef    = (HI_FLOAT (*)[4])g_coef_calib_ov2718;
			memcpy(&pstDef->stNoiseTbl.stIsoParaTable[0], &g_stNrIsoParaTab[0],sizeof(ISP_NR_ISO_PARA_TABLE_S)*HI_ISP_NR_ISO_LEVEL_MAX);
			
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
                pstBlackLevel->au16BlackLevel[i] = 0x40;
            }
            break;
        case WDR_MODE_BUILT_IN :
            for (i=0; i<4; i++)
            {
                pstBlackLevel->au16BlackLevel[i] = 0x40;
            }
            break;
    }
    
    return 0;  
    
}

HI_VOID cmos_set_pixel_detect(HI_BOOL bEnable)
{
    HI_U32 u32FullLines_5Fps = 0; 
    HI_U32 u32MaxIntTime_5Fps = 0;
    
    if (WDR_MODE_NONE == genSensorMode)
    {
        if (SENSOR_1080P_30FPS_MODE == gu8SensorImageMode)
        {
            u32FullLines_5Fps = (VMAX_1080P30_LINEAR * 30) / 5;
        }
        else
        {
            return;
        }
    }
    else
    {
        return;
    }

    u32FullLines_5Fps = (u32FullLines_5Fps > FULL_LINES_MAX) ? FULL_LINES_MAX : u32FullLines_5Fps;
    u32MaxIntTime_5Fps = u32FullLines_5Fps - 4;

    if (bEnable) /* setup for ISP pixel calibration mode */
    {
        sensor_write_register(VMAX_ADDR_H, (u32FullLines_5Fps & 0xFF00) >> 8);  /* 5fps */
        sensor_write_register(VMAX_ADDR_L, u32FullLines_5Fps & 0xFF);           /* 5fps */
        sensor_write_register(EXPOSURE_ADDR_H, (u32MaxIntTime_5Fps & 0xFF00) >> 8);      /* max exposure lines */
        sensor_write_register(EXPOSURE_ADDR_L, u32MaxIntTime_5Fps & 0xFF);               /* max exposure lines */
        sensor_write_register(AGC_ADDR, 0x0);                                    /* min AG */
        sensor_write_register(DGC_HCG_ADDR_H, 0x02);                                    /* min DG */
        sensor_write_register(DGC_HCG_ADDR_L, 0x85);                                    /* min DG */
    }
    else /* setup for ISP 'normal mode' */
    {
        sensor_write_register(VMAX_ADDR_H, (gu32FullLinesStd & 0xFF00) >> 8);
        sensor_write_register(VMAX_ADDR_L, gu32FullLinesStd & 0xFF);
        
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
            if (SENSOR_1080P_30FPS_MODE == gu8SensorImageMode)
            {
                gu32FullLinesStd = VMAX_1080P30_LINEAR;
            }
            genSensorMode = WDR_MODE_NONE;
            printf("linear mode\n");
        break;

        case WDR_MODE_BUILT_IN:
            if (SENSOR_1080P_30FPS_MODE == gu8SensorImageMode)
            {
                gu32FullLinesStd = VMAX_1080P30_LINEAR;
            }            
            genSensorMode = WDR_MODE_BUILT_IN;
        break;
        
        default:
            printf("NOT support this mode!\n");
            return;
        break;
    }
    return;
}

HI_U32 cmos_get_sns_regs_info(ISP_SNS_REGS_INFO_S *pstSnsRegsInfo)
{
    HI_S32 i;

    if (HI_FALSE == bInit)
    {
        g_stSnsRegsInfo.enSnsType = ISP_SNS_I2C_TYPE;
        g_stSnsRegsInfo.u8Cfg2ValidDelayMax = 3;
        
        g_stSnsRegsInfo.u32RegNum = 9;
        
        for (i=0; i<g_stSnsRegsInfo.u32RegNum; i++)
        {	
            g_stSnsRegsInfo.astI2cData[i].bUpdate = HI_TRUE;
            g_stSnsRegsInfo.astI2cData[i].u8DevAddr = sensor_i2c_addr;
            g_stSnsRegsInfo.astI2cData[i].u32AddrByteNum = sensor_addr_byte;
            g_stSnsRegsInfo.astI2cData[i].u32DataByteNum = sensor_data_byte;
        }		
        g_stSnsRegsInfo.astI2cData[0].u8DelayFrmNum = 1;
        g_stSnsRegsInfo.astI2cData[0].u32RegAddr = EXPOSURE_ADDR_H;
        g_stSnsRegsInfo.astI2cData[1].u8DelayFrmNum = 1;
        g_stSnsRegsInfo.astI2cData[1].u32RegAddr = EXPOSURE_ADDR_L;

        /* AG */
        g_stSnsRegsInfo.astI2cData[2].u8DelayFrmNum = 1;
        g_stSnsRegsInfo.astI2cData[2].u32RegAddr = AGC_ADDR;

        /* DG(HCG) */
        g_stSnsRegsInfo.astI2cData[3].u8DelayFrmNum = 1;
        g_stSnsRegsInfo.astI2cData[3].u32RegAddr = DGC_HCG_ADDR_H;
        g_stSnsRegsInfo.astI2cData[4].u8DelayFrmNum = 1;
        g_stSnsRegsInfo.astI2cData[4].u32RegAddr = DGC_HCG_ADDR_L;
        
        /* DG(LCG) */
        g_stSnsRegsInfo.astI2cData[5].u8DelayFrmNum = 1;
        g_stSnsRegsInfo.astI2cData[5].u32RegAddr = DGC_LCG_ADDR_H;
        g_stSnsRegsInfo.astI2cData[6].u8DelayFrmNum = 1;
        g_stSnsRegsInfo.astI2cData[6].u32RegAddr = DGC_LCG_ADDR_L;        

        /* VMAX */       
        g_stSnsRegsInfo.astI2cData[7].u32RegAddr = VMAX_ADDR_H;  
        g_stSnsRegsInfo.astI2cData[7].u8DelayFrmNum = 1;
        g_stSnsRegsInfo.astI2cData[8].u32RegAddr = VMAX_ADDR_L;
        g_stSnsRegsInfo.astI2cData[8].u8DelayFrmNum = 1;
        
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
    
    if(gu32PreFullLines < gu32FullLines)
    {
        g_stSnsRegsInfo.astI2cData[7].u8DelayFrmNum = 0;
        g_stSnsRegsInfo.astI2cData[8].u8DelayFrmNum = 0;
    }
    else
    {
        g_stSnsRegsInfo.astI2cData[7].u8DelayFrmNum = 1;
        g_stSnsRegsInfo.astI2cData[8].u8DelayFrmNum = 1;
    }
    
    if (HI_NULL == pstSnsRegsInfo)
    {
        printf("null pointer when get sns reg info!\n");
        return -1;
    }
    
    gu32PreFullLines = gu32FullLines;
    
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

static HI_S32 cmos_set_image_mode(ISP_CMOS_SENSOR_IMAGE_MODE_S *pstSensorImageMode)
{
    HI_U8 u8SensorImageMode = gu8SensorImageMode;
    
    bInit = HI_FALSE;    
        
    if (HI_NULL == pstSensorImageMode )
    {
        printf("null pointer when set image mode\n");
        return -1;
    }
    
    if((pstSensorImageMode->u16Width <= 1920) && (pstSensorImageMode->u16Height <= 1080))
    {
        if (WDR_MODE_NONE == genSensorMode)
        {
            if (pstSensorImageMode->f32Fps <= 30)
            {
                u8SensorImageMode = SENSOR_1080P_30FPS_MODE;
            }
            else
            {
                printf("Not support! Width:%d, Height:%d, Fps:%f, WDRMode:%d\n", 
                    pstSensorImageMode->u16Width, 
                    pstSensorImageMode->u16Height,
                    pstSensorImageMode->f32Fps,
                    genSensorMode);

                return -1;
            }
        }
        
        else if (WDR_MODE_BUILT_IN == genSensorMode)
        {
            if (pstSensorImageMode->f32Fps <= 30)
            {
                u8SensorImageMode = SENSOR_1080P_30FPS_MODE;
            }
            else
            {
                printf("Not support! Width:%d, Height:%d, Fps:%f, WDRMode:%d\n", 
                    pstSensorImageMode->u16Width, 
                    pstSensorImageMode->u16Height,
                    pstSensorImageMode->f32Fps,
                    genSensorMode);

                return -1;
            }
        }
        else
        {
            printf("Not support! Width:%d, Height:%d, Fps:%f, WDRMode:%d\n", 
                pstSensorImageMode->u16Width, 
                pstSensorImageMode->u16Height,
                pstSensorImageMode->f32Fps,
                genSensorMode);

            return -1;
        }
    }
    else
    {
        printf("Not support! Width:%d, Height:%d, Fps:%f, WDRMode:%d\n", 
            pstSensorImageMode->u16Width, 
            pstSensorImageMode->u16Height,
            pstSensorImageMode->f32Fps,
            genSensorMode);

        return -1;
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

HI_VOID sensor_global_init()
{     
    gu8SensorImageMode = SENSOR_1080P_30FPS_MODE;
    genSensorMode = WDR_MODE_NONE;
    gu32FullLinesStd = VMAX_1080P30_LINEAR;
    gu32FullLines = VMAX_1080P30_LINEAR; 
    gu32PreFullLines = VMAX_1080P30_LINEAR;
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
    s32Ret = HI_MPI_ISP_SensorRegCallBack(IspDev, OV2718_ID, &stIspRegister);
    if (s32Ret)
    {
        printf("sensor register callback function failed!\n");
        return s32Ret;
    }
    
    stLib.s32Id = 0;
    strncpy(stLib.acLibName, HI_AE_LIB_NAME, sizeof(HI_AE_LIB_NAME));
    cmos_init_ae_exp_function(&stAeRegister.stSnsExp);
    s32Ret = HI_MPI_AE_SensorRegCallBack(IspDev, &stLib, OV2718_ID, &stAeRegister);
    if (s32Ret)
    {
        printf("sensor register callback function to ae lib failed!\n");
        return s32Ret;
    }

    stLib.s32Id = 0;
    strncpy(stLib.acLibName, HI_AWB_LIB_NAME, sizeof(HI_AWB_LIB_NAME));
    cmos_init_awb_exp_function(&stAwbRegister.stSnsExp);
    s32Ret = HI_MPI_AWB_SensorRegCallBack(IspDev, &stLib, OV2718_ID, &stAwbRegister);
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

    s32Ret = HI_MPI_ISP_SensorUnRegCallBack(IspDev, OV2718_ID);
    if (s32Ret)
    {
        printf("sensor unregister callback function failed!\n");
        return s32Ret;
    }
    
    stLib.s32Id = 0;
    strncpy(stLib.acLibName, HI_AE_LIB_NAME, sizeof(HI_AE_LIB_NAME));
    s32Ret = HI_MPI_AE_SensorUnRegCallBack(IspDev, &stLib, OV2718_ID);
    if (s32Ret)
    {
        printf("sensor unregister callback function to ae lib failed!\n");
        return s32Ret;
    }

    stLib.s32Id = 0;
    strncpy(stLib.acLibName, HI_AWB_LIB_NAME, sizeof(HI_AWB_LIB_NAME));
    s32Ret = HI_MPI_AWB_SensorUnRegCallBack(IspDev, &stLib, OV2718_ID);
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

#endif /* __OV9750_CMOS_H_ */
