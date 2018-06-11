/*
 * Simple synchronous userspace interface to SPI devices
 *
 * Copyright (C) 2006 SWAPP
 *	Andrea Paterniani <a.paterniani@swapp-eng.it>
 * Copyright (C) 2007 David Brownell (simplification, cleanup)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/ioctl.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/list.h>
#include <linux/errno.h>
#include <linux/mutex.h>
#include <linux/slab.h>
#include <linux/compat.h>
#include <linux/delay.h>
#include <linux/spi/spi.h>
#include <asm/uaccess.h>

#include "isp_ext.h"

static unsigned bus_num = 0;
static unsigned csn = 0;
static char* sensor = "";

module_param(bus_num, uint, S_IRUGO);
MODULE_PARM_DESC(bus_num, "spi bus number");

module_param(csn, uint, S_IRUGO);
MODULE_PARM_DESC(csn, "chip select number");

/* some sensor has special dev addr */
module_param(sensor, charp, S_IRUGO);
MODULE_PARM_DESC(sensor, "sensor name");


struct spi_master* hi_master;
struct spi_device* hi_spi;
extern struct bus_type   spi_bus_type;


#define SPI_MSG_NUM		20
typedef struct hi_spi_message_s
{
    struct spi_transfer	t;
    struct spi_message	m;
    unsigned char	buf[8];
} spi_message_s;

typedef struct hi_spi_message_info_s
{
    int msg_idx;
    spi_message_s spi_msg_array[SPI_MSG_NUM];
} spi_message_info_s;

static spi_message_info_s g_spi_msg = {0};


/*****************************************************************
This function will be called in interrupt route.
So use spi_async, can't call spi_sync here.
*****************************************************************/
int ssp_write_alt(unsigned int addr1, unsigned int addr1bytenum,
                  unsigned int addr2, unsigned int addr2bytenum,
                  unsigned int data , unsigned int databytenum)
{
    struct spi_master*	master = hi_master;
    struct spi_device*	spi = hi_spi;
    struct spi_transfer* t;
    struct spi_message*	m;
    unsigned char*		buf;
    int 				status = 0;
    unsigned long		flags;
    int                 buf_idx = 0;
    int idx = g_spi_msg.msg_idx;

    g_spi_msg.msg_idx++;
    if (g_spi_msg.msg_idx > SPI_MSG_NUM - 1)
    {
        g_spi_msg.msg_idx = 0;
    }

    buf = g_spi_msg.spi_msg_array[idx].buf;
    t	= &g_spi_msg.spi_msg_array[idx].t;
    m	= &g_spi_msg.spi_msg_array[idx].m;

    /* check spi_message is or no finish */
    spin_lock_irqsave(&master->queue_lock, flags);
    if (m->state != NULL)
    {
        spin_unlock_irqrestore(&master->queue_lock, flags);
        dev_err(&spi->dev, "%s, %s, %d line: spi_message no finish!\n", __FILE__, __func__, __LINE__);
        return -EFAULT;
    }
    spin_unlock_irqrestore(&master->queue_lock, flags);

    spi->mode = SPI_MODE_3 | SPI_LSB_FIRST;

    memset(buf, 0, sizeof(g_spi_msg.spi_msg_array[idx].buf));

    if (strcmp(sensor, "imx117"))
    {
        buf[buf_idx++] = addr1 & (~0x80);
    }
    else
    {
        /* imx117 has different dev addr format */
        buf[buf_idx++] = addr1;
    }

    if (2 == addr2bytenum)
    {
        buf[buf_idx++] = addr2 >> 8;
    }
    buf[buf_idx++] = addr2;

    if (2 == databytenum)
    {
        buf[buf_idx++] = data >> 8;
    }
    buf[buf_idx++] = data;

    t->tx_buf	 = buf;
    t->rx_buf	 = buf;
    t->len		 = buf_idx;
    t->cs_change = 1;
    t->speed_hz  = 2000000;
    t->bits_per_word = 8;

    spi_message_init(m);
    spi_message_add_tail(t, m);
    m->state = m;
    status = spi_async(spi, m);
    if (status)
    {
        dev_err(&spi->dev, "%s: spi_async() error!\n", __func__);
        status = -EFAULT;
    }

    return status;
}

int hi_ssp_write(unsigned int addr1, unsigned int addr1bytenum,
                 unsigned int addr2, unsigned int addr2bytenum,
                 unsigned int data , unsigned int databytenum)
{
    if ((addr1bytenum > 1) || (addr2bytenum > 2) || (databytenum > 2))
    {
        printk("addr1_num: %d, addr2_num: %d, data_num: %d, bit_width not support now.\n",
               addr1bytenum, addr2bytenum, databytenum);
        return -1;
    }

#if 0
    printk("addr1: 0x%x, addr1_num: %d, addr2: 0x%x, addr2_num: %d, data: 0x%x, data_num: %d.\n",
           addr1, addr1bytenum, addr2, addr2bytenum, data, databytenum);
#endif
    return ssp_write_alt(addr1, addr1bytenum, addr2, addr2bytenum, data, databytenum);
}

/*****************************************************************
This function can't be called in interrupt route because spi_sync will
schedule out.
*****************************************************************/
int ssp_read_alt(unsigned char devaddr, unsigned char addr, unsigned char* data)
{
    struct spi_master*	         master = hi_master;
    struct spi_device*           spi = hi_spi;
    int			                status = 0;
    unsigned long		        flags;
    static struct spi_transfer 	t;
    static struct spi_message	m;
    static unsigned char        buf[8];
    int                         buf_idx = 0;

    /* check spi_message is or no finish */
    spin_lock_irqsave(&master->queue_lock, flags);
    if (m.state != NULL)
    {
        spin_unlock_irqrestore(&master->queue_lock, flags);
        dev_err(&spi->dev, "\n**********%s, %s, %d line: spi_message no finish!*********\n", __FILE__, __func__, __LINE__);
        return -EFAULT;
    }
    spin_unlock_irqrestore(&master->queue_lock, flags);

    spi->mode = SPI_MODE_3 | SPI_LSB_FIRST;

    memset(buf, 0, sizeof(buf));
    buf[buf_idx++] = devaddr | 0x80;

    buf[buf_idx++] = addr;
    buf[buf_idx++] = 0;

    t.tx_buf    = buf;
    t.rx_buf    = buf;
    t.len       = buf_idx;
    t.cs_change = 1;
    t.speed_hz  = 2000000;
    t.bits_per_word = 8;

    spi_message_init(&m);
    spi_message_add_tail(&t, &m);
    m.state = &m;
    status  = spi_sync(spi, &m);
    if (status)
    {
        dev_err(&spi->dev, "%s: spi_async() error!\n", __func__);
        status = -EFAULT;
    }

    *data = buf[2];

    printk("func:%s rx_buf = %#x, %#x, %#x\n", __func__, buf[0], buf[1], buf[2]);
    return status;
}

#if 0
static void ssp_test(void)
{
    unsigned char data;
    ssp_write_alt(0x2, 1, 0x14, 1, 0x34, 1);
    // wait spi write finish
    msleep(1);
    ssp_read_alt(0x2, 0x14, &data);
}
#endif

static int __init sensor_spi_dev_init(void)
{
    int 			    status = 0;
    struct spi_master*	 master;
    struct device*		dev;
    char 			    spi_name[128] = {0};

    ISP_BUS_CALLBACK_S stBusCb = {0};
    stBusCb.pfnISPWriteSSPData = hi_ssp_write;
    if (CKFN_ISP_RegisterBusCallBack())
    {
        CALL_ISP_RegisterBusCallBack(0, ISP_BUS_TYPE_SSP, &stBusCb);
    }
    else
    {
        printk("register ssp_write_callback to isp failed, ssp init is failed!\n");
        return -1;
    }

    master = spi_busnum_to_master(bus_num);
    if (master)
    {
        hi_master = master;
        snprintf(spi_name, sizeof(spi_name), "%s.%u", dev_name(&master->dev), csn);
        dev = bus_find_device_by_name(&spi_bus_type, NULL, spi_name);
        if (dev == NULL)
        {
            dev_err(NULL, "chipselect %d has not been used\n", csn);
            status = -ENXIO;
            goto end1;
        }

        hi_spi = to_spi_device(dev);
        if (hi_spi == NULL)
        {
            dev_err(dev, "to_spi_device() error!\n");
            status = -ENXIO;
            goto end1;
        }
    }
    else
    {
        dev_err(NULL, "spi_busnum_to_master() error!\n");
        status = -ENXIO;
        goto end0;
    }

    //ssp_test();
end1:
    put_device(dev);
end0:
    printk(KERN_INFO "load sensor_spi.ko ...OK!\n");
    return status;
}

static void __exit sensor_spi_dev_exit(void)
{
    printk("%s, %s, %d line\n", __FILE__, __func__, __LINE__);
    printk(KERN_INFO "unload sensor_spi.ko ...OK!\n");
}

module_init(sensor_spi_dev_init);
module_exit(sensor_spi_dev_exit);

MODULE_AUTHOR("BVT OSDRV");
MODULE_LICENSE("GPL");
MODULE_ALIAS("sensor spidev");
