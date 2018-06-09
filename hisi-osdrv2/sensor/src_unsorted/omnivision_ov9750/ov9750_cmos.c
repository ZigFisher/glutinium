#if !defined(__OV9750_CMOS_H_)
#define __OV9750_CMOS_H_

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "hi_comm_sns.h"
#include "hi_comm_video.h"
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


#define OV9750_ID 9750


/* To change the mode of config. ifndef INIFILE_CONFIG_MODE, quick config mode.*/
/* else, cmos_cfg.ini file config mode*/
#ifdef INIFILE_CONFIG_MODE

extern AE_SENSOR_DEFAULT_S  g_AeDft[];
extern AWB_SENSOR_DEFAULT_S g_AwbDft[];
extern ISP_CMOS_DEFAULT_S   g_IspDft[];
extern HI_S32 Cmos_LoadINIPara(const HI_CHAR *pcName);
#else

#endif

/****************************************************************************
 * local variables                                                            *
 ****************************************************************************/

extern const unsigned int sensor_i2c_addr;
extern unsigned int sensor_addr_byte;
extern unsigned int sensor_data_byte;

#define FULL_LINES_MAX  (0xFFFF)

#define LONG_EXPOSURE_ADDR       (0x3500)
#define MIDDLE_EXPOSURE_ADDR     (0x350a)
#define SHORT_EXPOSURE_ADDR      (0x3510)
#define LONG_AGC_ADDR            (0x3508)
#define MIDDLE_AGC_ADDR          (0x350E)
#define SHORT_AGC_ADDR           (0x3514)
#define DC_GAIN_ADDR             (0x37c7)
#define VMAX_ADDR_H              (0x380E)
#define VMAX_ADDR_L              (0x380F)
#define R_DGC_ADDR               (0x5032)
#define G_DGC_ADDR               (0x5034)
#define B_DGC_ADDR               (0x5036)

#define SENSOR_720P_30FPS_MODE  (1)
#define SENSOR_960P_30FPS_MODE  (2)
#define INCREASE_LINES (4) /* make real fps less than stand fps because NVR require*/
#define VMAX_720P30_LINEAR     (988+INCREASE_LINES) /*for fps*/
#define VMAX_960P30_LINEAR     (988+INCREASE_LINES)
//#define AGAIN_USE_OV_TABLE /* use ov's again table, WDR scene video will flicker */

HI_U8 gu8SensorImageMode = SENSOR_720P_30FPS_MODE;
WDR_MODE_E genSensorMode = WDR_MODE_NONE;

static HI_U32 gu32FullLinesStd = VMAX_720P30_LINEAR;
static HI_U32 gu32FullLines = VMAX_720P30_LINEAR;

static HI_BOOL bInit = HI_FALSE;
HI_BOOL bSensorInit = HI_FALSE;
ISP_SNS_REGS_INFO_S g_stSnsRegsInfo = {0};
ISP_SNS_REGS_INFO_S g_stPreSnsRegsInfo = {0};

#if 1
/* Piris attr */
static ISP_PIRIS_ATTR_S gstPirisAttr=
{
    0,      // bStepFNOTableChange
    1,      // bZeroIsMax
    94,     // u16TotalStep
    62,     // u16StepCount
    /* Step-F number mapping table. Must be from small to large. F1.0 is 1024 and F32.0 is 1 */
    {30,35,40,45,50,56,61,67,73,79,85,92,98,105,112,120,127,135,143,150,158,166,174,183,191,200,208,217,225,234,243,252,261,270,279,289,298,307,316,325,335,344,353,362,372,381,390,399,408,417,426,435,444,453,462,470,478,486,493,500,506,512},
    ISP_IRIS_F_NO_1_4, // enMaxIrisFNOTarget
    ISP_IRIS_F_NO_5_6,  // enMinIrisFNOTarget
    0,                 // bFNOExValid
    512,              // u32MaxIrisFNOTarget  
    30                // u32MinIrisFNOTarget
};
#endif

#define PATHLEN_MAX 256
#define CMOS_CFG_INI "ov9750_cfg.ini"
static char pcName[PATHLEN_MAX] = "configs/ov9750_cfg.ini";

/* AE default parameter and function */
#ifdef INIFILE_CONFIG_MODE

static HI_S32 cmos_get_ae_default(AE_SENSOR_DEFAULT_S *pstAeSnsDft)
{
    if (HI_NULL == pstAeSnsDft)
    {
        printf("null pointer when get ae default value!\n");
        return -1;
    }
    
    pstAeSnsDft->u32LinesPer500ms = gu32FullLinesStd*30/2;
    pstAeSnsDft->u32FullLinesStd = gu32FullLinesStd;
    pstAeSnsDft->u32FlickerFreq = 0;
    pstAeSnsDft->u32FullLinesMax = FULL_LINES_MAX;

    pstAeSnsDft->stIntTimeAccu.enAccuType = AE_ACCURACY_LINEAR;
    pstAeSnsDft->stIntTimeAccu.f32Accuracy = 1;
    pstAeSnsDft->stIntTimeAccu.f32Offset = 0;

    pstAeSnsDft->stAgainAccu.enAccuType = AE_ACCURACY_TABLE;
    pstAeSnsDft->stAgainAccu.f32Accuracy = 0.1;

    pstAeSnsDft->stDgainAccu.enAccuType = AE_ACCURACY_LINEAR;
    pstAeSnsDft->stDgainAccu.f32Accuracy = 0.00390625;
    
    pstAeSnsDft->u32ISPDgainShift = 8;
    pstAeSnsDft->u32MinISPDgainTarget = 1 << pstAeSnsDft->u32ISPDgainShift;
    pstAeSnsDft->u32MaxISPDgainTarget = 32 << pstAeSnsDft->u32ISPDgainShift;
    
    switch(genSensorMode)
    {
        default:
        case WDR_MODE_NONE:   /*linear mode*/
            pstAeSnsDft->au8HistThresh[0] = 0xd;
            pstAeSnsDft->au8HistThresh[1] = 0x28;
            pstAeSnsDft->au8HistThresh[2] = 0x60;
            pstAeSnsDft->au8HistThresh[3] = 0x80;
            
            pstAeSnsDft->u8AeCompensation = g_AeDft[0].u8AeCompensation;
            
            pstAeSnsDft->u32MaxIntTime = gu32FullLinesStd - 4;
            pstAeSnsDft->u32MinIntTime = 2;
            pstAeSnsDft->u32MaxIntTimeTarget = g_AeDft[0].u32MaxIntTimeTarget;
            pstAeSnsDft->u32MinIntTimeTarget = g_AeDft[0].u32MinIntTimeTarget;
            
            pstAeSnsDft->u32MaxAgain = 16383*2; /*2.3x DCgain*/
            pstAeSnsDft->u32MinAgain = 1024;
            pstAeSnsDft->u32MaxAgainTarget = g_AeDft[0].u32MaxAgainTarget;
            pstAeSnsDft->u32MinAgainTarget = g_AeDft[0].u32MinAgainTarget;
            
            pstAeSnsDft->u32MaxDgain = 256;  
            pstAeSnsDft->u32MinDgain = 256;
            pstAeSnsDft->u32MaxDgainTarget = g_AeDft[0].u32MaxDgainTarget;
            pstAeSnsDft->u32MinDgainTarget = g_AeDft[0].u32MinDgainTarget;
          
            pstAeSnsDft->u32ISPDgainShift = g_AeDft[0].u32ISPDgainShift;
            pstAeSnsDft->u32MinISPDgainTarget = g_AeDft[0].u32MinISPDgainTarget;
            pstAeSnsDft->u32MaxISPDgainTarget = g_AeDft[0].u32MaxISPDgainTarget;    
        break;    
    }    
    return 0;
}

#else

static HI_S32 cmos_get_ae_default(AE_SENSOR_DEFAULT_S *pstAeSnsDft)
{
    if (HI_NULL == pstAeSnsDft)
    {
        printf("null pointer when get ae default value!\n");
        return -1;
    }

    pstAeSnsDft->u32LinesPer500ms = gu32FullLinesStd*30/2;
    pstAeSnsDft->u32FullLinesStd = gu32FullLinesStd;
    pstAeSnsDft->u32FlickerFreq = 0;
    pstAeSnsDft->u32FullLinesMax = FULL_LINES_MAX;

    pstAeSnsDft->stIntTimeAccu.enAccuType = AE_ACCURACY_LINEAR;
    pstAeSnsDft->stIntTimeAccu.f32Accuracy = 1;
    pstAeSnsDft->stIntTimeAccu.f32Offset = 0;

    pstAeSnsDft->stAgainAccu.enAccuType = AE_ACCURACY_TABLE;
    pstAeSnsDft->stAgainAccu.f32Accuracy = 0.1;

    pstAeSnsDft->stDgainAccu.enAccuType = AE_ACCURACY_LINEAR;
    pstAeSnsDft->stDgainAccu.f32Accuracy = 0.00390625;
    
    pstAeSnsDft->u32ISPDgainShift = 8;
    pstAeSnsDft->u32MinISPDgainTarget = 1 << pstAeSnsDft->u32ISPDgainShift;
    pstAeSnsDft->u32MaxISPDgainTarget = 2 << pstAeSnsDft->u32ISPDgainShift; 
	
    pstAeSnsDft->u32MaxIntTime = gu32FullLinesStd - 4;
    pstAeSnsDft->u32MinIntTime = 2;
    pstAeSnsDft->u32MaxIntTimeTarget = 65535;
    pstAeSnsDft->u32MinIntTimeTarget = 2;

    pstAeSnsDft->u32MaxAgain = 39680; //39680
    //pstAeSnsDft->u32MaxAgain = 15871; /*15.5x */
    pstAeSnsDft->u32MinAgain = 1024;
    pstAeSnsDft->u32MaxAgainTarget = pstAeSnsDft->u32MaxAgain;
    pstAeSnsDft->u32MinAgainTarget = pstAeSnsDft->u32MinAgain;

    pstAeSnsDft->u32MaxDgain = 4095;  
    pstAeSnsDft->u32MinDgain = 256;
    pstAeSnsDft->u32MaxDgainTarget = pstAeSnsDft->u32MaxDgain;
    pstAeSnsDft->u32MinDgainTarget = pstAeSnsDft->u32MinDgain;
   
    pstAeSnsDft->au8HistThresh[0] = 0xd;
    pstAeSnsDft->au8HistThresh[1] = 0x28;
    pstAeSnsDft->au8HistThresh[2] = 0x60;
    pstAeSnsDft->au8HistThresh[3] = 0x80;
            
    pstAeSnsDft->u8AeCompensation = 0x38;
    
    pstAeSnsDft->u32LinesPer500ms = gu32FullLinesStd*30/2;

    memcpy(&pstAeSnsDft->stPirisAttr, &gstPirisAttr, sizeof(ISP_PIRIS_ATTR_S));
    pstAeSnsDft->enMaxIrisFNO = ISP_IRIS_F_NO_1_4;
    pstAeSnsDft->enMinIrisFNO = ISP_IRIS_F_NO_5_6;

    pstAeSnsDft->bAERouteExValid = HI_FALSE;
    pstAeSnsDft->stAERouteAttr.u32TotalNum = 0;
    pstAeSnsDft->stAERouteAttrEx.u32TotalNum = 0;

    return 0;
}
#endif

/* the function of sensor set fps */
static HI_VOID cmos_fps_set(HI_FLOAT f32Fps, AE_SENSOR_DEFAULT_S *pstAeSnsDft)
{
    if ((f32Fps <= 30) && (f32Fps >= 0.45))
    {
        if(SENSOR_720P_30FPS_MODE == gu8SensorImageMode)
        {
            gu32FullLinesStd = (VMAX_720P30_LINEAR * 30) / f32Fps;
        }
        else if(SENSOR_960P_30FPS_MODE == gu8SensorImageMode)
        {
            gu32FullLinesStd = (VMAX_960P30_LINEAR * 30) / f32Fps;
        }
    }
    else
    {
        printf("Not support Fps: %f\n", f32Fps);
        return;
    }
    
    gu32FullLinesStd = gu32FullLinesStd > FULL_LINES_MAX ? FULL_LINES_MAX : gu32FullLinesStd;
    g_stSnsRegsInfo.astI2cData[6].u32Data = (gu32FullLinesStd & 0xFF00) >> 8;
    g_stSnsRegsInfo.astI2cData[7].u32Data = gu32FullLinesStd & 0xFF;

    pstAeSnsDft->f32Fps = f32Fps;
    pstAeSnsDft->u32LinesPer500ms = gu32FullLinesStd * f32Fps / 2;
    pstAeSnsDft->u32MaxIntTime = gu32FullLinesStd - 4;
    pstAeSnsDft->u32FullLinesStd = gu32FullLinesStd;
    gu32FullLines = gu32FullLinesStd;
    pstAeSnsDft->u32FullLines = gu32FullLines;

    return;
}

static HI_VOID cmos_slow_framerate_set(HI_U32 u32FullLines,
    AE_SENSOR_DEFAULT_S *pstAeSnsDft)
{
    u32FullLines = (u32FullLines > FULL_LINES_MAX) ? FULL_LINES_MAX : u32FullLines;
    gu32FullLines = u32FullLines;
    pstAeSnsDft->u32FullLines = gu32FullLines;

    g_stSnsRegsInfo.astI2cData[6].u32Data = (gu32FullLines & 0xFF00) >> 8;
    g_stSnsRegsInfo.astI2cData[7].u32Data = gu32FullLines & 0xFF;

    pstAeSnsDft->u32MaxIntTime = gu32FullLines - 4;
    
    return;
}

/* while isp notify ae to update sensor regs, ae call these funcs. */
static HI_VOID cmos_inttime_update(HI_U32 u32IntTime)
{

    g_stSnsRegsInfo.astI2cData[0].u32Data = u32IntTime >> 12;
    g_stSnsRegsInfo.astI2cData[1].u32Data = (u32IntTime & 0xFFF) >> 4;
    g_stSnsRegsInfo.astI2cData[2].u32Data = (u32IntTime & 0xF) << 4;

    return;
}

#if 0
static HI_U32 analog_gain_table[64] =
{
    1024, 1088, 1152, 1216, 1280, 1344, 1408, 1472, 1536, 1600, 1664, 1728, 1792, 1856, 1920, 1984, 
    2048, 2176, 2304, 2432, 2560, 2688, 2816, 2944, 3072, 3200, 3328, 3456, 3584, 3712, 3840, 3968, 
    4096, 4352, 4608, 4864, 5120, 5376, 5632, 5888, 6144, 6400, 6656, 6912, 7168, 7424, 7680, 7936, 
    8192, 8704, 9216, 9728, 10240, 10752, 11264, 11776, 12288, 12800, 13312, 13824, 14336, 14848, 15360, 15872, 
};

static HI_U32 analog_gain_plus_dcg_table[64] =
{
    2560, 2720, 2880, 3040, 3200, 3360, 3520, 3680, 3840, 4000, 4160, 4320, 4480, 4640, 4800, 4960, 
    5120, 5440, 5760, 6080, 6400, 6720, 7040, 7360, 7680, 8000, 8320, 8640, 8960, 9280, 9600, 9920, 
    10240, 10880, 11520, 12160, 12800, 13440, 14080, 14720, 15360, 16000, 16640, 17280, 17920, 18560, 19200, 19840, 
    20480, 21760, 23040, 24320, 25600, 26880, 28160, 29440, 30720, 32000, 33280, 34560, 35840, 37120, 38400, 39680, 
};
#endif

#ifdef AGAIN_USE_OV_TABLE
/* use ov's again table, WDR scene video will flicker */
static HI_VOID cmos_again_calc_table(HI_U32 *pu32AgainLin, HI_U32 *pu32AgainDb)
{
    int i;
 
    /* when ov9750 use DCG under flip and mirror on, image will red under low light.*/ 
    HI_U32 u32DCGain = 2560;//(HI_U32)(2.5*1024);//DC-gain:2.3X,usually measure value is 2.53.

    if (*pu32AgainLin <= u32DCGain)
    {
        for (i = 1; i < 64; i++)
        {
            if (*pu32AgainLin < analog_gain_table[i])
            {
                *pu32AgainLin = analog_gain_table[i - 1];
                *pu32AgainDb = (analog_gain_table[i - 1] >> 3);
                break;
            }
        }

        u32DCGain = 0x39; //low gain
    }
    else
    {
        for (i = 1; i < 64; i++)
        {
            if (*pu32AgainLin < analog_gain_plus_dcg_table[i])
            {
                *pu32AgainLin = analog_gain_plus_dcg_table[i - 1];
                *pu32AgainDb = (analog_gain_table[i - 1] >> 3);
                break;
            }
        }
        
        u32DCGain = 0x38; //high gain
    }

    g_stSnsRegsInfo.astI2cData[5].u32Data = u32DCGain; 
    
    return;
}

#else
static HI_VOID cmos_again_calc_table(HI_U32 *pu32AgainLin, HI_U32 *pu32AgainDb)
{
    *pu32AgainLin = *pu32AgainLin & 0xff00;
    *pu32AgainDb = (*pu32AgainLin >> 3);
   
    /* when ov9750 use DCG under flip and mirror on, image will red under low light.*/ 
    HI_U32 u32DCGain = (HI_U32)(2.5*1024);//DC-gain:2.3X,usually measure value is 2.53.
    HI_U32 u32Tmp;
    
    if (*pu32AgainLin > u32DCGain)
    {
        u32Tmp = (((*pu32AgainLin << 13)/u32DCGain)>>3);
        if (u32Tmp&0x2000)
        {
            *pu32AgainLin = *pu32AgainLin & 0xffc0;
        }    
        else if (u32Tmp&0x1000)
        {
            *pu32AgainLin = *pu32AgainLin & 0xffe0;
        }
        else if (u32Tmp&0x800)
        {
            *pu32AgainLin = *pu32AgainLin & 0xfff0;
        }
        else 
        {
            *pu32AgainLin = *pu32AgainLin & 0xfff8;
        }

        *pu32AgainDb = (u32Tmp >> 3);
        u32DCGain = 0x38; //high gain
    }
    else
    {
        if (*pu32AgainLin&0x800)
        {
            *pu32AgainLin = *pu32AgainLin & 0xff0;
        }
        else 
        {
            *pu32AgainLin = *pu32AgainLin & 0x7f8;
        }
        
        *pu32AgainDb = (*pu32AgainLin >> 3);
        u32DCGain = 0x39; //low gain
    }

    g_stSnsRegsInfo.astI2cData[5].u32Data = u32DCGain; 

	return;
}
#endif

static HI_VOID cmos_dgain_calc_table(HI_U32 *pu32DgainLin, HI_U32 *pu32DgainDb)
{
	#if 0
    HI_U32 u32Dgain;

    if (*pu32DgainLin < 8191)
    {
        if (*pu32DgainLin >= 2048)
        {
            *pu32DgainDb = (*pu32DgainLin >> 1);
            u32Dgain = 0x60;
        }
        else
        {
            *pu32DgainDb = *pu32DgainLin;
            u32Dgain = 0x20;
        }
    }
    else
    {
        *pu32DgainDb = (*pu32DgainLin >> 2);
        u32Dgain = 0xE0;
    }

    g_stSnsRegsInfo.astI2cData[14].u32Data = u32Dgain & 0xff;
	#endif
    
    return;
}

static HI_VOID cmos_gains_update(HI_U32 u32Again, HI_U32 u32Dgain)
{
	HI_U32 u32DgainTmp;
	u32Dgain = u32Dgain << 2;
	
	g_stSnsRegsInfo.astI2cData[3].u32Data = u32Again >> 8;
    g_stSnsRegsInfo.astI2cData[4].u32Data = u32Again & 0xff;

    if (u32Dgain < 8191)
    {
        if (u32Dgain >= 2048)
        {
            u32Dgain = (u32Dgain >> 1);
            u32DgainTmp = 0x60;
        }
        else
        {
            u32Dgain = u32Dgain;
            u32DgainTmp = 0x20;
        }
    }
    else
    {
        u32Dgain = (u32Dgain >> 2);
        u32DgainTmp = 0xE0;
    }

    g_stSnsRegsInfo.astI2cData[14].u32Data = u32DgainTmp & 0xff;

    g_stSnsRegsInfo.astI2cData[8].u32Data = u32Dgain >> 8;
    g_stSnsRegsInfo.astI2cData[9].u32Data = u32Dgain & 0xff;
    g_stSnsRegsInfo.astI2cData[10].u32Data = u32Dgain >> 8;
    g_stSnsRegsInfo.astI2cData[11].u32Data = u32Dgain & 0xff;
    g_stSnsRegsInfo.astI2cData[12].u32Data = u32Dgain >> 8;
    g_stSnsRegsInfo.astI2cData[13].u32Data = u32Dgain & 0xff;

    return;
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

/* AWB default parameter and function */
#ifdef INIFILE_CONFIG_MODE

static HI_S32 cmos_get_awb_default(AWB_SENSOR_DEFAULT_S *pstAwbSnsDft)
{
    HI_U8 i;
    
    if (HI_NULL == pstAwbSnsDft)
    {
        printf("null pointer when get awb default value!\n");
        return -1;
    }

    memset(pstAwbSnsDft, 0, sizeof(AWB_SENSOR_DEFAULT_S));
    switch (genSensorMode)
    {
        default:
        case WDR_MODE_NONE:
            pstAwbSnsDft->u16WbRefTemp = g_AwbDft[0].u16WbRefTemp;

            for(i= 0; i < 4; i++)
            {
                pstAwbSnsDft->au16GainOffset[i] = g_AwbDft[0].au16GainOffset[i];
            }
           
            for(i= 0; i < 6; i++)
            {
                pstAwbSnsDft->as32WbPara[i] = g_AwbDft[0].as32WbPara[i];
            }
            memcpy(&pstAwbSnsDft->stCcm, &g_AwbDft[0].stCcm, sizeof(AWB_CCM_S));
            memcpy(&pstAwbSnsDft->stAgcTbl, &g_AwbDft[0].stAgcTbl, sizeof(AWB_AGC_TABLE_S));
        break;
        
    }
    return 0;
}

#else

static AWB_CCM_S g_stAwbCcm =
{  
    5120,    
    {       
        0x01E0,  0x80C0,  0x8020,
        0x8056,  0x01AA,  0x8054,       
        0x000E,  0x80DA,  0x01CC    
    },  
    
    3633,    
    {       
        0x01DB,  0x80A6,  0x8035,       
        0x8081,  0x01C8,  0x8047,       
        0x0003,  0x80FF,  0x01FC    
    },
    
    2465,    
    {            
        0x01D9,  0x80A3,  0x8036,        
        0x8094,  0x01C8,  0x8034,       
        0x8011,  0x8211,  0x0322    
    }
};

static AWB_AGC_TABLE_S g_stAwbAgcTable =
{
    /* bvalid */
    1,
	
    /*1,  2,  4,  8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384, 32768*/
    /* saturation */   
    //{0x80,0x80,0x7b,0x78,0x70,0x68,0x60,0x58,0x50,0x48,0x40,0x38,0x38,0x38,0x38,0x38}
    {0x7e,0x7a,0x76,0x74,0x60,0x58,0x4c,0x40,0x3a,0x3a,0x3a,0x3a,0x3a,0x3a,0x3a,0x3a}


};

static HI_S32 cmos_get_awb_default(AWB_SENSOR_DEFAULT_S *pstAwbSnsDft)
{
    if (HI_NULL == pstAwbSnsDft)
    {
        printf("null pointer when get awb default value!\n");
        return -1;
    }

    memset(pstAwbSnsDft, 0, sizeof(AWB_SENSOR_DEFAULT_S));
    pstAwbSnsDft->u16WbRefTemp = 5120;
    pstAwbSnsDft->au16GainOffset[0] = 0x1F8;    
    pstAwbSnsDft->au16GainOffset[1] = 0x100;    
    pstAwbSnsDft->au16GainOffset[2] = 0x100;    
    pstAwbSnsDft->au16GainOffset[3] = 0x1B0;    
    pstAwbSnsDft->as32WbPara[0] = 60;    
    pstAwbSnsDft->as32WbPara[1] = 85;    
    pstAwbSnsDft->as32WbPara[2] = -111;    
    pstAwbSnsDft->as32WbPara[3] = 186961;    
    pstAwbSnsDft->as32WbPara[4] = 128;    
    pstAwbSnsDft->as32WbPara[5] = -138300;
    
    memcpy(&pstAwbSnsDft->stCcm, &g_stAwbCcm, sizeof(AWB_CCM_S));
    memcpy(&pstAwbSnsDft->stAgcTbl, &g_stAwbAgcTable, sizeof(AWB_AGC_TABLE_S));
    
    return 0;
}
#endif

HI_S32 cmos_init_awb_exp_function(AWB_SENSOR_EXP_FUNC_S *pstExpFuncs)
{
    memset(pstExpFuncs, 0, sizeof(AWB_SENSOR_EXP_FUNC_S));

    pstExpFuncs->pfn_cmos_get_awb_default = cmos_get_awb_default;

    return 0;
}

/* ISP default parameter and function */
#ifdef INIFILE_CONFIG_MODE

HI_U32 cmos_get_isp_default(ISP_CMOS_DEFAULT_S *pstDef)
{   
    if (HI_NULL == pstDef)
    {
        printf("null pointer when get isp default value!\n");
        return -1;
    }

    memset(pstDef, 0, sizeof(ISP_CMOS_DEFAULT_S));

    switch (genSensorMode)
    {
        default:
        case WDR_MODE_NONE:    
            memcpy(&pstDef->stDrc, &g_IspDft[0].stDrc, sizeof(ISP_CMOS_DRC_S));
            memcpy(&pstDef->stNoiseTbl, &g_IspDft[0].stNoiseTbl, sizeof(ISP_CMOS_NOISE_TABLE_S));            
            memcpy(&pstDef->stDemosaic, &g_IspDft[0].stDemosaic, sizeof(ISP_CMOS_DEMOSAIC_S));
            memcpy(&pstDef->stRgbSharpen, &g_IspDft[0].stRgbSharpen, sizeof(ISP_CMOS_RGBSHARPEN_S));
            memcpy(&pstDef->stGamma, &g_IspDft[0].stGamma, sizeof(ISP_CMOS_GAMMA_S));
            memcpy(&pstDef->stGe, &g_IspDft[0].stGe, sizeof(ISP_CMOS_GE_S));            
        break;
    }
    pstDef->stSensorMaxResolution.u32MaxWidth  = 1280;
    pstDef->stSensorMaxResolution.u32MaxHeight = 720;

    return 0;
}

#else


#define DMNR_CALIB_CARVE_NUM_OV9750 12

float g_coef_calib_ov9750[DMNR_CALIB_CARVE_NUM_OV9750][4] = 
{
    {100.000000f, 2.000000f, 0.000000f, 22.000000f, }, 
    {206.000000f, 2.313867f, 0.000000f, 24.000000f, }, 
    {443.000000f, 2.646404f, 0.000000f, 26.000000f, }, 
    {805.000000f, 2.905796f, 0.000000f, 28.000000f, }, 
    {1586.000000f, 3.200303f, 0.000000f, 32.000000f, }, 
    {3271.000000f, 3.514681f, 0.000000f, 41.000000f, }, 
    {6549.000000f, 3.816175f, 0.000000f, 52.000000f, }, 
    {13085.000000f, 4.116774f, 0.000000f, 65.000000f, }, 
    {26171.000000f, 4.417820f, 0.000000f, 69.000000f, }, 
    {35111.000000f, 4.545443f, 0.092135f, 36.210743f, }, 
    {50897.000000f, 4.706692f, 0.100546f, 43.523537f, },
    {74403.000000f, 4.871591f, 0.095404f, 60.593433f, },
};

static ISP_NR_ISO_PARA_TABLE_S g_stNrIsoParaTab[HI_ISP_NR_ISO_LEVEL_MAX] = 
{
     //u16Threshold//u8varStrength//u8fixStrength//u8LowFreqSlope	
       {1100,       160,             0,            0 },  //100    //                      //                                                
       {1500,       120,             0,            0 },  //200    // ISO                  // ISO //u8LowFreqSlope
       {1500,       100,             0,            0 },  //400    //{400,  1200, 96,256}, //{400 , 0  }
       {1750,       80,              0,            8 },  //800    //{800,  1400, 80,256}, //{600 , 2  }
       {1500,       255,             0,            6 },  //1600   //{1600, 1200, 72,256}, //{800 , 8  }
       {1500,       255,             20,            0 },  //3200   //{3200, 1200, 64,256}, //{1000, 12 }
       {1375,       255,             80,            0 },  //6400   //{6400, 1100, 56,256}, //{1600, 6  }
       {1375,       255,             120,            0 },  //12800  //{12000,1100, 48,256}, //{2400, 0  }
       {1375,       255,             160,            0 },  //25600  //{36000,1100, 48,256}, //
       {1375,       255,             180,            0 },  //51200  //{64000,1100, 96,256}, //
       {1250,       255,             180,            0 },  //102400 //{82000,1000,240,256}, //
       {1250,       255,             180,            0 },  //204800 //                           //
       {1250,       255,             180,            0 },  //409600 //                           //
       {1250,       255,             180,            0 },  //819200 //                           //
       {1250,       255,             180,            0 },  //1638400//                           //
       {1250,       255,             180,            0 },  //3276800//                           //
};

static ISP_CMOS_DEMOSAIC_S g_stIspDemosaic =
{
	/*For Demosaic*/
	1, /*bEnable*/			
	12,/*u16VhLimit*/	
	8,/*u16VhOffset*/
	48,   /*u16VhSlope*/
	/*False Color*/
	1,    /*bFcrEnable*/
	{24,24,24,24,24,24,24,24,10, 0, 0, 0, 0, 0, 0, 0},    /*au8FcrStrength[ISP_AUTO_ISO_STENGTH_NUM]*/
	{24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,24},    /*au8FcrThreshold[ISP_AUTO_ISO_STENGTH_NUM]*/	
	/*For Ahd*/
	400, /*u16UuSlope*/	
	{512,512,512,512,512,512,512,400,0,0,0,0,0,0,0,0}    /*au16NpOffset[ISP_AUTO_ISO_STENGTH_NUM]*/	
};

static ISP_CMOS_GE_S g_stIspGe =
{
	/*For GE*/
	0,    /*bEnable*/			
	7,    /*u8Slope*/	
	7,    /*u8Sensitivity*/
	4096, /*u16Threshold*/
	4096, /*u16SensiThreshold*/	
	{1024,1024,1024,2048,2048,2048,2048,2048,2048,2048,2048,2048,2048,2048,2048,2048}    /*au16Strength[ISP_AUTO_ISO_STENGTH_NUM]*/	
};


static ISP_CMOS_RGBSHARPEN_S g_stIspRgbSharpen =
{      
  //{100,	200,	400,	800,	1600,	3200,	6400,	12800,	25600,	51200,	102400,	204800,	409600,	819200,	1638400,	3276800}; //ISO
	{0,		0,		0,		0,		1,		1,		1,		1,		1,		1,		1,		1,		1,		1,		1,			1},/* bEnLowLumaShoot */
    {37,	30,		28, 	28, 	26, 	26,  	24,  	22,    	20,    	18,    	15,    	12,    	12,    	12,    	12,    		12},/*SharpenUD*/
    {25,	25,		28,	 	28, 	35, 	40,  	50, 	60, 	68, 	68, 	68, 	68, 	68, 	68, 	68, 		68},/*SharpenD*/
    {10,    10,  	12,  	14, 	16, 	18,  	19,  	20,    	22,    	24,    	26,    	28,    	28,    	28,    	28,    		28},/*TextureNoiseThd*/
    {0,  	0,  	2,  	2,   	5,   	7,    	12,    	12,    	0,    	0,    	0,    	0,    	0,    	0,    	0,    		0},/*EdgeNoiseThd*/
    {150,   130,  	120,  	110,  	110,  	90,    	60,    	40,    	20,    	10,    	0,    	0,    	0,    	0,    	0,    		0},/*overshoot*/
    {160,   160,  	160,  	160, 	200, 	200,  	200,  	200,   	200,  	220,  	255,  	255,   	255,  	255,  	255,  		255},/*undershoot*/
};

static ISP_CMOS_UVNR_S g_stIspUVNR = 
{
  //{100,	200,	400,	800,	1600,	3200,	6400,	12800,	25600,	51200,	102400,	204800,	409600,	819200,	1638400,	3276800};
	{1,	    2,      4,      5,      7,      48,     32,     16,     16,     16,     16,     16,     16,     16,     16,     16},      /*UVNRThreshold*/
 	{0,		0,		0,		0,		0,		0,		0,		0,		0,		1,		1,		2,		2,		2,		2,		2},  /*Coring_lutLimit*/
	{0,		0,		0,		16,		34,		34,		34,		34,		34,		34,		34,		34,		34,		34,		34,		34}  /*UVNR_blendRatio*/
};

static ISP_CMOS_DPC_S g_stCmosDpc = 
{
	//1,/*IR_channel*/
	//1,/*IR_position*/
	{45,110,152,204,220,224,224,224,224,224,224,224,224,224,224,224},/*au16Strength[16]*/
	{0,0,0,0,0,0,0x28,0x5A,0x5A,0x8C,0x8C,0x8C,0xE5,0xE5,0xE5,0xE5},/*au16BlendRatio[16]*/
};

static ISP_CMOS_RGBIR_S g_stCmosRgbir =
{
    0, /* bValid*/
        
    {
        0,
        0,/*0:ISP_CMOS_IRPOS_TYPE_GR; 1:ISP_CMOS_IRPOS_TYPE_GB*/
        4031,
    },
    {
        0,
        1,
        1,
        0,/*0:OP_CMOS_TYPE_AUTO; 1:OP_CMOS_TYPE_MANUL*/
        0x100,
        {266,3,36,8,272,30,15,5,283,-300,-305,-313,-3,-12,-4}/*new method*/
    }
};

static ISP_CMOS_DRC_S g_stIspDrc =
{
    0,
    10,
    0,
    2,
    192,
    60,
    0,
    0,
    0,
    {1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024}
};

HI_U32 cmos_get_isp_default(ISP_CMOS_DEFAULT_S *pstDef)
{
    if (HI_NULL == pstDef)
    {
        printf("null pointer when get isp default value!\n");
        return -1;
    }

    memset(pstDef, 0, sizeof(ISP_CMOS_DEFAULT_S));

    memcpy(&pstDef->stDrc, &g_stIspDrc, sizeof(ISP_CMOS_DRC_S));
    memcpy(&pstDef->stDemosaic, &g_stIspDemosaic, sizeof(ISP_CMOS_DEMOSAIC_S));
	memcpy(&pstDef->stRgbSharpen, &g_stIspRgbSharpen, sizeof(ISP_CMOS_RGBSHARPEN_S));
    memcpy(&pstDef->stGe, &g_stIspGe, sizeof(ISP_CMOS_GE_S));

	pstDef->stNoiseTbl.stNrCaliPara.pCalibcoef    = (HI_FLOAT (*)[4])g_coef_calib_ov9750;
	memcpy(&pstDef->stNoiseTbl.stIsoParaTable[0], &g_stNrIsoParaTab[0],sizeof(ISP_NR_ISO_PARA_TABLE_S)*HI_ISP_NR_ISO_LEVEL_MAX);
			
    memcpy(&pstDef->stUvnr,      &g_stIspUVNR,       sizeof(ISP_CMOS_UVNR_S));
	memcpy(&pstDef->stDpc,       &g_stCmosDpc,       sizeof(ISP_CMOS_DPC_S));
    memcpy(&pstDef->stRgbir,     &g_stCmosRgbir,     sizeof(ISP_CMOS_RGBIR_S));

    pstDef->stSensorMaxResolution.u32MaxWidth  = 1280;
    pstDef->stSensorMaxResolution.u32MaxHeight = 720;

    return 0;
}
#endif

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
        pstBlackLevel->au16BlackLevel[i] = 0x40; 
    }

    return 0;  
    
}

HI_VOID cmos_set_pixel_detect(HI_BOOL bEnable)
{
    HI_U32 u32FullLines_5Fps = 0; 
    HI_U32 u32MaxIntTime_5Fps = 0;
    
    if (WDR_MODE_NONE == genSensorMode)
    {
        if (SENSOR_720P_30FPS_MODE == gu8SensorImageMode)
        {
            u32FullLines_5Fps = (VMAX_720P30_LINEAR * 30) / 5;
        }
        else if (SENSOR_960P_30FPS_MODE == gu8SensorImageMode)
        {
            u32FullLines_5Fps = (VMAX_960P30_LINEAR * 30) / 5;
        }
        else
        {
            return;
        }
    }
    else
    {
        return;
    }

    u32FullLines_5Fps = (u32FullLines_5Fps > FULL_LINES_MAX) ? FULL_LINES_MAX : u32FullLines_5Fps;
    u32MaxIntTime_5Fps = u32FullLines_5Fps - 4;

    if (bEnable) /* setup for ISP pixel calibration mode */
    {
        sensor_write_register(VMAX_ADDR_H, (u32FullLines_5Fps & 0xFF00) >> 8);  /* 5fps */
        sensor_write_register(VMAX_ADDR_L, u32FullLines_5Fps & 0xFF);           /* 5fps */
        sensor_write_register(LONG_EXPOSURE_ADDR, (u32MaxIntTime_5Fps >> 12));               /* max exposure lines */
        sensor_write_register(LONG_EXPOSURE_ADDR+1, ((u32MaxIntTime_5Fps & 0xFFF)>>4));     /* max exposure lines */
        sensor_write_register(LONG_EXPOSURE_ADDR+2, ((u32MaxIntTime_5Fps & 0xF)<<4));       /* max exposure lines */
        sensor_write_register(LONG_AGC_ADDR, 0x00);                                    /* min AG */
        sensor_write_register(LONG_AGC_ADDR+1, 0x80);                                  /* min AG */
    }
    else /* setup for ISP 'normal mode' */
    {
        sensor_write_register(VMAX_ADDR_H, (gu32FullLinesStd & 0xFF00) >> 8);
        sensor_write_register(VMAX_ADDR_L, gu32FullLinesStd & 0xFF);
        
        bInit = HI_FALSE;
    }

    return;
}

HI_VOID cmos_set_wdr_mode(HI_U8 u8Mode)
{
    bInit = HI_FALSE;
    
    switch(u8Mode)
    {
        case WDR_MODE_NONE:
            if (SENSOR_720P_30FPS_MODE == gu8SensorImageMode)
            {
                gu32FullLinesStd = VMAX_720P30_LINEAR;
            }
            else if (SENSOR_960P_30FPS_MODE == gu8SensorImageMode)
            {
                gu32FullLinesStd = VMAX_960P30_LINEAR;
            }
            genSensorMode = WDR_MODE_NONE;
            printf("linear mode\n");
        break;
        default:
            printf("NOT support this mode!\n");
            return;
        break;
    }
    
    return;
}

HI_U32 cmos_get_sns_regs_info(ISP_SNS_REGS_INFO_S *pstSnsRegsInfo)
{
    HI_S32 i;

    if (HI_FALSE == bInit)
    {
        g_stSnsRegsInfo.enSnsType = ISP_SNS_I2C_TYPE;
        g_stSnsRegsInfo.u8Cfg2ValidDelayMax = 2;		
        g_stSnsRegsInfo.u32RegNum = 15;
	
        for (i=0; i<g_stSnsRegsInfo.u32RegNum; i++)
        {	
            g_stSnsRegsInfo.astI2cData[i].bUpdate = HI_TRUE;
            g_stSnsRegsInfo.astI2cData[i].u8DevAddr = sensor_i2c_addr;
            g_stSnsRegsInfo.astI2cData[i].u32AddrByteNum = sensor_addr_byte;
            g_stSnsRegsInfo.astI2cData[i].u32DataByteNum = sensor_data_byte;
        }		
        g_stSnsRegsInfo.astI2cData[0].u8DelayFrmNum = 0;
        g_stSnsRegsInfo.astI2cData[0].u32RegAddr = LONG_EXPOSURE_ADDR;
        g_stSnsRegsInfo.astI2cData[1].u8DelayFrmNum = 0;
        g_stSnsRegsInfo.astI2cData[1].u32RegAddr = LONG_EXPOSURE_ADDR+1;
        g_stSnsRegsInfo.astI2cData[2].u8DelayFrmNum = 0;
        g_stSnsRegsInfo.astI2cData[2].u32RegAddr = LONG_EXPOSURE_ADDR+2;
        g_stSnsRegsInfo.astI2cData[3].u8DelayFrmNum = 0;
        g_stSnsRegsInfo.astI2cData[3].u32RegAddr = LONG_AGC_ADDR;
        g_stSnsRegsInfo.astI2cData[4].u8DelayFrmNum = 0;
        g_stSnsRegsInfo.astI2cData[4].u32RegAddr = LONG_AGC_ADDR+1;
        
        g_stSnsRegsInfo.astI2cData[5].u8DelayFrmNum = 0; 
        g_stSnsRegsInfo.astI2cData[5].u32RegAddr = DC_GAIN_ADDR; 

        g_stSnsRegsInfo.astI2cData[6].u8DelayFrmNum = 0; 
        g_stSnsRegsInfo.astI2cData[6].u32RegAddr = VMAX_ADDR_H; 
        g_stSnsRegsInfo.astI2cData[7].u8DelayFrmNum = 0; 
        g_stSnsRegsInfo.astI2cData[7].u32RegAddr = VMAX_ADDR_L; 

        g_stSnsRegsInfo.astI2cData[8].u8DelayFrmNum =  1;
        g_stSnsRegsInfo.astI2cData[8].u32RegAddr = R_DGC_ADDR; 
        g_stSnsRegsInfo.astI2cData[9].u8DelayFrmNum =  1;
        g_stSnsRegsInfo.astI2cData[9].u32RegAddr = R_DGC_ADDR+1;
        g_stSnsRegsInfo.astI2cData[10].u8DelayFrmNum = 1;
        g_stSnsRegsInfo.astI2cData[10].u32RegAddr = G_DGC_ADDR; 
        g_stSnsRegsInfo.astI2cData[11].u8DelayFrmNum = 1;
        g_stSnsRegsInfo.astI2cData[11].u32RegAddr = G_DGC_ADDR+1;
        g_stSnsRegsInfo.astI2cData[12].u8DelayFrmNum = 1;
        g_stSnsRegsInfo.astI2cData[12].u32RegAddr = B_DGC_ADDR; 
        g_stSnsRegsInfo.astI2cData[13].u8DelayFrmNum = 1;
        g_stSnsRegsInfo.astI2cData[13].u32RegAddr = B_DGC_ADDR+1; 
        g_stSnsRegsInfo.astI2cData[14].u8DelayFrmNum = 1;
        g_stSnsRegsInfo.astI2cData[14].u32RegAddr = 0x5003; //Digital Gain:R5003[7:6] 1x,2x,3x,4x
        
        bInit = HI_TRUE;
    }
    else    
    {        
        for (i=0; i<g_stSnsRegsInfo.u32RegNum; i++)        
        {            
            if (g_stSnsRegsInfo.astI2cData[i].u32Data == g_stPreSnsRegsInfo.astI2cData[i].u32Data)            
            {                
                g_stSnsRegsInfo.astI2cData[i].bUpdate = HI_FALSE;
            }            
            else            
            {                
                g_stSnsRegsInfo.astI2cData[i].bUpdate = HI_TRUE;
            }        
        }    
    }

    if (HI_NULL == pstSnsRegsInfo)
    {
        printf("null pointer when get sns reg info!\n");
        return -1;
    }

    memcpy(pstSnsRegsInfo, &g_stSnsRegsInfo, sizeof(ISP_SNS_REGS_INFO_S)); 
    memcpy(&g_stPreSnsRegsInfo, &g_stSnsRegsInfo, sizeof(ISP_SNS_REGS_INFO_S)); 

    return 0;
}

int  sensor_set_inifile_path(const char *pcPath)
{
    memset(pcName, 0, sizeof(pcName));
        
    if (HI_NULL == pcPath)
    {        
        strncat(pcName, "configs/", strlen("configs/"));
        strncat(pcName, CMOS_CFG_INI, sizeof(CMOS_CFG_INI));
    }
    else
    {
		if(strlen(pcPath) > (PATHLEN_MAX - 30))
		{
			printf("Set inifile path is larger PATHLEN_MAX!\n");
			return -1;        
		}
        strncat(pcName, pcPath, strlen(pcPath));
        strncat(pcName, CMOS_CFG_INI, sizeof(CMOS_CFG_INI));
    }
    
    return 0;

}

static HI_S32 cmos_set_image_mode(ISP_CMOS_SENSOR_IMAGE_MODE_S *pstSensorImageMode)
{
    HI_U8 u8SensorImageMode = gu8SensorImageMode;

    bInit = HI_FALSE;
    
    if (HI_NULL == pstSensorImageMode )
    {
        printf("null pointer when set image mode\n");
        return -1;
    }

    if ((pstSensorImageMode->u16Width <= 1280) && (pstSensorImageMode->u16Height <= 720))
    {
        if (WDR_MODE_NONE == genSensorMode)
        {
            if (pstSensorImageMode->f32Fps <= 30)
            {
                u8SensorImageMode = SENSOR_720P_30FPS_MODE;
            }
            else
            {
                printf("Not support! Width:%d, Height:%d, Fps:%f, WDRMode:%d\n", 
                    pstSensorImageMode->u16Width, 
                    pstSensorImageMode->u16Height,
                    pstSensorImageMode->f32Fps,
                    genSensorMode);

                return -1;
            }
        }
        else
        {
            printf("Not support! Width:%d, Height:%d, Fps:%f, WDRMode:%d\n", 
                pstSensorImageMode->u16Width, 
                pstSensorImageMode->u16Height,
                pstSensorImageMode->f32Fps,
                genSensorMode);

            return -1;
        }
    }
    else if((pstSensorImageMode->u16Width <= 1280) && (pstSensorImageMode->u16Height <= 960))
    {
        if (WDR_MODE_NONE == genSensorMode)
        {
            if (pstSensorImageMode->f32Fps <= 30)
            {
                u8SensorImageMode = SENSOR_960P_30FPS_MODE;
            }
            else
            {
                printf("Not support! Width:%d, Height:%d, Fps:%f, WDRMode:%d\n", 
                    pstSensorImageMode->u16Width, 
                    pstSensorImageMode->u16Height,
                    pstSensorImageMode->f32Fps,
                    genSensorMode);

                return -1;
            }
        }
        else
        {
            printf("Not support! Width:%d, Height:%d, Fps:%f, WDRMode:%d\n", 
                pstSensorImageMode->u16Width, 
                pstSensorImageMode->u16Height,
                pstSensorImageMode->f32Fps,
                genSensorMode);

            return -1;
        }
    }
    else
    {
        printf("Not support! Width:%d, Height:%d, Fps:%f, WDRMode:%d\n", 
            pstSensorImageMode->u16Width, 
            pstSensorImageMode->u16Height,
            pstSensorImageMode->f32Fps,
            genSensorMode);

        return -1;
    }

    /* Sensor first init */
    if (HI_FALSE == bSensorInit)
    {
        gu8SensorImageMode = u8SensorImageMode;
        
        return 0;
    }

    /* Switch SensorImageMode */
    if (u8SensorImageMode == gu8SensorImageMode)
    {
        /* Don't need to switch SensorImageMode */
        return -1;
    }
    
    gu8SensorImageMode = u8SensorImageMode;

    return 0;
}

HI_VOID sensor_global_init()
{   
    gu8SensorImageMode = SENSOR_720P_30FPS_MODE;
    genSensorMode = WDR_MODE_NONE;
    gu32FullLinesStd = VMAX_720P30_LINEAR; 
    gu32FullLines = VMAX_720P30_LINEAR;
    bInit = HI_FALSE;
    bSensorInit = HI_FALSE; 

    memset(&g_stSnsRegsInfo, 0, sizeof(ISP_SNS_REGS_INFO_S));
    memset(&g_stPreSnsRegsInfo, 0, sizeof(ISP_SNS_REGS_INFO_S));
    
#ifdef INIFILE_CONFIG_MODE 
    HI_S32 s32Ret = HI_SUCCESS;
    s32Ret = Cmos_LoadINIPara(pcName);
    if (HI_SUCCESS != s32Ret)
    {
        printf("Cmos_LoadINIPara failed!!!!!!\n");
    }
#else

#endif    
}

HI_S32 cmos_init_sensor_exp_function(ISP_SENSOR_EXP_FUNC_S *pstSensorExpFunc)
{
    memset(pstSensorExpFunc, 0, sizeof(ISP_SENSOR_EXP_FUNC_S));

    pstSensorExpFunc->pfn_cmos_sensor_init = sensor_init;
    pstSensorExpFunc->pfn_cmos_sensor_exit = sensor_exit;
    pstSensorExpFunc->pfn_cmos_sensor_global_init = sensor_global_init;
    pstSensorExpFunc->pfn_cmos_set_image_mode = cmos_set_image_mode;
    pstSensorExpFunc->pfn_cmos_set_wdr_mode = cmos_set_wdr_mode;
    
    pstSensorExpFunc->pfn_cmos_get_isp_default = cmos_get_isp_default;
    pstSensorExpFunc->pfn_cmos_get_isp_black_level = cmos_get_isp_black_level;
    pstSensorExpFunc->pfn_cmos_set_pixel_detect = cmos_set_pixel_detect;
    pstSensorExpFunc->pfn_cmos_get_sns_reg_info = cmos_get_sns_regs_info;

    return 0;
}

/****************************************************************************
 * callback structure                                                       *
 ****************************************************************************/
 
int sensor_register_callback(void)
{
    ISP_DEV IspDev = 0;
    HI_S32 s32Ret;
    ALG_LIB_S stLib;
    ISP_SENSOR_REGISTER_S stIspRegister;
    AE_SENSOR_REGISTER_S  stAeRegister;
    AWB_SENSOR_REGISTER_S stAwbRegister;

    cmos_init_sensor_exp_function(&stIspRegister.stSnsExp);
    s32Ret = HI_MPI_ISP_SensorRegCallBack(IspDev, OV9750_ID, &stIspRegister);
    if (s32Ret)
    {
        printf("sensor register callback function failed!\n");
        return s32Ret;
    }
    
    stLib.s32Id = 0;
    strncpy(stLib.acLibName, HI_AE_LIB_NAME, sizeof(HI_AE_LIB_NAME));
    cmos_init_ae_exp_function(&stAeRegister.stSnsExp);
    s32Ret = HI_MPI_AE_SensorRegCallBack(IspDev, &stLib, OV9750_ID, &stAeRegister);
    if (s32Ret)
    {
        printf("sensor register callback function to ae lib failed!\n");
        return s32Ret;
    }

    stLib.s32Id = 0;
    strncpy(stLib.acLibName, HI_AWB_LIB_NAME, sizeof(HI_AWB_LIB_NAME));
    cmos_init_awb_exp_function(&stAwbRegister.stSnsExp);
    s32Ret = HI_MPI_AWB_SensorRegCallBack(IspDev, &stLib, OV9750_ID, &stAwbRegister);
    if (s32Ret)
    {
        printf("sensor register callback function to awb lib failed!\n");
        return s32Ret;
    }
    
    return 0;
}

int sensor_unregister_callback(void)
{
    ISP_DEV IspDev = 0;
    HI_S32 s32Ret;
    ALG_LIB_S stLib;

    s32Ret = HI_MPI_ISP_SensorUnRegCallBack(IspDev, OV9750_ID);
    if (s32Ret)
    {
        printf("sensor unregister callback function failed!\n");
        return s32Ret;
    }
    
    stLib.s32Id = 0;
    strncpy(stLib.acLibName, HI_AE_LIB_NAME, sizeof(HI_AE_LIB_NAME));
    s32Ret = HI_MPI_AE_SensorUnRegCallBack(IspDev, &stLib, OV9750_ID);
    if (s32Ret)
    {
        printf("sensor unregister callback function to ae lib failed!\n");
        return s32Ret;
    }

    stLib.s32Id = 0;
    strncpy(stLib.acLibName, HI_AWB_LIB_NAME, sizeof(HI_AWB_LIB_NAME));
    s32Ret = HI_MPI_AWB_SensorUnRegCallBack(IspDev, &stLib, OV9750_ID);
    if (s32Ret)
    {
        printf("sensor unregister callback function to awb lib failed!\n");
        return s32Ret;
    }
    
    return 0;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /* __OV9750_CMOS_H_ */
