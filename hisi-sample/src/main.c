/*
*Lucifer
*QQ:398031055
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <netinet/in.h>
#include <pthread.h>

#include "rtspservice.h"
#include "rtputils.h"
#include "ringfifo.h"
#include "sample_comm.h"

extern int g_s32Quit ;

/**************************************************************************************************
**
**
**
**************************************************************************************************/
extern void * SAMPLE_VENC_1080P_CLASSIC(void *p);
int main(int argc, char *argv[])
{
	SOC_TYPE socType;
	HI_S32 s32Ret = initSoc(argc, argv, &socType);
	if (s32Ret == HI_FAILURE) return EXIT_FAILURE;

	SAMPLE_VI_MODE_E enMode;
	s32Ret = initSensor(argc, argv, &enMode);
	if (s32Ret == HI_FAILURE) return EXIT_FAILURE;

	signal(SIGINT, SAMPLE_VENC_HandleSig);
	signal(SIGTERM, SAMPLE_VENC_HandleSig);

	int s32MainFd,temp;
	struct timespec ts = { 2, 0 };
	pthread_t id;
	//分配环形缓冲区，总共32个，每个大小为size
	ringmalloc(720*576);
	printf("RTSP server START\n");
//设置服务器信息全局变量，设置内容保存在stPrefs结构体
	PrefsInit();/////////////////////////////////////////////////////未引用头文件
	printf("listen for client connecting...\n");
	signal(SIGINT, IntHandl);
	//创建、设置、绑定、监听套接字，并将套接字设为非阻塞模式
	s32MainFd = tcp_listen(SERVER_RTSP_PORT_DEFAULT);
	/*创建线程schedule_do:nanosleep(&ts, NULL)让出CPU，超时时间33333纳秒=33微秒到重新获得CPU。获得CPU时，在stop_schedule=0的情况下，循环
ringgetringget(&ringinfo)来取得3518里的H264数据然后用sched[i].play_action=RtpSend函数发送出去*/
	if (ScheduleInit() == ERR_FATAL)
	{
		fprintf(stderr,"Fatal: Can't start scheduler %s, %i \nServer is aborting.\n", __FILE__, __LINE__);
		return 0;
	}
	//设置rtp端口值,前面tcp_listen参数是RTSP端口，与RTP端口不同
	RTP_port_pool_init(RTP_DEFAULT_PORT);/////////////////////////////////////////////////////未引用头文件
	//SAMPLE_VENC_1080P_CLASSIC与H264编码差不多，只不过不是保存在文件里，而是通过网络直接发送
	//出去，里面有个线程去处理视频流，读视频流时也会休眠
	// 4个线程:1.main:负责切换所有线程，nanosleep让出cpu，超时唤醒
	// 2.schedule_do:服务器取视频数据发 送线程，nanosleep让出cpu，超时唤醒
	// 3.SAMPLE_VENC_1080P_CLASSIC:h264编码线程，sleep(1)让出CPU，但超时唤醒又死循环sleep(1)，基本上可以不考虑这个线程
	// 4.SAMPLE_COMM_VENC_GetVencStreamProc:读取3518的H264数据并保存到环形缓冲区线程，select休眠，有数据或超时唤醒
	struct hithreadArgs_tag *args = malloc(sizeof(struct hithreadArgs_tag));
	args->socType = socType;
	args->enMode = enMode;

	pthread_create(&id,NULL,SAMPLE_VENC_1080P_CLASSIC, (void *)args);
	while (!g_s32Quit)
	{
		nanosleep(&ts, NULL);//超时时间为ts,NULL表示不记录剩余时间
		EventLoop(s32MainFd);//这个s32MainFd是由上面的tcp_listen创建且是无阻塞模式
	}
	sleep(2);
	ringfree();
	printf("The Server quit!\n");
	free(args);

	return 0;
}

