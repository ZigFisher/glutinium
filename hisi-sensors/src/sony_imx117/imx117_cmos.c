#if !defined(__IMX117_CMOS_H_)
#define __IMX117_CMOS_H_

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
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

#define IMX117_ID 117

// usefull macro

#define FULL_LINES_MAX  (0xFFFF)

#define HIGH_8BITS(x) ((x & 0xff00) >> 8)
#define LOW_8BITS(x) (x & 0x00ff)
#ifndef MAX
#define MAX(a, b) (((a) < (b)) ?  (b) : (a))
#endif

#ifndef MIN
#define MIN(a, b) (((a) > (b)) ?  (b) : (a))
#endif


#define PWM_HSYNC                   (5     )
#define PWM_VSYNC                   (6     )

// sensor register addr
#define SHR_L                       (0x000b)
#define SHR_H                       (0x000c)
#define PGC_L                       (0x0009)
#define PGC_H                       (0x000a)
#define APGC01_L                    (0x0352)
#define APGC01_H                    (0x0353)
#define APGC02_L                    (0x0356)
#define APGC02_H                    (0x0357)
#define SVR_L                       (0x000d)
#define SVR_H                       (0x000e)
                                    
#define VMAX_1080P60                (2600  )
                                    
// mode of image                    
#define SENSOR_1080P_60FPS_MODE     (0     )
#define SENSOR_720P_120FPS_MODE     (1     )
#define SENSOR_VGA_240FPS_MODE      (2     )
#define SENSOR_1080P_50FPS_MODE     (3     )

typedef struct hiIMX117_FRAME_ARCH_S {
    HI_U32 hsync_div;
    HI_U32 pixel_clk;
} IMX117_FRAME_ARCH_S;

typedef struct hiIMX117_SHUTTER_INFO_S {
    HI_U32 min_line;
    HI_U32 max_line;
    HI_U32 full_lines;
    HI_U32 svr;
} IMX117_SHUTTER_INFO_S;

extern IMX117_FRAME_ARCH_S stImx117FrmAch[];


// the min sht lines of 720P mode was difference from datasheet SHR range..
// ...because of the accuracy of vsync pulse

IMX117_SHUTTER_INFO_S g_stImx117ShtInfo[4] = {
    { 4, 2590, 2600, 0 },
    { 2, 1044, 1048, 0 },   
    { 2, 821,  825,  0 },
    { 4, 2390, 2400, 0 }
};


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
 * extern function reference                                                *
 ****************************************************************************/
extern int IMX117_pwmWrite(int pwmChn, unsigned int period, unsigned int duty);



static HI_BOOL bInit = HI_FALSE;
HI_BOOL bSensorInit = HI_FALSE;
static HI_U32 gu32FullLinesStd = VMAX_1080P60;

WDR_MODE_E genSensorMode = WDR_MODE_NONE;



HI_U8 gu8SensorImageMode = SENSOR_1080P_60FPS_MODE;

ISP_SNS_REGS_INFO_S g_stSnsRegsInfo = {0};
ISP_SNS_REGS_INFO_S g_stPreSnsRegsInfo = {0};

#define PATHLEN_MAX 256
#define CMOS_CFG_INI "imx117_cfg.ini"
static char pcName[PATHLEN_MAX] = "configs/imx117_cfg.ini";
    
    
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
    pstAeSnsDft->u32FlickerFreq = 50*256;
    pstAeSnsDft->u32FullLinesMax = FULL_LINES_MAX;

    pstAeSnsDft->stIntTimeAccu.enAccuType = AE_ACCURACY_LINEAR;
    pstAeSnsDft->stIntTimeAccu.f32Accuracy = 1;
    pstAeSnsDft->stIntTimeAccu.f32Offset = (1-0.1083);  //3.85us

    pstAeSnsDft->stAgainAccu.enAccuType = AE_ACCURACY_TABLE;
    pstAeSnsDft->stAgainAccu.f32Accuracy = 0.2;

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
            
            //pstAeSnsDft->u8AeCompensation = g_AeDft[0].u8AeCompensation;
            pstAeSnsDft->u8AeCompensation = 0x3e;
            
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
    
    pstAeSnsDft->u32LinesPer500ms = gu32FullLinesStd*60/2;
    pstAeSnsDft->u32FullLinesStd = gu32FullLinesStd;
    pstAeSnsDft->u32FlickerFreq = 0;
    pstAeSnsDft->u32FullLinesMax = FULL_LINES_MAX;

    pstAeSnsDft->stIntTimeAccu.enAccuType = AE_ACCURACY_LINEAR;
    pstAeSnsDft->stIntTimeAccu.f32Accuracy = 1;
    pstAeSnsDft->stIntTimeAccu.f32Offset = (1-0.1083);  //3.85us

    pstAeSnsDft->stAgainAccu.enAccuType = AE_ACCURACY_TABLE;
    pstAeSnsDft->stAgainAccu.f32Accuracy = 0.3;

    pstAeSnsDft->stDgainAccu.enAccuType = AE_ACCURACY_TABLE;
    pstAeSnsDft->stDgainAccu.f32Accuracy = 0.3;
    
    pstAeSnsDft->u32ISPDgainShift = 8;
    pstAeSnsDft->u32MinISPDgainTarget = 1 << pstAeSnsDft->u32ISPDgainShift;
    pstAeSnsDft->u32MaxISPDgainTarget = 8 << pstAeSnsDft->u32ISPDgainShift; 
    
    switch(gu8SensorImageMode)
    {
        case SENSOR_1080P_60FPS_MODE:  
        {
            pstAeSnsDft->au8HistThresh[0] = 0xd;
            pstAeSnsDft->au8HistThresh[1] = 0x28;
            pstAeSnsDft->au8HistThresh[2] = 0x60;
            pstAeSnsDft->au8HistThresh[3] = 0x80;
            
            pstAeSnsDft->u8AeCompensation = 0x38;
            
            pstAeSnsDft->u32MaxIntTime = 2590;
            pstAeSnsDft->u32MinIntTime = 10;
            pstAeSnsDft->u32MaxIntTimeTarget = 65535;
            pstAeSnsDft->u32MinIntTimeTarget = 2;
            
            pstAeSnsDft->u32MaxAgain = 22528;
            pstAeSnsDft->u32MinAgain = 1024;
            pstAeSnsDft->u32MaxAgainTarget = pstAeSnsDft->u32MaxAgain;
            pstAeSnsDft->u32MinAgainTarget = pstAeSnsDft->u32MinAgain;
            
            pstAeSnsDft->u32MaxDgain = 1024;  
            pstAeSnsDft->u32MinDgain = 1024;
            pstAeSnsDft->u32MaxDgainTarget = pstAeSnsDft->u32MaxDgain;
            pstAeSnsDft->u32MinDgainTarget = pstAeSnsDft->u32MinDgain;   
            break;  
        }    

        default:
            break;
        
    }

    return 0;
}

#endif
 
//----------------------------------------------------------------------------------
// the function of sensor set fps 
// in imx117 we don't use vmax to decrease frame rate, instead of useing SVR to downsample...
// ...vertical frequency
//  it only supports etc FPS/n (n = 1 2 3 .... k, FPS = 50, 60, 120, 240)  
//-----------------------------------------------------------------------------------

static HI_VOID cmos_fps_set(HI_FLOAT f32Fps, AE_SENSOR_DEFAULT_S *pstAeSnsDft)
{ 
    //HI_U32 u32PwmPeriod;
    HI_U32 u32UpFpsLmt = 60;
    HI_U32 u32LinesMargin = 0;
    HI_U32 u32Svr;
    HI_U32 u32Fps;

    
    f32Fps = MAX(f32Fps, 2);
    
    if (SENSOR_1080P_60FPS_MODE == gu8SensorImageMode)
    {
        u32UpFpsLmt = 60;
    }
    else if(SENSOR_720P_120FPS_MODE == gu8SensorImageMode)
    {
        u32UpFpsLmt = 120;
    }
    else if((SENSOR_VGA_240FPS_MODE == gu8SensorImageMode))
    {   
        u32UpFpsLmt = 240;
    }
    else if((SENSOR_1080P_50FPS_MODE== gu8SensorImageMode))
    {   
        u32UpFpsLmt = 50;
    }
    
    u32Fps = f32Fps * 100;
    
    if (u32UpFpsLmt * 100 % u32Fps || f32Fps > u32UpFpsLmt)
    {
        return;
    }
    u32Svr = u32UpFpsLmt / f32Fps;
    if (u32Svr)
    {
        --u32Svr;
    }
    
    g_stImx117ShtInfo[gu8SensorImageMode].svr = u32Svr;
    {
        u32LinesMargin = g_stImx117ShtInfo[gu8SensorImageMode].full_lines - g_stImx117ShtInfo[gu8SensorImageMode].max_line;
        gu32FullLinesStd = g_stImx117ShtInfo[gu8SensorImageMode].full_lines * (u32Svr + 1);
        
 
        g_stSnsRegsInfo.astSspData[8].u32Data = LOW_8BITS(u32Svr);
        g_stSnsRegsInfo.astSspData[9].u32Data = HIGH_8BITS(u32Svr);
        
    }

    pstAeSnsDft->f32Fps = f32Fps;
    pstAeSnsDft->u32MaxIntTime = gu32FullLinesStd - u32LinesMargin;
    pstAeSnsDft->u32MinIntTime = g_stImx117ShtInfo[gu8SensorImageMode].min_line; 
    pstAeSnsDft->u32LinesPer500ms = gu32FullLinesStd * f32Fps / 2;
    pstAeSnsDft->u32FullLinesStd = gu32FullLinesStd;
    pstAeSnsDft->u32FullLines = gu32FullLinesStd;
    
    return;
}

static HI_VOID cmos_slow_framerate_set(HI_U32 u32FullLines,
    AE_SENSOR_DEFAULT_S *pstAeSnsDft)
{
    pstAeSnsDft->u32FullLines = gu32FullLinesStd;
    // current do not support slow shutter
    return; 
}

/* while isp notify ae to update sensor regs, ae call these funcs. */
static HI_VOID cmos_inttime_update(HI_U32 u32IntTime)
{
    // SHR range 10 ~ (2600-4 2596) @1080P60
    
    HI_U32 u32Value = g_stImx117ShtInfo[gu8SensorImageMode].full_lines * (1 + g_stImx117ShtInfo[gu8SensorImageMode].svr ) - u32IntTime;

    u32Value = MIN(u32Value, 0xffff);
    
    g_stSnsRegsInfo.astSspData[0].u32Data = (u32Value & 0xFF);
    g_stSnsRegsInfo.astSspData[1].u32Data = ((u32Value & 0xFF00) >> 8);  
    
    return;


}


/* Again segmentation = 123 */
static HI_U32 ad_gain_table[123]=
{    
    1024,1032, 1040, 1049, 1057, 1066, 1074, 1083, 1092,1101,1111,1120,1130,1140,1150,1160,1170,1181,1192,
    1202,1214, 1225, 1237, 1248, 1260, 1273, 1285, 1298,1311,1324,1337,1351,1365,1380,1394,1409,1425,1440,
    1456,1473, 1489, 1507, 1524, 1542, 1560, 1579, 1598,1618,1638,1659,1680,1702,1725,1748,1771,1796,1820,
    1846,1872, 1900, 1928, 1956, 1986, 2016, 2048, 2081,2114,2149,2185,2222,2260,2300,2341,2383,2427,2473,
    2521,2570, 2621, 2675, 2731, 2789, 2849, 2913, 2979,3048,3121,3197,3277,3361,3449,3542,3641,3745,3855,
    3972,4096, 4228, 4369, 4520, 4681, 4855, 5041, 5243,5461,5699,5958,6242,6554,6899,7282,7710,8192,8738,
    9362,10082,10923,11916,13107,14564,16384,18725,21845
};

static HI_VOID cmos_again_calc_table(HI_U32 *pu32AgainLin, HI_U32 *pu32AgainDb)
{

    
    int i;

    if((HI_NULL == pu32AgainLin) ||(HI_NULL == pu32AgainDb))
    {
        printf("null pointer when get ae sensor gain info  value!\n");
        return;
    }

    if (*pu32AgainLin >= ad_gain_table[122])
    {
         *pu32AgainLin = ad_gain_table[122];
         *pu32AgainDb = 122;
         goto calc_table_end;
    }
    
    for (i = 1; i < 123; i++)
    {
        if (*pu32AgainLin < ad_gain_table[i])
        {
            *pu32AgainLin = ad_gain_table[i - 1];
            *pu32AgainDb = i - 1;
            goto calc_table_end;;
        }
    }


calc_table_end:

    // downsample require
    *pu32AgainDb <<= 4;

    return;
}


static HI_VOID cmos_gains_update(HI_U32 u32Again, HI_U32 u32Dgain)
{  
    //HI_U32 u32Tmp = u32Again + u32Dgain;
    
    HI_U16 u16Agc01 = 0;
    HI_U16 u16Agc02 = 0;
    
    if (u32Again <= 0x146) 
    {
        u16Agc01 = 0x01F;
        u16Agc02 = 0x01E;
    } 
    else if (u32Again <= 0x400) 
    {
        u16Agc01 = 0x021;
        u16Agc02 = 0x020;
    }
    else if (u32Again <= 0x52C) 
    {
        u16Agc01 = 0x026;
        u16Agc02 = 0x025;
    } 
    else if (u32Again <= 0x696) 
    {
        u16Agc01 = 0x028;
        u16Agc02 = 0x027;
    } 
    else if (u32Again <= 0x74B) 
    {
        u16Agc01 = 0x02C;
        u16Agc02 = 0x02B;
    } 
    else if (u32Again <= 0x7A5) 
    {
        u16Agc01 = 0x030;
        u16Agc02 = 0x02F;
    }

    g_stSnsRegsInfo.astSspData[2].u32Data = (u32Again & 0xFF);
    g_stSnsRegsInfo.astSspData[3].u32Data = ((u32Again >> 8) & 0x00FF);
    g_stSnsRegsInfo.astSspData[4].u32Data = (u16Agc01 & 0xFF);
    g_stSnsRegsInfo.astSspData[5].u32Data = ((u16Agc01 >> 8) & 0x00FF);
    g_stSnsRegsInfo.astSspData[6].u32Data = (u16Agc02 & 0xFF);
    g_stSnsRegsInfo.astSspData[7].u32Data = ((u16Agc02 >> 8) & 0x00FF);

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
    pstExpFuncs->pfn_cmos_dgain_calc_table  = HI_NULL;   

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
      0x01b0, 0x8091, 0x801f,   
      0x8043, 0x0191, 0x804e,   
      0x000e, 0x80bf, 0x01b1    
   },
   3770,
   {
      0x01b3, 0x8095, 0x801e,   
      0x8067, 0x019e, 0x8037,   
      0x000a, 0x80b9, 0x01af    
   }, 
   
   2640,
   {     
      0x01b0, 0x807c, 0x8034,   
      0x8069, 0x019f, 0x8036,   
      0x8005, 0x80ab, 0x01b0,   
   }
};

static AWB_AGC_TABLE_S g_stAwbAgcTable =
{
    /* bvalid */
    1,

    /* saturation */ 
    {0x80,0x80,0x80,0x80,0x80,0x80,0x5a,0x50,0x4a,0x40,0x3a,0x3a,0x3a,0x3a,0x3a,0x3a}
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
    //{0x58,0x58,0x56,0x4e,0x46,0x36,0x36,0x34,0x34,0x32,0x30,0x20,0x20,0x20,0x20,0x20}
    //{0x86,0x86,0x84,0x7a,0x60,0x50,0x40,0x30,0x20,0x10,0x10,0x10,0x10,0x10,0x10,0x10}
    {0x86,0x86,0x86,0x86,0x86,0x50,0x40,0x30,0x20,0x10,0x10,0x10,0x10,0x10,0x10,0x10}
};


static ISP_CMOS_NOISE_TABLE_S g_stIspNoiseTable =
{
    /* bvalid */
    1,

    /* nosie_profile_weight_lut */
    {
    0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x3,  0xC,  0x11, 0x15, 0x17, 0x1A, 0x1B, 0x1D, 0x1E, 0x1F, 
    0x21, 0x22, 0x23, 0x23, 0x24, 0x25, 0x26, 0x26, 0x27, 0x27, 0x28, 0x29, 0x29, 0x2A, 0x2A, 0x2A, 
    0x2B, 0x2B, 0x2C, 0x2C, 0x2C, 0x2D, 0x2D, 0x2E, 0x2E, 0x2E, 0x2E, 0x2F, 0x2F, 0x2F, 0x30, 0x30, 
    0x30, 0x30, 0x31, 0x31, 0x31, 0x31, 0x32, 0x32, 0x32, 0x32, 0x33, 0x33, 0x33, 0x33, 0x33, 0x34, 
    0x34, 0x34, 0x34, 0x34, 0x35, 0x35, 0x35, 0x35, 0x35, 0x35, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 
    0x37, 0x37, 0x37, 0x37, 0x37, 0x37, 0x38, 0x38, 0x38, 0x38, 0x38, 0x38, 0x38, 0x38, 0x39, 0x39,
    0x39, 0x39, 0x39, 0x39, 0x39, 0x39, 0x3A, 0x3A, 0x3A, 0x3A, 0x3A, 0x3A, 0x3A, 0x3A, 0x3B, 0x3B, 
    0x3B, 0x3B, 0x3B, 0x3B, 0x3B, 0x3B, 0x3B, 0x3B, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 
    },

    /* demosaic_weight_lut */
    {
    0x3,  0xC,  0x11, 0x15, 0x17, 0x1A, 0x1B, 0x1D, 0x1E, 0x1F, 0x21, 0x22, 0x23, 0x23, 0x24, 0x25, 
    0x26, 0x26, 0x27, 0x27, 0x28, 0x29, 0x29, 0x2A, 0x2A, 0x2A, 0x2B, 0x2B, 0x2C, 0x2C, 0x2C, 0x2D, 
    0x2D, 0x2E, 0x2E, 0x2E, 0x2E, 0x2F, 0x2F, 0x2F, 0x30, 0x30, 0x30, 0x30, 0x31, 0x31, 0x31, 0x31, 
    0x32, 0x32, 0x32, 0x32, 0x33, 0x33, 0x33, 0x33, 0x33, 0x34, 0x34, 0x34, 0x34, 0x34, 0x35, 0x35, 
    0x35, 0x35, 0x35, 0x35, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x37, 0x37, 0x37, 0x37, 0x37, 0x37, 
    0x38, 0x38, 0x38, 0x38, 0x38, 0x38, 0x38, 0x38, 0x39, 0x39, 0x39, 0x39, 0x39, 0x39, 0x39, 0x39, 
    0x3A, 0x3A, 0x3A, 0x3A, 0x3A, 0x3A, 0x3A, 0x3A, 0x3B, 0x3B, 0x3B, 0x3B, 0x3B, 0x3B, 0x3B, 0x3B, 
    0x3B, 0x3B, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 
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
    0x5a,//0xa0,

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


static const ISP_CMOS_SHADING_S g_stLensShading =
{
    /* bvalid */   
    1, 
    /*u16RCenterX*/
    0x3c0,
    /*u16RCenterY*/
    0x21c,
    /*u16GCenterX*/
    0x3c0,
    /*u16GCenterY*/
    0x21c,
    /*u16BCenterX*/
    0x3c0,
    /*u16BCenterY*/
    0x21c,
    /*HI_U16 au16RShadingTbl*/
    {
        0x1000,0x1000,0x1000,0x1000,0x1001,0x1003,0x1005,0x1007,0x1009,0x100b,0x100d,0x100f,\
        0x1010,0x1012,0x1013,0x1014,0x1015,0x1016,0x1016,0x1017,0x1017,0x1018,0x1018,0x1018,\
        0x1018,0x1018,0x1018,0x1018,0x1017,0x1017,0x1017,0x1016,0x1016,0x1015,0x1014,0x1014,\
        0x1013,0x1012,0x1012,0x1011,0x1010,0x100f,0x100f,0x100e,0x100d,0x100c,0x100c,0x100b,\
        0x100a,0x1009,0x1009,0x1008,0x1007,0x1007,0x1006,0x1006,0x1005,0x1005,0x1004,0x1004,\
        0x1004,0x1003,0x1003,0x1003,0x1003,0x1003,0x1003,0x1003,0x1004,0x1004,0x1004,0x1005,\
        0x1005,0x1006,0x1006,0x1007,0x1008,0x1009,0x100a,0x100b,0x100c,0x100e,0x100f,0x1010,\
        0x1012,0x1014,0x1016,0x1018,0x101a,0x101c,0x101e,0x1020,0x1023,0x1025,0x1028,0x102b,\
        0x102e,0x1031,0x1034,0x1038,0x103b,0x103f,0x1043,0x1047,0x104b,0x104f,0x1053,0x1058,\
        0x105c,0x1061,0x1066,0x106b,0x1071,0x1076,0x107c,0x1081,0x1087,0x108d,0x1094,0x109a,\
        0x10a1,0x10a8,0x10af,0x10b6,0x10bd,0x10c5,0x10cc,0x10d4,0x10da
    },
    /*HI_U16 au16GShadingTbl*/
    {
        0x1000,0x1000,0x1000,0x1000,0x1001,0x1002,0x1004,0x1006,0x1008,0x1009,0x100b,0x100d,\
        0x100e,0x100f,0x1011,0x1012,0x1013,0x1013,0x1014,0x1015,0x1015,0x1015,0x1015,0x1016,\
        0x1016,0x1015,0x1015,0x1015,0x1015,0x1014,0x1014,0x1014,0x1013,0x1012,0x1012,0x1011,\
        0x1011,0x1010,0x100f,0x100e,0x100e,0x100d,0x100c,0x100b,0x100b,0x100a,0x1009,0x1008,\
        0x1008,0x1007,0x1006,0x1006,0x1005,0x1005,0x1004,0x1004,0x1003,0x1003,0x1002,0x1002,\
        0x1002,0x1002,0x1001,0x1001,0x1001,0x1001,0x1001,0x1002,0x1002,0x1002,0x1003,0x1003,\
        0x1004,0x1004,0x1005,0x1006,0x1006,0x1007,0x1008,0x100a,0x100b,0x100c,0x100e,0x100f,\
        0x1011,0x1012,0x1014,0x1016,0x1018,0x101a,0x101d,0x101f,0x1022,0x1024,0x1027,0x102a,\
        0x102d,0x1030,0x1033,0x1037,0x103a,0x103e,0x1042,0x1045,0x1049,0x104e,0x1052,0x1056,\
        0x105b,0x1060,0x1065,0x106a,0x106f,0x1075,0x107a,0x1080,0x1086,0x108c,0x1092,0x1099,\
        0x109f,0x10a6,0x10ad,0x10b4,0x10bb,0x10c3,0x10ca,0x10d2,0x10d8
    },
    /*HI_U16 au16BShadingTbl*/
    {
        0x1000,0x1000,0x1000,0x1000,0x1000,0x1000,0x1000,0x1000,0x1000,0x1000,0x1000,0x1000,\
        0x1000,0x1000,0x1000,0x1000,0x1000,0x1000,0x1000,0x1000,0x1000,0x1000,0x1000,0x1000,\
        0x1000,0x1000,0x1000,0x1000,0x1000,0x1000,0x1000,0x1000,0x1000,0x1000,0x1000,0x1000,\
        0x1000,0x1000,0x1000,0x1000,0x1000,0x1000,0x1000,0x1000,0x1000,0x1000,0x1000,0x1000,\
        0x1000,0x1000,0x1000,0x1000,0x1000,0x1000,0x1000,0x1000,0x1000,0x1000,0x1000,0x1000,\
        0x1000,0x1000,0x1000,0x1000,0x1000,0x1000,0x1000,0x1000,0x1000,0x1000,0x1000,0x1001,\
        0x1001,0x1002,0x1003,0x1003,0x1004,0x1005,0x1006,0x1007,0x1009,0x100a,0x100c,0x100d,\
        0x100f,0x1011,0x1013,0x1015,0x1017,0x1019,0x101b,0x101e,0x1021,0x1023,0x1026,0x1029,\
        0x102d,0x1030,0x1033,0x1037,0x103b,0x103f,0x1043,0x1047,0x104b,0x1050,0x1054,0x1059,\
        0x105e,0x1063,0x1069,0x106e,0x1074,0x107a,0x1080,0x1086,0x108b,0x1093,0x109a,0x10a1,\
        0x10a8,0x10af,0x10b7,0x10be,0x10c6,0x10cf,0x10d7,0x10e0,0x10e6
    },

    /*u16ROffCenter*/
    0x599,
    /*u16GOffCenter*/
    0x51c,
    /*u16BOffCenter*/
    0x3fc,
    /*u16TblNodeNum*/
    129
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
            memcpy(&pstDef->stShading, &g_stLensShading, sizeof(ISP_CMOS_SHADING_S));
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

    for (i=0; i<4; i++)
    {
        pstBlackLevel->au16BlackLevel[i] = 0xc8;/*10bit : 0x3c*/
    }

    return 0;    
}

HI_VOID cmos_set_pixel_detect(HI_BOOL bEnable)
{    
    HI_U32 u32Svr, u32PixDetShr;
    HI_U32 u32UpFpsLmt;

    // detect at 5FPS
    if (SENSOR_1080P_60FPS_MODE == gu8SensorImageMode)
    {
        u32UpFpsLmt = 60;
        u32PixDetShr = 10;
    }
    else if (SENSOR_1080P_50FPS_MODE == gu8SensorImageMode)
    {
        u32UpFpsLmt = 50;
        u32PixDetShr = 10;
    }
    else if (SENSOR_720P_120FPS_MODE == gu8SensorImageMode)
    {
        u32UpFpsLmt = 120;
        u32PixDetShr = 4;
    }
    else 
    {
        u32UpFpsLmt = 240;
        u32PixDetShr = 4;
    }
    
    u32Svr = u32UpFpsLmt / 5 - 1;
    

    if (bEnable)
    {
        /* Analog and Digital gains both must be programmed for their minimum values */
        sensor_write_register (PGC_L, 0x0);                 
        sensor_write_register (PGC_H, 0x0);
        sensor_write_register (APGC01_L, 0x1F);                 
        sensor_write_register (APGC01_H, 0x0);
        sensor_write_register (APGC02_L, 0x1E);                 
        sensor_write_register (APGC02_H, 0x0);       
      
        sensor_write_register(SHR_L, LOW_8BITS(u32PixDetShr));
        sensor_write_register(SHR_H, HIGH_8BITS(u32PixDetShr));
        sensor_write_register(SVR_L, LOW_8BITS(u32Svr));
        sensor_write_register(SVR_H, HIGH_8BITS(u32Svr)); 
        usleep(10000);
    }
    else /* setup for ISP 'normal mode' */
    {
        u32Svr = g_stImx117ShtInfo[gu8SensorImageMode].svr;
        sensor_write_register(SVR_L, LOW_8BITS(u32Svr));
        sensor_write_register(SVR_H, HIGH_8BITS(u32Svr));
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
            gu32FullLinesStd = VMAX_1080P60;
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

HI_U32 cmos_get_sns_regs_info(ISP_SNS_REGS_INFO_S *pstSnsRegsInfo)
{
    HI_S32 i;

    if (HI_FALSE == bInit)
    {
        g_stSnsRegsInfo.enSnsType = ISP_SNS_SSP_TYPE;
        g_stSnsRegsInfo.u8Cfg2ValidDelayMax = 2;        
        g_stSnsRegsInfo.u32RegNum = 10;        

        for (i=0; i<g_stSnsRegsInfo.u32RegNum; i++)
        {
            g_stSnsRegsInfo.astSspData[i].bUpdate = HI_TRUE;
            g_stSnsRegsInfo.astSspData[i].u32DevAddr = 0x81;
            g_stSnsRegsInfo.astSspData[i].u32DevAddrByteNum = 1;
            g_stSnsRegsInfo.astSspData[i].u32RegAddrByteNum = 2;
            g_stSnsRegsInfo.astSspData[i].u32DataByteNum = 1;
        }        
        g_stSnsRegsInfo.astSspData[0].u8DelayFrmNum = 0;
        g_stSnsRegsInfo.astSspData[0].u32RegAddr = SHR_L;
        g_stSnsRegsInfo.astSspData[1].u8DelayFrmNum = 0;
        g_stSnsRegsInfo.astSspData[1].u32RegAddr = SHR_H;

        // gain related
        g_stSnsRegsInfo.astSspData[2].u32RegAddr = PGC_L;
        g_stSnsRegsInfo.astSspData[2].u8DelayFrmNum = 1;
        g_stSnsRegsInfo.astSspData[3].u32RegAddr = PGC_H;
        g_stSnsRegsInfo.astSspData[3].u8DelayFrmNum = 1;
        g_stSnsRegsInfo.astSspData[4].u32RegAddr = APGC01_L;
        g_stSnsRegsInfo.astSspData[4].u8DelayFrmNum = 1;
        g_stSnsRegsInfo.astSspData[5].u32RegAddr = APGC01_H;
        g_stSnsRegsInfo.astSspData[5].u8DelayFrmNum = 1;
        g_stSnsRegsInfo.astSspData[6].u32RegAddr = APGC02_L;
        g_stSnsRegsInfo.astSspData[6].u8DelayFrmNum = 1;
        g_stSnsRegsInfo.astSspData[7].u32RegAddr = APGC02_H;
        g_stSnsRegsInfo.astSspData[7].u8DelayFrmNum = 1;
        //svr registers
        g_stSnsRegsInfo.astSspData[8].u32RegAddr = SVR_L;
        g_stSnsRegsInfo.astSspData[8].u8DelayFrmNum = 0;
        g_stSnsRegsInfo.astSspData[9].u32RegAddr = SVR_H;
        g_stSnsRegsInfo.astSspData[9].u8DelayFrmNum = 0;
        
    

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
    HI_U16 u16Fps;
    HI_BOOL bNotSupport = HI_FALSE;
    
    bInit = HI_FALSE;    

        
    if (HI_NULL == pstSensorImageMode )
    {
        printf("null pointer when set image mode\n");
        return -1;
    }
    
    if ((pstSensorImageMode->u16Width <= 800) && (pstSensorImageMode->u16Height <= 480))
    {
        if (WDR_MODE_NONE == genSensorMode)
        {
            if (pstSensorImageMode->f32Fps <= 240)
            {
                u8SensorImageMode = SENSOR_VGA_240FPS_MODE;
            }
            else
            {
                printf("Not support! Width:%d, Height:%d, Fps:%f, WDRMode:%d\n", 
                    pstSensorImageMode->u16Width, 
                    pstSensorImageMode->u16Height,
                    pstSensorImageMode->f32Fps,
                    genSensorMode);
            }
        }
        else
        {
            printf("Not support! Width:%d, Height:%d, Fps:%f, WDRMode:%d\n", 
                    pstSensorImageMode->u16Width, 
                    pstSensorImageMode->u16Height,
                    pstSensorImageMode->f32Fps,
                    genSensorMode);
        }
    }
    else if ((pstSensorImageMode->u16Width <= 1280) && (pstSensorImageMode->u16Height <= 720))
    {
        if (WDR_MODE_NONE == genSensorMode)
        {
            if (pstSensorImageMode->f32Fps <= 120)
            {
                u8SensorImageMode = SENSOR_720P_120FPS_MODE;
            }
            else
            {
                printf("Not support! Width:%d, Height:%d, Fps:%f, WDRMode:%d\n", 
                    pstSensorImageMode->u16Width, 
                    pstSensorImageMode->u16Height,
                    pstSensorImageMode->f32Fps,
                    genSensorMode);
            }
        }
        else
        {
            printf("Not support! Width:%d, Height:%d, Fps:%f, WDRMode:%d\n", 
                    pstSensorImageMode->u16Width, 
                    pstSensorImageMode->u16Height,
                    pstSensorImageMode->f32Fps,
                    genSensorMode);
        }
    }
    else if ((pstSensorImageMode->u16Width <= 1920) && (pstSensorImageMode->u16Height <= 1080))
    {
        if (WDR_MODE_NONE == genSensorMode)
        {
            // scale fps as 100 times
            u16Fps = pstSensorImageMode->f32Fps * 100;
            if (u16Fps > 6000)
            {
                bNotSupport = HI_TRUE;
            }
            else if (u16Fps <= 5000 && !(5000 % u16Fps))
            {
                u8SensorImageMode = SENSOR_1080P_50FPS_MODE;
            }
            else if (!(6000 % u16Fps))
            {
                u8SensorImageMode = SENSOR_1080P_60FPS_MODE;
            }
            else 
            {
                bNotSupport = HI_TRUE;
            }
            
            if (bNotSupport)
            {
                printf("Not support! Width:%d, Height:%d, Fps:%f, WDRMode:%d\n", 
                    pstSensorImageMode->u16Width, 
                    pstSensorImageMode->u16Height,
                    pstSensorImageMode->f32Fps,
                    genSensorMode);
            }
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
    genSensorMode = WDR_MODE_NONE;
    bSensorInit = HI_FALSE;
    bInit = HI_FALSE;

    gu32FullLinesStd = VMAX_1080P60;
    genSensorMode = WDR_MODE_NONE;
    gu8SensorImageMode = SENSOR_1080P_60FPS_MODE;
    
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
    s32Ret = HI_MPI_ISP_SensorRegCallBack(IspDev, IMX117_ID, &stIspRegister);
    if (s32Ret)
    {
        printf("sensor register callback function failed!\n");
        return s32Ret;
    }
    
    stLib.s32Id = 0;
    strncpy(stLib.acLibName, HI_AE_LIB_NAME, sizeof(HI_AE_LIB_NAME));
    cmos_init_ae_exp_function(&stAeRegister.stSnsExp);
    s32Ret = HI_MPI_AE_SensorRegCallBack(IspDev, &stLib, IMX117_ID, &stAeRegister);
    if (s32Ret)
    {
        printf("sensor register callback function to ae lib failed!\n");
        return s32Ret;
    }

    stLib.s32Id = 0;
    strncpy(stLib.acLibName, HI_AWB_LIB_NAME, sizeof(HI_AWB_LIB_NAME));
    cmos_init_awb_exp_function(&stAwbRegister.stSnsExp);
    s32Ret = HI_MPI_AWB_SensorRegCallBack(IspDev, &stLib, IMX117_ID, &stAwbRegister);
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

    s32Ret = HI_MPI_ISP_SensorUnRegCallBack(IspDev, IMX117_ID);
    if (s32Ret)
    {
        printf("sensor unregister callback function failed!\n");
        return s32Ret;
    }
    
    stLib.s32Id = 0;
    strncpy(stLib.acLibName, HI_AE_LIB_NAME, sizeof(HI_AE_LIB_NAME));
    s32Ret = HI_MPI_AE_SensorUnRegCallBack(IspDev, &stLib, IMX117_ID);
    if (s32Ret)
    {
        printf("sensor unregister callback function to ae lib failed!\n");
        return s32Ret;
    }

    stLib.s32Id = 0;
    strncpy(stLib.acLibName, HI_AWB_LIB_NAME, sizeof(HI_AWB_LIB_NAME));
    s32Ret = HI_MPI_AWB_SensorUnRegCallBack(IspDev, &stLib, IMX117_ID);
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

