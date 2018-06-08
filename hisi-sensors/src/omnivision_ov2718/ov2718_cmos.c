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


/****************************************************************************
 * local variables                                                            *
 ****************************************************************************/

extern const unsigned int sensor_i2c_addr;
extern unsigned int sensor_addr_byte;
extern unsigned int sensor_data_byte;

#define FULL_LINES_MAX  (0xFFFF)

#define EXPOSURE_ADDR_HHH     (0x0220)
#define EXPOSURE_ADDR_H       (0x30B6)
#define EXPOSURE_ADDR_L       (0x30B7)
#define SHORT_EXPOSURE_ADDR_H (0x312A)
#define SHORT_EXPOSURE_ADDR_L (0x312B)
#define AGC_ADDR              (0x30BB)
#define DGC_ADDR_H            (0x315A)
#define DGC_ADDR_L            (0x315B)
#define LCG_ADDR_H            (0x315C)
#define LCG_ADDR_L            (0x315D)
#define AGC_ADDR_H            (0x0204)
#define AGC_ADDR_L            (0x0205)
#define VMAX_ADDR_H    		  (0x30B2)
#define VMAX_ADDR_L           (0x30B3)

#define INCREASE_LINES (2) /* make real fps less than stand fps because NVR require*/
#define VMAX_1080P30_LINEAR  (1136+INCREASE_LINES)
#define VMAX_1080P60_LINEAR  (1125+INCREASE_LINES)
#define VMAX_1080P60TO30_WDR (1136+INCREASE_LINES)
#define VMAX_1080P50TO25_WDR (1500+INCREASE_LINES)
#define VMAX_1080P120TO30_WDR (1125+INCREASE_LINES)
#define VMAX_720P60TO30_WDR   (750+INCREASE_LINES)
#define VMAX_720P60TO20_WDR   (750+INCREASE_LINES)
#define VMAX_720P60TO15_WDR   (750+INCREASE_LINES)
#define VMAX_720P120_LINEAR  (750+INCREASE_LINES)
#define VMAX_VGA240_LINEAR   (562+INCREASE_LINES)

#define SENSOR_1080P_60FPS_MODE (1)
#define SENSOR_1080P_30FPS_MODE (2)
#define SENSOR_720P_120FPS_MODE (3)
#define SENSOR_VGA_240FPS_MODE  (4)
#define SENSOR_720P_30FPS_MODE (5)  /* for FPGA */

HI_U8 gu8SensorImageMode = SENSOR_1080P_30FPS_MODE;
WDR_MODE_E genSensorMode = WDR_MODE_NONE;

static HI_U32 gu32FullLinesStd = VMAX_1080P30_LINEAR;
static HI_U32 gu32FullLines = VMAX_1080P30_LINEAR;
static HI_U32 gu32PreFullLines = VMAX_1080P30_LINEAR;
static HI_BOOL bInit = HI_FALSE;
HI_BOOL bSensorInit = HI_FALSE; 
ISP_SNS_REGS_INFO_S g_stSnsRegsInfo = {0};
ISP_SNS_REGS_INFO_S g_stPreSnsRegsInfo = {0};
static HI_U32 au32WDRIntTime[4] = {0};

static HI_U32 again_table[8]=
{    
    1024, 2048, 4096, 8192, 11264, 22528, 45056, 90112    
};

static HI_S32 cmos_get_ae_default(AE_SENSOR_DEFAULT_S *pstAeSnsDft)
{
    if (HI_NULL == pstAeSnsDft)
    {
        printf("null pointer when get ae default value!\n");
        return -1;
    }

    memset(&pstAeSnsDft->stAERouteAttr, 0, sizeof(ISP_AE_ROUTE_S));
      
    pstAeSnsDft->u32LinesPer500ms = gu32FullLinesStd*25/2;
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
    pstAeSnsDft->u32MaxISPDgainTarget = 6 << pstAeSnsDft->u32ISPDgainShift;

    pstAeSnsDft->enMaxIrisFNO = ISP_IRIS_F_NO_1_0;
    pstAeSnsDft->enMinIrisFNO = ISP_IRIS_F_NO_32_0;

    pstAeSnsDft->bAERouteExValid = HI_FALSE;
    pstAeSnsDft->stAERouteAttr.u32TotalNum = 0;
    pstAeSnsDft->stAERouteAttrEx.u32TotalNum = 0;

    pstAeSnsDft->u32InitExposure = 921600;

    if((pstAeSnsDft->f32Fps == 50) || (pstAeSnsDft->f32Fps == 60))
    {
        pstAeSnsDft->stIntTimeAccu.f32Offset = 0.6082;
    }
    else if((pstAeSnsDft->f32Fps == 25) || (pstAeSnsDft->f32Fps == 30))
    {
        pstAeSnsDft->stIntTimeAccu.f32Offset = 0.8045;
    }
    else
    {}

    switch(genSensorMode)
    {
        default:
        case WDR_MODE_NONE:   /*linear mode*/
            pstAeSnsDft->au8HistThresh[0] = 0xd;
            pstAeSnsDft->au8HistThresh[1] = 0x28;
            pstAeSnsDft->au8HistThresh[2] = 0x60;
            pstAeSnsDft->au8HistThresh[3] = 0x80;            
            pstAeSnsDft->u8AeCompensation = 0x30;            
		    pstAeSnsDft->u32MaxIntTime = gu32FullLinesStd - 4;
		    pstAeSnsDft->u32MinIntTime = 2;
		    pstAeSnsDft->u32MaxIntTimeTarget = 65535;
		    pstAeSnsDft->u32MinIntTimeTarget = 2;
		    pstAeSnsDft->u32MaxAgain = 90112; 
		    pstAeSnsDft->u32MinAgain = 2048;
		    pstAeSnsDft->u32MaxAgainTarget = pstAeSnsDft->u32MaxAgain;
		    pstAeSnsDft->u32MinAgainTarget = pstAeSnsDft->u32MinAgain;
		    pstAeSnsDft->u32MaxDgain = 1536;  
    		pstAeSnsDft->u32MinDgain = 384;
		    pstAeSnsDft->u32MaxDgainTarget = pstAeSnsDft->u32MaxDgain;
		    pstAeSnsDft->u32MinDgainTarget = pstAeSnsDft->u32MinDgain;	
			pstAeSnsDft->u32LinesPer500ms = gu32FullLinesStd*25/2;
	        break;
		case WDR_MODE_BUILT_IN:
			pstAeSnsDft->au8HistThresh[0] = 0xd;
            pstAeSnsDft->au8HistThresh[1] = 0x28;
            pstAeSnsDft->au8HistThresh[2] = 0x60;
            pstAeSnsDft->au8HistThresh[3] = 0x80;            
            pstAeSnsDft->u8AeCompensation = 0x38;            
		    pstAeSnsDft->u32MaxIntTime = gu32FullLinesStd - 4;
		    pstAeSnsDft->u32MinIntTime = 2;
		    pstAeSnsDft->u32MaxIntTimeTarget = 65535;
		    pstAeSnsDft->u32MinIntTimeTarget = 2;
		    pstAeSnsDft->u32MaxAgain = 90112; 
		    pstAeSnsDft->u32MinAgain = 2048;
		    pstAeSnsDft->u32MaxAgainTarget = pstAeSnsDft->u32MaxAgain;
		    pstAeSnsDft->u32MinAgainTarget = pstAeSnsDft->u32MinAgain;
		    pstAeSnsDft->u32MaxDgain = 16383;  
    		pstAeSnsDft->u32MinDgain = 384;
		    pstAeSnsDft->u32MaxDgainTarget = pstAeSnsDft->u32MaxDgain;
		    pstAeSnsDft->u32MinDgainTarget = pstAeSnsDft->u32MinDgain;	
			pstAeSnsDft->u32LinesPer500ms = gu32FullLinesStd*25/2;
			pstAeSnsDft->u16ManRatioEnable = HI_TRUE;
			pstAeSnsDft->u32Ratio = 16*64;
			break;

        case WDR_MODE_2To1_LINE:
            pstAeSnsDft->au8HistThresh[0] = 0xC;
            pstAeSnsDft->au8HistThresh[1] = 0x18;
            pstAeSnsDft->au8HistThresh[2] = 0x60;
            pstAeSnsDft->au8HistThresh[3] = 0x80;          
            pstAeSnsDft->u8AeCompensation = 64;            
            pstAeSnsDft->u32MaxIntTime = gu32FullLinesStd - 2;  
            pstAeSnsDft->u32MinIntTime = 2;
            pstAeSnsDft->u32MaxIntTimeTarget = 65535;
            pstAeSnsDft->u32MinIntTimeTarget = 140;           
            pstAeSnsDft->u32MaxAgain = 8192;
            pstAeSnsDft->u32MinAgain = 2048;
            pstAeSnsDft->u32MaxAgainTarget = 8192;
            //pstAeSnsDft->u32MinAgainTarget = pstAeSnsDft->u32MinAgain;  
            pstAeSnsDft->u32MinAgainTarget = 2048;
            pstAeSnsDft->u32MaxDgain = 16382;
            pstAeSnsDft->u32MinDgain = 384;
            pstAeSnsDft->u32MaxDgainTarget = 1024;

			pstAeSnsDft->u32LFMaxShortTime = 300;
			pstAeSnsDft->u32MaxIntTimeStep = 3000;
			pstAeSnsDft->u32LFMinExposure = 880000;
			
            //pstAeSnsDft->u32MinDgainTarget = pstAeSnsDft->u32MinDgain;
            pstAeSnsDft->u32MinDgainTarget = 384;
			pstAeSnsDft->u16ManRatioEnable = HI_TRUE;
			pstAeSnsDft->u32Ratio = 11*64;
            if((pstAeSnsDft->f32Fps == 50) || (pstAeSnsDft->f32Fps == 60))
            {
                pstAeSnsDft->stIntTimeAccu.f32Offset = 0.2155;
            }
            else if((pstAeSnsDft->f32Fps == 25) || (pstAeSnsDft->f32Fps == 30))
            {
                pstAeSnsDft->stIntTimeAccu.f32Offset = 0.2818;
            }
            else
            {}
            //pstAeSnsDft->bAERouteExValid = HI_TRUE;
        break;
    }
    return 0;
}


static AWB_CCM_S g_stAwbCcm =
{  
    4900,
    {
       0x01c6,  0x809b,  0x802b,
       0x8046,  0x01a3,  0x805d,
       0x0015,  0x80de,  0x01c9
    },

    3850,
    {
       0x01ce,  0x80d1,  0x0003,
       0x806b,  0x016d,  0x8002,
       0x0026,  0x812c,  0x0206
    },
       
    2650,
    {     
       0x01ee,  0x80c0,  0x802e, 
       0x8071,  0x0190,  0x801f,
       0x0040,  0x81be,  0x027e
    }  
}; 

static AWB_CCM_S g_stWdrAwbCcm =
{  

    4900,
	{
		0xD9,0x3A,0x8013,
		0x4, 0x119,0x801e,
		0x1f,0x43,0x9d
	},
	3850,
	{
		0xd0,0x36,0x8006,
		0xB,0xF4,0x1,
		0x2B,0x36,0x9f
	},
	2650,
	{
		0xc8,0x42,0x800a,
		0x8008,0x108,0x8003,
		0x3e,0x7,0xBB
	}

};


static AWB_AGC_TABLE_S g_stAwbAgcTable =
{
    /* bvalid */
    1,
	
    /*1,  2,  4,  8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384, 32768*/
    /* saturation */   
    {128,128,124,120,116,110,106,100,90,80,70,64,64,56,56,56}
};

static AWB_AGC_TABLE_S g_stWdrAwbAgcTable =
{
    /* bvalid */
    1,

    /* saturation */ 
    {128,128,124,120,116,110,106,100,90,80,70,64,64,56,56,56}

};

static HI_S32 cmos_get_awb_default(AWB_SENSOR_DEFAULT_S *pstAwbSnsDft)
{
	HI_U8 i;
	
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
	for (i=0; i<4; i++)
    {
        pstAwbSnsDft->au32BlcOffset[i] = 0x0; 
    }       
    switch (genSensorMode)
    {
        default:
        case WDR_MODE_NONE:
			memcpy(&pstAwbSnsDft->stCcm, &g_stAwbCcm, sizeof(AWB_CCM_S));
            memcpy(&pstAwbSnsDft->stAgcTbl, &g_stAwbAgcTable, sizeof(AWB_AGC_TABLE_S));			
        break;
        case WDR_MODE_2To1_LINE:
			memcpy(&pstAwbSnsDft->stCcm, &g_stWdrAwbCcm, sizeof(AWB_CCM_S));
            memcpy(&pstAwbSnsDft->stAgcTbl, &g_stWdrAwbAgcTable, sizeof(AWB_AGC_TABLE_S));		
		break;
		case WDR_MODE_BUILT_IN:
			memcpy(&pstAwbSnsDft->stCcm, &g_stWdrAwbCcm, sizeof(AWB_CCM_S));
            memcpy(&pstAwbSnsDft->stAgcTbl, &g_stWdrAwbAgcTable, sizeof(AWB_AGC_TABLE_S));		
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


/* ISP default parameter and function */
static ISP_CMOS_DEMOSAIC_S g_stIspDemosaic =
{
	1,    //bEnable
    /*au16EdgeSmoothThr*/
	{6,6,6,16,16,16,128,1022,1022,1022,1022,1022,1022,1022,1022,1022},
	/*au16EdgeSmoothSlope*/
	{8,8,8,16,16,32,32,0,0,0,0,0,0,0,0,0},
	/*au16AntiAliasThr*/
	{53,53,53,53,53,53,1022,1022,1022,1022,1022,1022,1022,1022,1022,1022},
	/*au16AntiAliasSlope*/
	{256,256,256,256,256,256,0,0,0,0,0,0,0,0,0,0},
    /*NrCoarseStr*/
    {128, 128, 128, 112, 96, 48, 16, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	/*NoiseSuppressStr*/
	{5, 5, 6, 6, 8, 8, 9, 10, 10,10,10,10,10,10,10,10},
	/*DetailEnhanceStr*/
	{6,6, 6, 6, 6, 7, 8, 9, 9, 9, 9, 9, 9, 9, 9, 9},
    /*SharpenLumaStr*/
    {256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256},
	/*ColorNoiseCtrlThr*/
    {50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 0, 0, 0, 0, 0}
};	

static ISP_CMOS_DEMOSAIC_S g_stIspWdrDemosaic =
{
	1,    //bEnable

    /*au16EdgeSmoothThr*/
	{256,256,256,256,256,256,512,1022,1022,1022,1022,1022,1022,1022,1022,1022},
	/*au16EdgeSmoothSlope*/
	{ 32, 32, 32, 32, 32,16,8,0,0,0,0,0,0,0,0,0},
	/*au16AntiAliasThr*/
	{289,289,289,289,289,289,1022,1022,1022,1022,1022,1022,1022,1022,1022,1022},
	/*au16AntiAliasSlope*/
	{256,256,256,256,256,256,0,0,0,0,0,0,0,0,0,0},
    /*NrCoarseStr*/
    {64, 64, 64, 64, 64, 48, 16, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	/*NoiseSuppressStr*/
	{10, 10, 10, 10, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	/*SharpenStr*/
	{10, 10, 10, 10, 6, 6, 4, 2, 1, 0, 0, 0, 0, 0, 0, 0},
    /*SharpenLumaStr*/
    {350, 350, 350, 350, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256},
	/*ColorNoiseCtrlThr*/
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
};

static ISP_CMOS_DEMOSAIC_S g_stIspBuiltinDemosaic =
{
	1,    //bEnable
    /*au16EdgeSmoothThr*/
	{100,100,100,100,1022,1022,1022,1022,1022,1022,1022,1022,1022,1022,1022,1022},
	/*au16EdgeSmoothSlope*/
	{32,32,32,32,0,0,0,0,0,0,0,0,0,0,0,0},
	/*au16AntiAliasThr*/
	{164,164,218,218,1022,1022,1022,1022,1022,1022,1022,1022,1022,1022,1022,1022},
	/*au16AntiAliasSlope*/
	{256,256,256,256,256,256,0,0,0,0,0,0,0,0,0,0},
    /*NrCoarseStr*/
    {96, 96, 96, 96, 64, 32, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16},
	/*NoiseSuppressStr*/
	{10, 10, 10, 10, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	/*SharpenStr*/
	{6, 6, 7, 7, 7, 6, 4, 2, 1, 0, 0, 0, 0, 0, 0, 0},
    /*SharpenLumaStr*/
    {256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256},
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
static ISP_CMOS_GE_S g_stIspBuiltinGe =
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
static ISP_CMOS_FCR_S g_stIspBuiltinFcr =
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
       
	 /* au16SharpenUd */
	 {25 ,	25 ,	   23,    21,     18,	    16,	    14,   	12,	    10,	   	9,	  	8,	  	6,	  	3,	  	2,	  	1,    	1},
	
	/* au8SharpenD */
     {80 ,	80 ,	   80,   90,    100,	    100,	120,   140,	    140,   140,	  150,	  150,	  170,	  180,	  200,    200},
	 
	/*au8TextureThd*/
	{0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0},

     /* au8SharpenEdge */
     {60 ,   60 ,    60,    70,    70,    80,     80,     80,     80,    80,    100,      120,     150,     180,     180,    200},

     /* au8EdgeThd */
     {80 ,   80 ,    80,    80,     80,    80,     80,    90,    100,   110,   120,    130,    150,    170,    180,   190},	 
	 	
	 /* au8OverShoot */
     {150 ,   150 ,    140,    140,    130,    130,     120,     110,     100,    80,    60,     40,     60,     70,     70,    70},
        
     /* au8UnderShoot */
     {180  ,  180  ,   170,    170,    160,    130,     130,     130,    110,    90,    80,     60,     70,     80,     80,    80},

	 /*au8shootSupSt*/
	 {10,	  10,	   10,	   10,	   10,		10,     10,	10,		10,		10,		10,			0,			0,		0,		0,		0},
	 
	 /* au8DetailCtrl */
     {128  ,  128  ,   128,    128,    128,    128,    128,    128,    128,    128,    128,     128,    128,    128,     128,      128},
	
	
};                                                                                                           

static ISP_CMOS_YUV_SHARPEN_S g_stIspYuvWdrSharpen = 
{
     /* bvalid */
     1,

     /* 100,  200,    400,     800,    1600,    3200,    6400,    12800,    25600,   51200,  102400,  204800,   409600,   819200,   1638400,  3276800 */
     
	  /* au16SharpenUd */
	  {22 ,  22 ,		22,    16,	   15,		 13,	 12,	 12,	 10,	 9, 	 8, 	 6, 	 3, 	 2, 	 1, 	 1},
	 
	 /* au8SharpenD */
	  {80 ,  80 ,		80,   90,	 90,		 90,	 120,	140,	 140,	140,   150,    150,    170,    180,    200,    200},
	  
	 /*au8TextureThd*/
	 {0,	 0, 	 0, 	 0, 	 0, 	 0, 	 0, 	 0, 	 0, 	 0, 	 0, 	 0, 	 0, 	 0, 	 0, 	 0},
	 
	  /* au8SharpenEdge */
	  {60 ,   60 ,	  60,	 70,	70,    80,	   80,	   80,	   80,	  80,	 100,	   120, 	150,	 180,	  180,	  200},
	 
	  /* au8EdgeThd */
	  {80 ,   80 ,	  80,	 80,	 80,	80, 	80,    90,	  100,	 110,	120,	130,	150,	170,	180,   190},  
		 
	  /* au8OverShoot */
	  {150 ,   150 ,	130,	130,	130,	130,	 120,	  110,	   100,    80,	  60,	  40,	  60,	  70,	  70,	 70},
		 
	  /* au8UnderShoot */
	  {180	,  180	,	180,	160,	140,	130,	 130,	  130,	  110,	  90,	 80,	 60,	 70,	 80,	 80,	80},
	 
	  /*au8shootSupSt*/
	  {30,	   30,		30, 	20, 	10, 	 10,	 10, 10,	 10,	 10,	 10,		 0, 		 0, 	 0, 	 0, 	 0},
	  
	  /* au8DetailCtrl */
	  {128	,  128	,	128,	130,	128,	128,	128,	128,	128,	128,	128,	 128,	 128,	 128,	  128,		128},
	 
	 

	
};   
	static ISP_CMOS_YUV_SHARPEN_S g_stIspYuvBuiltinSharpen = 
	{
		/* bvalid */
			1,
	
			/* 100,  200,	 400,	  800,	  1600,    3200,	6400,	 12800,    25600,	51200,	102400,  204800,   409600,	 819200,   1638400,  3276800 */
		
			/* au8SharpenUd */
			{20 ,  20 , 	  20,	20,    20,	   8,	   8,	   4,	   3,	   2,	   1,	   0,	   0,	   0,	   0,	   0},
	   
		   /* au8SharpenD */
			{70,   80,	  90,	90,    80,	  70,	   70,	 70,   60,	  50,	 40,	 40,	 40,	 40,	 40,	40},
		
			/*au8TextureThd*/
		   {0,	   0,	   0,	   0,	   0,	   0,	   0,	   0,	   0,	   0,	   0,	   0,	   0,	   0,	   0,	   0},
	
			/* au8SharpenEdge */
			{50,   60,	  60,	 60,	60,    45,	   45,	   40,	   30,	  30,	 30,	  30,	  30,	  30,	  30,	 30},
	
			/* au8EdgeThd */
			{60,	70,    80,	  80,	 70,	60, 	50,    50,	  50,	50,   55,	 60,	70,    80,	  80,	80},	
		
			/* au8OverShoot */
			{150,	150,	150,	150,	150,	120,	 80,	 60,	 50,	40,    30,	   20,	   10,	   0,	  0,	0},
		   
			/* au8UnderShoot */
			{180,	180,	180,	160,	150,   120, 	110,	 90,	 70,	50,    30,	   25,	   20,	   10,	   10,	  10},
		
			/*au8shootSupSt*/
			{33,	  33,	   33,	   33,	   16,		0,     0,		0,		0,		0,		0,			0,			0,		0,		0,		0},
	
			/* au8DetailCtrl */
			{128  ,  128,	 140,	 140,	 128,	 128,	 128,	 128,	 128,	 128,	 128,	  128,	  128,	  128,	   128, 	 128},

	
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

static ISP_CMOS_DRC_S g_stIspDRCWDR = 
{
    /*bEnable*/
    1,    
    /*u8SpatialVar*/
    6,    
	/*u8RangeVar*/             
	8,
	/*u8Asymmetry8*/
    5,
	/*u8SecondPole; */
    195,
	/*u8Stretch*/
    40,
	/*u8Compress*/ 
    117,
	/*u8PDStrength*/            
    0,
	/*u8LocalMixingBrigtht*/  
    58,
    /*u8LocalMixingDark*/
    30,
    /*ColorCorrectionLut[33];*/
	//{1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024},
   {900,895,890,885,880,875,870,865,860,855,850,845,840,835,830,825,820,815,810,805,800,800,800,800,800,800,800,800,800,800,800,800,800},
   //{800,790,780,770,760,750,740,730,720,710,700,690,680,670,660,650,640,630,620,610,600,590,580,575,570,565,560,555,550,545,540,535,530},
};                                                                                                                                   

static ISP_CMOS_GAMMA_S g_stIspGamma =
{
    /* bvalid */
    1,
    {0   ,87   ,176  ,266  ,354  ,440  ,523  ,600  ,656  ,707  ,755  ,801  ,846  ,892  ,939  ,988  ,1040 ,
	1071 ,1103 ,1136 ,1169 ,1203 ,1237 ,1271 ,1306 ,1341 ,1376 ,1411 ,1446 ,1481 ,1516 ,1550 ,1584 ,
	1635 ,1676 ,1710 ,1740 ,1769 ,1800 ,1835 ,1865 ,1895 ,1924 ,1955 ,1986 ,2018 ,2052 ,2089 ,2128 ,
	2151 ,2176 ,2202 ,2229 ,2256 ,2285 ,2313 ,2342 ,2372 ,2400 ,2429 ,2457 ,2484 ,2510 ,2536 ,2560 ,
	2582 ,2605 ,2626 ,2647 ,2668 ,2688 ,2707 ,2727 ,2745 ,2764 ,2782 ,2800 ,2818 ,2835 ,2852 ,2870 ,
	2887 ,2904 ,2920 ,2936 ,2952 ,2968 ,2983 ,2998 ,3013 ,3028 ,3043 ,3057 ,3071 ,3085 ,3098 ,3112 ,
	3125 ,3138 ,3150 ,3162 ,3174 ,3186 ,3198 ,3209 ,3220 ,3231 ,3242 ,3253 ,3264 ,3276 ,3287 ,3298 ,
	3309 ,3320 ,3332 ,3343 ,3355 ,3366 ,3377 ,3389 ,3400 ,3411 ,3421 ,3432 ,3442 ,3452 ,3462 ,3472 ,
	3481 ,3490 ,3499 ,3508 ,3516 ,3525 ,3533 ,3541 ,3548 ,3556 ,3563 ,3570 ,3577 ,3584 ,3591 ,3598 ,
	3605 ,3611 ,3618 ,3624 ,3631 ,3637 ,3643 ,3649 ,3655 ,3661 ,3667 ,3672 ,3678 ,3684 ,3690 ,3696 ,
	3702 ,3708 ,3714 ,3720 ,3726 ,3732 ,3738 ,3744 ,3750 ,3756 ,3762 ,3768 ,3774 ,3779 ,3785 ,3790 ,
	3795 ,3800 ,3805 ,3810 ,3815 ,3819 ,3824 ,3829 ,3833 ,3838 ,3842 ,3846 ,3851 ,3855 ,3860 ,3864 ,
	3868 ,3873 ,3877 ,3881 ,3886 ,3890 ,3894 ,3898 ,3902 ,3907 ,3911 ,3915 ,3919 ,3923 ,3927 ,3931 ,
	3935 ,3939 ,3943 ,3947 ,3951 ,3955 ,3959 ,3963 ,3966 ,3970 ,3974 ,3978 ,3981 ,3985 ,3989 ,3992 ,
	3995 ,3999 ,4002 ,4005 ,4008 ,4011 ,4014 ,4017 ,4020 ,4023 ,4026 ,4029 ,4032 ,4035 ,4038 ,4041 ,
	4044 ,4048 ,4051 ,4054 ,4058 ,4061 ,4065 ,4068 ,4072 ,4075 ,4079 ,4082 ,4086 ,4089 ,4093 ,4095 
	}
};

static ISP_CMOS_GAMMA_S g_stIspWdrGamma =
{
    /* bvalid */
    1,   
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

};

static ISP_CMOS_GAMMA_S g_stIspBuiltinGamma =
{
    /* bvalid */
    1,
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

};





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
 	{     36,   63,   92,  120,  150,  179,  208,  238,
		
        267,  295,  323,  350,  377,  402,  425,  447,

        468,  487,  505,  521,  537,  551,  564,  577,

        588,  600,  610,  620,  629,  639,  647,  656,

        665,  673,  681,  688,  695,  701,  707,  712,

        717,  721,  725,  729,  732,  735,  738,  741,

        743,  745,  746,  747,  747,  747,  747,  746,

        745,  744,  742,  741,  739,  737,  736,  734,

        733,  732,  731,  730,  729,  728,  727,  725,

        724,  722,  721,  718,  716,  713,  710,  706,

        702,  697,  693,  688,  683,  678,  673,  667,

        661,  655,  647,  639,  631,  621,  610,  599,

        586,  572,  556,  539,  521,  502,  482,  462,

        440,  419,  398,  376,  355,  334,  313,  294,

        275,  256,  237,  218,  199,  181,  162,  144,

        126,  108,   90,   72,   54,   36,   18,    0},
	/*ISORatio*/
    {1024, 1024, 1200, 1200, 1150, 1100, 1050, 1000,

     950,  900,  900,  800,  800,  800,  800,  800,}

};




static ISP_CMOS_DPC_S g_stCmosDpc = 
{

	{220,220,220,230,230,245,245,245,245,245,245,245,245,245,245,245},/*au16Strength[16]*/
	{0,    0,  0,  0,  0, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50},/*au16BlendRatio[16]*/
};	
static ISP_CMOS_DPC_S g_stCmosWdrDpc = 
{
	{245,245,245,246,250,250,250,250,250,250,250,250,250,250,250,250},/*au16Strength[16]*/
	{0,    0, 0, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50},/*au16BlendRatio[16]*/
};	



/***BAYER NR**/
static ISP_CMOS_BAYERNR_S g_stIspBayerNr =
{
	14,     //Calibration Lut Num
	/*************Calibration LUT Table*************/
	{         
	    {305.000000f,    0.054764f },     	
	    {720.000000f,    0.05051f }, 
	    {1442.000000f,   0.081461f }, 
	    {3264.000000f,   0.137406f }, 
	    {6000.000000f,   0.232523f }, 
	    {12300.000000f,  0.456193f }, 
	    {24600.000000f,  0.905304f }, 
	    {49000.000000f,  1.874653f }, 
	    {82000.000000f,  3.288012f }, 
	    {230000.000000f, 10.71757f }, 
	    {431701.000000f, 20.72827f},
	    {833749.000000f, 28.82481f}, 
	    {1808289.000000f,27.12077f }
	},
	/*********************************************/
	{ 140, 110, 110, 140},           //CoarseStr		
	{ 20, 20, 30, 30, 20, 20, 20, 30, 30, 30, 35, 35, 35, 40, 40, 50},    //lutFineStr
	{
	  {1,1,1,1,2,2,2,3,3,3,3,3,3,3,3,3},   //ChromaStrR
      {0,0,0,0,1,1,1,2,2,2,2,2,2,0,0,0},   //ChromaStrGr
	  {0,0,0,0,1,1,1,2,2,2,2,2,2,0,0,0},   //ChromaStrGb
	  {1,1,1,1,2,2,2,3,3,3,3,3,3,3,3,3}    //ChromaStrB 
	},
	{ 600, 600, 600, 600, 650, 750, 750, 700, 700, 600, 600, 600, 600, 600, 600, 600},	   //lutCoringWeight
	{ 600, 600, 600, 600, 650, 650, 650, 650, 700, 700, 700, 700, 700, 700, 700, 700, \
           800, 800, 800, 850, 850, 850, 900, 900, 900, 950, 950, 950, 1000, 1000, 1000, 1000, 1000}
};

static ISP_CMOS_BAYERNR_S g_stIspWdrBayerNr =
{	
	14,     //Calibration Lut Num
	/*************Calibration LUT Table*************/
	{         
	    {305.000000f,    0.054764f },     	
	    {720.000000f,    0.05051f }, 
	    {1442.000000f,   0.081461f }, 
	    {3264.000000f,   0.137406f }, 
	    {6000.000000f,   0.232523f }, 
	    {12300.000000f,  0.456193f }, 
	    {24600.000000f,  0.905304f }, 
	    {49000.000000f,  1.874653f }, 
	    {82000.000000f,  3.288012f }, 
	    {230000.000000f, 10.71757f }, 
	    {431701.000000f, 20.72827f},
	    {833749.000000f, 28.82481f}, 
	    {1808289.000000f,27.12077f }
	},
	/*********************************************/
	{ 140, 110, 110, 140},		 //CoarseStr
	{ 90, 90, 90, 90, 90, 90, 90, 90, 50, 50, 50, 50, 50, 50, 50, 50},    //lutFineStr
	{
	  {3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3},   //ChromaStrR
      {3,3,3,3,3,3,2,2,2,2,2,2,2,2,2,2},   //ChromaStrGr
	  {3,3,3,3,3,3,2,2,2,2,2,2,2,2,2,2},   //ChromaStrGb
	  {3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3}    //ChromaStrB 
	},
	{ 100, 100, 100, 100, 200, 300, 400, 500, 600, 600, 600, 600, 500, 500, 500, 500},     //lutCoringWeight
	{ 50, 80, 100, 120, 150, 200, 250, 300, 350, 400, 450, 500, 600, 650, 700, 700, \
           800, 800, 800, 850, 850, 850, 900, 900, 900, 950, 950, 950, 1000, 1000, 1000, 1000, 1000}
};

/*
static ISP_CMOS_SPLIT_S g_stIspWdrSplit = 
{
	0,   //enable
	2,   // in width sel
	0,   // mode in
	1,   // mode out
	16,
	256,256,256,256, //offset  14bit
	0,//out blc
};

static ISP_CMOS_SPLIT_S g_stIspWdrCombineSplit = 
{
	1,   //enable
	2,   // in width sel
	0,   // mode in
	1,   // mode out
	16,
	114,114,114,114, //offset  14bit
	0,//out blc
};
*/
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

HI_U32 cmos_get_isp_default(ISP_CMOS_DEFAULT_S *pstDef)
{
    if (HI_NULL == pstDef)
    {
        printf("null pointer when get isp default value!\n");
        return -1;
    }

    memset(pstDef, 0, sizeof(ISP_CMOS_DEFAULT_S));

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
            memcpy(&pstDef->stDemosaic, &g_stIspBuiltinDemosaic, sizeof(ISP_CMOS_DEMOSAIC_S));
            memcpy(&pstDef->stYuvSharpen, &g_stIspYuvBuiltinSharpen, sizeof(ISP_CMOS_YUV_SHARPEN_S));          
			memcpy(&pstDef->stDrc, &g_stIspDRCWDR, sizeof(ISP_CMOS_DRC_S)); 
            memcpy(&pstDef->stGamma, &g_stIspBuiltinGamma, sizeof(ISP_CMOS_GAMMA_S));
			memcpy(&pstDef->stBayerNr, &g_stIspWdrBayerNr, sizeof(ISP_CMOS_BAYERNR_S));			
            memcpy(&pstDef->stGe, &g_stIspBuiltinGe, sizeof(ISP_CMOS_GE_S));					
            memcpy(&pstDef->stFcr, &g_stIspBuiltinFcr, sizeof(ISP_CMOS_FCR_S));
			memcpy(&pstDef->stDpc,       &g_stCmosDpc,       sizeof(ISP_CMOS_DPC_S));
			memcpy(&pstDef->stCompander,  &g_stCmosCompander, sizeof(ISP_CMOS_COMPANDER_S));
			memcpy(&pstDef->stCa,        &g_stIspWDRCA,sizeof(ISP_CMOS_CA_S));
		break;
        case WDR_MODE_2To1_LINE:
            memcpy(&pstDef->stDemosaic, &g_stIspWdrDemosaic, sizeof(ISP_CMOS_DEMOSAIC_S));
            memcpy(&pstDef->stYuvSharpen, &g_stIspYuvWdrSharpen, sizeof(ISP_CMOS_YUV_SHARPEN_S));
            memcpy(&pstDef->stDrc, &g_stIspDRCWDR, sizeof(ISP_CMOS_DRC_S));             
            memcpy(&pstDef->stGamma, &g_stIspWdrGamma, sizeof(ISP_CMOS_GAMMA_S));
			memcpy(&pstDef->stBayerNr, &g_stIspWdrBayerNr, sizeof(ISP_CMOS_BAYERNR_S));			
            memcpy(&pstDef->stGe, &g_stIspWdrGe, sizeof(ISP_CMOS_GE_S));					
            memcpy(&pstDef->stFcr, &g_stIspWdrFcr, sizeof(ISP_CMOS_FCR_S));
			memcpy(&pstDef->stDpc,       &g_stCmosWdrDpc,       sizeof(ISP_CMOS_DPC_S));			
			memcpy(&pstDef->stCa,        &g_stIspWDRCA,sizeof(ISP_CMOS_CA_S));
			pstDef->stWDRAttr.au32ExpRatio[0]   = 0x2C0;
            pstDef->stWDRAttr.au32ExpRatio[1]   = 0x40;
            pstDef->stWDRAttr.au32ExpRatio[2]   = 0x40;
        break;
    }
    pstDef->stSensorMaxResolution.u32MaxWidth  = 1920;
    pstDef->stSensorMaxResolution.u32MaxHeight = 1080;
    return 0;
}


/* the function of sensor set fps */
static HI_VOID cmos_fps_set(HI_FLOAT f32Fps, AE_SENSOR_DEFAULT_S *pstAeSnsDft)
{
    if ((WDR_MODE_2To1_LINE == genSensorMode))
    {
        /* In WDR_MODE_2To1_LINE mode, 30fps means 60 frames(30 short and 30 long) combine to 30 frames per sencond */
        if ((f32Fps <= 30) && (f32Fps >= 0.51))
        {
			if (SENSOR_1080P_30FPS_MODE == gu8SensorImageMode)
			{
				gu32FullLinesStd = VMAX_1080P60TO30_WDR * 30 / f32Fps;
				pstAeSnsDft->stIntTimeAccu.f32Offset = 0.2818;        	
			}
			else if(SENSOR_720P_30FPS_MODE == gu8SensorImageMode)
			{
				gu32FullLinesStd = VMAX_720P60TO30_WDR * 30 / f32Fps;
				pstAeSnsDft->stIntTimeAccu.f32Offset = 0.2818;
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
        if ((f32Fps <= 30) && (f32Fps >= 0.51))
        {
			if (SENSOR_1080P_30FPS_MODE == gu8SensorImageMode)
			{
				gu32FullLinesStd = VMAX_1080P60TO30_WDR * 30 / f32Fps;
				pstAeSnsDft->stIntTimeAccu.f32Offset = 0.2818;        	
			}
			else if(SENSOR_720P_30FPS_MODE == gu8SensorImageMode)
			{
				gu32FullLinesStd = VMAX_720P60TO30_WDR * 30 / f32Fps;
				pstAeSnsDft->stIntTimeAccu.f32Offset = 0.2818;
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
        if (SENSOR_1080P_30FPS_MODE == gu8SensorImageMode)
        {
            if ((f32Fps <= 30) && (f32Fps >= 0.51))
            {
                /* In 1080P30fps mode, the VMAX(FullLines) is VMAX_1080P30_LINEAR, 
                    	 and there are (VMAX_1080P30_LINEAR*30) lines in 1 second,
                    	 so in f32Fps mode, VMAX(FullLines) is (VMAX_1080P30_LINEAR*30)/f32Fps */
                gu32FullLinesStd = (VMAX_1080P30_LINEAR * 30) / f32Fps;
                pstAeSnsDft->stIntTimeAccu.f32Offset = 0.8045;
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
    }

    gu32FullLinesStd = (gu32FullLinesStd > FULL_LINES_MAX) ? FULL_LINES_MAX : gu32FullLinesStd;
    if(WDR_MODE_NONE == genSensorMode)
    {
	    g_stSnsRegsInfo.astI2cData[5].u32Data = ((gu32FullLinesStd & 0xFF00) >> 8);
	    g_stSnsRegsInfo.astI2cData[6].u32Data = (gu32FullLinesStd & 0xFF);   
        
    }
    else if ((WDR_MODE_2To1_LINE == genSensorMode) || (WDR_MODE_2To1_FRAME_FULL_RATE == genSensorMode) || (WDR_MODE_2To1_FRAME == genSensorMode))
    {
    	g_stSnsRegsInfo.astI2cData[5].u32Data = ((gu32FullLinesStd & 0xFF00) >> 8);
	    g_stSnsRegsInfo.astI2cData[6].u32Data = (gu32FullLinesStd & 0xFF);   
        g_stSnsRegsInfo.astI2cData[9].u32Data = ((gu32FullLinesStd & 0xFF00) >> 8);
        g_stSnsRegsInfo.astI2cData[10].u32Data = (gu32FullLinesStd & 0xFF);
    }
    else if(WDR_MODE_BUILT_IN == genSensorMode)
    {
    	g_stSnsRegsInfo.astI2cData[5].u32Data = ((gu32FullLinesStd & 0xFF00) >> 8);
	    g_stSnsRegsInfo.astI2cData[6].u32Data = (gu32FullLinesStd & 0xFF);   
       
    }
    else
    {
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
    if(WDR_MODE_NONE == genSensorMode)
    {
    	u32FullLines = (u32FullLines > FULL_LINES_MAX) ? FULL_LINES_MAX : u32FullLines;
	    gu32FullLines = u32FullLines;
	    pstAeSnsDft->u32FullLines = gu32FullLines;
        g_stSnsRegsInfo.astI2cData[5].u32Data = ((u32FullLines & 0xFF00) >> 8);
        g_stSnsRegsInfo.astI2cData[6].u32Data = (u32FullLines & 0xFF);  
    }
    else if ((WDR_MODE_2To1_LINE == genSensorMode)||(WDR_MODE_BUILT_IN == genSensorMode))
    {
    	u32FullLines = (u32FullLines + 20) / 12;
		u32FullLines = (u32FullLines < pstAeSnsDft->u32FullLinesStd) ? pstAeSnsDft->u32FullLinesStd : u32FullLines;
    	u32FullLines = (u32FullLines > FULL_LINES_MAX) ? FULL_LINES_MAX : u32FullLines;
	    gu32FullLines = u32FullLines;
	    pstAeSnsDft->u32FullLines = gu32FullLines;
        g_stSnsRegsInfo.astI2cData[5].u32Data = ((u32FullLines & 0xFF00) >> 8);
        g_stSnsRegsInfo.astI2cData[6].u32Data = (u32FullLines & 0xFF);  
        g_stSnsRegsInfo.astI2cData[9].u32Data = ((u32FullLines & 0xFF00) >> 8);
        g_stSnsRegsInfo.astI2cData[10].u32Data = (u32FullLines & 0xFF);
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
    static HI_BOOL bFirst = HI_TRUE; 

    if ((WDR_MODE_2To1_FRAME_FULL_RATE == genSensorMode) || (WDR_MODE_2To1_FRAME == genSensorMode))
    {
        if (bFirst) /* short exposure */
        {
            g_stSnsRegsInfo.astI2cData[0].u32Data = ((u32IntTime & 0xFF00) >> 8);
            g_stSnsRegsInfo.astI2cData[1].u32Data = (u32IntTime & 0xFF);
            bFirst = HI_FALSE;
        }
        else    /* long exposure */
        {
            bFirst = HI_TRUE;
        }        
    }
    else if (WDR_MODE_2To1_LINE == genSensorMode)
    {
        if (bFirst) /* short exposure */
        {
            au32WDRIntTime[0] = u32IntTime;
            g_stSnsRegsInfo.astI2cData[0].u32Data = ((u32IntTime & 0xFF00) >> 8);
            g_stSnsRegsInfo.astI2cData[1].u32Data = (u32IntTime & 0xFF);
            bFirst = HI_FALSE;
        }
        else    /* long exposure */
        {
            //au32WDRIntTime[1] = u32IntTime;
			//g_stSnsRegsInfo.astI2cData[0].u32Data = ((u32IntTime & 0xFF00) >> 8);
            //g_stSnsRegsInfo.astI2cData[1].u32Data = (u32IntTime & 0xFF);
            bFirst = HI_TRUE;
        }
    }
    else
    {
        g_stSnsRegsInfo.astI2cData[0].u32Data = ((u32IntTime & 0xFF00) >> 8);
        g_stSnsRegsInfo.astI2cData[1].u32Data = (u32IntTime & 0xFF);
        bFirst = HI_TRUE;
    }
    return;
}

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
	if(genSensorMode == WDR_MODE_NONE)
	{
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
		 g_stSnsRegsInfo.astI2cData[2].u32Data = u32Again & 0xFF;
    	 g_stSnsRegsInfo.astI2cData[3].u32Data = ((u32Dgain & 0xFF00) >> 8);
    	 g_stSnsRegsInfo.astI2cData[4].u32Data = (u32Dgain & 0xFF);
		 
	}
	else if(genSensorMode == WDR_MODE_2To1_LINE)
	{
		u32Again = (u32Again>3)? 3:u32Again;
		g_stSnsRegsInfo.astI2cData[2].u32Data = (u32Again<<2)|u32Again;
		g_stSnsRegsInfo.astI2cData[3].u32Data = ((u32Dgain & 0xFF00) >> 8);
    	g_stSnsRegsInfo.astI2cData[4].u32Data = (u32Dgain & 0xFF);
		g_stSnsRegsInfo.astI2cData[7].u32Data = ((u32Dgain & 0xFF00) >> 8);
    	g_stSnsRegsInfo.astI2cData[8].u32Data = (u32Dgain & 0xFF);
		
	}
	else if(genSensorMode == WDR_MODE_BUILT_IN){
		u32Again = (u32Again>3)? 3:u32Again;	
		g_stSnsRegsInfo.astI2cData[2].u32Data = (u32Again<<2)|u32Again;
		g_stSnsRegsInfo.astI2cData[3].u32Data = ((u32Dgain & 0xFF00) >> 8);
    	g_stSnsRegsInfo.astI2cData[4].u32Data = (u32Dgain & 0xFF);
		g_stSnsRegsInfo.astI2cData[7].u32Data = ((u32Dgain & 0xFF00) >> 8);
    	g_stSnsRegsInfo.astI2cData[8].u32Data = (u32Dgain & 0xFF);

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
    if ((WDR_MODE_2To1_LINE == genSensorMode) )
    {
    	for (i=0; i<4; i++)
    	{
        	pstBlackLevel->au16BlackLevel[i] = 0x40; 
		}
    }
	else if(WDR_MODE_BUILT_IN == genSensorMode)
	{
		for (i=0; i<4; i++)
    	{
			pstBlackLevel->au16BlackLevel[i] = 0x40; 
		}
	}
	else
	{
		for (i=0; i<4; i++)
		{
			pstBlackLevel->au16BlackLevel[i] = 0x40; 
    	}
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
        if (SENSOR_1080P_30FPS_MODE == gu8SensorImageMode)
        {
            u32FullLines_5Fps = (VMAX_1080P30_LINEAR * 30) / 5;
        }
        else if (SENSOR_1080P_60FPS_MODE == gu8SensorImageMode)
        {
            u32FullLines_5Fps = (VMAX_1080P60_LINEAR * 60) / 5;
        }
        else if (SENSOR_720P_120FPS_MODE == gu8SensorImageMode)
        {
            u32FullLines_5Fps = (VMAX_720P120_LINEAR * 120) / 5;
        }
        else if (SENSOR_VGA_240FPS_MODE == gu8SensorImageMode)
        {
            u32FullLines_5Fps = (VMAX_VGA240_LINEAR * 240) / 5;
        }
        else
        {
            return;
        }
    }

 
    u32MaxIntTime_5Fps = u32FullLines_5Fps - 2;

    if (bEnable) /* setup for ISP pixel calibration mode */
    {
        sensor_write_register(VMAX_ADDR_H, (u32FullLines_5Fps & 0xFF00) >> 8);  /* 5fps */
        sensor_write_register(VMAX_ADDR_L, u32FullLines_5Fps & 0xFF);           /* 5fps */
        sensor_write_register(0x0202, (u32MaxIntTime_5Fps & 0xFF00) >> 8);      /* max exposure lines */
        sensor_write_register(0x0203, u32MaxIntTime_5Fps & 0xFF);               /* max exposure lines */
        sensor_write_register(0x0204, 0x01);                                    /* min AG */
        sensor_write_register(0x0205, 0x00);                                    /* min AG */
        sensor_write_register(0x3108, 0x01);                                    /* min DG */
        sensor_write_register(0x3109, 0x00);                                    /* min DG */
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
            else if (SENSOR_1080P_60FPS_MODE == gu8SensorImageMode)
            {
                gu32FullLinesStd = VMAX_1080P60_LINEAR;
            }
            else if (SENSOR_720P_120FPS_MODE == gu8SensorImageMode)
            {
                gu32FullLinesStd = VMAX_720P120_LINEAR;
            }
            else if (SENSOR_VGA_240FPS_MODE == gu8SensorImageMode)
            {
                gu32FullLinesStd = VMAX_VGA240_LINEAR;
            }
            genSensorMode = WDR_MODE_NONE;
            printf("linear mode\n");
        break;

        case WDR_MODE_2To1_LINE:
			if (SENSOR_1080P_30FPS_MODE == gu8SensorImageMode)
			{
				gu32FullLinesStd = VMAX_1080P60TO30_WDR;
			}
			else if (SENSOR_720P_30FPS_MODE == gu8SensorImageMode)
			{
				gu32FullLinesStd = VMAX_720P60TO30_WDR;
			}
			else
			{
				gu32FullLinesStd = VMAX_720P60TO30_WDR;
			}
            genSensorMode = WDR_MODE_2To1_LINE;
            memset(au32WDRIntTime, 0, sizeof(au32WDRIntTime));
            printf("2to1 line WDR mode\n");
        break;

	    case WDR_MODE_BUILT_IN:
            if (SENSOR_1080P_30FPS_MODE == gu8SensorImageMode)
            {
				gu32FullLinesStd = VMAX_1080P60TO30_WDR;
            }
			else if (SENSOR_720P_30FPS_MODE == gu8SensorImageMode)
            {
				gu32FullLinesStd = VMAX_720P60TO30_WDR;
            }        
			else
            {
				gu32FullLinesStd = VMAX_720P60TO30_WDR;
            }


            genSensorMode = WDR_MODE_BUILT_IN;
            memset(au32WDRIntTime, 0, sizeof(au32WDRIntTime));
            printf("sensor built in WDR mode\n");           
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

        /* DG */
        g_stSnsRegsInfo.astI2cData[3].u8DelayFrmNum = 1;
        g_stSnsRegsInfo.astI2cData[3].u32RegAddr = DGC_ADDR_H;
        g_stSnsRegsInfo.astI2cData[4].u8DelayFrmNum = 1;
        g_stSnsRegsInfo.astI2cData[4].u32RegAddr = DGC_ADDR_L;

        /* VMAX */       
        g_stSnsRegsInfo.astI2cData[5].u32RegAddr = VMAX_ADDR_H;  
        g_stSnsRegsInfo.astI2cData[5].u8DelayFrmNum = 1;
        g_stSnsRegsInfo.astI2cData[6].u32RegAddr = VMAX_ADDR_L;
        g_stSnsRegsInfo.astI2cData[6].u8DelayFrmNum = 1;
        
        if ((WDR_MODE_2To1_LINE == genSensorMode) || (WDR_MODE_BUILT_IN == genSensorMode))
        {
            g_stSnsRegsInfo.astI2cData[7].u8DelayFrmNum = 1;
            g_stSnsRegsInfo.astI2cData[7].u32RegAddr = LCG_ADDR_H;
            g_stSnsRegsInfo.astI2cData[8].u8DelayFrmNum = 1;
            g_stSnsRegsInfo.astI2cData[8].u32RegAddr = LCG_ADDR_L;
            
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

	if(gu32PreFullLines < gu32FullLines)
    {
        g_stSnsRegsInfo.astI2cData[5].u8DelayFrmNum = 0;
        g_stSnsRegsInfo.astI2cData[6].u8DelayFrmNum = 0;
    }
    else
    {
        g_stSnsRegsInfo.astI2cData[5].u8DelayFrmNum = 1;
        g_stSnsRegsInfo.astI2cData[6].u8DelayFrmNum = 1;
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
    HI_U8 u8SensorImageMode;
    
    bInit = HI_FALSE;    
        
    if (HI_NULL == pstSensorImageMode )
    {
        printf("null pointer when set image mode\n");
        return -1;
    }

  
    if ((pstSensorImageMode->u16Width <= 1920) && (pstSensorImageMode->u16Height <= 1080))
    {
        if ((WDR_MODE_2To1_LINE == genSensorMode))
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
            if (pstSensorImageMode->f32Fps <= 30)
            {
                u8SensorImageMode = SENSOR_1080P_30FPS_MODE;
            }
            else if (pstSensorImageMode->f32Fps <= 60)
            {
                u8SensorImageMode = SENSOR_1080P_60FPS_MODE;
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
    memset(au32WDRIntTime, 0, sizeof(au32WDRIntTime));

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

#endif 
