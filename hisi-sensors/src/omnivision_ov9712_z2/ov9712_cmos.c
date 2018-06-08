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
    5300,
    {   0x0254, 0x8102, 0x8052,
        0x8041, 0x01d5, 0x8094,
        0x8027, 0x8136, 0x025d
    },
    3558,
    {   0x02a0, 0x8125, 0x807b,
        0x8043, 0x0156, 0x8013,
        0x802a, 0x8268, 0x0392
    },
    3084,
    {   0x023a, 0x808d, 0x80ad,
        0x8033, 0x01c0, 0x808d,
        0x8071, 0x82af, 0x0420
    }
};

static AWB_AGC_TABLE_S g_stAwbAgcTable =
{
    /* bvalid */
    1,

    /* saturation */
    {0x80,0x80,0x80,0x80,0x68,0x48,0x35,0x30}
};


static ISP_CMOS_AGC_TABLE_S g_stIspAgcTable =
{
    /* bvalid */
    1,
    
#if CMOS_OV9712_MORE_SHARPEN
    //sharpen_alt_d
    {0x50,0x4b,0x46,0x41,0x37,0x2c,0x1e,0xf},

    //sharpen_alt_ud
    {0x4b,0x46,0x41,0x3c,0x32,0x28,0x19,0xa},

    //snr_thresh
    {0x10,0x14,0x1a,0x22,0x2a,0x37,0x46,0x50},
#else    
    /* sharpen_alt_d */
    {0x8e,0x8b,0x88,0x83,0x7d,0x76,0x75,0x74},
        
    /* sharpen_alt_ud */
    {0x8f,0x89,0x7e,0x78,0x6f,0x44,0x40,0x35},
        
    /* snr_thresh */
    {0x19,0x1e,0x2d,0x32,0x39,0x3f,0x48,0x4b},
#endif    
        
    /* demosaic_lum_thresh */
    {0x50,0x50,0x40,0x40,0x30,0x30,0x20,0x20},
        
    /* demosaic_np_offset */
    {0x0,0xa,0x12,0x1a,0x20,0x28,0x30,0x30},
        
    /* ge_strength */
    {0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55}

};

static ISP_CMOS_NOISE_TABLE_S g_stIspNoiseTable =
{
    /* bvalid */
    1,
    
    /* nosie_profile_weight_lut */
    {0, 27, 31, 33, 35, 36, 37, 38, 39, 40, 40, 41, 41, 42, 42, 43,
    43, 43, 44, 44, 44, 45, 45, 45, 45, 46, 46, 46, 46, 46, 47, 47,
    47, 47, 47, 48, 48, 48, 48, 48, 48, 48, 49, 49, 49, 49, 49, 49,
    49, 49, 50, 50, 50, 50, 50, 50, 50, 50, 50, 51, 51, 51, 51, 51,
    51, 51, 51, 51, 51, 51, 51, 52, 52, 52, 52, 52, 52, 52, 52, 52,
    52, 52, 52, 52, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53,
    53, 53, 53, 53, 54, 54, 54, 54, 54, 54, 54, 54, 54, 54, 54, 54,
    54, 54, 54, 54, 54, 54, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55},

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
    0xcd,

    /*aa_slope*/
    0xbf,
#if CMOS_OV9712_MORE_SHARPEN
    /*va_slope*/
    0xc1,

    /*uu_slope*/
    0xc8,
#else
    /*va_slope*/
    0xc1,

    /*uu_slope*/
    0xa0,
#endif
    /*sat_slope*/
    0x5d,

    /*ac_slope*/
    0xcf,

    /*vh_thresh*/
    0x10,

    /*aa_thresh*/
    0x10,

    /*va_thresh*/
    0x10,

    /*uu_thresh*/
    0xa,

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
    0x27a, 0x168,

    /*shading_center_g*/
    0x276, 0x16f,

    /*shading_center_b*/
    0x27a, 0x16c,

    /*shading_table_r*/
    {0x1000,0x1018,0x1028,0x103a,0x104c,0x105c,0x1072,0x1089,0x109e,0x10ba,0x10d5,0x10ef,
    0x110b,0x112b,0x114c,0x116d,0x118b,0x11ae,0x11d0,0x11f5,0x1218,0x123e,0x1260,0x1283,
    0x12aa,0x12cf,0x12f7,0x131b,0x1341,0x1369,0x138f,0x13b5,0x13db,0x1401,0x1423,0x1446,
    0x146d,0x148f,0x14b4,0x14d7,0x14fe,0x151e,0x153e,0x155a,0x1579,0x159a,0x15b7,0x15d3,
    0x15f4,0x1612,0x162e,0x164d,0x1663,0x167f,0x169a,0x16b1,0x16cb,0x16e4,0x16fc,0x170f,
    0x1727,0x173e,0x1753,0x176a,0x1783,0x1793,0x17a5,0x17b8,0x17c9,0x17da,0x17ec,0x17fe,
    0x180d,0x181a,0x182a,0x183b,0x184c,0x185a,0x1865,0x1876,0x1883,0x1890,0x189f,0x18a9,
    0x18b5,0x18c3,0x18cf,0x18d8,0x18e2,0x18e8,0x18ec,0x18f5,0x1901,0x190e,0x191f,0x1934,
    0x1946,0x1955,0x1968,0x197e,0x1993,0x19a4,0x19b5,0x19cc,0x19e1,0x19f5,0x1a06,0x1a16,
    0x1a2a,0x1a3d,0x1a4f,0x1a5d,0x1a6e,0x1a84,0x1a96,0x1aa7,0x1abb,0x1ad2,0x1ae5,0x1af9,
    0x1b0e,0x1b27,0x1b40,0x1b59,0x1b68,0x1b72,0x1b91,0x1bbf,0x1bf6},

    /*shading_table_g*/
    {0x1000,0x1013,0x1022,0x1033,0x1043,0x1054,0x1066,0x107b,0x108e,0x10a5,0x10bc,0x10d7,
    0x10f1,0x110c,0x112a,0x114b,0x116b,0x118c,0x11ae,0x11d0,0x11f3,0x1216,0x1238,0x125c,
    0x1282,0x12a9,0x12ce,0x12f3,0x131b,0x133f,0x1363,0x1386,0x13a9,0x13cc,0x13f0,0x1412,
    0x1434,0x1457,0x1479,0x149c,0x14bc,0x14da,0x14fa,0x1519,0x1537,0x1557,0x1575,0x1590,
    0x15ac,0x15c8,0x15e1,0x15fd,0x1617,0x162f,0x1648,0x165f,0x1673,0x168b,0x16a1,0x16b5,
    0x16c9,0x16db,0x16ee,0x1702,0x1714,0x1726,0x1736,0x1744,0x1752,0x1760,0x176c,0x1778,
    0x1785,0x1794,0x179e,0x17a8,0x17b5,0x17c1,0x17ca,0x17d5,0x17e1,0x17e9,0x17ef,0x17f3,
    0x17f9,0x17ff,0x1800,0x1801,0x1803,0x1806,0x180d,0x181c,0x182f,0x183e,0x184a,0x1854,
    0x185e,0x186a,0x1878,0x188b,0x189b,0x18a8,0x18b5,0x18c2,0x18d0,0x18e0,0x18ef,0x18fc,
    0x1907,0x1910,0x191a,0x1927,0x1936,0x1941,0x194a,0x1958,0x1965,0x1971,0x197e,0x1989,
    0x199c,0x19aa,0x19b7,0x19bd,0x19cc,0x19cc,0x19ce,0x19f6,0x1a0a},

    /*shading_table_b*/
    {0x1000,0x1012,0x101c,0x1025,0x102c,0x1031,0x103b,0x1045,0x104f,0x1059,0x1060,0x1072,
    0x107f,0x108d,0x109f,0x10b5,0x10c9,0x10dc,0x10ee,0x1109,0x111c,0x1137,0x1150,0x1167,
    0x1183,0x119e,0x11bb,0x11d5,0x11f4,0x120c,0x1228,0x1242,0x125a,0x1276,0x1292,0x12ac,
    0x12c7,0x12e0,0x12f9,0x1313,0x132d,0x1346,0x135d,0x1372,0x138a,0x13a4,0x13bb,0x13cf,
    0x13e5,0x13fb,0x1410,0x1427,0x143c,0x1450,0x1462,0x1475,0x1488,0x1496,0x14a5,0x14b7,
    0x14ca,0x14d7,0x14e5,0x14f4,0x1503,0x1511,0x151d,0x1529,0x1532,0x153c,0x1548,0x1552,
    0x155b,0x1566,0x156c,0x1573,0x157d,0x1587,0x158d,0x1593,0x159b,0x15a3,0x15a9,0x15ac,
    0x15b1,0x15b4,0x15b5,0x15b5,0x15b6,0x15b5,0x15b5,0x15ba,0x15c4,0x15d3,0x15e2,0x15ef,
    0x15f9,0x1604,0x1611,0x161e,0x1626,0x1632,0x1641,0x164c,0x165b,0x1667,0x1672,0x1679,
    0x1681,0x168e,0x1699,0x16a0,0x16aa,0x16bc,0x16ce,0x16dc,0x16e9,0x16f1,0x16f9,0x170c,
    0x1722,0x1735,0x173d,0x1739,0x1739,0x173c,0x173c,0x1731,0x1724},

    /*shading_off_center_r_g_b*/
    0xf57, 0xf0e, 0xf42,

    /*shading_table_nobe_number*/
    129
};

static ISP_CMOS_GAMMA_S g_stIspGamma =
{
    /* bvalid */
    1,
    
    {0  ,27  ,60  ,100 ,140 ,178 ,216 ,242 ,276 ,312 ,346 ,380 ,412 ,444 ,476 ,508,
    540 ,572 ,604 ,636 ,667 ,698 ,729 ,760 ,791 ,822 ,853 ,884 ,915 ,945 ,975 ,1005,
    1035,1065,1095,1125,1155,1185,1215,1245,1275,1305,1335,1365,1395,1425,1455,1485,
    1515,1544,1573,1602,1631,1660,1689,1718,1746,1774,1802,1830,1858,1886,1914,1942,
    1970,1998,2026,2054,2082,2110,2136,2162,2186,2220,2244,2268,2292,2316,2340,2362,
    2384,2406,2428,2448,2468,2488,2508,2528,2548,2568,2588,2608,2628,2648,2668,2688,
    2708,2728,2748,2768,2788,2808,2828,2846,2862,2876,2890,2903,2917,2930,2944,2957,
    2970,2983,2996,3008,3021,3033,3046,3058,3070,3082,3094,3106,3118,3129,3141,3152,
    3164,3175,3186,3197,3208,3219,3230,3240,3251,3262,3272,3282,3293,3303,3313,3323,
    3333,3343,3352,3362,3372,3381,3391,3400,3410,3419,3428,3437,3446,3455,3464,3473,
    3482,3490,3499,3508,3516,3525,3533,3541,3550,3558,3566,3574,3582,3590,3598,3606,
    3614,3621,3629,3637,3644,3652,3660,3667,3674,3682,3689,3696,3703,3711,3718,3725,
    3732,3739,3746,3752,3759,3766,3773,3779,3786,3793,3799,3806,3812,3819,3825,3831,
    3838,3844,3850,3856,3863,3869,3875,3881,3887,3893,3899,3905,3910,3916,3922,3928,
    3933,3939,3945,3950,3956,3962,3967,3973,3978,3983,3989,3994,3999,4005,4010,4015,
    4020,4026,4031,4036,4041,4046,4051,4056,4061,4066,4071,4076,4081,4085,4090,4095,4095}
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

    pstBlackLevel->au16BlackLevel[0] = 64;
    pstBlackLevel->au16BlackLevel[1] = 64;
    pstBlackLevel->au16BlackLevel[2] = 64;
    pstBlackLevel->au16BlackLevel[3] = 64;

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
