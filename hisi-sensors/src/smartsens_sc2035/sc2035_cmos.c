#if !defined(__SC2035_CMOS_H_)
#define __SC2035_CMOS_H_

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


#define SC2035_ID 2035

#define CMOS_SC2035_ISP_WRITE_SENSOR_ENABLE (1)
#define FULL_LINES_MAX  (0xFFFF)


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

#define VMAX_ADDR_H              (0x320e)
#define VMAX_ADDR_L              (0x320f)

#define SENSOR_1080P_30FPS_MODE  (1)

#define INCREASE_LINES (0) /* make real fps less than stand fps because NVR require*/
#define VMAX_1080P30_LINEAR     (1125+INCREASE_LINES)
#define CMOS_OV2035_SLOW_FRAMERATE_MODE (0)



HI_U8 gu8SensorImageMode = SENSOR_1080P_30FPS_MODE;
WDR_MODE_E genSensorMode = WDR_MODE_NONE;

static HI_U32 gu32FullLinesStd = VMAX_1080P30_LINEAR;
static HI_U32 gu32FullLines = VMAX_1080P30_LINEAR;

static HI_BOOL bInit = HI_FALSE;
HI_BOOL bSensorInit = HI_FALSE;
ISP_SNS_REGS_INFO_S g_stSnsRegsInfo = {0};
ISP_SNS_REGS_INFO_S g_stPreSnsRegsInfo = {0};
static HI_U8 gu8Fps = 30;


/* AE default parameter and function */
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

    pstAeSnsDft->au8HistThresh[0] = 0xd;
    pstAeSnsDft->au8HistThresh[1] = 0x28;
    pstAeSnsDft->au8HistThresh[2] = 0x60;
    pstAeSnsDft->au8HistThresh[3] = 0x80;
            
    pstAeSnsDft->u8AeCompensation = 0x2b;

    pstAeSnsDft->stIntTimeAccu.enAccuType = AE_ACCURACY_LINEAR;
    pstAeSnsDft->stIntTimeAccu.f32Accuracy = 1;
    pstAeSnsDft->stIntTimeAccu.f32Offset = 0;
    pstAeSnsDft->u32MaxIntTime = gu32FullLinesStd - 4;
    pstAeSnsDft->u32MinIntTime = 1;
    pstAeSnsDft->u32MaxIntTimeTarget = pstAeSnsDft->u32MaxIntTime;
    pstAeSnsDft->u32MinIntTimeTarget = pstAeSnsDft->u32MinIntTime;

    pstAeSnsDft->stAgainAccu.enAccuType = AE_ACCURACY_LINEAR;
    pstAeSnsDft->stAgainAccu.f32Accuracy = 0.0625; 
    pstAeSnsDft->u32MaxAgain = 992;  //62±¶ 
    pstAeSnsDft->u32MinAgain = 16;
    pstAeSnsDft->u32MaxAgainTarget = 992;
    pstAeSnsDft->u32MinAgainTarget = 16;

    pstAeSnsDft->stDgainAccu.enAccuType = AE_ACCURACY_LINEAR;
    pstAeSnsDft->stDgainAccu.f32Accuracy = 0.0625;//invalidate
    pstAeSnsDft->u32MaxDgain = 16;  
    pstAeSnsDft->u32MinDgain = 16;
    pstAeSnsDft->u32MaxDgainTarget = 16;
    pstAeSnsDft->u32MinDgainTarget = 16; 
	
    pstAeSnsDft->u32ISPDgainShift = 8;
    pstAeSnsDft->u32MinISPDgainTarget = 1 << pstAeSnsDft->u32ISPDgainShift;
    pstAeSnsDft->u32MaxISPDgainTarget = 4 << pstAeSnsDft->u32ISPDgainShift; 
	  
    pstAeSnsDft->u32LinesPer500ms = gu32FullLinesStd*30/2;

    return 0;
}

/* the function of sensor set fps */
static HI_VOID cmos_fps_set(HI_FLOAT f32Fps, AE_SENSOR_DEFAULT_S *pstAeSnsDft)
{

    HI_U32 u32VblankingLines = 0xFFFF;
    if ((f32Fps <= 30) && (f32Fps >= 0.5))
    {
        if(SENSOR_1080P_30FPS_MODE == gu8SensorImageMode)
        {
            u32VblankingLines = VMAX_1080P30_LINEAR * 30 / f32Fps;
        }
    }
    else
    {
        printf("Not support Fps: %f\n", f32Fps);
        return;
    }

    gu32FullLinesStd = gu32FullLinesStd > FULL_LINES_MAX ? FULL_LINES_MAX : gu32FullLinesStd;

#if CMOS_SC2035_ISP_WRITE_SENSOR_ENABLE
    g_stSnsRegsInfo.astI2cData[4].u32Data = (u32VblankingLines >> 8) & 0xFF ;
	g_stSnsRegsInfo.astI2cData[5].u32Data = u32VblankingLines & 0xFF;
#else
    sensor_write_register(0x320e, (u32VblankingLines >> 8) & 0xff) ;
    sensor_write_register(0x320f, u32VblankingLines & 0xff);
#endif

    pstAeSnsDft->f32Fps = f32Fps;
    pstAeSnsDft->u32MaxIntTime = u32VblankingLines - 4;
    gu32FullLinesStd = u32VblankingLines;
    gu8Fps = f32Fps;
    pstAeSnsDft->u32LinesPer500ms = gu32FullLinesStd * 30 / 2;
    pstAeSnsDft->u32FullLinesStd = gu32FullLinesStd;

    return;
}


static HI_VOID cmos_slow_framerate_set(HI_U32 u32FullLines,
    AE_SENSOR_DEFAULT_S *pstAeSnsDft)
{
    u32FullLines = (u32FullLines > 0xFFFF) ? 0xFFFF : u32FullLines;
    gu32FullLines = u32FullLines;

    u32FullLines = (u32FullLines > FULL_LINES_MAX) ? FULL_LINES_MAX : u32FullLines;

#if CMOS_SC2035_ISP_WRITE_SENSOR_ENABLE
	g_stSnsRegsInfo.astI2cData[4].u32Data = (u32FullLines >> 8) & 0xFF;
	g_stSnsRegsInfo.astI2cData[5].u32Data = u32FullLines & 0xFf;
#else
	sensor_write_register(0x320e, ((u32FullLines >> 8) & 0xFF));
	sensor_write_register(0x320f, u32FullLines & 0xFf);
#endif

    pstAeSnsDft->u32MaxIntTime = gu32FullLines - 4;

#if 1
    sensor_write_register(0x336a, ((u32FullLines >> 8) & 0xFF));
    sensor_write_register(0x336b, u32FullLines & 0xFf);

    sensor_write_register(0x3368, (((u32FullLines - 0x265) >> 8) & 0xFF));
    sensor_write_register(0x3369, (u32FullLines - 0x265) & 0xFf);
#endif
    return;
}

/* while isp notify ae to update sensor regs, ae call these funcs. */
static HI_U32 u32OldIntTime = 0;
static HI_VOID cmos_inttime_update(HI_U32 u32IntTime)
{
#if 1
if (u32OldIntTime != u32IntTime)
{
	printf("u32IntTime:%#x...........%#x........%#x..\n",u32IntTime, (u32IntTime >> 4) & 0xFF,(u32IntTime<<4) & 0xF0);
	u32OldIntTime = u32IntTime;
}
#endif

#if CMOS_SC2035_ISP_WRITE_SENSOR_ENABLE
    g_stSnsRegsInfo.astI2cData[0].u32Data = (u32IntTime >> 4) & 0xFF; 
    g_stSnsRegsInfo.astI2cData[1].u32Data = (u32IntTime<<4) & 0xF0;
#else
	sensor_write_register(0x3e01, ((u32IntTime >> 4) & 0xFF));
	sensor_write_register(0x3e02, (u32IntTime<<4) & 0xF0);
#endif

    return;
}


static HI_VOID cmos_gains_update(HI_U32 u32Again, HI_U32 u32Dgain)
{
 // 20151222

    HI_U32 u32TotalAgain = 0, u32RegValul = 0;
	u32TotalAgain = u32Again;
//	printf("%s......%d......%#x.....%#x......%d.....%#x.....\n",__FUNCTION__,__LINE__, u32Again, u32Dgain, u32TotalAgain, u32RegValul);

#if CMOS_SC2035_ISP_WRITE_SENSOR_ENABLE
    g_stSnsRegsInfo.astI2cData[2].u32Data = (u32Again & 0xF00) >> 8;
	g_stSnsRegsInfo.astI2cData[3].u32Data = u32Again & 0xFF;
#else
    sensor_write_register(0x3e08, (u32Again & 0xF00) >> 8);
    sensor_write_register(0x3e09, u32Again & 0xFF);
#endif	

//½µÔëÂß¼­
#if 1
    if(u32Again <= 0x20)
        sensor_write_register(0x3630, 0xb4);
    else
        sensor_write_register(0x3630, 0x94);

	if(u32Again <= 0x20)
        sensor_write_register(0x3635, 0x6c);
    else if(u32Again <= 0x40)
        sensor_write_register(0x3635, 0x6a);
	else if(u32Again <= 0x80)
		sensor_write_register(0x3635, 0x68);
	else if(u32Again <= 0x100)
		sensor_write_register(0x3635, 0x66);
	else 
		sensor_write_register(0x3635, 0x64);
		
#endif

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
    pstExpFuncs->pfn_cmos_again_calc_table  = NULL;//cmos_again_calc_table;
    pstExpFuncs->pfn_cmos_dgain_calc_table  = NULL;//cmos_dgain_calc_table;

    return 0;
}


/* AWB default parameter and function */
static AWB_CCM_S g_stAwbCcm =
{  
	4850,	 
	{		
		0x01ED,  0x80C3,  0x802A,
		0x806E,  0x01E4,  0x8076,		
		0x801D,  0x8198,  0x02B5	
	},	

	3160,	 
	{		
		0x021E,  0x80dF,  0x803F,		
		0x8075,  0x01A1,  0x802C,		
		0x800D,  0x826C,  0x0379	
	},

	2470,	 
	{			 
		0x023C,  0x8122,  0x801A,		 
		0x804D,  0x01E5,  0x8098,		
		0x0013,  0x8173,  0x0260	
	} 

};

static AWB_AGC_TABLE_S g_stAwbAgcTable =
{
    /* bvalid */
    1,
	
    /*1,  2,  4,  8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384, 32768*/
    /* saturation */   
    //{0x7a,0x7a,0x78,0x74,0x68,0x60,0x58,0x50,0x48,0x40,0x38,0x38,0x38,0x38,0x38,0x38}
    //{0x8a,0x8a,0x88,0x84,0x78,0x70,0x68,0x60,0x58,0x50,0x48,0x48,0x48,0x48,0x48,0x48}
	{0x90,0x90,0x80,0x80,0x70,0x70,0x50,0x50,0x50,0x50,0x50,0x50,0x50,0x50,0x50,0x50}//3.23 wangji

};

static HI_S32 cmos_get_awb_default(AWB_SENSOR_DEFAULT_S *pstAwbSnsDft)
{
    if (HI_NULL == pstAwbSnsDft)
    {
        printf("null pointer when get awb default value!\n");
        return -1;
    }

    memset(pstAwbSnsDft, 0, sizeof(AWB_SENSOR_DEFAULT_S));

    pstAwbSnsDft->u16WbRefTemp = 4850;
    pstAwbSnsDft->au16GainOffset[0] = 0x18b;    
    pstAwbSnsDft->au16GainOffset[1] = 0x100;    
    pstAwbSnsDft->au16GainOffset[2] = 0x100;    
    pstAwbSnsDft->au16GainOffset[3] = 0x167;    
    pstAwbSnsDft->as32WbPara[0] = 142;    
    pstAwbSnsDft->as32WbPara[1] = -34;    
    pstAwbSnsDft->as32WbPara[2] = -148;    
    pstAwbSnsDft->as32WbPara[3] = 134479;    
    pstAwbSnsDft->as32WbPara[4] = 128;    
    pstAwbSnsDft->as32WbPara[5] = -80717;
    
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

#define DMNR_CALIB_CARVE_NUM_SC2035 7

float g_coef_calib_sc2035[DMNR_CALIB_CARVE_NUM_SC2035][4] = 
{
    {100.000000f, 2.000000f, 0.039263f, 6.146605f, }, 
    {200.000000f, 2.301030f, 0.040718f, 6.193045f, }, 
    {400.000000f, 2.602060f, 0.042503f, 6.561516f, }, 
    {800.000000f, 2.903090f, 0.044908f, 7.221792f, }, 
    {1600.000000f, 3.204120f, 0.049715f, 8.224653f, }, 
    {3100.000000f, 3.491362f, 0.057821f, 9.768325f, }, 
    {6200.000000f, 3.792392f, 0.068945f, 13.604248f, }, 
};


static ISP_NR_ISO_PARA_TABLE_S g_stNrIsoParaTab[HI_ISP_NR_ISO_LEVEL_MAX] = 
{
     //u16Threshold//u8varStrength//u8fixStrength//u8LowFreqSlope	
       {1500,       160,             256-256,            0 },  //100    //                      //                                                
       {1500,       120,             256-256,            0 },  //200    // ISO                  // ISO //u8LowFreqSlope
       {1500,       100,             256-256,            0 },  //400    //{400,  1200, 96,256}, //{400 , 0  }
       {1750,       80,              256-256,            8 },  //800    //{800,  1400, 80,256}, //{600 , 2  }
       {1500,       255,             256-256,            6 },  //1600   //{1600, 1200, 72,256}, //{800 , 8  }
       {1500,       255,             256-256,            0 },  //3200   //{3200, 1200, 64,256}, //{1000, 12 }
       {1375,       255,             256-256,            0 },  //6400   //{6400, 1100, 56,256}, //{1600, 6  }
       {1375,       255,             256-256,            0 },  //12800  //{12000,1100, 48,256}, //{2400, 0  }
       {1375,       255,             256-256,            0 },  //25600  //{36000,1100, 48,256}, //
       {1375,       255,             256-256,            0 },  //51200  //{64000,1100, 96,256}, //
       {1250,       255,             256-256,            0 },  //102400 //{82000,1000,240,256}, //
       {1250,       255,             256-256,            0 },  //204800 //                           //
       {1250,       255,             256-256,            0 },  //409600 //                           //
       {1250,       255,             256-256,            0 },  //819200 //                           //
       {1250,       255,             256-256,            0 },  //1638400//                           //
       {1250,       255,             256-256,            0 },  //3276800//                           //
};

static ISP_CMOS_DEMOSAIC_S g_stIspDemosaic =
{
	/*For Demosaic*/
	1, /*bEnable*/			
	24,/*u16VhLimit*/	
	40-24,/*u16VhOffset*/
	24,   /*u16VhSlope*/
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
  //{100,200,400,800,1600,3200,6400,12800,25600,51200,102400,204800,409600,819200,1638400,3276800};
    {0,	  0,   0,  0,   0,   0,   0,    0,    0,    1,    1,     1,     1,     1,     1,       1},/* enPixSel */
    //{40, 45,  45, 50,  50,  55,  55,   60,   60,   70,   80,    90,   110,   120,   120,     120},/*SharpenUD*/
    //{20, 20,  30, 45,  30,  35,  35,   40,   50,   60,   70,    90,   110,   120,   120,     120},/*SharpenD*/
    {0xf0, 0x70, 0x68, 0x60, 0x50, 0x40, 0x20, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},/*SharpenUD*/ //3.23 wangji
    {0x80, 0x70, 0x68, 0x60, 0x50, 0x40, 0x20, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},/*SharpenD*/  //3.23 wangji
    //{0,   2,   4,  6,   6,  12,  30,   60,   80,    0,    0,     0,    0,     0,     0,        0},/*NoiseThd*/
    //{2,   4,   8, 16,  25,  11,  12,    0,    0,    0,    0,     0,    0,     0,     0,        0},/*EdgeThd2*/
    //{220,230, 200,175, 150, 120, 110,  95,   80,   70,   40,    20,   20,    20,    20,       20},/*overshootAmt*/
    //{210,220, 190,140, 135, 130, 110,  95,   75,   60,   50,    50,   50,    50,    50,       50},/*undershootAmt*/
    
    {0x10, 0x18, 0x20, 0x30, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},/*NoiseThd*/
    {0x18, 0x2f, 0x18, 0x09, 0x04, 0x07, 0x07, 0x07, 0x8c, 0x8c, 0x8c, 0x8c, 0x8c, 0x8c, 0x8c, 0x8c},/*EdgeThd2*/
  	{0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},/*overshootAmt*/ //3.23 wangji
  	{0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},/*undershootAmt*///3.23 wangji
};

static ISP_CMOS_UVNR_S g_stIspUVNR = 
{
   //{100,	200, 400, 800, 1600, 3200, 6400, 12800,	25600, 51200, 102400, 204800, 409600, 819200, 1638400, 3276800};
	  {1,	  2,   4,   5,    7,   10,   12,    16,    18,    20,     22,     24,     24,     24,      24,      24},  /*UVNRThreshold*/
 	  {0,	  0,   0,   0,	  0, 	0,    0,     0,     0,	   1,      1,      2,      2,      2,       2,       2},  /*Coring_lutLimit*/
      {0,	  0,   0,  16,   34,   34,   34,    34,    34,    34,     34,     34,     34,     34,      34,      34}   /*UVNR_blendRatio*/
};

static ISP_CMOS_DPC_S g_stCmosDpc = 
{
	//0,/*IR_channel*/
	//0,/*IR_position*/
	{70,150,240,248,250,252,252,252,252,252,252,252,252,252,252,252},/*au16Strength[16]*/
	{0,0,0,0,0,0,0,0x30,0x60,0x80,0x80,0x80,0xE5,0xE5,0xE5,0xE5},/*au16BlendRatio[16]*/
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
    memcpy(&pstDef->stGe, &g_stIspGe, sizeof(ISP_CMOS_GE_S));	

    pstDef->stNoiseTbl.stNrCaliPara.u8CalicoefRow = DMNR_CALIB_CARVE_NUM_SC2035;
    pstDef->stNoiseTbl.stNrCaliPara.pCalibcoef    = (HI_FLOAT (*)[4])g_coef_calib_sc2035;
	memcpy(&pstDef->stNoiseTbl.stIsoParaTable[0], &g_stNrIsoParaTab[0],sizeof(ISP_NR_ISO_PARA_TABLE_S)*HI_ISP_NR_ISO_LEVEL_MAX);
	
    memcpy(&pstDef->stRgbSharpen, &g_stIspRgbSharpen, sizeof(ISP_CMOS_RGBSHARPEN_S));
	memcpy(&pstDef->stUvnr,       &g_stIspUVNR,       sizeof(ISP_CMOS_UVNR_S));
	memcpy(&pstDef->stDpc,       &g_stCmosDpc,       sizeof(ISP_CMOS_DPC_S));

    pstDef->stSensorMaxResolution.u32MaxWidth  = 1920;
    pstDef->stSensorMaxResolution.u32MaxHeight = 1080;

    return 0;
}


HI_U32 cmos_get_isp_black_level(ISP_CMOS_BLACK_LEVEL_S *pstBlackLevel)
{
   // HI_S32  i;
    
    if (HI_NULL == pstBlackLevel)
    {
        printf("null pointer when get isp black level value!\n");
        return -1;
    }

    /* Don't need to update black level when iso change */
    pstBlackLevel->bUpdate = HI_FALSE;
          
    pstBlackLevel->au16BlackLevel[0] = 65;
    pstBlackLevel->au16BlackLevel[1] = 65;
    pstBlackLevel->au16BlackLevel[2] = 65;
    pstBlackLevel->au16BlackLevel[3] = 65;

    return 0;  
    
}

HI_VOID cmos_set_pixel_detect(HI_BOOL bEnable)
{
	HI_U32 u32Lines = VMAX_1080P30_LINEAR * 30 /5;
	
#if CMOS_SC2035_ISP_WRITE_SENSOR_ENABLE
    if (bEnable) /* setup for ISP pixel calibration mode */
    {
        /* 5 fps */
		sensor_write_register(0x320e, (u32Lines >> 4) && 0xFF);
		sensor_write_register(0x320f, ((u32Lines<<4)&&0xF0));
       
        /* max exposure time*/
		

    }
    else /* setup for ISP 'normal mode' */
    { 
        sensor_write_register(0x320e, (gu32FullLinesStd >> 8) && 0XFF);
        sensor_write_register(0x320f, gu32FullLinesStd && 0xFF);
        
        bInit = HI_FALSE;
    }
#else
    if (bEnable) /* setup for ISP pixel calibration mode */
    {
        
		sensor_write_register(0x3e01, (u32Lines >> 8) && 0xFF);
		sensor_write_register(0x3e02, (u32Lines - 4) && 0xFF);
        
        /* min gain */
        sensor_write_register(0x3e0e, 0x00);
		sensor_write_register(0x3e0f, 0x00);

		/* 5 fps */
        sensor_write_register(0x320e, (u32Lines >> 8) && 0xFF);
        sensor_write_register(0x320f, u32Lines && 0xFF);
    }
    else /* setup for ISP 'normal mode' */
    { 
        sensor_write_register(0x320e, (gu32FullLinesStd >> 8) && 0XFF);
        sensor_write_register(0x320f, gu32FullLinesStd && 0xFF);
        
        bInit = HI_FALSE;
    }
#endif

    return;
}

HI_VOID cmos_set_wdr_mode(HI_U8 u8Mode)
{
    bInit = HI_FALSE;
    
    switch(u8Mode)
    {
        case WDR_MODE_NONE:
            if (SENSOR_1080P_30FPS_MODE == gu8SensorImageMode)
            {
                gu32FullLinesStd = VMAX_1080P30_LINEAR;
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

#if CMOS_SC2035_ISP_WRITE_SENSOR_ENABLE

    HI_S32 i;

    if (HI_FALSE == bInit)
    {
        g_stSnsRegsInfo.enSnsType = ISP_SNS_I2C_TYPE;
        g_stSnsRegsInfo.u8Cfg2ValidDelayMax = 2;		
        g_stSnsRegsInfo.u32RegNum = 6;
	
        for (i=0; i<g_stSnsRegsInfo.u32RegNum; i++)
        {	
            g_stSnsRegsInfo.astI2cData[i].bUpdate = HI_TRUE;
            g_stSnsRegsInfo.astI2cData[i].u8DevAddr = sensor_i2c_addr;
            g_stSnsRegsInfo.astI2cData[i].u32AddrByteNum = sensor_addr_byte;
            g_stSnsRegsInfo.astI2cData[i].u32DataByteNum = sensor_data_byte;
        }		
        g_stSnsRegsInfo.astI2cData[0].u8DelayFrmNum = 0;
        g_stSnsRegsInfo.astI2cData[0].u32RegAddr = 0x3e01;     //exp high  bit[7:0] 
        g_stSnsRegsInfo.astI2cData[1].u8DelayFrmNum = 0;
        g_stSnsRegsInfo.astI2cData[1].u32RegAddr = 0x3e02;     //exp low  bit[7:4] 
        g_stSnsRegsInfo.astI2cData[2].u8DelayFrmNum = 0;
        g_stSnsRegsInfo.astI2cData[2].u32RegAddr = 0x3e08;     //digita agin[6:5];    coarse analog again[4:2]
		g_stSnsRegsInfo.astI2cData[3].u8DelayFrmNum = 0;
        g_stSnsRegsInfo.astI2cData[3].u32RegAddr = 0x3e09;     //fine analog again[4:0]

		g_stSnsRegsInfo.astI2cData[4].u8DelayFrmNum = 0;
        g_stSnsRegsInfo.astI2cData[4].u32RegAddr = 0x320e;     //TIMING_VTS  high bit[7:0] 
		g_stSnsRegsInfo.astI2cData[5].u8DelayFrmNum = 0;
        g_stSnsRegsInfo.astI2cData[5].u32RegAddr = 0x320f;     //TIMING_VTS  low bit[7:0] 

	
        bInit = HI_TRUE;
    }
    else    
    {        
        for (i=0; i<g_stSnsRegsInfo.u32RegNum; i++)        
        {            
            if (g_stSnsRegsInfo.astI2cData[i].u32Data == g_stPreSnsRegsInfo.astI2cData[i].u32Data)            
            {                
               // printf("HI_FALSE....\n");
                g_stSnsRegsInfo.astI2cData[i].bUpdate = HI_TRUE;
            }            
            else            
	        {
	             if(i == 0)
	             {
					printf("i:%d..g_stSnsRegsInfo:%#x  g_stPreSnsRegsInfo:%#x.....\n",i,g_stSnsRegsInfo.astI2cData[i].u32Data, g_stPreSnsRegsInfo.astI2cData[i].u32Data);
	                printf("#####################HI_TRUE....\n");       
	             }
                g_stSnsRegsInfo.astI2cData[i].bUpdate = HI_TRUE;
            }        
        }    
    }

    if (HI_NULL == pstSnsRegsInfo)
    {
        printf("null pointer when get sns reg info!\n");
        return -1;
    }

//printf(".%#X..%#X.\n",g_stSnsRegsInfo.astI2cData[0].u32Data,g_stPreSnsRegsInfo.astI2cData[0].u32Data);
    memcpy(pstSnsRegsInfo, &g_stSnsRegsInfo, sizeof(ISP_SNS_REGS_INFO_S)); 
    memcpy(&g_stPreSnsRegsInfo, &g_stSnsRegsInfo, sizeof(ISP_SNS_REGS_INFO_S)); 
#endif
    return 0;
}

static HI_S32 cmos_set_image_mode(ISP_CMOS_SENSOR_IMAGE_MODE_S *pstSensorImageMode)
{
    HI_U8 u8SensorImageMode = gu8SensorImageMode;

    bInit = HI_FALSE;
 printf("%s.............................%d....\n",__FUNCTION__,__LINE__);   
    if (HI_NULL == pstSensorImageMode )
    {
        printf("null pointer when set image mode\n");
        return -1;
    }

    if ((pstSensorImageMode->u16Width <= 1920) && (pstSensorImageMode->u16Height <= 1080))
    {
        if (WDR_MODE_NONE == genSensorMode)
        {
            if (pstSensorImageMode->f32Fps <= 30)
            {
                u8SensorImageMode = SENSOR_1080P_30FPS_MODE;
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
	printf("%s.............................%d....\n",__FUNCTION__,__LINE__);   

    return 0;
}

HI_VOID sensor_global_init()
{   
    gu8SensorImageMode = SENSOR_1080P_30FPS_MODE;
    genSensorMode = WDR_MODE_NONE;
    gu32FullLinesStd = VMAX_1080P30_LINEAR; 
    gu32FullLines = VMAX_1080P30_LINEAR;
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
    s32Ret = HI_MPI_ISP_SensorRegCallBack(IspDev, SC2035_ID, &stIspRegister);
    if (s32Ret)
    {
        printf("sensor register callback function failed!\n");
        return s32Ret;
    }
    
    stLib.s32Id = 0;
    strncpy(stLib.acLibName, HI_AE_LIB_NAME, sizeof(HI_AE_LIB_NAME));
    cmos_init_ae_exp_function(&stAeRegister.stSnsExp);
    s32Ret = HI_MPI_AE_SensorRegCallBack(IspDev, &stLib, SC2035_ID, &stAeRegister);
    if (s32Ret)
    {
        printf("sensor register callback function to ae lib failed!\n");
        return s32Ret;
    }

    stLib.s32Id = 0;
    strncpy(stLib.acLibName, HI_AWB_LIB_NAME, sizeof(HI_AWB_LIB_NAME));
    cmos_init_awb_exp_function(&stAwbRegister.stSnsExp);
    s32Ret = HI_MPI_AWB_SensorRegCallBack(IspDev, &stLib, SC2035_ID, &stAwbRegister);
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

    s32Ret = HI_MPI_ISP_SensorUnRegCallBack(IspDev, SC2035_ID);
    if (s32Ret)
    {
        printf("sensor unregister callback function failed!\n");
        return s32Ret;
    }
    
    stLib.s32Id = 0;
    strncpy(stLib.acLibName, HI_AE_LIB_NAME, sizeof(HI_AE_LIB_NAME));
    s32Ret = HI_MPI_AE_SensorUnRegCallBack(IspDev, &stLib, SC2035_ID);
    if (s32Ret)
    {
        printf("sensor unregister callback function to ae lib failed!\n");
        return s32Ret;
    }

    stLib.s32Id = 0;
    strncpy(stLib.acLibName, HI_AWB_LIB_NAME, sizeof(HI_AWB_LIB_NAME));
    s32Ret = HI_MPI_AWB_SensorUnRegCallBack(IspDev, &stLib, SC2035_ID);
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

#endif /* __SC2035_CMOS_H_ */
