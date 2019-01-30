#if !defined(__OV9732_CMOS_H_)
#define __OV9732_CMOS_H_

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


#define OV9732_ID 9732

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

#define SENSOR_720P_30FPS_MODE (1)

#define VMAX_OV9732_720P30_LINEAR     (812)
#define FULL_LINES_MAX  (0xFFFF)


HI_U8 gu8SensorImageMode = SENSOR_720P_30FPS_MODE;
WDR_MODE_E genSensorMode = WDR_MODE_NONE;

static HI_U32 gu32FullLinesStd = VMAX_OV9732_720P30_LINEAR;
static HI_U32 gu32FullLines = VMAX_OV9732_720P30_LINEAR;

static HI_BOOL bInit = HI_FALSE;
HI_BOOL bSensorInit = HI_FALSE;
ISP_SNS_REGS_INFO_S g_stSnsRegsInfo = {0};
ISP_SNS_REGS_INFO_S g_stPreSnsRegsInfo = {0};

#if 0
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
    30                  // u32MinIrisFNOTarget
};
#endif

#define PATHLEN_MAX 256
#define CMOS_CFG_INI "ov9732_cfg.ini"
static char pcName[PATHLEN_MAX] = "configs/ov9732_cfg.ini";


/* AE default parameter and function */
static HI_S32 cmos_get_ae_default(AE_SENSOR_DEFAULT_S *pstAeSnsDft)
{
    if (HI_NULL == pstAeSnsDft)
    {
        printf("null pointer when get ae default value!\n");
        return -1;
    }

    pstAeSnsDft->u32LinesPer500ms = VMAX_OV9732_720P30_LINEAR * 30 / 2;
    pstAeSnsDft->u32FullLinesStd = gu32FullLinesStd;
    pstAeSnsDft->u32FlickerFreq = 0;

    pstAeSnsDft->au8HistThresh[0] = 0xd;
    pstAeSnsDft->au8HistThresh[1] = 0x28;
    pstAeSnsDft->au8HistThresh[2] = 0x60;
    pstAeSnsDft->au8HistThresh[3] = 0x80;
            
    pstAeSnsDft->u8AeCompensation = 0x38;

    pstAeSnsDft->stIntTimeAccu.enAccuType = AE_ACCURACY_LINEAR;
    pstAeSnsDft->stIntTimeAccu.f32Accuracy = 1;
    pstAeSnsDft->stIntTimeAccu.f32Offset = 0;
    pstAeSnsDft->u32MaxIntTime = gu32FullLinesStd - 4;
    pstAeSnsDft->u32MinIntTime = 4;
    pstAeSnsDft->u32MaxIntTimeTarget = 65535;
    pstAeSnsDft->u32MinIntTimeTarget = 2;


    pstAeSnsDft->stAgainAccu.enAccuType = AE_ACCURACY_TABLE;
    pstAeSnsDft->stAgainAccu.f32Accuracy = 1;
    pstAeSnsDft->u32MaxAgain = 16229;
    pstAeSnsDft->u32MinAgain = 1024;
    pstAeSnsDft->u32MaxAgainTarget = pstAeSnsDft->u32MaxAgain;
    pstAeSnsDft->u32MinAgainTarget = pstAeSnsDft->u32MinAgain;
        
    pstAeSnsDft->stDgainAccu.enAccuType = AE_ACCURACY_LINEAR;
    pstAeSnsDft->stDgainAccu.f32Accuracy = 1.0/256;
    pstAeSnsDft->u32MaxDgain = 256;
    pstAeSnsDft->u32MinDgain = 256;
    pstAeSnsDft->u32MaxDgainTarget = 256;  
    pstAeSnsDft->u32MinDgainTarget = 256; 
    
    pstAeSnsDft->u32ISPDgainShift = 8;
    pstAeSnsDft->u32MinISPDgainTarget = 1 << pstAeSnsDft->u32ISPDgainShift;
    pstAeSnsDft->u32MaxISPDgainTarget = 16 << pstAeSnsDft->u32ISPDgainShift;
	
    pstAeSnsDft->u32LinesPer500ms = gu32FullLinesStd*30/2;

    //memcpy(&pstAeSnsDft->stPirisAttr, &gstPirisAttr, sizeof(ISP_PIRIS_ATTR_S));
    pstAeSnsDft->enMaxIrisFNO = ISP_IRIS_F_NO_1_4;
    pstAeSnsDft->enMinIrisFNO = ISP_IRIS_F_NO_5_6;

    pstAeSnsDft->bAERouteExValid = HI_FALSE;
    pstAeSnsDft->stAERouteAttr.u32TotalNum = 0;
    pstAeSnsDft->stAERouteAttrEx.u32TotalNum = 0;

    /*For some OV sensors, AERunInterval needs to be set more than 1*/
    pstAeSnsDft->u8AERunInterval = 2;
    
    return 0;
}

/* the function of sensor set fps */
static HI_VOID cmos_fps_set(HI_FLOAT f32Fps, AE_SENSOR_DEFAULT_S *pstAeSnsDft)
{
    if ((f32Fps <= 30) && (f32Fps >= 0.37))
    {
        if(SENSOR_720P_30FPS_MODE == gu8SensorImageMode)
        {
            gu32FullLinesStd = VMAX_OV9732_720P30_LINEAR * 30 / f32Fps;

            g_stSnsRegsInfo.astI2cData[10].u32Data = (gu32FullLinesStd >> 8) & 0xff;
            g_stSnsRegsInfo.astI2cData[11].u32Data = gu32FullLinesStd & 0xff;

            pstAeSnsDft->f32Fps = f32Fps;
            pstAeSnsDft->u32MaxIntTime = gu32FullLinesStd - 4;
            pstAeSnsDft->u32FullLinesStd = gu32FullLinesStd;
            pstAeSnsDft->u32LinesPer500ms = gu32FullLinesStd * f32Fps/2;
            gu32FullLines = gu32FullLinesStd;
            pstAeSnsDft->u32FullLines = gu32FullLines;
        }
    }
    else
    {
        printf("Not support Fps: %f\n", f32Fps);
        return;
    }
  
    return;
}

static HI_VOID cmos_slow_framerate_set(HI_U32 u32FullLines,
    AE_SENSOR_DEFAULT_S *pstAeSnsDft)
{
    u32FullLines = (u32FullLines > FULL_LINES_MAX) ? FULL_LINES_MAX : u32FullLines;

    gu32FullLines = u32FullLines;
    
    g_stSnsRegsInfo.astI2cData[10].u32Data = (gu32FullLines >> 8) & 0xff;
    g_stSnsRegsInfo.astI2cData[11].u32Data = gu32FullLines & 0xff;

    pstAeSnsDft->u32FullLines = gu32FullLines;
    pstAeSnsDft->u32MaxIntTime = gu32FullLines - 4;
    
    return;
}

/* while isp notify ae to update sensor regs, ae call these funcs. */
static HI_VOID cmos_inttime_update(HI_U32 u32IntTime)
{


    g_stSnsRegsInfo.astI2cData[0].u32Data = (u32IntTime >> 12) & 0xF;
    g_stSnsRegsInfo.astI2cData[1].u32Data = (u32IntTime >> 4) & 0xFF;
    g_stSnsRegsInfo.astI2cData[2].u32Data = (u32IntTime & 0xF) << 4;

	
    return;
}

static HI_U32 analog_gain_table[64] =
{
    1024, 1088, 1152, 1216, 1280, 1344, 1408, 1472, 1536, 1600, 1664, 1728, 1792, 1856, 1920, 1984, 2048,
    2176, 2304, 2432, 2560, 2688, 2816, 2944, 3072, 3200, 3328, 3456, 3584, 3712, 3840, 3968, 4096, 4352,
    4608, 4864, 5120, 5376, 5632, 5888, 6144, 6400, 6656, 6912, 7168, 7424, 7680, 7936, 8192, 8704, 9216,
    9728, 10240, 10752, 11264, 11776, 12288, 12800, 13312, 13824, 14336, 14848, 15360, 15872
};

static HI_U32 analog_gain_reg_table[64] =
{     
    0x010, 0x011, 0x012, 0x013, 0x014, 0x015, 0x016, 0x017, 0x018, 0x019, 0x01A, 0x01B, 0x01C, 0x01D,
    0x01E, 0x01F, 0x020, 0x022, 0x024, 0x026, 0x028, 0x02A, 0x02C, 0x02E, 0x030, 0x032, 0x034, 0x036,
    0x038, 0x03A, 0x03C, 0x03E, 0x040, 0x044, 0x048, 0x04C, 0x050, 0x054, 0x058, 0x05C, 0x060, 0x064,
    0x068, 0x06C, 0x070, 0x074, 0x078, 0x07C, 0x080, 0x088, 0x090, 0x098, 0x0A0, 0x0A8, 0x0B0, 0x0B8,
    0x0C0, 0x0C8, 0x0D0, 0x0D8, 0x0E0, 0x0E8, 0x0F0, 0x0F8
};


static HI_VOID cmos_again_calc_table(HI_U32 *pu32AgainLin, HI_U32 *pu32AgainDb)
{
    int i;

    if (*pu32AgainLin >= analog_gain_table[63])
    {
         *pu32AgainLin = analog_gain_table[63];
         *pu32AgainDb = 63;
         return ;
    }
    
    for (i = 1; i < 64; i++)
    {
        if (*pu32AgainLin < analog_gain_table[i])
        {
            *pu32AgainLin = analog_gain_table[i - 1];
            *pu32AgainDb = i - 1;
            break;
        }
    }

    return;

}

static HI_VOID cmos_gains_update(HI_U32 u32Again, HI_U32 u32Dgain)
{  
    g_stSnsRegsInfo.astI2cData[3].u32Data = analog_gain_reg_table[u32Again];

    g_stSnsRegsInfo.astI2cData[4].u32Data = (u32Dgain >> 8 ) & 0xF;
    g_stSnsRegsInfo.astI2cData[5].u32Data = u32Dgain & 0xFF;
    g_stSnsRegsInfo.astI2cData[6].u32Data = g_stSnsRegsInfo.astI2cData[4].u32Data;
    g_stSnsRegsInfo.astI2cData[7].u32Data = g_stSnsRegsInfo.astI2cData[5].u32Data ;
    g_stSnsRegsInfo.astI2cData[8].u32Data = g_stSnsRegsInfo.astI2cData[4].u32Data;
    g_stSnsRegsInfo.astI2cData[9].u32Data = g_stSnsRegsInfo.astI2cData[5].u32Data;

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
    pstExpFuncs->pfn_cmos_dgain_calc_table  = NULL;

    return 0;
}


/* AWB default parameter and function */
static AWB_CCM_S g_stAwbCcm =
{  
#if 0
    7490,
    {
        0x02a3, 0x818c, 0x8017,
        0x8058, 0x01d1, 0x8079,
        0xa, 0x815b, 0x251,           
    },
    5120,
    {
        0x028b, 0x8161, 0x802a,
        0x8080, 0x01e3, 0x8063,
        0x801d, 0x81fc, 0x319,
    },
    2465,
    {
        0x339, 0x8216, 0x8023,
        0x8097, 0x01aa, 0x8013,
        0x806f, 0x84c6, 0x635,
    }
#endif
    5120,
    {
        0x0241, 0x8105, 0x803c,
        0x8078, 0x01de, 0x8066,
        0x800e, 0x810d, 0x021b,           
    },
    3633,
    {
        0x0246, 0x810c, 0x803a,
        0x8090, 0x01d6, 0x8046,
        0x8013, 0x81a9, 0x02bc,
    },
    2449,
    {
        0x0255, 0x80fa, 0x805b,
        0x80bf, 0x0200, 0x8041,
        0x806d, 0x8269, 0x03d6,
    }

};

static AWB_AGC_TABLE_S g_stAwbAgcTable =
{
    /* bvalid */
    1,
	
    /*1,  2,  4,  8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384, 32768*/
    /* saturation */   
    //{0x80,0x80,0x7a,0x78,0x70,0x68,0x60,0x58,0x50,0x48,0x40,0x38,0x38,0x38,0x38,0x38}
    {0x80,0x78,0x70,0x68,0x60,0x55,0x4b,0x40,0x38,0x38,0x38,0x38,0x38,0x38,0x38,0x38}

};

static HI_S32 cmos_get_awb_default(AWB_SENSOR_DEFAULT_S *pstAwbSnsDft)
{
    if (HI_NULL == pstAwbSnsDft)
    {
        printf("null pointer when get awb default value!\n");
        return -1;
    }

    memset(pstAwbSnsDft, 0, sizeof(AWB_SENSOR_DEFAULT_S));

    pstAwbSnsDft->u16WbRefTemp = 5200;
    pstAwbSnsDft->au16GainOffset[0] = 0x1eb;//0X1C2    
    pstAwbSnsDft->au16GainOffset[1] = 0x100;    
    pstAwbSnsDft->au16GainOffset[2] = 0x100;    
    pstAwbSnsDft->au16GainOffset[3] = 0x18b;//0X1C0    
    pstAwbSnsDft->as32WbPara[0] = 72;    //128
    pstAwbSnsDft->as32WbPara[1] = 75;    //-26
    pstAwbSnsDft->as32WbPara[2] = -109;    //-154
    pstAwbSnsDft->as32WbPara[3] = 216868;    //233501
    pstAwbSnsDft->as32WbPara[4] = 128;    
    pstAwbSnsDft->as32WbPara[5] = -168656; //-184710
    
    memcpy(&pstAwbSnsDft->stCcm, &g_stAwbCcm, sizeof(AWB_CCM_S));
    memcpy(&pstAwbSnsDft->stAgcTbl, &g_stAwbAgcTable, sizeof(AWB_AGC_TABLE_S));
    
    return 0;
}

HI_S32 cmos_init_awb_exp_function(AWB_SENSOR_EXP_FUNC_S *pstExpFuncs)
{
    memset(pstExpFuncs, 0, sizeof(AWB_SENSOR_EXP_FUNC_S));

    pstExpFuncs->pfn_cmos_get_awb_default = cmos_get_awb_default;

    return 0;
}

#define DMNR_CALIB_CARVE_NUM_OV9732 5

float g_coef_calib_ov9732[DMNR_CALIB_CARVE_NUM_OV9732][4] = 
{
    {100.000000f, 2.000000f, 0.040110f, 8.391255f, }, 
    {800.000000f, 2.903090f, 0.047486f, 9.635272f, }, 
    {2525.000000f, 3.402261f, 0.055028f, 13.068982f, }, 
    {5500.000000f, 3.740363f, 0.063797f, 17.750826f, }, 
    {6194.000000f, 3.791971f, 0.062829f, 17.908888f, }, 
};



static ISP_NR_ISO_PARA_TABLE_S g_stNrIsoParaTab[HI_ISP_NR_ISO_LEVEL_MAX] = 
{
     //u16Threshold//u8varStrength//u8fixStrength//u8LowFreqSlope	
       {1500,       80,             256-256,            0 },  //100    //                      //                                                
       {1500,       80,             256-256,            0 },  //200    // ISO                  // ISO //u8LowFreqSlope
       {1500,       80,             256-256,            0 },  //400    //{400,  1200, 96,256}, //{400 , 0  }
       {1750,       80,              256-256,           0 },  //800    //{800,  1400, 80,256}, //{600 , 2  }
       {1500,       80,             256-256,            0 },  //1600   //{1600, 1200, 72,256}, //{800 , 8  }
       {1500,       80,             256-256,            0 },  //3200   //{3200, 1200, 64,256}, //{1000, 12 }
       {1375,       65,             256-256,            0 },  //6400   //{6400, 1100, 56,256}, //{1600, 6  }
       {1375,       70,             256-256,            0 },  //12800  //{12000,1100, 48,256}, //{2400, 0  }
       {1375,       65,             256-256,            0 },  //25600  //{36000,1100, 48,256}, //
       {1375,       70,             256-256,            0 },  //51200  //{64000,1100, 96,256}, //
       {1250,       70,             256-256,            0 },  //102400 //{82000,1000,240,256}, //
       {1250,       70,             256-256,            0 },  //204800 //                           //
       {1250,       70,             256-256,            0 },  //409600 //                           //
       {1250,       70,             256-256,            0 },  //819200 //                           //
       {1250,       70,             256-256,            0 },  //1638400//                           //
       {1250,       70,             256-256,            0 },  //3276800//                           //
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
	{ 8, 8, 8, 8, 8, 8, 8, 8, 3, 0, 0, 0, 0, 0, 0, 0},    /*au8FcrStrength[ISP_AUTO_ISO_STENGTH_NUM]*/
	{24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,24},    /*au8FcrThreshold[ISP_AUTO_ISO_STENGTH_NUM]*/
	/*For Ahd*/
	400, /*u16UuSlope*/	
	{512,512,512,512,512,512,512,  400,  0,0,0,0,0,0,0,0}    /*au16NpOffset[ISP_AUTO_ISO_STENGTH_NUM]*/
};

static ISP_CMOS_GE_S g_stIspGe =
{
	/*For GE*/
	0,    /*bEnable*/			
	7,    /*u8Slope*/	
	7,    /*u8Sensitivity*/
	4096, /*u16Threshold*/
	4096, /*u16SensiThreshold*/	
	{1024,1024,1024,2048,2048,2048,2048,  2048,  2048,2048,2048,2048,2048,2048,2048,2048}    /*au16Strength[ISP_AUTO_ISO_STENGTH_NUM]*/	
};

static ISP_CMOS_RGBSHARPEN_S g_stIspRgbSharpen =
{      
    {   0,   0,  0,  0,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},/* bEnLowLumaShoot */
    {  40,  40, 40, 43, 45, 48, 50, 55, 58, 60, 60, 60, 60, 60, 60, 65},/*SharpenUD*/
    {  58,  58, 60, 60, 63, 63, 65, 65, 68, 70, 70, 70, 70, 70, 70, 70},/*SharpenD*/
    {   0,   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},/*TextureNoiseThd*/
    {   0,   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},/*EdgeNoiseThd*/
    { 100,  85, 80, 75, 70, 60, 55, 50, 48, 45, 45, 45, 45, 45, 45, 45},/*overshoot*/
    { 100, 100, 95, 95, 85, 80, 75, 70, 65, 60, 60, 60, 60, 60, 60, 60},/*undershoot*/

};


static ISP_CMOS_UVNR_S g_stIspUVNR = 
{
  //{100,	200,	400,	800,	1600,	3200,	6400,	12800,	25600,	51200,	102400,	204800,	409600,	819200,	1638400,	3276800};
	{1,	    2,       4,      5,      7,      48,     32,     16,     16,     16,      16,     16,     16,     16,     16,        16},      /*UVNRThreshold*/
 	{0,		0,		0,		0,		0,		0,		0,		0,		0,		1,			1,		2,		2,		2,		2,		2},  /*Coring_lutLimit*/
	{0,		0,		0,		16,		34,		34,		34,		34,		34,		34,		34,		34,		34,		34,		34,			34}  /*UVNR_blendRatio*/
};

static ISP_CMOS_DPC_S g_stCmosDpc = 
{
	//1,/*IR_channel*/
	//1,/*IR_position*/
	{45,110,152,204,220,224,224,224,224,224,224,224,224,224,224,224},/*au16Strength[16]*/
	{0,0,0,0,0,0,0,0,0x24,0x80,0x80,0x80,0xE5,0xE5,0xE5,0xE5},/*au16BlendRatio[16]*/
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
  //  pstDef->stNoiseTbl.u8SensorIndex = HI_ISP_NR_SENSOR_INDEX_OV9712;
    pstDef->stNoiseTbl.stNrCaliPara.u8CalicoefRow = DMNR_CALIB_CARVE_NUM_OV9732;
    pstDef->stNoiseTbl.stNrCaliPara.pCalibcoef    = (HI_FLOAT (*)[4])g_coef_calib_ov9732;

    memcpy(&pstDef->stNoiseTbl.stIsoParaTable[0], &g_stNrIsoParaTab[0],sizeof(ISP_NR_ISO_PARA_TABLE_S)*HI_ISP_NR_ISO_LEVEL_MAX);

    memcpy(&pstDef->stUvnr,       &g_stIspUVNR,       sizeof(ISP_CMOS_UVNR_S));
    memcpy(&pstDef->stDpc,       &g_stCmosDpc,       sizeof(ISP_CMOS_DPC_S));

    pstDef->stSensorMaxResolution.u32MaxWidth  = 1280;
    pstDef->stSensorMaxResolution.u32MaxHeight = 720;

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
    pstBlackLevel->au16BlackLevel[1] = 58;
    pstBlackLevel->au16BlackLevel[2] = 58;
    pstBlackLevel->au16BlackLevel[3] = 64;
    

    return 0;  
    
}

HI_VOID cmos_set_pixel_detect(HI_BOOL bEnable)
{
    HI_U32 u32FullLines_5Fps = 0; 
    HI_U32 u32MaxIntTime_5Fps = 0;

    if (SENSOR_720P_30FPS_MODE == gu8SensorImageMode)
    {
        u32FullLines_5Fps = (VMAX_OV9732_720P30_LINEAR * 30) / 5;
    }
    else
    {
        return;
    }

    u32FullLines_5Fps = (u32FullLines_5Fps > FULL_LINES_MAX) ? FULL_LINES_MAX : u32FullLines_5Fps;
    u32MaxIntTime_5Fps = u32FullLines_5Fps - 2;

    if (bEnable) /* setup for ISP pixel calibration mode */
    {
        sensor_write_register(0x380e, (u32FullLines_5Fps & 0xFF00) >> 8);  /* 5fps */
        sensor_write_register(0x380f, u32FullLines_5Fps & 0xFF);           /* 5fps */
        sensor_write_register(0x3500, (u32MaxIntTime_5Fps >> 12));               /* max exposure lines */
        sensor_write_register(0x3500+1, ((u32MaxIntTime_5Fps & 0xFFF)>>4));     /* max exposure lines */
        sensor_write_register(0x3500+2, ((u32MaxIntTime_5Fps & 0xF)<<4));       /* max exposure lines */
        sensor_write_register(0x350a, 0x00);                                    /* min AG */
        sensor_write_register(0x350a+1, 0x10);                                    /* min AG */
    }
    else /* setup for ISP 'normal mode' */
    {
        sensor_write_register(0x380e, (gu32FullLinesStd & 0xFF00) >> 8);
        sensor_write_register(0x380f, gu32FullLinesStd & 0xFF);
        
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
                gu32FullLinesStd = VMAX_OV9732_720P30_LINEAR;
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
        g_stSnsRegsInfo.u32RegNum = 12;
	
        for (i=0; i<g_stSnsRegsInfo.u32RegNum; i++)
        {	
            g_stSnsRegsInfo.astI2cData[i].bUpdate = HI_TRUE;
            g_stSnsRegsInfo.astI2cData[i].u8DevAddr = sensor_i2c_addr;
            g_stSnsRegsInfo.astI2cData[i].u32AddrByteNum = sensor_addr_byte;
            g_stSnsRegsInfo.astI2cData[i].u32DataByteNum = sensor_data_byte;
        }

        g_stSnsRegsInfo.astI2cData[0].u8DelayFrmNum = 0;         //exposure time: astI2cData[0:2]
        g_stSnsRegsInfo.astI2cData[0].u32RegAddr = 0x3500;
        g_stSnsRegsInfo.astI2cData[1].u8DelayFrmNum = 0;
        g_stSnsRegsInfo.astI2cData[1].u32RegAddr = 0x3501;
        g_stSnsRegsInfo.astI2cData[2].u8DelayFrmNum = 0;
        g_stSnsRegsInfo.astI2cData[2].u32RegAddr = 0x3502;

        g_stSnsRegsInfo.astI2cData[3].u8DelayFrmNum = 0;       //gain
        g_stSnsRegsInfo.astI2cData[3].u32RegAddr = 0x350b;
 
        /* digital gain */
        g_stSnsRegsInfo.astI2cData[4].u8DelayFrmNum = 1;         //gain
        g_stSnsRegsInfo.astI2cData[4].u32RegAddr = 0x5180;
        g_stSnsRegsInfo.astI2cData[5].u8DelayFrmNum = 1;
        g_stSnsRegsInfo.astI2cData[5].u32RegAddr = 0x5181;

        g_stSnsRegsInfo.astI2cData[6].u8DelayFrmNum = 1;         //gain
        g_stSnsRegsInfo.astI2cData[6].u32RegAddr = 0x5182;
        g_stSnsRegsInfo.astI2cData[7].u8DelayFrmNum = 1;
        g_stSnsRegsInfo.astI2cData[7].u32RegAddr = 0x5183;

        g_stSnsRegsInfo.astI2cData[8].u8DelayFrmNum = 1;         //gain
        g_stSnsRegsInfo.astI2cData[8].u32RegAddr = 0x5184;
        g_stSnsRegsInfo.astI2cData[9].u8DelayFrmNum = 1;
        g_stSnsRegsInfo.astI2cData[9].u32RegAddr = 0x5185;

        g_stSnsRegsInfo.astI2cData[10].u8DelayFrmNum = 0;       //VTS
        g_stSnsRegsInfo.astI2cData[10].u32RegAddr = 0x380e;
        g_stSnsRegsInfo.astI2cData[11].u8DelayFrmNum = 0;       
        g_stSnsRegsInfo.astI2cData[11].u32RegAddr = 0x380f;

        bInit = HI_TRUE;
    }
    else    
    {        
        for (i = 0; i < g_stSnsRegsInfo.u32RegNum; i++)        
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
    gu32FullLinesStd = VMAX_OV9732_720P30_LINEAR; 
    gu32FullLines = VMAX_OV9732_720P30_LINEAR;
    bInit = HI_FALSE;
    bSensorInit = HI_FALSE; 

    memset(&g_stSnsRegsInfo, 0, sizeof(ISP_SNS_REGS_INFO_S));
    memset(&g_stPreSnsRegsInfo, 0, sizeof(ISP_SNS_REGS_INFO_S));
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
    s32Ret = HI_MPI_ISP_SensorRegCallBack(IspDev, OV9732_ID, &stIspRegister);
    if (s32Ret)
    {
        printf("sensor register callback function failed!\n");
        return s32Ret;
    }
    
    stLib.s32Id = 0;
    strncpy(stLib.acLibName, HI_AE_LIB_NAME, sizeof(HI_AE_LIB_NAME));
    cmos_init_ae_exp_function(&stAeRegister.stSnsExp);
    s32Ret = HI_MPI_AE_SensorRegCallBack(IspDev, &stLib, OV9732_ID, &stAeRegister);
    if (s32Ret)
    {
        printf("sensor register callback function to ae lib failed!\n");
        return s32Ret;
    }

    stLib.s32Id = 0;
    strncpy(stLib.acLibName, HI_AWB_LIB_NAME, sizeof(HI_AWB_LIB_NAME));
    cmos_init_awb_exp_function(&stAwbRegister.stSnsExp);
    s32Ret = HI_MPI_AWB_SensorRegCallBack(IspDev, &stLib, OV9732_ID, &stAwbRegister);
    if (s32Ret)
    {
        printf("sensor register callback function to ae lib failed!\n");
        return s32Ret;
    }
    
    return 0;
}

int sensor_unregister_callback(void)
{
    ISP_DEV IspDev = 0;
    HI_S32 s32Ret;
    ALG_LIB_S stLib;

    s32Ret = HI_MPI_ISP_SensorUnRegCallBack(IspDev, OV9732_ID);
    if (s32Ret)
    {
        printf("sensor unregister callback function failed!\n");
        return s32Ret;
    }
    
    stLib.s32Id = 0;
    strncpy(stLib.acLibName, HI_AE_LIB_NAME, sizeof(HI_AE_LIB_NAME));
    s32Ret = HI_MPI_AE_SensorUnRegCallBack(IspDev, &stLib, OV9732_ID);
    if (s32Ret)
    {
        printf("sensor unregister callback function to ae lib failed!\n");
        return s32Ret;
    }

    stLib.s32Id = 0;
    strncpy(stLib.acLibName, HI_AWB_LIB_NAME, sizeof(HI_AWB_LIB_NAME));
    s32Ret = HI_MPI_AWB_SensorUnRegCallBack(IspDev, &stLib, OV9732_ID);
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

#endif /* __MN34220_CMOS_H_ */
