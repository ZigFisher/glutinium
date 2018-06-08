#ifndef __I2C_DRV_H__
#define __I2C_DRV_H__

HI_VOID i2c_get_ops(HI_U32 u32BusNum, SENSOR_CTRL_OPS_S *pstCtrlOps);

HI_S32 i2c_drv_init(HI_U32 u32BusNum);
HI_VOID i2c_drv_exit(HI_U32 u32BusNum);

#endif