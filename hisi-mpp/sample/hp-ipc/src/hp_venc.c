#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/poll.h>
#include <sys/time.h>
#include <fcntl.h>
#include <errno.h>
#include <pthread.h>
#include <math.h>
#include <unistd.h>
#include <signal.h>

#include "hp_common.h"
#include "hp_sys.h"
#include "hp_ipc.h"
#include "hp_venc.h"
#include "rtsp_server.h"
static ST_VENC_ATTR g_stVencAttr;
static pthread_t gs_VencPid;


static HI_S32 s32GetVencChnAttr(VENC_CHN VencChn,const IPC_VEDIO_S* pstIpcVedio, VENC_CHN_ATTR_S * pstVencChnAttr)
{  
    VENC_ATTR_H264_S stH264Attr;
    VENC_ATTR_H264_CBR_S    stH264Cbr;
    VENC_ATTR_H264_VBR_S    stH264Vbr;
    VENC_ATTR_H264_FIXQP_S  stH264FixQp;
    VENC_ATTR_H265_S        stH265Attr;
    VENC_ATTR_H265_CBR_S    stH265Cbr;
    VENC_ATTR_H265_VBR_S    stH265Vbr;
    VENC_ATTR_H265_FIXQP_S  stH265FixQp;
    VENC_ATTR_MJPEG_S stMjpegAttr;
    VENC_ATTR_MJPEG_FIXQP_S stMjpegeFixQp;
    VENC_ATTR_JPEG_S stJpegAttr;
    SIZE_S stPicSize;
	PIC_SIZE_E enSize;
	PAYLOAD_TYPE_E  enType;
	RC_MODE_E enRcMode;
	HI_U32 u32FrameRate;
	memcpy(&stPicSize,&(pstIpcVedio->astStreamFmt[VencChn].stSize),sizeof(stPicSize));
	enSize = pstIpcVedio->astStreamFmt[VencChn].e_Size;
	enType = pstIpcVedio->astStreamFmt[VencChn].e_payload;
	enRcMode = pstIpcVedio->astStreamFmt[VencChn].e_RCMode;
	u32FrameRate = pstIpcVedio->astStreamFmt[VencChn].f32FrameRate;
    pstVencChnAttr->stVeAttr.enType = enType;
	
    switch(enType)
    {
        case PT_H264:
        {
            stH264Attr.u32MaxPicWidth = stPicSize.u32Width;
            stH264Attr.u32MaxPicHeight = stPicSize.u32Height;
            stH264Attr.u32PicWidth = stPicSize.u32Width;/*the picture width*/
            stH264Attr.u32PicHeight = stPicSize.u32Height;/*the picture height*/
            stH264Attr.u32BufSize  = stPicSize.u32Width * stPicSize.u32Height;/*stream buffer size*/
            stH264Attr.u32Profile  = 0;/*0: baseline; 1:MP; 2:HP;  3:svc_t */
            stH264Attr.bByFrame = HI_TRUE;/*get stream mode is slice mode or frame mode?*/
			stH264Attr.u32BFrameNum = 0;/* 0: not support B frame; >=1: number of B frames */
			stH264Attr.u32RefNum = 1;/* 0: default; number of refrence frame*/
			memcpy(&(pstVencChnAttr->stVeAttr.stAttrH264e), &stH264Attr, sizeof(VENC_ATTR_H264_S));

            if(RC_MODE_CBR == enRcMode)
            {
                pstVencChnAttr->stRcAttr.enRcMode = VENC_RC_MODE_H264CBR;
                stH264Cbr.u32Gop            = 32;
                stH264Cbr.u32StatTime       = 2; /* stream rate statics time(s) */
                stH264Cbr.u32SrcFrmRate      = u32FrameRate;/* input (vi) frame rate */
                stH264Cbr.fr32DstFrmRate = u32FrameRate;/* target frame rate */
				
				switch (enSize)
                {
                  case PIC_QCIF:
                       stH264Cbr.u32BitRate = 256; /* average bit rate */
                       break;
                  case PIC_QVGA:    /* 320 * 240 */
                  case PIC_CIF: 

                	   stH264Cbr.u32BitRate = 512;
                       break;

                  case PIC_D1:
                  case PIC_VGA:	   /* 640 * 480 */
                	   stH264Cbr.u32BitRate = 1024*2;
                       break;
                  case PIC_HD720:   /* 1280 * 720 */
                	   stH264Cbr.u32BitRate = 1024*2;
                	   break;
                  case PIC_HD1080:  /* 1920 * 1080 */
                  	   stH264Cbr.u32BitRate = 1024*4;
                	   break;
                  case PIC_5M:  /* 2592 * 1944 */
                  	   stH264Cbr.u32BitRate = 1024*8;
                	   break;                       
                  default :
                       stH264Cbr.u32BitRate = 1024*4;
                       break;
                }
                
                stH264Cbr.u32FluctuateLevel = 0; /* average bit rate */
                memcpy(&(pstVencChnAttr->stRcAttr.stAttrH264Cbr), &stH264Cbr, sizeof(VENC_ATTR_H264_CBR_S));
            }
            else if (RC_MODE_FIXQP == enRcMode) 
            {
                pstVencChnAttr->stRcAttr.enRcMode = VENC_RC_MODE_H264FIXQP;
                stH264FixQp.u32Gop = 30;
                stH264FixQp.u32SrcFrmRate = u32FrameRate;
                stH264FixQp.fr32DstFrmRate = u32FrameRate;
                stH264FixQp.u32IQp = 20;
                stH264FixQp.u32PQp = 23;
                memcpy(&(pstVencChnAttr->stRcAttr.stAttrH264FixQp), &stH264FixQp,sizeof(VENC_ATTR_H264_FIXQP_S));
            }
            else if (RC_MODE_VBR == enRcMode) 
            {
                pstVencChnAttr->stRcAttr.enRcMode = VENC_RC_MODE_H264VBR;
                stH264Vbr.u32Gop = 30;
                stH264Vbr.u32StatTime = 1;
                stH264Vbr.u32SrcFrmRate = u32FrameRate;
                stH264Vbr.fr32DstFrmRate = u32FrameRate;
                stH264Vbr.u32MinQp = 10;
                stH264Vbr.u32MaxQp = 40;
                switch (enSize)
                {
                  case PIC_QCIF:
                	   stH264Vbr.u32MaxBitRate= 256*3; /* average bit rate */
                	   break;
                  case PIC_QVGA:    /* 320 * 240 */
                  case PIC_CIF:
                	   stH264Vbr.u32MaxBitRate = 512*3;
                       break;
                  case PIC_D1:
                  case PIC_VGA:	   /* 640 * 480 */
                	   stH264Vbr.u32MaxBitRate = 1024*2;
                       break;
                  case PIC_HD720:   /* 1280 * 720 */
                	   stH264Vbr.u32MaxBitRate = 1024*3;
                	   break;
                  case PIC_HD1080:  /* 1920 * 1080 */
                  	   stH264Vbr.u32MaxBitRate = 1024*6;
                	   break;
                  case PIC_5M:  /* 2592 * 1944 */
                  	   stH264Vbr.u32MaxBitRate = 1024*8;
                	   break;                        
                  default :
                       stH264Vbr.u32MaxBitRate = 1024*4;
                       break;
                }
                memcpy(&(pstVencChnAttr->stRcAttr.stAttrH264Vbr), &stH264Vbr, sizeof(VENC_ATTR_H264_VBR_S));
            }
            else
            {
                return HI_FAILURE;
            }
        }
        break;
        
        case PT_MJPEG:
        {
            stMjpegAttr.u32MaxPicWidth = stPicSize.u32Width;
            stMjpegAttr.u32MaxPicHeight = stPicSize.u32Height;
            stMjpegAttr.u32PicWidth = stPicSize.u32Width;
            stMjpegAttr.u32PicHeight = stPicSize.u32Height;
            stMjpegAttr.u32BufSize = (((stPicSize.u32Width+15)>>4)<<4) * (((stPicSize.u32Height+15)>>4)<<4);
            stMjpegAttr.bByFrame = HI_TRUE;  /*get stream mode is field mode  or frame mode*/
            memcpy(&(pstVencChnAttr->stVeAttr.stAttrMjpeg), &stMjpegAttr, sizeof(VENC_ATTR_MJPEG_S));

            if(RC_MODE_FIXQP == enRcMode)
            {
                pstVencChnAttr->stRcAttr.enRcMode = VENC_RC_MODE_MJPEGFIXQP;
                stMjpegeFixQp.u32Qfactor        = 90;
                stMjpegeFixQp.u32SrcFrmRate      = u32FrameRate;
                stMjpegeFixQp.fr32DstFrmRate = u32FrameRate;
                memcpy(&(pstVencChnAttr->stRcAttr.stAttrMjpegeFixQp), &stMjpegeFixQp,
                       sizeof(VENC_ATTR_MJPEG_FIXQP_S));
            }
            else if (RC_MODE_CBR == enRcMode)
            {
                pstVencChnAttr->stRcAttr.enRcMode = VENC_RC_MODE_MJPEGCBR;
                pstVencChnAttr->stRcAttr.stAttrMjpegeCbr.u32StatTime       = 1;
                pstVencChnAttr->stRcAttr.stAttrMjpegeCbr.u32SrcFrmRate      = u32FrameRate;
                pstVencChnAttr->stRcAttr.stAttrMjpegeCbr.fr32DstFrmRate = u32FrameRate;
                pstVencChnAttr->stRcAttr.stAttrMjpegeCbr.u32FluctuateLevel = 0;
                switch (enSize)
                {
                  case PIC_QCIF:
                	   pstVencChnAttr->stRcAttr.stAttrMjpegeCbr.u32BitRate = 384*3; /* average bit rate */
                	   break;
                  case PIC_QVGA:    /* 320 * 240 */
                  case PIC_CIF:
                	   pstVencChnAttr->stRcAttr.stAttrMjpegeCbr.u32BitRate = 768*3;
                       break;
                  case PIC_D1:
                  case PIC_VGA:	   /* 640 * 480 */
                	   pstVencChnAttr->stRcAttr.stAttrMjpegeCbr.u32BitRate = 1024*3*3;
                       break;
                  case PIC_HD720:   /* 1280 * 720 */
                	   pstVencChnAttr->stRcAttr.stAttrMjpegeCbr.u32BitRate = 1024*5*3;
                	   break;
                  case PIC_HD1080:  /* 1920 * 1080 */
                  	   pstVencChnAttr->stRcAttr.stAttrMjpegeCbr.u32BitRate = 1024*10*3;
                	   break;
                  case PIC_5M:  /* 2592 * 1944 */
                  	   pstVencChnAttr->stRcAttr.stAttrMjpegeCbr.u32BitRate = 1024*10*3;
                	   break;                       
                  default :
                       pstVencChnAttr->stRcAttr.stAttrMjpegeCbr.u32BitRate = 1024*10*3;
                       break;
                }
            }
            else if (RC_MODE_VBR == enRcMode) 
            {
                pstVencChnAttr->stRcAttr.enRcMode = VENC_RC_MODE_MJPEGVBR;
                pstVencChnAttr->stRcAttr.stAttrMjpegeVbr.u32StatTime = 1;
                pstVencChnAttr->stRcAttr.stAttrMjpegeVbr.u32SrcFrmRate = u32FrameRate;
                pstVencChnAttr->stRcAttr.stAttrMjpegeVbr.fr32DstFrmRate = u32FrameRate;
                pstVencChnAttr->stRcAttr.stAttrMjpegeVbr.u32MinQfactor = 50;
                pstVencChnAttr->stRcAttr.stAttrMjpegeVbr.u32MaxQfactor = 95;
                switch (enSize)
                {
                  case PIC_QCIF:
                	   pstVencChnAttr->stRcAttr.stAttrMjpegeVbr.u32MaxBitRate= 256*3; /* average bit rate */
                	   break;
                  case PIC_QVGA:    /* 320 * 240 */
                  case PIC_CIF:
                	   pstVencChnAttr->stRcAttr.stAttrMjpegeVbr.u32MaxBitRate = 512*3;
                       break;
                  case PIC_D1:
                  case PIC_VGA:	   /* 640 * 480 */
                	   pstVencChnAttr->stRcAttr.stAttrMjpegeVbr.u32MaxBitRate = 1024*2*3;
                       break;
                  case PIC_HD720:   /* 1280 * 720 */
                	   pstVencChnAttr->stRcAttr.stAttrMjpegeVbr.u32MaxBitRate = 1024*3*3;
                	   break;
                  case PIC_HD1080:  /* 1920 * 1080 */
                  	   pstVencChnAttr->stRcAttr.stAttrMjpegeVbr.u32MaxBitRate = 1024*6*3;
                	   break;
                  case PIC_5M:  /* 2592 * 1944 */
                  	   pstVencChnAttr->stRcAttr.stAttrMjpegeVbr.u32MaxBitRate = 1024*12*3;
                	   break;                        
                  default :
                       pstVencChnAttr->stRcAttr.stAttrMjpegeVbr.u32MaxBitRate = 1024*4*3;
                       break;
                }
            }
            else 
            {
                printf("cann't support other mode in this version!\n");

                return HI_FAILURE;
            }
        }
        break;
            
        case PT_JPEG:
            stJpegAttr.u32PicWidth  = stPicSize.u32Width;
            stJpegAttr.u32PicHeight = stPicSize.u32Height;
			stJpegAttr.u32MaxPicWidth  = stPicSize.u32Width;
            stJpegAttr.u32MaxPicHeight = stPicSize.u32Height;
            stJpegAttr.u32BufSize   = (((stPicSize.u32Width+15)>>4)<<4) * (((stPicSize.u32Height+15)>>4)<<4);
            stJpegAttr.bByFrame     = HI_TRUE;/*get stream mode is field mode  or frame mode*/
            stJpegAttr.bSupportDCF  = HI_FALSE;
            memcpy(&(pstVencChnAttr->stVeAttr.stAttrJpeg), &stJpegAttr, sizeof(VENC_ATTR_JPEG_S));
            break;

        case PT_H265:
        {
            stH265Attr.u32MaxPicWidth = stPicSize.u32Width;
            stH265Attr.u32MaxPicHeight = stPicSize.u32Height;
            stH265Attr.u32PicWidth = stPicSize.u32Width;/*the picture width*/
            stH265Attr.u32PicHeight = stPicSize.u32Height;/*the picture height*/
            stH265Attr.u32BufSize  = stPicSize.u32Width * stPicSize.u32Height * 2;/*stream buffer size*/
			stH265Attr.u32Profile  = 0;/*0:MP*/
            stH265Attr.bByFrame = HI_TRUE;/*get stream mode is slice mode or frame mode?*/
			stH265Attr.u32BFrameNum = 0;/* 0: not support B frame; >=1: number of B frames */
			stH265Attr.u32RefNum = 1;/* 0: default; number of refrence frame*/
            memcpy(&(pstVencChnAttr->stVeAttr.stAttrH265e), &stH265Attr, sizeof(VENC_ATTR_H265_S));

            if(RC_MODE_CBR == enRcMode)
            {
                pstVencChnAttr->stRcAttr.enRcMode = VENC_RC_MODE_H265CBR;
                stH265Cbr.u32Gop            = 30;
                stH265Cbr.u32StatTime       = 1; /* stream rate statics time(s) */
                stH265Cbr.u32SrcFrmRate      = u32FrameRate;/* input (vi) frame rate */
                stH265Cbr.fr32DstFrmRate = u32FrameRate;/* target frame rate */
                switch (enSize)
                {
                  case PIC_QCIF:
                       stH265Cbr.u32BitRate = 256; /* average bit rate */
                       break;
                  case PIC_QVGA:    /* 320 * 240 */
                  case PIC_CIF: 

                	   stH265Cbr.u32BitRate = 512;
                       break;

                  case PIC_D1:
                  case PIC_VGA:	   /* 640 * 480 */
                	   stH265Cbr.u32BitRate = 1024*2;
                       break;
                  case PIC_HD720:   /* 1280 * 720 */
                	   stH265Cbr.u32BitRate = 1024*3;
                	   break;
                  case PIC_HD1080:  /* 1920 * 1080 */
                  	   stH265Cbr.u32BitRate = 1024*4;
                	   break;
                  case PIC_5M:  /* 2592 * 1944 */
                  	   stH265Cbr.u32BitRate = 1024*8;
                	   break;                        
                  default :
                       stH265Cbr.u32BitRate = 1024*4;
                       break;
                }
                
                stH265Cbr.u32FluctuateLevel = 0; /* average bit rate */
                memcpy(&(pstVencChnAttr->stRcAttr.stAttrH265Cbr), &stH265Cbr, sizeof(VENC_ATTR_H265_CBR_S));
            }
            else if (RC_MODE_FIXQP == enRcMode) 
            {
                pstVencChnAttr->stRcAttr.enRcMode = VENC_RC_MODE_H265FIXQP;
                stH265FixQp.u32Gop = 30;
                stH265FixQp.u32SrcFrmRate = u32FrameRate;
                stH265FixQp.fr32DstFrmRate = u32FrameRate;
                stH265FixQp.u32IQp = 20;
                stH265FixQp.u32PQp = 23;
                memcpy(&(pstVencChnAttr->stRcAttr.stAttrH265FixQp), &stH265FixQp,sizeof(VENC_ATTR_H265_FIXQP_S));
            }
            else if (RC_MODE_VBR == enRcMode) 
            {
                pstVencChnAttr->stRcAttr.enRcMode = VENC_RC_MODE_H265VBR;
                stH265Vbr.u32Gop = 30;
                stH265Vbr.u32StatTime = 1;
                stH265Vbr.u32SrcFrmRate = u32FrameRate;
                stH265Vbr.fr32DstFrmRate = u32FrameRate;
                stH265Vbr.u32MinQp = 10;
                stH265Vbr.u32MaxQp = 40;
                switch (enSize)
                {
                  case PIC_QCIF:
                	   stH265Vbr.u32MaxBitRate= 256*3; /* average bit rate */
                	   break;
                  case PIC_QVGA:    /* 320 * 240 */
                  case PIC_CIF:
                	   stH265Vbr.u32MaxBitRate = 512*3;
                       break;
                  case PIC_D1:
                  case PIC_VGA:	   /* 640 * 480 */
                	   stH265Vbr.u32MaxBitRate = 1024*2;
                       break;
                  case PIC_HD720:   /* 1280 * 720 */
                	   stH265Vbr.u32MaxBitRate = 1024*3;
                	   break;
                  case PIC_HD1080:  /* 1920 * 1080 */
                  	   stH265Vbr.u32MaxBitRate = 1024*6;
                	   break;
                  case PIC_5M:  /* 2592 * 1944 */
                  	   stH265Vbr.u32MaxBitRate = 1024*8;
                	   break;                        
                  default :
                       stH265Vbr.u32MaxBitRate = 1024*4;
                       break;
                }
                memcpy(&(pstVencChnAttr->stRcAttr.stAttrH265Vbr), &stH265Vbr, sizeof(VENC_ATTR_H265_VBR_S));
            }
            else
            {
                return HI_FAILURE;
            }
        }
        break;
        default:
            return HI_ERR_VENC_NOT_SUPPORT;
    }
	return HI_SUCCESS;
}

static HI_S32 s32VencStartChn(VENC_CHN VencChn,VENC_CHN_ATTR_S * pstVencChnAttr)
{
	HI_S32 s32Ret;
	
    s32Ret = HI_MPI_VENC_CreateChn(VencChn, pstVencChnAttr);
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_VENC_CreateChn [%d] faild with %#x!\n",\
                VencChn, s32Ret);
        return s32Ret;
    }
	
    s32Ret = HI_MPI_VENC_StartRecvPic(VencChn);
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_VENC_StartRecvPic faild with%#x!\n", s32Ret);
        return HI_FAILURE;
    }

    return HI_SUCCESS;

}

/******************************************************************************
* funciton : get file postfix according palyload_type.
******************************************************************************/
static HI_S32 s32GetFilePostfix(PAYLOAD_TYPE_E enPayload, char *szFilePostfix)
{
    if (PT_H264 == enPayload)
    {
        strcpy(szFilePostfix, ".h264");
    }
    else if (PT_H265 == enPayload)
    {
        strcpy(szFilePostfix, ".h265");
    }
    else if (PT_JPEG == enPayload)
    {
        strcpy(szFilePostfix, ".jpg");
    }
    else if (PT_MJPEG == enPayload)
    {
        strcpy(szFilePostfix, ".mjp");
    }
    else if (PT_MP4VIDEO == enPayload)
    {
        strcpy(szFilePostfix, ".mp4");
    }
    else
    {
        printf("payload type err!\n");
        return HI_FAILURE;
    }
    return HI_SUCCESS;
}

/******************************************************************************
* funciton : save mjpeg stream. 
******************************************************************************/
static HI_S32 s32SaveMJpeg(FILE* fpMJpegFile, VENC_STREAM_S *pstStream)
{
    VENC_PACK_S*  pstData;
    HI_U32 i;

    //fwrite(g_SOI, 1, sizeof(g_SOI), fpJpegFile); //in Hi3531, user needn't write SOI!

    for (i = 0; i < pstStream->u32PackCount; i++)
    {
        pstData = &pstStream->pstPack[i];
        fwrite(pstData->pu8Addr+pstData->u32Offset, pstData->u32Len-pstData->u32Offset, 1, fpMJpegFile);
        fflush(fpMJpegFile);
    }

    return HI_SUCCESS;
}

/******************************************************************************
* funciton : save jpeg stream. 
******************************************************************************/
static HI_S32 s32SaveJpeg(FILE* fpJpegFile, VENC_STREAM_S *pstStream)
{
    VENC_PACK_S*  pstData;
    HI_U32 i;

    for (i = 0; i < pstStream->u32PackCount; i++)
    {
        pstData = &pstStream->pstPack[i];
        fwrite(pstData->pu8Addr+pstData->u32Offset, pstData->u32Len-pstData->u32Offset, 1, fpJpegFile);
        fflush(fpJpegFile);
    }

    return HI_SUCCESS;
}

/******************************************************************************
* funciton : save H264 stream
******************************************************************************/
static HI_S32 s32SaveH264(FILE* fpH264File, VENC_STREAM_S *pstStream)
{
    HI_S32 i;

    
    for (i = 0; i < pstStream->u32PackCount; i++)
    {
        fwrite(pstStream->pstPack[i].pu8Addr+pstStream->pstPack[i].u32Offset,
               pstStream->pstPack[i].u32Len-pstStream->pstPack[i].u32Offset, 1, fpH264File);

        fflush(fpH264File);
    }
    

    return HI_SUCCESS;
}

/******************************************************************************
* funciton : save H265 stream
******************************************************************************/
static HI_S32 s32SaveH265(FILE* fpH265File, VENC_STREAM_S *pstStream)
{
    HI_S32 i;
    
    for (i = 0; i < pstStream->u32PackCount; i++)
    {
        fwrite(pstStream->pstPack[i].pu8Addr+pstStream->pstPack[i].u32Offset,
               pstStream->pstPack[i].u32Len-pstStream->pstPack[i].u32Offset, 1, fpH265File);

        fflush(fpH265File);
    }

    return HI_SUCCESS;
}

/******************************************************************************
* funciton : save jpeg stream
******************************************************************************/
static HI_S32 s32SaveJPEG(FILE *fpJpegFile, VENC_STREAM_S *pstStream)
{
    VENC_PACK_S*  pstData;
    HI_U32 i;

    for (i = 0; i < pstStream->u32PackCount; i++)
    {
        pstData = &pstStream->pstPack[i];
        fwrite(pstData->pu8Addr+pstData->u32Offset, pstData->u32Len-pstData->u32Offset, 1, fpJpegFile);
        fflush(fpJpegFile);
    }

    return HI_SUCCESS;
}


/******************************************************************************
* funciton : save stream
******************************************************************************/
static HI_S32 s32SaveStream(PAYLOAD_TYPE_E enType,FILE *pFd, VENC_STREAM_S *pstStream)
{
    HI_S32 s32Ret;

    if (PT_H264 == enType)
    {
        s32Ret = s32SaveH264(pFd, pstStream);
    }
    else if (PT_MJPEG == enType)
    {
        s32Ret = s32SaveMJpeg(pFd, pstStream);
    }
    else if (PT_H265 == enType)
    {
        s32Ret = s32SaveH265(pFd, pstStream);        
    }
    else
    {
        return HI_FAILURE;
    }
    return s32Ret;
}


/******************************************************************************
* funciton : get stream from each channels and save them
******************************************************************************/
HI_VOID* vdGetVencStreamProc(HI_VOID *p)
{
    HI_S32 i;
    HI_S32 s32ChnTotal;
    VENC_CHN_ATTR_S stVencChnAttr;
    HI_S32 maxfd = 0;
    struct timeval TimeoutVal;
    fd_set read_fds;
    HI_S32 VencFd[VENC_MAX_CHN_NUM];
    HI_CHAR aszFileName[VENC_MAX_CHN_NUM][64];
    FILE *pFile[VENC_MAX_CHN_NUM];
    char szFilePostfix[10];
    VENC_CHN_STAT_S stStat;
    VENC_STREAM_S stStream;
    HI_S32 s32Ret;
    VENC_CHN VencChn;
    PAYLOAD_TYPE_E enPayLoadType[VENC_MAX_CHN_NUM];
	ST_VENC_ATTR * pstVencAttr;
	pstVencAttr = (ST_VENC_ATTR *)p;
    s32ChnTotal = ((ST_VENC_ATTR *)p)->vencChnNum;

    /******************************************
     step 1:  check & prepare save-file & venc-fd
    ******************************************/
    if (s32ChnTotal >= VENC_MAX_CHN_NUM)
    {
        printf("input count invaild\n");
        return NULL;
    }
    for (i = 0; i < s32ChnTotal; i++)
    {
        /* decide the stream file name, and open file to save stream */
        VencChn = i;
        s32Ret = HI_MPI_VENC_GetChnAttr(VencChn, &stVencChnAttr);
        if(s32Ret != HI_SUCCESS)
        {
            printf("HI_MPI_VENC_GetChnAttr chn[%d] failed with %#x!\n", \
                   VencChn, s32Ret);
            return NULL;
        }
        enPayLoadType[i] = stVencChnAttr.stVeAttr.enType;

        s32Ret = s32GetFilePostfix(enPayLoadType[i], szFilePostfix);
        if(s32Ret != HI_SUCCESS)
        {
            printf("s32GetFilePostfix [%d] failed with %#x!\n", \
                   stVencChnAttr.stVeAttr.enType, s32Ret);
            return NULL;
        }
        sprintf(aszFileName[i], "stream_chn%d%s", i, szFilePostfix);
        pFile[i] = fopen(aszFileName[i], "wb");
        if (!pFile[i])
        {
            printf("open file[%s] failed!\n", 
                   aszFileName[i]);
            return NULL;
        }

        /* Set Venc Fd. */
        VencFd[i] = HI_MPI_VENC_GetFd(i);
        if (VencFd[i] < 0)
        {
            printf("HI_MPI_VENC_GetFd failed with %#x!\n", 
                   VencFd[i]);
            return NULL;
        }
        if (maxfd <= VencFd[i])
        {
            maxfd = VencFd[i];
        }
    }

    /******************************************
     step 2:  Start to get streams of each channel.
    ******************************************/
    while (HI_TRUE == pstVencAttr->bGetStreamProcThreadStart)
    {
        FD_ZERO(&read_fds);
        for (i = 0; i < s32ChnTotal; i++)
        {
            FD_SET(VencFd[i], &read_fds);
        }

        TimeoutVal.tv_sec  = 2;
        TimeoutVal.tv_usec = 0;
        s32Ret = select(maxfd + 1, &read_fds, NULL, NULL, &TimeoutVal);
        if (s32Ret < 0)
        {
            printf("select failed!\n");
            break;
        }
        else if (s32Ret == 0)
        {
            printf("get venc stream time out, exit thread\n");
            continue;
        }
        else
        {
            for (i = 0; i < s32ChnTotal; i++)
            {
                if (FD_ISSET(VencFd[i], &read_fds))
                {
                    /*******************************************************
                     step 2.1 : query how many packs in one-frame stream.
                    *******************************************************/
                    memset(&stStream, 0, sizeof(stStream));
                    s32Ret = HI_MPI_VENC_Query(i, &stStat);
                    if (HI_SUCCESS != s32Ret)
                    {
                        printf("HI_MPI_VENC_Query chn[%d] failed with %#x!\n", i, s32Ret);
                        break;
                    }
					
					/*******************************************************
					step 2.2 :suggest to check both u32CurPacks and u32LeftStreamFrames at the same time,for example:
					 if(0 == stStat.u32CurPacks || 0 == stStat.u32LeftStreamFrames)
					 {
						SAMPLE_PRT("NOTE: Current  frame is NULL!\n");
						continue;
					 }
					*******************************************************/
					if(0 == stStat.u32CurPacks)
					{
						  printf("NOTE: Current  frame is NULL!\n");
						  continue;
					}
                    /*******************************************************
                     step 2.3 : malloc corresponding number of pack nodes.
                    *******************************************************/
                    stStream.pstPack = (VENC_PACK_S*)malloc(sizeof(VENC_PACK_S) * stStat.u32CurPacks);
                    if (NULL == stStream.pstPack)
                    {
                        printf("malloc stream pack failed!\n");
                        break;
                    }
                    
                    /*******************************************************
                     step 2.4 : call mpi to get one-frame stream
                    *******************************************************/
                    stStream.u32PackCount = stStat.u32CurPacks;
                    s32Ret = HI_MPI_VENC_GetStream(i, &stStream, HI_TRUE);
                    if (HI_SUCCESS != s32Ret)
                    {
                        free(stStream.pstPack);
                        stStream.pstPack = NULL;
                        printf("HI_MPI_VENC_GetStream failed with %#x!\n", \
                               s32Ret);
                        break;
                    }

                    /*******************************************************
                     step 2.5 : save frame to file
                    *******************************************************/
                    s32Ret = s32SaveStream(enPayLoadType[i], pFile[i], &stStream);
					//SAMPLE_COMM_VENC_Sentjin(&stStream);
					saveStream(&stStream);
                    if (HI_SUCCESS != s32Ret)
                    {
                        free(stStream.pstPack);
                        stStream.pstPack = NULL;
                        printf("save stream failed!\n");
                        break;
                    }
                    /*******************************************************
                     step 2.6 : release stream
                    *******************************************************/
                    s32Ret = HI_MPI_VENC_ReleaseStream(i, &stStream);
                    if (HI_SUCCESS != s32Ret)
                    {
                        free(stStream.pstPack);
                        stStream.pstPack = NULL;
                        break;
                    }
                    /*******************************************************
                     step 2.7 : free pack nodes
                    *******************************************************/
                    free(stStream.pstPack);
                    stStream.pstPack = NULL;
                }
            }
        }
    }

    /*******************************************************
    * step 3 : close save-file
    *******************************************************/
    for (i = 0; i < s32ChnTotal; i++)
    {
        fclose(pFile[i]);
    }

    return NULL;
}

/******************************************************************************
* funciton : start get venc stream process thread
******************************************************************************/
static HI_S32 s32StartGetStream(void)
{   
	g_stVencAttr.bGetStreamProcThreadStart = HI_TRUE;
    return pthread_create(&gs_VencPid, 0, vdGetVencStreamProc, (HI_VOID*)&g_stVencAttr);
}


static HI_S32 s32StartVENC(const IPC_VEDIO_S* pstIpcVedio)
{
	VENC_CHN_ATTR_S stVencChnAttr;
	VENC_CHN VencChn;
	HI_S32 s32Ret;
	g_stVencAttr.vencChnNum = pstIpcVedio->u32StreamNum;
	for(VencChn = 0; VencChn < g_stVencAttr.vencChnNum; VencChn++)
	{
		s32GetVencChnAttr(VencChn, pstIpcVedio, &stVencChnAttr);
		s32Ret = s32VencStartChn(VencChn, &stVencChnAttr);
		if (HI_SUCCESS != s32Ret)
		{
			printf("Start Venc failed!\n");
			return HI_FAILURE;
		}
	}
	s32StartGetStream();
	return HI_SUCCESS;
}
static HI_S32 s32VencStopChn(VENC_CHN VencChn)
{
    HI_S32 s32Ret;

    /******************************************
     step 1:  Stop Recv Pictures
    ******************************************/
    s32Ret = HI_MPI_VENC_StopRecvPic(VencChn);
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_VENC_StopRecvPic vechn[%d] failed with %#x!\n",\
               VencChn, s32Ret);
        return HI_FAILURE;
    }

    /******************************************
     step 2:  Distroy Venc Channel
    ******************************************/
    s32Ret = HI_MPI_VENC_DestroyChn(VencChn);
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_VENC_DestroyChn vechn[%d] failed with %#x!\n",\
               VencChn, s32Ret);
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

/******************************************************************************
* funciton : stop get venc stream process.
******************************************************************************/
static HI_S32 s32StopGetStream(void)
{
    if(HI_TRUE == g_stVencAttr.bGetStreamProcThreadStart)
    {
		g_stVencAttr.bGetStreamProcThreadStart = HI_FALSE;
        pthread_join(gs_VencPid, 0);
		gs_VencPid = 0;
    }
    return HI_SUCCESS;
}

static HI_S32 s32StopVENC(void)
{
	HI_S32 s32Ret = HI_SUCCESS;
	VENC_CHN VencChn;
	s32StopGetStream();
	
	for(VencChn = 0; VencChn < g_stVencAttr.vencChnNum; VencChn++)
	{
		s32Ret = s32VencStopChn(VencChn);
		if (HI_SUCCESS != s32Ret)
		{
			printf("Stop Venc failed!\n");
			return HI_FAILURE;
		}
	}	
	return HI_SUCCESS;
}

HI_S32 VENC_init(const IPC_VEDIO_S* pstIpcVedio)
{
	HI_S32 s32Ret = HI_SUCCESS;
	if(!pstIpcVedio)
    {
        printf("%s: null ptr\n", __FUNCTION__);
        return HI_FAILURE;
    }
	s32Ret = s32StartVENC(pstIpcVedio);
	return s32Ret;
}

HI_S32 VENC_exit(void)
{
	HI_S32 s32Ret = HI_SUCCESS;
	s32Ret = s32StopVENC();
	return s32Ret;
}

