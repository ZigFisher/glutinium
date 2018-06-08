#if !defined(__IMX104_CMOS_H_)
#define __IMX104_CMOS_H_

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

#define IMX104_ID 104
/*set Frame End Update Mode 2 with HI_MPI_ISP_SetAEAttr and set this value 1 to avoid flicker */
/*when use Frame End Update Mode 2, the speed of i2c will affect whole system's performance   */
/*increase I2C_DFT_RATE in Hii2c.c to 400000 to increase the speed of i2c                     */
#define CMOS_IMX104_ISP_WRITE_SENSOR_ENABLE (1)
/****************************************************************************
 * local variables                                                            *
 ****************************************************************************/

HI_U8 gu8SensorMode = 0;
#if CMOS_IMX104_ISP_WRITE_SENSOR_ENABLE
ISP_SNS_REGS_INFO_S g_stSnsRegsInfo = {0};
static HI_BOOL gsbRegInit = HI_FALSE;
#endif
static HI_U32 gu32FullLinesStd = 750;
static HI_U32 gu32FullLines = 750;

static AWB_CCM_S g_stAwbCcm =
{
    5000,
    {
        0x01D3, 0x80A0, 0x8033,
        0x8036, 0x0192, 0x805C,
        0x0023, 0x80CC, 0x01A9,
    },
    3200,
    {
        0x01DA, 0x8094, 0x8046,
        0x8064, 0x01B0, 0x804C,
        0x0034, 0x8107, 0x01D3,
    },
    2600,
    {
        0x0231, 0x80E9, 0x8048,
        0x8019, 0x014D, 0x8034,
        0x0079, 0x81CA, 0x0251,
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
    {80,75,70,65,60,50,40,30},
        
    /* sharpen_alt_ud */
    {75,70,65,60,50,40,30,20},
        
    /* snr_thresh */
    {8,12,18,26,36,46,56,70},
        
    /* demosaic_lum_thresh */
    {0x50,0x50,0x40,0x40,0x30,0x30,0x20,0x20},
        
    /* demosaic_np_offset */
    {0,8,16,24,32,40,48,56},
        
    /* ge_strength */
    {0x55,0x55,0x55,0x55,0x55,0x55,0x37,0x37}
};

static ISP_CMOS_NOISE_TABLE_S g_stIspNoiseTableLin =
{
    /* bvalid */
    1,
    
    /* nosie_profile_weight_lut */
    {
        0,0,0,0,0,0,0,15,25,29,31,33,32,35,36,37,37,38,39,39,40,40,40,41,41,41,42,42,42,43,43,43,43,44,44,44,44,44,45,45,45,45,45,45,45,46,46,46,46,46,46,47,47,47,47,47,47,47,47,48,48,48,48,48,48,48,48,48,49,49,49,49,49,49,49,49,49,49,49,49,50,50,50,50,50,50,50,50,50,50,50,50,50,50,51,51,51,51,51,51,51,51,51,51,51,51,51,51,51,51,51,52,52,52,52,52,52,52,52,52,52,52,52,52,52,52,52,52
    },

    /* demosaic_weight_lut */
    {
        0,15,25,29,31,33,34,35,36,37,37,38,39,39,40,40,40,41,41,41,42,42,42,43,43,43,43,44,44,44,44,44,45,45,45,45,45,45,46,46,46,46,46,46,46,47,47,47,47,47,47,47,47,48,48,48,48,48,48,48,48,48,49,49,49,49,49,49,49,49,49,49,49,49,49,50,50,50,50,50,50,50,50,50,50,50,50,50,51,51,51,51,51,51,51,51,51,51,51,51,51,51,51,51,52,52,52,52,52,52,52,52,52,52,52,52,52,52,52,52,52,52,52,52,52,52,52,52
    }
};

static ISP_CMOS_NOISE_TABLE_S g_stIspNoiseTableWdr =
{
    /* bvalid */
    1,
    
    /* nosie_profile_weight_lut */
    {
        0,37,37,37,37,37,37,37,37,37,37,37,37,37,37,37,37,37,37,37,37,37,37,37,37,37,45,48,57,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64
    },

    /* demosaic_weight_lut */
    {
        0,37,37,37,37,37,37,37,37,37,37,37,37,37,37,37,37,37,37,37,37,37,37,37,37,37,45,48,57,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64
    }
};


static ISP_CMOS_DEMOSAIC_S g_stIspDemosaic =
{
    /* bvalid */
    1,
    
    /*vh_slope*/
    0xc6,

    /*aa_slope*/
    0xc3,

    /*va_slope*/
    0xc4,

    /*uu_slope*/
    0xcd,

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
    0x6,

    /*sat_thresh*/
    0x171,

    /*ac_thresh*/
    0x1b3,
};

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
            pstDef->stComm.u8Rggb           = 0x2;      //2: gbrg  
            pstDef->stComm.u8BalanceFe      = 0x1;

            pstDef->stDenoise.u8SinterThresh= 0x8;
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
            pstDef->stComm.u8Rggb           = 0x2;      //2: gbrg  
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
        sensor_write_register(VMAX_ADDR, 0x94);
        sensor_write_register(VMAX_ADDR + 1, 0x11);

        /* max Exposure time */
        sensor_write_register(EXPOSURE_ADDR, 0x00);
        sensor_write_register(EXPOSURE_ADDR + 1, 0x00);

        /* Analog and Digital gains both must be programmed for their minimum values */
        sensor_write_register(PGC_ADDR, 0x00);
    }
    else /* setup for ISP 'normal mode' */
    {
        sensor_write_register(VMAX_ADDR, 0xEE);
        sensor_write_register(VMAX_ADDR + 1, 0x02);
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
            sensor_write_register(0x20C, 0x00);
            sensor_write_register(0x20F, 0x01);
            sensor_write_register(0x210, 0x39);
            sensor_write_register(0x212, 0x50);
            sensor_write_register(0x265, 0x20);
            sensor_write_register(0x286, 0x01);
            sensor_write_register(0x2CF, 0xD1);
            sensor_write_register(0x2D0, 0x1B);
            sensor_write_register(0x2D2, 0x5F);
            sensor_write_register(0x2D3, 0x00);
            sensor_write_register(0x461, 0x9B);
            sensor_write_register(0x466, 0xD0);
            sensor_write_register(0x467, 0x08);
           
            printf("imx104 linear mode\n");
        break;
        //sensor mode 1
        case 1:
            gu8SensorMode = 1;
            sensor_write_register(0x20C, 0x02);
            sensor_write_register(0x20F, 0x05);
            sensor_write_register(0x210, 0x38);
            sensor_write_register(0x212, 0x0F);
            sensor_write_register(0x265, 0x00);
            sensor_write_register(0x286, 0x10);
            sensor_write_register(0x2CF, 0xE1);
            sensor_write_register(0x2D0, 0x29);
            sensor_write_register(0x2D2, 0x9B);
            sensor_write_register(0x2D3, 0x01);
            sensor_write_register(0x461, 0x9B);
            sensor_write_register(0x466, 0xD0);
            sensor_write_register(0x467, 0x08);

            printf("imx104 wdr mode\n");
        break;

        default:
            printf("NOT support this mode!\n");
            return;
        break;
    }
#if CMOS_IMX104_ISP_WRITE_SENSOR_ENABLE
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

    gu32FullLinesStd = 750;
    
    pstAeSnsDft->u32LinesPer500ms = 750*30/2;
    pstAeSnsDft->u32FullLinesStd = gu32FullLinesStd;
    pstAeSnsDft->u32FlickerFreq = 0;//60*256;//50*256;

    pstAeSnsDft->stIntTimeAccu.enAccuType = AE_ACCURACY_LINEAR;
    pstAeSnsDft->stIntTimeAccu.f32Accuracy = 1;
    pstAeSnsDft->u32MaxIntTime = 748;
    pstAeSnsDft->u32MinIntTime = 2;

    pstAeSnsDft->stAgainAccu.enAccuType = AE_ACCURACY_TABLE;
    pstAeSnsDft->stAgainAccu.f32Accuracy = 0.3;

    pstAeSnsDft->stDgainAccu.enAccuType = AE_ACCURACY_TABLE;
    pstAeSnsDft->stDgainAccu.f32Accuracy = 0.3;

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
            
            pstAeSnsDft->u8AeCompensation = 0x40;
            
            pstAeSnsDft->u32MaxIntTimeTarget = 65535;
            pstAeSnsDft->u32MinIntTimeTarget = 2;
            
            pstAeSnsDft->u32MaxAgain = 16229;  /* */
            pstAeSnsDft->u32MinAgain = 1024;
            pstAeSnsDft->u32MaxAgainTarget = 16229;
            pstAeSnsDft->u32MinAgainTarget = 1024;
            
            pstAeSnsDft->u32MaxDgain = 16229;  /*  */
            pstAeSnsDft->u32MinDgain = 1024;
            pstAeSnsDft->u32MaxDgainTarget = 16229;
            pstAeSnsDft->u32MinDgainTarget = 1024;
            
            pstAeSnsDft->u32MaxISPDgainTarget = 3 << pstAeSnsDft->u32ISPDgainShift;
        break;
        case 1: //WDR mode
            pstAeSnsDft->au8HistThresh[0] = 0x20;
            pstAeSnsDft->au8HistThresh[1] = 0x40;
            pstAeSnsDft->au8HistThresh[2] = 0x60;
            pstAeSnsDft->au8HistThresh[3] = 0x80;
            
            pstAeSnsDft->u8AeCompensation = 0x40;

        
            pstAeSnsDft->u32MaxIntTimeTarget = 46;  /* for short exposure, Exposure ratio = 16X */
            pstAeSnsDft->u32MinIntTimeTarget = 5;

            pstAeSnsDft->u32MaxAgain = 1024;  /* 4.5db fixed */
            pstAeSnsDft->u32MinAgain = 1024;
            pstAeSnsDft->u32MaxAgainTarget = 1024;
            pstAeSnsDft->u32MinAgainTarget = 1024;
            
            pstAeSnsDft->u32MaxDgain = 4096;  /*  */
            pstAeSnsDft->u32MinDgain = 1024;
            pstAeSnsDft->u32MaxDgainTarget = 1024;
            pstAeSnsDft->u32MinDgainTarget = 1024;
            
            pstAeSnsDft->u32MaxISPDgainTarget = 32 << pstAeSnsDft->u32ISPDgainShift;
        break;
    }

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
            sensor_write_register(VMAX_ADDR, 0xEE);
            sensor_write_register(VMAX_ADDR+1, 0x02);
        break;
        
        case 25:
            // Change the frame rate via changing the vertical blanking
            gu32FullLinesStd = 900;
            pstAeSnsDft->u32MaxIntTime = 898;
            pstAeSnsDft->u32LinesPer500ms = 900 * 25 / 2;
            sensor_write_register(VMAX_ADDR, 0x84);
            sensor_write_register(VMAX_ADDR+1, 0x03);
        break;
        
        default:
        break;
    }

    if(1 == gu8SensorMode)
    {
        pstAeSnsDft->u32MaxIntTime = 46;
    }

    pstAeSnsDft->u32FullLinesStd = gu32FullLinesStd;
    gu32FullLines = gu32FullLinesStd;

    return;
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


static HI_VOID cmos_slow_framerate_set(HI_U16 u16FullLines,
    AE_SENSOR_DEFAULT_S *pstAeSnsDft)
{
    HI_U16 u16Vmax = u16FullLines;
    HI_U16 u16Current;
    u16Current = sensor_read_register(VMAX_ADDR+2);
    
    sensor_write_register(VMAX_ADDR, (u16Vmax&0x00ff));
    sensor_write_register(VMAX_ADDR+1, ((u16Vmax&0xff00) >> 8));
    sensor_write_register(VMAX_ADDR+2,(((u16Vmax & 0x10000) >> 16)+(u16Current&0xFE)));

    pstAeSnsDft->u32MaxIntTime = u16Vmax - 2;
    gu32FullLines = u16Vmax;
    
    return;
}

static HI_VOID cmos_init_regs_info(HI_VOID)
{
#if CMOS_IMX104_ISP_WRITE_SENSOR_ENABLE
    HI_S32 i;

    if (HI_FALSE == gsbRegInit)
    {
        g_stSnsRegsInfo.enSnsType = ISP_SNS_SSP_TYPE;
        for (i=0; i<7; i++)
        {
            g_stSnsRegsInfo.astSspData[i].u32DevAddr = 0x02;
            g_stSnsRegsInfo.astSspData[i].u32DevAddrByteNum = 1;
            g_stSnsRegsInfo.astSspData[i].u32RegAddrByteNum = 1;
            g_stSnsRegsInfo.astSspData[i].u32DataByteNum = 1;
        }
        g_stSnsRegsInfo.astSspData[0].bDelayCfg = HI_FALSE;
        g_stSnsRegsInfo.astSspData[0].u32RegAddr = 0x20;
        g_stSnsRegsInfo.astSspData[1].bDelayCfg = HI_FALSE;
        g_stSnsRegsInfo.astSspData[1].u32RegAddr = 0x21;
        g_stSnsRegsInfo.astSspData[2].bDelayCfg = HI_FALSE;
        g_stSnsRegsInfo.astSspData[2].u32RegAddr = 0x22;
        switch(gu8SensorMode)
        {
            default:
            case 0: //linear mode
            g_stSnsRegsInfo.u32RegNum = 4;
            g_stSnsRegsInfo.astSspData[3].bDelayCfg = HI_TRUE;
            g_stSnsRegsInfo.astSspData[3].u32RegAddr = 0x14;
			
            break;
            case 1: //WDR mode
            g_stSnsRegsInfo.u32RegNum = 7;
            g_stSnsRegsInfo.astSspData[3].bDelayCfg = HI_FALSE;
            g_stSnsRegsInfo.astSspData[3].u32RegAddr = 0x23;
            g_stSnsRegsInfo.astSspData[4].bDelayCfg = HI_FALSE;
            g_stSnsRegsInfo.astSspData[4].u32RegAddr = 0x24;
            g_stSnsRegsInfo.astSspData[5].bDelayCfg = HI_FALSE;
            g_stSnsRegsInfo.astSspData[5].u32RegAddr = 0x25;
            g_stSnsRegsInfo.astSspData[6].bDelayCfg = HI_TRUE;
            g_stSnsRegsInfo.astSspData[6].u32RegAddr = 0x14;
			
            break;
        }
        g_stSnsRegsInfo.bDelayCfgIspDgain = HI_TRUE;

        gsbRegInit = HI_TRUE;
    }
#endif
    return;
}


static  HI_U32   gain_table[81]={

 1024,   1060,   1097,   1136,   1176,   1217,   1260,   1304,   1350,   1397,   1446,   1497,   1550,   1604,   1661,   1719,   1780,
 1842,   1907,   1974,   2043,   2115,   2189,   2266,   2346,   2428,   2514,   2602,   2693,   2788,   2886,   2987,   3092,
 3201,   3314,   3430,   3551,   3675,   3805,   3938,   4077,   4220,   4368,   4522,   4681,   4845,   5015,   5192,   5374,
 5563,   5758,   5961,   6170,   6387,   6611,   6844,   7084,   7333,   7591,   7858,   8134,   8420,   8716,   9022,   9339,
 9667,  10007,  10359,  10723,  11099,  11489,  11893,  12311,  12744,  13192,  13655,  14135,  14632,  15146,  15678,   16229

};


/* while isp notify ae to update sensor regs, ae call these funcs. */
static HI_VOID cmos_inttime_update(HI_U32 u32IntTime)
{
    HI_U16 u16ExpTime,u16Current;

    switch(gu8SensorMode)
    {
        default:
        case 0: //linear mode
            //Integration time = (VMAX - (SHS1+1)) + tOFFSET
            u16ExpTime = gu32FullLines - u32IntTime - 1;
            u16Current = sensor_read_register(EXPOSURE_ADDR+2);
			
            #if CMOS_IMX104_ISP_WRITE_SENSOR_ENABLE
            cmos_init_regs_info();
            g_stSnsRegsInfo.astSspData[0].u32Data = u16ExpTime & 0xFF;
            g_stSnsRegsInfo.astSspData[1].u32Data = (u16ExpTime & 0xFF00) >> 8;
            g_stSnsRegsInfo.astSspData[2].u32Data = (((u16ExpTime & 0x10000) >> 16)+(u16Current&0xFE));
            #else            
            sensor_write_register(EXPOSURE_ADDR, u16ExpTime & 0xFF);
            sensor_write_register(EXPOSURE_ADDR+1, (u16ExpTime & 0xFF00) >> 8);
            sensor_write_register(EXPOSURE_ADDR+2, (((u16ExpTime & 0x10000) >> 16)+(u16Current&0xFE)) );
            #endif
        break;
        case 1: //WDR mode
            #if CMOS_IMX104_ISP_WRITE_SENSOR_ENABLE
            //short exposure
            u16ExpTime = gu32FullLines - u32IntTime - 1;
            u16Current = sensor_read_register(EXPOSURE_ADDR+2);
            
            cmos_init_regs_info();
            g_stSnsRegsInfo.astSspData[0].u32Data = u16ExpTime & 0xFF;
            g_stSnsRegsInfo.astSspData[1].u32Data = (u16ExpTime & 0xFF00) >> 8;
            g_stSnsRegsInfo.astSspData[2].u32Data = (((u16ExpTime & 0x10000) >> 16)+(u16Current&0xFE));
            
            //long exposure
            u16ExpTime = gu32FullLines - (u32IntTime << 4) - 1;
            u16Current = sensor_read_register(LONG_EXPOSURE_ADDR+2);

            g_stSnsRegsInfo.astSspData[3].u32Data = u16ExpTime & 0xFF;
            g_stSnsRegsInfo.astSspData[4].u32Data = (u16ExpTime & 0xFF00) >> 8;
            g_stSnsRegsInfo.astSspData[5].u32Data = (((u16ExpTime & 0x10000) >> 16)+(u16Current&0xFE));
            #else
            //short exposure
            u16ExpTime = gu32FullLines - u32IntTime - 1;
            u16Current = sensor_read_register(EXPOSURE_ADDR+2);
            
            sensor_write_register(EXPOSURE_ADDR, u16ExpTime & 0xFF);
            sensor_write_register(EXPOSURE_ADDR+1, (u16ExpTime & 0xFF00) >> 8);
            sensor_write_register(EXPOSURE_ADDR+2, (((u16ExpTime & 0x10000) >> 16)+(u16Current&0xFE)) );

            //long exposure
            u16ExpTime = gu32FullLines - (u32IntTime << 4) - 1;
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
    
    if (u32InTimes >= gain_table[80])
    {
         pstAeSnsGainInfo->u32SnsTimes = gain_table[80];
         pstAeSnsGainInfo->u32GainDb = 80;
         return ;
    }
    
    for(i = 1; i < 81; i++)
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
    
    if (u32InTimes >= gain_table[80])
    {
         pstAeSnsGainInfo->u32SnsTimes = gain_table[80];
         pstAeSnsGainInfo->u32GainDb = 80;
         return ;
    }
    
    for(i = 1; i < 81; i++)
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
    switch(gu8SensorMode)
    {
        default:
        case 0: /* linear mode */
            /* analog gain = APGC * 0.3 db, APGC = [0x0,0x50], ag_db=[0db, 24db] */
            /* digital gain = DPGC * 0.3 db, DPCG = [0x50,0xA0], dg_db=[0db, 24db] */
            #if CMOS_IMX104_ISP_WRITE_SENSOR_ENABLE
            cmos_init_regs_info();
		
            if((u32Again + u32Dgain) <= 0xA0)
            {
                g_stSnsRegsInfo.astSspData[3].u32Data = u32Again + u32Dgain;
            }
            else
            {
                g_stSnsRegsInfo.astSspData[3].u32Data = 0xA0;
            }
            HI_MPI_ISP_SnsRegsCfg(&g_stSnsRegsInfo);
            #else
            if((u32Again + u32Dgain) <= 0xA0)
            {
                sensor_write_register(PGC_ADDR, u32Again + u32Dgain);
            }
            else
            {
                sensor_write_register(PGC_ADDR, 0xA0);
            }
            #endif
        break;
        case 1: //WDR mode
        
            /* analog gain : 4.5dB fixed
              * digital gain : 0 to 12dB  0.3dB step
              * DPGC = [00h,28h];
              * digital_gain = DPGC * 0.3 db. */
            #if CMOS_IMX104_ISP_WRITE_SENSOR_ENABLE
            cmos_init_regs_info();
            if(u32Dgain <= 40)
            {
                g_stSnsRegsInfo.astSspData[6].u32Data = u32Dgain;
            }
            else
            {
                g_stSnsRegsInfo.astSspData[6].u32Data = 0x28;
            }
            HI_MPI_ISP_SnsRegsCfg(&g_stSnsRegsInfo);
            #else
            if(u32Dgain <= 40)
            {
                sensor_write_register(PGC_ADDR, u32Dgain);
            }
            else
            {
                sensor_write_register(PGC_ADDR, 0x28);
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

    pstAwbSnsDft->u16WbRefTemp = 5000;

    pstAwbSnsDft->au16GainOffset[0] = 0x01D6;
    pstAwbSnsDft->au16GainOffset[1] = 0x100;
    pstAwbSnsDft->au16GainOffset[2] = 0x100;
    pstAwbSnsDft->au16GainOffset[3] = 0x01DA;

    pstAwbSnsDft->as32WbPara[0] = 19;
    pstAwbSnsDft->as32WbPara[1] = 147;
    pstAwbSnsDft->as32WbPara[2] = -89;
    pstAwbSnsDft->as32WbPara[3] = 186629;
    pstAwbSnsDft->as32WbPara[4] = 128;
    pstAwbSnsDft->as32WbPara[5] = -137989;

    memcpy(&pstAwbSnsDft->stCcm, &g_stAwbCcm, sizeof(AWB_CCM_S));
    memcpy(&pstAwbSnsDft->stAgcTbl, &g_stAwbAgcTable, sizeof(AWB_AGC_TABLE_S));
    
    return 0;
}

HI_VOID sensor_global_init()
{

   gu8SensorMode = 0;
   
#if CMOS_IMX104_ISP_WRITE_SENSOR_ENABLE
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
    s32Ret = HI_MPI_ISP_SensorRegCallBack(IMX104_ID, &stIspRegister);
    if (s32Ret)
    {
        printf("sensor register callback function failed!\n");
        return s32Ret;
    }
    
    stLib.s32Id = 0;
    strcpy(stLib.acLibName, HI_AE_LIB_NAME);
    cmos_init_ae_exp_function(&stAeRegister.stSnsExp);
    s32Ret = HI_MPI_AE_SensorRegCallBack(&stLib, IMX104_ID, &stAeRegister);
    if (s32Ret)
    {
        printf("sensor register callback function to ae lib failed!\n");
        return s32Ret;
    }

    stLib.s32Id = 0;
    strcpy(stLib.acLibName, HI_AWB_LIB_NAME);
    cmos_init_awb_exp_function(&stAwbRegister.stSnsExp);
    s32Ret = HI_MPI_AWB_SensorRegCallBack(&stLib, IMX104_ID, &stAwbRegister);
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

    s32Ret = HI_MPI_ISP_SensorUnRegCallBack(IMX104_ID);
    if (s32Ret)
    {
        printf("sensor unregister callback function failed!\n");
        return s32Ret;
    }
    
    stLib.s32Id = 0;
    strcpy(stLib.acLibName, HI_AE_LIB_NAME);
    s32Ret = HI_MPI_AE_SensorUnRegCallBack(&stLib, IMX104_ID);
    if (s32Ret)
    {
        printf("sensor unregister callback function to ae lib failed!\n");
        return s32Ret;
    }

    stLib.s32Id = 0;
    strcpy(stLib.acLibName, HI_AWB_LIB_NAME);
    s32Ret = HI_MPI_AWB_SensorUnRegCallBack(&stLib, IMX104_ID);
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
