/*  extdrv/interface/pwm.c
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
 * along with this program;
 *
 * History:
 *      23-march-2011 create this file
 */

#include <linux/module.h>
#include <linux/errno.h>
#ifndef CONFIG_HISI_SNAPSHOT_BOOT
#include <linux/miscdevice.h>
#endif
#include <linux/fcntl.h>

#include <linux/init.h>
#include <linux/delay.h>
#include <linux/proc_fs.h>
#include <linux/workqueue.h>

#include <asm/uaccess.h>
#include <asm/io.h>

#include "pwm.h"
#ifdef CONFIG_HISI_SNAPSHOT_BOOT
#include "himedia.h"
#endif


#define PWMI_ADRESS_BASE         0x20130000

void __iomem *reg_pwmI_base_va = 0;


#define HI_IO_PWMI_ADDRESS(x)  (reg_pwmI_base_va + ((x)-(PWMI_ADRESS_BASE)))



//PWMI
#define PWM0_CFG_REG0      HI_IO_PWMI_ADDRESS(PWMI_ADRESS_BASE + 0x0000)
#define PWM0_CFG_REG1      HI_IO_PWMI_ADDRESS(PWMI_ADRESS_BASE + 0x0004)
#define PWM0_CFG_REG2      HI_IO_PWMI_ADDRESS(PWMI_ADRESS_BASE + 0x0008)
#define PWM0_CTRL_REG       HI_IO_PWMI_ADDRESS(PWMI_ADRESS_BASE + 0x000C)
#define PWM0_STATE_REG0       HI_IO_PWMI_ADDRESS(PWMI_ADRESS_BASE + 0x0010)
#define PWM0_STATE_REG1       HI_IO_PWMI_ADDRESS(PWMI_ADRESS_BASE + 0x0014)
#define PWM0_STATE_REG2       HI_IO_PWMI_ADDRESS(PWMI_ADRESS_BASE + 0x0018)

#define PWM1_CFG_REG0      HI_IO_PWMI_ADDRESS(PWMI_ADRESS_BASE + 0x0020)
#define PWM1_CFG_REG1      HI_IO_PWMI_ADDRESS(PWMI_ADRESS_BASE + 0x0024)
#define PWM1_CFG_REG2      HI_IO_PWMI_ADDRESS(PWMI_ADRESS_BASE + 0x0028)
#define PWM1_CTRL_REG       HI_IO_PWMI_ADDRESS(PWMI_ADRESS_BASE + 0x002C)
#define PWM1_STATE_REG0       HI_IO_PWMI_ADDRESS(PWMI_ADRESS_BASE + 0x0030)
#define PWM1_STATE_REG1       HI_IO_PWMI_ADDRESS(PWMI_ADRESS_BASE + 0x0034)
#define PWM1_STATE_REG2       HI_IO_PWMI_ADDRESS(PWMI_ADRESS_BASE + 0x0038)

#define PWM2_CFG_REG0      HI_IO_PWMI_ADDRESS(PWMI_ADRESS_BASE + 0x0040)
#define PWM2_CFG_REG1      HI_IO_PWMI_ADDRESS(PWMI_ADRESS_BASE + 0x0044)
#define PWM2_CFG_REG2      HI_IO_PWMI_ADDRESS(PWMI_ADRESS_BASE + 0x0048)
#define PWM2_CTRL_REG       HI_IO_PWMI_ADDRESS(PWMI_ADRESS_BASE + 0x004C)
#define PWM2_STATE_REG0       HI_IO_PWMI_ADDRESS(PWMI_ADRESS_BASE + 0x0050)
#define PWM2_STATE_REG1       HI_IO_PWMI_ADDRESS(PWMI_ADRESS_BASE + 0x0054)
#define PWM2_STATE_REG2       HI_IO_PWMI_ADDRESS(PWMI_ADRESS_BASE + 0x0058)

#define PWM3_CFG_REG0      HI_IO_PWMI_ADDRESS(PWMI_ADRESS_BASE + 0x0060)
#define PWM3_CFG_REG1      HI_IO_PWMI_ADDRESS(PWMI_ADRESS_BASE + 0x0064)
#define PWM3_CFG_REG2      HI_IO_PWMI_ADDRESS(PWMI_ADRESS_BASE + 0x0068)
#define PWM3_CTRL_REG       HI_IO_PWMI_ADDRESS(PWMI_ADRESS_BASE + 0x006C)
#define PWM3_STATE_REG0       HI_IO_PWMI_ADDRESS(PWMI_ADRESS_BASE + 0x0070)
#define PWM3_STATE_REG1       HI_IO_PWMI_ADDRESS(PWMI_ADRESS_BASE + 0x0074)
#define PWM3_STATE_REG2       HI_IO_PWMI_ADDRESS(PWMI_ADRESS_BASE + 0x0078)

#define  PWM_WRITE_REG(Addr, Value) ((*(volatile unsigned int *)(Addr)) = (Value))
#define  PWM_READ_REG(Addr)         (*(volatile unsigned int *)(Addr))

//PWM
#define PWM_NUM_MAX         0x04
#define PWM_ENABLE          0x01
#define PWM_DISABLE         0x00

#ifdef CONFIG_HISI_SNAPSHOT_BOOT
#define DEV_NAME  "pwm"
static struct himedia_device s_stPwmDevice;
#endif

static int PWM_DRV_Disable(unsigned char pwm_num)
{
	if(pwm_num >= PWM_NUM_MAX)
	{
		printk("The pwm number is big than the max value!\n");
        return -1;
	}
	switch(pwm_num)
	{
		case 0:
			PWM_WRITE_REG(PWM0_CTRL_REG, PWM_DISABLE);
			break;
		case 1:
			PWM_WRITE_REG(PWM1_CTRL_REG, PWM_DISABLE);
			break;
		case 2:
			PWM_WRITE_REG(PWM2_CTRL_REG, PWM_DISABLE);
			break;
        case 3:
			PWM_WRITE_REG(PWM3_CTRL_REG, PWM_DISABLE);
			break;
        default:
			break;
	}

	return 0;
}


int PWM_DRV_Write(unsigned char pwm_num,unsigned short duty, unsigned short period,unsigned char enable)
{
	if(pwm_num >= PWM_NUM_MAX)
	{
		printk("The pwm number is big than the max value!\n");
        return -1;
	}
	if(enable)
	{

		switch(pwm_num)
		{
			case 0:
				PWM_WRITE_REG(PWM0_CTRL_REG,PWM_DISABLE);

				PWM_WRITE_REG(PWM0_CFG_REG0,period);
				PWM_WRITE_REG(PWM0_CFG_REG1,duty);
				PWM_WRITE_REG(PWM0_CFG_REG2,10);//pwm output number

				PWM_WRITE_REG(PWM0_CTRL_REG,(1<<2|PWM_ENABLE));// keep the pwm always output;
				//printk("The PWMI0 state %x\n",PWM_READ_REG(PWM0_STATE_REG));
				break;

			case 1:
				PWM_WRITE_REG(PWM1_CTRL_REG,PWM_DISABLE);

				PWM_WRITE_REG(PWM1_CFG_REG0,period);
				PWM_WRITE_REG(PWM1_CFG_REG1,duty);
				PWM_WRITE_REG(PWM1_CFG_REG2,10);//pwm output number

				PWM_WRITE_REG(PWM1_CTRL_REG,(1<<2|PWM_ENABLE));// keep the pwm always output;
				//printk("The PWMI1 state %x\n",PWM_READ_REG(PWM1_STATE_REG));
				break;

			case 2:
				PWM_WRITE_REG(PWM2_CTRL_REG,PWM_DISABLE);

				PWM_WRITE_REG(PWM2_CFG_REG0,period);
				PWM_WRITE_REG(PWM2_CFG_REG1,duty);
				PWM_WRITE_REG(PWM2_CFG_REG2,10);//pwm output number

				PWM_WRITE_REG(PWM2_CTRL_REG,(1<<2|PWM_ENABLE));// keep the pwm always output;
				//printk("The PWMI2 state %x\n",PWM_READ_REG(PWM2_STATE_REG));
				break;

            case 3:
				PWM_WRITE_REG(PWM3_CTRL_REG,PWM_DISABLE);

				PWM_WRITE_REG(PWM3_CFG_REG0,period);
				PWM_WRITE_REG(PWM3_CFG_REG1,duty);
				PWM_WRITE_REG(PWM3_CFG_REG2,10);//pwm output number

				PWM_WRITE_REG(PWM3_CTRL_REG,(1<<2|PWM_ENABLE));// keep the pwm always output;
				//printk("The PWMI3 state %x\n",PWM_READ_REG(PWM3_STATE_REG));
				break;

			default:
				PWM_WRITE_REG(PWM0_CTRL_REG,PWM_DISABLE);

				PWM_WRITE_REG(PWM0_CFG_REG0,period);
				PWM_WRITE_REG(PWM0_CFG_REG1,duty);
				PWM_WRITE_REG(PWM0_CFG_REG2,10);//pwm output number

				PWM_WRITE_REG(PWM0_CTRL_REG,(1<<2|PWM_ENABLE));// keep the pwm always output;
				//printk("The PWMII0 state %x\n",PWM_READ_REG(PWM0_STATE_REG));
				break;
		}
	}
	else
	{
		PWM_DRV_Disable(pwm_num);
	}


	return 0;
}

/* file operation                                                           */

int PWM_Open(struct inode * inode, struct file * file)
{
   return 0 ;

}

int  PWM_Close(struct inode * inode, struct file * file)
{
    return 0;
}

static long PWM_Ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
    PWM_DATA_S __user *argp = (PWM_DATA_S __user*)arg;

	unsigned char  PwmNum;
    unsigned int Duty;
    unsigned int Period;
	unsigned char  enable;

    switch (cmd)
    {
        case PWM_CMD_WRITE:
        {
            PWM_DATA_S pwm_data;

            if (copy_from_user(&pwm_data, argp, sizeof(PWM_DATA_S)))
            {
                return -EFAULT;
            }

            PwmNum  = pwm_data.pwm_num;
            Duty    = pwm_data.duty;
            Period  = pwm_data.period;
            enable  = pwm_data.enable;

            PWM_DRV_Write(PwmNum,Duty,Period,enable);
            break;
        }

        case PWM_CMD_READ:
        {
		    break;
        }

        default:
        {
            printk("invalid ioctl command!\n");
            return -ENOIOCTLCMD;
        }
    }

    return 0 ;
}

#ifdef CONFIG_HISI_SNAPSHOT_BOOT
static int PWM_freeze(struct himedia_device *pdev)
{
    printk(KERN_DEBUG "%s  %d\n", __FUNCTION__, __LINE__);
    return 0;
}

static int PWM_restore(struct himedia_device *pdev)
{
    printk(KERN_DEBUG "%s  %d\n", __FUNCTION__, __LINE__);
    return 0;
}
#endif

static struct file_operations pwm_fops =
{
    .owner      = THIS_MODULE,
    .unlocked_ioctl = PWM_Ioctl  ,
    .open       = PWM_Open   ,
    .release    = PWM_Close  ,
};

#ifdef CONFIG_HISI_SNAPSHOT_BOOT
struct himedia_ops stPwmDrvOps =
{
    .pm_freeze = PWM_freeze,
    .pm_restore  = PWM_restore
};
#else
static struct miscdevice pwm_dev =
{
    .minor   = MISC_DYNAMIC_MINOR,
    .name    = "pwm"    ,
    .fops    = &pwm_fops,
};
#endif

/* module init and exit                                                     */

static int __init pwm_init(void)
{
    int     ret;

    reg_pwmI_base_va = (void __iomem*)ioremap_nocache(PWMI_ADRESS_BASE, 0x10000);

#ifdef CONFIG_HISI_SNAPSHOT_BOOT
    snprintf(s_stPwmDevice.devfs_name, sizeof(s_stPwmDevice.devfs_name), DEV_NAME);

    s_stPwmDevice.minor  = HIMEDIA_DYNAMIC_MINOR;
    s_stPwmDevice.fops   = &pwm_fops;
    s_stPwmDevice.drvops = &stPwmDrvOps;
    s_stPwmDevice.owner  = THIS_MODULE;

    ret = himedia_register(&s_stPwmDevice);
    if (ret)
    {
        printk("register i2c device failed with %#x!\n", ret);
        return -1;
    }
#else
    ret = misc_register(&pwm_dev);
    if (ret != 0)
    {
        printk("register i2c device failed with %#x!\n", ret);
        return -1;
    }
#endif

    printk(KERN_INFO "load pwm.ko ...OK!\n");
    return 0;
}

static void __exit pwm_exit(void)
{
    int i;

    for (i = 0; i < PWM_NUM_MAX; i++)
    {
        PWM_DRV_Disable(i);
    }

    iounmap(reg_pwmI_base_va);
    reg_pwmI_base_va = NULL;

#ifdef CONFIG_HISI_SNAPSHOT_BOOT
    himedia_unregister(&s_stPwmDevice);
#else
    misc_deregister(&pwm_dev);
#endif
    printk(KERN_INFO "unload pwm.ko ...OK!\n");
}


module_init(pwm_init);
module_exit(pwm_exit);

MODULE_DESCRIPTION("PWM Driver");
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Hisilicon");

