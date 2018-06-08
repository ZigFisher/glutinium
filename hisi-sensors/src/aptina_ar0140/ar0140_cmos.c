#if !defined(__AR0140_CMOS_H_)
#define __AR0140_CMOS_H_

#include <stdio.h>
#include <string.h>
#include "hi_comm_sns.h"
#include "hi_sns_ctrl.h"
#include "mpi_isp.h"
#include "mpi_ae.h"
#include "mpi_awb.h"
#include "mpi_af.h"

#include "ar0140_sensor_config.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* End of #ifdef __cplusplus */

#define AR0140_ID 0140

/*set Frame End Update Mode 2 with HI_MPI_ISP_SetAEAttr and set this value 1 to avoid flicker in antiflicker mode */
/*when use Frame End Update Mode 2, the speed of i2c will affect whole system's performance                       */
/*increase I2C_DFT_RATE in Hii2c.c to 400000 to increase the speed of i2c                                         */
#define CMOS_AR0140_ISP_WRITE_SENSOR_ENABLE (1)
/****************************************************************************
 * local variables                                                            *
 ****************************************************************************/

extern const unsigned int sensor_i2c_addr;
extern unsigned int sensor_addr_byte;
extern unsigned int sensor_data_byte;


HI_U8 gu8SensorMode = 0;
static HI_U32 gu32FullLinesStd = 750;

static HI_U32 gu32SensorTotalGain = 1024;
static HI_U32 gu32SensorAnalogGain = 1024;

#if CMOS_AR0140_ISP_WRITE_SENSOR_ENABLE
ISP_SNS_REGS_INFO_S g_stSnsRegsInfo = {0};
#endif

//#define AGAIN_TABLE_NUM 78
#define AGAIN_TABLE_NUM 81

static AWB_CCM_S g_stAwbCcm =
{
    4850,
    {
        0x0170,0x8053,0x801d,
        0x8033,0x0128,0x000a,
        0x0010,0x808a,0x0179
    },

    3160,
    {
        0x162,0x8036,0x802c,
        0x804c,0x133,0x0018,
        0x000f,0x80d1,0x1c1
    },

    2470,
    {
        0x0142,0x6,0x8049,
        0x802a,0x00ee,0x003b,
        0x8007,0x80fc,0x0203
    }
};

static AWB_AGC_TABLE_S g_stAwbAgcTableLin =
{
    /* bvalid */
    1,

    /* saturation */
    {0x80,0x78,0x70,0x60,0x50,0x40,0x30,0x20}
};

static AWB_AGC_TABLE_S g_stAwbAgcTableWdr =
{
    /* bvalid */
    1,

    /* saturation */
    {0x80,0x80,0x78,0x70,0x60,0x50,0x40,0x30}
};


static ISP_CMOS_AGC_TABLE_S g_stIspAgcTableLin =
{
    /* bvalid */
    1,

    /* sharpen_alt_d */
    {0xa2,0xa0,0x8a,0x70,0x60,0x58,0x50,0x48},
        
    /* sharpen_alt_ud */
    {0xae,0xab,0x98,0x70,0x60,0x5a,0x50,0x48},
        
    /* snr_thresh */
    {0xc,0x10,0x28,0x3a,0x46,0x50,0x54,0x5A},
        
    /* demosaic_lum_thresh */
    {0x60,0x60,0x80,0x80,0x80,0x80,0x80,0x80},
        
    /* demosaic_np_offset */
    {0x0,0xa,0x12,0x1a,0x20,0x28,0x30,0x37},
        
    /* ge_strength */
    {0x55,0x55,0x55,0x55,0x55,0x55,0x37,0x37}
};

static ISP_CMOS_AGC_TABLE_S g_stIspAgcTableWdr =
{
    /* bvalid */
    1,

    /* sharpen_alt_d */
    {0x50,0x50,0x50,0x50,0x4a,0x45,0x40,0x3a},
        
    /* sharpen_alt_ud */
    {0x90,0x90,0x90,0x90,0x88,0x80,0x78,0x70},
        
    /* snr_thresh */
    {0x05,0x05,0x05,0x05,0x08,0x0c,0x10,0x18},
        
    /* demosaic_lum_thresh */
    {0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80},
        
    /* demosaic_np_offset */
    {0x0,0xa,0x12,0x1a,0x20,0x28,0x30,0x37},
        
    /* ge_strength */
    {0x55,0x55,0x55,0x55,0x55,0x55,0x37,0x37}
};


static ISP_CMOS_NOISE_TABLE_S g_stIspNoiseTableLin =
{
    /* bvalid */
    1,
    
      /* nosie_profile_weight_lut */
    /*
     {
      0,0,0,0,0,6,13,18,21,23,25,27,28,30,31,32,33,34,35,36,36,37,38,38,39,39,40,41,41,
	41,42,42,43,43,44,44,44,45,45,45,46,46,46,47,47,47,47,48,48,48,48,49,49,49,49,50,
	50,50,50,51,51,51,51,51,52,52,52,52,52,52,53,53,53,53,53,53,54,54,54,54,54,54,55,
	55,55,55,55,55,55,56,56,56,56,56,56,56,56,57,57,57,57,57,57,57,57,58,58,58,58,58,
	58,58,58,58,59,59,59,59,59,59,59,59,59,59,60,60,60,60 
     }, 
     */
      	{0, 0, 0, 0, 0 ,3, 12, 17, 20, 23, 25, 27, 28, 30 ,31 ,32, 33, 34, 35 ,35, 37 ,37, 38, 38, 39, 40, 40, 41, 41,
      	42, 42, 43, 43, 43, 44, 44, 44, 45, 45, 46, 46, 46, 46, 47, 47, 47, 48 ,48, 48, 48 ,49, 49, 49, 49, 50, 50,
      	50, 50, 50, 51, 51, 51 ,51 ,52, 52, 52, 52 ,52, 52, 53, 53, 53, 53, 53, 53, 54 ,54, 54, 54, 54, 54 ,55, 55,
      55,55,55,55,55,56,56,56,56,56,56,56,56,57,57,57,57,57,57,57,57,58,58,58,58,58,58,
      58,58,58,59,59,59,59,59,59,59,59,59,59,60,60,60,60,60
      	},
    
     /* demosaic_weight_lut */
    /*
    {
       0,6,13,18,21,23,25,27,28,30,31,32,33,34,35,36,36,37,38,38,39,39,40,41,41,
	41,42,42,43,43,44,44,44,45,45,45,46,46,46,47,47,47,47,48,48,48,48,49,49,49,49,50,
	50,50,50,51,51,51,51,51,52,52,52,52,52,52,53,53,53,53,53,53,54,54,54,54,54,54,55,
	55,55,55,55,55,55,56,56,56,56,56,56,56,56,57,57,57,57,57,57,57,57,58,58,58,58,58,
	58,58,58,58,59,59,59,59,59,59,59,59,59,59,60,60,60,60, 60,60,60,60
    }
    */
	{
	3,12,17,20,23,25,27,28,30,31,32,33,34,35,35,37,37,38,38,39,40,40,41,41,42,42,43,43,
	43,44,44,44,45,45,46,46,46,46,47,47,47,48,48,48,48,49,49,49,49,50,50,50,50,50,
	51,51,51,51,52,52,52,52,52,52,53,53,53,53,53,53,54,54,54,54,54,54,55,55,55,55,
	55,55,55,56,56,56,56,56,56,56,56,57,57,57,57,57,57,57,57,58,58,58,58,58,58,58,
	58,58,59,59,59,59,59,59,59,59,59,59,60,60,60,60,60,60,60,60,60,60
   	}
      
};

#if 0
static ISP_CMOS_NOISE_TABLE_S g_stIspNoiseTableWdr =
{
    /* bvalid */
    1,
    
    /* nosie_profile_weight_lut */
    {
        13,13,13,13,13,14,15,25,31,31,31,31,31,31,31,31,31,31,31,31,31,32,32,32,32,32,32,32,39,49,54,56,58,59,59,59,59,59,59,59,59,59,59,59,59,59,59,59,59,59,59,59,59,59,59,59,59,59,59,59,59,59,59,59,59,59,59,59,59,59,59,59,59,59,59,59,59,59,59,59,59,59,59,59,59,59,59,59,59,59,59,59,59,59,59,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60
    },

    /* demosaic_weight_lut */
    {
        13,13,13,13,13,14,15,25,31,31,31,31,31,31,31,31,31,31,31,31,31,32,32,32,32,32,32,32,39,49,54,56,58,59,59,59,59,59,59,59,59,59,59,59,59,59,59,59,59,59,59,59,59,59,59,59,59,59,59,59,59,59,59,59,59,59,59,59,59,59,59,59,59,59,59,59,59,59,59,59,59,59,59,59,59,59,59,59,59,59,59,59,59,59,59,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60
    }
};
#endif

static ISP_CMOS_NOISE_TABLE_S g_stIspNoiseTableWdr =
{
    /* bvalid */
    1,
    
    /* nosie_profile_weight_lut WDR */
    {
        13,13,13,13,13,14,15,25,31,31,31,31,31,31,31,31,31,31,31,31,31,32,32,32,
        32,32,32,32,39,49,54,56,58,59,59,59,59,59,59,59,59,59,59,59,59,59,59,59,59,
        59,59,59,59,59,59,59,59,59,59,59,59,59,59,59,59,59,59,59,59,59,59,59,59,59,59,
        59,59,59,59,59,59,59,59,59,59,59,59,59,59,59,59,59,59,59,59,60,60,60,60,60,60,60,60,
        60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60
    },

    /* demosaic_weight_lut WDR */
    {
        13,13,13,13,13,14,15,25,31,31,31,31,31,31,31,31,31,31,31,31,31,32,32,32,
        32,32,32,32,39,49,54,56,58,59,59,59,59,59,59,59,59,59,59,59,59,59,59,
        59,59,59,59,59,59,59,59,59,59,59,59,59,59,59,59,59,59,59,59,59,59,59,59,59,
        59,59,59,59,59,59,59,59,59,59,59,59,59,59,59,59,59,59,59,59,59,59,59,60,60,60,
        60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,
        60,60,60
    }
};


static ISP_CMOS_DEMOSAIC_S g_stIspDemosaicLin =
{
    /* bvalid */
    1,
    
    /*vh_slope*/
    0xB9,

    /*aa_slope*/
    0x9B,

    /*va_slope*/
    0x9E,

    /*uu_slope*/
    0xA2,

    /*sat_slope*/
    0x5D,

    /*ac_slope*/
    0xCF,

    /*vh_thresh*/
    0x0,

    /*aa_thresh*/
    0x0,

    /*va_thresh*/
    0x0,

    /*uu_thresh*/
    0x8,

    /*sat_thresh*/
    0x171,

    /*ac_thresh*/
    0x1b3
};

static ISP_CMOS_DEMOSAIC_S g_stIspDemosaicWdr =
{
    /* bvalid */
    1,
    
    /*vh_slope*/
    0xA0,

    /*aa_slope*/
    0x6E,

    /*va_slope*/
    0x96,

    /*uu_slope*/
    0x78,

    /*sat_slope*/
    0x5D,

    /*ac_slope*/
    0xCF,

    /*vh_thresh*/
    0x78,

    /*aa_thresh*/
    0x73,

    /*va_thresh*/
    0x6E,

    /*uu_thresh*/
    0x67,

    /*sat_thresh*/
    0x171,

    /*ac_thresh*/
    0x1b3
};


static ISP_CMOS_GAMMAFE_S g_stGammafe = 
{
    /* bvalid */
    1,

    { 
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 45, 78, 101, 120, 136, 150, 163, 175, 187, 197, 208, 217, 227, 235,
	244, 252, 260, 268, 276, 283, 290, 297, 304, 311, 317, 324, 330, 336, 342, 348, 354, 360, 365, 
	371, 376, 382, 387, 392, 398, 403, 408, 413, 418, 423, 427, 432, 437, 442, 446, 451, 455, 460, 
	464, 469, 477, 486, 494, 502, 511, 519, 526, 534, 542, 549, 557, 564, 571, 578, 585, 592, 599, 
	606, 613, 619, 626, 633, 639, 645, 652, 658, 664, 670, 677, 683, 689, 694, 700, 706, 712, 718, 
	723, 729, 735, 740, 746, 751, 757, 762, 767, 773, 778, 783, 789, 794, 799, 804, 809, 814, 819, 
	824, 829, 834, 839, 844, 849, 854, 858, 863, 868, 873, 877, 882, 887, 891, 896, 900, 905, 909, 
	914, 918, 923, 927, 932, 936, 940, 945, 949, 953, 958, 962, 966, 971, 975, 979, 983, 987, 991,
	996, 1000, 1004, 1067, 1127, 1184, 1238, 1290, 1340, 1388, 1435, 1480, 1524, 1566, 1607, 1648, 
	1687, 1726, 1763, 1800, 1836, 1872, 1906, 1941, 1974, 2007, 2040, 2072, 2103, 2134, 2165, 2195, 
	2224, 2254, 2283, 2311, 2340, 2367, 2395, 2422, 2449, 2476, 2502, 2528, 2554, 2580, 2605, 2630, 
	2655, 2680, 2704, 2728, 2752, 2776, 2800, 2823, 2846, 2869, 2892, 2937, 2981, 3025, 3068, 3111, 
	3153, 3194, 3235, 3275, 3315, 3354, 3393, 3432, 3470, 3508, 3545, 3582, 3618, 3654, 3690, 3726,
	3761, 3795, 3830, 3864, 3898, 3931, 3965, 3998, 4030, 4063, 4095, 4095, 4095, 4095, 4095, 4095,
	4095, 4095, 4095,
    }
};

static ISP_CMOS_GAMMA_S g_stIspGamma =
{
    /* bvalid */
    1,
    
#if 0    
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


static ISP_CMOS_GAMMA_S g_stGammaWdr =
{
    /* bvalid */
    1,
    
    {0, 0, 1, 2, 3, 5, 8, 10, 14, 17, 21, 26, 30, 36, 41, 47, 54, 61, 68, 75, 83, 92, 100, 109, 119, 129, 139,
    150, 161, 173, 184, 196, 209, 222, 235, 248, 262, 276, 290, 305, 320, 335, 351, 366, 382, 399, 415,
    433, 450, 467, 484, 502, 520, 539, 557, 576, 595, 614, 634, 653, 673, 693, 714, 734, 754, 775, 796, 
    816, 837, 858, 879, 901, 923, 944, 966, 988, 1010, 1032, 1054, 1076, 1098, 1120, 1142, 1165, 1188,
    1210, 1232, 1255, 1278, 1301, 1324, 1346, 1369, 1391, 1414, 1437, 1460, 1483, 1505, 1528, 1551, 1574, 
    1597, 1619, 1642, 1665, 1687, 1710, 1732, 1755, 1777, 1799, 1822, 1845, 1867, 1889, 1911, 1933, 1955, 
    1977, 1999, 2021, 2043, 2064, 2086, 2108, 2129, 2150, 2172, 2193, 2214, 2236, 2256, 2277, 2298, 2319, 
    2340, 2360, 2380, 2401, 2421, 2441, 2461, 2481, 2501, 2521, 2541, 2560, 2580, 2599, 2618, 2637, 2656, 
    2675, 2694, 2713, 2732, 2750, 2769, 2787, 2805, 2823, 2841, 2859, 2877, 2895, 2912, 2929, 2947, 2964, 
    2982, 2999, 3015, 3032, 3049, 3066, 3082, 3099, 3115, 3131, 3147, 3164, 3179, 3195, 3211, 3227, 3242, 
    3258, 3273, 3288, 3303, 3318, 3333, 3348, 3362, 3377, 3392, 3406, 3420, 3434, 3448, 3462, 3477, 3490,
    3504, 3517, 3531, 3544, 3558, 3571, 3584, 3597, 3611, 3623, 3636, 3649, 3662, 3674, 3686, 3698, 3711, 
    3723, 3736, 3748, 3759, 3771, 3783, 3795, 3806, 3818, 3829, 3841, 3852, 3863, 3874, 3885, 3896, 3907, 
    3918, 3929, 3939, 3949, 3961, 3971, 3981, 3991, 4001, 4012, 4022, 4032, 4042, 4051, 4061, 4071, 4081, 
    4090, 4095,}
};

#if 0
static  HI_U32   au32Again_table[AGAIN_TABLE_NUM] = 
{ 
    1024,1088,1152,1216,1280,1344,1408,1472,1536,1600,1664,1728,1792,1856,1920,1984,2048,2176,2304,
    2432,2560,2662,2828,2995,3161,3328,3494,3660,3827,3993,4160,4326,4492,4659,4825,4992,5158,5324,
    5657,5990,6323,6656,6988,7321,7654,7987,8320,8652,8985,9318,9651,9984,10316,10649,11315,11980,
    12646,13312,13977,14643,15308,15974,16640,17305,17971,18636,19302,19968,20633,21299,22630,23961,
    25292,26624,27955,29286,30617,31948
};
#endif

static  HI_U32   au32Again_table[AGAIN_TABLE_NUM] = 
{
    1024,1088,1152,1216,1280,1344,1408,1472,1536,1600,1664,1728,1792,1856,1920,1984,2048,2176,2304,
    2432,2560,2688,2816,2944,3041,3231,3421,3611,3801,3991,4181,4371,4561,4752,4942,5132,5322,5512,
    5702,5892,6082,6462,6842,7223,7603,7983,8363,8743,9123,9504,9884,10264,10644,11024,11404,11784,
    12165,12925,13685,14446,15206,15966,16727,17487,18247,19008,19768,20528,21288,22049,22809,23569,
    24330,25850,27371,28892,30412,31933,33454,34974,36495
};

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
   
    if (u32InTimes >= au32Again_table[AGAIN_TABLE_NUM -1])
    {
         pstAeSnsGainInfo->u32SnsTimes = au32Again_table[AGAIN_TABLE_NUM -1];
         pstAeSnsGainInfo->u32GainDb = AGAIN_TABLE_NUM -1;
         gu32SensorAnalogGain = pstAeSnsGainInfo->u32SnsTimes;
         return ;
    }
    
    for(i = 1; i < AGAIN_TABLE_NUM; i++)
    {
        if(u32InTimes < au32Again_table[i])
        {
            pstAeSnsGainInfo->u32SnsTimes = au32Again_table[i - 1];
            pstAeSnsGainInfo->u32GainDb = i - 1;
            gu32SensorAnalogGain = pstAeSnsGainInfo->u32SnsTimes;
            break;
        }

    }
          
    return;

}


HI_U32 cmos_get_isp_default(ISP_CMOS_DEFAULT_S *pstDef)
{
    if (HI_NULL == pstDef)
    {
        printf("null pointer when get isp default value!\n");
        return -1;
    }

    memset(pstDef, 0, sizeof(ISP_CMOS_DEFAULT_S));
    
    switch (gu8SensorMode)
    {
        default:
        case 0:
            pstDef->stComm.u8Rggb           = 0x1;      // 1: grbg
            pstDef->stComm.u8BalanceFe      = 0x1;

            pstDef->stDenoise.u8SinterThresh= 0x23;
            pstDef->stDenoise.u8NoiseProfile= 0x1;      //0: use default profile table; 1: use calibrated profile lut, the setting for nr0 and nr1 must be correct.
            pstDef->stDenoise.u16Nr0        = 0x0;
            pstDef->stDenoise.u16Nr1        = 546;

            pstDef->stDrc.u8DrcBlack        = 0x00;
            pstDef->stDrc.u8DrcVs           = 0x04;     // variance space
            pstDef->stDrc.u8DrcVi           = 0x01;     // variance intensity
            pstDef->stDrc.u8DrcSm           = 0x80;     // slope max
            pstDef->stDrc.u16DrcWl          = 0x4FF;    // white level

            memcpy(&pstDef->stDemosaic, &g_stIspDemosaicLin, sizeof(ISP_CMOS_DEMOSAIC_S));
            memcpy(&pstDef->stNoiseTbl, &g_stIspNoiseTableLin, sizeof(ISP_CMOS_NOISE_TABLE_S));
            memcpy(&pstDef->stAgcTbl, &g_stIspAgcTableLin, sizeof(ISP_CMOS_AGC_TABLE_S));
            memcpy(&pstDef->stGamma, &g_stIspGamma, sizeof(ISP_CMOS_GAMMA_S));
        break;
        case 1:
            pstDef->stComm.u8Rggb           = 0x1;      // 1 :grbg 
            pstDef->stComm.u8BalanceFe      = 0x0;

            pstDef->stDenoise.u8SinterThresh= 0x9;
            pstDef->stDenoise.u8NoiseProfile= 0x0;      //0: use default profile table; 1: use calibrated profile lut, the setting for nr0 and nr1 must be correct.
            pstDef->stDenoise.u16Nr0        = 0x0;
            pstDef->stDenoise.u16Nr1        = 0x0;

            pstDef->stDrc.u8DrcBlack        = 0x00;
            pstDef->stDrc.u8DrcVs           = 0x04;     // variance space
            pstDef->stDrc.u8DrcVi           = 0x04;     // variance intensity
            pstDef->stDrc.u8DrcSm           = 0x3c;     // slope max
            pstDef->stDrc.u16DrcWl          = 0xFFF;    // white level

            memcpy(&pstDef->stDemosaic, &g_stIspDemosaicWdr, sizeof(ISP_CMOS_DEMOSAIC_S));
            memcpy(&pstDef->stNoiseTbl, &g_stIspNoiseTableWdr, sizeof(ISP_CMOS_NOISE_TABLE_S));
            memcpy(&pstDef->stGammafe, &g_stGammafe, sizeof(ISP_CMOS_GAMMAFE_S));
            memcpy(&pstDef->stAgcTbl, &g_stIspAgcTableWdr, sizeof(ISP_CMOS_AGC_TABLE_S));
            memcpy(&pstDef->stGamma, &g_stGammaWdr, sizeof(ISP_CMOS_GAMMA_S));
            
        break;
    }

    return 0;
}

HI_U32 cmos_get_isp_black_level(ISP_CMOS_BLACK_LEVEL_S *pstBlackLevel)
{
    HI_S32  i;
    HI_U32  u32SensorGainLimitDown = 1024,u32SensorGainLimitUp = 1024;
    HI_U16  au16BlackLevelLimitDown[4],au16BlackLevelLimitUp[4];
    
    if (HI_NULL == pstBlackLevel)
    {
        printf("null pointer when get isp black level value!\n");
        return -1;
    }

    /* Don't need to update black level when iso change */
    pstBlackLevel->bUpdate = HI_TRUE;//HI_FALSE;
// 1x       170,170,170,170
// 2x       171,171,170,170
// 4x       173,173,169,169
// 8x       182,182,171,171
// 16x       178,178,178,178
// 20x       182,182,182,182
// 1x       182,182,182,182     //Again = 20x, Dgain = 1x
// 2x       190,190,190,190
// 4x       213,214,205,205
// 6x       235,235,222,222
// 8x       258,259,241,242

#if 0
    switch (gu8SensorMode)
    {
        case 0 :
            for (i=0; i<4; i++)
            {
                pstBlackLevel->au16BlackLevel[i] = 170;
            }
            break;
        case 1 :
            for (i=0; i<4; i++)
            {
                pstBlackLevel->au16BlackLevel[i] = 0x0;
            }
            break;
    }
#endif


    switch (gu8SensorMode)
    {
        default :
        case 0 :
            if(gu32SensorTotalGain <= 1024 *2)
            {
                u32SensorGainLimitDown = 1024;
                u32SensorGainLimitUp = 1024 *2;
                au16BlackLevelLimitDown[0] = 170;
                au16BlackLevelLimitDown[1] = 170;
                au16BlackLevelLimitDown[2] = 170;
                au16BlackLevelLimitDown[3] = 170;
                au16BlackLevelLimitUp[0] = 171;
                au16BlackLevelLimitUp[1] = 171;
                au16BlackLevelLimitUp[2] = 170;
                au16BlackLevelLimitUp[3] = 170;
            }
            else if(gu32SensorTotalGain <= 1024 *4)
            {
                u32SensorGainLimitDown = 1024 *2;
                u32SensorGainLimitUp = 1024 *4;
                au16BlackLevelLimitDown[0] = 171;
                au16BlackLevelLimitDown[1] = 171;
                au16BlackLevelLimitDown[2] = 170;
                au16BlackLevelLimitDown[3] = 170;
                au16BlackLevelLimitUp[0] = 173;
                au16BlackLevelLimitUp[1] = 173;
                au16BlackLevelLimitUp[2] = 169;
                au16BlackLevelLimitUp[3] = 169;
            }
            else if(gu32SensorTotalGain <= 1024 *8)
            {
                u32SensorGainLimitDown = 1024 *4;
                u32SensorGainLimitUp = 1024 *8;
                au16BlackLevelLimitDown[0] = 173;
                au16BlackLevelLimitDown[1] = 173;
                au16BlackLevelLimitDown[2] = 169;
                au16BlackLevelLimitDown[3] = 169;
                au16BlackLevelLimitUp[0] = 182;
                au16BlackLevelLimitUp[1] = 182;
                au16BlackLevelLimitUp[2] = 171;
                au16BlackLevelLimitUp[3] = 171;
            }
            else if(gu32SensorTotalGain <= 1024 *16)
            {
                u32SensorGainLimitDown = 1024 *8;
                u32SensorGainLimitUp = 1024 *16;
                au16BlackLevelLimitDown[0] = 182;
                au16BlackLevelLimitDown[1] = 182;
                au16BlackLevelLimitDown[2] = 171;
                au16BlackLevelLimitDown[3] = 171;
                au16BlackLevelLimitUp[0] = 178;
                au16BlackLevelLimitUp[1] = 178;
                au16BlackLevelLimitUp[2] = 178;
                au16BlackLevelLimitUp[3] = 178;
            }
            else if(gu32SensorTotalGain <= 1024 *20)
            {
                u32SensorGainLimitDown = 1024 *16;
                u32SensorGainLimitUp = 1024 *20;
                au16BlackLevelLimitDown[0] = 178;
                au16BlackLevelLimitDown[1] = 178;
                au16BlackLevelLimitDown[2] = 178;
                au16BlackLevelLimitDown[3] = 178;
                au16BlackLevelLimitUp[0] = 182;
                au16BlackLevelLimitUp[1] = 182;
                au16BlackLevelLimitUp[2] = 182;
                au16BlackLevelLimitUp[3] = 182;
            }
            else if(gu32SensorTotalGain <= 1024 *20*2)
            {
                u32SensorGainLimitDown = 1024 *20;
                u32SensorGainLimitUp = 1024 *20*2;
                au16BlackLevelLimitDown[0] = 182;
                au16BlackLevelLimitDown[1] = 182;
                au16BlackLevelLimitDown[2] = 182;
                au16BlackLevelLimitDown[3] = 182;
                au16BlackLevelLimitUp[0] = 190;
                au16BlackLevelLimitUp[1] = 190;
                au16BlackLevelLimitUp[2] = 190;
                au16BlackLevelLimitUp[3] = 190;
            }
            else if(gu32SensorTotalGain <= 1024 *20*4)
            {
                u32SensorGainLimitDown = 1024 *20*2;
                u32SensorGainLimitUp = 1024 *20*4;
                au16BlackLevelLimitDown[0] = 190;
                au16BlackLevelLimitDown[1] = 190;
                au16BlackLevelLimitDown[2] = 190;
                au16BlackLevelLimitDown[3] = 190;
                au16BlackLevelLimitUp[0] = 213;
                au16BlackLevelLimitUp[1] = 214;
                au16BlackLevelLimitUp[2] = 205;
                au16BlackLevelLimitUp[3] = 205;
            }
            else if(gu32SensorTotalGain <= 1024 *20*6)
            {
                u32SensorGainLimitDown = 1024 *20*4;
                u32SensorGainLimitUp = 1024 *20*6;
                au16BlackLevelLimitDown[0] = 213;
                au16BlackLevelLimitDown[1] = 214;
                au16BlackLevelLimitDown[2] = 205;
                au16BlackLevelLimitDown[3] = 205;
                au16BlackLevelLimitUp[0] = 235;
                au16BlackLevelLimitUp[1] = 235;
                au16BlackLevelLimitUp[2] = 222;
                au16BlackLevelLimitUp[3] = 222;
            }
            else
            {
                u32SensorGainLimitDown = 1024 *20*6;
                u32SensorGainLimitUp = 1024 *20*8;
                au16BlackLevelLimitDown[0] = 235;
                au16BlackLevelLimitDown[1] = 235;
                au16BlackLevelLimitDown[2] = 222;
                au16BlackLevelLimitDown[3] = 222;
                au16BlackLevelLimitUp[0] = 258;
                au16BlackLevelLimitUp[1] = 259;
                au16BlackLevelLimitUp[2] = 241;
                au16BlackLevelLimitUp[3] = 242;
            }

            for (i=0; i<4; i++)
            {
                pstBlackLevel->au16BlackLevel[i] = ( (gu32SensorTotalGain - u32SensorGainLimitDown) *au16BlackLevelLimitDown[i] + \
             (u32SensorGainLimitUp - gu32SensorTotalGain) * au16BlackLevelLimitUp[i] )  /(u32SensorGainLimitUp - u32SensorGainLimitDown);
            }

            break;
        case 1 :
            for (i=0; i<4; i++)
            {
                pstBlackLevel->au16BlackLevel[i] = 0x0;
            }
            break;
    }



    return 0;    
}

HI_VOID cmos_set_pixel_detect(HI_BOOL bEnable)
{
    if (bEnable) /* setup for ISP pixel calibration mode */
    {
        sensor_write_register(0x300C, 0x4D58);    //5fps
        sensor_write_register(0x3012, 0x2EC);    //max exposure lines
        sensor_write_register(0x3060, 0x0000);    //AG, Context A

        //DG
        sensor_write_register(0x305E, 0x0080);    //Global_gain
    }
    else /* setup for ISP 'normal mode' */
    {
        sensor_write_register(0x300C, 0xCE4);    //30fps
    }

    return;
}

HI_VOID cmos_set_wdr_mode(HI_U8 u8Mode)
{
    switch(u8Mode)
    {
        //720P30 linear
        case 0:
            gu8SensorMode = 0;
            printf("linear mode\n");

            /* program sensor to linear mode */
            sensor_prog(sensor_rom_30_lin);


        break;

        //720P30 wdr
        case 1:
            gu8SensorMode = 1;
            printf("wdr mode\n");

            /* program sensor to wdr mode */
            sensor_prog(sensor_rom_30_wdr);

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
    
    pstAeSnsDft->u32LinesPer500ms = 750*30/2;
    pstAeSnsDft->u32FlickerFreq = 0;    //60*256;//50*256;

    pstAeSnsDft->u32FullLinesStd = gu32FullLinesStd;

    pstAeSnsDft->stIntTimeAccu.enAccuType = AE_ACCURACY_LINEAR;
    pstAeSnsDft->stIntTimeAccu.f32Accuracy = 1;
    

    pstAeSnsDft->stAgainAccu.enAccuType = AE_ACCURACY_TABLE;
    pstAeSnsDft->stAgainAccu.f32Accuracy = 0.0625;

    pstAeSnsDft->stDgainAccu.enAccuType = AE_ACCURACY_LINEAR;
    pstAeSnsDft->stDgainAccu.f32Accuracy = 0.0078125;
    pstAeSnsDft->u32ISPDgainShift = 8;
    switch(gu8SensorMode)
    {
        default:
        case 0: //linear mode
            pstAeSnsDft->au8HistThresh[0] = 0xd;
            pstAeSnsDft->au8HistThresh[1] = 0x28;
            pstAeSnsDft->au8HistThresh[2] = 0x60;
            pstAeSnsDft->au8HistThresh[3] = 0x80;
            
            pstAeSnsDft->u8AeCompensation = 0x40;
            
            pstAeSnsDft->u32MaxIntTime = 748;
            pstAeSnsDft->u32MinIntTime = 2;
            pstAeSnsDft->u32MaxIntTimeTarget = 65535;
            pstAeSnsDft->u32MinIntTimeTarget = 2;
            
           pstAeSnsDft->u32MaxAgain = 31948;	//  max Again: 12x2.6
           pstAeSnsDft->u32MinAgain = 1664;     // min Again: 1.6;
          // pstAeSnsDft->u32MaxAgainTarget = 31948;
           pstAeSnsDft->u32MaxAgainTarget = 20480;

           pstAeSnsDft->u32MinAgainTarget = 1664;
            
            pstAeSnsDft->u32MaxDgain = 2047;  
            pstAeSnsDft->u32MinDgain = 160;     // min dgain: 1.25
            //pstAeSnsDft->u32MaxDgainTarget = 2047;
            pstAeSnsDft->u32MaxDgainTarget = 768;
            pstAeSnsDft->u32MinDgainTarget = 160;
            
            pstAeSnsDft->u32MaxISPDgainTarget = 4 << pstAeSnsDft->u32ISPDgainShift;
        break;
        case 1: //WDR mode
            pstAeSnsDft->au8HistThresh[0] = 0x20;
            pstAeSnsDft->au8HistThresh[1] = 0x40;
            pstAeSnsDft->au8HistThresh[2] = 0x60;
            pstAeSnsDft->au8HistThresh[3] = 0x80;
            
            pstAeSnsDft->u8AeCompensation = 22;

            pstAeSnsDft->u32MaxIntTime = 698;  //750 - 52 = 698
            pstAeSnsDft->u32MinIntTime = 8;
            pstAeSnsDft->u32MaxIntTimeTarget = 698;  /* for short exposure, Exposure ratio = 16X */
            pstAeSnsDft->u32MinIntTimeTarget = 8;

           //pstAeSnsDft->u32MaxAgain = 31948;
            pstAeSnsDft->u32MaxAgain = 4096;
           // pstAeSnsDft->u32MinAgain = 1664;
            pstAeSnsDft->u32MinAgain = 1024;
          // pstAeSnsDft->u32MaxAgainTarget = 31948;
            pstAeSnsDft->u32MaxAgainTarget = 4096;
          //  pstAeSnsDft->u32MinAgainTarget = 1664;
            pstAeSnsDft->u32MinAgainTarget = 1024;
            
           // pstAeSnsDft->u32MaxDgain = 2047;
            pstAeSnsDft->u32MaxDgain = 512;
            pstAeSnsDft->u32MinDgain = 160;
           // pstAeSnsDft->u32MaxDgainTarget = 2047;
            pstAeSnsDft->u32MaxDgainTarget = 512;
            pstAeSnsDft->u32MinDgainTarget = 160;
            
         //   pstAeSnsDft->u32MaxISPDgainTarget = 4 << pstAeSnsDft->u32ISPDgainShift;
            pstAeSnsDft->u32MaxISPDgainTarget = 32 << pstAeSnsDft->u32ISPDgainShift;
        break;
    }

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
            sensor_write_register(0x300C, 0xCE4);
        break;
        case 25:
            gu32FullLinesStd = 750;
            pstAeSnsDft->u32MaxIntTime = 748;
            pstAeSnsDft->u32LinesPer500ms = 750 * 25 / 2;
            sensor_write_register(0x300C, 0xF78);
        break;        
        default:
        break;
    }
    
    if(1 == gu8SensorMode)
    {
        pstAeSnsDft->u32MaxIntTime = 698;
    }

    pstAeSnsDft->u32FullLinesStd = gu32FullLinesStd;
    return;
}

static HI_VOID cmos_slow_framerate_set(HI_U16 u16FullLines,
    AE_SENSOR_DEFAULT_S *pstAeSnsDft)
{    
    sensor_write_register(0x300A, u16FullLines);

    pstAeSnsDft->u32MaxIntTime = u16FullLines - 2;
    
    return;
}

static HI_VOID cmos_init_regs_info(HI_VOID)
{
#if CMOS_AR0140_ISP_WRITE_SENSOR_ENABLE
    HI_S32 i;
    static HI_BOOL bInit = HI_FALSE;

    if (HI_FALSE == bInit)
    {
        g_stSnsRegsInfo.enSnsType = ISP_SNS_I2C_TYPE;
        g_stSnsRegsInfo.u32RegNum = 4;
        for (i=0; i<4; i++)
        {
            g_stSnsRegsInfo.astI2cData[i].u8DevAddr = sensor_i2c_addr;
            g_stSnsRegsInfo.astI2cData[i].u32AddrByteNum = sensor_addr_byte;
            g_stSnsRegsInfo.astI2cData[i].u32DataByteNum = sensor_data_byte;
        }
        g_stSnsRegsInfo.astI2cData[0].bDelayCfg = HI_FALSE;
        g_stSnsRegsInfo.astI2cData[0].u32RegAddr = 0x3012;		//exposure time
        g_stSnsRegsInfo.astI2cData[1].bDelayCfg = HI_FALSE;
        g_stSnsRegsInfo.astI2cData[1].u32RegAddr = 0x3060;		//Again
        g_stSnsRegsInfo.astI2cData[2].bDelayCfg = HI_FALSE;
        g_stSnsRegsInfo.astI2cData[2].u32RegAddr = 0x305E;      //Dgain
         g_stSnsRegsInfo.astI2cData[3].bDelayCfg = HI_FALSE;
        g_stSnsRegsInfo.astI2cData[3].u32RegAddr = 0x3100;      //DCGgain       
        g_stSnsRegsInfo.bDelayCfgIspDgain = HI_TRUE;

        bInit = HI_TRUE;
    }
#endif
    return;
}


/* while isp notify ae to update sensor regs, ae call these funcs. */
static HI_VOID cmos_inttime_update(HI_U32 u32IntTime)
{

#if CMOS_AR0140_ISP_WRITE_SENSOR_ENABLE
    cmos_init_regs_info();
    g_stSnsRegsInfo.astI2cData[0].u32Data = u32IntTime;
#else
    sensor_write_register(0x3012, u32IntTime);
#endif
    return;
}

static HI_VOID cmos_gains_update(HI_U32 u32Again, HI_U32 u32Dgain)
{

   HI_U32 u32DCGgain = 0;

  if(u32Again < 24)//21)
    u32DCGgain = 0;
  else
    {
    u32DCGgain = 4;
    u32Again = u32Again - 24;//21;
    }
    gu32SensorTotalGain =gu32SensorAnalogGain * u32Dgain / 128;
   // printf("gu32SensorTotalGain = %d\n",gu32SensorTotalGain);

#if CMOS_AR0140_ISP_WRITE_SENSOR_ENABLE
    cmos_init_regs_info();
    g_stSnsRegsInfo.astI2cData[1].u32Data = u32Again;
    g_stSnsRegsInfo.astI2cData[2].u32Data = u32Dgain;
    g_stSnsRegsInfo.astI2cData[3].u32Data = u32DCGgain;
    
    HI_MPI_ISP_SnsRegsCfg(&g_stSnsRegsInfo);
#else
    sensor_write_register(0x3060, u32Again);
    sensor_write_register(0x3100, u32DCGgain);
    sensor_write_register(0x305E, u32Dgain);
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

    pstAwbSnsDft->au16GainOffset[0] = 0x0187;
    pstAwbSnsDft->au16GainOffset[1] = 0x0100;
    pstAwbSnsDft->au16GainOffset[2] = 0x0100;
    pstAwbSnsDft->au16GainOffset[3] = 0x01A7;

    pstAwbSnsDft->as32WbPara[0] = 86;
    pstAwbSnsDft->as32WbPara[1] = -9;
    pstAwbSnsDft->as32WbPara[2] = -179;
    pstAwbSnsDft->as32WbPara[3] = 223078;
    pstAwbSnsDft->as32WbPara[4] = 128;
    pstAwbSnsDft->as32WbPara[5] = -174506;

    memcpy(&pstAwbSnsDft->stCcm, &g_stAwbCcm, sizeof(AWB_CCM_S));

    
    switch (gu8SensorMode)
    {
        default:
        case 0:
            memcpy(&pstAwbSnsDft->stAgcTbl, &g_stAwbAgcTableLin, sizeof(AWB_AGC_TABLE_S));
        break;
        case 1:
            memcpy(&pstAwbSnsDft->stAgcTbl, &g_stAwbAgcTableWdr, sizeof(AWB_AGC_TABLE_S));
        break;
    }
    
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
    s32Ret = HI_MPI_ISP_SensorRegCallBack(AR0140_ID, &stIspRegister);
    if (s32Ret)
    {
        printf("sensor register callback function failed!\n");
        return s32Ret;
    }
    
    stLib.s32Id = 0;
    strcpy(stLib.acLibName, HI_AE_LIB_NAME);
    cmos_init_ae_exp_function(&stAeRegister.stSnsExp);
    s32Ret = HI_MPI_AE_SensorRegCallBack(&stLib, AR0140_ID, &stAeRegister);
    if (s32Ret)
    {
        printf("sensor register callback function to ae lib failed!\n");
        return s32Ret;
    }

    stLib.s32Id = 0;
    strcpy(stLib.acLibName, HI_AWB_LIB_NAME);
    cmos_init_awb_exp_function(&stAwbRegister.stSnsExp);
    s32Ret = HI_MPI_AWB_SensorRegCallBack(&stLib, AR0140_ID, &stAwbRegister);
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

    s32Ret = HI_MPI_ISP_SensorUnRegCallBack(AR0140_ID);
    if (s32Ret)
    {
        printf("sensor unregister callback function failed!\n");
        return s32Ret;
    }
    
    stLib.s32Id = 0;
    strcpy(stLib.acLibName, HI_AE_LIB_NAME);
    s32Ret = HI_MPI_AE_SensorUnRegCallBack(&stLib, AR0140_ID);
    if (s32Ret)
    {
        printf("sensor unregister callback function to ae lib failed!\n");
        return s32Ret;
    }

    stLib.s32Id = 0;
    strcpy(stLib.acLibName, HI_AWB_LIB_NAME);
    s32Ret = HI_MPI_AWB_SensorUnRegCallBack(&stLib, AR0140_ID);
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

#endif //__AR0140_CMOS_H_
