#include "hi_osal.h"

#include "sensor.h"
#include "isp_ext.h"
#include "i2c_drv.h"
#include "ssp_drv.h"

struct sensor_device *g_stSensorDev[SENSOR_NUM] = {0};

static int sensor_drv_init(struct sensor_device *sensor_dev)
{
	SENSOR_CTRL_BUS_S *pstCtrlBus;
	HI_S32 ret = 0;
	
	pstCtrlBus = &sensor_dev->stCtrlBus;
	switch(pstCtrlBus->enBusType)
	{
		case SENSOR_CTRL_BUS_I2C:
			ret = i2c_drv_init(pstCtrlBus->u32BusNum);
			break;
		case SENSOR_CTRL_BUS_SSP:
			ret = ssp_drv_init(pstCtrlBus->u32BusNum);
			break;
		default:
			break;
	}
	
	return ret;
}

static void sensor_drv_exit(struct sensor_device *sensor_dev)
{
	SENSOR_CTRL_BUS_S *pstCtrlBus;
	
	pstCtrlBus = &sensor_dev->stCtrlBus;
	switch(pstCtrlBus->enBusType)
	{
		case SENSOR_CTRL_BUS_I2C:
			i2c_drv_exit(pstCtrlBus->u32BusNum);	
			break;
		case SENSOR_CTRL_BUS_SSP:
			ssp_drv_exit(pstCtrlBus->u32BusNum);
			break;
		default:
			break;
	}
	
	return;
}

static HI_S32 isp_register_callback(struct sensor_device *sensor_dev)
{
	ISP_BUS_CALLBACK_S stBusCb = {0};
	
	SENSOR_CTRL_BUS_S *pstCtrlBus;
	SENSOR_CTRL_OPS_S *pstCtrlOps;

	pstCtrlBus = &sensor_dev->stCtrlBus;
	pstCtrlOps = &sensor_dev->stCtrlOps;
	
	switch(pstCtrlBus->enBusType)
	{
		case SENSOR_CTRL_BUS_I2C:
			i2c_get_ops(pstCtrlBus->u32BusNum, pstCtrlOps);		
			break;
		case SENSOR_CTRL_BUS_SSP:
			ssp_get_ops(pstCtrlBus->u32BusNum, pstCtrlOps);		
			break;
		default:
			break;
	}

	stBusCb.pfnISPWriteSensorData = pstCtrlOps->write;
	stBusCb.pfnISPReadSensorData  = pstCtrlOps->read;
	if (CKFN_ISP_RegisterBusCallBack())
	{
		CALL_ISP_RegisterBusCallBack(sensor_dev->s32IspBindDev, &stBusCb);
	}
	else
	{
		osal_printk("register senor_callback to isp failed!\n");
		return -1;
	}

	return 0;
}

HI_S32 sensor_dev_init(HI_S32 s32SensorIndex)
{
	struct sensor_device *sensor_dev;
	HI_S32 ret;
	
	sensor_dev = g_stSensorDev[s32SensorIndex];
	if (NULL == sensor_dev)
	{
		osal_printk("%s(%d): sensor_dev is NULL.\n", __func__, __LINE__);
		return -1;
	}

	ret = sensor_drv_init(sensor_dev);
	if (ret)
	{
		return ret;
	}

	ret = isp_register_callback(sensor_dev);
	if (ret)
	{
		sensor_drv_exit(sensor_dev);
		return ret;
	}

	return 0;
}

HI_VOID sensor_dev_exit(HI_S32 s32SensorIndex)
{
	struct sensor_device *sensor_dev;
	
	sensor_dev = g_stSensorDev[s32SensorIndex];
	if (NULL == sensor_dev)
	{
		osal_printk("%s(%d): sensor_dev is NULL.\n", __func__, __LINE__);
		return;
	}
	
	sensor_dev->stCtrlOps.read = NULL;
	sensor_dev->stCtrlOps.write = NULL;
	
	sensor_drv_exit(sensor_dev);
	
	g_stSensorDev[s32SensorIndex] = NULL;
}
