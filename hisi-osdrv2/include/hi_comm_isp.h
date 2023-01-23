/******************************************************************************

  Copyright (C), 2001-2011, Hisilicon Tech. Co., Ltd.

 ******************************************************************************
  File Name     : hi_comm_isp.h
  Version       : Initial Draft
  Author        : Hisilicon multimedia software group
  Created       : 2010/12/20
  Description   : 
  History       :
  1.Date        : 2010/12/20
    Author      : x00100808
    Modification: Created file

  2.Date         : 2014/04/24
    Author       : y00246723
    Modification : Add ACM structure define

******************************************************************************/

#ifndef __HI_COMM_ISP_H__
#define __HI_COMM_ISP_H__

#include "hi_type.h"
#include "hi_errno.h"
#include "hi_common.h"
#include "hi_isp_debug.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* End of #ifdef __cplusplus */


/****************************************************************************
 * MACRO DEFINITION                                                         *
 ****************************************************************************/
    
#define VREG_MAX_NUM         (16)

#define AE_ZONE_ROW          (15)
#define AE_ZONE_COLUMN       (17)
#define AWB_ZONE_ROW         (15)
#define AWB_ZONE_COLUMN      (17)
#define AF_ZONE_ROW          (15)
#define AF_ZONE_COLUMN       (17)
#define LIGHTSOURCE_NUM      (4)

#define STATIC_DP_COUNT_MAX  (2048)
#define GAMMA_NODE_NUM       (257)
#define GAMMA_FE0_NODE_NUM   (33)
#define GAMMA_FE1_NODE_NUM   (257)
#define SHADING_MESH_NUM (289)

#define DEFOG_ZONE_ROW       (16)
#define DEFOG_ZONE_COLUMN    (15)
#define DEFOG_ZONE_NUM       (240)
#define DEFOG_MAX_ZONE_NUM   (256)

#define ISP_AUTO_ISO_STENGTH_NUM (16)

#define ISP_MAX_SNS_REGS     (16)
#define ISP_MAX_DEV_NUM      (1)

#define ISP_CAC_MAX_LUT_SIZE (64)

#define RGBIR_MATRIX_NUM     (15)

#define AI_MAX_STEP_FNO_NUM  (1024)



/****************************************************************************
 * GENERAL STRUCTURES                                                       *
 ****************************************************************************/

typedef enum hiISP_ERR_CODE_E
{
    ERR_ISP_NOT_INIT                = 0x40,
    ERR_ISP_MEM_NOT_INIT            = 0x41,
    ERR_ISP_ATTR_NOT_CFG            = 0x42,
    ERR_ISP_SNS_UNREGISTER          = 0x43,
    ERR_ISP_INVALID_ADDR            = 0x44,
    ERR_ISP_NOMEM                   = 0x45,
    ERR_ISP_NO_INT                  = 0x46,
} ISP_ERR_CODE_E;

#define HI_ERR_ISP_NULL_PTR             HI_DEF_ERR(HI_ID_ISP, EN_ERR_LEVEL_ERROR, EN_ERR_NULL_PTR)
#define HI_ERR_ISP_ILLEGAL_PARAM        HI_DEF_ERR(HI_ID_ISP, EN_ERR_LEVEL_ERROR, EN_ERR_ILLEGAL_PARAM)
#define HI_ERR_ISP_NOT_SUPPORT          HI_DEF_ERR(HI_ID_ISP, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_SUPPORT)

#define HI_ERR_ISP_NOT_INIT             HI_DEF_ERR(HI_ID_ISP, EN_ERR_LEVEL_ERROR, ERR_ISP_NOT_INIT)
#define HI_ERR_ISP_MEM_NOT_INIT         HI_DEF_ERR(HI_ID_ISP, EN_ERR_LEVEL_ERROR, ERR_ISP_MEM_NOT_INIT)
#define HI_ERR_ISP_ATTR_NOT_CFG         HI_DEF_ERR(HI_ID_ISP, EN_ERR_LEVEL_ERROR, ERR_ISP_ATTR_NOT_CFG)
#define HI_ERR_ISP_SNS_UNREGISTER       HI_DEF_ERR(HI_ID_ISP, EN_ERR_LEVEL_ERROR, ERR_ISP_SNS_UNREGISTER)
#define HI_ERR_ISP_INVALID_ADDR         HI_DEF_ERR(HI_ID_ISP, EN_ERR_LEVEL_ERROR, ERR_ISP_INVALID_ADDR)
#define HI_ERR_ISP_NOMEM                HI_DEF_ERR(HI_ID_ISP, EN_ERR_LEVEL_ERROR, ERR_ISP_NOMEM)
#define HI_ERR_ISP_NO_INT               HI_DEF_ERR(HI_ID_ISP, EN_ERR_LEVEL_ERROR, ERR_ISP_NO_INT)


typedef enum hiISP_OP_TYPE_E
{
    OP_TYPE_AUTO    = 0,
    OP_TYPE_MANUAL  = 1,
    OP_TYPE_BUTT
} ISP_OP_TYPE_E;

typedef enum hiISP_STATE_E
{
    ISP_STATE_INIT     = 0, 
    ISP_STATE_SUCCESS  = 1,
    ISP_STATE_TIMEOUT  = 2,
    ISP_STATE_BUTT
} ISP_STATUS_E;

typedef enum hiISP_BAYER_FORMAT_E
{
    BAYER_RGGB    = 0,
    BAYER_GRBG    = 1,
    BAYER_GBRG    = 2,
    BAYER_BGGR    = 3,
    BAYER_BUTT    
} ISP_BAYER_FORMAT_E;

typedef struct hiISP_PUB_ATTR_S
{
    RECT_S          stWndRect;      /* RW. */
    HI_FLOAT        f32FrameRate;   /* RW. */
    ISP_BAYER_FORMAT_E  enBayer;    /* RW. */
} ISP_PUB_ATTR_S;

typedef enum hiISP_FMW_STATE_E
{
    ISP_FMW_STATE_RUN = 0,
    ISP_FMW_STATE_FREEZE,
    ISP_FMW_STATE_BUTT
} ISP_FMW_STATE_E;

typedef struct hiISP_WDR_MODE_S
{
    WDR_MODE_E  enWDRMode;
} ISP_WDR_MODE_S;

typedef union hiISP_MODULE_CTRL_U
{
    HI_U32  u32Key;
    struct
    {
        HI_U32  bitBypassVideoTest  : 1 ;   /* [0] */
        HI_U32  bitBypassBalanceFe  : 1 ;   /* [1] */
        HI_U32  bitBypassISPDGain   : 1 ;   /* [2] */
        HI_U32  bitBypassGammaFe    : 1 ;   /* [3] */
        HI_U32  bitBypassCrosstalkR : 1 ;   /* [4] */
        HI_U32  bitBypassDPC        : 1 ;   /* [5] */
        HI_U32  bitBypassNR         : 1 ;   /* [6] */
        HI_U32  bitBypassDehaze     : 1 ;   /* [7] */
        HI_U32  bitBypassWBGain     : 1 ;   /* [8] */
        HI_U32  bitBypassShading    : 1 ;   /* [9] */
        HI_U32  bitBypassACM        : 1 ;   /* [10] */
        HI_U32  bitBypassDRC        : 1 ;   /* [11] */
        HI_U32  bitBypassDemosaic   : 1 ;   /* [12] */
        HI_U32  bitBypassColorMatrix: 1 ;   /* [13] */
        HI_U32  bitBypassGamma      : 1 ;   /* [14] */
        HI_U32  bitBypassFSWDR      : 1 ;   /* [15] */
        HI_U32  bitGammaFePosition  : 1 ;   /* [16] */
        HI_U32  bit2Rsv3            : 2 ;   /* [17:18] */
        HI_U32  bitBypassCsConv     : 1 ;   /* [19] */
        HI_U32  bit2Rsv4            : 2 ;   /* [20:21] */
        HI_U32  bitBypassSharpen    : 1 ;   /* [22] */
        HI_U32  bitBypassUVNR		: 1 ;   /* [23] */
        HI_U32  bitChnSwitch        : 1 ;   /* [24] */
        HI_U32  bit2BypassMode      : 2 ;   /* [25:26] */
        HI_U32  bitBypassRGBIR      : 1 ;   /* [27] */
        HI_U32  bitBypassAll        : 1 ;   /* [28] */
        HI_U32  bit5Rsv5            : 3 ;   /* [29:31] */
    };
}ISP_MODULE_CTRL_U;

typedef struct hiISP_MOD_PARAM_S
{
	HI_U32		proc_param;
}ISP_MOD_PARAM_S;

typedef struct hiISP_DRC_MANUAL_ATTR_S
{
    HI_U8  u8Strength;          /* RW, Range: [0, 0xFF]. Strength of dynamic range compression. 
                                          Higher values lead to higher differential gain between shadows and highlights. */
    HI_U8  u8LocalMixingBright;     /*RW, Range: [0x0,0x80]. Controls the gain of the details where the value of the area is greater than a certain threshold value */
	HI_U8  u8LocalMixingDark;        /*RW, Range: [0x0,0x60]. Controls the gain of the details where the value of the area is smaller than a certain threshold value*/
} ISP_DRC_MANUAL_ATTR_S;

typedef struct hiISP_DRC_AUTO_ATTR_S
{
    HI_U8  u8Strength;          /* RW, Range: [0, 0xFF]. It is the bias of base strength. The strength used in ISP is generated by firmware. 
                                        In linear mode, strength = f1(u8Strength, histogram, ISO)
                                        In sensor WDR mode: strength = f2(u8Strength, histogram, ISO)
                                        In 2to1 WDR mode: strength = f3(ExpRatio, ISO) */
    HI_U8  u8LocalMixingBright;     /*R, Range: [0x0,0x80]. Controls the gain of the details where the value of the area is greater than a certain threshold value */
	HI_U8  u8LocalMixingDark;        /*R, Range: [0x0,0x60]. Controls the gain of the details where the value of the area is smaller than a certain threshold value*/
} ISP_DRC_AUTO_ATTR_S;

#define COLORCORRECTIONLUT_NODE_NUMBER 33
typedef struct hiISP_DRC_ATTR_S
{
    HI_BOOL bEnable;

	HI_U8  u8SpatialVar;             /*RW, Range: [0, 0xF].Controls the frequency of the spatial filter  */
	HI_U8  u8RangeVar;               /*RW, Range: [0, 0xF].Controls the frequency of the range filter*/
	
	HI_U8  u8Asymmetry;              /*RW, Range: [0x1,0x1E].The parameter0 of DRC tone mapping curve*/
	HI_U8  u8SecondPole;             /*RW, Range: [0x96,0xD2].The parameter1 of DRC tone mapping curve*/
	HI_U8  u8Stretch;                /*RW, Range: [0x1E,0x3C].The parameter2 of DRC tone mapping curve*/
	HI_U8  u8LocalMixingThres;       /*RW, Range: [0x2,0xA].The threshold used to distinguish light and dark areas */

	HI_U16 u16DarkGainLmtY;          /*RW, Range: [0x0,0x85].Restricts the dark luminance gain*/
	HI_U16 u16DarkGainLmtC;          /*RW, Range: [0x0,0x85].Restricts the dark chroma gain*/
	HI_U16 u16BrightGainLmt;         /*RW, Range: [0x0,0xA0].Restricts the bright luminance gain*/

    HI_U16 au16ColorCorrectionLut[COLORCORRECTIONLUT_NODE_NUMBER];/*RW,Range:[0x0,0xFFF]*/

    ISP_OP_TYPE_E enOpType;
    ISP_DRC_MANUAL_ATTR_S stManual;
    ISP_DRC_AUTO_ATTR_S   stAuto;
} ISP_DRC_ATTR_S;

typedef enum hiISP_STATIC_DP_TYPE_E{
    ISP_STATIC_DP_BRIGHT = 0x0,  
    ISP_STATIC_DP_DARK,    
    ISP_STATIC_DP_BUTT
} ISP_STATIC_DP_TYPE_E;


typedef struct hiISP_DP_STATIC_CALIBRATE_S
{
    HI_BOOL bEnable;             /* not support */ 
    HI_BOOL bEnableDetect;        /* RW. Set 'HI_TRUE'to start static defect-pixel calibration, and firmware will set 'HI_FALSE' when finished. */
    
    ISP_STATIC_DP_TYPE_E enStaticDPType;    /* Select static bright/dark defect-pixel calibration. */
    HI_U8   u8StartThresh;        /* RW, Range: [1, 0xFF]. Start threshold for static defect-pixel calibraiton.Default Value:0x3.. */
    HI_U16  u16CountMax;          /* RW, Range: [0, 0x800]. Limit of max number of static defect-pixel calibraiton.Default Value:0x400 . */
    HI_U16  u16CountMin;          /* RW, Range: [0, u16CountMax]. Limit of min number of static defect-pixel calibraiton.Default Value:0x1. */
    HI_U16  u16TimeLimit;         /* RW, Range: [0x0, 0x640].Time limit for static defect-pixel calibraiton, in frame number.Default Value:1600(0x640). */

    HI_U32      au32Table[STATIC_DP_COUNT_MAX];  /* RO, Static defect-pixel calibraiton table,the first 13 bits represents the X coordinate of the defect pixel, the second 12 bits represent the Y coordinate of the defect pixel. */
    HI_U8       u8FinishThresh;   /* RO, Range: [0, 0xFF]. Finish threshold for static defect-pixel calibraiton. */    
    HI_U16      u16Count;         /* RO, Range: [0, 0x800]. Finish number for static defect-pixel calibraiton. */
    ISP_STATUS_E enStatus;        /* RO, Status of static defect-pixel calibraiton.Default Value:0(0x0). */
} ISP_DP_STATIC_CALIBRATE_S;

typedef struct hiISP_DP_STATIC_ATTR_S
{
    HI_BOOL bEnable;				/* RW. Enable/disable the static defect-pixel module.Default Value:1(0x1). */
    HI_U16  u16BrightCount;       /* RW, When used as input(W), indicate the number of static bright defect pixels,Range:[0x0, 0x800]; As output(R),indicate the number of static bright and dark defect pixels */
    HI_U16  u16DarkCount;         /* RW, When used as input(W), indicate the number of static dark defect pixels,Range:[0x0, 0x800]; As output(R), invalid value 0 */    
    HI_U32  au32BrightTable[STATIC_DP_COUNT_MAX];   /* RW, Range: [0x0, 0x1FFFFFF],the first 13 bits represents the X coordinate of the defect pixel, the second 12 bits represent the Y coordinate of the defect pixel
                                                                    Notice : When used as input(W), indicate static bright defect pixels table;  As output(R), indicate static bright and dark defect pixels table */
    HI_U32  au32DarkTable[STATIC_DP_COUNT_MAX];     /* RW, Range: [0x0, 0x1FFFFFF],the first 13 bits represents the X coordinate of the defect pixel, the second 12 bits represent the Y coordinate of the defect pixel
                                                                    Notice : When used as input(W), indicate static dark defect pixels table;  As output(R), invalid value */
    HI_BOOL bShow;					/*RW,highlight static defect pixel,default value:0x0*/
} ISP_DP_STATIC_ATTR_S;

typedef struct hiISP_DP_DYNAMIC_MANUAL_ATTR_S
{
	HI_U16	u16Slope;			  /* RW, Range: [0x0, 0xff] */
	HI_U16	u16Thresh;			  /* not support */
	HI_U16	u16LineThresh;		  /* not support */
	HI_U16	u16HpThresh;		  /* not support */  
	HI_U16	u16BlendRatio;		  /* RW, Range: [0x0, 0x100] */
} ISP_DP_DYNAMIC_MANUAL_ATTR_S;

typedef struct hiISP_DP_DYNAMIC_AUTO_ATTR_S
{
	HI_U16	au16Slope[ISP_AUTO_ISO_STENGTH_NUM];			  /* RW, Range: [0x0, 0xff] */
	HI_U16	u16Thresh;			                      /* not support */
	HI_U16	u16LineThresh;		                      /* not support */
	HI_U16	u16HpThresh;		                      /* not support */  
	HI_U16	au16BlendRatio[ISP_AUTO_ISO_STENGTH_NUM];	  /* RW, Range: [0x0, 0x100] */
} ISP_DP_DYNAMIC_AUTO_ATTR_S;

typedef struct hiISP_DP_DYNAMIC_ATTR_S
{
    HI_BOOL bEnable;
    ISP_OP_TYPE_E enOpType;
    ISP_DP_DYNAMIC_MANUAL_ATTR_S stManual;
    ISP_DP_DYNAMIC_AUTO_ATTR_S   stAuto;
} ISP_DP_DYNAMIC_ATTR_S;
typedef struct hiISP_DIS_ATTR_S
{
    HI_BOOL bEnable;
} ISP_DIS_ATTR_S;



typedef struct hiISP_SHADING_ATTR_S
{
    HI_BOOL bEnable;
	ISP_OP_TYPE_E enOpType;
    HI_U32  au32XGridWidth[8];/*RW, Range:[0x0, u32Width/4]*/
    HI_U32  au32YGridWidth[8];/*RW, Range:[0x0, u32Height/4]*/
    HI_U32  au32NoiseControlGain[SHADING_MESH_NUM]; /*RW, Range:[0x0, 0x1FFF]*/        
	HI_U32  au32RGain[SHADING_MESH_NUM];  /*RW, Range:[0x0, 0x1FFF]*/
	HI_U32  au32GrGain[SHADING_MESH_NUM]; /*RW, Range:[0x0, 0x1FFF]*/
	HI_U32  au32GbGain[SHADING_MESH_NUM]; /*RW, Range:[0x0, 0x1FFF]*/
	HI_U32  au32BGain[SHADING_MESH_NUM];  /*RW, Range:[0x0, 0x1FFF]*/
} ISP_SHADING_ATTR_S;

typedef enum hiISP_IRPOS_TYPE_E
{
    ISP_IRPOS_TYPE_GR = 0x0,
    ISP_IRPOS_TYPE_GB,
    ISP_IRPOS_TYPE_BUTT
}ISP_IRPOS_TYPE_E;

typedef struct hiISP_RGBIR_ATTR_S
{
    HI_BOOL bEnable;                            /*RW, Range: [false, true] Rgbir moudle enable  */
    ISP_IRPOS_TYPE_E enIrPosType;               /*RW, Range: [0, 1] Rgbir moudle ir position: 0 Gr, 1 Gb  */
    HI_U16  u16OverExpThresh;                   /*RW, Range: [0,4095] Rgbir moudle over exposure threshold*/    
}ISP_RGBIR_ATTR_S;

typedef struct hiISP_RGBIR_CTRL_S
{
    HI_BOOL bIrOutEn;                           /*RW, Range: [false, true] Rgbir moudle output ir image enable  */
    HI_BOOL bIrFilterEn;                        /*RW, Range: [false, true] Rgbir moudle noise filter enable  */
    HI_BOOL bRemovelEn;                         /*RW, Range: [false, true] Rgbir moudle removel ir enable  */
    ISP_OP_TYPE_E enCompType;                   /*RW, Range: [false, true] Rgbir moudle after removel ir, compensation type 0:auto,1:manul  */
    HI_U16  u16ManuGain;                        /*RW, Range: [256,1023] Rgbir moudle after removel ir, manul-compensation gain,2bit integer,8bit decimal */
    HI_S16  as16ScaleCoef[RGBIR_MATRIX_NUM];    /*RW, Range: [-512,511] Rgbir moudle removel ir translate matrix,bit9:signed bit,bit8:integer bit,bit7_0:decimal bit*/
}ISP_RGBIR_CTRL_S;

typedef struct hiISP_NR_MANUAL_ATTR_S
{
    HI_U8 u8Strength;      /*not support */          
    HI_U8 u8VarStrength;    /*Range: [0x0,0xFF]*/           
    HI_U8 u8FixStrength;   /*Range: [0x0,0xFF]*/ 
    HI_U8 u8LowFreqSlope;  /*Range: [0x0,0x10]  */
	HI_U16 u16Threshold;  /*Range: [0x0,0xFFFF] */
} ISP_NR_MANUAL_ATTR_S;

typedef struct hiISP_NR_AUTO_ATTR_S
{
    HI_U8 au8Strength[ISP_AUTO_ISO_STENGTH_NUM];  /*not support */
    HI_U8 au8FixStrength[ISP_AUTO_ISO_STENGTH_NUM];     /*Range: [0x0,0xFF]*/ 
    HI_U8 au8VarStrength[ISP_AUTO_ISO_STENGTH_NUM];     /*Range: [0x0,0xFF]*/ 
    HI_U8 au8LowFreqSlope[ISP_AUTO_ISO_STENGTH_NUM];    /*Range: [0x0,0x10]*/
	HI_U16 au16Threshold[ISP_AUTO_ISO_STENGTH_NUM]; /*Range: [0x0,0xFFFF]*/  
} ISP_NR_AUTO_ATTR_S;

typedef struct hiISP_NR_ATTR_S
{
    HI_BOOL bEnable;
    ISP_OP_TYPE_E enOpType;
    ISP_NR_MANUAL_ATTR_S stManual;
    ISP_NR_AUTO_ATTR_S   stAuto;
} ISP_NR_ATTR_S;

typedef enum hiISP_GAMMA_CURVE_TYPE_E
{
    ISP_GAMMA_CURVE_DEFAULT = 0x0,  /* default Gamma curve */
    ISP_GAMMA_CURVE_SRGB,
    ISP_GAMMA_CURVE_USER_DEFINE,    /* user defined Gamma curve, Gamma Table must be correct */
    ISP_GAMMA_CURVE_BUTT
} ISP_GAMMA_CURVE_TYPE_E;
typedef struct hiISP_GAMMA_ATTR_S
{
    HI_BOOL bEnable;

    ISP_GAMMA_CURVE_TYPE_E enCurveType;
    HI_U16 u16Table[GAMMA_NODE_NUM];
} ISP_GAMMA_ATTR_S;

typedef struct hiISP_GAMMAFE_ATTR_S
{
    HI_BOOL bEnable;

    HI_U16 u16Table[GAMMA_FE0_NODE_NUM + GAMMA_FE1_NODE_NUM]; /* only for WDR sensor mode */
} ISP_GAMMAFE_ATTR_S;

typedef struct hiISP_SHARPEN_MANUAL_ATTR_S
{
  HI_BOOL bEnLowLumaShoot;
  HI_U8 u8SharpenD;     /*Sharpen strength of Edge, range [0: 255], equal to maxEdgeAmt in fw.*/
  HI_U8 u8SharpenUd;	/*Sharpen strength of detail, range [0: 255], equal to maxSharpAmt1 in fw  */    
  HI_U8 u8OverShoot;	/*RW:  range [0: 255] */
  HI_U8 u8UnderShoot;	/*RW:  range [0: 255] */
  HI_U8 u8TextureNoiseThd;	/*RW:  range [0: 255] */
  HI_U8 u8EdgeNoiseThd;	/*RW:  range [0: 255] */

} ISP_SHARPEN_MANUAL_ATTR_S;
typedef struct hiISP_SHARPEN_AUTO_ATTR_S
{
    HI_BOOL abEnLowLumaShoot [ISP_AUTO_ISO_STENGTH_NUM];
    HI_U8 au8SharpenD  [ISP_AUTO_ISO_STENGTH_NUM];  /*Sharpen strength of Edge, range [0: 255], equal to maxEdgeAmt in fw.*/
    HI_U8 au8SharpenUd [ISP_AUTO_ISO_STENGTH_NUM];  /*Sharpen strength of detail, range [0: 255], equal to maxSharpAmt1 in fw */
	HI_U8 au8OverShoot [ISP_AUTO_ISO_STENGTH_NUM];	/*RW:  range [0: 255] */
	HI_U8 au8UnderShoot[ISP_AUTO_ISO_STENGTH_NUM];	/*RW:  range [0: 255] */
	HI_U8 au8TextureNoiseThd[ISP_AUTO_ISO_STENGTH_NUM];	/*RW:  range [0: 255] */
    HI_U8 au8EdgeNoiseThd[ISP_AUTO_ISO_STENGTH_NUM];	/*RW:  range [0: 255] */
} ISP_SHARPEN_AUTO_ATTR_S;

typedef struct hiISP_SHARPEN_ATTR_S
{
    HI_BOOL bEnable;
    ISP_OP_TYPE_E enOpType;
    ISP_SHARPEN_MANUAL_ATTR_S stManual;
    ISP_SHARPEN_AUTO_ATTR_S   stAuto;
} ISP_SHARPEN_ATTR_S;

typedef struct hiISP_UVNR_MANUAL_ATTR_S
{
    HI_U8 u8ColorCast;   		/* RW,  Range:[0, 3]. equal to coring limit in fw */ 
    HI_U8 u8UvnrThreshold;   /* RW,  Range:[0, 64]. */   
    HI_U8 u8UvnrStrength; 	/* RW,  Range:[0, 34]. */ //0: Med & UVNR all close; 1: Med open, UVNR close; 2---34¡oMed Close, UNVR open;
} ISP_UVNR_MANUAL_ATTR_S;

typedef struct hiISP_UVNR_AUTO_ATTR_S
{
    HI_U8 au8ColorCast[ISP_AUTO_ISO_STENGTH_NUM];    		/* RW,  Range:[0, 3]. */
    HI_U8 au8UvnrThreshold[ISP_AUTO_ISO_STENGTH_NUM];    /* RW,  Range:[0, 64]. */
    HI_U8 au8UvnrStrength[ISP_AUTO_ISO_STENGTH_NUM]; 	/* RW,  Range:[0, 34]. */
} ISP_UVNR_AUTO_ATTR_S;

typedef struct hiISP_UVNR_ATTR_S
{
    HI_BOOL bEnable;
    ISP_OP_TYPE_E enOpType;
    ISP_UVNR_MANUAL_ATTR_S stManual;
    ISP_UVNR_AUTO_ATTR_S   stAuto;
} ISP_UVNR_ATTR_S;
/*Crosstalk Removal*/
typedef struct hiISP_CR_ATTR_S
{
    HI_BOOL  bEnable;
    HI_U16   au16Strength[ISP_AUTO_ISO_STENGTH_NUM];   /*RW, Range:[0, 0x3FFF]*/    
    HI_U16   u16Threshold;             /*RW, Range:[0, 0x3FFF]*/                
    HI_U8    u8Slope;              /*RW, Range:[0, 0xE]*/                   
    HI_U8    u8Sensitivity;      /*RW, Range:[0, 0xE]*/                      
    HI_U16   u16SensiThreshold;     /*RW, Range:[0, 0x3FFF]*/  
}ISP_CR_ATTR_S;

typedef struct hiISP_ANTI_FALSECOLOR_MANUAL_S
{
    HI_U8 u8Strength;           /*RW, Range:[0, 0xFF]*/
    HI_U8 u8Threshold;          /*RW, Range:[0, 0xFF]*/             
} ISP_ANTI_FALSECOLOR_MANUAL_S;

typedef struct hiISP_ANTI_FALSECOLOR_AUTO_ATTR_S
{
    HI_U8 au8Strength[ISP_AUTO_ISO_STENGTH_NUM];    /*RW, Range:[0, 0xFF]*/  
    HI_U8 au8Threshold[ISP_AUTO_ISO_STENGTH_NUM];   /*RW, Range:[0, 0xFF]*/  
} ISP_ANTI_FALSECOLOR_AUTO_ATTR_S;

typedef struct hiISP_ANTI_FALSECOLOR_S
{
    HI_BOOL bEnable;
	ISP_OP_TYPE_E enOpType;
    ISP_ANTI_FALSECOLOR_MANUAL_S stManual;
    ISP_ANTI_FALSECOLOR_AUTO_ATTR_S stAuto;
} ISP_ANTI_FALSECOLOR_S;

typedef enum hiISP_DEMOSAIC_CFG_E
{
    ISP_DEMOSAIC_CFG_DEFAULT = 0,
    ISP_DEMOSAIC_CFG_VH,
    ISP_DEMOSAIC_CFG_AA,
    ISP_DEMOSAIC_CFG_VA,
    ISP_DEMOSAIC_CFG_UU,

    ISP_DEMOSAIC_CFG_BUTT,
} ISP_DEMOSAIC_CFG_E;
    
typedef struct hiISP_DEMOSAIC_ATTR_S
{
    HI_U16  u16VhSlope;              /* RW,Range: [0x0, 0xFF]  */
    HI_U16  u16AaSlope;              /* not support */
    HI_U16  u16VaSlope;              /* not support */
    HI_U16  u16UuSlope;              /* RW,Range: [0x0, 0x3FF] */
    HI_U8   u8VhLimit;              /* RW,Range: [0x0, 0xFF]  */    
    HI_U8   u8VhOffset;             /* RW,Range: [0x0, 0xFF]  */      
    HI_U16  u16VhThresh;            /* not support */   
    HI_U16  u16AaThresh;            /* not support */
    HI_U16  u16VaThresh;            /* not support */
    HI_U16  u16UuThresh;            /* not support */
    ISP_DEMOSAIC_CFG_E   enCfgType;       /* not support*/    
    HI_U16   au16LumThresh[ISP_AUTO_ISO_STENGTH_NUM];         /* NOT SUPPORT */
    HI_U16   au16NpOffset[ISP_AUTO_ISO_STENGTH_NUM];   /* RW, Range:[0x0, 0x3FF] */
}ISP_DEMOSAIC_ATTR_S;

typedef struct hiISP_BLACK_LEVEL_S 
{ 
    HI_U16 au16BlackLevel[4]; /* RW, Range: [0x0, 0x7FF]*/ 
} ISP_BLACK_LEVEL_S;

/* FPN */
typedef enum hiISP_FPN_TYPE_E
{
    ISP_FPN_TYPE_FRAME = 0, 
    ISP_FPN_TYPE_LINE = 1, 
    ISP_FPN_TYPE_BUTT       
}ISP_FPN_TYPE_E;

typedef struct hiISP_FPN_FRAME_INFO_S
{
    HI_U32              u32Iso;             /* FPN CALIBRATE ISO */
    HI_U32              u32Offset;          /* FPN frame u32Offset (agv pixel value) */    
    HI_U32              u32FrmSize;         /* FPN frame size (exactly frm size or compress len) */
    VIDEO_FRAME_INFO_S  stFpnFrame;         /* FPN frame info, 8bpp,10bpp,12bpp,16bpp. Compression or not */
}ISP_FPN_FRAME_INFO_S;

typedef struct hiISP_FPN_CALIBRATE_ATTR_S
{    
    HI_U32                          u32Threshold;        /* pix value > threshold means defective pixel, [1, 0xFFF] */
    HI_U32                          u32FrameNum;         /* value is 2^N, range: [1, 16] */
    ISP_FPN_TYPE_E                  enFpnType;           /* frame mode or line mode */
    ISP_FPN_FRAME_INFO_S            stFpnCaliFrame;
}ISP_FPN_CALIBRATE_ATTR_S;

typedef struct hiISP_FPN_MANUAL_ATTR_S
{
    HI_U32               u32Strength;         /* gain, RW fp 4.8 */
}ISP_FPN_MANUAL_ATTR_S;

typedef struct hiISP_FPN_AUTO_ATTR_S
{
    HI_U32               u32Strength;         /* gain, RO fp 4.8 */
}ISP_FPN_AUTO_ATTR_S;

typedef struct hiISP_FPN_ATTR_S
{
    HI_BOOL               bEnable;
    ISP_OP_TYPE_E         enOpType;           /* manual or auto mode */   
    ISP_FPN_TYPE_E        enFpnType;          /* frame mode or line mode */
    ISP_FPN_FRAME_INFO_S  stFpnFrmInfo;       /* input in correction mode. */
    ISP_FPN_MANUAL_ATTR_S stManual;
    ISP_FPN_AUTO_ATTR_S   stAuto;
}ISP_FPN_ATTR_S;

typedef struct hiISP_BLC_ATTR_S
{
    HI_BOOL             bEnable;    

    HI_U32              u32ROffset;          /* WO, R component */
    HI_U32              u32GROffset;         /* WO, GR component */
    HI_U32              u32BOffset;          /* WO, B component */
    HI_U32              u32GBOffset;         /* WO, GB component */
}ISP_BLC_ATTR_S;

typedef enum hiISP_IR_STATUS_E
{
    ISP_IR_STATUS_NORMAL = 0,
    ISP_IR_STATUS_IR     = 1,
    ISP_IR_BUTT
} ISP_IR_STATUS_E;

typedef enum hiISP_IR_SWITCH_STATUS_E
{
    ISP_IR_SWITCH_NONE      = 0,
    ISP_IR_SWITCH_TO_NORMAL = 1,
    ISP_IR_SWITCH_TO_IR     = 2,
    ISP_IR_SWITCH_BUTT
} ISP_IR_SWITCH_STATUS_E;

typedef struct hiISP_IR_AUTO_ATTR_S
{
    HI_BOOL bEnable;            /* RW, HI_TRUE: enable IR_auto function;  HI_TRUE: disable IR_auto function. */
    HI_U32  u32Normal2IrIsoThr; /* RW, Range: [0, 0xFFFFFFFF]. ISO threshold of switching from normal to IR mode. */
    HI_U32  u32Ir2NormalIsoThr; /* RW, Range: [0, 0xFFFFFFFF]. ISO threshold of switching from IR to normal mode. */
    HI_U32  u32RGMax;           /* RW, Range: [0x0, 0xFFF].    Maximum value of R/G in IR scene, 4.8-bit fix-point. */
    HI_U32  u32RGMin;           /* RW, Range: [0x0, u32RGMax]. Minimum value of R/G in IR scene, 4.8-bit fix-point. */
    HI_U32  u32BGMax;           /* RW, Range: [0x0, 0xFFF].    Maximum value of B/G in IR scene, 4.8-bit fix-point. */
    HI_U32  u32BGMin;           /* RW, Range: [0x0, u32BGMax]. Minimum value of B/G in IR scene, 4.8-bit fix-point. */

    ISP_IR_STATUS_E enIrStatus; /* RW. Current IR status. */

    ISP_IR_SWITCH_STATUS_E enIrSwitch; /* RO, IR switch status. */
} ISP_IR_AUTO_ATTR_S;

/* DeFog */
typedef struct hiISP_DEFOG_MANUAL_ATTR_S
{
    HI_U8			   u8strength;
}ISP_DEFOG_MANUAL_ATTR_S;

typedef struct hiISP_DEFOG_AUTO_ATTR_S
{
    HI_U8			   u8strength;          
}ISP_DEFOG_AUTO_ATTR_S;
   
typedef struct hiISP_DEFOG_ATTR_S
{
    HI_BOOL            bEnable;
    HI_U8              u8HorizontalBlock;      /* RW,Range: [0x6, 0x10] */
    HI_U8              u8VerticalBlock;     /* RW,Range: [0x6, 0xF] */
    ISP_OP_TYPE_E      enOpType;
    ISP_DEFOG_MANUAL_ATTR_S stManual;
    ISP_DEFOG_AUTO_ATTR_S   stAuto;
}ISP_DEFOG_ATTR_S;

/********************** ACM **************************************/

typedef enum hiISP_ACM_MODE_E
{
    ISP_ACM_MODE_BLUE          = 0,
    ISP_ACM_MODE_GREEN            ,
    ISP_ACM_MODE_BG               ,
    ISP_ACM_MODE_SKIN             ,
    ISP_ACM_MODE_VIVID            ,
    ISP_ACM_MODE_BUTT

} ISP_ACM_MODE_E;

typedef struct hi_ISP_ACM_ATTR_S
{
    HI_BOOL         bEnable         ;
    HI_BOOL         bDemoEnable     ;/*0-debug closed; 1-debug open, the left screen is original video and the right screen is ACM-processed video*/
    ISP_ACM_MODE_E  enMode          ;
    HI_U32          u32Stretch      ; /*input data Clip range: 0-Y 64-940, C 64-960; 1-Y 0-1023, C 0-1023*/
    HI_U32          u32ClipRange    ; /*output data Clip range: 0-Y 64-940, C 64-960; 1-Y 0-1023, C 0-1023*/
    HI_U32          u32AcmClipOrWrap; /*0-wrap around; 1-clip*/
    HI_U32          u32CbcrThr      ; /* [0,255]*/
    HI_U32          u32GainLuma     ; /* [0,512]*/
    HI_U32          u32GainHue      ; /* [0,512]*/
    HI_U32          u32GainSat      ; /* [0,512]*/
}ISP_ACM_ATTR_S;

#define ACM_Y_NUM            5
#define ACM_H_NUM            29
#define ACM_S_NUM            7

typedef struct hi_ISP_ACM_LUT_S
{
    HI_S16 as16Y[ACM_Y_NUM][ACM_S_NUM][ACM_H_NUM];   /**<  */
    HI_S16 as16H[ACM_Y_NUM][ACM_S_NUM][ACM_H_NUM];   /**<  */
    HI_S16 as16S[ACM_Y_NUM][ACM_S_NUM][ACM_H_NUM];   /**<  */
}ISP_ACM_LUT_S;

typedef struct hi_ISP_ACM_COEF_S
{
    ISP_ACM_LUT_S stLut;
    ISP_ACM_MODE_E enMode;
}ISP_ACM_COEF_S;

typedef struct hiISP_INNER_STATE_INFO_S
{
    HI_U32 u32DRCStrengthActual;
    HI_U32 u32NRStrengthActual;        /*not support*/
    HI_U32 u32SharpenStrengthDActual;
    HI_U32 u32SharpenStrengthUdActual;
    HI_U32 u32SharpenStrengthRGBActual; /*not support*/
    HI_U32 u32DefogStrengthActual;
    HI_U32 u32RgbirGain;                /* RO, Range: [0x0, 0x3ff]*/

    HI_BOOL bWDRSwitchFinish;          /*not support*/
    HI_BOOL bResSwitchFinish;        /*not support*/
} ISP_INNER_STATE_INFO_S;


typedef enum hiISP_AE_SWITCH_E
{
    ISP_AE_AFTER_STATIC_WB        = 0 ,
    ISP_AE_FROM_SENSOR_CHANNEL_1        , /*not support*/
    ISP_AE_AFTER_SHADING                , /*not support*/
    ISP_AE_AFTER_GAMMA_FE               , /*not support*/
    ISP_AE_AFTER_DRC                    ,
    ISP_AE_FROM_SENSOR_CHANNEL_2        , /*not support*/
    ISP_AE_AFTER_WDR_STITCH             , /*not support*/
    ISP_AE_AFTER_BLC_CHANNEL_2          , /*not support*/
    ISP_AE_AFTER_DG                  ,
    ISP_AE_SWITCH_BUTT 
} ISP_AE_SWITCH_E;


typedef enum hiISP_AE_HIST_SWITCH_E
{
    ISP_AE_HIST_SAME_AS_AE                = 0,
    ISP_AE_HIST_FROM_SENSOR_CHANNEL_1        ,
    ISP_AE_HIST_AFTER_SHADING                ,
    ISP_AE_HIST_AFTER_GAMMA_FE               ,
    ISP_AE_HIST_AFTER_DRC                    ,
    ISP_AE_HIST_FROM_SENSOR_CHANNEL_2        ,
    ISP_AE_HIST_AFTER_WDR_STITCH             ,
    ISP_AE_HIST_AFTER_BLC_CHANNEL_2,
    ISP_AE_HIST_SWITCH_BUTT
} ISP_AE_HIST_SWITCH_E;


/* config of statistics structs */
typedef struct hiISP_AE_STATISTICS_CFG_S
{
    HI_U8  au8HistThresh[4];    /*RW, Histogram threshold for bin 0/1 1/2 2/3 3/4 boundary, Range: [0x0, 0xFF]*/
    ISP_AE_SWITCH_E  enAESwitch;          /*RW, The position of AE statistics module in ISP pipeline, 3518EV200 AE statistics here include Global 256 bins histogram, Global average and zone average */ 
                                /* 0 = After DG;   1 = After static WB */
                                /* 2 = After DRC;     */

    ISP_AE_HIST_SWITCH_E  enHistSwitch;        /*RW, The position of Global 256 bins histogram in ISP pipeline*/
                                /* 0 = Same as AE;       1 = Immediately from sensor, channel 1 (for WDR modes); */
                                /* 2 = After shading;     3 = After Gamma FE; */
                                /* 4 = After DRC;         5 = Immediately from sensor, channel 2 (for WDR modes); */
                                /* 6 = After WDR stitch; 7 = After BLC, channel 2 (for WDR modes);*/                                

    ISP_AE_SWITCH_E  enAESumSwitch;       /*RW, The position of Global average and zone average in ISP pipeline*/
                                /* 0 = After static WB;   1 = Immediately from sensor, channel 1 (for WDR modes); */
                                /* 2 = After shading;     3 = After Gamma FE; */
                                /* 4 = After DRC;         5 = Immediately from sensor, channel 2 (for WDR modes); */
                                /* 6 = After WDR stitch; 7 = After BLC, channel 2 (for WDR modes);*/                                
} ISP_AE_STATISTICS_CFG_S;

typedef struct hiISP_WB_STATISTICS_CFG_PARA_S
{
    HI_U16 u16WhiteLevel;       /*RW, Upper limit of valid data for white region, Range: [0x0, 0xFFF] for Bayer statistics, [0x0, 0x3FF] for RGB statistics*/
    HI_U16 u16BlackLevel;       /*RW, Lower limit of valid data for white region, Range: [0x0, u16WhiteLevel]. for Bayer statistics, bitwidth is 12, for RGB statistics, bitwidth is 10*/
    HI_U16 u16CbMax;            /*RW, Maximum value of B/G for white region, Range: [0x0,0xFFF]*/
    HI_U16 u16CbMin;            /*RW, Minimum value of B/G for white region, Range: [0x0, u16CbMax]*/
    HI_U16 u16CrMax;            /*RW, Maximum value of R/G for white region, Range: [0x0, 0xFFF]*/
    HI_U16 u16CrMin;            /*RW, Minimum value of R/G for white region, Range: [0x0, u16CrMax]*/
    HI_U16 u16CbHigh;           /*Hi3518EV200 does't support hexagon limit*/  
    HI_U16 u16CbLow;            /*Hi3518EV200 does't support hexagon limit*/ 
    HI_U16 u16CrHigh;           /*Hi3518EV200 does't support hexagon limit*/   
    HI_U16 u16CrLow;            /*Hi3518EV200 does't support hexagon limit*/  
} ISP_WB_STATISTICS_CFG_PARA_S;
typedef struct hiISP_WB_STATISTICS_CFG_S
{
    ISP_WB_STATISTICS_CFG_PARA_S stBayerCfg;
    ISP_WB_STATISTICS_CFG_PARA_S stRGBCfg;       /*Hi3518EV200 does't support RGB Statistics*/
} ISP_WB_STATISTICS_CFG_S;

typedef enum hiISP_AF_PEAK_MODE_E
{
    ISP_AF_STA_NORM         = 0,    /* use every value of the block statistic*/
    ISP_AF_STA_PEAK            ,    /* use the maximum value in one line of the block statistic*/
    ISP_AF_STA_BUTT    
}ISP_AF_PEAK_MODE_E;

typedef enum hiISP_AF_SQU_MODE_E
{
    ISP_AF_STA_SUM_NORM     = 0,    /* statistic value accumlate*/
    ISP_AF_STA_SUM_SQU         ,    /* statistic value square then accumlate*/
    ISP_AF_STA_SUM_BUTT    
}ISP_AF_SQU_MODE_E;

typedef enum hiISP_AF_STATISTICS_POS_E
{
   ISP_AF_STATISTICS_YUV    = 0,
   ISP_AF_STATISTICS_RAW       ,
   ISP_AF_STATISTICS_BUTT

}ISP_AF_STATISTICS_POS_E;

typedef struct hiISP_AF_CFG_S
{
    HI_BOOL             bEnable;    /* RW, Range: [0,1]. AF enable. */
    HI_U16              u16Hwnd;    /* RW, Range: [1, 17]. AF statistic window horizontal block. */
    HI_U16              u16Vwnd;    /* RW, Range: [1, 15]. AF statistic window veritical block. */
    HI_U16              u16Hsize;   /* RW, Range: [0x0, 0xFFF]. AF image wide. */
    HI_U16              u16Vsize;   /* RW, Range: [0x0, 0xFFF]. AF image hight. */
    ISP_AF_PEAK_MODE_E  enPeakMode; /* RW, Range: [0,1]. AF peak value statistic mode. */
    ISP_AF_SQU_MODE_E   enSquMode;  /* RW, Range: [0,1]. AF statistic square accumulate. */
    ISP_AF_STATISTICS_POS_E enStatisticsPos; /* RW, Range: [0,1]. AF statistic position, it can be set to yuv or raw */
}ISP_AF_CFG_S;

typedef struct hiISP_AF_H_PARAM_S
{
    HI_BOOL     abIIREn[3];         /* RW, Range: [0, 1]. IIR enable. */
    HI_S16      as16IIRGain[7];     /* RW, Range: gain0:[0,255]; others:[-511, 511]. IIR gain. */
    HI_U16      au16IIRShift[4];    /* RW, Range: [0x0, 0xF]. IIR shift. */
    HI_U16      u16IIRThd;          /* RW, Range: [0x0, 0x7FF]. IIR threshold. */
}ISP_AF_H_PARAM_S;

typedef struct hiISP_AF_V_PARAM_S
{
    HI_S16 as16FIRH[5];     /* RW, Range: [-31, 31]. FIR gain. */
    HI_U16 u16FIRThd;       /* RW, Range: [0x0, 0x7FF]. FIR threshold. */
}ISP_AF_V_PARAM_S;

typedef struct hiISP_AF_FV_PARAM_S
{
    HI_U16 u16AccShiftY;        /* RW, Range: [0x0, 0xF]. luminance Y statistic shift. */
    HI_U16 au16AccShiftH[2];    /* RW, Range: [0x0, 0xF]. IIR statistic shift. */
    HI_U16 au16AccShiftV[2];    /* RW, Range: [0x0, 0xF]. FIR statistic shift. */
}ISP_AF_FV_PARAM_S;

typedef struct hiISP_FOCUS_STATISTICS_CFG_S
{
    ISP_AF_CFG_S        stConfig;
    ISP_AF_H_PARAM_S    stHParam_IIR0;
    ISP_AF_H_PARAM_S    stHParam_IIR1;
    ISP_AF_V_PARAM_S    stVParam_FIR0;
    ISP_AF_V_PARAM_S    stVParam_FIR1;
    ISP_AF_FV_PARAM_S   stFVParam;
} ISP_FOCUS_STATISTICS_CFG_S;

typedef union hiISP_STATISTICS_CTRL_U
{
    HI_U32  u32Key;
    struct
    {
        HI_U32  bit1AeStat1     : 1 ;   /* [0] */      /* not support */
        HI_U32  bit1AeStat2     : 1 ;   /* [1] */      /* not support */
        HI_U32  bit1AeStat3     : 1 ;   /* [2] */
        HI_U32  bit1AeStat4     : 1 ;   /* [3] */
        HI_U32  bit1AeStat5     : 1 ;   /* [4] */
        HI_U32  bit1AwbStat1    : 1 ;   /* [5] */
        HI_U32  bit1AwbStat2    : 1 ;   /* [6] */
        HI_U32  bit1AwbStat3    : 1 ;   /* [7] */
        HI_U32  bit1AwbStat4    : 1 ;   /* [8] */
        HI_U32  bit1AfStat      : 1 ;   /* [9] */
        HI_U32  bit22Rsv        : 22;   /* [10:31] */
    };
}ISP_STATISTICS_CTRL_U;

typedef struct hiISP_STATISTICS_CFG_S
{
    ISP_STATISTICS_CTRL_U       unKey; 
    ISP_AE_STATISTICS_CFG_S     stAECfg;
    ISP_WB_STATISTICS_CFG_S     stWBCfg;
    ISP_FOCUS_STATISTICS_CFG_S  stFocusCfg;
} ISP_STATISTICS_CFG_S;

/* statistics structs */
typedef struct hiISP_AE_STATISTICS_S
{   
    HI_U16 au16Hist5Value[5];                                   /*RO, Global 5 bins histogram, Range: [0x0, 0xFFFF]*/
    HI_U16 au16ZoneHist5Value[AE_ZONE_ROW][AE_ZONE_COLUMN][5];  /*RO, Zone 5 bins histogram,Range: [0x0, 0xFFFF]*/
    HI_U32 au32Hist256Value[256];                               /*RO, Global 256 bins histogram,Range: [0x0, 0xFFFFFFFF]*/
    HI_U16 au16GlobalAvg[4];                                    /*RO, Global average value, Range: [0x0, 0xFFFF]*/
    HI_U16 au16ZoneAvg[AE_ZONE_ROW][AE_ZONE_COLUMN][4];         /*RO, Zone average value, Range: [0x0, 0xFFFF]*/
}ISP_AE_STATISTICS_S;

typedef struct hiISP_WB_BAYER_STATISTICS_S
{
    HI_U16 u16GlobalR;          /*RO, Global WB output Average R, Range: [0x0, 0xFFFF]*/   
    HI_U16 u16GlobalG;          /*RO, Global WB output Average G, Range: [0x0, 0xFFFF]*/  
    HI_U16 u16GlobalB;          /*RO, Global WB output Average B, Range: [0x0, 0xFFFF]*/  
    HI_U16 u16CountAll;         /*RO, normalized number of Gray points, Range: [0x0, 0xFFFF]*/  
    HI_U16 u16CountMin;         /*RO, normalized number of pixels under BlackLevel, Range: [0x0, 0xFFFF]*/  
    HI_U16 u16CountMax;         /*RO, normalized number of pixels above Whitelevel, Range: [0x0, 0xFFFF], u16CountAll + u16CountMin + u16CountMax = 0xFFFF*/  
           
    HI_U16 au16ZoneAvgR[AWB_ZONE_ROW][AWB_ZONE_COLUMN];            /*RO, Zone Average R, Range: [0x0, 0xFFFF]*/   
    HI_U16 au16ZoneAvgG[AWB_ZONE_ROW][AWB_ZONE_COLUMN];            /*RO, Zone Average G, Range: [0x0, 0xFFFF]*/   
    HI_U16 au16ZoneAvgB[AWB_ZONE_ROW][AWB_ZONE_COLUMN];            /*RO, Zone Average B, Range: [0x0, 0xFFFF]*/   
    HI_U16 au16ZoneCountAll[AWB_ZONE_ROW][AWB_ZONE_COLUMN];        /*RO, normalized number of Gray points, Range: [0x0, 0xFFFF]*/   
    HI_U16 au16ZoneCountMin[AWB_ZONE_ROW][AWB_ZONE_COLUMN];        /*RO, normalized number of pixels under BlackLevel, Range: [0x0, 0xFFFF]*/   
    HI_U16 au16ZoneCountMax[AWB_ZONE_ROW][AWB_ZONE_COLUMN];        /*RO, normalized number of pixels above Whitelevel, Range: [0x0, 0xFFFF]*/   
} ISP_WB_BAYER_STATISTICS_INFO_S; 

typedef struct hiISP_WB_RGB_STATISTICS_S
{
    HI_U16 u16GlobalGR;         /*RO, Global WB output G/R, 4.8-bit fix-point, Range: [0x0, 0xFFF]*/
    HI_U16 u16GlobalGB;         /*RO, Global WB output G/B, 4.8-bit fix-point, Range: [0x0, 0xFFF]*/
    HI_U32 u32GlobalSum;        /*RO, Global WB output population*/

    HI_U16 au16ZoneGR[AWB_ZONE_ROW][AWB_ZONE_COLUMN];       /*RO, Zone WB output G/R, 4.8-bit fix-point, Range: [0x0, 0xFFF]*/
    HI_U16 au16ZoneGB[AWB_ZONE_ROW][AWB_ZONE_COLUMN];       /*RO, Zone WB output G/B, 4.8-bit fix-point, Range: [0x0, 0xFFF]*/
    HI_U32 au32ZoneSum[AWB_ZONE_ROW][AWB_ZONE_COLUMN];      /*RO, Zone WB output population*/
} ISP_WB_RGB_STATISTICS_INFO_S; 

typedef struct hiISP_WB_STATISTICS_S
{
    ISP_WB_BAYER_STATISTICS_INFO_S stBayerStatistics;
    ISP_WB_RGB_STATISTICS_INFO_S   stRGBStatistics;     /* Hi3518EV200 does't support RGB Statistics */
} ISP_WB_STATISTICS_S;

// TODO: need to check
typedef struct hiISP_FOCUS_ZONE_S
{
    HI_U16  u16v1;
    HI_U16  u16h1;
    HI_U16  u16v2;
    HI_U16  u16h2;
    HI_U16  u16y;
} ISP_FOCUS_ZONE_S;
typedef struct hiISP_FOCUS_STATISTICS_S
{
    ISP_FOCUS_ZONE_S stZoneMetrics[AF_ZONE_ROW][AF_ZONE_COLUMN]; /*RO, The zoned measure of contrast*/
} ISP_FOCUS_STATISTICS_S;

typedef struct hiISP_STATISTICS_S
{
    ISP_AE_STATISTICS_S         stAEStat;   
    ISP_WB_STATISTICS_S         stWBStat;
    ISP_FOCUS_STATISTICS_S      stFocusStat;
} ISP_STATISTICS_S;

typedef struct hiISP_INIT_ATTR_S 
{
    HI_U16 u16WBRgain;
    HI_U16 u16WBGgain;
    HI_U16 u16WBBgain;
}ISP_INIT_ATTR_S ;

/*ISP debug information*/
typedef struct hiISP_DEBUG_INFO_S
{
    HI_BOOL bDebugEn;       /*RW, 1:enable debug, 0:disable debug*/
    HI_U32  u32PhyAddr;     /*RW, phy address of debug info */
    HI_U32  u32Depth;       /*RW, depth of debug info */
} ISP_DEBUG_INFO_S;

typedef struct hiISP_DBG_ATTR_S
{
    HI_U32  u32Rsv;         /* need to add member */
} ISP_DBG_ATTR_S;

typedef struct hiISP_DBG_STATUS_S
{
    HI_U32  u32FrmNumBgn;
    HI_U32  u32Rsv;         /* need to add member */
    HI_U32  u32FrmNumEnd;
} ISP_DBG_STATUS_S;

typedef enum hiISP_SNS_TYPE_E
{
    ISP_SNS_I2C_TYPE = 0,
    ISP_SNS_SSP_TYPE,
    
    ISP_SNS_TYPE_BUTT,
} ISP_SNS_TYPE_E;

typedef struct hiISP_I2C_DATA_S
{
    HI_BOOL bUpdate;
    HI_U8   u8DelayFrmNum;
    
    HI_U8   u8DevAddr;
    HI_U32  u32RegAddr;
    HI_U32  u32AddrByteNum;
    HI_U32  u32Data;
    HI_U32  u32DataByteNum;
} ISP_I2C_DATA_S;

typedef struct hiISP_SSP_DATA_S
{
    HI_BOOL bUpdate;
    HI_U8   u8DelayFrmNum;
    
    HI_U32  u32DevAddr;
    HI_U32  u32DevAddrByteNum;
    HI_U32  u32RegAddr;
    HI_U32  u32RegAddrByteNum;
    HI_U32  u32Data;
    HI_U32  u32DataByteNum;
} ISP_SSP_DATA_S;

typedef struct hiISP_SNS_REGS_INFO_S
{
    ISP_SNS_TYPE_E enSnsType;
    HI_U32  u32RegNum;
    HI_U8   u8Cfg2ValidDelayMax;

    union
    {
        ISP_I2C_DATA_S astI2cData[ISP_MAX_SNS_REGS];
        ISP_SSP_DATA_S astSspData[ISP_MAX_SNS_REGS];
    };
} ISP_SNS_REGS_INFO_S;

typedef struct hiISP_VD_INFO_S
{
  HI_U32  u32Reserved;                       /*RO, Range: [0x0, 0xFFFFFFFF] */
}ISP_VD_INFO_S;


typedef struct hiISP_REG_ATTR_S
{
    HI_U32 u32IspRegAddr;
    HI_U32 u32IspRegSize;
    HI_U32 u32IspExtRegAddr;
    HI_U32 u32IspExtRegSize;
    HI_U32 u32AeExtRegAddr;
    HI_U32 u32AeExtRegSize;
    HI_U32 u32AwbExtRegAddr;
    HI_U32 u32AwbExtRegSize;
} ISP_REG_ATTR_S;


/******************************************************/
/********************* AI structs ************************/

typedef enum hiISP_IRIS_TYPE_E
{
    ISP_IRIS_DC_TYPE = 0,
    ISP_IRIS_P_TYPE,
    
    ISP_IRIS_TYPE_BUTT,
} ISP_IRIS_TYPE_E;

typedef enum hiISP_IRIS_F_NO_E
{
    ISP_IRIS_F_NO_32_0 = 0,
    ISP_IRIS_F_NO_22_0,
    ISP_IRIS_F_NO_16_0,
    ISP_IRIS_F_NO_11_0,
    ISP_IRIS_F_NO_8_0,
    ISP_IRIS_F_NO_5_6,
    ISP_IRIS_F_NO_4_0,
    ISP_IRIS_F_NO_2_8,
    ISP_IRIS_F_NO_2_0,
    ISP_IRIS_F_NO_1_4,
    ISP_IRIS_F_NO_1_0,    
    
    ISP_IRIS_F_NO_BUTT,
} ISP_IRIS_F_NO_E;

typedef struct hiISP_DCIRIS_ATTR_S
{
    HI_S32 s32Kp;                    /*RW, Range:[0, 100000], the proportional gain of PID algorithm, default value is 7000 */
    HI_S32 s32Ki;                    /*RW, Range:[0, 1000], the integral gain of PID algorithm, default value is 100 */
    HI_S32 s32Kd;                    /*RW, Range:[0, 100000], the derivative gain of PID algorithm, default value is 3000 */
    HI_U32 u32MinPwmDuty;            /*RW, Range:[0, 1000], which is the min pwm duty for dciris control */
    HI_U32 u32MaxPwmDuty;            /*RW, Range:[0, 1000], which is the max pwm duty for dciris control */
    HI_U32 u32OpenPwmDuty;           /*RW, Range:[0, 1000], which is the open pwm duty for dciris control */
} ISP_DCIRIS_ATTR_S;

typedef struct hiISP_PIRIS_ATTR_S
{
    HI_BOOL bStepFNOTableChange;    /*WO, Step-F number mapping table change or not */
    HI_BOOL bZeroIsMax;             /*RW, Step 0 corresponds to max aperture or not, it's related to the specific iris */
    HI_U16 u16TotalStep;            /*RW, Range:[1, 1024], Total steps of  Piris's aperture, it's related to the specific iris */
    HI_U16 u16StepCount;            /*RW, Range:[1, 1024], Used steps of Piris's aperture. when Piris's aperture is too small, the F number precision is not enough, 
                                          so not all the steps can be used. It's related to the specific iris*/
    HI_U16 au16StepFNOTable[AI_MAX_STEP_FNO_NUM];   /*RW, Range:[0, 1024], Step-F number mapping table. F1.0 is expressed as 1024, F32 is expressed as 1, it's related to the specific iris*/
    ISP_IRIS_F_NO_E  enMaxIrisFNOTarget;  /*RW, Range:[F32.0, F1.0], Max F number of Piris's aperture, it's related to the specific iris */
    ISP_IRIS_F_NO_E  enMinIrisFNOTarget;  /*RW, Range:[F32.0, F1.0], Min F number of Piris's aperture, it's related to the specific iris */
    
    HI_BOOL bFNOExValid;            /*RW, use equivalent gain to present FNO or not */
    HI_U32 u32MaxIrisFNOTarget;    /*RW, Range:[1, 1024], Max equivalent gain of F number of Piris's aperture, only used when bFNOExValid is true, it's related to the specific iris */
    HI_U32 u32MinIrisFNOTarget;    /*RW, Range:[1, 1024], Min equivalent gain of F number of Piris's aperture, only used when bFNOExValid is true, it's related to the specific iris */
} ISP_PIRIS_ATTR_S;

typedef struct hiISP_MI_ATTR_S
{
    HI_U32  u32HoldValue;            /*RW, Range: [0x0, 0x3E8], iris hold value for DC-iris*/
    ISP_IRIS_F_NO_E  enIrisFNO;      /*RW, the F number of P-iris*/
} ISP_MI_ATTR_S;

typedef enum hiISP_IRIS_STATUS_E
{
    ISP_IRIS_KEEP  = 0,       /* In this mode can set the MI holdvalue */
    ISP_IRIS_OPEN  = 1,       /* Open Iris to the max */
    ISP_IRIS_CLOSE = 2,       /* Close Iris to the min */
    ISP_IRIS_BUTT
} ISP_IRIS_STATUS_E;

typedef struct hiISP_IRIS_ATTR_S
{
    HI_BOOL bEnable;                 /* iris enable/disable */
    ISP_OP_TYPE_E   enOpType;        /* auto iris or manual iris */
    ISP_IRIS_TYPE_E enIrisType;      /* DC-iris or P-iris */
    ISP_IRIS_STATUS_E enIrisStatus;  /*RW, status of Iris*/
    ISP_MI_ATTR_S   stMIAttr;
} ISP_IRIS_ATTR_S;


/******************************************************/
/********************* AE structs ************************/
typedef enum hiISP_AE_MODE_E
{
    AE_MODE_SLOW_SHUTTER = 0,
    AE_MODE_FIX_FRAME_RATE  = 1,
    AE_MODE_BUTT
} ISP_AE_MODE_E;

typedef enum hiISP_AE_STRATEGY_E
{
    AE_EXP_HIGHLIGHT_PRIOR = 0,
    AE_EXP_LOWLIGHT_PRIOR  = 1,
    AE_STRATEGY_MODE_BUTT
} ISP_AE_STRATEGY_E;

typedef struct hiISP_AE_RANGE_S
{
    HI_U32 u32Max;
    HI_U32 u32Min;
} ISP_AE_RANGE_S;

typedef struct hiISP_AE_DELAY_S
{
    HI_U16 u16BlackDelayFrame;    /*RW, Range: [0x0, 0xFFFF], AE black delay frame count*/
    HI_U16 u16WhiteDelayFrame;    /*RW, Range: [0x0, 0xFFFF], AE white delay frame count*/
} ISP_AE_DELAY_S;

typedef enum hiISP_ANTIFLICKER_MODE_E
{
    /* The epxosure time is fixed to be the multiplies of 1/(2*frequency) sec,
     * it may lead to over exposure in the high-luminance environments. */
    ISP_ANTIFLICKER_NORMAL_MODE = 0x0,

    /* The anti flicker may be closed to avoid over exposure in the high-luminance environments. */
    ISP_ANTIFLICKER_AUTO_MODE = 0x1,

    ISP_ANTIFLICKER_MODE_BUTT
}ISP_ANTIFLICKER_MODE_E;

typedef struct hiISP_ANTIFLICKER_S
{
    HI_BOOL bEnable;
    HI_U8   u8Frequency;          /*RW, Range:[0x0,0xFF], usually this value is 50 or 60  which is the frequency of the AC power supply*/
    ISP_ANTIFLICKER_MODE_E  enMode;
} ISP_ANTIFLICKER_S;

typedef struct hiISP_SUBFLICKER_S
{
    HI_BOOL bEnable;

    /* RW, Range: [0x0, 0x64], if subflicker mode enable, current luma is less than AE compensation plus LumaDiff, 
      AE will keep min antiflicker shutter time(for example: 1/100s or 1/120s) to avoid flicker. while current luma is 
      larger than AE compensation plus the LumaDiff, AE will reduce shutter time to avoid over-exposure and introduce 
      flicker in the pircture */
    HI_U8   u8LumaDiff;          
} ISP_SUBFLICKER_S;


typedef enum hiISP_FSWDR_MODE_E
{
    /* Normal FSWDR mode */
    ISP_FSWDR_NORMAL_MODE = 0x0,

    /* Long frame mode, only effective in LINE_WDR, when running in this mode FSWDR module only output the long frame data */
    ISP_FSWDR_LONG_FRAME_MODE = 0x1,

    ISP_FSWDR_MODE_BUTT
}ISP_FSWDR_MODE_E;

 
typedef struct hiISP_AE_ATTR_S
{
    /* base parameter */
    ISP_AE_RANGE_S stExpTimeRange;   /*RW,  sensor exposure time (unit: us ), Range: [0x0, 0xFFFFFFFF], it's related to the specific sensor */
    ISP_AE_RANGE_S stAGainRange;     /*RW,  sensor analog gain (unit: times, 10bit precision), Range : [0x400, 0xFFFFFFFF], it's related to the specific sensor */
    ISP_AE_RANGE_S stDGainRange;     /*RW,  sensor digital gain (unit: times, 10bit precision), Range : [0x400, 0xFFFFFFFF], it's related to the specific sensor */
    ISP_AE_RANGE_S stISPDGainRange;  /*RW,  ISP digital gain (unit: times, 10bit precision), Range : [0x400, 0x40000], it's related to the ISP digital gain range */
    ISP_AE_RANGE_S stSysGainRange;   /*RW,  system gain (unit: times, 10bit precision), Range : [0x400, 0xFFFFFFFF], it's related to the specific sensor and ISP Dgain range */
    HI_U32 u32GainThreshold;         /*RW,  Gain threshold for slow shutter mode (unit: times, 10bit precision), Range : [0x400, 0xFFFFFFFF] */

    HI_U8  u8Speed;                  /*RW, Range: [0x0, 0xFF], AE adjust step for dark scene to bright scene switch */
    HI_U16  u16BlackSpeedBias;       /*RW, Range: [0x0, 0xFFFF], AE adjust step bias for bright scene to dark scene switch */
    HI_U8  u8Tolerance;              /*RW, Range: [0x0, 0xFF], AE adjust tolerance*/
    HI_U8  u8Compensation;           /*RW, Range: [0x0, 0xFF], AE compensation*/ 
    HI_U16  u16EVBias;               /*RW, Range: [0x0, 0xFFFF], AE EV bias*/
    ISP_AE_STRATEGY_E enAEStrategyMode;  /*RW, Support Highlight prior or Lowlight prior*/
    HI_U16  u16HistRatioSlope;       /*RW, Range: [0x0, 0xFFFF], AE hist ratio slope*/
    HI_U8   u8MaxHistOffset;         /*RW, Range: [0x0, 0xFF], Max hist offset*/
  
    ISP_AE_MODE_E  enAEMode;         /*RW, AE mode(slow shutter/fix framerate)(onvif)*/
    ISP_ANTIFLICKER_S stAntiflicker;
    ISP_SUBFLICKER_S stSubflicker;
    ISP_AE_DELAY_S stAEDelayAttr;       

    HI_BOOL bManualExpValue;         /*RW, manual exposure value or not*/
    HI_U32 u32ExpValue;              /*RW, Range: (0x0, 0xFFFFFFFF], manual exposure value */ 

    /*AE weighting table*/
    HI_U8 au8Weight[AE_ZONE_ROW][AE_ZONE_COLUMN]; /*Range :  [0, 0xF]*/
} ISP_AE_ATTR_S;

typedef struct hiISP_ME_ATTR_S
{
    ISP_OP_TYPE_E enExpTimeOpType;
    ISP_OP_TYPE_E enAGainOpType;
    ISP_OP_TYPE_E enDGainOpType;
    ISP_OP_TYPE_E enISPDGainOpType;

    HI_U32 u32ExpTime;        /*RW,  sensor exposure time (unit: us ), Range: [0x0, 0xFFFFFFFF], it's related to the specific sensor */
    HI_U32 u32AGain;          /*RW,  sensor analog gain (unit: times, 10bit precision), Range : [0x400, 0xFFFFFFFF], it's related to the specific sensor */
    HI_U32 u32DGain;          /*RW,  sensor digital gain (unit: times, 10bit precision), Range : [0x400, 0xFFFFFFFF], it's related to the specific sensor */
    HI_U32 u32ISPDGain;       /*RW,  ISP digital gain (unit: times, 10bit precision), Range : [0x400, 0x40000], it's related to the ISP digital gain range */
} ISP_ME_ATTR_S;

typedef struct hiISP_EXPOSURE_ATTR_S
{    
    HI_BOOL         bByPass;
    ISP_OP_TYPE_E   enOpType;
    HI_U8     u8AERunInterval;         /*RW, Range: [0x1, 0xFF], set the AE run interval*/
    HI_BOOL   bHistStatAdjust;         /*RW, HI_TRUE: 256 bin histogram statistic config will adjust when large red or blue area detected. 
                                                 HI_FALSE: 256 bin histogram statistic config will not change */
    HI_BOOL   bAERouteExValid;         /*RW,  use extend AE route or not */
    ISP_ME_ATTR_S   stManual;
    ISP_AE_ATTR_S   stAuto;    
} ISP_EXPOSURE_ATTR_S;


#define ISP_AE_ROUTE_MAX_NODES (16)
typedef struct hiISP_AE_ROUTE_NODE_S
{
    HI_U32  u32IntTime;     /*RW,  sensor exposure time (unit: us ), Range: (0x0, 0xFFFFFFFF], it's related to the specific sensor */
    HI_U32  u32SysGain;     /*RW,  system gain (unit: times, 10bit precision), Range : [0x400, 0xFFFFFFFF], it's related to the specific sensor and ISP Dgain range */
    ISP_IRIS_F_NO_E  enIrisFNO;  /*RW, the F number of the iris's aperture, Range:[F32.0, F1.0], only support for Piris */
    HI_U32  u32IrisFNOLin;  /*RW, the equivalent gain of F number of the iris's aperture, Range:[1,1024], only support for Piris */
} ISP_AE_ROUTE_NODE_S;

typedef struct hiISP_AE_ROUTE_S
{
    HI_U32 u32TotalNum;     /*RW,  Range: [0x0, 0x10], total node number of AE route */ 
    ISP_AE_ROUTE_NODE_S astRouteNode[ISP_AE_ROUTE_MAX_NODES];
} ISP_AE_ROUTE_S;

#define ISP_AE_ROUTE_EX_MAX_NODES (16)
typedef struct hiISP_AE_ROUTE_EX_NODE_S
{
    HI_U32  u32IntTime;     /*RW,  sensor exposure time (unit: us ), Range: (0x0, 0xFFFFFFFF], it's related to the specific sensor */
    HI_U32  u32Again;       /*RW,  sensor analog gain (unit: times, 10bit precision), Range : [0x400, 0x3FFFFF], it's related to the specific sensor*/
    HI_U32  u32Dgain;       /*RW,  sensor digital gain (unit: times, 10bit precision), Range : [0x400, 0x3FFFFF], it's related to the specific sensor*/
    HI_U32  u32IspDgain;    /*RW,  ISP digital gain (unit: times, 10bit precision), Range : [0x400, 0x40000] */
    ISP_IRIS_F_NO_E  enIrisFNO;  /*RW, the F number of the iris's aperture, Range:[F32.0, F1.0], only support for Piris */
    HI_U32  u32IrisFNOLin;  /*RW, the equivalent gain of F number of the iris's aperture, Range:[1,1024], only support for Piris */
} ISP_AE_ROUTE_EX_NODE_S;

typedef struct hiISP_AE_ROUTE_EX_S
{
    HI_U32 u32TotalNum;       /*RW,  Range: [0x0, 0x10], total node number of extend AE route */ 
    ISP_AE_ROUTE_EX_NODE_S astRouteExNode[ISP_AE_ROUTE_EX_MAX_NODES];
} ISP_AE_ROUTE_EX_S;

typedef struct hiISP_EXP_INFO_S
{
    HI_U32 u32ExpTime;              /* RO, Range: [0x0, 0xFFFFFFFF] */  
    HI_U32 u32AGain;                /* RO, Range: [0x400, 0xFFFFFFFF] */                
    HI_U32 u32DGain;                /* RO, Range: [0x400, 0xFFFFFFFF] */            
    HI_U32 u32ISPDGain;             /* RO, Range: [0x400, 0x40000] */            
    HI_U32 u32Exposure;             /* RO, Range: [0x400, 0xFFFFFFFF] */  
    HI_BOOL bExposureIsMAX;         /* RO, Range: [0x0, 0x1]*/
    HI_S16 s16HistError;            /* RO, Range: [-0x8000, 0x7FFF]*/               
    HI_U32 u32AE_Hist256Value[256]; /* RO, 256 bins histogram */
    HI_U16 u16AE_Hist5Value[5];     /* not support */
    HI_U8  u8AveLum;                /* RO, Range: [0x0, 0xFF]*/
    HI_U32 u32LinesPer500ms;        /* RO, Range: [0x0, 0xFFFFFFFF], exposure lines per 500ms */ 
    HI_U32 u32PirisFNO;             /* RO, Range: [0x0, 0x400] */  
    HI_U32 u32Fps;                  /* RO, actual fps */  
    HI_U32 u32ISO;                  /* RO, Range: [0x64, 0xFFFFFFFF] */
    HI_U32 u32FirstStableTime;      /* RO, AE first stable time for quick start */
    ISP_AE_ROUTE_S stAERoute;       /* RO, Actual AE route */
    ISP_AE_ROUTE_EX_S stAERouteEx;  /* RO, Actual AE route_ex */
}ISP_EXP_INFO_S;


/********************* AWB structs ************************/
typedef enum hiISP_AWB_ALG_TYPE_E
{
    AWB_ALG_LOWCOST = 0,
    AWB_ALG_ADVANCE = 1,
    AWB_ALG_BUTT
} ISP_AWB_ALG_TYPE_E;

typedef enum hiISP_AWB_MULTI_LS_TYPE_E
{
    AWB_MULTI_LS_SAT = 0,
    AWB_MULTI_LS_CCM = 1,
    AWB_MULTI_LS_BUTT
} ISP_AWB_MULTI_LS_TYPE_E;

typedef struct hiISP_AWB_CT_LIMIT_ATTR_S
{
    HI_BOOL bEnable;
    ISP_OP_TYPE_E   enOpType;

    HI_U16 u16HighRgLimit;     /*RW, Range:[0x0, 0xFFF], in Manual Mode, user define the Max Rgain of High Color Temperature*/
    HI_U16 u16HighBgLimit;     /*RW, Range:[0x0, 0xFFF], in Manual Mode, user define the Min Bgain of High Color Temperature*/
    HI_U16 u16LowRgLimit;      /*RW, Range:[0x0, u16HighRgLimit), in Manual Mode, user define the Min Rgain of Low Color Temperature*/
    HI_U16 u16LowBgLimit;      /*RW, Range:(u16HighBgLimit, 0xFFF], in Manual Mode, user define the Max Bgain of Low Color Temperature*/
} ISP_AWB_CT_LIMIT_ATTR_S;

typedef struct hiISP_AWB_IN_OUT_ATTR_S
{
    HI_BOOL bEnable;
    ISP_OP_TYPE_E   enOpType;
    HI_BOOL bOutdoorStatus;              /*in Auto mode, this is RO, in Manual mode, this is WO*/
    HI_U32 u32OutThresh;                 /*shutter time(in us) to judge indoor or outdoor */
    HI_U16 u16LowStart;                  /*5000K is recommend*/
    HI_U16 u16LowStop;                   /*4500K is recommend, should be smaller than u8LowStart*/
    HI_U16 u16HighStart;                 /*6500K is recommend, shoule be larger than u8LowStart*/
    HI_U16 u16HighStop;                  /*8000K is recommend, should be larger than u8HighStart*/
    HI_BOOL bGreenEnhanceEn;             /*If this is enabled, Green channel will be enhanced based on the ratio of green plant*/
    HI_U8   u8OutShiftLimit;             /*Max white point zone distance to Planckian Curve*/	
} ISP_AWB_IN_OUT_ATTR_S;

typedef struct hiISP_AWB_CBCR_TRACK_ATTR_S
{
    HI_BOOL bEnable;                          /*If enabled, statistic parameter cr, cb will change according to iso*/
    
    HI_U16  au16CrMax[ISP_AUTO_ISO_STENGTH_NUM];   /*RW, Range:[0x0, 0xFFFF], au16CrMax[i] >= au16CrMin[i]*/
    HI_U16  au16CrMin[ISP_AUTO_ISO_STENGTH_NUM];   /*RW, Range:[0x0, 0xFFFF]*/
    HI_U16  au16CbMax[ISP_AUTO_ISO_STENGTH_NUM];   /*RW, Range:[0x0, 0xFFFF], au16CbMax[i] >= au16CbMin[i]*/
    HI_U16  au16CbMin[ISP_AUTO_ISO_STENGTH_NUM];   /*RW, Range:[0x0, 0xFFFF]*/
} ISP_AWB_CBCR_TRACK_ATTR_S;

typedef struct hiISP_AWB_LUM_HISTGRAM_ATTR_S
{
    HI_BOOL bEnable;                          /*If enabled, zone weight to awb is combined with zone luma*/
    ISP_OP_TYPE_E   enOpType;                 /*In auto mode, the weight distribution follows Gaussian distribution*/
    HI_U8   au8HistThresh[6];                 /*In manual mode, user define luma thresh, thresh[0] = 0, thresh[5] = 0xFF,  thresh[i] >=  thresh[i-1]*/
    HI_U16  au16HistWt[6];                    /*In manual mode, user define luma weight. Range:[0x0, 0xFFFF]*/
} ISP_AWB_LUM_HISTGRAM_ATTR_S;

typedef struct hiISP_AWB_LIGHTSOURCE_INFO_S
{
    HI_U16 u16WhiteRgain;         /*G/R of White points at this light source, RW, Range:[0x0, 0xFFF]*/
    HI_U16 u16WhiteBgain;         /*G/B of White points at this light source, RW, Range:[0x0, 0xFFF]*/
    HI_U16 u16ExpQuant;           /*shutter time * again * dgain >> 4, Not support Now*/
    HI_U8  u8LightStatus;         /*RW, 0: idle  1:add light source  2:delete sensitive color */
    HI_U8  u8Radius;              /*Radius of light source, RW, Range:[0x0, 0xFF]*/
} ISP_AWB_EXTRA_LIGHTSOURCE_INFO_S;

typedef struct hiISP_AWB_ATTR_EX_S
{  
    HI_U8  u8Tolerance;                                 /*RW, Range:[0x0, 0xFF], AWB adjust tolerance*/   
    HI_U8  u8ZoneRadius;                                /*RW, Range:[0x0, 0xFF], radius of AWB blocks*/  
    HI_U16 u16CurveLLimit;                              /*RW, Range:[0x0, 0x100],   Left limit of AWB Curve, recomend for indoor 0xE0, outdoor 0xE0*/ 
    HI_U16 u16CurveRLimit;                              /*RW, Range:[0x100, 0xFFF], Right Limit of AWB Curve,recomend for indoor 0x130, outdoor 0x120*/ 
 
    HI_BOOL  bExtraLightEn;                             /*Enable special light source function*/
    ISP_AWB_EXTRA_LIGHTSOURCE_INFO_S    stLightInfo[4]; 
    ISP_AWB_IN_OUT_ATTR_S               stInOrOut;

    HI_BOOL bMultiLightSourceEn;                        /*If enabled, awb will do special process in multi light source enviroment*/ 
    ISP_AWB_MULTI_LS_TYPE_E enMultiLSType;              /*Saturation or CCM Tunning*/
    HI_U16  u16MultiLSScaler;                           /*In saturation type, it means the max saturation it can achieve, in ccm type, it means the strenght of multi process. [0x0, 0x100]*/ 
    HI_U16  au16MultiCTBin[8];                          /*AWB Support divide the color temperature range by 8 bins*/
    HI_U16  au16MultiCTWt[8];                           /*Weight for different color temperature, same value of 8 means CT weight does't work, [0x0, 0x400]*/

    HI_BOOL bFineTunEn;                                 /*If enabled, skin color scene will be optimized*/
    HI_U8   u8FineTunStrength;                          /*larger value means better performance of skin color scene, but will increase error probability in low color temperature scene */	
} ISP_AWB_ATTR_EX_S;

typedef struct hiISP_AWB_ATTR_S
{
    HI_BOOL bEnable;               /*If AWB is disabled, static wb gain will be used, otherwise auto wb gain will be used*/

    HI_U16 u16RefColorTemp;        /*Calibration Information*/
    HI_U16 au16StaticWB[4];        /*Calibration Information, Range:[0x0, 0xFFF]*/
    HI_S32 as32CurvePara[6];       /*Calibration Information, as32CurvePara[3] != 0, as32CurvePara[4]==128*/

    ISP_AWB_ALG_TYPE_E          enAlgType;

    HI_U8  u8RGStrength;                            /*RW, AWB Strength of R Channel, Range: [0x0, 0xFF]*/
    HI_U8  u8BGStrength;                            /*RW, AWB Strength of B Channel, Range: [0x0, 0xFF]*/
    HI_U16 u16Speed;                                /*RW, Convergence speed of AWB, Range:[0x0, 0xFFF] */
    HI_U16 u16ZoneSel;                              /*RW,  A value of 0 or 0xFF means global AWB, A value between 0 and 0xFF means zoned AWB */
    HI_U16 u16HighColorTemp;                        /*RW, AWB max temperature, Recommended: [8500, 10000] */
    HI_U16 u16LowColorTemp;                         /*RW, AWB min temperature, Range: [0x0, u8HighColorTemp), Recommended: [2000, 2500] */
    ISP_AWB_CT_LIMIT_ATTR_S   stCTLimit;
    HI_BOOL bShiftLimitEn;                          /*RW, If enabled, when the statistic information is out of range, it should be project back*/
    HI_U8  u8ShiftLimit;                            /*RW, planckian curve range, Range: [0x0, 0xFF], Recommended: [0x30, 0x50] */
    HI_BOOL bGainNormEn;                            /*RW, if enabled, the min of RGB gain is fixed. */

    ISP_AWB_CBCR_TRACK_ATTR_S stCbCrTrack;
    ISP_AWB_LUM_HISTGRAM_ATTR_S stLumaHist;
} ISP_AWB_ATTR_S;

typedef struct hiISP_MWB_ATTR_S        
{
    HI_U16 u16Rgain;            /*RW, Multiplier for R  color channel, Range: [0x0, 0xFFF]*/
    HI_U16 u16Grgain;           /*RW, Multiplier for Gr color channel, Range: [0x0, 0xFFF]*/
    HI_U16 u16Gbgain;           /*RW, Multiplier for Gb color channel, Range: [0x0, 0xFFF]*/
    HI_U16 u16Bgain;            /*RW, Multiplier for B  color channel, Range: [0x0, 0xFFF]*/
} ISP_MWB_ATTR_S;

typedef struct hiISP_WB_ATTR_S
{
    HI_BOOL bByPass;
    HI_U8   u8AWBRunInterval;       /*RW, Range: [0x1, 0xFF], set the AWB run interval*/
    ISP_OP_TYPE_E   enOpType;       /* manual or auto mode */  
    ISP_MWB_ATTR_S  stManual;
    ISP_AWB_ATTR_S  stAuto;
} ISP_WB_ATTR_S;

typedef struct hiISP_COLORMATRIX_MANUAL_S
{  
    HI_BOOL bSatEn;             /*If bSatEn=1, the active CCM = SatMatrix * ManualMatrix, else tha active CCM =  ManualMatrix*/
    HI_U16 au16CCM[9];          /*RW,  Range: [0x0,  0xFFFF]*/
} ISP_COLORMATRIX_MANUAL_S;

typedef struct hiISP_COLORMATRIX_AUTO_S
{ 
    HI_BOOL bISOActEn;           /*if enabled, CCM will bypass in low light*/
    HI_BOOL bTempActEn;          /*if enabled, CCM will bypass when color temperate is larger than 10K or less than 2500K*/ 
    
    HI_U16 u16HighColorTemp;    /*RW,  Range: <=10000*/
    HI_U16 au16HighCCM[9];      /*RW,  Range: [0x0,  0xFFFF]*/
    HI_U16 u16MidColorTemp;     /*RW,  the MidColorTemp should be at least 400 smaller than HighColorTemp*/
    HI_U16 au16MidCCM[9];       /*RW,  Range: [0x0,  0xFFFF]*/
    HI_U16 u16LowColorTemp;     /*RW,  the LowColorTemp should be at least 400 smaller than u16MidColorTemp, >= 2000*/
    HI_U16 au16LowCCM[9];       /*RW,  Range: [0x0,  0xFFFF]*/
} ISP_COLORMATRIX_AUTO_S;

typedef struct hiISP_COLORMATRIX_ATTR_S
{
    ISP_OP_TYPE_E enOpType;
    ISP_COLORMATRIX_MANUAL_S stManual;
    ISP_COLORMATRIX_AUTO_S stAuto;   
}ISP_COLORMATRIX_ATTR_S;

typedef struct hiISP_SATURATION_MANUAL_S
{
    HI_U8   u8Saturation;        /*RW,  Range: [0, 0xFF] */
} ISP_SATURATION_MANUAL_S;

typedef struct hiISP_SATURATION_AUTO_S
{
    HI_U8   au8Sat[ISP_AUTO_ISO_STENGTH_NUM];           /*RW,  Range: [0, 0xFF], should be decreased based on ISO increase */ 
} ISP_SATURATION_AUTO_S;

typedef struct hiISP_SATURATION_ATTR_S
{
    ISP_OP_TYPE_E enOpType;
    ISP_SATURATION_MANUAL_S stManual;
    ISP_SATURATION_AUTO_S stAuto;   
}ISP_SATURATION_ATTR_S;

typedef struct hiISP_COLOR_TONE_ATTR_S
{
    HI_U16 u16RedCastGain;          /*RW, R channel gain except AWB gain*/
    HI_U16 u16GreenCastGain;        /*RW, G channel gain except AWB gain*/
    HI_U16 u16BlueCastGain;         /*RW, B channel gain except AWB gain*/
} ISP_COLOR_TONE_ATTR_S;

typedef struct hiISP_WB_INFO_S
{
    HI_U16 u16Rgain;                /*RO, AWB result of R color channel, Range: [0x0, 0xFFF]*/
    HI_U16 u16Grgain;               /*RO, AWB result of Gr color channel, Range: [0x0, 0xFFF]*/
    HI_U16 u16Gbgain;               /*RO, AWB result of Gb color channel, Range: [0x0, 0xFFF]*/
    HI_U16 u16Bgain;                /*RO, AWB result of B color channel, Range: [0x0, 0xFFF]*/
    HI_U16 u16Saturation;           /*RO, Current saturation, Range:[0x0, 0xFF]*/
    HI_U16 u16ColorTemp;            /*RO, Detect color temperature, maybe out of color cemeprature range*/
    HI_U16 au16CCM[9];              /*RO, Current color correction matrix*/

    HI_U16 u16LS0CT;                /*RO, color tempearture of primary light source*/
    HI_U16 u16LS1CT;                /*RO, color tempearture of secondary light source*/
    HI_U16 u16LS0Area;              /*RO, area of primary light source*/
    HI_U16 u16LS1Area;              /*RO, area of secondary light source*/
    HI_U8  u8MultiDegree;           /*RO, 0 means uniform light source, larger value means multi light source*/ 
}ISP_WB_INFO_S;


/********************* AF structs ************************/
typedef struct hiISP_AF_ATTR_S
{
    HI_S32 s32DistanceMax; /* the focuse range */
    HI_S32 s32DistanceMin;

    /* weighting table */
    HI_U8 u8Weight[AF_ZONE_ROW][AF_ZONE_COLUMN];
} ISP_AF_ATTR_S;
typedef struct hiISP_MF_ATTR_S
{
    HI_S32 s32DefaultSpeed;     /* 1,default speed(unit:m/s).(onvif)*/    
} ISP_MF_ATTR_S;
typedef struct hiISP_FOCUS_ATTR_S
{
    ISP_OP_TYPE_E  enOpType;
    ISP_MF_ATTR_S  stManual;
    ISP_AF_ATTR_S  stAuto;
} ISP_FOCUS_ATTR_S;


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /* __HI_COMM_ISP_H__ */

