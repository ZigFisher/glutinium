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


IPC_VEDIO_S g_stIpcVedio;

/******************************************************************************
* function : vb init & MPI system init
******************************************************************************/
HI_S32 mpi_sys_init(IPC_VEDIO_S *pstIpcVedio)
{
    MPP_SYS_CONF_S stSysConf = {0};
	VB_CONF_S stVbConf;
    HI_S32 i,s32Ret = HI_FAILURE;
	SIZE_S *stSize;
	PIXEL_FORMAT_E e_pixFmt;
	HI_U32 u32VbBlkCnt;
	
	if (NULL == pstIpcVedio)
    {
        printf("input parameter is null, it is invaild!\n");
        return HI_FAILURE;
    }
	HI_MPI_SYS_Exit();
    HI_MPI_VB_Exit();
	memset(&stVbConf,0,sizeof(VB_CONF_S));
	stVbConf.u32MaxPoolCnt = 128;
	u32VbBlkCnt = GET_VB_BLK_CNT(pstIpcVedio->e_chip);
	for(i = 0; i < pstIpcVedio->u32StreamNum; i++)
	{
		stSize = &(pstIpcVedio->astStreamFmt[i].stSize);
		e_pixFmt = pstIpcVedio->e_pixFmt;
		stVbConf.astCommPool[i].u32BlkSize = GET_VB_PIC_BLK_SIZE(stSize->u32Width, stSize->u32Height, e_pixFmt);
	    stVbConf.astCommPool[i].u32BlkCnt = u32VbBlkCnt;
	}
    
    s32Ret = HI_MPI_VB_SetConf(&stVbConf);
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_VB_SetConf failed!\n");
        return HI_FAILURE;
    }

    s32Ret = HI_MPI_VB_Init();
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_VB_Init failed!\n");
        return HI_FAILURE;
    }

    stSysConf.u32AlignWidth = SYS_ALIGN_WIDTH;
    s32Ret = HI_MPI_SYS_SetConf(&stSysConf);
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_SYS_SetConf failed\n");
        return HI_FAILURE;
    }

    s32Ret = HI_MPI_SYS_Init();
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_SYS_Init failed!\n");
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

/******************************************************************************
* function : vb exit & MPI system exit
******************************************************************************/
HI_VOID mpi_sys_exit(void)
{
    HI_MPI_SYS_Exit();
    HI_MPI_VB_Exit();
    return;
}


/******************************************************************************
* function : get picture size(w*h), according Norm and enPicSize
******************************************************************************/
HI_S32 s32GetPicSize(PIC_SIZE_E enPicSize, SIZE_S *pstSize)
{
	VIDEO_NORM_E enNorm = VIDEO_ENCODING_MODE_PAL;
    switch (enPicSize)
    {
        case PIC_QCIF:
            pstSize->u32Width  = 176;
            pstSize->u32Height = (VIDEO_ENCODING_MODE_PAL==enNorm)?144:120;
            break;
        case PIC_CIF:
            pstSize->u32Width  = 352;
            pstSize->u32Height = (VIDEO_ENCODING_MODE_PAL==enNorm)?288:240;
            break;
        case PIC_D1:
            pstSize->u32Width  = 720;
            pstSize->u32Height = (VIDEO_ENCODING_MODE_PAL==enNorm)?576:480;
            break;
        case PIC_960H:
            pstSize->u32Width  = 960;
            pstSize->u32Height = (VIDEO_ENCODING_MODE_PAL==enNorm)?576:480;
            break;			
        case PIC_2CIF:
            pstSize->u32Width  = 360;
            pstSize->u32Height = (VIDEO_ENCODING_MODE_PAL==enNorm)?576:480;
            break;
        case PIC_QVGA:    /* 320 * 240 */
            pstSize->u32Width  = 320;
            pstSize->u32Height = 240;
            break;
        case PIC_VGA:     /* 640 * 480 */
            pstSize->u32Width  = 640;
            pstSize->u32Height = 480;
            break;
        case PIC_XGA:     /* 1024 * 768 */
            pstSize->u32Width  = 1024;
            pstSize->u32Height = 768;
            break;
        case PIC_SXGA:    /* 1400 * 1050 */
            pstSize->u32Width  = 1400;
            pstSize->u32Height = 1050;
            break;
        case PIC_UXGA:    /* 1600 * 1200 */
            pstSize->u32Width  = 1600;
            pstSize->u32Height = 1200;
            break;
        case PIC_QXGA:    /* 2048 * 1536 */
            pstSize->u32Width  = 2048;
            pstSize->u32Height = 1536;
            break;
        case PIC_WVGA:    /* 854 * 480 */
            pstSize->u32Width  = 854;
            pstSize->u32Height = 480;
            break;
        case PIC_WSXGA:   /* 1680 * 1050 */
            pstSize->u32Width = 1680;
            pstSize->u32Height = 1050;
            break;
        case PIC_WUXGA:   /* 1920 * 1200 */
            pstSize->u32Width  = 1920;
            pstSize->u32Height = 1200;
            break;
        case PIC_WQXGA:   /* 2560 * 1600 */
            pstSize->u32Width  = 2560;
            pstSize->u32Height = 1600;
            break;
        case PIC_HD720:   /* 1280 * 720 */
            pstSize->u32Width  = 1280;
            pstSize->u32Height = 720;
            break;
        case PIC_HD1080:  /* 1920 * 1080 */
            pstSize->u32Width  = 1920;
            pstSize->u32Height = 1080;
            break;
        case PIC_5M:      /* 2592 * 1944 */
            pstSize->u32Width  = 2592;
            pstSize->u32Height = 1944;
            break;
            
        default:
            return HI_FAILURE;
    }
    return HI_SUCCESS;
}
HI_S32 mpi_sys_Bind(IPC_VEDIO_S *pstIpcVedio)
{
	HI_S32 j, s32Ret;
    
    MPP_CHN_S stSrcChn;
    MPP_CHN_S stDestChn;
    VI_DEV ViDev = 0;
    VI_CHN ViChn = 0;
	VPSS_GRP VpssGrp = 0;
	VPSS_CHN VpssChn = 0;
	VENC_CHN VencChn = 0;

	stSrcChn.enModId  = HI_ID_VIU;
    stSrcChn.s32DevId = ViDev;
    stSrcChn.s32ChnId = ViChn;
    stDestChn.enModId  = HI_ID_VPSS;
    stDestChn.s32DevId = VpssGrp;
    stDestChn.s32ChnId = VpssChn;
    s32Ret = HI_MPI_SYS_Bind(&stSrcChn, &stDestChn);
    if (s32Ret != HI_SUCCESS)
    {
        printf("VI Bind VPSS failed with %#x!\n", s32Ret);
        return HI_FAILURE;
    }
	for (j = 0; j < pstIpcVedio->u32StreamNum; j++)
	{
		VpssChn = j;
		VencChn = j;
		stSrcChn.enModId = HI_ID_VPSS;
	    stSrcChn.s32DevId = VpssGrp;
	    stSrcChn.s32ChnId = VpssChn;

	    stDestChn.enModId = HI_ID_VENC;
	    stDestChn.s32DevId = 0;
	    stDestChn.s32ChnId = VencChn;

	    s32Ret = HI_MPI_SYS_Bind(&stSrcChn, &stDestChn);
	    if (s32Ret != HI_SUCCESS)
	    {
	        printf("VPSS Bind VENC failed with %#x!\n", s32Ret);
	        return HI_FAILURE;
	    }
	}
    return HI_SUCCESS;
}

HI_S32 mpi_sys_UnBind(IPC_VEDIO_S *pstIpcVedio)
{
	HI_S32 j, s32Ret;
    
    MPP_CHN_S stSrcChn;
    MPP_CHN_S stDestChn;
    VI_DEV ViDev = 0;
    VI_CHN ViChn = 0;
	VPSS_GRP VpssGrp = 0;
	VPSS_CHN VpssChn = 0;
	VENC_CHN VencChn = 0;

	stSrcChn.enModId  = HI_ID_VIU;
    stSrcChn.s32DevId = ViDev;
    stSrcChn.s32ChnId = ViChn;
    stDestChn.enModId  = HI_ID_VPSS;
    stDestChn.s32DevId = VpssGrp;
    stDestChn.s32ChnId = VpssChn;
    s32Ret = HI_MPI_SYS_UnBind(&stSrcChn, &stDestChn);
    if (s32Ret != HI_SUCCESS)
    {
        printf("VI UnBind VPSS failed with %#x!\n", s32Ret);
        return HI_FAILURE;
    }
	for (j = 0; j < pstIpcVedio->u32StreamNum; j++)
	{
		VpssChn = j;
		VencChn = j;
		stSrcChn.enModId = HI_ID_VPSS;
	    stSrcChn.s32DevId = VpssGrp;
	    stSrcChn.s32ChnId = VpssChn;

	    stDestChn.enModId = HI_ID_VENC;
	    stDestChn.s32DevId = 0;
	    stDestChn.s32ChnId = VencChn;

	    s32Ret = HI_MPI_SYS_UnBind(&stSrcChn, &stDestChn);
	    if (s32Ret != HI_SUCCESS)
	    {
	        printf("VPSS UnBind VENC failed with %#x!\n", s32Ret);
	        return HI_FAILURE;
	    }
	}
    return HI_SUCCESS;
}


