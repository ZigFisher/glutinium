#ifndef __SSP_DRV_H__
#define __SSP_DRV_H__

#include "sensor.h"

HI_VOID ssp_get_ops(HI_U32 u32BusNum, SENSOR_CTRL_OPS_S *pstCtrlOps);

HI_S32 ssp_drv_init(HI_U32 u32BusNum);
HI_VOID ssp_drv_exit(HI_U32 u32BusNum);

#endif