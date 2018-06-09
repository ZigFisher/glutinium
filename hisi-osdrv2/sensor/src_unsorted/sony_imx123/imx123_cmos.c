#if !defined(__IMX123_CMOS_H_)
#define __IMX123_CMOS_H_

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

#define IMX123_ID 123

#define FIXED_RHS1_MODE 1

/* To change the mode of config. ifndef INIFILE_CONFIG_MODE, quick config mode.*/
/* else, cmos_cfg.ini file config mode*/
#ifdef INIFILE_CONFIG_MODE

extern AE_SENSOR_DEFAULT_S  g_AeDft[];
extern AWB_SENSOR_DEFAULT_S g_AwbDft[];
extern ISP_CMOS_DEFAULT_S   g_IspDft[];
extern HI_S32 Cmos_LoadINIPara(const HI_CHAR *pcName);
#else

#endif

#define FULL_LINES_MAX  (0x1FFFF)

/* AE default parameter and function */
/* 2to1 WDR 60->30fps*/

static ISP_AE_ROUTE_S gstAERouteAttr_30Fps = 
{
    5,

    {
        {5,   1024,   1},
        {140, 1024,   1},
        {140, 8134,   1},
        {2000, 8134,   1},
        {2000, 65072, 1}
    }
};


/* 2to1 WDR 50->25fps*/

static ISP_AE_ROUTE_S gstAERouteAttr_25Fps = 
{
    5,

    {
        {2,   1024,   1},
        {140, 1024,   1},
        {140, 8134,   1},
        {2000, 8134,   1},
        {2000, 65072, 1}
    }
};

/****************************************************************************
 * local variables                                                            *
 ****************************************************************************/

extern const unsigned int sensor_i2c_addr;
extern unsigned int sensor_addr_byte;
extern unsigned int sensor_data_byte;

//Full line std
#define INCREASE_LINES (1) /* make real fps less than stand fps because NVR require*/

#define IMX123_VMAX_1080P_60FPS_LINEAR  (1125+INCREASE_LINES)    // 1080p60
#define IMX123_VMAX_QXGA_30FPS_LINEAR   (0x672+INCREASE_LINES)   // 1650 QXGA 30fps
#define IMX123_VMAX_QXGA_60FPS_LINEAR   (0x708+INCREASE_LINES)   // 1536p60 ;Frame switching WDR
#define IMX123_VMAX_QXGA_30FPS_DOL_WDR  (0x708+INCREASE_LINES)   // 1800

#define SENSOR_1080P_60FPS_MODE   (0)
#define SENSOR_1080P_30FPS_MODE   (1)
#define SENSOR_QXGA_30FPS_MODE    (2)
#define SENSOR_QXGA_60FPS_MODE    (3)

#define SHS1_ADDR (0x21E) 
#define SHS2_ADDR (0x221) 
#define RHS1_ADDR (0x22e) 
#define GAIN_ADDR (0x214)
#define VMAX_ADDR (0x218)

// hard limitation of RHS1 location Right Limit
        // 4n + 10
        // RHS1 <= FSC - BRL*2 -11
static HI_U32 gu32RHS1_LocRLimit = (2* IMX123_VMAX_QXGA_30FPS_DOL_WDR - 2*1564 - 11) - (((2* IMX123_VMAX_QXGA_30FPS_DOL_WDR - 2* 1564 - 11) - 10 ) %4) ;

HI_U8 gu8SensorImageMode = SENSOR_QXGA_30FPS_MODE;
WDR_MODE_E genSensorMode = WDR_MODE_NONE;

static HI_U32 gu32FullLinesStd = IMX123_VMAX_QXGA_30FPS_LINEAR; 
static HI_U32 gu32FullLines = IMX123_VMAX_QXGA_30FPS_LINEAR;
static HI_U32 gu32PreFullLines = IMX123_VMAX_QXGA_30FPS_LINEAR;
static HI_U32 gu32RHS1 = 0xE6;
static HI_BOOL bInit = HI_FALSE;
HI_BOOL bSensorInit = HI_FALSE; 
static ISP_FSWDR_MODE_E genFSWDRMode = ISP_FSWDR_NORMAL_MODE;
static HI_U32 gu32MaxTimeGetCnt = 0;

ISP_SNS_REGS_INFO_S g_stSnsRegsInfo = {0};
ISP_SNS_REGS_INFO_S g_stPreSnsRegsInfo = {0};

static HI_U32 au32WDRIntTime[2] = {0};

#define PATHLEN_MAX 256
#define CMOS_CFG_INI "imx123_cfg.ini"
static char pcName[PATHLEN_MAX] = "configs/imx123_cfg.ini";

/* AE default parameter and function */
#ifdef INIFILE_CONFIG_MODE
static HI_S32 cmos_get_ae_default(AE_SENSOR_DEFAULT_S *pstAeSnsDft)
{
    if (HI_NULL == pstAeSnsDft)
    {
        printf("null pointer when get ae default value!\n");
        return -1;
    }
   
    if( ( SENSOR_1080P_60FPS_MODE == gu8SensorImageMode) || (SENSOR_QXGA_60FPS_MODE == gu8SensorImageMode) )
    {
        pstAeSnsDft->u32LinesPer500ms = gu32FullLinesStd * 60 / 2;
    }        
    else
    {
        pstAeSnsDft->u32LinesPer500ms = gu32FullLinesStd * 30 / 2;
    }

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
             pstAeSnsDft->u32MinIntTime = 3;
             pstAeSnsDft->u32MaxIntTimeTarget = g_AeDft[0].u32MaxIntTimeTarget;
             pstAeSnsDft->u32MinIntTimeTarget = g_AeDft[0].u32MinIntTimeTarget;
             
             pstAeSnsDft->u32MaxAgain = 363329; 
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
 
         case WDR_MODE_2To1_LINE:
             pstAeSnsDft->au8HistThresh[0] = 0xC;
             pstAeSnsDft->au8HistThresh[1] = 0x18;
             pstAeSnsDft->au8HistThresh[2] = 0x60;
             pstAeSnsDft->au8HistThresh[3] = 0x80;
            
             pstAeSnsDft->u8AeCompensation = g_AeDft[1].u8AeCompensation;
             pstAeSnsDft->u16ManRatioEnable = HI_TRUE;
             pstAeSnsDft->u32Ratio = 0x400;
                
             pstAeSnsDft->u32MaxIntTime = 447;  
             pstAeSnsDft->u32MinIntTime = 5;
             pstAeSnsDft->u32MaxIntTimeTarget = g_AeDft[1].u32MaxIntTimeTarget;
             pstAeSnsDft->u32MinIntTimeTarget = g_AeDft[1].u32MinIntTimeTarget;

             pstAeSnsDft->u32LinesPer500ms = 2 * IMX123_VMAX_QXGA_30FPS_DOL_WDR * 30 / 2;
             pstAeSnsDft->u32MaxAgain = 363329;
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
        case WDR_MODE_2To1_FRAME:
        case WDR_MODE_2To1_FRAME_FULL_RATE: /*linear mode for ISP frame switching WDR*/
            pstAeSnsDft->au8HistThresh[0] = 0xC;
            pstAeSnsDft->au8HistThresh[1] = 0x18;
            pstAeSnsDft->au8HistThresh[2] = 0x60;
            pstAeSnsDft->au8HistThresh[3] = 0x80;
           
            pstAeSnsDft->u8AeCompensation = g_AeDft[2].u8AeCompensation;
            
            pstAeSnsDft->u32MaxIntTime = gu32FullLinesStd - 4;
            pstAeSnsDft->u32MinIntTime = 3;
            pstAeSnsDft->u32MaxIntTimeTarget = g_AeDft[2].u32MaxIntTimeTarget;
            pstAeSnsDft->u32MinIntTimeTarget = g_AeDft[2].u32MinIntTimeTarget;
            
            pstAeSnsDft->u32MaxAgain = 363329;  
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

    if( ( SENSOR_1080P_60FPS_MODE == gu8SensorImageMode) || (SENSOR_QXGA_60FPS_MODE == gu8SensorImageMode) )
    {
        pstAeSnsDft->u32LinesPer500ms = gu32FullLinesStd * 60 / 2;
    }        
    else
    {
        pstAeSnsDft->u32LinesPer500ms = gu32FullLinesStd * 30 / 2;
    }    
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
    pstAeSnsDft->u32MaxISPDgainTarget = 8 << pstAeSnsDft->u32ISPDgainShift; 

    pstAeSnsDft->u32MaxAgain = 1446438;  
    pstAeSnsDft->u32MinAgain = 1024;
    pstAeSnsDft->u32MaxAgainTarget = pstAeSnsDft->u32MaxAgain;
    pstAeSnsDft->u32MinAgainTarget = pstAeSnsDft->u32MinAgain;

    pstAeSnsDft->u32MaxDgain = 1;  
    pstAeSnsDft->u32MinDgain = 1;
    pstAeSnsDft->u32MaxDgainTarget = pstAeSnsDft->u32MaxDgain;
    pstAeSnsDft->u32MinDgainTarget = pstAeSnsDft->u32MinDgain;

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

            pstAeSnsDft->u32MaxIntTime = gu32FullLinesStd - 4;
            pstAeSnsDft->u32MinIntTime = 3;
            pstAeSnsDft->u32MaxIntTimeTarget = 0xffff;
            pstAeSnsDft->u32MinIntTimeTarget = pstAeSnsDft->u32MinIntTime;
            
            pstAeSnsDft->u8AeCompensation = 0x38;
        break;

        case WDR_MODE_2To1_FRAME:
        case WDR_MODE_2To1_FRAME_FULL_RATE:
            pstAeSnsDft->au8HistThresh[0] = 0xC;
            pstAeSnsDft->au8HistThresh[1] = 0x18;
            pstAeSnsDft->au8HistThresh[2] = 0x60;
            pstAeSnsDft->au8HistThresh[3] = 0x80;

            pstAeSnsDft->u32MaxIntTime = gu32FullLinesStd - 4;
            pstAeSnsDft->u32MinIntTime = 3;
            pstAeSnsDft->u32MaxIntTimeTarget = 0xffff;
            pstAeSnsDft->u32MinIntTimeTarget = pstAeSnsDft->u32MinIntTime;
            
            pstAeSnsDft->u8AeCompensation = 0x38;
        break;
  
        case WDR_MODE_2To1_LINE:
            pstAeSnsDft->au8HistThresh[0] = 0xC;
            pstAeSnsDft->au8HistThresh[1] = 0x18;
            pstAeSnsDft->au8HistThresh[2] = 0x60;
            pstAeSnsDft->au8HistThresh[3] = 0x80;

            pstAeSnsDft->u32MaxIntTime = 447; // short exposure time can reach the maximum value when @30fps
            pstAeSnsDft->u32MinIntTime = 5;
            pstAeSnsDft->u32MaxIntTimeTarget = 0xffff;
            pstAeSnsDft->u32MinIntTimeTarget = pstAeSnsDft->u32MinIntTime;

            pstAeSnsDft->u32MaxAgain = 363329;  
            pstAeSnsDft->u32MinAgain = 1024;
            pstAeSnsDft->u32MaxAgainTarget = 8134;  // sensor again's noise is bigger than isp dgain
            pstAeSnsDft->u32MinAgainTarget = pstAeSnsDft->u32MinAgain;

            if(ISP_FSWDR_LONG_FRAME_MODE == genFSWDRMode)
            {
                pstAeSnsDft->u8AeCompensation = 0x64;
                pstAeSnsDft->u32MaxIntTime = gu32FullLinesStd - 2;
                pstAeSnsDft->u32MaxAgain = 1446438;  
                pstAeSnsDft->u32MaxAgainTarget = pstAeSnsDft->u32MaxAgain;
            }
            else
            {
                pstAeSnsDft->u8AeCompensation = 0x38;
                pstAeSnsDft->u16ManRatioEnable = HI_TRUE;
                pstAeSnsDft->u32Ratio = 0x400;   

                if (IMX123_VMAX_QXGA_30FPS_DOL_WDR == gu32FullLinesStd)
                {
                    memcpy(&pstAeSnsDft->stAERouteAttr, &gstAERouteAttr_30Fps, sizeof(ISP_AE_ROUTE_S));
                }
                else if( (IMX123_VMAX_QXGA_30FPS_DOL_WDR * 30 /25) == gu32FullLinesStd)
                {
                    memcpy(&pstAeSnsDft->stAERouteAttr, &gstAERouteAttr_25Fps, sizeof(ISP_AE_ROUTE_S));
                }
            }
        break;
    }
    
    return 0;
}

#endif

/* the function of sensor set fps */
static HI_VOID cmos_fps_set(HI_FLOAT f32Fps, AE_SENSOR_DEFAULT_S *pstAeSnsDft)
{
    HI_U32 u32VMAX = IMX123_VMAX_QXGA_30FPS_LINEAR;

    switch (gu8SensorImageMode)
    {        
        case SENSOR_QXGA_30FPS_MODE:
            if ((f32Fps <= 30) && (f32Fps >= 2))
            {
                if(WDR_MODE_2To1_LINE == genSensorMode)
                {
                    u32VMAX = IMX123_VMAX_QXGA_30FPS_DOL_WDR * 30 / f32Fps;
                    u32VMAX = (u32VMAX > FULL_LINES_MAX) ? FULL_LINES_MAX : u32VMAX;
                    gu32FullLinesStd = u32VMAX * 2;
                    
                    // RHS1 Constraint:
                    // RHS1 = 4n +10
                    // RHS1 <= 2*VMAX - 2*BRL - 11
                    gu32RHS1_LocRLimit = (2* u32VMAX - 2*1564 - 11) - (((2* u32VMAX - 2* 1564 - 11) - 10 ) %4);

                    if(ISP_FSWDR_NORMAL_MODE == genFSWDRMode)
                    {
                        if (25 >= f32Fps)
                        {
                            memcpy(&pstAeSnsDft->stAERouteAttr, &gstAERouteAttr_25Fps, sizeof(ISP_AE_ROUTE_S));
                        }
                        else
                        {
                            memcpy(&pstAeSnsDft->stAERouteAttr, &gstAERouteAttr_30Fps, sizeof(ISP_AE_ROUTE_S));
                        } 
                    }
                }
                else
                {
                    gu32FullLinesStd = (IMX123_VMAX_QXGA_30FPS_LINEAR * 30) / f32Fps;
                    gu32FullLinesStd = (gu32FullLinesStd > FULL_LINES_MAX) ? FULL_LINES_MAX : gu32FullLinesStd;
                    u32VMAX = gu32FullLinesStd;
                }
            }
            else
            {
                printf("Not support Fps: %f\n", f32Fps);
                return;
            }            
        break;

        case SENSOR_QXGA_60FPS_MODE:        // half rate 2to1 frame wdr
            if ((f32Fps <= 60) && (f32Fps >= 0.5))
            {
                gu32FullLinesStd = (IMX123_VMAX_QXGA_60FPS_LINEAR * 60) / f32Fps;
                gu32FullLinesStd = (gu32FullLinesStd > FULL_LINES_MAX) ? FULL_LINES_MAX : gu32FullLinesStd;
                u32VMAX = gu32FullLinesStd;
            }
            else
            {
                printf("Not support Fps: %f\n", f32Fps);
                return;
            }            

        break;
        
        case SENSOR_1080P_60FPS_MODE:
            if ((f32Fps <= 60) && (f32Fps >= 0.5))
            {
                gu32FullLinesStd = (IMX123_VMAX_1080P_60FPS_LINEAR * 60) / f32Fps;
                gu32FullLinesStd = (gu32FullLinesStd > FULL_LINES_MAX) ? FULL_LINES_MAX : gu32FullLinesStd;
                u32VMAX = gu32FullLinesStd;
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

    if (WDR_MODE_NONE == genSensorMode)
    {
        g_stSnsRegsInfo.astSspData[5].u32Data = (u32VMAX & 0xFF);
        g_stSnsRegsInfo.astSspData[6].u32Data = ((u32VMAX & 0xFF00) >> 8);
        g_stSnsRegsInfo.astSspData[7].u32Data = ((u32VMAX & 0x10000) >> 16);
    }
    else
    {
        g_stSnsRegsInfo.astSspData[8].u32Data = (u32VMAX & 0xFF);
        g_stSnsRegsInfo.astSspData[9].u32Data = ((u32VMAX & 0xFF00) >> 8);
        g_stSnsRegsInfo.astSspData[10].u32Data = ((u32VMAX & 0x10000) >> 16);
    }
    
    if( ( WDR_MODE_NONE== genSensorMode ) || (WDR_MODE_2To1_FRAME == genSensorMode) ||
        ( WDR_MODE_2To1_FRAME_FULL_RATE == genSensorMode) )
    {
        pstAeSnsDft->u32MaxIntTime = gu32FullLinesStd - 4;
    }
    else if (WDR_MODE_2To1_LINE == genSensorMode)
    {
        pstAeSnsDft->u32MaxIntTime = gu32RHS1_LocRLimit - 11;
    }
    else
    {
    }
    
    pstAeSnsDft->f32Fps = f32Fps;
    pstAeSnsDft->u32LinesPer500ms = gu32FullLinesStd * f32Fps / 2;
    pstAeSnsDft->u32FullLinesStd = gu32FullLinesStd;
    gu32FullLines = gu32FullLinesStd;
    pstAeSnsDft->u32FullLines = gu32FullLines;

    return;
}

// not support slow frameRate in WDR mode
static HI_VOID cmos_slow_framerate_set(HI_U32 u32FullLines,
    AE_SENSOR_DEFAULT_S *pstAeSnsDft)
{
    u32FullLines = (u32FullLines > FULL_LINES_MAX) ? FULL_LINES_MAX : u32FullLines;
    gu32FullLines = (u32FullLines >> 1) << 1;
    pstAeSnsDft->u32FullLines = gu32FullLines;

    if (WDR_MODE_NONE == genSensorMode)
    {
        g_stSnsRegsInfo.astSspData[5].u32Data = (u32FullLines & 0xFF);
        g_stSnsRegsInfo.astSspData[6].u32Data = ((u32FullLines & 0xFF00) >> 8);
        g_stSnsRegsInfo.astSspData[7].u32Data = ((u32FullLines & 0x10000) >> 16);
    }
    else if ((WDR_MODE_2To1_FRAME == genSensorMode) || ( WDR_MODE_2To1_FRAME_FULL_RATE == genSensorMode))
    {
        g_stSnsRegsInfo.astSspData[8].u32Data = (u32FullLines & 0xFF);
        g_stSnsRegsInfo.astSspData[9].u32Data = ((u32FullLines & 0xFF00) >> 8);
        g_stSnsRegsInfo.astSspData[10].u32Data = ((u32FullLines & 0x10000) >> 16);
    }
    else if (WDR_MODE_2To1_LINE == genSensorMode)
    {
        g_stSnsRegsInfo.astSspData[8].u32Data = ((u32FullLines >> 1) & 0xFF);
        g_stSnsRegsInfo.astSspData[9].u32Data = (((u32FullLines >> 1) & 0xFF00) >> 8);
        g_stSnsRegsInfo.astSspData[10].u32Data = (((u32FullLines >> 1) & 0x10000) >> 16);
    }

    if(( WDR_MODE_NONE== genSensorMode ) || (WDR_MODE_2To1_FRAME == genSensorMode) ||
       ( WDR_MODE_2To1_FRAME_FULL_RATE == genSensorMode))
    {
        pstAeSnsDft->u32MaxIntTime = gu32FullLines - 4;
    }
    else if (WDR_MODE_2To1_LINE == genSensorMode)
    {
        gu32RHS1_LocRLimit = (u32FullLines - 2*1564 - 11) - (((u32FullLines - 2* 1564 - 11) - 10 ) %4);
        pstAeSnsDft->u32MaxIntTime = gu32RHS1_LocRLimit - 11;
    }
    else
    {
    }

    return;
}


/* while isp notify ae to update sensor regs, ae call these funcs. */
static HI_VOID cmos_inttime_update(HI_U32 u32IntTime)
{
    static HI_BOOL bFirst = HI_TRUE;
    HI_U32 u32Value = 0;

    static HI_U32 u32ShortIntTime;
    static HI_U32 u32LongIntTime;
    HI_U32 u32SHS1;
    HI_U32 u32SHS2;

    if (WDR_MODE_2To1_LINE == genSensorMode)
    {
        if (bFirst) /* short exposure */
        {
            au32WDRIntTime[0] = u32IntTime;
            u32ShortIntTime = u32IntTime;
            bFirst = HI_FALSE;
        }
        else /* long exposure */
        {
            au32WDRIntTime[1] = u32IntTime;
            u32LongIntTime = u32IntTime;
#if FIXED_RHS1_MODE
            u32SHS2 = gu32PreFullLines - u32LongIntTime;
            u32SHS1 = gu32RHS1 - u32ShortIntTime;
#else
        //allocate the RHS1
        if( 0 == (u32ShortIntTime +11 -10) %4 )      
        {
            gu32RHS1 = u32ShortIntTime +11;
        }
        else
        {
            gu32RHS1 = u32ShortIntTime +11 + (4 - (u32ShortIntTime +11 -10) %4);          // right shif the gu32RHS1
        }
            
        u32SHS2 = gu32PreFullLines - u32LongIntTime;
        u32SHS1 = gu32RHS1 - u32ShortIntTime;    
#endif
        g_stSnsRegsInfo.astSspData[0].u32Data = (u32SHS1 & 0xFF);
        g_stSnsRegsInfo.astSspData[1].u32Data = ((u32SHS1 & 0xFF00) >> 8);
        g_stSnsRegsInfo.astSspData[2].u32Data = ((u32SHS1 & 0x10000) >> 16);

        g_stSnsRegsInfo.astSspData[5].u32Data = (u32SHS2 & 0xFF);
        g_stSnsRegsInfo.astSspData[6].u32Data = ((u32SHS2 & 0xFF00) >> 8);
        g_stSnsRegsInfo.astSspData[7].u32Data = ((u32SHS2 & 0x10000) >> 16);

        g_stSnsRegsInfo.astSspData[11].u32Data = (gu32RHS1 & 0xFF);
        g_stSnsRegsInfo.astSspData[12].u32Data = ((gu32RHS1 & 0xFF00) >> 8);
        g_stSnsRegsInfo.astSspData[13].u32Data = ((gu32RHS1 & 0xF0000) >> 16);

        bFirst = HI_TRUE;
        }
    }
    else if (WDR_MODE_2To1_FRAME == genSensorMode ||WDR_MODE_2To1_FRAME_FULL_RATE ==genSensorMode)
    {
        if (bFirst) /* short exposure */
        {
            bFirst = HI_FALSE;
            u32Value = gu32FullLines - u32IntTime;
            g_stSnsRegsInfo.astSspData[0].u32Data = u32Value & 0xFF;
            g_stSnsRegsInfo.astSspData[1].u32Data = (u32Value & 0xFF00) >> 8;
            g_stSnsRegsInfo.astSspData[2].u32Data = (u32Value & 0x10000) >> 16;
            
        }
        else /* long exposure */
        {
            bFirst = HI_TRUE;
            u32Value = gu32FullLines - u32IntTime;
            g_stSnsRegsInfo.astSspData[5].u32Data = u32Value & 0xFF;
            g_stSnsRegsInfo.astSspData[6].u32Data = (u32Value & 0xFF00) >> 8;
            g_stSnsRegsInfo.astSspData[7].u32Data = (u32Value & 0x10000) >> 16; 
        }
    }
    else
    {        
        u32Value = gu32FullLines - u32IntTime;
        g_stSnsRegsInfo.astSspData[0].u32Data = (u32Value & 0xFF);
        g_stSnsRegsInfo.astSspData[1].u32Data = ((u32Value & 0xFF00) >> 8);
        g_stSnsRegsInfo.astSspData[2].u32Data = ((u32Value & 0x10000) >> 16);
        bFirst = HI_TRUE;
    }

    return;
}

static HI_U32 gain_table[631]=
{
   1024,   1036,   1048,   1060,   1072,   1085,   1097,   1110,   1123,   1136,   1149,   1162,   1176,   1189,
   1203,   1217,   1231,   1245,   1260,   1274,   1289,   1304,   1319,   1334,   1350,   1366,   1381,   1397,
   1414,   1430,   1446,   1463,   1480,   1497,   1515,   1532,   1550,   1568,   1586,   1604,   1623,   1642,
   1661,   1680,   1699,   1719,   1739,   1759,   1780,   1800,   1821,   1842,   1863,   1885,   1907,   1929,
   1951,   1974,   1997,   2020,   2043,   2067,   2091,   2115,   2139,   2164,   2189,   2215,   2240,   2266,
   2292,   2319,   2346,   2373,   2400,   2428,   2456,   2485,   2514,   2543,   2572,   2602,   2632,   2663,
   2693,   2725,   2756,   2788,   2820,   2853,   2886,   2919,   2953,   2987,   3022,   3057,   3092,   3128,
   3164,   3201,   3238,   3276,   3314,   3352,   3391,   3430,   3470,   3510,   3551,   3592,   3633,   3675,
   3718,   3761,   3805,   3849,   3893,   3938,   3984,   4030,   4077,   4124,   4172,   4220,   4269,   4318,
   4368,   4419,   4470,   4522,   4574,   4627,   4681,   4735,   4790,   4845,   4901,   4958,   5015,   5073,
   5132,   5192,   5252,   5313,   5374,   5436,   5499,   5563,   5627,   5692,   5758,   5825,   5893,   5961,
   6030,   6100,   6170,   6242,   6314,   6387,   6461,   6536,   6611,   6688,   6766,   6844,   6923,   7003,
   7084,   7166,   7249,   7333,   7418,   7504,   7591,   7679,   7768,   7858,   7949,   8041,   8134,   8228,
   8323,   8420,   8517,   8616,   8716,   8817,   8919,   9022,   9126,   9232,   9339,   9447,   9557,   9667,
   9779,   9892,  10007,  10123,  10240,  10359,  10479,  10600,  10723,  10847,  10972,  11099,  11228,  11358,
  11489,  11623,  11757,  11893,  12031,  12170,  12311,  12454,  12598,  12744,  12891,  13041,  13192,  13344,
  13499,  13655,  13813,  13973,  14135,  14299,  14464,  14632,  14801,  14973,  15146,  15321,  15499,  15678,
  15860,  16044,  16229,  16417,  16607,  16800,  16994,  17191,  17390,  17591,  17795,  18001,  18210,  18420,
  18634,  18850,  19068,  19289,  19512,  19738,  19966,  20198,  20431,  20668,  20907,  21149,  21394,  21642,
  21893,  22146,  22403,  22662,  22925,  23190,  23458,  23730,  24005,  24283,  24564,  24848,  25136,  25427,
  25722,  26020,  26321,  26626,  26934,  27246,  27561,  27880,  28203,  28530,  28860,  29194,  29532,  29874,
  30220,  30570,  30924,  31282,  31645,  32011,  32382,  32757,  33136,  33520,  33908,  34300,  34698,  35099,
  35506,  35917,  36333,  36754,  37179,  37610,  38045,  38486,  38931,  39382,  39838,  40300,  40766,  41238,
  41716,  42199,  42687,  43182,  43682,  44188,  44699,  45217,  45740,  46270,  46806,  47348,  47896,  48451,
  49012,  49579,  50153,  50734,  51322,  51916,  52517,  53125,  53740,  54363,  54992,  55629,  56273,  56925,
  57584,  58251,  58925,  59607,  60298,  60996,  61702,  62417,  63139,  63870,  64610,  65358,  66115,  66881,
  67655,  68438,  69231,  70033,  70843,  71664,  72494,  73333,  74182,  75041,  75910,  76789,  77678,  78578,
  79488,  80408,  81339,  82281,  83234,  84198,  85173,  86159,  87157,  88166,  89187,  90219,  91264,  92321,
  93390,  94471,  95565,  96672,  97791,  98924, 100069, 101228, 102400, 103586, 104785, 105999, 107226, 108468,
 109724, 110994, 112279, 113580, 114895, 116225, 117571, 118932, 120310, 121703, 123112, 124537, 125980, 127438,
 128914, 130407, 131917, 133444, 134989, 136553, 138134, 139733, 141351, 142988, 144644, 146319, 148013, 149727,
 151461, 153215, 154989, 156783, 158599, 160435, 162293, 164172, 166073, 167996, 169942, 171910, 173900, 175914,
 177951, 180011, 182096, 184204, 186337, 188495, 190678, 192886, 195119, 197379, 199664, 201976, 204315, 206681,
 209074, 211495, 213944, 216421, 218927, 221462, 224027, 226621, 229245, 231900, 234585, 237301, 240049, 242829,
 245640, 248485, 251362, 254273, 257217, 260196, 263209, 266256, 269339, 272458, 275613, 278805, 282033, 285299,
 288602, 291944, 295325, 298745, 302204, 305703, 309243, 312824, 316446, 320111, 323817, 327567, 331360, 335197,
 339078, 343005, 346976, 350994, 355059, 359170, 363329, 367536, 371792, 376097, 380452, 384858, 389314, 393822,
 398382, 402995, 407662, 412382, 417157, 421988, 426874, 431817, 436817, 441876, 446992, 452168, 457404, 462700,
 468058, 473478, 478961, 484507, 490117, 495793, 501534, 507341, 513216, 519158, 525170, 531251, 537403, 543626,
 549921, 556288, 562730, 569246, 575838, 582505, 589250, 596074, 602976, 609958, 617021, 624166, 631393, 638704,
 646100, 653582, 661150, 668806, 676550, 684384, 692309, 700326, 708435, 716638, 724936, 733331, 741822, 750412,
 759102, 767892, 776783, 785778, 794877, 804081, 813392, 822811, 832338, 841976, 851726, 861589, 871565, 881658,
 891867, 902194, 912641, 923209, 933899, 944713, 955652, 966718, 977912, 989236,1000691,1012278,1024000,1035857,
1047852,1059986,1072260,1084676,1097236,1109941,1122794,1135795,1148947,1162251,1175709,1189323,1203095,1217026,
1231119,1245374,1259795,1274383,1289140,1304067,1319168,1334443,1349895,1365526,1381338,1397333,1413513,1429881,
1446438

};


static HI_VOID cmos_again_calc_table(HI_U32 *pu32AgainLin, HI_U32 *pu32AgainDb)
{
    int i;
    HI_U32 u32GainTableMax;

    if(WDR_MODE_2To1_LINE == genSensorMode)
    {
        u32GainTableMax = 510;
    }
    else
    {
        u32GainTableMax = 630;
    }

    if (*pu32AgainLin >= gain_table[u32GainTableMax])
    {
         *pu32AgainLin = gain_table[u32GainTableMax];
         *pu32AgainDb = u32GainTableMax;
         return ;
    }
    
    for (i = 1; i <= u32GainTableMax; i++)
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
    g_stSnsRegsInfo.astSspData[3].u32Data = (u32Again & 0xFF);
    g_stSnsRegsInfo.astSspData[4].u32Data = (u32Again >> 8) & 0x3;
    return;
}


/* Only used in WDR_MODE_2To1_LINE and WDR_MODE_2To1_FRAME mode */
static HI_VOID cmos_get_inttime_max(HI_U32 u32Ratio, HI_U32 *pu32IntTimeMax)
{
#if FIXED_RHS1_MODE
#else
    HI_U32  u32LongIntTimeMax;
#endif
    HI_U32  u32ShortIntTimeMax;
    HI_U32  u32IntTimeMaxTmp = 0;

    if(HI_NULL == pu32IntTimeMax)
    {
        printf("null pointer when get ae sensor IntTimeMax value!\n");
        return;
    }

    if((WDR_MODE_2To1_FRAME_FULL_RATE == genSensorMode) || (WDR_MODE_2To1_FRAME == genSensorMode))
    {
        *pu32IntTimeMax = (gu32FullLines - 4) * 0x40 / DIV_0_TO_1(u32Ratio);
    }

    if((WDR_MODE_2To1_LINE == genSensorMode) && (SENSOR_QXGA_30FPS_MODE == gu8SensorImageMode) )
    {
        if(ISP_FSWDR_LONG_FRAME_MODE == genFSWDRMode)  /* long frame mode enable */
        {
            u32ShortIntTimeMax = 5;
            gu32RHS1 = u32ShortIntTimeMax +11 + (4 - (u32ShortIntTimeMax +11 -10) %4);
            
            /* when change LongFrameMode, the first 2 frames must limit the MaxIntTime to avoid flicker */
            if(gu32MaxTimeGetCnt < 2)
            {
                u32IntTimeMaxTmp = gu32PreFullLines - 22 - 1800;
            }
            else
            {
                u32IntTimeMaxTmp = gu32PreFullLines - 22 - au32WDRIntTime[0];
            }
            *pu32IntTimeMax = gu32FullLines - 30;
            *pu32IntTimeMax = (u32IntTimeMaxTmp < *pu32IntTimeMax) ? u32IntTimeMaxTmp : *pu32IntTimeMax;
            gu32MaxTimeGetCnt++;
        }
        else
        {
            // 4n + 10
            // RHS1 <= FSC - BRL*2 -11
            // short exposure time = RHS1 - SHS1 (Let shs1 = 11)
            // short exposure time = (FSC - 22)/(ratio + 1)  //round to the bottom ;  maybe if can not reach that value because the RHS1 limitation;

#if FIXED_RHS1_MODE
            // if in FIXED_RHS1_MODE , the RHS1 should satisfy all kind of exposure ratio.
            // so just use the 16:1 to find a suitable RHS1 location;
            // short exposure range:   RHS1 - SHS1   [ RHS1- (RHS1-5) :  RHS1 - 11]
            // u32Ratio = 0x400;
            // PreShort + Long <= 1 PreVmax 

            u32IntTimeMaxTmp = ((gu32PreFullLines-22-au32WDRIntTime[0])*0x40 - ((gu32PreFullLines-22-au32WDRIntTime[0])*0x40%0x400)) / 0x400; 
            u32ShortIntTimeMax = ((gu32FullLines-22)*0x40  - ((gu32FullLines - 22) * 0x40%(DIV_0_TO_1(0x400 + 0x40)))) / DIV_0_TO_1(0x400 + 0x40); 
            u32ShortIntTimeMax = (u32IntTimeMaxTmp < u32ShortIntTimeMax) ? u32IntTimeMaxTmp : u32ShortIntTimeMax;
            u32ShortIntTimeMax = (u32ShortIntTimeMax < 5)? 5 : u32ShortIntTimeMax;       //min value clip

            if( u32ShortIntTimeMax > (gu32RHS1_LocRLimit - 11))     //located in the right handside of gu32RHS1_LocRLimit
            {
                u32ShortIntTimeMax = gu32RHS1_LocRLimit - 11;
                gu32RHS1 = gu32RHS1_LocRLimit;
            }
            else     //shift the gu32RHS1_LocRLimit to minimize the ghosting effect
            {
                if( 0 == (u32ShortIntTimeMax +11 -10) %4 )      
                {
                    gu32RHS1 = u32ShortIntTimeMax +11;
                }
                else
                {
                    gu32RHS1 = u32ShortIntTimeMax +11 + ( 4 - (u32ShortIntTimeMax +11 -10) %4 );  // right shif the gu32RHS1_LocRLimit
                }
            }
            
#else      // RHS1 adjustable

            u32IntTimeMaxTmp = ((gu32PreFullLines-22-au32WDRIntTime[0])*0x40 - ((gu32PreFullLines-22-au32WDRIntTime[0])*0x40%DIV_0_TO_1(u32Ratio))) / DIV_0_TO_1(u32Ratio); 
            u32ShortIntTimeMax = ((gu32FullLines-22)*0x40  - ((gu32FullLines-22)*0x40%(DIV_0_TO_1(u32Ratio + 0x40)))) / DIV_0_TO_1(u32Ratio + 0x40);
            u32ShortIntTimeMax = (u32IntTimeMaxTmp < u32ShortIntTimeMax) ? u32IntTimeMaxTmp : u32ShortIntTimeMax;
            u32ShortIntTimeMax = (u32ShortIntTimeMax < 5) ? 5 : u32ShortIntTimeMax;       //min value clip

            if(u32ShortIntTimeMax <=  (gu32RHS1_LocRLimit - 11))
            {
                u32LongIntTimeMax = u32ShortIntTimeMax * u32Ratio >> 6;       //  0x40;

                if(u32LongIntTimeMax <= (gu32FullLines - gu32RHS1_LocRLimit - 11))     //keep the RHS1;
                {
                    //easy to allocate RHS1
                    //gu32RHS1 = gu32RHS1_LocRLimit;
                }
                else     //shift the RHS1;
                {
                    gu32RHS1 = gu32FullLines - u32LongIntTimeMax - 11;
                    gu32RHS1 = gu32RHS1 - (gu32RHS1 -10) %4;
                      
                    if(u32ShortIntTimeMax <=  (gu32RHS1 - 11) )
                    {
                        //ok, keep this short exposure time
                    }
                    else
                    {
                       u32ShortIntTimeMax  = gu32RHS1 - 11;
                    }            
                }
            }
            else        // short exposure > gu32RHS1_LocRLimit -11
            {
                u32ShortIntTimeMax = gu32RHS1_LocRLimit - 11;
            }
#endif  

            *pu32IntTimeMax = u32ShortIntTimeMax;
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
         0x01DC, 0x80B7, 0x8025,
         0x804F, 0x0196, 0x8047,
         0x001C, 0x80C9, 0x01AD
    },
    
    3160,
    {
         0x01C3, 0x808B, 0x8038,
         0x8080, 0x01B1, 0x8031,
         0x0032, 0x8108, 0x01D6
    },
    
    2470,
    {     
         0x01BE, 0x8085, 0x8039,
         0x8064, 0x018A, 0x8026,
         0x0048, 0x8172, 0x022A
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

    pstAwbSnsDft->u16WbRefTemp = 4850;

    pstAwbSnsDft->au16GainOffset[0] = 0x1C2;
    pstAwbSnsDft->au16GainOffset[1] = 0x100;
    pstAwbSnsDft->au16GainOffset[2] = 0x100;
    pstAwbSnsDft->au16GainOffset[3] = 0x1F3;

    pstAwbSnsDft->as32WbPara[0] = 80;
    pstAwbSnsDft->as32WbPara[1] = 5;
    pstAwbSnsDft->as32WbPara[2] = -171;
    pstAwbSnsDft->as32WbPara[3] = 181935;
    pstAwbSnsDft->as32WbPara[4] = 128;
    pstAwbSnsDft->as32WbPara[5] = -136460;

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
    {0x50,0x40,0x32,0x20,0x16,0x12,0x10,0x0e,0x0c,0x0a,0x08,0x08,0x08,0x08,0x08,0x08},
        
    /* sharpen_alt_ud */
    {0x62,0x50,0x3f,0x30,0x26,0x22,0x20,0x1b,0x19,0x17,0x10,0x10,0x10,0x10,0x10,0x10},
        
    /* snr_thresh Max=0x54 */
    {0x06,0x0e,0x1a,0x20,0x25,0x3a,0x3c,0x3f,0x42,0x44,0x46,0x46,0x46,0x46,0x46,0x46},
        
    /* demosaic_lum_thresh */
    {0x50,0x50,0x4e,0x49,0x45,0x45,0x40,0x3a,0x3a,0x30,0x30,0x2a,0x20,0x20,0x20,0x20},
        
    /* demosaic_np_offset */
    {0x00,0x0a,0x12,0x1a,0x20,0x28,0x30,0x32,0x34,0x36,0x38,0x38,0x38,0x38,0x38,0x38},
        
    /* ge_strength */
    {0x55,0x55,0x55,0x55,0x55,0x55,0x37,0x37,0x37,0x35,0x35,0x35,0x35,0x35,0x35,0x35},

    /* rgbsharp_strength */
    {0x46,0x38,0x29,0x22,0x16,0x12,0x10,0x0e,0x0c,0x0a,0x0a,0x0a,0x0a,0x0a,0x0a,0x0a}
};



static ISP_CMOS_AGC_TABLE_S g_stIspAgcTableFSWDR =
{
    /* bvalid */
    1,
    
    /* 100, 200, 400, 800, 1600, 3200, 6400, 12800, 25600, 51200, 102400, 204800, 409600, 819200, 1638400, 3276800 */

    /* sharpen_alt_d */
    {0x50,0x40,0x32,0x20,0x16,0x12,0x10,0x0e,0x0c,0x0a,0x08,0x08,0x08,0x08,0x08,0x08},
        
    /* sharpen_alt_ud */
    {0x62,0x50,0x3f,0x30,0x26,0x22,0x20,0x1b,0x19,0x17,0x10,0x10,0x10,0x10,0x10,0x10},
        
    /* snr_thresh Max=0x54 */
    //{0x06,0x0e,0x1a,0x20,0x25,0x3a,0x3c,0x3f,0x42,0x44,0x46,0x46,0x46,0x46,0x46,0x46},
    {0x6,0x8,0xc,0x10,0x16,0x1c,0x22,0x28,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30},
        
    /* demosaic_lum_thresh */
    {0x50,0x50,0x4e,0x49,0x45,0x45,0x40,0x3a,0x3a,0x30,0x30,0x2a,0x20,0x20,0x20,0x20},
        
    /* demosaic_np_offset */
    {0x00,0x0a,0x12,0x1a,0x20,0x28,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30},
        
    /* ge_strength */
    {0x55,0x55,0x55,0x55,0x55,0x55,0x37,0x37,0x37,0x35,0x35,0x35,0x35,0x35,0x35,0x35},

    /* rgbsharp_strength */
 //   {0x46,0x38,0x29,0x22,0x16,0x12,0x10,0x0e,0x0c,0x0a,0x0a,0x0a,0x0a,0x0a,0x0a,0x0a}
     {0x45,0x37,0x28,0x22,0x16,0x12,0x10,0x0e,0x0c,0x0a,0x0a,0x0a,0x0a,0x0a,0x0a,0x0a}
};


static ISP_CMOS_NOISE_TABLE_S g_stIspNoiseTable =
{
    /* bvalid */
    1,

    /* nosie_profile_weight_lut */
    {0,0,0,0,0,0,0,0,8,14,18,21,23,25,27,28,29,31,32,33,34,34,35,36,37,37,38,38,39,39,
     40,40,41,41,42,42,43,43,43,44,44,44,45,45,45,46,46,46,46,47,47,47,47,48,48,48,48,
     49,49,49,49,50,50,50,50,50,51,51,51,51,51,52,52,52,52,52,52,53,53,53,53,53,53,53,
     54,54,54,54,54,54,54,55,55,55,55,55,55,55,56,56,56,56,56,56,56,56,56,57,57,57,57,
     57,57,57,57,57,58,58,58,58,58,58,58,58,58,59,59,59},   

    /* demosaic_weight_lut */
    {8,14,18,21,23,25,27,28,29,31,32,33,34,34,35,36,37,37,38,38,39,39,40,40,41,41,42,
     42,43,43,43,44,44,44,45,45,45,46,46,46,46,47,47,47,47,48,48,48,48,49,49,49,49,50,
     50,50,50,50,51,51,51,51,51,52,52,52,52,52,52,53,53,53,53,53,53,53,54,54,54,54,54,
     54,54,55,55,55,55,55,55,55,56,56,56,56,56,56,56,56,56,57,57,57,57,57,57,57,57,57,
     58,58,58,58,58,58,58,58,58,59,59,59,59,59,59,59,59,59,59,59}        
    
};

static ISP_CMOS_NOISE_TABLE_S g_stIspNoiseTableFSWDR =
{
    /* bvalid */
    1,
    
    /* nosie_profile_weight_lut */
    {
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,45
    },

    /* demosaic_weight_lut */
    {
        3,11,16,19,22,24,26,27,28,30,31,32,32,34,35,35,36,37,37,37,38,39,39,40,40,41,
        41,42,42,42,43,43,43,44,44,44,45,45,45,46,46,46,47,47,47,47,48,48,48,48,48,49,
        49,49,49,50,50,50,50,50,51,51,51,51,51,51,52,52,52,52,52,52,53,53,53,53,53,53,
        54,54,54,54,54,54,54,54,55,55,55,55,55,55,55,56,56,56,56,56,56,56,56,56,57,57,
        57,57,57,57,57,57,57,58,58,58,58,58,58,58,58,58,58,58,58,58,58,58,58,58
    }
};


static ISP_CMOS_DEMOSAIC_S g_stIspDemosaic =
{
    /* bvalid */
    1,
    
    /*vh_slope*/
    0x94,

    /*aa_slope*/
    0x92,

    /*va_slope*/
    0x87,

    /*uu_slope*/
    0x6e,

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




static ISP_CMOS_DEMOSAIC_S g_stIspDemosaicFSWDR =
{
    /* bvalid */
    1,
    
    /*vh_slope*/
    0xf0,

    /*aa_slope*/
    0xf0,

    /*va_slope*/
    0x87,

    /*uu_slope*/
    0x6e,

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


static ISP_CMOS_RGBSHARPEN_S g_stIspRgbSharpen_wdr =
{   
    /* bvalid */   
    1,   
    
    /*lut_core*/   
    182,  
    
    /*lut_strength*/  
    117, 
    
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
        0,   1,   2,   4,   8,  12,  17,  23,  30,  38,  47,  57,  68,  79,  92, 105, 120, 133, 147, 161, 176, 192, 209, 226, 243, 260, 278, 296, 315, 333, 351, 370, 390, 410, 431, 453, 474, 494, 515, 536, 558, 580, 602, 623, 644, 665, 686, 708, 730, 751, 773, 795, 818, 840, 862, 884, 907, 929, 951, 974, 998,1024,1051,1073,1096,1117,1139,1159,1181,1202,1223,1243,1261,1275,1293,1313,1332,1351,1371,1389,1408,1427,1446,1464,1482,1499,1516,1533,1549,1567,1583,1600,1616,1633,1650,1667,1683,1700,1716,1732,1749,1766,1782,1798,1815,1831,1847,1863,1880,1896,1912,1928,1945,1961,1977,1993,2009,2025,2041,2057,2073,2089,2104,2121,2137,2153,2168,2184,2200,2216,2231,2248,2263,2278,2294,2310,2326,2341,2357,2373,2388,2403,2419,2434,2450,2466,2481,2496,2512,2527,2543,2558,2573,2589,2604,2619,2635,2650,2665,2680,2696,2711,2726,2741,2757,2771,2787,2801,2817,2832,2847,2862,2877,2892,2907,2922,2937,2952,2967,2982,2997,3012,3027,3041,3057,3071,3086,3101,3116,3130,3145,3160,3175,3190,3204,3219,3234,3248,3263,3278,3293,3307,3322,3337,3351,3365,3380,3394,3409,3424,3438,3453,3468,3482,3497,3511,3525,3540,3554,3569,3584,3598,3612,3626,3641,3655,3670,3684,3699,3713,3727,3742,3756,3770,3784,3799,3813,3827,3841,3856,3870,3884,3898,3912,3927,3941,3955,3969,3983,3997,4011,4026,4039,4054,4068,4082,4095
    }
#else  /*higher  contrast*/
    {
        0,1,2,4,8,12,17,23,30,38,47,57,68,79,92,105,120,133,147,161,176,192,209,226,243,260,278,296,317,340,365,390,416,440,466,491,517,538,561,584,607,631,656,680,705,730,756,784,812,835,858,882,908,934,958,982,1008,1036,1064,1092,1119,1143,1167,1192,1218,1243,1269,1296,1323,1351,1379,1408,1434,1457,1481,1507,1531,1554,1579,1603,1628,1656,1683,1708,1732,1756,1780,1804,1829,1854,1877,1901,1926,1952,1979,2003,2024,2042,2062,2084,2106,2128,2147,2168,2191,2214,2233,2256,2278,2296,2314,2335,2352,2373,2391,2412,2431,2451,2472,2492,2513,2531,2547,2566,2581,2601,2616,2632,2652,2668,2688,2705,2721,2742,2759,2779,2796,2812,2826,2842,2857,2872,2888,2903,2920,2934,2951,2967,2983,3000,3015,3033,3048,3065,3080,3091,3105,3118,3130,3145,3156,3171,3184,3197,3213,3224,3240,3252,3267,3281,3295,3310,3323,3335,3347,3361,3372,3383,3397,3409,3421,3432,3447,3459,3470,3482,3497,3509,3521,3534,3548,3560,3572,3580,3592,3602,3613,3625,3633,3646,3657,3667,3679,3688,3701,3709,3719,3727,3736,3745,3754,3764,3773,3781,3791,3798,3806,3816,3823,3833,3840,3847,3858,3865,3872,3879,3888,3897,3904,3911,3919,3926,3933,3940,3948,3955,3962,3970,3973,3981,3988,3996,4003,4011,4018,4026,4032,4037,4045,4053,4057,4064,4072,4076,4084,4088,4095        
        //0,1,2,4,8,12,17,23,30,38,47,57,68,79,92,105,120,133,147,161,176,192,209,226,243,260,278,296,317,340,365,390,416,440,466,491,517,538,561,584,607,631,656,680,705,730,756,784,812,835,858,882,908,934,958,982,1008,1036,1064,1092,1119,1143,1167,1192,1218,1243,1269,1294,1320,1346,1372,1398,1424,1450,1476,1502,1528,1554,1580,1607,1633,1658,1684,1710,1735,1761,1786,1811,1836,1860,1884,1908,1932,1956,1979,2002,2024,2046,2068,2090,2112,2133,2154,2175,2196,2217,2237,2258,2278,2298,2318,2337,2357,2376,2395,2414,2433,2451,2469,2488,2505,2523,2541,2558,2575,2592,2609,2626,2642,2658,2674,2690,2705,2720,2735,2750,2765,2779,2793,2807,2821,2835,2848,2861,2874,2887,2900,2913,2925,2937,2950,2962,2974,2986,2998,3009,3021,3033,3044,3056,3067,3078,3088,3099,3109,3119,3129,3139,3148,3158,3168,3177,3187,3197,3207,3217,3227,3238,3248,3259,3270,3281,3292,3303,3313,3324,3335,3346,3357,3368,3379,3389,3400,3410,3421,3431,3441,3451,3461,3471,3481,3491,3501,3511,3521,3531,3541,3552,3562,3572,3583,3593,3604,3615,3625,3636,3646,3657,3668,3679,3689,3700,3711,3721,3732,3743,3753,3764,3774,3784,3795,3805,3816,3826,3837,3847,3858,3869,3880,3891,3902,3913,3925,3937,3949,3961,3973,3985,3997,4009,4022,4034,4046,4058,4071,4083,4095
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
            pstDef->stDrc.u32VarianceSpace      = 0x06;
            pstDef->stDrc.u32VarianceIntensity  = 0x08;
            pstDef->stDrc.u32Asymmetry          = 0x14;
            pstDef->stDrc.u32BrightEnhance      = 0xC8;

            memcpy(&pstDef->stNoiseTbl, &g_stIspNoiseTableFSWDR, sizeof(ISP_CMOS_NOISE_TABLE_S));            
            memcpy(&pstDef->stAgcTbl, &g_stIspAgcTableFSWDR, sizeof(ISP_CMOS_AGC_TABLE_S));
            memcpy(&pstDef->stDemosaic, &g_stIspDemosaicFSWDR, sizeof(ISP_CMOS_DEMOSAIC_S));
            memcpy(&pstDef->stRgbSharpen, &g_stIspRgbSharpen_wdr, sizeof(ISP_CMOS_RGBSHARPEN_S));        
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

    if (WDR_MODE_NONE == genSensorMode)
    {          
        for (i=0; i<4; i++)
        {
            pstBlackLevel->au16BlackLevel[i] = 0xEF;
        }
    }
    else if(WDR_MODE_2To1_LINE == genSensorMode)
    {
        for (i=0; i<4; i++)
        {
            pstBlackLevel->au16BlackLevel[i] = 0xF0;        //240
        }
    }
    else
    {
        for (i=0; i<4; i++)
        {
            pstBlackLevel->au16BlackLevel[i] = 0xEF;
        }
    }

    return 0;    
}

HI_VOID cmos_set_pixel_detect(HI_BOOL bEnable)
{
    HI_U32 u32FullLines_5Fps;
    if (WDR_MODE_2To1_LINE == genSensorMode)
    {   
        return;
    }
    else
    {
        if (SENSOR_1080P_60FPS_MODE == gu8SensorImageMode)
        {
            u32FullLines_5Fps = (IMX123_VMAX_1080P_60FPS_LINEAR * 60) / 5;
        }
        else  if (SENSOR_QXGA_30FPS_MODE == gu8SensorImageMode)
        {
            if(WDR_MODE_2To1_LINE == genSensorMode)
            {
                // TODO:
            }
            else
            {
                u32FullLines_5Fps = (IMX123_VMAX_QXGA_30FPS_LINEAR* 30) / 5;
            }
        }
        else
        {
            return;
        }
    }

    u32FullLines_5Fps = (u32FullLines_5Fps > FULL_LINES_MAX) ? FULL_LINES_MAX : u32FullLines_5Fps;

    if (bEnable) /* setup for ISP pixel calibration mode */
    {
        sensor_write_register (GAIN_ADDR,0x00);
        sensor_write_register (GAIN_ADDR + 1,0x00);
        
        sensor_write_register (VMAX_ADDR, u32FullLines_5Fps & 0xFF); 
        sensor_write_register (VMAX_ADDR + 1, (u32FullLines_5Fps & 0xFF00) >> 8); 
        sensor_write_register (VMAX_ADDR + 2, (u32FullLines_5Fps & 0x10000) >> 16);

        sensor_write_register (SHS1_ADDR, 0x4);
        sensor_write_register (SHS1_ADDR + 1, 0x0); 
        sensor_write_register (SHS1_ADDR + 2, 0x0);          
    }
    else /* setup for ISP 'normal mode' */
    {
        gu32FullLinesStd = (gu32FullLinesStd > FULL_LINES_MAX) ? FULL_LINES_MAX : gu32FullLinesStd;
        gu32FullLines = gu32FullLinesStd;
        sensor_write_register (VMAX_ADDR, gu32FullLines & 0xFF); 
        sensor_write_register (VMAX_ADDR + 1, (gu32FullLines & 0xFF00) >> 8); 
        sensor_write_register (VMAX_ADDR + 2, (gu32FullLines & 0x10000) >> 16);
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
            
            if(SENSOR_QXGA_30FPS_MODE == gu8SensorImageMode)
            {
                gu32FullLinesStd = IMX123_VMAX_QXGA_30FPS_LINEAR;
            }
            else if(SENSOR_1080P_60FPS_MODE== gu8SensorImageMode)
            {
                gu32FullLinesStd = IMX123_VMAX_1080P_60FPS_LINEAR;
            }
            else
            {
            
            }
            printf("linear mode\n");
        break;

        case WDR_MODE_2To1_LINE:
            genSensorMode = WDR_MODE_2To1_LINE;

            if(SENSOR_QXGA_30FPS_MODE == gu8SensorImageMode)
            {
                gu32FullLinesStd = IMX123_VMAX_QXGA_30FPS_DOL_WDR *2;
            }
            else
            {
            
            }
            printf("2to1 line WDR mode\n");
        break;

        case WDR_MODE_2To1_FRAME:       //half rate
            genSensorMode = WDR_MODE_2To1_FRAME;
            if(SENSOR_1080P_60FPS_MODE== gu8SensorImageMode)
            {
                gu32FullLinesStd = IMX123_VMAX_1080P_60FPS_LINEAR;
            }
            else if(SENSOR_QXGA_60FPS_MODE == gu8SensorImageMode)
            {
                gu32FullLinesStd = IMX123_VMAX_QXGA_60FPS_LINEAR;
            }
            else
            {
            }
        break;

        case WDR_MODE_2To1_FRAME_FULL_RATE:
            genSensorMode = WDR_MODE_2To1_FRAME_FULL_RATE;
            if(SENSOR_QXGA_30FPS_MODE == gu8SensorImageMode)
            {
                gu32FullLinesStd = IMX123_VMAX_QXGA_30FPS_LINEAR;
            }
            else if(SENSOR_1080P_60FPS_MODE== gu8SensorImageMode)
            {
                gu32FullLinesStd = IMX123_VMAX_1080P_60FPS_LINEAR;
            }
            else
            {
            
            }
        break;
        
        default:
            printf("NOT support this mode!\n");
            return;
        break;
    }

    gu32FullLines = gu32FullLinesStd;
    gu32PreFullLines = gu32FullLines;
    memset(au32WDRIntTime, 0, sizeof(au32WDRIntTime));
    
    return;
}

static HI_S32 cmos_set_image_mode(ISP_CMOS_SENSOR_IMAGE_MODE_S *pstSensorImageMode)
{
    HI_U8 u8SensorImageMode = gu8SensorImageMode;
    
    bInit = HI_FALSE;    

    if (HI_NULL == pstSensorImageMode)
    {
        printf("null pointer when set image mode\n");
        return -1;
    }

    if((pstSensorImageMode->u16Width <= 1920)&&(pstSensorImageMode->u16Height <= 1080))
    {
        if (pstSensorImageMode->f32Fps <= 60)
        {
            u8SensorImageMode = SENSOR_1080P_60FPS_MODE;
            if(WDR_MODE_NONE == genSensorMode)
            {
                gu32FullLinesStd = IMX123_VMAX_1080P_60FPS_LINEAR;
            }
            else
            {
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
    }
    else if((pstSensorImageMode->u16Width <= 2048)&&(pstSensorImageMode->u16Height <= 1536))
    {
        if (pstSensorImageMode->f32Fps <= 30)
        {
            u8SensorImageMode = SENSOR_QXGA_30FPS_MODE;
            
            if(WDR_MODE_NONE == genSensorMode)
            {
                gu32FullLinesStd = IMX123_VMAX_QXGA_30FPS_LINEAR;
            }
            else if(WDR_MODE_2To1_LINE == genSensorMode)
            {
                gu32FullLinesStd = IMX123_VMAX_QXGA_30FPS_DOL_WDR *2;
            }
            else if(WDR_MODE_2To1_FRAME_FULL_RATE == genSensorMode) //full rate
            {
                gu32FullLinesStd = IMX123_VMAX_QXGA_30FPS_LINEAR;
            }
            
        }
        else if(pstSensorImageMode->f32Fps <= 60)
        {
            if (WDR_MODE_2To1_LINE == genSensorMode)
            {

            }
            else
            {
                u8SensorImageMode = SENSOR_QXGA_60FPS_MODE;
            }

            if(WDR_MODE_2To1_FRAME == genSensorMode) //half rate
            {
                gu32FullLinesStd = IMX123_VMAX_QXGA_60FPS_LINEAR;
            }
            else        //not support!
            {
                
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
    gu32FullLines = gu32FullLinesStd;
    gu32PreFullLines = gu32FullLines;
    memset(au32WDRIntTime, 0, sizeof(au32WDRIntTime));

    return 0;
    
}

HI_U32 cmos_get_sns_regs_info(ISP_SNS_REGS_INFO_S *pstSnsRegsInfo)
{
    HI_S32 i;

    if (HI_FALSE == bInit)
    {
        g_stSnsRegsInfo.enSnsType = ISP_SNS_SSP_TYPE;
        g_stSnsRegsInfo.u8Cfg2ValidDelayMax =  2;        
        g_stSnsRegsInfo.u32RegNum = 8;

        if (WDR_MODE_2To1_FRAME == genSensorMode 
            ||WDR_MODE_2To1_FRAME_FULL_RATE == genSensorMode)
        {
            g_stSnsRegsInfo.u32RegNum += 3; 
        }
        else if (WDR_MODE_2To1_LINE == genSensorMode)
        {
            g_stSnsRegsInfo.u32RegNum += 6; 
        }
        else
        {
        }
        
         
        for (i = 0; i < g_stSnsRegsInfo.u32RegNum; i++)
        {    
            g_stSnsRegsInfo.astSspData[i].bUpdate = HI_TRUE;
            g_stSnsRegsInfo.astSspData[i].u32DevAddr = 0x02;
            g_stSnsRegsInfo.astSspData[i].u32DevAddrByteNum = 1;
            g_stSnsRegsInfo.astSspData[i].u32RegAddrByteNum = 1;
            g_stSnsRegsInfo.astSspData[i].u32DataByteNum = 1;
        }        
        
        g_stSnsRegsInfo.astSspData[0].u8DelayFrmNum =  0;       //shutter
        g_stSnsRegsInfo.astSspData[0].u32RegAddr = SHS1_ADDR;
        g_stSnsRegsInfo.astSspData[1].u8DelayFrmNum =  0;
        g_stSnsRegsInfo.astSspData[1].u32RegAddr = SHS1_ADDR + 1;
        g_stSnsRegsInfo.astSspData[2].u8DelayFrmNum =  0;
        g_stSnsRegsInfo.astSspData[2].u32RegAddr = SHS1_ADDR + 2;
        
        g_stSnsRegsInfo.astSspData[3].u8DelayFrmNum = 0;        //gain
        g_stSnsRegsInfo.astSspData[3].u32RegAddr = GAIN_ADDR;
        g_stSnsRegsInfo.astSspData[4].u8DelayFrmNum = 0;
        g_stSnsRegsInfo.astSspData[4].u32RegAddr = GAIN_ADDR + 1;

        g_stSnsRegsInfo.astSspData[5].u8DelayFrmNum = 0;
        g_stSnsRegsInfo.astSspData[5].u32RegAddr = VMAX_ADDR;
        g_stSnsRegsInfo.astSspData[6].u8DelayFrmNum = 0;       
        g_stSnsRegsInfo.astSspData[6].u32RegAddr = VMAX_ADDR + 1;
        g_stSnsRegsInfo.astSspData[7].u8DelayFrmNum = 0;
        g_stSnsRegsInfo.astSspData[7].u32RegAddr = VMAX_ADDR + 2;

        if (WDR_MODE_2To1_LINE == genSensorMode )
        {
            g_stSnsRegsInfo.astSspData[0].u8DelayFrmNum = 1;
            g_stSnsRegsInfo.astSspData[1].u8DelayFrmNum = 1;
            g_stSnsRegsInfo.astSspData[2].u8DelayFrmNum = 1;

            g_stSnsRegsInfo.astSspData[5].u8DelayFrmNum = 0;
            g_stSnsRegsInfo.astSspData[5].u32RegAddr = SHS2_ADDR;
            g_stSnsRegsInfo.astSspData[6].u8DelayFrmNum = 0;
            g_stSnsRegsInfo.astSspData[6].u32RegAddr = SHS2_ADDR + 1;
            g_stSnsRegsInfo.astSspData[7].u8DelayFrmNum = 0;
            g_stSnsRegsInfo.astSspData[7].u32RegAddr = SHS2_ADDR + 2;

            g_stSnsRegsInfo.astSspData[8].u8DelayFrmNum = 1;
            g_stSnsRegsInfo.astSspData[8].u32RegAddr = VMAX_ADDR;
            g_stSnsRegsInfo.astSspData[9].u8DelayFrmNum = 1;       
            g_stSnsRegsInfo.astSspData[9].u32RegAddr = VMAX_ADDR + 1;
            g_stSnsRegsInfo.astSspData[10].u8DelayFrmNum = 1;
            g_stSnsRegsInfo.astSspData[10].u32RegAddr = VMAX_ADDR + 2;

            g_stSnsRegsInfo.astSspData[11].u8DelayFrmNum = 1;
            g_stSnsRegsInfo.astSspData[11].u32RegAddr = RHS1_ADDR;           // [19:0]
            g_stSnsRegsInfo.astSspData[12].u8DelayFrmNum = 1;       
            g_stSnsRegsInfo.astSspData[12].u32RegAddr = RHS1_ADDR + 1;
            g_stSnsRegsInfo.astSspData[13].u8DelayFrmNum = 1;
            g_stSnsRegsInfo.astSspData[13].u32RegAddr = RHS1_ADDR + 2;
            
        }    
        else if ((WDR_MODE_2To1_FRAME == genSensorMode) || (WDR_MODE_2To1_FRAME_FULL_RATE == genSensorMode))
        {
            g_stSnsRegsInfo.astSspData[5].u8DelayFrmNum = 1;
            g_stSnsRegsInfo.astSspData[5].u32RegAddr = SHS1_ADDR;
            g_stSnsRegsInfo.astSspData[6].u8DelayFrmNum = 1;
            g_stSnsRegsInfo.astSspData[6].u32RegAddr = SHS1_ADDR + 1;
            g_stSnsRegsInfo.astSspData[7].u8DelayFrmNum = 1;
            g_stSnsRegsInfo.astSspData[7].u32RegAddr = SHS1_ADDR + 2;

            g_stSnsRegsInfo.astSspData[8].u8DelayFrmNum = 0;
            g_stSnsRegsInfo.astSspData[8].u32RegAddr = VMAX_ADDR;
            g_stSnsRegsInfo.astSspData[9].u8DelayFrmNum = 0;       
            g_stSnsRegsInfo.astSspData[9].u32RegAddr = VMAX_ADDR + 1;
            g_stSnsRegsInfo.astSspData[10].u8DelayFrmNum = 0;
            g_stSnsRegsInfo.astSspData[10].u32RegAddr = VMAX_ADDR + 2;

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

        if ((WDR_MODE_2To1_FRAME == genSensorMode) || (WDR_MODE_2To1_FRAME_FULL_RATE == genSensorMode))
        {
            g_stSnsRegsInfo.astSspData[0].bUpdate = HI_TRUE;
            g_stSnsRegsInfo.astSspData[1].bUpdate = HI_TRUE;
            g_stSnsRegsInfo.astSspData[2].bUpdate = HI_TRUE;
            g_stSnsRegsInfo.astSspData[5].bUpdate = HI_TRUE;
            g_stSnsRegsInfo.astSspData[6].bUpdate = HI_TRUE;
            g_stSnsRegsInfo.astSspData[7].bUpdate = HI_TRUE;
        }
    }
    
    if (HI_NULL == pstSnsRegsInfo)
    {
        printf("null pointer when get sns reg info!\n");
        return -1;
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
    gu8SensorImageMode = SENSOR_QXGA_30FPS_MODE;
    genSensorMode = WDR_MODE_NONE;
    gu32FullLinesStd = IMX123_VMAX_QXGA_30FPS_LINEAR; 
    gu32FullLines = IMX123_VMAX_QXGA_30FPS_LINEAR;
    gu32PreFullLines = IMX123_VMAX_QXGA_30FPS_LINEAR;
    gu32RHS1 = 0xe6;

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
    s32Ret = HI_MPI_ISP_SensorRegCallBack(IspDev, IMX123_ID, &stIspRegister);
    if (s32Ret)
    {
        printf("sensor register callback function failed!\n");
        return s32Ret;
    }
    
    stLib.s32Id = 0;
    strncpy(stLib.acLibName, HI_AE_LIB_NAME, sizeof(HI_AE_LIB_NAME));
    cmos_init_ae_exp_function(&stAeRegister.stSnsExp);
    s32Ret = HI_MPI_AE_SensorRegCallBack(IspDev, &stLib, IMX123_ID, &stAeRegister);
    if (s32Ret)
    {
        printf("sensor register callback function to ae lib failed!\n");
        return s32Ret;
    }

    stLib.s32Id = 0;
    strncpy(stLib.acLibName, HI_AWB_LIB_NAME, sizeof(HI_AWB_LIB_NAME));
    cmos_init_awb_exp_function(&stAwbRegister.stSnsExp);
    s32Ret = HI_MPI_AWB_SensorRegCallBack(IspDev, &stLib, IMX123_ID, &stAwbRegister);
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

    s32Ret = HI_MPI_ISP_SensorUnRegCallBack(IspDev, IMX123_ID);
    if (s32Ret)
    {
        printf("sensor unregister callback function failed!\n");
        return s32Ret;
    }
    
    stLib.s32Id = 0;
    strncpy(stLib.acLibName, HI_AE_LIB_NAME, sizeof(HI_AE_LIB_NAME));
    s32Ret = HI_MPI_AE_SensorUnRegCallBack(IspDev, &stLib, IMX123_ID);
    if (s32Ret)
    {
        printf("sensor unregister callback function to ae lib failed!\n");
        return s32Ret;
    }

    stLib.s32Id = 0;
    strncpy(stLib.acLibName, HI_AWB_LIB_NAME, sizeof(HI_AWB_LIB_NAME));
    s32Ret = HI_MPI_AWB_SensorUnRegCallBack(IspDev, &stLib, IMX123_ID);
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
