#if !defined(__SONY_ICX692_H_)
#define __SONY_ICX692_H_

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

#define ICX692_ID 692

/*set Frame End Update Mode 2 with HI_MPI_ISP_SetAEAttr and set this value 1 to avoid flicker */
/*when use Frame End Update Mode 2, the speed of i2c will affect whole system's performance   */
/*increase I2C_DFT_RATE in Hii2c.c to 400000 to increase the speed of i2c                     */
#define CMOS_ICX692_ISP_WRITE_SENSOR_ENABLE (0)
/****************************************************************************
 * local variables                                                            *
 ****************************************************************************/

HI_U32 gu32FullLinesStd = 748;
HI_U8 gu8SensorMode = 0;

#if CMOS_ICX692_ISP_WRITE_SENSOR_ENABLE
ISP_SNS_REGS_INFO_S g_stSnsRegsInfo = {0};
static HI_BOOL gsbRegInit = HI_FALSE;
#endif

static AWB_CCM_S g_stAwbCcm =
{
    5000,
	{	0x1e5,  0x80ba, 0x802b,
		0x8024, 0x0183, 0x805e,
		0x0021, 0x80ac, 0x018a
	},
	3200,
    {
        0x0241, 0x80c5, 0x807c,
        0x802c, 0x016f, 0x8043,
        0x003d, 0x80ee, 0x01b0
    },
    2600,
    {
        0x0245, 0x80ce, 0x8077,
        0x802c, 0x0150, 0x8024,
        0x54  , 0x820a, 0x02b6
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
    {0x50,0x45,0x40,0x38,0x34,0x30,0x28,0x28},
        
    /* sharpen_alt_ud */
    {0x3b,0x38,0x34,0x30,0x2b,0x28,0x24,0x20},
        
    /* snr_thresh */
    {0x23,0x2c,0x34,0x3e,0x46,0x4e,0x54,0x54},
        
    /* demosaic_lum_thresh */
    {0x60,0x60,0x80,0x80,0x80,0x80,0x80,0x80},
        
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

    pstDef->stDenoise.u8SinterThresh= 0x20;
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
        pstBlackLevel->au16BlackLevel[i] = 0xf;
    }

    return 0;    
}

HI_VOID cmos_set_pixel_detect(HI_BOOL bEnable)
{
    if (bEnable) /* setup for ISP pixel calibration mode */
    {
        sensor_write_register(0xC001, 0x4);
        sensor_write_register(0xC002, 0x0);
        sensor_write_register(0x407e, 0x1194);
    }
    else /* setup for ISP 'normal mode' */
    {
        sensor_write_register(0x407e,0x2EA);
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

    gu32FullLinesStd = 748;
    
    pstAeSnsDft->au8HistThresh[0] = 0xd;
    pstAeSnsDft->au8HistThresh[1] = 0x28;
    pstAeSnsDft->au8HistThresh[2] = 0x60;
    pstAeSnsDft->au8HistThresh[3] = 0x80;
    
    pstAeSnsDft->u8AeCompensation = 0x40;
    
    pstAeSnsDft->u32LinesPer500ms = 748*30/2;
    pstAeSnsDft->u32FullLinesStd = gu32FullLinesStd;
    pstAeSnsDft->u32FlickerFreq = 0;//60*256;//50*256;

    pstAeSnsDft->stIntTimeAccu.enAccuType = AE_ACCURACY_LINEAR;
    pstAeSnsDft->stIntTimeAccu.f32Accuracy = 1;
    pstAeSnsDft->u32MaxIntTime = 746;
    pstAeSnsDft->u32MinIntTime = 1;    
    pstAeSnsDft->u32MaxIntTimeTarget = 65535;
    pstAeSnsDft->u32MinIntTimeTarget = 1;

    pstAeSnsDft->stAgainAccu.enAccuType = AE_ACCURACY_DB;
    pstAeSnsDft->stAgainAccu.f32Accuracy = 3;    
    pstAeSnsDft->u32MaxAgain = 6;  /* 18db / 3db = 6 */
    pstAeSnsDft->u32MinAgain = 4;
    pstAeSnsDft->u32MaxAgainTarget = 6;
    pstAeSnsDft->u32MinAgainTarget = 4;

    pstAeSnsDft->stDgainAccu.enAccuType = AE_ACCURACY_TABLE;
    pstAeSnsDft->stDgainAccu.f32Accuracy = 0.035;    
    pstAeSnsDft->u32MaxDgain = 62921;  /* 36db / 0.035db = 1028 */
    pstAeSnsDft->u32MinDgain = 1024;
    pstAeSnsDft->u32MaxDgainTarget = 62921;
    pstAeSnsDft->u32MinDgainTarget = 1024;

    pstAeSnsDft->u32ISPDgainShift = 8;
    pstAeSnsDft->u32MaxISPDgainTarget = 4 << pstAeSnsDft->u32ISPDgainShift;
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

    pstSensorMaxResolution->u32MaxWidth  = 1280;
    pstSensorMaxResolution->u32MaxHeight = 720;

    return 0;
}


/* the function of sensor set fps */
static HI_VOID cmos_fps_set(HI_U8 u8Fps, AE_SENSOR_DEFAULT_S *pstAeSnsDft)
{
    switch(u8Fps)
    {
        case 30:
            gu32FullLinesStd = 750;
            pstAeSnsDft->u32MaxIntTime = 748;
            pstAeSnsDft->u32LinesPer500ms = 750 * 30 / 2;
		   sensor_write_register(0x407e, 746);
        break;        
        case 25:
            gu32FullLinesStd = 900;
            pstAeSnsDft->u32MaxIntTime = 898;
            pstAeSnsDft->u32LinesPer500ms = 900 * 25 / 2;
			sensor_write_register(0x407e, 896);
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

    sensor_write_register(0x407e, u16FullLines - 4);
    
    return;
}

static HI_VOID cmos_init_regs_info(HI_VOID)
{
#if CMOS_ICX692_ISP_WRITE_SENSOR_ENABLE
    HI_S32 i;

    if (HI_FALSE == gsbRegInit)
    {
        g_stSnsRegsInfo.enSnsType = ISP_SNS_SSP_TYPE;
        g_stSnsRegsInfo.u32RegNum = 3;
        for (i=0; i<3; i++)
        {
            g_stSnsRegsInfo.astSspData[i].u32DevAddr = 0x00;
            g_stSnsRegsInfo.astSspData[i].u32DevAddrByteNum = 0;
            g_stSnsRegsInfo.astSspData[i].u32RegAddrByteNum = 2;
            g_stSnsRegsInfo.astSspData[i].u32DataByteNum = 2;
        }
        g_stSnsRegsInfo.astSspData[0].bDelayCfg = HI_FALSE;
        g_stSnsRegsInfo.astSspData[0].u32RegAddr = 0x7c14;
        g_stSnsRegsInfo.astSspData[1].bDelayCfg = HI_TRUE;
        g_stSnsRegsInfo.astSspData[1].u32RegAddr = 0xC001;
        g_stSnsRegsInfo.astSspData[2].bDelayCfg = HI_TRUE;
        g_stSnsRegsInfo.astSspData[2].u32RegAddr = 0xC002;
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

#if CMOS_ICX692_ISP_WRITE_SENSOR_ENABLE
    cmos_init_regs_info();
    g_stSnsRegsInfo.astSspData[0].u32Data = (u32Tmp & 0xffff);
#else
	//sensor_write_register(0x7c13, (u32Tmp & 0xffff));  //SUB Start lines
	sensor_write_register(0x7c14, (u32Tmp & 0xffff));  //SUB End lines,only one sub pulse .
#endif
    return;
}

static HI_U32 digital_gain_table[1023]=
{
 1024,   1028,   1032,   1036,   1040,   1044,   1049,   1053,   1057,   1061,   1066,   1070,   1074,   1079,  1083,   1087, 
 1092,   1096,   1101,   1105,   1109,   1114,   1118,   1123,   1127,   1132,   1137,   1141,   1146,   1150,  1155,   1160, 
 1164,   1169,   1174,   1179,   1183,   1188,   1193,   1198,   1203,   1207,   1212,   1217,   1222,   1227,  1232,   1237, 
 1242,   1247,   1252,   1257,   1262,   1267,   1272,   1278,   1283,   1288,   1293,   1298,   1304,   1309,  1314,   1319, 
 1325,   1330,   1335,   1341,   1346,   1352,   1357,   1363,   1368,   1374,   1379,   1385,   1390,   1396,  1402,   1407, 
 1413,   1419,   1424,   1430,   1436,   1442,   1448,   1453,   1459,   1465,   1471,   1477,   1483,   1489,  1495,   1501, 
 1507,   1513,   1519,   1525,   1532,   1538,   1544,   1550,   1557,   1563,   1569,   1575,   1582,   1588,  1595,   1601,
 1608,   1614,   1621,   1627,   1634,   1640,   1647,   1654,   1660,   1667,   1674,   1680,   1687,   1694,  1701,   1708,
 1715,   1722,   1729,   1736,   1743,   1750,   1757,   1764,   1771,   1778,   1785,   1792,   1800,   1807,  1814,   1822,
 1829,   1836,   1844,   1851,   1859,   1866,   1874,   1881,   1889,   1896,   1904,   1912,   1919,   1927,  1935,   1943,
 1951,   1959,   1966,   1974,   1982,   1990,   1998,   2007,   2015,   2023,   2031,   2039,   2047,   2056,  2064,   2072,
 2081,   2089,   2097,   2106,   2114,   2123,   2132,   2140,   2149,   2157,   2166,   2175,   2184,   2193,  2201,   2210,
 2219,   2228,   2237,   2246,   2255,   2264,   2274,   2283,   2292,   2301,   2311,   2320,   2329,   2339,  2348,   2358,
 2367,   2377,   2386,   2396,   2406,   2415,   2425,   2435,   2445,   2454,   2464,   2474,   2484,   2494,  2504,   2515,
 2525,   2535,   2545,   2555,   2566,   2576,   2587,   2597,   2607,   2618,   2629,   2639,   2650,   2661,  2671,   2682,
 2693,   2704,   2715,   2726,   2737,   2748,   2759,   2770,   2781,   2792,   2804,   2815,   2826,   2838,  2849,   2861,
 2872,   2884,   2896,   2907,   2919,   2931,   2943,   2954,   2966,   2978,   2990,   3002,   3015,   3027,  3039,   3051,
 3064,   3076,   3088,   3101,   3113,   3126,   3139,   3151,   3164,   3177,   3190,   3202,   3215,   3228,  3241,   3254,
 3268,   3281,   3294,   3307,   3321,   3334,   3348,   3361,   3375,   3388,   3402,   3416,   3430,   3443,  3457,   3471,  
 3485,   3499,   3513,   3528,   3542,   3556,   3571,   3585,   3599,   3614,   3629,   3643,   3658,   3673,  3688,   3702,  
 3717,   3732,   3748,   3763,   3778,   3793,   3808,   3824,   3839,   3855,   3870,   3886,   3902,   3917,  3933,   3949,  
 3965,   3981,   3997,   4013,   4029,   4046,   4062,   4078,   4095,   4111,   4128,   4145,   4161,   4178,  4195,   4212,  
 4229,   4246,   4263,   4281,   4298,   4315,   4333,   4350,   4368,   4385,   4403,   4421,   4439,   4457,  4475,   4493,  
 4511,   4529,   4547,   4566,   4584,   4603,   4621,   4640,   4659,   4677,   4696,   4715,   4734,   4753,  4773,   4792,  
 4811,   4831,   4850,   4870,   4889,   4909,   4929,   4949,   4969,   4989,   5009,   5029,   5050,   5070,  5090,   5111,  
 5132,   5152,   5173,   5194,   5215,   5236,   5257,   5278,   5300,   5321,   5343,   5364,   5386,   5408,  5430,   5451,  
 5473,   5496,   5518,   5540,   5562,   5585,   5607,   5630,   5653,   5676,   5699,   5722,   5745,   5768,  5791,   5815,  
 5838,   5862,   5885,   5909,   5933,   5957,   5981,   6005,   6029,   6054,   6078,   6103,   6127,   6152,  6177,   6202,  
 6227,   6252,   6277,   6303,   6328,   6354,   6379,   6405,   6431,   6457,   6483,   6509,   6535,   6562,  6588,   6615,  
 6642,   6668,   6695,   6722,   6749,   6777,   6804,   6832,   6859,   6887,   6915,   6943,   6971,   6999,  7027,   7055,  
 7084,   7112,   7141,   7170,   7199,   7228,   7257,   7287,   7316,   7345,   7375,   7405,   7435,   7465,  7495,   7525,  
 7556,   7586,   7617,   7648,   7678,   7709,   7741,   7772,   7803,   7835,   7866,   7898,   7930,   7962,  7994,   8026,  
 8059,   8091,   8124,   8157,   8190,   8223,   8256,   8289,   8323,   8356,   8390,   8424,   8458,   8492,  8527,   8561,  
 8596,   8630,   8665,   8700,   8735,   8771,   8806,   8841,   8877,   8913,   8949,   8985,   9021,   9058,  9094,   9131,  
 9168,   9205,   9242,   9280,   9317,   9355,   9392,   9430,   9468,   9507,   9545,   9584,   9622,   9661,  9700,   9739,  
 9779,   9818,   9858,   9898,   9938,   9978,  10018,  10058,  10099,  10140,  10181,  10222,  10263,  10305, 10346,  10388,  
10430,  10472,  10514,  10557,  10599,  10642,  10685,  10728,  10772,  10815,  10859,  10903,  10947,  10991, 11035,  11080,  
11124,  11169,  11215,  11260,  11305,  11351,  11397,  11443,  11489,  11535,  11582,  11629,  11676,  11723, 11770,  11818,  
11865,  11913,  11961,  12010,  12058,  12107,  12156,  12205,  12254,  12304,  12353,  12403,  12453,  12504, 12554,  12605,  
12656,  12707,  12758,  12810,  12861,  12913,  12965,  13018,  13070,  13123,  13176,  13229,  13283,  13336, 13390,  13444,  
13498,  13553,  13608,  13663,  13718,  13773,  13829,  13885,  13941,  13997,  14054,  14110,  14167,  14224, 14282,  14340,  
14397,  14456,  14514,  14573,  14631,  14690,  14750,  14809,  14869,  14929,  14989,  15050,  15111,  15172, 15233,  15295,  
15356,  15418,  15481,  15543,  15606,  15669,  15732,  15796,  15859,  15923,  15988,  16052,  16117,  16182, 16248,  16313,  
16379,  16445,  16512,  16578,  16645,  16712,  16780,  16848,  16916,  16984,  17052,  17121,  17190,  17260, 17330,  17400,  
17470,  17540,  17611,  17682,  17754,  17825,  17897,  17970,  18042,  18115,  18188,  18262,  18335,  18409, 18484,  18558,  
18633,  18708,  18784,  18860,  18936,  19012,  19089,  19166,  19244,  19321,  19399,  19478,  19556,  19635, 19715,  19794,  
19874,  19954,  20035,  20116,  20197,  20279,  20361,  20443,  20525,  20608,  20691,  20775,  20859,  20943, 21028,  21113,  
21198,  21283,  21369,  21456,  21542,  21629,  21717,  21804,  21892,  21981,  22069,  22158,  22248,  22338, 22428,  22519,  
22609,  22701,  22792,  22884,  22977,  23070,  23163,  23256,  23350,  23444,  23539,  23634,  23730,  23825, 23922,  24018,  
24115,  24213,  24310,  24408,  24507,  24606,  24705,  24805,  24905,  25006,  25107,  25208,  25310,  25412, 25515,  25618,  
25721,  25825,  25929,  26034,  26139,  26245,  26351,  26457,  26564,  26671,  26779,  26887,  26996,  27105, 27214,  27324,  
27434,  27545,  27656,  27768,  27880,  27993,  28106,  28219,  28333,  28447,  28562,  28678,  28793,  28910, 29026,  29144,  
29261,  29379,  29498,  29617,  29737,  29857,  29977,  30098,  30220,  30342,  30464,  30587,  30711,  30835, 30959,  31084,  
31210,  31336,  31462,  31590,  31717,  31845,  31974,  32103,  32232,  32363,  32493,  32624,  32756,  32888, 33021,  33155,  
33288,  33423,  33558,  33693,  33829,  33966,  34103,  34241,  34379,  34518,  34657,  34797,  34938,  35079, 35220,  35363,  
35505,  35649,  35793,  35937,  36082,  36228,  36374,  36521,  36669,  36817,  36965,  37115,  37264,  37415, 37566,  37718,  
37870,  38023,  38176,  38330,  38485,  38641,  38797,  38953,  39111,  39269,  39427,  39586,  39746,  39907, 40068,  40229,  
40392,  40555,  40719,  40883,  41048,  41214,  41380,  41547,  41715,  41884,  42053,  42223,  42393,  42564, 42736,  42909,  
43082,  43256,  43431,  43606,  43782,  43959,  44136,  44314,  44493,  44673,  44853,  45034,  45216,  45399, 45582,  45766,  
45951,  46137,  46323,  46510,  46698,  46886,  47076,  47266,  47456,  47648,  47840,  48034,  48228,  48422, 48618,  48814,  
49011,  49209,  49408,  49607,  49808,  50009,  50211,  50413,  50617,  50821,  51026,  51233,  51439,  51647, 51856,  52065,  
52275,  52486,  52698,  52911,  53125,  53339,  53554,  53771,  53988,  54206,  54425,  54644,  54865,  55087, 55309,  55532,  
55757,  55982,  56208,  56435,  56663,  56891,  57121,  57352,  57583,  57816,  58049,  58284,  58519,  58755, 58992,  59231,  
59470,  59710,  59951,  60193,  60436,  60680,  60925,  61171,  61418,  61666,  61915,  62165,  62416,  62668, 62921           

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
    
    if (u32InTimes >= digital_gain_table[1022])
    {
        pstAeSnsGainInfo->u32SnsTimes = digital_gain_table[1022];
        pstAeSnsGainInfo->u32GainDb = 1022;
        
        return;
    }
    
    for(i = 1; i < 1023; i++)
    {
        if(u32InTimes < digital_gain_table[i - 1])
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
#if CMOS_ICX692_ISP_WRITE_SENSOR_ENABLE
    cmos_init_regs_info();    
    g_stSnsRegsInfo.astSspData[1].u32Data = u32Again;
    g_stSnsRegsInfo.astSspData[2].u32Data = (u32Dgain & 0x3ff);
    HI_MPI_ISP_SnsRegsCfg(&g_stSnsRegsInfo);
#else
    sensor_write_register(0xC001, u32Again);
    sensor_write_register(0xC002, (u32Dgain & 0x3ff));
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

    pstAwbSnsDft->au16GainOffset[0] = 0x176;
    pstAwbSnsDft->au16GainOffset[1] = 0x100;
    pstAwbSnsDft->au16GainOffset[2] = 0x100;
    pstAwbSnsDft->au16GainOffset[3] = 0x1f1;
    
    pstAwbSnsDft->as32WbPara[0] = 33;
    pstAwbSnsDft->as32WbPara[1] = 113;
    pstAwbSnsDft->as32WbPara[2] = -110;
    pstAwbSnsDft->as32WbPara[3] = 186795;
    pstAwbSnsDft->as32WbPara[4] = 128;
    pstAwbSnsDft->as32WbPara[5] = -138463;

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
    s32Ret = HI_MPI_ISP_SensorRegCallBack(ICX692_ID, &stIspRegister);
    if (s32Ret)
    {
        printf("sensor register callback function failed!\n");
        return s32Ret;
    }
    
    stLib.s32Id = 0;
    strcpy(stLib.acLibName, HI_AE_LIB_NAME);
    cmos_init_ae_exp_function(&stAeRegister.stSnsExp);
    s32Ret = HI_MPI_AE_SensorRegCallBack(&stLib, ICX692_ID, &stAeRegister);
    if (s32Ret)
    {
        printf("sensor register callback function to ae lib failed!\n");
        return s32Ret;
    }

    stLib.s32Id = 0;
    strcpy(stLib.acLibName, HI_AWB_LIB_NAME);
    cmos_init_awb_exp_function(&stAwbRegister.stSnsExp);
    s32Ret = HI_MPI_AWB_SensorRegCallBack(&stLib, ICX692_ID, &stAwbRegister);
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

    s32Ret = HI_MPI_ISP_SensorUnRegCallBack(ICX692_ID);
    if (s32Ret)
    {
        printf("sensor unregister callback function failed!\n");
        return s32Ret;
    }
    
    stLib.s32Id = 0;
    strcpy(stLib.acLibName, HI_AE_LIB_NAME);
    s32Ret = HI_MPI_AE_SensorUnRegCallBack(&stLib, ICX692_ID);
    if (s32Ret)
    {
        printf("sensor unregister callback function to ae lib failed!\n");
        return s32Ret;
    }

    stLib.s32Id = 0;
    strcpy(stLib.acLibName, HI_AWB_LIB_NAME);
    s32Ret = HI_MPI_AWB_SensorUnRegCallBack(&stLib, ICX692_ID);
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
