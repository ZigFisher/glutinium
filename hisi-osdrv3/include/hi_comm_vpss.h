/******************************************************************************

  Copyright (C), 2001-2011, Hisilicon Tech. Co., Ltd.

 ******************************************************************************
  File Name     : hi_comm_vpss.h
  Version       : Initial Draft
  Author        : Hisilicon multimedia software group
  Created       :
  Last Modified :
  Description   : common struct definition for vpss
  Function List :
  History       :
  1.Date        : 20130508
    Author      : l00183122
    Modification: Create


******************************************************************************/

#ifndef __HI_COMM_VPSS_H__
#define __HI_COMM_VPSS_H__


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#include "hi_type.h"
#include "hi_common.h"
#include "hi_errno.h"
#include "hi_comm_video.h"

#define HI_ERR_VPSS_NULL_PTR        HI_DEF_ERR(HI_ID_VPSS, EN_ERR_LEVEL_ERROR, EN_ERR_NULL_PTR)
#define HI_ERR_VPSS_NOTREADY        HI_DEF_ERR(HI_ID_VPSS, EN_ERR_LEVEL_ERROR, EN_ERR_SYS_NOTREADY)
#define HI_ERR_VPSS_INVALID_DEVID   HI_DEF_ERR(HI_ID_VPSS, EN_ERR_LEVEL_ERROR, EN_ERR_INVALID_DEVID)
#define HI_ERR_VPSS_INVALID_CHNID   HI_DEF_ERR(HI_ID_VPSS, EN_ERR_LEVEL_ERROR, EN_ERR_INVALID_CHNID)
#define HI_ERR_VPSS_EXIST           HI_DEF_ERR(HI_ID_VPSS, EN_ERR_LEVEL_ERROR, EN_ERR_EXIST)
#define HI_ERR_VPSS_UNEXIST         HI_DEF_ERR(HI_ID_VPSS, EN_ERR_LEVEL_ERROR, EN_ERR_UNEXIST)
#define HI_ERR_VPSS_NOT_SUPPORT     HI_DEF_ERR(HI_ID_VPSS, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_SUPPORT)
#define HI_ERR_VPSS_NOT_PERM        HI_DEF_ERR(HI_ID_VPSS, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_PERM)
#define HI_ERR_VPSS_NOMEM           HI_DEF_ERR(HI_ID_VPSS, EN_ERR_LEVEL_ERROR, EN_ERR_NOMEM)
#define HI_ERR_VPSS_NOBUF           HI_DEF_ERR(HI_ID_VPSS, EN_ERR_LEVEL_ERROR, EN_ERR_NOBUF)
#define HI_ERR_VPSS_ILLEGAL_PARAM   HI_DEF_ERR(HI_ID_VPSS, EN_ERR_LEVEL_ERROR, EN_ERR_ILLEGAL_PARAM)
#define HI_ERR_VPSS_BUSY            HI_DEF_ERR(HI_ID_VPSS, EN_ERR_LEVEL_ERROR, EN_ERR_BUSY)
#define HI_ERR_VPSS_BUF_EMPTY       HI_DEF_ERR(HI_ID_VPSS, EN_ERR_LEVEL_ERROR, EN_ERR_BUF_EMPTY)


typedef   HI_S32 VPSS_GRP;
typedef   HI_S32 VPSS_CHN;

#define VPSS_INVALID_FRMRATE  (-1UL)
#define VPSS_CHN0               0
#define VPSS_CHN1               1
#define VPSS_CHN2               2
#define VPSS_CHN3               3
#define VPSS_INVALID_CHN       -1

/*Define de-interlace mode*/
typedef enum  hiVPSS_DIE_MODE_E
{
    VPSS_DIE_MODE_AUTO      = 0,
    VPSS_DIE_MODE_NODIE     = 1,
    VPSS_DIE_MODE_DIE       = 2,
    VPSS_DIE_MODE_BUTT
}VPSS_DIE_MODE_E;

/*Define attributes of vpss channel*/
typedef struct hiVPSS_CHN_ATTR_S
{
    HI_BOOL bSpEn;              /*Sharpen enable*/
    HI_BOOL bBorderEn;          /*Frame enable*/
    HI_BOOL bMirror;            /*mirror enable*/
    HI_BOOL bFlip;              /*flip   enable*/
    HI_S32  s32SrcFrameRate;    /* source frame rate */
    HI_S32  s32DstFrameRate;    /* dest  frame rate */
    BORDER_S  stBorder;
}VPSS_CHN_ATTR_S;

typedef struct hiVPSS_GRP_PARAM_S
{
    HI_U32 u32Contrast;         /*strength of dymanic contrast improve*/
    HI_S32 s32GlobalStrength;   /*strength of whole 3DNR*/
    HI_S32 s32IeStrength;       /*strength of image enhance*/
    HI_S32 s32YSFStrength;      /*strength of space filter*/
    HI_S32 s32YTFStrength;      /*strength of time filter*/
    HI_S32 s32CSFStrength;      /*strength of chroma filter*/
    HI_S32 s32CTFStrength;      /*strength of chroma filter*/
    HI_S32 s32MotionLimen;      /*limen for motion*/
}VPSS_GRP_PARAM_S;

typedef struct hiVPSS_GRP_PARAM_V2_S
{
    /* Strength of spatial-domain filtering for Chroma. */
    HI_U8  Chroma_SF_Strength;              /* [0,255] */
    /* Strength of time-domain filtering for Chroma. */
    HI_U8  Chroma_TF_Strength;              /* [0, 32]:  */

    /* Whether IE is processed in post step, 0: the first step, 1: the last step */
    HI_U16  IE_PostFlag;                    /* [0, 1] */

    /* Texture/Edge enhancement strength.
    IES indicates the strength of texture/edge enhancement (IE) */
    HI_U16  IE_Strength;                    /* [0, 63] */

    /* Motion detection threshold.
    This parameter determines the sensitivity of the motion detection unit to
    the pixel luminance variance.
    A larger value indicates lower sensitivity to the pixel luminance variance.
    To be specific, implements spatial-domain filtering on the pixel whose
    luminance variance exceeds the threshold and the filtering strength is determined
    by Luma_SF_Strength,
    and implements time-domain filtering on the pixel
    whose luminance variance is less than or equal to the threshold and the filtering
    strength is determined by Luma_TF_Strength. */
    HI_U16  Luma_MotionThresh;              /* [0,511] */

    /* Strength of spatial-domain filtering for moving area Luma. */
    HI_U8   Luma_SF_MoveArea;               /* [0,255] */
    /* Strength of spatial-domain filtering for Still Area Luma */
    HI_U8   Luma_SF_StillArea;              /* [0, 64] */
    /* Strength of time-domain filtering for Luma. */
    HI_U8   Luma_TF_Strength;               /* [0,15] */
    /* Strength of Desand */
    HI_U8   DeSand_Strength;                /* [0, 8], default 0 */
} VPSS_GRP_PARAM_V2_S;

typedef VPSS_GRP_PARAM_V2_S tVppNRsEx;


typedef struct hiVPSS_NR_PARAM_V1_S
{
  HI_S32  s32YPKStr;		/* [ 0 .. 63 ] */
  HI_S32  s32YSFStr;		/* [ 0 .. 200 ] */
  HI_S32  s32YTFStr;		/* [ 0 .. 128 ] */
  HI_S32  s32TFStrMax;		/* [ 0 .. 15 ] */
  HI_S32  s32TFStrMov;		/* [ 0 .. 31 ] */
  HI_S32  s32YSmthStr;		/* [ 0 .. 200 ] */
  HI_S32  s32YSmthRat;		/* [ 0 .. 32  ] */
  HI_S32  s32YSFStrDlt;		/* [ -128 .. 127 ] */
  HI_S32  s32YSFStrDl;		/* [ 0 .. 255 ] */
  HI_S32  s32YTFStrDlt;		/* [  -64 .. 63  ] */
  HI_S32  s32YTFStrDl;		/* [    0 .. 31  ] */
  HI_S32  s32YSFBriRat;		/* [ 0 .. 64 ] */
  HI_S32  s32CSFStr;		/* [ 0 ..  80 ] */
  HI_S32  s32CTFstr;		/* [ 0 ..  32 ] */
  HI_S32  s32YTFMdWin;      /* [ 0 .. 1 ] */
} VPSS_NR_PARAM_V1_S;


typedef union hiVPSS_NR_PARAM_U
{
    VPSS_NR_PARAM_V1_S stNRParam_V1;
}VPSS_NR_PARAM_U;

typedef struct hiVPSS_GRP_VPPNRS_S
{
  int  tss,      sfc, tfc;
  int  sfs, tfs, mdz, ies;
}VPSS_GRP_VPPNRS_S;

typedef struct hiVPSS_GRP_VPPNRZ_S
{
    int  SFS,  SFS1, SPS1,SPT1,  TFP1,TFR1,SBF1,  MDDZ, MAAT,  SFC;
    int  TFS,  SFS2, SPS2,SPT2,  TFP2,TFR2,SBF2,  MATH, MATW,  TFC;
}VPSS_GRP_VPPNRZ_S;

typedef struct hiVPSS_GRP_VPPNRX_S
{
  int  StStrength, StProSpati, StProTempo, StBaseFreq;
  int  TFS,   TFP, TFQ, TFR,     SHP,      MDDZ, MATH;
}VPSS_GRP_VPPNRX_S;

typedef struct hiVPSS_GRP_LEVEL2_PARAM_S
{
    VPSS_GRP_VPPNRS_S *pNrS;
    VPSS_GRP_VPPNRZ_S *pNrZ;
    VPSS_GRP_VPPNRX_S *pNrX;
}VPSS_GRP_LEVEL2_PARAM_S;

typedef struct hiVPSS_GRP_VPPNRXCORE_S
{
    HI_U8  SBS ;
    HI_U8  SDS , _reserved_b_ : 6;
    HI_U8  SBF:2;

    HI_U8  MiBaTFP;
    HI_U8  MiDaTFP;

    HI_U8  HiBaTFP;
    HI_U8  HiDaTFP, _reserved_B_[3];

    HI_U16  MDDZ : 7;
    HI_U16  TFP   : 6, _reserved_b0_ : 1;
    HI_U16  MaTFP : 2;

    HI_U16  TFR : 5;
    HI_U16  TFS : 4;

    HI_U16  SHP : 7;
    HI_U16  MaTFR : 5;

    HI_U16  MaTFS : 4;
    HI_U16  MaSHP : 7;
} VPSS_GRP_VPPNRXCORE_S;

typedef struct hiVPSS_GRP_VPPNRXEX_S
{
    HI_U32 ISO;

    VPSS_GRP_VPPNRXCORE_S stUnit[3];

    HI_U8  SFC;
    HI_U8  PostSBS;
    HI_U8  PostSDS;
    HI_U8  PostSHP;

    HI_U8  PostROW : 5, _reserved_b1_ : 3;
    HI_U8  PostSFS : 4,  _reserved_b2_ : 1;
    HI_U8   MATW : 3;
    HI_U16  MATH : 9;
    HI_U16  MABW : 1;
    HI_U16  TFC  : 6;
}VPSS_GRP_VPPNRXEX_S;

///////////////////////////////////////////////////////////
/* NR B interface */
typedef struct
{
    HI_S8 ISO;          /* reserved */
    HI_U8 SFC;          /*[0, 255]*/
    HI_U8 TFC;          /*[0, 32]*/

    HI_U8 SHPi;         /*[0, 127]*/
    HI_U8 SBSi;         /*[0, 255]*/
    HI_U8 SBTi;         /*[0, 64]*/
    HI_U8 SDSi;         /*[0, 255]*/
    HI_U8 SDTi;         /*[0, 64]*/
    HI_U8 MDZi;         /*[0, 127]*/

    HI_U8 SHPj;         /*Post=1,SHPj=[0, 127];  Post=0,SHPj=[0, 64]*/
    HI_U8 SBSj;         /*[0, 255]*/
    HI_U8 SBTj;         /*[0, 64]*/
    HI_U8 SDSj;         /*[0, 255]*/
    HI_U8 SDTj;         /*[0, 64]*/
    HI_U8 MDZj;         /*[0, 127]*/

    HI_U8 SHPk;         /*[0, 64]*/
    HI_U8 SBSk;         /*[0, 255]*/
    HI_U8 SBTk;         /*[0, 64]*/
    HI_U8 SDSk;         /*[0, 255]*/
    HI_U8 SDTk;         /*[0, 64]*/

    HI_U16 SBFi : 2;    /*[0, 3]*/
    HI_U16 SBFj : 2;    /*[0, 3]*/
    HI_U16 SBFk : 2;    /*[0, 3]*/
    HI_U16 MATH : 10;   /*[0, 511]*/

    HI_U16 TFSi : 4;    /*[0, 15]*/
    HI_U16 TFSj : 4;    /*[0, 15]*/
    HI_U16 TFSk : 4;    /*[0, 15]*/
    HI_U16 PSFS : 4;    /*[0, 8]*/

    HI_U16 TFRi : 5;    /*[0, 31]*/
    HI_U16 TFRj : 5;    /*[0, 31]*/
    HI_U16 TFRk : 5;    /*[0, 31]*/
    HI_U16 Post : 1;    /*[0, 1]*/
} VPSS_GRP_VPPNRBCORE_S;
typedef VPSS_GRP_VPPNRBCORE_S tVppNRbCore;

typedef struct
{
    VPSS_GRP_VPPNRBCORE_S iNRb;

    HI_U8 MDAF    : 3;  /*[0, 7]*/
    HI_U8 PostROW : 5;  /*[0, 31]*/
    HI_U8 MATW    : 2;  /*[0, 3]*/
    HI_U8 ExTfThr : 5;  /*[0, 31]*/
    HI_U8 MABW    : 1;  /*[0, 1]*/
    HI_U8 TextThr;      /*[0, 255]*/
    HI_U8 MTFS;         /*[0, 255]*/
} VPSS_GRP_VPPNRBEX_S;
typedef VPSS_GRP_VPPNRBEX_S tVppNRbEx;

typedef struct
{
    /*3DNR enable*/
    HI_U8 EN   : 1;    /*[0, 1]*/
    HI_U8 ISH  : 7;    /*[0, 64]*/
    /*SBS indicates the filtering strength of NR in bright region*/
    HI_U8 SBS;         /*[0, 255]*/
    /*SDS indicates the filtering strength of the dark region NR*/
    HI_U8 SDS;         /*[0, 255]*/
    HI_U8 IDZ  : 7;    /*[0, 100]*/
    HI_U8 STYP : 1;	/*[0, 1]*/
    HI_U8 SBT;         /*[0, 255]*/
    HI_U8 IEB; 		/*[0, 63]*/
    HI_U8 SDT;         /*[0, 255]*/
    HI_U8 ITX;         /*[0, 255]*/
    HI_U8 MSBF;		/*[0, 128]*/
    HI_U8 SBF;         /*[0, 128]*/
    HI_U8 MSTH;		/*[0, 100]*/
    /* STH indicates the big edge detection threshold. A larger value indicates
     that more pixels are considered as flat by the big edge detection unit. When
     spatial filtering is performed on more pixels, the picture becomes smoother
    and more picture details are lost*/
    HI_U8 STH;         /*[0, 100]*/
    /*MSDZ indicates the strength of texture protection in the motion region. A larger value indicates
    stronger strength of the motion region texture protection, a clearer motion region,
    and more obvious noises.*/
    HI_U8 MSDZ;		/*[0, 255]*/
    HI_U8 SDZ;         /*[0, 255]*/
    HI_U8 MSHT;		/*[0, 64]*/
    HI_U8 SHT;         /*[0, 64]*/
    HI_U8 MSHP;		/*[0, 255]*/
    HI_U8 SHP;         /*[0, 255]*/
    HI_U8 MTFR;		/*[0, 63]*/
    /*TFR indicates the relative strength of the temporal filtering*/
    HI_U8 TFR;         /*[0, 63]*/
    /*MATH indicates the motion threshold. A larger value indicates that more pixels are considered as
     static by the motion detection unit. When temporal filtering is performed on more pixels,
     the picture noises become less*/
    HI_U8 MATH;		/*[0, 100]*/
    /* TFS indicates the absolute strength of the temporal filtering */
    HI_U8 TFS; 		/*[0, 15]*/
    HI_U8 MODZ;		/*[0, 100]*/
    /*MATE indicates the motion threshold,During the debugging of MATH, MATE needs to be debugged. The
     value of MATE[i]   (i refers to 0, 1, and 2) should be set to 0. When the values of MATH[1]
     and MATH[2] are set to 75 and the picture noises are obvious, the value of MATE can be
     adjusted from 0 to 5 in sequence to obtain a balance between the picture noise and smearing.
     When the value of MATE is set to 5 and the picture noises are still obvious, the value of MATH
     can continue to be increased. (When a large number of flickering noises occur on the picture,
     the values of MATH need to be increased to suppress them.)*/
    HI_U8 MATE; 		/*[0, 8]*/
} tV19zNRbCore;

typedef tV19zNRbCore VPSS_GRP_VPPNRB19CORE_S;

typedef struct
{
    VPSS_GRP_VPPNRB19CORE_S  Unit[4];
    HI_U8  PBW        : 1;	    /*[0, 1]*/
    HI_U8  ClassicEn  : 1;	 	/*[0, 1]*/
    HI_U8  PSF        : 1;		/*[0, 1]*/
    HI_U8  _reserved_ : 1;	    /*[0, 1]*/
    HI_U8  PROW       : 4;		/*[0, 15]*/
    HI_U8  MOMD       : 6;		/*[0, 48]*/
    /*SFyEx is used to correct a side effect of the spatial filtering.
     When the picture noises are obvious, the spatial filtering causes the visible
     tiny grid-shaped pseudo textures in the flat region*/
    HI_U8  SFyEx      : 2;		/*[0, 2]*/
    HI_U8  MamiMax;			    /*[0, 255]*/
    HI_U8  PSBS;				/*[0, 32]*/
    HI_U8  PSDS;				/*[0, 32]*/
    HI_U8  RefMode;			    /*[0, 3]*/
    /*TFC indicates the strength of color difference temporal filtering*/
    HI_U8  TFC;				    /*[0, 32]*/
    /*SFC indicates the strength of spatial filtering for the color difference signal*/
    HI_U8  SFC;				    /*[0,255]*/
} tV19zNRb;

typedef tV19zNRb VPSS_GRP_VPPNRB_S;

/* only for Hi3516CV300 sharpen */
#define ISO_STENGTH_NUM    16
#define YUV_SHPLUMA_NUM    32

typedef enum hiVPSS_GRP_SHARPEN_TYPE_E
{
    SHARPEN_OP_TYPE_AUTO    = 0,
    SHARPEN_OP_TYPE_MANUAL  = 1,
    SHARPEN_OP_TYPE_BUTT
} VPSS_GRP_SHARPEN_TYPE_E;

typedef struct hiVPSS_GRP_SHARPEN_MANUAL_ATTR_S
{
    HI_U8 u8SharpenUd;          /*[0, 255]*/
    HI_U8 u8SharpenD;           /*[0, 255]*/
    HI_U8 u8TextureThr;         /*[0, 255]*/
    HI_U8 u8SharpenEdge;        /*[0, 255]*/
    HI_U8 u8EdgeThd;            /*[0, 255]*/
    HI_U8 u8OverShoot;          /*[0, 255]*/
    HI_U8 u8UnderShoot;         /*[0, 255]*/
    HI_U8 u8ShootSupStr;        /*[0, 255]*/
    HI_U8 u8DetailCtrl;         /*[0, 255]*/
    HI_U8 u8EdgeFiltStr;        /* RW; range: [0, 127]; Format:7.0;Edge noise suppression strength*/
    HI_U8 u8JagCtrl;            /* RW; range: [0, 255]; Format:8.0;Edge Jag Control*/
    HI_U8 u8NoiseLumaCtrl;      /* RW; range: [0, 31]; Format:5.0;Noise suppression based on the LumaWgt*/
} VPSS_GRP_SHARPEN_MANUAL_ATTR_S;

typedef struct hiVPSS_GRP_SHARPEN_AUTO_ATTR_S
{
    HI_U8 au8SharpenUd[ISO_STENGTH_NUM];    /*Undirectional sharpen strength for undirectional detail enhancement,[0, 255]*/
    HI_U8 au8SharpenD[ISO_STENGTH_NUM];     /*Directional sharpen strength for directional detail and edge enhancement,[0, 255]*/
    HI_U8 au8TextureThr[ISO_STENGTH_NUM];   /*Sharpen threshold for SharpenUd and SharpenD, noise is deemed as below the TextureThd without sharpness,[0, 255]*/
    HI_U8 au8SharpenEdge[ISO_STENGTH_NUM];  /*Sharpen strength for strong edge enhancement,[0, 255]*/
    HI_U8 au8EdgeThd[ISO_STENGTH_NUM];      /*Sharpen threshold to distinguish the strong edge, only the edge bigger than the EdgeThd can be sharpen by SharpenEdge,[0, 255]*/
    HI_U8 au8OverShoot[ISO_STENGTH_NUM];    /*[0, 255]*/
    HI_U8 au8UnderShoot[ISO_STENGTH_NUM];   /*[0, 255]*/
    HI_U8 au8ShootSupStr[ISO_STENGTH_NUM];  /*Overshoot and undershoot suppression strength, the amplitude and width of shoot will be decrease when shootSupSt increase,[0, 255]*/
    HI_U8 au8DetailCtrl[ISO_STENGTH_NUM];   /*Different sharpen strength for detail and edge. When it is bigger than 128, detail sharpen strength will be stronger than edge,[0, 255]*/
    HI_U8 au8EdgeFiltStr[ISO_STENGTH_NUM];                 /* RW; range: [0, 127]; Format:7.0;Edge noise suppression strength*/
	HI_U8 au8JagCtrl[ISO_STENGTH_NUM];                     /* RW; range: [0, 255]; Format:8.0;Edge Jag Control*/
	HI_U8 au8NoiseLumaCtrl[ISO_STENGTH_NUM];               /* RW; range: [0, 31]; Format:5.0;Noise suppression based on the LumaWgt*/
} VPSS_GRP_SHARPEN_AUTO_ATTR_S;

typedef struct hiVPSS_GRP_SHARPEN_ATTR_S
{
    VPSS_GRP_SHARPEN_TYPE_E enOpType;
	HI_U8 au8LumaWgt[YUV_SHPLUMA_NUM];       /*sharpen weight based on loacal luma*/
    VPSS_GRP_SHARPEN_MANUAL_ATTR_S stSharpenManualAttr;
    VPSS_GRP_SHARPEN_AUTO_ATTR_S stSharpenAutoAttr;
} VPSS_GRP_SHARPEN_ATTR_S;

///////////////////////////////////////////////////////////

/*Define coordinate mode*/
typedef enum hiVPSS_CROP_COORDINATE_E
{
    VPSS_CROP_RATIO_COOR = 0,   /*Ratio coordinate*/
    VPSS_CROP_ABS_COOR          /*Absolute coordinate*/
}VPSS_CROP_COORDINATE_E;

/*Define attributes of CLIP function*/
typedef struct hiVPSS_CROP_INFO_S
{
    HI_BOOL bEnable;        /*CROP enable*/
    VPSS_CROP_COORDINATE_E  enCropCoordinate;   /*Coordinate mode of the crop start point*/
    RECT_S  stCropRect;     /*CROP rectangular*/
}VPSS_CROP_INFO_S;

typedef struct hiVPSS_LDC_ATTR_S
{
    HI_BOOL bEnable;                    /* Whether LDC is enbale */
    LDC_ATTR_S stAttr;                  /* LDC Attribute */
}VPSS_LDC_ATTR_S;

/*Define attributes of vpss GROUP*/
typedef struct hiVPSS_GRP_ATTR_S
{
    /*statistic attributes*/
    HI_U32          u32MaxW;  	/*MAX width of the group*/
    HI_U32          u32MaxH;  	/*MAX height of the group*/
    PIXEL_FORMAT_E  enPixFmt; 	/*Pixel format*/

    HI_BOOL         bIeEn;    	/*Image enhance enable*/
    HI_BOOL         bDciEn;   	/*Dynamic contrast Improve enable*/

    HI_BOOL         bNrEn;    	/*Noise reduce enable*/

    HI_BOOL         bHistEn;  	/*Hist enable*/
    VPSS_DIE_MODE_E enDieMode;	/*De-interlace enable*/

    HI_BOOL         bSharpenEn;	/*Sharpen enable*/
}VPSS_GRP_ATTR_S;

/*Define vpss channel's work mode*/
typedef enum hiVPSS_CHN_MODE_E
{
    VPSS_CHN_MODE_AUTO = 0, /*Auto mode*/
    VPSS_CHN_MODE_USER  =1 /*User mode*/
}VPSS_CHN_MODE_E;

/*Define attributes of vpss channel's work mode*/
typedef struct hiVPSS_CHN_MODE_S
{
    VPSS_CHN_MODE_E  enChnMode;   /*Vpss channel's work mode*/
    HI_U32 u32Width;              /*Width of target image*/
    HI_U32 u32Height;             /*Height of target image*/
    HI_BOOL bDouble;              /*Field-frame transfer,only valid for VPSS_PRE0_CHN*/
    PIXEL_FORMAT_E  enPixelFormat;/*Pixel format of target image*/
    COMPRESS_MODE_E enCompressMode;   /*Compression mode of the output*/
}VPSS_CHN_MODE_S;

/*Define detailed params for channel image process*/
typedef struct hiVPSS_CHN_PARAM_S
{
    HI_U32 u32SpStrength;
} VPSS_CHN_PARAM_S;

/*Define vpss frame control info*/
typedef struct hiVPSS_FRAME_RATE_S
{
    HI_S32  s32SrcFrmRate;        /* Input frame rate of a  group*/
    HI_S32  s32DstFrmRate;        /* Output frame rate of a channel group */
} VPSS_FRAME_RATE_S;

/*Define attributes of vpss extend channel*/
typedef struct hiVPSS_EXT_CHN_ATTR_S
{
    VPSS_CHN        s32BindChn;             /*channel bind to*/
    HI_U32          u32Width;               /*Width of target image*/
    HI_U32          u32Height;              /*Height of target image*/
    HI_S32          s32SrcFrameRate;        /*Frame rate of source*/
    HI_S32          s32DstFrameRate;        /*Frame rate of extend chn input&output*/
    PIXEL_FORMAT_E  enPixelFormat;          /*Pixel format of target image*/
    COMPRESS_MODE_E enCompressMode;   /*Compression mode of the output*/
}VPSS_EXT_CHN_ATTR_S;

typedef struct hiVPSS_REGION_INFO_S
{
    RECT_S *pstRegion;    /*region attribute*/
    HI_U32 u32RegionNum;       /*count of the region*/
}VPSS_REGION_INFO_S;

typedef enum hiVPSS_REF_SEL_MODE_E
{
    VPSS_REF_FROM_RFR  = 0, /*reference frame from the rfr frame */
    VPSS_REF_FROM_CHN0 = 1, /*reference frame from the CHN0   */
    VPSS_REF_FROM_BUTT
}VPSS_REF_SEL_MODE_E;

typedef struct hiVPSS_LOW_DELAY_INFO_S
{
    HI_BOOL bEnable;
    HI_U32 u32LineCnt;
}VPSS_LOW_DELAY_INFO_S;

typedef struct hiVPSS_PARAM_MOD_S
{
    HI_BOOL bOneBufForLowDelay;
}VPSS_MOD_PARAM_S;

/****************************VPSS 3DNR B interface ***********start********************/

/* for Hi3518EV200 3DNR B interface */
typedef struct
{
  HI_U8 SDS;        /*[0, 255]*/
  HI_U8 SDT;        /*[0, 64]*/
  HI_U8 EDM;        /*[0, 3]*/
  HI_U8 SHP;        /*[0, 32]*/
  HI_U8 SBS;        /*[0, 255]*/
  HI_U8 SBT;        /*[0, 64]*/
  HI_U8 SFB;        /*[0, 255]*/
  HI_U8 SBF;        /*[0, 3]*/
} NRB_SF_PARAM_V1_S;

typedef struct
{
  HI_U8 TFR;        /*[0, 31]*/
  HI_U8 TFP;        /*[0, 31]*/
  HI_U8 TFS;        /*[0, 15]*/
  HI_U8 MSHP;       /*[0, 16]*/
  HI_U8 MTFR;       /*[0, 16]*/
  HI_U8 MDZ;        /*[0, 127]*/
} NRB_TF_PARAM_V1_S;

typedef struct
{
  HI_S16 IES;       /*[-255,64]*/
  HI_U8  ISHT;      /*[0, 64]*/
  HI_U8  SFC;       /*[0, 80]*/
  HI_U8  IEX[3];    /*[0, 127]*/
  HI_U8  TFC;       /*[0, 32]*/

  NRB_SF_PARAM_V1_S sf[3];
  NRB_TF_PARAM_V1_S tf[2];

  HI_U16 MATH : 9;  /*[0, 511]*/
  HI_U16 MDAF : 3;  /*[0, 7]*/
  HI_U16 MATW : 3;  /*[0, 5]*/
  HI_U16 MABW : 1;  /*[0, 1]*/

  HI_U8 PSFS;       /*[0, 8]*/

} NRB_PARAM_V1_S;

/* 3DNR B interface for Hi3519V100 */
typedef VPSS_GRP_VPPNRB_S NRB_PARAM_V2_S;

/****************************VPSS 3DNR B interface ***********end**********************/


/****************************VPSS 3DNR S interface ***********start********************/

/* for Hi3518EV200 3DNR S interface */
typedef VPSS_NR_PARAM_V1_S NRS_PARAM_V1_S;


/* 3DNR S interface for Hi3516CV300  */
typedef struct hiNRS_PARAM_V2_S
{
    HI_U8 IES0;
    HI_U8 SBS0,  SBS1,  SBS2,  SBS3;
    HI_U8 SDS0,  SDS1,  SDS2,  SDS3;
    HI_U8 STH0,  STH1,  STH2,  STH3;
    HI_U8 MDP,   LNTH, Pro3;
    HI_U16 MATH1, MATH2;
    HI_U8 MDDZ1, MDDZ2;
    HI_U8 TFS1,  TFS2;

    HI_U8 SFC, TFC, TPC, TRC;
} NRS_PARAM_V2_S;

/****************************VPSS 3DNR S interface ***********end*********************/

/****************************VPSS 3DNR X interface ***********start********************/
typedef struct
{
    HI_U8  IES, IET, IEF[4];
} tV16cIEy;

typedef struct
{
    HI_U8 horPro : 2, verPro : 2, kPro : 1, _rb1_ : 3;
} tV16cSFyPro;

typedef struct
{
    HI_U8  SBF0 : 1, SBF1 : 1, SBF2 : 2, _rb1_ : 4;
    HI_U8  SFR : 5, _rb2_ : 3;
    HI_U16 STH[3], SBS[3], SDS[3];
} tV16cSFySub;

typedef struct
{
    tV16cSFySub SFySub[4];
    tV16cSFyPro SFyPro[3];
    HI_U8 kProDD;
    HI_U8 VRTO[3];
} tV16cSFy;

typedef struct
{
    HI_U16 MATH : 10,  MATE : 4,  MATW : 2;
    HI_U8  MASW :  4,  MABW : 3,  MAXN : 1, _rB_;
} tV16cMDy;

typedef struct
{
    HI_U8  TFS :  4, TSS  : 4, _rB_;
    HI_U16 TSDZ: 10, TFR1 : 5, _rb0_ : 1;
    HI_U16 STR :  5, TFR0 : 5, _rb1_ : 2, TSR : 4;
} tV16cTFy;

typedef struct
{
    HI_U8  SFC, TFC : 6, _rb0_ : 2;
    HI_U8  TRC, TPC : 6, _rb1_ : 2;
}  tV16cNRc;

/* 3DNR X interface for Hi3516CV300  */
typedef struct hiNRX_PARAM_V2_S
{
    tV16cIEy  IEy;
    tV16cSFy  SFy;
    tV16cMDy  MDy[3];
    tV16cTFy  TFy[3];
    tV16cNRc  NRc;

    HI_U8  kRfr : 6, kOut : 1, _rb_ : 1, _rB_;
} NRX_PARAM_V2_S;

/****************************VPSS 3DNR X interface ***********end*********************/

typedef enum hiVPSS_NR_INTF_E
{
    VPSS_NR_INTF_S = 0,
    VPSS_NR_INTF_X = 1,
    VPSS_NR_INTF_B = 2,
    VPSS_NR_INTF_BUTT
}VPSS_NR_INTF_E;

typedef enum hiVPSS_NR_VER_E
{
    VPSS_NR_V1 = 1,
    VPSS_NR_V2 = 2,
    VPSS_NR_V3 = 3,
    VPSS_NR_V4 = 4,
    VPSS_NR_BUTT
}VPSS_NR_VER_E;

typedef struct hiVPSS_GRP_NRB_PARAM_S
{
    VPSS_NR_VER_E enNRVer;
    union
    {
        NRB_PARAM_V1_S stNRBParam_V1;   /* interface B V1 for Hi3518EV200 */
        NRB_PARAM_V2_S stNRBParam_V2;   /* interface B V2 for Hi3519V100  */
    };

}VPSS_GRP_NRB_PARAM_S;

typedef struct hiVPSS_GRP_NRS_PARAM_S
{
    VPSS_NR_VER_E enNRVer;
    union
    {
        NRS_PARAM_V1_S stNRSParam_V1;   /* interface S V1 for Hi3518EV200 */
        NRS_PARAM_V2_S stNRSParam_V2;   /* interface S V2 for Hi3516CV300 */
    };

}VPSS_GRP_NRS_PARAM_S;

typedef struct hiVPSS_GRP_NRX_PARAM_S
{
    VPSS_NR_VER_E enNRVer;
    union
    {
        NRX_PARAM_V2_S stNRXParam_V2;   /* interface X V2 for Hi3516CV300 */
    };

}VPSS_GRP_NRX_PARAM_S;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */
#endif /* __HI_COMM_VPSS_H__ */


