#if !defined(__IMX236_CMOS_H_)
#define __IMX236_CMOS_H_

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

/* Note: format of address is special.
 * chip_id + reg_adddr */
#define EXPOSURE_ADDR (0x220) //2:chip_id, 20: reg addr.
#define LONG_EXPOSURE_ADDR (0x223)
#define PGC_ADDR (0x214)
#define VMAX_ADDR (0x218)
#define HMAX_ADDR (0x21B)

#define IMX236_ID 236
/*set Frame End Update Mode 2 with HI_MPI_ISP_SetAEAttr and set this value 1 to avoid flicker */
/*when use Frame End Update Mode 2, the speed of i2c will affect whole system's performance   */
/*increase I2C_DFT_RATE in Hii2c.c to 400000 to increase the speed of i2c                     */
#define CMOS_IMX236_ISP_WRITE_SENSOR_ENABLE (1)
/****************************************************************************
 * local variables                                                            *
 ****************************************************************************/
extern const unsigned int sensor_i2c_addr;
extern const unsigned int sensor_addr_byte;
extern const unsigned int sensor_data_byte;

HI_U8 gu8SensorMode = 0;
#if CMOS_IMX236_ISP_WRITE_SENSOR_ENABLE
ISP_SNS_REGS_INFO_S g_stSnsRegsInfo = {0};
static HI_BOOL gsbRegInit = HI_FALSE;
#endif
static HI_U32 gu32FullLinesStd = 1125;
static HI_U32 gu32FullLines = 1125;

static AWB_CCM_S g_stAwbCcm =
{
    4850,
    {
        0x01f7, 0x80ef, 0x8008,
        0x804b, 0x019d, 0x8052,
        0x0015, 0x8133, 0x021d
    },
    3140,
    {
        0x0199, 0x8080, 0x8019,
        0x808e, 0x01b3, 0x8025,
        0x000f, 0x817c, 0x026c
    },
    2470,
    {
        0x0147, 0x8025, 0x8022,
        0x8086, 0x018b, 0x8005,
        0x801e, 0x8228, 0x0346
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

    /* sharpen_alt_d */
    {0x88,0x85,0x80,0x7b,0x78,0x72,0x70,0x60},
        
    /* sharpen_alt_ud */
    {0xc8,0xc0,0xb8,0xb0,0xa8,0xa0,0x72,0x4b},
        
    /* snr_thresh */
    {0x06,0x8,0xb,0x16,0x22,0x28,0x32,0x54},
        
    /* demosaic_lum_thresh */
    {0x60,0x60,0x80,0x80,0x80,0x80,0x80,0x80},
        
    /* demosaic_np_offset */
    {0x0,0xa,0x12,0x1a,0x20,0x28,0x30,0x30},
        
    /* ge_strength */
    {0x55,0x55,0x55,0x55,0x55,0x55,0x37,0x37},
};

static ISP_CMOS_NOISE_TABLE_S g_stIspNoiseTableLin =
{
    /* bvalid */
    1,
    
    /* nosie_profile_weight_lut */
    {
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x04,0x0c,0x11,0x14,0x17,0x19,0x1b,0x1c,0x1e,0x1f,
        0x20,0x21,0x22,0x23,0x24,0x24,0x25,0x26,0x26,0x27,0x28,0x28,0x29,0x29,0x2a,0x2a,0x2a,
        0x2b,0x2b,0x2c,0x2c,0x2c,0x2d,0x2d,0x2d,0x2e,0x2e,0x2e,0x2f,0x2f,0x2f,0x30,0x30,0x30,
        0x30,0x31,0x31,0x31,0x31,0x32,0x32,0x32,0x32,0x32,0x33,0x33,0x33,0x33,0x34,0x34,0x34,
        0x34,0x34,0x34,0x35,0x35,0x35,0x35,0x35,0x35,0x36,0x36,0x36,0x36,0x36,0x36,0x37,0x37,
        0x37,0x37,0x37,0x37,0x37,0x38,0x38,0x38,0x38,0x38,0x38,0x38,0x39,0x39,0x39,0x39,0x39,
        0x39,0x39,0x39,0x39,0x3a,0x3a,0x3a,0x3a,0x3a,0x3a,0x3a,0x3a,0x3a,0x3b,0x3b,0x3b,0x3b,
        0x3b,0x3b,0x3b,0x3b,0x3b,0x3c,0x3c,0x3c,0x3c
    },

    /* demosaic_weight_lut */
    {
        0x04,0x0c,0x11,0x14,0x17,0x19,0x1b,0x1c,0x1e,0x1f,
        0x20,0x21,0x22,0x23,0x24,0x24,0x25,0x26,0x26,0x27,0x28,0x28,0x29,0x29,0x2a,0x2a,0x2a,
        0x2b,0x2b,0x2c,0x2c,0x2c,0x2d,0x2d,0x2d,0x2e,0x2e,0x2e,0x2f,0x2f,0x2f,0x30,0x30,0x30,
        0x30,0x31,0x31,0x31,0x31,0x32,0x32,0x32,0x32,0x32,0x33,0x33,0x33,0x33,0x34,0x34,0x34,
        0x34,0x34,0x34,0x35,0x35,0x35,0x35,0x35,0x35,0x36,0x36,0x36,0x36,0x36,0x36,0x37,0x37,
        0x37,0x37,0x37,0x37,0x37,0x38,0x38,0x38,0x38,0x38,0x38,0x38,0x39,0x39,0x39,0x39,0x39,
        0x39,0x39,0x39,0x39,0x3a,0x3a,0x3a,0x3a,0x3a,0x3a,0x3a,0x3a,0x3a,0x3b,0x3b,0x3b,0x3b,
        0x3b,0x3b,0x3b,0x3b,0x3b,0x3c,0x3c,0x3c,0x3c,0x3c,0x3c,0x3c,0x3c,0x3c,0x3c,0x3c
    }
};

static ISP_CMOS_NOISE_TABLE_S g_stIspNoiseTableWdr =
{
    /* bvalid */
    1,
    
    /* nosie_profile_weight_lut */
    {
        0,37,37,37,37,37,37,37,37,37,37,37,37,37,37,37,37,37,37,37,37,37,37,37,37,37,45,48,57,
        63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,
        63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,
        63,63,63,63,63,63,63,63,63,63,63,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,
        64,64,64,64,64,64,64,64,64,64,64,64,64,64,64
    },

    /* demosaic_weight_lut */
    {
        0,37,37,37,37,37,37,37,37,37,37,37,37,37,37,37,37,37,37,37,37,37,37,37,37,37,45,48,57,
        63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,
        63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,
        63,63,63,63,63,63,63,63,63,63,63,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,
        64,64,64,64,64,64,64,64,64,64,64,64,64,64,64
    }
};


static ISP_CMOS_DEMOSAIC_S g_stIspDemosaic =
{
    /* bvalid */
    1,
    
     /*vh_slope*/
    0xd8,

    /*aa_slope*/
    0xce,

    /*va_slope*/
    0xe0,

    /*uu_slope*/
    0xc4,

    /*sat_slope*/
    0x5d,

    /*ac_slope*/
    0xcf,

    /*vh_thresh*/
    0x4c,

    /*aa_thresh*/
    0x5b,

    /*va_thresh*/
    0x39,

    /*uu_thresh*/
    0x3d,

    /*sat_thresh*/
    0x171,

    /*ac_thresh*/
    0x1b3,
};


static HI_U32 gain_table[481]=
{
    1024, 1035, 1047, 1059, 1072, 1084, 1097, 1109, 1122, 1135, 1148, 1162, 1175, 1189, 1203, 1217, 1231, 1245, 1259, 1274, 
    1289, 1304, 1319, 1334, 1349, 1365, 1381, 1397, 1413, 1429, 1446, 1463, 1480, 1497, 1514, 1532, 1549, 1567, 1585, 1604, 
    1622, 1641, 1660, 1679, 1699, 1719, 1739, 1759, 1779, 1800, 1820, 1842, 1863, 1884, 1906, 1928, 1951, 1973, 1996, 2019, 
    2043, 2066, 2090, 2114, 2139, 2164, 2189, 2214, 2240, 2266, 2292, 2318, 2345, 2373, 2400, 2428, 2456, 2484, 2513, 2542, 
    2572, 2601, 2632, 2662, 2693, 2724, 2756, 2788, 2820, 2852, 2886, 2919, 2953, 2987, 3022, 3057, 3092, 3128, 3164, 3201, 
    3238, 3275, 3313, 3351, 3390, 3430, 3469, 3509, 3550, 3591, 3633, 3675, 3717, 3760, 3804, 3848, 3893, 3938, 3983, 4029, 
    4076, 4123, 4171, 4219, 4268, 4318, 4368, 4418, 4469, 4521, 4574, 4627, 4680, 4734, 4789, 4845, 4901, 4957, 5015, 5073, 
    5132, 5191, 5251, 5312, 5374, 5436, 5499, 5562, 5627, 5692, 5758, 5825, 5892, 5960, 6029, 6099, 6170, 6241, 6313, 6387, 
    6461, 6535, 6611, 6688, 6765, 6843, 6923, 7003, 7084, 7166, 7249, 7333, 7418, 7504, 7591, 7678, 7767, 7857, 7948, 8040, 
    8133, 8228, 8323, 8419, 8517, 8615, 8715, 8816, 8918, 9021, 9126, 9232, 9338, 9447, 9556, 9667, 9779, 9892, 10006, 10122, 
    10240, 10358, 10478, 10599, 10722, 10846, 10972, 11099, 11227, 11357, 11489, 11622, 11757, 11893, 12030, 12170, 12311, 12453, 12597, 12743, 
    12891, 13040, 13191, 13344, 13498, 13655, 13813, 13973, 14135, 14298, 14464, 14631, 14801, 14972, 15146, 15321, 15498, 15678, 15859, 16043, 
    16229, 16417, 16607, 16799, 16994, 17190, 17390, 17591, 17795, 18001, 18209, 18420, 18633, 18849, 19067, 19288, 19511, 19737, 19966, 20197, 
    20431, 20668, 20907, 21149, 21394, 21642, 21892, 22146, 22402, 22662, 22924, 23189, 23458, 23730, 24004, 24282, 24564, 24848, 25136, 25427, 
    25721, 26019, 26320, 26625, 26933, 27245, 27561, 27880, 28203, 28529, 28860, 29194, 29532, 29874, 30220, 30570, 30924, 31282, 31644, 32011, 
    32381, 32756, 33135, 33519, 33907, 34300, 34697, 35099, 35505, 35916, 36332, 36753, 37179, 37609, 38045, 38485, 38931, 39382, 39838, 40299, 
    40766, 41238, 41715, 42198, 42687, 43181, 43681, 44187, 44699, 45216, 45740, 46270, 46805, 47347, 47896, 48450, 49011, 49579, 50153, 50734, 
    51321, 51915, 52517, 53125, 53740, 54362, 54992, 55628, 56272, 56924, 57583, 58250, 58925, 59607, 60297, 60995, 61702, 62416, 63139, 63870, 
    64610, 65358, 66114, 66880, 67655, 68438, 69230, 70032, 70843, 71663, 72493, 73333, 74182, 75041, 75910, 76789, 77678, 78577, 79487, 80408, 
    81339, 82281, 83233, 84197, 85172, 86158, 87156, 88165, 89186, 90219, 91264, 92320, 93389, 94471, 95565, 96671, 97791, 98923, 100069, 101227, 
    102400, 103585, 104785, 105998, 107225, 108467, 109723, 110994, 112279, 113579, 114894, 116225, 117570, 118932, 120309, 121702, 123111, 124537, 125979, 127438, 
    128913, 130406, 131916, 133444, 134989, 136552, 138133, 139733, 141351, 142988, 144643, 146318, 148013, 149726, 151460, 153214, 154988, 156783, 158598, 160435, 
    162293, 164172, 166073, 167996, 169941, 171909, 173900, 175913, 177950, 180011, 182095, 184204, 186337, 188495, 190677, 192885, 195119, 197378, 199664, 201976, 
    204314, 206680, 209073, 211494, 213943, 216421, 218927, 221462, 224026, 226620, 229244, 231899, 234584, 237301, 240048, 242828, 245640, 248484, 251362, 254272, 
    257217

};

extern void sensor_linner_init();
extern void sensor_wdr_init();

static ISP_CMOS_GAMMAFE_S g_stGammafe = 
{
    /* bvalid */
    1,

    {
       0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,  28,
      70,  95, 114, 131, 146, 159, 172, 183, 195, 205, 215, 224, 233, 242, 250, 258,
     266, 273, 281, 288, 295, 302, 309, 315, 322, 328, 334, 340, 346, 352, 358, 364,
     369, 375, 380, 386, 391, 396, 401, 406, 411, 416, 421, 426, 431, 436, 440, 445,
     450, 454, 459, 463, 467, 472, 476, 480, 485, 489, 493, 497, 502, 506, 510, 514,
     518, 522, 525, 529, 533, 537, 541, 545, 548, 552, 556, 559, 563, 567, 571, 574,
     578, 581, 585, 588, 592, 595, 599, 602, 605, 609, 612, 615, 619, 622, 625, 629,
     632, 635, 638, 642, 645, 648, 651, 654, 657, 661, 664, 667, 670, 673, 676, 679,
     682, 729, 772, 814, 853, 891, 927, 962, 995,1028,1060,1090,1120,1149,1177,1205,
    1232,1258,1284,1309,1334,1359,1383,1406,1429,1452,1475,1497,1519,1540,1561,1582,
    1603,1623,1643,1663,1683,1702,1721,1740,1759,1778,1796,1814,1832,1850,1868,1885,
    1903,1920,1937,1954,1971,1987,2004,2020,2036,2100,2162,2222,2280,2337,2393,2447,
    2500,2552,2603,2653,2702,2751,2798,2844,2890,2936,2980,3024,3067,3110,3151,3193,
    3234,3274,3314,3354,3393,3431,3469,3507,3544,3581,3618,3654,3690,3725,3760,3795,
    3830,3864,3898,3931,3965,3998,4030,4063,4095,4095,4095,4095,4095,4095,4095,4095,
    4095,4095,4095,4095,4095,4095,4095,4095,4095,4095,4095,4095,4095,4095,4095,4095,4095
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
    
    switch (gu8SensorMode)
    {
        default:
        case 0:
            pstDef->stComm.u8Rggb           = 0x0;      //2: gbrg  
            pstDef->stComm.u8BalanceFe      = 0x1;

            pstDef->stDenoise.u8SinterThresh= 0x20;
            pstDef->stDenoise.u8NoiseProfile= 0x1;      //0: use default profile table; 1: use calibrated profile lut, the setting for nr0 and nr1 must be correct.
            pstDef->stDenoise.u16Nr0        = 0x0;
            pstDef->stDenoise.u16Nr1        = 455;

            pstDef->stDrc.u8DrcBlack        = 0x00;
            pstDef->stDrc.u8DrcVs           = 0x04;     // variance space
            pstDef->stDrc.u8DrcVi           = 0x08;     // variance intensity
            pstDef->stDrc.u8DrcSm           = 0xa0;     // slope max
            pstDef->stDrc.u16DrcWl          = 0x8ff;    // white level

            memcpy(&pstDef->stNoiseTbl, &g_stIspNoiseTableLin, sizeof(ISP_CMOS_NOISE_TABLE_S));            
        break;
        case 1:
            pstDef->stComm.u8Rggb           = 0x0;      //2: gbrg  
            pstDef->stComm.u8BalanceFe      = 0x0;

            pstDef->stDenoise.u8SinterThresh= 0x9;
            pstDef->stDenoise.u8NoiseProfile= 0x0;      //0: use default profile table; 1: use calibrated profile lut, the setting for nr0 and nr1 must be correct.
            pstDef->stDenoise.u16Nr0        = 0x0;
            pstDef->stDenoise.u16Nr1        = 0x0;

            pstDef->stDrc.u8DrcBlack        = 0x00;
            pstDef->stDrc.u8DrcVs           = 0x08;     // variance space
            pstDef->stDrc.u8DrcVi           = 0x01;     // variance intensity
            pstDef->stDrc.u8DrcSm           = 0x3C;     // slope max
            pstDef->stDrc.u16DrcWl          = 0xFFF;    // white level

            memcpy(&pstDef->stNoiseTbl, &g_stIspNoiseTableWdr, sizeof(ISP_CMOS_NOISE_TABLE_S));

            memcpy(&pstDef->stGammafe, &g_stGammafe, sizeof(ISP_CMOS_GAMMAFE_S));
        break;
    }

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

    switch (gu8SensorMode)
    {
        default :
        case 0 :
            for (i=0; i<4; i++)
            {
                pstBlackLevel->au16BlackLevel[i] = 0xf0;
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
        /* Sensor must be programmed for slow frame rate (5 fps and below)*/
        /* change frame rate to 5 fps by setting 1 frame length = 750 * 30 / 5 */
        sensor_write_register(VMAX_ADDR, 0x5E);
        sensor_write_register(VMAX_ADDR + 1, 0x1A);

        /* max Exposure time */
        sensor_write_register(EXPOSURE_ADDR, 0x00);
        sensor_write_register(EXPOSURE_ADDR + 1, 0x00);

        /* Analog and Digital gains both must be programmed for their minimum values */
        sensor_write_register(PGC_ADDR, 0x00);
        sensor_write_register(PGC_ADDR + 1, 0x00); 
    }
    else /* setup for ISP 'normal mode' */
    {
        sensor_write_register(VMAX_ADDR, 0x65);
        sensor_write_register(VMAX_ADDR + 1, 0x04);      
    }

    return;
}

HI_VOID cmos_set_wdr_mode(HI_U8 u8Mode)
{
    switch(u8Mode)
    {
        case 0:
            gu8SensorMode = 0;
            sensor_linner_init();
            printf("imx236 linear mode\n");
        break;
        case 1:
            gu8SensorMode = 1;
            sensor_wdr_init();
            printf("imx236 wdr mode\n");
        break;

        default:
            printf("NOT support this mode!\n");
            return;
        break;
    }
#if CMOS_IMX236_ISP_WRITE_SENSOR_ENABLE
    gsbRegInit = HI_FALSE;
#endif
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
    
    pstAeSnsDft->u32LinesPer500ms = 1125*30/2;
    pstAeSnsDft->u32FullLinesStd = gu32FullLinesStd;
    pstAeSnsDft->u32FlickerFreq = 0;//60*256;//50*256;

    pstAeSnsDft->stIntTimeAccu.enAccuType = AE_ACCURACY_LINEAR;
    pstAeSnsDft->stIntTimeAccu.f32Accuracy = 1;
    pstAeSnsDft->u32MaxIntTime = 1123;
    pstAeSnsDft->u32MinIntTime = 2;

    pstAeSnsDft->stAgainAccu.enAccuType = AE_ACCURACY_TABLE;
    pstAeSnsDft->stAgainAccu.f32Accuracy = 0.1;

    pstAeSnsDft->stDgainAccu.enAccuType = AE_ACCURACY_TABLE;
    pstAeSnsDft->stDgainAccu.f32Accuracy = 0.1;

    pstAeSnsDft->u32ISPDgainShift = 8;
    pstAeSnsDft->u32MinISPDgainTarget = 1 << pstAeSnsDft->u32ISPDgainShift;

    switch(gu8SensorMode)
    {
        default:
        case 0: //linear mode
            pstAeSnsDft->au8HistThresh[0] = 0xd;
            pstAeSnsDft->au8HistThresh[1] = 0x28;
            pstAeSnsDft->au8HistThresh[2] = 0x60;
            pstAeSnsDft->au8HistThresh[3] = 0x80;
            
            pstAeSnsDft->u8AeCompensation = 0x50;
            
            pstAeSnsDft->u32MaxIntTimeTarget = 65535;
            pstAeSnsDft->u32MinIntTimeTarget = 2;
            
            pstAeSnsDft->u32MaxAgain = 257217;  /*10bit precision */
            pstAeSnsDft->u32MinAgain = 1024;
            pstAeSnsDft->u32MaxAgainTarget = 257217;
            pstAeSnsDft->u32MinAgainTarget = 1024;
            
            pstAeSnsDft->u32MaxDgain = 1024;  /* 24db / 0.1db = 240 */
            pstAeSnsDft->u32MinDgain = 1024;
            pstAeSnsDft->u32MaxDgainTarget = 1024;
            pstAeSnsDft->u32MinDgainTarget = 1024;
            
            pstAeSnsDft->u32MaxISPDgainTarget = 4 << pstAeSnsDft->u32ISPDgainShift;
        break;
        case 1: //WDR mode
            pstAeSnsDft->au8HistThresh[0] = 0x20;
            pstAeSnsDft->au8HistThresh[1] = 0x40;
            pstAeSnsDft->au8HistThresh[2] = 0x60;
            pstAeSnsDft->au8HistThresh[3] = 0x80;
            
            pstAeSnsDft->u8AeCompensation = 0x50;

        
            pstAeSnsDft->u32MaxIntTimeTarget = 70;  /* for short exposure, Exposure ratio = 16X */
            pstAeSnsDft->u32MinIntTimeTarget = 8;

            pstAeSnsDft->u32MaxAgain = 1024;  /* 4.5db fixed */
            pstAeSnsDft->u32MinAgain = 1024;
            pstAeSnsDft->u32MaxAgainTarget = 1024;
            pstAeSnsDft->u32MinAgainTarget = 1024;
            
            pstAeSnsDft->u32MaxDgain = 4076;  /* 10bit shift */
            pstAeSnsDft->u32MinDgain = 1024;
            pstAeSnsDft->u32MaxDgainTarget = 4076;
            pstAeSnsDft->u32MinDgainTarget = 1024;
            
            pstAeSnsDft->u32MaxISPDgainTarget = 32 << pstAeSnsDft->u32ISPDgainShift;
        break;
    }

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
    if(0 == gu8SensorMode)
    {
        sensor_write_register(HMAX_ADDR,  0x30);
        sensor_write_register(HMAX_ADDR+1, 0x11);
        switch(u8Fps)
        {
            case 30:
                // Change the frame rate via changing the vertical blanking
                gu32FullLinesStd = 1125;
                pstAeSnsDft->u32MaxIntTime = 1123;
                pstAeSnsDft->u32LinesPer500ms = 1125 * 30 / 2;
                sensor_write_register(VMAX_ADDR, 0x65);
                sensor_write_register(VMAX_ADDR+1, 0x4);
            break;
            
            case 25:
                // Change the frame rate via changing the vertical blanking
                gu32FullLinesStd = 1350;
                pstAeSnsDft->u32MaxIntTime = 1348;
                pstAeSnsDft->u32LinesPer500ms = 1350 * 25 / 2;
                sensor_write_register(VMAX_ADDR, 0x46);
                sensor_write_register(VMAX_ADDR+1, 0x05);
            break;
            
            default:
            break;
        }
    }
    else if (1 == gu8SensorMode)   /* Built-in WDR */
    {
        gu32FullLinesStd = 1125;
        pstAeSnsDft->u32LinesPer500ms = 1125 * 30 / 2;
        pstAeSnsDft->u32MaxIntTime = 70;
        sensor_write_register(VMAX_ADDR, 0x65);
        sensor_write_register(VMAX_ADDR+1, 0x4);
        switch(u8Fps)
        {
            case 30:
                sensor_write_register(HMAX_ADDR, 0x30);
                sensor_write_register(HMAX_ADDR+1, 0x11);
            break;
            case 25:
                sensor_write_register(HMAX_ADDR, 0xA0);
                sensor_write_register(HMAX_ADDR+1, 0x14);
            break;
            default:
                break;
    	}
    }


    pstAeSnsDft->u32FullLinesStd = gu32FullLinesStd;
    gu32FullLines = gu32FullLinesStd;

    return;
}

static HI_VOID cmos_slow_framerate_set(HI_U16 u16FullLines,
    AE_SENSOR_DEFAULT_S *pstAeSnsDft)
{
    HI_U16 u16Vmax = u16FullLines;
    HI_U16 u16Current;
    u16Current = sensor_read_register(VMAX_ADDR+2);
    
    sensor_write_register(VMAX_ADDR, (u16Vmax&0x00ff));
    sensor_write_register(VMAX_ADDR+1, ((u16Vmax&0xff00) >> 8));
    //sensor_write_register(VMAX_ADDR+2,(((u16Vmax & 0x10000) >> 16)+(u16Current&0xFE)));

    pstAeSnsDft->u32MaxIntTime = u16Vmax - 2;
    gu32FullLines = u16Vmax;

    return;
}

static HI_VOID cmos_init_regs_info(HI_VOID)
{
#if CMOS_IMX236_ISP_WRITE_SENSOR_ENABLE
    HI_S32 i;

    if (HI_FALSE == gsbRegInit)
    {
        g_stSnsRegsInfo.enSnsType = ISP_SNS_I2C_TYPE;
        for (i=0; i<8; i++)
        {
            g_stSnsRegsInfo.astI2cData[i].u8DevAddr = sensor_i2c_addr;
            g_stSnsRegsInfo.astI2cData[i].u32AddrByteNum = sensor_addr_byte;
            g_stSnsRegsInfo.astI2cData[i].u32DataByteNum = sensor_data_byte;
        }
        g_stSnsRegsInfo.astI2cData[0].bDelayCfg = HI_FALSE;
        g_stSnsRegsInfo.astI2cData[0].u32RegAddr = 0x3020;
        g_stSnsRegsInfo.astI2cData[1].bDelayCfg = HI_FALSE;
        g_stSnsRegsInfo.astI2cData[1].u32RegAddr = 0x3021;
        g_stSnsRegsInfo.astI2cData[2].bDelayCfg = HI_FALSE;
        g_stSnsRegsInfo.astI2cData[2].u32RegAddr = 0x3022;
        switch(gu8SensorMode)
        {
            default:
            case 0: //linear mode
            g_stSnsRegsInfo.u32RegNum = 5;
            g_stSnsRegsInfo.astI2cData[3].bDelayCfg = HI_TRUE;
            g_stSnsRegsInfo.astI2cData[3].u32RegAddr = 0x3014;
            g_stSnsRegsInfo.astI2cData[4].bDelayCfg = HI_TRUE;
            g_stSnsRegsInfo.astI2cData[4].u32RegAddr = 0x3015;
            break;
            case 1: //WDR mode
            g_stSnsRegsInfo.u32RegNum = 8;
            g_stSnsRegsInfo.astI2cData[3].bDelayCfg = HI_FALSE;
            g_stSnsRegsInfo.astI2cData[3].u32RegAddr = 0x3023;
            g_stSnsRegsInfo.astI2cData[4].bDelayCfg = HI_FALSE;
            g_stSnsRegsInfo.astI2cData[4].u32RegAddr = 0x3024;
            g_stSnsRegsInfo.astI2cData[5].bDelayCfg = HI_FALSE;
            g_stSnsRegsInfo.astI2cData[5].u32RegAddr = 0x3025;
            
            g_stSnsRegsInfo.astI2cData[6].bDelayCfg = HI_TRUE;
            g_stSnsRegsInfo.astI2cData[6].u32RegAddr = 0x3014;
            g_stSnsRegsInfo.astI2cData[7].bDelayCfg = HI_TRUE;
            g_stSnsRegsInfo.astI2cData[7].u32RegAddr = 0x3015;
            break;
        }
        g_stSnsRegsInfo.bDelayCfgIspDgain = HI_TRUE;
        gsbRegInit = HI_TRUE;
    }
#endif
    return;
}


/* while isp notify ae to update sensor regs, ae call these funcs. */
static HI_VOID cmos_inttime_update(HI_U32 u32IntTime)
{
    HI_U16 u16ExpTime,u16Current;

    switch(gu8SensorMode)
    {
        default:
        case 0: //linear mode
            //Integration time = (VMAX - (SHS1)) + tOFFSET
            u16ExpTime = gu32FullLines - u32IntTime;
            u16Current = sensor_read_register(EXPOSURE_ADDR+2);

            #if CMOS_IMX236_ISP_WRITE_SENSOR_ENABLE
            cmos_init_regs_info();
            g_stSnsRegsInfo.astI2cData[0].u32Data = u16ExpTime & 0xFF;
            g_stSnsRegsInfo.astI2cData[1].u32Data = (u16ExpTime & 0xFF00) >> 8;
            g_stSnsRegsInfo.astI2cData[2].u32Data = (((u16ExpTime & 0x10000) >> 16)+(u16Current&0xFE));
            #else            
            sensor_write_register(EXPOSURE_ADDR, u16ExpTime & 0xFF);
            sensor_write_register(EXPOSURE_ADDR+1, (u16ExpTime & 0xFF00) >> 8);
            sensor_write_register(EXPOSURE_ADDR+2, (((u16ExpTime & 0x10000) >> 16)+(u16Current&0xFE)) );
            #endif
        break;
        case 1: //WDR mode
            #if CMOS_IMX236_ISP_WRITE_SENSOR_ENABLE
            //short exposure
            u16ExpTime = gu32FullLines - u32IntTime;
            u16Current = sensor_read_register(EXPOSURE_ADDR+2);
            
            cmos_init_regs_info();
            g_stSnsRegsInfo.astI2cData[0].u32Data = u16ExpTime & 0xFF;
            g_stSnsRegsInfo.astI2cData[1].u32Data = (u16ExpTime & 0xFF00) >> 8;
            g_stSnsRegsInfo.astI2cData[2].u32Data = (((u16ExpTime & 0x10000) >> 16)+(u16Current&0xFE));
            
            //long exposure
            u16ExpTime = gu32FullLines - (u32IntTime << 4);
            u16Current = sensor_read_register(LONG_EXPOSURE_ADDR+2);

            g_stSnsRegsInfo.astI2cData[3].u32Data = u16ExpTime & 0xFF;
            g_stSnsRegsInfo.astI2cData[4].u32Data = (u16ExpTime & 0xFF00) >> 8;
            g_stSnsRegsInfo.astI2cData[5].u32Data = (((u16ExpTime & 0x10000) >> 16)+(u16Current&0xFE));
            #else
            //short exposure
            u16ExpTime = gu32FullLines - u32IntTime;
            u16Current = sensor_read_register(EXPOSURE_ADDR+2);
            
            sensor_write_register(EXPOSURE_ADDR, u16ExpTime & 0xFF);
            sensor_write_register(EXPOSURE_ADDR+1, (u16ExpTime & 0xFF00) >> 8);
            sensor_write_register(EXPOSURE_ADDR+2, (((u16ExpTime & 0x10000) >> 16)+(u16Current&0xFE)) );

            //long exposure
            u16ExpTime = gu32FullLines - (u32IntTime << 4);
            u16Current = sensor_read_register(LONG_EXPOSURE_ADDR+2);
            
            sensor_write_register(LONG_EXPOSURE_ADDR, u16ExpTime & 0xFF);
            sensor_write_register(LONG_EXPOSURE_ADDR+1, (u16ExpTime & 0xFF00) >> 8);
            sensor_write_register(LONG_EXPOSURE_ADDR+2, (((u16ExpTime & 0x10000) >> 16)+(u16Current&0xFE)) );
            #endif
        break;
    }

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
   
    if (u32InTimes >= gain_table[480])
    {
         pstAeSnsGainInfo->u32SnsTimes = gain_table[480];
         pstAeSnsGainInfo->u32GainDb = 480;
         return ;
    }
    
    for(i = 1; i < 481; i++)
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
   
    if (u32InTimes >= gain_table[480])
    {
         pstAeSnsGainInfo->u32SnsTimes = gain_table[480];
         pstAeSnsGainInfo->u32GainDb = 480;
         return ;
    }
    
    for(i = 1; i < 481; i++)
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
    HI_U32 u32Tmp = u32Again + u32Dgain;
    switch(gu8SensorMode)
    {
        default:
        case 0: /* linear mode */
            if( u32Tmp > 0x1E0)
            {
                u32Tmp = 0x1E0;
            }
            #if CMOS_IMX236_ISP_WRITE_SENSOR_ENABLE
            cmos_init_regs_info();

            g_stSnsRegsInfo.astI2cData[3].u32Data = (u32Tmp & 0xff);
            g_stSnsRegsInfo.astI2cData[4].u32Data = ((u32Tmp & 0x100)>>8);

            HI_MPI_ISP_SnsRegsCfg(&g_stSnsRegsInfo);
            #else
            sensor_write_register(PGC_ADDR, u32Tmp & 0xff);
            sensor_write_register(PGC_ADDR + 1, (u32Tmp & 0x100) >> 8);
            #endif
        break;
        case 1: //WDR mode      
            #if CMOS_IMX236_ISP_WRITE_SENSOR_ENABLE
            cmos_init_regs_info();
            if(u32Dgain <= 120)
            {
                g_stSnsRegsInfo.astI2cData[6].u32Data = (u32Dgain & 0xff);
            }
            else
            {
                g_stSnsRegsInfo.astI2cData[6].u32Data = 0x78;
            }
            HI_MPI_ISP_SnsRegsCfg(&g_stSnsRegsInfo);
            #else
            if(u32Dgain <= 120)
            {
                sensor_write_register(PGC_ADDR, u32Dgain);
            }
            else
            {
                sensor_write_register(PGC_ADDR, 0x78);
            }
            #endif
        break;
    }

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

    pstAwbSnsDft->au16GainOffset[0] = 0x190;
    pstAwbSnsDft->au16GainOffset[1] = 0x100;
    pstAwbSnsDft->au16GainOffset[2] = 0x100;
    pstAwbSnsDft->au16GainOffset[3] = 0x204;

    pstAwbSnsDft->as32WbPara[0] = 62;
    pstAwbSnsDft->as32WbPara[1] = 68;
    pstAwbSnsDft->as32WbPara[2] = -125;
    pstAwbSnsDft->as32WbPara[3] = 201189;
    pstAwbSnsDft->as32WbPara[4] = 128;
    pstAwbSnsDft->as32WbPara[5] = -150451;

    memcpy(&pstAwbSnsDft->stCcm, &g_stAwbCcm, sizeof(AWB_CCM_S));
    memcpy(&pstAwbSnsDft->stAgcTbl, &g_stAwbAgcTable, sizeof(AWB_AGC_TABLE_S));
    
    return 0;
}

HI_VOID sensor_global_init()
{

   gu8SensorMode = 0;
   
#if CMOS_IMX236_ISP_WRITE_SENSOR_ENABLE
	gsbRegInit = HI_FALSE;
#endif
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
    s32Ret = HI_MPI_ISP_SensorRegCallBack(IMX236_ID, &stIspRegister);
    if (s32Ret)
    {
        printf("sensor register callback function failed!\n");
        return s32Ret;
    }
    
    stLib.s32Id = 0;
    strcpy(stLib.acLibName, HI_AE_LIB_NAME);
    cmos_init_ae_exp_function(&stAeRegister.stSnsExp);
    s32Ret = HI_MPI_AE_SensorRegCallBack(&stLib, IMX236_ID, &stAeRegister);
    if (s32Ret)
    {
        printf("sensor register callback function to ae lib failed!\n");
        return s32Ret;
    }

    stLib.s32Id = 0;
    strcpy(stLib.acLibName, HI_AWB_LIB_NAME);
    cmos_init_awb_exp_function(&stAwbRegister.stSnsExp);
    s32Ret = HI_MPI_AWB_SensorRegCallBack(&stLib, IMX236_ID, &stAwbRegister);
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

    s32Ret = HI_MPI_ISP_SensorUnRegCallBack(IMX236_ID);
    if (s32Ret)
    {
        printf("sensor unregister callback function failed!\n");
        return s32Ret;
    }
    
    stLib.s32Id = 0;
    strcpy(stLib.acLibName, HI_AE_LIB_NAME);
    s32Ret = HI_MPI_AE_SensorUnRegCallBack(&stLib, IMX236_ID);
    if (s32Ret)
    {
        printf("sensor unregister callback function to ae lib failed!\n");
        return s32Ret;
    }

    stLib.s32Id = 0;
    strcpy(stLib.acLibName, HI_AWB_LIB_NAME);
    s32Ret = HI_MPI_AWB_SensorUnRegCallBack(&stLib, IMX236_ID);
    if (s32Ret)
    {
        printf("sensor unregister callback function to ae lib failed!\n");
        return s32Ret;
    }
    
    gu8SensorMode = 0;
    return 0;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif // __IMX236_CMOS_H_
