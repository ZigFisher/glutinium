#if !defined(__IMX222_CMOS_H_)
#define __IMX222_CMOS_H_

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

#define IMX222_ID 222


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

#define FULL_LINES_MAX  (0xFFFF)

#define SHS1_ADDR (0x208) 
#define GAIN_ADDR (0x21E)
#define VMAX_ADDR (0x205)

#define SENSOR_1080P_30FPS_MODE (1) 
#define SENSOR_720P_60FPS_MODE  (2) 
#define INCREASE_LINES (0) /* make real fps less than stand fps because NVR require*/
#define VMAX_1080P30    (1125+INCREASE_LINES)
#define VMAX_720P60     (750+INCREASE_LINES)

HI_U8 gu8SensorImageMode = SENSOR_1080P_30FPS_MODE;
WDR_MODE_E genSensorMode = WDR_MODE_NONE;

static HI_U32 gu32FullLinesStd = VMAX_1080P30; 
static HI_U32 gu32FullLines = VMAX_1080P30;
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
#define CMOS_CFG_INI "imx222_cfg.ini"
static char pcName[PATHLEN_MAX] = "configs/imx222_cfg.ini";

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
    pstAeSnsDft->stAgainAccu.f32Accuracy = 0.1;

    pstAeSnsDft->stDgainAccu.enAccuType = AE_ACCURACY_TABLE;
    pstAeSnsDft->stDgainAccu.f32Accuracy = 0.1;
    
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
    
            pstAeSnsDft->u32MaxAgain = 16229;  
            pstAeSnsDft->u32MinAgain = 1024;
            pstAeSnsDft->u32MaxAgainTarget = g_AeDft[0].u32MaxAgainTarget;
            pstAeSnsDft->u32MinAgainTarget = g_AeDft[0].u32MinAgainTarget;
    
            pstAeSnsDft->u32MaxDgain = 8134;  
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
    pstAeSnsDft->stAgainAccu.f32Accuracy = 0.1;

    pstAeSnsDft->stDgainAccu.enAccuType = AE_ACCURACY_TABLE;
    pstAeSnsDft->stDgainAccu.f32Accuracy = 0.1;
    
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
        default:
        case WDR_MODE_NONE:   /*linear mode*/
            pstAeSnsDft->au8HistThresh[0] = 0xd;
            pstAeSnsDft->au8HistThresh[1] = 0x28;
            pstAeSnsDft->au8HistThresh[2] = 0x60;
            pstAeSnsDft->au8HistThresh[3] = 0x80;
    
            pstAeSnsDft->u8AeCompensation = 0x38; 
    
            pstAeSnsDft->u32MaxIntTime = gu32FullLinesStd - 2;
            pstAeSnsDft->u32MinIntTime = 2;
            pstAeSnsDft->u32MaxIntTimeTarget = 65535;
            pstAeSnsDft->u32MinIntTimeTarget = 2;
            pstAeSnsDft->u32MaxAgain = 16229;  
            pstAeSnsDft->u32MinAgain = 1024;
            pstAeSnsDft->u32MaxAgainTarget = pstAeSnsDft->u32MaxAgain;
            pstAeSnsDft->u32MinAgainTarget = pstAeSnsDft->u32MinAgain;
    
            pstAeSnsDft->u32MaxDgain = 8134;  
            pstAeSnsDft->u32MinDgain = 1024;
            pstAeSnsDft->u32MaxDgainTarget = pstAeSnsDft->u32MaxDgain;
            pstAeSnsDft->u32MinDgainTarget = pstAeSnsDft->u32MinDgain;
        break;
        
    }
    return 0;
}

#endif

/* the function of sensor set fps */
static HI_VOID cmos_fps_set(HI_FLOAT f32Fps, AE_SENSOR_DEFAULT_S *pstAeSnsDft)
{
    if (SENSOR_1080P_30FPS_MODE == gu8SensorImageMode)
    {
        if ((f32Fps <= 30) && (f32Fps >= 0.51))
        {
            gu32FullLinesStd = VMAX_1080P30*30/f32Fps;
        }
        else
        {
            printf("Not support Fps: %f\n", f32Fps);
            return;
        }
    }
    else if (SENSOR_720P_60FPS_MODE == gu8SensorImageMode)
    {
        if ((f32Fps <= 60) && (f32Fps >= 0.68))
        {
            gu32FullLinesStd = VMAX_720P60*60/f32Fps;
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

    g_stSnsRegsInfo.astSspData[3].u32Data = (gu32FullLinesStd & 0xFF);/* VMAX[7:0] */
    g_stSnsRegsInfo.astSspData[4].u32Data = ((gu32FullLinesStd & 0xFF00) >> 8);/* VMAX[15:8] */

    pstAeSnsDft->f32Fps = f32Fps;
    pstAeSnsDft->u32LinesPer500ms = gu32FullLinesStd * f32Fps / 2;
    pstAeSnsDft->u32MaxIntTime = gu32FullLinesStd - 2;
    pstAeSnsDft->u32FullLinesStd = gu32FullLinesStd;
    pstAeSnsDft->u32FullLines = gu32FullLinesStd;
    

    gu32FullLines = gu32FullLinesStd;

    return;
}

static HI_VOID cmos_slow_framerate_set(HI_U32 u32FullLines,
    AE_SENSOR_DEFAULT_S *pstAeSnsDft)
{
    u32FullLines = (u32FullLines > FULL_LINES_MAX) ? FULL_LINES_MAX : u32FullLines;
    gu32FullLines = u32FullLines;
    pstAeSnsDft->u32FullLines = gu32FullLines;

    g_stSnsRegsInfo.astSspData[3].u32Data = (gu32FullLines & 0xFF);
    g_stSnsRegsInfo.astSspData[4].u32Data = ((gu32FullLines & 0xFF00) >> 8);

    pstAeSnsDft->u32MaxIntTime = gu32FullLines - 2;
    
    return;
}

/* while isp notify ae to update sensor regs, ae call these funcs. */
static HI_VOID cmos_inttime_update(HI_U32 u32IntTime)
{
    HI_U32 u32Value = gu32FullLines - u32IntTime;

    g_stSnsRegsInfo.astSspData[0].u32Data = (u32Value & 0xFF);
    g_stSnsRegsInfo.astSspData[1].u32Data = ((u32Value & 0xFF00) >> 8);   

    return;
}

static HI_U32 digital_gain_table[61]=
{
    1024,  1060,  1097,  1136,  1176,  1217,  1260,  1304,  1350,  1397,  1446,   1497,  1550,   1604,  1661,   1719,  
    1780,  1842,  1907,  1974,  2043,  2115,  2189,  2266,  2346,  2428,  2514,   2602,  2693,   2788,  2886,   2987,  
    3092,  3201,  3314,  3430,  3551,  3675,  3805,  3938,  4077,  4220,  4368,   4522,  4681,   4845,  5015,   5192,  
    5374,  5563,  5758,  5961,  6170,  6387,  6611,  6844,  7084,  7333,  7591,   7858,  8134  
};


static HI_U32 analog_gain_table[81] =
{
     1024 , 1060 ,  1097 ,  1136 ,  1176,  1217 , 1260 ,  1304,  1350 ,  1397 ,  1446 ,  1497 , 1550 , 1604 ,  1661 ,  1719 , 
     1780 , 1842 ,  1907 ,  1974 ,  2043,  2115 , 2189 ,  2266,  2346 ,  2428 ,  2514 ,  2602 , 2693 , 2788 ,  2886 ,  2987 , 
     3092 , 3201 ,  3314 ,  3430 ,  3551,  3675 , 3805 ,  3938,  4077 ,  4220 ,  4368 ,  4522 , 4681 , 4845 ,  5015 ,  5192 , 
     5374 , 5563 ,  5758 ,  5961 ,  6170,  6387 , 6611 ,  6844,  7084 ,  7333 ,  7591 ,  7858 , 8134 , 8420 ,  8716 ,  9022 , 
     9339 , 9667 , 10007 , 10359 , 10723, 11099 ,11489 , 11893, 12311 , 12744 , 13192 , 13655 ,14135 ,14632 , 15146 , 15678 , 
    16229     

};


static HI_VOID cmos_again_calc_table(HI_U32 *pu32AgainLin, HI_U32 *pu32AgainDb)
{
    int i;

    if (*pu32AgainLin >= analog_gain_table[80])
    {
         *pu32AgainLin = analog_gain_table[80];
         *pu32AgainDb = 80;
         return ;
    }
    
    for (i = 1; i < 81; i++)
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

static HI_VOID cmos_dgain_calc_table(HI_U32 *pu32DgainLin, HI_U32 *pu32DgainDb)
{
    int i;

    if (*pu32DgainLin >= digital_gain_table[60])
    {
         *pu32DgainLin = digital_gain_table[60];
         *pu32DgainDb = 60;
         return ;
    }
    
    for (i = 1; i < 61; i++)
    {
        if (*pu32DgainLin < digital_gain_table[i])
        {
            *pu32DgainLin = digital_gain_table[i - 1];
            *pu32DgainDb = i - 1;
            break;
        }
    }

    return;
}

static HI_VOID cmos_gains_update(HI_U32 u32Again, HI_U32 u32Dgain)
{  

    HI_U32 u32Tmp = u32Again + u32Dgain;
    u32Tmp = u32Tmp > 0x8C ? 0x8C : u32Tmp;

    g_stSnsRegsInfo.astSspData[2].u32Data = (u32Tmp & 0xFF);

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
    pstExpFuncs->pfn_cmos_dgain_calc_table  = cmos_dgain_calc_table;

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
	    0x01fc, 0x80dc, 0x8020,
	    0x8055, 0x01aa, 0x8055,
	    0x0019, 0x80db, 0x01c2
    },
    3633,
    {
        0x01ef, 0x80bb, 0x8034,
        0x807f, 0x01c7, 0x8048,
        0x001b, 0x80fc, 0x01e1
    },
    2465,
    {
        0x01ca, 0x8097, 0x8033,
        0x8099, 0x01c7, 0x802e,
        0x0007, 0x80f7, 0x01f0
    }

/*
    5120,
    {   
        0x01e0, 0x80c8, 0x8018,
        0x804b, 0x018f, 0x8044,
        0x0019, 0x80d5, 0x01bc
    },
    3633,
    {
        0x01cf, 0x80a8, 0x8027,
        0x8072, 0x01a8, 0x8036,
        0x001a, 0x80ed, 0x01d3
    },
    2465,
    {
        0x01ce, 0x80ad, 0x8021,
        0x807c, 0x0192, 0x8016,
        0x0022, 0x80fc, 0x01da
    }
 */
};

static AWB_AGC_TABLE_S g_stAwbAgcTable =
{
    /* bvalid */
    1,

    /* saturation */
    {0x82,0x82,0x80,0x7c,0x70,0x69,0x5c,0x5c,0x57,0x57,0x50,0x50,0x58,0x48,0x40,0x38}
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

 /*   pstAwbSnsDft->au16GainOffset[0] = 0x1c0;
    pstAwbSnsDft->au16GainOffset[1] = 0x100;
    pstAwbSnsDft->au16GainOffset[2] = 0x100;
    pstAwbSnsDft->au16GainOffset[3] = 0x1e4;

    pstAwbSnsDft->as32WbPara[0] = -40;
    pstAwbSnsDft->as32WbPara[1] = 296;
    pstAwbSnsDft->as32WbPara[2] = 0;
    pstAwbSnsDft->as32WbPara[3] = 169788;
    pstAwbSnsDft->as32WbPara[4] = 128;
    pstAwbSnsDft->as32WbPara[5] = -119571;
*/

    pstAwbSnsDft->au16GainOffset[0] = 0x1dc;
    pstAwbSnsDft->au16GainOffset[1] = 0x100;
    pstAwbSnsDft->au16GainOffset[2] = 0x100;
    pstAwbSnsDft->au16GainOffset[3] = 0x1c9;

    pstAwbSnsDft->as32WbPara[0] = 34;
    pstAwbSnsDft->as32WbPara[1] = 120;
    pstAwbSnsDft->as32WbPara[2] = -102;
    pstAwbSnsDft->as32WbPara[3] = 186418;
    pstAwbSnsDft->as32WbPara[4] = 128;
    pstAwbSnsDft->as32WbPara[5] = -137285;


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
            memcpy(&pstDef->stNoiseTbl, &g_IspDft[0].stNoiseTbl, sizeof(ISP_CMOS_NOISE_TABLE_S));            
            memcpy(&pstDef->stDemosaic, &g_IspDft[0].stDemosaic, sizeof(ISP_CMOS_DEMOSAIC_S));
            memcpy(&pstDef->stRgbSharpen, &g_IspDft[0].stRgbSharpen, sizeof(ISP_CMOS_RGBSHARPEN_S));
            memcpy(&pstDef->stGamma, &g_IspDft[0].stGamma, sizeof(ISP_CMOS_GAMMA_S));
            memcpy(&pstDef->stGe, &g_IspDft[0].stGe, sizeof(ISP_CMOS_GE_S));
        break;           

        break;

    }
    pstDef->stSensorMaxResolution.u32MaxWidth  = 2592;
    pstDef->stSensorMaxResolution.u32MaxHeight = 1944;

    return 0;
}

#else



#define DMNR_CALIB_CARVE_NUM_IMX222 12

float g_coef_calib_imx222[DMNR_CALIB_CARVE_NUM_IMX222][4] = 
{
    {100.000000f, 2.000000f, 0.037048f, 9.002845f, }, 
    {204.000000f, 2.309630f, 0.038180f, 9.011998f, }, 
    {407.000000f, 2.609594f, 0.039593f, 9.160780f, }, 
    {812.000000f, 2.909556f, 0.042155f, 9.453709f, }, 
    {1640.000000f, 3.214844f, 0.047406f, 9.905107f, }, 
    {3223.000000f, 3.508260f, 0.055695f, 11.039407f, }, 
    {6457.000000f, 3.810031f, 0.068628f, 13.358517f, }, 
    {10311.000000f, 4.013301f, 0.080570f, 16.527088f, }, 
    {29899.000000f, 4.475657f, 0.112936f, 25.649172f, },
    {35111.000000f, 4.545443f, 0.092135f, 36.210743f, }, 
    {50897.000000f, 4.706692f, 0.100546f, 43.523537f, },
    {74403.000000f, 4.871591f, 0.095404f, 60.593433f, },
};


static ISP_NR_ISO_PARA_TABLE_S g_stNrIsoParaTab[HI_ISP_NR_ISO_LEVEL_MAX] = 
{
     //u16Threshold//u8varStrength//u8fixStrength//u8LowFreqSlope	
       {1500,       256-96,             256-256,            0 },  //100    //                      //           
       {1500,       256-96,             256-256,            0 },  //200    // ISO                  // ISO //u8LowFreqSlope
       {1500,       256-96,             256-256,            0 },  //400    //{400,  1200, 96,256}, //{400 , 0  }
       {1750,       256-80,             256-256,            2 },  //800    //{800,  1400, 80,256}, //{600 , 2  }
       {1500,       256-72,             256-256,            6 },  //1600   //{1600, 1200, 72,256}, //{800 , 8  }
       {1500,       256-64,             256-256,           12 },  //3200   //{3200, 1200, 64,256}, //{1000, 12 }
       {1375,       256-56,             256-256,            6 },  //6400   //{6400, 1100, 56,256}, //{1600, 6  }
       {1375,       256-48,             256-256,            0 },  //12800  //{12000,1100, 48,256}, //{2400, 0  }
       {1375,       256-48,             256-256,            0 },  //25600  //{36000,1100, 48,256}, //
       {1375,      256-128,             256-128,            0 },  //51200  //{64000,1100, 96,256}, //
       {1250,      256-192,             256-256,            0 },  //102400 //{82000,1000,240,256}, //
       {1250,      256-224,             256-256,            0 },  //204800 //                           //
       {1250,      256-224,             256-256,            0 },  //409600 //                           //
       {1250,      256-224,             256-256,            0 },  //819200 //                           //
       {1250,      256-240,             256-256,            0 },  //1638400//                           //
       {1250,      256-240,             256-256,            0 },  //3276800//                           //
};


static ISP_CMOS_DEMOSAIC_S g_stIspDemosaic =
{
	/*For Demosaic*/
	1, /*bEnable*/			
	16,/*u16VhLimit*/	
	8,/*u16VhOffset*/
	24,   /*u16VhSlope*/
	/*False Color*/
	1,    /*bFcrEnable*/
	{ 8, 8, 8, 8, 8, 8, 8, 8, 3, 0, 0, 0, 0, 0, 0, 0},    /*au8FcrStrength[ISP_AUTO_ISO_STENGTH_NUM]*/
	{24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,24},    /*au8FcrThreshold[ISP_AUTO_ISO_STENGTH_NUM]*/
	/*For Ahd*/
	400, /*u16UuSlope*/	
	{512,512,512,512,512,512,512,  400,  0,0,0,0,0,0,0,0}    /*au16NpOffset[ISP_AUTO_ISO_STENGTH_NUM]*/	
};

static ISP_CMOS_GE_S g_stIspGe =
{
	/*For GE*/
	0,    /*bEnable*/			
	7,    /*u8Slope*/	
	7,    /*u8Sensitivity*/
	4096, /*u16Threshold*/
	4096, /*u16SensiThreshold*/	
	{1024,1024,1024,2048,2048,2048,2048,  2048,  2048,2048,2048,2048,2048,2048,2048,2048}    /*au16Strength[ISP_AUTO_ISO_STENGTH_NUM]*/	
};
static ISP_CMOS_RGBSHARPEN_S g_stIspRgbSharpen =
{      
  //{100,200,400,800,1600,3200,6400,12800,25600,51200,102400,204800,409600,819200,1638400,3276800};
    {0,	  0,   0,  0,   0,   0,   0,    0,    0,    1,    1,     1,     1,     1,     1,       1},/* enPixSel */
    {40, 45,  45, 50,  50,  55,  55,   60,   60,   70,   80,    90,   110,   120,   120,     120},/*SharpenUD*/
    {20, 20,  30, 45,  30,  35,  35,   40,   50,   60,   70,    90,   110,   120,   120,     120},/*SharpenD*/
    {0,   2,   4,  6,   6,  12,  30,   60,   80,    0,    0,     0,    0,     0,     0,        0},/*NoiseThd*/
    {2,   4,   8, 16,  25,  11,  12,    0,    0,    0,    0,     0,    0,     0,     0,        0},/*EdgeThd2*/
    {220,230, 200,175, 150, 120, 110,  95,   80,   70,   40,    20,   20,    20,    20,       20},/*overshootAmt*/
    {210,220, 190,140, 135, 130, 110,  95,   75,   60,   50,    50,   50,    50,    50,       50},/*undershootAmt*/
};

static ISP_CMOS_UVNR_S g_stIspUVNR = 
{
   //{100,	200, 400, 800, 1600, 3200, 6400, 12800,	25600, 51200, 102400, 204800, 409600, 819200, 1638400, 3276800};
	  {1,	  2,   4,   5,    7,   10,   12,    16,    18,    20,     22,     24,     24,     24,      24,      24},  /*UVNRThreshold*/
 	  {0,	  0,   0,   0,	  0, 	0,    0,     0,     0,	   1,      1,      2,      2,      2,       2,       2},  /*Coring_lutLimit*/
      {0,	  0,   0,  16,   34,   34,   34,    34,    34,    34,     34,     34,     34,     34,      34,      34}   /*UVNR_blendRatio*/
};

static ISP_CMOS_DPC_S g_stCmosDpc = 
{
	//0,/*IR_channel*/
	//0,/*IR_position*/
	{45,100,152,152,220,224,224,224,224,224,224,224,224,224,224,224},/*au16Strength[16]*/
	{0,0,0,0,0,0,0,0x30,0x60,0x80,0x80,0x80,0xE5,0xE5,0xE5,0xE5},/*au16BlendRatio[16]*/
};

static ISP_CMOS_DRC_S g_stIspDrc =
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

HI_U32 cmos_get_isp_default(ISP_CMOS_DEFAULT_S *pstDef)
{   
    if (HI_NULL == pstDef)
    {
        printf("null pointer when get isp default value!\n");
        return -1;
    }

    memset(pstDef, 0, sizeof(ISP_CMOS_DEFAULT_S));
    
    memcpy(&pstDef->stDrc, &g_stIspDrc, sizeof(ISP_CMOS_DRC_S));
    memcpy(&pstDef->stDemosaic, &g_stIspDemosaic, sizeof(ISP_CMOS_DEMOSAIC_S));
    memcpy(&pstDef->stGe, &g_stIspGe, sizeof(ISP_CMOS_GE_S));	

    pstDef->stNoiseTbl.stNrCaliPara.u8CalicoefRow = DMNR_CALIB_CARVE_NUM_IMX222;
    pstDef->stNoiseTbl.stNrCaliPara.pCalibcoef    = (HI_FLOAT (*)[4])g_coef_calib_imx222;
	memcpy(&pstDef->stNoiseTbl.stIsoParaTable[0], &g_stNrIsoParaTab[0],sizeof(ISP_NR_ISO_PARA_TABLE_S)*HI_ISP_NR_ISO_LEVEL_MAX);
	
    memcpy(&pstDef->stRgbSharpen, &g_stIspRgbSharpen, sizeof(ISP_CMOS_RGBSHARPEN_S));
	memcpy(&pstDef->stUvnr,       &g_stIspUVNR,       sizeof(ISP_CMOS_UVNR_S));
	memcpy(&pstDef->stDpc,       &g_stCmosDpc,       sizeof(ISP_CMOS_DPC_S));

    pstDef->stSensorMaxResolution.u32MaxWidth  = 1920;
    pstDef->stSensorMaxResolution.u32MaxHeight = 1080;

    return 0;
}

#endif

HI_U32 cmos_get_isp_black_level(ISP_CMOS_BLACK_LEVEL_S *pstBlackLevel)
{
    if (HI_NULL == pstBlackLevel)
    {
        printf("null pointer when get isp black level value!\n");
        return -1;
    }

    /* Don't need to update black level when iso change */
    pstBlackLevel->bUpdate = HI_FALSE;
            
    pstBlackLevel->au16BlackLevel[0] = 0xf1;
    pstBlackLevel->au16BlackLevel[1] = 0xf0;
    pstBlackLevel->au16BlackLevel[2] = 0xf1;
    pstBlackLevel->au16BlackLevel[3] = 0xf1;

    return 0;    
}

HI_VOID cmos_set_pixel_detect(HI_BOOL bEnable)
{
    HI_U32 u32FullLines_5Fps = VMAX_1080P30;
    HI_U32 u32MaxExpTime_5Fps = VMAX_1080P30 - 2;
    
    if (SENSOR_1080P_30FPS_MODE == gu8SensorImageMode)
    {
        u32FullLines_5Fps = VMAX_1080P30 * 30 / 5;
    }
    else if (SENSOR_720P_60FPS_MODE == gu8SensorImageMode)
    {
        u32FullLines_5Fps = VMAX_720P60 * 60 / 5;
    }
    else
    {
        return;
    }

    u32FullLines_5Fps = (u32FullLines_5Fps > FULL_LINES_MAX) ? FULL_LINES_MAX : u32FullLines_5Fps;
    u32MaxExpTime_5Fps = 2;
    
    if (bEnable) /* setup for ISP pixel calibration mode */
    {
        sensor_write_register(VMAX_ADDR, u32FullLines_5Fps & 0xFF); 
        sensor_write_register(VMAX_ADDR + 1, (u32FullLines_5Fps & 0xFF00) >> 8);
        sensor_write_register(SHS1_ADDR, u32MaxExpTime_5Fps & 0xFF);    /* shutter */
        sensor_write_register(SHS1_ADDR +1, (u32MaxExpTime_5Fps & 0xFF00) >> 8);
        sensor_write_register(GAIN_ADDR, 0x00); //gain
    }
    else /* setup for ISP 'normal mode' */
    {
        gu32FullLinesStd = (gu32FullLinesStd > FULL_LINES_MAX) ? FULL_LINES_MAX : gu32FullLinesStd;
        sensor_write_register (VMAX_ADDR, gu32FullLinesStd & 0xFF); 
        sensor_write_register (VMAX_ADDR + 1, (gu32FullLinesStd & 0xFF00) >> 8);

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
            printf("linear mode\n");
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
        if (pstSensorImageMode->f32Fps <= 60)
        {
            u8SensorImageMode = SENSOR_720P_60FPS_MODE;
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
    else if((pstSensorImageMode->u16Width <= 1920)&&(pstSensorImageMode->u16Height <= 1080))
    {
        if (pstSensorImageMode->f32Fps <= 30)
        {
            u8SensorImageMode = SENSOR_1080P_30FPS_MODE;
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
        g_stSnsRegsInfo.enSnsType = ISP_SNS_SSP_TYPE;
        g_stSnsRegsInfo.u8Cfg2ValidDelayMax = 2;        
        g_stSnsRegsInfo.u32RegNum = 5;        
        
        for (i=0; i<g_stSnsRegsInfo.u32RegNum; i++)
        {
            g_stSnsRegsInfo.astSspData[i].bUpdate = HI_TRUE;
            g_stSnsRegsInfo.astSspData[i].u32DevAddr = 0x02;
            g_stSnsRegsInfo.astSspData[i].u32DevAddrByteNum = 1;
            g_stSnsRegsInfo.astSspData[i].u32RegAddrByteNum = 1;
            g_stSnsRegsInfo.astSspData[i].u32DataByteNum = 1;
        }        
        g_stSnsRegsInfo.astSspData[0].u8DelayFrmNum = 0;
        g_stSnsRegsInfo.astSspData[0].u32RegAddr = SHS1_ADDR;//shutter SHS1[7:0]
        g_stSnsRegsInfo.astSspData[1].u8DelayFrmNum = 0;
        g_stSnsRegsInfo.astSspData[1].u32RegAddr = SHS1_ADDR+1;//shutter SHS1[15:8]
        g_stSnsRegsInfo.astSspData[2].u8DelayFrmNum = 0;
        g_stSnsRegsInfo.astSspData[2].u32RegAddr = GAIN_ADDR;//gain     
        g_stSnsRegsInfo.astSspData[3].u8DelayFrmNum = 0;
        g_stSnsRegsInfo.astSspData[3].u32RegAddr = VMAX_ADDR;
        g_stSnsRegsInfo.astSspData[4].u8DelayFrmNum = 0;
        g_stSnsRegsInfo.astSspData[4].u32RegAddr = VMAX_ADDR + 1;
      
        bInit = HI_TRUE;
    }
    else
    {
        for (i=0; i<g_stSnsRegsInfo.u32RegNum; i++)
        {
            if (g_stSnsRegsInfo.astSspData[i].u32Data == g_stPreSnsRegsInfo.astSspData[i].u32Data)
            {
                g_stSnsRegsInfo.astSspData[i].bUpdate = HI_FALSE;
            }
            else
            {
                g_stSnsRegsInfo.astSspData[i].bUpdate = HI_TRUE;
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
    gu8SensorImageMode = SENSOR_1080P_30FPS_MODE;
    genSensorMode = WDR_MODE_NONE;
    gu32FullLinesStd = VMAX_1080P30; 
    gu32FullLines = VMAX_1080P30;
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
    s32Ret = HI_MPI_ISP_SensorRegCallBack(IspDev, IMX222_ID, &stIspRegister);
    if (s32Ret)
    {
        printf("sensor register callback function failed!\n");
        return s32Ret;
    }
    
    stLib.s32Id = 0;
    strncpy(stLib.acLibName, HI_AE_LIB_NAME, sizeof(HI_AE_LIB_NAME));
    cmos_init_ae_exp_function(&stAeRegister.stSnsExp);
    s32Ret = HI_MPI_AE_SensorRegCallBack(IspDev, &stLib, IMX222_ID, &stAeRegister);
    if (s32Ret)
    {
        printf("sensor register callback function to ae lib failed!\n");
        return s32Ret;
    }

    stLib.s32Id = 0;
    strncpy(stLib.acLibName, HI_AWB_LIB_NAME, sizeof(HI_AWB_LIB_NAME));
    cmos_init_awb_exp_function(&stAwbRegister.stSnsExp);
    s32Ret = HI_MPI_AWB_SensorRegCallBack(IspDev, &stLib, IMX222_ID, &stAwbRegister);
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

    s32Ret = HI_MPI_ISP_SensorUnRegCallBack(IspDev, IMX222_ID);
    if (s32Ret)
    {
        printf("sensor unregister callback function failed!\n");
        return s32Ret;
    }
    
    stLib.s32Id = 0;
    strncpy(stLib.acLibName, HI_AE_LIB_NAME, sizeof(HI_AE_LIB_NAME));
    s32Ret = HI_MPI_AE_SensorUnRegCallBack(IspDev, &stLib, IMX222_ID);
    if (s32Ret)
    {
        printf("sensor unregister callback function to ae lib failed!\n");
        return s32Ret;
    }

    stLib.s32Id = 0;
    strncpy(stLib.acLibName, HI_AWB_LIB_NAME, sizeof(HI_AWB_LIB_NAME));
    s32Ret = HI_MPI_AWB_SensorUnRegCallBack(IspDev, &stLib, IMX222_ID);
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
