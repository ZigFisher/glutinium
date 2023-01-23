/*   extdrv/peripheral/vda/adv7179.c
 *
 *
 * Copyright (c) 2006 Hisilicon Co., Ltd.
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
 * along with this program.
 *
 *
 * History:
 *     17-Apr-2006 create this file
 *
 */

#include <linux/string.h>
#include <linux/slab.h>
#include <linux/module.h>
// BEGIN: Modified by x00132463, 2010-8-20 15 : 57 : 28
//#include <linux/config.h>
//#include <linux/autoconf.h>
#include <linux/errno.h>
#include <linux/miscdevice.h>
#include <linux/fcntl.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/proc_fs.h>
#include <linux/workqueue.h>

#include <linux/proc_fs.h>
#include <linux/poll.h>

//#include <asm/hardware.h>
#include <asm/bitops.h>
#include <asm/uaccess.h>
#include <asm/irq.h>

#include <linux/moduleparam.h>
#include <linux/ioport.h>
#include <linux/interrupt.h>
//#include <linux/kcom.h>
//#include <kcom/gpio_i2c.h>
#include "adv7179.h"

#include <asm/io.h>
#include <linux/miscdevice.h>

#include <linux/i2c.h>

//#define HI_GPIO_I2C
//#ifdef HI_GPIO_I2C
//#include "gpio_i2c.h"
//#else
//#include "hi_i2c.h"
//#endif

//#ifdef HI_GPIO_I2C
//#define hi_i2c_write(chip_addr,reg_addr,value) gpio_i2c_write(chip_addr,reg_addr,value)
//#define hi_i2c_read(chip_addr,reg_addr) gpio_i2c_read(chip_addr,reg_addr)
//#else
//#define hi_i2c_write(chip_addr,reg_addr,value) HI_I2C_Write(chip_addr,reg_addr,1,value,1)
//#define hi_i2c_read(chip_addr,reg_addr) HI_I2C_Read(chip_addr,reg_addr,1,1)
//#endif
#define I2C_ADV7179       0x56
static struct i2c_client* sil_client;
static struct i2c_board_info sil_i2c_info =
{
    I2C_BOARD_INFO("sil9034", I2C_ADV7179),
};

unsigned char _hi_i2c_read_byte(unsigned char devaddress, unsigned char address)
{
    unsigned char ret_data = 0xFF;
    int ret;
    struct i2c_client* client = sil_client;
    unsigned char buf[2];

    buf[0] = address;
    ret = hi_i2c_master_recv(client, buf, 1);
    if (ret >= 0)
    {
        ret_data = buf[0];
    }
    return ret_data;
}

int _hi_i2c_write_byte(unsigned char devaddress, unsigned char address, unsigned char data)
{
    int ret;
    unsigned char buf[2];
    struct i2c_client* client = sil_client;

    buf[0] = address;
    buf[1] = data;

    ret = hi_i2c_master_send(client, buf, 2);
    return ret;
}
static int i2c_client_init(void)
{
    struct i2c_adapter* i2c_adap;

    // use i2c1
    i2c_adap   = i2c_get_adapter(1);
    sil_client = i2c_new_device(i2c_adap, &sil_i2c_info);

    i2c_put_adapter(i2c_adap);

    return 0;
}

static void i2c_client_exit(void)
{
    i2c_unregister_device(sil_client);
}

#define  I2CReadByte   _hi_i2c_read_byte
#define  I2CWriteByte  _hi_i2c_write_byte

/* adv7179 i2c slaver address micro-definition. */

#define ADV7179_NUM       1
#define ADV7179_REG_WIDTH 1
#define ADV7179_DAT_WIDTH 1

#define adv7179_NUM_REGISTERS   128



/* Output filter:  S-Video  Composite */

#define MR050       0x11
#define MR060       0x14


#define TR0MODE_656  0x08
#define TR0MODE_601  0x0c


#define TR0RST      0x80

#define TR1CAPT	    0x00
#define TR1PLAY	    0x00

static int norm_mode =  VIDEO_NORM_PAL;


static const unsigned char init_656_NTSC[] =
{
    0x00, 0x00,		/* MR0 */
    0x01, 0x00,		/* MR1 */
    0x02, 0x00,		/* MR2 RTC control: bits 2 and 1 */
    0x03, 0x00,		/* MR3 */
    0x04, 0x15,		/* MR4 */
    0x05, 0x00,		/* Reserved */
    0x06, 0x00,		/* Reserved */
    0x07, TR0MODE_656,	/* TM0 */
    0x08, TR1CAPT,		/* TM1 */
    0x09, 0x16,		/* Fsc0 */
    0x0a, 0x7c,		/* Fsc1 */
    0x0b, 0xf0,		/* Fsc2 */
    0x0c, 0x21,		/* Fsc3 */
    0x0d, 0x00,		/* Subcarrier Phase */
    0x0e, 0x00,		/* Closed Capt. Ext 0 */
    0x0f, 0x00,		/* Closed Capt. Ext 1 */
    0x10, 0x00,		/* Closed Capt. 0 */
    0x11, 0x00,		/* Closed Capt. 1 */
    0x12, 0x00,		/* Pedestal Ctl 0 */
    0x13, 0x00,		/* Pedestal Ctl 1 */
    0x14, 0x00,		/* Pedestal Ctl 2 */
    0x15, 0x00,		/* Pedestal Ctl 3 */
    0x16, 0x00,		/* CGMS_WSS_0 */
    0x17, 0x00,		/* CGMS_WSS_1 */
    0x18, 0x00,		/* CGMS_WSS_2 */
    0x19, 0x00,		/* Teletext Ctl */
};


static const unsigned char init_656_PAL[] =
{
    0x00, 0x05,		/* MR0, PAL BDGHI */
    0x01, 0x00,		/* MR1 */
    0x02, 0x00,		/* MR2 RTC control: bits 2 and 1 */
    0x03, 0x00,		/* MR3 */
    0x04, 0x15,		/* MR4, RGB OUTPUT */
    0x05, 0x00,		/* Reserved */
    0x06, 0x00,		/* Reserved */
    0x07, TR0MODE_656,	/* TM0 */
    0x08, TR1CAPT,		/* TM1 */
    0x09, 0xcb,		/* Fsc0 */
    0x0a, 0x8a,		/* Fsc1 */
    0x0b, 0x09,		/* Fsc2 */
    0x0c, 0x2a,		/* Fsc3 */
    0x0d, 0x00,		/* Subcarrier Phase */
    0x0e, 0x00,		/* Closed Capt. Ext 0 */
    0x0f, 0x00,		/* Closed Capt. Ext 1 */
    0x10, 0x00,		/* Closed Capt. 0 */
    0x11, 0x00,		/* Closed Capt. 1 */
    0x12, 0x00,		/* Pedestal Ctl 0 */
    0x13, 0x00,		/* Pedestal Ctl 1 */
    0x14, 0x00,		/* Pedestal Ctl 2 */
    0x15, 0x00,		/* Pedestal Ctl 3 */
    0x16, 0x00,		/* CGMS_WSS_0 */
    0x17, 0x00,		/* CGMS_WSS_1 */
    0x18, 0x00,		/* CGMS_WSS_2 */
    0x19, 0x00,		/* Teletext Ctl */
};




static  unsigned char init_601_NTSC[] =
{
    0x00, 0x00,		/* MR0 */
    0x01, 0x00,  	/* MR1 */
    0x02, 0x08,  	/* MR2 RTC control: bits 2 and 1 */
    0x03, 0x04,  	/* MR3 */
    0x04, 0x0C,  	/* MR4 */
    0x05, 0x00,		/* Reserved */
    0x06, 0x00,		/* Reserved */
    0x07, TR0MODE_601,		/* TM0 */
    0x08, TR1CAPT,		/* TM1 */
    0x09, 0x16,		/* Fsc0 */
    0x0a, 0x7c,		/* Fsc1 */
    0x0b, 0xf0,		/* Fsc2 */
    0x0c, 0x21,		/* Fsc3 */
    0x0d, 0x00,		/* Subcarrier Phase */
    0x0e, 0x00,		/* Closed Capt. Ext 0 */
    0x0f, 0x00,		/* Closed Capt. Ext 1 */
    0x10, 0x00,		/* Closed Capt. 0 */
    0x11, 0x00,		/* Closed Capt. 1 */
    0x12, 0x00,		/* Pedestal Ctl 0 */
    0x13, 0x00,		/* Pedestal Ctl 1 */
    0x14, 0x00,		/* Pedestal Ctl 2 */
    0x15, 0x00,		/* Pedestal Ctl 3 */
    0x16, 0x00,		/* CGMS_WSS_0 */
    0x17, 0x00,		/* CGMS_WSS_1 */
    0x18, 0x00,		/* CGMS_WSS_2 */
    0x19, 0x00,		/* Teletext Ctl  */
};



static  unsigned char init_601_PAL[] =
{
    0x00, 0x05,		/* MR0, PAL BDGHI */
    0x01, 0x00,		/* MR1 */
    0x02, 0x08, 	/* MR2 RTC control: bits 2 and 1 */
    0x03, 0x04,     /* MR3 */
    0x04, 0x1C,     /* MR4, RGB OUTPUT */
    0x05, 0x00,		/* Reserved */
    0x06, 0x00,		/* Reserved */
    0x07, TR0MODE_601,	/* TM0 */
    0x08, TR1CAPT,		/* TM1 */
    0x09, 0xcb,		/* Fsc0 */
    0x0a, 0x8a,		/* Fsc1 */
    0x0b, 0x09,		/* Fsc2 */
    0x0c, 0x2a,		/* Fsc3 */
    0x0d, 0x00,		/* Subcarrier Phase */
    0x0e, 0x00,		/* Closed Capt. Ext 0 */
    0x0f, 0x00,		/* Closed Capt. Ext 1 */
    0x10, 0x00,		/* Closed Capt. 0 */
    0x11, 0x00,		/* Closed Capt. 1 */
    0x12, 0x00,		/* Pedestal Ctl 0 */
    0x13, 0x00,		/* Pedestal Ctl 1 */
    0x14, 0x00,		/* Pedestal Ctl 2 */
    0x15, 0x00,		/* Pedestal Ctl 3 */
    0x16, 0x00,		/* CGMS_WSS_0 */
    0x17, 0x00,		/* CGMS_WSS_1 */
    0x18, 0x00,		/* CGMS_WSS_2 */
    0x19, 0x00,		/* Teletext Ctl */
};




static int write_regs(unsigned char* pdevdata,   unsigned long datalen)
{
    int i = 0;

    while (i < datalen)
    {
        I2CWriteByte(I2C_ADV7179, pdevdata[i], pdevdata[i + 1]);
        i += 2;
    }
    return 0;
}


/*
 * adv7179 initialise routine.
 *
 * @param vdaccir: adv7179's working mode:0--VIDEO_MODE_CCIR656; 1--VIDEO_MODE_CCIR601
 * @param vdanorm: adv7179's norm mode;
 * @param vdamaster: adv7179's slave or master mode;
 *
 * @return value:0--success; -1--error.
 *
 */

int init_vda(int vdaccir, int vdanorm, int vdamaster)
{

    if ((vdaccir == VIDEO_MODE_CCIR656) && (vdanorm == VIDEO_NORM_PAL))
    {
        if (write_regs((unsigned char*)&init_656_PAL, sizeof(init_656_PAL)) != 0)
        {
            goto err_out;
        }
    }
    else if ((vdaccir == VIDEO_MODE_CCIR656) && (vdanorm == VIDEO_NORM_NTSC))
    {
        if (write_regs((unsigned char*)&init_656_NTSC, sizeof(init_656_NTSC)) != 0)
        {
            goto err_out;
        }
    }
    else if ((vdaccir == VIDEO_MODE_CCIR601) && (vdanorm == VIDEO_NORM_PAL))
    {
        if (vdamaster == VIDEO_MODE_MASTER)
        {
            init_601_PAL[15] |= VIDEO_MODE_MASTER;
        }
        else if (vdamaster == VIDEO_MODE_SLAVER)
        {
            init_601_PAL[15] &= ~VIDEO_MODE_MASTER;
        }

        if (write_regs((unsigned char*)&init_601_PAL, sizeof(init_601_PAL))  != 0)
        {
            goto err_out;
        }
    }
    else if ((vdaccir == VIDEO_MODE_CCIR601) && (vdanorm == VIDEO_NORM_NTSC))
    {
        if (vdamaster == VIDEO_MODE_MASTER)
        {
            init_601_NTSC[15] |= VIDEO_MODE_MASTER;
        }
        if (vdamaster == VIDEO_MODE_SLAVER)
        {
            init_601_NTSC[15] &= ~VIDEO_MODE_MASTER;
        }

        if (write_regs((unsigned char*)&init_601_NTSC, sizeof(init_601_NTSC))  != 0)
        {
            goto err_out;
        }
    }

    if (vdaccir == VIDEO_MODE_CCIR656)
    {
        I2CWriteByte(I2C_ADV7179, 0x07, (TR0MODE_656 | TR0RST));
        I2CWriteByte(I2C_ADV7179, 0x07, TR0MODE_656);
    }
    else if (vdaccir == VIDEO_MODE_CCIR601)
    {
        if (vdamaster == VIDEO_MODE_MASTER)
        {
            I2CWriteByte(I2C_ADV7179, 0x07, (TR0MODE_601 | TR0RST | VIDEO_MODE_MASTER));
            I2CWriteByte(I2C_ADV7179, 0x07, (TR0MODE_601 | VIDEO_MODE_MASTER));
        }
        else
        {
            I2CWriteByte(I2C_ADV7179, 0x07, (TR0MODE_601 | TR0RST));
            I2CWriteByte(I2C_ADV7179, 0x07, TR0MODE_601);
        }
    }

    return (0);

err_out:
    printk("err_out\n");
    return -1;
}

/*
 * adv7179 open routine.
 * do nothing.
 *
 */
int adv7179_open(struct inode* inode, struct file* file)
{

    return 0;
}

/*
 * adv7179 close routine.
 * do nothing.
 *
 */
int adv7179_close(struct inode* inode, struct file* file)
{

    return 0;
}

/*
 * adv7179 ioctl routine.
 * @param inode: pointer of the node;
 * @param file: pointer of the file;
 *
 * @param cmd: command from the app:
 * ENCODER_SET_NORM(2):set adv7179's work mode.
 *
 * @param arg:arg from app layer.
 *
 * @return value:0-- set success; -1-- set error.
 *
 */
static long adv7179_ioctl(struct file* file, unsigned int cmd, unsigned long arg)
{

    switch (cmd)
    {

        case ENCODER_SET_NORM:
        {
            int iarg = (int) arg;


            switch (iarg)
            {
                case VIDEO_MODE_656_PAL:
                    write_regs((unsigned char*)&init_656_PAL, sizeof(init_656_PAL));

                    I2CWriteByte(I2C_ADV7179, 0x07, (TR0MODE_656 | TR0RST));
                    I2CWriteByte(I2C_ADV7179, 0x07, TR0MODE_656);
                    break;

                case VIDEO_MODE_656_NTSC:
                    write_regs((unsigned char*)&init_656_NTSC, sizeof(init_656_NTSC));

                    I2CWriteByte(I2C_ADV7179, 0x07, (TR0MODE_656 | TR0RST));
                    I2CWriteByte(I2C_ADV7179, 0x07, TR0MODE_656);
                    break;

                case VIDEO_MODE_601_PAL_MASTER:
                    init_601_PAL[15] |= VIDEO_MODE_MASTER;
                    write_regs((unsigned char*)&init_601_PAL, sizeof(init_601_PAL));

                    I2CWriteByte(I2C_ADV7179, 0x07, (TR0MODE_601 | TR0RST | VIDEO_MODE_MASTER));
                    I2CWriteByte(I2C_ADV7179, 0x07, (TR0MODE_601 | VIDEO_MODE_MASTER));
                    break;


                case VIDEO_MODE_601_NTSC_MASTER:
                    init_601_NTSC[15] |= VIDEO_MODE_MASTER;
                    write_regs((unsigned char*)&init_601_NTSC, sizeof(init_601_NTSC));

                    I2CWriteByte(I2C_ADV7179, 0x07, (TR0MODE_601 | TR0RST | VIDEO_MODE_MASTER));
                    I2CWriteByte(I2C_ADV7179, 0x07, (TR0MODE_601 | VIDEO_MODE_MASTER));
                    break;

                case VIDEO_MODE_601_PAL_SLAVER:
                    init_601_PAL[15] &= ~VIDEO_MODE_MASTER;
                    write_regs((unsigned char*)&init_601_PAL, sizeof(init_601_PAL));

                    I2CWriteByte(I2C_ADV7179, 0x07, (TR0MODE_601 | TR0RST));
                    I2CWriteByte(I2C_ADV7179, 0x07, TR0MODE_601);
                    break;

                case VIDEO_MODE_601_NTSC_SLAVER:
                    init_601_NTSC[15] &= ~VIDEO_MODE_MASTER;
                    write_regs((unsigned char*)&init_601_NTSC, sizeof(init_601_NTSC));

                    I2CWriteByte(I2C_ADV7179, 0x07, (TR0MODE_601 | TR0RST));
                    I2CWriteByte(I2C_ADV7179, 0x07, TR0MODE_601);
                    break;

                default:
                    return -1;

            }

        }
        break;

        default:
            return -1;
    }

    return 0;
}

/*
 *  The various file operations we support.
 */

static struct file_operations adv7179_fops =
{
    .owner		= THIS_MODULE,
    .unlocked_ioctl		= adv7179_ioctl,
    .open		  = adv7179_open ,
    .release	= adv7179_close
};

static struct miscdevice adv7179_dev =
{
    MISC_DYNAMIC_MINOR,
    "adv7179",
    &adv7179_fops,
};

static int adv7179_device_init(void)
{
    unsigned char regvalue1, regvalue2;
    regvalue1 = I2CReadByte(I2C_ADV7179, 0x07);
    I2CWriteByte(I2C_ADV7179, 0x07, 0xa5);
    regvalue2 = I2CReadByte(I2C_ADV7179, 0x07);
    if (regvalue2 != 0xa5)
    {
        printk("read adv7179 register is %x\n", regvalue2);
        printk("check adv7179 error.\n");
        return -EFAULT;
    }
    I2CWriteByte(I2C_ADV7179, 0x07, regvalue1);
    if (norm_mode == VIDEO_NORM_NTSC)
    {
        if (init_vda(VIDEO_MODE_CCIR656, VIDEO_NORM_NTSC, VIDEO_MODE_MASTER) == 0)
        {
            return 0;
        }
        else
        {
            return -EFAULT;
        }
    }
    else
    {
        if (init_vda(VIDEO_MODE_CCIR656, VIDEO_NORM_PAL, VIDEO_MODE_MASTER) == 0)
        {
            return 0;
        }
        else
        {
            return -EFAULT;
        }
    }

}

//DECLARE_KCOM_GPIO_I2C();
static int __init adv7179_init(void)
{
    int ret = 0;

#if 0
    ret = KCOM_GPIO_I2C_INIT();
    if (ret)
    {
        printk("GPIO I2C module is not load.\n");
        return -1;
    }
#endif

    i2c_client_init();

    ret = misc_register(&adv7179_dev);
    if (ret)
    {
        //KCOM_GPIO_I2C_EXIT();
        printk("could not register adv7179 devices. \n");
        return -1;
    }
    if (adv7179_device_init() < 0)
    {
        i2c_client_exit();
        misc_deregister(&adv7179_dev);
        //KCOM_GPIO_I2C_EXIT();
        printk("adv7179 driver init fail for device init error!\n");
        return -1;
    }

    printk("adv7179 driver init successful!\n");
    return ret;
}

static void __exit adv7179_exit(void)
{
    i2c_client_exit();
    misc_deregister(&adv7179_dev);
    //KCOM_GPIO_I2C_EXIT();

}

module_init(adv7179_init);
module_exit(adv7179_exit);

#ifdef MODULE
//#include <linux/compile.h>
#endif

module_param(norm_mode, int, S_IRUGO);

//MODULE_INFO(build, UTS_VERSION);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("hisilicon");


