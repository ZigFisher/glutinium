#include <string.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <signal.h>
#include <sys/mman.h>
#include <linux/fb.h>
#include <assert.h>

#include "hp_common.h"
#include "camera.h"
#include "public_api.h"
#include "rtsp_server.h"
#include "hp_ipc.h"


int camera_init(void)
{
	FARMAT_S stFmt_a[3] = 
	{
		{PIC_HD1080,16},
		{PIC_VGA,30},
		{PIC_QVGA,30}
	};
	IPC_Init(1,stFmt_a);
	public_init();
	RtspServer_init();
	FPN_init(NULL);
	return 0;
}
int camera_exit(void)
{
	
	IPC_exit();
	public_exit();
	RtspServer_exit();
	return 0;
}

int main()
{
	camera_init();
	getchar();
	getchar();
	camera_exit();
	return 0;
}

int camera_set_Ircut(int mode)
{
	
	int ret = -1;
	ret = public_ircut_switch(mode);
	return ret;
}

