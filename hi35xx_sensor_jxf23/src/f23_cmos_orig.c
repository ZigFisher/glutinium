#if !defined(__JXF23_CMOS_H_)
#define __JXF23_CMOS_H_

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


#define JXF23_ID 23

/**************************************************************************** 
* local variables                                                            
* 
****************************************************************************/
extern const unsigned int sensor_i2c_addr;
extern unsigned int sensor_addr_byte;
extern unsigned int sensor_data_byte;

#define FULL_LINES_MAX          (0xFFFF)
#define SENSOR_720P_30FPS_MODE  (1)
#define SENSOR_960P_30FPS_MODE  (2)
#define SENSOR_1080P_30FPS_MODE (3)
#define SENSOR_1080P_60FPS_MODE (4)
#define VMAX_720P30_LINEAR      (1328)
#define VMAX_960P30_LINEAR      (1328)
#define VMAX_1080P30_LINEAR      (1134)

HI_U8 gu8SensorImageMode = SENSOR_720P_30FPS_MODE;
WDR_MODE_E genSensorMode = WDR_MODE_NONE;
static HI_U32 gu32FullLinesStd = VMAX_720P30_LINEAR;
static HI_U32 gu32FullLines = VMAX_720P30_LINEAR;
static HI_BOOL bInit = HI_FALSE;
HI_BOOL bSensorInit = HI_FALSE;
ISP_SNS_REGS_INFO_S g_stSnsRegsInfo = {0};
ISP_SNS_REGS_INFO_S g_stPreSnsRegsInfo = {0};

/* AE default parameter and function */
static HI_S32 cmos_get_ae_default(AE_SENSOR_DEFAULT_S *pstAeSnsDft)
{
	if (HI_NULL == pstAeSnsDft)
	{        
		printf("null pointer when get ae default value!\n");
		return -1;    
	}
	pstAeSnsDft->u32LinesPer500ms = VMAX_720P30_LINEAR * 30 / 2;    
	pstAeSnsDft->u32FullLinesStd = gu32FullLinesStd;
	pstAeSnsDft->u32FlickerFreq = 0;
	pstAeSnsDft->au8HistThresh[0] = 0xd;
	pstAeSnsDft->au8HistThresh[1] = 0x28;
	pstAeSnsDft->au8HistThresh[2] = 0x60;    
	pstAeSnsDft->au8HistThresh[3] = 0x80;
	pstAeSnsDft->u8AeCompensation = 0x38;
	pstAeSnsDft->stIntTimeAccu.enAccuType = AE_ACCURACY_LINEAR;
	pstAeSnsDft->stIntTimeAccu.f32Accuracy = 1;    
	pstAeSnsDft->stIntTimeAccu.f32Offset = 0;
	pstAeSnsDft->u32MaxIntTime = gu32FullLinesStd - 2;
	pstAeSnsDft->u32MinIntTime = 2;
	pstAeSnsDft->u32MaxIntTimeTarget = pstAeSnsDft->u32MaxIntTime;
	pstAeSnsDft->u32MinIntTimeTarget = pstAeSnsDft->u32MinIntTime;
	pstAeSnsDft->stAgainAccu.enAccuType = AE_ACCURACY_TABLE;
	pstAeSnsDft->stAgainAccu.f32Accuracy = 1;
	pstAeSnsDft->u32MaxAgain = 8196;
	pstAeSnsDft->u32MinAgain = 1024;    
	pstAeSnsDft->u32MaxAgainTarget = pstAeSnsDft->u32MaxAgain;
	pstAeSnsDft->u32MinAgainTarget = pstAeSnsDft->u32MinAgain;
	pstAeSnsDft->stDgainAccu.enAccuType = AE_ACCURACY_LINEAR;
	pstAeSnsDft->stDgainAccu.f32Accuracy = 0.0625;
	pstAeSnsDft->u32MaxDgain = 31;
	pstAeSnsDft->u32MinDgain = 16;
	pstAeSnsDft->u32MaxDgainTarget = pstAeSnsDft->u32MaxDgain;
	pstAeSnsDft->u32MinDgainTarget = pstAeSnsDft->u32MinDgain;
	pstAeSnsDft->u32ISPDgainShift = 8;
	pstAeSnsDft->u32MinISPDgainTarget = 1 << pstAeSnsDft->u32ISPDgainShift;
	pstAeSnsDft->u32MaxISPDgainTarget = 2 << pstAeSnsDft->u32ISPDgainShift;
	return 0;
}

/* the function of sensor set fps */
static HI_VOID cmos_fps_set(HI_FLOAT f32Fps, AE_SENSOR_DEFAULT_S *pstAeSnsDft)
{
	if ((f32Fps <= 30) && (f32Fps >= 0.5))
	{
		if(SENSOR_720P_30FPS_MODE == gu8SensorImageMode)        
		{
			gu32FullLinesStd = VMAX_720P30_LINEAR * 30 / f32Fps;
		}
		if(SENSOR_960P_30FPS_MODE == gu8SensorImageMode)        
		{
			gu32FullLinesStd = VMAX_960P30_LINEAR * 30 / f32Fps;
		}
		if(SENSOR_1080P_30FPS_MODE == gu8SensorImageMode)        
		{
			gu32FullLinesStd = VMAX_1080P30_LINEAR * 30 / f32Fps;
		}
		g_stSnsRegsInfo.astI2cData[3].u32Data = (gu32FullLinesStd >> 8) & 0xff;
		g_stSnsRegsInfo.astI2cData[4].u32Data = gu32FullLinesStd & 0xff;
		pstAeSnsDft->u32MaxIntTime = gu32FullLinesStd - 2;
		pstAeSnsDft->u32FullLinesStd = gu32FullLinesStd;
	}
	else
	{
		printf("Not support Fps: %f\n", f32Fps);
		return;
	}
	return;
}

static HI_VOID cmos_slow_framerate_set(HI_U32 u32FullLines, AE_SENSOR_DEFAULT_S *pstAeSnsDft)
{
	gu32FullLines = u32FullLines;        
	g_stSnsRegsInfo.astI2cData[3].u32Data = (gu32FullLines >> 8) & 0xff;    
	g_stSnsRegsInfo.astI2cData[4].u32Data = gu32FullLines & 0xff;        
	pstAeSnsDft->u32MaxIntTime = gu32FullLines - 2;
	return;
}

/* while isp notify ae to update sensor regs, ae call these funcs. */static 
HI_VOID cmos_inttime_update(HI_U32 u32IntTime)
{
	g_stSnsRegsInfo.astI2cData[0].u32Data = (u32IntTime >> 8) & 0xFF;
	g_stSnsRegsInfo.astI2cData[1].u32Data = u32IntTime & 0xFF;
	return;
}

static HI_U32 analog_gain_table[4] =
{
	1024, 2048, 4096, 8192
};

static HI_VOID cmos_again_calc_table(HI_U32 *pu32AgainLin, HI_U32 *pu32AgainDb)
{
	int i;
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
	g_stSnsRegsInfo.astI2cData[2].u32Data = (u32Again << 4) | ((u32Dgain - 16) & 0xf);
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
	pstExpFuncs->pfn_cmos_dgain_calc_table  = NULL;
	return 0;
}

/* AWB default parameter and function */
static AWB_CCM_S g_stAwbCcm =
{
	5000,
	{
		0x195, 0x8092, 0x8003,
		0x8067, 0x17e, 0x8017,
		0x8067, 0x80f3, 0x25b
	},
	4000,
	{
		0x195, 0x8092, 0x8003,
		0x8067, 0x17e, 0x8017,
		0x8067, 0x80f3, 0x25b
	},
	2850,
	{
		0xe9, 0x25, 0x800e,
		0x80bb, 0x269, 0x80ad,
		0x80a9, 0x8046, 0x1ef
	}
};
static AWB_AGC_TABLE_S g_stAwbAgcTable =
{
	/* bvalid */
	1,
	/*1,  2,  4,  8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384, 32768*/
	/* saturation */
	{
		0x80,0x80,0x7a,0x78,0x70,0x68,0x60,0x58,0x50,0x48,0x40,0x38,0x38,0x38,0x38,0x38
	}
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
	pstAwbSnsDft->au16GainOffset[0] = 400;
	pstAwbSnsDft->au16GainOffset[1] = 0x100;
	pstAwbSnsDft->au16GainOffset[2] = 0x100;
	pstAwbSnsDft->au16GainOffset[3] = 0x1b9;
	pstAwbSnsDft->as32WbPara[0] = 168;        
	pstAwbSnsDft->as32WbPara[1] = -66;
	pstAwbSnsDft->as32WbPara[2] = -154;
	pstAwbSnsDft->as32WbPara[3] = 178500;
	pstAwbSnsDft->as32WbPara[4] = 128;
	pstAwbSnsDft->as32WbPara[5] = -130964;
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

#define DMNR_CALIB_CARVE_NUM_H42 5
float g_coef_calib_h42[DMNR_CALIB_CARVE_NUM_H42][4] = 
{
	{100.000000f, 2.000000f, 0.040110f, 8.391255f, },
	{800.000000f, 2.903090f, 0.047486f, 9.635272f, },
	{2525.000000f, 3.402261f, 0.055028f, 13.068982f, },
	{5500.000000f, 3.740363f, 0.063797f, 17.750826f, },
	{6194.000000f, 3.791971f, 0.062829f, 17.908888f, },
};
static ISP_NR_ISO_PARA_TABLE_S g_stNrIsoParaTab[HI_ISP_NR_ISO_LEVEL_MAX] = 
{
	//u16Threshold//u8varStrength//u8fixStrength//u8LowFreqSlope
	{1500,       160,             256-256,            0 },  //100    //                      //
	{1500,       120,             256-256,            0 },  //200    // ISO                  // ISO //u8LowFreqSlope
	{1500,       100,             256-256,            0 },  //400    //{400,  1200, 96,256}, //{400 , 0  }
	{1750,       80,              256-256,            8 },  //800    //{800,  1400, 80,256}, //{600 , 2  }
	{1500,       255,             256-256,            6 },  //1600   //{1600, 1200, 72,256}, //{800 , 8  }
	{1500,       255,             256-256,            0 },  //3200   //{3200, 1200, 64,256}, //{1000, 12 }
	{1375,       255,             256-256,            0 },  //6400   //{6400, 1100, 56,256}, //{1600, 6  }
	{1375,       255,             256-256,            0 },  //12800  //{12000,1100, 48,256}, //{2400, 0  }
	{1375,       255,             256-256,            0 },  //25600  //{36000,1100, 48,256}, //
	{1375,       255,             256-256,            0 },  //51200  //{64000,1100, 96,256}, //
	{1250,       255,             256-256,            0 },  //102400 //{82000,1000,240,256}, //
	{1250,       255,             256-256,            0 },  //204800 //                           //
	{1250,       255,             256-256,            0 },  //409600 //                           //
	{1250,       255,             256-256,            0 },  //819200 //                           //
	{1250,       255,             256-256,            0 },  //1638400//                           //
	{1250,       255,             256-256,            0 },  //3276800//                           //
};
static ISP_CMOS_DEMOSAIC_S g_stIspDemosaic =
{
	/*For Demosaic*/
	1, /*bEnable*/
	24,/*u16VhLimit*/
	40-24,/*u16VhOffset*/
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
	1,    /*bEnable*/
	7,    /*u8Slope*/
	7,    /*u8Sensitivity*/
	8192, /*u16Threshold*/
	8192, /*u16SensiThreshold*/
	{1024,1024,1024,2048,2048,2048,2048,  2048,  2048,2048,2048,2048,2048,2048,2048,2048}    /*au16Strength[ISP_AUTO_ISO_STENGTH_NUM]*/
};
static ISP_CMOS_RGBSHARPEN_S g_stIspRgbSharpen =
{
	//{100,200,400,800,1600,3200,6400,12800,25600,51200,102400,204800,409600,819200,1638400,3276800};
	{0,	  0,   0,   0,    0,   1,   1,   1,    1,    1,    1,     1,     1,     1,     1,       1},/* enPixSel = ~bEnLowLumaShoot */
	{120,  64,  64,  43,  43,  43,  18,  18,    18,  200,  250,   250,   250,   250,    250,    250},/*maxSharpAmt1 = SharpenUD*16 */
	{128, 200, 103, 86,  86,  86,  80,  80,    80,  250,  250,   250,   250,   250,    250,    250},/*maxEdgeAmt = SharpenD*16 */
	{0,   0,   0,    0,   0,   0,   0,   40,  190,  200,  220,   250,   250,   250,     250,       250},/*sharpThd2 = TextureNoiseThd*4 */
	{0,   0,   0,    0,   0,   0,   0,   60,  140,    0,    0,     0,    0,     0,     0,       0},/*edgeThd2 = EdgeNoiseThd*4 */
	{59,  59,  59,  59,  59,  59,  59,   59,  101,  0,   0,    0,   0,    0,    0,      0},/*overshootAmt*/
	{117, 117, 117, 108, 108, 108, 122,  122, 139,  0,   0,    0,   0,    0,    0,     0},/*undershootAmt*/
};
static ISP_CMOS_UVNR_S g_stIspUVNR = 
{
	/*中值滤波切换到UVNR的ISO阈值*/
	/*UVNR切换到中值滤波的ISO阈值*/
	/*0.0   -> disable，(0.0, 1.0]  -> weak，(1.0, 2.0]  -> normal，(2.0, 10.0) -> strong*/
	/*高斯滤波器的标准差*/
	//{100,	200,	400,	800,	1600,	3200,	6400,	12800,	25600,	51200,	102400,	204800,	409600,	819200,	1638400,	3276800};
	{1,	    2,       4,      5,      7,      48,     32,     16,     16,     16,      16,     16,     16,     16,     16,        16},      /*UVNRThreshold*/
	{0,		0,		0,		0,		0,		0,		0,		0,		0,		1,			1,		2,		2,		2,		2,		2},  /*Coring_lutLimit*/
	{0,		0,		0,		16,		34,		34,		34,		34,		34,		34,		34,		34,		34,		34,		34,			34}  /*UVNR_blendRatio*/
};
static ISP_CMOS_DPC_S g_stCmosDpc = 
{
	//1,/*IR_channel*/
	//1,/*IR_position*/
	{0,0,0,1,1,1,2,2,2,3,3,3,3,3,3,3},/*au16Strength[16]*/
	{0,0,0,0,0,0,0,0,0x24,0x80,0x80,0x80,0xE5,0xE5,0xE5,0xE5},/*au16BlendRatio[16]*/
};
HI_U32 cmos_get_isp_default(ISP_CMOS_DEFAULT_S *pstDef)
{
	if (HI_NULL == pstDef)
	{
		printf("null pointer when get isp default value!\n");
		return -1;
	}
	memset(pstDef, 0, sizeof(ISP_CMOS_DEFAULT_S));
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
	memcpy(&pstDef->stDemosaic, &g_stIspDemosaic, sizeof(ISP_CMOS_DEMOSAIC_S));
	memcpy(&pstDef->stRgbSharpen, &g_stIspRgbSharpen, sizeof(ISP_CMOS_RGBSHARPEN_S));
	memcpy(&pstDef->stGe, &g_stIspGe, sizeof(ISP_CMOS_GE_S));
	//pstDef->stNoiseTbl.u8SensorIndex = HI_ISP_NR_SENSOR_INDEX_OV9712;
	pstDef->stNoiseTbl.stNrCaliPara.u8CalicoefRow = DMNR_CALIB_CARVE_NUM_H42;
	pstDef->stNoiseTbl.stNrCaliPara.pCalibcoef    = (HI_FLOAT (*)[4])g_coef_calib_h42;
	memcpy(&pstDef->stNoiseTbl.stIsoParaTable[0], &g_stNrIsoParaTab[0],sizeof(ISP_NR_ISO_PARA_TABLE_S)*HI_ISP_NR_ISO_LEVEL_MAX);
	memcpy(&pstDef->stUvnr,       &g_stIspUVNR,       sizeof(ISP_CMOS_UVNR_S));
	memcpy(&pstDef->stDpc,       &g_stCmosDpc,       sizeof(ISP_CMOS_DPC_S));
	pstDef->stSensorMaxResolution.u32MaxWidth  = 1920;
	pstDef->stSensorMaxResolution.u32MaxHeight = 1080;
	return 0;
}

HI_U32 cmos_get_isp_black_level(ISP_CMOS_BLACK_LEVEL_S *pstBlackLevel)
{
	if (HI_NULL == pstBlackLevel)
	{
		printf("null pointer when get isp black level value!\n");
		return -1;
	}

	/* Don't need to update black level when iso change */
	pstBlackLevel->bUpdate = HI_FALSE;

	pstBlackLevel->au16BlackLevel[0] = 64;
	pstBlackLevel->au16BlackLevel[1] = 64;
	pstBlackLevel->au16BlackLevel[2] = 64;
	pstBlackLevel->au16BlackLevel[3] = 64;
	return 0;  
	
}

HI_VOID cmos_set_pixel_detect(HI_BOOL bEnable)
{
/*	HI_U32 u32FullLines_5Fps = 0;

	if ((WDR_MODE_NONE == genSensorMode) && (SENSOR_720P_30FPS_MODE == gu8SensorImageMode))
	{
		u32FullLines_5Fps = (VMAX_720P30_LINEAR * 30) / 5;
	}
	else
	{
		return;
	}
	
	u32FullLines_5Fps = (u32FullLines_5Fps > FULL_LINES_MAX) ? FULL_LINES_MAX : u32FullLines_5Fps;
*/
	if (bEnable) /* setup for ISP pixel calibration mode */
	{        
		sensor_write_register(0x22, 0x94);
		sensor_write_register(0x23, 0x11);
		sensor_write_register(0x01, 0x92);
		sensor_write_register(0x02, 0x11);
		sensor_write_register(0x00, 0x00);
	}
	else /* setup for ISP 'normal mode' */
	{
		sensor_write_register(0x22, (gu32FullLinesStd >> 8) & 0xff);
		sensor_write_register(0x23, gu32FullLinesStd & 0xff);
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
			if (SENSOR_720P_30FPS_MODE == gu8SensorImageMode)
			{
				gu32FullLinesStd = VMAX_720P30_LINEAR;
			}
			if (SENSOR_960P_30FPS_MODE == gu8SensorImageMode)
			{
				gu32FullLinesStd = VMAX_960P30_LINEAR;
			}
			if (SENSOR_1080P_30FPS_MODE == gu8SensorImageMode)
			{
				gu32FullLinesStd = VMAX_1080P30_LINEAR;
			}
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
		g_stSnsRegsInfo.enSnsType = ISP_SNS_I2C_TYPE;
		g_stSnsRegsInfo.u8Cfg2ValidDelayMax = 2;
		
		g_stSnsRegsInfo.u32RegNum = 5;
		
		for (i=0; i<g_stSnsRegsInfo.u32RegNum; i++)
		{
			g_stSnsRegsInfo.astI2cData[i].bUpdate = HI_TRUE;
			g_stSnsRegsInfo.astI2cData[i].u8DevAddr = sensor_i2c_addr;
			g_stSnsRegsInfo.astI2cData[i].u32AddrByteNum = sensor_addr_byte;
			g_stSnsRegsInfo.astI2cData[i].u32DataByteNum = sensor_data_byte;
		}
		g_stSnsRegsInfo.astI2cData[0].u8DelayFrmNum = 0;
		g_stSnsRegsInfo.astI2cData[0].u32RegAddr = 0x02;

		g_stSnsRegsInfo.astI2cData[1].u8DelayFrmNum = 0;
		g_stSnsRegsInfo.astI2cData[1].u32RegAddr = 0x01;

		g_stSnsRegsInfo.astI2cData[2].u8DelayFrmNum = 1;
		g_stSnsRegsInfo.astI2cData[2].u32RegAddr = 0x00;

		g_stSnsRegsInfo.astI2cData[3].u8DelayFrmNum = 0;
		g_stSnsRegsInfo.astI2cData[3].u32RegAddr = 0x23;
		g_stSnsRegsInfo.astI2cData[4].u8DelayFrmNum = 0;
		g_stSnsRegsInfo.astI2cData[4].u32RegAddr = 0x22;

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

static HI_S32 cmos_set_image_mode(ISP_CMOS_SENSOR_IMAGE_MODE_S *pstSensorImageMode)
{
	HI_U8 u8SensorImageMode = gu8SensorImageMode;

	bInit = HI_FALSE;	 

	if (HI_NULL == pstSensorImageMode )
	{
		printf("null pointer when set image mode\n");
		return -1;
	}

	if ((pstSensorImageMode->u16Width <= 1280) && (pstSensorImageMode->u16Height <= 720))
	{
		if (pstSensorImageMode->f32Fps <= 30)
		{
			u8SensorImageMode = SENSOR_720P_30FPS_MODE;
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
	else if ((pstSensorImageMode->u16Width <= 1280) && (pstSensorImageMode->u16Height <= 960))
	{
		if (pstSensorImageMode->f32Fps <= 30)
		{
			u8SensorImageMode = SENSOR_960P_30FPS_MODE;
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
	else if ((pstSensorImageMode->u16Width <= 1920) && (pstSensorImageMode->u16Height <= 1080))
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
	gu8SensorImageMode = SENSOR_720P_30FPS_MODE;
	genSensorMode = WDR_MODE_NONE;
	gu32FullLinesStd = VMAX_720P30_LINEAR;
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
 * callback structure														*
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
	s32Ret = HI_MPI_ISP_SensorRegCallBack(IspDev, JXF23_ID, &stIspRegister);
	if (s32Ret)
	{
		printf("sensor register callback function failed!\n");
		return s32Ret;
	}
	
	stLib.s32Id = 0;
	strncpy(stLib.acLibName, HI_AE_LIB_NAME, sizeof(HI_AE_LIB_NAME));
	cmos_init_ae_exp_function(&stAeRegister.stSnsExp);
	s32Ret = HI_MPI_AE_SensorRegCallBack(IspDev, &stLib, JXF23_ID, &stAeRegister);
	if (s32Ret)
	{
		printf("sensor register callback function to ae lib failed!\n");
		return s32Ret;
	}

	stLib.s32Id = 0;
	strncpy(stLib.acLibName, HI_AWB_LIB_NAME, sizeof(HI_AWB_LIB_NAME));
	cmos_init_awb_exp_function(&stAwbRegister.stSnsExp);
	s32Ret = HI_MPI_AWB_SensorRegCallBack(IspDev, &stLib, JXF23_ID, &stAwbRegister);
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

	s32Ret = HI_MPI_ISP_SensorUnRegCallBack(IspDev, JXF23_ID);
	if (s32Ret)
	{
		printf("sensor unregister callback function failed!\n");
		return s32Ret;
	}
	
	stLib.s32Id = 0;
	strncpy(stLib.acLibName, HI_AE_LIB_NAME, sizeof(HI_AE_LIB_NAME));
	s32Ret = HI_MPI_AE_SensorUnRegCallBack(IspDev, &stLib, JXF23_ID);
	if (s32Ret)
	{
		printf("sensor unregister callback function to ae lib failed!\n");
		return s32Ret;
	}

	stLib.s32Id = 0;
	strncpy(stLib.acLibName, HI_AWB_LIB_NAME, sizeof(HI_AWB_LIB_NAME));
	s32Ret = HI_MPI_AWB_SensorUnRegCallBack(IspDev, &stLib, JXF23_ID);
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

#endif /* __MN34222_CMOS_H_ */
