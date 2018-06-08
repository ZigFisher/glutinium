#if !defined(__PANA_MN34031_H_)
#define __PANA_MN34031_H_

#include <stdio.h>
#include <unistd.h>
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

#define PANA34031_ID 34031

/****************************************************************************
 * local variables                                                            *
 ****************************************************************************/

HI_U32 gu32FullLinesStd = 718;
HI_U8 gu8SensorMode = 0;

static AWB_CCM_S g_stAwbCcm =
{
    5048,
	{	0x1f5, 0x80b1, 0x8044,
		0x803f, 0x1d2, 0x8094,
		0x1e, 0x80f8, 0x1db
	},
    3200,
	{	0x01d5, 0x806d, 0x802e,
		0x8069, 0x1d2, 0x8069,
		0x25, 0x8115, 0x01ef
	},
    2480,
	{	0x1f1, 0x80b8, 0x8039,
		0x8040, 0x1cf, 0x808f,
		0x1e, 0x80fc, 0x1dd
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
    {0x50,0x45,0x40,0x38,0x34,0x40,0x30,0x5c},
        
    /* sharpen_alt_ud */
    {0x3b,0x38,0x34,0x30,0x2b,0x40,0x30,0x5f},
        
    /* snr_thresh */
    {0x23,0x2c,0x34,0x3e,0x46,0x60,0x70,0x8f},
        
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
    {0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x3,0x9,0xd,0x10,0x12,0x14,0x16,0x17,0x18,0x1a,0x1b,
	0x1c,0x1c,0x1d,0x1e,0x1f,0x1f,0x20,0x21,0x21,0x22,0x22,0x23,0x23,0x24,0x24,0x25,0x25,
	0x25,0x26,0x26,0x27,0x27,0x27,0x28,0x28,0x28,0x29,0x29,0x29,0x29,0x2a,0x2a,0x2a,0x2a,
	0x2b,0x2b,0x2b,0x2b,0x2c,0x2c,0x2c,0x2c,0x2d,0x2d,0x2d,0x2d,0x2d,0x2e,0x2e,0x2e,0x2e,
	0x2e,0x2f,0x2f,0x2f,0x2f,0x2f,0x2f,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x31,0x31,0x31,
	0x31,0x31,0x31,0x32,0x32,0x32,0x32,0x32,0x32,0x32,0x32,0x33,0x33,0x33,0x33,0x33,0x33,
	0x33,0x33,0x34,0x34,0x34,0x34,0x34,0x34,0x34,0x34,0x34,0x35,0x35,0x35,0x35,0x35,0x35,
	0x35,0x35,0x35,0x35,0x36,0x36,0x36},

    /* demosaic_weight_lut */
    {0x0,0x3,0x9,0xd,0x10,0x12,0x14,0x16,0x17,0x18,0x1a,0x1b,
	0x1c,0x1c,0x1d,0x1e,0x1f,0x1f,0x20,0x21,0x21,0x22,0x22,0x23,0x23,0x24,0x24,0x25,0x25,
	0x25,0x26,0x26,0x27,0x27,0x27,0x28,0x28,0x28,0x29,0x29,0x29,0x29,0x2a,0x2a,0x2a,0x2a,
	0x2b,0x2b,0x2b,0x2b,0x2c,0x2c,0x2c,0x2c,0x2d,0x2d,0x2d,0x2d,0x2d,0x2e,0x2e,0x2e,0x2e,
	0x2e,0x2f,0x2f,0x2f,0x2f,0x2f,0x2f,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x31,0x31,0x31,
	0x31,0x31,0x31,0x32,0x32,0x32,0x32,0x32,0x32,0x32,0x32,0x33,0x33,0x33,0x33,0x33,0x33,
	0x33,0x33,0x34,0x34,0x34,0x34,0x34,0x34,0x34,0x34,0x34,0x35,0x35,0x35,0x35,0x35,0x35,
	0x35,0x35,0x35,0x35,0x36,0x36,0x36,0x36,0x36,0x36,0x36,0x36,0x36,0x36}
};

static ISP_CMOS_DEMOSAIC_S g_stIspDemosaic =
{
    /* bvalid */
    1,
    
    /*vh_slope*/
    0xd6,

    /*aa_slope*/
    0xbf,

    /*va_slope*/
    200,

    /*uu_slope*/
    190,

    /*sat_slope*/
    0x5d,

    /*ac_slope*/
    0xcf,

    /*vh_thresh*/
    0xa2,

    /*aa_thresh*/
    0xcf,

    /*va_thresh*/
    0x64,

    /*uu_thresh*/
    0x64,

    /*sat_thresh*/
    0x171,

    /*ac_thresh*/
    0x1b3
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

    pstDef->stDenoise.u8SinterThresh= 0x8;
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
        pstBlackLevel->au16BlackLevel[i] = 256;
    }

    return 0;    
}

HI_VOID cmos_set_pixel_detect(HI_BOOL bEnable)
{
    if (bEnable) /* setup for ISP pixel calibration mode */
    {
        //set the gain to 0
		sensor_write_register(0x0020,0x0080);
		sensor_write_register(0x0021,0x0080);
		sensor_write_register(0x00A1,0x0400);
		sensor_write_register(0x00A2,0x0002);
		sensor_write_register(0x00A5,0x0005);
    }
    else /* setup for ISP 'normal mode' */
    {
        sensor_write_register(0x00A5,0x0000);
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

    gu32FullLinesStd = 1065;
    
    pstAeSnsDft->au8HistThresh[0] = 0x10;
    pstAeSnsDft->au8HistThresh[1] = 0x40;
    pstAeSnsDft->au8HistThresh[2] = 0xc0;
    pstAeSnsDft->au8HistThresh[3] = 0xf0;
    
    pstAeSnsDft->u8AeCompensation = 0x40;
    
    pstAeSnsDft->u32LinesPer500ms = 1065*30/2;
    pstAeSnsDft->u32FullLinesStd = gu32FullLinesStd;
    pstAeSnsDft->u32FlickerFreq = 0;//60*256;//50*256;

    pstAeSnsDft->stIntTimeAccu.enAccuType = AE_ACCURACY_LINEAR;
    pstAeSnsDft->stIntTimeAccu.f32Accuracy = 1;
    pstAeSnsDft->u32MaxIntTime = 718;
    pstAeSnsDft->u32MinIntTime = 2;    
    pstAeSnsDft->u32MaxIntTimeTarget = 65535;
    pstAeSnsDft->u32MinIntTimeTarget = 2;

    pstAeSnsDft->stAgainAccu.enAccuType = AE_ACCURACY_DB;
    pstAeSnsDft->stAgainAccu.f32Accuracy = 6;    
    pstAeSnsDft->u32MaxAgain = 3;  /* 18db / 6db = 3 */
    pstAeSnsDft->u32MinAgain = 0;
    pstAeSnsDft->u32MaxAgainTarget = 3;
    pstAeSnsDft->u32MinAgainTarget = 0;

    pstAeSnsDft->stDgainAccu.enAccuType = AE_ACCURACY_DB;
    pstAeSnsDft->stDgainAccu.f32Accuracy = 0.19;    
    pstAeSnsDft->u32MaxDgain = 128;
    pstAeSnsDft->u32MinDgain = 0;
    pstAeSnsDft->u32MaxDgainTarget = 128;
    pstAeSnsDft->u32MinDgainTarget = 0;

    pstAeSnsDft->u32ISPDgainShift = 8;
    pstAeSnsDft->u32MaxISPDgainTarget = 1 << pstAeSnsDft->u32ISPDgainShift;
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
	usleep(40000);
    switch(u8Fps)
    {
        case 30:
        pstAeSnsDft->u32MaxIntTime = 1062;
        pstAeSnsDft->u32LinesPer500ms = 1062 * 30 / 2;
        gu32FullLinesStd = 1065;
	    sensor_write_register(0x1A0, gu32FullLinesStd & 0xffff);
	    sensor_write_register(0x2A0, gu32FullLinesStd & 0xffff);
        break;
        
        case 25:
        pstAeSnsDft->u32MaxIntTime = 1275;
        pstAeSnsDft->u32LinesPer500ms = 1278 * 25 / 2;
        gu32FullLinesStd = 1278;
	    sensor_write_register(0x1A0, gu32FullLinesStd & 0xffff);
	    sensor_write_register(0x2A0, gu32FullLinesStd & 0xffff);
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
    pstAeSnsDft->u32MaxIntTime = u16FullLines - 2;

	sensor_write_register(0x1A0, u16FullLines & 0xffff);
	sensor_write_register(0x2A0, u16FullLines & 0xffff);
    
    return;
}

/* while isp notify ae to update sensor regs, ae call these funcs. */
static HI_VOID cmos_inttime_update(HI_U32 u32IntTime)
{
    HI_U32 u32Tmp = u32IntTime;
	HI_U32 u32IntFrames = u32Tmp / gu32FullLinesStd;

	u32Tmp = u32Tmp - u32IntFrames * gu32FullLinesStd;
	u32Tmp = gu32FullLinesStd - u32Tmp;

	sensor_write_register(0x00A1, (u32Tmp & 0xffff));
	sensor_write_register(0x00A3, u32IntFrames);
    
    return;
}

static HI_VOID cmos_gains_update(HI_U32 u32Again, HI_U32 u32Dgain)
{
    switch (u32Again)
    {
        case 0:
            //r_colgsw = 0dB, r_a_gain = 0dB
            sensor_write_register(0x20, 0x0080);
            break;
        case 1:
            //r_colgsw = 6dB, r_a_gain = 0dB
            sensor_write_register(0x20, 0x2080);
            break;
        case 2:
            //r_colgsw = 12dB, r_a_gain = 0dB
            sensor_write_register(0x20, 0x6080);
            break;
        case 3:
            //r_colgsw = 12dB, r_a_gain = 6dB
            sensor_write_register(0x20, 0xE080);
            break;
        default:
            break;
    }
    
    sensor_write_register(0x21, u32Dgain * 2);

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

    pstAwbSnsDft->au16GainOffset[0] = 0x1CF;
    pstAwbSnsDft->au16GainOffset[1] = 0x100;
    pstAwbSnsDft->au16GainOffset[2] = 0x100;
    pstAwbSnsDft->au16GainOffset[3] = 0x1BD;    

    pstAwbSnsDft->as32WbPara[0] = 67;
    pstAwbSnsDft->as32WbPara[1] = 91;
    pstAwbSnsDft->as32WbPara[2] = -98;
    pstAwbSnsDft->as32WbPara[3] = 167236;
    pstAwbSnsDft->as32WbPara[4] = 128;
    pstAwbSnsDft->as32WbPara[5] = -119483;

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
    s32Ret = HI_MPI_ISP_SensorRegCallBack(PANA34031_ID, &stIspRegister);
    if (s32Ret)
    {
        printf("sensor register callback function failed!\n");
        return s32Ret;
    }
    
    stLib.s32Id = 0;
    strcpy(stLib.acLibName, HI_AE_LIB_NAME);
    cmos_init_ae_exp_function(&stAeRegister.stSnsExp);
    s32Ret = HI_MPI_AE_SensorRegCallBack(&stLib, PANA34031_ID, &stAeRegister);
    if (s32Ret)
    {
        printf("sensor register callback function to ae lib failed!\n");
        return s32Ret;
    }

    stLib.s32Id = 0;
    strcpy(stLib.acLibName, HI_AWB_LIB_NAME);
    cmos_init_awb_exp_function(&stAwbRegister.stSnsExp);
    s32Ret = HI_MPI_AWB_SensorRegCallBack(&stLib, PANA34031_ID, &stAwbRegister);
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

    s32Ret = HI_MPI_ISP_SensorUnRegCallBack(PANA34031_ID);
    if (s32Ret)
    {
        printf("sensor unregister callback function failed!\n");
        return s32Ret;
    }
    
    stLib.s32Id = 0;
    strcpy(stLib.acLibName, HI_AE_LIB_NAME);
    s32Ret = HI_MPI_AE_SensorUnRegCallBack(&stLib, PANA34031_ID);
    if (s32Ret)
    {
        printf("sensor unregister callback function to ae lib failed!\n");
        return s32Ret;
    }

    stLib.s32Id = 0;
    strcpy(stLib.acLibName, HI_AWB_LIB_NAME);
    s32Ret = HI_MPI_AWB_SensorUnRegCallBack(&stLib, PANA34031_ID);
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
