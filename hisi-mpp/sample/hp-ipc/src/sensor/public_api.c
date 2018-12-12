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
#include "ps3210k_api.h"
#include "public_api.h"

static pthread_t public_monitor_threadID = -1;
public_sensor_interface_st * public_sensor_interface = NULL;

static IRCUT_SWITCH_DIRECTION ircutDirectioin = NORMAL_DIRECTION;
static IRCUT_MODE ircutMode = IRCUT_DAY;
static IRCUT_SWITCH_MODE ircut_switch_mode = IRCUT_SYN_INFRARED;
static CAMERA_SCENE public_camera_scene = SCENE_DAY;

#define CHECK_INTERFACE(interface)\
do{\
	if(interface == NULL)\
	{\
		printf("not register sensor %s\n",#interface);\
		exit(1);\
	}\
}while(0);

#define REGISTER_SENSOR(interface) \
do{\
	public_sensor_interface = &(interface);\
}while(0);

static void * pubic_monitor(void *p);





int public_init(void)
{
	ircutDirectioin = NORMAL_DIRECTION;
	ircutMode = IRCUT_DAY;
	ircut_switch_mode = IRCUT_SYN_INFRARED;
	public_camera_scene = SCENE_DAY;
	REGISTER_SENSOR(PS3210k_sensor);
	pthread_create(&public_monitor_threadID, NULL, pubic_monitor, NULL);
	return 0;
}
int public_exit(void)
{
	CHECK_INTERFACE(public_sensor_interface->sensor_exit);
	public_sensor_interface->sensor_exit();
	return 0;
}
void * pubic_monitor(void *p)
{
	CHECK_INTERFACE(public_sensor_interface->sensor_init);
	public_sensor_interface->sensor_init();
	public_ircut_init();

	while(1)
	{	

		usleep(200000);
		public_auto_scene_control();
		
		
	}

}
int public_scene_switch(CAMERA_SCENE scene_t)
{
	ISP_DEV IspDev = 0;
	ISP_SATURATION_ATTR_S stSatAttr;
	if(SCENE_DAY == scene_t)
	{
		public_ircut_switch(IRCUT_DAY);
		HI_MPI_ISP_GetSaturationAttr(IspDev, &stSatAttr);
		stSatAttr.enOpType = OP_TYPE_AUTO;
		stSatAttr.stManual.u8Saturation = 0;
		HI_MPI_ISP_SetSaturationAttr(IspDev, &stSatAttr);
	}
	else if(SCENE_NIGHT == scene_t)
	{
		public_ircut_switch(IRCUT_NIGHT);
		HI_MPI_ISP_GetSaturationAttr(IspDev, &stSatAttr);
		stSatAttr.enOpType = OP_TYPE_MANUAL;
		stSatAttr.stManual.u8Saturation = 0;
		HI_MPI_ISP_SetSaturationAttr(IspDev, &stSatAttr);
	}
	return 0;
}

CAMERA_SCENE public_get_scene(IRCUT_SWITCH_MODE ircut_sw_m)
{
	CAMERA_SCENE camera_scene = SCENE_DAY;
	int val_t = 0;
	CHECK_INTERFACE(public_sensor_interface->sensor_get_infraredTypeOrYAVG);
	val_t = public_sensor_interface->sensor_get_infraredTypeOrYAVG(ircut_sw_m);
	if( IRCUT_SYN_INFRARED == ircut_sw_m)
	{
		camera_scene = (val_t == 0) ? SCENE_DAY : SCENE_NIGHT;
	}
	else if(IRCUT_SWITCH_AUTO == ircut_sw_m)
	{
		camera_scene = 	SCENE_DAY;
	}
	return camera_scene;
}
int public_auto_scene_control(void)
{
	CAMERA_SCENE scene_t;
	static int ircut_day_time = 0;
	static int ircut_night_time = 0;
	scene_t = public_get_scene(ircut_switch_mode);
	if((SCENE_DAY == scene_t) && (SCENE_DAY != public_camera_scene))
	{
		ircut_day_time++;
		if(5 <= ircut_day_time)
		{
			ircut_day_time = 0;
			public_camera_scene = SCENE_DAY;
			public_scene_switch(public_camera_scene);
			
			
		}
		ircut_night_time = 0;
	}
	else if((SCENE_NIGHT == scene_t) && (SCENE_NIGHT != public_camera_scene))
	{
		
		ircut_night_time++;
		if(5 <= ircut_night_time)
		{
			ircut_night_time = 0;
			public_camera_scene = SCENE_NIGHT;
			public_scene_switch(public_camera_scene);
		}
		ircut_day_time = 0;
	}
	else
	{
		ircut_day_time = 0;
		ircut_night_time = 0;
	}
	return 0;
}

int public_ircut_init(void)
{
	public_ircut_switch(IRCUT_NIGHT);
	sleep(1);
	public_ircut_switch(IRCUT_DAY);
	return 0;
}

int public_ircut_switch(int mode)
{
	CHECK_INTERFACE(public_sensor_interface->sensor_ircut_witch);
	if(NORMAL_DIRECTION == ircutDirectioin)
	{
		ircutMode = mode;
	}
	else
	{
		ircutMode = !mode;
	}
	public_sensor_interface->sensor_ircut_witch(ircutMode);
	return 0;
}
