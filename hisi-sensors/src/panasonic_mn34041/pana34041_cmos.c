#if !defined(__PANA_MN34041_H_)
#define __PANA_MN34041_H_

#include <stdio.h>
#include <string.h>
#include "hi_comm_sns.h"
#include "hi_comm_isp.h"
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

#define PANA34041_ID 34041

/*set Frame End Update Mode 2 with HI_MPI_ISP_SetAEAttr and set this value 1 to avoid flicker */
/*when use Frame End Update Mode 2, the speed of i2c will affect whole system's performance   */
/*increase I2C_DFT_RATE in Hii2c.c to 400000 to increase the speed of i2c                     */
#define CMOS_PANA34041_ISP_WRITE_SENSOR_ENABLE (0)
/****************************************************************************
 * local variables                                                            *
 ****************************************************************************/

HI_U32 gu32FullLinesStd = 1125;
HI_U8 gu8SensorMode = 0;
#if CMOS_PANA34041_ISP_WRITE_SENSOR_ENABLE
ISP_SNS_REGS_INFO_S g_stSnsRegsInfo = {0};
static HI_BOOL gsbRegInit = HI_FALSE;
#endif

static AWB_CCM_S g_stAwbCcm =
{
    5000,
	{	0x022d, 0x8114, 0x8019,
		0x8055, 0x01e4, 0x808f,
		0x0016, 0x8178, 0x0262
	},
    3200,
	{	0x020e, 0x80e7, 0x8027,
		0x809a, 0x01fd, 0x8063,
		0x002a, 0x81ee, 0x02c3
	},
    2600,
	{	0x0207, 0x80d5, 0x8032,
		0x8084, 0x01cb, 0x8047,
		0x003f, 0x82c4, 0x0384
	}
};

static AWB_AGC_TABLE_S g_stAwbAgcTable =
{
    /* bvalid */
    1,

    /* saturation */
    {0x80,0x80,0x6C,0x48,0x44,0x40,0x3C,0x38}
};

static ISP_CMOS_AGC_TABLE_S g_stIspAgcTable =
{
    /* bvalid */
    1,

    /* sharpen_alt_d */
    {0x78,0x70,0x68,0x58,0x44,0x30,0x28,0x28},
        
    /* sharpen_alt_ud */
    {0x68,0x60,0x58,0x48,0x38,0x30,0x28,0x24},
        
    /* snr_thresh */
    {0x20,0x28,0x30,0x38,0x40,0x48,0x54,0x54},
        
    /* demosaic_lum_thresh */
    {0x60,0x60,0x40,0x40,0x40,0x30,0x30,0x30},
        
    /* demosaic_np_offset */
    {0x0,0xa,0x12,0x1a,0x20,0x28,0x30,0x30},
        
    /* ge_strength */
    {0x55,0x55,0x55,0x55,0x55,0x55,0x37,0x37}
};

static ISP_CMOS_NOISE_TABLE_S g_stIspNoiseTable =
{
    /* bvalid */
    1,
    
    /* nosie_profile_weight_lut */
    {0,  0,  0,  0,  0,  0,  11, 15, 17, 19, 20, 21, 22, 22, 23, 24,
    25, 25, 26, 26, 26, 27, 27, 27, 28, 28, 28, 29, 29, 29, 29, 29,
    30, 30, 30, 30, 30, 31, 31, 31, 31, 31, 32, 32, 32, 32, 32, 32,
    32, 33, 33, 33, 33, 33, 33, 33, 33, 33, 34, 34, 34, 34, 34, 34,
    34, 34, 34, 34, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 36, 36,
    36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 37, 37,
    37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 38, 38,
    38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38},

    /* demosaic_weight_lut */
    {0,11,15,17,19,20,21,22,23,23,24,25,25,26,26,26,
    27,27,27,28,28,28,29,29,29,29,29,30,30,30,30,30,
    31,31,31,31,31,32,32,32,32,32,32,32,33,33,33,33,
    33,33,33,33,33,34,34,34,34,34,34,34,34,34,34,35,
    35,35,35,35,35,35,35,35,35,35,35,36,36,36,36,36,
    36,36,36,36,36,36,36,36,36,37,37,37,37,37,37,37,
    37,37,37,37,37,37,37,37,37,38,38,38,38,38,38,38,
    38,38,38,38,38,38,38,38,38,38,38,38,38,38,38,38}
};

static ISP_CMOS_DEMOSAIC_S g_stIspDemosaic =
{
    /* bvalid */
    1,
    
    /*vh_slope*/
    200,

    /*aa_slope*/
    220,

    /*va_slope*/
    200,

    /*uu_slope*/
    190,

    /*sat_slope*/
    93,

    /*ac_slope*/
    0xcf,

    /*vh_thresh*/
    0x60,

    /*aa_thresh*/
    0x64,

    /*va_thresh*/
    0x64,

    /*uu_thresh*/
    0x64,

    /*sat_thresh*/
    0x171,

    /*ac_thresh*/
    0x1b3
};

HI_U32 cmos_get_isp_default(ISP_CMOS_DEFAULT_S *pstDef)
{
    if (HI_NULL == pstDef)
    {
        printf("null pointer when get isp default value!\n");
        return -1;
    }

    memset(pstDef, 0, sizeof(ISP_CMOS_DEFAULT_S));

    pstDef->stComm.u8Rggb           = 0x1;      //1: rggb  
    pstDef->stComm.u8BalanceFe      = 0x1;

    pstDef->stDenoise.u8SinterThresh= 0x8;
    pstDef->stDenoise.u8NoiseProfile= 0x1;      //0: use default profile table; 1: use calibrated profile lut, the setting for nr0 and nr1 must be correct.
    pstDef->stDenoise.u16Nr0        = 0x0;
    pstDef->stDenoise.u16Nr1        = 1528;

    pstDef->stDrc.u8DrcBlack        = 0x00;
    pstDef->stDrc.u8DrcVs           = 0x02;     // variance space
    pstDef->stDrc.u8DrcVi           = 0x08;     // variance intensity
    pstDef->stDrc.u8DrcSm           = 0x80;     // slope max
    pstDef->stDrc.u16DrcWl          = 0x8ff;    // white level

    memcpy(&pstDef->stNoiseTbl, &g_stIspNoiseTable, sizeof(ISP_CMOS_NOISE_TABLE_S));            
    memcpy(&pstDef->stAgcTbl, &g_stIspAgcTable, sizeof(ISP_CMOS_AGC_TABLE_S));
    memcpy(&pstDef->stDemosaic, &g_stIspDemosaic, sizeof(ISP_CMOS_DEMOSAIC_S));

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
        pstBlackLevel->au16BlackLevel[i] = 0x100;
    }

    return 0;    
}

HI_VOID cmos_set_pixel_detect(HI_BOOL bEnable)
{
    if (bEnable) /* setup for ISP pixel calibration mode */
    {
        //set the gain to 0
		sensor_write_register(0x0020,0x0080);
		sensor_write_register(0x0021,0x0080);
		sensor_write_register(0x00A1,0x0400);
		sensor_write_register(0x00A2,0x0002);
		sensor_write_register(0x00A5,0x0005);
    }
    else /* setup for ISP 'normal mode' */
    {
        sensor_write_register(0x00A5,0x0000);
    }

    return;
}

HI_VOID cmos_set_wdr_mode(HI_U8 u8Mode)
{
    switch(u8Mode)
    {
        //sensor mode 0
        case 0:
            gu8SensorMode = 0;
            // TODO:
        break;
        //sensor mode 1
        case 1:
            gu8SensorMode = 1;
             // TODO:
        break;

        default:
            printf("NOT support this mode!\n");
            return;
        break;
    }
    
    return;
}

static HI_S32 cmos_get_ae_default(AE_SENSOR_DEFAULT_S *pstAeSnsDft)
{
    if (HI_NULL == pstAeSnsDft)
    {
        printf("null pointer when get ae default value!\n");
        return -1;
    }

    gu32FullLinesStd = 1125;
    
    pstAeSnsDft->au8HistThresh[0] = 0xd;
    pstAeSnsDft->au8HistThresh[1] = 0x28;
    pstAeSnsDft->au8HistThresh[2] = 0x60;
    pstAeSnsDft->au8HistThresh[3] = 0x80;
    
    pstAeSnsDft->u8AeCompensation = 0x40;
    
    pstAeSnsDft->u32LinesPer500ms = 1125*30/2;
    pstAeSnsDft->u32FullLinesStd = gu32FullLinesStd;
    pstAeSnsDft->u32FlickerFreq = 0;//60*256;//50*256;

    pstAeSnsDft->stIntTimeAccu.enAccuType = AE_ACCURACY_LINEAR;
    pstAeSnsDft->stIntTimeAccu.f32Accuracy = 1;
    pstAeSnsDft->u32MaxIntTime = 1122;
    pstAeSnsDft->u32MinIntTime = 1;    
    pstAeSnsDft->u32MaxIntTimeTarget = 65535;
    pstAeSnsDft->u32MinIntTimeTarget = 1;

    pstAeSnsDft->stAgainAccu.enAccuType = AE_ACCURACY_DB;
    pstAeSnsDft->stAgainAccu.f32Accuracy = 6;    
    pstAeSnsDft->u32MaxAgain = 3;  /* 18db / 6db = 3 */
    pstAeSnsDft->u32MinAgain = 0;
    pstAeSnsDft->u32MaxAgainTarget = 3;
    pstAeSnsDft->u32MinAgainTarget = 0;

    pstAeSnsDft->stDgainAccu.enAccuType = AE_ACCURACY_TABLE;
    pstAeSnsDft->stDgainAccu.f32Accuracy = 0.1;    
    pstAeSnsDft->u32MaxDgain = 4080;  /* 12db / 0.1 = 120 */
    pstAeSnsDft->u32MinDgain = 1024;
    pstAeSnsDft->u32MaxDgainTarget = 4080;
    pstAeSnsDft->u32MinDgainTarget = 1024;

    pstAeSnsDft->u32ISPDgainShift = 8;
    pstAeSnsDft->u32MaxISPDgainTarget = 1 << pstAeSnsDft->u32ISPDgainShift;
    pstAeSnsDft->u32MinISPDgainTarget = 1 << pstAeSnsDft->u32ISPDgainShift;

    return 0;
}

static HI_S32 cmos_get_sensor_max_resolution(ISP_CMOS_SENSOR_MAX_RESOLUTION *pstSensorMaxResolution)
{
    if (HI_NULL == pstSensorMaxResolution)
    {
        printf("null pointer when get sensor max resolution \n");
        return -1;
    }

    memset(pstSensorMaxResolution, 0, sizeof(ISP_CMOS_SENSOR_MAX_RESOLUTION));

    pstSensorMaxResolution->u32MaxWidth  = 1920;
    pstSensorMaxResolution->u32MaxHeight = 1080;

    return 0;
}


/* the function of sensor set fps */
static HI_VOID cmos_fps_set(HI_U8 u8Fps, AE_SENSOR_DEFAULT_S *pstAeSnsDft)
{
    switch(u8Fps)
    {
        case 30:
            //sensor_init_exit(30);
            gu32FullLinesStd = 1125;
            pstAeSnsDft->u32MaxIntTime = 1122;
            pstAeSnsDft->u32LinesPer500ms = 1125 * 30 / 2;
        break;
        
        case 25:
            //sensor_init_exit(25);
            gu32FullLinesStd = 1350;
            pstAeSnsDft->u32MaxIntTime = 1347;
            pstAeSnsDft->u32LinesPer500ms = 1350 * 25 / 2;
        break;
        
        default:
        break;
    }

    pstAeSnsDft->u32FullLinesStd = gu32FullLinesStd;

    return;
}

static HI_VOID cmos_slow_framerate_set(HI_U16 u16FullLines,
    AE_SENSOR_DEFAULT_S *pstAeSnsDft)
{
    pstAeSnsDft->u32MaxIntTime = u16FullLines - 3;
    
    return;
}

static HI_VOID cmos_init_regs_info(HI_VOID)
{
#if CMOS_PANA34041_ISP_WRITE_SENSOR_ENABLE
    HI_S32 i;

    if (HI_FALSE == gsbRegInit)
    {
        g_stSnsRegsInfo.enSnsType = ISP_SNS_SSP_TYPE;
        g_stSnsRegsInfo.u32RegNum = 5;
        for (i=0; i<5; i++)
        {
            g_stSnsRegsInfo.astSspData[i].u32RegAddr = 0x00;
            g_stSnsRegsInfo.astSspData[i].u32DevAddrByteNum = 2;
            g_stSnsRegsInfo.astSspData[i].u32RegAddrByteNum = 0;
            g_stSnsRegsInfo.astSspData[i].u32DataByteNum = 2;
        }
        g_stSnsRegsInfo.astSspData[0].bDelayCfg = HI_FALSE;
        g_stSnsRegsInfo.astSspData[0].u32DevAddr = 0x00A1;
        g_stSnsRegsInfo.astSspData[1].bDelayCfg = HI_FALSE;
        g_stSnsRegsInfo.astSspData[1].u32DevAddr = 0x00A2;
        g_stSnsRegsInfo.astSspData[2].bDelayCfg = HI_FALSE;
        g_stSnsRegsInfo.astSspData[2].u32DevAddr = 0x00A5;
        g_stSnsRegsInfo.astSspData[3].bDelayCfg = HI_TRUE;
        g_stSnsRegsInfo.astSspData[3].u32DevAddr = 0x0020;
        g_stSnsRegsInfo.astSspData[4].bDelayCfg = HI_TRUE;
        g_stSnsRegsInfo.astSspData[4].u32DevAddr = 0x0021;
        g_stSnsRegsInfo.bDelayCfgIspDgain = HI_TRUE;

        gsbRegInit = HI_TRUE;
    }
#endif
    return;
}


/* while isp notify ae to update sensor regs, ae call these funcs. */
static HI_VOID cmos_inttime_update(HI_U32 u32IntTime)
{
    HI_U32 u32Tmp = u32IntTime;
	HI_U32 u32IntFrames = u32Tmp / gu32FullLinesStd;

	u32Tmp = u32Tmp - u32IntFrames * gu32FullLinesStd;
	u32Tmp = gu32FullLinesStd - u32Tmp;
#if CMOS_PANA34041_ISP_WRITE_SENSOR_ENABLE
    cmos_init_regs_info();
    g_stSnsRegsInfo.astSspData[0].u32Data = (u32Tmp & 0xffff);
    g_stSnsRegsInfo.astSspData[1].u32Data = (0x2 + ((u32Tmp&0x10000) >> 16) );
    g_stSnsRegsInfo.astSspData[2].u32Data = u32IntFrames;
#else
	sensor_write_register(0x00A1, (u32Tmp & 0xffff));
	sensor_write_register(0x00A2, (0x2 + ((u32Tmp&0x10000) >> 16) ));  //0x2 is the reserved bits value
	sensor_write_register(0x00A5, u32IntFrames);
#endif
    return;
}

static HI_U32  digital_gain_table[129]=
{ 
 1024,  1035,  1046,  1058,  1069,  1081,  1093,  1104,  1116,  1129,  1141,  1153,  1166,  1178,  1191,  1204, 
 1217,  1230,  1244,  1257,  1271,  1285,  1299,  1313,  1327,  1341,  1356,  1371,  1386,  1401,  1416,  1431, 
 1447,  1462,  1478,  1494,  1511,  1527,  1544,  1560,  1577,  1594,  1612,  1629,  1647,  1665,  1683,  1701, 
 1720,  1738,  1757,  1776,  1795,  1815,  1835,  1855,  1875,  1895,  1916,  1936,  1957,  1979,  2000,  2022, 
 2044,  2066,  2089,  2111,  2134,  2157,  2181,  2204,  2228,  2253,  2277,  2302,  2327,  2352,  2377,  2403, 
 2429,  2456,  2482,  2509,  2537,  2564,  2592,  2620,  2649,  2677,  2706,  2736,  2766,  2796,  2826,  2857, 
 2888,  2919,  2951,  2983,  3015,  3048,  3081,  3114,  3148,  3182,  3217,  3252,  3287,  3323,  3359,  3395, 
 3432,  3470,  3507,  3545,  3584,  3623,  3662,  3702,  3742,  3783,  3824,  3865,  3907,  3950,  3992,  4036, 
 4080
};

static HI_VOID cmos_dgain_calc_table(HI_U32 u32InTimes,AE_SENSOR_GAININFO_S *pstAeSnsGainInfo)
{
    int i;

    if(HI_NULL == pstAeSnsGainInfo)
    {
        printf("null pointer when get ae sensor gain info  value!\n");
        return;
    }
 
    pstAeSnsGainInfo->u32GainDb = 0;
    pstAeSnsGainInfo->u32SnsTimes = 1024;
    if (u32InTimes >= digital_gain_table[128])
    {
         pstAeSnsGainInfo->u32SnsTimes = digital_gain_table[128];
         pstAeSnsGainInfo->u32GainDb = 128;
         return;
    }
    
    for(i = 1; i < 129; i++)
    {
        if(u32InTimes < digital_gain_table[i])
        {
            pstAeSnsGainInfo->u32SnsTimes = digital_gain_table[i - 1];
            pstAeSnsGainInfo->u32GainDb = i - 1;
            break;
        }
    }

    return;

}


static HI_VOID cmos_gains_update(HI_U32 u32Again, HI_U32 u32Dgain)
{
#if CMOS_PANA34041_ISP_WRITE_SENSOR_ENABLE
    cmos_init_regs_info();
    switch (u32Again)
    {
        case 0:
            //r_colgsw = 0dB, r_a_gain = 0dB
            g_stSnsRegsInfo.astSspData[3].u32Data = 0x0080;
            break;
        case 1:
            //r_colgsw = 6dB, r_a_gain = 0dB
            g_stSnsRegsInfo.astSspData[3].u32Data = 0x8080;
            break;
        case 2:
            //r_colgsw = 12dB, r_a_gain = 0dB
            g_stSnsRegsInfo.astSspData[3].u32Data = 0xC080;
            break;
        case 3:
            //r_colgsw = 12dB, r_a_gain = 6dB
            g_stSnsRegsInfo.astSspData[3].u32Data = 0xC0C0;
            break;
        default:
            break;
    }    

    #if 0
    for (i = 0; i < 0x80; i++)
    {
        if (u32Dgain >= gau8DgainFine[i] && u32Dgain <= gau8DgainFine[i+1])
        {
            break;
        }
    }
    u32Dgain = 0x80 + i;
    #endif
    u32Dgain = 0x80 + u32Dgain;

    g_stSnsRegsInfo.astSspData[4].u32Data = u32Dgain;
    HI_MPI_ISP_SnsRegsCfg(&g_stSnsRegsInfo);
#else
    switch (u32Again)
    {
        case 0:
            //r_colgsw = 0dB, r_a_gain = 0dB
            sensor_write_register(0x20, 0x0080);
            break;
        case 1:
            //r_colgsw = 6dB, r_a_gain = 0dB
            sensor_write_register(0x20, 0x8080);
            break;
        case 2:
            //r_colgsw = 12dB, r_a_gain = 0dB
            sensor_write_register(0x20, 0xC080);
            break;
        case 3:
            //r_colgsw = 12dB, r_a_gain = 6dB
            sensor_write_register(0x20, 0xC0C0);
            break;
        default:
            break;
    }    

    u32Dgain = 0x80 + u32Dgain;

    sensor_write_register(0x21, u32Dgain);
#endif
    return;
}

static HI_S32 cmos_get_awb_default(AWB_SENSOR_DEFAULT_S *pstAwbSnsDft)
{
    if (HI_NULL == pstAwbSnsDft)
    {
        printf("null pointer when get awb default value!\n");
        return -1;
    }

    memset(pstAwbSnsDft, 0, sizeof(AWB_SENSOR_DEFAULT_S));

    pstAwbSnsDft->u16WbRefTemp = 5000;

    pstAwbSnsDft->au16GainOffset[0] = 0x1eb;
    pstAwbSnsDft->au16GainOffset[1] = 0x100;
    pstAwbSnsDft->au16GainOffset[2] = 0x100;
    pstAwbSnsDft->au16GainOffset[3] = 0x1c5;

    pstAwbSnsDft->as32WbPara[0] = 49;
    pstAwbSnsDft->as32WbPara[1] = 121;
    pstAwbSnsDft->as32WbPara[2] = -86;
    pstAwbSnsDft->as32WbPara[3] = 185444;
    pstAwbSnsDft->as32WbPara[4] = 128;
    pstAwbSnsDft->as32WbPara[5] = -134952;

    memcpy(&pstAwbSnsDft->stCcm, &g_stAwbCcm, sizeof(AWB_CCM_S));
    memcpy(&pstAwbSnsDft->stAgcTbl, &g_stAwbAgcTable, sizeof(AWB_AGC_TABLE_S));
    
    return 0;
}

HI_VOID sensor_global_init()
{

   gu8SensorMode = 0;
   
}


/****************************************************************************
 * callback structure                                                       *
 ****************************************************************************/
HI_S32 cmos_init_sensor_exp_function(ISP_SENSOR_EXP_FUNC_S *pstSensorExpFunc)
{
    memset(pstSensorExpFunc, 0, sizeof(ISP_SENSOR_EXP_FUNC_S));

    pstSensorExpFunc->pfn_cmos_sensor_init = sensor_init;
    pstSensorExpFunc->pfn_cmos_sensor_global_init = sensor_global_init;
    pstSensorExpFunc->pfn_cmos_get_isp_default = cmos_get_isp_default;
    pstSensorExpFunc->pfn_cmos_get_isp_black_level = cmos_get_isp_black_level;
    pstSensorExpFunc->pfn_cmos_set_pixel_detect = cmos_set_pixel_detect;
    pstSensorExpFunc->pfn_cmos_set_wdr_mode = cmos_set_wdr_mode;
    pstSensorExpFunc->pfn_cmos_get_sensor_max_resolution = cmos_get_sensor_max_resolution;

    return 0;
}

HI_S32 cmos_init_ae_exp_function(AE_SENSOR_EXP_FUNC_S *pstExpFuncs)
{
    memset(pstExpFuncs, 0, sizeof(AE_SENSOR_EXP_FUNC_S));

    pstExpFuncs->pfn_cmos_get_ae_default    = cmos_get_ae_default;
    pstExpFuncs->pfn_cmos_fps_set           = cmos_fps_set;
    pstExpFuncs->pfn_cmos_slow_framerate_set= cmos_slow_framerate_set;    
    pstExpFuncs->pfn_cmos_inttime_update    = cmos_inttime_update;
    pstExpFuncs->pfn_cmos_gains_update      = cmos_gains_update;
    pstExpFuncs->pfn_cmos_dgain_calc_table  = cmos_dgain_calc_table;

    return 0;
}

HI_S32 cmos_init_awb_exp_function(AWB_SENSOR_EXP_FUNC_S *pstExpFuncs)
{
    memset(pstExpFuncs, 0, sizeof(AWB_SENSOR_EXP_FUNC_S));

    pstExpFuncs->pfn_cmos_get_awb_default = cmos_get_awb_default;

    return 0;
}

int sensor_register_callback(void)
{
    HI_S32 s32Ret;
    ALG_LIB_S stLib;
    ISP_SENSOR_REGISTER_S stIspRegister;
    AE_SENSOR_REGISTER_S  stAeRegister;
    AWB_SENSOR_REGISTER_S stAwbRegister;

    cmos_init_sensor_exp_function(&stIspRegister.stSnsExp);
    s32Ret = HI_MPI_ISP_SensorRegCallBack(PANA34041_ID, &stIspRegister);
    if (s32Ret)
    {
        printf("sensor register callback function failed!\n");
        return s32Ret;
    }
    
    stLib.s32Id = 0;
    strcpy(stLib.acLibName, HI_AE_LIB_NAME);
    cmos_init_ae_exp_function(&stAeRegister.stSnsExp);
    s32Ret = HI_MPI_AE_SensorRegCallBack(&stLib, PANA34041_ID, &stAeRegister);
    if (s32Ret)
    {
        printf("sensor register callback function to ae lib failed!\n");
        return s32Ret;
    }

    stLib.s32Id = 0;
    strcpy(stLib.acLibName, HI_AWB_LIB_NAME);
    cmos_init_awb_exp_function(&stAwbRegister.stSnsExp);
    s32Ret = HI_MPI_AWB_SensorRegCallBack(&stLib, PANA34041_ID, &stAwbRegister);
    if (s32Ret)
    {
        printf("sensor register callback function to ae lib failed!\n");
        return s32Ret;
    }
    
    return 0;
}

int sensor_unregister_callback(void)
{
    HI_S32 s32Ret;
    ALG_LIB_S stLib;

    s32Ret = HI_MPI_ISP_SensorUnRegCallBack(PANA34041_ID);
    if (s32Ret)
    {
        printf("sensor unregister callback function failed!\n");
        return s32Ret;
    }
    
    stLib.s32Id = 0;
    strcpy(stLib.acLibName, HI_AE_LIB_NAME);
    s32Ret = HI_MPI_AE_SensorUnRegCallBack(&stLib, PANA34041_ID);
    if (s32Ret)
    {
        printf("sensor unregister callback function to ae lib failed!\n");
        return s32Ret;
    }

    stLib.s32Id = 0;
    strcpy(stLib.acLibName, HI_AWB_LIB_NAME);
    s32Ret = HI_MPI_AWB_SensorUnRegCallBack(&stLib, PANA34041_ID);
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

#endif // __IMX104_CMOS_H_
