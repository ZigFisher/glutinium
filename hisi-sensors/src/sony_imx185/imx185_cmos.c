#if !defined(__IMX185_CMOS_H_)
#define __IMX185_CMOS_H_

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

#define IMX185_ID 185

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

#define SHS1_ADDR (0x220) 
#define GAIN_ADDR (0x214)
#define VMAX_ADDR (0x218)
#define HMAX_ADDR (0x21B)
#define INCREASE_LINES (1) /* make real fps less than stand fps because NVR require*/
#define VMAX_1080P30 (1125+INCREASE_LINES)
#define VMAX_1080P30_WDR (1125+INCREASE_LINES)
#define VMAX_1080P25_WDR (1320)
#define HMAX_1080P30_WDR (2200)
#define HMAX_1080P25_WDR (2250)
#define SENSOR_1080P_30FPS_MODE    (1) 

#define FULL_LINES_MAX  (0xFFFF)

static HI_BOOL bInit = HI_FALSE;
HI_BOOL bSensorInit = HI_FALSE;
static HI_U32 gu32FullLinesStd = VMAX_1080P30;
static HI_U32 gu32FullLines = VMAX_1080P30;
static HI_U32 gu32Hmax = HMAX_1080P30_WDR;

HI_U8 gu8SensorImageMode = SENSOR_1080P_30FPS_MODE;
WDR_MODE_E genSensorMode = WDR_MODE_NONE;

ISP_SNS_REGS_INFO_S g_stSnsRegsInfo = {0};
ISP_SNS_REGS_INFO_S g_stPreSnsRegsInfo = {0};

#define PATHLEN_MAX 256
#define CMOS_CFG_INI "imx185_cfg.ini"
static char pcName[PATHLEN_MAX] = "configs/imx185_cfg.ini";

/* AE default parameter and function */
/* Built-in WDR 30fps*/
static ISP_AE_ROUTE_S gstAERouteAttr = 
{
    3,

    {
        {5*16,  1024,   1},
        {70*16, 1024,   1},
        {70*16, 3938,   1}
    }
};

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
    pstAeSnsDft->stIntTimeAccu.f32Offset = 0.1083;  //3.85us

    pstAeSnsDft->stAgainAccu.enAccuType = AE_ACCURACY_TABLE;
    pstAeSnsDft->stAgainAccu.f32Accuracy = 0.3;

    pstAeSnsDft->stDgainAccu.enAccuType = AE_ACCURACY_TABLE;
    pstAeSnsDft->stDgainAccu.f32Accuracy = 0.3;

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
            
            pstAeSnsDft->u32MaxDgain = 16229;  
            pstAeSnsDft->u32MinDgain = 1024;
            pstAeSnsDft->u32MaxDgainTarget = g_AeDft[0].u32MaxDgainTarget;
            pstAeSnsDft->u32MinDgainTarget = g_AeDft[0].u32MinDgainTarget;

            pstAeSnsDft->u32ISPDgainShift = g_AeDft[0].u32ISPDgainShift;
            pstAeSnsDft->u32MinISPDgainTarget = g_AeDft[0].u32MinISPDgainTarget;
            pstAeSnsDft->u32MaxISPDgainTarget = g_AeDft[0].u32MaxISPDgainTarget;             
        break;  
        case WDR_MODE_BUILT_IN:
            pstAeSnsDft->au8HistThresh[0] = 0xC;
            pstAeSnsDft->au8HistThresh[1] = 0x18;
            pstAeSnsDft->au8HistThresh[2] = 0x60;
            pstAeSnsDft->au8HistThresh[3] = 0x80;
           
            pstAeSnsDft->u8AeCompensation = g_AeDft[1].u8AeCompensation;
            
            pstAeSnsDft->u32MaxIntTime = 70*16;  
            pstAeSnsDft->u32MinIntTime = 5*16;
            pstAeSnsDft->u32MaxIntTimeTarget = g_AeDft[1].u32MaxIntTimeTarget;
            pstAeSnsDft->u32MinIntTimeTarget = g_AeDft[1].u32MinIntTimeTarget;
            
            pstAeSnsDft->u32MaxAgain = 1719; /* Built-in WDR Again fix 4.5DB */
            //pstAeSnsDft->u32MaxAgain = 8134;
            pstAeSnsDft->u32MinAgain = 1719;
            pstAeSnsDft->u32MaxAgainTarget = g_AeDft[1].u32MaxAgainTarget;
            pstAeSnsDft->u32MinAgainTarget = g_AeDft[1].u32MinAgainTarget;
            
            pstAeSnsDft->u32MaxDgain = 3938;
            //pstAeSnsDft->u32MaxDgain = 1024;
            pstAeSnsDft->u32MinDgain = 1024;
            pstAeSnsDft->u32MaxDgainTarget = g_AeDft[1].u32MaxDgainTarget;
            pstAeSnsDft->u32MinDgainTarget = g_AeDft[1].u32MinDgainTarget;

            pstAeSnsDft->u32ISPDgainShift = g_AeDft[1].u32ISPDgainShift;
            pstAeSnsDft->u32MinISPDgainTarget = g_AeDft[1].u32MinISPDgainTarget;
            pstAeSnsDft->u32MaxISPDgainTarget = g_AeDft[1].u32MaxISPDgainTarget;

            memcpy(&pstAeSnsDft->stAERouteAttr, &gstAERouteAttr, sizeof(ISP_AE_ROUTE_S));
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
    pstAeSnsDft->stIntTimeAccu.f32Offset = 0.1083;  //3.85us

    pstAeSnsDft->stAgainAccu.enAccuType = AE_ACCURACY_TABLE;
    pstAeSnsDft->stAgainAccu.f32Accuracy = 0.3;

    pstAeSnsDft->stDgainAccu.enAccuType = AE_ACCURACY_TABLE;
    pstAeSnsDft->stDgainAccu.f32Accuracy = 0.3;
    
    pstAeSnsDft->u32ISPDgainShift = 8;
    pstAeSnsDft->u32MinISPDgainTarget = 1 << pstAeSnsDft->u32ISPDgainShift;
    pstAeSnsDft->u32MaxISPDgainTarget = 8 << pstAeSnsDft->u32ISPDgainShift; 
    
    switch(genSensorMode)
    {
        default:
        case WDR_MODE_NONE:   /*linear mode*/
            pstAeSnsDft->au8HistThresh[0] = 0xd;
            pstAeSnsDft->au8HistThresh[1] = 0x28;
            pstAeSnsDft->au8HistThresh[2] = 0x60;
            pstAeSnsDft->au8HistThresh[3] = 0x80;
            
            pstAeSnsDft->u8AeCompensation = 0x38;
            
            pstAeSnsDft->u32MaxIntTime = gu32FullLinesStd - 1;
            pstAeSnsDft->u32MinIntTime = 2;
            pstAeSnsDft->u32MaxIntTimeTarget = 65535;
            pstAeSnsDft->u32MinIntTimeTarget = 2;
            
            pstAeSnsDft->u32MaxAgain = 16229;
            pstAeSnsDft->u32MinAgain = 1024;
            pstAeSnsDft->u32MaxAgainTarget = pstAeSnsDft->u32MaxAgain;
            pstAeSnsDft->u32MinAgainTarget = pstAeSnsDft->u32MinAgain;
            
            pstAeSnsDft->u32MaxDgain = 16229;  
            pstAeSnsDft->u32MinDgain = 1024;
            pstAeSnsDft->u32MaxDgainTarget = pstAeSnsDft->u32MaxDgain;
            pstAeSnsDft->u32MinDgainTarget = pstAeSnsDft->u32MinDgain;        
        break;  
        case WDR_MODE_BUILT_IN:
            pstAeSnsDft->stIntTimeAccu.f32Accuracy = 16;
            
            pstAeSnsDft->au8HistThresh[0] = 0xC;
            pstAeSnsDft->au8HistThresh[1] = 0x18;
            pstAeSnsDft->au8HistThresh[2] = 0x60;
            pstAeSnsDft->au8HistThresh[3] = 0x80;
           
            pstAeSnsDft->u8AeCompensation = 0x40;
            
            pstAeSnsDft->u32MaxIntTime = 70*16;  
            pstAeSnsDft->u32MinIntTime = 5*16;
            pstAeSnsDft->u32MaxIntTimeTarget = pstAeSnsDft->u32MaxIntTime;
            pstAeSnsDft->u32MinIntTimeTarget = pstAeSnsDft->u32MinIntTime;
            
            pstAeSnsDft->u32MaxAgain = 1719; /* Built-in WDR Again fix 4.5DB */
            pstAeSnsDft->u32MinAgain = 1719;
            pstAeSnsDft->u32MaxAgainTarget = 1719;
            pstAeSnsDft->u32MinAgainTarget = pstAeSnsDft->u32MinAgain;
            
            pstAeSnsDft->u32MaxDgain = 3938;
            pstAeSnsDft->u32MinDgain = 1024;
            pstAeSnsDft->u32MaxDgainTarget = 3938;
            pstAeSnsDft->u32MinDgainTarget = pstAeSnsDft->u32MinDgain;

            memcpy(&pstAeSnsDft->stAERouteAttr, &gstAERouteAttr, sizeof(ISP_AE_ROUTE_S));

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
            gu32FullLinesStd = VMAX_1080P30_WDR;
            gu32Hmax = HMAX_1080P30_WDR;
            memcpy(&pstAeSnsDft->stAERouteAttr, &gstAERouteAttr, sizeof(ISP_AE_ROUTE_S));
            pstAeSnsDft->u32MaxIntTime = 70*16;
        }
        else if (25 == f32Fps)
        {
            gu32FullLinesStd = VMAX_1080P25_WDR;
            gu32Hmax = HMAX_1080P25_WDR;
            memcpy(&pstAeSnsDft->stAERouteAttr, &gstAERouteAttr, sizeof(ISP_AE_ROUTE_S));
            pstAeSnsDft->u32MaxIntTime = 70*16;
        }
        else
        {
            printf("Not support Fps: %f\n", f32Fps);
            return;                
        }
        gu32FullLinesStd = (gu32FullLinesStd > FULL_LINES_MAX) ? FULL_LINES_MAX : gu32FullLinesStd;
    }
    else if (WDR_MODE_NONE == genSensorMode)
    {
        if ((f32Fps <= 30) && (f32Fps >= 0.5))
        {
            gu32FullLinesStd = VMAX_1080P30*30/f32Fps;
        }
        else
        {
            printf("Not support Fps: %f\n", f32Fps);
            return;
        }
        gu32FullLinesStd = (gu32FullLinesStd > FULL_LINES_MAX) ? FULL_LINES_MAX : gu32FullLinesStd;
        pstAeSnsDft->u32MaxIntTime = gu32FullLinesStd - 2;
    }

    if (WDR_MODE_NONE == genSensorMode)
    {
        g_stSnsRegsInfo.astSspData[3].u32Data = (gu32FullLinesStd & 0xFF);/* VMAX[7:0] */
        g_stSnsRegsInfo.astSspData[4].u32Data = ((gu32FullLinesStd & 0xFF00) >> 8);/* VMAX[15:8] */
    }    
    else if (WDR_MODE_BUILT_IN == genSensorMode)
    {
        g_stSnsRegsInfo.astSspData[5].u32Data = (gu32FullLinesStd & 0xFF);/* VMAX[7:0] */
        g_stSnsRegsInfo.astSspData[6].u32Data = ((gu32FullLinesStd & 0xFF00) >> 8);/* VMAX[15:8] */    
        g_stSnsRegsInfo.astSspData[7].u32Data = (gu32Hmax & 0xFF);/* HMAX[7:0] */
        g_stSnsRegsInfo.astSspData[8].u32Data = ((gu32Hmax & 0xFF00) >> 8);/* HMAX[15:8] */    
    }
    else
    {}

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
        gu32FullLines = (u32FullLines > FULL_LINES_MAX) ? FULL_LINES_MAX : u32FullLines;
        pstAeSnsDft->u32FullLines = gu32FullLines;
        g_stSnsRegsInfo.astSspData[3].u32Data = (gu32FullLines & 0xFF);
        g_stSnsRegsInfo.astSspData[4].u32Data = ((gu32FullLines & 0xFF00) >> 8);
    }
    else if(WDR_MODE_BUILT_IN == genSensorMode)
    {
        pstAeSnsDft->u32FullLines = gu32FullLinesStd;
        return; 
    }
    else
    {}
    
    pstAeSnsDft->u32MaxIntTime = gu32FullLines - 2;
    
    return;
}

/* while isp notify ae to update sensor regs, ae call these funcs. */
static HI_VOID cmos_inttime_update(HI_U32 u32IntTime)
{
    HI_U32 u32Value, u32ValueLong; 

    u32Value = gu32FullLines - 1 - u32IntTime;/*integration time = 1 frame period - (SHS1+1)*(1H period) + toffset*/
    g_stSnsRegsInfo.astSspData[0].u32Data = (u32Value & 0xFF);
    g_stSnsRegsInfo.astSspData[1].u32Data = ((u32Value & 0xFF00) >> 8); 

    /* WDR mode, u32IntTime is long frame */
    if (WDR_MODE_BUILT_IN == genSensorMode)
    {
        u32ValueLong = gu32FullLines - 1 - u32IntTime;
        u32Value = gu32FullLines - 1 - u32IntTime/16;
        g_stSnsRegsInfo.astSspData[0].u32Data = (u32Value & 0xFF);
        g_stSnsRegsInfo.astSspData[1].u32Data = ((u32Value & 0xFF00) >> 8);  
        g_stSnsRegsInfo.astSspData[3].u32Data = (u32ValueLong & 0xFF);
        g_stSnsRegsInfo.astSspData[4].u32Data = ((u32ValueLong & 0xFF00) >> 8);
    }

    return;
}


static HI_U32 ad_gain_table[81]=
{    
    1024,1060,1097,1136,1176,1217,1260,1304,1350,1397,1446,1497,1550,1604,1661,1719,
    1780,1842,1907,1974,2043,2115,2189,2266,2346,2428,2514,2602,2693,2788,2886,2987,
    3092,3201,3314,3430,3551,3675,3805,3938,4077,4220,4368,4522,4681,4845,5015,5192,
    5374,5563,5758,5961,6170,6387,6611,6844,7084,7333,7591,7858,8134,8420,8716,9022,
    9339,9667,10007,10359,10723,11099,11489,11893,12311,12744,13192,13655,14135,14632,15146,15678,
    16229,
};

static HI_VOID cmos_again_calc_table(HI_U32 *pu32AgainLin, HI_U32 *pu32AgainDb)
{
    int i;

    if((HI_NULL == pu32AgainLin) ||(HI_NULL == pu32AgainDb))
    {
        printf("null pointer when get ae sensor gain info value!\n");
        return;
    }

    if (*pu32AgainLin >= ad_gain_table[80])
    {
         *pu32AgainLin = ad_gain_table[80];
         *pu32AgainDb = 80;
         return ;
    }
    
    for (i = 1; i < 81; i++)
    {
        if (*pu32AgainLin < ad_gain_table[i])
        {
            *pu32AgainLin = ad_gain_table[i - 1];
            *pu32AgainDb = i - 1;
            break;
        }
    }
    
    return;
}

static HI_VOID cmos_dgain_calc_table(HI_U32 *pu32DgainLin, HI_U32 *pu32DgainDb)
{
    int i;

    if((HI_NULL == pu32DgainLin) ||(HI_NULL == pu32DgainDb))
    {
        printf("null pointer when get ae sensor gain info value!\n");
        return;
    }

    if (*pu32DgainLin >= ad_gain_table[80])
    {
         *pu32DgainLin = ad_gain_table[80];
         *pu32DgainDb = 80;
         return ;
    }
    
    for (i = 1; i < 81; i++)
    {
        if (*pu32DgainLin < ad_gain_table[i])
        {
            *pu32DgainLin = ad_gain_table[i - 1];
            *pu32DgainDb = i - 1;
            break;
        }
    }

    return;
}

static HI_VOID cmos_gains_update(HI_U32 u32Again, HI_U32 u32Dgain)
{  

    HI_U32 u32Tmp = u32Again + u32Dgain;

    if (WDR_MODE_BUILT_IN == genSensorMode)
    {
        u32Tmp = u32Dgain;
    }

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

static AWB_CCM_S g_stAwbCcm =
{
 
   4900,
   {
      0x01d8, 0x80ec, 0x0014,
      0x803e, 0x016f, 0x8031,
      0x003e, 0x817d, 0x023f
   },
   3770,
   {
      0x01eb, 0x80f9, 0x000d,
      0x8048, 0x0151, 0x8009,
      0x006b, 0x81af, 0x0243
   },
   
   2640,
   {     
      0x01d4, 0x8053, 0x8081,
      0x805b, 0x017a, 0x801f, 
      0x0038, 0x814e, 0x0215
   }
};

static AWB_AGC_TABLE_S g_stAwbAgcTable =
{
    /* bvalid */
    1,

    /* saturation */ 
    {0x7a,0x7a,0x78,0x76,0x70,0x68,0x40,0x40,0x40,0x40,0x40,0x3a,0x3a,0x3a,0x3a,0x3a}
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

    pstAwbSnsDft->au16GainOffset[0] = 0x1D0;
    pstAwbSnsDft->au16GainOffset[1] = 0x100;
    pstAwbSnsDft->au16GainOffset[2] = 0x100;
    pstAwbSnsDft->au16GainOffset[3] = 0x1F4;

    pstAwbSnsDft->as32WbPara[0] = 21;
    pstAwbSnsDft->as32WbPara[1] = 147;
    pstAwbSnsDft->as32WbPara[2] = -87;
    pstAwbSnsDft->as32WbPara[3] = 179332;
    pstAwbSnsDft->as32WbPara[4] = 128;
    pstAwbSnsDft->as32WbPara[5] = -130261;

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
    {0x38,0x38,0x36,0x32,0x2b,0x26,0x20,0x1c,0x1a,0x16,0x14,0x12,0x12,0x12,0x12,0x12},
        
    /* sharpen_alt_ud */
    {0x32,0x32,0x30,0x30,0x30,0x2a,0x2a,0x2a,0x26,0x26,0x20,0x16,0x12,0x12,0x12,0x12},
        
    /* snr_thresh Max=0x54 */
    {0x08,0x0a,0x0f,0x12,0x16,0x1a,0x22,0x28,0x2e,0x36,0x3a,0x40,0x40,0x40,0x40,0x40},
        
    /* demosaic_lum_thresh */
    {0x58,0x58,0x56,0x4e,0x46,0x3a,0x30,0x28,0x24,0x20,0x20,0x20,0x20,0x20,0x20,0x20},
        
    /* demosaic_np_offset */
    {0x00,0x0a,0x12,0x1a,0x20,0x28,0x30,0x32,0x34,0x36,0x38,0x38,0x38,0x38,0x38,0x38},
        
    /* ge_strength */
    {0x55,0x55,0x55,0x55,0x55,0x55,0x37,0x37,0x37,0x35,0x35,0x35,0x35,0x35,0x35,0x35},

    /* rgb_sharpen_strength */
    {0x58,0x58,0x56,0x4e,0x46,0x36,0x36,0x34,0x34,0x32,0x30,0x20,0x20,0x20,0x20,0x20}
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
    {          
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x8, 0xF, 0x13, 0x15, 0x18, 0x1A, 0x1C, 0x1D,
    0x1E, 0x20, 0x21, 0x22, 0x23, 0x23, 0x24, 0x25, 0x26, 0x26, 0x26, 0x27, 0x28, 0x28, 0x29, 0x29, 
    0x2A, 0x2A, 0x2B, 0x2B, 0x2C, 0x2C, 0x2C, 0x2D, 0x2D, 0x2D, 0x2E, 0x2E, 0x2E, 0x2F, 0x2F, 0x2F, 
    0x2F, 0x30, 0x30, 0x30, 0x31, 0x31, 0x31, 0x31, 0x31, 0x32, 0x32, 0x32, 0x32, 0x33, 0x33, 0x33, 
    0x33, 0x33, 0x34, 0x34, 0x34, 0x34, 0x34, 0x35, 0x35, 0x35, 0x35, 0x35, 0x35, 0x36, 0x36, 0x36, 
    0x36, 0x36, 0x36, 0x36, 0x37, 0x37, 0x37, 0x37, 0x37, 0x37, 0x38, 0x38, 0x38, 0x38, 0x38, 0x38, 
    0x38, 0x38, 0x39, 0x39, 0x39, 0x39, 0x39, 0x39, 0x39, 0x39, 0x3A, 0x3A, 0x3A, 0x3A, 0x3A, 0x3A, 
    0x3A, 0x3A, 0x3A, 0x3B, 0x3B, 0x3B, 0x3B, 0x3B, 0x3B, 0x3B, 0x3B, 0x3B, 0x3B, 0x3C, 0x3C, 0x3C 
    },

    /* demosaic_weight_lut */
    {
    0x0, 0x0,   0x8, 0xF, 0x13, 0x15, 0x18, 0x1A, 0x1C, 0x1D,
    0x1E, 0x20, 0x21, 0x22, 0x23, 0x23, 0x24, 0x25, 0x26, 0x26, 0x26, 0x27, 0x28, 0x28, 0x29, 0x29, 
    0x2A, 0x2A, 0x2B, 0x2B, 0x2C, 0x2C, 0x2C, 0x2D, 0x2D, 0x2D, 0x2E, 0x2E, 0x2E, 0x2F, 0x2F, 0x2F, 
    0x2F, 0x30, 0x30, 0x30, 0x31, 0x31, 0x31, 0x31, 0x31, 0x32, 0x32, 0x32, 0x32, 0x33, 0x33, 0x33, 
    0x33, 0x33, 0x34, 0x34, 0x34, 0x34, 0x34, 0x35, 0x35, 0x35, 0x35, 0x35, 0x35, 0x36, 0x36, 0x36, 
    0x36, 0x36, 0x36, 0x36, 0x37, 0x37, 0x37, 0x37, 0x37, 0x37, 0x38, 0x38, 0x38, 0x38, 0x38, 0x38, 
    0x38, 0x38, 0x39, 0x39, 0x39, 0x39, 0x39, 0x39, 0x39, 0x39, 0x3A, 0x3A, 0x3A, 0x3A, 0x3A, 0x3A, 
    0x3A, 0x3A, 0x3A, 0x3B, 0x3B, 0x3B, 0x3B, 0x3B, 0x3B, 0x3B, 0x3B, 0x3B, 0x3B, 0x3C, 0x3C, 0x3C,
    0x3C, 0x3C, 0x3C,0x3C, 0x3C, 0x3C
    }        
    
};

static ISP_CMOS_DEMOSAIC_S g_stIspDemosaic =
{
    /* bvalid */
    1,
    
    /*vh_slope*/
    0xac,

    /*aa_slope*/
    0xa3,

    /*va_slope*/
    0xa0,

    /*uu_slope*/
    0xa0,

    /*sat_slope*/
    0x5d,

    /*ac_slope*/
    0xa0,
    
    /*fc_slope*/
    0x8a,

    /*vh_thresh*/
    0x0,

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
    255,  
    
    /*lut_strength*/  
    127, 
    
    /*lut_magnitude*/   
    8      
};

static ISP_CMOS_GAMMA_S g_stIspGamma =
{
    /* bvalid */
    1,

    {
        0, 180, 320, 426, 516, 590, 660, 730, 786, 844, 896, 946, 994, 1040, 1090, 1130, 1170, 1210, 1248,
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
        3994, 4002, 4006, 4010, 4018, 4022, 4032, 4038, 4046, 4050, 4056, 4062, 4072, 4076, 4084, 4090, 4095
    }
};

static ISP_CMOS_GAMMA_S g_stIspGammaBuiltInWDR =
{
    /* bvalid */
    1,
    
#if 0    
    {
        0,   1,   2,   4,   8,  12,  17,  23,  30,  38,  47,  57,  68,  79,  92, 105, 120, 133, 147, 161, 176, 192, 209, 226, 243, 260, 278, 296, 315, 333, 351, 370, 390, 410, 431, 453, 474, 494, 515, 536, 558, 580, 602, 623, 644, 665, 686, 708, 730, 751, 773, 795, 818, 840, 862, 884, 907, 929, 951, 974, 998,1024,1051,1073,1096,1117,1139,1159,1181,1202,1223,1243,1261,1275,1293,1313,1332,1351,1371,1389,1408,1427,1446,1464,1482,1499,1516,1533,1549,1567,1583,1600,1616,1633,1650,1667,1683,1700,1716,1732,1749,1766,1782,1798,1815,1831,1847,1863,1880,1896,1912,1928,1945,1961,1977,1993,2009,2025,2041,2057,2073,2089,2104,2121,2137,2153,2168,2184,2200,2216,2231,2248,2263,2278,2294,2310,2326,2341,2357,2373,2388,2403,2419,2434,2450,2466,2481,2496,2512,2527,2543,2558,2573,2589,2604,2619,2635,2650,2665,2680,2696,2711,2726,2741,2757,2771,2787,2801,2817,2832,2847,2862,2877,2892,2907,2922,2937,2952,2967,2982,2997,3012,3027,3041,3057,3071,3086,3101,3116,3130,3145,3160,3175,3190,3204,3219,3234,3248,3263,3278,3293,3307,3322,3337,3351,3365,3380,3394,3409,3424,3438,3453,3468,3482,3497,3511,3525,3540,3554,3569,3584,3598,3612,3626,3641,3655,3670,3684,3699,3713,3727,3742,3756,3770,3784,3799,3813,3827,3841,3856,3870,3884,3898,3912,3927,3941,3955,3969,3983,3997,4011,4026,4039,4054,4068,4082,4095
    }
#else  /*higher  contrast*/
    {
        0,1,2,4,8,12,17,23,30,38,47,57,68,79,92,105,120,133,147,161,176,192,209,226,243,260,278,296,317,340,365,390,416,440,466,491,517,538,561,584,607,631,656,680,705,730,756,784,812,835,858,882,908,934,958,982,1008,1036,1064,1092,1119,1143,1167,1192,1218,1243,1269,1296,1323,1351,1379,1408,1434,1457,1481,1507,1531,1554,1579,1603,1628,1656,1683,1708,1732,1756,1780,1804,1829,1854,1877,1901,1926,1952,1979,2003,2024,2042,2062,2084,2106,2128,2147,2168,2191,2214,2233,2256,2278,2296,2314,2335,2352,2373,2391,2412,2431,2451,2472,2492,2513,2531,2547,2566,2581,2601,2616,2632,2652,2668,2688,2705,2721,2742,2759,2779,2796,2812,2826,2842,2857,2872,2888,2903,2920,2934,2951,2967,2983,3000,3015,3033,3048,3065,3080,3091,3105,3118,3130,3145,3156,3171,3184,3197,3213,3224,3240,3252,3267,3281,3295,3310,3323,3335,3347,3361,3372,3383,3397,3409,3421,3432,3447,3459,3470,3482,3497,3509,3521,3534,3548,3560,3572,3580,3592,3602,3613,3625,3633,3646,3657,3667,3679,3688,3701,3709,3719,3727,3736,3745,3754,3764,3773,3781,3791,3798,3806,3816,3823,3833,3840,3847,3858,3865,3872,3879,3888,3897,3904,3911,3919,3926,3933,3940,3948,3955,3962,3970,3973,3981,3988,3996,4003,4011,4018,4026,4032,4037,4045,4053,4057,4064,4072,4076,4084,4088,4095        
        //0,1,2,4,8,12,17,23,30,38,47,57,68,79,92,105,120,133,147,161,176,192,209,226,243,260,278,296,317,340,365,390,416,440,466,491,517,538,561,584,607,631,656,680,705,730,756,784,812,835,858,882,908,934,958,982,1008,1036,1064,1092,1119,1143,1167,1192,1218,1243,1269,1294,1320,1346,1372,1398,1424,1450,1476,1502,1528,1554,1580,1607,1633,1658,1684,1710,1735,1761,1786,1811,1836,1860,1884,1908,1932,1956,1979,2002,2024,2046,2068,2090,2112,2133,2154,2175,2196,2217,2237,2258,2278,2298,2318,2337,2357,2376,2395,2414,2433,2451,2469,2488,2505,2523,2541,2558,2575,2592,2609,2626,2642,2658,2674,2690,2705,2720,2735,2750,2765,2779,2793,2807,2821,2835,2848,2861,2874,2887,2900,2913,2925,2937,2950,2962,2974,2986,2998,3009,3021,3033,3044,3056,3067,3078,3088,3099,3109,3119,3129,3139,3148,3158,3168,3177,3187,3197,3207,3217,3227,3238,3248,3259,3270,3281,3292,3303,3313,3324,3335,3346,3357,3368,3379,3389,3400,3410,3421,3431,3441,3451,3461,3471,3481,3491,3501,3511,3521,3531,3541,3552,3562,3572,3583,3593,3604,3615,3625,3636,3646,3657,3668,3679,3689,3700,3711,3721,3732,3743,3753,3764,3774,3784,3795,3805,3816,3826,3837,3847,3858,3869,3880,3891,3902,3913,3925,3937,3949,3961,3973,3985,3997,4009,4022,4034,4046,4058,4071,4083,4095
    }
#endif
};

static ISP_CMOS_GAMMAFE_S g_stGammafeBuiltInWDR = 
{
    /* bvalid */
    1,

    /* gamma_fe0 */
    {
        //0,4096,45056,51200,55296,57344,57647,57951,58254,58558,58861,59164,59468,59771,60075,60378,60681,60985,61288,61592,61895,62199,62502,62805,63109,63412,63716,64019,64322,64626,64929,65233,65535
        0, 2048, 34816, 35840, 36864, 37888, 38912, 39936, 40960, 41984, 43008, 44032, 45056, 46080, 47104, 48128, 49152, 50176, 51200, 52224, 53248, 54272, 55296, 56320, 57344, 58368, 59392, 60416, 61440, 62464, 63488, 64512, 65535
    },

    /* gamma_fe1 */
    {
        //0,63,127,192,259,326,395,466,538,612,687,765,844,926,1010,1097,1187,1198,1210,1210,1222,1233,1245,1257,1257,1268,1280,1292,1304,1304,1316,1328,1340,1352,1352,1365,1377,1389,1402,1402,1414,1427,1439,1452,1452,1465,1478,1491,1504,1504,1517,1530,1543,1556,1556,1570,1583,1597,1610,1610,1624,1638,1652,1666,1666,1680,1694,1709,1723,1723,1737,1752,1767,1782,1782,1797,1812,1827,1842,1842,1857,1873,1889,1905,1905,1920,1937,1953,1969,1969,1986,2002,2019,2036,2036,2053,2071,2088,2106,2106,2124,2142,2160,2179,2179,2198,2217,2236,2255,2255,2275,2295,2316,2336,2336,2357,2378,2400,2422,2422,2444,2467,2490,2513,2513,2537,2561,2586,2612,2612,2638,2665,2692,2720,2720,2749,2779,2810,2841,2841,2874,2908,2944,2981,2981,3021,3062,3106,3154,3154,3205,3262,3327,3404,3404,3504,3746,3988,4088,4088,4165,4230,4287,4338,4338,4386,4430,4471,4511,4511,4548,4584,4618,4651,4651,4683,4713,4854,5003,5114,5217,5332,5421,5506,5603,5681,5755,5840,5909,5975,6053,6115,6176,6247,6305,6362,6428,6482,6535,6597,6648,6727,6805,6880,6954,7026,7097,7166,7233,7300,7365,7429,7492,7554,7615,7675,7734,7850,7962,8072,8178,8283,8385,8485,8582,9196,9748,10252,10721,11160,11574,11968,12343,12703,13049,13381,13704,14017,17978,21171,23860,26230,28376,30344,32182,33907,38543,43929,48679,52963,56914,60593,64038,65535,65535,65535,65535
        0, 127, 259, 395, 538, 687, 844, 1010, 1187, 1198, 1210, 1222, 1233, 1245, 1257, 1268, 1280, 1292, 1304, 1316, 1328, 1340, 1352, 1365, 1377, 1389, 1402, 1414, 1427, 1439, 1452, 1465, 1478, 1491, 1504, 1517, 1530, 1543, 1556, 1570, 1583, 1597, 1610, 1624, 1638, 1652, 1666, 1680, 1694, 1709, 1723, 1737, 1752, 1767, 1782, 1797, 1812, 1827, 1842, 1858, 1873, 1889, 1905, 1920, 1937, 1953, 1969, 1986, 2002, 2019, 2036, 2053, 2071, 2088, 2106, 2124, 2142, 2160, 2179, 2198, 2217, 2236, 2255, 2275, 2295, 2316, 2336, 2357, 2378, 2400, 2422, 2444, 2467, 2490, 2513, 2537, 2561, 2586, 2612, 2638, 2665, 2692, 2720, 2749, 2779, 2810, 2841, 2874, 2908, 2944, 2981, 3021, 3062, 3106, 3154, 3205, 3262, 3327, 3404, 3504, 3746, 3988, 4088, 4165, 4230, 4287, 4338, 4386, 4430, 4472, 4511, 4548, 4584, 4618, 4651, 4683, 4713, 5421, 5909, 6305, 6648, 6954, 7234, 7492, 7734, 7962, 8179, 8385, 8582, 8772, 8955, 9131, 9302, 9468, 9630, 9787, 9939, 10089, 10235, 10377, 10517, 10654, 10788, 10919, 11049, 11176, 11300, 11423, 11544, 11663, 11781, 11896, 12010, 12123, 12234, 12343, 12451, 12558, 12664, 12768, 12871, 12973, 13074, 13174, 13272, 13370, 13467, 13563, 13657, 13751, 13844, 13937, 14028, 15373, 16596, 17713, 18746, 19713, 20624, 21489, 22314, 23101, 23860, 24591, 25297, 25981, 26644, 27289, 27916, 28528, 29123, 29707, 30277, 30835, 31382, 31919, 32445, 32962, 33469, 33968, 34460, 36321, 38110, 39810, 41434, 42990, 44487, 45930, 47326, 48678, 49991, 51268, 52511, 53724, 54908, 56065, 57196, 58305, 59391, 60457, 61502, 62530, 63540, 64532, 65509, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535
        //0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1026,1451,1777,2052,2294,2513,2714,2902,3078,3244,3402,3554,3699,3838,3973,4103,4230,4352,4472,4588,4701,4812,4920,5026,5129,5231,5331,5428,5524,5619,5712,5803,5893,5982,6069,6155,6240,6324,6407,6488,6569,6648,6727,6805,6882,6958,7033,7107,7181,7254,7326,7398,7468,7539,7608,7677,7745,7813,7880,7946,8012,8078,8143,8207,8271,8334,8397,8460,8522,8583,8644,8705,8765,8825,8884,8943,9002,9060,9118,9176,9233,9290,9346,9402,9458,9514,9569,9624,9678,9732,9786,9840,9893,9946,9999,10051,10104,10156,10207,10259,10310,10361,10411,10462,10512,10562,10612,10661,10710,10759,10808,10857,10905,11652,12353,13017,13648,14252,14831,15388,15926,16446,16950,17440,17916,18380,18833,19274,19706,20129,20543,20949,21347,21738,22122,22499,22870,23235,23595,23949,24298,24642,24982,25316,25647,25973,26295,26613,26928,27239,27546,27850,28151,28448,28743,29034,29323,29608,29891,30172,30450,30725,30998,31268,31536,31802,32066,32327,32587,33604,34592,35552,36487,37399,38289,39158,40009,40842,41659,42459,43245,44017,44776,45521,46255,46978,47689,48390,49081,49763,50435,51098,51753,52399,53038,53669,54293,54910,55520,56123,56720,57311,57895,58474,59047,59615,60177,60734,61286,61833,62376,62913,63446,63975,64499,65019,65535,65535,65535,65535,65535,65535,65535,65535,65535,65535,65535,65535,65535,65535,65535,65535,65535,65535,65535,65535,65535,65535,65535,65535,65535
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
        default:
        case WDR_MODE_NONE:
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
        case WDR_MODE_BUILT_IN:
            pstDef->stDrc.bEnable               = HI_TRUE;
            pstDef->stDrc.u32BlackLevel         = 0x00;
            pstDef->stDrc.u32WhiteLevel         = 0xFFF; 
            pstDef->stDrc.u32SlopeMax           = 0x38;
            pstDef->stDrc.u32SlopeMin           = 0x00;
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
            pstBlackLevel->au16BlackLevel[i] = 0xF0;/*10bit : 0x3c*/
        }
    }
    else if (WDR_MODE_BUILT_IN == genSensorMode)
    {
        for (i=0; i<4; i++)
        {
            pstBlackLevel->au16BlackLevel[i] = 0xEA;/*10bit : 0x3c*/
        }
    }
    else
    {}

    return 0;    
}

HI_VOID cmos_set_pixel_detect(HI_BOOL bEnable)
{
    if (WDR_MODE_BUILT_IN == genSensorMode)
    {
        return;
    }
    
    if (bEnable) /* setup for ISP pixel calibration mode */
    {
        /* Sensor must be programmed for slow frame rate (5 fps and below) */
        /* change frame rate to 5 fps by setting 1 frame length = 1125 * (30/5) */
        sensor_write_register(VMAX_ADDR, 0x5E);
        sensor_write_register(VMAX_ADDR + 1, 0x1A);

        /* Analog and Digital gains both must be programmed for their minimum values */
        sensor_write_register(SHS1_ADDR, 0x2);
        sensor_write_register(SHS1_ADDR + 1, 0);
        sensor_write_register(GAIN_ADDR, 0x00);
    }
    else /* setup for ISP 'normal mode' */
    {
        sensor_write_register(VMAX_ADDR, (gu32FullLinesStd & 0xff)); //30fps
        sensor_write_register(VMAX_ADDR + 1, (gu32FullLinesStd & 0xff00)>>8);
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
            gu32FullLinesStd = VMAX_1080P30;
            genSensorMode = WDR_MODE_NONE;
            printf("linear mode\n");
        break;

        case WDR_MODE_BUILT_IN:
            gu32FullLinesStd = VMAX_1080P30_WDR;
            genSensorMode = WDR_MODE_BUILT_IN;
            printf("Built-in WDR mode\n");
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
        g_stSnsRegsInfo.enSnsType = ISP_SNS_SSP_TYPE;
        g_stSnsRegsInfo.u8Cfg2ValidDelayMax = 2;        
        g_stSnsRegsInfo.u32RegNum = 5;        
        if (WDR_MODE_BUILT_IN == genSensorMode)
        {
            g_stSnsRegsInfo.u32RegNum += 4;
        }
        
        for (i=0; i<g_stSnsRegsInfo.u32RegNum; i++)
        {
            g_stSnsRegsInfo.astSspData[i].bUpdate = HI_TRUE;
            g_stSnsRegsInfo.astSspData[i].u32DevAddr = 0x02;
            g_stSnsRegsInfo.astSspData[i].u32DevAddrByteNum = 1;
            g_stSnsRegsInfo.astSspData[i].u32RegAddrByteNum = 1;
            g_stSnsRegsInfo.astSspData[i].u32DataByteNum = 1;
        }        
        g_stSnsRegsInfo.astSspData[0].u8DelayFrmNum = 0;
        g_stSnsRegsInfo.astSspData[0].u32RegAddr = 0x20;//shutter SHS1[7:0]
        g_stSnsRegsInfo.astSspData[1].u8DelayFrmNum = 0;
        g_stSnsRegsInfo.astSspData[1].u32RegAddr = 0x21;//shutter SHS1[15:8]
        g_stSnsRegsInfo.astSspData[2].u8DelayFrmNum = 1;//make shutter and gain effective at the same time
        g_stSnsRegsInfo.astSspData[2].u32RegAddr = 0x14;//gain     
        g_stSnsRegsInfo.astSspData[3].u8DelayFrmNum = 0;
        g_stSnsRegsInfo.astSspData[3].u32RegAddr = VMAX_ADDR;
        g_stSnsRegsInfo.astSspData[4].u8DelayFrmNum = 0;
        g_stSnsRegsInfo.astSspData[4].u32RegAddr = VMAX_ADDR + 1;
      
        if (WDR_MODE_BUILT_IN == genSensorMode)
        {
            g_stSnsRegsInfo.astSspData[3].u8DelayFrmNum = 0;
            g_stSnsRegsInfo.astSspData[3].u32RegAddr = 0x23;
            g_stSnsRegsInfo.astSspData[4].u8DelayFrmNum = 0;
            g_stSnsRegsInfo.astSspData[4].u32RegAddr = 0x24;
            g_stSnsRegsInfo.astSspData[5].u8DelayFrmNum = 0;
            g_stSnsRegsInfo.astSspData[5].u32RegAddr = VMAX_ADDR;
            g_stSnsRegsInfo.astSspData[6].u8DelayFrmNum = 0;
            g_stSnsRegsInfo.astSspData[6].u32RegAddr = VMAX_ADDR + 1;    
            g_stSnsRegsInfo.astSspData[7].u8DelayFrmNum = 0;
            g_stSnsRegsInfo.astSspData[7].u32RegAddr = HMAX_ADDR;
            g_stSnsRegsInfo.astSspData[8].u8DelayFrmNum = 0;
            g_stSnsRegsInfo.astSspData[8].u32RegAddr = HMAX_ADDR + 1;   
        }

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
    bInit = HI_FALSE;
    bSensorInit = HI_FALSE;
    gu32FullLinesStd = VMAX_1080P30;
    gu32FullLines = VMAX_1080P30;    
    genSensorMode = WDR_MODE_NONE;
    
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

    pstSensorExpFunc->pfn_cmos_sensor_init                  = sensor_init;
    pstSensorExpFunc->pfn_cmos_sensor_exit                  = sensor_exit;
    pstSensorExpFunc->pfn_cmos_sensor_global_init           = sensor_global_init;
    pstSensorExpFunc->pfn_cmos_set_image_mode               = cmos_set_image_mode;
    pstSensorExpFunc->pfn_cmos_set_wdr_mode                 = cmos_set_wdr_mode;
    pstSensorExpFunc->pfn_cmos_get_isp_default              = cmos_get_isp_default;
    pstSensorExpFunc->pfn_cmos_get_isp_black_level          = cmos_get_isp_black_level;
    pstSensorExpFunc->pfn_cmos_set_pixel_detect             = cmos_set_pixel_detect;
    pstSensorExpFunc->pfn_cmos_get_sns_reg_info             = cmos_get_sns_regs_info;

    return 0;
}

/****************************************************************************
 * callback structure                                                       *
 ****************************************************************************/

int sensor_register_callback(void)
{
    HI_S32 s32Ret;
    ALG_LIB_S stLib;
    ISP_DEV IspDev=0;
    ISP_SENSOR_REGISTER_S stIspRegister;
    AE_SENSOR_REGISTER_S  stAeRegister;
    AWB_SENSOR_REGISTER_S stAwbRegister;

    cmos_init_sensor_exp_function(&stIspRegister.stSnsExp);
    s32Ret = HI_MPI_ISP_SensorRegCallBack(IspDev, IMX185_ID, &stIspRegister);
    if (s32Ret)
    {
        printf("sensor register callback function failed!\n");
        return s32Ret;
    }
    
    stLib.s32Id = 0;
    strncpy(stLib.acLibName, HI_AE_LIB_NAME, sizeof(HI_AE_LIB_NAME));
    cmos_init_ae_exp_function(&stAeRegister.stSnsExp);
    s32Ret = HI_MPI_AE_SensorRegCallBack(IspDev, &stLib, IMX185_ID, &stAeRegister);
    if (s32Ret)
    {
        printf("sensor register callback function to ae lib failed!\n");
        return s32Ret;
    }

    stLib.s32Id = 0;
    strncpy(stLib.acLibName, HI_AWB_LIB_NAME, sizeof(HI_AWB_LIB_NAME));
    cmos_init_awb_exp_function(&stAwbRegister.stSnsExp);
    s32Ret = HI_MPI_AWB_SensorRegCallBack(IspDev, &stLib, IMX185_ID, &stAwbRegister);
    if (s32Ret)
    {
        printf("sensor register callback function to awb lib failed!\n");
        return s32Ret;
    }
    
    return 0;
}

int sensor_unregister_callback(void)
{
    HI_S32 s32Ret;
    ALG_LIB_S stLib;
    ISP_DEV IspDev=0;

    s32Ret = HI_MPI_ISP_SensorUnRegCallBack(IspDev, IMX185_ID);
    if (s32Ret)
    {
        printf("sensor unregister callback function failed!\n");
        return s32Ret;
    }
    
    stLib.s32Id = 0;
    strncpy(stLib.acLibName, HI_AE_LIB_NAME, sizeof(HI_AE_LIB_NAME));
    s32Ret = HI_MPI_AE_SensorUnRegCallBack(IspDev, &stLib, IMX185_ID);
    if (s32Ret)
    {
        printf("sensor unregister callback function to ae lib failed!\n");
        return s32Ret;
    }

    stLib.s32Id = 0;
    strncpy(stLib.acLibName, HI_AWB_LIB_NAME, sizeof(HI_AWB_LIB_NAME));
    s32Ret = HI_MPI_AWB_SensorUnRegCallBack(IspDev, &stLib, IMX185_ID);
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
