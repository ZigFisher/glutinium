#if !defined(__IMX138_CMOS_H_)
#define __IMX138_CMOS_H_

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

#define CMOS_IMX138_ISP_WRITE_SENSOR_ENABLE (1)


#if CMOS_IMX138_ISP_WRITE_SENSOR_ENABLE
ISP_SNS_REGS_INFO_S g_stSnsRegsInfo = {0};
static HI_BOOL gsbRegInit = HI_FALSE;
#endif

#define EXPOSURE_ADDR (0x220) //2:chip_id, 0C: reg addr.

#define PGC_ADDR (0x214)
#define VMAX_ADDR (0x218)

#define IMX138_ID 138

/****************************************************************************
 * local variables                                                            *
 ****************************************************************************/

HI_U32 gu32FullLinesStd = 750;
HI_U32 gu32FullLines = 750;
HI_U8 gu8SensorMode = 0;

static AWB_CCM_S g_stAwbCcm =
{
    5048,
    {
        0x01f2, 0x80bd, 0x8035,
        0x8042, 0x01a3, 0x8061,
        0x0026, 0x80e0, 0x1b9,
    },
    3200,
    {
        0x01dd, 0x807b, 0x8062,
        0x807b, 0x01d2, 0x8057,
        0x0036, 0x8110, 0x01d9,
    },
    2480,
    {
        0x01b1, 0x8066, 0x804b,
        0x8074, 0x01b2, 0x803e,
        0x001e, 0x81d9, 0x02ba,
    }
};

static AWB_AGC_TABLE_S g_stAwbAgcTable =
{
    /* bvalid */
    1,

    /* saturation */   
    {0x80,0x80,0x80,0x75,0x60,0x55,0x50,0x48}

};

static ISP_CMOS_AGC_TABLE_S g_stIspAgcTable =
{
    /* bvalid */
    1,

    /* sharpen_alt_d */
   {48,48,40,35,30,30,20,15}, 
    
    /* sharpen_alt_ud */
   {40,40,35,30,25,25,15,10},    
    
    /* snr_thresh    */
   {10,16,24,34,39,44,49,54},  
    
    /*  demosaic_lum_thresh   */
   {0x60,0x60,0x80,0x80,0x80,0x80,0x80,0x80}, 
    
    /* demosaic_np_offset   */
   {0,0xa,16,24,32,40,48,56},  
    
    /* ge_strength    */
   {0x82,0x55,0x55,0x55,0x55,0x55,0x37,0x37}, 
    

};

static ISP_CMOS_NOISE_TABLE_S g_stIspNoiseTable =
{
    /* bvalid */
    1,
    
    //nosie_profile_weight_lut
   {
     0,  0,  0,  0,  0,  0,  0,  0,  5, 10, 15, 17, 20, 22, 25, 27, 
    27, 30, 30, 32, 32, 35, 35, 35, 37, 37, 37, 40, 40, 40, 40, 42,
    42, 42, 42, 42, 45, 45, 45, 45, 45, 45, 47, 47, 47, 47, 47, 47,
    50, 50, 50, 50, 50, 50, 50, 50, 52, 52, 52, 52, 52, 52, 52, 52,
    52, 52, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 57, 57, 57,
    57, 57, 57, 57, 57, 57, 57, 57, 57, 57, 57, 60, 60, 60, 60, 60,
    60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 62, 62, 62, 62, 62, 62,
    62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 65, 65, 65,
   },

    //demosaic_weight_lut
   {
      0,  5, 10, 15, 17, 20, 22, 25, 27, 27, 30, 30, 32, 32, 35, 35,
     35, 37, 37, 37, 40, 40, 40, 40, 42, 42, 42, 42, 42, 45, 45, 45,
     45, 45, 45, 47, 47, 47, 47, 47, 47, 50, 50, 50, 50, 50, 50, 50,
     50, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 55, 55, 55, 55, 55,
     55, 55, 55, 55, 55, 55, 57, 57, 57, 57, 57, 57, 57, 57, 57, 57,
     57, 57, 57, 57, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60,
     60, 60, 60, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62,
     62, 62, 62, 62, 62, 62, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65
   }

};

static ISP_CMOS_DEMOSAIC_S g_stIspDemosaic =
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
        pstBlackLevel->au16BlackLevel[i] = 0xF0;
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

    gu32FullLinesStd = 752;
    
    pstAeSnsDft->au8HistThresh[0] = 0xd;
    pstAeSnsDft->au8HistThresh[1] = 0x28;
    pstAeSnsDft->au8HistThresh[2] = 0x60;
    pstAeSnsDft->au8HistThresh[3] = 0x80;
    
    pstAeSnsDft->u8AeCompensation = 0x40;
    
    pstAeSnsDft->u32LinesPer500ms = 752*30/2;
    pstAeSnsDft->u32FullLinesStd = gu32FullLinesStd;
    pstAeSnsDft->u32FlickerFreq = 0;//60*256;//50*256;

    pstAeSnsDft->stIntTimeAccu.enAccuType = AE_ACCURACY_LINEAR;
    pstAeSnsDft->stIntTimeAccu.f32Accuracy = 1;
    pstAeSnsDft->u32MaxIntTime = 749;
    pstAeSnsDft->u32MinIntTime = 3;    
    pstAeSnsDft->u32MaxIntTimeTarget = 65535;
    pstAeSnsDft->u32MinIntTimeTarget = 3;

    pstAeSnsDft->stAgainAccu.enAccuType = AE_ACCURACY_TABLE;
    pstAeSnsDft->stAgainAccu.f32Accuracy = 0.3;    
    pstAeSnsDft->u32MaxAgain = 16229;  /**/
    pstAeSnsDft->u32MinAgain = 1024;
    pstAeSnsDft->u32MaxAgainTarget = 16229;
    pstAeSnsDft->u32MinAgainTarget = 1024;

    pstAeSnsDft->stDgainAccu.enAccuType = AE_ACCURACY_TABLE;
    pstAeSnsDft->stDgainAccu.f32Accuracy = 0.3;    
    pstAeSnsDft->u32MaxDgain = 16229;  /*  */
    pstAeSnsDft->u32MinDgain = 1024;
    pstAeSnsDft->u32MaxDgainTarget = 16229;
    pstAeSnsDft->u32MinDgainTarget = 1024;

    pstAeSnsDft->u32ISPDgainShift = 8;
    pstAeSnsDft->u32MaxISPDgainTarget = 4 << pstAeSnsDft->u32ISPDgainShift;
    pstAeSnsDft->u32MinISPDgainTarget = 1 << pstAeSnsDft->u32ISPDgainShift;

    return 0;
}

/* the function of sensor set fps */
static HI_VOID cmos_fps_set(HI_U8 u8Fps, AE_SENSOR_DEFAULT_S *pstAeSnsDft)
{
    switch(u8Fps)
    {
        case 30:
            // Change the frame rate via changing the vertical blanking
            gu32FullLinesStd = 752;
			pstAeSnsDft->u32MaxIntTime = 749;
            pstAeSnsDft->u32LinesPer500ms = 752 * 30 / 2;
			sensor_write_register(VMAX_ADDR, 0xF0);
			sensor_write_register(VMAX_ADDR+1, 0x02);
        break;
        
        case 25:
            // Change the frame rate via changing the vertical blanking
            gu32FullLinesStd = 902;
            pstAeSnsDft->u32MaxIntTime = 899;
            pstAeSnsDft->u32LinesPer500ms = 902 * 25 / 2;
			sensor_write_register(VMAX_ADDR, 0x86);
			sensor_write_register(VMAX_ADDR+1, 0x03);
        break;
        
        default:
        break;
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
    gu32FullLines = u16FullLines;

	sensor_write_register(VMAX_ADDR, (gu32FullLines & 0x00ff));
	sensor_write_register(VMAX_ADDR+1, ((gu32FullLines & 0xff00) >> 8));
    
    pstAeSnsDft->u32MaxIntTime = gu32FullLines - 3;
    
    return;
}

static HI_VOID cmos_init_regs_info(HI_VOID)
{
#if CMOS_IMX138_ISP_WRITE_SENSOR_ENABLE
    HI_S32 i;

    if (HI_FALSE == gsbRegInit)
    {
        g_stSnsRegsInfo.enSnsType = ISP_SNS_SSP_TYPE;
        for (i=0; i<3; i++)
        {
            g_stSnsRegsInfo.astSspData[i].u32DevAddr = 0x02;
            g_stSnsRegsInfo.astSspData[i].u32DevAddrByteNum = 1;
            g_stSnsRegsInfo.astSspData[i].u32RegAddrByteNum = 1;
            g_stSnsRegsInfo.astSspData[i].u32DataByteNum = 1;
        }
        g_stSnsRegsInfo.u32RegNum = 3;
        g_stSnsRegsInfo.astSspData[0].bDelayCfg = HI_FALSE;
        g_stSnsRegsInfo.astSspData[0].u32RegAddr = 0x20;
        g_stSnsRegsInfo.astSspData[1].bDelayCfg = HI_FALSE;
        g_stSnsRegsInfo.astSspData[1].u32RegAddr = 0x21;
        g_stSnsRegsInfo.astSspData[2].bDelayCfg = HI_TRUE;
        g_stSnsRegsInfo.astSspData[2].u32RegAddr = 0x14;
    
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
    HI_U32 u32Value = gu32FullLines - u32IntTime - 1;

#if CMOS_IMX138_ISP_WRITE_SENSOR_ENABLE
    cmos_init_regs_info();
    g_stSnsRegsInfo.astSspData[0].u32Data = (u32Value & 0xFF);
    g_stSnsRegsInfo.astSspData[1].u32Data = ((u32Value & 0xFF00) >> 8);
   
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
    HI_U32 u32Tmp = u32Again + u32Dgain;
    u32Tmp = u32Tmp > 0xA0 ? 0xA0 : u32Tmp;
#if CMOS_IMX138_ISP_WRITE_SENSOR_ENABLE   
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

    pstAwbSnsDft->u16WbRefTemp = 5048;

    pstAwbSnsDft->au16GainOffset[0] = 0x1ff;
    pstAwbSnsDft->au16GainOffset[1] = 0x100;
    pstAwbSnsDft->au16GainOffset[2] = 0x100;
    pstAwbSnsDft->au16GainOffset[3] = 0x1e4;

    pstAwbSnsDft->as32WbPara[0] = 40;
    pstAwbSnsDft->as32WbPara[1] = 106;
    pstAwbSnsDft->as32WbPara[2] = -110;
    pstAwbSnsDft->as32WbPara[3] = 184787;
    pstAwbSnsDft->as32WbPara[4] = 128;
    pstAwbSnsDft->as32WbPara[5] = -134867;

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
    s32Ret = HI_MPI_ISP_SensorRegCallBack(IMX138_ID, &stIspRegister);
    if (s32Ret)
    {
        printf("sensor register callback function failed!\n");
        return s32Ret;
    }
    
    stLib.s32Id = 0;
    strcpy(stLib.acLibName, HI_AE_LIB_NAME);
    cmos_init_ae_exp_function(&stAeRegister.stSnsExp);
    s32Ret = HI_MPI_AE_SensorRegCallBack(&stLib, IMX138_ID, &stAeRegister);
    if (s32Ret)
    {
        printf("sensor register callback function to ae lib failed!\n");
        return s32Ret;
    }

    stLib.s32Id = 0;
    strcpy(stLib.acLibName, HI_AWB_LIB_NAME);
    cmos_init_awb_exp_function(&stAwbRegister.stSnsExp);
    s32Ret = HI_MPI_AWB_SensorRegCallBack(&stLib, IMX138_ID, &stAwbRegister);
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

    s32Ret = HI_MPI_ISP_SensorUnRegCallBack(IMX138_ID);
    if (s32Ret)
    {
        printf("sensor unregister callback function failed!\n");
        return s32Ret;
    }
    
    stLib.s32Id = 0;
    strcpy(stLib.acLibName, HI_AE_LIB_NAME);
    s32Ret = HI_MPI_AE_SensorUnRegCallBack(&stLib, IMX138_ID);
    if (s32Ret)
    {
        printf("sensor unregister callback function to ae lib failed!\n");
        return s32Ret;
    }

    stLib.s32Id = 0;
    strcpy(stLib.acLibName, HI_AWB_LIB_NAME);
    s32Ret = HI_MPI_AWB_SensorUnRegCallBack(&stLib, IMX138_ID);
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
