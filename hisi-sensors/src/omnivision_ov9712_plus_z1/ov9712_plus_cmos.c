#if !defined(__OV9715_CMOS_H_)
#define __OV9715_CMOS_H_

#include <stdio.h>
#include <string.h>
#include <assert.h>
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

#define OV9712_ID 9712

/*set Frame End Update Mode 2 with HI_MPI_ISP_SetAEAttr and set this value 1 to avoid flicker in antiflicker mode */
/*when use Frame End Update Mode 2, the speed of i2c will affect whole system's performance                       */
/*increase I2C_DFT_RATE in Hii2c.c to 400000 to increase the speed of i2c                                         */
#define CMOS_OV9712_ISP_WRITE_SENSOR_ENABLE (1)

/*change this value to 1 to make the image looks more sharpen*/    
#define CMOS_OV9712_MORE_SHARPEN (1)

/* To change the mode of slow framerate. When the value is 0, add the line numbers to slow framerate.
 * When the value is 1, add the line length to slow framerate. */
#define CMOS_OV9712_SLOW_FRAMERATE_MODE (0)

/****************************************************************************
 * local variables                                                            *
 ****************************************************************************/

extern const unsigned int sensor_i2c_addr;
extern unsigned int sensor_addr_byte;
extern unsigned int sensor_data_byte;

HI_U8 gu8SensorMode = 0;

static HI_U32 gu8Fps = 30;
static HI_U32 gu32FullLinesStd = 810;
static HI_U32 gu32FullLines = 810;

#if CMOS_OV9712_ISP_WRITE_SENSOR_ENABLE
ISP_SNS_REGS_INFO_S g_stSnsRegsInfo = {0};
#endif

static AWB_CCM_S g_stAwbCcm =
{
     5000,
     {
      0x0146, 0x8068, 0x0021,
      0x800c, 0x00e9, 0x0022,
      0x0010, 0x80bd, 0x01ac
     },
     3200,
     {
      0x016a, 0x8034, 0x8036,
      0x801d, 0x00da, 0x0042,
      0x8017, 0x81d2, 0x02e9
     },
     2700,
     {
      0x016a, 0x8034, 0x8036,
      0x801d, 0x00da, 0x0042,
      0x8017, 0x81d2, 0x02e9
     }
};

static AWB_AGC_TABLE_S g_stAwbAgcTable =
{
    /* bvalid */
    1,

    /* saturation */
    {0x90,0x90,0x80,0x80,0x68,0x48,0x35,0x30}
};


static ISP_CMOS_AGC_TABLE_S g_stIspAgcTable =
{
    /* bvalid */
    1,
    
#if CMOS_OV9712_MORE_SHARPEN
    //sharpen_alt_d
    {80,75,70,65,55,45,35,20},

    //sharpen_alt_ud
    {75,70,65,60,50,40,25,10},

    //snr_thresh
    {0x23,0x28,0x2b,0x35,0x3f,0x46,0x4b,0x4f},
#else    
    /* sharpen_alt_d */
    {0x8e,0x8b,0x88,0x83,0x7d,0x76,0x75,0x74},
        
    /* sharpen_alt_ud */
    {0x8f,0x89,0x7e,0x78,0x6f,0x44,0x40,0x35},
        
    /* snr_thresh */
    {0x19,0x1e,0x2d,0x32,0x39,0x3f,0x48,0x4b},
#endif    
        
    //demosaic_lum_thresh
    {80,64,64,48,48,32,32,16},

    //demosaic_np_offset
    {0x0,0xa,0x12,0x1a,0x20,0x28,0x30,0x30},

    //ge_strength
    {0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55},

};

static ISP_CMOS_NOISE_TABLE_S g_stIspNoiseTable =
{
    /* bvalid */
    1,
    
    /* nosie_profile_weight_lut */
    {   
    0x00,0x04,0x10,0x16,0x1a,0x1d,0x1f,0x21,0x23,0x24,0x26,0x27,0x28,0x29,0x2a,0x2a,
    0x2b,0x2c,0x2d,0x2d,0x2e,0x2e,0x2f,0x2f,0x30,0x30,0x31,0x31,0x32,0x32,0x33,0x33,
    0x33,0x34,0x34,0x34,0x35,0x35,0x35,0x36,0x36,0x36,0x37,0x37,0x37,0x37,0x38,0x38,
    0x38,0x38,0x39,0x39,0x39,0x39,0x39,0x3a,0x3a,0x3a,0x3a,0x3b,0x3b,0x3b,0x3b,0x3b,
    0x3b,0x3c,0x3c,0x3c,0x3c,0x3c,0x3c,0x3d,0x3d,0x3d,0x3d,0x3d,0x3d,0x3e,0x3e,0x3e,
    0x3e,0x3e,0x3e,0x3e,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x40,0x40,0x40,0x40,0x40,
    0x40,0x40,0x40,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x42,0x42,0x42,0x42,
    0x42,0x42,0x42,0x42,0x42,0x42,0x43,0x43,0x43,0x43,0x43,0x43,0x43,0x43,0x43,0x43
    },

    /* demosaic_weight_lut */
    {0, 27, 31, 33, 35, 36, 37, 38, 39, 40, 40, 41, 41, 42, 42, 43,
    43, 43, 44, 44, 44, 45, 45, 45, 45, 46, 46, 46, 46, 46, 47, 47,
    47, 47, 47, 48, 48, 48, 48, 48, 48, 48, 49, 49, 49, 49, 49, 49,
    49, 49, 50, 50, 50, 50, 50, 50, 50, 50, 50, 51, 51, 51, 51, 51,
    51, 51, 51, 51, 51, 51, 51, 52, 52, 52, 52, 52, 52, 52, 52, 52,
    52, 52, 52, 52, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53,
    53, 53, 53, 53, 54, 54, 54, 54, 54, 54, 54, 54, 54, 54, 54, 54,
    54, 54, 54, 54, 54, 54, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55}
};

static ISP_CMOS_DEMOSAIC_S g_stIspDemosaic =
{
    /* bvalid */
    1,
    
    /*vh_slope*/
    205,

    /*aa_slope*/
    191,
#if CMOS_OV9712_MORE_SHARPEN
    /*va_slope*/
    193,

    /*uu_slope*/
    200,
#else
    /*va_slope*/
    0xec,

    /*uu_slope*/
    0x89,
#endif

    /*sat_slope*/
    0x5d,

    /*ac_slope*/
    0xcf,

    /*vh_thresh*/
    16,

    /*aa_thresh*/
    10,

    /*va_thresh*/
    10,

    /*uu_thresh*/
    10,

    /*sat_thresh*/
    0x171,

    /*ac_thresh*/
    0x1b3
};

static ISP_CMOS_SHADING_S g_stIspShading =
{
    /* bvalid */
    1,
    
    /*shading_center_r*/
    0x28f, 0x16c,

    /*shading_center_g*/
    0x28e, 0x177,

    /*shading_center_b*/
    0x293, 0x16c,

    /*shading_table_r*/
  {   
    0x1000,0x100c,0x1035,0x1065,0x1099,0x10cd,0x1102,0x1137,0x116c,0x11a1,0x11d5,0x1209,
    0x123d,0x126f,0x12a2,0x12d3,0x1303,0x1335,0x1365,0x1394,0x13c2,0x13f0,0x141c,0x1449,
    0x1475,0x149f,0x14c9,0x14f3,0x151b,0x1543,0x156a,0x1590,0x15b5,0x15d9,0x15fd,0x1620,
    0x1642,0x1663,0x1684,0x16a3,0x16c2,0x16e0,0x16fd,0x1719,0x1735,0x1750,0x176a,0x1783,
    0x179b,0x17b3,0x17ca,0x17e0,0x17f5,0x180a,0x181e,0x1831,0x1843,0x1855,0x1866,0x1877,
    0x1887,0x1896,0x18a4,0x18b2,0x18bf,0x18cb,0x18d7,0x18e3,0x18ef,0x18f8,0x1903,0x190d,
    0x1916,0x191e,0x1926,0x192d,0x1935,0x193c,0x1942,0x1948,0x194d,0x1953,0x1958,0x195d,
    0x1961,0x1964,0x1968,0x196c,0x196f,0x1972,0x1975,0x1977,0x1978,0x197b,0x197d,0x197e,
    0x1981,0x1982,0x1983,0x1984,0x1986,0x1987,0x1988,0x1989,0x198a,0x198a,0x198a,0x198c,
    0x198d,0x198e,0x198e,0x198f,0x1990,0x1991,0x1992,0x1993,0x1994,0x1996,0x1997,0x1999,
    0x199a,0x199c,0x199d,0x199f,0x19a2,0x19a5,0x19a7,0x19aa,0x19ac},

    /*shading_table_g*/
    {
    0x1000,0x1005,0x1028,0x1055,0x1087,0x10bb,0x10f0,0x1125,0x115b,0x1190,0x11c5,0x11fa,
    0x122f,0x1263,0x1296,0x12c9,0x12fc,0x132d,0x135e,0x138f,0x13be,0x13ed,0x141b,0x1449,
    0x1475,0x14a1,0x14cc,0x14f6,0x1520,0x1548,0x1570,0x1597,0x15bd,0x15e2,0x1606,0x1629,
    0x164b,0x166d,0x168e,0x16ad,0x16cc,0x16ea,0x1707,0x1724,0x173f,0x175a,0x1773,0x178c,
    0x17a4,0x17bc,0x17d2,0x17e8,0x17fc,0x1810,0x1824,0x1836,0x1848,0x1859,0x1869,0x1879,
    0x1888,0x1896,0x18a4,0x18b1,0x18bd,0x18c9,0x18d4,0x18df,0x18e9,0x18f2,0x18fb,0x1904,
    0x190c,0x1914,0x191b,0x1921,0x1928,0x192d,0x1933,0x1938,0x193d,0x1941,0x1945,0x1949,
    0x194d,0x1950,0x1953,0x1956,0x1959,0x195b,0x195d,0x195f,0x1961,0x1963,0x1965,0x1966,
    0x1967,0x1969,0x196a,0x196b,0x196d,0x196e,0x196f,0x1970,0x1972,0x1973,0x1974,0x1975,
    0x1977,0x1978,0x197a,0x197c,0x197e,0x1980,0x1982,0x1984,0x1987,0x1989,0x198c,0x198f,
    0x1992,0x1996,0x199a,0x199e,0x19a2,0x19a6,0x19ab,0x19b0,0x19b4},

    /*shading_table_b*/
    {
    0x1000,0x1000,0x1011,0x102f,0x1054,0x107c,0x10a5,0x10cf,0x10fa,0x1125,0x1150,0x117c,
    0x11a7,0x11d1,0x11fc,0x1226,0x1250,0x1279,0x12a2,0x12ca,0x12f2,0x131a,0x1340,0x1367,
    0x138c,0x13b1,0x13d6,0x13fa,0x141d,0x143f,0x1461,0x1483,0x14a3,0x14c3,0x14e3,0x1502,
    0x1520,0x153d,0x155a,0x1576,0x1591,0x15ac,0x15c6,0x15e0,0x15f8,0x1610,0x1628,0x163f,
    0x1655,0x166b,0x1680,0x1694,0x16a8,0x16bb,0x16cd,0x16df,0x16f1,0x1701,0x1712,0x1721,
    0x1730,0x173f,0x174d,0x175b,0x1768,0x1774,0x1780,0x178c,0x1797,0x17a2,0x17ac,0x17b6,
    0x17bf,0x17c8,0x17d1,0x17d9,0x17e1,0x17e8,0x17ef,0x17f6,0x17fd,0x1803,0x1809,0x180f,
    0x1814,0x1819,0x181e,0x1822,0x1827,0x182b,0x182f,0x1833,0x1836,0x183a,0x183d,0x1840,
    0x1843,0x1846,0x1849,0x184b,0x184e,0x1850,0x1853,0x1855,0x1857,0x185a,0x185c,0x185e,
    0x1860,0x1862,0x1864,0x1867,0x1869,0x186b,0x186d,0x1870,0x1872,0x1875,0x1877,0x187a,
    0x187d,0x1880,0x1883,0x1886,0x1889,0x188c,0x1890,0x1894,0x1895},

    /*shading_off_center_r_g_b*/
    0xef0, 0xec3, 0xecd,

    /*shading_table_nobe_number*/
    129
};

static ISP_CMOS_GAMMA_S g_stIspGamma =
{
    /* bvalid */
    1,
    
    {0,  54, 106, 158, 209, 259, 308, 356, 403, 450, 495, 540, 584, 628, 670, 713, 
    754, 795, 835, 874, 913, 951, 989,1026,1062,1098,1133,1168,1203,1236,1270,1303, 
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
    4020,4026,4031,4036,4041,4046,4051,4056,4061,4066,4071,4076,4081,4085,4090,4095, 4095}
};

HI_U32 cmos_get_isp_default(ISP_CMOS_DEFAULT_S *pstDef)
{
    if (HI_NULL == pstDef)
    {
        printf("null pointer when get isp default value!\n");
        return -1;
    }

    memset(pstDef, 0, sizeof(ISP_CMOS_DEFAULT_S));
    
    pstDef->stComm.u8Rggb           = 0x3;      //3: bggr  
    pstDef->stComm.u8BalanceFe      = 0x1;

    pstDef->stDenoise.u8SinterThresh= 0x15;
    pstDef->stDenoise.u8NoiseProfile= 0x0;      //0: use default profile table; 1: use calibrated profile lut, the setting for nr0 and nr1 must be correct.
    pstDef->stDenoise.u16Nr0        = 0x0;
    pstDef->stDenoise.u16Nr1        = 0x0;

    pstDef->stDrc.u8DrcBlack        = 0x00;
    pstDef->stDrc.u8DrcVs           = 0x04;     // variance space
    pstDef->stDrc.u8DrcVi           = 0x08;     // variance intensity
    pstDef->stDrc.u8DrcSm           = 0xa0;     // slope max
    pstDef->stDrc.u16DrcWl          = 0x4ff;    // white level

    memcpy(&pstDef->stAgcTbl, &g_stIspAgcTable, sizeof(ISP_CMOS_AGC_TABLE_S));
    memcpy(&pstDef->stNoiseTbl, &g_stIspNoiseTable, sizeof(ISP_CMOS_NOISE_TABLE_S));
    memcpy(&pstDef->stDemosaic, &g_stIspDemosaic, sizeof(ISP_CMOS_DEMOSAIC_S));
    memcpy(&pstDef->stShading, &g_stIspShading, sizeof(ISP_CMOS_SHADING_S));
    memcpy(&pstDef->stGamma, &g_stIspGamma, sizeof(ISP_CMOS_GAMMA_S));

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

    pstBlackLevel->au16BlackLevel[0] = 68;
    pstBlackLevel->au16BlackLevel[1] = 64;
    pstBlackLevel->au16BlackLevel[2] = 64;
    pstBlackLevel->au16BlackLevel[3] = 68;

    return 0;    
}

HI_VOID cmos_set_pixel_detect(HI_BOOL bEnable)
{
    if (bEnable) /* setup for ISP pixel calibration mode */
    {
        /* 5 fps */
        sensor_write_register(0x2d, 0xd2); 
        sensor_write_register(0x2e, 0x0f); 
        
        /* min gain */
        sensor_write_register(0x0, 0x00);

        /* max exposure time*/
        sensor_write_register(0x10, 0xf8);
        sensor_write_register(0x16, 0x12);
    }
    else /* setup for ISP 'normal mode' */
    {
        sensor_write_register(0x2d, 0x0);
        sensor_write_register(0x2e, 0x0);
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
        break;
        //sensor mode 1
        case 1:
            gu8SensorMode = 1;
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

    gu32FullLinesStd = 810;
    
    pstAeSnsDft->u32LinesPer500ms = 810*30/2;
    pstAeSnsDft->u32FullLinesStd = gu32FullLinesStd;
    pstAeSnsDft->u32FlickerFreq = 0;//60*256;//50*256;
    
    //gu8Fps = 30;

    pstAeSnsDft->stIntTimeAccu.enAccuType = AE_ACCURACY_LINEAR;
    pstAeSnsDft->stIntTimeAccu.f32Accuracy = 1;
    pstAeSnsDft->u32MaxIntTime = 806;
    pstAeSnsDft->u32MinIntTime = 2;
    
    pstAeSnsDft->au8HistThresh[0] = 0xd;
    pstAeSnsDft->au8HistThresh[1] = 0x28;
    pstAeSnsDft->au8HistThresh[2] = 0x60;
    pstAeSnsDft->au8HistThresh[3] = 0x80;
    
    pstAeSnsDft->u8AeCompensation = 0x40;
    
    pstAeSnsDft->u32MaxIntTimeTarget = 65535;
    pstAeSnsDft->u32MinIntTimeTarget = 2;

    /* 1(1+1/16), 1(1+2/16), ... , 2(1+1/16), ... , 16(1+15/16) */
    pstAeSnsDft->stAgainAccu.enAccuType = AE_ACCURACY_DB;
    pstAeSnsDft->stAgainAccu.f32Accuracy = 6;
    pstAeSnsDft->u32MaxAgain = 4;  /* 1, 2, 4, ... 16 (0~24db, unit is 6db) */
    pstAeSnsDft->u32MinAgain = 0;
    pstAeSnsDft->u32MaxAgainTarget = 4;
    pstAeSnsDft->u32MinAgainTarget = 0;
    

    pstAeSnsDft->stDgainAccu.enAccuType = AE_ACCURACY_LINEAR;
    pstAeSnsDft->stDgainAccu.f32Accuracy = 0.0625;
    pstAeSnsDft->u32MaxDgain = 31;  /* 1 ~ 31/16, unit is 1/16 */
    pstAeSnsDft->u32MinDgain = 16;
    pstAeSnsDft->u32MaxDgainTarget = 32;
    pstAeSnsDft->u32MinDgainTarget = 16; 

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
    HI_U32 tp = 1692;
#if 0
    switch(fps)
    {
        case 30:
            tp = 1688;
            break;
        case 25:
            tp = 2028;
            break;
        default:
            break;
    }
#endif
    tp = 1692 * 30 / u8Fps + 3;
    sensor_write_register(0x2a, tp & 0xfc);
    sensor_write_register(0x2b, (tp & 0xff00) >> 8);

    pstAeSnsDft->u32MaxIntTime = 806;
    gu32FullLinesStd = 810;
    gu8Fps = u8Fps;
    pstAeSnsDft->u32LinesPer500ms = gu32FullLinesStd * u8Fps / 2;
    pstAeSnsDft->u32FullLinesStd = gu32FullLinesStd;
    
    return;
}

static HI_VOID cmos_slow_framerate_set(HI_U16 u16FullLines,
    AE_SENSOR_DEFAULT_S *pstAeSnsDft)
{
/* Mode 1 : slow framerate by add the time of each line. */
#if CMOS_OV9712_SLOW_FRAMERATE_MODE
    HI_U32 u32Tp = 50760;   /* (0x69c * 30) = 50760*/
    HI_U16 u16SlowFrameRate = (u16FullLines << 8) / gu32FullLinesStd;
    u32Tp = (((u32Tp * u16SlowFrameRate) / gu8Fps) >> 8) + 3;
    if (u32Tp > 0x2000)     /* the register 0x2a adn 0x2b's max value is 0x2000 */
    {
        u32Tp = 0x2000;
        u16SlowFrameRate = ((gu8Fps * u32Tp) << 8) / 50760;
        printf("Warning! The slow_framerate is out of ov9712's range!\n");
    }

    
    pstAeSnsDft->u32LinesPer500ms = gu32FullLines * (gu8Fps << 8) / (2*u16SlowFrameRate);
    pstAeSnsDft->u32MaxIntTime = ((gu32FullLines * u16SlowFrameRate) >> 8) - 4;
    #if CMOS_OV9712_ISP_WRITE_SENSOR_ENABLE
    g_stSnsRegsInfo.u32RegNum = 5;
    g_stSnsRegsInfo.astI2cData[3].u32Data = u32Tp & 0xfc;
    g_stSnsRegsInfo.astI2cData[4].u32Data = (u32Tp & 0xff00) >> 8;
    #else
    sensor_write_register(0x2a, u32Tp & 0xfc);
    sensor_write_register(0x2b, (u32Tp & 0xff00) >> 8);    
    #endif
#else
/* Mode 2 : slow framerate by add the lines of each frame. */
    #if CMOS_OV9712_ISP_WRITE_SENSOR_ENABLE
    HI_U32 u32VblankingLines;
    static HI_U32 u32LastVblankingLines = 0;

    gu32FullLines = u16FullLines;
    u32VblankingLines = gu32FullLines - gu32FullLinesStd;

    /*avoid flicker in slow frame rate*/
    if(u32LastVblankingLines < u32VblankingLines)
    {
        g_stSnsRegsInfo.astI2cData[3].bDelayCfg = HI_FALSE;
        g_stSnsRegsInfo.astI2cData[4].bDelayCfg = HI_FALSE;        
    }
    else
    {
        g_stSnsRegsInfo.astI2cData[3].bDelayCfg = HI_TRUE;
        g_stSnsRegsInfo.astI2cData[4].bDelayCfg = HI_TRUE;
    }
    g_stSnsRegsInfo.u32RegNum = 5;
    g_stSnsRegsInfo.astI2cData[3].u32Data = u32VblankingLines & 0xff;
    g_stSnsRegsInfo.astI2cData[4].u32Data = (u32VblankingLines & 0xff00) >> 8;
    u32LastVblankingLines = u32VblankingLines;
    #else
    HI_U32 u32VblankingLines;

    gu32FullLines = u16FullLines;
    u32VblankingLines = gu32FullLines - gu32FullLinesStd;
    
    sensor_write_register(0x2d, u32VblankingLines & 0xff);
    sensor_write_register(0x2e, (u32VblankingLines & 0xff00) >> 8);
    #endif
    
    pstAeSnsDft->u32MaxIntTime = gu32FullLines - 4;
#endif
    return;
}

static HI_VOID cmos_init_regs_info(HI_VOID)
{
#if CMOS_OV9712_ISP_WRITE_SENSOR_ENABLE
    HI_S32 i;
    static HI_BOOL bInit = HI_FALSE;

    if (HI_FALSE == bInit)
    {
        g_stSnsRegsInfo.enSnsType = ISP_SNS_I2C_TYPE;
        g_stSnsRegsInfo.u32RegNum = 3;
        for (i=0; i<5; i++)
        {
            g_stSnsRegsInfo.astI2cData[i].u8DevAddr = sensor_i2c_addr;
            g_stSnsRegsInfo.astI2cData[i].u32AddrByteNum = sensor_addr_byte;
            g_stSnsRegsInfo.astI2cData[i].u32DataByteNum = sensor_data_byte;
        }
        g_stSnsRegsInfo.astI2cData[0].bDelayCfg = HI_FALSE;
        g_stSnsRegsInfo.astI2cData[0].u32RegAddr = 0x10;
        g_stSnsRegsInfo.astI2cData[1].bDelayCfg = HI_FALSE;
        g_stSnsRegsInfo.astI2cData[1].u32RegAddr = 0x16;
        g_stSnsRegsInfo.astI2cData[2].bDelayCfg = HI_TRUE;
        g_stSnsRegsInfo.astI2cData[2].u32RegAddr = 0x00;
    #if CMOS_OV9712_SLOW_FRAMERATE_MODE
        g_stSnsRegsInfo.astI2cData[3].bDelayCfg = HI_TRUE;
        g_stSnsRegsInfo.astI2cData[3].u32RegAddr = 0x2a;
        g_stSnsRegsInfo.astI2cData[4].bDelayCfg = HI_TRUE;
        g_stSnsRegsInfo.astI2cData[4].u32RegAddr = 0x2b;
    #else
        g_stSnsRegsInfo.astI2cData[3].bDelayCfg = HI_FALSE;
        g_stSnsRegsInfo.astI2cData[3].u32RegAddr = 0x2d;
        g_stSnsRegsInfo.astI2cData[4].bDelayCfg = HI_FALSE;
        g_stSnsRegsInfo.astI2cData[4].u32RegAddr = 0x2e;
    #endif
        g_stSnsRegsInfo.bDelayCfgIspDgain = HI_TRUE;

        bInit = HI_TRUE;
    }
#endif
    return;
}

/* while isp notify ae to update sensor regs, ae call these funcs. */
static HI_VOID cmos_inttime_update(HI_U32 u32IntTime)
{
#if CMOS_OV9712_ISP_WRITE_SENSOR_ENABLE
    cmos_init_regs_info();
    g_stSnsRegsInfo.astI2cData[0].u32Data = u32IntTime & 0xFF;
    g_stSnsRegsInfo.astI2cData[1].u32Data = (u32IntTime >> 8) & 0xFF;
#else 
    //refresh the sensor setting every frame to avoid defect pixel error
    sensor_write_register(0x10, u32IntTime&0xFF);
    sensor_write_register(0x16, (u32IntTime>>8)&0xFF);
#endif
    return;
}

static HI_VOID cmos_gains_update(HI_U32 u32Again, HI_U32 u32Dgain)
{
    HI_U8 u8High, u8Low;
    switch (u32Again)
    {
        case 0 :    /* 0db, 1 multiplies */
            u8High = 0x00;
            break;
        case 1 :    /* 6db, 2 multiplies */
            u8High = 0x10;
            break;
        case 2 :    /* 12db, 4 multiplies */
            u8High = 0x30;
            break;
        case 3 :    /* 18db, 8 multiplies */
            u8High = 0x70;
            break;
        case 4 :    /* 24db, 16 multiplies */
            u8High = 0xf0;
            break;
        default:
            u8High = 0x00;
            break;
    }

    u8Low = (u32Dgain - 16) & 0xf;

#if CMOS_OV9712_ISP_WRITE_SENSOR_ENABLE
    cmos_init_regs_info();
    g_stSnsRegsInfo.astI2cData[2].u32Data = (u8High | u8Low);
    HI_MPI_ISP_SnsRegsCfg(&g_stSnsRegsInfo);
    if (5 == g_stSnsRegsInfo.u32RegNum)
    {
        g_stSnsRegsInfo.u32RegNum = 3;
    }
#else
    sensor_write_register(0x00, (u8High | u8Low));
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

    pstAwbSnsDft->au16GainOffset[0] = 0x015b;
    pstAwbSnsDft->au16GainOffset[1] = 0x100;
    pstAwbSnsDft->au16GainOffset[2] = 0x100;
    pstAwbSnsDft->au16GainOffset[3] = 0x0199;

    pstAwbSnsDft->as32WbPara[0] = 127;
    pstAwbSnsDft->as32WbPara[1] = -23;
    pstAwbSnsDft->as32WbPara[2] = -152;
    pstAwbSnsDft->as32WbPara[3] = 154393;
    pstAwbSnsDft->as32WbPara[4] = 128;
    pstAwbSnsDft->as32WbPara[5] = -105036;

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
    s32Ret = HI_MPI_ISP_SensorRegCallBack(OV9712_ID, &stIspRegister);
    if (s32Ret)
    {
        printf("sensor register callback function failed!\n");
        return s32Ret;
    }
    
    stLib.s32Id = 0;
    strcpy(stLib.acLibName, HI_AE_LIB_NAME);
    cmos_init_ae_exp_function(&stAeRegister.stSnsExp);
    s32Ret = HI_MPI_AE_SensorRegCallBack(&stLib, OV9712_ID, &stAeRegister);
    if (s32Ret)
    {
        printf("sensor register callback function to ae lib failed!\n");
        return s32Ret;
    }

    stLib.s32Id = 0;
    strcpy(stLib.acLibName, HI_AWB_LIB_NAME);
    cmos_init_awb_exp_function(&stAwbRegister.stSnsExp);
    s32Ret = HI_MPI_AWB_SensorRegCallBack(&stLib, OV9712_ID, &stAwbRegister);
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

    s32Ret = HI_MPI_ISP_SensorUnRegCallBack(OV9712_ID);
    if (s32Ret)
    {
        printf("sensor unregister callback function failed!\n");
        return s32Ret;
    }
    
    stLib.s32Id = 0;
    strcpy(stLib.acLibName, HI_AE_LIB_NAME);
    s32Ret = HI_MPI_AE_SensorUnRegCallBack(&stLib, OV9712_ID);
    if (s32Ret)
    {
        printf("sensor unregister callback function to ae lib failed!\n");
        return s32Ret;
    }

    stLib.s32Id = 0;
    strcpy(stLib.acLibName, HI_AWB_LIB_NAME);
    s32Ret = HI_MPI_AWB_SensorUnRegCallBack(&stLib, OV9712_ID);
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


#endif // __OV9715_CMOS_H_
