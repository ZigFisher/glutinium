#ifndef __HP_SYS_H__
#define __HP_SYS_H__


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* End of #ifdef __cplusplus */

#define SYS_ALIGN_WIDTH 64
#define STREAM_MAX_NUM 8


typedef enum{
	SENSOR_PIXELPLUS_PS4100K = 0,
	SENSOR_PIXELPLUS_PS1210K,
	SENSOR_PIXELPLUS_PS3210K,
	SENSOR_SONY_IMX290,
}SENSOR_TYPE_E;

typedef enum
{
	HI3518EV200 = 0,
	HI3518EV201,
}CHIP_E; 

typedef enum
{
    RC_MODE_CBR = 0,
    RC_MODE_VBR,
    RC_MODE_FIXQP
}RC_MODE_E;

typedef struct{
	SIZE_S stSize;
	PIC_SIZE_E e_Size;
	HI_FLOAT	f32FrameRate;
	PAYLOAD_TYPE_E e_payload;
	RC_MODE_E e_RCMode;
}STREAM_FORMAT_S;

typedef struct{
	CHIP_E 	e_chip;
	SENSOR_TYPE_E e_sensor_type;
	WDR_MODE_E	e_wdrMode;
	PIXEL_FORMAT_E e_pixFmt;
	HI_U32	u32StreamNum;/*Number of Stream*/	
	STREAM_FORMAT_S astStreamFmt[STREAM_MAX_NUM];
}IPC_VEDIO_S;


#define GET_VB_PIC_HEADER_SIZE(u32AlignHeight)\
	({\
		unsigned int u32HeadSize;\
        u32HeadSize = VB_HEADER_STRIDE * u32AlignHeight;/* compress header stride 16 */\
        u32HeadSize;\
	})


#define GET_VB_PIXEL_MULTIPLE(e_pixFmt)\
	({\
		float f32Multiple = 1;\
		switch(e_pixFmt)\
		{\
			case PIXEL_FORMAT_YUV_SEMIPLANAR_444:\
				f32Multiple = 3;\
				break;\
			case PIXEL_FORMAT_YUV_SEMIPLANAR_422:\
				f32Multiple = 2;\
				break;\
			case PIXEL_FORMAT_YUV_SEMIPLANAR_420:\
				f32Multiple = 1.5;\
				break;\
			case PIXEL_FORMAT_YUV_400:\
				f32Multiple = 1;\
				break;\
			default:;break;\
		}\
		f32Multiple;\
	})	
    
#define GET_VB_PIC_BLK_SIZE(Width, Height, e_pixFmt)\
    ({\
        unsigned int u32AlignWidth;\
        unsigned int u32AlignHeight;\
        unsigned int u32Size;\
        u32AlignWidth = VB_ALIGN(Width, SYS_ALIGN_WIDTH);\
        u32AlignHeight= VB_ALIGN(Height, SYS_ALIGN_WIDTH);\
        u32Size = (u32AlignWidth * u32AlignHeight + GET_VB_PIC_HEADER_SIZE(u32AlignHeight))\
        		* GET_VB_PIXEL_MULTIPLE(e_pixFmt);\
        u32Size;\
    })
#define GET_VB_BLK_CNT(e_chip)\
    ({\
		unsigned int u32VbBlkCnt = 4;\
		switch(e_chip)\
		{\
			case HI3518EV200:\
				u32VbBlkCnt = 4;\
				break;\
			case HI3518EV201:\
				u32VbBlkCnt = 2;\
				break;\
			default:;break;\
		}\
		u32VbBlkCnt;\
	})

extern IPC_VEDIO_S g_stIpcVedio;

extern HI_S32 mpi_sys_init(IPC_VEDIO_S *pstIpcVedio);
extern HI_VOID mpi_sys_exit(void);
extern HI_S32 s32GetPicSize(PIC_SIZE_E enPicSize, SIZE_S *pstSize);
extern HI_S32 mpi_sys_Bind(IPC_VEDIO_S *pstIpcVedio);
extern HI_S32 mpi_sys_UnBind(IPC_VEDIO_S *pstIpcVedio);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif
