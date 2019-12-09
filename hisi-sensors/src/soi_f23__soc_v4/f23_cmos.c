/******************************************************************************

  Copyright (C), 2016, Hisilicon Tech. Co., Ltd.

 ******************************************************************************
  File Name		: f23_cmos.c
  Version		: Initial Draft
  Author		: Hisilicon multimedia software group
  Created		: 2019/02/12
  Description	:
  History		:
  1.Date		: 2019/02/12
    Author		:
******************************************************************************/
#if !defined(__JXF23_CMOS_H_)
#define __JXF23_CMOS_H_

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "hi_comm_sns.h"
#include "hi_comm_video.h"
#include "hi_sns_ctrl.h"
#include "mpi_isp.h"
#include "mpi_ae.h"
#include "mpi_awb.h"
//#include "mpi_af.h"
#include "hi_comm_vpss.h"									//min@20180326 3DNR

#include "f23_cmos_ex.h"									//hi3516ev200

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* End of #ifdef __cplusplus */


#define		SOI_SENSOR_ID		23


#define		_WDR_OPT
#define		_SOI_BSUN_OPT


//min@20180412 In order to add or delete registers easily.
enum {
	R01_IDX	= 0,
	R02_IDX,	//1
	R00_IDX,	//2
	R22_IDX,	//3
	R23_IDX,	//4
	#ifdef _WDR_OPT
	R05_IDX,	//5
	#endif
	#ifdef _SOI_BSUN_OPT //min@201800510
	R66_IDX,	//6
	R0C_IDX,	//7
	#endif
	REG_IDX_MAX	//8
};

//hi3516ev200 global variables 
/****************************************************************************
 * global variables                                                            *
 ****************************************************************************/

int   gu8F23_FlipValue = 0x0;

#define	HIGH_8BITS(x)	(((x) & 0xFF00) >> 8)
#define	LOW_8BITS(x)	( (x) & 0x00FF)
#define	LOWER_4BITS(x)	( (x) & 0x000F)
#define	HIGHER_4BITS(x)	(((x) & 0xF000) >> 12)
#define	HIGHER_8BITS(x)	(((x) & 0x0FF0) >> 4)

#ifndef MAX
#define	MAX(a, b)	(((a) < (b)) ?  (b) : (a))
#endif

#ifndef MIN
#define	MIN(a, b)	(((a) > (b)) ?  (b) : (a))
#endif

ISP_SNS_STATE_S *g_pastSoiSensor[ISP_MAX_PIPE_NUM] = {HI_NULL};

#define	SOI_SENSOR_GET_CTX(dev, pstCtx)	(pstCtx = g_pastSoiSensor[dev])
#define	SOI_SENSOR_SET_CTX(dev, pstCtx)	(g_pastSoiSensor[dev] = pstCtx)
#define	SOI_SENSOR_RESET_CTX(dev)		(g_pastSoiSensor[dev] = HI_NULL)

ISP_SNS_COMMBUS_U g_aunSoiSensorBusInfo[ISP_MAX_PIPE_NUM] = {
	[0] = { .s8I2cDev = 0},
	[1 ... ISP_MAX_PIPE_NUM - 1] = { .s8I2cDev = -1}
};

static ISP_FSWDR_MODE_E genFSWDRMode[ISP_MAX_PIPE_NUM] = {
	[0 ... ISP_MAX_PIPE_NUM - 1] = ISP_FSWDR_NORMAL_MODE
};

static HI_U32 gu32MaxTimeGetCnt[ISP_MAX_PIPE_NUM] = {0};
static HI_U32 g_au32InitExposure[ISP_MAX_PIPE_NUM]  = {0};
static HI_U32 g_au32LinesPer500ms[ISP_MAX_PIPE_NUM] = {0};

static HI_U16 g_au16InitWBGain[ISP_MAX_PIPE_NUM][3] = {{0}};
static HI_U16 g_au16SampleRgain[ISP_MAX_PIPE_NUM] = {0};
static HI_U16 g_au16SampleBgain[ISP_MAX_PIPE_NUM] = {0};

/****************************************************************************
 * extern                                                                   *
 ****************************************************************************/
extern const unsigned int soi_sensor_i2c_addr;
extern unsigned int soi_sensor_addr_byte;
extern unsigned int soi_sensor_data_byte;

extern void soi_sensor_init(VI_PIPE ViPipe);
extern void soi_sensor_exit(VI_PIPE ViPipe);
extern void soi_sensor_standby(VI_PIPE ViPipe);
extern void soi_sensor_restart(VI_PIPE ViPipe);
extern int  soi_sensor_write_register(VI_PIPE ViPipe, int addr, int data);
extern int  soi_sensor_read_register(VI_PIPE ViPipe, int addr);

/****************************************************************************
 * local variables                                                            *
 ****************************************************************************/

//sensor fps mode
#define		SOI_SENSOR_1080P_30FPS_LINEAR_MODE	(1)
#define		SOI_SENSOR_1080P_30FPS_WDR_MODE		(2)

#define		SOI_SENSOR_RES_IS_1296P(w, h)		((w) <= 2304 && (h) <= 1296)

#define		SOI_SENSOR_INCREASE_LINES			(0)			/* make real fps less than stand fps because NVR require*/

#define		SOI_SENSOR_VMAX_1080P30_LINEAR		(1125 + SOI_SENSOR_INCREASE_LINES)		///(1134 + SOI_SENSOR_INCREASE_LINES)
#define		SOI_SENSOR_VMAX_1080P30_2wdr1		(2250 + SOI_SENSOR_INCREASE_LINES)
	
#define		SOI_SENSOR_FULL_LINES_MAX			(0x7FFF)
#define		SOI_SENSOR_FULL_LINES_MAX_2wdr1		SOI_SENSOR_VMAX_1080P30_2wdr1


HI_U8		Reg0D	= 0x64;					//min@20180705


static HI_S32 cmos_get_ae_default(VI_PIPE ViPipe, AE_SENSOR_DEFAULT_S *pstAeSnsDft)
{
	ISP_SNS_STATE_S *pstSnsState = HI_NULL;
	
	printf( ">> cmos_get_ae_default()\n" );
	
	CMOS_CHECK_POINTER( pstAeSnsDft );
	SOI_SENSOR_GET_CTX( ViPipe, pstSnsState );
	CMOS_CHECK_POINTER( pstSnsState );
	
	memset( &pstAeSnsDft->stAERouteAttr, 0, sizeof(ISP_AE_ROUTE_S) );
	
	pstAeSnsDft->stIntTimeAccu.enAccuType	 = AE_ACCURACY_LINEAR;
	pstAeSnsDft->stIntTimeAccu.f32Accuracy	 = 1;
	pstAeSnsDft->stIntTimeAccu.f32Offset	 = 0;
	
	pstAeSnsDft->stAgainAccu.enAccuType		 = AE_ACCURACY_TABLE;
	pstAeSnsDft->stAgainAccu.f32Accuracy	 = 1;
	
	pstAeSnsDft->stDgainAccu.enAccuType		 = AE_ACCURACY_LINEAR;
	pstAeSnsDft->stDgainAccu.f32Accuracy	 = 0.0625;
	
	pstAeSnsDft->enMaxIrisFNO				 = ISP_IRIS_F_NO_1_0;
	pstAeSnsDft->enMinIrisFNO				 = ISP_IRIS_F_NO_32_0;
	
	pstAeSnsDft->bAERouteExValid			 = HI_FALSE;
	pstAeSnsDft->stAERouteAttr.u32TotalNum	 = 0;
	pstAeSnsDft->stAERouteAttrEx.u32TotalNum = 0;
	
	switch( pstSnsState->enWDRMode )
	{
		default:
		case WDR_MODE_NONE:   /*linear mode*/
			pstAeSnsDft->au8HistThresh[0]		= 0xd;
			pstAeSnsDft->au8HistThresh[1]		= 0x28;
			pstAeSnsDft->au8HistThresh[2]		= 0x60;
			pstAeSnsDft->au8HistThresh[3]		= 0x80;
			
			pstAeSnsDft->u8AeCompensation		= 56;
			pstAeSnsDft->u32InitExposure		= g_au32InitExposure[ViPipe] ? g_au32InitExposure[ViPipe] : 40000;
//			pstAeSnsDft->enAeExpMode			= AE_EXP_HIGHLIGHT_PRIOR;
			
			pstSnsState->u32FLStd				= SOI_SENSOR_VMAX_1080P30_LINEAR;
			pstAeSnsDft->u32LinesPer500ms		= pstSnsState->u32FLStd * 300 / 20;
			
			pstAeSnsDft->u32FullLinesStd		= pstSnsState->u32FLStd;
			pstAeSnsDft->u32FlickerFreq			= 0;	///50 * 256;
			pstAeSnsDft->u32FullLinesMax		= SOI_SENSOR_FULL_LINES_MAX;
			
//			pstAeSnsDft->u32MaxIntTime			= pstSnsState->u32FLStd - 4;
			pstAeSnsDft->u32MaxIntTime			= pstSnsState->u32FLStd - 4;
			pstAeSnsDft->u32MinIntTime			= 2;
			pstAeSnsDft->u32MaxIntTimeTarget	= pstAeSnsDft->u32MaxIntTime;	//65535;
			pstAeSnsDft->u32MinIntTimeTarget	= pstAeSnsDft->u32MinIntTime;	//2;
			
			pstAeSnsDft->u32MaxAgain			= 15872;	//15.5x
			pstAeSnsDft->u32MinAgain			= 1024;		//1x
			pstAeSnsDft->u32MaxAgainTarget		= pstAeSnsDft->u32MaxAgain;
			pstAeSnsDft->u32MinAgainTarget		= pstAeSnsDft->u32MinAgain;
			
			pstAeSnsDft->u32MaxDgain			= 16;		//16;
			pstAeSnsDft->u32MinDgain			= 16;		//16;
			pstAeSnsDft->u32MaxDgainTarget		= pstAeSnsDft->u32MaxDgain;
			pstAeSnsDft->u32MinDgainTarget		= pstAeSnsDft->u32MinDgain;
			
			pstAeSnsDft->u32ISPDgainShift		= 8;
			pstAeSnsDft->u32MinISPDgainTarget	= 1 << pstAeSnsDft->u32ISPDgainShift;
			pstAeSnsDft->u32MaxISPDgainTarget	= 4 << pstAeSnsDft->u32ISPDgainShift;
			break;
		case WDR_MODE_2To1_LINE:
			pstAeSnsDft->au8HistThresh[0]		= 0xC;
			pstAeSnsDft->au8HistThresh[1]		= 0x18;
			pstAeSnsDft->au8HistThresh[2]		= 0x60;
			pstAeSnsDft->au8HistThresh[3]		= 0x80;
			
			pstAeSnsDft->u8AeCompensation		= 64;					///
			pstAeSnsDft->u32InitExposure		= 18168;				///SOI_SENSOR_VMAX_1080P30_2wdr1;
			pstSnsState->u32FLStd				= SOI_SENSOR_VMAX_1080P30_2wdr1;	///
			
			pstAeSnsDft->u32LinesPer500ms		= pstSnsState->u32FLStd * 30 / 2;
			pstAeSnsDft->u32FullLinesStd		= pstSnsState->u32FLStd;
			pstAeSnsDft->u32FlickerFreq			= 0;	///50 * 256;
			pstAeSnsDft->u32FullLinesMax		= SOI_SENSOR_FULL_LINES_MAX;
			
			pstAeSnsDft->u32MaxIntTime			= 511;		///pstSnsState->u32FLStd - 4;
			pstAeSnsDft->u32MinIntTime			= 8;		///2;
			pstAeSnsDft->u32MaxIntTimeTarget	= pstAeSnsDft->u32MaxIntTime;	///65535;
			pstAeSnsDft->u32MinIntTimeTarget	= pstAeSnsDft->u32MinIntTime;	///8;
			
			pstAeSnsDft->u32MaxAgain			= 15872;	//15.5x
			pstAeSnsDft->u32MinAgain			= 1024;		//1x
			pstAeSnsDft->u32MaxAgainTarget		= pstAeSnsDft->u32MaxAgain;
			pstAeSnsDft->u32MinAgainTarget		= pstAeSnsDft->u32MinAgain;
			
			pstAeSnsDft->u32MaxDgain			= 1024;		//16;
			pstAeSnsDft->u32MinDgain			= 1024;		//16;
			pstAeSnsDft->u32MaxDgainTarget		= pstAeSnsDft->u32MaxDgain;
			pstAeSnsDft->u32MinDgainTarget		= pstAeSnsDft->u32MinDgain;
			
			pstAeSnsDft->u32LFMaxShortTime		= 511;		///688;
			pstAeSnsDft->u32MaxIntTimeStep		= 128;		///3000;
			pstAeSnsDft->u32LFMinExposure		= 8;		///880000;
			
			//??
			pstAeSnsDft->u16ManRatioEnable		= HI_FALSE;	///HI_TRUE;
			//pstAeSnsDft->u32Ratio				= 0x400;	///11*64;
			
			pstAeSnsDft->u32ISPDgainShift		= 8;
			pstAeSnsDft->u32MinISPDgainTarget	= 1 << pstAeSnsDft->u32ISPDgainShift;
			pstAeSnsDft->u32MaxISPDgainTarget	= 2 << pstAeSnsDft->u32ISPDgainShift;
			break;
	}
	
	printf( "<< cmos_get_ae_default()\n" );
	return HI_SUCCESS;
}

/* the function of sensor set fps */
static HI_VOID cmos_fps_set(VI_PIPE ViPipe, HI_FLOAT f32Fps, AE_SENSOR_DEFAULT_S *pstAeSnsDft)
{
	//HI_U32	u32FullLines;
	HI_U32	u32VMAX = SOI_SENSOR_VMAX_1080P30_LINEAR;
	ISP_SNS_STATE_S *pstSnsState = HI_NULL;
	
	CMOS_CHECK_POINTER_VOID( pstAeSnsDft );
	SOI_SENSOR_GET_CTX( ViPipe, pstSnsState );
	CMOS_CHECK_POINTER_VOID( pstSnsState );
	
	switch( pstSnsState->u8ImgMode )
	{
		case SOI_SENSOR_1080P_30FPS_WDR_MODE:
			if( (f32Fps <= 30) && (f32Fps >= 5) )  /* Minimum 1.5fps */
			{
				u32VMAX = SOI_SENSOR_VMAX_1080P30_2wdr1 * 30 / f32Fps;
			}
			else
			{
				ISP_TRACE( HI_DBG_ERR, "Not support Fps: %f\n", f32Fps );
				return;
			}
			u32VMAX = (u32VMAX > SOI_SENSOR_FULL_LINES_MAX_2wdr1) ? SOI_SENSOR_FULL_LINES_MAX_2wdr1 : u32VMAX;
			break;
		case SOI_SENSOR_1080P_30FPS_LINEAR_MODE:
			if( (f32Fps <= 30) && (f32Fps >= 0.51) ) ///if( (f32Fps <= 30) && (f32Fps >= 0.11) )
			{
				u32VMAX = SOI_SENSOR_VMAX_1080P30_LINEAR * 30 / f32Fps;
			}
			else
			{
				printf( "Not support Fps: %f\n", f32Fps );
				return;
			}
			u32VMAX = (u32VMAX > SOI_SENSOR_FULL_LINES_MAX) ? SOI_SENSOR_FULL_LINES_MAX : u32VMAX;
			break;
		default:
			return;
	}
	
	pstSnsState->u32FLStd = u32VMAX;
	
	if( WDR_MODE_NONE == pstSnsState->enWDRMode )
	{
		pstSnsState->u32FLStd = (pstSnsState->u32FLStd > SOI_SENSOR_FULL_LINES_MAX) ? SOI_SENSOR_FULL_LINES_MAX : pstSnsState->u32FLStd;
		pstSnsState->astRegsInfo[0].astI2cData[R23_IDX].u32Data = (pstSnsState->u32FLStd >> 8) & 0xFF;	//0x23
		pstSnsState->astRegsInfo[0].astI2cData[R22_IDX].u32Data = (pstSnsState->u32FLStd     ) & 0xFF;	//0x22
	}
	else //WDR_MODE_2To1_LINE
	{
		pstSnsState->u32FLStd = (pstSnsState->u32FLStd > SOI_SENSOR_FULL_LINES_MAX_2wdr1) ? SOI_SENSOR_FULL_LINES_MAX_2wdr1 : pstSnsState->u32FLStd;
		pstSnsState->astRegsInfo[0].astI2cData[R23_IDX].u32Data = (pstSnsState->u32FLStd >> 8) & 0xFF;	//0x23
		pstSnsState->astRegsInfo[0].astI2cData[R22_IDX].u32Data = (pstSnsState->u32FLStd     ) & 0xFF;	//0x22
	}
	
	pstAeSnsDft->f32Fps				= f32Fps;
	pstAeSnsDft->u32LinesPer500ms	= pstSnsState->u32FLStd * f32Fps / 2;
	
	pstAeSnsDft->u32MaxIntTime		= pstSnsState->u32FLStd - 4;
	pstAeSnsDft->u32FullLinesStd	= pstSnsState->u32FLStd;
	pstSnsState->au32FL[0]			= pstSnsState->u32FLStd;
	pstAeSnsDft->u32FullLines		= pstSnsState->au32FL[0];
	
	//printf( "<< cmos_fps_set(): f32Fps=%f, pstSnsState->u32FLStd=%d\n", f32Fps, pstSnsState->u32FLStd );
}

static HI_VOID cmos_slow_framerate_set(VI_PIPE ViPipe, HI_U32 u32FullLines, AE_SENSOR_DEFAULT_S *pstAeSnsDft)
{
	ISP_SNS_STATE_S *pstSnsState = HI_NULL;
	
	CMOS_CHECK_POINTER_VOID( pstAeSnsDft );
	SOI_SENSOR_GET_CTX( ViPipe, pstSnsState );
	CMOS_CHECK_POINTER_VOID( pstSnsState );
	
	if( WDR_MODE_NONE == pstSnsState->enWDRMode )
	{
		u32FullLines = (u32FullLines > SOI_SENSOR_FULL_LINES_MAX ) ? SOI_SENSOR_FULL_LINES_MAX : u32FullLines;
		pstSnsState->au32FL[0] = u32FullLines;
		
		pstAeSnsDft->u32FullLines = pstSnsState->au32FL[0];
		pstAeSnsDft->u32MaxIntTime = pstSnsState->au32FL[0] - 4;
		
		pstSnsState->astRegsInfo[0].astI2cData[R23_IDX].u32Data = (u32FullLines >> 8) & 0xFF;
		pstSnsState->astRegsInfo[0].astI2cData[R22_IDX].u32Data = (u32FullLines     ) & 0xFF;
	}
	else //WDR_MODE_2To1_LINE
	{
		u32FullLines = (u32FullLines > SOI_SENSOR_FULL_LINES_MAX_2wdr1) ? SOI_SENSOR_FULL_LINES_MAX_2wdr1 : u32FullLines;
		pstSnsState->au32FL[0] = u32FullLines;
		
		pstAeSnsDft->u32FullLines = pstSnsState->au32FL[0];
		pstAeSnsDft->u32MaxIntTime = pstSnsState->au32FL[0] - 4;
		
		pstSnsState->astRegsInfo[0].astI2cData[R23_IDX].u32Data = (u32FullLines >> 8) & 0xFF;
		pstSnsState->astRegsInfo[0].astI2cData[R22_IDX].u32Data = (u32FullLines     ) & 0xFF;
	}
	
	//printf( "<< cmos_slow_framerate_set()\n" );
}

/* while isp notify ae to update sensor regs, ae call these funcs. */
static HI_VOID cmos_inttime_update(VI_PIPE ViPipe, HI_U32 u32IntTime)
{
	static HI_BOOL	bFirst = HI_TRUE; 
	//HI_U32	tmp = 0;
	//HI_U32	u32Val;
	ISP_SNS_STATE_S *pstSnsState = HI_NULL;
	
	SOI_SENSOR_GET_CTX( ViPipe, pstSnsState );
	CMOS_CHECK_POINTER_VOID( pstSnsState );
	
	if( WDR_MODE_2To1_LINE == pstSnsState->enWDRMode )
	{
		#ifdef _WDR_OPT //min@20180412
		static HI_U32	exp[2]	= {-1,-1};
		static HI_U8	sexp[2] = { 1, 1};
		//HI_U32		tmp = 0;
		
		if( bFirst ) /* short exposure */
		{
			pstSnsState->au32WDRIntTime[0] = u32IntTime;
			
			// Calculate short exposure value.
			sexp[1] = sexp[0];
			sexp[0] = (u32IntTime < 512) ? ((u32IntTime - 1) >> 1) : 0xFF;
			
			exp[0] = sexp[0];
			if( ((exp[0] << 1) + 1 + exp[1]) > SOI_SENSOR_FULL_LINES_MAX_2wdr1 )
				exp[1] = SOI_SENSOR_FULL_LINES_MAX_2wdr1 - (exp[0] << 1) - 1;
			
			bFirst = HI_FALSE;
		}
		else /* long exposure */
		{
			pstSnsState->au32WDRIntTime[1] = u32IntTime;
			
			exp[1] = u32IntTime;
			if( ((exp[0] << 1) + 1 + exp[1]) > SOI_SENSOR_FULL_LINES_MAX_2wdr1 )
				exp[1] = SOI_SENSOR_FULL_LINES_MAX_2wdr1 - (exp[0] << 1) - 1;
 			
 			pstSnsState->astRegsInfo[0].astI2cData[R05_IDX].u32Data = exp[0];
			pstSnsState->astRegsInfo[0].astI2cData[R01_IDX].u32Data = (exp[1]     ) & 0xFF;
			pstSnsState->astRegsInfo[0].astI2cData[R02_IDX].u32Data = (exp[1] >> 8) & 0xFF;
			
			bFirst = HI_TRUE;
		}
		#endif //_WDR_OPT
	}
	else //WDR_MODE_NONE
	{
		pstSnsState->astRegsInfo[0].astI2cData[R01_IDX].u32Data = (u32IntTime     ) & 0xFF;
		pstSnsState->astRegsInfo[0].astI2cData[R02_IDX].u32Data = (u32IntTime >> 8) & 0xFF;
		bFirst = HI_TRUE;
	}
}

static HI_VOID cmos_again_calc_table(VI_PIPE ViPipe, HI_U32 *pu32AgainLin, HI_U32 *pu32AgainDb)
{
	int again;
	int tmp = 0;
	
	CMOS_CHECK_POINTER_VOID( pu32AgainLin );
	CMOS_CHECK_POINTER_VOID( pu32AgainDb );
	
	again = *pu32AgainLin >> 6;  //10bit -> 4bit, 1 = 16;
	
	while( again > 31 )
	{
		again = again/2;
		tmp++;
	}
	if( again > 15 )
		again = again-16;
	
	*pu32AgainDb = (tmp<<4)|again;
}

static HI_VOID cmos_dgain_calc_table(VI_PIPE ViPipe, HI_U32 *pu32DgainLin, HI_U32 *pu32DgainDb)
{
	CMOS_CHECK_POINTER_VOID( pu32DgainLin );
	CMOS_CHECK_POINTER_VOID( pu32DgainDb );
}

static HI_VOID cmos_gains_update(VI_PIPE ViPipe, HI_U32 u32Again, HI_U32 u32Dgain)
{
	//static HI_U32	again = -1;
	//static HI_U32	dgain = -1;
	//HI_S32	tmp	= 0;
	HI_U32	gain = 0;
	ISP_SNS_STATE_S *pstSnsState = HI_NULL;
	
	SOI_SENSOR_GET_CTX( ViPipe, pstSnsState );
	CMOS_CHECK_POINTER_VOID( pstSnsState );
	
	// Update AGain.
	gain = u32Again & 0xFF;
	pstSnsState->astRegsInfo[0].astI2cData[R00_IDX].u32Data = gain & 0xFF;
	
	#ifdef _SOI_BSUN_OPT //min@20180105
	{	//min@20180115 update //min@20180112 update
		static HI_U8	BSunMode = 0xFF;	//0
		
		if( gain >= 0x30 ) { // When AGain >= 8X
			if( BSunMode != 2 ) {
				pstSnsState->astRegsInfo[0].astI2cData[R66_IDX].u32Data	= 0x44;
				pstSnsState->astRegsInfo[0].astI2cData[R0C_IDX].u32Data	= 0x40;
				BSunMode = 2;
			}
		}
		else if( gain >= 0x20 ) { // When AGain = 4X ~ 8X
			if( BSunMode != 1 ) {
				pstSnsState->astRegsInfo[0].astI2cData[R66_IDX].u32Data	= 0x44;
				pstSnsState->astRegsInfo[0].astI2cData[R0C_IDX].u32Data	= 0x00;
				BSunMode = 1;
			}
		}
		else { // When AGain < 4X
			if( BSunMode != 0 ) {
				pstSnsState->astRegsInfo[0].astI2cData[R66_IDX].u32Data	= 0x04;
				pstSnsState->astRegsInfo[0].astI2cData[R0C_IDX].u32Data	= 0x00;
				BSunMode = 0;
			}
		}
	}
	#endif
}

static HI_VOID cmos_get_inttime_max(VI_PIPE ViPipe, HI_U16 u16ManRatioEnable, HI_U32 *au32Ratio, HI_U32 *au32IntTimeMax, HI_U32 *au32IntTimeMin, HI_U32 *pu32LFMaxIntTime)
{
	return;
}

/* Only used in LINE_WDR mode */
static HI_VOID cmos_ae_fswdr_attr_set(VI_PIPE ViPipe, AE_FSWDR_ATTR_S *pstAeFSWDRAttr)
{
	CMOS_CHECK_POINTER_VOID( pstAeFSWDRAttr );
	
	genFSWDRMode[ViPipe] = pstAeFSWDRAttr->enFSWDRMode;
	gu32MaxTimeGetCnt[ViPipe] = 0;
}

static HI_S32 cmos_init_ae_exp_function(AE_SENSOR_EXP_FUNC_S *pstExpFuncs)
{
	CMOS_CHECK_POINTER( pstExpFuncs );
	
	memset( pstExpFuncs, 0, sizeof(AE_SENSOR_EXP_FUNC_S) );
	
	pstExpFuncs->pfn_cmos_get_ae_default	 = cmos_get_ae_default;
	pstExpFuncs->pfn_cmos_fps_set			 = cmos_fps_set;
	pstExpFuncs->pfn_cmos_slow_framerate_set = cmos_slow_framerate_set;
	pstExpFuncs->pfn_cmos_inttime_update	 = cmos_inttime_update;
	pstExpFuncs->pfn_cmos_gains_update		 = cmos_gains_update;
	pstExpFuncs->pfn_cmos_again_calc_table	 = cmos_again_calc_table;
///	pstExpFuncs->pfn_cmos_dgain_calc_table	 = cmos_dgain_calc_table;
///	pstExpFuncs->pfn_cmos_get_inttime_max	 = cmos_get_inttime_max;
///	pstExpFuncs->pfn_cmos_ae_fswdr_attr_set	 = cmos_ae_fswdr_attr_set;
	
	return HI_SUCCESS;
}

/* Rgain and Bgain of the golden sample */
#define		GOLDEN_RGAIN	0
#define		GOLDEN_BGAIN	0

static HI_S32 cmos_get_awb_default(VI_PIPE ViPipe, AWB_SENSOR_DEFAULT_S *pstAwbSnsDft)
{
	ISP_SNS_STATE_S *pstSnsState = HI_NULL;
	
	CMOS_CHECK_POINTER( pstAwbSnsDft );
	SOI_SENSOR_GET_CTX( ViPipe, pstSnsState );
	CMOS_CHECK_POINTER( pstSnsState );
	
	memset( pstAwbSnsDft, 0, sizeof(AWB_SENSOR_DEFAULT_S) );
	pstAwbSnsDft->u16WbRefTemp		= 4970;
	pstAwbSnsDft->au16GainOffset[0] = 410;
	pstAwbSnsDft->au16GainOffset[1] = 256;
	pstAwbSnsDft->au16GainOffset[2] = 256;
	pstAwbSnsDft->au16GainOffset[3] = 350;
	pstAwbSnsDft->as32WbPara[0]		= 59;
	pstAwbSnsDft->as32WbPara[1]		= 197;
	pstAwbSnsDft->as32WbPara[2]		= 0;
	pstAwbSnsDft->as32WbPara[3]		= 170828;
	pstAwbSnsDft->as32WbPara[4]		= 128;
	pstAwbSnsDft->as32WbPara[5]		= -118550;
	pstAwbSnsDft->u16GoldenRgain	= GOLDEN_RGAIN;
	pstAwbSnsDft->u16GoldenBgain	= GOLDEN_BGAIN;
	
	switch( pstSnsState->enWDRMode )
	{
		default:
		case WDR_MODE_NONE:
			memcpy( &pstAwbSnsDft->stCcm,    &g_stAwbCcm,      sizeof(AWB_CCM_S) );
			memcpy( &pstAwbSnsDft->stAgcTbl, &g_stAwbAgcTable, sizeof(AWB_AGC_TABLE_S) );
			break;
		case WDR_MODE_2To1_LINE:
			memcpy( &pstAwbSnsDft->stCcm,    &g_stWdrAwbCcm,      sizeof(AWB_CCM_S) );
			memcpy( &pstAwbSnsDft->stAgcTbl, &g_stWdrAwbAgcTable, sizeof(AWB_AGC_TABLE_S) );
			break;
	}
	
	pstAwbSnsDft->u16InitRgain	 = g_au16InitWBGain[ViPipe][0];
	pstAwbSnsDft->u16InitGgain	 = g_au16InitWBGain[ViPipe][1];
	pstAwbSnsDft->u16InitBgain	 = g_au16InitWBGain[ViPipe][2];
	pstAwbSnsDft->u16SampleRgain = g_au16SampleRgain[ViPipe];
	pstAwbSnsDft->u16SampleBgain = g_au16SampleBgain[ViPipe];
	
	return HI_SUCCESS;
}

static HI_S32 cmos_init_awb_exp_function(AWB_SENSOR_EXP_FUNC_S *pstExpFuncs)
{
	CMOS_CHECK_POINTER( pstExpFuncs );
	
	memset( pstExpFuncs, 0, sizeof(AWB_SENSOR_EXP_FUNC_S) );
	pstExpFuncs->pfn_cmos_get_awb_default = cmos_get_awb_default;
	
	return HI_SUCCESS;
}

static ISP_CMOS_DNG_COLORPARAM_S g_stDngColorParam = {
	{378, 256, 430},
	{439, 256, 439}
};

static HI_S32 cmos_get_isp_default(VI_PIPE ViPipe, ISP_CMOS_DEFAULT_S *pstDef)
{
	ISP_SNS_STATE_S *pstSnsState = HI_NULL;
	
	printf( ">> cmos_get_isp_default()\n" );
	
	CMOS_CHECK_POINTER( pstDef );
	SOI_SENSOR_GET_CTX( ViPipe, pstSnsState );
	CMOS_CHECK_POINTER( pstSnsState );
	
	memset( pstDef, 0, sizeof(ISP_CMOS_DEFAULT_S) );
	
	//hi3516ev200
	pstDef->unKey.bit1Ca       = 1;
	pstDef->pstCa              = &g_stIspCA;
	pstDef->unKey.bit1Dpc      = 1;
	pstDef->pstDpc             = &g_stCmosDpc;
	pstDef->unKey.bit1Wdr      = 1;
	pstDef->pstWdr             = &g_stIspWDR;
	pstDef->unKey.bit1Lsc      = 0;
	pstDef->pstLsc             = &g_stCmosLsc;
	pstDef->unKey.bit1PreGamma = 0;
	pstDef->pstPreGamma        = &g_stPreGamma;
	
	switch( pstSnsState->enWDRMode )
	{
		default:
		case WDR_MODE_NONE:
	        pstDef->unKey.bit1Demosaic       = 1;
	        pstDef->pstDemosaic              = &g_stIspDemosaic;
	        pstDef->unKey.bit1Sharpen        = 1;
	        pstDef->pstSharpen               = &g_stIspYuvSharpen;
	        pstDef->unKey.bit1Drc            = 1;
	        pstDef->pstDrc                   = &g_stIspDRC;
	        pstDef->unKey.bit1BayerNr        = 1;
	        pstDef->pstBayerNr               = &g_stIspBayerNr;
	        pstDef->unKey.bit1AntiFalseColor = 1;
	        pstDef->pstAntiFalseColor        = &g_stIspAntiFalseColor;
	        pstDef->unKey.bit1Ldci           = 1;
	        pstDef->pstLdci                  = &g_stIspLdci;
	        pstDef->unKey.bit1Gamma          = 1;
	        pstDef->pstGamma                 = &g_stIspGamma;
	        pstDef->unKey.bit1Ge             = 1;
	        pstDef->pstGe                    = &g_stIspGe;
	        pstDef->unKey.bit1Dehaze         = 1;
	        pstDef->pstDehaze                = &g_stIspDehaze;
	        pstDef->unKey.bit1Rgbir          = 0;
	        memcpy(&pstDef->stNoiseCalibration,   &g_stIspNoiseCalibratio, sizeof(ISP_CMOS_NOISE_CALIBRATION_S));
	        break;
		case WDR_MODE_2To1_LINE:
	        break;
	}
	
	//hi3516ev200
	pstDef->stSensorMode.u32SensorID	= SOI_SENSOR_ID;
	pstDef->stSensorMode.u8SensorMode	= pstSnsState->u8ImgMode;
	
	memcpy( &pstDef->stDngColorParam, &g_stDngColorParam, sizeof(ISP_CMOS_DNG_COLORPARAM_S) );
	
	switch( pstSnsState->u8ImgMode )
	{
	    default:
	    case SOI_SENSOR_1080P_30FPS_LINEAR_MODE:
	        pstDef->stSensorMode.stDngRawFormat.u8BitsPerSample	= 10;
	        pstDef->stSensorMode.stDngRawFormat.u32WhiteLevel	= 4095;
	        break;
	}
	
	pstDef->stSensorMode.stDngRawFormat.stDefaultScale.stDefaultScaleH.u32Denominator	= 1;
	pstDef->stSensorMode.stDngRawFormat.stDefaultScale.stDefaultScaleH.u32Numerator		= 1;
	pstDef->stSensorMode.stDngRawFormat.stDefaultScale.stDefaultScaleV.u32Denominator	= 1;
	pstDef->stSensorMode.stDngRawFormat.stDefaultScale.stDefaultScaleV.u32Numerator		= 1;
	pstDef->stSensorMode.stDngRawFormat.stCfaRepeatPatternDim.u16RepeatPatternDimRows	= 2;
	pstDef->stSensorMode.stDngRawFormat.stCfaRepeatPatternDim.u16RepeatPatternDimCols	= 2;
	pstDef->stSensorMode.stDngRawFormat.stBlcRepeatDim.u16BlcRepeatRows	= 2;
	pstDef->stSensorMode.stDngRawFormat.stBlcRepeatDim.u16BlcRepeatCols	= 2;
	pstDef->stSensorMode.stDngRawFormat.enCfaLayout			= CFALAYOUT_TYPE_RECTANGULAR;
	pstDef->stSensorMode.stDngRawFormat.au8CfaPlaneColor[0]	= 0;
	pstDef->stSensorMode.stDngRawFormat.au8CfaPlaneColor[1]	= 1;
	pstDef->stSensorMode.stDngRawFormat.au8CfaPlaneColor[2]	= 2;
	pstDef->stSensorMode.stDngRawFormat.au8CfaPattern[0]	= 0;
	pstDef->stSensorMode.stDngRawFormat.au8CfaPattern[1]	= 1;
	pstDef->stSensorMode.stDngRawFormat.au8CfaPattern[2]	= 1;
	pstDef->stSensorMode.stDngRawFormat.au8CfaPattern[3]	= 2;
	pstDef->stSensorMode.bValidDngRawFormat					= HI_TRUE;
	
	printf( "<< cmos_get_isp_default()\n" );
	return HI_SUCCESS;
}

static HI_S32 cmos_get_isp_black_level(VI_PIPE ViPipe, ISP_CMOS_BLACK_LEVEL_S *pstBlackLevel)
{
	HI_S32  i;
	ISP_SNS_STATE_S *pstSnsState = HI_NULL;
	
	CMOS_CHECK_POINTER( pstBlackLevel );
	SOI_SENSOR_GET_CTX( ViPipe, pstSnsState );
	CMOS_CHECK_POINTER( pstSnsState );
	
	/* Don't need to update black level when iso change */
	pstBlackLevel->bUpdate = HI_FALSE;
	
	for( i=0; i<4; i++ )
	{
		pstBlackLevel->au16BlackLevel[i] = 0x40;
	}
	
	return HI_SUCCESS;
}

static HI_VOID cmos_set_pixel_detect(VI_PIPE ViPipe, HI_BOOL bEnable)
{
	HI_U32	u32FullLines_5Fps = 0;
	HI_U32	u32MaxIntTime_5Fps = 0;
	ISP_SNS_STATE_S *pstSnsState = HI_NULL;
	
	SOI_SENSOR_GET_CTX( ViPipe, pstSnsState );
	CMOS_CHECK_POINTER_VOID( pstSnsState );
	
	u32FullLines_5Fps	= (u32FullLines_5Fps > SOI_SENSOR_FULL_LINES_MAX) ? SOI_SENSOR_FULL_LINES_MAX : u32FullLines_5Fps;
	u32MaxIntTime_5Fps	= u32FullLines_5Fps - 4;
	
	if( bEnable ) /* setup for ISP pixel calibration mode */
	{
		soi_sensor_write_register( ViPipe, 0x23, (u32FullLines_5Fps  >> 8) & 0xFF );	// 5fps
		soi_sensor_write_register( ViPipe, 0x22, (u32FullLines_5Fps      ) & 0xFF );	// 5fps
		soi_sensor_write_register( ViPipe, 0x02, (u32MaxIntTime_5Fps >> 8) & 0xFF );	// max exposure lines
		soi_sensor_write_register( ViPipe, 0x01, (u32MaxIntTime_5Fps     ) & 0xFF );	// max exposure lines
		soi_sensor_write_register( ViPipe, 0x00, 0x00 );								// min AG
	}
	else /* setup for ISP 'normal mode' */
	{
		soi_sensor_write_register( ViPipe, 0x23, (pstSnsState->u32FLStd >> 8) & 0xFF );
		soi_sensor_write_register( ViPipe, 0x22, (pstSnsState->u32FLStd     ) & 0xFF );
		pstSnsState->bSyncInit = HI_FALSE;
	}
}

static HI_S32 cmos_set_wdr_mode(VI_PIPE ViPipe, HI_U8 u8Mode)
{
	ISP_SNS_STATE_S *pstSnsState = HI_NULL;

	printf( ">> cmos_set_wdr_mode(): %d\n", u8Mode );
	
	SOI_SENSOR_GET_CTX( ViPipe, pstSnsState );
	CMOS_CHECK_POINTER( pstSnsState );
	
	pstSnsState->bSyncInit = HI_FALSE;
	
	switch( u8Mode )
	{
		case WDR_MODE_NONE:
			pstSnsState->enWDRMode	= WDR_MODE_NONE;
			pstSnsState->u8ImgMode	= SOI_SENSOR_1080P_30FPS_LINEAR_MODE;
			pstSnsState->u32FLStd	= SOI_SENSOR_VMAX_1080P30_LINEAR;
			ISP_TRACE( HI_DBG_ERR, "linear mode\n" );
			break;
		case WDR_MODE_2To1_LINE:
			pstSnsState->enWDRMode	= WDR_MODE_2To1_LINE;
			pstSnsState->u8ImgMode	= SOI_SENSOR_1080P_30FPS_WDR_MODE;
			pstSnsState->u32FLStd	= SOI_SENSOR_VMAX_1080P30_2wdr1 << 1;
			pstSnsState->au32FL[0]	= SOI_SENSOR_VMAX_1080P30_2wdr1 << 1;
			memset( pstSnsState->au32WDRIntTime, 0, sizeof(pstSnsState->au32WDRIntTime) );	///
		ISP_TRACE( HI_DBG_ERR, "2to1 line WDR mode\n" );
			break;
		default:
			ISP_TRACE( HI_DBG_ERR, "NOT support this mode!\n" );
			return HI_FAILURE;
	}
	
	pstSnsState->au32FL[0] = pstSnsState->u32FLStd;
	pstSnsState->au32FL[1] = pstSnsState->au32FL[0];
	memset( pstSnsState->au32WDRIntTime, 0, sizeof(pstSnsState->au32WDRIntTime) );
	
	printf( "<< cmos_set_wdr_mode()\n" );
	return HI_SUCCESS;
}

static HI_S32 cmos_get_sns_regs_info(VI_PIPE ViPipe, ISP_SNS_REGS_INFO_S *pstSnsRegsInfo)
{
	HI_S32	i;
	ISP_SNS_STATE_S *pstSnsState = HI_NULL;
	
	//printf( ">> cmos_get_sns_regs_info()\n" );
	
	CMOS_CHECK_POINTER( pstSnsRegsInfo );
	SOI_SENSOR_GET_CTX( ViPipe, pstSnsState );
	CMOS_CHECK_POINTER( pstSnsState );
	
	if( (HI_FALSE == pstSnsState->bSyncInit) || (HI_FALSE == pstSnsRegsInfo->bConfig) )
	{
		pstSnsState->astRegsInfo[0].enSnsType			= ISP_SNS_I2C_TYPE;
		pstSnsState->astRegsInfo[0].unComBus.s8I2cDev	= g_aunSoiSensorBusInfo[ViPipe].s8I2cDev;	//hi3516ev200
		pstSnsState->astRegsInfo[0].u8Cfg2ValidDelayMax	= 2;
		pstSnsState->astRegsInfo[0].u32RegNum			= REG_IDX_MAX;
		
		for( i=0; i<pstSnsState->astRegsInfo[0].u32RegNum; i++ ) {
			pstSnsState->astRegsInfo[0].astI2cData[i].bUpdate		 = HI_TRUE;
			pstSnsState->astRegsInfo[0].astI2cData[i].u8DevAddr		 = soi_sensor_i2c_addr;
			pstSnsState->astRegsInfo[0].astI2cData[i].u32AddrByteNum = soi_sensor_addr_byte;
			pstSnsState->astRegsInfo[0].astI2cData[i].u32DataByteNum = soi_sensor_data_byte;
		}
		
		pstSnsState->astRegsInfo[0].astI2cData[R01_IDX].u8DelayFrmNum	= 0;
		pstSnsState->astRegsInfo[0].astI2cData[R01_IDX].u32RegAddr		= 0x01;		//shutter
		pstSnsState->astRegsInfo[0].astI2cData[R02_IDX].u8DelayFrmNum	= 0;
		pstSnsState->astRegsInfo[0].astI2cData[R02_IDX].u32RegAddr		= 0x02;		//shutter
		pstSnsState->astRegsInfo[0].astI2cData[R00_IDX].u8DelayFrmNum	= 0;
		pstSnsState->astRegsInfo[0].astI2cData[R00_IDX].u32RegAddr		= 0x00;		//gain
		pstSnsState->astRegsInfo[0].astI2cData[R22_IDX].u8DelayFrmNum	= 0;
		pstSnsState->astRegsInfo[0].astI2cData[R22_IDX].u32RegAddr		= 0x22;		//Vmax
		pstSnsState->astRegsInfo[0].astI2cData[R23_IDX].u8DelayFrmNum	= 0;
		pstSnsState->astRegsInfo[0].astI2cData[R23_IDX].u32RegAddr		= 0x23;		//Vmax
		#ifdef _WDR_OPT //min@20180412
		pstSnsState->astRegsInfo[0].astI2cData[R05_IDX].u8DelayFrmNum	= 0;
		pstSnsState->astRegsInfo[0].astI2cData[R05_IDX].u32RegAddr		= 0x05;
		#endif
		#ifdef _SOI_BSUN_OPT //min@20180115 begin
		pstSnsState->astRegsInfo[0].astI2cData[R66_IDX].u8DelayFrmNum	= 2;		//2;
		pstSnsState->astRegsInfo[0].astI2cData[R66_IDX].u32RegAddr		= 0x66;		//BSUN_ADDR_H;
		pstSnsState->astRegsInfo[0].astI2cData[R0C_IDX].u8DelayFrmNum	= 2;		//2;
		pstSnsState->astRegsInfo[0].astI2cData[R0C_IDX].u32RegAddr		= 0x0C;		//BSUN_ADDR_L;
		#endif //min@20180115 end
		
		pstSnsState->bSyncInit = HI_TRUE;
	}
	else
	{
		for( i=0; i<pstSnsState->astRegsInfo[0].u32RegNum; i++ )
		{
			if( pstSnsState->astRegsInfo[0].astI2cData[i].u32Data == pstSnsState->astRegsInfo[1].astI2cData[i].u32Data )
			{
				pstSnsState->astRegsInfo[0].astI2cData[i].bUpdate = HI_FALSE;
			}
			else
			{
				pstSnsState->astRegsInfo[0].astI2cData[i].bUpdate = HI_TRUE;
			}
		}
	}
	
	pstSnsRegsInfo->bConfig = HI_FALSE;
	memcpy( pstSnsRegsInfo, &pstSnsState->astRegsInfo[0], sizeof(ISP_SNS_REGS_INFO_S) );
	memcpy( &pstSnsState->astRegsInfo[1], &pstSnsState->astRegsInfo[0], sizeof(ISP_SNS_REGS_INFO_S) );
	
	pstSnsState->au32FL[1] = pstSnsState->au32FL[0];
	
	//printf( "<< cmos_get_sns_regs_info()\n" );
	return HI_SUCCESS;
}

static HI_S32 cmos_set_image_mode(VI_PIPE ViPipe, ISP_CMOS_SENSOR_IMAGE_MODE_S *pstSensorImageMode)
{
	HI_U8	u8SensorImageMode = 0;
	ISP_SNS_STATE_S *pstSnsState = HI_NULL;
	
	printf( ">> cmos_set_image_mode()\n" );
	
	CMOS_CHECK_POINTER( pstSensorImageMode );
	SOI_SENSOR_GET_CTX( ViPipe, pstSnsState );
	CMOS_CHECK_POINTER( pstSnsState );
	
	u8SensorImageMode = pstSnsState->u8ImgMode;
	pstSnsState->bSyncInit = HI_FALSE;
	
	if( (pstSensorImageMode->u16Width <= 1920) && (pstSensorImageMode->u16Height <= 1080) )
	{
		#if 1 //min@20180705 test
		if( WDR_MODE_2To1_LINE == pstSnsState->enWDRMode )
		{
			if( pstSensorImageMode->f32Fps <= 30 )
			{
				u8SensorImageMode = SOI_SENSOR_1080P_30FPS_WDR_MODE;
			}
			else
			{
				ISP_TRACE( HI_DBG_ERR, "Not support! Width:%d, Height:%d, Fps:%f, WDRMode:%d\n",
				          pstSensorImageMode->u16Width,
				          pstSensorImageMode->u16Height,
				          pstSensorImageMode->f32Fps,
				          pstSnsState->enWDRMode );
				return HI_FAILURE;
			}
		}
		else
		{
			if( pstSensorImageMode->f32Fps <= 30 )
			{
				u8SensorImageMode = SOI_SENSOR_1080P_30FPS_LINEAR_MODE;
			}
			else if( pstSensorImageMode->f32Fps <= 60 )
			{
				u8SensorImageMode = SOI_SENSOR_1080P_30FPS_WDR_MODE;
			}
			else
			{
			    ISP_TRACE( HI_DBG_ERR, "Not support! Width:%d, Height:%d, Fps:%f, WDRMode:%d\n",
			              pstSensorImageMode->u16Width,
			              pstSensorImageMode->u16Height,
			              pstSensorImageMode->f32Fps,
			              pstSnsState->enWDRMode );
			    return HI_FAILURE;
			}
		}
		#endif
	}
	else
	{
		ISP_TRACE( HI_DBG_ERR, "Not support! Width:%d, Height:%d, Fps:%f, WDRMode:%d\n",
			      pstSensorImageMode->u16Width,
			      pstSensorImageMode->u16Height,
			      pstSensorImageMode->f32Fps,
			      pstSnsState->enWDRMode );
		return HI_FAILURE;
	}
	
	/* Sensor first init */
	if( HI_FALSE == pstSnsState->bInit )
	{
		pstSnsState->u8ImgMode = u8SensorImageMode;
		return HI_SUCCESS;
	}
	
	/* Switch SensorImageMode */
	if( u8SensorImageMode == pstSnsState->u8ImgMode )
	{
		// Don't need to switch SensorImageMode
		return HI_FAILURE;
	}
	
	pstSnsState->u8ImgMode = u8SensorImageMode;
	memset( pstSnsState->au32WDRIntTime, 0, sizeof(pstSnsState->au32WDRIntTime) );
	
	printf( "<< cmos_set_image_mode()\n" );
	return HI_SUCCESS;
}

static HI_VOID sensor_global_init(VI_PIPE ViPipe)
{
	ISP_SNS_STATE_S *pstSnsState = HI_NULL;
	
	printf( ">> sensor_global_init()\n" );
	
	SOI_SENSOR_GET_CTX( ViPipe, pstSnsState );
	CMOS_CHECK_POINTER_VOID( pstSnsState );
	
	//hi3516ev200
	pstSnsState->bInit		= HI_FALSE;
	pstSnsState->bSyncInit	= HI_FALSE;
	pstSnsState->u8ImgMode	= SOI_SENSOR_1080P_30FPS_LINEAR_MODE;
	pstSnsState->enWDRMode	= WDR_MODE_NONE;
	pstSnsState->u32FLStd	= SOI_SENSOR_VMAX_1080P30_LINEAR;
	pstSnsState->au32FL[0]	= SOI_SENSOR_VMAX_1080P30_LINEAR;
	pstSnsState->au32FL[1]	= SOI_SENSOR_VMAX_1080P30_LINEAR;
	
	printf( "<< sensor_global_init()\n" );
}

static HI_S32 cmos_init_sensor_exp_function(ISP_SENSOR_EXP_FUNC_S *pstSensorExpFunc)
{
	CMOS_CHECK_POINTER( pstSensorExpFunc );
	
	memset( pstSensorExpFunc, 0, sizeof(ISP_SENSOR_EXP_FUNC_S) );
	
	pstSensorExpFunc->pfn_cmos_sensor_init			= soi_sensor_init;
	pstSensorExpFunc->pfn_cmos_sensor_exit			= soi_sensor_exit;
	pstSensorExpFunc->pfn_cmos_sensor_global_init	= sensor_global_init;
	pstSensorExpFunc->pfn_cmos_set_image_mode		= cmos_set_image_mode;
	pstSensorExpFunc->pfn_cmos_set_wdr_mode			= cmos_set_wdr_mode;
	
	pstSensorExpFunc->pfn_cmos_get_isp_default		= cmos_get_isp_default;
	pstSensorExpFunc->pfn_cmos_get_isp_black_level	= cmos_get_isp_black_level;
	pstSensorExpFunc->pfn_cmos_set_pixel_detect		= cmos_set_pixel_detect;
	pstSensorExpFunc->pfn_cmos_get_sns_reg_info		= cmos_get_sns_regs_info;
	
	return HI_SUCCESS;
}

/****************************************************************************
 * callback structure                                                       *
 ****************************************************************************/

//hi3516ev200
static HI_S32 soi_sensor_set_bus_info(VI_PIPE ViPipe, ISP_SNS_COMMBUS_U unSNSBusInfo)
{
	g_aunSoiSensorBusInfo[ViPipe].s8I2cDev = unSNSBusInfo.s8I2cDev;
	
	return HI_SUCCESS;
}

//hi3516ev200
static HI_S32 sensor_ctx_init(VI_PIPE ViPipe)
{
	ISP_SNS_STATE_S *pastSnsStateCtx = HI_NULL;
	
	SOI_SENSOR_GET_CTX( ViPipe, pastSnsStateCtx );
	
	if( HI_NULL == pastSnsStateCtx )
	{
		pastSnsStateCtx = (ISP_SNS_STATE_S *)malloc( sizeof(ISP_SNS_STATE_S) );
		if( HI_NULL == pastSnsStateCtx )
		{
			ISP_TRACE( HI_DBG_ERR, "Isp[%d] SnsCtx malloc memory failed!\n", ViPipe );
			return HI_ERR_ISP_NOMEM;
		}
	}
	
	memset( pastSnsStateCtx, 0, sizeof(ISP_SNS_STATE_S) );
	
	SOI_SENSOR_SET_CTX( ViPipe, pastSnsStateCtx );
	
	return HI_SUCCESS;
}

//hi3516ev200
static HI_VOID sensor_ctx_exit(VI_PIPE ViPipe)
{
	ISP_SNS_STATE_S *pastSnsStateCtx = HI_NULL;
	
	SOI_SENSOR_GET_CTX( ViPipe, pastSnsStateCtx );
	SENSOR_FREE( pastSnsStateCtx );
	SOI_SENSOR_RESET_CTX( ViPipe );
}

static HI_S32 sensor_register_callback(VI_PIPE ViPipe, ALG_LIB_S *pstAeLib, ALG_LIB_S *pstAwbLib)
{
	HI_S32	s32Ret;
	ISP_SENSOR_REGISTER_S	stIspRegister;
	AE_SENSOR_REGISTER_S	stAeRegister;
	AWB_SENSOR_REGISTER_S	stAwbRegister;
	ISP_SNS_ATTR_INFO_S		stSnsAttrInfo;
	
	CMOS_CHECK_POINTER( pstAeLib );
	CMOS_CHECK_POINTER( pstAwbLib );
	
	s32Ret = sensor_ctx_init( ViPipe );
	if( HI_SUCCESS != s32Ret ) {
		return HI_FAILURE;
	}
	
	stSnsAttrInfo.eSensorId = SOI_SENSOR_ID;
	
	s32Ret  = cmos_init_sensor_exp_function( &stIspRegister.stSnsExp );
	s32Ret |= HI_MPI_ISP_SensorRegCallBack( ViPipe, &stSnsAttrInfo, &stIspRegister );
	if( HI_SUCCESS != s32Ret ) {
		ISP_TRACE( HI_DBG_ERR, "sensor register callback function failed!\n" );
		return s32Ret;
	}
	
	s32Ret  = cmos_init_ae_exp_function( &stAeRegister.stSnsExp );
	s32Ret |= HI_MPI_AE_SensorRegCallBack( ViPipe, pstAeLib, &stSnsAttrInfo, &stAeRegister );
	if( HI_SUCCESS != s32Ret ) {
		ISP_TRACE( HI_DBG_ERR, "sensor register callback function to ae lib failed!\n" );
		return s32Ret;
	}
	
	s32Ret  = cmos_init_awb_exp_function( &stAwbRegister.stSnsExp );
	s32Ret |= HI_MPI_AWB_SensorRegCallBack( ViPipe, pstAwbLib, &stSnsAttrInfo, &stAwbRegister );
	if( HI_SUCCESS != s32Ret ) {
		ISP_TRACE( HI_DBG_ERR, "sensor register callback function to awb lib failed!\n" );
		return s32Ret;
	}
	
	return HI_SUCCESS;
}

static HI_S32 sensor_unregister_callback(VI_PIPE ViPipe, ALG_LIB_S *pstAeLib, ALG_LIB_S *pstAwbLib)
{
	HI_S32	s32Ret;
	
	CMOS_CHECK_POINTER( pstAeLib );
	CMOS_CHECK_POINTER( pstAwbLib );
	
	s32Ret = HI_MPI_ISP_SensorUnRegCallBack( ViPipe, SOI_SENSOR_ID );
	if( HI_SUCCESS != s32Ret ) {
	    ISP_TRACE( HI_DBG_ERR, "sensor unregister callback function failed!\n" );
	    return s32Ret;
	}
	
	s32Ret = HI_MPI_AE_SensorUnRegCallBack( ViPipe, pstAeLib, SOI_SENSOR_ID );
	if( HI_SUCCESS != s32Ret ) {
	    ISP_TRACE( HI_DBG_ERR, "sensor unregister callback function to ae lib failed!\n" );
	    return s32Ret;
	}
	
	s32Ret = HI_MPI_AWB_SensorUnRegCallBack( ViPipe, pstAwbLib, SOI_SENSOR_ID );
	if( HI_SUCCESS != s32Ret ) {
	    ISP_TRACE( HI_DBG_ERR, "sensor unregister callback function to awb lib failed!\n" );
	    return s32Ret;
	}
	
	sensor_ctx_exit( ViPipe );
	
	return HI_SUCCESS;
}

static HI_S32 sensor_set_init(VI_PIPE ViPipe, ISP_INIT_ATTR_S *pstInitAttr)
{
	CMOS_CHECK_POINTER( pstInitAttr );
	
	g_au32InitExposure[ViPipe]	= pstInitAttr->u32Exposure;
	g_au32LinesPer500ms[ViPipe]	= pstInitAttr->u32LinesPer500ms;
	g_au16InitWBGain[ViPipe][0]	= pstInitAttr->u16WBRgain;
	g_au16InitWBGain[ViPipe][1]	= pstInitAttr->u16WBGgain;
	g_au16InitWBGain[ViPipe][2]	= pstInitAttr->u16WBBgain;
	g_au16SampleRgain[ViPipe]	= pstInitAttr->u16SampleRgain;
	g_au16SampleBgain[ViPipe]	= pstInitAttr->u16SampleBgain;
	
	return HI_SUCCESS;
}

//hi3516ev200
ISP_SNS_OBJ_S stSnsSoiSensorObj = {
	.pfnRegisterCallback	= sensor_register_callback,
	.pfnUnRegisterCallback	= sensor_unregister_callback,
	.pfnStandby				= soi_sensor_standby,
	.pfnRestart				= soi_sensor_restart,
	.pfnMirrorFlip			= HI_NULL,
	.pfnWriteReg			= soi_sensor_write_register,
	.pfnReadReg				= soi_sensor_read_register,
	.pfnSetBusInfo			= soi_sensor_set_bus_info,
	.pfnSetInit				= sensor_set_init
};

int sensor_register_set_flip(int flip)
{
    gu8F23_FlipValue = flip;
    
    return 0;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /* __SOI_SENSOR_CMOS_H_ */

