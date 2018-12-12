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
#include "hp_vpss.h"
static VPSS_ATTR_S * g_pstVpssAttr;
static HI_S32 s32VpssStartGrp(VPSS_GRP VpssGrp, VPSS_GRP_ATTR_S *pstVpssGrpAttr)
{
    HI_S32 s32Ret;
    VPSS_NR_PARAM_U unNrParam = {{0}};
    
    if (VpssGrp < 0 || VpssGrp > VPSS_MAX_GRP_NUM)
    {
        printf("VpssGrp%d is out of rang. \n", VpssGrp);
        return HI_FAILURE;
    }

    if (HI_NULL == pstVpssGrpAttr)
    {
        printf("null ptr,line%d. \n", __LINE__);
        return HI_FAILURE;
    }

    s32Ret = HI_MPI_VPSS_CreateGrp(VpssGrp, pstVpssGrpAttr);
    if (s32Ret != HI_SUCCESS)
    {
        printf("HI_MPI_VPSS_CreateGrp failed with %#x!\n", s32Ret);
        return HI_FAILURE;
    }

    /*** set vpss 3DNR param ***/
    s32Ret = HI_MPI_VPSS_GetNRParam(VpssGrp, &unNrParam);
    if (s32Ret != HI_SUCCESS)
    {
        printf("failed with %#x!\n", s32Ret);
        return HI_FAILURE;
    }
    
    
    s32Ret = HI_MPI_VPSS_SetNRParam(VpssGrp, &unNrParam);
    if (s32Ret != HI_SUCCESS)
    {
        printf("failed with %#x!\n", s32Ret);
        return HI_FAILURE;
    }

    s32Ret = HI_MPI_VPSS_StartGrp(VpssGrp);
    if (s32Ret != HI_SUCCESS)
    {
        printf("HI_MPI_VPSS_StartGrp failed with %#x\n", s32Ret);
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

static HI_S32 s32VpssStartChn(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, 
                                                  VPSS_CHN_ATTR_S *pstVpssChnAttr,
                                                  VPSS_CHN_MODE_S *pstVpssChnMode,
                                                  VPSS_EXT_CHN_ATTR_S *pstVpssExtChnAttr)
{
    HI_S32 s32Ret;

    if (VpssGrp < 0 || VpssGrp > VPSS_MAX_GRP_NUM)
    {
        printf("VpssGrp%d is out of rang[0,%d]. \n", VpssGrp, VPSS_MAX_GRP_NUM);
        return HI_FAILURE;
    }

    if (VpssChn < 0 || VpssChn > VPSS_MAX_CHN_NUM)
    {
        printf("VpssChn%d is out of rang[0,%d]. \n", VpssChn, VPSS_MAX_CHN_NUM);
        return HI_FAILURE;
    }

    if (HI_NULL == pstVpssChnAttr && HI_NULL == pstVpssExtChnAttr)
    {
        printf("null ptr,line%d. \n", __LINE__);
        return HI_FAILURE;
    }

    if (VpssChn < VPSS_MAX_PHY_CHN_NUM)
    {
        s32Ret = HI_MPI_VPSS_SetChnAttr(VpssGrp, VpssChn, pstVpssChnAttr);
        if (s32Ret != HI_SUCCESS)
        {
            printf("HI_MPI_VPSS_SetChnAttr failed with %#x\n", s32Ret);
            return HI_FAILURE;
        }
    }
    else
    {
        s32Ret = HI_MPI_VPSS_SetExtChnAttr(VpssGrp, VpssChn, pstVpssExtChnAttr);
        if (s32Ret != HI_SUCCESS)
        {
            printf("%s failed with %#x\n", __FUNCTION__, s32Ret);
            return HI_FAILURE;
        }
    }
    
    if (VpssChn < VPSS_MAX_PHY_CHN_NUM && HI_NULL != pstVpssChnMode)
    {
        s32Ret = HI_MPI_VPSS_SetChnMode(VpssGrp, VpssChn, pstVpssChnMode);
        if (s32Ret != HI_SUCCESS)
        {
            printf("%s failed with %#x\n", __FUNCTION__, s32Ret);
            return HI_FAILURE;
        }     
    }
    
    s32Ret = HI_MPI_VPSS_EnableChn(VpssGrp, VpssChn);
    if (s32Ret != HI_SUCCESS)
    {
        printf("HI_MPI_VPSS_EnableChn failed with %#x\n", s32Ret);
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

static HI_S32 s32StartVPSS(const IPC_VEDIO_S* pstIpcVedio)
{
	HI_S32 s32Ret,i;
	
	static VPSS_GRP_ATTR_S stVpssGrpAttr =
	{
    	.u32MaxW = 1920,  /*MAX width of the group*/                    
    	.u32MaxH = 1080,  /*MAX height of the group*/
    	.enPixFmt = PIXEL_FORMAT_YUV_SEMIPLANAR_420, /*Pixel format*/
    	.bIeEn = HI_FALSE,    				/*Image enhance enable*/
    	.bDciEn =  HI_FALSE, 				/*Dynamic contrast Improve enable*/
    	.bNrEn =   HI_TRUE,					/*Noise reduce enable*/
    	.bHistEn =  HI_FALSE, 				/*Hist enable*/
		.enDieMode =  VPSS_DIE_MODE_NODIE	/*De-interlace enable*/
	};
	
	static VPSS_CHN_ATTR_S stVpssChnAttr =
	{
		.bSpEn = HI_FALSE,            	/*Sharpen enable*/         
	    .bBorderEn = HI_FALSE,          /*Frame enable*/
	    .bMirror = HI_FALSE,            /*mirror enable*/
	    .bFlip = HI_FALSE,              /*flip   enable*/
	    .s32SrcFrameRate = 25,    		/* source frame rate */
	    .s32DstFrameRate = -1,    		/* dest  frame rate */   
	    .stBorder =
	    {
			0,0,0,0,0
		}
	};
	
	static VPSS_CHN_MODE_S stVpssChnMode = 
	{
		.enChnMode = VPSS_CHN_MODE_USER,   /*Vpss channel's work mode*/
	    .u32Width = 1920,              		/*Width of target image*/
	    .u32Height = 1080,             		/*Height of target image*/
	    .bDouble =  HI_FALSE,            	/*Field-frame transfer£¬only valid for VPSS_PRE0_CHN*/
	    .enPixelFormat =PIXEL_FORMAT_YUV_SEMIPLANAR_420,/*Pixel format of target image*/
	    .enCompressMode = COMPRESS_MODE_SEG   /*Compression mode of the output*/
	};
	
    static VPSS_ATTR_S stVpssAttr = {
		.VpssGrp = 0,
  		.VpssChnNum = 1,
  		.pstVpssGrpAttr = &stVpssGrpAttr,
  		.pstVpssChnAttr = &stVpssChnAttr,
  		.pstVpssChnMode = &stVpssChnMode
	};

	g_pstVpssAttr = &stVpssAttr;
	g_pstVpssAttr->VpssChnNum = pstIpcVedio->u32StreamNum;
	/*sync grp attr*/
	g_pstVpssAttr->pstVpssGrpAttr->u32MaxW = pstIpcVedio->astStreamFmt[0].stSize.u32Width;
	g_pstVpssAttr->pstVpssGrpAttr->u32MaxH = pstIpcVedio->astStreamFmt[0].stSize.u32Height;
	g_pstVpssAttr->pstVpssGrpAttr->enPixFmt = pstIpcVedio->e_pixFmt;

	s32Ret = s32VpssStartGrp(g_pstVpssAttr->VpssGrp, g_pstVpssAttr->pstVpssGrpAttr);
	if (HI_SUCCESS != s32Ret)
    {
        printf("Start Vpss Grp failed!\n");
		return s32Ret;
    }
	
	for(i = 0; i < g_pstVpssAttr->VpssChnNum; i++)
	{
		/*sync chn attr*/
		g_pstVpssAttr->pstVpssChnMode->u32Width = pstIpcVedio->astStreamFmt[i].stSize.u32Width;
		g_pstVpssAttr->pstVpssChnMode->u32Height = pstIpcVedio->astStreamFmt[i].stSize.u32Height;
		g_pstVpssAttr->pstVpssChnMode->enPixelFormat = pstIpcVedio->e_pixFmt;

		g_pstVpssAttr->pstVpssChnAttr->s32DstFrameRate = pstIpcVedio->astStreamFmt[i].f32FrameRate;
		s32Ret = s32VpssStartChn(g_pstVpssAttr->VpssGrp,i,g_pstVpssAttr->pstVpssChnAttr,g_pstVpssAttr->pstVpssChnMode,HI_NULL);
		if (HI_SUCCESS != s32Ret)
    	{
	        printf("Start Vpss Chn failed!\n");
			return s32Ret;
    	}
	}
	return HI_SUCCESS;
}

static HI_S32 s32VpssStopGrp(VPSS_GRP VpssGrp)
{
    HI_S32 s32Ret;

    if (VpssGrp < 0 || VpssGrp > VPSS_MAX_GRP_NUM)
    {
        printf("VpssGrp%d is out of rang[0,%d]. \n", VpssGrp, VPSS_MAX_GRP_NUM);
        return HI_FAILURE;
    }

    s32Ret = HI_MPI_VPSS_StopGrp(VpssGrp);
    if (s32Ret != HI_SUCCESS)
    {
        printf("%s failed with %#x\n", __FUNCTION__, s32Ret);
        return HI_FAILURE;
    }

    s32Ret = HI_MPI_VPSS_DestroyGrp(VpssGrp);
    if (s32Ret != HI_SUCCESS)
    {
        printf("%s failed with %#x\n", __FUNCTION__, s32Ret);
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

static HI_S32 s32VpssStopChn(VPSS_GRP VpssGrp, VPSS_CHN VpssChn)
{
    HI_S32 s32Ret;

    if (VpssGrp < 0 || VpssGrp > VPSS_MAX_GRP_NUM)
    {
        printf("VpssGrp%d is out of rang[0,%d]. \n", VpssGrp, VPSS_MAX_GRP_NUM);
        return HI_FAILURE;
    }

    if (VpssChn < 0 || VpssChn > VPSS_MAX_CHN_NUM)
    {
        printf("VpssChn%d is out of rang[0,%d]. \n", VpssChn, VPSS_MAX_CHN_NUM);
        return HI_FAILURE;
    }
    
    s32Ret = HI_MPI_VPSS_DisableChn(VpssGrp, VpssChn);
    if (s32Ret != HI_SUCCESS)
    {
        printf("%s failed with %#x\n", __FUNCTION__, s32Ret);
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}
HI_S32 VPSS_init(const IPC_VEDIO_S* pstIpcVedio)
{
	HI_S32 s32Ret;
	if (HI_NULL == pstIpcVedio)
    {
        printf("null ptr,line%d. \n", __LINE__);
        return HI_FAILURE;
    }
	s32Ret = s32StartVPSS(pstIpcVedio);
	return s32Ret;
}
HI_S32 VPSS_exit(void)
{
	HI_S32 i;
	for(i = 0; i < g_pstVpssAttr->VpssChnNum; i++)
	{
		s32VpssStopChn(g_pstVpssAttr->VpssGrp,i);
	}
	s32VpssStopGrp(g_pstVpssAttr->VpssGrp);
	return HI_SUCCESS;
}