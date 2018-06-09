#ifndef __SENSOR_H__
#define __SENSOR_H__

#include "hi_type.h"

#define SENSOR_NUM 1

typedef enum hiSENSOR_CTRL_BUS_TYPE_E {
	SENSOR_CTRL_BUS_I2C,
	SENSOR_CTRL_BUS_SSP,

	SENSOR_CTRL_BUS_BUTT,
}SENSOR_CTRL_BUS_TYPE_E;

typedef struct hiSENSOR_CTRL_BUS_S {
	SENSOR_CTRL_BUS_TYPE_E enBusType;
	HI_U32 u32BusNum;
}SENSOR_CTRL_BUS_S;

typedef struct hiSENSOR_CTRL_OPS_S {
	HI_S32 (*write)(HI_VOID *pSensorData);
	HI_S32 (*read)(HI_VOID *pSensorData);
}SENSOR_CTRL_OPS_S;

struct sensor_device{
	HI_S32 s32SensorIndex;
	HI_S32 s32IspBindDev;
	SENSOR_CTRL_BUS_S stCtrlBus;
	SENSOR_CTRL_OPS_S stCtrlOps;
};

extern struct sensor_device *g_stSensorDev[SENSOR_NUM];

HI_S32 sensor_dev_init(HI_S32 s32SensorIndex);
HI_VOID sensor_dev_exit(HI_S32 s32SensorIndex);

#endif
