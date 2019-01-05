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


#define HI_ISP_NR_CALIB_COEF_COL (4)
#define HI_ISP_NR_PARA_LUT_COUNT (6)
#define HI_ISP_NR_ISO_LEVEL_MAX  (16)
#define HI_ISP_LSC_LIGHT_NUM	 (3)
#define RGBIR_MATRIX_NUM         (15)

typedef struct hiISP_NR_CABLI_PARA_S
{
	HI_U8   u8CalicoefRow;
	HI_FLOAT (*pCalibcoef)[HI_ISP_NR_CALIB_COEF_COL];
}ISP_NR_CABLI_PARA_S;

typedef struct hiISP_NR_ISO_PARA_TABLE_S
{

	HI_U16	u16Threshold;	
	HI_U16	u8varStrength;
	HI_U16	u8fixStrength;
    HI_U8   u8LowFreqSlope;
}ISP_NR_ISO_PARA_TABLE_S;

typedef struct hiISP_CMOS_NOISE_TABLE_S
{	
	/*noise reduction calibration para*/
	ISP_NR_CABLI_PARA_S     stNrCaliPara;
	/*noise reduction iso para*/
	ISP_NR_ISO_PARA_TABLE_S stIsoParaTable[HI_ISP_NR_ISO_LEVEL_MAX];
} ISP_CMOS_NOISE_TABLE_S;

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

typedef struct hiISP_CMOS_DEMOSAIC_S
{
	/*For Demosaic*/
	HI_BOOL bEnable;			
	HI_U8   u8VhLimit; /* RW,Range: [0x0, 0xFF]  */ 
	HI_U8   u8VhOffset; /* RW,Range: [0x0, 0xFF]  */
	HI_U16  u16VhSlope; /* RW,Range: [0x0, 0xFF]  */
	/*False Color*/
	HI_BOOL bFcrEnable;
	HI_U8   au8FcrStrength[ISP_AUTO_ISO_STENGTH_NUM];
	HI_U8   au8FcrThreshold[ISP_AUTO_ISO_STENGTH_NUM];
	/*For Ahd*/
	HI_U16  u16UuSlope;	
	HI_U16  au16NpOffset[ISP_AUTO_ISO_STENGTH_NUM];	
} ISP_CMOS_DEMOSAIC_S;

#define HI_ISP_SHARPEN_PARA_NUM (3)

typedef struct hiISP_CMOS_RGBSHARPEN_S
{    
    HI_BOOL abEnPixSel[ISP_AUTO_ISO_STENGTH_NUM];

    HI_U8  au8MaxSharpAmt1[ISP_AUTO_ISO_STENGTH_NUM];
    HI_U8  au8MaxEdgeAmt[ISP_AUTO_ISO_STENGTH_NUM];
    
    HI_U8  au8SharpThd2[ISP_AUTO_ISO_STENGTH_NUM]; 
    HI_U8  au8EdgeThd2[ISP_AUTO_ISO_STENGTH_NUM];
    
    HI_U8  au8OvershootAmt[ISP_AUTO_ISO_STENGTH_NUM]; 
    HI_U8  au8UndershootAmt[ISP_AUTO_ISO_STENGTH_NUM];
} ISP_CMOS_RGBSHARPEN_S;

#define COLORCORRECTIONLUT_NODE_NUMBER 33
typedef struct hiISP_CMOS_DRC_S
{
    HI_BOOL bEnable;

	HI_U8  u8SpatialVar;             
	HI_U8  u8RangeVar;              
	
	HI_U8  u8Asymmetry;              
	HI_U8  u8SecondPole;             
	HI_U8  u8Stretch;                

	HI_U16 u16DarkGainLmtY;         
	HI_U16 u16DarkGainLmtC;         
	HI_U16 u16BrightGainLmt; 
    HI_U16 au16ColorCorrectionLut[COLORCORRECTIONLUT_NODE_NUMBER];
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
typedef struct hiISP_CMOS_UVNR_S
{    
	HI_S32  UVNR_lutSigma   [ISP_AUTO_ISO_STENGTH_NUM];
	HI_S8   Coring_lutLimit [ISP_AUTO_ISO_STENGTH_NUM];
	HI_S8   UVNR_blendRatio [ISP_AUTO_ISO_STENGTH_NUM];

} ISP_CMOS_UVNR_S;

typedef struct hiISP_CMOS_SENSOR_MAX_RESOLUTION_S
{
    HI_U32  u32MaxWidth;
    HI_U32  u32MaxHeight;
}ISP_CMOS_SENSOR_MAX_RESOLUTION_S;

typedef struct hiISP_CMOS_DPC_S
{    
	//HI_U8	u8IRChannel;// 1 yes;0 no
	//HI_U8	u8IRPosition;//0:Gb,1:Gr
	HI_U16	au16Slope[ISP_AUTO_ISO_STENGTH_NUM];
	HI_U16	au16BlendRatio[ISP_AUTO_ISO_STENGTH_NUM];
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

typedef enum hiISP_CMOS_IRPOS_TYPE_E
{
    ISP_CMOS_IRPOS_TYPE_GR = 0x0,
    ISP_CMOS_IRPOS_TYPE_GB,
    ISP_CMOS_IRPOS_TYPE_BUTT
}ISP_CMOS_IRPOS_TYPE_E;

typedef enum hiISP_CMOS_OP_TYPE_E
{
    OP_CMOS_TYPE_AUTO    = 0,
    OP_CMOS_TYPE_MANUAL  = 1,
    OP_CMOS_TYPE_BUTT
} ISP_CMOS_OP_TYPE_E;

typedef struct hiISP_CMOS_RGBIR_ATTR_S
{
    HI_BOOL bEnable;                            /*RW, Range: [false, true] Rgbir moudle enable  */
    ISP_CMOS_IRPOS_TYPE_E enIrPosType;          /*RW, Range: [0, 1] Rgbir moudle ir position: 0 Gr, 1 Gb  */
    HI_U16  u16OverExpThresh;                   /*RW, Range: [0,4095] Rgbir moudle over exposure threshold*/    
}ISP_CMOS_RGBIR_ATTR_S;

typedef struct hiISP_CMOS_RGBIR_CTRL_S
{
    HI_BOOL bIrOutEn;                           /*RW, Range: [false, true] Rgbir moudle output ir image enable  */
    HI_BOOL bIrFilterEn;                        /*RW, Range: [false, true] Rgbir moudle noise filter enable  */
    HI_BOOL bRemovelEn;                         /*RW, Range: [false, true] Rgbir moudle removel ir enable  */
    ISP_CMOS_OP_TYPE_E enCompType;              /*RW, Range: [false, true] Rgbir moudle after removel ir, compensation type 0:auto,1:manul  */
    HI_U16  u16ManuGain;                        /*RW, Range: [256,1023] Rgbir moudle after removel ir, manul-compensation gain,2bit integer,8bit decimal */
    HI_S16  as16ScaleCoef[RGBIR_MATRIX_NUM];    /*RW, Range: [-512,511] Rgbir moudle removel ir translate matrix,bit9:signed bit,bit8:integer bit,bit7_0:decimal bit*/
}ISP_CMOS_RGBIR_CTRL_S;

typedef struct hiISP_CMOS_RGBIR_S
{
    HI_BOOL               bValid;       /* if bValid is false, below paramter is not setted in xxx_cmos.c*/
    ISP_CMOS_RGBIR_ATTR_S stRgbirAttr;
    ISP_CMOS_RGBIR_CTRL_S stRgbirCtrl;   
}ISP_CMOS_RGBIR_S;

typedef struct hiISP_CMOS_GE_S
{
    HI_BOOL bEnable;
	HI_U8   u8Slope;
	HI_U8   u8Sensitivity;
	HI_U16  u16Threshold;
	HI_U16  u16SensiThreshold;
	HI_U16  au16Strength[ISP_AUTO_ISO_STENGTH_NUM];
}ISP_CMOS_GE_S;


typedef struct hiISP_CMOS_DEFAULT_S
{    
    ISP_CMOS_DRC_S          stDrc;
    ISP_CMOS_NOISE_TABLE_S  stNoiseTbl;
    ISP_CMOS_DEMOSAIC_S     stDemosaic;
    ISP_CMOS_GAMMAFE_S      stGammafe;
    ISP_CMOS_GAMMA_S        stGamma;
    ISP_CMOS_RGBSHARPEN_S   stRgbSharpen;
	ISP_CMOS_UVNR_S         stUvnr;
	ISP_CMOS_DPC_S          stDpc;
	ISP_CMOS_LSC_S			stLsc;
    ISP_CMOS_RGBIR_S        stRgbir;
	ISP_CMOS_GE_S           stGe;
	ISP_CMOS_COMPANDER_S    stCompander;

    ISP_CMOS_SENSOR_MAX_RESOLUTION_S stSensorMaxResolution;
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

