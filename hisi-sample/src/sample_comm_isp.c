/******************************************************************************
  Some simple Hisilicon Hi35xx isp functions.

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
#include "sample_comm.h"

static pthread_t gs_IspPid = 0;
static HI_BOOL gbIspInited = HI_FALSE;

#include <dlfcn.h>
#include <inttypes.h>

int (*sensor_register_callback_fn)(void);
int (*sensor_unregister_callback_fn)(void);
void *libsns_so = NULL;


HI_S32 tryLoadLibrary(const char *path) {
    printf("try to load: %s\n", path);
    libsns_so = dlopen(path, RTLD_NOW | RTLD_GLOBAL);
    printf("libsns_so 0x%016" PRIXPTR "\n", (uintptr_t) libsns_so);
    if (libsns_so == NULL) {
        //printf("dlopen \"%s\" error:\n", path);
        printf("dlopen \"%s\" error: %s\n", path, dlerror());
        return HI_FAILURE;
    }
    return HI_SUCCESS;
}
HI_S32 LoadSensorLibrary(const char *libsns_name) {
    char path[250];
    sprintf(path, "%s", libsns_name);
    if (tryLoadLibrary(path) != HI_SUCCESS) {
        sprintf(path, "./%s", libsns_name);
        if (tryLoadLibrary(path) != HI_SUCCESS) {
            sprintf(path, "/usr/lib/%s", libsns_name);
            if (tryLoadLibrary(path) != HI_SUCCESS) {
                return HI_FAILURE;
            }
        }
    }
    sensor_register_callback_fn = dlsym(libsns_so, "sensor_register_callback");
    sensor_unregister_callback_fn = dlsym(libsns_so, "sensor_unregister_callback");
    return HI_SUCCESS;
}
HI_VOID UnloadSensorLibrary() {
    dlclose(libsns_so);
    libsns_so = NULL;
}


/******************************************************************************
* funciton : ISP init
******************************************************************************/
HI_S32 SAMPLE_COMM_ISP_Init(WDR_MODE_E  enWDRMode, SAMPLE_VI_MODE_E enMode)
{
    ISP_DEV IspDev = 0;
    HI_S32 s32Ret;
    ISP_PUB_ATTR_S stPubAttr;
    ALG_LIB_S stLib;
    
#if 0   
    /* 0. set cmos iniparser file path */
    s32Ret = sensor_set_inifile_path("configs/");
    if (s32Ret != HI_SUCCESS)
    {
        printf("%s: set cmos iniparser file path failed with %#x!\n", \
               __FUNCTION__, s32Ret);
        return s32Ret;
    }
#endif

    /* 1. sensor register callback */
    s32Ret = sensor_register_callback_fn();
    if (s32Ret != HI_SUCCESS)
    {
        printf("%s: sensor_register_callback_fn failed with %#x!\n", \
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
    ISP_WDR_MODE_S stWdrMode;
    stWdrMode.enWDRMode  = enWDRMode;
    s32Ret = HI_MPI_ISP_SetWDRMode(0, &stWdrMode);    
    if (HI_SUCCESS != s32Ret)
    {
        printf("start ISP WDR failed!\n");
        return s32Ret;
    }

    /* 7. isp set pub attributes */
    /* note : different sensor, different ISP_PUB_ATTR_S define.
              if the sensor you used is different, you can change
              ISP_PUB_ATTR_S definition */
              
    switch(enMode)
    {
        case APTINA_9M034_DC_720P_30FPS:
        case APTINA_AR0130_DC_720P_30FPS:
            stPubAttr.enBayer               = BAYER_GRBG;
            stPubAttr.f32FrameRate          = 30;
            stPubAttr.stWndRect.s32X        = 0;
            stPubAttr.stWndRect.s32Y        = 0;
            stPubAttr.stWndRect.u32Width    = 1280;
            stPubAttr.stWndRect.u32Height   = 720;
            break;

		case SONY_IMX222_DC_1080P_30FPS:
			stPubAttr.enBayer               = BAYER_RGGB;
            stPubAttr.f32FrameRate          = 30;
            stPubAttr.stWndRect.s32X        = 200;
            stPubAttr.stWndRect.s32Y        = 20;
            stPubAttr.stWndRect.u32Width    = 1920;
            stPubAttr.stWndRect.u32Height   = 1080;
            break;

        case SONY_IMX222_DC_720P_30FPS:
			stPubAttr.enBayer               = BAYER_RGGB;
            stPubAttr.f32FrameRate          = 30;
            stPubAttr.stWndRect.s32X        = 200;
            stPubAttr.stWndRect.s32Y        = 20;
            stPubAttr.stWndRect.u32Width    = 1280;
            stPubAttr.stWndRect.u32Height   = 720;
            break;

        case APTINA_AR0230_HISPI_1080P_30FPS:
            stPubAttr.enBayer               = BAYER_GRBG;
            stPubAttr.f32FrameRate          = 30;
            stPubAttr.stWndRect.s32X        = 0;
            stPubAttr.stWndRect.s32Y        = 0;
            stPubAttr.stWndRect.u32Width    = 1920;
            stPubAttr.stWndRect.u32Height   = 1080;
            break;

        case PANASONIC_MN34222_MIPI_1080P_30FPS:
            stPubAttr.enBayer               = BAYER_GRBG;
            stPubAttr.f32FrameRate          = 30;
            stPubAttr.stWndRect.s32X        = 0;
            stPubAttr.stWndRect.s32Y        = 0;
            stPubAttr.stWndRect.u32Width    = 1920;
            stPubAttr.stWndRect.u32Height   = 1080;
            break;

        case OMNIVISION_OV9712_DC_720P_30FPS:
            stPubAttr.enBayer               = BAYER_BGGR;
            stPubAttr.f32FrameRate          = 30;
            stPubAttr.stWndRect.s32X        = 0;
            stPubAttr.stWndRect.s32Y        = 0;
            stPubAttr.stWndRect.u32Width    = 1280;
            stPubAttr.stWndRect.u32Height   = 720;
            break;
            
        case OMNIVISION_OV9732_DC_720P_30FPS:
            stPubAttr.enBayer               = BAYER_BGGR;
            stPubAttr.f32FrameRate          = 30;
            stPubAttr.stWndRect.s32X        = 0;
            stPubAttr.stWndRect.s32Y        = 0;
            stPubAttr.stWndRect.u32Width    = 1280;
            stPubAttr.stWndRect.u32Height   = 720;
            break;

        case OMNIVISION_OV9750_MIPI_720P_30FPS:
        case OMNIVISION_OV9752_MIPI_720P_30FPS:
            stPubAttr.enBayer               = BAYER_BGGR;
            stPubAttr.f32FrameRate          = 30;
            stPubAttr.stWndRect.s32X        = 0;
            stPubAttr.stWndRect.s32Y        = 0;
            stPubAttr.stWndRect.u32Width    = 1280;
            stPubAttr.stWndRect.u32Height   = 720;
            break;

        case OMNIVISION_OV2718_MIPI_1080P_25FPS:
            stPubAttr.enBayer               = BAYER_BGGR;
            stPubAttr.f32FrameRate          = 25;
            stPubAttr.stWndRect.s32X        = 0;
            stPubAttr.stWndRect.s32Y        = 0;
            stPubAttr.stWndRect.u32Width    = 1920;
            stPubAttr.stWndRect.u32Height   = 1080;
            break;
            
        default:
            stPubAttr.enBayer      = BAYER_GRBG;
            stPubAttr.f32FrameRate          = 30;
            stPubAttr.stWndRect.s32X        = 0;
            stPubAttr.stWndRect.s32Y        = 0;
            stPubAttr.stWndRect.u32Width    = 1920;
            stPubAttr.stWndRect.u32Height   = 1080;
            break;
    }

    s32Ret = HI_MPI_ISP_SetPubAttr(IspDev, &stPubAttr);
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

    return HI_SUCCESS;
}

HI_VOID* Test_ISP_Run(HI_VOID *param)
{
    ISP_DEV IspDev = 0;
    HI_MPI_ISP_Run(IspDev);

    return HI_NULL;
}

/******************************************************************************
* funciton : ISP Run
******************************************************************************/
HI_S32 SAMPLE_COMM_ISP_Run()
{
#if 1

#if (CHIP_ID == CHIP_HI3516C_V200)
    if (0 != pthread_create(&gs_IspPid, 0, (void* (*)(void*))Test_ISP_Run, NULL))
    {
        printf("%s: create isp running thread failed!\n", __FUNCTION__);
        return HI_FAILURE;
    }
    usleep(1000);
#else
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setstacksize(&attr, 4096 * 1024);
    if (0 != pthread_create(&gs_IspPid, &attr, (void* (*)(void*))Test_ISP_Run, NULL))
    {
        printf("%s: create isp running thread failed!\n", __FUNCTION__);
        pthread_attr_destroy(&attr);
        return HI_FAILURE;
    }
    usleep(1000);
    pthread_attr_destroy(&attr);
#endif

#else
	/* configure thread priority */
	if (1)
	{
		#include <sched.h>

		pthread_attr_t attr;
		struct sched_param param;
		int newprio = 50;

		pthread_attr_init(&attr);

		if (1)
		{
			int policy = 0;
			int min, max;

			pthread_attr_getschedpolicy(&attr, &policy);
			printf("-->default thread use policy is %d --<\n", policy);

			pthread_attr_setschedpolicy(&attr, SCHED_RR);
			pthread_attr_getschedpolicy(&attr, &policy);
			printf("-->current thread use policy is %d --<\n", policy);

			switch (policy)
			{
				case SCHED_FIFO:
					printf("-->current thread use policy is SCHED_FIFO --<\n");
					break;

				case SCHED_RR:
					printf("-->current thread use policy is SCHED_RR --<\n");
					break;

				case SCHED_OTHER:
					printf("-->current thread use policy is SCHED_OTHER --<\n");
					break;

				default:
					printf("-->current thread use policy is UNKNOW --<\n");
					break;
			}

			min = sched_get_priority_min(policy);
			max = sched_get_priority_max(policy);

			printf("-->current thread policy priority range (%d ~ %d) --<\n", min, max);
		}

		pthread_attr_getschedparam(&attr, &param);

		printf("-->default isp thread priority is %d , next be %d --<\n", param.sched_priority, newprio);
		param.sched_priority = newprio;
		pthread_attr_setschedparam(&attr, &param);

		if (0 != pthread_create(&gs_IspPid, &attr, (void* (*)(void*))HI_MPI_ISP_Run, NULL))
		{
			printf("%s: create isp running thread failed!\n", __FUNCTION__);
			return HI_FAILURE;
		}

		pthread_attr_destroy(&attr);
	}
#endif

    return HI_SUCCESS;
}

/******************************************************************************
* funciton : change between linear and wdr mode
******************************************************************************/
HI_S32 SAMPLE_COMM_ISP_ChangeSensorMode(HI_U8 u8Mode)
{
    ISP_DEV IspDev = 0;
    HI_S32 s32Ret;
    
    ISP_WDR_MODE_S stWDRMode;
    stWDRMode.enWDRMode = u8Mode;
    s32Ret = HI_MPI_ISP_SetWDRMode(IspDev, &stWDRMode);
    if (s32Ret != HI_SUCCESS)
    {
        printf("%s: HI_MPI_ISP_SetWDRMode failed with %#x!\n", 
               __FUNCTION__, s32Ret);
        return s32Ret;
    }
    
    return HI_SUCCESS;
}


/******************************************************************************
* funciton : stop ISP, and stop isp thread
******************************************************************************/
HI_VOID SAMPLE_COMM_ISP_Stop()
{
    ISP_DEV IspDev = 0;
    HI_S32 s32Ret;
    ALG_LIB_S stLib;

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
    
    /* unregister hisi af lib */
    stLib.s32Id = 0;
    strcpy(stLib.acLibName, HI_AF_LIB_NAME);
    s32Ret = HI_MPI_AF_UnRegister(IspDev, &stLib);
    if (s32Ret != HI_SUCCESS)
    {
        printf("%s: HI_MPI_AF_UnRegister failed!\n", __FUNCTION__);
        return;
    }

    /* unregister hisi awb lib */
    stLib.s32Id = 0;
    strcpy(stLib.acLibName, HI_AWB_LIB_NAME);
    s32Ret = HI_MPI_AWB_UnRegister(IspDev, &stLib);
    if (s32Ret != HI_SUCCESS)
    {
        printf("%s: HI_MPI_AWB_UnRegister failed!\n", __FUNCTION__);
        return;
    }

    /* unregister hisi ae lib */
    stLib.s32Id = 0;
    strcpy(stLib.acLibName, HI_AE_LIB_NAME);
    s32Ret = HI_MPI_AE_UnRegister(IspDev, &stLib);
    if (s32Ret != HI_SUCCESS)
    {
        printf("%s: HI_MPI_AE_UnRegister failed!\n", __FUNCTION__);
        return;
    }

    /* sensor unregister callback */
    s32Ret = sensor_unregister_callback_fn();
    if (s32Ret != HI_SUCCESS)
    {
        printf("%s: sensor_unregister_callback_fn failed with %#x!\n", \
               __FUNCTION__, s32Ret);
        return;
    }
    
    return;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

