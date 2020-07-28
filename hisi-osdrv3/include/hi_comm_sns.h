/******************************************************************************

  Copyright (C), 2001-2011, Hisilicon Tech. Co., Ltd.

 ******************************************************************************
  File Name     : hi_comm_sns.h
  Version       : Initial Draft
  Author        : Hisilicon multimedia software group
  Created       : 2011/01/05
  Description   : 
  History       :
  1.Date        : 2011/01/05
    Author      : x00100808
    Modification: Created file

******************************************************************************/

#ifndef __HI_COMM_SNS_H__
#define __HI_COMM_SNS_H__

#include "hi_type.h"
#include "hi_common.h"
#include "hi_comm_isp.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* End of #ifdef __cplusplus */

typedef struct hiISP_CMOS_BLACK_LEVEL_S
{
    HI_BOOL bUpdate;
    
    HI_U16  au16BlackLevel[4];
} ISP_CMOS_BLACK_LEVEL_S;



#define HI_ISP_NR_SENSOR_INDEX_MN34220 (0)
#define HI_ISP_NR_SENSOR_INDEX_9M034   (1)
#define HI_ISP_NR_SENSOR_INDEX_IMX222  (2)
#define HI_ISP_NR_SENSOR_INDEX_IMX122  (3)
#define HI_ISP_NR_SENSOR_INDEX_OV4682  (4)
#define HI_ISP_NR_CALIB_COEF_COL (4)
#define HI_ISP_NR_PARA_LUT_COUNT (6)
#define HI_ISP_NR_ISO_LEVEL_MAX  (16)
#define HI_ISP_LSC_LIGHT_NUM	 (3)



typedef struct hiISP_CMOS_DEMOSAIC_S
{
	HI_BOOL bEnable;			

	HI_U16 au16EdgeSmoothThr[ISP_AUTO_ISO_STRENGTH_NUM];
	HI_U16 au16EdgeSmoothSlope[ISP_AUTO_ISO_STRENGTH_NUM];
    HI_U16 au16AntiAliasThr[ISP_AUTO_ISO_STRENGTH_NUM]; 
	HI_U16 au16AntiAliasSlope[ISP_AUTO_ISO_STRENGTH_NUM]; 

	HI_U16 au16NrCoarseStr[ISP_AUTO_ISO_STRENGTH_NUM];
	HI_U16 au16NoiseSuppressStr[ISP_AUTO_ISO_STRENGTH_NUM];
	HI_U8  au8DetailEnhanceStr[ISP_AUTO_ISO_STRENGTH_NUM];
    HI_U16 au16SharpenLumaStr[ISP_AUTO_ISO_STRENGTH_NUM];
 	HI_U16 au16ColorNoiseCtrlThr[ISP_AUTO_ISO_STRENGTH_NUM]; 
} ISP_CMOS_DEMOSAIC_S;

                                                                                                                                                           
typedef struct hiISP_CMOS_BAYERNR_S                                                   
{
	HI_U16   u16CalibrationLutNum;
	HI_FLOAT afCalibrationCoef[BAYER_CALIBTAION_MAX_NUM][2];
	HI_U16   au16CoarseStr[BAYER_PATTER_NUM];
    HI_U8    au8LutFineStr[ISP_AUTO_ISO_STRENGTH_NUM]; 
	HI_U8    au8ChromaStr[BAYER_PATTER_NUM][ISP_AUTO_ISO_STRENGTH_NUM];
	
	HI_U16   au16LutCoringWeight[ISP_AUTO_ISO_STRENGTH_NUM];  
	HI_U16   au16LutCoringLRatio[HI_ISP_BAYERNR_LUT_LENGTH];

}ISP_CMOS_BAYERNR_S;

#define ISP_YUV_SHPLUMA_NUM (32)

typedef struct hiISP_CMOS_YUV_SHARPEN_S
{
	HI_BOOL  bInvalid;			

	HI_U16 au16SharpenUd[ISP_AUTO_ISO_STRENGTH_NUM];		//u16UDHfGain		[0, 1023]
	HI_U8 au8SharpenD[ISP_AUTO_ISO_STRENGTH_NUM];			//u16EdgeAmt			[0, 255]
	HI_U8 au8TextureThd[ISP_AUTO_ISO_STRENGTH_NUM];		//EdgeThd2   			[0, 255]
	HI_U8 au8SharpenEdge[ISP_AUTO_ISO_STRENGTH_NUM];		//u16SharpAmt		[0, 255]
	HI_U8 au8EdgeThd[ISP_AUTO_ISO_STRENGTH_NUM];			//u16SharpThd2		[0, 255]
	HI_U8 au8OverShoot[ISP_AUTO_ISO_STRENGTH_NUM];			//u8OvershootAmt		[0, 255]
	HI_U8 au8UnderShoot[ISP_AUTO_ISO_STRENGTH_NUM];			//u8UndershootAmt		[0, 255]
	HI_U8 au8shootSupSt[ISP_AUTO_ISO_STRENGTH_NUM]; 		//u8OvShtVarMax/u8UnShtVarMax	/u8ShtSupBldr		[0, 255]   
	HI_U8 au8DetailCtrl[ISP_AUTO_ISO_STRENGTH_NUM];  			//[0, 255]
	HI_U8 au8RGain[ISP_AUTO_ISO_STRENGTH_NUM];
	HI_U8 au8BGain[ISP_AUTO_ISO_STRENGTH_NUM];
	HI_U8 au8SkinGain[ISP_AUTO_ISO_STRENGTH_NUM];
	HI_U8 au8EdgeFiltStr[ISP_AUTO_ISO_STRENGTH_NUM];               /* RW; range: [0, 127]; Format:8.0;Edge noise suppression strength*/
	HI_U8 au8JagCtrl[ISP_AUTO_ISO_STRENGTH_NUM];                     /* RW; range: [0, 255]; Format:8.0;Edge Jag Control*/
	HI_U8 au8NoiseLumaCtrl[ISP_AUTO_ISO_STRENGTH_NUM];          /* RW; range: [0, 31]; Format:8.0;Noise suppression based on the LumaWgt*/


} ISP_CMOS_YUV_SHARPEN_S;

typedef struct hiISP_CMOS_DRC_S
{
	HI_BOOL bEnable;
	HI_U8   u8SpatialVar;           
	HI_U8   u8RangeVar;             
	HI_U8   u8Asymmetry;            
	HI_U8   u8SecondPole;             
	HI_U8   u8Stretch;                
	HI_U8   u8Compress; 
	HI_U8   u8PDStrength;            
	HI_U8   u8LocalMixingBrigtht;  
	HI_U8   u8LocalMixingDark;
	HI_U16  ColorCorrectionLut[33];
	HI_BOOL bOpType;            /* 0: Auto; 1: Manual */
    HI_U8   u8ManualStrength;
    HI_U8   u8AutoStrength;

} ISP_CMOS_DRC_S;


#define GAMMA_FE0_LUT_SIZE 33
#define GAMMA_FE1_LUT_SIZE 257
typedef struct hiISP_CMOS_GAMMAFE_S
{
    HI_BOOL bValid;         /* wdr sensor should set */

    HI_U16  au16Gammafe0[GAMMA_FE0_LUT_SIZE];    
    HI_U16  au16Gammafe1[GAMMA_FE1_LUT_SIZE];
} ISP_CMOS_GAMMAFE_S;

#define CMOS_SHADING_TABLE_NODE_NUMBER_MAX (129)
typedef struct hiISP_CMOS_SHADING_S
{
    HI_BOOL bValid;
    
    HI_U16 u16RCenterX;
    HI_U16 u16RCenterY;
    HI_U16 u16GCenterX;
    HI_U16 u16GCenterY;
    HI_U16 u16BCenterX;
    HI_U16 u16BCenterY;

    HI_U16 au16RShadingTbl[CMOS_SHADING_TABLE_NODE_NUMBER_MAX];
    HI_U16 au16GShadingTbl[CMOS_SHADING_TABLE_NODE_NUMBER_MAX];
    HI_U16 au16BShadingTbl[CMOS_SHADING_TABLE_NODE_NUMBER_MAX];

    HI_U16 u16ROffCenter;
    HI_U16 u16GOffCenter;
    HI_U16 u16BOffCenter;

    HI_U16 u16TblNodeNum;
} ISP_CMOS_SHADING_S;

#define GAMMA_NODE_NUMBER   257
typedef struct hiISP_CMOS_GAMMA_S
{
    HI_BOOL bValid;
    
    HI_U16  au16Gamma[GAMMA_NODE_NUMBER];
} ISP_CMOS_GAMMA_S;

#define HI_ISP_UVNR_SIGMA_PRECISION (1)



typedef struct hiISP_CMOS_SENSOR_MAX_RESOLUTION_S
{
    HI_U32  u32MaxWidth;
    HI_U32  u32MaxHeight;
}ISP_CMOS_SENSOR_MAX_RESOLUTION_S;

typedef struct hiISP_CMOS_DPC_S
{    
	//HI_U8	u8IRChannel;// 1 yes;0 no
	//HI_U8	u8IRPosition;//0:Gb,1:Gr
	//HI_U16	au16Slope[ISP_AUTO_ISO_STRENGTH_NUM];
	HI_U16	au16Strength[ISP_AUTO_ISO_STRENGTH_NUM];
	HI_U16	au16BlendRatio[ISP_AUTO_ISO_STRENGTH_NUM];
} ISP_CMOS_DPC_S;

#define HI_ISP_LSC_GRID_POINTS		(289)
#define HI_ISP_LSC_GRID_COL         (17)
#define HI_ISP_LSC_GRID_ROW         (17)

typedef struct hiISP_LSC_CABLI_UNI_TABLE_S
{
    HI_U32 u32XGrid[(HI_ISP_LSC_GRID_COL-1)/2];
    HI_U32 u32YGrid[(HI_ISP_LSC_GRID_ROW-1)/2];
}ISP_LSC_CABLI_UNI_TABLE_S;

typedef struct hiISP_LSC_CABLI_TABLE_S
{
	HI_U32 u32RGain;
    HI_U32 u32BGain;
	
	HI_U32 au32R_Gain[HI_ISP_LSC_GRID_POINTS];	
	HI_U32 au32Gr_Gain[HI_ISP_LSC_GRID_POINTS];	
	HI_U32 au32Gb_Gain[HI_ISP_LSC_GRID_POINTS];	
	HI_U32 au32B_Gain[HI_ISP_LSC_GRID_POINTS];	            
}ISP_LSC_CABLI_TABLE_S;

typedef struct hiISP_CMOS_LSC_S
{
	ISP_LSC_CABLI_UNI_TABLE_S stLscUniParaTable;
	ISP_LSC_CABLI_TABLE_S stLscParaTable[HI_ISP_LSC_LIGHT_NUM];
}ISP_CMOS_LSC_S;


typedef struct hiISP_CMOS_CA_S
{
	HI_BOOL   bEnable;
	HI_U16    au16YRatioLut[128];  //1.10bit  Y Ratio For UV ; Max = 2047 FW Limit
	HI_S32    as32ISORatio[16];          //1.10bit  ISO Ratio  For UV ,Max = 2047 FW Limi
}ISP_CMOS_CA_S;

typedef struct hiISP_CMOS_SPLIT_S
{
	HI_BOOL bEnable;
	HI_U8 u8InputWidthSel;//Inputwidthselect: 0=12bit; 1=14bit; 2=16bit; 3=20bit
	HI_U8 u8ModeIn;       //ModeIn: 0=lineare; 1=2~3mux; 2=16LOG; 3=sensor-build-in
	HI_U8 u8ModeOut;      //ModeOut: 0= bypass; 1=2chn ; 2=3chn; 3=4chn; 4=20bit when decompress
	HI_U16 u16OutBits;
	HI_U32 u32OffsetR;
	HI_U32 u32OffsetGR;
	HI_U32 u32OffsetGB;
	HI_U32 u32OffsetB;
	HI_U32 u32OutBlc;
}ISP_CMOS_SPLIT_S;

typedef struct hiISP_CMOS_COMPANDER_S
{

	HI_U32 u32BitDepthIn;
	HI_U32 u32BitDepthOut;
	HI_U32 u32X0;
	HI_U32 u32Y0;
	HI_U32 u32X1;
	HI_U32 u32Y1;
	HI_U32 u32X2;
	HI_U32 u32Y2;
	HI_U32 u32X3;
	HI_U32 u32Y3;
	HI_U32 u32Xmax;
	HI_U32 u32Ymax;

}ISP_CMOS_COMPANDER_S;

typedef struct hiISP_CMOS_GE_S
{
    HI_BOOL bEnable;                                 /*RW,Range: [   0, 1]      */ 
 
    HI_U8  u8Slope;                                  /*RW,Range: [   0, 0xE]    */
    HI_U8  u8SensiSlope;                            /*RW,Range: [   0, 0xE]    */
    HI_U16 u16SensiThr;                        /*RW,Range: [   0, 0x3FFF] */ 
    HI_U16 au16Threshold[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW,Range: [   0, 0x3FFF] */
    HI_U16 au16Strength[ISP_AUTO_ISO_STRENGTH_NUM];  /*RW,Range: [   0, 0x100]  */
    HI_U16 au16NpOffset[ISP_AUTO_ISO_STRENGTH_NUM];  /*RW,Range: [   0, 0x3FFF] */
}ISP_CMOS_GE_S;

typedef struct hiISP_CMOS_FCR_S
{
    HI_BOOL bEnable;                                  /*RW,Range: [   0, 1]    */
	HI_U8 au8Strength[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW,Range: [   0, 0xFF] */
	HI_U8 au8Threshold[ISP_AUTO_ISO_STRENGTH_NUM];/*RW,Range: [   0, 0xFF] */
   HI_U16 au16Offset[ISP_AUTO_ISO_STRENGTH_NUM];     /*RW,Range: [   0, 0xFFF] */
}ISP_CMOS_FCR_S;

typedef struct hiISP_CMOS_WDR_ATTR_S
{
    HI_U32  au32ExpRatio[3];
}ISP_CMOS_WDR_ATTR_S;

typedef struct hiISP_CMOS_DEFAULT_S
{    
    ISP_CMOS_DRC_S          stDrc;
    ISP_CMOS_DEMOSAIC_S     stDemosaic;
    ISP_CMOS_GAMMAFE_S      stGammafe;
    ISP_CMOS_GAMMA_S        stGamma;
    ISP_CMOS_YUV_SHARPEN_S  stYuvSharpen;
	ISP_CMOS_DPC_S          stDpc;
	ISP_CMOS_LSC_S			stLsc;
	ISP_CMOS_GE_S           stGe;  
	ISP_CMOS_FCR_S          stFcr;
	ISP_CMOS_BAYERNR_S      stBayerNr;
	ISP_CMOS_SPLIT_S        stSplit;
	ISP_CMOS_CA_S           stCa;
	ISP_CMOS_COMPANDER_S    stCompander;
    ISP_CMOS_SENSOR_MAX_RESOLUTION_S stSensorMaxResolution;
	ISP_CMOS_WDR_ATTR_S    stWDRAttr;
} ISP_CMOS_DEFAULT_S;

typedef struct hiISP_CMOS_SENSOR_IMAGE_MODE_S
{
    HI_U16   u16Width;
    HI_U16   u16Height;
    HI_FLOAT f32Fps;
}ISP_CMOS_SENSOR_IMAGE_MODE_S;

typedef struct hiISP_SENSOR_EXP_FUNC_S
{
    HI_VOID(*pfn_cmos_sensor_init)(HI_VOID);
    HI_VOID(*pfn_cmos_sensor_exit)(HI_VOID);
    HI_VOID(*pfn_cmos_sensor_global_init)(HI_VOID);
    HI_S32(*pfn_cmos_set_image_mode)(ISP_CMOS_SENSOR_IMAGE_MODE_S *pstSensorImageMode);
    HI_VOID(*pfn_cmos_set_wdr_mode)(HI_U8 u8Mode);
    
    /* the algs get data which is associated with sensor, except 3a */
    HI_U32(*pfn_cmos_get_isp_default)(ISP_CMOS_DEFAULT_S *pstDef);
    HI_U32(*pfn_cmos_get_isp_black_level)(ISP_CMOS_BLACK_LEVEL_S *pstBlackLevel);
    HI_U32(*pfn_cmos_get_sns_reg_info)(ISP_SNS_REGS_INFO_S *pstSnsRegsInfo);

    /* the function of sensor set pixel detect */
    HI_VOID(*pfn_cmos_set_pixel_detect)(HI_BOOL bEnable);
} ISP_SENSOR_EXP_FUNC_S;

typedef struct hiISP_SENSOR_REGISTER_S
{
    ISP_SENSOR_EXP_FUNC_S stSnsExp;
} ISP_SENSOR_REGISTER_S;


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /*__HI_COMM_SNS_H__ */

