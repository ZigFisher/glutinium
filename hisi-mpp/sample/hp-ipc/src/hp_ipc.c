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

#include "hp_ipc.h"
#include "hp_sys.h"
#include "hp_vi.h"
#include "hp_vpss.h"
#include "hp_venc.h"


HI_S32 IPC_Init(HI_U32 u32StreamNum, FARMAT_S *stFmt_a)
{
	HI_S32 i,s32Ret;
	if((STREAM_MAX_NUM < u32StreamNum) || (NULL == stFmt_a))
	{
		printf("Invalid parament\n");
		return HI_FAILURE;
	}
	g_stIpcVedio.e_chip = HI3518EV200;
	g_stIpcVedio.e_sensor_type = SENSOR_PIXELPLUS_PS3210K;
	g_stIpcVedio.e_wdrMode = WDR_MODE_NONE;
	g_stIpcVedio.e_pixFmt = PIXEL_FORMAT_YUV_SEMIPLANAR_420;
	g_stIpcVedio.u32StreamNum = u32StreamNum;
	for(i = 0; i < u32StreamNum; i++)
	{
		s32GetPicSize(stFmt_a[i].e_Size,&(g_stIpcVedio.astStreamFmt[i].stSize));
		g_stIpcVedio.astStreamFmt[i].e_Size = stFmt_a[i].e_Size;
		g_stIpcVedio.astStreamFmt[i].f32FrameRate = stFmt_a[i].f32FrameRate;
		g_stIpcVedio.astStreamFmt[i].e_payload = PT_H264;
		g_stIpcVedio.astStreamFmt[i].e_RCMode  = RC_MODE_CBR;
	}


    s32Ret = mpi_sys_init(&g_stIpcVedio);
    if (HI_SUCCESS != s32Ret)
    {
        printf("system init failed with %d!\n", s32Ret);
        goto END_EXIT_0;
    }
	
    s32Ret = VI_init(&g_stIpcVedio);
    if (HI_SUCCESS != s32Ret)
    {
        printf("start vi failed!\n");
        goto END_EXIT_1;
    }
	
    s32Ret = VPSS_init(&g_stIpcVedio);
	if (HI_SUCCESS != s32Ret)
    {
        printf("start vpss failed!\n");
        goto END_EXIT_2;
    }
	
	s32Ret = mpi_sys_Bind(&g_stIpcVedio);
	if (HI_SUCCESS != s32Ret)
    {
        printf("Bind failed!\n");
        goto END_EXIT_3;
    }
	
	s32Ret = VENC_init(&g_stIpcVedio);
	if (HI_SUCCESS != s32Ret)
    {
        printf("start venc failed!\n");
        goto END_EXIT_4;
    }
	
	return HI_SUCCESS;
	
END_EXIT_4:
	VENC_exit();
END_EXIT_3:
	mpi_sys_UnBind(&g_stIpcVedio);
END_EXIT_2:
	VPSS_exit();
END_EXIT_1:
	VI_exit();
END_EXIT_0:
	mpi_sys_exit();
	exit(0);
	return HI_SUCCESS;
}

HI_S32 IPC_exit(void)
{	
	VENC_exit();
	mpi_sys_UnBind(&g_stIpcVedio);
	VPSS_exit();
	VI_exit();
	mpi_sys_exit();
	return HI_SUCCESS;
}


