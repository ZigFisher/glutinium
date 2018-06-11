/******************************************************************************
  A simple program of Hisilicon HI3531 video encode implementation.
  Copyright (C), 2010-2011, Hisilicon Tech. Co., Ltd.
 ******************************************************************************
    Modification:  2011-2 Created
******************************************************************************/
#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* End of #ifdef __cplusplus */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>

#include "sample_comm.h"

VIDEO_NORM_E gs_enNorm = VIDEO_ENCODING_MODE_NTSC;
//
//#ifdef hi3518ev201
//
//HI_U32 g_u32BlkCnt = 4;
//#endif
//
//#ifdef hi3518ev200
//
//HI_U32 g_u32BlkCnt = 4;
//
//#endif
//
//#ifdef hi3516cv200
//
//HI_U32 g_u32BlkCnt = 10;
//
//#endif




/******************************************************************************
* function :  H.264@1080p@30fps+H.264@VGA@30fps


******************************************************************************/
HI_S32 SAMPLE_VENC_1080P_CLASSIC(HI_VOID *args)
{
    hithreadArgs_t *arg = (hithreadArgs_t*) args;
    SOC_TYPE socType = arg->socType;
    SAMPLE_VI_MODE_E enMode = arg->enMode;

    PAYLOAD_TYPE_E enPayLoad[3]= {PT_H264, PT_H264,PT_H264};
    PIC_SIZE_E enSize[3] = {PIC_HD1080, PIC_VGA,PIC_QVGA};
	HI_U32 u32Profile = 0;
	
    VB_CONF_S stVbConf;
    SAMPLE_VI_CONFIG_S stViConfig = {0};
    
    VPSS_GRP VpssGrp;
    VPSS_CHN VpssChn;
    VPSS_GRP_ATTR_S stVpssGrpAttr;
    VPSS_CHN_ATTR_S stVpssChnAttr;
    VPSS_CHN_MODE_S stVpssChnMode;
    
    VENC_CHN VencChn;
    SAMPLE_RC_E enRcMode= SAMPLE_RC_CBR;
	
    HI_S32 s32ChnNum=0;
    
    HI_S32 s32Ret = HI_SUCCESS;
    HI_U32 u32BlkSize;
    SIZE_S stSize;
    char c;

    HI_U32 g_u32BlkCnt = 4;
    switch (socType) {
        case hi3516cv200: { g_u32BlkCnt = 10; break; }
        case hi3518ev200: { g_u32BlkCnt = 4; break; }
        case hi3518ev201: { g_u32BlkCnt = 4; break; }
        default:
            printf("not support this chip\n");
            return HI_FAILURE;
    }

    /******************************************
     step  1: init sys variable 
    ******************************************/
    memset(&stVbConf,0,sizeof(VB_CONF_S));
    
	SAMPLE_COMM_VI_GetSizeBySensor(&enSize[0], enMode);
    if (PIC_HD1080 == enSize[0])
    {
        enSize[1] = PIC_VGA;
		s32ChnNum = 2;
    }
    else if (PIC_HD720 == enSize[0])
    {
        enSize[1] = PIC_VGA;			
		enSize[2] = PIC_QVGA;
		s32ChnNum = 3;
    }
    else
    {
        printf("not support this sensor\n");
        return HI_FAILURE;
    }
#ifdef hi3518ev201
	s32ChnNum = 1;
#endif
	s32ChnNum=1;
	printf("s32ChnNum = %d\n",s32ChnNum);

    stVbConf.u32MaxPoolCnt = 128;
    /*video buffer*/
	if(s32ChnNum >= 1)
    {
	    u32BlkSize = SAMPLE_COMM_SYS_CalcPicVbBlkSize(gs_enNorm,\
	                enSize[0], SAMPLE_PIXEL_FORMAT, SAMPLE_SYS_ALIGN_WIDTH);
	    stVbConf.astCommPool[0].u32BlkSize = u32BlkSize;
	    stVbConf.astCommPool[0].u32BlkCnt = g_u32BlkCnt;
	}
	if(s32ChnNum >= 2)
    {
	    u32BlkSize = SAMPLE_COMM_SYS_CalcPicVbBlkSize(gs_enNorm,\
	                enSize[1], SAMPLE_PIXEL_FORMAT, SAMPLE_SYS_ALIGN_WIDTH);
	    stVbConf.astCommPool[1].u32BlkSize = u32BlkSize;
	    stVbConf.astCommPool[1].u32BlkCnt =g_u32BlkCnt;
	}
	if(s32ChnNum >= 3)
    {
		u32BlkSize = SAMPLE_COMM_SYS_CalcPicVbBlkSize(gs_enNorm,\
                enSize[2], SAMPLE_PIXEL_FORMAT, SAMPLE_SYS_ALIGN_WIDTH);
		stVbConf.astCommPool[2].u32BlkSize = u32BlkSize;
		stVbConf.astCommPool[2].u32BlkCnt = g_u32BlkCnt;
    }

    /******************************************
     step 2: mpp system init. 
    ******************************************/
    s32Ret = SAMPLE_COMM_SYS_Init(&stVbConf);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("system init failed with %d!\n", s32Ret);
        goto END_VENC_1080P_CLASSIC_0;
    }

    /******************************************
     step 3: start vi dev & chn to capture
    ******************************************/
    stViConfig.enViMode   = enMode;
    stViConfig.enRotate   = ROTATE_NONE;
    stViConfig.enNorm     = VIDEO_ENCODING_MODE_AUTO;
    stViConfig.enViChnSet = VI_CHN_SET_NORMAL;
    stViConfig.enWDRMode  = WDR_MODE_NONE;
    s32Ret = SAMPLE_COMM_VI_StartVi(&stViConfig);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("start vi failed!\n");
        goto END_VENC_1080P_CLASSIC_1;
    }
    
    /******************************************
     step 4: start vpss and vi bind vpss
    ******************************************/
    s32Ret = SAMPLE_COMM_SYS_GetPicSize(gs_enNorm, enSize[0], &stSize);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("SAMPLE_COMM_SYS_GetPicSize failed!\n");
        goto END_VENC_1080P_CLASSIC_1;
    }
	if(s32ChnNum >= 1)
	{
		VpssGrp = 0;
	    stVpssGrpAttr.u32MaxW = stSize.u32Width;
	    stVpssGrpAttr.u32MaxH = stSize.u32Height;
	    stVpssGrpAttr.bIeEn = HI_FALSE;
	    stVpssGrpAttr.bNrEn = HI_TRUE;
	    stVpssGrpAttr.bHistEn = HI_FALSE;
	    stVpssGrpAttr.bDciEn = HI_FALSE;
	    stVpssGrpAttr.enDieMode = VPSS_DIE_MODE_NODIE;
	    stVpssGrpAttr.enPixFmt = PIXEL_FORMAT_YUV_SEMIPLANAR_420;
		
	    s32Ret = SAMPLE_COMM_VPSS_StartGroup(VpssGrp, &stVpssGrpAttr);
	    if (HI_SUCCESS != s32Ret)
	    {
	        SAMPLE_PRT("Start Vpss failed!\n");
	        goto END_VENC_1080P_CLASSIC_2;
	    }

	    s32Ret = SAMPLE_COMM_VI_BindVpss(stViConfig.enViMode);
	    if (HI_SUCCESS != s32Ret)
	    {
	        SAMPLE_PRT("Vi bind Vpss failed!\n");
	        goto END_VENC_1080P_CLASSIC_3;
	    }

		VpssChn = 0;
	    stVpssChnMode.enChnMode      = VPSS_CHN_MODE_USER;
	    stVpssChnMode.bDouble        = HI_FALSE;
	    stVpssChnMode.enPixelFormat  = PIXEL_FORMAT_YUV_SEMIPLANAR_420;
	    stVpssChnMode.u32Width       = stSize.u32Width;
	    stVpssChnMode.u32Height      = stSize.u32Height;
	    stVpssChnMode.enCompressMode = COMPRESS_MODE_SEG;
	    memset(&stVpssChnAttr, 0, sizeof(stVpssChnAttr));
	    stVpssChnAttr.s32SrcFrameRate = -1;
	    stVpssChnAttr.s32DstFrameRate = -1;
	    s32Ret = SAMPLE_COMM_VPSS_EnableChn(VpssGrp, VpssChn, &stVpssChnAttr, &stVpssChnMode, HI_NULL);
	    if (HI_SUCCESS != s32Ret)
	    {
	        SAMPLE_PRT("Enable vpss chn failed!\n");
	        goto END_VENC_1080P_CLASSIC_4;
	    }
	}

	if(s32ChnNum >= 2)
	{
		s32Ret = SAMPLE_COMM_SYS_GetPicSize(gs_enNorm, enSize[1], &stSize);
	    if (HI_SUCCESS != s32Ret)
	    {
	        SAMPLE_PRT("SAMPLE_COMM_SYS_GetPicSize failed!\n");
	        goto END_VENC_1080P_CLASSIC_4;
	    }
	    VpssChn = 1;
	    stVpssChnMode.enChnMode       = VPSS_CHN_MODE_USER;
	    stVpssChnMode.bDouble         = HI_FALSE;
	    stVpssChnMode.enPixelFormat   = PIXEL_FORMAT_YUV_SEMIPLANAR_420;
	    stVpssChnMode.u32Width        = stSize.u32Width;
	    stVpssChnMode.u32Height       = stSize.u32Height;
	    stVpssChnMode.enCompressMode  = COMPRESS_MODE_SEG;
	    stVpssChnAttr.s32SrcFrameRate = -1;
	    stVpssChnAttr.s32DstFrameRate = -1;
	    s32Ret = SAMPLE_COMM_VPSS_EnableChn(VpssGrp, VpssChn, &stVpssChnAttr, &stVpssChnMode, HI_NULL);
	    if (HI_SUCCESS != s32Ret)
	    {
	        SAMPLE_PRT("Enable vpss chn failed!\n");
	        goto END_VENC_1080P_CLASSIC_4;
	    }
	}
	

	if(s32ChnNum >= 3)
	{	
		s32Ret = SAMPLE_COMM_SYS_GetPicSize(gs_enNorm, enSize[2], &stSize);
	    if (HI_SUCCESS != s32Ret)
	    {
	        SAMPLE_PRT("SAMPLE_COMM_SYS_GetPicSize failed!\n");
	        goto END_VENC_1080P_CLASSIC_4;
	    }
		VpssChn = 2;
		stVpssChnMode.enChnMode 	= VPSS_CHN_MODE_USER;
		stVpssChnMode.bDouble		= HI_FALSE;
		stVpssChnMode.enPixelFormat = PIXEL_FORMAT_YUV_SEMIPLANAR_420;
		stVpssChnMode.u32Width		= stSize.u32Width;
		stVpssChnMode.u32Height 	= stSize.u32Height;
		stVpssChnMode.enCompressMode = COMPRESS_MODE_NONE;
		
		stVpssChnAttr.s32SrcFrameRate = -1;
		stVpssChnAttr.s32DstFrameRate = -1;
		
		s32Ret = SAMPLE_COMM_VPSS_EnableChn(VpssGrp, VpssChn, &stVpssChnAttr, &stVpssChnMode, HI_NULL);
		if (HI_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("Enable vpss chn failed!\n");
			goto END_VENC_1080P_CLASSIC_4;
		}
	}
    /******************************************
     step 5: start stream venc
    ******************************************/
    /*** HD1080P **/
    //printf("\t c) cbr.\n");
    //printf("\t v) vbr.\n");
    //printf("\t f) fixQp\n");
    //printf("please input choose rc mode!\n");
    //c = (char)getchar();
   // switch(c)
    //{
    //    case 'c':
            enRcMode = SAMPLE_RC_CBR;
    //        break;
    //    case 'v':
    //        enRcMode = SAMPLE_RC_VBR;
    //        break;
    //    case 'f':
    //        enRcMode = SAMPLE_RC_FIXQP;
    //        break;
    //    default:
    //        printf("rc mode! is invaild!\n");
    //        goto END_VENC_1080P_CLASSIC_4;
    //}

	/*** enSize[0] **/
	if(s32ChnNum >= 1)
	{
		VpssGrp = 0;
	    VpssChn = 0;
	    VencChn = 0;
	    s32Ret = SAMPLE_COMM_VENC_Start(VencChn, enPayLoad[0],\
	                                   gs_enNorm, enSize[0], enRcMode,u32Profile);
	    if (HI_SUCCESS != s32Ret)
	    {
	        SAMPLE_PRT("Start Venc failed!\n");
	        goto END_VENC_1080P_CLASSIC_5;
	    }

	    s32Ret = SAMPLE_COMM_VENC_BindVpss(VencChn, VpssGrp, VpssChn);
	    if (HI_SUCCESS != s32Ret)
	    {
	        SAMPLE_PRT("Start Venc failed!\n");
	        goto END_VENC_1080P_CLASSIC_5;
	    }
	}

	/*** enSize[1] **/
	if(s32ChnNum >= 2)
	{
		VpssChn = 1;
	    VencChn = 1;
	    s32Ret = SAMPLE_COMM_VENC_Start(VencChn, enPayLoad[1], \
	                                    gs_enNorm, enSize[1], enRcMode,u32Profile);
	    if (HI_SUCCESS != s32Ret)
	    {
	        SAMPLE_PRT("Start Venc failed!\n");
	        goto END_VENC_1080P_CLASSIC_5;
	    }

	    s32Ret = SAMPLE_COMM_VENC_BindVpss(VencChn, VpssGrp, VpssChn);
	    if (HI_SUCCESS != s32Ret)
	    {
	        SAMPLE_PRT("Start Venc failed!\n");
	        goto END_VENC_1080P_CLASSIC_5;
	    }
	}
	/*** enSize[2] **/
	if(s32ChnNum >= 3)
	{
	    VpssChn = 2;
	    VencChn = 2;
	    s32Ret = SAMPLE_COMM_VENC_Start(VencChn, enPayLoad[2], \
	                                    gs_enNorm, enSize[2], enRcMode,u32Profile);
	    if (HI_SUCCESS != s32Ret)
	    {
	        SAMPLE_PRT("Start Venc failed!\n");
	        goto END_VENC_1080P_CLASSIC_5;
	    }

	    s32Ret = SAMPLE_COMM_VENC_BindVpss(VencChn, VpssGrp, VpssChn);
	    if (HI_SUCCESS != s32Ret)
	    {
	        SAMPLE_PRT("Start Venc failed!\n");
	        goto END_VENC_1080P_CLASSIC_5;
	    }
	}
    /******************************************
     step 6: stream venc process -- get stream, then save it to file. 
    ******************************************/
    s32Ret = SAMPLE_COMM_VENC_StartGetStream(s32ChnNum);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("Start Venc failed!\n");
        goto END_VENC_1080P_CLASSIC_5;
    }
    while(1)
	{
		sleep(1);
	}
    //printf("please press twice ENTER to exit this sample\n");
    //getchar();
    //getchar();

    /******************************************
     step 7: exit process
    ******************************************/
    SAMPLE_COMM_VENC_StopGetStream();
    
END_VENC_1080P_CLASSIC_5:
	
    VpssGrp = 0;
	switch(s32ChnNum)
	{
		case 3:
			VpssChn = 2;   
		    VencChn = 2;
		    SAMPLE_COMM_VENC_UnBindVpss(VencChn, VpssGrp, VpssChn);
		    SAMPLE_COMM_VENC_Stop(VencChn);
		case 2:
			VpssChn = 1;   
		    VencChn = 1;
		    SAMPLE_COMM_VENC_UnBindVpss(VencChn, VpssGrp, VpssChn);
		    SAMPLE_COMM_VENC_Stop(VencChn);
		case 1:
			VpssChn = 0;  
		    VencChn = 0;
		    SAMPLE_COMM_VENC_UnBindVpss(VencChn, VpssGrp, VpssChn);
		    SAMPLE_COMM_VENC_Stop(VencChn);
			break;
			
	}
    SAMPLE_COMM_VI_UnBindVpss(stViConfig.enViMode);
	
END_VENC_1080P_CLASSIC_4:	//vpss stop

    VpssGrp = 0;
	switch(s32ChnNum)
	{
		case 3:
			VpssChn = 2;
			SAMPLE_COMM_VPSS_DisableChn(VpssGrp, VpssChn);
		case 2:
			VpssChn = 1;
			SAMPLE_COMM_VPSS_DisableChn(VpssGrp, VpssChn);
		case 1:
			VpssChn = 0;
			SAMPLE_COMM_VPSS_DisableChn(VpssGrp, VpssChn);
		break;
	
	}

END_VENC_1080P_CLASSIC_3:    //vpss stop       
    SAMPLE_COMM_VI_UnBindVpss(stViConfig.enViMode);
END_VENC_1080P_CLASSIC_2:    //vpss stop   
    SAMPLE_COMM_VPSS_StopGroup(VpssGrp);
END_VENC_1080P_CLASSIC_1:	//vi stop
    SAMPLE_COMM_VI_StopVi(&stViConfig);
END_VENC_1080P_CLASSIC_0:	//system exit
    SAMPLE_COMM_SYS_Exit();
    
    return s32Ret;    
}


/******************************************************************************
* function    : main()
* Description : video venc sample
******************************************************************************/
//int main(int argc, char *argv[])
//{
//    HI_S32 s32Ret;
//    if ( (argc < 2) || (1 != strlen(argv[1])))
//    {
//        SAMPLE_VENC_Usage(argv[0]);
//        return HI_FAILURE;
//    }

//    signal(SIGINT, SAMPLE_VENC_HandleSig);
//    signal(SIGTERM, SAMPLE_VENC_HandleSig);
    
//    switch (*argv[1])
//    {
//        case '0':/* H.264@1080p@30fps+H.265@1080p@30fps+H.264@D1@30fps */
//            s32Ret = SAMPLE_VENC_1080P_CLASSIC();
//            break;
//        case '1':/* 1*1080p mjpeg encode + 1*1080p jpeg  */
//            s32Ret = SAMPLE_VENC_1080P_MJPEG_JPEG();
//            break;
//        case '2':/* low delay */
//            s32Ret = SAMPLE_VENC_LOW_DELAY();
//            break;
//        case '3':/* roibg framerate */
//            s32Ret = SAMPLE_VENC_ROIBG_CLASSIC();
//            break;
//        case '4':/* Thumbnail of 1*1080p jpeg  */
//            s32Ret = SAMPLE_VENC_1080P_JPEG_Thumb();
//            break;
//#ifndef hi3518ev201			
//		case '5':/* H.264 Svc-t */
//			s32Ret = SAMPLE_VENC_SVC_H264();
//			break;
//#endif
//        default:
//            printf("the index is invaild!\n");
//            SAMPLE_VENC_Usage(argv[0]);
//            return HI_FAILURE;
//    }
    
//    if (HI_SUCCESS == s32Ret)
//        printf("program exit normally!\n");
////    else
       // printf("program exit abnormally!\n");
 //   exit(s32Ret);
//}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */
