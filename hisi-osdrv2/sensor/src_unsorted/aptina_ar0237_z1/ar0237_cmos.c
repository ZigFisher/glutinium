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

			pstAeSnsDft->u32MaxDgain = 2046;
			pstAeSnsDft->u32MinDgain = 128;
			pstAeSnsDft->u32MaxDgainTarget = pstAeSnsDft->u32MaxDgain;
			pstAeSnsDft->u32MinDgainTarget = pstAeSnsDft->u32MinDgain;
			pstAeSnsDft->u32MaxISPDgainTarget = 1 << pstAeSnsDft->u32ISPDgainShift;
			break;
		}
		case WDR_MODE_2To1_LINE:
		{
			pstAeSnsDft->au8HistThresh[0] = 0xC;
			pstAeSnsDft->au8HistThresh[1] = 0x18;
			pstAeSnsDft->au8HistThresh[2] = 0x60;
			pstAeSnsDft->au8HistThresh[3] = 0x80;
			pstAeSnsDft->u8AeCompensation = 0x34;
			pstAeSnsDft->u32InitExposure = 27228;

			pstAeSnsDft->u32MaxIntTime = gu32FullLinesStd - 2;
			pstAeSnsDft->u32MinIntTime = 8;
			pstAeSnsDft->u32MaxIntTimeTarget = 65535;
			pstAeSnsDft->u32MinIntTimeTarget = pstAeSnsDft->u32MinIntTime;

			pstAeSnsDft->u32MaxAgain = 15483;
			pstAeSnsDft->u32MinAgain = 1024;
			pstAeSnsDft->u32MaxAgainTarget = pstAeSnsDft->u32MaxAgain;
			pstAeSnsDft->u32MinAgainTarget = pstAeSnsDft->u32MinAgain;

			pstAeSnsDft->u32MaxDgain = 128;
			pstAeSnsDft->u32MinDgain = 128;
			pstAeSnsDft->u32MaxDgainTarget = pstAeSnsDft->u32MaxDgain;
			pstAeSnsDft->u32MinDgainTarget = pstAeSnsDft->u32MinDgain;
			pstAeSnsDft->u32MaxISPDgainTarget = 2 << pstAeSnsDft->u32ISPDgainShift;

			pstAeSnsDft->u32LFMaxShortTime = 350;
			pstAeSnsDft->u32MaxIntTimeStep = 3000;
			pstAeSnsDft->u32LFMinExposure = 580000;

			pstAeSnsDft->u16ManRatioEnable = HI_FALSE;
			pstAeSnsDft->u32Ratio = 0x400;
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
			*pu32IntTimeMax = (gu32FullLines - 30) * 0x40 / (u32Ratio + 0x40);
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
		0x8064,  0x0179,  0x8015,
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
	4950,
	{ 
		0x0194,  0x8058,  0x803C,
		0x8064,  0x0179,  0x8015,
		0x800A,  0x8086,  0x0190,
	},

	3708,
	{
		0x01C8,  0x8092,  0x8036,
		0x8065,  0x0164,  0x0001,
		0x800F,  0x809A,  0x01A9,
	},

	2604,
	{ 
		0x00DE, 0x004A, 0x8028,
		0x8045, 0x0131, 0x0014,
		0x802E, 0x802E, 0x015C,
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
    {0x80,0x78,0x6e,0x64,0x5E,0x58,0x50,0x48,0x40,0x38,0x38,0x38,0x38,0x38,0x38,0x38}
};

static HI_S32 cmos_get_awb_default(AWB_SENSOR_DEFAULT_S *pstAwbSnsDft)
{
    if(HI_NULL == pstAwbSnsDft)
    {
        printf("null pointer when get awb default value!\n");
        return -1;
    }

    memset(pstAwbSnsDft, 0, sizeof(AWB_SENSOR_DEFAULT_S));

    pstAwbSnsDft->u16WbRefTemp = 4962;

    pstAwbSnsDft->au16GainOffset[0] = 0x14A;
    pstAwbSnsDft->au16GainOffset[1] = 0x100;
    pstAwbSnsDft->au16GainOffset[2] = 0x100;
    pstAwbSnsDft->au16GainOffset[3] = 0x1BA;

    pstAwbSnsDft->as32WbPara[0] = 50;
    pstAwbSnsDft->as32WbPara[1] = 108;
    pstAwbSnsDft->as32WbPara[2] = -98;
    pstAwbSnsDft->as32WbPara[3] = 199762;
    pstAwbSnsDft->as32WbPara[4] = 128;
    pstAwbSnsDft->as32WbPara[5] = -146394;

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


static ISP_CMOS_CA_S g_stIspCA = 
{
    /*CA enable*/
    1,
    /*Y Gain table*/
    {36 ,81  ,111 ,136 ,158 ,182 ,207 ,228 ,259 ,290 ,317 ,345 ,369 ,396 ,420 ,444 ,
	468 ,492 ,515 ,534 ,556 ,574 ,597 ,614 ,632 ,648 ,666 ,681 ,697 ,709 ,723 ,734 ,
	748 ,758 ,771 ,780 ,788 ,800 ,808 ,815 ,822 ,829 ,837 ,841 ,848 ,854 ,858 ,864 ,
	868 ,871 ,878 ,881 ,885 ,890 ,893 ,897 ,900 ,903 ,906 ,909 ,912 ,915 ,918 ,921 ,
	924 ,926 ,929 ,931 ,934 ,936 ,938 ,941 ,943 ,945 ,947 ,949 ,951 ,952 ,954 ,956 ,
	958 ,961 ,962 ,964 ,966 ,968 ,969 ,970 ,971 ,973 ,974 ,976 ,977 ,979 ,980 ,981 ,
	983 ,984 ,985 ,986 ,988 ,989 ,990 ,991 ,992 ,993 ,995 ,996 ,997 ,998 ,999 ,1000,
	1001,1004,1005,1006,1007,1009,1010,1011,1012,1014,1016,1017,1019,1020,1022,1024},
	/*ISO table*/
    {1300,1300,1250,1200,1150,1100,1050,1000,950,900,900,800,800,800,800,800}
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
	{0x64, 0x55, 0x4B, 0x3a, 0x34, 0x32, 0x28, 0x19, 0xa, 0xa, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10},     //lutFineStr
	{
	  	{1,1,1,1,2,2,2,3,3,3,3,3,3,3,3,3},   //ChromaStrR
      	{0,0,0,0,1,1,1,2,2,2,2,2,2,2,2,2},   //ChromaStrGr
	  	{0,0,0,0,1,1,1,2,2,2,2,2,2,2,2,2},   //ChromaStrGb
	  	{1,1,1,1,2,2,2,3,3,3,3,3,3,3,3,3}    //ChromaStrB 
	},
	{80, 80, 80, 90, 110, 160, 170, 180, 185, 200, 270, 300, 320, 320, 320, 320},     //lutCoringWeight
	{600, 600, 600, 600, 650, 650, 650, 650, 700, 700, 700, 700, 700, 700, 700, 700, \
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
	{0,0,152,200,200,200,220,220,220,220,152,152,152,152,152,152},/*au16Strength[16]*/
	{0,0, 0, 0, 0, 0, 0, 0, 0, 0, 50, 50, 50, 50, 50, 50},/*au16BlendRatio[16]*/
};	

static ISP_CMOS_DEMOSAIC_S g_stIspDemosaicWDR =
{
	1,//bEnable
    /*au16EdgeSmoothThr*/
	{16,16,16,16,16,16,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023},
	/*au16EdgeSmoothSlope*/
	{32,32,32,32,32,32,0,0,0,0,0,0,0,0,0,0},
	/*au16AntiAliasThr*/
	{53,53,53,53,53,53,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023},
	/*au16AntiAliasSlope*/
	{256,256,256,256,256,256,0,0,0,0,0,0,0,0,0,0},

    /*NrCoarseStr*/
    {8, 8, 8, 4, 4, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
	/*NoiseSuppressStr*/
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	/*SharpenStr*/
	{4, 4, 4, 4, 2, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    /*SharpenLumaStr*/
    {0+256, 0+256, 0+256, 0+256, 0+256, 0+256, 0+256, 0+256, 0+256, 0+256, 0+256, 0+256, 0+256, 0+256, 0+256, 0+256},
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
	{60, 50, 40, 30, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20},     //lutCoringWeight
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
    2,
	/*u8SecondPole; */
    170,
	/*u8Stretch*/
    50,
	/*u8Compress*/
    155,
	/*u8PDStrength*/
    20,
	/*u8LocalMixingBrigtht*/
    25,
    /*u8LocalMixingDark*/
    40,
    /*ColorCorrectionLut[33];*/
   	{1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024},
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



static ISP_CMOS_GAMMA_S g_stIspGammaFSWDR =
{
    /* bvalid */
    1,
    
    {
        0,19,39,59,78,98,118,138,157,177,197,216,235,255,274,292,311,329,347,365,383,400,418,435,452,469,
		486,503,521,538,555,573,591,609,627,645,663,681,699,717,735,754,772,791,809,828,847,867,886,906,
		925,945,965,984,1004,1024,1045,1065,1086,1107,1129,1150,1173,1195,1218,1242,1266,1290,1315,1341,
		1367,1393,1419,1445,1472,1498,1525,1551,1577,1603,1628,1656,1683,1708,1732,1756,1780,1804,1829,
		1854,1877,1901,1926,1952,1979,2003,2024,2042,2062,2084,2106,2128,2147,2168,2191,2214,2233,2256,
		2278,2296,2314,2335,2352,2373,2391,2412,2431,2451,2472,2492,2513,2531,2547,2566,2581,2601,2616,
		2632,2652,2668,2688,2705,2721,2742,2759,2779,2796,2812,2826,2842,2857,2872,2888,2903,2920,2934,
		2951,2967,2983,3000,3015,3033,3048,3065,3080,3091,3105,3118,3130,3145,3156,3171,3184,3197,3213,
		3224,3240,3252,3267,3281,3295,3310,3323,3335,3347,3361,3372,3383,3397,3409,3421,3432,3447,3459,
		3470,3482,3497,3509,3521,3534,3548,3560,3572,3580,3592,3602,3613,3625,3633,3646,3657,3667,3679,
		3688,3701,3709,3719,3727,3736,3745,3754,3764,3773,3781,3791,3798,3806,3816,3823,3833,3840,3847,
		3858,3865,3872,3879,3888,3897,3904,3911,3919,3926,3933,3940,3948,3955,3962,3970,3973,3981,3988,
		3996,4003,4011,4018,4026,4032,4037,4045,4053,4057,4064,4072,4076,4084,4088,4095
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
    memcpy(&pstDef->stCa,        &g_stIspCA,sizeof(ISP_CMOS_CA_S));

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
			memcpy(&pstDef->stDpc,       &g_stCmosDpc,       sizeof(ISP_CMOS_DPC_S));	
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
			printf("Line mode\n");
        	break;
        }

        case WDR_MODE_2To1_LINE:
		{
			genSensorMode = WDR_MODE_2To1_LINE;
	     	gu32FullLinesStd = FRAME_LINES_2M_WDR * 2;
	     	gu32FullLines =  FRAME_LINES_2M_WDR * 2;
	     	memset(au32WDRIntTime, 0, sizeof(au32WDRIntTime));
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

HI_VOID sensor_global_init()
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

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif 
