#if !defined(__AR0237_CMOS_H_)
#define __AR0237_CMOS_H_

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

#define AR0237_ID 237

/****************************************************************************
 * local variables                                                            *
 ****************************************************************************/

static HI_U16 g_au16InitWBGain[ISP_MAX_DEV_NUM][3] = {{0}};
static HI_U16 g_au16SampleRgain[ISP_MAX_DEV_NUM] = {0};
static HI_U16 g_au16SampleBgain[ISP_MAX_DEV_NUM] = {0};

extern const unsigned int sensor_i2c_addr;
extern unsigned int sensor_addr_byte;
extern unsigned int sensor_data_byte;

#define FULL_LINES_MAX  (0xFFFF)

#define ANALOG_GAIN      	(0x3060)
#define DIGITAL_GAIN     	(0x305E)
#define HCG_ADDR 		(0x3100)
#define VMAX_ADDR         (0x300A)
#define EXPOSURE_TIME_L  (0x3012)
#define EXPOSURE_TIME_S  (0x3212)
#define Y_OUTPUT 		(1080)

#define SENSOR_2M_1080p30_MODE  (1) 

#define INCREASE_LINES (1) 
#define FRAME_LINES_2M_LIN  (0x454+INCREASE_LINES)
#define FRAME_LINES_2M_WDR  (0x4C4+INCREASE_LINES)


// Max integration time for HDR mode: T1 max = min ( 70* 16, FLL*16/17), when ratio=16;
#define LONG_EXP_SHT_CLIP     (FRAME_LINES_2M_LIN*16/17)

static HI_U32 au32WDRIntTime[4] = {0};
static ISP_FSWDR_MODE_E genFSWDRMode = ISP_FSWDR_NORMAL_MODE;
static HI_U32 gu32MaxTimeGetCnt = 0;

HI_U8 gu8SensorImageMode = SENSOR_2M_1080p30_MODE;
WDR_MODE_E genSensorMode = WDR_MODE_NONE;

static HI_U32 gu32FullLinesStd = FRAME_LINES_2M_LIN;
static HI_U32 gu32FullLines = FRAME_LINES_2M_LIN; 
static HI_U32 gu32PreFullLines = FRAME_LINES_2M_LIN;
static HI_BOOL bInit = HI_FALSE;
HI_BOOL bSensorInit = HI_FALSE; 
static HI_FLOAT gf32Fps = 30;

ISP_SNS_REGS_INFO_S g_stSnsRegsInfo = {0};
ISP_SNS_REGS_INFO_S g_stPreSnsRegsInfo = {0};

#define PATHLEN_MAX 256
#define CMOS_CFG_INI "AR0237_cfg.ini"
static char pcName[PATHLEN_MAX] = "configs/AR0237_cfg.ini";


/* AE default parameter and function */

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
	pstAeSnsDft->u32FlickerFreq = 0;
	pstAeSnsDft->u32FullLinesMax = FULL_LINES_MAX;

	pstAeSnsDft->stIntTimeAccu.enAccuType = AE_ACCURACY_LINEAR;
	pstAeSnsDft->stIntTimeAccu.f32Accuracy = 1;
	pstAeSnsDft->stIntTimeAccu.f32Offset = 0;

	pstAeSnsDft->stAgainAccu.enAccuType = AE_ACCURACY_TABLE;
	pstAeSnsDft->stAgainAccu.f32Accuracy = 0.0078125;

	pstAeSnsDft->stDgainAccu.enAccuType = AE_ACCURACY_LINEAR;
	pstAeSnsDft->stDgainAccu.f32Accuracy = 0.0078125;

	pstAeSnsDft->u32ISPDgainShift = 8;
	pstAeSnsDft->u32MinISPDgainTarget = 1 << pstAeSnsDft->u32ISPDgainShift;
	pstAeSnsDft->u32MaxISPDgainTarget = 4 << pstAeSnsDft->u32ISPDgainShift;

	pstAeSnsDft->enMaxIrisFNO = ISP_IRIS_F_NO_1_0;
	pstAeSnsDft->enMinIrisFNO = ISP_IRIS_F_NO_32_0;

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

			pstAeSnsDft->u8AeCompensation = 0x38;
			pstAeSnsDft->u32InitExposure = 96181;

			pstAeSnsDft->u32MaxIntTime = gu32FullLinesStd - 2;
			pstAeSnsDft->u32MinIntTime = 2;
			pstAeSnsDft->u32MaxIntTimeTarget = 65535;
			pstAeSnsDft->u32MinIntTimeTarget = 2;

			pstAeSnsDft->u32MaxAgain = 29030;
			pstAeSnsDft->u32MinAgain = 1024;
			pstAeSnsDft->u32MaxAgainTarget = pstAeSnsDft->u32MaxAgain;
			pstAeSnsDft->u32MinAgainTarget = pstAeSnsDft->u32MinAgain;

			pstAeSnsDft->u32MaxDgain = 1024;
			pstAeSnsDft->u32MinDgain = 128;
			pstAeSnsDft->u32MaxDgainTarget = pstAeSnsDft->u32MaxDgain;
			pstAeSnsDft->u32MinDgainTarget = pstAeSnsDft->u32MinDgain;
			pstAeSnsDft->u32MaxISPDgainTarget = 2 << pstAeSnsDft->u32ISPDgainShift;
			break;
		}
		case WDR_MODE_2To1_LINE:
		{
			pstAeSnsDft->au8HistThresh[0] = 0xC;
			pstAeSnsDft->au8HistThresh[1] = 0x18;
			pstAeSnsDft->au8HistThresh[2] = 0x60;
			pstAeSnsDft->au8HistThresh[3] = 0x80;
			pstAeSnsDft->u8AeCompensation = 40;
			pstAeSnsDft->u32InitExposure = 27228;

			pstAeSnsDft->u32MaxIntTime = gu32FullLinesStd - 2;
			pstAeSnsDft->u32MinIntTime = 8;
			pstAeSnsDft->u32MaxIntTimeTarget = 65535;
			pstAeSnsDft->u32MinIntTimeTarget = pstAeSnsDft->u32MinIntTime;

			pstAeSnsDft->u32MaxAgain = 16589;
			pstAeSnsDft->u32MinAgain = 1024;
			pstAeSnsDft->u32MaxAgainTarget = pstAeSnsDft->u32MaxAgain;
			pstAeSnsDft->u32MinAgainTarget = pstAeSnsDft->u32MinAgain;

			pstAeSnsDft->u32MaxDgain = 1024;
			pstAeSnsDft->u32MinDgain = 128;
			pstAeSnsDft->u32MaxDgainTarget = pstAeSnsDft->u32MaxDgain;
			pstAeSnsDft->u32MinDgainTarget = pstAeSnsDft->u32MinDgain;
			pstAeSnsDft->u32MaxISPDgainTarget = 1 << pstAeSnsDft->u32ISPDgainShift;

			pstAeSnsDft->u32LFMaxShortTime = 350;
			pstAeSnsDft->u32MaxIntTimeStep = 3000;
			pstAeSnsDft->u32LFMinExposure = 580000;

			pstAeSnsDft->u16ManRatioEnable = HI_FALSE;
			pstAeSnsDft->u32Ratio = 0x400;
			pstAeSnsDft->u32InitExposure = 17099;
			break;
		}
		default:
			break;
	}
	return 0;
}


/* the function of sensor set fps */
static HI_VOID cmos_fps_set(HI_FLOAT f32Fps, AE_SENSOR_DEFAULT_S *pstAeSnsDft)
{
    if(f32Fps<=30)
    {
        gf32Fps = f32Fps;
    }
	if (WDR_MODE_2To1_LINE == genSensorMode)
	{
		if (30 == f32Fps)
		{
			gu32FullLinesStd = FRAME_LINES_2M_WDR;
		}
		else if ((f32Fps < 30) && (f32Fps >= 0.5))
		{
			gu32FullLinesStd = (FRAME_LINES_2M_WDR * 30) / f32Fps;
		}
		else
		{
			printf("Not support Fps: %f\n", f32Fps);
			return;
		}
	}
	else
	{
		if ((f32Fps <= 30) && (f32Fps >= 0.5))
		{
			gu32FullLinesStd = (FRAME_LINES_2M_LIN * 30) / f32Fps;
		}
		else
		{
			printf("Not support Fps: %f\n", f32Fps);
			return;
		}
	}

	gu32FullLinesStd = gu32FullLinesStd > FULL_LINES_MAX ? FULL_LINES_MAX : gu32FullLinesStd;
	g_stSnsRegsInfo.astI2cData[4].u32Data = gu32FullLinesStd;

	pstAeSnsDft->f32Fps = f32Fps;
	pstAeSnsDft->u32LinesPer500ms = gu32FullLinesStd * f32Fps / 2;
	pstAeSnsDft->u32FullLinesStd = gu32FullLinesStd;
	pstAeSnsDft->u32MaxIntTime = gu32FullLinesStd - 4;
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
	g_stSnsRegsInfo.astI2cData[4].u32Data = gu32FullLines;
	pstAeSnsDft->u32MaxIntTime = gu32FullLines - 2;

	return;
}

/* while isp notify ae to update sensor regs, ae call these funcs. */
static HI_VOID cmos_inttime_update(HI_U32 u32IntTime)
{
    static HI_BOOL bFirst = HI_TRUE;

    if(WDR_MODE_2To1_LINE == genSensorMode)
    {
        if (bFirst) /* short exposure */
        {
            au32WDRIntTime[0] = u32IntTime;
            g_stSnsRegsInfo.astI2cData[0].u32Data = u32IntTime;
            bFirst = HI_FALSE;
        }
        else /* long exposure */
        {
            au32WDRIntTime[1] = u32IntTime;
            g_stSnsRegsInfo.astI2cData[5].u32Data = u32IntTime;
            bFirst = HI_TRUE;
        }
    }
    else
    {
        g_stSnsRegsInfo.astI2cData[5].u32Data = u32IntTime;
        bFirst = HI_TRUE;
    }
    return;
}

static  HI_U32  again_table[51] = 
{
    //here 1024 means x1.52
    1024,1075,1132,1195,1265,1344,1434,1536,1654,1792,1814,1873,1935,2002,2074,
    2150,2233,2322,2419,2524,2639,2765,2903,3056,3226,3415,3629,3871,4147,4466,
    4838,5278,5806,6451,7258,7741,8294,8932,9677,10557,11612,12902,14515,15483,16589,
    17865,19354,21113,23224,25805,29030
};

static HI_VOID cmos_again_calc_table(HI_U32 *pu32AgainLin, HI_U32 *pu32AgainDb)
{
    int i;
    HI_U32 u32InTimes;

    if(!pu32AgainDb || !pu32AgainLin)
    {
        printf("null pointer when get ae sensor gain info  value!\n");
        return;
    }

    u32InTimes = *pu32AgainLin;

    if(u32InTimes >= again_table[50])
    {
        *pu32AgainLin = again_table[50];
        *pu32AgainDb = 50;
        return;
    }

    for(i = 1; i < 51; i++)
    {
        if(u32InTimes < again_table[i])
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
    static HI_BOOL bHCG = HI_FALSE;

    if(u32Again < 10)
    {
        if(HI_TRUE == bHCG)
        {
            g_stSnsRegsInfo.astI2cData[3].u32Data = 0x0000;
            bHCG = HI_FALSE;
        }

        if(u32Again < 5)
        {
            g_stSnsRegsInfo.astI2cData[1].u32Data = u32Again + 0xb;
        }
        else
        {
            //0x10 + (again - 5) * 2
            g_stSnsRegsInfo.astI2cData[1].u32Data = u32Again * 2 + 0x6;
        }
    }
    else
    {
        if(HI_FALSE == bHCG)
        {
            g_stSnsRegsInfo.astI2cData[3].u32Data = 0x0004;
            bHCG = HI_TRUE;
        }
        if(u32Again < 26)
        {
            g_stSnsRegsInfo.astI2cData[1].u32Data = u32Again - 10;
        }
        else
        {
            //0x10 + (again - 26) * 2
            g_stSnsRegsInfo.astI2cData[1].u32Data = u32Again * 2 - 36;
        }
    }
    g_stSnsRegsInfo.astI2cData[2].u32Data = u32Dgain;
    return;
}

/* Only used in WDR_MODE_2To1_LINE mode */

/* [T2 max = FLL - y_output 16] The y_output is the output rows calculated by
    (Y_ADDR_END - Y_ADDR_START +1).*/

static HI_VOID cmos_get_inttime_max(HI_U32 u32Ratio, HI_U32 u32MinTime, HI_U32 *pu32IntTimeMax, HI_U32 *pu32LFMaxIntTime)
{
	HI_U32 u32IntTimeMaxTmp0 = 0;
	HI_U32 u32IntTimeMaxTmp  = 0;
	HI_U32 u32T2Max = 0;

	if(HI_NULL == pu32IntTimeMax)
	{
		printf("null pointer when get ae sensor IntTimeMax value!\n");
		return;
	}

	if((WDR_MODE_2To1_LINE == genSensorMode))
	{
		/* Short + Long < 1V - 2; 
			Ratio = Long * 0x40 / Short */
		if(ISP_FSWDR_LONG_FRAME_MODE == genFSWDRMode)
		{
			u32T2Max = gu32PreFullLines - 16 - au32WDRIntTime[0];
			*pu32IntTimeMax = gu32FullLines - 16;
			*pu32IntTimeMax = (u32T2Max < *pu32IntTimeMax) ? u32T2Max : *pu32IntTimeMax;
		}
		else if(ISP_FSWDR_AUTO_LONG_FRAME_MODE == genFSWDRMode)
		{
			if(au32WDRIntTime[0] == u32MinTime && u32Ratio == 0x40)
			{
				u32T2Max = gu32PreFullLines - 16 - au32WDRIntTime[0];
				*pu32IntTimeMax = gu32FullLines - 16;
				*pu32IntTimeMax = (u32T2Max < *pu32IntTimeMax) ? u32T2Max : *pu32IntTimeMax;
			}
			else
			{
				u32IntTimeMaxTmp0 = ((gu32PreFullLines - 30 - au32WDRIntTime[0]) * 0x40)  / DIV_0_TO_1(u32Ratio);
				u32IntTimeMaxTmp = ((gu32FullLines - 30) * 0x40)  / DIV_0_TO_1(u32Ratio + 0x40);
				u32IntTimeMaxTmp = (u32IntTimeMaxTmp0 < u32IntTimeMaxTmp) ? u32IntTimeMaxTmp0 : u32IntTimeMaxTmp;
				u32IntTimeMaxTmp0 = gu32FullLines - 30 - (gu32FullLines - 16 - Y_OUTPUT);
				u32IntTimeMaxTmp0 = (u32IntTimeMaxTmp0 * 0x40) / DIV_0_TO_1(u32Ratio);
				*pu32IntTimeMax = (u32IntTimeMaxTmp0 > u32IntTimeMaxTmp) ? u32IntTimeMaxTmp0 : u32IntTimeMaxTmp;
				*pu32LFMaxIntTime = gu32FullLines - 16 - Y_OUTPUT;
			}
		}
		else
		{
			u32T2Max = gu32FullLines - 16 - Y_OUTPUT;
			*pu32IntTimeMax = (gu32FullLines - 30) * 0x40 / DIV_0_TO_1(u32Ratio + 0x40);
			if ( *pu32IntTimeMax > u32T2Max )
			{
				*pu32IntTimeMax = u32T2Max;
			}
		}
	}
	else
	{
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
    pstExpFuncs->pfn_cmos_dgain_calc_table  = NULL;
    pstExpFuncs->pfn_cmos_get_inttime_max   = cmos_get_inttime_max;
    pstExpFuncs->pfn_cmos_ae_fswdr_attr_set = cmos_ae_fswdr_attr_set;

    return 0;
}

static AWB_CCM_S g_stAwbCcm =
{
   	4850,
   	{
        0x0194,  0x8058,  0x803C,
		0x8054,  0x0179,  0x8025,
		0x800A,  0x8086,  0x0190
   	},
   	3850,
   	{
        0x01C8,  0x8092,  0x8036,
		0x8065,  0x0164,  0x0001,
		0x800F,  0x809A,  0x01A9
   	}, 
   
   	2450,
   	{
        0x01A1,  0x803C,  0x8065,
		0x807C,  0x0172,  0x000A,
		0x8050,  0x811A,  0x026A
   	}                 
};

static AWB_CCM_S g_stAwbCcmFsWdr =
{  
   	5100,
	{ 
        0x0122,  0x8001,  0x8021,

		0x8037,  0x015a,  0x8023,
		0x8011,  0x8017,  0x0128
	},
3850,
{
        0x010B,  0x8017,  0x000C,
		0x801E,  0x0149,  0x802B,
		0x0013,  0x8055,  0x0142

},
   	2450,
	{ 
        0x01B6,  0x8057,  0x805F,
		0x806D,  0x01CD,  0x8060,
		0x0000,  0x8125,  0x0225
	} 

};  

static AWB_AGC_TABLE_S g_stAwbAgcTable =
{
    /* bvalid */
    1,
    /* saturation */ 
    {128,128,126,120,115,105,100,90,80,70,56,56,56,56,56,56}
};

static AWB_AGC_TABLE_S g_stAwbAgcTableFSWDR =
{
    /* bvalid */
    1,

    /* saturation */
    {128,128,118,108,98,88,80,80,70,70,56,56,56,56,56,56}
};

/* Rgain and Bgain of the golden sample */
#define GOLDEN_RGAIN 0   
#define GOLDEN_BGAIN 0  

static HI_S32 cmos_get_awb_default(AWB_SENSOR_DEFAULT_S *pstAwbSnsDft)
{
    ISP_DEV IspDev = 0;

    if (HI_NULL == pstAwbSnsDft)
    {
        printf("null pointer when get awb default value!\n");
        return -1;
    }

    memset(pstAwbSnsDft, 0, sizeof(AWB_SENSOR_DEFAULT_S));

    pstAwbSnsDft->u16WbRefTemp = 4962;

    pstAwbSnsDft->au16GainOffset[0] = 392;
    pstAwbSnsDft->au16GainOffset[1] = 256;
    pstAwbSnsDft->au16GainOffset[2] = 256;
    pstAwbSnsDft->au16GainOffset[3] = 453;

    pstAwbSnsDft->as32WbPara[0] = 40;
    pstAwbSnsDft->as32WbPara[1] = 108;
    pstAwbSnsDft->as32WbPara[2] = -108;
    pstAwbSnsDft->as32WbPara[3] = 172722;
    pstAwbSnsDft->as32WbPara[4] = 128;
    pstAwbSnsDft->as32WbPara[5] = -122283;

    pstAwbSnsDft->u16GoldenRgain = GOLDEN_RGAIN;
    pstAwbSnsDft->u16GoldenBgain = GOLDEN_BGAIN;
    
	switch (genSensorMode)
    {
        default:
        case WDR_MODE_NONE:
    		memcpy(&pstAwbSnsDft->stCcm, &g_stAwbCcm, sizeof(AWB_CCM_S));
    		memcpy(&pstAwbSnsDft->stAgcTbl, &g_stAwbAgcTable, sizeof(AWB_AGC_TABLE_S));
		break;
		
	    case WDR_MODE_2To1_LINE:
		    memcpy(&pstAwbSnsDft->stCcm, &g_stAwbCcmFsWdr, sizeof(AWB_CCM_S));
    		memcpy(&pstAwbSnsDft->stAgcTbl, &g_stAwbAgcTableFSWDR, sizeof(AWB_AGC_TABLE_S));
		break;
	}
	
    pstAwbSnsDft->u16SampleRgain = g_au16SampleRgain[IspDev];
    pstAwbSnsDft->u16SampleBgain = g_au16SampleBgain[IspDev];
    pstAwbSnsDft->u16InitRgain = g_au16InitWBGain[IspDev][0];
    pstAwbSnsDft->u16InitGgain = g_au16InitWBGain[IspDev][1];
    pstAwbSnsDft->u16InitBgain = g_au16InitWBGain[IspDev][2];

    pstAwbSnsDft->u8AWBRunInterval = 4;

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
    {64,64,128,200,256,512,1022,1022,1022,1022,1022,1022,1022,1022,1022,1022},
    /*au16EdgeSmoothSlope*/
    {32,32,32,48,48,48,0,0,0,0,0,0,0,0,0,0},
    //{4,16,32,32,32,32,0,0,0,0,0,0,0,0,0,0},
    /*au16AntiAliasThr*/
    {1022,1022,1022,1022,1022,1022,1022,1022,1022,1022,1022,1022,1022,1022,1022,1022},
    /*au16AntiAliasSlope*/
	{256,256,256,256,256,256,0,0,0,0,0,0,0,0,0,0},
    /*NrCoarseStr*/
    {128, 128, 128, 128, 128, 32, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16},
	/*NoiseSuppressStr*/
	//{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{6, 8, 8, 10, 40, 40, 40, 100, 400, 400, 400, 400, 400, 400, 400, 400},
	/*DetailEnhanceStr*/
	{8, 6, 4, 4, 4, 3, 2, 1, 0, 0, 0, 0, 0, 0, 0, 0},
    /*SharpenLumaStr*/
    {256, 256, 256, 256, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200},
    /*ColorNoiseCtrlThr*/
    {0, 0, 100, 150, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300}
};


static ISP_CMOS_YUV_SHARPEN_S g_stIspYuvSharpen = 
{
	/* bvalid */
    1,

    /* 100,  200,    400,     800,    1600,    3200,    6400,    12800,    25600,   51200,  102400,  204800,   409600,   819200,   1638400,  3276800 */
    /* au16SharpenUd */
	{20, 20, 18, 16, 14, 12, 8, 6, 4, 2, 0, 0, 0, 0, 0, 0},

    /* au8SharpenD */
    {80, 70, 70, 70, 70, 60, 80, 120, 140, 160, 180, 200, 200, 200, 200, 200},
	 
	/*au8TextureThd*/
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},

    /* au8SharpenEdge */
    {80, 70, 70, 60, 50, 60, 60, 60, 70, 80, 100, 120, 150, 180, 180, 200},

    /* au8EdgeThd */
    {80, 80, 80, 80, 80, 80, 80, 90, 100, 110, 120, 130, 150, 170, 180, 190},

    /* au8OverShoot */
    {150, 150, 150, 150, 130, 90, 60, 40, 30, 20, 10, 10, 10, 10, 10, 10},

    /* au8UnderShoot */
    {200, 200, 200, 200, 180, 120, 80, 60, 50, 40, 20, 15, 15, 15, 15, 15},

	/*au8shootSupSt*/
	{33, 33, 33, 33, 16, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	 
	/* au8DetailCtrl */
    {128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128},

	 /* au8RGain */
    {31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31},

	/* au8BGain */
    {31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31},

	/* au8SkinGain */
    {255, 255,  200,  170,  150,  135,  145,  155,  165,  175,  185,  195,  205,  205,  205,  205},
	
	/*  au8EdgeFiltStr*/
	{0,	  0,	   0,	   0,	   0,		0,     0,		0,		0,		0,		0,			0,			0,		0,		0,		0}, 
	
	/*  au8JagCtrl*/
	{252 ,   252 ,    252,     252,    252,	       252,      252,      252,	   252,	     252,	   252,	  252,	  252,	  252,	  252,    252}, 
	
	/*  au8NoiseLumaCtrl*/
	{0,	  0,	   0,	   0,	   0,		0,     0,		0,		0,		0,		0,			0,			0,		0,		0,		0}, 
};


static ISP_CMOS_DRC_S g_stIspDRC = 
{
    /*bEnable*/
    1,   
    /*u8SpatialVar*/
    3,    
	/*u8RangeVar*/             
	3,
	/*u8Asymmetry8*/
    2,
	/*u8SecondPole; */
    180,
	/*u8Stretch*/
    54,
	/*u8Compress*/ 
    180,
	/*u8PDStrength*/ 
    0,
	/*u8LocalMixingBrigtht*/
    20,
    /*u8LocalMixingDark*/
    76,
    /*ColorCorrectionLut[33];*/
    {1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024},
    /*bOpType 0:auto  1:manual*/
    0,
    /*u8ManualStrength*/
    128,
    /*u8AutoStrength*/
    128,
};


static ISP_CMOS_CA_S g_stIspCA = 
{
    /*CA enable*/
    1,
    /*Y Gain table*/
    {
         403,  421,  439,  457,  475,  493,  512,  530,
         549,  568,  586,  605,  624,  642,  661,  679,
         698,  716,  734,  753,  771,  788,  806,  824,
         841,  858,  875,  891,  908,  924,  939,  955,
         970,  985, 1001, 1016, 1033, 1049, 1065, 1082,
        
        1099, 1115, 1132, 1148, 1164, 1180, 1196, 1211,
        1226, 1240, 1254, 1267, 1279, 1291, 1302, 1313,
        1322, 1330, 1338, 1344, 1349, 1353, 1356, 1357,
        1358, 1357, 1356, 1355, 1353, 1351, 1349, 1346,
        1343, 1340, 1336, 1332, 1327, 1323, 1318, 1313,
        1307, 1302, 1296, 1289, 1283, 1276, 1270, 1263,
        1255, 1248, 1240, 1233, 1225, 1217, 1209, 1201,
        1192, 1184, 1175, 1167, 1158, 1149, 1141, 1132,
        1123, 1114, 1105, 1096, 1087, 1079, 1070, 1061,
        1052, 1044, 1035, 1027, 1018, 1010, 1002,  993,
         986,  978,  970,  962,  955,  948,  941,  934

	},
	/*ISO table*/
    {1300,1300,1250,1200,1150,1100,1050,1000,950,900,900,800,800,800,800,800}
};
static ISP_CMOS_CA_S g_stIspWDRCA = 
{
	1,
 	{
          0,   29,   59,   89,  120,  150,  181,  211,
        241,  271,  301,  330,  359,  388,  415,  442,
        468,  493,  519,  544,  568,  592,  616,  640,
        662,  684,  706,  727,  747,  766,  784,  801,
        818,  833,  848,  862,  875,  887,  899,  910,
        920,  929,  938,  946,  953,  960,  966,  971,
        976,  980,  983,  985,  987,  988,  988,  987,
        986,  984,  982,  979,  976,  972,  969,  965,
        960,  955,  950,  943,  937,  929,  921,  913,
        904,  896,  887,  878,  869,  860,  851,  842,
        834,  826,  818,  809,  801,  792,  784,  775,
        767,  758,  750,  742,  733,  725,  717,  710,
        702,  694,  686,  678,  670,  662,  655,  647,
        640,  633,  626,  619,  613,  607,  601,  596,
        592,  588,  584,  581,  578,  576,  573,  571,
        569,  567,  565,  563,  561,  559,  557,  554,
        

 	},
    {1024, 1024, 1024, 1024, 1024, 1024, 1000, 1000,
     950,  900,  900,  800,  800,  800,  800,  800,}
};

/***BAYER NR**/
static ISP_CMOS_BAYERNR_S g_stIspBayerNr =
{
    14,     //Calibration Lut Num
    /*************Calibration LUT Table*************/
    {
        {100.000000f, 0.029009f},
        {200.000000f, 0.047543f},
        {400.000000f, 0.088356f}, 
        {800.000000f, 0.177481f}, 
    	{1600.000000f, 0.339525f}, 
    	{3200.000000f, 0.678188f}, 
    	{6400.000000f, 1.392002f}, 
    	{13400.000000f, 3.072300f}, 
    	{26800.000000f, 6.686838f}, 
    	{53800.000000f, 13.862728f}, 
    	{108500.000000f, 16.115625f}, 
   		{112500.000000f, 16.729803f}, 
    	{161700.000000f, 17.042057f}, 
    	{181200.000000f, 17.576839f}, 
	},
	/*********************************************/
	{140, 110, 110, 140},		  //CoraseStr
	{ 20, 20, 20, 20, 20, 20, 20, 30, 30, 30, 35, 35, 35, 40, 40, 50},    //lutFineStr
	{
	  	{1,1,1,1,2,2,2,3,3,3,3,3,3,3,3,3},   //ChromaStrR
      {0,0,0,0,1,1,1,2,2,2,2,2,2,0,0,0},   //ChromaStrGr
	  {0,0,0,0,1,1,1,2,2,2,2,2,2,0,0,0},   //ChromaStrGb
	  	{1,1,1,1,2,2,2,3,3,3,3,3,3,3,3,3}    //ChromaStrB 
	},
	{ 600, 600, 600, 600, 650, 750, 750, 700, 700, 600, 600, 600, 600, 600, 600, 600},	   //lutCoringWeight
	{600, 600, 600, 600, 650, 650, 650, 650, 700, 700, 700, 700, 700, 700, 700, 700, 
           800, 800, 800, 850, 850, 850, 900, 900, 900, 950, 950, 950, 1000, 1000, 1000, 1000, 1000}
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


static ISP_CMOS_FCR_S g_stIspFcr =
{
	/*For FCR*/
	1,    /*bEnable*/				
	{8,8,8,8,7,7,7,  6,  6,6,5,4,3,2,1,0}, /*au8Strength[ISP_AUTO_ISO_STRENGTH_NUM]*/
	{ 24, 24, 24, 24, 20, 20, 20,   16,   14, 12, 10, 8, 6, 4, 2, 0}, /*au8Threhold[ISP_AUTO_ISO_STRENGTH_NUM]*/
	{150,150,150,150,150,150,150,  150,  150,150,150,150,150,150,150,150}    /*au16Offset[ISP_AUTO_ISO_STRENGTH_NUM]*/	
};

static ISP_CMOS_DPC_S g_stCmosDpc = 
{
	//0,/*IR_channel*/
	//0,/*IR_position*/
	{220,220,220,230,240,240, 240, 240, 250, 250,255,255,255,255,255,255},/*au16Strength[16]*/
	{0,  0,  0,  0,  0,  10,   30, 50,  60,  80, 100,180,185,180,200, 200},/*au16BlendRatio[16]*/
};	
static ISP_CMOS_DPC_S g_stCmosDpcWDR = 
{
	{200,200,200,200,220,230,240,240,240,240,240,240,240,245,245,245},/*au16Strength[16]*/
	{0,0, 0, 0, 0, 0, 0, 0, 0, 0, 50, 50, 50, 50, 50, 50},/*au16BlendRatio[16]*/
};	

static ISP_CMOS_DEMOSAIC_S g_stIspDemosaicWDR =
{
	1,//bEnable
    /*au16EdgeSmoothThr*/
	{256,256,256,256,512,714,1022,1022,1022,1022,1022,1022,1022,1022,1022,1022},
	/*au16EdgeSmoothSlope*/
	{32,32,32,32,32,32,0,0,0,0,0,0,0,0,0,0},
	/*au16AntiAliasThr*/
	{1022,1022,1022,1022,1022,1022,1022,1022,1022,1022,1022,1022,1022,1022,1022,1022},
	/*au16AntiAliasSlope*/
	{256,256,256,256,256,256,0,0,0,0,0,0,0,0,0,0},

    /*NrCoarseStr*/
    {128, 128, 120, 64, 64, 32, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16},
	/*NoiseSuppressStr*/
	{20, 20, 20, 20, 20, 10, 10, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	/*DetailEnhanceStr*/
	{6, 4, 4, 4, 2, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    /*SharpenLumaStr*/
    {256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256},
    /*ColorNoiseCtrlThr*/
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
};

static ISP_CMOS_YUV_SHARPEN_S g_stIspYuvWdrSharpen = 
{
    /* bvalid */
    1,

    /* 100,  200,    400,     800,    1600,    3200,    6400,    12800,    25600,   51200,  102400,  204800,   409600,   819200,   1638400,  3276800 */
     
	/* au8SharpenUd */
	{24, 23, 21, 18, 15, 13, 13, 13, 6, 6, 3, 10, 3, 2, 1, 1},
	
	/* au8SharpenD */
    {84, 82, 78, 70, 62, 55, 60, 60, 40, 38, 35, 30, 170, 180, 200, 200},
	 
	/*au8TextureThd*/
	{0, 0, 0, 0, 0,	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},

    /* au8SharpenEdge */
    {90, 100, 115, 130, 130, 140, 180, 100, 115, 120, 120, 110, 150, 180, 180, 200},

    /* au8EdgeThd */
    {80,    80,    80,    95,    100,    100,     110,    110,    115,   115,  115,   120,   120,    140,    180,   180},	 
	 
	/* au8OverShoot */
    {88, 83, 78, 70, 60, 50, 60, 84, 75, 70, 60, 10, 10, 10, 10, 10},

    /* au8UnderShoot */
    {90, 85, 80, 72, 62, 55, 60, 94, 80, 60, 70, 10, 15, 15, 15, 15},
	 
	/*au8shootSupSt*/
	{30, 20, 10, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},

    /* au8DetailCtrl */
    {128, 120, 118, 115, 110, 105, 100, 75, 70, 65, 55, 80, 128, 128, 128, 128},
    
    /* au8RGain */
    {31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31},

	/* au8BGain */
    {31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31},

	/* au8SkinGain */
    {255, 255,  255,  255,  245,  235,  225,  210,  205,  205,  205,  205,  205,  205,  205,  205},
};  

static ISP_CMOS_BAYERNR_S g_stIspBayerNrWDR =
{
	14,     //Calibration Lut Num
	/*************Calibration LUT Table*************/
	{
        {100.000000f, 0.029009f}, 
    	{200.000000f, 0.047543f}, 
    	{400.000000f, 0.088356f}, 
    	{800.000000f, 0.177481f}, 
    	{1600.000000f, 0.339525f}, 
    	{3200.000000f, 0.678188f}, 
    	{6400.000000f, 1.392002f}, 
    	{13400.000000f, 3.072300f}, 
    	{26800.000000f, 6.686838f}, 
    	{53800.000000f, 13.862728f}, 
    	{108500.000000f, 16.115625f}, 
   		{112500.000000f, 16.729803f}, 
    	{161700.000000f, 17.042057f}, 
    	{181200.000000f, 17.576839f}, 
	},
	{140, 110, 110, 140},		  //CoraseStr
	{0x70, 0x70, 0x70, 0x60, 0x40, 0x30, 0x15, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10},     //lutFineStr
	{
	  	{1,1,1,1,2,2,2,3,3,3,3,3,3,3,3,3},   //ChromaStrR
      	{0,0,0,0,1,1,1,2,2,2,2,2,2,2,2,2},   //ChromaStrGr
	  	{0,0,0,0,1,1,1,2,2,2,2,2,2,2,2,2},   //ChromaStrGb
	  	{1,1,1,1,2,2,2,3,3,3,3,3,3,3,3,3}    //ChromaStrB 
	},
	{ 400, 400, 500, 600, 650, 750, 750, 700, 700, 600, 600, 600, 600, 600, 600, 600},	   //lutCoringWeight
	{600, 700, 800, 900, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, \
           1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000}
};

static ISP_CMOS_DRC_S g_stIspDRCWDR = 
{
    /*bEnable*/
    1,
    /*u8SpatialVar*/
    3,    
	/*u8RangeVar*/             
	3,
	/*u8Asymmetry8*/
    5,
	/*u8SecondPole; */
    150,
	/*u8Stretch*/
    58,
	/*u8Compress*/
    102,
	/*u8PDStrength*/
    0,
	/*u8LocalMixingBrigtht*/
    50,
    /*u8LocalMixingDark*/
    25,
    /*ColorCorrectionLut[33];*/
   	{1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024},
    /*bOpType 0:auto  1:manual*/
    1,
    /*u8ManualStrength*/
    128,
    /*u8AutoStrength*/
    128,
}; 

static ISP_CMOS_GE_S g_stIspWdrGe =
{
	/*For GE*/
	0,    /*bEnable*/			
	9,    /*u8Slope*/	
	9,    /*u8SensiSlope*/	
	300, /*u16SensiThr*/	
	{300,300,300,300,310,310,310,  310,  320,320,320,320,330,330,330,330}, /*au16Threshold[ISP_AUTO_ISO_STRENGTH_NUM]*/
	{128, 128, 128, 128, 129, 129, 129,   129,   130, 130, 130, 130, 131, 131, 131, 131}, /*au16Strength[ISP_AUTO_ISO_STRENGTH_NUM]*/
	{1024,1024,1024,2048,2048,2048,2048,  2048,  2048,2048,2048,2048,2048,2048,2048,2048}    /*au16NpOffset[ISP_AUTO_ISO_STRENGTH_NUM]*/	
};

static ISP_CMOS_FCR_S g_stIspWdrFcr =
{
	/*For FCR*/
	1,    /*bEnable*/				
	{20,16,12,8,7,7,7,  6,  6,6,5,4,3,2,1,0}, /*au8Strength[ISP_AUTO_ISO_STRENGTH_NUM]*/
	{50, 40, 30, 24, 20, 20, 20,   16,   14, 12, 10, 8, 6, 4, 2, 0}, /*au8Threhold[ISP_AUTO_ISO_STRENGTH_NUM]*/
	{150,150,150,150,150,150,150,  150,  150,150,150,150,150,150,150,150}    /*au16Offset[ISP_AUTO_ISO_STRENGTH_NUM]*/	
};

static ISP_CMOS_GAMMA_S g_stIspGamma =
{
    /* bvalid */
    1,
    
	/* Normal mode */
    {  	
    #if 0
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
    #else
    0,70,142,216,290,365,440,514,588,661,733,803,871,937,999,1059,1116,1168,1219,1266,1311,1354,1395,1435,1472,1509,1544,
    1578,1611,1644,1677,1709,1742,1773,1804,1834,1863,1891,1918,1944,1970,1995,2019,2043,2067,2090,2112,2135,2158,2178,2202,
    2222,2246,2266,2282,2300,2324,2344,2360,2372,2390,2406,2422,2438,2458,2478,2494,2510,2526,2546,2562,2582,2598,2614,2630,
    2648,2660,2670,2682,2698,2710,2724,2736,2752,2764,2780,2792,2808,2820,2836,2848,2864,2876,2888,2896,2908,2920,2928,2940,
    2948,2960,2972,2984,2992,3004,3014,3028,3036,3048,3056,3068,3080,3088,3100,3110,3120,3128,3140,3148,3160,3168,3174,3182,
    3190,3202,3210,3218,3228,3240,3256,3266,3276,3288,3300,3306,3318,3326,3334,3342,3350,3360,3370,3378,3386,3394,3398,3406,
    3414,3422,3426,3436,3444,3454,3466,3476,3486,3498,3502,3510,3518,3526,3530,3538,3546,3554,3558,3564,3570,3574,3582,3590,
    3598,3604,3610,3618,3628,3634,3640,3644,3652,3656,3664,3670,3678,3688,3696,3700,3708,3712,3716,3722,3730,3736,3740,3748,
    3752,3756,3760,3766,3774,3778,3786,3790,3800,3808,3812,3816,3824,3830,3832,3842,3846,3850,3854,3858,3862,3864,3870,3874,
    3878,3882,3888,3894,3900,3908,3912,3918,3924,3928,3934,3940,3946,3952,3958,3966,3974,3978,3982,3986,3990,3994,4002,4006,
    4010,4018,4022,4032,4038,4046,4050,4056,4062,4072,4076,4084,4090,4095
    #endif
    }
};



static ISP_CMOS_GAMMA_S g_stIspGammaFSWDR =
{
    /* bvalid */
    1,
    
    {
    #if 0
        0,16,32,47,63,79,94,109,125,140,155,170,186,201,216,232,247,263,278,294,310,325,341,358,374,390,407,424,441,458,476,494,512,530,549,568,587,606,625,645,665,685,705,725,746,767,787,808,829,849,870,890,910,
        931,951,972,992,1013,1034,1055,1077,1098,1120,1143,1165,1188,1211,1235,1260,1284,1309,1334,1359,1384,1409,1434,1459,1483,1507,1531,1554,1577,1600,1623,1645,1668,1690,1713,1735,1756,1778,1798,1819,1839,
        1859,1878,1896,1914,1931,1948,1964,1979,1995,2009,2024,2038,2052,2066,2079,2093,2106,2120,2133,2146,2159,2172,2185,2198,2210,2222,2234,2246,2258,2269,2281,2292,2303,2313,2324,2334,2344,2354,2363,2372,
        2381,2389,2398,2406,2415,2423,2431,2440,2449,2458,2467,2477,2486,2496,2505,2515,2524,2534,2544,2554,2564,2574,2584,2594,2605,2616,2627,2638,2650,2661,2673,2684,2696,2708,2720,2733,2745,2758,2771,2784,
        2798,2811,2825,2839,2853,2868,2883,2898,2913,2929,2945,2960,2976,2992,3008,3024,3040,3056,3071,3087,3102,3117,3133,3149,3164,3180,3196,3212,3228,3244,3260,3276,3292,3308,3324,3340,3357,3373,3389,3405,
        3422,3438,3454,3470,3486,3503,3519,3535,3551,3567,3583,3599,3615,3631,3647,3663,3679,3695,3711,3727,3743,3759,3775,3791,3807,3823,3839,3855,3871,3887,3903,3919,3935,3951,3967,3983,3999,4015,4031,4047,4063,4079,4095
    #else
    0,20,41,62,83,104,125,146,167,188,209,230,251,272,293,314,335,356,377,398,419,440,461,482,503,524,544,565,586,607,628,649,670,690,711,
    732,753,773,794,815,836,856,877,898,919,939,960,981,1002,1022,1043,1063,1084,1105,1125,1146,1166,1187,1207,1228,1248,1269,1289,1309,1330,
    1350,1370,1390,1411,1431,1452,1472,1493,1513,1533,1554,1574,1594,1615,1635,1655,1675,1695,1715,1735,1755,1775,1795,1814,1834,1853,1873,
    1892,1911,1930,1949,1968,1986,2004,2023,2041,2059,2077,2095,2112,2130,2148,2165,2182,2200,2217,2234,2251,2268,2285,2302,2319,2336,2353,
    2369,2386,2403,2420,2437,2453,2470,2487,2504,2521,2537,2554,2571,2589,2606,2623,2640,2657,2675,2692,2709,2726,2743,2760,2777,2794,2811,
    2828,2845,2861,2878,2894,2910,2926,2942,2957,2973,2988,3003,3018,3032,3046,3059,3073,3087,3100,3113,3126,3139,3151,3163,3176,3188,3200,
    3211,3223,3234,3246,3257,3268,3280,3291,3302,3313,3324,3335,3346,3357,3368,3379,3391,3402,3413,3425,3436,3447,3459,3470,3481,3493,3504,
    3515,3526,3537,3548,3560,3571,3582,3593,3604,3615,3626,3637,3647,3658,3669,3680,3691,3701,3712,3723,3733,3744,3754,3765,3776,3786,3796,
    3807,3817,3827,3837,3847,3858,3868,3878,3888,3897,3907,3917,3927,3937,3947,3957,3966,3976,3986,3996,4006,4016,4025,4035,4045,4055,4065,4075,4085,4095

    #endif
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
        case WDR_MODE_NONE:
        {
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
        }
        
        case WDR_MODE_2To1_LINE:
		{
			memcpy(&pstDef->stDemosaic, &g_stIspDemosaicWDR, sizeof(ISP_CMOS_DEMOSAIC_S));           		
			memcpy(&pstDef->stYuvSharpen, &g_stIspYuvWdrSharpen, sizeof(ISP_CMOS_YUV_SHARPEN_S));            		
			memcpy(&pstDef->stDrc, &g_stIspDRCWDR, sizeof(ISP_CMOS_DRC_S));            		
			memcpy(&pstDef->stGamma, &g_stIspGammaFSWDR, sizeof(ISP_CMOS_GAMMA_S));		
			memcpy(&pstDef->stBayerNr, &g_stIspBayerNrWDR, sizeof(ISP_CMOS_BAYERNR_S));          			      		
			memcpy(&pstDef->stGe, &g_stIspWdrGe, sizeof(ISP_CMOS_GE_S));					           		
			memcpy(&pstDef->stFcr, &g_stIspWdrFcr, sizeof(ISP_CMOS_FCR_S));							
			memcpy(&pstDef->stDpc,       &g_stCmosDpcWDR,       sizeof(ISP_CMOS_DPC_S));	
			memcpy(&pstDef->stCa,        &g_stIspWDRCA,sizeof(ISP_CMOS_CA_S));
			break;
        }
			
        default:
            break;
    }

    pstDef->stSensorMaxResolution.u32MaxWidth  = 1920;
    pstDef->stSensorMaxResolution.u32MaxHeight = 1080;

    return 0;
}

HI_U32 cmos_get_isp_black_level(ISP_CMOS_BLACK_LEVEL_S *pstBlackLevel)
{
    HI_S32  i;
    HI_U16 u16Bl;
    
    if(HI_NULL == pstBlackLevel)
    {
        printf("null pointer when get isp black level value!\n");
        return -1;
    }

    /* Don't need to update black level when iso change */
    pstBlackLevel->bUpdate = HI_FALSE;

    u16Bl = (WDR_MODE_NONE == genSensorMode)? 0xA8 : 0xA6;

    for (i=0; i<4; i++)
    {
        pstBlackLevel->au16BlackLevel[i] = u16Bl;
    }

    return 0;    
}

HI_VOID cmos_set_pixel_detect(HI_BOOL bEnable)
{
    HI_U32 u32FullLines_5Fps = (FRAME_LINES_2M_LIN * 30) / 5;
    HI_U32 u32MaxExpTime_5Fps = u32FullLines_5Fps - 2;
    
    if(WDR_MODE_2To1_LINE == genSensorMode)
    {
        return;
    }

    u32MaxExpTime_5Fps = 4; 
    
    if(bEnable) /* setup for ISP pixel calibration mode */
    {
        sensor_write_register(VMAX_ADDR, u32FullLines_5Fps); /* 5fps */
        sensor_write_register(EXPOSURE_TIME_L, u32MaxExpTime_5Fps); /* max exposure time */
        sensor_write_register(ANALOG_GAIN, 0x0);      /* AG, Context A */
        sensor_write_register(DIGITAL_GAIN, 0x0080);  /* DG, Context A */
    }
    else /* setup for ISP 'normal mode' */
    {
        gu32FullLinesStd = (gu32FullLinesStd > 0xFFFF) ? 0xFFFF : gu32FullLinesStd;
        gu32FullLines = gu32FullLinesStd;
        sensor_write_register(VMAX_ADDR, gu32FullLinesStd);    /* 30fps */
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
	    {
            genSensorMode = WDR_MODE_NONE;
	     	gu32FullLinesStd = FRAME_LINES_2M_LIN;
	     	g_stSnsRegsInfo.astI2cData[4].u32Data = (gu32FullLinesStd*30)/DIV_0_TO_1(gf32Fps);
			printf("Line mode\n");
        	break;
        }

        case WDR_MODE_2To1_LINE:
		{
			genSensorMode = WDR_MODE_2To1_LINE;
	     	gu32FullLinesStd = FRAME_LINES_2M_WDR ;
	     	gu32FullLines =  FRAME_LINES_2M_WDR  ;
	     	memset(au32WDRIntTime, 0, sizeof(au32WDRIntTime));
	     	g_stSnsRegsInfo.astI2cData[4].u32Data = (gu32FullLinesStd*30)/DIV_0_TO_1(gf32Fps);
	     	printf("2to1 line WDR mode\n");
        	break;
        }

        default:
		{
            printf("NOT support this mode!\n");
            return;
        	break;
        }
    }
    gu32FullLines = gu32FullLinesStd;
   
    return;
}    


static HI_S32 cmos_set_image_mode(ISP_CMOS_SENSOR_IMAGE_MODE_S *pstSensorImageMode)
{
    HI_U8 u8SensorImageMode = gu8SensorImageMode;
    
    bInit = HI_FALSE;    
        
    if(HI_NULL == pstSensorImageMode )
    {
        printf("null pointer when set image mode\n");
        return -1;
    }

    if((pstSensorImageMode->u16Width <= 1920)&&(pstSensorImageMode->u16Height <= 1080))
    {
        if (pstSensorImageMode->f32Fps <= 30)
        {
            u8SensorImageMode = SENSOR_2M_1080p30_MODE;
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
    if(u8SensorImageMode == gu8SensorImageMode)
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

    if(HI_FALSE == bInit)
    {
        g_stSnsRegsInfo.enSnsType = ISP_SNS_I2C_TYPE;
        g_stSnsRegsInfo.u8Cfg2ValidDelayMax = 3;
        g_stSnsRegsInfo.u32RegNum = 6;
        for(i=0; i < g_stSnsRegsInfo.u32RegNum; i++)
        {
            g_stSnsRegsInfo.astI2cData[i].bUpdate = HI_TRUE;
            g_stSnsRegsInfo.astI2cData[i].u8DevAddr = sensor_i2c_addr;
            g_stSnsRegsInfo.astI2cData[i].u32AddrByteNum = sensor_addr_byte;
            g_stSnsRegsInfo.astI2cData[i].u32DataByteNum = sensor_data_byte;
        }
        g_stSnsRegsInfo.astI2cData[0].u8DelayFrmNum = 1;
        g_stSnsRegsInfo.astI2cData[0].u32RegAddr = EXPOSURE_TIME_S;	
        g_stSnsRegsInfo.astI2cData[5].u8DelayFrmNum = 1;
        g_stSnsRegsInfo.astI2cData[5].u32RegAddr = EXPOSURE_TIME_L;	
		
        g_stSnsRegsInfo.astI2cData[1].u8DelayFrmNum = 1;
        g_stSnsRegsInfo.astI2cData[1].u32RegAddr = ANALOG_GAIN;
        g_stSnsRegsInfo.astI2cData[2].u8DelayFrmNum = 1;
        g_stSnsRegsInfo.astI2cData[2].u32RegAddr = DIGITAL_GAIN;
		
        //related registers in DCG mode
        g_stSnsRegsInfo.astI2cData[3].u8DelayFrmNum = 1;
        g_stSnsRegsInfo.astI2cData[3].u32RegAddr = HCG_ADDR;
		
        g_stSnsRegsInfo.astI2cData[4].u8DelayFrmNum = 0;
        g_stSnsRegsInfo.astI2cData[4].u32RegAddr = VMAX_ADDR;
      
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
    
    if(HI_NULL == pstSnsRegsInfo)
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

HI_VOID sensor_global_init(HI_VOID)
{   
    gu8SensorImageMode = SENSOR_2M_1080p30_MODE;
    genSensorMode = WDR_MODE_NONE;       
    gu32FullLinesStd = FRAME_LINES_2M_LIN;
    gu32FullLines = FRAME_LINES_2M_LIN; 
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
    s32Ret = HI_MPI_ISP_SensorRegCallBack(IspDev, AR0237_ID, &stIspRegister);
    if(s32Ret)
    {
        printf("sensor register callback function failed!\n");
        return s32Ret;
    }
    
    stLib.s32Id = 0;
    strncpy(stLib.acLibName, HI_AE_LIB_NAME, sizeof(HI_AE_LIB_NAME));
    cmos_init_ae_exp_function(&stAeRegister.stSnsExp);
    s32Ret = HI_MPI_AE_SensorRegCallBack(IspDev, &stLib, AR0237_ID, &stAeRegister);
    if(s32Ret)
    {
        printf("sensor register callback function to ae lib failed!\n");
        return s32Ret;
    }

    stLib.s32Id = 0;
    strncpy(stLib.acLibName, HI_AWB_LIB_NAME, sizeof(HI_AWB_LIB_NAME));
    cmos_init_awb_exp_function(&stAwbRegister.stSnsExp);
    s32Ret = HI_MPI_AWB_SensorRegCallBack(IspDev, &stLib, AR0237_ID, &stAwbRegister);
    if(s32Ret)
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

    s32Ret = HI_MPI_ISP_SensorUnRegCallBack(IspDev, AR0237_ID);
    if (s32Ret)
    {
    	printf("sensor unregister callback function failed!\n");
        return s32Ret;
    }

    stLib.s32Id = 0;
    strncpy(stLib.acLibName, HI_AE_LIB_NAME, sizeof(HI_AE_LIB_NAME));
    s32Ret = HI_MPI_AE_SensorUnRegCallBack(IspDev, &stLib, AR0237_ID);
    if(s32Ret)
    {
    	printf("sensor unregister callback function to ae lib failed!\n");
       	return s32Ret;
    }

    stLib.s32Id = 0;
    strncpy(stLib.acLibName, HI_AWB_LIB_NAME, sizeof(HI_AWB_LIB_NAME));
    s32Ret = HI_MPI_AWB_SensorUnRegCallBack(IspDev, &stLib, AR0237_ID);
    if(s32Ret)
    {
    	printf("sensor unregister callback function to awb lib failed!\n");
        return s32Ret;
    }

    return 0;
}

int sensor_set_init(ISP_INIT_ATTR_S *pstInitAttr)
{
    ISP_DEV IspDev = 0;

    g_au16InitWBGain[IspDev][0] = pstInitAttr->u16WBRgain;
    g_au16InitWBGain[IspDev][1] = pstInitAttr->u16WBGgain;
    g_au16InitWBGain[IspDev][2] = pstInitAttr->u16WBBgain;
    g_au16SampleRgain[IspDev] = pstInitAttr->u16SampleRgain;
    g_au16SampleBgain[IspDev] = pstInitAttr->u16SampleBgain;
    
    return 0;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif 
