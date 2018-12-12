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
#include "hp_vi.h"
static pthread_t gs_IspPid = 0;
static HI_BOOL gbIspInited = HI_FALSE;
static VI_ATTR_S *g_pstViAttr;

/******************************************************************************
* funciton : stop ISP, and stop isp thread
******************************************************************************/
static HI_VOID vdIspStop(VI_ATTR_S * pstViAttr)
{
    ISP_DEV IspDev = pstViAttr->IspDev;

    if (!gbIspInited)
    {
        return;
    }
    
    HI_MPI_ISP_Exit(IspDev);
    if (gs_IspPid)
    {
        pthread_join(gs_IspPid, 0);
        gs_IspPid = 0;
    }
    gbIspInited = HI_FALSE;
    return;
}
static HI_S32 s32StopVi(VI_ATTR_S * pstViAttr)
{
    HI_S32 i;
    HI_S32 s32Ret;
    HI_U32 u32ChnNum;

    if(!pstViAttr)
    {
        printf("%s: null ptr\n", __FUNCTION__);
        return HI_FAILURE;
    }
    
    /*** Stop VI Chn ***/
	u32ChnNum = pstViAttr->u32ChnNum;
    for(i=0;i < u32ChnNum; i++)
    {
        /* Stop vi phy-chn */
        s32Ret = HI_MPI_VI_DisableChn(i);
        if (HI_SUCCESS != s32Ret)
        {
            printf("HI_MPI_VI_DisableChn failed with %#x\n",s32Ret);
            return HI_FAILURE;
        }
    }

    /*** Stop VI Dev ***/
    s32Ret = HI_MPI_VI_DisableDev(pstViAttr->ViDev);
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_VI_DisableDev failed with %#x\n", s32Ret);
        return HI_FAILURE;
    }
    return HI_SUCCESS;
}

static HI_S32 s32StartMIPI(VI_ATTR_S * pstViAttr)
{   
    HI_S32 fd;
	combo_dev_attr_t *pstcomboDevAttr = NULL;

	if (NULL == pstViAttr)
    {
        printf("Func %s() Line[%d], parament is null\n", __FUNCTION__, __LINE__);
        return HI_FAILURE;   
    }
	
	pstcomboDevAttr = pstViAttr->pstcomboDevAttr;
	if (NULL == pstcomboDevAttr)
    {
        printf("Func %s() Line[%d], parament is null\n", __FUNCTION__, __LINE__);
        return HI_FAILURE;   
    }
	
    /* mipi reset unrest */
    fd = open("/dev/hi_mipi", O_RDWR);
    if (fd < 0)
    {
        printf("warning: open hi_mipi dev failed\n");
        return HI_FAILURE;
    }
    if (ioctl(fd, HI_MIPI_SET_DEV_ATTR, pstcomboDevAttr))
    {
        printf("set mipi attr failed\n");
        close(fd);
        return HI_FAILURE;
    }
    close(fd);
    return HI_SUCCESS;
}

static HI_VOID* vdIspRun(HI_VOID *param)
{
    ISP_DEV IspDev = 0;
    HI_MPI_ISP_Run(IspDev);

    return HI_NULL;
}

/******************************************************************************
* funciton : ISP init
******************************************************************************/
static HI_S32 s32IspInit(VI_ATTR_S * pstViAttr)
{
    ISP_DEV IspDev = 0;
    HI_S32 s32Ret;
    ALG_LIB_S stLib;
	ISP_PUB_ATTR_S *pstPubAttr;
	ISP_WDR_MODE_S stWdrMode;
	
	if (NULL == pstViAttr)
    {
        printf("Func %s() Line[%d], parament is null\n", __FUNCTION__, __LINE__);
        return HI_FAILURE;   
    }
	IspDev = pstViAttr->IspDev;
	pstPubAttr = pstViAttr->pstIspPubAttr;
	stWdrMode.enWDRMode = pstViAttr->enWDRMode;
	
    /* 1. sensor register callback */
    s32Ret = sensor_register_callback();
    if (s32Ret != HI_SUCCESS)
    {
        printf("%s: sensor_register_callback failed with %#x!\n", \
               __FUNCTION__, s32Ret);
        return s32Ret;
    }

    /* 2. register hisi ae lib */
    stLib.s32Id = 0;
    strcpy(stLib.acLibName, HI_AE_LIB_NAME);
    s32Ret = HI_MPI_AE_Register(IspDev, &stLib);
    if (s32Ret != HI_SUCCESS)
    {
        printf("%s: HI_MPI_AE_Register failed!\n", __FUNCTION__);
        return s32Ret;
    }

    /* 3. register hisi awb lib */
    stLib.s32Id = 0;
    strcpy(stLib.acLibName, HI_AWB_LIB_NAME);
    s32Ret = HI_MPI_AWB_Register(IspDev, &stLib);
    if (s32Ret != HI_SUCCESS)
    {
        printf("%s: HI_MPI_AWB_Register failed!\n", __FUNCTION__);
        return s32Ret;
    }

    /* 4. register hisi af lib */
    stLib.s32Id = 0;
    strcpy(stLib.acLibName, HI_AF_LIB_NAME);
    s32Ret = HI_MPI_AF_Register(IspDev, &stLib);
    if (s32Ret != HI_SUCCESS)
    {
        printf("%s: HI_MPI_AF_Register failed!\n", __FUNCTION__);
        return s32Ret;
    }

    /* 5. isp mem init */
    s32Ret = HI_MPI_ISP_MemInit(IspDev);
    if (s32Ret != HI_SUCCESS)
    {
        printf("%s: HI_MPI_ISP_Init failed!\n", __FUNCTION__);
        return s32Ret;
    }

    /* 6. isp set WDR mode */
    s32Ret = HI_MPI_ISP_SetWDRMode(IspDev, &stWdrMode);    
    if (HI_SUCCESS != s32Ret)
    {
        printf("start ISP WDR failed!\n");
        return s32Ret;
    }

    /* 7. isp set pub attributes */
    s32Ret = HI_MPI_ISP_SetPubAttr(IspDev, pstPubAttr);
    if (s32Ret != HI_SUCCESS)
    {
        printf("%s: HI_MPI_ISP_SetPubAttr failed with %#x!\n", __FUNCTION__, s32Ret);
        return s32Ret;
    }

    /* 8. isp init */
    s32Ret = HI_MPI_ISP_Init(IspDev);
    if (s32Ret != HI_SUCCESS)
    {
        printf("%s: HI_MPI_ISP_Init failed!\n", __FUNCTION__);
        return s32Ret;
    }
	gbIspInited = HI_TRUE;
    /*9. isp run*/
    if (0 != pthread_create(&gs_IspPid, NULL, (void* (*)(void*))vdIspRun, NULL))
    {
        printf("%s: create isp running thread failed!\n", __FUNCTION__);
        return HI_FAILURE;
    }
    usleep(1000);

    return HI_SUCCESS;
}

/*****************************************************************************
* function : star vi dev
*****************************************************************************/
static HI_S32 s32ViStartDev(VI_ATTR_S * pstViAttr)
{
    HI_S32 s32Ret;
    VI_DEV ViDev;
	VI_DEV_ATTR_S * pstViDevAttr;
	
	if (NULL == pstViAttr)
    {
        printf("Func %s() Line[%d], parament is null\n", __FUNCTION__, __LINE__);
        return HI_FAILURE;   
    }

	ViDev = pstViAttr->ViDev;
	pstViDevAttr = pstViAttr->pstViDevAttr;
	
    s32Ret = HI_MPI_VI_SetDevAttr(ViDev, pstViDevAttr);
    if (s32Ret != HI_SUCCESS)
    {
        printf("HI_MPI_VI_SetDevAttr failed with %#x!\n", s32Ret);
        return HI_FAILURE;
    }
    
    VI_WDR_ATTR_S stWdrAttr;
    stWdrAttr.enWDRMode = pstViAttr ->enWDRMode;
    stWdrAttr.bCompress = HI_FALSE;
    s32Ret = HI_MPI_VI_SetWDRAttr(ViDev, &stWdrAttr);
    if (s32Ret)
    {
        printf("HI_MPI_VI_SetWDRAttr failed with %#x!\n", s32Ret);
        return HI_FAILURE;
    }
    
    s32Ret = HI_MPI_VI_EnableDev(ViDev);
    if (s32Ret != HI_SUCCESS)
    {
        printf("HI_MPI_VI_EnableDev failed with %#x!\n", s32Ret);
        return HI_FAILURE;
    }
    return HI_SUCCESS;
}

/*****************************************************************************
* function : star vi chn
*****************************************************************************/
static HI_S32 s32ViStartChn(VI_CHN ViChn, VI_ATTR_S * pstViAttr)
{
    HI_S32 s32Ret;
    VI_CHN_ATTR_S * pstChnAttr;
    
	if (NULL == pstViAttr)
    {
        printf("Func %s() Line[%d], parament is null\n", __FUNCTION__, __LINE__);
        return HI_FAILURE;   
    }

	pstChnAttr = pstViAttr->pstChnAttr;
    s32Ret = HI_MPI_VI_SetChnAttr(ViChn, pstChnAttr);
    if (s32Ret != HI_SUCCESS)
    {
        printf("failed with %#x!\n", s32Ret);
        return HI_FAILURE;
    }

    s32Ret = HI_MPI_VI_EnableChn(ViChn);
    if (s32Ret != HI_SUCCESS)
    {
        printf("failed with %#x!\n", s32Ret);
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}


static HI_S32 s32StartVi(VI_ATTR_S *pstViAttr)
{
    HI_S32 i, s32Ret = HI_SUCCESS;
    VI_CHN ViChn;
    HI_U32 u32ChnNum;

    if(!pstViAttr)
    {
        printf("%s: null ptr\n", __FUNCTION__);
        return HI_FAILURE;
    }
		
    /******************************************
     step 1: mipi configure
    ******************************************/
    s32Ret = s32StartMIPI(pstViAttr);
    if (HI_SUCCESS != s32Ret)
    {
        printf("%s: MIPI init failed!\n", __FUNCTION__);
        return HI_FAILURE;
    }     

    /******************************************
     step 2: configure sensor and ISP (include WDR mode).
     note: you can jump over this step, if you do not use interal isp. 
    ******************************************/
    s32Ret = s32IspInit(pstViAttr);
    if (HI_SUCCESS != s32Ret)
    {
        printf("%s: Sensor init failed!\n", __FUNCTION__);
        return HI_FAILURE;
    }

    /******************************************************
     step 3 : config & start vicap dev
    ******************************************************/
    s32Ret = s32ViStartDev(pstViAttr);
    if (HI_SUCCESS != s32Ret)
    {
        printf("%s: start vi dev[%d] failed!\n", __FUNCTION__, pstViAttr->ViDev);
        return HI_FAILURE;
    }
    
    /******************************************************
    * Step 4: config & start vicap chn (max 1) 
    ******************************************************/
    u32ChnNum = pstViAttr->u32ChnNum;
    for (i = 0; i < u32ChnNum; i++)
    {
        ViChn = i;
        s32Ret = s32ViStartChn(ViChn,pstViAttr);
        if (HI_SUCCESS != s32Ret)
        {
            printf("%s: start vi chn[%d] failed!\n", __FUNCTION__, ViChn);
            return HI_FAILURE;
        }
    }

    return s32Ret;
}

static HI_VOID viAttrInit(const IPC_VEDIO_S* pstIpcVedio,VI_ATTR_S **pstViAttr)
{
	static combo_dev_attr_t MIPI_CMOS3V3_ATTR =
	{
	    /* input mode */
	    .input_mode = INPUT_MODE_CMOS_33V,
	    {
	        
	    }
	};
	static ISP_PUB_ATTR_S ISP_PUB_ATTR =
	{
		.stWndRect = {0, 0, 1920, 1080},
		.f32FrameRate = 30,
		.enBayer = BAYER_RGGB,
	};
	
	/* the attributes of a VI device */
	static VI_DEV_ATTR_S VI_DEV_ATTR =
	{
	    .enIntfMode = VI_MODE_DIGITAL_CAMERA,	/* Interface mode */
	    .enWorkMode = VI_WORK_MODE_1Multiplex,	/*1-, 2-, or 4-channel multiplexed work mode */
	    .au32CompMask = {0x3FF00000,    0x0},	/* r_mask    g_mask    b_mask*/
	    .enScanMode = VI_SCAN_PROGRESSIVE,		/* Input scanning mode (progressive or interlaced) */
	    .s32AdChnId = {-1, -1, -1, -1},			/* AD channel ID. Typically, the default value -1 is recommended */
	    
	    /* The below members must be configured in BT.601 mode or DC mode and are invalid in other modes */
	    .enDataSeq = VI_INPUT_DATA_YUYV,		/* Input data sequence (only the YUV format is supported) */
	    .stSynCfg =
	    {
			VI_VSYNC_PULSE, 		/*Vsync*/
			VI_VSYNC_NEG_HIGH, 		/*VsyncNeg*/
			VI_HSYNC_VALID_SINGNAL,	/*Hsync*/
			VI_HSYNC_NEG_HIGH,		/*HsyncNeg*/
			VI_VSYNC_VALID_SINGAL,	/*VsyncValid*/
			VI_VSYNC_VALID_NEG_HIGH,/*VsyncValidNeg*/
			/*The below TimingBlank only for BT.601*/
		    /*hsync_hfb    hsync_act    hsync_hhb*/
		    {0,            1920,        0,
		    /*vsync0_vhb vsync0_act vsync0_hhb*/
		     0,            1080,        0,
		    /*vsync1_vhb vsync1_act vsync1_hhb*/
		     0,            0,            0}
		},
	    .enDataPath = VI_PATH_ISP,			/* ISP enable or bypass */
	    .enInputDataType = VI_DATA_TYPE_RGB,/* RGB: CSC-709 or CSC-601, PT YUV444 disable; YUV: default yuv CSC coef PT YUV444 enable. */
	    .bDataRev = HI_FALSE,				/* Data Reverse */
	    .stDevRect = {0, 0, 1920, 1080}		/* Dev capture rect */
	};

	/* the attributes of a VI channel */
	static VI_CHN_ATTR_S VI_CHN_ATTR = 
	{
		.stCapRect = {0, 0, 1920, 1080},
		.stDestSize	= {1920, 1080},
		.enCapSel = VI_CAPSEL_BOTH,
		.enPixFormat = PIXEL_FORMAT_YUV_SEMIPLANAR_420,
		.enCompressMode = COMPRESS_MODE_NONE,
		.bMirror = HI_FALSE,
		.bFlip = HI_FALSE,
		.s32SrcFrameRate = -1,
		.s32DstFrameRate = -1
	};

	static VI_ATTR_S VI_ATTR = 
	{
		.IspDev = 0,
		.ViDev = 0,
		.u32ChnNum = 1,
		.enWDRMode = WDR_MODE_NONE,
		.pstcomboDevAttr = &MIPI_CMOS3V3_ATTR,
		.pstIspPubAttr = &ISP_PUB_ATTR,
		.pstViDevAttr = &VI_DEV_ATTR,
		.pstChnAttr = &VI_CHN_ATTR
	};
	/*sync isp pub attr*/
	VI_ATTR.pstIspPubAttr->stWndRect.u32Width = pstIpcVedio->astStreamFmt[0].stSize.u32Width;
	VI_ATTR.pstIspPubAttr->stWndRect.u32Height = pstIpcVedio->astStreamFmt[0].stSize.u32Height;
	VI_ATTR.pstIspPubAttr->f32FrameRate = 25;//pstIpcVedio->astStreamFmt[0].f32FrameRate;
	/*sync vi dev attr*/
	VI_ATTR.pstViDevAttr->stDevRect.u32Width = pstIpcVedio->astStreamFmt[0].stSize.u32Width;
	VI_ATTR.pstViDevAttr->stDevRect.u32Height = pstIpcVedio->astStreamFmt[0].stSize.u32Height;
	/*sync vi chn attr*/
	VI_ATTR.pstChnAttr->stCapRect.u32Width = pstIpcVedio->astStreamFmt[0].stSize.u32Width;
	VI_ATTR.pstChnAttr->stCapRect.u32Height = pstIpcVedio->astStreamFmt[0].stSize.u32Height;
	VI_ATTR.pstChnAttr->stDestSize.u32Width = pstIpcVedio->astStreamFmt[0].stSize.u32Width;
	VI_ATTR.pstChnAttr->stDestSize.u32Height = pstIpcVedio->astStreamFmt[0].stSize.u32Height;
	VI_ATTR.pstChnAttr->enPixFormat = pstIpcVedio->e_pixFmt;
	/*sync wdr mode*/
	VI_ATTR.enWDRMode = pstIpcVedio->e_wdrMode;
	
	*pstViAttr = &VI_ATTR;
}
HI_S32 VI_init(const IPC_VEDIO_S* pstIpcVedio)
{
	HI_S32 s32Ret = HI_SUCCESS;
	if(!pstIpcVedio)
    {
        printf("%s: null ptr\n", __FUNCTION__);
        return HI_FAILURE;
    }
	viAttrInit(pstIpcVedio,&g_pstViAttr);
	s32Ret = s32StartVi(g_pstViAttr);
	return s32Ret;
}

HI_S32 VI_exit(void)
{
	HI_S32 s32Ret = HI_SUCCESS;
	s32Ret = s32StopVi(g_pstViAttr);
	vdIspStop(g_pstViAttr);
	return s32Ret;
}

