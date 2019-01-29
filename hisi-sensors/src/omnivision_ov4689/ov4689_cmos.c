#if !defined(__OV4689_CMOS_H_)
#define __OV4689_CMOS_H_

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

#define OV4689_ID 4689


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

#define FULL_LINES_MAX  (0x7FFF)

#define SENSOR_4M_30FPS_MODE         (0)
#define SENSOR_1080P_60FPS_MODE      (1)
//#define SENSOR_1080P_30FPS_MODE      (2)
#define SENSOR_2304_1296_30FPS_MODE  (2)
#define SENSOR_2048_1520_30FPS_MODE  (3)
#define SENSOR_720p_180FPS_MODE (4)

#define OV4689_VMAX_4M30_WDR      (0x72a)
#define OV4689_VMAX_4M30      (0x614)
#define OV4689_VMAX_1080P     (0x612)
#define OV4689_VMAX_1080P_WDR_LINE (0x614)
#define OV4689_VMAX_2304_1296 (0x534)
#define OV4689_VMAX_2048_1520 (0x614)
#define OV4689_VMAX_720P  (0x305)

HI_U8 gu8SensorImageMode = SENSOR_4M_30FPS_MODE;
WDR_MODE_E genSensorMode = WDR_MODE_NONE;

static HI_U32 gu32FullLinesStd = 1632; 
static HI_U32 gu32FullLines = 1632;
static HI_U32 gu32PreFullLines = 1632;
static HI_BOOL bInit = HI_FALSE;
HI_BOOL bSensorInit = HI_FALSE; 
static ISP_FSWDR_MODE_E genFSWDRMode = ISP_FSWDR_NORMAL_MODE;
static HI_U32 gu32MaxTimeGetCnt = 0;

ISP_SNS_REGS_INFO_S g_stSnsRegsInfo = {0};
ISP_SNS_REGS_INFO_S g_stPreSnsRegsInfo = {0};

static HI_U32 au32WDRIntTime[2] = {0};

#define PATHLEN_MAX 256
#define CMOS_CFG_INI "ov4689_cfg.ini"
static char pcName[PATHLEN_MAX] = "configs/ov4689_cfg.ini";

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

    pstAeSnsDft->stDgainAccu.enAccuType = AE_ACCURACY_LINEAR;
    pstAeSnsDft->stDgainAccu.f32Accuracy = 1;
    
    switch(genSensorMode)
     {
         default:
         case WDR_MODE_NONE:   /*linear mode*/
             pstAeSnsDft->au8HistThresh[0] = 0xd;
             pstAeSnsDft->au8HistThresh[1] = 0x28;
             pstAeSnsDft->au8HistThresh[2] = 0x60;
             pstAeSnsDft->au8HistThresh[3] = 0x80;
             
             pstAeSnsDft->u8AeCompensation = g_AeDft[0].u8AeCompensation;
             
             pstAeSnsDft->u32MaxIntTime = gu32FullLinesStd - 4;
             pstAeSnsDft->u32MinIntTime = 2;
             pstAeSnsDft->u32MaxIntTimeTarget = g_AeDft[0].u32MaxIntTimeTarget;
             pstAeSnsDft->u32MinIntTimeTarget = g_AeDft[0].u32MinIntTimeTarget;
             
             pstAeSnsDft->u32MaxAgain = 16384; 
             pstAeSnsDft->u32MinAgain = 1024;
             pstAeSnsDft->u32MaxAgainTarget = g_AeDft[0].u32MaxAgainTarget;
             pstAeSnsDft->u32MinAgainTarget = g_AeDft[0].u32MinAgainTarget;
             
             pstAeSnsDft->u32MaxDgain = 1;  
             pstAeSnsDft->u32MinDgain = 1;
             pstAeSnsDft->u32MaxDgainTarget = g_AeDft[0].u32MaxDgainTarget;
             pstAeSnsDft->u32MinDgainTarget = g_AeDft[0].u32MinDgainTarget;
    
             pstAeSnsDft->u32ISPDgainShift = g_AeDft[0].u32ISPDgainShift;
             pstAeSnsDft->u32MinISPDgainTarget = g_AeDft[0].u32MinISPDgainTarget;
             pstAeSnsDft->u32MaxISPDgainTarget = g_AeDft[0].u32MaxISPDgainTarget;                  
        break;
        case WDR_MODE_2To1_FRAME:
        case WDR_MODE_2To1_FRAME_FULL_RATE: 
            pstAeSnsDft->au8HistThresh[0] = 0xC;
            pstAeSnsDft->au8HistThresh[1] = 0x18;
            pstAeSnsDft->au8HistThresh[2] = 0x60;
            pstAeSnsDft->au8HistThresh[3] = 0x80;
           
            pstAeSnsDft->u8AeCompensation = g_AeDft[2].u8AeCompensation;
            
            pstAeSnsDft->u32MaxIntTime = gu32FullLinesStd - 4;
            pstAeSnsDft->u32MinIntTime = 2;
            pstAeSnsDft->u32MaxIntTimeTarget = g_AeDft[2].u32MaxIntTimeTarget;
            pstAeSnsDft->u32MinIntTimeTarget = g_AeDft[2].u32MinIntTimeTarget;
            
            pstAeSnsDft->u32MaxAgain = 16384;
            pstAeSnsDft->u32MinAgain = 1024;
            pstAeSnsDft->u32MaxAgainTarget = g_AeDft[2].u32MaxAgainTarget;
            pstAeSnsDft->u32MinAgainTarget = g_AeDft[2].u32MinAgainTarget;
            
            pstAeSnsDft->u32MaxDgain = 1;  
            pstAeSnsDft->u32MinDgain = 1;
            pstAeSnsDft->u32MaxDgainTarget = g_AeDft[2].u32MaxDgainTarget;
            pstAeSnsDft->u32MinDgainTarget = g_AeDft[2].u32MinDgainTarget;

            pstAeSnsDft->u32ISPDgainShift = g_AeDft[2].u32ISPDgainShift;
            pstAeSnsDft->u32MinISPDgainTarget = g_AeDft[2].u32MinISPDgainTarget;
            pstAeSnsDft->u32MaxISPDgainTarget = g_AeDft[2].u32MaxISPDgainTarget;            
        break; 
        case WDR_MODE_2To1_LINE:
            pstAeSnsDft->au8HistThresh[0] = 0xC;
            pstAeSnsDft->au8HistThresh[1] = 0x18;
            pstAeSnsDft->au8HistThresh[2] = 0x60;
            pstAeSnsDft->au8HistThresh[3] = 0x80;

            pstAeSnsDft->u8AeCompensation = g_AeDft[1].u8AeCompensation;

            pstAeSnsDft->u32MaxIntTime = gu32FullLinesStd - 4;  
            pstAeSnsDft->u32MinIntTime = 2;
            pstAeSnsDft->u32MaxIntTimeTarget = g_AeDft[1].u32MaxIntTimeTarget;
            pstAeSnsDft->u32MinIntTimeTarget = g_AeDft[1].u32MinIntTimeTarget;

            pstAeSnsDft->u32MaxAgain = 16384;
            pstAeSnsDft->u32MinAgain = 1024;
            pstAeSnsDft->u32MaxAgainTarget = g_AeDft[1].u32MaxAgainTarget;
            pstAeSnsDft->u32MinAgainTarget = g_AeDft[1].u32MinAgainTarget;

            pstAeSnsDft->u32MaxDgain = 1;
            pstAeSnsDft->u32MinDgain = 1;
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
    pstAeSnsDft->stAgainAccu.f32Accuracy = 0.1;

    pstAeSnsDft->stDgainAccu.enAccuType = AE_ACCURACY_LINEAR;
    pstAeSnsDft->stDgainAccu.f32Accuracy = 1;
    
    pstAeSnsDft->u32ISPDgainShift = 8;
    pstAeSnsDft->u32MinISPDgainTarget = 1 << pstAeSnsDft->u32ISPDgainShift;
    pstAeSnsDft->u32MaxISPDgainTarget = 16 << pstAeSnsDft->u32ISPDgainShift; 
    
    pstAeSnsDft->u32MaxIntTime = gu32FullLinesStd - 4;
    pstAeSnsDft->u32MinIntTime = 2;
    pstAeSnsDft->u32MaxIntTimeTarget = 65535;
    pstAeSnsDft->u32MinIntTimeTarget = 2;

    pstAeSnsDft->u32MaxAgain = 16832;  
    pstAeSnsDft->u32MinAgain = 1024;
    pstAeSnsDft->u32MaxAgainTarget = pstAeSnsDft->u32MaxAgain;
    pstAeSnsDft->u32MinAgainTarget = pstAeSnsDft->u32MinAgain;

    pstAeSnsDft->u32MaxDgain = 1;  
    pstAeSnsDft->u32MinDgain = 1;
    pstAeSnsDft->u32MaxDgainTarget = pstAeSnsDft->u32MaxDgain;
    pstAeSnsDft->u32MinDgainTarget = pstAeSnsDft->u32MinDgain;
    switch(genSensorMode)
    {
        default:
        case WDR_MODE_NONE:   /*linear mode*/
            pstAeSnsDft->au8HistThresh[0] = 0xd;
            pstAeSnsDft->au8HistThresh[1] = 0x28;
            pstAeSnsDft->au8HistThresh[2] = 0x60;
            pstAeSnsDft->au8HistThresh[3] = 0x80;
            
            pstAeSnsDft->u8AeCompensation = 0x38;
        break;

        case WDR_MODE_2To1_FRAME:
        case WDR_MODE_2To1_FRAME_FULL_RATE:
            pstAeSnsDft->au8HistThresh[0] = 0xC;
            pstAeSnsDft->au8HistThresh[1] = 0x18;
            pstAeSnsDft->au8HistThresh[2] = 0x60;
            pstAeSnsDft->au8HistThresh[3] = 0x80;
           
            pstAeSnsDft->u8AeCompensation = 0x38;
        break;
  
        case WDR_MODE_2To1_LINE:
            pstAeSnsDft->au8HistThresh[0] = 0xC;
            pstAeSnsDft->au8HistThresh[1] = 0x18;
            pstAeSnsDft->au8HistThresh[2] = 0x60;
            pstAeSnsDft->au8HistThresh[3] = 0x80;
           
            if(ISP_FSWDR_LONG_FRAME_MODE == genFSWDRMode)
            {
                pstAeSnsDft->u8AeCompensation = 0x64;
            }
            else
            {
                pstAeSnsDft->u8AeCompensation = 0x40;
            }
        break;
    }
    
    switch(gu8SensorImageMode)
    {
        case 1:
            pstAeSnsDft->u32LinesPer500ms = gu32FullLinesStd*60/2;
        break;
        
        case 5:
            pstAeSnsDft->u32LinesPer500ms = gu32FullLinesStd*120/2;
        break;
            
        case 6:
            pstAeSnsDft->u32LinesPer500ms = gu32FullLinesStd*180/2;
        break;
        
        default:
            pstAeSnsDft->u32LinesPer500ms = gu32FullLinesStd*30/2;
        break;
    }
    return 0;
}
#endif

/* the function of sensor set fps */
static HI_VOID cmos_fps_set(HI_FLOAT f32Fps, AE_SENSOR_DEFAULT_S *pstAeSnsDft)
{
    switch (gu8SensorImageMode)
    {
        case SENSOR_4M_30FPS_MODE:
            if(genSensorMode == WDR_MODE_NONE ||genSensorMode == WDR_MODE_2To1_FRAME ||genSensorMode == WDR_MODE_2To1_FRAME_FULL_RATE)
            {
                if ((f32Fps <= 30) && (f32Fps >= 0.5))
                {
                    gu32FullLinesStd = (OV4689_VMAX_4M30 * 30) / f32Fps;
                }
                else
                {
                    printf("Not support Fps: %f\n", f32Fps);
                    return;
                }
            }
            else
            {
                if((f32Fps <= 25) && (f32Fps >= 0.5))
                {
                     gu32FullLinesStd = OV4689_VMAX_4M30_WDR*25 /(f32Fps);
                }
                else
                {
                    //gu32FullLinesStd = OV4689_VMAX_4M30;
                    printf("Not support Fps: %f\n", f32Fps);
                    return;
                }
            }
            
        break;
        
        case SENSOR_2304_1296_30FPS_MODE:
            if ((f32Fps <= 30) && (f32Fps >= 0.5))
            {
                if(WDR_MODE_2To1_LINE == genSensorMode)
                {
                    gu32FullLinesStd = (1340 * 30) / f32Fps;
                }
                else
                {
                    gu32FullLinesStd = (OV4689_VMAX_2304_1296 * 30) / f32Fps;
                }
            }
            else
            {
                printf("Not support Fps: %f\n", f32Fps);
                return;
            }
        break;
        
        case SENSOR_2048_1520_30FPS_MODE:
            if ((f32Fps <= 30) && (f32Fps >= 0.5))
            {
                if(WDR_MODE_2To1_LINE == genSensorMode)
                {
                   gu32FullLinesStd = (0x620 * 30) / f32Fps;
                }
                else
                {
                    gu32FullLinesStd = (OV4689_VMAX_2048_1520 * 30) / f32Fps;
                }
            }
            else
            {
                printf("Not support Fps: %f\n", f32Fps);
                return;
            }

        break;
        
        case SENSOR_1080P_60FPS_MODE:
            if(WDR_MODE_2To1_LINE == genSensorMode)
            {
                if ((f32Fps <= 30) && (f32Fps >= 0.5))
                {
                    gu32FullLinesStd = (OV4689_VMAX_1080P * 30) / f32Fps;
                }
                else
                {
                    printf("Not support Fps: %f\n", f32Fps);
                    return;
                }   
            }else
            {
                if ((f32Fps <= 60) && (f32Fps >= 0.5))
                {
                    gu32FullLinesStd = (OV4689_VMAX_1080P * 60) / f32Fps;
                }
                else
                {
                    printf("Not support Fps: %f\n", f32Fps);
                    return;
                }   
            }
        break;

        case SENSOR_720p_180FPS_MODE:
            if ((f32Fps <= 180) && (f32Fps >= 0.5))
            {
                gu32FullLinesStd = (OV4689_VMAX_720P * 180) / f32Fps;
            }
            else
            {
                printf("Not support Fps: %f\n", f32Fps);
                return;
            }            
        break;

        default:
        break;
    }
    
    gu32FullLinesStd = gu32FullLinesStd > FULL_LINES_MAX ? FULL_LINES_MAX : gu32FullLinesStd;

    if (WDR_MODE_NONE == genSensorMode)
    {
        g_stSnsRegsInfo.astI2cData[5].u32Data = (gu32FullLinesStd & 0xFF);
        g_stSnsRegsInfo.astI2cData[6].u32Data = (gu32FullLinesStd & 0x7F00) >> 8;
    }
    else
    {
        g_stSnsRegsInfo.astI2cData[10].u32Data = (gu32FullLinesStd & 0xFF);
        g_stSnsRegsInfo.astI2cData[11].u32Data = (gu32FullLinesStd & 0x7F00) >> 8;
    }

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
        g_stSnsRegsInfo.astI2cData[5].u32Data = (gu32FullLines & 0xFF);
        g_stSnsRegsInfo.astI2cData[6].u32Data = (gu32FullLines & 0x7F00) >> 8;
    }
    else
    {
        g_stSnsRegsInfo.astI2cData[10].u32Data = (gu32FullLines & 0xFF);
        g_stSnsRegsInfo.astI2cData[11].u32Data = (gu32FullLines & 0x7F00) >> 8;
    }

    pstAeSnsDft->u32MaxIntTime = gu32FullLines - 4;
    
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
            au32WDRIntTime[0] = u32IntTime;
            g_stSnsRegsInfo.astI2cData[5].u32Data = u32IntTime >> 12;
            g_stSnsRegsInfo.astI2cData[6].u32Data = (u32IntTime & 0xFF0) >> 4;
            g_stSnsRegsInfo.astI2cData[7].u32Data = (u32IntTime & 0xF) << 4;
            bFirst = HI_FALSE;
        }
        else /* long exposure */
        {
            au32WDRIntTime[1] = u32IntTime;
            g_stSnsRegsInfo.astI2cData[0].u32Data = u32IntTime >> 12;
            g_stSnsRegsInfo.astI2cData[1].u32Data = (u32IntTime & 0xFF0) >> 4;
            g_stSnsRegsInfo.astI2cData[2].u32Data = (u32IntTime & 0xF) << 4;
            bFirst = HI_TRUE;
        }
    }
    else if (WDR_MODE_2To1_FRAME == genSensorMode ||WDR_MODE_2To1_FRAME_FULL_RATE ==genSensorMode)
    {
        if (bFirst) /* short exposure */
        {
            g_stSnsRegsInfo.astI2cData[0].u32Data = u32IntTime >> 12;
            g_stSnsRegsInfo.astI2cData[1].u32Data = (u32IntTime & 0xFF0) >> 4;
            g_stSnsRegsInfo.astI2cData[2].u32Data = (u32IntTime & 0xF) << 4;
            bFirst = HI_FALSE;
            
        }
        else /* long exposure */
        {
            g_stSnsRegsInfo.astI2cData[5].u32Data = u32IntTime >> 12;
            g_stSnsRegsInfo.astI2cData[6].u32Data = (u32IntTime & 0xFF0) >> 4;
            g_stSnsRegsInfo.astI2cData[7].u32Data = (u32IntTime & 0xF) << 4;
            bFirst = HI_TRUE;
        }
    }
    else
    {
        g_stSnsRegsInfo.astI2cData[0].u32Data = u32IntTime >> 12;
        g_stSnsRegsInfo.astI2cData[1].u32Data = (u32IntTime & 0xFF0) >> 4;
        g_stSnsRegsInfo.astI2cData[2].u32Data = (u32IntTime & 0xF) << 4;
        bFirst = HI_TRUE;
    }

    return;
}

static HI_U32 gain_table[520]=
{
     1024,  1032,  1040,  1048,  1056,  1064,  1072,  1080,  1088,  1096,  1104,  1112,  1120,  1128,
     1136,  1144,  1152,  1160,  1168,  1176,  1184,  1192,  1200,  1208,  1216,  1224,  1232,  1240,
     1248,  1256,  1264,  1272,  1280,  1288,  1296,  1304,  1312,  1320,  1328,  1336,  1344,  1352,
     1360,  1368,  1376,  1384,  1392,  1400,  1408,  1416,  1424,  1432,  1440,  1448,  1456,  1464,
     1472,  1480,  1488,  1496,  1504,  1512,  1520,  1528,  1536,  1544,  1552,  1560,  1568,  1576,
     1584,  1592,  1600,  1608,  1616,  1624,  1632,  1640,  1648,  1656,  1664,  1672,  1680,  1688,
     1696,  1704,  1712,  1720,  1728,  1736,  1744,  1752,  1760,  1768,  1776,  1784,  1792,  1800,
     1808,  1816,  1824,  1832,  1840,  1848,  1856,  1864,  1872,  1880,  1888,  1896,  1904,  1912,
     1920,  1928,  1936,  1944,  1952,  1960,  1968,  1976,  1984,  1992,  2000,  2008,  2016,  2024,
     2032,  2040,  2048,  2064,  2080,  2096,  2112,  2128,  2144,  2160,  2176,  2192,  2208,  2224,
     2240,  2256,  2272,  2288,  2304,  2320,  2336,  2352,  2368,  2384,  2400,  2416,  2432,  2448,
     2464,  2480,  2496,  2512,  2528,  2544,  2560,  2576,  2592,  2608,  2624,  2640,  2656,  2672,
     2688,  2704,  2720,  2736,  2752,  2768,  2784,  2800,  2816,  2832,  2848,  2864,  2880,  2896,
     2912,  2928,  2944,  2960,  2976,  2992,  3008,  3024,  3040,  3056,  3072,  3088,  3104,  3120,
     3136,  3152,  3168,  3184,  3200,  3216,  3232,  3248,  3264,  3280,  3296,  3312,  3328,  3344,
     3360,  3376,  3392,  3408,  3424,  3440,  3456,  3472,  3488,  3504,  3520,  3536,  3552,  3568,
     3584,  3600,  3616,  3632,  3648,  3664,  3680,  3696,  3712,  3728,  3744,  3760,  3776,  3792,
     3808,  3824,  3840,  3856,  3872,  3888,  3904,  3920,  3936,  3952,  3968,  3984,  4000,  4016,
     4032,  4048,  4064,  4080,  4096,  4128,  4160,  4192,  4224,  4256,  4288,  4320,  4352,  4384,
     4416,  4448,  4480,  4512,  4544,  4576,  4608,  4640,  4672,  4704,  4736,  4768,  4800,  4832,
     4864,  4896,  4928,  4960,  4992,  5024,  5056,  5088,  5120,  5152,  5184,  5216,  5248,  5280,
     5312,  5344,  5376,  5408,  5440,  5472,  5504,  5536,  5568,  5600,  5632,  5664,  5696,  5728,
     5760,  5792,  5824,  5856,  5888,  5920,  5952,  5984,  6016,  6048,  6080,  6112,  6144,  6176,
     6208,  6240,  6272,  6304,  6336,  6368,  6400,  6432,  6464,  6496,  6528,  6560,  6592,  6624,
     6656,  6688,  6720,  6752,  6784,  6816,  6848,  6880,  6912,  6944,  6976,  7008,  7040,  7072,
     7104,  7136,  7168,  7200,  7232,  7264,  7296,  7328,  7360,  7392,  7424,  7456,  7488,  7520,
     7552,  7584,  7616,  7648,  7680,  7712,  7744,  7776,  7808,  7840,  7872,  7904,  7936,  7968,
     8000,  8032,  8064,  8096,  8128,  8160,  8192,  8256,  8320,  8384,  8448,  8512,  8576,  8640,
     8704,  8768,  8832,  8896,  8960,  9024,  9088,  9152,  9216,  9280,  9344,  9408,  9472,  9536,
     9600,  9664,  9728,  9792,  9856,  9920,  9984, 10048, 10112, 10176, 10240, 10304, 10368, 10432,
     10496, 10560, 10624, 10688, 10752, 10816, 10880, 10944, 11008, 11072, 11136, 11200, 11264, 11328,
     11392, 11456, 11520, 11584, 11648, 11712, 11776, 11840, 11904, 11968, 12032, 12096, 12160, 12224,
     12288, 12352, 12416, 12480, 12544, 12608, 12672, 12736, 12800, 12864, 12928, 12992, 13056, 13120,
     13184, 13248, 13312, 13376, 13440, 13504, 13568, 13632, 13696, 13760, 13824, 13888, 13952, 14016,
     14080, 14144, 14208, 14272, 14336, 14400, 14464, 14528, 14592, 14656, 14720, 14784, 14848, 14912,
     14976, 15040, 15104, 15168, 15232, 15296, 15360, 15424, 15488, 15552, 15616, 15680, 15744, 15808,
     15872, 15936, 16000, 16064, 16128, 16192, 16256, 16320, 16384, 16448, 16512, 16576, 16640, 16704,
     16768, 16832
};


static HI_VOID cmos_again_calc_table(HI_U32 *pu32AgainLin, HI_U32 *pu32AgainDb)
{
    int i;

    if (*pu32AgainLin >= gain_table[519])
    {
         *pu32AgainLin = gain_table[519];
         *pu32AgainDb = 519;
         return ;
    }
    
    for (i = 1; i < 520; i++)
    {
        if (*pu32AgainLin < gain_table[i])
        {
            *pu32AgainLin = gain_table[i - 1];
            *pu32AgainDb = i - 1;
            break;
        }
    }

    
    return;
}

static HI_VOID cmos_gains_update(HI_U32 u32Again, HI_U32 u32Dgain)
{
    HI_U32 u32AGainReg = 0;

    if(u32Again < 128)
    {
        u32AGainReg = 0;
        u32Again += 128;    //128~255
    }
    else if(u32Again < 256)
    {
        u32AGainReg = 1;
        u32Again -= 8;      //120~247
    }
    else if(u32Again < 384)
    {
        u32AGainReg = 3;
        u32Again -= 140;    //116~243
    }
    else
    {
        u32AGainReg = 7;
        u32Again -= 264;    //120~255
    }

    g_stSnsRegsInfo.astI2cData[3].u32Data = u32AGainReg;
    g_stSnsRegsInfo.astI2cData[4].u32Data = u32Again; 
    
    if (WDR_MODE_2To1_LINE == genSensorMode)
    {
        g_stSnsRegsInfo.astI2cData[8].u32Data = g_stSnsRegsInfo.astI2cData[3].u32Data;
        g_stSnsRegsInfo.astI2cData[9].u32Data = g_stSnsRegsInfo.astI2cData[4].u32Data;
    }

    return;
}

/* Only used in WDR_MODE_2To1_LINE mode */
static HI_VOID cmos_get_inttime_max(HI_U32 u32Ratio, HI_U32 *pu32IntTimeMax)
{
    HI_U32 u32IntTimeMaxTmp;
    
    if(HI_NULL == pu32IntTimeMax)
    {
        printf("null pointer when get ae sensor IntTimeMax value!\n");
        return;
    }

    if ((WDR_MODE_2To1_FRAME_FULL_RATE == genSensorMode) || (WDR_MODE_2To1_FRAME == genSensorMode))
    {
        *pu32IntTimeMax = (gu32FullLines - 4) * 0x40 / DIV_0_TO_1(u32Ratio);
    }
    else if ((WDR_MODE_2To1_LINE == genSensorMode))
    {
        if(ISP_FSWDR_LONG_FRAME_MODE == genFSWDRMode)  /* long frame mode enable */
        {
            /* when change LongFrameMode, the first 2 frames must limit the MaxIntTime to avoid flicker */
            if(gu32MaxTimeGetCnt < 2)
            {
                u32IntTimeMaxTmp = gu32PreFullLines - 10 - 900;
            }
            else
            {
                u32IntTimeMaxTmp = gu32PreFullLines - 10 - au32WDRIntTime[0];
            }
            *pu32IntTimeMax = gu32FullLines - 10;
            *pu32IntTimeMax = (u32IntTimeMaxTmp < *pu32IntTimeMax) ? u32IntTimeMaxTmp : *pu32IntTimeMax;
            gu32MaxTimeGetCnt++;
        }
        else
        {
            /* PreShort + Long < 1PreVMax - 4  */
            /* PreShort + Long < 1V - 4  */
            /* Short + Long < 1V - 4; 
               Ratio = Long * 0x40 / Short */
            u32IntTimeMaxTmp = ((gu32PreFullLines - 50 - au32WDRIntTime[0]) << 6) / DIV_0_TO_1(u32Ratio);
            *pu32IntTimeMax = (gu32FullLines - 50) * 0x40 / (u32Ratio + 0x40);
            if(gu8SensorImageMode == SENSOR_1080P_60FPS_MODE)
            {
                *pu32IntTimeMax = (gu32FullLines - 50) * 0x40 / (u32Ratio + 0x40);
            }
            else if((gu8SensorImageMode == SENSOR_2304_1296_30FPS_MODE) || (gu8SensorImageMode == SENSOR_2048_1520_30FPS_MODE))
            {
                *pu32IntTimeMax = (gu32FullLines - 50) * 0x40 / (u32Ratio + 0x40);
            }
            *pu32IntTimeMax = (u32IntTimeMaxTmp < *pu32IntTimeMax) ? u32IntTimeMaxTmp : *pu32IntTimeMax;
        }
    }
    
    return;
}


/* Only used in FSWDR mode */
static HI_VOID cmos_ae_fswdr_attr_set(AE_FSWDR_ATTR_S *pstAeFSWDRAttr)
{
    genFSWDRMode = pstAeFSWDRAttr->enFSWDRMode;
    gu32MaxTimeGetCnt = 0;
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
    pstExpFuncs->pfn_cmos_ae_fswdr_attr_set = cmos_ae_fswdr_attr_set;

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
        case WDR_MODE_2To1_LINE:
        case WDR_MODE_2To1_FRAME:
        case WDR_MODE_2To1_FRAME_FULL_RATE:
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
   4850,
   {
        0x01bf, 0x809f, 0x8020,
        0x804e, 0x01a8, 0x805a,
        0x0010, 0x80ee, 0x01dc
   },
   
   3100,
   {
        0x01b7, 0x807c, 0x803a,
        0x807e, 0x01c0, 0x8042,
        0x001c, 0x813c, 0x021f
   },
   
   2450,
   {     
        0x0183, 0x8078, 0x800a,
        0x8073, 0x0186, 0x8013,
        0x0010, 0x820a, 0x02fa
   }
};

static AWB_AGC_TABLE_S g_stAwbAgcTable =
{
    /* bvalid */
    1,

    /* saturation */ 
    {0x80,0x80,0x7e,0x72,0x68,0x60,0x58,0x50,0x48,0x40,0x38,0x38,0x38,0x38,0x38,0x38}
};

static AWB_AGC_TABLE_S g_stAwbAgcTableFSWDR =
{
    /* bvalid */
    1,

    /* saturation */ 
    {0x80,0x80,0x80,0x78,0x70,0x68,0x58,0x48,0x40,0x38,0x38,0x38,0x38,0x38,0x38,0x38}
};

static HI_S32 cmos_get_awb_default(AWB_SENSOR_DEFAULT_S *pstAwbSnsDft)
{
    if (HI_NULL == pstAwbSnsDft)
    {
        printf("null pointer when get awb default value!\n");
        return -1;
    }

    memset(pstAwbSnsDft, 0, sizeof(AWB_SENSOR_DEFAULT_S));

    pstAwbSnsDft->u16WbRefTemp = 4850;

    pstAwbSnsDft->au16GainOffset[0] = 0x212;
    pstAwbSnsDft->au16GainOffset[1] = 0x100;
    pstAwbSnsDft->au16GainOffset[2] = 0x100;
    pstAwbSnsDft->au16GainOffset[3] = 0x1c8;

    pstAwbSnsDft->as32WbPara[0] = 63;
    pstAwbSnsDft->as32WbPara[1] = 76;
    pstAwbSnsDft->as32WbPara[2] = -117;
    pstAwbSnsDft->as32WbPara[3] = 119178;
    pstAwbSnsDft->as32WbPara[4] = 128;
    pstAwbSnsDft->as32WbPara[5] = -73499;

    memcpy(&pstAwbSnsDft->stCcm, &g_stAwbCcm, sizeof(AWB_CCM_S));

    switch (genSensorMode)
    {
        default:
        case WDR_MODE_NONE:
            memcpy(&pstAwbSnsDft->stAgcTbl, &g_stAwbAgcTable, sizeof(AWB_AGC_TABLE_S));
        break;
        case WDR_MODE_2To1_LINE:
        case WDR_MODE_2To1_FRAME:
        case WDR_MODE_2To1_FRAME_FULL_RATE:
            memcpy(&pstAwbSnsDft->stAgcTbl, &g_stAwbAgcTableFSWDR, sizeof(AWB_AGC_TABLE_S));
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
            memcpy(&pstDef->stAgcTbl, &g_IspDft[0].stAgcTbl, sizeof(ISP_CMOS_AGC_TABLE_S));
            memcpy(&pstDef->stNoiseTbl, &g_IspDft[0].stNoiseTbl, sizeof(ISP_CMOS_NOISE_TABLE_S));            
            memcpy(&pstDef->stDemosaic, &g_IspDft[0].stDemosaic, sizeof(ISP_CMOS_DEMOSAIC_S));
            memcpy(&pstDef->stRgbSharpen, &g_IspDft[0].stRgbSharpen, sizeof(ISP_CMOS_RGBSHARPEN_S));
            memcpy(&pstDef->stGamma, &g_IspDft[0].stGamma, sizeof(ISP_CMOS_GAMMA_S));
            break;
            
        case WDR_MODE_2To1_LINE:
        case WDR_MODE_2To1_FRAME:
        case WDR_MODE_2To1_FRAME_FULL_RATE:
            memcpy(&pstDef->stDrc, &g_IspDft[1].stDrc, sizeof(ISP_CMOS_DRC_S));
            memcpy(&pstDef->stAgcTbl, &g_IspDft[1].stAgcTbl, sizeof(ISP_CMOS_AGC_TABLE_S));
            memcpy(&pstDef->stNoiseTbl, &g_IspDft[1].stNoiseTbl, sizeof(ISP_CMOS_NOISE_TABLE_S));            
            memcpy(&pstDef->stDemosaic, &g_IspDft[1].stDemosaic, sizeof(ISP_CMOS_DEMOSAIC_S));
            memcpy(&pstDef->stRgbSharpen, &g_IspDft[1].stRgbSharpen, sizeof(ISP_CMOS_RGBSHARPEN_S));
            memcpy(&pstDef->stGamma, &g_IspDft[1].stGamma, sizeof(ISP_CMOS_GAMMA_S));
            memcpy(&pstDef->stGammafe, &g_IspDft[1].stGammafe, sizeof(ISP_CMOS_GAMMAFE_S));
            break;
    }

    pstDef->stSensorMaxResolution.u32MaxWidth  = 2592;
    pstDef->stSensorMaxResolution.u32MaxHeight = 1520;

    return 0;
}

#else

static ISP_CMOS_AGC_TABLE_S g_stIspAgcTable =
{
    /* bvalid */
    1,
    
    /* 100, 200, 400, 800, 1600, 3200, 6400, 12800, 25600, 51200, 102400, 204800, 409600, 819200, 1638400, 3276800 */

    /* sharpen_alt_d */
    {0x3a,0x3a,0x38,0x34,0x30,0x2e,0x28,0x24,0x20,0x1b,0x18,0x14,0x12,0x10,0x10,0x10},
        
    /* sharpen_alt_ud */
    {0x60,0x60,0x58,0x50,0x48,0x40,0x38,0x28,0x20,0x20,0x18,0x12,0x10,0x10,0x10,0x10},
        
    /* snr_thresh Max=0x54 */
    {0x08,0x0a,0x0f,0x12,0x16,0x1a,0x22,0x28,0x2e,0x36,0x3a,0x40,0x40,0x40,0x40,0x40},
        
    /* demosaic_lum_thresh */
    {0x50,0x50,0x4e,0x49,0x45,0x45,0x40,0x3a,0x3a,0x30,0x30,0x2a,0x20,0x20,0x20,0x20},
        
    /* demosaic_np_offset */
    {0x00,0x0a,0x12,0x1a,0x20,0x28,0x30,0x32,0x34,0x36,0x38,0x38,0x38,0x38,0x38,0x38},
        
    /* ge_strength */
    {0x55,0x55,0x55,0x55,0x55,0x55,0x37,0x37,0x37,0x35,0x35,0x35,0x35,0x35,0x35,0x35},

    /* rgbsharp_strength */
    {0x88,0x86,0x84,0x78,0x6a,0x60,0x58,0x50,0x40,0x30,0x20,0x16,0x12,0x12,0x12,0x12}
};

static ISP_CMOS_NOISE_TABLE_S g_stIspNoiseTable =
{
    /* bvalid */
    1,

    /* nosie_profile_weight_lut */
    {0x0, 0x0, 0x8, 0xF, 0x13, 0x16, 0x19, 0x1A, 0x1C, 0x1D, 0x1F, 0x20, 0x21, 0x22, 0x23, 0x23, 0x25, 
    0x25, 0x26, 0x27, 0x27, 0x27, 0x28, 0x29, 0x29, 0x2A, 0x2A, 0x2B, 0x2B, 0x2C, 0x2C, 0x2C, 0x2D, 
    0x2D, 0x2D, 0x2E, 0x2E, 0x2E, 0x2F, 0x2F, 0x2F, 0x30, 0x30, 0x30, 0x30, 0x31, 0x31, 0x31, 0x31, 
    0x32, 0x32, 0x32, 0x32, 0x33, 0x33, 0x33, 0x33, 0x33, 0x34, 0x34, 0x34, 0x34, 0x34, 0x35, 0x35, 0x35, 
    0x35, 0x35, 0x35, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x37, 0x37, 0x37, 0x37, 0x37, 0x37, 0x37, 0x38,
    0x38, 0x38, 0x38, 0x38, 0x38, 0x38, 0x39, 0x39, 0x39, 0x39, 0x39, 0x39, 0x39, 0x39, 0x3A, 0x3A, 
    0x3A, 0x3A, 0x3A, 0x3A, 0x3A, 0x3A, 0x3A, 0x3B, 0x3B, 0x3B, 0x3B, 0x3B, 0x3B, 0x3B, 0x3B, 0x3B, 
    0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3D, 0x3D},   

    /* demosaic_weight_lut */
    {0x8, 0xF, 0x13, 0x16, 0x19, 0x1A, 0x1C, 0x1D, 0x1F, 0x20, 0x21, 0x22, 0x23, 0x23, 0x25, 0x25, 0x26, 
    0x27, 0x27, 0x27, 0x28, 0x29, 0x29, 0x2A, 0x2A, 0x2B, 0x2B, 0x2C, 0x2C, 0x2C, 0x2D, 0x2D, 0x2D, 
    0x2E, 0x2E, 0x2E, 0x2F, 0x2F, 0x2F, 0x30, 0x30, 0x30, 0x30, 0x31, 0x31, 0x31, 0x31, 0x32, 0x32, 
    0x32, 0x32, 0x33, 0x33, 0x33, 0x33, 0x33, 0x34, 0x34, 0x34, 0x34, 0x34, 0x35, 0x35, 0x35, 0x35, 0x35, 
    0x35, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x37, 0x37, 0x37, 0x37, 0x37, 0x37, 0x37, 0x38, 0x38, 0x38, 
    0x38, 0x38, 0x38, 0x38, 0x39, 0x39, 0x39, 0x39, 0x39, 0x39, 0x39, 0x39, 0x3A, 0x3A, 0x3A, 0x3A, 
    0x3A, 0x3A, 0x3A, 0x3A, 0x3A, 0x3B, 0x3B, 0x3B, 0x3B, 0x3B, 0x3B, 0x3B, 0x3B, 0x3B, 0x3C, 0x3C, 
    0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3D, 0x3D, 0x3D, 0x3D}        
    
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
    0xbb,

    /*aa_slope*/
    0xb8,

    /*va_slope*/
    0xb4,

    /*uu_slope*/
    0xb8,

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
        case WDR_MODE_2To1_LINE:
        case WDR_MODE_2To1_FRAME:
        case WDR_MODE_2To1_FRAME_FULL_RATE:   
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

    }
    
    pstDef->stSensorMaxResolution.u32MaxWidth  = 2592;
    pstDef->stSensorMaxResolution.u32MaxHeight = 1520;
    
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
        pstBlackLevel->au16BlackLevel[i] = 0x40; /*12bit,0x40 */
    }

    return 0;    
}

HI_VOID cmos_set_pixel_detect(HI_BOOL bEnable)
{
    HI_U32 u32FullLines_5Fps, u32MaxIntTime_5Fps;
    
    if (WDR_MODE_2To1_LINE == genSensorMode)
    {   
        return;
    }
    else
    {
        if (SENSOR_4M_30FPS_MODE == gu8SensorImageMode)
        {
            u32FullLines_5Fps = (OV4689_VMAX_4M30 * 30) / 5;
        }
        else if (SENSOR_1080P_60FPS_MODE == gu8SensorImageMode)
        {
            u32FullLines_5Fps = (OV4689_VMAX_1080P * 60) / 5;
        }
        else  if (SENSOR_2304_1296_30FPS_MODE == gu8SensorImageMode)
        {
            u32FullLines_5Fps = (OV4689_VMAX_2304_1296 * 30) / 5;
        }
        else  if (SENSOR_2048_1520_30FPS_MODE == gu8SensorImageMode)
        {
            u32FullLines_5Fps = (OV4689_VMAX_2048_1520 * 30) / 5;
        }
        else  if (SENSOR_720p_180FPS_MODE == gu8SensorImageMode)
        {
            u32FullLines_5Fps = (OV4689_VMAX_720P * 180) / 5;
        }
        else
        {
            return;
        }
    }

    u32FullLines_5Fps = (u32FullLines_5Fps > FULL_LINES_MAX) ? FULL_LINES_MAX : u32FullLines_5Fps;
    u32MaxIntTime_5Fps = u32FullLines_5Fps - 4;

    if (bEnable) /* setup for ISP pixel calibration mode */
    {
        sensor_write_register (0x380F, u32FullLines_5Fps & 0xFF); 
        sensor_write_register (0x380E, (u32FullLines_5Fps & 0x7F00) >> 8); 
        sensor_write_register(0x3500, (u32MaxIntTime_5Fps>>12));
        sensor_write_register(0x3501, ((u32MaxIntTime_5Fps & 0xFF0) >> 4));
        sensor_write_register(0x3502, ((u32MaxIntTime_5Fps & 0xF) << 4));
        
        sensor_write_register(0x3508, 0x00);
        sensor_write_register(0x3509, 0x80);            
    }
    else /* setup for ISP 'normal mode' */
    {
        sensor_write_register (0x380F, gu32FullLinesStd& 0xFF); 
        sensor_write_register (0x380E, (gu32FullLinesStd & 0x7F00) >> 8); 
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

        case WDR_MODE_2To1_LINE:
            genSensorMode = WDR_MODE_2To1_LINE;
            printf("2to1 line WDR mode\n");
        break;

        case WDR_MODE_2To1_FRAME:
            genSensorMode = WDR_MODE_2To1_FRAME;
        break;

        case WDR_MODE_2To1_FRAME_FULL_RATE:
            genSensorMode = WDR_MODE_2To1_FRAME_FULL_RATE;
        break;
        
        default:
            printf("NOT support this mode!\n");
            return;
        break;
    }

    memset(au32WDRIntTime, 0, sizeof(au32WDRIntTime));
    
    return;
}

static HI_S32 cmos_set_image_mode(ISP_CMOS_SENSOR_IMAGE_MODE_S *pstSensorImageMode)
{
    HI_U8 u8SensorImageMode = gu8SensorImageMode;
    HI_U8 u8MaxFrameRate = 30;
    bInit = HI_FALSE;
    
    if (HI_NULL == pstSensorImageMode )
    {
        printf("null pointer when set image mode\n");
        return -1;
    }
    if((pstSensorImageMode->u16Width <= 1280)&&(pstSensorImageMode->u16Height <= 720))
    {
        if (pstSensorImageMode->f32Fps <= 180)
        {
            u8SensorImageMode = SENSOR_720p_180FPS_MODE;
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
        if (pstSensorImageMode->f32Fps <= 60)
        {
            u8SensorImageMode = SENSOR_1080P_60FPS_MODE;
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
    else if((pstSensorImageMode->u16Width <= 2048)&&(pstSensorImageMode->u16Height <= 1520))
    {
        if (pstSensorImageMode->f32Fps <= 30)
        {
            u8SensorImageMode = SENSOR_2048_1520_30FPS_MODE;
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
    else if((pstSensorImageMode->u16Width <= 2304)&&(pstSensorImageMode->u16Height <= 1296))
    {
        if (pstSensorImageMode->f32Fps <= 30)
        {
            u8SensorImageMode = SENSOR_2304_1296_30FPS_MODE;
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
    else if((pstSensorImageMode->u16Width <= 2592)&&(pstSensorImageMode->u16Height <= 1520))
    {
        if(WDR_MODE_2To1_LINE == genSensorMode)
        {
            u8MaxFrameRate = 25;
        }
        else
        {
            u8MaxFrameRate = 30;
        }
        if (pstSensorImageMode->f32Fps <= u8MaxFrameRate)
        {
            u8SensorImageMode = SENSOR_4M_30FPS_MODE;
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
    memset(au32WDRIntTime, 0, sizeof(au32WDRIntTime));

    return 0;
    
}

HI_U32 cmos_get_sns_regs_info(ISP_SNS_REGS_INFO_S *pstSnsRegsInfo)
{
    HI_S32 i;
    if (HI_FALSE == bInit)
    {
        g_stSnsRegsInfo.enSnsType = ISP_SNS_I2C_TYPE;
        g_stSnsRegsInfo.u8Cfg2ValidDelayMax = 2;        
        g_stSnsRegsInfo.u32RegNum = 7;
        
        if ((WDR_MODE_2To1_LINE == genSensorMode) || (WDR_MODE_2To1_FRAME == genSensorMode) || (WDR_MODE_2To1_FRAME_FULL_RATE == genSensorMode))
        {
            g_stSnsRegsInfo.u32RegNum += 5; 
        }
        
        for (i=0; i<g_stSnsRegsInfo.u32RegNum; i++)
        {
            g_stSnsRegsInfo.astI2cData[i].bUpdate = HI_TRUE;
            g_stSnsRegsInfo.astI2cData[i].u8DevAddr = sensor_i2c_addr;
            g_stSnsRegsInfo.astI2cData[i].u32AddrByteNum = sensor_addr_byte;
            g_stSnsRegsInfo.astI2cData[i].u32DataByteNum = sensor_data_byte;
        }        
        g_stSnsRegsInfo.astI2cData[0].u8DelayFrmNum = 0; 
        g_stSnsRegsInfo.astI2cData[0].u32RegAddr = 0x3500;        // Long shutter   
        g_stSnsRegsInfo.astI2cData[1].u8DelayFrmNum = 0;
        g_stSnsRegsInfo.astI2cData[1].u32RegAddr = 0x3501;        // Long shutter 
        g_stSnsRegsInfo.astI2cData[2].u8DelayFrmNum = 0;
        g_stSnsRegsInfo.astI2cData[2].u32RegAddr = 0x3502;        // Long shutter   
        g_stSnsRegsInfo.astI2cData[3].u8DelayFrmNum = 1;
        g_stSnsRegsInfo.astI2cData[3].u32RegAddr = 0x3508;        // Again
        g_stSnsRegsInfo.astI2cData[4].u8DelayFrmNum = 1; 
        g_stSnsRegsInfo.astI2cData[4].u32RegAddr = 0x3509;        // Again
        g_stSnsRegsInfo.astI2cData[5].u8DelayFrmNum = 0;
        g_stSnsRegsInfo.astI2cData[5].u32RegAddr = 0x380F;        // Vmax
        g_stSnsRegsInfo.astI2cData[6].u8DelayFrmNum = 0;
        g_stSnsRegsInfo.astI2cData[6].u32RegAddr = 0x380E;        // Vmax
        if (WDR_MODE_2To1_LINE == genSensorMode )
        {
            g_stSnsRegsInfo.astI2cData[5].u8DelayFrmNum = 0;
            g_stSnsRegsInfo.astI2cData[5].u32RegAddr = 0x350A;     // Short shutter   
            g_stSnsRegsInfo.astI2cData[6].u8DelayFrmNum = 0;
            g_stSnsRegsInfo.astI2cData[6].u32RegAddr = 0x350B;     // Short shutter
            g_stSnsRegsInfo.astI2cData[7].u8DelayFrmNum = 0;
            g_stSnsRegsInfo.astI2cData[7].u32RegAddr = 0x350C;     // Short shutter
            g_stSnsRegsInfo.astI2cData[8].u8DelayFrmNum = 1;
            g_stSnsRegsInfo.astI2cData[8].u32RegAddr = 0x350E;     // Again 
            g_stSnsRegsInfo.astI2cData[9].u8DelayFrmNum = 1;
            g_stSnsRegsInfo.astI2cData[9].u32RegAddr = 0x350F;     // Again
            g_stSnsRegsInfo.astI2cData[10].u8DelayFrmNum = 2;
            g_stSnsRegsInfo.astI2cData[10].u32RegAddr = 0x380F;    // Vmax
            g_stSnsRegsInfo.astI2cData[11].u8DelayFrmNum = 2;
            g_stSnsRegsInfo.astI2cData[11].u32RegAddr = 0x380E;    // Vmax
        }    
        else if ((WDR_MODE_2To1_FRAME == genSensorMode) || (WDR_MODE_2To1_FRAME_FULL_RATE == genSensorMode))
        {
            g_stSnsRegsInfo.astI2cData[5].u8DelayFrmNum = 1;
            g_stSnsRegsInfo.astI2cData[5].u32RegAddr = 0x3500;
            g_stSnsRegsInfo.astI2cData[6].u8DelayFrmNum = 1;
            g_stSnsRegsInfo.astI2cData[6].u32RegAddr = 0x3501;
            g_stSnsRegsInfo.astI2cData[7].u8DelayFrmNum = 1;
            g_stSnsRegsInfo.astI2cData[7].u32RegAddr = 0x3502;
            g_stSnsRegsInfo.astI2cData[8].u8DelayFrmNum = 1;
            g_stSnsRegsInfo.astI2cData[8].u32RegAddr = 0x350E;
            g_stSnsRegsInfo.astI2cData[9].u8DelayFrmNum = 1;
            g_stSnsRegsInfo.astI2cData[9].u32RegAddr = 0x350F;
            g_stSnsRegsInfo.astI2cData[10].u8DelayFrmNum = 0;
            g_stSnsRegsInfo.astI2cData[10].u32RegAddr = 0x380F;
            g_stSnsRegsInfo.astI2cData[11].u8DelayFrmNum = 0;
            g_stSnsRegsInfo.astI2cData[11].u32RegAddr = 0x380E;            
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

        if ((WDR_MODE_2To1_FRAME_FULL_RATE == genSensorMode) || (WDR_MODE_2To1_FRAME == genSensorMode))
        {
            g_stSnsRegsInfo.astI2cData[0].bUpdate = HI_TRUE;
            g_stSnsRegsInfo.astI2cData[1].bUpdate = HI_TRUE;
            g_stSnsRegsInfo.astI2cData[2].bUpdate = HI_TRUE;
            g_stSnsRegsInfo.astI2cData[5].bUpdate = HI_TRUE;
            g_stSnsRegsInfo.astI2cData[6].bUpdate = HI_TRUE;
            g_stSnsRegsInfo.astI2cData[7].bUpdate = HI_TRUE;
        }
    }
    
    if (HI_NULL == pstSnsRegsInfo)
    {
        printf("null pointer when get sns reg info!\n");
        return -1;
    }

    if (WDR_MODE_2To1_LINE == genSensorMode )
    {
        if(gu32PreFullLines > gu32FullLines)
        {
            g_stSnsRegsInfo.astI2cData[10].u8DelayFrmNum = 2;
            g_stSnsRegsInfo.astI2cData[11].u8DelayFrmNum = 2;
        }
        else if(gu32PreFullLines < gu32FullLines)
        {
            g_stSnsRegsInfo.astI2cData[10].u8DelayFrmNum = 1;
            g_stSnsRegsInfo.astI2cData[11].u8DelayFrmNum = 1;
        }
        else
        {
            //do nothing
        }
    }
    
    memcpy(pstSnsRegsInfo, &g_stSnsRegsInfo, sizeof(ISP_SNS_REGS_INFO_S)); 
    memcpy(&g_stPreSnsRegsInfo, &g_stSnsRegsInfo, sizeof(ISP_SNS_REGS_INFO_S)); 

    gu32PreFullLines = gu32FullLines;

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
    gu8SensorImageMode = SENSOR_4M_30FPS_MODE;
    genSensorMode = WDR_MODE_NONE;
    gu32FullLinesStd = 1632; 
    gu32FullLines = 1632;
    gu32PreFullLines = 1632;
    bInit = HI_FALSE;
    bSensorInit = HI_FALSE; 
    genFSWDRMode = ISP_FSWDR_NORMAL_MODE;

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
    s32Ret = HI_MPI_ISP_SensorRegCallBack(IspDev, OV4689_ID, &stIspRegister);
    if (s32Ret)
    {
        printf("sensor register callback function failed!\n");
        return s32Ret;
    }
    
    stLib.s32Id = 0;
    strncpy(stLib.acLibName, HI_AE_LIB_NAME, sizeof(HI_AE_LIB_NAME));
    cmos_init_ae_exp_function(&stAeRegister.stSnsExp);
    s32Ret = HI_MPI_AE_SensorRegCallBack(IspDev, &stLib, OV4689_ID, &stAeRegister);
    if (s32Ret)
    {
        printf("sensor register callback function to ae lib failed!\n");
        return s32Ret;
    }

    stLib.s32Id = 0;
    strncpy(stLib.acLibName, HI_AWB_LIB_NAME, sizeof(HI_AWB_LIB_NAME));
    cmos_init_awb_exp_function(&stAwbRegister.stSnsExp);
    s32Ret = HI_MPI_AWB_SensorRegCallBack(IspDev, &stLib, OV4689_ID, &stAwbRegister);
    if (s32Ret)
    {
        printf("sensor register callback function to ae lib failed!\n");
        return s32Ret;
    }
    
    return 0;
}

int sensor_unregister_callback(void)
{
    ISP_DEV IspDev = 0;
    HI_S32 s32Ret;
    ALG_LIB_S stLib;

    s32Ret = HI_MPI_ISP_SensorUnRegCallBack(IspDev, OV4689_ID);
    if (s32Ret)
    {
        printf("sensor unregister callback function failed!\n");
        return s32Ret;
    }
    
    stLib.s32Id = 0;
    strncpy(stLib.acLibName, HI_AE_LIB_NAME, sizeof(HI_AE_LIB_NAME));
    s32Ret = HI_MPI_AE_SensorUnRegCallBack(IspDev, &stLib, OV4689_ID);
    if (s32Ret)
    {
        printf("sensor unregister callback function to ae lib failed!\n");
        return s32Ret;
    }

    stLib.s32Id = 0;
    strncpy(stLib.acLibName, HI_AWB_LIB_NAME, sizeof(HI_AWB_LIB_NAME));
    s32Ret = HI_MPI_AWB_SensorUnRegCallBack(IspDev, &stLib, OV4689_ID);
    if (s32Ret)
    {
        printf("sensor unregister callback function to ae lib failed!\n");
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
