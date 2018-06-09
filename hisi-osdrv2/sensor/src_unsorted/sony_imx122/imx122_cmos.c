#if !defined(__IMX122_CMOS_H_)
#define __IMX122_CMOS_H_

#include <stdio.h>
#include <string.h>
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

#define EXPOSURE_ADDR (0x208) //2:chip_id, 0C: reg addr.

#define PGC_ADDR (0x21E)
#define VMAX_ADDR (0x205)

#define IMX122_ID 122

#define CMOS_IMX122_ISP_WRITE_SENSOR_ENABLE  1
#define SENSOR_720P_30FPS_MODE   1
#define SENSOR_720P_60FPS_MODE   2
#define SENSOR_1080P_30FPS_MODE  3

/****************************************************************************
 * local variables                                                            *
 ****************************************************************************/




#if CMOS_IMX122_ISP_WRITE_SENSOR_ENABLE
ISP_SNS_REGS_INFO_S g_stSnsRegsInfo = {0};
static HI_BOOL gsbRegInit = HI_FALSE;
#endif


HI_U32 gu32FullLinesStd = 1125;
HI_U32 gu32FullLines = 1125;
HI_U8 gu8SensorMode = 0;

HI_U8 gu8SensorImageMode = 3;

extern void sensor_init_720p_60fps();

extern void sensor_init_720p_30fps();


static AWB_CCM_S g_stAwbCcm =
{
#if 0
    5000,
	{	0x1b7,  0x8079, 0x803d,
		0x806d, 0x01f2, 0x8084,
		0x800a, 0x80b9, 0x01c4
	},
	3200,
    {
        0x01e7, 0x80cd, 0x801a,
        0x808f, 0x01d3, 0x8044,
        0x001b, 0x813b, 0x021f
    },
    2600,
    {
        0x020a, 0x80ed, 0x801d,
        0x806e, 0x0196, 0x8028,
        0x0015, 0x820f, 0x02f9
    }
#else
    5000,
	{	0x024d, 0x8140, 0x800d,
		0x8052, 0x01cf, 0x807d,
		0x0010, 0x80e0, 0x01d0
	},
	3200,
    {
        0x0213, 0x80e8, 0x802b,
        0x8080, 0x01d3, 0x8053,
        0x0012, 0x80ea, 0x01d8
    },
    2600,
    {
        0x0226, 0x80ea, 0x803c,
        0x8067, 0x01d7, 0x8070,
        0x000a, 0x810a, 0x0200
    }
#endif
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
    {0x50,0x4b,0x46,0x41,0x3c,0x32,0x28,0x1e},
        
    /* sharpen_alt_ud */
    {0x4b,0x46,0x41,0x3c,0x32,0x28,0x1e,0x14},
        
    /* snr_thresh */
    {0x8,0xe,0x14,0x1e,0x28,0x32,0x40,0x50},
        
    /* demosaic_lum_thresh */
    {0x50,0x50,0x40,0x40,0x30,0x30,0x20,0x20},
        
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

static ISP_CMOS_DEMOSAIC_S g_stIspDemosaic =
{
    /* bvalid */
    1,
    
    /*vh_slope*/
    0xc5,

    /*aa_slope*/
    0xbb,

    /*va_slope*/
    0xc8,

    /*uu_slope*/
    0xca,

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

HI_U32 cmos_get_isp_default(ISP_CMOS_DEFAULT_S *pstDef)
{
    if (HI_NULL == pstDef)
    {
        printf("null pointer when get isp default value!\n");
        return -1;
    }

    memset(pstDef, 0, sizeof(ISP_CMOS_DEFAULT_S));

    pstDef->stComm.u8Rggb           = 0x0;      //1: rggb  
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
        pstBlackLevel->au16BlackLevel[i] = 0xf0;
    }

    return 0;    
}

HI_VOID cmos_set_pixel_detect(HI_BOOL bEnable)
{
    if (bEnable) /* setup for ISP pixel calibration mode */
    {
        //TODO: finish this.
        /* Sensor must be programmed for slow frame rate (5 fps and below)*/
        /* change frame rate to 3 fps by setting 1 frame length = 1125 * (30/3) */
        sensor_write_register(VMAX_ADDR, 0xF2);
        sensor_write_register(VMAX_ADDR + 1, 0x2B);

        /* Analog and Digital gains both must be programmed for their minimum values */
		sensor_write_register(PGC_ADDR, 0x00);
        //sensor_write_register(APGC_ADDR + 1, 0x00);
	    //sensor_write_register(DPGC_ADDR, 0x00);
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


static HI_U32 digital_gain_table[61]=
{
    1024,  1060,  1097,  1136,  1176,  1217,  1260,  1304,  1350,  1397,  1446,   1497,  1550,   1604,  1661,   1719,  
    1780,  1842,  1907,  1974,  2043,  2115,  2189,  2266,  2346,  2428,  2514,   2602,  2693,   2788,  2886,   2987,  
    3092,  3201,  3314,  3430,  3551,  3675,  3805,  3938,  4077,  4220,  4368,   4522,  4681,   4845,  5015,   5192,  
    5374,  5563,  5758,  5961,  6170,  6387,  6611,  6844,  7084,  7333,  7591,   7858,  8134  
};


static HI_U32 analog_gain_table[81] =
{
     1024 , 1060 ,  1097 ,  1136 ,  1176,  1217 , 1260 ,  1304,  1350 ,  1397 ,  1446 ,  1497 , 1550 , 1604 ,  1661 ,  1719 , 
     1780 , 1842 ,  1907 ,  1974 ,  2043,  2115 , 2189 ,  2266,  2346 ,  2428 ,  2514 ,  2602 , 2693 , 2788 ,  2886 ,  2987 , 
     3092 , 3201 ,  3314 ,  3430 ,  3551,  3675 , 3805 ,  3938,  4077 ,  4220 ,  4368 ,  4522 , 4681 , 4845 ,  5015 ,  5192 , 
     5374 , 5563 ,  5758 ,  5961 ,  6170,  6387 , 6611 ,  6844,  7084 ,  7333 ,  7591 ,  7858 , 8134 , 8420 ,  8716 ,  9022 , 
     9339 , 9667 , 10007 , 10359 , 10723, 11099 ,11489 , 11893, 12311 , 12744 , 13192 , 13655 ,14135 ,14632 , 15146 , 15678 , 
    16229     

};


static HI_S32 cmos_get_ae_default(AE_SENSOR_DEFAULT_S *pstAeSnsDft)
{
    if (HI_NULL == pstAeSnsDft)
    {
        printf("null pointer when get ae default value!\n");
        return -1;
    }

    switch(gu8SensorImageMode)
    {
      case SENSOR_720P_30FPS_MODE:
        
         pstAeSnsDft->u32MaxIntTime = 748;
         pstAeSnsDft->u32LinesPer500ms = 750*30/2;
         gu32FullLinesStd = 750;

      break;
      

      case SENSOR_720P_60FPS_MODE:
        
         pstAeSnsDft->u32MaxIntTime = 748;
         pstAeSnsDft->u32LinesPer500ms = 750*60/2;
         gu32FullLinesStd = 750;
        

      break;
      

      case SENSOR_1080P_30FPS_MODE:

         pstAeSnsDft->u32MaxIntTime = 1122;
         pstAeSnsDft->u32LinesPer500ms = 1125*30/2;
         gu32FullLinesStd = 1125;
         
      break;


      default:
      
      break;
        
    }

    
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
    pstAeSnsDft->u32MinIntTime = 2;    
    pstAeSnsDft->u32MaxIntTimeTarget = 65535;
    pstAeSnsDft->u32MinIntTimeTarget = 2;

    pstAeSnsDft->stAgainAccu.enAccuType = AE_ACCURACY_TABLE;
    pstAeSnsDft->stAgainAccu.f32Accuracy = 0.3;    
    pstAeSnsDft->u32MaxAgain = 16229;  /*the max again value is 10bit precison, the same with the table*/
    pstAeSnsDft->u32MinAgain = 1024;
    pstAeSnsDft->u32MaxAgainTarget = 16229;
    pstAeSnsDft->u32MinAgainTarget = 1024;

    pstAeSnsDft->stDgainAccu.enAccuType = AE_ACCURACY_TABLE;
    pstAeSnsDft->stDgainAccu.f32Accuracy = 0.3;    
    pstAeSnsDft->u32MaxDgain = 8134;  /*the max again value is 10bit precison, the same with the table*/ 
    pstAeSnsDft->u32MinDgain = 1024;
    pstAeSnsDft->u32MaxDgainTarget = 8134;
    pstAeSnsDft->u32MinDgainTarget = 1024;

    pstAeSnsDft->u32ISPDgainShift = 8;
    pstAeSnsDft->u32MaxISPDgainTarget = 4 << pstAeSnsDft->u32ISPDgainShift;
    pstAeSnsDft->u32MinISPDgainTarget = 1 << pstAeSnsDft->u32ISPDgainShift;

    return 0;
}

/* the function of sensor set fps */
static HI_VOID cmos_fps_set(HI_U8 u8Fps, AE_SENSOR_DEFAULT_S *pstAeSnsDft)
{
    if(gu8SensorImageMode == 3)
    {
        switch(u8Fps)
        {
            case 30:
                // Change the frame rate via changing the vertical blanking
                gu32FullLinesStd = 1125;
    			pstAeSnsDft->u32MaxIntTime = 1122;
                pstAeSnsDft->u32LinesPer500ms = 1125 * 30 / 2;
    			sensor_write_register(VMAX_ADDR, 0x65);
    			sensor_write_register(VMAX_ADDR+1, 0x04);
            break;
            
            case 25:
                // Change the frame rate via changing the vertical blanking
                gu32FullLinesStd = 1350;
                pstAeSnsDft->u32MaxIntTime = 1347;
                pstAeSnsDft->u32LinesPer500ms = 1350 * 25 / 2;
    			sensor_write_register(VMAX_ADDR, 0x46);
    			sensor_write_register(VMAX_ADDR+1, 0x05);
            break;
            
            default:
            break;
        }
    }
    else if(gu8SensorImageMode == 2)
    {
        switch(u8Fps)
        {
            case 60:
                // Change the frame rate via changing the vertical blanking
                gu32FullLinesStd = 750;
    			pstAeSnsDft->u32MaxIntTime = 748;
                pstAeSnsDft->u32LinesPer500ms = 750 * 60 / 2;
    			sensor_write_register(VMAX_ADDR, 0xEE);
    			sensor_write_register(VMAX_ADDR+1, 0x02);
            break;
            
            case 50:
                // Change the frame rate via changing the vertical blanking
                gu32FullLinesStd = 900;
                pstAeSnsDft->u32MaxIntTime = 897;
                pstAeSnsDft->u32LinesPer500ms = 900 * 50 / 2;
    			sensor_write_register(VMAX_ADDR, 0x84);
    			sensor_write_register(VMAX_ADDR+1, 0x03);
            break;
            
            default:
            break;
        }

    }
    else if(gu8SensorImageMode == 1)
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
                pstAeSnsDft->u32MaxIntTime = 897;
                pstAeSnsDft->u32LinesPer500ms = 900 * 25 / 2;
                sensor_write_register(VMAX_ADDR, 0x84);
                sensor_write_register(VMAX_ADDR+1, 0x03);
            break;
            
            default:
            break;
       }


    }
    else
    {

    }

    pstAeSnsDft->u32FullLinesStd = gu32FullLinesStd;
    gu32FullLines = gu32FullLinesStd;
        
    return;
}

static HI_VOID cmos_slow_framerate_set(HI_U16 u16FullLines,
    AE_SENSOR_DEFAULT_S *pstAeSnsDft)
{
    gu32FullLines = u16FullLines;

	sensor_write_register(VMAX_ADDR, (gu32FullLines & 0x00ff));
	sensor_write_register(VMAX_ADDR+1, ((gu32FullLines & 0xff00) >> 8));
    
    pstAeSnsDft->u32MaxIntTime = gu32FullLines - 3;
    
    return;
}


static HI_VOID cmos_init_regs_info(HI_VOID)
{
#if CMOS_IMX122_ISP_WRITE_SENSOR_ENABLE
    HI_S32 i;

    if (HI_FALSE == gsbRegInit)
    {
        g_stSnsRegsInfo.enSnsType = ISP_SNS_SSP_TYPE;
        g_stSnsRegsInfo.u32RegNum = 3;
        for (i=0; i<3; i++)
        {
            g_stSnsRegsInfo.astSspData[i].u32DevAddr = 0x02;
            g_stSnsRegsInfo.astSspData[i].u32DevAddrByteNum = 1;
            g_stSnsRegsInfo.astSspData[i].u32RegAddrByteNum = 1;
            g_stSnsRegsInfo.astSspData[i].u32DataByteNum = 1;
        }
        g_stSnsRegsInfo.astSspData[0].bDelayCfg = HI_FALSE;
        g_stSnsRegsInfo.astSspData[0].u32RegAddr = 0x08;
        g_stSnsRegsInfo.astSspData[1].bDelayCfg = HI_FALSE;
        g_stSnsRegsInfo.astSspData[1].u32RegAddr = 0x09;

        g_stSnsRegsInfo.astSspData[2].bDelayCfg = HI_FALSE;
        g_stSnsRegsInfo.astSspData[2].u32RegAddr = 0x1E;
        
        g_stSnsRegsInfo.bDelayCfgIspDgain = HI_FALSE;

        gsbRegInit = HI_TRUE;
    }
#endif
    return;
}



/* while isp notify ae to update sensor regs, ae call these funcs. */
static HI_VOID cmos_inttime_update(HI_U32 u32IntTime)
{
    HI_U32 u32Value = gu32FullLines - u32IntTime;
    
#if CMOS_IMX122_ISP_WRITE_SENSOR_ENABLE
    cmos_init_regs_info();

    g_stSnsRegsInfo.astSspData[0].u32Data = (u32Value & 0xFF);
    g_stSnsRegsInfo.astSspData[1].u32Data = (u32Value & 0xFF00) >> 8;

#else
    sensor_write_register(EXPOSURE_ADDR, u32Value & 0xFF);
    sensor_write_register(EXPOSURE_ADDR + 1, (u32Value & 0xFF00) >> 8);
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

    if (u32InTimes >= analog_gain_table[80])
    {
         pstAeSnsGainInfo->u32SnsTimes = analog_gain_table[80];
         pstAeSnsGainInfo->u32GainDb = 80;
         return ;
    }
    
    for(i = 1; i < 81; i++)
    {
        if(u32InTimes < analog_gain_table[i])
        {
            pstAeSnsGainInfo->u32SnsTimes = analog_gain_table[i - 1];
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
    if (u32InTimes >= digital_gain_table[60])
    {
         pstAeSnsGainInfo->u32SnsTimes = digital_gain_table[60];
         pstAeSnsGainInfo->u32GainDb = 60;
         return;
    }
    
    for(i = 1; i < 61; i++)
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
    HI_U32 u32Tmp = u32Again + u32Dgain;
    
    u32Tmp = u32Tmp > 0x8C ? 0x8C : u32Tmp;
    
#if CMOS_IMX122_ISP_WRITE_SENSOR_ENABLE 
    cmos_init_regs_info();
    g_stSnsRegsInfo.astSspData[2].u32Data = u32Tmp;
    HI_MPI_ISP_SnsRegsCfg(&g_stSnsRegsInfo);
#else
    sensor_write_register(PGC_ADDR, u32Tmp);
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

    pstAwbSnsDft->au16GainOffset[0] = 0x1c5;
    pstAwbSnsDft->au16GainOffset[1] = 0x100;
    pstAwbSnsDft->au16GainOffset[2] = 0x100;
    pstAwbSnsDft->au16GainOffset[3] = 0x1ec;

    pstAwbSnsDft->as32WbPara[0] = 22;
    pstAwbSnsDft->as32WbPara[1] = 141;
    pstAwbSnsDft->as32WbPara[2] = -84;
    pstAwbSnsDft->as32WbPara[3] = 186260;
    pstAwbSnsDft->as32WbPara[4] = 128;
    pstAwbSnsDft->as32WbPara[5] = -134565;

    memcpy(&pstAwbSnsDft->stCcm, &g_stAwbCcm, sizeof(AWB_CCM_S));
    memcpy(&pstAwbSnsDft->stAgcTbl, &g_stAwbAgcTable, sizeof(AWB_AGC_TABLE_S));
    
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

static HI_U8  isp_image_mode_get(ISP_CMOS_SENSOR_IMAGE_MODE *pstSensorImageMode)
{
    if(HI_NULL == pstSensorImageMode)
    {
        printf("null pointer when set image mode111\n");
        return -1;
    }

    if((pstSensorImageMode->u16Width == 1280)&&(pstSensorImageMode->u16Height == 720))
    {
      if(pstSensorImageMode->u16Fps== 30)
      {
        
        gu8SensorImageMode = SENSOR_720P_30FPS_MODE;
      }
      else if(pstSensorImageMode->u16Fps== 60)
      {
         
          gu8SensorImageMode = SENSOR_720P_60FPS_MODE;
      }
      else
      {
         return -1;
      }
    }
    else if((pstSensorImageMode->u16Width == 1920)&&(pstSensorImageMode->u16Height == 1080))
    {
        if(pstSensorImageMode->u16Fps == 30)
        {
          gu8SensorImageMode = SENSOR_1080P_30FPS_MODE;
        }
        else
        {
           return -1;
        }
    }

    return 0;

}

static HI_S32 cmos_set_image_mode(ISP_CMOS_SENSOR_IMAGE_MODE *pstSensorImageMode)
{
   HI_S32 s32Ret;
     
    if (HI_NULL == pstSensorImageMode )
    {
        printf("null pointer when set image mode\n");
        return -1;
    }
    
    s32Ret = isp_image_mode_get(pstSensorImageMode);
    if(s32Ret == -1 )
    {
     return -1;
    }
    
    switch(gu8SensorImageMode)
    {
      case SENSOR_720P_30FPS_MODE:
        
          sensor_init_720p_30fps();

      break;
      
      case SENSOR_720P_60FPS_MODE:
        
          sensor_init_720p_60fps();
          
      break;
      
      case SENSOR_1080P_30FPS_MODE:

           sensor_init();
          
      break;

      default:
      
      break;
        
    }

    return 0;
    
    
}

HI_VOID sensor_global_init()
{
     gu32FullLinesStd = 1125;
     gu32FullLines = 1125;
     gu8SensorMode = 0;

     gu8SensorImageMode = 3;

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
    pstSensorExpFunc->pfn_cmos_set_image_mode = cmos_set_image_mode;

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
    s32Ret = HI_MPI_ISP_SensorRegCallBack(IMX122_ID, &stIspRegister);
    if (s32Ret)
    {
        printf("sensor register callback function failed!\n");
        return s32Ret;
    }
    
    stLib.s32Id = 0;
    strcpy(stLib.acLibName, HI_AE_LIB_NAME);
    cmos_init_ae_exp_function(&stAeRegister.stSnsExp);
    s32Ret = HI_MPI_AE_SensorRegCallBack(&stLib, IMX122_ID, &stAeRegister);
    if (s32Ret)
    {
        printf("sensor register callback function to ae lib failed!\n");
        return s32Ret;
    }

    stLib.s32Id = 0;
    strcpy(stLib.acLibName, HI_AWB_LIB_NAME);
    cmos_init_awb_exp_function(&stAwbRegister.stSnsExp);
    s32Ret = HI_MPI_AWB_SensorRegCallBack(&stLib, IMX122_ID, &stAwbRegister);
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

    s32Ret = HI_MPI_ISP_SensorUnRegCallBack(IMX122_ID);
    if (s32Ret)
    {
        printf("sensor unregister callback function failed!\n");
        return s32Ret;
    }
    
    stLib.s32Id = 0;
    strcpy(stLib.acLibName, HI_AE_LIB_NAME);
    s32Ret = HI_MPI_AE_SensorUnRegCallBack(&stLib, IMX122_ID);
    if (s32Ret)
    {
        printf("sensor unregister callback function to ae lib failed!\n");
        return s32Ret;
    }

    stLib.s32Id = 0;
    strcpy(stLib.acLibName, HI_AWB_LIB_NAME);
    s32Ret = HI_MPI_AWB_SensorUnRegCallBack(&stLib, IMX122_ID);
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
