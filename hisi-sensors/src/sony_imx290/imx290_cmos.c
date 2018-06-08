#if !defined(__IMX290_CMOS_H_)
#define __IMX290_CMOS_H_

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


#define IMX290_ID 290

extern const unsigned int sensor_i2c_addr;
extern unsigned int sensor_addr_byte;
extern unsigned int sensor_data_byte;

#define FULL_LINES_MAX  (0x3FFFF)
#define FULL_LINES_MAX_2TO1_WDR (0x8AA)


/*****Imx290 Register Address*****/
#define SHS1_ADDR (0x3020) 
#define SHS2_ADDR (0x3024) 
#define SHS3_ADDR (0x3028) 
#define GAIN_ADDR (0x3014)
#define HCG_ADDR  (0x3009)
#define VMAX_ADDR (0x3018)
#define HMAX_ADDR (0x301c)
#define RHS1_ADDR (0x3030) 
#define RHS2_ADDR (0x3034)
#define Y_OUT_SIZE_ADDR (0x3418)

#define INCREASE_LINES (1) /* make real fps less than stand fps because NVR require*/
#define IMX290_VMAX_1080P30_LINEAR  (1125+INCREASE_LINES)
#define IMX290_VMAX_1080P60TO30_WDR (1220+INCREASE_LINES)
#define IMX290_VMAX_1080P120TO30_WDR (1125+INCREASE_LINES)

/* sensor fps mode */
#define IMX290_SENSOR_1080P_30FPS_LINEAR_MODE  (1)
#define IMX290_SENSOR_1080P_30FPS_WDR_MODE     (2)
#define IMX290_SENSOR_1080P_60FPS_WDR_MODE     (3)

/* global variables */
static HI_BOOL bInit = HI_FALSE;
HI_BOOL bSensorInit = HI_FALSE; 

static HI_U8 gu8HCGReg = 0x2;
HI_U8 gu8SensorImageMode = IMX290_SENSOR_1080P_30FPS_LINEAR_MODE;
HI_U8 genSensorMode = WDR_MODE_NONE;

static HI_U32 gu32BRL = 1109;                                          
static HI_U32 gu32RHS1_Max = (IMX290_VMAX_1080P60TO30_WDR - 1109) * 2 - 21;
//static HI_U32 gu32RHS2_Max = IMX290_VMAX_1080P120TO30_WDR*4-3*1109-25;
static HI_U32 gu32FullLinesStd = IMX290_VMAX_1080P30_LINEAR;
static HI_U32 gu32FullLines = IMX290_VMAX_1080P30_LINEAR;
static HI_U32 gu32PreFullLines = IMX290_VMAX_1080P30_LINEAR;

ISP_SNS_REGS_INFO_S g_stSnsRegsInfo = {0};
ISP_SNS_REGS_INFO_S g_stPreSnsRegsInfo = {0};

static HI_U32 au32WDRIntTime[4] = {0};
static ISP_FSWDR_MODE_E genFSWDRMode = ISP_FSWDR_NORMAL_MODE;
static HI_U32 gu32MaxTimeGetCnt = 0;

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
    512,               // u32MaxIrisFNOTarget  
    30                 // u32MinIrisFNOTarget
};

static HI_S32 cmos_get_ae_default(AE_SENSOR_DEFAULT_S *pstAeSnsDft)
{
    if (HI_NULL == pstAeSnsDft)
    {
        printf("null pointer when get ae default value!\n");
        return -1;
    }

    memset(&pstAeSnsDft->stAERouteAttr, 0, sizeof(ISP_AE_ROUTE_S));
      
    pstAeSnsDft->u32LinesPer500ms = gu32FullLinesStd*30/2;
    pstAeSnsDft->u32FullLinesStd = gu32FullLinesStd;
    pstAeSnsDft->u32FlickerFreq = 50*256;
    pstAeSnsDft->u32FullLinesMax = FULL_LINES_MAX;

    pstAeSnsDft->stIntTimeAccu.enAccuType = AE_ACCURACY_LINEAR;
    pstAeSnsDft->stIntTimeAccu.f32Accuracy = 1;
    pstAeSnsDft->stIntTimeAccu.f32Offset = 0;

    pstAeSnsDft->stAgainAccu.enAccuType = AE_ACCURACY_TABLE;
    pstAeSnsDft->stAgainAccu.f32Accuracy = 1;

    pstAeSnsDft->stDgainAccu.enAccuType = AE_ACCURACY_TABLE;
    pstAeSnsDft->stDgainAccu.f32Accuracy = 1;
    
    pstAeSnsDft->u32ISPDgainShift = 8;
    pstAeSnsDft->u32MinISPDgainTarget = 1 << pstAeSnsDft->u32ISPDgainShift;
    pstAeSnsDft->u32MaxISPDgainTarget = 2 << pstAeSnsDft->u32ISPDgainShift;

    memcpy(&pstAeSnsDft->stPirisAttr, &gstPirisAttr, sizeof(ISP_PIRIS_ATTR_S));
    pstAeSnsDft->enMaxIrisFNO = ISP_IRIS_F_NO_1_0;
    pstAeSnsDft->enMinIrisFNO = ISP_IRIS_F_NO_32_0;

    pstAeSnsDft->bAERouteExValid = HI_FALSE;
    pstAeSnsDft->stAERouteAttr.u32TotalNum = 0;
    pstAeSnsDft->stAERouteAttrEx.u32TotalNum = 0;

    pstAeSnsDft->u32InitExposure = 921600;

    switch(genSensorMode)
    {
        default:
        case WDR_MODE_NONE:   
            pstAeSnsDft->au8HistThresh[0] = 0xd;
            pstAeSnsDft->au8HistThresh[1] = 0x28;
            pstAeSnsDft->au8HistThresh[2] = 0x60;
            pstAeSnsDft->au8HistThresh[3] = 0x80;

            pstAeSnsDft->u32MaxAgain = 62564; 
            pstAeSnsDft->u32MinAgain = 1024;
            pstAeSnsDft->u32MaxAgainTarget = pstAeSnsDft->u32MaxAgain;
            pstAeSnsDft->u32MinAgainTarget = pstAeSnsDft->u32MinAgain;

            //pstAeSnsDft->u32MaxDgain = 124833; 
			pstAeSnsDft->u32MaxDgain = 20013;   
            pstAeSnsDft->u32MinDgain = 1024;
            pstAeSnsDft->u32MaxDgainTarget = pstAeSnsDft->u32MaxDgain;
            pstAeSnsDft->u32MinDgainTarget = pstAeSnsDft->u32MinDgain;
            
            pstAeSnsDft->u8AeCompensation = 0x38;
            pstAeSnsDft->u32InitExposure = 260000;
			
            pstAeSnsDft->u32MaxIntTime = gu32FullLinesStd - 2;
            pstAeSnsDft->u32MinIntTime = 1;
            pstAeSnsDft->u32MaxIntTimeTarget = 65535;
            pstAeSnsDft->u32MinIntTimeTarget = 1;
        break;

        case WDR_MODE_2To1_LINE:                                                                      
            pstAeSnsDft->au8HistThresh[0] = 0xC;                                                      
            pstAeSnsDft->au8HistThresh[1] = 0x18;                                                     
            pstAeSnsDft->au8HistThresh[2] = 0x60;                                                     
            pstAeSnsDft->au8HistThresh[3] = 0x80;                                                     
                                 
            pstAeSnsDft->u32MaxIntTime = gu32FullLinesStd - 2;                                                           
            pstAeSnsDft->u32MinIntTime = 2;                                                           
            pstAeSnsDft->u32MaxIntTimeTarget = 65535;                                                 
            pstAeSnsDft->u32MinIntTimeTarget = pstAeSnsDft->u32MinIntTime;                            
                                                                                                   
            pstAeSnsDft->u32MaxAgain = 62564; 
            pstAeSnsDft->u32MinAgain = 1024;                                                          
            pstAeSnsDft->u32MaxAgainTarget = 62564;                                                 
            pstAeSnsDft->u32MinAgainTarget = pstAeSnsDft->u32MinAgain; 

            pstAeSnsDft->u32MaxDgain = 124833;  
            pstAeSnsDft->u32MinDgain = 1024;
            pstAeSnsDft->u32MaxDgainTarget = pstAeSnsDft->u32MaxDgain;
            pstAeSnsDft->u32MinDgainTarget = pstAeSnsDft->u32MinDgain;
			
			pstAeSnsDft->u32LFMaxShortTime = 688;
			pstAeSnsDft->u32MaxIntTimeStep = 3000;
			pstAeSnsDft->u32LFMinExposure = 880000;
			pstAeSnsDft->u32InitExposure = 120000;

            if(ISP_FSWDR_LONG_FRAME_MODE == genFSWDRMode)
            {
                pstAeSnsDft->u32MaxISPDgainTarget = 8 << pstAeSnsDft->u32ISPDgainShift;
                pstAeSnsDft->u8AeCompensation = 64; 
            }
            else
            {
                pstAeSnsDft->u32MaxDgainTarget = 8153;
                pstAeSnsDft->u32MaxISPDgainTarget = 267;
                pstAeSnsDft->u8AeCompensation = 64;  
                pstAeSnsDft->u16ManRatioEnable = HI_FALSE;                                                 
                pstAeSnsDft->u32Ratio = 0x400;  
            }
       break;                                                                                       

      
    
    }

    return 0;
}


/* the function of sensor set fps */
static HI_VOID cmos_fps_set(HI_FLOAT f32Fps, AE_SENSOR_DEFAULT_S *pstAeSnsDft)
{

    HI_U32 u32VMAX = IMX290_VMAX_1080P30_LINEAR;                                                                            
                                                                                                                          
    switch (gu8SensorImageMode)                                                                                           
    {                
      case IMX290_SENSOR_1080P_30FPS_WDR_MODE:                                                                                  
           if ((f32Fps <= 30) && (f32Fps >= 16.5))                                                                            
           {
               u32VMAX = IMX290_VMAX_1080P60TO30_WDR * 30 / f32Fps;  
           }
           else                                                                                                              
           {                                                                                                                 
               printf("Not support Fps: %f\n", f32Fps);                                                                      
               return;                                                                                                       
           } 
		   u32VMAX = (u32VMAX > FULL_LINES_MAX_2TO1_WDR) ? FULL_LINES_MAX_2TO1_WDR : u32VMAX;  
           break;
           
      case IMX290_SENSOR_1080P_30FPS_LINEAR_MODE:
           if ((f32Fps <= 30) && (f32Fps >= 0.13))                                                                            
           {
               u32VMAX = IMX290_VMAX_1080P30_LINEAR * 30 / f32Fps;  
           }
           else                                                                                                              
           {                                                                                                                 
               printf("Not support Fps: %f\n", f32Fps);                                                                      
               return;                                                                                                       
           }
		   u32VMAX = (u32VMAX > FULL_LINES_MAX) ? FULL_LINES_MAX : u32VMAX;  
           break;
                                                                                                                      
      default:  
          return; 
          break;                                                                                                            
    }                                                                                                                                                                                                                                            
                                                                                                                          
    if (WDR_MODE_NONE == genSensorMode)                                                                                   
    {                                                                                                                     
        g_stSnsRegsInfo.astI2cData[5].u32Data = (u32VMAX & 0xFF);                                                         
        g_stSnsRegsInfo.astI2cData[6].u32Data = ((u32VMAX & 0xFF00) >> 8);                                                
        g_stSnsRegsInfo.astI2cData[7].u32Data = ((u32VMAX & 0x30000) >> 16);                                              
    }                                                                                                                     

    else                                                                                                                  
    {                                                                                                                     
        g_stSnsRegsInfo.astI2cData[8].u32Data = (u32VMAX & 0xFF);                                                         
        g_stSnsRegsInfo.astI2cData[9].u32Data = ((u32VMAX & 0xFF00) >> 8);                                                
        g_stSnsRegsInfo.astI2cData[10].u32Data = ((u32VMAX & 0xF0000) >> 16);                                             
    }                                                                                                                     
                                                                                                                          
    if (WDR_MODE_2To1_LINE == genSensorMode)                                                                              
    {                                                                                                                     
        gu32FullLinesStd = u32VMAX * 2;
                                                                                                                          
        /*                                                                                                                
            RHS1 limitation:                                                                                              
            2n + 5                                                                                                        
            RHS1 <= FSC - BRL*2 -21                                                                                       
            (2 * VMAX_IMX290_1080P30_WDR - 2 * gu32BRL - 21) - (((2 * VMAX_IMX290_1080P30_WDR - 2 * 1109 - 21) - 5) %2)   
        */                                                                                                                
        gu32RHS1_Max = (u32VMAX - gu32BRL) * 2 - 21;                                                                                                                                                  

    }
    else                                                                                                                  
    {                                                                                                                     
        gu32FullLinesStd = u32VMAX;                                                                                       
    }                                                                                                                     
                                                                                                                          
    pstAeSnsDft->f32Fps = f32Fps;                                                                                                                  
    pstAeSnsDft->u32LinesPer500ms = gu32FullLinesStd * f32Fps / 2;                                                        
    pstAeSnsDft->u32FullLinesStd = gu32FullLinesStd;  
    pstAeSnsDft->u32MaxIntTime = gu32FullLinesStd - 2;   
    gu32FullLines = gu32FullLinesStd;   
    pstAeSnsDft->u32FullLines = gu32FullLines; 
                                                                                                                          
    return;                                                                                                               

}

static HI_VOID cmos_slow_framerate_set(HI_U32 u32FullLines,
    AE_SENSOR_DEFAULT_S *pstAeSnsDft)
{    
    if(WDR_MODE_2To1_LINE == genSensorMode)
    {
    	u32FullLines = (u32FullLines > 2*FULL_LINES_MAX_2TO1_WDR) ? 2*FULL_LINES_MAX_2TO1_WDR : u32FullLines;
        gu32FullLines = (u32FullLines >> 1) << 1;
		gu32RHS1_Max = ((u32FullLines >> 1) - gu32BRL) * 2 - 21; 
    }
    else
    {
    	u32FullLines = (u32FullLines > FULL_LINES_MAX) ? FULL_LINES_MAX : u32FullLines;
        gu32FullLines = u32FullLines;  
    }
    pstAeSnsDft->u32FullLines = gu32FullLines; 

    if(WDR_MODE_NONE == genSensorMode)
    {
        g_stSnsRegsInfo.astI2cData[5].u32Data = (gu32FullLines & 0xFF);
        g_stSnsRegsInfo.astI2cData[6].u32Data = ((gu32FullLines & 0xFF00) >> 8);
        g_stSnsRegsInfo.astI2cData[7].u32Data = ((gu32FullLines & 0x30000) >> 16);
    }
    else if(WDR_MODE_2To1_LINE == genSensorMode)                                                                                     
    {                                                                                         
        g_stSnsRegsInfo.astI2cData[8].u32Data = ((gu32FullLines>>1) & 0xFF);                       
        g_stSnsRegsInfo.astI2cData[9].u32Data = (((gu32FullLines>>1) & 0xFF00) >> 8);              
        g_stSnsRegsInfo.astI2cData[10].u32Data = (((gu32FullLines>>1) & 0xF0000) >> 16);           
    }
    else                                                                                      
    {                                                                                         
    }   

    pstAeSnsDft->u32MaxIntTime = gu32FullLines - 2; 

    return;
}

/* while isp notify ae to update sensor regs, ae call these funcs. */
static HI_VOID cmos_inttime_update(HI_U32 u32IntTime)
{
    static HI_BOOL bFirst = HI_TRUE; 
    HI_U32 u32Value = 0;

    static HI_U32 u32ShortIntTime;
    static HI_U32 u32LongIntTime;         
    static HI_U32 u32RHS1;  

    HI_U32 u32SHS1;                       
    HI_U32 u32SHS2; 

    HI_U32 u32YOUTSIZE;

    if (WDR_MODE_2To1_LINE == genSensorMode)                                                          
    {                                                                                                 
        /* short exposure */
        if (bFirst)                                                               
        {   
            au32WDRIntTime[0] = u32IntTime;   
            u32ShortIntTime = u32IntTime;                                                             
            bFirst = HI_FALSE;  

        }                                                                                             

       /* long exposure */ 
       else                                                                      
       {                                                                                             
           au32WDRIntTime[1] = u32IntTime;  
           u32LongIntTime = u32IntTime;                                                              

           /* allocate the SHS1 ,SHS2 and u32YOUTSIZE */ 
           u32SHS2 = gu32PreFullLines - u32LongIntTime - 1; 
           u32SHS1 = (u32ShortIntTime % 2) + 2;                                                      
           u32RHS1 = u32ShortIntTime + u32SHS1 + 1; 

           u32YOUTSIZE = (1097+(u32RHS1-1)/2+7)*2;
           u32YOUTSIZE = (u32YOUTSIZE >= 0x1FFF)?0x1FFF:u32YOUTSIZE;
                                                                                     
           g_stSnsRegsInfo.astI2cData[0].u32Data = (u32SHS1 & 0xFF);                                 
           g_stSnsRegsInfo.astI2cData[1].u32Data = ((u32SHS1 & 0xFF00) >> 8);                        
           g_stSnsRegsInfo.astI2cData[2].u32Data = ((u32SHS1 & 0xF0000) >> 16);                      
                                                                                                     
           g_stSnsRegsInfo.astI2cData[5].u32Data = (u32SHS2 & 0xFF);                                 
           g_stSnsRegsInfo.astI2cData[6].u32Data = ((u32SHS2 & 0xFF00) >> 8);                        
           g_stSnsRegsInfo.astI2cData[7].u32Data = ((u32SHS2 & 0xF0000) >> 16); 
                                                                                                      
           g_stSnsRegsInfo.astI2cData[11].u32Data = (u32RHS1 & 0xFF);                                
           g_stSnsRegsInfo.astI2cData[12].u32Data = ((u32RHS1 & 0xFF00) >> 8);                       
           g_stSnsRegsInfo.astI2cData[13].u32Data = ((u32RHS1 & 0xF0000) >> 16);  

           g_stSnsRegsInfo.astI2cData[14].u32Data = (u32YOUTSIZE & 0xFF);                                
           g_stSnsRegsInfo.astI2cData[15].u32Data = ((u32YOUTSIZE & 0x1F00) >> 8);
                                                                                              
          bFirst = HI_TRUE;                                                                         
       }                                                                                             
   }   
  else                                                                                             
  {
      u32Value = gu32FullLines - u32IntTime - 1; 
      
                                                                                                    
      g_stSnsRegsInfo.astI2cData[0].u32Data = (u32Value & 0xFF);                                    
      g_stSnsRegsInfo.astI2cData[1].u32Data = ((u32Value & 0xFF00) >> 8);                           
      g_stSnsRegsInfo.astI2cData[2].u32Data = ((u32Value & 0x30000) >> 16);                         
      bFirst = HI_TRUE;                                                                             
  }                                                                                                 
                                                                                                    
  return;                                                                                           

}

static HI_U32 gain_table[262]=
{
    1024,1059,1097,1135,1175,1217,1259,1304,1349,1397,1446,1497,1549,1604,1660,1719,1779,1842,1906,
    1973,2043,2048,2119,2194,2271,2351,2434,2519,2608,2699,2794,2892,2994,3099,3208,3321,3438,3559,
    3684,3813,3947,4086,4229,4378,4532,4691,4856,5027,5203,5386,5576,5772,5974,6184,6402,6627,6860,
    7101,7350,7609,7876,8153,8439,8736,9043,9361,9690,10030,10383,10748,11125,11516,11921,12340,12774,
    13222,13687,14168,14666,15182,15715,16267,16839,17431,18043,18677,19334,20013,20717,21445,22198,
    22978,23786,24622,25487,26383,27310,28270,29263,30292,31356,32458,33599,34780,36002,37267,38577,
    39932,41336,42788,44292,45849,47460,49128,50854,52641,54491,56406,58388,60440,62564,64763,67039,
    69395,71833,74358,76971,79676,82476,85374,88375,91480,94695,98023,101468,105034,108725,112545,
    116501,120595,124833,129220,133761,138461,143327,148364,153578,158975,164562,170345,176331,182528,
    188942,195582,202455,209570,216935,224558,232450,240619,249074,257827,266888,276267,285976,296026,
    306429,317197,328344,339883,351827,364191,376990,390238,403952,418147,432842,448053,463799,480098,
    496969,514434,532512,551226,570597,590649,611406,632892,655133,678156,701988,726657,752194,778627,
    805990,834314,863634,893984,925400,957921,991585,1026431,1062502,1099841,1138491,1178500,1219916,
    1262786,1307163,1353100,1400651,1449872,1500824,1553566,1608162,1664676,1723177,1783733,1846417,
    1911304,1978472,2048000,2119971,2194471,2271590,2351418,2434052,2519590,2608134,2699789,2794666,
    2892876,2994538,3099773,3208706,3321467,3438190,3559016,3684087,3813554,3947571,4086297,4229898,
    4378546,4532417,4691696,4856573,5027243,5203912,5386788,5576092,5772048,5974890,6184861,6402210,
    6627198,6860092,7101170,7350721,7609041,7876439,8153234
};

static HI_VOID cmos_again_calc_table(HI_U32 *pu32AgainLin, HI_U32 *pu32AgainDb)
{
    int i;

    if (*pu32AgainLin >= gain_table[120])
    {
         *pu32AgainLin = gain_table[120];
         *pu32AgainDb = 120;
         return ;
    }
    
    for (i = 1; i < 121; i++)
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

static HI_VOID cmos_dgain_calc_table(HI_U32 *pu32DgainLin, HI_U32 *pu32DgainDb)
{
    int i;

    if((HI_NULL == pu32DgainLin) ||(HI_NULL == pu32DgainDb))
    {
        printf("null pointer when get ae sensor gain info value!\n");
        return;
    }

    if (*pu32DgainLin >= gain_table[140])
    {
         *pu32DgainLin = gain_table[140];
         *pu32DgainDb = 140;
         return ;
    }
    
    for (i = 1; i < 141; i++)
    {
        if (*pu32DgainLin < gain_table[i])
        {
            *pu32DgainLin = gain_table[i - 1];
            *pu32DgainDb = i - 1;
            break;
        }
    }

    return;
}

static HI_VOID cmos_gains_update(HI_U32 u32Again, HI_U32 u32Dgain)
{  
    HI_U32 u32HCG = gu8HCGReg;
    HI_U32 u32Tmp;
    
    if(u32Again >= 21)
    {
        u32HCG = u32HCG | 0x10;  // bit[4] HCG  .Reg0x3009[7:0]
        u32Again = u32Again - 21;
    }

    u32Tmp=u32Again+u32Dgain;
        
    g_stSnsRegsInfo.astI2cData[3].u32Data = (u32Tmp & 0xFF);
    g_stSnsRegsInfo.astI2cData[4].u32Data = (u32HCG & 0xFF);

    return;
}

static HI_VOID cmos_get_inttime_max(HI_U32 u32Ratio, HI_U32 u32MinTime, HI_U32 *pu32IntTimeMax, HI_U32 *pu32LFMaxIntTime)
{
    HI_U32 u32IntTimeMaxTmp0 = 0;
    HI_U32 u32IntTimeMaxTmp  = 0;
    HI_U32 u32ShortTimeMinLimit = 0;

    u32ShortTimeMinLimit = (WDR_MODE_2To1_LINE == genSensorMode) ? 2 : ((WDR_MODE_3To1_LINE == genSensorMode) ? 3 : 2);
        
    if(WDR_MODE_2To1_LINE == genSensorMode)                                                                             
    {                                                                                                                        
        /*  limitation for DOL 2t1                                                                                     
                                                                                                                             
            SHS1 limitation:                                                                                                 
            2 or more                                                                                                        
            RHS1 - 2 or less                                                                                                 
                                                                                                                             
            SHS2 limitation:                                                                                                 
            RHS1 + 2 or more                                                                                                 
            FSC - 2 or less                                                                                                  
                                                                                                                             
            RHS1 Limitation                                                                                                  
            2n + 5 (n = 0,1,2...)                                                                                            
            RHS1 <= FSC - BRL * 2 - 21                                                                                       
                                                                                                                             
            short exposure time = RHS1 - (SHS1 + 1) <= RHS1 - 3                                                              
            long exposure time = FSC - (SHS2 + 1) <= FSC - (RHS1 + 3)                                                        
            ExposureShort + ExposureLong <= FSC - 6                                                                          
            short exposure time <= (FSC - 6) / (ratio + 1)                                                                   
        */
        if(ISP_FSWDR_LONG_FRAME_MODE == genFSWDRMode)
        {
            u32IntTimeMaxTmp0 = gu32PreFullLines - 6 - au32WDRIntTime[0];
            u32IntTimeMaxTmp = gu32FullLines - 10;
            u32IntTimeMaxTmp = (u32IntTimeMaxTmp0 < u32IntTimeMaxTmp) ? u32IntTimeMaxTmp0 : u32IntTimeMaxTmp;
            *pu32IntTimeMax = u32IntTimeMaxTmp;
            return;
        }
		else if(ISP_FSWDR_AUTO_LONG_FRAME_MODE == genFSWDRMode)
        {
        	if(au32WDRIntTime[0] == 2 && u32Ratio == 0x40)
			{
				u32IntTimeMaxTmp = gu32PreFullLines - 6 - au32WDRIntTime[0];
			}
			else
			{
				u32IntTimeMaxTmp0 = ((gu32PreFullLines - 6 - au32WDRIntTime[0]) * 0x40)  / DIV_0_TO_1(u32Ratio);                                                                                                                     
            	u32IntTimeMaxTmp = ((gu32FullLines - 6) * 0x40)  / DIV_0_TO_1(u32Ratio + 0x40); 
            	u32IntTimeMaxTmp = (u32IntTimeMaxTmp0 < u32IntTimeMaxTmp) ? u32IntTimeMaxTmp0 : u32IntTimeMaxTmp;
				u32IntTimeMaxTmp0 = gu32FullLines - 6 - (gu32RHS1_Max - 3);
				u32IntTimeMaxTmp0 = (u32IntTimeMaxTmp0 * 0x40) / DIV_0_TO_1(u32Ratio); 
				u32IntTimeMaxTmp = (u32IntTimeMaxTmp0 > u32IntTimeMaxTmp) ? u32IntTimeMaxTmp0 : u32IntTimeMaxTmp;
			}
			*pu32IntTimeMax = u32IntTimeMaxTmp;
			*pu32LFMaxIntTime = gu32RHS1_Max - 3;
            return;
        }
        else
        {
            u32IntTimeMaxTmp0 = ((gu32PreFullLines - 6 - au32WDRIntTime[0]) * 0x40)  / DIV_0_TO_1(u32Ratio);                                                                                                                     
            u32IntTimeMaxTmp = ((gu32FullLines - 6) * 0x40)  / DIV_0_TO_1(u32Ratio + 0x40); 
            u32IntTimeMaxTmp = (u32IntTimeMaxTmp0 < u32IntTimeMaxTmp) ? u32IntTimeMaxTmp0 : u32IntTimeMaxTmp;
            u32IntTimeMaxTmp = (u32IntTimeMaxTmp > (gu32RHS1_Max - 3))? (gu32RHS1_Max - 3) : u32IntTimeMaxTmp; 
        }   
    }
    else
    {
    }

    if(u32IntTimeMaxTmp >= u32ShortTimeMinLimit)
    {
        if (IS_LINE_WDR_MODE(genSensorMode))
        {
            *pu32IntTimeMax = u32IntTimeMaxTmp;
        }
        else
        {
        }
    }
    else
    {
        u32IntTimeMaxTmp = u32ShortTimeMinLimit; 
        if (WDR_MODE_2To1_LINE == genSensorMode)
        {
            *pu32IntTimeMax = u32IntTimeMaxTmp;
        }
        else
        {
        }
    }

    return;
                                                                                                                
}       


/* Only used in LINE_WDR mode */
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
    pstExpFuncs->pfn_cmos_dgain_calc_table  = cmos_dgain_calc_table;
    pstExpFuncs->pfn_cmos_get_inttime_max   = cmos_get_inttime_max;  
    pstExpFuncs->pfn_cmos_ae_fswdr_attr_set = cmos_ae_fswdr_attr_set;  

    return 0;
}


static AWB_CCM_S g_stAwbCcm =
{   
   4900,
   {
      0x1A4, 0x8096, 0x800E,
      0x8046,0x173,  0x802D,
      0x9,   0x80A6, 0x19D
   },
   3770,
   {
      0x19F,  0x8087, 0x8018,
      0x8060, 0x17B,  0x801B,
      0x11,   0x80BF, 0x1AE
   }, 
   2640,
   {     
      0x1a7,  0x8091, 0x8016,
      0x8084, 0x18F,  0x800B,
      0x1B,   0x8168, 0x24D
   }
  
};

static AWB_CCM_S g_stAwbCcmFsWdr =
{  

#if 0
	4900,
	{ 
		0x0122, 0x8042, 0x20,
		0x8040, 0x149, 0x8009,
		0x0029, 0x80B1, 0x0188 
	},

	3850,
	{
		0x011B, 0x8017, 0x8004,
		0x8036, 0x017B, 0x8045,
		0x000F, 0x80C8, 0x01B9
	},

	2650,
	{ 
		0x011B, 0x8017, 0x8004,
		0x8036, 0x017B, 0x8045,
		0x000F, 0x80C8, 0x01B9
	} 
#else

    4900,
	{ 
		0x0D2,  0x45,  0x8015,
		0x800E, 0x11A, 0x800C,
		0x00C,  0x12,  0xE2 
	},

	3850,
	{
		0xce,   0x4a, 0x8018,
		0x1,   0x10f, 0x8010,
		0x2a,   0x2e, 0xa8
	},

	2650,
	{ 
		0xce,     0x46, 0x8014,
		0x8004, 0x0113, 0x800f,
		0x0043, 0x8021, 0x0de
	}
        
  #endif

};  


static AWB_AGC_TABLE_S g_stAwbAgcTable =
{
    /* bvalid */
    1,
    
    /*1,  2,  4,  8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384, 32768*/
    /* saturation */   
    {0x80,0x7a,0x78,0x74,0x68,0x60,0x58,0x50,0x48,0x40,0x38,0x38,0x38,0x38,0x38,0x38}
};

static AWB_AGC_TABLE_S g_stAwbAgcTableFSWDR =
{
    /* bvalid */
    1,
    /* saturation */ 
    //{0x80,0x78,0x6e,0x64,0x5E,0x58,0x50,0x48,0x40,0x38,0x38,0x38,0x38,0x38,0x38,0x38}
    //{0x5a,0x5a,0x5a,0x58,0x55,0x50,0x50,0x48,0x40,0x38,0x38,0x38,0x38,0x38,0x38,0x38}
    {0x64,0x64,0x64,0x5a,0x55,0x50,0x50,0x48,0x40,0x38,0x38,0x38,0x38,0x38,0x38,0x38}
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

    pstAwbSnsDft->au16GainOffset[0] = 0x1C3;//R
    pstAwbSnsDft->au16GainOffset[1] = 0x100;//G
    pstAwbSnsDft->au16GainOffset[2] = 0x100;//G
    pstAwbSnsDft->au16GainOffset[3] = 0x1D4;//B

    pstAwbSnsDft->as32WbPara[0] = -37;
    pstAwbSnsDft->as32WbPara[1] = 293;
    pstAwbSnsDft->as32WbPara[2] = 0;
    pstAwbSnsDft->as32WbPara[3] = 179537;
    pstAwbSnsDft->as32WbPara[4] = 128;
    pstAwbSnsDft->as32WbPara[5] = -123691;

    
    
    switch (genSensorMode)
    {
        default:
        case WDR_MODE_NONE:
            memcpy(&pstAwbSnsDft->stAgcTbl, &g_stAwbAgcTable, sizeof(AWB_AGC_TABLE_S));
            memcpy(&pstAwbSnsDft->stCcm, &g_stAwbCcm, sizeof(AWB_CCM_S));
        break;
        case WDR_MODE_2To1_LINE:
            memcpy(&pstAwbSnsDft->stAgcTbl, &g_stAwbAgcTableFSWDR, sizeof(AWB_AGC_TABLE_S));            
            memcpy(&pstAwbSnsDft->stCcm, &g_stAwbCcmFsWdr, sizeof(AWB_CCM_S));
        break;
    }
    return 0;
}

HI_S32 cmos_init_awb_exp_function(AWB_SENSOR_EXP_FUNC_S *pstExpFuncs)
{
    memset(pstExpFuncs, 0, sizeof(AWB_SENSOR_EXP_FUNC_S));

    pstExpFuncs->pfn_cmos_get_awb_default = cmos_get_awb_default;

    return 0;
}




static ISP_CMOS_DEMOSAIC_S g_stIspDemosaic =
{
	1,    //bEnable
    /*au16EdgeSmoothThr*/
	{8,16,16,32,32,32,1022,1022,1022,1022,1022,1022,1022,1022,1022,1022},	
	/*au16EdgeSmoothSlope*/
	{8,32,32,48,48,48,0,0,0,0,0,0,0,0,0,0},
	//{4,16,32,32,32,32,0,0,0,0,0,0,0,0,0,0},
	/*au16AntiAliasThr*/
	{53,53,53,86,112,112,1022,1022,1022,1022,1022,1022,1022,1022,1022,1022},
	/*au16AntiAliasSlope*/
	{256,256,256,256,256,256,0,0,0,0,0,0,0,0,0,0},
    /*NrCoarseStr*/
    {128, 128, 128, 64, 64, 32, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16},
	/*NoiseSuppressStr*/
	//{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{4, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24, 26, 28, 30, 32, 36},
	/*DetailEnhanceStr*/
	{4, 4, 4, 4, 2, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    /*SharpenLumaStr*/
    {0+256, 0+256, 0+256, 0+256, 0+256, 0+256, 0+256, 0+256, 0+256, 0+256, 0+256, 0+256, 0+256, 0+256, 0+256, 0+256},
    /*ColorNoiseCtrlThr*/
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
};

static ISP_CMOS_DEMOSAIC_S g_stIspDemosaicWDR =
{
	1,    //bEnable
    /*au16EdgeSmoothThr*/
	{160,160,160,160,160,160,1022,1022,1022,1022,1022,1022,1022,1022,1022,1022},
	/*au16EdgeSmoothSlope*/
	{32,32,32,32,32,32,0,0,0,0,0,0,0,0,0,0},
	/*au16AntiAliasThr*/
	{193,193,193,193,193,193,1022,1022,1022,1022,1022,1022,1022,1022,1022,1022},
	/*au16AntiAliasSlope*/
	{256,256,256,256,256,256,0,0,0,0,0,0,0,0,0,0},
    /*NrCoarseStr*/
    {128, 128, 128, 64, 64, 32, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16},
	/*NoiseSuppressStr*/
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	/*DetailEnhanceStr*/
	{10, 8, 4, 4, 2, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    /*SharpenLumaStr*/
    {0+256, 0+256, 0+256, 0+256, 0+256, 0+256, 0+256, 0+256, 0+256, 0+256, 0+256, 0+256, 0+256, 0+256, 0+256, 0+256},
    /*ColorNoiseCtrlThr*/
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
};

static ISP_CMOS_GE_S g_stIspGe =
{
	/*For GE*/
	1,    /*bEnable*/			
	9,    /*u8Slope*/	
	9,    /*u8SensiSlope*/	
	300, /*u16SensiThr*/	
	{300,300,300,300,310,310,310,  310,  320,320,320,320,330,330,330,330}, /*au16Threshold[ISP_AUTO_ISO_STRENGTH_NUM]*/
	{ 128, 128, 128, 128, 129, 129, 129,   129,   130, 130, 130, 130, 131, 131, 131, 131}, /*au16Strength[ISP_AUTO_ISO_STRENGTH_NUM]*/
	{1024,1024,1024,2048,2048,2048,2048,  2048,  2048,2048,2048,2048,2048,2048,2048,2048}    /*au16NpOffset[ISP_AUTO_ISO_STRENGTH_NUM]*/	
};

static ISP_CMOS_GE_S g_stIspWdrGe =
{
	/*For GE*/
	0,    /*bEnable*/			
	9,    /*u8Slope*/	
	9,    /*u8SensiSlope*/	
	300, /*u16SensiThr*/	
	{300,300,300,300,310,310,310,  310,  320,320,320,320,330,330,330,330}, /*au16Threshold[ISP_AUTO_ISO_STRENGTH_NUM]*/
	{ 128, 128, 128, 128, 129, 129, 129,   129,   130, 130, 130, 130, 131, 131, 131, 131}, /*au16Strength[ISP_AUTO_ISO_STRENGTH_NUM]*/
	{1024,1024,1024,2048,2048,2048,2048,  2048,  2048,2048,2048,2048,2048,2048,2048,2048}    /*au16NpOffset[ISP_AUTO_ISO_STRENGTH_NUM]*/	
};

static ISP_CMOS_FCR_S g_stIspFcr =
{
	/*For FCR*/
	1,    /*bEnable*/				
	{8,8,8,8,7,7,7,  6,  6,6,5,4,3,2,1,0}, /*au8Strength[ISP_AUTO_ISO_STRENGTH_NUM]*/
	{ 24, 24, 24, 24, 20, 20, 20,   16,   14, 12, 10, 8, 6, 4, 2, 0}, /*au8Threhold[ISP_AUTO_ISO_STRENGTH_NUM]*/
	{150,150,150,150,150,150,150,  150,  150,150,150,150,150,150,150,150}    /*au16Offset[ISP_AUTO_ISO_STRENGTH_NUM]*/	
};

static ISP_CMOS_FCR_S g_stIspWdrFcr =
{
	/*For FCR*/
	1,    /*bEnable*/				
	{8,8,8,8,7,7,7,  6,  6,6,5,4,3,2,1,0}, /*au8Strength[ISP_AUTO_ISO_STRENGTH_NUM]*/
	{ 24, 24, 24, 24, 20, 20, 20,   16,   14, 12, 10, 8, 6, 4, 2, 0}, /*au8Threhold[ISP_AUTO_ISO_STRENGTH_NUM]*/
	{150,150,150,150,150,150,150,  150,  150,150,150,150,150,150,150,150}    /*au16Offset[ISP_AUTO_ISO_STRENGTH_NUM]*/	
};
               
static ISP_CMOS_YUV_SHARPEN_S g_stIspYuvSharpen = 
{
     /* bvalid */
     1,

     /* 100,  200,    400,     800,    1600,    3200,    6400,    12800,    25600,   51200,  102400,  204800,   409600,   819200,   1638400,  3276800 */   
   
     
	 /* au8SharpenUd */
	 {23 ,	 22 ,     20,    19,     18,	   15,	    15,   	 13,	   6,	   	  6,	  	6,	  	5,	  	5,	  	5,	  	5,    5},
	
	/* au8SharpenD */

     {75 ,   75 ,    72,     70,    70,	       68,      65,      60,	   45,	     45,	   40,	  40,	  38,	  38,	  38,    38},
	
     /*au8TextureThd*/
     {0 ,    0,        0,      0 ,    0,       0,       0,       0,      0,        0,         0,      0,       0,    0,      0,     0},

     /* au8SharpenEdge */  

     {80  ,    80,     80,     72,     72,     70,      70,      70,       110,      120,       120,     120,   120,    120,    120,   120},
     
     /* au8EdgeThd */
     {80 ,     80 ,     80,    80,     80,     80,     90,       90,      120,       150,      150,    150,    150,    150,    150,   150},

     /* au8OverShoot */
     {150 ,   150 ,    150,    150,    140,    130,     120,     100,     80,       80,    80,     80,     80,     80,     80,    80},
        
     /* au8UnderShoot */
     {200  ,  200  ,   200,    200,    170,    150,     120,     100,     80,       80,    80,     80,    80,     80,     80,    80},

     /*au8shootSupSt*/
	 {33,	  33,	   33,	   33,	   16,		0,     0,		0,		0,		0,		0,			0,			0,		0,		0,		0},

     /* au8DetailCtrl  */
     {128  ,  128  ,   128,    128,    128,    118,    118,     108,     80,      80,    78,     50,    50,    50,     40,      30},
	
	
};   

static ISP_CMOS_YUV_SHARPEN_S g_stIspYuvWdrSharpen = 
{
     /* bvalid */
     1,

     /* 100,  200,    400,     800,    1600,    3200,    6400,    12800,    25600,   51200,  102400,  204800,   409600,   819200,   1638400,  3276800 */
     
	 /* au8SharpenUd */
	 {25 ,	23 ,  22,   16,    14,	    12,	    10,   	8,	    6,	   	4,	  	2,	  	2,	  	2,	  	2,	  	2,    	2},
	
	/* au8SharpenD */

     {75,	72,	   72,   70,    70,	   70,	  70,      70,	  60,      50,	  40,	  40,	  40,	  40,	  40,    40},

     //{75,	72,	   70,   70,    70,	   70,	  70,      70,	  60,      50,	  40,	  40,	  40,	  40,	  40,    40},
	 
	 /*au8TextureThd*/
	 {0,    0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0},

     /* au8SharpenEdge */
     {70,   70,    70,    40,    40,    45,     45,     40,     30,    30,    30,      30,     30,     30,     30,    30},


     /* au8EdgeThd */     
     {80 ,     80 ,     80,    80,     80,     80,     90,       90,     90,       100,   100,    100,    100,    100,    100,   100}, 
	 
	 /* au8OverShoot */
     {140,   120,    120,    120,    120,    100,     80,     60,     50,    40,    30,     20,     10,     0,     0,    0},
        
     /* au8UnderShoot */
     {200,   160,    150,    140,    130,   120,     110,     90,     70,    50,    30,     25,     20,     10,     10,    10},
	 
	 /*au8shootSupSt*/
	 {33,	  33,	   33,	   33,	   16,		0,     0,		0,		0,		0,		0,			0,			0,		0,		0,		0},

     /* au8DetailCtrl */
     {160  ,  128,	  128,    128,    128,    128,    128,    128,    128,    128,    128,     128,    128,    128,     128,      128},

	
};   

static ISP_CMOS_DRC_S g_stIspDRC = 
{
    /*bEnable*/
    0,    
    /*u8SpatialVar*/
    6,    
	/*u8RangeVar*/             
	8,
	/*u8Asymmetry8*/
    2,
	/*u8SecondPole; */
    180,
	/*u8Stretch*/
    54,
	/*u8Compress*/ 
    180,
	/*u8PDStrength*/            
    35,
	/*u8LocalMixingBrigtht*/  
    45,
    /*u8LocalMixingDark*/
    45,
    /*ColorCorrectionLut[33];*/
   {1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024},
};

#if 0

static ISP_CMOS_DRC_S g_stIspDRCWDR = 
{
    /*bEnable*/
    1,    
    /*u8SpatialVar*/
    10,    
	/*u8RangeVar*/             
	15,
	/*u8Asymmetry8*/
    2,
	/*u8SecondPole; */
    200,
	/*u8Stretch*/
    40,
	/*u8Compress*/ 
    100,
	/*u8PDStrength*/            
    35,
	/*u8LocalMixingBrigtht*/  
    45,
    /*u8LocalMixingDark*/
    45,
    /*ColorCorrectionLut[33];*/
   /*{900,890,880,870,860,850,840,830,820,810,800,790,780,770,760,750,740,730,720,710,700,690,680,675,670,665,660,655,650,645,640,635,630},*/
    {1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024},
};
#endif


static ISP_CMOS_DRC_S g_stIspDRCWDR = 
{
    /*bEnable*/
    1,    
    /*u8SpatialVar*/
    10,    
	/*u8RangeVar*/             
	10,
	/*u8Asymmetry8*/
    2,
	/*u8SecondPole; */
    200,
	/*u8Stretch*/
    40,
	/*u8Compress*/ 
    110,
	/*u8PDStrength*/            
    35,
	/*u8LocalMixingBrigtht*/  
    45,
    /*u8LocalMixingDark*/
    35,
    /*ColorCorrectionLut[33];*/
    /*{900,890,880,870,860,850,840,830,820,810,800,790,780,770,760,750,740,730,720,710,700,690,680,675,670,665,660,655,650,645,640,635,630},*/
    {1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024},
}; 


#if 0


static ISP_CMOS_DRC_S g_stIspDRCWDR = 
{
    /*bEnable*/
    1,    
    /*u8SpatialVar*/
    3,    
	/*u8RangeVar*/             
	6,
	/*u8Asymmetry8*/
    2,
	/*u8SecondPole; */
    200,
	/*u8Stretch*/
    40,
	/*u8Compress*/ 
    110,
	/*u8PDStrength*/            
    35,
	/*u8LocalMixingBrigtht*/  
    45,
    /*u8LocalMixingDark*/
    30,
    /*ColorCorrectionLut[33];*/
    /*{900,890,880,870,860,850,840,830,820,810,800,790,780,770,760,750,740,730,720,710,700,690,680,675,670,665,660,655,650,645,640,635,630},*/
    {1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024},
};  

#endif


static ISP_CMOS_GAMMA_S g_stIspGamma =
{
    /* bvalid */
    1,
    
#if 0    
    {0, 180, 320, 426, 516, 590, 660, 730, 786, 844, 896, 946, 994, 1040, 1090, 1130, 1170, 1210, 1248,
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
#else  /*higher  contrast*/
    {0   ,120 ,220 ,310 ,390 ,470 ,540 ,610 ,670 ,730 ,786 ,842 ,894 ,944 ,994 ,1050,    
    1096,1138,1178,1218,1254,1280,1314,1346,1378,1408,1438,1467,1493,1519,1543,1568,    
    1592,1615,1638,1661,1683,1705,1726,1748,1769,1789,1810,1830,1849,1869,1888,1907,    
    1926,1945,1963,1981,1999,2017,2034,2052,2069,2086,2102,2119,2136,2152,2168,2184,    
    2200,2216,2231,2247,2262,2277,2292,2307,2322,2337,2351,2366,2380,2394,2408,2422,    
    2436,2450,2464,2477,2491,2504,2518,2531,2544,2557,2570,2583,2596,2609,2621,2634,    
    2646,2659,2671,2683,2696,2708,2720,2732,2744,2756,2767,2779,2791,2802,2814,2825,    
    2837,2848,2859,2871,2882,2893,2904,2915,2926,2937,2948,2959,2969,2980,2991,3001,    
    3012,3023,3033,3043,3054,3064,3074,3085,3095,3105,3115,3125,3135,3145,3155,3165,    
    3175,3185,3194,3204,3214,3224,3233,3243,3252,3262,3271,3281,3290,3300,3309,3318,    
    3327,3337,3346,3355,3364,3373,3382,3391,3400,3409,3418,3427,3436,3445,3454,3463,    
    3471,3480,3489,3498,3506,3515,3523,3532,3540,3549,3557,3566,3574,3583,3591,3600,    
    3608,3616,3624,3633,3641,3649,3657,3665,3674,3682,3690,3698,3706,3714,3722,3730,    
    3738,3746,3754,3762,3769,3777,3785,3793,3801,3808,3816,3824,3832,3839,3847,3855,    
    3862,3870,3877,3885,3892,3900,3907,3915,3922,3930,3937,3945,3952,3959,3967,3974,    
    3981,3989,3996,4003,4010,4018,4025,4032,4039,4046,4054,4061,4068,4075,4082,4089,4095}
#endif
};

static ISP_CMOS_GAMMA_S g_stIspGammaFSWDR =
{
    /* bvalid */
    1,
    
#if 1    
	{
		0,    16,    32,    48,    64,    80,    96,   112,   128,   144,   160,   176,   192,   208,   224,   240,
		256,   272,   288,   304,   320,   336,   352,   368,   384,   400,   416,   432,   448,   464,   480,   496,
		512,   528,   544,   560,   576,   592,   608,   624,   640,   656,   672,   688,   704,   720,   736,   752,
		768,   784,   800,   816,   832,   848,   864,   880,   896,   912,   928,   944,   960,   976,   992,  1008,
		1024,  1040,  1056,  1072,  1088,  1104,  1120,  1136,  1152,  1168,  1184,  1200,  1216,  1232,  1248,  1264,
		1280,  1296,  1312,  1328,  1344,  1360,  1376,  1392,  1408,  1424,  1440,  1456,  1472,  1488,  1504,  1520,
		1536,  1552,  1568,  1584,  1600,  1616,  1632,  1648,  1664,  1680,  1696,  1712,  1728,  1744,  1760,  1776,
		1792,  1808,  1824,  1840,  1856,  1872,  1888,  1904,  1920,  1936,  1952,  1968,  1984,  2000,  2016,  2032,
		2048,  2064,  2080,  2096,  2112,  2128,  2144,  2160,  2176,  2192,  2208,  2224,  2240,  2256,  2272,  2288,
		2304,  2320,  2336,  2352,  2368,  2384,  2400,  2416,  2432,  2448,  2464,  2480,  2496,  2512,  2528,  2544,
		2560,  2576,  2592,  2608,  2624,  2640,  2656,  2672,  2688,  2704,  2720,  2736,  2752,  2768,  2784,  2800,
		2816,  2832,  2848,  2864,  2880,  2896,  2912,  2928,  2944,  2960,  2976,  2992,  3008,  3024,  3040,  3056,
		3072,  3088,  3104,  3120,  3136,  3152,  3168,  3184,  3200,  3216,  3232,  3248,  3264,  3280,  3296,  3312,
		3328,  3344,  3360,  3376,  3392,  3408,  3424,  3440,  3456,  3472,  3488,  3504,  3520,  3536,  3552,  3568,
		3584,  3600,  3616,  3632,  3648,  3664,  3680,  3696,  3712,  3728,  3744,  3760,  3776,  3792,  3808,  3824,
		3840,  3856,  3872,  3888,  3904,  3920,  3936,  3952,  3968,  3984,  4000,  4016,  4032,  4048,  4064,  4080,
		4095
	}
#else  /*higher  contrast*/
	{
	    //0,1,2,4,8,12,17,23,30,38,47,57,68,79,92,105,120,133,147,161,176,192,209,226,243,260,278,296,317,340,365,390,416,440,466,491,517,538,561,584,607,631,656,680,705,730,756,784,812,835,858,882,908,934,958,982,1008,1036,1064,1092,1119,1143,1167,1192,1218,1243,1269,1296,1323,1351,1379,1408,1434,1457,1481,1507,1531,1554,1579,1603,1628,1656,1683,1708,1732,1756,1780,1804,1829,1854,1877,1901,1926,1952,1979,2003,2024,2042,2062,2084,2106,2128,2147,2168,2191,2214,2233,2256,2278,2296,2314,2335,2352,2373,2391,2412,2431,2451,2472,2492,2513,2531,2547,2566,2581,2601,2616,2632,2652,2668,2688,2705,2721,2742,2759,2779,2796,2812,2826,2842,2857,2872,2888,2903,2920,2934,2951,2967,2983,3000,3015,3033,3048,3065,3080,3091,3105,3118,3130,3145,3156,3171,3184,3197,3213,3224,3240,3252,3267,3281,3295,3310,3323,3335,3347,3361,3372,3383,3397,3409,3421,3432,3447,3459,3470,3482,3497,3509,3521,3534,3548,3560,3572,3580,3592,3602,3613,3625,3633,3646,3657,3667,3679,3688,3701,3709,3719,3727,3736,3745,3754,3764,3773,3781,3791,3798,3806,3816,3823,3833,3840,3847,3858,3865,3872,3879,3888,3897,3904,3911,3919,3926,3933,3940,3948,3955,3962,3970,3973,3981,3988,3996,4003,4011,4018,4026,4032,4037,4045,4053,4057,4064,4072,4076,4084,4088,4095        
	    0,1,2,4,8,12,17,23,30,38,47,57,68,79,92,105,120,133,147,161,176,192,209,226,243,260,278,296,317,340,365,390,416,440,466,491,517,538,561,584,607,631,656,680,705,730,756,784,812,835,858,882,908,934,958,982,1008,1036,1064,1092,1119,1143,1167,1192,1218,1243,1269,1294,1320,1346,1372,1398,1424,1450,1476,1502,1528,1554,1580,1607,1633,1658,1684,1710,1735,1761,1786,1811,1836,1860,1884,1908,1932,1956,1979,2002,2024,2046,2068,2090,2112,2133,2154,2175,2196,2217,2237,2258,2278,2298,2318,2337,2357,2376,2395,2414,2433,2451,2469,2488,2505,2523,2541,2558,2575,2592,2609,2626,2642,2658,2674,2690,2705,2720,2735,2750,2765,2779,2793,2807,2821,2835,2848,2861,2874,2887,2900,2913,2925,2937,2950,2962,2974,2986,2998,3009,3021,3033,3044,3056,3067,3078,3088,3099,3109,3119,3129,3139,3148,3158,3168,3177,3187,3197,3207,3217,3227,3238,3248,3259,3270,3281,3292,3303,3313,3324,3335,3346,3357,3368,3379,3389,3400,3410,3421,3431,3441,3451,3461,3471,3481,3491,3501,3511,3521,3531,3541,3552,3562,3572,3583,3593,3604,3615,3625,3636,3646,3657,3668,3679,3689,3700,3711,3721,3732,3743,3753,3764,3774,3784,3795,3805,3816,3826,3837,3847,3858,3869,3880,3891,3902,3913,3925,3937,3949,3961,3973,3985,3997,4009,4022,4034,4046,4058,4071,4083,4095
	}
#endif
};

#if 0
static ISP_CMOS_GAMMAFE_S g_stGammafeFSWDR = 
{
    /* bvalid */
    1,

    /* gamma_fe0 */
    {
        0,32767,65535,98303,131071,163839,196607,229375,262143,294911,327679,360447,393215,425983,458751,491519,524287,557055,589823,622591,655359,688127,720895,753663,786431,819199,851967,884735,917503,950271,983039,1015807,1048575
    },

    /* gamma_fe1 */
    {
        0,4095,8191,12287,16383,20479,24575,28671,32767,36863,40959,45055,49151,53247,57343,61439,65535,69631,73727,77823,81919,86015,90111,94207,98303,102399,106495,110591,114687,118783,122879,126975,131071,135167,139263,143359,147455,151551,155647,159743,163839,167935,172031,176127,180223,184319,188415,192511,196607,200703,204799,208895,212991,217087,221183,225279,229375,233471,237567,241663,245759,249855,253951,258047,262143,266239,270335,274431,278527,282623,286719,290815,294911,299007,303103,307199,311295,315391,319487,323583,327679,331775,335871,339967,344063,348159,352255,356351,360447,364543,368639,372735,376831,380927,385023,389119,393215,397311,401407,405503,409599,413695,417791,421887,425983,430079,434175,438271,442367,446463,450559,454655,458751,462847,466943,471039,475135,479231,483327,487423,491519,495615,499711,503807,507903,511999,516095,520191,524287,528383,532479,536575,540671,544767,548863,552959,557055,561151,565247,569343,573439,577535,581631,585727,589823,593919,598015,602111,606207,610303,614399,618495,622591,626687,630783,634879,638975,643071,647167,651263,655359,659455,663551,667647,671743,675839,679935,684031,688127,692223,696319,700415,704511,708607,712703,716799,720895,724991,729087,733183,737279,741375,745471,749567,753663,757759,761855,765951,770047,774143,778239,782335,786431,790527,794623,798719,802815,806911,811007,815103,819199,823295,827391,831487,835583,839679,843775,847871,851967,856063,860159,864255,868351,872447,876543,880639,884735,888831,892927,897023,901119,905215,909311,913407,917503,921599,925695,929791,933887,937983,942079,946175,950271,954367,958463,962559,966655,970751,974847,978943,983039,987135,991231,995327,999423,1003519,1007615,1011711,1015807,1019903,1023999,1028095,1032191,1036287,1040383,1044479,1048575
    }
};
#endif



static ISP_CMOS_DPC_S g_stCmosDpc = 
{
	//0,/*IR_channel*/
	//0,/*IR_position*/
	{0,0,0,0,200,200,220,220,220,220,152,152,152,152,152,152},/*au16Strength[16]*/
	{0,0, 0, 0, 0, 0, 0, 0, 0, 0, 50, 50, 50, 50, 50, 50},/*au16BlendRatio[16]*/
};	

static ISP_LSC_CABLI_UNI_TABLE_S g_stCmosLscUniTable = 
{
   /*Mesh Grid Info: Width*/
   {60, 60, 60, 60, 60, 60, 60, 60}, 
   /*Mesh Grid Info: Height*/
   {34, 34, 34, 34, 34, 34, 33, 33},
};


static ISP_LSC_CABLI_TABLE_S g_stCmosLscTable[HI_ISP_LSC_LIGHT_NUM] = 
{
    {
		/*AWB RGain*/
        384,
        /*AWB BGain*/
        563,
		//Rgain:
		{
			1431,1326,1239,1187,1148,1117,1103,1090,1091,1095,1108,1128,1161,1203,1288,1409,1564,
			1391,1293,1215,1166,1129,1103,1085,1077,1077,1079,1091,1107,1136,1175,1248,1365,1512,
			1351,1260,1193,1148,1110,1084,1071,1067,1068,1068,1075,1090,1114,1156,1221,1325,1458,
			1320,1233,1173,1128,1094,1072,1064,1059,1058,1060,1064,1072,1103,1137,1196,1293,1429,
			1296,1211,1158,1113,1080,1064,1057,1051,1051,1048,1056,1065,1089,1125,1179,1280,1402,
			1273,1202,1145,1099,1071,1055,1052,1045,1041,1040,1050,1057,1079,1116,1170,1263,1380,
			1258,1186,1132,1090,1064,1053,1046,1038,1036,1037,1045,1057,1073,1109,1163,1252,1363,
			1245,1181,1124,1087,1062,1051,1040,1036,1030,1036,1042,1051,1067,1105,1159,1241,1354,
			1242,1179,1125,1086,1062,1048,1036,1030,1026,1033,1039,1050,1066,1102,1155,1231,1337,
			1239,1175,1126,1085,1057,1045,1037,1030,1024,1030,1037,1051,1064,1100,1152,1230,1326,
			1247,1177,1129,1085,1060,1048,1037,1033,1030,1033,1038,1049,1063,1101,1150,1227,1336,
			1252,1187,1133,1089,1066,1053,1040,1035,1034,1037,1039,1051,1066,1101,1155,1236,1343,
			1267,1192,1140,1104,1070,1058,1048,1039,1040,1042,1047,1055,1069,1110,1171,1252,1356,
			1280,1204,1152,1114,1078,1060,1052,1047,1043,1047,1052,1058,1082,1133,1186,1271,1380,
			1309,1224,1168,1125,1092,1071,1061,1053,1053,1053,1057,1067,1113,1158,1208,1297,1408,
			1339,1247,1186,1142,1109,1083,1072,1065,1061,1060,1066,1093,1142,1181,1238,1335,1446,
			1375,1277,1206,1162,1122,1102,1083,1070,1066,1070,1085,1118,1164,1198,1262,1372,1487,
		},        
               
		//Grgain:
		{
			1452,1343,1249,1194,1156,1128,1109,1098,1099,1106,1131,1157,1195,1245,1336,1465,1630,
			1411,1306,1226,1173,1134,1106,1089,1086,1083,1090,1108,1132,1164,1212,1295,1419,1582,
			1372,1272,1200,1151,1111,1086,1074,1069,1070,1075,1085,1109,1140,1189,1262,1378,1528,
			1340,1243,1181,1133,1093,1075,1065,1062,1061,1063,1073,1092,1124,1173,1235,1346,1493,
			1319,1223,1166,1117,1081,1065,1057,1052,1050,1053,1065,1081,1111,1154,1218,1334,1466,
			1292,1213,1152,1103,1073,1057,1052,1044,1041,1045,1061,1075,1101,1147,1212,1313,1445,
			1278,1201,1140,1098,1068,1053,1044,1037,1038,1041,1055,1074,1092,1139,1205,1299,1422,
			1269,1193,1136,1093,1067,1052,1040,1034,1030,1040,1052,1070,1089,1133,1200,1290,1416,
			1260,1188,1131,1091,1065,1047,1039,1031,1025,1034,1049,1064,1090,1132,1193,1282,1401,
			1259,1188,1132,1091,1062,1045,1036,1030,1024,1033,1046,1065,1087,1128,1189,1275,1387,
			1265,1190,1137,1092,1065,1051,1039,1034,1029,1035,1049,1064,1085,1129,1187,1274,1391,
			1275,1200,1142,1095,1068,1057,1042,1038,1035,1042,1052,1069,1087,1130,1194,1284,1405,
			1286,1208,1150,1110,1075,1060,1049,1043,1042,1047,1057,1070,1090,1140,1210,1301,1423,
			1308,1221,1162,1121,1082,1063,1056,1050,1049,1052,1060,1074,1100,1174,1231,1324,1447,
			1332,1238,1180,1135,1099,1071,1065,1059,1059,1060,1071,1086,1142,1198,1257,1356,1476,
			1367,1262,1199,1153,1114,1090,1074,1069,1067,1070,1080,1115,1176,1225,1290,1393,1516,
			1399,1292,1218,1171,1132,1105,1092,1074,1071,1078,1099,1142,1201,1249,1318,1434,1560,
		},          
        
		//Gbgain:
		{
			1348,1252,1171,1131,1098,1074,1065,1054,1048,1050,1059,1075,1089,1125,1197,1311,1442,
			1313,1218,1151,1110,1084,1062,1053,1043,1039,1038,1045,1056,1072,1101,1170,1267,1400,
			1285,1195,1136,1099,1071,1058,1045,1038,1037,1036,1038,1045,1062,1091,1146,1233,1367,
			1257,1176,1129,1092,1065,1053,1044,1040,1036,1034,1034,1038,1056,1081,1123,1209,1344,
			1238,1163,1123,1081,1056,1048,1042,1035,1033,1031,1032,1034,1050,1072,1113,1205,1322,
			1220,1162,1110,1075,1056,1048,1042,1034,1033,1029,1030,1035,1048,1070,1107,1192,1297,
			1213,1148,1105,1073,1058,1045,1039,1034,1035,1027,1030,1037,1041,1065,1106,1181,1282,
			1207,1147,1104,1075,1056,1046,1039,1038,1026,1028,1031,1031,1039,1064,1105,1172,1270,
			1200,1146,1102,1076,1060,1046,1039,1035,1024,1031,1032,1035,1040,1064,1100,1167,1262,
			1205,1144,1108,1077,1060,1047,1042,1035,1027,1029,1033,1038,1041,1065,1099,1161,1246,
			1209,1149,1114,1081,1062,1055,1047,1040,1033,1031,1040,1036,1039,1067,1106,1164,1255,
			1218,1160,1121,1086,1068,1057,1047,1046,1040,1039,1036,1039,1045,1068,1105,1168,1276,
			1232,1174,1131,1098,1071,1065,1055,1050,1045,1046,1046,1043,1043,1074,1113,1183,1291,
			1253,1182,1140,1111,1079,1067,1062,1054,1051,1049,1046,1044,1054,1090,1124,1202,1301,
			1272,1203,1158,1122,1093,1075,1069,1062,1055,1055,1054,1051,1078,1108,1137,1222,1329,
			1314,1224,1170,1137,1108,1091,1077,1071,1063,1061,1061,1073,1102,1124,1168,1254,1367,
			1351,1251,1187,1146,1126,1109,1091,1077,1072,1068,1076,1095,1116,1140,1190,1292,1421,
		},      
               
		//Bgain: 
		{
			1408,1301,1212,1156,1120,1093,1077,1068,1067,1074,1093,1115,1153,1202,1287,1412,1577,
			1370,1270,1187,1141,1105,1078,1064,1058,1057,1063,1075,1099,1130,1175,1256,1376,1517,
			1334,1242,1174,1129,1091,1066,1053,1052,1052,1056,1065,1083,1114,1159,1230,1339,1479,
			1306,1220,1158,1111,1078,1058,1048,1046,1047,1048,1056,1068,1102,1144,1206,1309,1449,
			1289,1202,1147,1100,1067,1053,1045,1040,1040,1040,1052,1065,1092,1135,1195,1300,1422,
			1270,1195,1137,1090,1062,1047,1040,1035,1033,1038,1048,1063,1084,1127,1189,1287,1407,
			1254,1185,1128,1086,1056,1047,1039,1031,1031,1037,1044,1061,1078,1122,1182,1278,1391,
			1250,1181,1125,1084,1058,1045,1036,1030,1027,1034,1045,1058,1077,1120,1179,1270,1379,
			1243,1179,1126,1085,1061,1044,1035,1027,1024,1035,1043,1058,1077,1118,1179,1262,1375,
			1245,1180,1127,1085,1061,1043,1036,1028,1024,1033,1043,1058,1080,1119,1176,1262,1368,
			1252,1184,1134,1089,1063,1049,1039,1034,1034,1038,1048,1061,1080,1122,1178,1262,1375,
			1263,1193,1140,1094,1067,1054,1043,1038,1039,1043,1050,1064,1082,1125,1185,1272,1386,
			1274,1203,1146,1109,1074,1062,1051,1044,1047,1052,1059,1071,1090,1135,1204,1292,1404,
			1293,1216,1159,1121,1083,1066,1058,1052,1052,1059,1065,1074,1103,1162,1221,1310,1425,
			1320,1234,1178,1133,1098,1077,1069,1064,1062,1064,1072,1085,1139,1189,1244,1339,1455,
			1352,1259,1197,1152,1115,1093,1080,1074,1072,1073,1085,1115,1171,1212,1282,1379,1503,
			1396,1287,1215,1172,1133,1109,1095,1082,1080,1088,1105,1140,1194,1240,1305,1424,1546,
		},                                                                                                                          
	},	

	{
		/*AWB RGain*/
        486,
        /*AWB BGain*/
        409,
		//Rgain:
		{
			1478,1356,1253,1196,1156,1137,1124,1112,1107,1110,1122,1143,1175,1219,1297,1404,1564,
			1433,1312,1225,1172,1139,1115,1104,1097,1093,1090,1102,1124,1152,1190,1257,1361,1504,
			1384,1276,1200,1150,1114,1093,1083,1081,1078,1077,1082,1098,1127,1166,1223,1317,1442,
			1341,1244,1176,1126,1094,1078,1074,1068,1065,1065,1069,1081,1110,1146,1196,1279,1404,
			1312,1219,1157,1109,1078,1068,1062,1056,1054,1050,1059,1069,1093,1129,1175,1268,1381,
			1282,1202,1138,1091,1064,1057,1055,1046,1041,1041,1050,1058,1081,1114,1166,1250,1356,
			1267,1184,1125,1083,1059,1051,1046,1036,1032,1034,1041,1054,1069,1108,1156,1235,1338,
			1256,1179,1118,1078,1055,1048,1040,1032,1028,1028,1037,1048,1063,1102,1152,1226,1326,
			1246,1176,1115,1074,1056,1042,1035,1028,1026,1026,1033,1042,1057,1097,1146,1219,1319,
			1245,1174,1118,1076,1054,1040,1034,1029,1024,1024,1028,1040,1055,1096,1146,1212,1313,
			1250,1177,1123,1077,1057,1044,1035,1030,1027,1025,1030,1039,1056,1096,1145,1215,1317,
			1262,1184,1127,1082,1059,1051,1040,1031,1027,1025,1030,1040,1056,1095,1150,1225,1332,
			1275,1192,1133,1094,1067,1053,1046,1036,1034,1031,1032,1043,1060,1105,1163,1241,1349,
			1291,1207,1144,1106,1074,1056,1057,1043,1036,1036,1038,1045,1070,1128,1179,1264,1366,
			1320,1221,1162,1115,1084,1064,1061,1048,1042,1037,1040,1050,1101,1148,1197,1285,1396,
			1353,1246,1176,1130,1099,1080,1066,1056,1048,1044,1046,1075,1127,1168,1225,1321,1441,
			1389,1277,1195,1147,1113,1095,1082,1058,1054,1053,1064,1098,1146,1186,1250,1359,1480,

		},        
	           
		//Grgain:
		{
			1505,1370,1264,1205,1165,1143,1125,1116,1116,1120,1142,1172,1212,1265,1345,1467,1633,
			1457,1331,1239,1184,1143,1119,1109,1102,1099,1103,1117,1144,1180,1228,1304,1418,1579,
			1409,1291,1210,1156,1118,1096,1088,1085,1081,1084,1094,1116,1154,1201,1266,1371,1519,
			1364,1254,1184,1134,1097,1079,1073,1070,1067,1068,1078,1099,1133,1178,1237,1333,1478,
			1337,1234,1167,1113,1081,1069,1062,1056,1054,1056,1069,1085,1120,1161,1218,1321,1450,
			1305,1215,1147,1099,1069,1060,1053,1045,1042,1045,1058,1075,1105,1148,1206,1303,1422,
			1287,1200,1135,1091,1064,1051,1045,1038,1033,1038,1049,1072,1094,1136,1199,1289,1407,
			1276,1195,1126,1085,1058,1049,1040,1033,1028,1032,1046,1063,1084,1131,1193,1277,1390,
			1270,1190,1125,1081,1060,1045,1035,1030,1027,1029,1040,1056,1079,1128,1187,1267,1379,
			1269,1188,1128,1080,1058,1042,1035,1028,1024,1027,1036,1054,1078,1122,1184,1263,1377,
			1277,1193,1133,1086,1061,1045,1037,1032,1025,1028,1036,1053,1077,1124,1184,1263,1381,
			1281,1199,1137,1086,1064,1053,1038,1034,1028,1030,1038,1053,1075,1124,1190,1272,1396,
			1298,1210,1144,1103,1071,1056,1046,1039,1034,1037,1043,1057,1081,1136,1207,1294,1410,
			1317,1223,1158,1113,1077,1058,1060,1044,1038,1039,1048,1058,1091,1164,1224,1318,1440,
			1345,1240,1175,1122,1089,1068,1065,1049,1045,1044,1052,1069,1127,1188,1247,1343,1467,
			1376,1261,1190,1138,1105,1084,1069,1059,1054,1051,1058,1094,1159,1211,1277,1381,1506,
			1418,1289,1208,1158,1122,1099,1084,1064,1057,1057,1077,1120,1183,1229,1308,1419,1549,
		},          
	    
		//Gbgain:
		{
			1395,1279,1191,1140,1110,1091,1084,1070,1063,1064,1071,1084,1103,1137,1205,1306,1431,
			1354,1235,1162,1120,1090,1078,1069,1057,1048,1050,1052,1068,1086,1111,1166,1254,1387,
			1316,1208,1144,1107,1077,1063,1062,1054,1047,1043,1045,1054,1074,1102,1144,1222,1345,
			1290,1184,1132,1088,1067,1059,1056,1051,1044,1040,1041,1044,1064,1088,1120,1191,1317,
			1250,1166,1117,1078,1061,1055,1048,1045,1040,1033,1036,1041,1054,1077,1108,1191,1296,
			1231,1159,1107,1071,1055,1048,1044,1040,1033,1030,1035,1038,1045,1070,1103,1169,1272,
			1222,1145,1098,1065,1054,1049,1042,1037,1028,1026,1029,1037,1038,1062,1096,1165,1255,
			1210,1149,1095,1065,1052,1047,1038,1034,1030,1025,1027,1030,1039,1058,1095,1159,1243,
			1210,1143,1099,1069,1059,1046,1041,1034,1029,1024,1027,1028,1033,1057,1094,1151,1236,
			1212,1143,1101,1070,1055,1045,1041,1037,1031,1027,1025,1030,1030,1057,1095,1147,1230,
			1221,1153,1109,1079,1062,1051,1046,1041,1032,1027,1028,1030,1036,1062,1099,1147,1239,
			1232,1163,1116,1078,1062,1058,1051,1044,1035,1032,1028,1031,1034,1061,1098,1154,1255,
			1246,1169,1124,1089,1065,1061,1059,1048,1040,1036,1034,1033,1040,1070,1108,1169,1277,
			1265,1182,1136,1103,1077,1063,1065,1052,1045,1040,1037,1035,1046,1081,1117,1189,1290,
			1294,1199,1150,1113,1085,1073,1070,1058,1046,1043,1037,1036,1068,1099,1128,1209,1315,
			1336,1224,1165,1124,1102,1091,1074,1063,1055,1044,1039,1053,1085,1110,1154,1246,1358,
			1368,1255,1179,1133,1115,1097,1083,1062,1062,1051,1053,1068,1098,1113,1171,1273,1394,
		},      
	           
		//Bgain: 
		{
			1449,1324,1222,1164,1131,1104,1096,1081,1082,1086,1104,1130,1162,1215,1291,1413,1568,
			1401,1289,1200,1148,1112,1090,1079,1072,1070,1075,1086,1110,1145,1188,1260,1368,1514,
			1367,1257,1176,1131,1094,1073,1064,1061,1058,1062,1071,1090,1123,1168,1231,1327,1458,
			1332,1227,1159,1108,1077,1059,1055,1051,1049,1052,1059,1076,1110,1150,1202,1297,1429,
			1301,1207,1143,1096,1064,1053,1047,1045,1042,1044,1053,1066,1096,1137,1191,1287,1405,
			1276,1194,1131,1083,1055,1046,1042,1034,1032,1036,1046,1061,1087,1125,1182,1273,1390,
			1259,1179,1119,1075,1051,1044,1038,1028,1027,1031,1041,1059,1078,1120,1175,1260,1371,
			1255,1181,1114,1073,1052,1042,1033,1028,1024,1029,1037,1053,1073,1116,1174,1252,1358,
			1246,1175,1118,1074,1052,1041,1031,1026,1024,1027,1036,1050,1070,1113,1170,1248,1352,
			1249,1180,1120,1075,1053,1038,1032,1028,1024,1028,1032,1050,1070,1113,1168,1244,1347,
			1258,1182,1123,1080,1059,1044,1036,1032,1027,1030,1036,1050,1070,1116,1172,1252,1353,
			1269,1189,1130,1084,1060,1053,1042,1035,1032,1033,1039,1051,1073,1118,1178,1261,1371,
			1283,1200,1137,1097,1070,1057,1047,1040,1038,1040,1044,1056,1079,1129,1195,1279,1390,
			1303,1217,1152,1109,1077,1061,1061,1047,1043,1044,1050,1059,1091,1157,1210,1301,1417,
			1330,1232,1168,1119,1090,1068,1066,1055,1050,1048,1054,1067,1126,1176,1233,1329,1440,
			1362,1257,1185,1137,1106,1087,1074,1064,1059,1056,1061,1094,1154,1201,1264,1364,1491,
			1406,1286,1209,1153,1119,1103,1088,1068,1062,1064,1082,1119,1171,1219,1287,1404,1527,
		},                                                                                                                          
	},

	{
		/*AWB RGain*/
        538,
        /*AWB BGain*/
        384,
		//Rgain:
		{
			1466,1340,1249,1191,1151,1123,1107,1095,1095,1095,1102,1117,1134,1171,1238,1352,1508,
			1420,1306,1222,1171,1135,1108,1091,1085,1079,1078,1084,1098,1116,1145,1206,1307,1450,
			1375,1273,1202,1152,1116,1089,1076,1072,1070,1069,1069,1077,1096,1127,1177,1265,1395,
			1343,1243,1182,1133,1098,1076,1068,1064,1061,1057,1058,1064,1084,1111,1152,1235,1360,
			1316,1223,1165,1119,1083,1069,1059,1055,1051,1045,1049,1055,1071,1098,1136,1223,1337,
			1287,1209,1148,1104,1073,1060,1054,1045,1041,1039,1044,1049,1060,1087,1128,1205,1315,
			1271,1195,1136,1094,1071,1056,1047,1039,1033,1035,1035,1045,1054,1082,1122,1194,1302,
			1262,1190,1130,1089,1065,1051,1042,1035,1029,1029,1035,1041,1048,1076,1117,1187,1288,
			1253,1185,1129,1087,1063,1047,1037,1029,1026,1029,1030,1035,1045,1072,1113,1178,1279,
			1256,1183,1128,1086,1061,1044,1035,1030,1026,1027,1028,1034,1043,1071,1109,1173,1272,
			1258,1183,1131,1086,1061,1045,1036,1030,1027,1024,1027,1032,1040,1070,1108,1174,1274,
			1263,1188,1132,1089,1061,1048,1034,1028,1027,1026,1024,1032,1040,1068,1109,1179,1285,
			1277,1197,1137,1099,1066,1051,1038,1031,1031,1028,1030,1034,1041,1077,1121,1193,1301,
			1289,1207,1147,1106,1071,1054,1047,1038,1033,1032,1031,1033,1049,1095,1133,1210,1314,
			1315,1223,1162,1116,1082,1059,1053,1039,1035,1034,1034,1038,1078,1114,1152,1234,1345,
			1350,1244,1175,1128,1094,1071,1058,1048,1042,1037,1040,1062,1104,1133,1178,1268,1381,
			1387,1274,1192,1144,1110,1084,1066,1052,1047,1047,1055,1086,1120,1147,1200,1303,1417,
		},        
               
		//Grgain:
		{
			1464,1339,1242,1187,1150,1126,1108,1101,1099,1102,1115,1134,1159,1196,1266,1386,1544,
			1424,1301,1220,1170,1134,1104,1092,1087,1082,1084,1093,1113,1133,1164,1231,1335,1489,
			1377,1268,1199,1150,1113,1088,1079,1073,1069,1072,1077,1089,1113,1144,1199,1294,1436,
			1345,1238,1178,1132,1096,1077,1069,1065,1062,1063,1066,1074,1096,1127,1173,1264,1399,
			1314,1220,1163,1116,1082,1068,1057,1056,1051,1052,1057,1066,1087,1115,1158,1250,1375,
			1291,1207,1149,1101,1076,1060,1052,1045,1041,1040,1050,1062,1076,1106,1151,1236,1358,
			1271,1193,1136,1092,1068,1057,1047,1040,1034,1036,1045,1057,1069,1099,1147,1222,1332,
			1262,1184,1130,1089,1064,1050,1039,1037,1030,1030,1041,1051,1059,1093,1138,1212,1327,
			1258,1185,1127,1086,1060,1048,1039,1031,1029,1030,1038,1045,1057,1091,1132,1204,1308,
			1261,1182,1128,1083,1062,1043,1037,1029,1025,1027,1031,1041,1058,1086,1130,1198,1306,
			1265,1185,1129,1085,1063,1045,1036,1032,1024,1026,1033,1044,1055,1085,1130,1197,1305,
			1269,1188,1130,1086,1061,1048,1036,1031,1027,1027,1033,1041,1050,1083,1134,1204,1322,
			1281,1194,1137,1098,1064,1051,1040,1035,1030,1032,1035,1042,1054,1091,1148,1223,1326,
			1296,1205,1146,1105,1069,1053,1047,1037,1032,1035,1038,1043,1059,1117,1160,1241,1353,
			1321,1223,1160,1114,1081,1059,1054,1042,1036,1035,1040,1048,1093,1139,1180,1263,1381,
			1354,1241,1174,1126,1092,1068,1054,1050,1043,1043,1046,1074,1124,1161,1210,1298,1423,
			1395,1269,1192,1144,1108,1086,1069,1050,1048,1049,1064,1099,1146,1173,1236,1342,1468,
		},          
        
		//Gbgain:
		{
			1386,1269,1187,1139,1107,1083,1073,1061,1053,1052,1060,1066,1078,1109,1169,1267,1409,
			1345,1238,1166,1123,1091,1069,1061,1052,1046,1045,1043,1052,1063,1081,1140,1226,1358,
			1309,1212,1153,1112,1077,1062,1052,1043,1044,1039,1037,1040,1050,1073,1116,1191,1315,
			1283,1191,1140,1096,1071,1056,1053,1043,1038,1036,1031,1031,1045,1063,1097,1168,1297,
			1257,1174,1126,1087,1061,1053,1045,1040,1038,1030,1027,1031,1038,1057,1084,1164,1275,
			1236,1170,1116,1080,1060,1051,1042,1033,1030,1025,1027,1028,1031,1051,1081,1149,1253,
			1225,1157,1111,1078,1059,1049,1038,1034,1030,1024,1026,1027,1030,1048,1074,1144,1237,
			1223,1159,1108,1075,1059,1049,1036,1035,1027,1025,1025,1027,1028,1047,1075,1138,1225,
			1220,1155,1107,1076,1060,1046,1038,1034,1029,1026,1027,1026,1026,1045,1075,1132,1225,
			1222,1156,1113,1079,1060,1044,1042,1033,1029,1027,1025,1028,1026,1041,1076,1128,1213,
			1231,1161,1118,1083,1062,1049,1042,1037,1032,1030,1030,1027,1025,1043,1076,1129,1218,
			1237,1169,1125,1085,1064,1057,1046,1039,1037,1031,1027,1028,1027,1046,1077,1134,1236,
			1251,1176,1130,1096,1068,1058,1052,1041,1040,1036,1033,1029,1028,1053,1083,1146,1249,
			1271,1188,1140,1105,1076,1062,1059,1049,1041,1040,1035,1028,1035,1065,1090,1163,1268,
			1294,1206,1154,1115,1085,1070,1064,1052,1047,1041,1037,1034,1057,1081,1105,1183,1289,
			1328,1230,1167,1128,1098,1083,1070,1060,1055,1048,1043,1054,1078,1094,1132,1218,1336,
			1369,1258,1182,1143,1117,1101,1082,1065,1056,1054,1059,1071,1092,1111,1155,1249,1379,
		},      
               
		//Bgain: 
		{
			1426,1308,1215,1157,1122,1097,1083,1070,1068,1070,1083,1101,1124,1161,1229,1343,1500,
			1387,1273,1191,1143,1108,1083,1067,1061,1059,1061,1068,1084,1106,1138,1200,1302,1448,
			1348,1247,1177,1127,1092,1069,1057,1052,1053,1051,1055,1066,1090,1122,1174,1268,1398,
			1314,1222,1159,1114,1079,1059,1054,1048,1045,1043,1047,1054,1079,1106,1154,1237,1369,
			1293,1204,1147,1102,1066,1053,1044,1042,1036,1037,1042,1050,1068,1098,1140,1230,1344,
			1273,1193,1135,1091,1063,1046,1042,1034,1030,1031,1038,1044,1062,1090,1136,1217,1323,
			1257,1181,1123,1082,1059,1047,1038,1028,1026,1026,1032,1042,1055,1084,1131,1205,1316,
			1253,1179,1120,1080,1058,1046,1034,1027,1024,1027,1034,1039,1050,1083,1128,1201,1310,
			1250,1176,1121,1080,1057,1041,1029,1026,1026,1027,1030,1039,1050,1082,1125,1194,1300,
			1246,1176,1124,1080,1055,1041,1032,1027,1024,1026,1028,1038,1051,1079,1121,1193,1292,
			1257,1180,1127,1084,1060,1045,1036,1029,1025,1028,1032,1038,1051,1082,1123,1194,1297,
			1265,1188,1133,1087,1060,1049,1036,1032,1031,1031,1034,1040,1053,1082,1130,1203,1313,
			1277,1196,1139,1098,1067,1054,1044,1037,1035,1037,1039,1045,1057,1093,1145,1220,1326,
			1291,1207,1149,1107,1072,1059,1053,1043,1040,1041,1044,1046,1067,1115,1158,1238,1350,
			1321,1226,1167,1118,1087,1065,1059,1049,1046,1045,1049,1055,1097,1139,1181,1265,1374,
			1353,1249,1180,1134,1101,1080,1065,1058,1054,1054,1056,1082,1125,1161,1211,1301,1417,
			1400,1277,1201,1154,1118,1097,1080,1067,1064,1064,1078,1110,1147,1182,1240,1338,1462,
		},                                                                                                                          
	}			
};

/***BAYER NR**/
static ISP_CMOS_BAYERNR_S g_stIspBayerNr =
{
	14,     //Calibration Lut Num
	/*************Calibration LUT Table*************/
	{         
	    {100.000000f,    0.0332f },     	
	    {184.000000f,    0.0447f }, 
	    {402.000000f,    0.1013f }, 
	    {812.000000f,    0.1916f }, 
	    {1819.000000f,   0.4491f }, 
	    {3199.000000f,   0.8100f }, 
	    {6825.000000f,   1.7288f }, 
	    {12536.000000f,  3.1812f }, 
	    {25413.000000f,  6.7176f }, 
	    {56773.000000f,  15.8910 }, 
	    {121095.000000f, 32.0750f},
	    {223441.000000f, 31.9170f}, 
	    {380084.000000f, 30.142f }, 
	    {800013.000000f, 25.8800f}
	},
	/*********************************************/
	{ 140, 110, 110, 140},	
	
	{ 50, 50, 45, 45, 50, 50, 52, 55, 56, 57, 58, 64, 64, 64, 64, 64},     //lutJNLMGain
	{
	  {0, 0, 1, 1, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3},   //ChromaStrR
      {0, 0, 0, 0, 1, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3},   //ChromaStrGr
	  {0, 0, 0, 0, 1, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3},   //ChromaStrGb
	  {0, 0, 1, 1, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3}    //ChromaStrB 
	},
	
	{ 1200, 1100, 1000, 1000, 1000, 1000, 900, 900, 850, 800, 750, 750, 750, 750, 750, 750},     //lutCoringWeight	
	 
	{300, 350, 400, 450, 500, 550, 650, 650, 700, 700, 700, 700, 700, 700, 700, 700, \
           800, 800, 800, 850, 850, 850, 900, 900, 900, 950, 950, 950, 1000, 1000, 1000, 1000, 1000}
};

static ISP_CMOS_BAYERNR_S g_stIspBayerNrWDR =
{
	14,     //Calibration Lut Num
	/*************Calibration LUT Table*************/
	{         
	    {100.000000f,    0.0332f },     	
	    {184.000000f,    0.0447f }, 
	    {402.000000f,    0.1013f }, 
	    {812.000000f,    0.1916f }, 
	    {1819.000000f,   0.4491f }, 
	    {3199.000000f,   0.8100f }, 
	    {6825.000000f,   1.7288f }, 
	    {12536.000000f,  3.1812f }, 
	    {25413.000000f,  6.7176f }, 
	    {56773.000000f,  15.8910 }, 
	    {121095.000000f, 32.0750f},
	    {223441.000000f, 31.9170f}, 
	    {380084.000000f, 30.142f }, 
	    {800013.000000f, 25.8800f}
	},
	/*********************************************/
	{ 140, 110, 110, 140},	
	
	//{ 50, 50, 50, 52, 55, 58, 60, 64, 64, 64, 64, 64, 64, 64, 64, 64},     //lutJNLMGain

    { 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100},     //lutJNLMGain
	
	{
	  {0, 0, 1, 1, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3},   //ChromaStrR
      {0, 0, 0, 0, 1, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3},   //ChromaStrGr
	  {0, 0, 0, 0, 1, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3},   //ChromaStrGb
	  {0, 0, 1, 1, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3}    //ChromaStrB 
	},
	
	//{ 800, 800, 750, 700, 650, 600, 550, 500, 500, 500, 500,500, 500, 500, 500, 500},     //lutCoringWeight		

    { 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50,50, 50, 50, 50, 50},     //lutCoringWeight		
		
	{ 400, 450, 500, 550, 600, 650, 650, 700, 700, 750, 750, 800, 800, 850, 850, 900, \
           950, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000}
};

/*
static ISP_CMOS_SPLIT_S g_stIspWdrSplit = 
{
	0,   //enable
	2,   // in width sel
	0,   // mode in
	1,   // mode out
	16,
	114,114,114,114, //offset
	0,//out blc
};
*/

static ISP_CMOS_CA_S g_stIspCA = 
{
    /*enable*/
	1,
	/*YRatioLut*/
    {36 ,81  ,111 ,136 ,158 ,182 ,207 ,228 ,259 ,290 ,317 ,345 ,369 ,396 ,420 ,444 ,
	468 ,492 ,515 ,534 ,556 ,574 ,597 ,614 ,632 ,648 ,666 ,681 ,697 ,709 ,723 ,734 ,
	748 ,758 ,771 ,780 ,788 ,800 ,808 ,815 ,822 ,829 ,837 ,841 ,848 ,854 ,858 ,864 ,
	868 ,871 ,878 ,881 ,885 ,890 ,893 ,897 ,900 ,903 ,906 ,909 ,912 ,915 ,918 ,921 ,
	924 ,926 ,929 ,931 ,934 ,936 ,938 ,941 ,943 ,945 ,947 ,949 ,951 ,952 ,954 ,956 ,
	958 ,961 ,962 ,964 ,966 ,968 ,969 ,970 ,971 ,973 ,974 ,976 ,977 ,979 ,980 ,981 ,
	983 ,984 ,985 ,986 ,988 ,989 ,990 ,991 ,992 ,993 ,995 ,996 ,997 ,998 ,999 ,1000,
	1001,1004,1005,1006,1007,1009,1010,1011,1012,1014,1016,1017,1019,1020,1022,1024},
	/*ISORatio*/
    {1300,1300,1250,1200,1150,1100,1050,1000,1000,1000,900,900,800,800,800,800}
};


static ISP_CMOS_CA_S g_stIspWDRCA = 
{
    /*enable*/
	1,
	/*YRatioLut*/
    {    
     913,  913,  913,  914,  914,  914,  915,  915,

    915,  915,  916,  916,  916,  917,  917,  917,

    917,  918,  918,  918,  918,  919,  919,  919,

    919,  919,  920,  920,  920,  920,  920,  920,

    920,  920,  920,  919,  919,  918,  918,  918,

    917,  916,  916,  915,  915,  914,  914,  913,

    913,  913,  912,  912,  912,  911,  911,  911,

    911,  910,  910,  910,  910,  909,  909,  908,

    908,  908,  907,  906,  906,  905,  904,  904,

    903,  902,  902,  901,  900,  900,  899,  899,

    898,  898,  897,  897,  896,  896,  896,  895,

    895,  895,  894,  894,  894,  894,  893,  893,

    893,  893,  893,  893,  893,  893,  893,  893,

    893,  893,  893,  893,  893,  893,  893,  893,

    893,  893,  893,  892,  892,  892,  891,  891,

    891,  890,  890,  889,  889,  889,  888,  888,
    
    },
	/*ISORatio*/
    {1024, 1024, 1200, 1200, 1150, 1100, 1050, 1000,

     950,  900,  900,  800,  800,  800,  800,  800,}
};

HI_U32 cmos_get_isp_default(ISP_CMOS_DEFAULT_S *pstDef)
{
    if (HI_NULL == pstDef)
    {
        printf("null pointer when get isp default value!\n");
        return -1;
    }

    memset(pstDef, 0, sizeof(ISP_CMOS_DEFAULT_S));
	memcpy(&pstDef->stLsc.stLscUniParaTable, &g_stCmosLscUniTable, sizeof(ISP_LSC_CABLI_UNI_TABLE_S));
	memcpy(&pstDef->stLsc.stLscParaTable[0], &g_stCmosLscTable[0], sizeof(ISP_LSC_CABLI_TABLE_S)*HI_ISP_LSC_LIGHT_NUM);
    //memcpy(&pstDef->stCa,        &g_stIspCA,sizeof(ISP_CMOS_CA_S));
    switch (genSensorMode)
    {
        default:
        case WDR_MODE_NONE:           	  
		    memcpy(&pstDef->stDemosaic, &g_stIspDemosaic, sizeof(ISP_CMOS_DEMOSAIC_S));
            memcpy(&pstDef->stYuvSharpen, &g_stIspYuvSharpen, sizeof(ISP_CMOS_YUV_SHARPEN_S));
            memcpy(&pstDef->stDrc, &g_stIspDRC, sizeof(ISP_CMOS_DRC_S));
            
			memcpy(&pstDef->stGamma, &g_stIspGamma, sizeof(ISP_CMOS_GAMMA_S));		
			memcpy(&pstDef->stBayerNr, &g_stIspBayerNr, sizeof(ISP_CMOS_BAYERNR_S));
		  			
		    memcpy(&pstDef->stGe, &g_stIspGe, sizeof(ISP_CMOS_GE_S));			
		    memcpy(&pstDef->stFcr, &g_stIspFcr, sizeof(ISP_CMOS_FCR_S));
			memcpy(&pstDef->stDpc,       &g_stCmosDpc,       sizeof(ISP_CMOS_DPC_S));
			memcpy(&pstDef->stCa,        &g_stIspCA,sizeof(ISP_CMOS_CA_S));
        break;
		case WDR_MODE_BUILT_IN:
        case WDR_MODE_2To1_LINE:
        case WDR_MODE_2To1_FRAME:
        case WDR_MODE_2To1_FRAME_FULL_RATE:
            memcpy(&pstDef->stDemosaic, &g_stIspDemosaicWDR, sizeof(ISP_CMOS_DEMOSAIC_S));
            memcpy(&pstDef->stYuvSharpen, &g_stIspYuvWdrSharpen, sizeof(ISP_CMOS_YUV_SHARPEN_S)); 
            memcpy(&pstDef->stDrc, &g_stIspDRCWDR, sizeof(ISP_CMOS_DRC_S));
            memcpy(&pstDef->stGamma, &g_stIspGammaFSWDR, sizeof(ISP_CMOS_GAMMA_S));
           
			memcpy(&pstDef->stBayerNr, &g_stIspBayerNrWDR, sizeof(ISP_CMOS_BAYERNR_S));          			
            memcpy(&pstDef->stGe, &g_stIspWdrGe, sizeof(ISP_CMOS_GE_S));					
            memcpy(&pstDef->stFcr, &g_stIspWdrFcr, sizeof(ISP_CMOS_FCR_S));		
			memcpy(&pstDef->stDpc,       &g_stCmosDpc,       sizeof(ISP_CMOS_DPC_S));			
			memcpy(&pstDef->stCa,        &g_stIspWDRCA,sizeof(ISP_CMOS_CA_S));
            pstDef->stWDRAttr.au32ExpRatio[0]   = 0x40;
            pstDef->stWDRAttr.au32ExpRatio[1]   = 0x40;
            pstDef->stWDRAttr.au32ExpRatio[2]   = 0x40;
        break;

    }

    pstDef->stSensorMaxResolution.u32MaxWidth  = 1920;
    pstDef->stSensorMaxResolution.u32MaxHeight = 1080;


    return 0;
}

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

    /* black level of linear mode */
    if (WDR_MODE_NONE == genSensorMode)
    {
        for (i=0; i<4; i++)
        {
            pstBlackLevel->au16BlackLevel[i] = 0xF0;    // 240
        }
    }

    /* black level of DOL mode */
    else
    {
        pstBlackLevel->au16BlackLevel[0] = 0xF0;
        pstBlackLevel->au16BlackLevel[1] = 0xF0;
        pstBlackLevel->au16BlackLevel[2] = 0xF0;
        pstBlackLevel->au16BlackLevel[3] = 0xF0;
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
    else if(WDR_MODE_3To1_LINE == genSensorMode)
    {
        return;
    }
    else
    {
        if (IMX290_SENSOR_1080P_30FPS_LINEAR_MODE == gu8SensorImageMode)
        {
            u32FullLines_5Fps = (IMX290_VMAX_1080P30_LINEAR * 30) / 5;
        }
        else
        {
            return;
        }
    }

 
    u32MaxIntTime_5Fps = 4;

    if (bEnable) /* setup for ISP pixel calibration mode */
    {
        sensor_write_register (GAIN_ADDR,0x00);
        
        sensor_write_register (VMAX_ADDR, u32FullLines_5Fps & 0xFF); 
        sensor_write_register (VMAX_ADDR + 1, (u32FullLines_5Fps & 0xFF00) >> 8); 
        sensor_write_register (VMAX_ADDR + 2, (u32FullLines_5Fps & 0x30000) >> 16);

        sensor_write_register (SHS1_ADDR, u32MaxIntTime_5Fps & 0xFF);
        sensor_write_register (SHS1_ADDR + 1,  (u32MaxIntTime_5Fps & 0xFF00) >> 8); 
        sensor_write_register (SHS1_ADDR + 2, (u32MaxIntTime_5Fps & 0x30000) >> 16); 
          
    }
    else /* setup for ISP 'normal mode' */
    {
        gu32FullLinesStd = (gu32FullLinesStd > 0x1FFFF) ? 0x1FFFF : gu32FullLinesStd;
        gu32FullLines = gu32FullLinesStd;
        sensor_write_register (VMAX_ADDR, gu32FullLines & 0xFF); 
        sensor_write_register (VMAX_ADDR + 1, (gu32FullLines & 0xFF00) >> 8); 
        sensor_write_register (VMAX_ADDR + 2, (gu32FullLines & 0x30000) >> 16);
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
            gu8SensorImageMode = IMX290_SENSOR_1080P_30FPS_LINEAR_MODE;
            gu32FullLinesStd = IMX290_VMAX_1080P30_LINEAR;
            gu8HCGReg = 0x2;                                                                   
            printf("linear mode\n");                                             
        break;                                                                   
                                                                                 
        case WDR_MODE_2To1_LINE:                                                 
            genSensorMode = WDR_MODE_2To1_LINE;
            gu8SensorImageMode = IMX290_SENSOR_1080P_30FPS_WDR_MODE;
            gu32FullLinesStd = IMX290_VMAX_1080P60TO30_WDR * 2;                  
            gu32FullLines = IMX290_VMAX_1080P60TO30_WDR * 2;                     
            gu32BRL = 1109;
            gu8HCGReg = 0x1;
            printf("2to1 line WDR 1080p mode(60fps->30fps)\n");
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

HI_U32 cmos_get_sns_regs_info(ISP_SNS_REGS_INFO_S *pstSnsRegsInfo)
{
    HI_S32 i;

    if (HI_FALSE == bInit)
    {
        g_stSnsRegsInfo.enSnsType = ISP_SNS_I2C_TYPE;
        g_stSnsRegsInfo.u8Cfg2ValidDelayMax = 2;
        g_stSnsRegsInfo.u32RegNum = 8;

        if (WDR_MODE_2To1_LINE == genSensorMode)                                                                
        {                                                                                                       
           g_stSnsRegsInfo.u32RegNum += 8; 
           g_stSnsRegsInfo.u8Cfg2ValidDelayMax = 2;                                                            
        } 

        for (i=0; i<g_stSnsRegsInfo.u32RegNum; i++)
        {
            g_stSnsRegsInfo.astI2cData[i].bUpdate = HI_TRUE;
            g_stSnsRegsInfo.astI2cData[i].u8DevAddr = sensor_i2c_addr;
            g_stSnsRegsInfo.astI2cData[i].u32AddrByteNum = sensor_addr_byte;
            g_stSnsRegsInfo.astI2cData[i].u32DataByteNum = sensor_data_byte;
        }

        //Linear Mode Regs
        g_stSnsRegsInfo.astI2cData[0].u8DelayFrmNum = 0;
        g_stSnsRegsInfo.astI2cData[0].u32RegAddr = SHS1_ADDR;       
        g_stSnsRegsInfo.astI2cData[1].u8DelayFrmNum = 0;
        g_stSnsRegsInfo.astI2cData[1].u32RegAddr = SHS1_ADDR + 1;        
        g_stSnsRegsInfo.astI2cData[2].u8DelayFrmNum = 0;
        g_stSnsRegsInfo.astI2cData[2].u32RegAddr = SHS1_ADDR + 2;   
     
        g_stSnsRegsInfo.astI2cData[3].u8DelayFrmNum = 0;       //make shutter and gain effective at the same time
        g_stSnsRegsInfo.astI2cData[3].u32RegAddr = GAIN_ADDR;  //gain     
        g_stSnsRegsInfo.astI2cData[4].u8DelayFrmNum = 1;       
        g_stSnsRegsInfo.astI2cData[4].u32RegAddr = HCG_ADDR;    
        
        g_stSnsRegsInfo.astI2cData[5].u8DelayFrmNum = 0;
        g_stSnsRegsInfo.astI2cData[5].u32RegAddr = VMAX_ADDR;
        g_stSnsRegsInfo.astI2cData[6].u8DelayFrmNum = 0;
        g_stSnsRegsInfo.astI2cData[6].u32RegAddr = VMAX_ADDR + 1;
        g_stSnsRegsInfo.astI2cData[7].u8DelayFrmNum = 0;
        g_stSnsRegsInfo.astI2cData[7].u32RegAddr = VMAX_ADDR + 2;

        //DOL 2t1 Mode Regs
        if (WDR_MODE_2To1_LINE == genSensorMode)                                                               
        {
            g_stSnsRegsInfo.astI2cData[0].u8DelayFrmNum = 0;
            g_stSnsRegsInfo.astI2cData[1].u8DelayFrmNum = 0;
            g_stSnsRegsInfo.astI2cData[2].u8DelayFrmNum = 0;
               
            g_stSnsRegsInfo.astI2cData[5].u8DelayFrmNum = 0;                                                   
            g_stSnsRegsInfo.astI2cData[5].u32RegAddr = SHS2_ADDR;                                              
            g_stSnsRegsInfo.astI2cData[6].u8DelayFrmNum = 0;                                                   
            g_stSnsRegsInfo.astI2cData[6].u32RegAddr = SHS2_ADDR + 1;                                          
            g_stSnsRegsInfo.astI2cData[7].u8DelayFrmNum = 0;                                                   
            g_stSnsRegsInfo.astI2cData[7].u32RegAddr = SHS2_ADDR + 2; 
                                                                          
            g_stSnsRegsInfo.astI2cData[8].u8DelayFrmNum = 1;                                                   
            g_stSnsRegsInfo.astI2cData[8].u32RegAddr = VMAX_ADDR;                                              
            g_stSnsRegsInfo.astI2cData[9].u8DelayFrmNum = 1;                                                   
            g_stSnsRegsInfo.astI2cData[9].u32RegAddr = VMAX_ADDR + 1;                                          
            g_stSnsRegsInfo.astI2cData[10].u8DelayFrmNum = 1;                                                  
            g_stSnsRegsInfo.astI2cData[10].u32RegAddr = VMAX_ADDR + 2; 
                                                                                          
            g_stSnsRegsInfo.astI2cData[11].u8DelayFrmNum = 1;                                                  
            g_stSnsRegsInfo.astI2cData[11].u32RegAddr = RHS1_ADDR;                                             
            g_stSnsRegsInfo.astI2cData[12].u8DelayFrmNum = 1;                                                  
            g_stSnsRegsInfo.astI2cData[12].u32RegAddr = RHS1_ADDR + 1;                                         
            g_stSnsRegsInfo.astI2cData[13].u8DelayFrmNum = 1;                                                  
            g_stSnsRegsInfo.astI2cData[13].u32RegAddr = RHS1_ADDR + 2; 

            g_stSnsRegsInfo.astI2cData[14].u8DelayFrmNum = 1;                                                  
            g_stSnsRegsInfo.astI2cData[14].u32RegAddr = Y_OUT_SIZE_ADDR;                                             
            g_stSnsRegsInfo.astI2cData[15].u8DelayFrmNum = 1;                                                  
            g_stSnsRegsInfo.astI2cData[15].u32RegAddr = Y_OUT_SIZE_ADDR + 1; 

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

    gu32PreFullLines = gu32FullLines;

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

    if ((pstSensorImageMode->u16Width <= 1920) && (pstSensorImageMode->u16Height <= 1080)) 
    {
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

    if ((HI_TRUE == bSensorInit) && (u8SensorImageMode == gu8SensorImageMode))     
    {                                                                              
        /* Don't need to switch SensorImageMode */                                 
        return -1;                                                                 
    }                                                                              
    return 0;
}

int  sensor_set_inifile_path(const char *pcPath)
{
    return 0;
}

HI_VOID sensor_global_init()
{     
    gu8SensorImageMode = IMX290_SENSOR_1080P_30FPS_LINEAR_MODE;
    genSensorMode = WDR_MODE_NONE;
    gu32FullLinesStd = IMX290_VMAX_1080P30_LINEAR;
    gu32FullLines = IMX290_VMAX_1080P30_LINEAR; 
    bInit = HI_FALSE;
    bSensorInit = HI_FALSE; 

    memset(&g_stSnsRegsInfo, 0, sizeof(ISP_SNS_REGS_INFO_S));
    memset(&g_stPreSnsRegsInfo, 0, sizeof(ISP_SNS_REGS_INFO_S));
    return;

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

    s32Ret = HI_MPI_ISP_SensorRegCallBack(IspDev, IMX290_ID, &stIspRegister);
    if (s32Ret)
    {
        printf("sensor register callback function failed!\n");
        return s32Ret;
    }
    
    stLib.s32Id = 0;
    strncpy(stLib.acLibName, HI_AE_LIB_NAME, sizeof(HI_AE_LIB_NAME));
    cmos_init_ae_exp_function(&stAeRegister.stSnsExp);

    s32Ret = HI_MPI_AE_SensorRegCallBack(IspDev, &stLib, IMX290_ID, &stAeRegister);
    if (s32Ret)
    {
        printf("sensor register callback function to ae lib failed!\n");
        return s32Ret;
    }

    stLib.s32Id = 0;
    strncpy(stLib.acLibName, HI_AWB_LIB_NAME, sizeof(HI_AWB_LIB_NAME));
    cmos_init_awb_exp_function(&stAwbRegister.stSnsExp);

    s32Ret = HI_MPI_AWB_SensorRegCallBack(IspDev, &stLib, IMX290_ID, &stAwbRegister);
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

    s32Ret = HI_MPI_ISP_SensorUnRegCallBack(IspDev, IMX290_ID);
    if (s32Ret)
    {
        printf("sensor unregister callback function failed!\n");
        return s32Ret;
    }
    
    stLib.s32Id = 0;
    strncpy(stLib.acLibName, HI_AE_LIB_NAME, sizeof(HI_AE_LIB_NAME));

    s32Ret = HI_MPI_AE_SensorUnRegCallBack(IspDev, &stLib, IMX290_ID);
    if (s32Ret)
    {
        printf("sensor unregister callback function to ae lib failed!\n");
        return s32Ret;
    }

    stLib.s32Id = 0;
    strncpy(stLib.acLibName, HI_AWB_LIB_NAME, sizeof(HI_AWB_LIB_NAME));

    s32Ret = HI_MPI_AWB_SensorUnRegCallBack(IspDev, &stLib, IMX290_ID);
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
#endif /* End of #ifdef __cplusplus */

#endif
#endif /* __IMX290_CMOS_H_ */
