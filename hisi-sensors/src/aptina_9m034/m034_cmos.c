#if !defined(__M034_CMOS_H_)
#define __M034_CMOS_H_

#include <stdio.h>
#include <string.h>
#include "m034_sensor_config.h"
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

#define M034_ID 9034

/*set Frame End Update Mode 2 with HI_MPI_ISP_SetAEAttr and set this value 1 to avoid flicker in antiflicker mode */
/*when use Frame End Update Mode 2, the speed of i2c will affect whole system's performance                       */
/*increase I2C_DFT_RATE in Hii2c.c to 400000 to increase the speed of i2c                                         */
#define CMOS_M034_ISP_WRITE_SENSOR_ENABLE (1)
/****************************************************************************
 * local variables                                                            *
 ****************************************************************************/

extern const unsigned int sensor_i2c_addr;
extern unsigned int sensor_addr_byte;
extern unsigned int sensor_data_byte;

HI_U8 gu8SensorMode = 0;
static HI_U32 gu32FullLinesStd = 750;

#if CMOS_M034_ISP_WRITE_SENSOR_ENABLE
ISP_SNS_REGS_INFO_S g_stSnsRegsInfo = {0};
#endif

static AWB_CCM_S g_stAwbCcm =
{
    5000,
    {
        0x0227,0x80CE,0x8059,
        0x8026,0x0151,0x802B,
        0x001E,0x80C0,0x01A2
    },

    3200,
    {
        0x1da,0x809e,0x803c,
        0x805f,0x175,0x8016,
        0x802c,0x8124,0x250
    },

    2500,
    {
        0x02AD,0x814A,0x8063,
        0x000C,0x00EB,0x0009,
        0x0031,0x81BE,0x028D
    }
};

static AWB_AGC_TABLE_S g_stAwbAgcTableLin =
{
    /* bvalid */
    1,

    /* saturation */
    {0x80,0x80,0x6C,0x48,0x44,0x40,0x3C,0x38}
};

static AWB_AGC_TABLE_S g_stAwbAgcTableWdr =
{
    /* bvalid */
    1,

    /* saturation */
    {0x80,0x80,0x80,0x80,0x70,0x60,0x50,0x40}
};


static ISP_CMOS_AGC_TABLE_S g_stIspAgcTableLin =
{
    /* bvalid */
    1,

    /* sharpen_alt_d */
    {0x50,0x48,0x40,0x38,0x34,0x30,0x28,0x20},
        
    /* sharpen_alt_ud */
    {0x90,0x88,0x80,0x70,0x58,0x40,0x20,0x0a},
        
    /* snr_thresh */
    {0x23,0x2c,0x34,0x3E,0x46,0x4E,0x54,0x5A},
        
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
     {
       0,  0,  0,  0,  0,  2,  9, 14, 16, 19, 21, 23, 24, 25, 27, 28, 
      29, 30, 31, 31, 32, 33, 33, 34, 35, 35, 36, 36, 37, 37, 38, 38, 
      39, 39, 39, 40, 40, 40, 41, 41, 42, 42, 42, 42, 43, 43, 43, 44, 
      44, 44, 44, 45, 45, 45, 45, 46, 46, 46, 46, 46, 47, 47, 47, 47, 
      47, 47, 48, 48, 48, 48, 48, 49, 49, 49, 49, 49, 49, 50, 50, 50, 
      50, 50, 50, 51, 51, 51, 51, 51, 51, 51, 51, 52, 52, 52, 52, 52, 
      52, 52, 53, 53, 53, 53, 53, 53, 53, 53, 53, 54, 54, 54, 54, 54, 
      54, 54, 54, 54, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 56, 56
     }, 
    
     /* demosaic_weight_lut */
    {
      2,  9, 14, 16, 19, 21, 23, 24, 25, 27, 28, 29, 30, 31, 31, 32, 
     33, 33, 34, 35, 35, 36, 36, 37, 37, 38, 38, 39, 39, 39, 40, 40, 
     40, 41, 41, 42, 42, 42, 42, 43, 43, 43, 44, 44, 44, 44, 45, 45, 
     45, 45, 46, 46, 46, 46, 46, 47, 47, 47, 47, 47, 47, 48, 48, 48, 
     48, 48, 49, 49, 49, 49, 49, 49, 50, 50, 50, 50, 50, 50, 51, 51, 
     51, 51, 51, 51, 51, 51, 52, 52, 52, 52, 52, 52, 52, 53, 53, 53, 
     53, 53, 53, 53, 53, 53, 54, 54, 54, 54, 54, 54, 54, 54, 54, 55, 
     55, 55, 55, 55, 55, 55, 55, 55, 55, 56, 56, 56, 56, 56, 56, 56
    }
};

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


static ISP_CMOS_DEMOSAIC_S g_stIspDemosaicLin =
{
	/* bvalid */
	 1,
	 
	 /*vh_slope*/
	 220,
	
	 /*aa_slope*/
	 200,
	
	 /*va_slope*/
	 185,
	
	 /*uu_slope*/
	 210,
	
	 /*sat_slope*/
	 93,
	
	 /*ac_slope*/
	 160,
	
	 /*vh_thresh*/
	 0,
	
	 /*aa_thresh*/
	 0,
	
	 /*va_thresh*/
	 0,
	
	 /*uu_thresh*/
	 8,
	
	 /*sat_thresh*/
	 0,
	
	 /*ac_thresh*/
	 0x1b3

};

static ISP_CMOS_DEMOSAIC_S g_stIspDemosaicWdr =
{
    /* bvalid */
    1,
    
    /*vh_slope*/
    220,

    /*aa_slope*/
    200,

    /*va_slope*/
    185,

    /*uu_slope*/
    210,

    /*sat_slope*/
    93,

    /*ac_slope*/
    160,

    /*vh_thresh*/
    0,

    /*aa_thresh*/
    0,

    /*va_thresh*/
    0,

    /*uu_thresh*/
    8,

    /*sat_thresh*/
    0,

    /*ac_thresh*/
    0x1b3
};


static ISP_CMOS_GAMMAFE_S g_stGammafe = 
{
    /* bvalid */
    1,

    { 
       0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,  16,  23,  28,
      32,  36,  39,  42,  45,  48,  51,  53,  55,  58,  60,  62,  64,  66,  68,  70,
      72,  73,  75,  77,  78,  80,  82,  83,  85,  86,  88,  89,  90,  92,  93,  95,
      96,  97,  99, 100, 101, 102, 104, 105, 106, 107, 108, 110, 111, 112, 113, 114,
     115, 116, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127, 128, 129, 130, 131,
     132, 133, 134, 135, 136, 137, 138, 139, 139, 140, 141, 142, 143, 144, 145, 146,
     147, 147, 148, 149, 150, 151, 152, 153, 153, 154, 155, 156, 157, 158, 158, 159,
     160, 161, 162, 162, 163, 164, 165, 165, 166, 167, 168, 169, 169, 170, 171, 172,
     172, 215, 250, 281, 309, 334, 358, 380, 401, 421, 440, 458, 476, 493, 509, 525,
     540, 555, 570, 584, 598, 611, 624, 637, 650, 663, 675, 687, 699, 710, 722, 733,
     744, 755, 766, 776, 787, 797, 807, 818, 827, 837, 847, 857, 866, 876, 885, 894,
     903, 912, 921, 930, 939, 947, 956, 965, 973, 981, 990, 998,1006,1014,1022,1143,
    1253,1353,1447,1535,1618,1697,1772,1845,1914,1982,2047,2110,2171,2231,2289,2345,
    2400,2454,2507,2559,2609,2659,2708,2756,2803,2849,2895,2940,2984,3028,3071,3113,
    3155,3196,3237,3277,3317,3356,3395,3433,3471,3509,3546,3583,3619,3655,3691,3726,
    3761,3796,3830,3864,3898,3931,3965,3997,4030,4063,4095,4095,4095,4095,4095,4095,4095
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
            pstDef->stComm.u8Rggb           = 0x1;      //1: grbg
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
        break;
        case 1:
            pstDef->stComm.u8Rggb           = 0x1;      //1: grbg 
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
        break;
    }

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
                pstBlackLevel->au16BlackLevel[i] = 0xA8;
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
        sensor_write_register(0x3012, 0x118);    //max exposure lines
        sensor_write_register(0x30B0, 0x1300);    //AG, Context A
        sensor_write_register(0x305E, 0x0020);    //DG, Context A
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

            /* Enable DCG */
            sensor_write_register(0x3100, 0x001E);

            /* Enable 1.25x analog gain */
            sensor_write_register(0x3EE4, 0xD308);
        break;

        //720P30 wdr
        case 1:
            gu8SensorMode = 1;
            printf("wdr mode\n");

            /* program sensor to wdr mode */
            sensor_prog(sensor_rom_30_wdr);

            /* Disable DCG */
            sensor_write_register(0x3100, 0x001A);

            /* Disable 1.25x analog gain */
            sensor_write_register(0x3EE4, 0xD208);
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
    pstAeSnsDft->u32FullLinesStd = gu32FullLinesStd;
    pstAeSnsDft->u32FlickerFreq = 0;//60*256;//50*256;


    pstAeSnsDft->stIntTimeAccu.enAccuType = AE_ACCURACY_LINEAR;
    pstAeSnsDft->stIntTimeAccu.f32Accuracy = 1;
    
    pstAeSnsDft->stAgainAccu.enAccuType = AE_ACCURACY_DB;
    pstAeSnsDft->stAgainAccu.f32Accuracy = 6;

    pstAeSnsDft->stDgainAccu.enAccuType = AE_ACCURACY_LINEAR;
    pstAeSnsDft->stDgainAccu.f32Accuracy = 0.03125;
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
            
            pstAeSnsDft->u32MaxAgain = 3;  /* 18db / 6db = 3 */
            pstAeSnsDft->u32MinAgain = 0;
            pstAeSnsDft->u32MaxAgainTarget = 3;
            pstAeSnsDft->u32MinAgainTarget = 0;
            
            pstAeSnsDft->u32MaxDgain = 255;  /* 8 / 0.03125 = 256 */
            pstAeSnsDft->u32MinDgain = 32;
            pstAeSnsDft->u32MaxDgainTarget = 256;
            pstAeSnsDft->u32MinDgainTarget = 32;
            
            pstAeSnsDft->u32MaxISPDgainTarget = 4 << pstAeSnsDft->u32ISPDgainShift;
        break;
        case 1: //WDR mode
            pstAeSnsDft->au8HistThresh[0] = 0x20;
            pstAeSnsDft->au8HistThresh[1] = 0x40;
            pstAeSnsDft->au8HistThresh[2] = 0x60;
            pstAeSnsDft->au8HistThresh[3] = 0x80;
            
            pstAeSnsDft->u8AeCompensation = 0x40;

            pstAeSnsDft->u32MaxIntTime = 675;
            pstAeSnsDft->u32MinIntTime = 128;
            pstAeSnsDft->u32MaxIntTimeTarget = 675;  /* for short exposure, Exposure ratio = 16X */
            pstAeSnsDft->u32MinIntTimeTarget = 128;

            pstAeSnsDft->u32MaxAgain = 3;  /* 18db / 6db = 3 */
            pstAeSnsDft->u32MinAgain = 0;
            pstAeSnsDft->u32MaxAgainTarget = 3;
            pstAeSnsDft->u32MinAgainTarget = 0;
            
            pstAeSnsDft->u32MaxDgain = 255;  /* 8 / 0.03125 = 256 */
            pstAeSnsDft->u32MinDgain = 32;
            pstAeSnsDft->u32MaxDgainTarget = 256;
            pstAeSnsDft->u32MinDgainTarget = 32;
            
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
        pstAeSnsDft->u32MaxIntTime = 675;
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
#if CMOS_M034_ISP_WRITE_SENSOR_ENABLE
    HI_S32 i;
    static HI_BOOL bInit = HI_FALSE;

    if (HI_FALSE == bInit)
    {
        g_stSnsRegsInfo.enSnsType = ISP_SNS_I2C_TYPE;
        g_stSnsRegsInfo.u32RegNum = 3;
        for (i=0; i<3; i++)
        {
            g_stSnsRegsInfo.astI2cData[i].u8DevAddr = sensor_i2c_addr;
            g_stSnsRegsInfo.astI2cData[i].u32AddrByteNum = sensor_addr_byte;
            g_stSnsRegsInfo.astI2cData[i].u32DataByteNum = sensor_data_byte;
        }
        g_stSnsRegsInfo.astI2cData[0].bDelayCfg = HI_FALSE;
        g_stSnsRegsInfo.astI2cData[0].u32RegAddr = 0x3012;
        g_stSnsRegsInfo.astI2cData[1].bDelayCfg = HI_FALSE;
        g_stSnsRegsInfo.astI2cData[1].u32RegAddr = 0x30B0;
        g_stSnsRegsInfo.astI2cData[2].bDelayCfg = HI_FALSE;
        g_stSnsRegsInfo.astI2cData[2].u32RegAddr = 0x305E;
        g_stSnsRegsInfo.bDelayCfgIspDgain = HI_FALSE;

        bInit = HI_TRUE;
    }
#endif
    return;
}


/* while isp notify ae to update sensor regs, ae call these funcs. */
static HI_VOID cmos_inttime_update(HI_U32 u32IntTime)
{
#if CMOS_M034_ISP_WRITE_SENSOR_ENABLE
    cmos_init_regs_info();
    g_stSnsRegsInfo.astI2cData[0].u32Data = u32IntTime;
#else
    sensor_write_register(0x3012, u32IntTime);
#endif
    return;
}

static HI_VOID cmos_gains_update(HI_U32 u32Again, HI_U32 u32Dgain)
{
#if CMOS_M034_ISP_WRITE_SENSOR_ENABLE
    cmos_init_regs_info();

    switch(u32Again)
    {
        case 0:
            g_stSnsRegsInfo.astI2cData[1].u32Data = 0x1300;
            break;
        case 1:
            g_stSnsRegsInfo.astI2cData[1].u32Data = 0x1310;
            break;
        case 2:
            g_stSnsRegsInfo.astI2cData[1].u32Data = 0x1320;
            break;
        case 3:
            g_stSnsRegsInfo.astI2cData[1].u32Data = 0x1330;
            break;
    }

    g_stSnsRegsInfo.astI2cData[2].u32Data = u32Dgain;
    HI_MPI_ISP_SnsRegsCfg(&g_stSnsRegsInfo);
#else
    switch(u32Again)
    {
        case 0:
            sensor_write_register(0x30B0, 0x1300);
            break;
        case 1:
            sensor_write_register(0x30B0, 0x1310);
            break;
        case 2:
            sensor_write_register(0x30B0, 0x1320);
            break;
        case 3:
            sensor_write_register(0x30B0, 0x1330);
            break;
    }

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

    pstAwbSnsDft->u16WbRefTemp = 5000;

    pstAwbSnsDft->au16GainOffset[0] = 0x018B;
    pstAwbSnsDft->au16GainOffset[1] = 0x100;
    pstAwbSnsDft->au16GainOffset[2] = 0x100;
    pstAwbSnsDft->au16GainOffset[3] = 0x01A2;

    pstAwbSnsDft->as32WbPara[0] = 88;
    pstAwbSnsDft->as32WbPara[1] = -17;
    pstAwbSnsDft->as32WbPara[2] = -185;
    pstAwbSnsDft->as32WbPara[3] = 239197;
    pstAwbSnsDft->as32WbPara[4] = 128;
    pstAwbSnsDft->as32WbPara[5] = -193985;

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
    s32Ret = HI_MPI_ISP_SensorRegCallBack(M034_ID, &stIspRegister);
    if (s32Ret)
    {
        printf("sensor register callback function failed!\n");
        return s32Ret;
    }
    
    stLib.s32Id = 0;
    strcpy(stLib.acLibName, HI_AE_LIB_NAME);
    cmos_init_ae_exp_function(&stAeRegister.stSnsExp);
    s32Ret = HI_MPI_AE_SensorRegCallBack(&stLib, M034_ID, &stAeRegister);
    if (s32Ret)
    {
        printf("sensor register callback function to ae lib failed!\n");
        return s32Ret;
    }

    stLib.s32Id = 0;
    strcpy(stLib.acLibName, HI_AWB_LIB_NAME);
    cmos_init_awb_exp_function(&stAwbRegister.stSnsExp);
    s32Ret = HI_MPI_AWB_SensorRegCallBack(&stLib, M034_ID, &stAwbRegister);
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

    s32Ret = HI_MPI_ISP_SensorUnRegCallBack(M034_ID);
    if (s32Ret)
    {
        printf("sensor unregister callback function failed!\n");
        return s32Ret;
    }
    
    stLib.s32Id = 0;
    strcpy(stLib.acLibName, HI_AE_LIB_NAME);
    s32Ret = HI_MPI_AE_SensorUnRegCallBack(&stLib, M034_ID);
    if (s32Ret)
    {
        printf("sensor unregister callback function to ae lib failed!\n");
        return s32Ret;
    }

    stLib.s32Id = 0;
    strcpy(stLib.acLibName, HI_AWB_LIB_NAME);
    s32Ret = HI_MPI_AWB_SensorUnRegCallBack(&stLib, M034_ID);
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
