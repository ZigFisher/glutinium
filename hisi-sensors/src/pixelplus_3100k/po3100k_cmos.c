#if !defined(__PO3100K_CMOS_H_)
#define __PO3100K_CMOS_H_

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "hi_comm_sns.h"
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

#define PO3100K_ID 3100

/*set Frame End Update Mode 2 with HI_MPI_ISP_SetAEAttr and set this value 1 to avoid flicker in antiflicker mode */
/*when use Frame End Update Mode 2, the speed of i2c will affect whole system's performance                       */
/*increase I2C_DFT_RATE in Hii2c.c to 400000 to increase the speed of i2c                                         */
#define CMOS_PO3100K_ISP_WRITE_SENSOR_ENABLE (1)
/****************************************************************************
 * local variables                                                            *
 ****************************************************************************/

extern const unsigned int sensor_i2c_addr;
extern unsigned int sensor_addr_byte;
extern unsigned int sensor_data_byte;

HI_U32 gu32FullLinesStd = 750;
HI_U8 gu8SensorMode = 0;

#if CMOS_PO3100K_ISP_WRITE_SENSOR_ENABLE
ISP_SNS_REGS_INFO_S g_stSnsRegsInfo = {0};
#endif

static AWB_CCM_S g_stAwbCcm =
{
    4850,
    {
        0x01ee, 0x80a3, 0x804b,
        0x8040, 0x0195, 0x8055,
        0x0006, 0x80f0, 0x01e9
    },
    3100,
    {
        0x01f4, 0x804f, 0x80a5,
        0x805b, 0x01bf, 0x8064,
        0x8014, 0x8147, 0x025b
    },
    2470,
    {
        0x0201, 0x80ca, 0x8037,
        0x806d, 0x017a, 0x800d,
        0x803c, 0x8285, 0x03c1
    }
};

static AWB_AGC_TABLE_S g_stAwbAgcTable =
{
    /* bvalid */
    1,

    /* saturation */    
    {0x80,0x80,0x80,0x48,0x44,0x40,0x3C,0x38}
};

static ISP_CMOS_AGC_TABLE_S g_stIspAgcTable =
{
    /* bvalid */
    1,

    /* sharpen_alt_d */
    {0x50,0x48,0x40,0x38,0x34,0x30,0x28,0x28},
        
    /* sharpen_alt_ud */
    {0xb0,0xa0,0xa0,0x90,0x80,0x70,0x60,0x50},
        
    /* snr_thresh */
    {0x13,0x19,0x20,0x26,0x2c,0x32,0x38,0x38},
        
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
    {0x11,0x17,0x1b,0x1e,0x20,0x22,0x24,0x26,0x27,0x28,0x29,0x2a,0x2b,0x2c,0x2d,0x2d,0x2e,
	0x2f,0x2f,0x30,0x30,0x31,0x31,0x32,0x32,0x33,0x33,0x34,0x34,0x34,0x35,0x35,0x36,0x36,
	0x36,0x37,0x37,0x37,0x37,0x38,0x38,0x38,0x39,0x39,0x39,0x39,0x3a,0x3a,0x3a,0x3a,0x3b,
	0x3b,0x3b,0x3b,0x3b,0x3c,0x3c,0x3c,0x3c,0x3c,0x3d,0x3d,0x3d,0x3d,0x3d,0x3d,0x3e,0x3e,
	0x3e,0x3e,0x3e,0x3e,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x40,0x40,0x40,0x40,0x40,0x40,0x40,
	0x40,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x42,0x42,0x42,0x42,0x42,0x42,0x42,0x42,0x42,
	0x43,0x43,0x43,0x43,0x43,0x43,0x43,0x43,0x43,0x44,0x44,0x44,0x44,0x44,0x44,0x44,0x44,
	0x44,0x44,0x45,0x45,0x45,0x45,0x45,0x45,0x45},

    /* demosaic_weight_lut */
    {0x11,0x17,0x1b,0x1e,0x20,0x22,0x24,0x26,0x27,0x28,0x29,0x2a,0x2b,0x2c,0x2d,0x2d,0x2e,
	0x2f,0x2f,0x30,0x30,0x31,0x31,0x32,0x32,0x33,0x33,0x34,0x34,0x34,0x35,0x35,0x36,0x36,
	0x36,0x37,0x37,0x37,0x37,0x38,0x38,0x38,0x39,0x39,0x39,0x39,0x3a,0x3a,0x3a,0x3a,0x3b,
	0x3b,0x3b,0x3b,0x3b,0x3c,0x3c,0x3c,0x3c,0x3c,0x3d,0x3d,0x3d,0x3d,0x3d,0x3d,0x3e,0x3e,
	0x3e,0x3e,0x3e,0x3e,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x40,0x40,0x40,0x40,0x40,0x40,0x40,
	0x40,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x42,0x42,0x42,0x42,0x42,0x42,0x42,0x42,0x42,
	0x43,0x43,0x43,0x43,0x43,0x43,0x43,0x43,0x43,0x44,0x44,0x44,0x44,0x44,0x44,0x44,0x44,
	0x44,0x44,0x45,0x45,0x45,0x45,0x45,0x45,0x45}
};

static ISP_CMOS_DEMOSAIC_S g_stIspDemosaic =
{
    /* bvalid */
    1,
    
    /*vh_slope*/
    0xf0,

    /*aa_slope*/
    0xe6,

    /*va_slope*/
    0xe7,

    /*uu_slope*/
    0xc0,

    /*sat_slope*/
    0x5d,

    /*ac_slope*/
    0xcf,

    /*vh_thresh*/
    0x0,

    /*aa_thresh*/
    0x0,

    /*va_thresh*/
    0x0,

    /*uu_thresh*/
    0x0,

    /*sat_thresh*/
    0x171,

    /*ac_thresh*/
    0x1b3
};

static HI_U32 gain_table[96]=
{
    1024,   1088,   1152,   1216,   1280,   1344,   1408,   1472,   1536,   1600,   1664,   1728,  1792,   1856, 
    1920,   1984,   2048,   2176,   2304,   2432,   2560,   2688,   2816,   2944,   3072,   3200,  3328,   3456, 
    3584,   3712,   3840,   3968,   4096,   4352,   4608,   4864,   5120,   5376,   5632,   5888,  6144,   6400,
    6656,   6912,   7168,   7424,   7680,   7936,   8192,   8704,   9216,   9728,   10240, 10752, 11264, 11776,
    12288, 12800, 13312, 13824, 14336, 14848, 15360, 15872, 16384, 17408, 18432, 19456, 
    20480, 21504, 22528, 23552, 24576, 25600, 26624, 27648, 28672, 29696, 30720, 31744, 
    32768, 34816, 36864, 38912, 40960, 43008, 45056, 47104, 49152, 51200, 53248, 55296, 
    57344, 59392, 61440, 63488  
};

static ISP_CMOS_GAMMA_S g_stIspGamma =
{
    /* bvalid */
    1,
    
#if 1    
    {0  ,120 ,220 ,310 ,390 ,470 ,540 ,610 ,670 ,730 ,786 ,842 ,894 ,944 ,994 ,1050,    
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
#else  /*higher  contrast*/
    {0  , 54 , 106, 158, 209, 259, 308, 356, 403, 450, 495, 540, 584, 628, 670, 713,
    754 ,795 , 835, 874, 913, 951, 989,1026,1062,1098,1133,1168,1203,1236,1270,1303,
    1335,1367,1398,1429,1460,1490,1520,1549,1578,1607,1635,1663,1690,1717,1744,1770,
    1796,1822,1848,1873,1897,1922,1946,1970,1993,2017,2040,2062,2085,2107,2129,2150,
    2172,2193,2214,2235,2255,2275,2295,2315,2335,2354,2373,2392,2411,2429,2447,2465,
    2483,2501,2519,2536,2553,2570,2587,2603,2620,2636,2652,2668,2684,2700,2715,2731,
    2746,2761,2776,2790,2805,2819,2834,2848,2862,2876,2890,2903,2917,2930,2944,2957,
    2970,2983,2996,3008,3021,3033,3046,3058,3070,3082,3094,3106,3118,3129,3141,3152,
    3164,3175,3186,3197,3208,3219,3230,3240,3251,3262,3272,3282,3293,3303,3313,3323,
    3333,3343,3352,3362,3372,3381,3391,3400,3410,3419,3428,3437,3446,3455,3464,3473,
    3482,3490,3499,3508,3516,3525,3533,3541,3550,3558,3566,3574,3582,3590,3598,3606,
    3614,3621,3629,3637,3644,3652,3660,3667,3674,3682,3689,3696,3703,3711,3718,3725,
    3732,3739,3746,3752,3759,3766,3773,3779,3786,3793,3799,3806,3812,3819,3825,3831,
    3838,3844,3850,3856,3863,3869,3875,3881,3887,3893,3899,3905,3910,3916,3922,3928,
    3933,3939,3945,3950,3956,3962,3967,3973,3978,3983,3989,3994,3999,4005,4010,4015,
    4020,4026,4031,4036,4041,4046,4051,4056,4061,4066,4071,4076,4081,4085,4090,4095,4095}
#endif
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

    pstDef->stDenoise.u8SinterThresh= 0x23;
    pstDef->stDenoise.u8NoiseProfile= 0x1;      //0: use default profile table; 1: use calibrated profile lut, the setting for nr0 and nr1 must be correct.
    pstDef->stDenoise.u16Nr0        = 0x0;
    pstDef->stDenoise.u16Nr1        = 546;

    pstDef->stDrc.u8DrcBlack        = 0x00;
    pstDef->stDrc.u8DrcVs           = 0x04;     // variance space
    pstDef->stDrc.u8DrcVi           = 0x01;     // variance intensity
    pstDef->stDrc.u8DrcSm           = 0x30;     // slope max
    pstDef->stDrc.u16DrcWl          = 0x4FF;    // white level

    memcpy(&pstDef->stNoiseTbl, &g_stIspNoiseTable, sizeof(ISP_CMOS_NOISE_TABLE_S));            
    memcpy(&pstDef->stAgcTbl, &g_stIspAgcTable, sizeof(ISP_CMOS_AGC_TABLE_S));
    memcpy(&pstDef->stDemosaic, &g_stIspDemosaic, sizeof(ISP_CMOS_DEMOSAIC_S));
    memcpy(&pstDef->stGamma, &g_stIspGamma, sizeof(ISP_CMOS_GAMMA_S));

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
        pstBlackLevel->au16BlackLevel[i] = 0x4;
    }

    return 0;    
}

HI_VOID cmos_set_pixel_detect(HI_BOOL bEnable)
{
    if (bEnable) /* setup for ISP pixel calibration mode */
    {
        sensor_write_register(0x03, 0x00);//5fps
        sensor_write_register(0x08, 0x11);
        sensor_write_register(0x09, 0x94);
        sensor_write_register(0x0A, 0x11);
        sensor_write_register(0x0B, 0x94);

        sensor_write_register(0x03, 0x01);

        sensor_write_register(0xC0, 0x11);
        sensor_write_register(0xC1, 0x92);

        sensor_write_register(0xC3, 0x0);
        sensor_write_register(0xC4, 0x40);
    }
    else /* setup for ISP 'normal mode' */
    {
        sensor_write_register(0x03, 0x00);//30fps
        sensor_write_register(0x08, 0x02);
        sensor_write_register(0x09, 0xED);
        sensor_write_register(0x0A, 0x02);
        sensor_write_register(0x0B, 0xED);
        sensor_write_register(0x03, 0x01);
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

    gu32FullLinesStd = 750;
    
    pstAeSnsDft->au8HistThresh[0] = 0xd;
    pstAeSnsDft->au8HistThresh[1] = 0x28;
    pstAeSnsDft->au8HistThresh[2] = 0x60;
    pstAeSnsDft->au8HistThresh[3] = 0x80;
    
    pstAeSnsDft->u8AeCompensation = 0x40;
    
    pstAeSnsDft->u32LinesPer500ms = 750*30/2;
    pstAeSnsDft->u32FullLinesStd = gu32FullLinesStd;
    pstAeSnsDft->u32FlickerFreq = 0;//60*256;//50*256;

    pstAeSnsDft->stIntTimeAccu.enAccuType = AE_ACCURACY_LINEAR;
    pstAeSnsDft->stIntTimeAccu.f32Accuracy = 1;
    pstAeSnsDft->u32MaxIntTime = 748;
    pstAeSnsDft->u32MinIntTime = 2;    
    pstAeSnsDft->u32MaxIntTimeTarget = 65535;
    pstAeSnsDft->u32MinIntTimeTarget = 2;

    pstAeSnsDft->stAgainAccu.enAccuType = AE_ACCURACY_TABLE;
    pstAeSnsDft->stAgainAccu.f32Accuracy = 0.0009765625;    
    pstAeSnsDft->u32MaxAgain = 63488;  /* 62 / 0.0009765625= 63488 */
    pstAeSnsDft->u32MinAgain = 1024;
    pstAeSnsDft->u32MaxAgainTarget = 63488;
    pstAeSnsDft->u32MinAgainTarget = 1024;

    pstAeSnsDft->stDgainAccu.enAccuType = AE_ACCURACY_LINEAR;
    pstAeSnsDft->stDgainAccu.f32Accuracy = 0.015625;    
    pstAeSnsDft->u32MaxDgain = 255;  /* 4 / 0.015625 = 256;*/
    pstAeSnsDft->u32MinDgain = 64;
    pstAeSnsDft->u32MaxDgainTarget = 255;
    pstAeSnsDft->u32MinDgainTarget = 64;

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
            // Change the frame rate via changing the vertical blanking
            gu32FullLinesStd = 750;
            pstAeSnsDft->u32MaxIntTime = 748;
            pstAeSnsDft->u32LinesPer500ms = 750 * 30 / 2;
            sensor_write_register(0x03, 0x00);
            sensor_write_register(0x08, 0x02);
            sensor_write_register(0x09, 0xED);
            sensor_write_register(0x0A, 0x02);
            sensor_write_register(0x0B, 0xED);
            sensor_write_register(0x03, 0x01);
        break;
        
        case 25:
            // Change the frame rate via changing the vertical blanking
            gu32FullLinesStd = 900;
            pstAeSnsDft->u32MaxIntTime = 898;
            pstAeSnsDft->u32LinesPer500ms = 900 * 25 / 2;
            sensor_write_register(0x03, 0x00);
            sensor_write_register(0x08, 0x03);
            sensor_write_register(0x09, 0x83);
            sensor_write_register(0x0A, 0x03);
            sensor_write_register(0x0B, 0x83);
            sensor_write_register(0x03, 0x01);
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
    static HI_U16 last_vblanking_lines = 65535;
    if(u16FullLines != last_vblanking_lines)
    {
        sensor_write_register(0x03, 0x0);
        sensor_write_register(0x08, u16FullLines>>8);
        sensor_write_register(0x09, u16FullLines&0xFF);
        sensor_write_register(0x0A, u16FullLines>>8);
        sensor_write_register(0x0B, u16FullLines&0xFF);
        sensor_write_register(0x03, 0x1);
        last_vblanking_lines = u16FullLines;
    }  
    return;
}

static HI_VOID cmos_init_regs_info(HI_VOID)
{
#if CMOS_PO3100K_ISP_WRITE_SENSOR_ENABLE
    HI_S32 i;
    static HI_BOOL bInit = HI_FALSE;

    if (HI_FALSE == bInit)
    {
        g_stSnsRegsInfo.enSnsType = ISP_SNS_I2C_TYPE;
        g_stSnsRegsInfo.u32RegNum = 4;
        for (i=0; i<g_stSnsRegsInfo.u32RegNum; i++)
        {
            g_stSnsRegsInfo.astI2cData[i].u8DevAddr = sensor_i2c_addr;
            g_stSnsRegsInfo.astI2cData[i].u32AddrByteNum = sensor_addr_byte;
            g_stSnsRegsInfo.astI2cData[i].u32DataByteNum = sensor_data_byte;
        }
        g_stSnsRegsInfo.astI2cData[0].bDelayCfg = HI_FALSE;
        g_stSnsRegsInfo.astI2cData[0].u32RegAddr = 0xc0;
        g_stSnsRegsInfo.astI2cData[1].bDelayCfg = HI_FALSE;
        g_stSnsRegsInfo.astI2cData[1].u32RegAddr = 0xc1;
        g_stSnsRegsInfo.astI2cData[2].bDelayCfg = HI_FALSE;
        g_stSnsRegsInfo.astI2cData[2].u32RegAddr = 0xc3;
        g_stSnsRegsInfo.astI2cData[3].bDelayCfg = HI_FALSE;
        g_stSnsRegsInfo.astI2cData[3].u32RegAddr = 0xc4;
        g_stSnsRegsInfo.bDelayCfgIspDgain = HI_FALSE;

        bInit = HI_TRUE;
    }
#endif
    return;
}

/* while isp notify ae to update sensor regs, ae call these funcs. */
static HI_VOID cmos_inttime_update(HI_U32 u32IntTime)
{
#if CMOS_PO3100K_ISP_WRITE_SENSOR_ENABLE
    cmos_init_regs_info();
    g_stSnsRegsInfo.astI2cData[0].u32Data = (u32IntTime>>8);
    g_stSnsRegsInfo.astI2cData[1].u32Data = (u32IntTime & 0xff);
#endif
    return;
}

static HI_VOID cmos_again_calc_table(HI_U32 u32InTimes,AE_SENSOR_GAININFO_S *pstAeSnsGainInfo)
{
    int i;

    if(HI_NULL == pstAeSnsGainInfo)
    {
        printf("null pointer when get ae sensor gain info  value!\n");
        return;
    }

    pstAeSnsGainInfo->u32GainDb = 0;
    pstAeSnsGainInfo->u32SnsTimes = 1024;

    if (u32InTimes >= gain_table[95])
    {
        pstAeSnsGainInfo->u32SnsTimes = gain_table[95];
        pstAeSnsGainInfo->u32GainDb = 95;
        return ;
    }

    for(i = 1; i < 96; i++)
    {
        if(u32InTimes < gain_table[i])
        {
            pstAeSnsGainInfo->u32SnsTimes = gain_table[i - 1];
            pstAeSnsGainInfo->u32GainDb = i - 1;
            break;
        }
    }

    return;
}

static HI_VOID cmos_gains_update(HI_U32 u32Again, HI_U32 u32Dgain)
{
#if CMOS_PO3100K_ISP_WRITE_SENSOR_ENABLE
    cmos_init_regs_info();
    
    g_stSnsRegsInfo.astI2cData[2].u32Data = u32Again;                
    g_stSnsRegsInfo.astI2cData[3].u32Data = u32Dgain;
    
    HI_MPI_ISP_SnsRegsCfg(&g_stSnsRegsInfo);    
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

    pstAwbSnsDft->u16WbRefTemp = 4850;

    pstAwbSnsDft->au16GainOffset[0] = 0x177;
    pstAwbSnsDft->au16GainOffset[1] = 0x100;
    pstAwbSnsDft->au16GainOffset[2] = 0x100;
    pstAwbSnsDft->au16GainOffset[3] = 0x0197;

    pstAwbSnsDft->as32WbPara[0] = 123;
    pstAwbSnsDft->as32WbPara[1] = -28;
    pstAwbSnsDft->as32WbPara[2] = -161;
    pstAwbSnsDft->as32WbPara[3] = 192773;
    pstAwbSnsDft->as32WbPara[4] = 128;
    pstAwbSnsDft->as32WbPara[5] = -142978;

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
    pstExpFuncs->pfn_cmos_again_calc_table  = cmos_again_calc_table;
    
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
    s32Ret = HI_MPI_ISP_SensorRegCallBack(PO3100K_ID, &stIspRegister);
    if (s32Ret)
    {
        printf("sensor register callback function failed!\n");
        return s32Ret;
    }
    
    stLib.s32Id = 0;
    strcpy(stLib.acLibName, HI_AE_LIB_NAME);
    cmos_init_ae_exp_function(&stAeRegister.stSnsExp);
    s32Ret = HI_MPI_AE_SensorRegCallBack(&stLib, PO3100K_ID, &stAeRegister);
    if (s32Ret)
    {
        printf("sensor register callback function to ae lib failed!\n");
        return s32Ret;
    }

    stLib.s32Id = 0;
    strcpy(stLib.acLibName, HI_AWB_LIB_NAME);
    cmos_init_awb_exp_function(&stAwbRegister.stSnsExp);
    s32Ret = HI_MPI_AWB_SensorRegCallBack(&stLib, PO3100K_ID, &stAwbRegister);
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

    s32Ret = HI_MPI_ISP_SensorUnRegCallBack(PO3100K_ID);
    if (s32Ret)
    {
        printf("sensor unregister callback function failed!\n");
        return s32Ret;
    }
    
    stLib.s32Id = 0;
    strcpy(stLib.acLibName, HI_AE_LIB_NAME);
    s32Ret = HI_MPI_AE_SensorUnRegCallBack(&stLib, PO3100K_ID);
    if (s32Ret)
    {
        printf("sensor unregister callback function to ae lib failed!\n");
        return s32Ret;
    }

    stLib.s32Id = 0;
    strcpy(stLib.acLibName, HI_AWB_LIB_NAME);
    s32Ret = HI_MPI_AWB_SensorUnRegCallBack(&stLib, PO3100K_ID);
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

#endif // __PO3100K_CMOS_H_
