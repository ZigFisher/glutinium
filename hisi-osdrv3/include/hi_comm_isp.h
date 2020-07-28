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
#define MG_ZONE_ROW          (15)
#define MG_ZONE_COLUMN       (17)
#define AWB_ZONE_ROW         (32)
#define AWB_ZONE_COLUMN      (32)

#define LIGHTSOURCE_NUM      (4)

#define STATIC_DP_COUNT_MAX  (2048)
#define DRC_TM_NODE_NUM      (200)
#define GAMMA_NODE_NUM       (257)
#define GAMMA_FE0_NODE_NUM   (33)
#define GAMMA_FE1_NODE_NUM   (257)
#define SHADING_MESH_NUM     (289)

#define DEFOG_ZONE_ROW       (32)
#define DEFOG_ZONE_COLUMN    (32)
#define DEFOG_ZONE_NUM       (1024)
#define DEFOG_MAX_ZONE_NUM   (1024)

#define ISP_AUTO_ISO_STRENGTH_NUM (16)
#define ISP_SHARPEN_IN_OUT_LEN		(2)
#define ISP_YUV_SHPLUMA_NUM  (32)

#define ISP_MAX_SNS_REGS     (32)
#define ISP_MAX_DEV_NUM      (1)

#define ISP_CAC_MAX_LUT_SIZE (64)

#define WDR_MAX_FRAME_NUM  (2)


#define BAYER_PATTER_NUM  (4)
#define BAYER_CALIBTAION_MAX_NUM  (50)

#define HI_ISP_BAYERNR_LUT_DIVISOR (1000)

#define HI_ISP_BAYERNR_STRENGTH_DIVISOR (100)


#define AI_MAX_STEP_FNO_NUM  (1024)
#define ISP_BAS_TAPS_ROW_NUM (17)
#define ISP_BAS_TAPS_COL_6   (6)
#define ISP_BAS_TAPS_COL_4   (4)

#define HI_ISP_NR_LUMA_LUT_LENGTH (6)
#define HI_ISP_BAYERNR_LUT_LENGTH (33)

#define HI_ISP_CA_YRATIO_LUT_LENGTH (128)

#define HI_ISP_NDDM_LUT_LENGTH     (17) 

#define ISP_BAYER_CHN_NUM   (4)
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
        HI_U32  bitBypassAntiFC     : 1 ;   /* [3] */
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
        HI_U32  bitBypassCA         : 1 ;   /* [16] */
        HI_U32  bitRsv0            : 2 ;   /* [17:18] */
        HI_U32  bitBypassCsConv     : 1 ;   /* [19] */
        HI_U32  bitRsv1            : 2 ;   /* [20:21] */
        HI_U32  bitBypassSharpen    : 1 ;   /* [22] */
        HI_U32  bitBypassCAC	    : 1 ;   /* [23] */
        HI_U32  bitChnSwitch        : 1 ;   /* [24] */
        HI_U32  bitRsv2            : 2 ;   /* [25:26] */
        HI_U32  bitRsv3            : 2 ;   /* [27:28] */
        HI_U32  bitRsv4            : 3 ;   /* [29:31] */
    };
}ISP_MODULE_CTRL_U;


typedef enum hiISP_COMBINE_MODE_E
{
    FS_WDR_COMBINE_SHORT_FIRST = 0, /* Short exposure data is used when combining */
    FS_WDR_COMBINE_LONG_FIRST = 1,  /* Long exposure data is used when combining */
    FS_WDR_COMBINE_BUTT
} ISP_COMBINE_MODE_E;



typedef struct hiISP_FSWDR_MANUAL_ATTR_S
{
   HI_U16     u16MDClipThr;          /*RW,u14.0,Range:[0x0,0x3FFF] .If the frame difference value below the threshold ,it will be set zero.*/
   HI_U8      u8MDLowThr;          /*RW,u4.2,Range:[0x0, 0x3F]*/
   HI_U8      u8MDHighThr;	          /*RW,u4.2,Range:[0x0, 0x3F]*/
   HI_U8      u8MDBldRatio;	         /*RW,u5.0,Range:[0x0, 0x1F]*/

} ISP_FSWDR_MANUAL_ATTR_S;

typedef struct hiISP_FSWDR_AUTO_ATTR_S
{
    HI_U16    au16MDClipThr[ISP_AUTO_ISO_STRENGTH_NUM];        /*RW,u14.0,Range:[0x0,0x3FFF] .If the frame difference value below the threshold ,it will be set zero.*/
    HI_U8     au8MDLowThr[ISP_AUTO_ISO_STRENGTH_NUM];       /*RW,u4.2,Range:[0x0, 0x3F]*/
    HI_U8     au8MDHighThr[ISP_AUTO_ISO_STRENGTH_NUM];      /*RW,u4.2,Range:[0x0, 0x3F]*/
    HI_U8     au8MDBldRatio[ISP_AUTO_ISO_STRENGTH_NUM];        /*RW,u5.0,Range:[0x0, 0x1F]*/
    
} ISP_FSWDR_AUTO_ATTR_S;

typedef struct hiISP_WDR_FS_ATTR_S
{
    HI_BOOL   bMotionComp;            /* RW, HI_TRUE: enable motion compensation; HI_FLASE: disable motion compensation*/
    HI_U16    u16ShortThr;              /* RW,u14.0, Range: [0x0, 0x3FFF]. Data above this threshold will be taken from short exposure only.
                                                      This value is normally not changed and should be left at the default value */
    HI_U16    u16LongThr;             /* RW,u14.0, Range: [0x0, u16ShortThresh]. Data below this threshold will be taken from long exposure only.
                                                       This value is normally not changed and should be left at the default value */
    HI_U16    u16MDSfNrThr;           /*RW,u9.0,Range:[0,0x1FF].If the motion degree is below this threshold,the short frame will be denoised*/
    HI_BOOL   bMDRefNoise;             /*RW,HI_TRUE: the motion detection refer to the noise LUT;HI_FALSE:the motion detection will not refer to the noise LUT*/													   	

    ISP_OP_TYPE_E enOpType;
    ISP_FSWDR_MANUAL_ATTR_S stManual;
    ISP_FSWDR_AUTO_ATTR_S     stAuto;						   
    ISP_COMBINE_MODE_E enFSWDRComMode; 
	
} ISP_WDR_FS_ATTR_S;




typedef struct hiISP_DRC_MANUAL_ATTR_S
{
    HI_U8  u8Strength;          /* RW, Range: [0, 0xFF]. Strength of dynamic range compression. 
                                          Higher values lead to higher differential gain between shadows and highlights. */
} ISP_DRC_MANUAL_ATTR_S;
typedef struct hiISP_DRC_AUTO_ATTR_S
{
    HI_U8 u8Strength;          /* RW, Range: [0, 0xFF]. It is the base strength. The strength used in ISP is generated by firmware. 
                                        In linear mode, strength = f1(u8Strength, histogram, ISO)
                                        In sensor WDR mode: strength = f2(u8Strength, histogram, ISO)
                                        In 2to1 WDR mode: strength = f3(ExpRatio, ISO) */
    HI_U8 u8StrengthMax; /* RW, Range: [0x0, 0xFF]. Maximum DRC strength in Auto mode */
    HI_U8 u8StrengthMin; /* RW, Range: [0x0, 0xFF]. Minimum DRC strength in Auto mode */
    
} ISP_DRC_AUTO_ATTR_S;

typedef struct hiISP_DRC_ATTR_S
{
    HI_BOOL bEnable;

    HI_BOOL bUserTmEnable;
    HI_BOOL bUserTmUpdate;

    HI_U16 u16DarkGainLmtC;       /* RW, Range: [0x0, 0x85]. Chroma gain limit for dark area */
    HI_U16 u16DarkGainLmtY;       /* RW, Range: [0x0, 0x85]. Luma gain limit for dark area */      
    HI_U16 u16BrightGainLmt;      /* RW, Range: [0x0, 0xA0]. Gain lower limit for bright area */
    HI_U8  u8GainClipStep;        /* RW, Range: [0x0,  0xF]. Gain clip step for bright area */
    HI_U8  u8GainClipKnee;        /* RW, Range: [0x0,  0xF]. Gain clip knee for bright area */

	HI_U8  u8SpatialVar;          /* RW, Range: [0x0, 0xF]. Controls the frequency of the spatial filter */
	HI_U8  u8RangeVar;            /* RW, Range: [0x0, 0xF]. Controls the frequency of the range filter */
    HI_U8  u8SpatialVarFlt1;     /* RW, Range: [0x0, 0xF]. Controls the frequency of the  filter */
    HI_U8  u8RangeVarFlt1;       /* RW, Range: [0x0, 0xF]. Controls the frequency of the  filter */
    HI_U8  u8SpatialVarFlt2;     /* RW, Range: [0x0, 0xF]. Controls the frequency of the  filter */
    HI_U8  u8RangeVarFlt2;       /* RW, Range: [0x0, 0xF]. Controls the frequency of the  filter */
    HI_U8  u8FltBldr;            /* RW, Range: [0x0, 0xF]. Controls the frequency of the  filter */
    HI_U8  u8SpatialStr;         /* RW, Range: [0x0, 0xF]. Controls the frequency of the  filter */
	 
	HI_U8  u8Asymmetry;           /* RW, Range: [0x1,  0x1E]. The parameter0 of DRC tone mapping curve */
	HI_U8  u8SecondPole;          /* RW, Range: [0x96, 0xD2]. The parameter1 of DRC tone mapping curve */
	HI_U8  u8Stretch;             /* RW, Range: [0x1E, 0x3C]. The parameter2 of DRC tone mapping curve */
	HI_U8  u8Compress;            /* RW, Range: [0x64, 0xC8]. The parameter3 of DRC tone mapping curve */

	HI_U8  u8PDStrength;          /* RW, Range: [0x0,  0x80]. Controls the purple detection strength */
 
	HI_U8  u8LocalMixingBrigtht;  /* RW, Range: [0x0, 0x40]. Controls the gain of the details where the value of the area is greater than a certain threshold value */
    HI_U8  u8LocalMixingBrigthtMax;  /* RW, Range: [0x0, 0x80]. Controls the gain of the details where the value of the area is greater than a certain threshold value */
	HI_U8  u8LocalMixingDark;     /* RW, Range: [0x0, 0x80]. Controls the gain of the details where the value of the area is smaller than a certain threshold value */
	HI_U8  u8LocalMixingDarkMin;     /* RW, Range: [0x0, 0x80]. Controls the gain of the details where the value of the area is smaller than a certain threshold value */
    HI_U8  u8LocalMixingThres;    /* RW, Range: [0x0, 0x80]. Threshold for amplitude-dependent detail gain control */

    HI_U16  u16ColorCorrectionLut[33]; /* RW,Range: [0x0, 0x400] */
    HI_U16  au16ToneMappingValue[DRC_TM_NODE_NUM]; /* RW, Range: [0x0, 0xffff] */

    ISP_OP_TYPE_E enOpType;
    ISP_DRC_MANUAL_ATTR_S stManual;
    ISP_DRC_AUTO_ATTR_S   stAuto;
} ISP_DRC_ATTR_S;

typedef struct hiISP_MOD_PARAM_S
{
	HI_U32		proc_param;
	HI_U32      u32UpdatePos;
	HI_U32      port_init_delay;         
}ISP_MOD_PARAM_S;


typedef struct hiISP_CA_ATT_S
{
    HI_BOOL   bEnable;

	HI_U16    au16YRatioLut[128];  //1.10bit  Y Ratio For UV ; Max = 2047 FW Limit
	HI_S32    as32ISORatio[16];          //1.10bit  ISO Ratio  For UV ,Max = 2047 FW Limi

}ISP_CA_ATTR_S;


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
    HI_U8   u8StartThresh;        /* RW, Range: [1, 0xFF]. Start threshold for static defect-pixel calibraiton.Default Value:3 */
    HI_U16  u16CountMax;          /* RW, Range: [0, 0x800]. Limit of max number of static defect-pixel calibraiton.Default Value:1024. */
    HI_U16  u16CountMin;          /* RW, Range: [0, u16CountMax]. Limit of min number of static defect-pixel calibraiton.Default Value:1. */
    HI_U16  u16TimeLimit;         /* RW, Range: [0x0, 0x640].Time limit for static defect-pixel calibraiton, in frame number.Default Value:1600(0x640). */

    HI_U32      au32Table[STATIC_DP_COUNT_MAX];  /* RO, Static defect-pixel calibraiton table,the first 13 bits represents the X coordinate of the defect pixel, the second 12 bits represent the Y coordinate of the defect pixel. */
    HI_U8       u8FinishThresh;   /* RO, Range: [0, 0xFF]. Finish threshold for static defect-pixel calibraiton. */    
    HI_U16      u16Count;         /* RO, Range: [0, 0x800]. Finish number for static defect-pixel calibraiton. */
    ISP_STATUS_E enStatus;        /* RO, Status of static defect-pixel calibraiton.Default Value:0(0x0). */
} ISP_DP_STATIC_CALIBRATE_S;

typedef struct hiISP_DP_STATIC_ATTR_S
{
    HI_BOOL bEnable;				/* RW. Enable/disable the static defect-pixel module.Default Value:1(0x1). */
    HI_U16  u16BrightCount;       /* RW, When used as input(W), indicate the number of static bright defect pixels; As output(R),indicate the number of static bright and dark defect pixels */
    HI_U16  u16DarkCount;         /* RW, When used as input(W), indicate the number of static dark defect pixels; As output(R), invalid value 0 */    
    HI_U32  au32BrightTable[STATIC_DP_COUNT_MAX];   /* RW, Range: [0x0, 0x1FFFFFF],the first 13 bits represents the X coordinate of the defect pixel, the second 12 bits represent the Y coordinate of the defect pixel
                                                                    Notice : When used as input(W), indicate static bright defect pixels table;  As output(R), indicate static bright and dark defect pixels table */
    HI_U32  au32DarkTable[STATIC_DP_COUNT_MAX];     /* RW, Range: [0x0, 0x1FFFFFF],the first 13 bits represents the X coordinate of the defect pixel, the second 12 bits represent the Y coordinate of the defect pixel
                                                                    Notice : When used as input(W), indicate static dark defect pixels table;  As output(R), invalid value */
    HI_BOOL bShow;					/*RW,highlight static defect pixel,default value:0x0*/
} ISP_DP_STATIC_ATTR_S;

typedef struct hiISP_DP_DYNAMIC_MANUAL_ATTR_S
{
	HI_U16	u16Strength;			/* RW, Range: [0, 255] */
	HI_U16	u16Thresh;			  	/* not support */
	HI_U16	u16LineThresh;		  	/* not support */
	HI_U16	u16HpThresh;		  	/* not support */  
	HI_U16	u16BlendRatio;		  	/* RW, Range: [0, 255] */

} ISP_DP_DYNAMIC_MANUAL_ATTR_S;

typedef struct hiISP_DP_DYNAMIC_AUTO_ATTR_S
{
	HI_U16	au16Strength[ISP_AUTO_ISO_STRENGTH_NUM];	/* RW, Range: [0, 255] */
	HI_U16	u16Thresh;			                       	/* not support */
	HI_U16	u16LineThresh;		                       	/* not support */
	HI_U16	u16HpThresh;		                      	/* not support */  
	HI_U16	au16BlendRatio[ISP_AUTO_ISO_STRENGTH_NUM];	/* RW, Range: [0, 256] */
} ISP_DP_DYNAMIC_AUTO_ATTR_S;

typedef struct hiISP_DP_DYNAMIC_ATTR_S
{
    HI_BOOL bEnable;			/* RW, Range: [0, 1] */
	HI_BOOL bSupTwinkleEn;		/* RW, Range: [0, 1]*/
	HI_S8  	s8SoftThr; 			/* RW, Range: [-128,127 ] */
	HI_U8   u8SoftSlope;		/* RW, Range: [0, 255] */
    ISP_OP_TYPE_E enOpType;
    ISP_DP_DYNAMIC_MANUAL_ATTR_S stManual;
    ISP_DP_DYNAMIC_AUTO_ATTR_S   stAuto;
} ISP_DP_DYNAMIC_ATTR_S;


typedef struct hiISP_DIS_ATTR_S
{
    HI_BOOL bEnable;
} ISP_DIS_ATTR_S;

/* Shading Calibration */
typedef enum hiISP_BAYER_RAW_E
{
    BAYER_RAW_8BIT  = 8,
    BAYER_RAW_10BIT = 10,
    BAYER_RAW_12BIT = 12,
    BAYER_RAW_14BIT = 14,
    BAYER_RAW_16BIT = 16,
    BAYER_RAW_BUTT    
} ISP_BAYER_RAW_E; 

typedef enum hiISP_LSC_GS_MODE
{
	LSC_GS_GEOMETRIC = 0,
	LSC_GS_MANUAL = 1,
	LSC_GS_BUTT
} ISP_LSC_GS_MODE;


typedef struct hiISP_LSC_CALIBRATION_CFG_S 
{
    ISP_LSC_GS_MODE enGSModeX;
    ISP_LSC_GS_MODE enGSModeY;
    ISP_BAYER_FORMAT_E enBayer;   /*Bayer Pattern RGGB:0 GRBG:1 GBRG:2 BGGR:3*/
    ISP_BAYER_RAW_E    enRawBit;  /*Raw bit bandwidth,Range:[8,10,12,14,16]*/

    HI_U16 u16ImageHeight;       /*Image Height Range:[0,0xFFFF]    */
    HI_U16 u16ImageWidth;        /*Image Width  Range:[0,0xFFFF]  */   

    HI_U16 u16BLCOffsetR;        /*Black level for R channel of 12bit,Range:[0,0xFFF)*/
    HI_U16 u16BLCOffsetGr;       /*Black level for Gr channel of 12bit,Range:[0,0xFFF)*/
    HI_U16 u16BLCOffsetGb;       /*Black level for Gb channel of 12bit,Range:[0,0xFFF)*/
    HI_U16 u16BLCOffsetB;        /*Black level for B channel of 12bit,Range:[0,0xFFF)*/

    HI_FLOAT fCommRatioX;        
    HI_FLOAT fCommRatioY;

    HI_U8  au8GridStepX[8];  /*Range:[1, 50]*/
    HI_U8  au8GridStepY[8];  /*Range:[1, 50]*/
}ISP_LSC_CALIBRATION_CFG_S;

typedef struct hiISP_MESH_SHADING_TABLE_S
{
    HI_U32  au32XGridWidth[8];/*RW, Range:[0x0, u32Width/4]*/
    HI_U32  au32YGridWidth[8];/*RW, Range:[0x0, u32Height/4]*/

	HI_U32  au32RGain[SHADING_MESH_NUM];  /*RW, Range:[0x0, 0x1FFF]*/
	HI_U32  au32GrGain[SHADING_MESH_NUM]; /*RW, Range:[0x0, 0x1FFF]*/
	HI_U32  au32GbGain[SHADING_MESH_NUM]; /*RW, Range:[0x0, 0x1FFF]*/
	HI_U32  au32BGain[SHADING_MESH_NUM];  /*RW, Range:[0x0, 0x1FFF]*/

}ISP_MESH_SHADING_TABLE_S;


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
    HI_U16  u16MeshStrength; 
} ISP_SHADING_ATTR_S;



typedef enum hiISP_IRPOS_TYPE_E
{
    ISP_IRPOS_TYPE_GR = 0x0,
    ISP_IRPOS_TYPE_GB,
    ISP_IRPOS_TYPE_BUTT
}ISP_IRPOS_TYPE_E;



typedef struct hiISP_NR_AUTO_ATTR_S
{
	HI_U8	 au8ChromaStr[BAYER_PATTER_NUM][ISP_AUTO_ISO_STRENGTH_NUM];  //[0, 3]      //Strength of Chrmoa noise reduction	
	HI_U8    au8FineStr[ISP_AUTO_ISO_STRENGTH_NUM];                      //[0, 128]    //Strength of luma noise reduction
	HI_U16   au16CoringWeight[ISP_AUTO_ISO_STRENGTH_NUM];                //[0, 3200]   //Strength of reserving the random noise 
	
} ISP_NR_AUTO_ATTR_S;

typedef struct hiISP_NR_MANUAL_ATTR_S
{			 
	HI_U8	 u8ChromaStr[BAYER_PATTER_NUM]; //[0, 3]      //Strength of Chrmoa noise reduction
	HI_U8    u8FineStr;                     //[0, 128]    //Strength of luma noise reduction
	HI_U16   u16CoringWeight;               //[0, 3200]   //Strength of reserving the random noise 
} ISP_NR_MANUAL_ATTR_S;

typedef struct hiISP_NR_WDR_ATTR_S
{			 
	HI_U8	 au8WDRCoarseStr[WDR_MAX_FRAME_NUM];  //[0, 80] //Denoise strength of long frame and short frame in wdr mode
}ISP_NR_WDR_ATTR_S;

typedef struct hiISP_NR_ATTR_S
{			 
	HI_BOOL bEnable;		                            //[0, 1]     //NR enable
	HI_U16  au16CoarseStr [BAYER_PATTER_NUM];           //[0, 1000]  //Coarse strength of noise reduction
	HI_U16  au16CoringRatio[HI_ISP_BAYERNR_LUT_LENGTH]; //[0, 1000]  //Strength of reserving the random noise according to luma
	  
	ISP_OP_TYPE_E enOpType;
	ISP_NR_AUTO_ATTR_S  stAuto;
	ISP_NR_MANUAL_ATTR_S stManual;
	ISP_NR_WDR_ATTR_S stWDR;
}ISP_NR_ATTR_S;

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
	HI_U16 u16SharpenUd;		//Undirectional sharpen strength for undirectional detail enhancement. [0, 1023]	
	HI_U8 u8SharpenD;           //Directional sharpen strength for directional detail and edge enhancement.	 [0, 255]		
	HI_U8 u8TextureThr;         //Sharpen threshold for SharpenUd and SharpenD, noise is deemed as below the TextureThd without sharpness. [0, 255]     
	HI_U8 u8SharpenEdge;        //Sharpen strength for strong edge enhancement.	[0, 255]			
	HI_U8 u8EdgeThr;            //Sharpen threshold to distinguish the strong edge, only the edge bigger than the EdgeThd can be sharpen by SharpenEdge.  [0, 255]	
	HI_U8 u8OverShoot;          //u8OvershootAmt		[0, 255]
	HI_U8 u8UnderShoot;         //u8UndershootAmt		[0, 255]
	HI_U8 u8shootSupStr;        //overshoot and undershoot suppression strength, the amplitude and width of shoot will be decrease when shootSupSt increase.  	[0, 255]	
	HI_U8 u8DetailCtrl;         //Different sharpen strength for detail and edge. When it is bigger than 128, detail sharpen strength will be stronger than edge.  [0, 255]
      HI_U8 u8RGain;          	/* RW; range: [0, 31]; Format:0.5;Sharpen Gain for Red Area*/ 
      HI_U8 u8BGain;          	/* RW; range: [0, 31]; Format:0.5;Sharpen Gain for Blue Area*/
      HI_U8 u8SkinGain;			/* RW; range: [0, 255]; Format:8.0;Sharpen Gain for Skin Area*/
	HI_U8 u8EdgeFiltStr;               /* RW; range: [0, 127]; Format:7.0;Edge noise suppression strength*/
	HI_U8 u8JagCtrl;                     /* RW; range: [0, 255]; Format:8.0;Edge Jag Control*/
	HI_U8 u8NoiseLumaCtrl;          /* RW; range: [0, 31]; Format:5.0;Noise suppression based on the LumaWgt*/
} ISP_SHARPEN_MANUAL_ATTR_S;


typedef struct hiISP_SHARPEN_AUTO_ATTR_S

{   	
	HI_U16 au16SharpenUd[ISP_AUTO_ISO_STRENGTH_NUM];	//Undirectional sharpen strength for undirectional detail enhancement. [0, 1023]	
	HI_U8 au8SharpenD[ISP_AUTO_ISO_STRENGTH_NUM];		//Directional sharpen strength for directional detail and edge enhancement.	 [0, 255]		
	HI_U8 au8TextureThr[ISP_AUTO_ISO_STRENGTH_NUM];		//Sharpen threshold for SharpenUd and SharpenD, noise is deemed as below the TextureThd without sharpness. [0, 255]     
	HI_U8 au8SharpenEdge[ISP_AUTO_ISO_STRENGTH_NUM];	//Sharpen strength for strong edge enhancement.	[0, 255]			
	HI_U8 au8EdgeThr[ISP_AUTO_ISO_STRENGTH_NUM];		//Sharpen threshold to distinguish the strong edge, only the edge bigger than the EdgeThd can be sharpen by SharpenEdge.  [0, 255]	
	HI_U8 au8OverShoot[ISP_AUTO_ISO_STRENGTH_NUM];		//u8OvershootAmt		[0, 255]
	HI_U8 au8UnderShoot[ISP_AUTO_ISO_STRENGTH_NUM];		//u8UndershootAmt		[0, 255]
	HI_U8 au8shootSupStr[ISP_AUTO_ISO_STRENGTH_NUM]; 	//overshoot and undershoot suppression strength, the amplitude and width of shoot will be decrease when shootSupSt increase.  	[0, 255]	
	HI_U8 au8DetailCtrl[ISP_AUTO_ISO_STRENGTH_NUM];  	//Different sharpen strength for detail and edge. When it is bigger than 128, detail sharpen strength will be stronger than edge.  [0, 255]
      HI_U8 au8RGain[ISP_AUTO_ISO_STRENGTH_NUM];          /* RW; range: [0, 31]; Format:0.5;Sharpen Gain for Red Area*/  
      HI_U8 au8BGain[ISP_AUTO_ISO_STRENGTH_NUM];          /* RW; range: [0, 31]; Format:0.5;Sharpen Gain for Blue Area*/
      HI_U8 au8SkinGain[ISP_AUTO_ISO_STRENGTH_NUM];       /* RW; range: [0, 255]; Format:8.0;Sharpen Gain for Skin Area*/
	HI_U8 au8EdgeFiltStr[ISP_AUTO_ISO_STRENGTH_NUM];               /* RW; range: [0, 127]; Format:7.0;Edge noise suppression strength*/
	HI_U8 au8JagCtrl[ISP_AUTO_ISO_STRENGTH_NUM];                     /* RW; range: [0, 255]; Format:8.0;Edge Jag Control*/
	HI_U8 au8NoiseLumaCtrl[ISP_AUTO_ISO_STRENGTH_NUM];          /* RW; range: [0, 31]; Format:5.0;Noise suppression based on the LumaWgt*/
}ISP_SHARPEN_AUTO_ATTR_S;



typedef struct hiISP_SHARPEN_ATTR_S
{
    HI_BOOL bEnable;
    HI_U8 u8LumaWgt[ISP_YUV_SHPLUMA_NUM];
    ISP_OP_TYPE_E enOpType;
    ISP_SHARPEN_MANUAL_ATTR_S stManual;
    ISP_SHARPEN_AUTO_ATTR_S   stAuto;
} ISP_SHARPEN_ATTR_S;



/*Crosstalk Removal*/
typedef struct hiISP_CR_ATTR_S
{
    HI_BOOL  bEnable;
                         
    HI_U8    u8Slope;            /*flat  */                   
    HI_U8    u8SensiSlope;      /*edge */                      
    HI_U16   u16SensiThr;  /*edge */  
    HI_U16   au16Strength[ISP_AUTO_ISO_STRENGTH_NUM];     
    HI_U16   au16Threshold[ISP_AUTO_ISO_STRENGTH_NUM];  /*flat  */         
    HI_U16   au16NpOffset[ISP_AUTO_ISO_STRENGTH_NUM];     
}ISP_CR_ATTR_S;


typedef struct hiISP_ANTI_FALSECOLOR_MANUAL_S
{
	HI_U8 u8Strength;  
	HI_U8 u8Threshold;
   HI_U16 u16Offset;
} ISP_ANTI_FALSECOLOR_MANUAL_S;

typedef struct hiISP_ANTI_FALSECOLOR_AUTO_ATTR_S
{
	HI_U8 au8Strength[ISP_AUTO_ISO_STRENGTH_NUM]; 
	HI_U8 au8Threshold[ISP_AUTO_ISO_STRENGTH_NUM];
   HI_U16 au16Offset[ISP_AUTO_ISO_STRENGTH_NUM];
} ISP_ANTI_FALSECOLOR_AUTO_ATTR_S;


typedef struct hiISP_ANTI_FALSECOLOR_S
{
    HI_BOOL bEnable;
	ISP_OP_TYPE_E enOpType;
    ISP_ANTI_FALSECOLOR_MANUAL_S stManual;
    ISP_ANTI_FALSECOLOR_AUTO_ATTR_S stAuto;
} ISP_ANTI_FALSECOLOR_S;



typedef struct hiISP_DEMOSAIC_MANUAL_ATTR_S
{                                                                                  
    HI_U16 u16EdgeSmoothThr;     //[0x0, 0x3FE]    //Threshold for smoothing the rough edges or edge noise
    HI_U16 u16EdgeSmoothSlope;   //[0x0, 0x3FF]    //Slope of decreasing the edge smooth strength
    HI_U16 u16AntiAliasThr;      //[0x0, 0x3FE]    //Threshold to decreasing the strength of moire pattern removing in high frequency areas
    HI_U16 u16AntiAliasSlope;    //[0x0, 0x3FF]    //Slope of increasing strength for removing the moire pattern 

	HI_U16 u16NrCoarseStr;       //[0, 0x100]      //Coarse strength for noise suppressing and detail enhancement
    HI_U8  u8DetailEnhanceStr;   //[0, 0xa]        //Strength for weak detail enhancement
	HI_U16 u16NoiseSuppressStr;  //[0, 0x3FF]      //Fine strength for noise suppressing
	HI_U16 u16SharpenLumaStr;    //[0, 0x200]      //Strength of luma sharpening in high saturation regions
	HI_U16 u16ColorNoiseCtrlThr; //[0, 0x12c]     //Threshold for dark color noise control
}ISP_DEMOSAIC_MANUAL_ATTR_S;  

typedef struct hiISP_DEMOSAIC_AUTO_ATTR_S
{                                        
    HI_U16  au16EdgeSmoothThr[ISP_AUTO_ISO_STRENGTH_NUM];     //[0x0, 0x3FE]    //Threshold for smoothing the rough edges or edge noise
    HI_U16  au16EdgeSmoothSlope[ISP_AUTO_ISO_STRENGTH_NUM];   //[0x0, 0x3FF]    //Slope of decreasing the edge smooth strength
    HI_U16  au16AntiAliasThr[ISP_AUTO_ISO_STRENGTH_NUM];      //[0x0, 0x3FE]    //Threshold to decreasing the strength of moire pattern removing in high frequency areas
    HI_U16  au16AntiAliasSlope[ISP_AUTO_ISO_STRENGTH_NUM];    //[0x0, 0x3FF]    //Slope of increasing strength for removing the moire pattern 

	HI_U16  au16NrCoarseStr[ISP_AUTO_ISO_STRENGTH_NUM];       //[0, 0x100]      //Coarse strength for noise suppressing and detail enhancement
    HI_U8   au8DetailEnhanceStr[ISP_AUTO_ISO_STRENGTH_NUM];   //[0, 0xa]        //Strength for weak detail enhancement
    HI_U16  au16NoiseSuppressStr[ISP_AUTO_ISO_STRENGTH_NUM];  //[0, 0x3FF]      //Fine strength for noise suppressing
    HI_U16  au16SharpenLumaStr[ISP_AUTO_ISO_STRENGTH_NUM];    //[0, 0x200]      //Strength of luma sharpening in high saturation regions
 	HI_U16  au16ColorNoiseCtrlThr[ISP_AUTO_ISO_STRENGTH_NUM]; //[0, 0x12c]     //Threshold for dark color noise control
 
}ISP_DEMOSAIC_AUTO_ATTR_S;                           

typedef struct hiISP_DEMOSAIC_ATTR_S
{                                    
    HI_BOOL bEnable;                          
    ISP_OP_TYPE_E enOpType;
    ISP_DEMOSAIC_MANUAL_ATTR_S stManual;
    ISP_DEMOSAIC_AUTO_ATTR_S stAuto;

}ISP_DEMOSAIC_ATTR_S;

typedef struct hiISP_BLACK_LEVEL_S 
{ 
    HI_U16 au16BlackLevel[4]; /* RW, Range: [0x0, 0xFFF]*/ 
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


/* AWB Calibration */
typedef struct hiISP_AWB_Calibration_Gain_S
{
	HI_U16 u16AvgRgain;    
	HI_U16 u16AvgBgain;
}ISP_AWB_Calibration_Gain_S;

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
    ISP_OP_TYPE_E      enOpType;
    ISP_DEFOG_MANUAL_ATTR_S stManual;
    ISP_DEFOG_AUTO_ATTR_S   stAuto;
    HI_BOOL bUserLutEnable;    /*RW,Range:[0,1],0:Auto Lut 1:User Lut*/
    HI_U8 au8DefogLut[256];       
}ISP_DEFOG_ATTR_S;


typedef struct hiISP_DEPURPLESTR_MANUAL_ATTR_S
{
	HI_U8 u8DePurpleCrStr;      /*RW, Range: [0,8]*/
	HI_U8 u8DePurpleCbStr;      /*RW, Range: [0,8]*/
}ISP_DEPURPLESTR_MANUAL_ATTR_S;

typedef struct hiISP_DEPURPLESTR_AUTO_ATTR_S
{
	HI_U8 au8DePurpleCrStr[ISP_AUTO_ISO_STRENGTH_NUM];   /*RW, Range: [0,8]*/
	HI_U8 au8DePurpleCbStr[ISP_AUTO_ISO_STRENGTH_NUM];   /*RW, Range: [0,8]*/
}ISP_DEPURPLESTR_AUTO_ATTR_S;

typedef struct hiISP_LOCAL_CAC_ATTR_S
{
	HI_BOOL bEnable;				/* RW, Range: [0,1]*/  
	HI_U16 u16PurpleDetRange;		/* RW, Range: [0,410]*/  
	HI_U16 u16VarThr;               /* RW, Range: [0, 4095]*/
	ISP_OP_TYPE_E      enOpType;
    ISP_DEPURPLESTR_MANUAL_ATTR_S stManual;
    ISP_DEPURPLESTR_AUTO_ATTR_S   stAuto;
} ISP_LOCAL_CAC_ATTR_S;


typedef struct hiISP_GLOBAL_CAC_ATTR_S
{
	HI_BOOL bEnable;		/* RW, Range: [0, 1]*/
	HI_U16 u16VerCoordinate;/* RW, Range: [0, ImageHeight - 1]*/    
	HI_U16 u16HorCoordinate;/* RW, Range: [0, ImageWidth - 1]*/      

	HI_S16 s16ParamRedA;	/* RW, Range: [-256, 255]*/
	HI_S16 s16ParamRedB;	/* RW, Range: [-256, 255]*/
	HI_S16 s16ParamRedC;	/* RW, Range: [-256, 255]*/
	HI_S16 s16ParamBlueA;	/* RW, Range: [-256, 255]*/
	HI_S16 s16ParamBlueB;	/* RW, Range: [-256, 255]*/
	HI_S16 s16ParamBlueC;	/* RW, Range: [-256, 255]*/
	HI_U8 u8VerNormShift;	/* RW, Range: [0, 7]*/
	HI_U8 u8VerNormFactor;	/* RW, Range: [0, 31]*/
	HI_U8 u8HorNormShift;	/* RW, Range: [0, 7]*/
	HI_U8 u8HorNormFactor;	/* RW, Range: [0, 31]*/

	HI_U16 u16CorVarThr;	/* RW, Range: [0, 4095]*/
} ISP_GLOBAL_CAC_ATTR_S;



/********************** ACM **************************************/

typedef struct hiISP_ACM_ATTR_S
{
    HI_BOOL         bEnable         ;
    HI_BOOL         bDemoEnable     ;/*0-debug closed; 1-debug open, the left screen is original video and the right screen is ACM-processed video*/
    HI_U32          u32Stretch      ; /*input data Clip range: 0-Y 64-940, C 64-960; 1-Y 0-1023, C 0-1023*/
    HI_U32          u32ClipRange    ; /*output data Clip range: 0-Y 64-940, C 64-960; 1-Y 0-1023, C 0-1023*/
    HI_U32          u32AcmClipOrWrap; /*0-wrap around; 1-clip*/
    HI_U32          u32CbcrThr      ; /* [0,255]*/
    HI_U32          u32GainLuma     ; /* [0,512]*/
    HI_U32          u32GainHue      ; /* [0,512]*/
    HI_U32          u32GainSat      ; /* [0,512]*/
}ISP_ACM_ATTR_S;

#define ACM_Y_NUM            9
#define ACM_S_NUM            13
#define ACM_H_NUM            29

typedef struct hiISP_ACM_LUT_S
{
    HI_S16 as16Y[ACM_Y_NUM][ACM_S_NUM][ACM_H_NUM];   /**<  */
    HI_S16 as16H[ACM_Y_NUM][ACM_S_NUM][ACM_H_NUM];   /**<  */
    HI_S16 as16S[ACM_Y_NUM][ACM_S_NUM][ACM_H_NUM];   /**<  */
}ISP_ACM_LUT_S;


typedef struct hiISP_INNER_STATE_INFO_S
{
   HI_U16 u16SharpenUd;
   HI_U8 u8SharpenD; 
   HI_U8 u8TextureThr; 
   HI_U8 u8SharpenEdge; 
   HI_U8 u8EdgeThr; 
   HI_U8 u8OverShoot; 
   HI_U8 u8UnderShoot; 
   HI_U8 u8ShootSupStr; 
   HI_U8 u8DetailCtrl; 
   HI_U8 u8ChromaStr[BAYER_PATTER_NUM]; 
   HI_U8 u8FineStr;					   
   HI_U16 u16CoringWeight;			
   HI_U32 u32DefogStrengthActual;        
   HI_U32 u32DRCStrengthActual;
   HI_U32 u32WDRExpRatioActual;    
   
   HI_BOOL bWDRSwitchFinish;          
   HI_BOOL bResSwitchFinish; 
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


typedef enum hiISP_AE_PRE_HIST_SWITCH_E
{
    ISP_AE_PRE_HIST_ENABLE           = 0,
    ISP_AE_PRE_HIST_DISABLE             ,
    ISP_AE_PRE_HIST_BUTT
} ISP_AE_PRE_HIST_SWITCH_E;

typedef enum hiISP_AE_FOUR_PLANE_MODE_E
{
    ISP_AE_FOUR_PLANE_MODE_DISABLE           = 0,
    ISP_AE_FOUR_PLANE_MODE_ENABLE             ,
    ISP_AE_FOUR_PLANE_MODE_BUTT
} ISP_AE_FOUR_PLANE_MODE_E;


typedef struct hiISP_AE_HIST_CONFIG_S
{
    HI_U8 u8HistSkipX;    /* Histogram decimation in horizontal direction: 0=every pixel; 1=every 2nd pixel; 2=every 
                                          3rd pixel; 3=every 4th pixel; 4=every 5th pixel; 5=every 8th pixel ; 6+=every 9th pixel */
    HI_U8 u8HistSkipY;    /* Histogram decimation in vertical direction: 0=every pixel; 1=every 2nd pixel; 2=every 
                                          3rd pixel; 3=every 4th pixel; 4=every 5th pixel; 5=every 8th pixel ; 6+=every 9th pixel */
    HI_U8 u8HistOffsetX;  /* 0= start from the first column; 1=start from second column */
    HI_U8 u8HistOffsetY;  /* 0= start from the first row; 1= start from second row */
} ISP_AE_HIST_CONFIG_S;

/* config of statistics structs */
typedef struct hiISP_AE_STATISTICS_CFG_S
{
    HI_U8  au8HistThresh[4];    				/*not support*/
    ISP_AE_SWITCH_E  enAESwitch;				/*RW, The position of Global 1024 bins histogram and  Global average and zone average in ISP pipeline*/ 
                                				/* 0 = After static WB;   */
                                				/* 4 = After DRC;      */
                                				/* 8 = After DG;  */
    ISP_AE_HIST_SWITCH_E  enHistSwitch; 		/*not support*/                                
    ISP_AE_SWITCH_E  enAESumSwitch; 			/*not support*/  
    ISP_AE_PRE_HIST_SWITCH_E  enPreHistSwitch; 	/*not support*/
    ISP_AE_HIST_CONFIG_S  stHistConfig;			/*RW, The configuration of histogram sampling;*/
    ISP_AE_FOUR_PLANE_MODE_E enFourPlaneMode; 	/*RW, Four Plane Mode Enable*/
												/*0 = Disabled; 1= Enabled*/
    HI_BOOL bAERootingEnable;
    HI_BOOL bMGRootingEnable;
} ISP_AE_STATISTICS_CFG_S;

/* 
   the main purpose of stat key was to access individual statistic info separately...
   ...for achieving performance optimization of CPU, because of we acquire stat...
   ... in ISP_DRV ISR for our firmware reference and USER can also use alternative MPI...
   ... to do this job, so bit1AeStat1~bit1AfStat for MPI behavior control, and bit16IsrAccess...
   ... for ISR access control, they were independent. but they have the same bit order, for example...
   ... bit1AeStat1 for MPI AeStat1 access key, and bit16 of u32Key for ISR AeStat1 access key 
*/
typedef union hiISP_STATISTICS_CTRL_U
{
    HI_U32  u32Key;
    struct
    {
        HI_U32  bit1AeStat1     : 1 ;   /* [0] */
        HI_U32  bit1AeStat2     : 1 ;   /* [1] */
        HI_U32  bit1AeStat3     : 1 ;   /* [2] */
        HI_U32  bit1AeStat4     : 1 ;   /* [3] */
        HI_U32  bit1AeStat5     : 1 ;   /* [4] */
        HI_U32  bit1AeStat6     : 1 ;   /* [5] */
        HI_U32  bit1AwbStat1    : 1 ;   /* [6] */
        HI_U32  bit1AwbStat2    : 1 ;   /* [7] */
        HI_U32  bit1AwbStat3    : 1 ;   /* [8] */
        HI_U32  bit1AwbStat4    : 1 ;   /* [9] */
        HI_U32  bit1AfStat      : 1 ;   /* [10] */
        HI_U32  bit3Rsv1        : 3 ;   /* [11:13] */
        HI_U32  bit1MgStat		: 1 ;   /* [14] */
        HI_U32  bit1Rsv2        : 1 ;   /* [15] */
        HI_U32  bit16IsrAccess  : 16;   /* [16:31] */
    };
}ISP_STATISTICS_CTRL_U;


/* statistics structs */

#define HIST_5_NUM      (5)
#define HIST_1024_NUM   (1024)
#define HIST_256_NUM    (256)
typedef struct hiISP_AE_STATISTICS_S
{   
    HI_U16 au16Hist5Value[HIST_5_NUM];                                   /*not support*/ /*RO, Global 5 bins histogram, Range: [0x0, 0xFFFF]*/
    HI_U16 au16ZoneHist5Value[AE_ZONE_ROW][AE_ZONE_COLUMN][HIST_5_NUM];  /*not support*/ /*RO, Zone 5 bins histogram, Range: [0x0, 0xFFFF]*/
    HI_U32 au32Hist1024Value[HIST_1024_NUM];                             /*RO, Global 1024 bins histogram, Range: [0x0, 0xFFFFFFFF]*/
    HI_U16 au16GlobalAvg[ISP_BAYER_CHN_NUM];                                    /*RO, Global average value, Range: [0x0, 0xFFFF]*/
    HI_U16 au16ZoneAvg[AE_ZONE_ROW][AE_ZONE_COLUMN][ISP_BAYER_CHN_NUM];         /*RO, Zone average value, Range: [0x0, 0xFFFF]*/
	HI_U32 au32PreChn0Hist256Value[HIST_256_NUM];						/*RO, Global 256 bins histogram of Ch1, Range: [0x0, 0xFFFFFFFF]*/
	HI_U32 au32PreChn1Hist256Value[HIST_256_NUM];						/*RO, Global 256 bins histogram of Ch2, Range: [0x0, 0xFFFFFFFF]*/
}ISP_AE_STATISTICS_S;

typedef struct hiISP_MG_STATISTICS_S
{   
    HI_U16 au16ZoneAvg[MG_ZONE_ROW][MG_ZONE_COLUMN][4];         /*RO, Zone average value, Range: [0x0, 0xFF]*/
}ISP_MG_STATISTICS_S;

typedef struct hiISP_WB_STATISTICS_CFG_PARA_S
{
    HI_U16 u16WhiteLevel;       /*RW, Upper limit of valid data for white region, Range: [0x0, 0xFFFF]*/
    HI_U16 u16BlackLevel;       /*RW, Lower limit of valid data for white region, Range: [0x0, u16WhiteLevel]*/
    HI_U16 u16CbMax;            /*RW, Maximum value of B/G for white region, Range: [0x0,0xFFF]*/
    HI_U16 u16CbMin;            /*RW, Minimum value of B/G for white region, Range: [0x0, u16CbMax]*/
    HI_U16 u16CrMax;            /*RW, Maximum value of R/G for white region, Range: [0x0, 0xFFF]*/
    HI_U16 u16CrMin;            /*RW, Minimum value of R/G for white region, Range: [0x0, u16CrMax]*/
    HI_U16 u16CbHigh;           /*Hi3516CV300 does't support hexagon limit*/  
    HI_U16 u16CbLow;            /*Hi3516CV300 does't support hexagon limit*/ 
    HI_U16 u16CrHigh;           /*Hi3516CV300 does't support hexagon limit*/   
    HI_U16 u16CrLow;            /*Hi3516CV300 does't support hexagon limit*/  
} ISP_WB_STATISTICS_CFG_PARA_S;

typedef struct hiISP_WB_STATISTICS_CFG_S
{
    ISP_WB_STATISTICS_CFG_PARA_S stBayerCfg;
    ISP_WB_STATISTICS_CFG_PARA_S stRGBCfg;       /*Hi3516CV300 does't support RGB Statistics*/
} ISP_WB_STATISTICS_CFG_S;


typedef struct hiISP_WB_BAYER_STATISTICS_S
{
    HI_U16 u16GlobalR;          /*RO, Global WB output Average R, Range: [0x0, 0xFFFF]*/   
    HI_U16 u16GlobalG;          /*RO, Global WB output Average G, Range: [0x0, 0xFFFF]*/  
    HI_U16 u16GlobalB;          /*RO, Global WB output Average B, Range: [0x0, 0xFFFF]*/  
    HI_U16 u16CountAll;         /*RO, normalized number of Gray points, Range: [0x0, 0xFFFF]*/  
    HI_U16 u16CountMin;         /*not support*/  
    HI_U16 u16CountMax;         /*not support*/  
           
    HI_U16 au16ZoneAvgR[AWB_ZONE_ROW][AWB_ZONE_COLUMN];            /*RO, Zone Average R, Range: [0x0, 0xFFFF]*/   
    HI_U16 au16ZoneAvgG[AWB_ZONE_ROW][AWB_ZONE_COLUMN];            /*RO, Zone Average G, Range: [0x0, 0xFFFF]*/   
    HI_U16 au16ZoneAvgB[AWB_ZONE_ROW][AWB_ZONE_COLUMN];            /*RO, Zone Average B, Range: [0x0, 0xFFFF]*/   
    HI_U16 au16ZoneCountAll[AWB_ZONE_ROW][AWB_ZONE_COLUMN];        /*RO, normalized number of Gray points, Range: [0x0, 0xFFFF]*/   
    HI_U16 au16ZoneCountMin[AWB_ZONE_ROW][AWB_ZONE_COLUMN];        /*not support*/  
    HI_U16 au16ZoneCountMax[AWB_ZONE_ROW][AWB_ZONE_COLUMN];        /*not support*/   
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
    ISP_WB_RGB_STATISTICS_INFO_S   stRGBStatistics;     /* Hi3516CV300 does't support RGB Statistics */
} ISP_WB_STATISTICS_S;

// TODO: need to check
typedef struct hiISP_FOCUS_ZONE_S
{
    HI_U16  u16v1;
    HI_U16  u16h1;
    HI_U16  u16v2;
    HI_U16  u16h2;
    HI_U16  u16y;
    HI_U16  u16HlCnt;
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
	ISP_MG_STATISTICS_S			stMGStat;
} ISP_STATISTICS_S;


typedef struct hiISP_STATISTICS_CFG_S
{
    ISP_STATISTICS_CTRL_U       unKey; 
    ISP_AE_STATISTICS_CFG_S     stAECfg;
    ISP_WB_STATISTICS_CFG_S     stWBCfg;
    ISP_FOCUS_STATISTICS_CFG_S  stFocusCfg;
} ISP_STATISTICS_CFG_S;

typedef struct hiISP_INIT_ATTR_S 
{
    HI_U16 u16WBRgain;
    HI_U16 u16WBGgain;
    HI_U16 u16WBBgain;
    HI_U16 u16SampleRgain;
    HI_U16 u16SampleBgain;
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
	HI_U8   u8IntPos;
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
	HI_U8   u8IntPos;
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

	/*Auto long frame mode, only effective in LINE_WDR, When running in this mode, normal WDR and long frame mode would auto switch*/
	ISP_FSWDR_AUTO_LONG_FRAME_MODE = 0x2,
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

    ISP_FSWDR_MODE_E enFSWDRMode;    /*RW, FSWDR running mode */
    HI_BOOL bWDRQuick;               /*RW, WDR use delay strategy or not; If is true, WDR AE adjust will be faster */

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
    HI_BOOL   bAERouteExValid;         /*RW, use extend AE route or not */
    ISP_ME_ATTR_S   stManual;
    ISP_AE_ATTR_S   stAuto;    
} ISP_EXPOSURE_ATTR_S;

typedef struct hiISP_WDR_EXPOSURE_ATTR_S
{
    ISP_OP_TYPE_E enExpRatioType;   /* RW, OP_TYPE_AUTO: The ExpRatio used in ISP is generated by firmware; OP_TYPE_MANUAL: The ExpRatio used in ISP is set by u32ExpRatio */
    HI_U32 u32ExpRatio;             /* RW, Range: [0x40, 0xFFF]. Format: unsigned 6.6-bit fixed-point. 0x40 means 1 times.
                                            When enExpRatioType is OP_TYPE_AUTO, u32ExpRatio is invalid.
                                            When enExpRatioType is OP_TYPE_MANUAL, u32ExpRatio is quotient of long exposure time / short exposure time. */
    HI_U32 u32ExpRatioMax;          /* RW, Range: [0x40, 0xFFF]. Format: unsigned 6.6-bit fixed-point. 0x40 means 1 times.
                                            When enExpRatioType is OP_TYPE_AUTO, u32ExpRatioMax is max(upper limit) of ExpRatio generated by firmware.
                                            When enExpRatioType is OP_TYPE_MANUAL, u32ExpRatioMax is invalid. */
    HI_U32 u32ExpRatioMin;          /* RW, Range: [0x40, u32ExpRatioMax]. 6bit precision, 0x40 means 1 times.
                                            When enExpRatioType is OP_TYPE_AUTO, u32ExpRatioMin is min(lower limit) of ExpRatio generated by firmware.
                                            When enExpRatioType is OP_TYPE_MANUAL, u32ExpRatioMin is invalid. */
    HI_U16 u16Tolerance;            /* RW, Range: [0x0, 0xFF], set the dynamic range tolerance. Format: unsigned 6.2-bit fixed-point. 0x4 means 1dB. */
    HI_U16 u16Speed;                /* RW, Range: [0x0, 0xFF], exposure ratio adjust speed */   
    HI_U16 u16RatioBias;            /* RW, Range: [0x0, 0xFFFF], exposure ratio bias */
} ISP_WDR_EXPOSURE_ATTR_S;

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
    HI_U32 u32LongExpTime;          /* RO, Range: [0x0, 0xFFFFFFFF] */
    HI_U32 u32AGain;                /* RO, Range: [0x400, 0xFFFFFFFF] */                
    HI_U32 u32DGain;                /* RO, Range: [0x400, 0xFFFFFFFF] */            
    HI_U32 u32ISPDGain;             /* RO, Range: [0x400, 0xFFFFFFFF] */            
    HI_U32 u32Exposure;             /* RO, Range: [0x40, 0xFFFFFFFF] */  
    HI_BOOL bExposureIsMAX;         /* RO, Range: [0x0, 0x1]*/
    HI_S16 s16HistError;            /* RO, Range: [-0x8000, 0x7FFF]*/               
    HI_U32 u32AE_Hist1024Value[1024]; /* RO, 1024 bins histogram */
    HI_U16 u16AE_Hist5Value[5];     /* not support */
    HI_U8  u8AveLum;                /* RO, Range: [0x0, 0xFF]*/
    HI_U32 u32LinesPer500ms;        /* RO, Range: [0x0, 0xFFFFFFFF], exposure lines per 500ms */ 
    HI_U32 u32PirisFNO;             /* RO, Range: [0x0, 0x400] */  
    HI_U32 u32Fps;                  /* RO, actual fps */  
    HI_U32 u32ISO;                  /* RO, Range: [0x64, 0xFFFFFFFF] */
    HI_U32 u32RefExpRatio;          /* RO, Range: [0x40, 0xFFF] */
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

typedef enum hiISP_AWB_INDOOR_OUTDOOR_STATUS_E
{
    AWB_INDOOR_MODE = 0,
    AWB_OUTDOOR_MODE = 1,
    AWB_INDOOR_OUTDOOR_BUTT
} ISP_AWB_INDOOR_OUTDOOR_STATUS_E;

typedef struct hiISP_AWB_CT_LIMIT_ATTR_S
{
    HI_BOOL bEnable;
    ISP_OP_TYPE_E   enOpType;

    HI_U16 u16HighRgLimit;     /*RW, Range:[0x0, 0xFFF], in Manual Mode, user define the Max Rgain of High Color Temperature*/
    HI_U16 u16HighBgLimit;     /*RW, Range:[0x0, 0xFFF], in Manual Mode, user define the Min Bgain of High Color Temperature*/
    HI_U16 u16LowRgLimit;      /*RW, Range:[0x0, 0xFFF], in Manual Mode, user define the Min Rgain of Low Color Temperature*/
    HI_U16 u16LowBgLimit;      /*RW, Range:[0x0, 0xFFF], in Manual Mode, user define the Max Bgain of Low Color Temperature*/
} ISP_AWB_CT_LIMIT_ATTR_S;

typedef struct hiISP_AWB_IN_OUT_ATTR_S
{
    HI_BOOL bEnable;
    ISP_OP_TYPE_E   enOpType;
    ISP_AWB_INDOOR_OUTDOOR_STATUS_E  enOutdoorStatus;              /*User should config indoor or outdoor status in Manual Mode*/
    HI_U32 u32OutThresh;                                          /*shutter time(in us) to judge indoor or outdoor */
    HI_U16 u16LowStart;                                           /*5000K is recommend*/
    HI_U16 u16LowStop;                                            /*4500K is recommend, should be smaller than u8LowStart*/
    HI_U16 u16HighStart;                                          /*6500K is recommend, shoule be larger than u8LowStart*/
    HI_U16 u16HighStop;                                           /*8000K is recommend, should be larger than u8HighStart*/
    HI_BOOL bGreenEnhanceEn;                                      /*If this is enabled, Green channel will be enhanced based on the ratio of green plant*/
    HI_U8   u8OutShiftLimit;                                      /*Max white point zone distance to Planckian Curve*/	
} ISP_AWB_IN_OUT_ATTR_S;

typedef struct hiISP_AWB_CBCR_TRACK_ATTR_S
{
    HI_BOOL bEnable;                          /*If enabled, statistic parameter cr, cb will change according to iso*/
    
    HI_U16  au16CrMax[ISP_AUTO_ISO_STRENGTH_NUM];  /*RW, Range:[0x0, 0xFFFF], au16CrMax[i] >= au16CrMin[i]*/
    HI_U16  au16CrMin[ISP_AUTO_ISO_STRENGTH_NUM];  /*RW, Range:[0x0, 0xFFFF]*/ 
    HI_U16  au16CbMax[ISP_AUTO_ISO_STRENGTH_NUM];  /*RW, Range:[0x0, 0xFFFF], au16CbMax[i] >= au16CbMin[i]*/
    HI_U16  au16CbMin[ISP_AUTO_ISO_STRENGTH_NUM];  /*RW, Range:[0x0, 0xFFFF]*/
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
    ISP_AWB_MULTI_LS_TYPE_E enMultiLSType;              /*0:Saturation or CCM Tunning*/
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
    HI_U8   u8AWBRunInterval;         /*RW, Range: [0x1, 0xFF], set the AWB run interval*/
    ISP_OP_TYPE_E   enOpType;
    ISP_MWB_ATTR_S  stManual;
    ISP_AWB_ATTR_S  stAuto;
} ISP_WB_ATTR_S;

typedef struct hiISP_COLORMATRIX_MANUAL_S
{  
    HI_BOOL bSatEn;             /*If bSatEn=1, the active CCM = SatMatrix * ManualMatrix */
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
    HI_U8   au8Sat[ISP_AUTO_ISO_STRENGTH_NUM];           /*RW,  Range: [0, 0xFF], should be decreased based on ISO increase */ 
} ISP_SATURATION_AUTO_S;

typedef struct hiISP_SATURATION_ATTR_S
{
    ISP_OP_TYPE_E enOpType;
    ISP_SATURATION_MANUAL_S stManual;
    ISP_SATURATION_AUTO_S stAuto;   
}ISP_SATURATION_ATTR_S;

typedef struct hiISP_COLOR_TONE_ATTR_S
{
    HI_U16 u16RedCastGain;        /*RW, R channel gain after CCM*/
    HI_U16 u16GreenCastGain;      /*RW, G channel gain after CCM*/
    HI_U16 u16BlueCastGain;       /*RW, B channel gain after CCM*/
} ISP_COLOR_TONE_ATTR_S;

typedef struct hiISP_WB_INFO_S
{
    HI_U16 u16Rgain;                                 /*RO, AWB result of R color channel, Range: [0x0, 0xFFF]*/
    HI_U16 u16Grgain;                                /*RO, AWB result of Gr color channel, Range: [0x0, 0xFFF]*/
    HI_U16 u16Gbgain;                                /*RO, AWB result of Gb color channel, Range: [0x0, 0xFFF]*/
    HI_U16 u16Bgain;                                 /*RO, AWB result of B color channel, Range: [0x0, 0xFFF]*/
    HI_U16 u16Saturation;                            /*RO, Current saturation, Range:[0x0, 0xFF]*/
    HI_U16 u16ColorTemp;                             /*RO, Detect color temperature, maybe out of color cemeprature range*/
    HI_U16 au16CCM[9];                               /*RO, Current color correction matrix*/

    HI_U16 u16LS0CT;                                 /*RO, color tempearture of primary light source*/
    HI_U16 u16LS1CT;                                 /*RO, color tempearture of secondary light source*/
    HI_U16 u16LS0Area;                               /*RO, area of primary light source*/
    HI_U16 u16LS1Area;                               /*RO, area of secondary light source*/
    HI_U8  u8MultiDegree;                            /*RO, 0 means uniform light source, larger value means multi light source*/ 
    HI_U16 u16ActiveShift;                           /*R; Range;[0x0,0xFF]*/
    ISP_AWB_INDOOR_OUTDOOR_STATUS_E enInOutStatus; 	 /*RO, indoor or outdoor status*/ 
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

typedef struct hiISP_DCF_CONST_INFO_S
{
    HI_U8       au8ImageDescription[DCF_DRSCRIPTION_LENGTH];        /*Describes image*/
    HI_U8       au8Make[DCF_DRSCRIPTION_LENGTH];                    /*Shows manufacturer of digital cameras*/
    HI_U8       au8Model[DCF_DRSCRIPTION_LENGTH];                   /*Shows model number of digital cameras*/
    HI_U8       au8Software[DCF_DRSCRIPTION_LENGTH];                /*Shows firmware (internal software of digital cameras) version number*/

    HI_U32      u32FNumber;                                         /*The actual F-number (F-stop) of lens when the image was taken*/
    HI_U32      u32MaxApertureValue;                                /*Maximum aperture value of lens.*/
    HI_U8       u8LightSource;                                      /*Light source, actually this means white balance setting. '0' means unknown, '1' daylight, '2'
                                                                               fluorescent, '3' tungsten, '10' flash, '17' standard light A, '18' standard light B, '19' standard light
                                                                               C, '20' D55, '21' D65, '22' D75, '255' other*/
    HI_U32      u32FocalLength;                                     /*Focal length of lens used to take image. Unit is millimeter*/
    HI_U8       u8SceneType;                                        /*Indicates the type of scene. Value '0x01' means that the image was directly photographed.*/
    HI_U8       u8CustomRendered;                                   /*Indicates the use of special processing on image data, such as rendering geared to output.
                                                                               0 = Normal process  1 = Custom process   */
    HI_U8       u8FocalLengthIn35mmFilm;                            /*Indicates the equivalent focal length assuming a 35mm film camera, in mm*/
    HI_U8       u8SceneCaptureType;                                 /*Indicates the type of scene that was shot. 0 = Standard,1 = Landscape,2 = Portrait,3 = Night scene. */
    HI_U8       u8GainControl;                                      /*Indicates the degree of overall image gain adjustment. 0 = None,1 = Low gain up,2 = High gain up,3 = Low gain down,4 = High gain down. */
    HI_U8       u8Contrast;                                         /*Indicates the direction of contrast processing applied by the camera when the image was shot.
                                                                               0 = Normal,1 = Soft,2 = Hard */
    HI_U8       u8Saturation;                                       /*Indicates the direction of saturation processing applied by the camera when the image was shot.
                                                                              0 = Normal,1 = Low saturation,2 = High saturation*/
    HI_U8       u8Sharpness;                                        /*Indicates the direction of sharpness processing applied by the camera when the image was shot.
                                                                              0 = Normal,1 = Soft,2 = Hard .*/
} ISP_DCF_CONST_INFO_S;

typedef struct hiISP_UPDATE_INFO_S
{
    HI_U16      u16ISOSpeedRatings;                                 /*CCD sensitivity equivalent to Ag-Hr film speedrate*/
    HI_U32      u32ExposureTime;                                    /*Exposure time (reciprocal of shutter speed).*/
    HI_U32      u32ExposureBiasValue;                               /*Exposure bias (compensation) value of taking picture*/
    HI_U8       u8ExposureProgram;                                  /*Exposure program that the camera used when image was taken. '1' means manual control, '2'
                                                                              program normal, '3' aperture priority, '4' shutter priority, '5' program creative (slow program),
                                                                              '6' program action(high-speed program), '7' portrait mode, '8' landscape mode*/
    HI_U8       u8MeteringMode;                                     /*Exposure metering method. '0' means unknown, '1' average, '2' center weighted average, '3'
                                                                              spot, '4' multi-spot, '5' multi-segment, '6' partial, '255' other*/
    HI_U8       u8ExposureMode;                                     /*Indicates the exposure mode set when the image was shot.
                                                                              0 = Auto exposure,1 = Manual exposure, 2 = Auto bracket*/
    HI_U8       u8WhiteBalance;                                     /* Indicates the white balance mode set when the image was shot.
                                                                                0 = Auto white balance ,1 = Manual white balance */
    HI_U8       u8WDRMode;
} ISP_UPDATE_INFO_S;


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

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /* __HI_COMM_ISP_H__ */

