#include <linux/init.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/string.h>
#include <linux/io.h>
#include <linux/i2c.h>
#include <linux/hardirq.h>

#include <linux/delay.h>
#include "isp_ext.h"

static struct i2c_board_info hi_info =
{
     I2C_BOARD_INFO("sensor_i2c", 0x6c),
};

static struct i2c_client *sensor_client;

int hi_i2c_read(unsigned char dev_addr, unsigned int reg_addr,
                unsigned int reg_addr_num, unsigned int data_byte_num);

int hi_sensor_i2c_write(unsigned char dev_addr,
                        unsigned int reg_addr, unsigned int reg_addr_num,
                        unsigned int data, unsigned int data_byte_num)
{
    unsigned char tmp_buf[8];
	int ret = 0;
    int idx = 0;
	struct i2c_client *client = sensor_client;
    unsigned int u32Tries = 0;

    sensor_client->addr = dev_addr;

    /* reg_addr config */
    tmp_buf[idx++] = reg_addr;
	if (reg_addr_num == 2)
	{
		client->flags  |= I2C_M_16BIT_REG;
        tmp_buf[idx++]  = (reg_addr >> 8);
	}
    else
    {
        client->flags &= ~I2C_M_16BIT_REG;
    }

    /* data config */
    tmp_buf[idx++] = data;
	if (data_byte_num == 2)
	{
		client->flags  |= I2C_M_16BIT_DATA;
        tmp_buf[idx++] = data >> 8;
	}
    else
    {
        client->flags &= ~I2C_M_16BIT_DATA;
    }

    while (1)
    {
        ret = hi_i2c_master_send(client, tmp_buf, idx);
        if (ret == idx)
        {
            break;
        }
        else if ((ret == -EAGAIN) && (in_atomic() || irqs_disabled()))
        {
            u32Tries++;
            if (u32Tries > 5)
            {
                return -1;
            }
        }
        else
        {
            printk("[%s %d] hi_i2c_master_send error, ret=%d. \n", __func__, __LINE__,
                ret);
            return ret;
        }
    }

	return 0;
}

int hi_i2c_read(unsigned char dev_addr, unsigned int reg_addr,
                unsigned int reg_addr_num, unsigned int data_byte_num)
{
    unsigned char tmp_buf[8];
	int ret = 0;
    int ret_data = 0xFF;
    int idx = 0;
	struct i2c_client *client = sensor_client;

    sensor_client->addr = dev_addr;

    /* reg_addr config */
    tmp_buf[idx++] = reg_addr;
	if (reg_addr_num == 2)
	{
		client->flags  |= I2C_M_16BIT_REG;
        tmp_buf[idx++] = reg_addr >> 8;
	}
    else
    {
        client->flags &= ~I2C_M_16BIT_REG;
    }

    /* data config */
	if (data_byte_num == 2)
	{
		client->flags |= I2C_M_16BIT_DATA;
	}
    else
    {
        client->flags &= ~I2C_M_16BIT_DATA;
    }

    while (1)
    {
        ret = hi_i2c_master_recv(client, tmp_buf, idx);;
        if (ret == idx)
        {
            if (data_byte_num == 2)
            {
                ret_data = tmp_buf[0] | (tmp_buf[1] << 8);
            }
            else
            {
                ret_data = tmp_buf[0];
            }
            break;
        }
        else if ((ret == -EAGAIN) && (in_atomic() || irqs_disabled()))
        {
            continue;
        }
        else
        {
            printk("[%s %d] hi_i2c_master_recv error, ret=%d. \n", __func__, __LINE__,
                ret);
            break;
        }
    }

	return ret_data;
}

static int hi_dev_isp_register(void)
{
    ISP_BUS_CALLBACK_S stBusCb = {0};

    stBusCb.pfnISPWriteI2CData = hi_sensor_i2c_write;
    if (CKFN_ISP_RegisterBusCallBack())
    {
        CALL_ISP_RegisterBusCallBack(0, ISP_BUS_TYPE_I2C, &stBusCb);
    }
    else
    {
        printk("register i2c_write_callback to isp failed, hi_i2c init is failed!\n");
        return -1;
    }

    return 0;
}

static int hi_dev_init(void)
{
    struct i2c_adapter *i2c_adap;

    // use i2c0
    i2c_adap = i2c_get_adapter(0);
	sensor_client = i2c_new_device(i2c_adap, &hi_info);

	i2c_put_adapter(i2c_adap);
    hi_dev_isp_register();

    printk(KERN_INFO "load sensor_i2c.ko ...OK!\n");
    return 0;
}

static void hi_dev_exit(void)
{
    i2c_unregister_device(sensor_client);
    printk(KERN_INFO "unload sensor_i2c.ko ...OK!\n");
}

module_init(hi_dev_init);
module_exit(hi_dev_exit);
MODULE_LICENSE("GPL");
