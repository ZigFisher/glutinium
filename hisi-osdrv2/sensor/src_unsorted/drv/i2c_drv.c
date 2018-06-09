#include <linux/init.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/string.h>
#include <linux/io.h>
#include <linux/i2c.h>

#include <linux/delay.h>
#include "sensor.h"
#include "hi_comm_isp.h"

#ifdef __HuaweiLite__
#include <stdio.h>
#include <i2c.h>
#endif

#define I2C_BUS_NUM 2

#define I2C_GET_FUNCTION(num, func) sensor_i2c_##func_##num

#ifndef __HuaweiLite__
static struct i2c_board_info hi_info[] =
{
	{ I2C_BOARD_INFO("sensor_i2c0", (0x6c>>1)) },
	{ I2C_BOARD_INFO("sensor_i2c1", (0x6c>>1)) }
};
#endif

static struct i2c_client *sensor_client[I2C_BUS_NUM] = {0};

static int sensor_i2c_write(HI_U32 client_index, HI_VOID *pSensorData)
{
    char tmp_buf[8];
	int ret = 0;
    int idx = 0;
#ifndef __HuaweiLite__
    unsigned int u32Tries = 0;
#endif
	struct i2c_client client;
	unsigned char dev_addr;
	unsigned int reg_addr, reg_addr_num;
	unsigned int data, data_byte_num;
	ISP_I2C_DATA_S *pstI2cData = HI_NULL;

	pstI2cData = (ISP_I2C_DATA_S *)pSensorData;

	dev_addr = pstI2cData->u8DevAddr;
	reg_addr = pstI2cData->u32RegAddr;
	reg_addr_num = pstI2cData->u32AddrByteNum;
	data = pstI2cData->u32Data;
	data_byte_num = pstI2cData->u32DataByteNum;

    memcpy(&client, sensor_client[client_index], sizeof(struct i2c_client));
    client.addr = (dev_addr>>1);

    /* reg_addr config */
    if(reg_addr_num == 1)
    {
        tmp_buf[idx++] = reg_addr&0xff;
    }
    else
    {
        tmp_buf[idx++] = (reg_addr >> 8)&0xff;
        tmp_buf[idx++] = reg_addr&0xff;
    }

    /* data config */
    if(data_byte_num == 1)
    {
        tmp_buf[idx++] = data;
    }
    else
    {
        tmp_buf[idx++] = (data >> 8)&0xff;
        tmp_buf[idx++] = data&0xff;
    }

    while (1)
    {
        ret = i2c_master_send(&client, tmp_buf, idx);
        if (ret == idx)
        {
            break;
        }
#ifndef __HuaweiLite__
        else if ((ret == -EAGAIN) && (in_atomic() || irqs_disabled()))
        {
            u32Tries++;
            if (u32Tries > 5)
            {
                printk("[%s %d] i2c_master_send try %d times, but failed. \n", __func__, __LINE__, u32Tries);
                return -1;
            }
        }
#endif
        else
        {
            printk("[%s %d] i2c_master_send error, ret=%d. \n", __func__, __LINE__,
                ret);
            return ret;
        }
    }

	return 0;
}

static int sensor_i2c_read(HI_U32 client_index, HI_VOID *pSensorData)
{
    unsigned char tmp_buf0[4];
    unsigned char tmp_buf1[4];
    int ret = 0;
    int ret_data = 0xFF;
    int idx = 0;
    struct i2c_client client;
    struct i2c_msg msg[2];
	unsigned char dev_addr;
	unsigned int reg_addr, reg_addr_num;
	unsigned int data_byte_num;
	ISP_I2C_DATA_S *pstI2cData = HI_NULL;

	pstI2cData = (ISP_I2C_DATA_S *)pSensorData;

	dev_addr = pstI2cData->u8DevAddr;
	reg_addr = pstI2cData->u32RegAddr;
	reg_addr_num = pstI2cData->u32AddrByteNum;
	data_byte_num = pstI2cData->u32DataByteNum;

    memcpy(&client, sensor_client[client_index], sizeof(struct i2c_client));

    msg[0].addr = (dev_addr>>1);
    msg[0].flags = client.flags & I2C_M_TEN;
    msg[0].len = reg_addr_num;
    msg[0].buf = tmp_buf0;

    /* reg_addr config */
    if(reg_addr_num == 1)
    {
        tmp_buf0[idx++] = reg_addr&0xff;
    }
    else
    {
        tmp_buf0[idx++] = (reg_addr >> 8)&0xff;
        tmp_buf0[idx++] = reg_addr&0xff;
    }

    msg[1].addr = (dev_addr>>1);
    msg[1].flags = client.flags & I2C_M_TEN;
    msg[1].flags |= I2C_M_RD;
    msg[1].len = data_byte_num;
    msg[1].buf = tmp_buf1;

    while (1)
    {
        ret = i2c_transfer(client.adapter, msg, 2);
        if (ret == 2)
        {
            if (data_byte_num == 2)
            {
                ret_data = tmp_buf1[1] | (tmp_buf1[0] << 8);
            }
            else
            {
                ret_data = tmp_buf1[0];
            }
            break;
        }
#ifndef __HuaweiLite__
        else if ((ret == -EAGAIN) && (in_atomic() || irqs_disabled()))
        {
            continue;
        }
#endif
        else
        {
            printk("[%s %d] i2c_transfer error, ret=%d. \n", __func__, __LINE__,
                ret);
            break;
        }
    }
    return ret_data;
}

int sensor_i2c_write_0(HI_VOID *pSensorData)
{
	return sensor_i2c_write(0, pSensorData);
}

int sensor_i2c_write_1(HI_VOID *pSensorData)
{
	return sensor_i2c_write(1, pSensorData);
}

int sensor_i2c_read_0(HI_VOID *pSensorData)
{
	return sensor_i2c_read(0, pSensorData);
}

int sensor_i2c_read_1(HI_VOID *pSensorData)
{
	return sensor_i2c_read(1, pSensorData);
}

HI_VOID i2c_get_ops(HI_U32 u32BusNum, SENSOR_CTRL_OPS_S *pstCtrlOps)
{
	if (0 == u32BusNum)
	{
		pstCtrlOps->write = sensor_i2c_write_0;
		pstCtrlOps->read  = sensor_i2c_read_0;
	}
	else
	{
		pstCtrlOps->write = sensor_i2c_write_1;
		pstCtrlOps->read  = sensor_i2c_read_1;
	}
}

#ifdef __HuaweiLite__
struct i2c_client * hi_sensor_i2c_client_init(HI_U32 u32BusNum)
{
    struct i2c_client *client;
    int ret;

    client = malloc(sizeof(struct i2c_client));
    if (NULL == client){
        printk("malloc i2c_client err. \n");
        return NULL;
    }

    memset(client, 0 ,sizeof(struct i2c_client));
    snprintf(client->name, sizeof(client->name), "dev/i2c_client-i2c%d", u32BusNum);
    client->addr = (0x6c>>1);

    ret = client_attach(client, u32BusNum);
    if (ret){
        printk("client_attach err. \n");
        free(client);
        return NULL;
    }

    return client;
}
#endif

HI_S32 i2c_drv_init(HI_U32 u32BusNum)
{

#ifdef __HuaweiLite__
	if (u32BusNum >= I2C_BUS_NUM)
	{
		return -1;
	}
    sensor_client[u32BusNum] = hi_sensor_i2c_client_init(u32BusNum);
	if (!sensor_client[u32BusNum])
	{
		printk("sensor i2c init failed!\n");
		return -1;
	}
#else
    struct i2c_adapter *i2c_adap;
	if (u32BusNum >= I2C_BUS_NUM)
	{
		return -1;
	}
    i2c_adap = i2c_get_adapter(u32BusNum);
    sensor_client[u32BusNum] = i2c_new_device(i2c_adap, &hi_info[u32BusNum]);
	if (!sensor_client[u32BusNum])
	{
		printk("sensor i2c init failed!\n");
		return -1;
	}
    i2c_put_adapter(i2c_adap);
#endif

    return 0;
}

HI_VOID i2c_drv_exit(HI_U32 u32BusNum)
{
	if (u32BusNum >= I2C_BUS_NUM)
	{
		return;
	}

#ifndef __HuaweiLite__
	if (sensor_client[u32BusNum])
	{
    	i2c_unregister_device(sensor_client[u32BusNum]);
	}
#endif
}
