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
#include "hp_fpn.h"

VB_POOL g_Pool;
char *FpnFileName= "PS3210_1920x1080_10bit.raw";
HI_S32 FPN_init(const IPC_VEDIO_S* pstIpcVedio)
{
    VI_CHN_ATTR_S stTempChnAttr;
    VI_CHN ViChn = 0;
	FPN_ATTR_S stFPNattr = {0};
	HI_S32 s32Ret = HI_SUCCESS;
	HI_U32 u32BlkSize;
	
	s32Ret =  HI_MPI_VI_GetChnAttr(ViChn, &stTempChnAttr);
	if(HI_SUCCESS != s32Ret)
	{
		printf("get vi chn attr failed!");
		return HI_FAILURE;
	}		  
	memcpy(&stFPNattr.stPicSize,&stTempChnAttr.stDestSize,sizeof(SIZE_S));
	stFPNattr.enFpnType = ISP_FPN_TYPE_FRAME;
	stFPNattr.stVideoFrame.stVFrame.enPixelFormat = PIXEL_FORMAT_RGB_BAYER_10BPP;
    stFPNattr.stVideoFrame.stVFrame.enCompressMode = COMPRESS_MODE_NONE;
	stFPNattr.stVideoFrame.stVFrame.u32Field = VIDEO_FIELD_FRAME;

	 /* FPN need one private 422 VB */
    u32BlkSize = GET_VB_PIC_BLK_SIZE(stFPNattr.stPicSize.u32Width, stFPNattr.stPicSize.u32Height, PIXEL_FORMAT_YUV_SEMIPLANAR_422);
    g_Pool = HI_MPI_VB_CreatePool(u32BlkSize, 1, NULL);
    if ( VB_INVALID_POOLID == g_Pool )
    {
        printf("create vb err\n");
        return HI_FAILURE;
    }
	
    /* fpn calibrate, save dark frame file */
    s32Ret = s32FPN_Calibrate(&stFPNattr);
    if (HI_SUCCESS != s32Ret)
    {
        printf("FPN calibrate failed %#x!\n", s32Ret);
        return HI_FAILURE;
    }

	/* fpn correction, read dark frame file */
    s32Ret = s32FPN_Correction(&stFPNattr);
    if (HI_SUCCESS != s32Ret)
    {
        printf("FPN correction failed %#x!\n", s32Ret);
        return HI_FAILURE;
    }
	return HI_SUCCESS;
}
HI_S32 s32FPN_Calibrate(FPN_ATTR_S *pstFPNattr)
{
    VI_CHN ViChn = 0;
	ISP_DEV IspDev = 0;
    ISP_FPN_CALIBRATE_ATTR_S stFpnCalAttr;   
    
    HI_S32 s32Ret = HI_SUCCESS;
     
    
    s32Ret = s32GetVBFromPool(pstFPNattr);
    if(HI_SUCCESS != s32Ret)
    {
        printf("alloc mem failed!");
        return HI_FAILURE;
    }
    printf("\n please turn off camera aperture!\n");
    printf("\nhit the Enter key ,start calibrate!\n");
    getchar();  
    
    s32Ret =  HI_MPI_VI_DisableChn(ViChn);
    if(HI_SUCCESS != s32Ret)
    {
        printf("disable vi chn failed!");
        return HI_FAILURE;
    }
    stFpnCalAttr.u32FrameNum  = 16;    
    stFpnCalAttr.u32Threshold = 512;
    stFpnCalAttr.enFpnType    = pstFPNattr->enFpnType;
    memcpy(&stFpnCalAttr.stFpnCaliFrame.stFpnFrame, &pstFPNattr->stVideoFrame, sizeof(VIDEO_FRAME_INFO_S)); 
    stFpnCalAttr.stFpnCaliFrame.u32Offset = 0;
    
    s32Ret = HI_MPI_ISP_FPNCalibrate(IspDev, &stFpnCalAttr);
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_ISP_FPNCalibrate err: 0x%x\n", s32Ret);
        return s32Ret;
    }  
    /* save RAW data */ 
    printf("\nafter calibrate, offset =0x%x,ISO = %d, frmsize = %d\n",\
    		stFpnCalAttr.stFpnCaliFrame.u32Offset,stFpnCalAttr.stFpnCaliFrame.u32Iso,stFpnCalAttr.stFpnCaliFrame.u32FrmSize);  

    s32Ret =  HI_MPI_VI_EnableChn(ViChn);
    if(HI_SUCCESS != s32Ret)
    {
        printf("enable vi chn failed!");
        return HI_FAILURE;
    }    
        
    s32Ret = s32SaveFpnData(&stFpnCalAttr.stFpnCaliFrame, pstFPNattr);
    s32Ret = s32ReleaseVBToPool(pstFPNattr);
    
    return HI_SUCCESS;
}

HI_S32 s32FPN_Correction(FPN_ATTR_S *pstFPNattr)
{
    VI_CHN ViChn = 0;     
    ISP_FPN_ATTR_S stFPNCorAttr;
    ISP_DEV IspDev = 0;
    HI_S32 s32Ret = HI_SUCCESS;

    s32Ret = s32ReadFpnData(&stFPNCorAttr.stFpnFrmInfo, pstFPNattr);
    if(HI_SUCCESS != s32Ret)
    {
        printf("read Fpn file failed!");
        return HI_FAILURE;
    }

    memcpy(&stFPNCorAttr.stFpnFrmInfo.stFpnFrame, &pstFPNattr->stVideoFrame, sizeof(VIDEO_FRAME_INFO_S));    
    
    stFPNCorAttr.bEnable  = HI_TRUE;
    stFPNCorAttr.enOpType = OP_TYPE_MANUAL;
	stFPNCorAttr.enFpnType   = pstFPNattr->enFpnType;
	stFPNCorAttr.stManual.u32Strength = 0x100;

    s32Ret =  HI_MPI_ISP_SetFPNAttr(IspDev, &stFPNCorAttr);
    if(HI_SUCCESS != s32Ret)
    {
        printf("fpn correction fail 0x%x\n", s32Ret);
        return HI_FAILURE;
    }
    s32Ret = s32ReleaseVBToPool(pstFPNattr);
    return HI_SUCCESS;
}

HI_S32 s32GetVBFromPool(FPN_ATTR_S *pstFPNattr)
{
	HI_U32 u32Width;
    HI_U32 u32Height;
	 PIXEL_FORMAT_E enPixelFormat;
	 
	HI_U32 u32Stride;
	HI_U32 u32CStride;
    HI_U32 u32LumaSize;
    HI_U32 u32ChrmSize;
    HI_U32 u32Size;    
    VB_BLK VbBlk;
    HI_U32 u32PhyAddr;
    HI_VOID *pVirAddr = NULL;
    VIDEO_FRAME_INFO_S *pstVFrameInfo = &pstFPNattr->stVideoFrame;
	u32Width = pstFPNattr->stPicSize.u32Width;
	u32Height = pstFPNattr->stPicSize.u32Height;
	enPixelFormat = pstFPNattr->stVideoFrame.stVFrame.enPixelFormat;

	/* alloc 16bit/pixel memory */
	u32Stride = ALIGN_BACK(u32Width * 2, 16);	
	printf("u32Stride: %d\n", u32Stride);
    u32CStride  = u32Width;  
    u32LumaSize = u32Stride * u32Height;
    if (PIXEL_FORMAT_YUV_SEMIPLANAR_422 == enPixelFormat)
    {
        u32ChrmSize = (u32CStride * u32Height) >> 1;/* YUV 422 */
    }
    else if ((PIXEL_FORMAT_RGB_BAYER == enPixelFormat)
        ||(PIXEL_FORMAT_RGB_BAYER_12BPP == enPixelFormat)
        ||(PIXEL_FORMAT_RGB_BAYER_10BPP == enPixelFormat)
        ||(PIXEL_FORMAT_RGB_BAYER_8BPP == enPixelFormat))
    {
        u32ChrmSize = 0;
    }
    else
    {
        u32ChrmSize = (u32CStride * u32Height) >> 2;/* YUV 420 */
    }
    u32Size = u32LumaSize + (u32ChrmSize << 1); 
    printf("===============u32Size: %d\n", u32Size);
	pstFPNattr->u32FrmSize = u32Size;
	
    /* alloc video buffer block ---------------------------------------------------------- */
    VbBlk = HI_MPI_VB_GetBlock(g_Pool, pstFPNattr->u32FrmSize, HI_NULL);
    if (VB_INVALID_HANDLE == VbBlk)        
    {
        printf("HI_MPI_VB_GetBlock err! size:%d\n",u32Size);
        return HI_FAILURE;
    }
    pstFPNattr->VbBlk = VbBlk;

    printf("VbBlk: 0x%x\n", VbBlk);
    
    u32PhyAddr = HI_MPI_VB_Handle2PhysAddr(VbBlk);
    if (0 == u32PhyAddr)
    {
        printf("HI_MPI_VB_Handle2PhysAddr err!\n");
        return HI_FAILURE;
    }

    pstVFrameInfo->u32PoolId = HI_MPI_VB_Handle2PoolId(VbBlk);
    if (VB_INVALID_POOLID == pstVFrameInfo->u32PoolId)
    {
		printf("HI_MPI_VB_Handle2PoolId err!\n");
        HI_MPI_SYS_Exit();
    	HI_MPI_VB_Exit();
        return HI_FAILURE;
    }

    pstVFrameInfo->stVFrame.u32PhyAddr[0] = u32PhyAddr;
    pstVFrameInfo->stVFrame.u32PhyAddr[1] = pstVFrameInfo->stVFrame.u32PhyAddr[0] + u32LumaSize;
    pstVFrameInfo->stVFrame.u32PhyAddr[2] = pstVFrameInfo->stVFrame.u32PhyAddr[1] + u32ChrmSize;

    pstVFrameInfo->stVFrame.pVirAddr[0] = pVirAddr;
    pstVFrameInfo->stVFrame.pVirAddr[1] = pstVFrameInfo->stVFrame.pVirAddr[0] + u32LumaSize;
    pstVFrameInfo->stVFrame.pVirAddr[2] = pstVFrameInfo->stVFrame.pVirAddr[1] + u32ChrmSize;

    pstVFrameInfo->stVFrame.u32Width  = u32Width;
    pstVFrameInfo->stVFrame.u32Height = u32Height;
    pstVFrameInfo->stVFrame.u32Stride[0] = u32Stride;
    pstVFrameInfo->stVFrame.u32Stride[1] = u32Stride;
    pstVFrameInfo->stVFrame.u32Stride[2] = u32Stride;
    
    return HI_SUCCESS;
}


 HI_S32 s32SaveFpnData(ISP_FPN_FRAME_INFO_S *pVBuf, FPN_ATTR_S *pstFPNattr)
{
    HI_U8* pU8VBufVirt_Y;
    HI_U32 phy_addr,size;
    HI_U8* pUserPageAddr[2];
	FILE * pfd;
	
	pfd = fopen(FpnFileName, "wb");
    if (NULL == pfd)
    {
        printf("open file %s err!\n",FpnFileName);
        return HI_FAILURE;
    }
	
    size = (pVBuf->stFpnFrame.stVFrame.u32Stride[0]) * (pVBuf->stFpnFrame.stVFrame.u32Height);
    printf("pVBuf->u32Stride[0]: %d, size: %d\n", pVBuf->stFpnFrame.stVFrame.u32Stride[0], size);
    phy_addr =  pVBuf->stFpnFrame.stVFrame.u32PhyAddr[0];

    pUserPageAddr[0] = (HI_U8 *) HI_MPI_SYS_Mmap(phy_addr, size);
    if (NULL == pUserPageAddr[0])
    {
        printf("HI_MPI_SYS_Mmap null\n");
        return HI_FAILURE;
    }

    pU8VBufVirt_Y = (HI_U8*)pUserPageAddr[0];

    /* save Y ----------------------------------------------------------------*/
    fprintf(stderr, "saving......Raw data......u32Stride[0]: %d, width: %d, height: %d\n"
                                , pVBuf->stFpnFrame.stVFrame.u32Stride[0]
                                , pVBuf->stFpnFrame.stVFrame.u32Width
                                , pVBuf->stFpnFrame.stVFrame.u32Height);
    fflush(stderr);

    fwrite(pU8VBufVirt_Y, pVBuf->u32FrmSize, 1, pfd);
    
    /* save offset */
    fwrite(&pVBuf->u32Offset, 4, 1, pfd);

    /* save fpn frame size */
    fwrite(&pVBuf->u32FrmSize, 4, 1, pfd);

    /* save ISO */
    fwrite(&pVBuf->u32Iso, 4, 1, pfd);
    fflush(pfd);        
    
    fflush(stderr);

    HI_MPI_SYS_Munmap(pUserPageAddr[0], size);
	fclose(pfd);
	return HI_FAILURE;
}

HI_S32 s32ReadFpnData(ISP_FPN_FRAME_INFO_S *pVBuf, FPN_ATTR_S *pstFPNattr)
{
	FILE *pfd;
    HI_S32 s32Ret = HI_SUCCESS;
    HI_U32 u32LStride;
	HI_U32 u32CStride;
	HI_U32 u32Width;
	HI_U32 u32Height;
	HI_U32 u32LumaSize = 0;
	HI_U32 u32ChrmSize = 0;
	HI_U32 u32Size;
	VB_BLK VbBlk;
	HI_U32 u32PhyAddr;
	HI_U8 *pVirAddr;
	HI_U32 u32Stride;
	VIDEO_FRAME_INFO_S *pstVFrameInfo = &pstFPNattr->stVideoFrame;
	
    /* open YUV file */
    printf("open dark frame file: %s. \n", FpnFileName);
    pfd = fopen(FpnFileName, "rb");
    if (!pfd)
    {
        printf("open file -> %s fail \n", FpnFileName);
        return HI_FAILURE;
    }

	u32Width = pstFPNattr->stPicSize.u32Width;
	u32Height = pstFPNattr->stPicSize.u32Height;
	
    /* read frame information from YUV file */
	if (PIXEL_FORMAT_RGB_BAYER == pstVFrameInfo->stVFrame.enPixelFormat)
	{
	   u32Stride = ALIGN_BACK(u32Width*2, 16);
	}
	else if (PIXEL_FORMAT_RGB_BAYER_10BPP == pstVFrameInfo->stVFrame.enPixelFormat)
	{
		u32Stride = (u32Width * 10 + 127) / 128 * 128 / 8;
	}
	else if (PIXEL_FORMAT_RGB_BAYER_8BPP == pstVFrameInfo->stVFrame.enPixelFormat)
	{
		u32Stride = (u32Width * 8 + 127) / 128 * 128 / 8;
	}
	else
	{
		printf("Func: %s() Line[%d], unsupported pixel format: %d\n", 
				__FUNCTION__, __LINE__, pstVFrameInfo->stVFrame.enPixelFormat);
		return HI_FAILURE;
	}

	u32LStride	= u32Stride;
	u32CStride	= u32Stride;

	////////////////////////////////////////////////////////
	/* seek end of file */
	s32Ret = fseek(pfd, -8, SEEK_END);
	if (0 != s32Ret)
	{
		printf("Func: %s(), line: [%d], get frame size failed:	%s\n", __FUNCTION__, __LINE__, strerror(errno));
		return HI_FAILURE;
	}
	/* get fpn frame size */
	fread(&u32Size, 1, 4, pfd);
	/* back to begin of file */
	fseek(pfd, 0L, SEEK_SET);
	////////////////////////////////////////////////////////

	/* alloc video buffer block ---------------------------------------------------------- */
    VbBlk = HI_MPI_VB_GetBlock(g_Pool, u32Size, HI_NULL);
	if (VB_INVALID_HANDLE == VbBlk)	
	{
		printf("HI_MPI_VB_GetBlock err! size:%d\n",u32Size);	   
		HI_MPI_SYS_Exit();
    	HI_MPI_VB_Exit();		 
		return HI_FAILURE;
	}
	pstFPNattr->VbBlk = VbBlk;
	
	u32PhyAddr = HI_MPI_VB_Handle2PhysAddr(VbBlk);
	if (0 == u32PhyAddr)
	{
		printf("HI_MPI_VB_Handle2PhysAddr err!\n");
		HI_MPI_SYS_Exit();
    	HI_MPI_VB_Exit(); 	 
		return HI_FAILURE;
	}
	pVirAddr = (HI_U8 *) HI_MPI_SYS_Mmap(u32PhyAddr, u32Size);
	if (NULL == pVirAddr)
	{
		printf("HI_MPI_SYS_Mmap err!\n");
		HI_MPI_SYS_Exit();
    	HI_MPI_VB_Exit();	 
		return HI_FAILURE;
	}

	pstVFrameInfo->u32PoolId = HI_MPI_VB_Handle2PoolId(VbBlk);
	if (VB_INVALID_POOLID == pstVFrameInfo->u32PoolId)
	{
		HI_MPI_SYS_Exit();
    	HI_MPI_VB_Exit();
		return HI_FAILURE;
	}

	pstVFrameInfo->stVFrame.u32PhyAddr[0] = u32PhyAddr;
	pstVFrameInfo->stVFrame.u32PhyAddr[1] = pstVFrameInfo->stVFrame.u32PhyAddr[0] + u32LumaSize;
	pstVFrameInfo->stVFrame.u32PhyAddr[2] = pstVFrameInfo->stVFrame.u32PhyAddr[1] + u32ChrmSize;

	pstVFrameInfo->stVFrame.pVirAddr[0] = pVirAddr;
	pstVFrameInfo->stVFrame.pVirAddr[1] = pstVFrameInfo->stVFrame.pVirAddr[0] + u32LumaSize;
	pstVFrameInfo->stVFrame.pVirAddr[2] = pstVFrameInfo->stVFrame.pVirAddr[1] + u32ChrmSize;

	pstVFrameInfo->stVFrame.u32Width  = u32Width;
	pstVFrameInfo->stVFrame.u32Height = u32Height;
	pstVFrameInfo->stVFrame.u32Stride[0] = u32LStride;
	pstVFrameInfo->stVFrame.u32Stride[1] = u32CStride;
	pstVFrameInfo->stVFrame.u32Stride[2] = u32CStride;

	/*****************************************************************
    raw file storage layout:
    ///////////////////////////////////////////////////
    |-------------------------------------------------|
    |-------------------------------------------------|
    |-------------------------------------------------|
    |-------------------------------------------------|
    |-------------------------------------------------|
    |-------------------------------------------------|
    |-------------------|------|------|---------------|
                        		offset     size    u32iso
    ****************************************************************/

    /* seek end of file */
    s32Ret = fseek(pfd, -4, SEEK_END);
    if (0 != s32Ret)
    {
        printf("Func: %s(), line: [%d], get iso failed:  %s\n", __FUNCTION__, __LINE__, strerror(errno));
        return;
    }
    /* get calibrate ISO */
    fread(&pVBuf->u32Iso, 4, 1, pfd);

    /* get fpn frame size */
    pVBuf->u32FrmSize = u32Size;

    /* get fpn offset */
    s32Ret = fseek(pfd, -12, SEEK_END);
    if (0 != s32Ret)
    {
        printf("Func: %s(), line: [%d], get fpn offset failed:  %s\n", __FUNCTION__, __LINE__, strerror(errno));
        return;
    }
    fread(&pVBuf->u32Offset, 4, 1, pfd);

    /* back to begin of file */
    fseek(pfd, 0L, SEEK_SET);
    fread(pstVFrameInfo->stVFrame.pVirAddr[0], 1, u32Size, pfd);
	
    fclose(pfd);
	HI_MPI_SYS_Munmap(pVirAddr, u32Size);
    return s32Ret;
}

HI_S32 s32ReleaseVBToPool(FPN_ATTR_S *pstFPNattr)
{
    HI_S32 s32Ret = HI_SUCCESS;
    s32Ret = HI_MPI_VB_ReleaseBlock(pstFPNattr->VbBlk);
	if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_VB_ReleaseBlock err: 0x%x\n", s32Ret);        
    }
    return HI_FAILURE;
}

