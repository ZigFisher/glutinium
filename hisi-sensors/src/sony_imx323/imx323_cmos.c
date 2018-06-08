#if !defined(__IMX323_CMOS_H_)
#define __IMX323_CMOS_H_

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

#define IMX323_ID 323

#define FULL_LINES_MAX  (0xFFFF)


/****************************************************************************
 * local variables                                                            *
 ****************************************************************************/

//Full line std
#define INCREASE_LINES (1) /* make real fps less than stand fps because NVR require*/

#define IMX323_VMAX_1080P_30FPS_LINEAR	(0x465+INCREASE_LINES) //1080p30
#define IMX323_VMAX_720P_30FPS_LINEAR	(750+INCREASE_LINES) //720p30
#define IMX323_VMAX_720P_60FPS_LINEAR	(750+INCREASE_LINES) //720p60

#define SENSOR_1080P_30FPS_MODE	(0)
#define SENSOR_720P_30FPS_MODE     (1)
#define SENSOR_720P_60FPS_MODE     (2)

#define SHS1_ADDR (0x208) 
#define GAIN_ADDR (0x21E)
#define VMAX_ADDR (0x205)

// hard limitation of RHS1 location Right Limit
        // 4n + 10
        // RHS1 <= FSC - BRL*2 -11

HI_U8 gu8SensorImageMode = SENSOR_1080P_30FPS_MODE;
WDR_MODE_E genSensorMode = WDR_MODE_NONE;

static HI_U32 gu32FullLinesStd = IMX323_VMAX_720P_30FPS_LINEAR; 
static HI_U32 gu32FullLines = IMX323_VMAX_720P_30FPS_LINEAR;
static HI_U32 gu32PreFullLines = IMX323_VMAX_720P_30FPS_LINEAR;
static HI_BOOL bInit = HI_FALSE;
HI_BOOL bSensorInit = HI_FALSE; 
static ISP_FSWDR_MODE_E genFSWDRMode = ISP_FSWDR_NORMAL_MODE;
static HI_U32 gu32MaxTimeGetCnt = 0;

ISP_SNS_REGS_INFO_S g_stSnsRegsInfo = {0};
ISP_SNS_REGS_INFO_S g_stPreSnsRegsInfo = {0};

static HI_U32 au32WDRIntTime[2] = {0};

#define PATHLEN_MAX 256
#define CMOS_CFG_INI "imx323_cfg.ini"
static char pcName[PATHLEN_MAX] = "configs/imx323_cfg.ini";


/* AE default parameter and function */


static HI_S32 cmos_get_ae_default(AE_SENSOR_DEFAULT_S *pstAeSnsDft)
{
    if(HI_NULL == pstAeSnsDft)
    {
        printf("null pointer when get ae default value!\n");
       	return -1;
    }

    if( SENSOR_1080P_30FPS_MODE == gu8SensorImageMode )
    {
       	pstAeSnsDft->u32LinesPer500ms = gu32FullLinesStd * 30 / 2;
    } 
 	else if(SENSOR_720P_30FPS_MODE == gu8SensorImageMode)
 	{
 		pstAeSnsDft->u32LinesPer500ms = gu32FullLinesStd * 30 / 2;
 	}
	else if(SENSOR_720P_60FPS_MODE == gu8SensorImageMode)
	{
		pstAeSnsDft->u32LinesPer500ms = gu32FullLinesStd * 60 / 2;
	}
    pstAeSnsDft->u32FullLinesStd = gu32FullLinesStd;
    pstAeSnsDft->u32FlickerFreq = 0;
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
    pstAeSnsDft->u32MaxISPDgainTarget = 16 << pstAeSnsDft->u32ISPDgainShift; 

    pstAeSnsDft->u32MaxAgain = 11489;
    pstAeSnsDft->u32MinAgain = 1024;
    pstAeSnsDft->u32MaxAgainTarget = pstAeSnsDft->u32MaxAgain;
    pstAeSnsDft->u32MinAgainTarget = pstAeSnsDft->u32MinAgain;

    pstAeSnsDft->u32MaxDgain = 16229;
    pstAeSnsDft->u32MinDgain = 1024;
    pstAeSnsDft->u32MaxDgainTarget = pstAeSnsDft->u32MaxDgain;
    pstAeSnsDft->u32MinDgainTarget = pstAeSnsDft->u32MinDgain;

    pstAeSnsDft->enMaxIrisFNO = ISP_IRIS_F_NO_1_0;
    pstAeSnsDft->enMinIrisFNO = ISP_IRIS_F_NO_32_0;
    pstAeSnsDft->bAERouteExValid = HI_FALSE;
    pstAeSnsDft->stAERouteAttr.u32TotalNum = 0;
    pstAeSnsDft->stAERouteAttrEx.u32TotalNum = 0;

    switch(genSensorMode)
    {
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

        default:
            printf("Sensor Mode is error!\n");
            break;
    }
    return 0;
}


/* the function of sensor set fps */
static HI_VOID cmos_fps_set(HI_FLOAT f32Fps, AE_SENSOR_DEFAULT_S *pstAeSnsDft)
{
    HI_U32 u32VMAX = IMX323_VMAX_1080P_30FPS_LINEAR;

    switch (gu8SensorImageMode)
    {        
		case SENSOR_1080P_30FPS_MODE:
		    if((f32Fps<=30)&&(f32Fps>=0.5))
			{
				u32VMAX = (IMX323_VMAX_1080P_30FPS_LINEAR * 30) / f32Fps;
			}
			else
			{
				printf("Not support Fps: %f\n", f32Fps);
                return;
			}
	  		break;
		case SENSOR_720P_30FPS_MODE:
			if((f32Fps<=30)&&(f32Fps>=0.5))
			{
				u32VMAX = (IMX323_VMAX_720P_30FPS_LINEAR * 30) / f32Fps;
			}
			else
			{
				printf("Not support Fps: %f\n", f32Fps);
                return;
			}
	  		break;
        case SENSOR_720P_60FPS_MODE:
		    if((f32Fps<=60)&&(f32Fps>=0.5))
		    {
			    u32VMAX = (IMX323_VMAX_720P_60FPS_LINEAR * 60) / f32Fps;
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

	u32VMAX = (u32VMAX > FULL_LINES_MAX) ? FULL_LINES_MAX : u32VMAX;

    if (WDR_MODE_NONE == genSensorMode)
    {
        g_stSnsRegsInfo.astSspData[3].u32Data = (u32VMAX & 0xFF);
        g_stSnsRegsInfo.astSspData[4].u32Data = ((u32VMAX & 0xFF00) >> 8);
    }
    else
    {
    }

	gu32FullLinesStd = u32VMAX;    
	
    if(WDR_MODE_NONE== genSensorMode )
    {
        pstAeSnsDft->u32MaxIntTime = gu32FullLinesStd - 4;
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
    gu32FullLines = u32FullLines;
    pstAeSnsDft->u32FullLines = gu32FullLines;

    if (WDR_MODE_NONE == genSensorMode)
    {
        g_stSnsRegsInfo.astSspData[3].u32Data = (u32FullLines & 0xFF);
        g_stSnsRegsInfo.astSspData[4].u32Data = ((u32FullLines & 0xFF00) >> 8);
    }
    else
    {
    }

    if( WDR_MODE_NONE== genSensorMode )
    {
        pstAeSnsDft->u32MaxIntTime = gu32FullLines - 4;
    }
    else
    {
    }

    return;
}


/* while isp notify ae to update sensor regs, ae call these funcs. */
static HI_VOID cmos_inttime_update(HI_U32 u32IntTime)
{
    HI_U32 u32Value = 0;
     
    u32Value = gu32FullLines - u32IntTime;
    g_stSnsRegsInfo.astSspData[0].u32Data = (u32Value & 0xFF);
    g_stSnsRegsInfo.astSspData[1].u32Data = ((u32Value & 0xFF00) >> 8);

    return;
}

static HI_U32 gain_table[151]=
{
    1024,1059,1097,1135,1175,1217,1259,1304,1349,1397,1446,1497,1549,1604,1660,1719,1779,1842,1906, //5.4dB
    1973,2043,2114,2189,2266,2345,2428,2513,2601,2693,2788,2886,2987,3092,3201,3313,3430,3550, 3675, //11.1dB
    3804,3938,4076,4219,4368,4521,4680,4845,5015,5191,5374,5562,5758,5960,6170,6387,6611,6843, 7084, //16.8dB
    7333,7591,7857,8133,8419,8715,9021,9338,9667,10006,10358,10722,11099,11489,11893,12311,12743, //21.9dB
    13191,13655,14135,14631,15146,15678,16229,16799,17390,18001,18633,19288,19966,20668,21394,22146, //26.7dB
    22924,23730,24564,25427,26320,27245,28203,29194,30220,31282,32381,33519,34697,35917,37179,38485, //31.5dB
    39838,41238,42687,44187,45740,47347,49011,50734,52517,54362,56273,58250,60297,62416,64610,66880, //36.3dB
    69230,71663,74182,76789,79487,82281,85172,88165,91264,94471,97791,101228,104785,108468,112279,  //40.8dB
    116225,120310,124537,128914,133444,138134,142988,148013,153215,158599,164172,169942,175914,182096, //45dB

};


static HI_VOID cmos_again_calc_table(HI_U32 *pu32AgainLin, HI_U32 *pu32AgainDb)
{
    int i;
	if((HI_NULL == pu32AgainLin) ||(HI_NULL == pu32AgainDb))
    {
        printf("null pointer when get ae sensor gain info value!\n");
        return;
    }

    if (*pu32AgainLin >= gain_table[71])
    {
        *pu32AgainLin = gain_table[71];
        *pu32AgainDb = 71;
        return ;
    }
    
    for (i = 1; i <= 71; i++)
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

    if (*pu32DgainLin >= gain_table[81])
    {
        *pu32DgainLin = gain_table[81];
        *pu32DgainDb = 81;
        return ;
    }
    
    for (i = 1; i <= 81; i++)
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
	HI_U32 u32Tmp;
	u32Tmp = u32Again + u32Dgain;
 	g_stSnsRegsInfo.astSspData[2].u32Data = (u32Tmp & 0xFF);
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
    pstExpFuncs->pfn_cmos_dgain_calc_table  = cmos_dgain_calc_table;
    pstExpFuncs->pfn_cmos_get_inttime_max   = NULL;
    pstExpFuncs->pfn_cmos_ae_fswdr_attr_set = cmos_ae_fswdr_attr_set;

    return 0;
}


/* AWB default parameter and function */


static AWB_CCM_S g_stAwbCcm =
{
    5096,
    {
		0x1B0,0x80A8,0x8008,
		0x804F,0x16E,0x801F,
		0x8005,0x80E5,0x1EA,
    },
    
    3700,
    {
		0x1D2,0x80BD,0x8015,
		0x807C,0x1A7,0x802B,
		0x8006,0x8136,0x23C,
    },
    
    2600,
    {
		0x20C,0x809B,0x8071,
		0x80B1,0x1F2,0x8041,
		0x8030,0x81B8,0x2E8
    }
};

static AWB_AGC_TABLE_S g_stAwbAgcTable =
{
    /* bvalid */
    1,
    /* saturation */ 
    /*100   200  400   800  1600 3200 6400 12800*/
    {0x78,0x62,0x5F,0x55,0x4B,0x41,0x37,0x30,0x28,0x28,0x23,0x23,0x23,0x23,0x23,0x23}
};

static HI_S32 cmos_get_awb_default(AWB_SENSOR_DEFAULT_S *pstAwbSnsDft)
{
    if (HI_NULL == pstAwbSnsDft)
    {
        printf("null pointer when get awb default value!\n");
        return -1;
    }

    memset(pstAwbSnsDft, 0, sizeof(AWB_SENSOR_DEFAULT_S));

    pstAwbSnsDft->u16WbRefTemp = 5096;
	
    pstAwbSnsDft->au16GainOffset[0] = 0x1B6;
    pstAwbSnsDft->au16GainOffset[1] = 0x100;
    pstAwbSnsDft->au16GainOffset[2] = 0x100;
    pstAwbSnsDft->au16GainOffset[3] = 0x1E2;

    pstAwbSnsDft->as32WbPara[0] = -37;
    pstAwbSnsDft->as32WbPara[1] = 293;
    pstAwbSnsDft->as32WbPara[2] = 0;
    pstAwbSnsDft->as32WbPara[3] = 160037;
    pstAwbSnsDft->as32WbPara[4] = 128;
    pstAwbSnsDft->as32WbPara[5] = -110984;

    memcpy(&pstAwbSnsDft->stCcm, &g_stAwbCcm, sizeof(AWB_CCM_S));
    memcpy(&pstAwbSnsDft->stAgcTbl, &g_stAwbAgcTable, sizeof(AWB_AGC_TABLE_S));

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
	{16,16,16,32,32,32,1022,1022,1022,1022,1022,1022,1022,1022,1022,1022},	
	/*au16EdgeSmoothSlope*/
	{16,32,32,48,48,48,0,0,0,0,0,0,0,0,0,0},
	//{4,16,32,32,32,32,0,0,0,0,0,0,0,0,0,0},
	/*au16AntiAliasThr*/
	{53,53,53,86,112,112,1022,1022,1022,1022,1022,1022,1022,1022,1022,1022},
	/*au16AntiAliasSlope*/
	{256,256,256,256,256,256,0,0,0,0,0,0,0,0,0,0},
    /*NrCoarseStr*/
    {128, 128, 128, 64, 64, 32, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16},
	/*NoiseSuppressStr*/
	//{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{6, 8, 8, 10, 12, 14, 16, 18, 20, 22, 24, 26, 28, 30, 32, 36},
	/*DetailEnhanceStr*/
	{4, 4, 4, 4, 2, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    /*SharpenLumaStr*/
    {0+200, 0+200, 0+200, 0+200, 0+200, 0+200, 0+200, 0+200, 0+256, 0+256, 0+256, 0+256, 0+256, 0+256, 0+256, 0+256},
    /*ColorNoiseCtrlThr*/
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
};

/***BAYER NR**/
static ISP_CMOS_BAYERNR_S g_stIspBayerNr =
{
	14,     //Calibration Lut Num
	/*************Calibration LUT Table*************/
	{
        {100.000000f, 0.031029f}, 
    	{200.000000f, 0.054216f}, 
    	{400.000000f, 0.099667f}, 
    	{800.000000f, 0.185811f}, 
    	{1600.000000f, 0.360527f}, 
    	{3200.000000f, 0.726792f}, 
    	{6400.000000f, 1.504356f}, 
    	{12000.000000f, 3.027248f}, 
    	{27300.000000f, 5.601446f}, 
    	{56800.000000f, 6.510498f}, 
    	{114000.000000f, 7.209918f}, 
    	{220600.000000f, 7.353228f}, 
    	{253200.000000f, 7.353253f}, 
    	{282200.000000f, 7.365693f},
	},
	/*********************************************/
	{140, 110, 110, 140},		  //CoraseStr
	{100, 90, 80, 73, 70, 60, 50, 65, 65, 75, 80, 80, 80, 80, 80, 80},     //lutFineStr
	{
	  	{1,1,1,1,2,2,2,3,3,3,3,3,3,3,3,3},   //ChromaStrR
      	{0,0,0,0,1,1,1,2,2,2,2,2,2,3,3,3},   //ChromaStrGr
	  	{0,0,0,0,1,1,1,2,2,2,2,2,2,3,3,3},   //ChromaStrGb
	  	{1,1,1,1,2,2,2,3,3,3,3,3,3,3,3,3}    //ChromaStrB 
	},
	{1100, 1100, 1000, 1000, 1000, 950, 900, 850, 800, 750, 750, 750, 750, 750, 750, 750},     //lutCoringWeight
	{600, 600, 600, 600, 650, 650, 650, 700, 700, 700, 700, 700, 700, 700, 700, 700, \
           800, 800, 800, 850, 850, 850, 900, 900, 900, 950, 950, 950, 1000, 1000, 1000, 1000, 1000}
};

static ISP_CMOS_YUV_SHARPEN_S g_stIspYuvSharpen = 
{
    /* bvalid */
    1,

    /* 100,  200,    400,     800,    1600,    3200,    6400,    12800,    25600,   51200,  102400,  204800,   409600,   819200,   1638400,  3276800 */

    /* au16SharpenUd */
	{40, 30, 26, 22, 18, 16, 14, 12, 10, 9,	8, 6, 3, 2,	1, 1},
	
	/* au8SharpenD */
    {85, 80, 80, 90, 100, 100, 120, 140, 140, 140, 150, 150, 170, 180, 200, 200},
	 
	/*au8TextureThd*/
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},

    /* au8SharpenEdge */
    {60, 60, 60, 70, 70, 80, 80, 80, 80, 80, 100, 120, 150, 180, 180, 200},

    /* au8EdgeThd */
    {80, 80, 80, 80, 80, 80, 80, 90, 100, 110, 120, 130, 150, 170, 180, 190},	 
	 	
	/* au8OverShoot */
    {80, 150, 150, 150, 130, 90, 60, 40, 30, 20, 10, 10, 10, 10, 10, 10},
        
    /* au8UnderShoot */
    {100, 200, 200, 200, 180, 120, 80, 60, 50, 40, 20, 15, 15, 15, 15, 15},

	/*au8shootSupSt*/
	{0, 33, 33, 33, 16, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	 
	/* au8DetailCtrl */
    {128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128},
};                                                                                                             

static ISP_CMOS_GAMMA_S g_stIspGamma =
{
    /* bvalid */
    1,
    
#if 1 /* Normal mode */   
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


static ISP_CMOS_CA_S g_stIspCA = 
{
    /*CA  Enable*/
	1,
	/*Y Gain Table*/
    {36 ,81  ,111 ,136 ,158 ,182 ,207 ,228 ,259 ,290 ,317 ,345 ,369 ,396 ,420 ,444 ,
	468 ,492 ,515 ,534 ,556 ,574 ,597 ,614 ,632 ,648 ,666 ,681 ,697 ,709 ,723 ,734 ,
	748 ,758 ,771 ,780 ,788 ,800 ,808 ,815 ,822 ,829 ,837 ,841 ,848 ,854 ,858 ,864 ,
	868 ,871 ,878 ,881 ,885 ,890 ,893 ,897 ,900 ,903 ,906 ,909 ,912 ,915 ,918 ,921 ,
	924 ,926 ,929 ,931 ,934 ,936 ,938 ,941 ,943 ,945 ,947 ,949 ,951 ,952 ,954 ,956 ,
	958 ,961 ,962 ,964 ,966 ,968 ,969 ,970 ,971 ,973 ,974 ,976 ,977 ,979 ,980 ,981 ,
	983 ,984 ,985 ,986 ,988 ,989 ,990 ,991 ,992 ,993 ,995 ,996 ,997 ,998 ,999 ,1000,
	1001,1004,1005,1006,1007,1009,1010,1011,1012,1014,1016,1017,1019,1020,1022,1024},
	/*ISO Gain Table*/
    {1300,1300,1250,1200,1150,1100,1050,1000,950,900,900,800,800,800,800,800}
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

static ISP_CMOS_GE_S g_stIspGe =
{
	/*For GE*/
	1,    /*bEnable*/			
	9,    /*u8Slope*/	
	9,    /*u8SensiSlope*/	
	300, /*u16SensiThr*/	
	{300,300,300,300,310,310,310,  310,  320,320,320,320,330,330,330,330}, /*au16Threshold[ISP_AUTO_ISO_STRENGTH_NUM]*/
	{128, 128, 128, 128, 129, 129, 129,   129,   130, 130, 130, 130, 131, 131, 131, 131}, /*au16Strength[ISP_AUTO_ISO_STRENGTH_NUM]*/
	{1024,1024,1024,2048,2048,2048,2048,  2048,  2048,2048,2048,2048,2048,2048,2048,2048}    /*au16NpOffset[ISP_AUTO_ISO_STRENGTH_NUM]*/	
};


static ISP_CMOS_FCR_S g_stIspFcr =
{
	/*For FCR*/
	1,    /*bEnable*/				
	{14,10,8,8,7,7,7,  6,  6,6,5,4,3,2,1,0}, /*au8Strength[ISP_AUTO_ISO_STRENGTH_NUM]*/
	{35, 27, 24, 24, 20, 20, 20,   16,   14, 12, 10, 8, 6, 4, 2, 0}, /*au8Threhold[ISP_AUTO_ISO_STRENGTH_NUM]*/
	{120,150,150,150,150,150,150,  150,  150,150,150,150,150,150,150,150}    /*au16Offset[ISP_AUTO_ISO_STRENGTH_NUM]*/	
};



static ISP_CMOS_DPC_S g_stCmosDpc = 
{
    //0,/*IR_channel*/  
    //0,/*IR_position*/
    {0,0,152,200,200,200,220,220,220,220,152,152,152,152,152,152},    /*au16Strength[16]*/
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 50, 50, 50, 50, 50, 50},    /*au16BlendRatio[16]*/
};	

HI_U32 cmos_get_isp_default(ISP_CMOS_DEFAULT_S *pstDef)
{
    if (HI_NULL == pstDef)
    {
        printf("null pointer when get isp default value!\n");
        return -1;
    }

    memset(pstDef, 0, sizeof(ISP_CMOS_DEFAULT_S));
   	memcpy(&pstDef->stCa, &g_stIspCA,sizeof(ISP_CMOS_CA_S));

    switch (genSensorMode)
    {
        case WDR_MODE_NONE:
            memcpy(&pstDef->stDemosaic, &g_stIspDemosaic, sizeof(ISP_CMOS_DEMOSAIC_S));            
	     	memcpy(&pstDef->stYuvSharpen, &g_stIspYuvSharpen, sizeof(ISP_CMOS_YUV_SHARPEN_S));            
	     	memcpy(&pstDef->stDrc, &g_stIspDRC, sizeof(ISP_CMOS_DRC_S));
	    	memcpy(&pstDef->stGamma, &g_stIspGamma, sizeof(ISP_CMOS_GAMMA_S));					
	    	memcpy(&pstDef->stBayerNr, &g_stIspBayerNr, sizeof(ISP_CMOS_BAYERNR_S));
		    memcpy(&pstDef->stGe, &g_stIspGe, sizeof(ISP_CMOS_GE_S));					   
		    memcpy(&pstDef->stFcr, &g_stIspFcr, sizeof(ISP_CMOS_FCR_S));			
		    memcpy(&pstDef->stDpc,       &g_stCmosDpc,       sizeof(ISP_CMOS_DPC_S));
        	break;
	    default:
		    printf("Sensor Mode is error! cmos_get_isp_default Failuer!\n");
		    break;
    }
    if(SENSOR_1080P_30FPS_MODE == gu8SensorImageMode)
    {
    	pstDef->stSensorMaxResolution.u32MaxWidth  = 1920;
    	pstDef->stSensorMaxResolution.u32MaxHeight = 1080;
    }
	else
	{
		pstDef->stSensorMaxResolution.u32MaxWidth  = 1280;
    	pstDef->stSensorMaxResolution.u32MaxHeight = 720;
	}
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
       
    for (i=0; i<4; i++)
    {
        pstBlackLevel->au16BlackLevel[i] = 0xF0;
    }
    
    return 0;    
}

HI_VOID cmos_set_pixel_detect(HI_BOOL bEnable)
{
    HI_U32 u32FullLines_5Fps;
    
    if(SENSOR_1080P_30FPS_MODE == gu8SensorImageMode)
    {
        u32FullLines_5Fps = (IMX323_VMAX_1080P_30FPS_LINEAR * 30) / 5;
    }
    else if(SENSOR_720P_30FPS_MODE == gu8SensorImageMode)
    {
        u32FullLines_5Fps = (IMX323_VMAX_720P_30FPS_LINEAR * 30) / 5;
    }
    else if(SENSOR_720P_60FPS_MODE == gu8SensorImageMode)
    {
        u32FullLines_5Fps = (IMX323_VMAX_720P_60FPS_LINEAR * 60) / 5;
    }
    else
    {
        return;
    }



    if (bEnable) /* setup for ISP pixel calibration mode */
    {
        sensor_write_register (GAIN_ADDR,0x00);
        sensor_write_register (GAIN_ADDR + 1,0x00);
        
        sensor_write_register (VMAX_ADDR, u32FullLines_5Fps & 0xFF); 
        sensor_write_register (VMAX_ADDR + 1, (u32FullLines_5Fps & 0xFF00) >> 8); 

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
        bInit = HI_FALSE;
    }

    return;
}

HI_VOID cmos_set_wdr_mode(HI_U8 u8Mode)
{
    bInit = HI_FALSE;

    switch(u8Mode)
    {
        case WDR_MODE_NONE://0
            genSensorMode = WDR_MODE_NONE;
            
            if(SENSOR_1080P_30FPS_MODE ==gu8SensorImageMode)
            {
                gu32FullLinesStd = IMX323_VMAX_1080P_30FPS_LINEAR;
            }
		    else if(SENSOR_720P_30FPS_MODE ==gu8SensorImageMode)
            {
                gu32FullLinesStd = IMX323_VMAX_720P_30FPS_LINEAR;
            }
		    else if(SENSOR_720P_60FPS_MODE ==gu8SensorImageMode)
            {
                gu32FullLinesStd = IMX323_VMAX_720P_60FPS_LINEAR;
            }
		    else
		    {
		    }
            printf("linear mode\n");
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
	HI_U8 u8SensorImageMode ;
	bInit = HI_FALSE;    

   	if (HI_NULL == pstSensorImageMode)
    {
        printf("null pointer when set image mode\n");
        return -1;
    }

	if((pstSensorImageMode->u16Width <= 1280)&&(pstSensorImageMode->u16Height <= 720))
	{
		if(pstSensorImageMode->f32Fps<=30)
    	{
            u8SensorImageMode = SENSOR_720P_30FPS_MODE;
            if(WDR_MODE_NONE == genSensorMode)
            {
                gu32FullLinesStd = IMX323_VMAX_720P_30FPS_LINEAR;
            }
            else
            {
                printf("=======Not Support This Mode=======");
            }
    	}
		else if(pstSensorImageMode->f32Fps<=60)
		{
            u8SensorImageMode = SENSOR_720P_60FPS_MODE;
            if(WDR_MODE_NONE == genSensorMode)
            {
                gu32FullLinesStd = IMX323_VMAX_720P_60FPS_LINEAR;
            }
            else
            {
                printf("=======Not Support This Mode=======");
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
    else if((pstSensorImageMode->u16Width <= 1920)&&(pstSensorImageMode->u16Height <= 1080))
    {
    	if(pstSensorImageMode->f32Fps<=30)
    	{
            u8SensorImageMode = SENSOR_1080P_30FPS_MODE;
            if(WDR_MODE_NONE == genSensorMode)
            {
                gu32FullLinesStd = IMX323_VMAX_1080P_30FPS_LINEAR;
            }
            else
            {
            	printf("=======Not Support This Mode=======");
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
        g_stSnsRegsInfo.u32RegNum = 5;
         
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
		
        
        g_stSnsRegsInfo.astSspData[2].u8DelayFrmNum = 0;        //gain
        g_stSnsRegsInfo.astSspData[2].u32RegAddr = GAIN_ADDR;

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
    gu8SensorImageMode = SENSOR_720P_30FPS_MODE;
    genSensorMode = WDR_MODE_NONE;
    gu32FullLinesStd = IMX323_VMAX_720P_30FPS_LINEAR; 
    gu32FullLines = IMX323_VMAX_720P_30FPS_LINEAR;
    gu32PreFullLines = IMX323_VMAX_720P_30FPS_LINEAR;

    bInit = HI_FALSE;
    bSensorInit = HI_FALSE;
    genFSWDRMode = ISP_FSWDR_NORMAL_MODE;
    
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
    s32Ret = HI_MPI_ISP_SensorRegCallBack(IspDev, IMX323_ID, &stIspRegister);
    if (s32Ret)
    {
        printf("sensor register callback function failed!\n");
        return s32Ret;
    }
    
    stLib.s32Id = 0;
    strncpy(stLib.acLibName, HI_AE_LIB_NAME, sizeof(HI_AE_LIB_NAME));
    cmos_init_ae_exp_function(&stAeRegister.stSnsExp);
    s32Ret = HI_MPI_AE_SensorRegCallBack(IspDev, &stLib, IMX323_ID, &stAeRegister);
    if (s32Ret)
    {
        printf("sensor register callback function to ae lib failed!\n");
        return s32Ret;
    }

    stLib.s32Id = 0;
    strncpy(stLib.acLibName, HI_AWB_LIB_NAME, sizeof(HI_AWB_LIB_NAME));
    cmos_init_awb_exp_function(&stAwbRegister.stSnsExp);
    s32Ret = HI_MPI_AWB_SensorRegCallBack(IspDev, &stLib, IMX323_ID, &stAwbRegister);
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

    s32Ret = HI_MPI_ISP_SensorUnRegCallBack(IspDev, IMX323_ID);
    if (s32Ret)
    {
        printf("sensor unregister callback function failed!\n");
        return s32Ret;
    }
    
    stLib.s32Id = 0;
    strncpy(stLib.acLibName, HI_AE_LIB_NAME, sizeof(HI_AE_LIB_NAME));
    s32Ret = HI_MPI_AE_SensorUnRegCallBack(IspDev, &stLib, IMX323_ID);
    if (s32Ret)
    {
        printf("sensor unregister callback function to ae lib failed!\n");
        return s32Ret;
    }

    stLib.s32Id = 0;
    strncpy(stLib.acLibName, HI_AWB_LIB_NAME, sizeof(HI_AWB_LIB_NAME));
    s32Ret = HI_MPI_AWB_SensorUnRegCallBack(IspDev, &stLib, IMX323_ID);
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
