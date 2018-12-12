#ifndef __public_API_H__
#define __public_API_H__ 

#ifdef __cplusplus
extern "C" {
#endif


#define SysRegRead 	HI_MPI_SYS_GetReg
#define SysRegWrite	HI_MPI_SYS_SetReg


typedef enum IRCUT_SWITCH_DIRECTION
{
	NORMAL_DIRECTION = 0,
	CONTRARY_DIRECTION
}IRCUT_SWITCH_DIRECTION;

typedef enum IRCUT_MODE
{
	IRCUT_NIGHT = 0,
	IRCUT_DAY,
}IRCUT_MODE;

typedef enum IRCUT_SWITCH_MODE
{
	IRCUT_SYN_INFRARED = 0,
	IRCUT_SWITCH_AUTO
}IRCUT_SWITCH_MODE;

typedef enum CAMERA_SCENE
{
	SCENE_DAY = 0,
	SCENE_NIGHT
}CAMERA_SCENE;


typedef struct _public_sensor_interface
{
	int (* sensor_init)(void);
	int (* sensor_exit)(void);
	int (* sensor_ircut_witch)(int mode);
	int (* sensor_get_infraredTypeOrYAVG)(IRCUT_SWITCH_MODE IRCut_switch_mode);
	
}public_sensor_interface_st;


extern int public_init(void);
extern int public_exit(void);
extern int public_ircut_switch(int mode);



#ifdef __cplusplus
}
#endif

#endif

