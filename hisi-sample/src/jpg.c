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



/******************************************************************************
* function :  Thumbnail of 1*1080p jpeg
******************************************************************************/
HI_S32 VENC_1080P_JPEG_Thumb(SOC_TYPE socType, SAMPLE_VI_MODE_E sensorType) {
    VIDEO_NORM_E gs_enNorm = VIDEO_ENCODING_MODE_NTSC;
    /******************************************
     step  1: init sys variable
    ******************************************/
    VB_CONF_S stVbConf;
    memset(&stVbConf,0,sizeof(VB_CONF_S));

    stVbConf.u32MaxPoolCnt = 128;
    PIC_SIZE_E enSize;
    SAMPLE_COMM_VI_GetSizeBySensor(&enSize, sensorType);

    /*video buffer*/
    HI_U32 u32BlkSize = SAMPLE_COMM_SYS_CalcPicVbBlkSize(gs_enNorm, enSize, SAMPLE_PIXEL_FORMAT, SAMPLE_SYS_ALIGN_WIDTH);

    HI_U32 g_u32BlkCnt = 4;
    switch (socType) {
        case hi3516cv200: { g_u32BlkCnt = 10; break; }
        case hi3518ev200: { g_u32BlkCnt = 4; break; }
        case hi3518ev201: { g_u32BlkCnt = 4; break; }
        default:
            printf("not support this chip\n");
            return HI_FAILURE;
    }

    stVbConf.astCommPool[0].u32BlkSize = u32BlkSize;
    stVbConf.astCommPool[0].u32BlkCnt = g_u32BlkCnt;

    /******************************************
     step 2: mpp system init.
    ******************************************/
    HI_S32 s32Ret = SAMPLE_COMM_SYS_Init_With_DCF(&stVbConf);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("system init failed with %d!\n", s32Ret);
        goto END_VENC_MJPEG_JPEG_0;
    }

    /******************************************
     step 3: start vi dev & chn to capture
    ******************************************/
    SAMPLE_VI_CONFIG_S stViConfig = {0};
    stViConfig.enViMode   = sensorType;
    stViConfig.enRotate   = ROTATE_NONE;
    stViConfig.enNorm     = VIDEO_ENCODING_MODE_AUTO;
    stViConfig.enViChnSet = VI_CHN_SET_NORMAL;
    s32Ret = SAMPLE_COMM_VI_StartVi(&stViConfig);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("start vi failed!\n");
        goto END_VENC_MJPEG_JPEG_1;
    }

    /******************************************
     step 4: start vpss and vi bind vpss
    ******************************************/
    SIZE_S stSize;
    s32Ret = SAMPLE_COMM_SYS_GetPicSize(gs_enNorm, enSize, &stSize);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("SAMPLE_COMM_SYS_GetPicSize failed!\n");
        goto END_VENC_MJPEG_JPEG_1;
    }

    VPSS_GRP_ATTR_S stVpssGrpAttr;
    stVpssGrpAttr.u32MaxW = stSize.u32Width;
    stVpssGrpAttr.u32MaxH = stSize.u32Height;
    stVpssGrpAttr.bIeEn = HI_FALSE;
    stVpssGrpAttr.bNrEn = HI_TRUE;
    stVpssGrpAttr.bHistEn = HI_FALSE;
    stVpssGrpAttr.enDieMode = VPSS_DIE_MODE_NODIE;
    stVpssGrpAttr.enPixFmt = PIXEL_FORMAT_YUV_SEMIPLANAR_420;
	stVpssGrpAttr.bDciEn = HI_FALSE;

    VPSS_GRP VpssGrp = 0;
    s32Ret = SAMPLE_COMM_VPSS_StartGroup(VpssGrp, &stVpssGrpAttr);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("Start Vpss failed!\n");
        goto END_VENC_MJPEG_JPEG_2;
    }

    s32Ret = SAMPLE_COMM_VI_BindVpss(stViConfig.enViMode);
    if (HI_SUCCESS != s32Ret) {
        SAMPLE_PRT("Vi bind Vpss failed!\n");
        goto END_VENC_MJPEG_JPEG_3;
    }

    VPSS_CHN_MODE_S stVpssChnMode;
    stVpssChnMode.enChnMode     = VPSS_CHN_MODE_USER;
    stVpssChnMode.bDouble       = HI_FALSE;
    stVpssChnMode.enPixelFormat = PIXEL_FORMAT_YUV_SEMIPLANAR_420;
    stVpssChnMode.u32Width      = stSize.u32Width;
    stVpssChnMode.u32Height     = stSize.u32Height;
    stVpssChnMode.enCompressMode = COMPRESS_MODE_NONE;

    VPSS_CHN_ATTR_S stVpssChnAttr;
    memset(&stVpssChnAttr, 0, sizeof(stVpssChnAttr));
    stVpssChnAttr.s32SrcFrameRate = -1;
    stVpssChnAttr.s32DstFrameRate = -1;

    VPSS_CHN VpssChn = 0;
    s32Ret = SAMPLE_COMM_VPSS_EnableChn(VpssGrp, VpssChn, &stVpssChnAttr, &stVpssChnMode, HI_NULL);
    if (HI_SUCCESS != s32Ret) {
        SAMPLE_PRT("Enable vpss chn failed!\n");
        goto END_VENC_MJPEG_JPEG_4;
    }

    /******************************************
     step 5: set CDF info
    ******************************************/
    ISP_DCF_INFO_S stIspDCF;
    HI_MPI_ISP_GetDCFInfo(&stIspDCF);

    //description: Thumbnail test
    memcpy(stIspDCF.au8ImageDescription,"Thumbnail test",strlen("Thumbnail test"));

    //manufacturer: Hisilicon
    memcpy(stIspDCF.au8Make,"Hisilicon",strlen("Hisilicon"));

    //model number: Hisilicon IP Camera
    memcpy(stIspDCF.au8Model,"Hisilicon IP Camera",strlen("Hisilicon IP Camera"));

    //firmware version: v.1.1.0
    memcpy(stIspDCF.au8Software,"v.1.1.0",strlen("v.1.1.0"));

    stIspDCF.u16ISOSpeedRatings         = 500;
    stIspDCF.u32ExposureBiasValue       = 5;
    stIspDCF.u32ExposureTime            = 0x00010004;
    stIspDCF.u32FNumber                 = 0x0001000f;
    stIspDCF.u32FocalLength             = 0x00640001;
    stIspDCF.u32MaxApertureValue        = 0x00010001;
    stIspDCF.u8Contrast                 = 5;
    stIspDCF.u8CustomRendered           = 0;
    stIspDCF.u8ExposureMode             = 0;
    stIspDCF.u8ExposureProgram          = 1;
    stIspDCF.u8FocalLengthIn35mmFilm    = 1;
    stIspDCF.u8GainControl              = 1;
    stIspDCF.u8LightSource              = 1;
    stIspDCF.u8MeteringMode             = 1;
    stIspDCF.u8Saturation               = 1;
    stIspDCF.u8SceneCaptureType         = 1;
    stIspDCF.u8SceneType                = 0;
    stIspDCF.u8Sharpness                = 5;
    stIspDCF.u8WhiteBalance             = 1;

    HI_MPI_ISP_SetDCFInfo(&stIspDCF);

    /******************************************
     step 6: start stream venc
    ******************************************/
    VpssGrp = 0;
    VpssChn = 0;
    VENC_CHN VencChn = 0;

    s32Ret = SAMPLE_COMM_VENC_SnapStart(VencChn, &stSize, HI_TRUE);
    if (HI_SUCCESS != s32Ret) {
        SAMPLE_PRT("Start snap failed!\n");
        goto END_VENC_MJPEG_JPEG_5;
    }

    s32Ret = SAMPLE_COMM_VENC_BindVpss(VencChn, VpssGrp, VpssChn);
    if (HI_SUCCESS != s32Ret) {
        SAMPLE_PRT("Start Venc failed!\n");
        goto END_VENC_MJPEG_JPEG_5;
    }

    /******************************************
     step 7: stream venc process -- get stream, then save it to file.
    ******************************************/
    s32Ret = SAMPLE_COMM_VENC_SnapProcess(VencChn, HI_TRUE, HI_TRUE);
    if (HI_SUCCESS != s32Ret) {
        printf("%s: sanp process failed!\n", __FUNCTION__);
    }

    /******************************************
     step 8: exit process
    ******************************************/
    SAMPLE_COMM_VENC_StopGetStream();

END_VENC_MJPEG_JPEG_5:
    VpssGrp = 0;
    VpssChn = 0;
    VencChn = 0;
    SAMPLE_COMM_VENC_UnBindVpss(VencChn, VpssGrp, VpssChn);
    SAMPLE_COMM_VENC_Stop(VencChn);
END_VENC_MJPEG_JPEG_4:    //vpss stop
    VpssGrp = 0;
    VpssChn = 0;
    SAMPLE_COMM_VPSS_DisableChn(VpssGrp, VpssChn);
END_VENC_MJPEG_JPEG_3:    //vpss stop
    SAMPLE_COMM_VI_UnBindVpss(stViConfig.enViMode);
END_VENC_MJPEG_JPEG_2:    //vpss stop
    SAMPLE_COMM_VPSS_StopGroup(VpssGrp);
END_VENC_MJPEG_JPEG_1:    //vi stop
    SAMPLE_COMM_VI_StopVi(&stViConfig);
END_VENC_MJPEG_JPEG_0:	//system exit
    SAMPLE_COMM_SYS_Exit();

    return s32Ret;
}


int main(int argc, char *argv[]) {
    SOC_TYPE socType;
    HI_S32 s32Ret = initSoc(argc, argv, &socType);
    if (s32Ret == HI_FAILURE) return EXIT_FAILURE;

    SAMPLE_VI_MODE_E sensorType;
    s32Ret = initSensor(argc, argv, &sensorType);
    if (s32Ret == HI_FAILURE) return EXIT_FAILURE;

    signal(SIGINT, SAMPLE_VENC_HandleSig);
    signal(SIGTERM, SAMPLE_VENC_HandleSig);

    s32Ret = VENC_1080P_JPEG_Thumb(socType, sensorType);
    if (HI_SUCCESS == s32Ret) return EXIT_SUCCESS;
    printf("program exit abnormally EXIT_FAILURE!\n");
    UnloadSensorLibrary();
    return EXIT_FAILURE;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */
