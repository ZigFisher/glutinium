/*  isp/piris.c
 * piris' drv for Forecam NV03105P.IR-B-S
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
#include <linux/miscdevice.h>
#include <linux/fcntl.h>

#include <linux/init.h>
#include <linux/delay.h>
#include <linux/proc_fs.h>
#include <linux/workqueue.h>
#include <linux/hrtimer.h>

#include <asm/uaccess.h>
#include <asm/io.h>
#include <linux/slab.h>

#include "piris.h"
#include "piris_ext.h"

/* Piris use GPIO3_7, GPIO4_0, GPIO4_1, GPIO4_2 */
/* GPIO3's base address is 0x2017 and GPIO4's 0x2018 */
#define PIRISI_ADRESS_BASE     0x20170000

void __iomem* reg_pirisI_base_va = 0;

#define HI_IO_PIRISI_ADDRESS(x)  (reg_pirisI_base_va + ((x)-(PIRISI_ADRESS_BASE)))

#define PIRIS_CFG_REG0        HI_IO_PIRISI_ADDRESS(PIRISI_ADRESS_BASE + 0x0200)
#define PIRIS_CTRL_REG0       HI_IO_PIRISI_ADDRESS(PIRISI_ADRESS_BASE + 0x0400)
#define PIRIS_CFG_REG1        HI_IO_PIRISI_ADDRESS(PIRISI_ADRESS_BASE + 0x10000 + 0x001C)
#define PIRIS_CTRL_REG1       HI_IO_PIRISI_ADDRESS(PIRISI_ADRESS_BASE + 0x10000 + 0x0400)


#define  PIRIS_WRITE_REG(Addr, Value) ((*(volatile unsigned int *)(Addr)) = (Value))
#define  PIRIS_READ_REG(Addr)         (*(volatile unsigned int *)(Addr))


#define MAX(a, b) (a > b? a : b)
#define MIN(a, b) (a < b? a : b)
#define MAX_MOTOR_PAHSE 4
#define PIRIS_PPS 100

static const unsigned char motor_phase_tbl[MAX_MOTOR_PAHSE] = { 0x0, 0x1, 0x2, 0x3};

typedef struct hiPIRIS_DEV
{
    int src_pos;
    int dest_pos;
    unsigned int pps;
    int phase;
    const unsigned char* phase_tbl;
	spinlock_t lock;
    struct semaphore sem;
    struct timer_list timer;
} PIRIS_DEV;

static PIRIS_DEV* p_piris_dev;

DECLARE_COMPLETION(piris_comp);


int piris_gpio_update(int* pPirisPos)
{
    unsigned long u32Flags;

    spin_lock_irqsave(&p_piris_dev->lock, u32Flags);

    p_piris_dev->dest_pos = *pPirisPos;
    /* according to Piris' introduction, it need to be stopped at phase "0" and "2". */
    //p_piris_dev->dest_pos = (p_piris_dev->dest_pos >> 1) << 1;

    p_piris_dev->pps = PIRIS_PPS;
    p_piris_dev->pps = MAX(MIN(p_piris_dev->pps, HZ), 1);
    p_piris_dev->timer.expires = jiffies + HZ / p_piris_dev->pps;

    /* whether piris timer already at the kerbel timer pending list */
    //if (p_piris_dev->timer.entry.next != NULL)
    //{
    //    spin_unlock_irqrestore(&p_piris_dev->lock, u32Flags);
    //    return -1;
    //}

    if (!timer_pending(&p_piris_dev->timer))
    {
        add_timer(&p_piris_dev->timer);
    }

    spin_unlock_irqrestore(&p_piris_dev->lock, u32Flags);

    return 0;
}


#define HYS_STEPS 100

/* first go to the full open iris step, set the full open as origin */
int piris_origin_set(PIRIS_DATA_S* pstPirisData)
{
	int piris_pos;
	unsigned long u32Flags;

	int sign, hys_offset;
	int hys_cnt = 3;	// should be even

	piris_pos = pstPirisData->CurPos;

	piris_gpio_update(&piris_pos);

	// wait for piris origin done
	init_completion(&piris_comp);
	wait_for_completion(&piris_comp);

	/* consider hysteresis effection */
	sign = (pstPirisData->ZeroIsMax)? 1 : -1;
	do
	{
		hys_offset = sign * HYS_STEPS;
		piris_pos += hys_offset;
		piris_gpio_update(&piris_pos);
		wait_for_completion(&piris_comp);
		sign = -sign;
		hys_cnt--;
	} while (hys_cnt);

	msleep(1000);
	piris_pos += pstPirisData->TotalStep * sign;
	piris_gpio_update(&piris_pos);
	wait_for_completion(&piris_comp);

	/* then offset to full open positon */
	spin_lock_irqsave(&p_piris_dev->lock, u32Flags);
	if (pstPirisData->ZeroIsMax == 1)
	{
		p_piris_dev->src_pos  = 0;
		p_piris_dev->dest_pos = 0;
	}
	else
	{
		p_piris_dev->src_pos  = pstPirisData->TotalStep - 1;
		p_piris_dev->dest_pos = pstPirisData->TotalStep - 1;
	}

    spin_unlock_irqrestore(&p_piris_dev->lock, u32Flags);

    return 0;

}


/* go to the full close iris step */
int piris_close_set(PIRIS_DATA_S* pstPirisData)
{
    int piris_pos;
    unsigned long u32Flags;

    piris_pos = pstPirisData->CurPos;

    piris_gpio_update(&piris_pos);

    // wait for piris origin done
    init_completion(&piris_comp);
    wait_for_completion(&piris_comp);

    spin_lock_irqsave(&p_piris_dev->lock, u32Flags);

    if (pstPirisData->ZeroIsMax == 1)
    {
        p_piris_dev->src_pos  = pstPirisData->TotalStep - 1;
        p_piris_dev->dest_pos = pstPirisData->TotalStep - 1;
    }
    else
    {
        p_piris_dev->src_pos  = 0;
        p_piris_dev->dest_pos = 0;
    }

    spin_unlock_irqrestore(&p_piris_dev->lock, u32Flags);

    return 0;

}


/* file operation */

int piris_open(struct inode* inode, struct file* file)
{
    file->private_data = p_piris_dev;

    return 0 ;

}

int  piris_close(struct inode* inode, struct file* file)
{
    return 0;
}

#if 0
static int PIRIS_DRV_Disable(void)
{
    PIRIS_WRITE_REG(PIRIS_CTRL_REG0, 0x80);
    PIRIS_WRITE_REG(PIRIS_CFG_REG0 , 0x0);
    PIRIS_WRITE_REG(PIRIS_CTRL_REG1, 0x7);
    PIRIS_WRITE_REG(PIRIS_CFG_REG1 , 0x0);
	return 0;
}
#endif

int PIRIS_DRV_Write(unsigned char bits)
{
    switch (bits)
    {
        case 0:
            PIRIS_WRITE_REG(PIRIS_CTRL_REG0, 0x80);
            PIRIS_WRITE_REG(PIRIS_CFG_REG0 , 0x80);
            PIRIS_WRITE_REG(PIRIS_CTRL_REG1, 0x7);
            PIRIS_WRITE_REG(PIRIS_CFG_REG1 , 0x2);
            break;

        case 1:
            PIRIS_WRITE_REG(PIRIS_CTRL_REG0, 0x80);
            PIRIS_WRITE_REG(PIRIS_CFG_REG0 , 0x0);
            PIRIS_WRITE_REG(PIRIS_CTRL_REG1, 0x7);
            PIRIS_WRITE_REG(PIRIS_CFG_REG1 , 0x3);
            break;

        case 2:
            PIRIS_WRITE_REG(PIRIS_CTRL_REG0, 0x80);
            PIRIS_WRITE_REG(PIRIS_CFG_REG0 , 0x0);
            PIRIS_WRITE_REG(PIRIS_CTRL_REG1, 0x7);
            PIRIS_WRITE_REG(PIRIS_CFG_REG1 , 0x5);
            break;

        case 3:
            PIRIS_WRITE_REG(PIRIS_CTRL_REG0, 0x80);
            PIRIS_WRITE_REG(PIRIS_CFG_REG0 , 0x80);
            PIRIS_WRITE_REG(PIRIS_CTRL_REG1, 0x7);
            PIRIS_WRITE_REG(PIRIS_CFG_REG1 , 0x4);
            break;

        default:
            break;
    }


    return 0;
}


static long piris_ioctl(struct file* file, unsigned int cmd, unsigned long arg)
{
    PIRIS_DEV* pstPirisDev = (PIRIS_DEV*) file->private_data;

    int ret;
    int PirisPos;
    PIRIS_DATA_S piris_data;
    PIRIS_STATUS_E  PirisStatus;

    int err = 0;

    if (_IOC_TYPE(cmd) != PIRIS_IOC_MAGIC)
    {
        return -ENOTTY;
    }
    if (_IOC_NR(cmd) > PIRIS_IOC_MAXNR)
    {
        return -ENOTTY;
    }

    if (_IOC_DIR(cmd) & _IOC_READ)
    {
        err = !access_ok(VERIFY_WRITE, (void __user*)arg, _IOC_SIZE(cmd));
    }
    else if (_IOC_DIR(cmd) & _IOC_WRITE)
    {
        err =  !access_ok(VERIFY_READ, (void __user*)arg, _IOC_SIZE(cmd));
    }

    if (err)
    {
        return -EFAULT;
    }

    // lock pstPirisDev
    if (down_interruptible(&pstPirisDev->sem))
    {
        return -ERESTARTSYS;
    }

    switch (cmd)
    {
        case PIRIS_SET_ACT_ARGS:
            ret = copy_from_user(&PirisPos, arg, sizeof(int));
            if (ret) {
                return -EFAULT;
            }
            piris_gpio_update(&PirisPos);
            break;

        case PIRIS_SET_ORGIN:
            ret = copy_from_user(&piris_data, arg, sizeof(PIRIS_DATA_S));
            if (ret) {
                return -EFAULT;
            }
            piris_origin_set(&piris_data);
            break;

        case PIRIS_SET_CLOSE:
            ret = copy_from_user(&piris_data, arg, sizeof(PIRIS_DATA_S));
            if (ret) {
                return -EFAULT;
            }
            piris_close_set(&piris_data);
            break;

        case PIRIS_GET_STATUS:
            if (pstPirisDev->dest_pos != pstPirisDev->src_pos)
            {
                PirisStatus = PIRIS_BUSY;
            }
            else
            {
                PirisStatus = PIRIS_IDLE;
            }

            ret = copy_to_user(arg, &PirisStatus, sizeof(PIRIS_STATUS_E));
            if (ret)
            {
                return -EFAULT;
            }

            break;

        default:  /* redundant, as cmd was checked against MAXNR */
            break;
    }

    // unlock pstPirisDev
    up(&pstPirisDev->sem);

    return 0 ;
}


static struct file_operations piris_fops =
{
    .owner      = THIS_MODULE,
    .unlocked_ioctl = piris_ioctl  ,
    .open       = piris_open   ,
    .release    = piris_close  ,
};

static struct miscdevice gstPirisDev =
{
    .minor   = MISC_DYNAMIC_MINOR,
    .name    = "piris"    ,
    .fops    = &piris_fops,
};

void piris_timer_cb(unsigned long arg)
{
    int sign = 1;
    unsigned char bits;
    unsigned long u32Flags;

    PIRIS_DEV* pstPirisDev = (PIRIS_DEV*)arg;

    spin_lock_irqsave(&pstPirisDev->lock, u32Flags);
    if (pstPirisDev->src_pos == pstPirisDev->dest_pos)
    {
        spin_unlock_irqrestore(&pstPirisDev->lock, u32Flags);
        return ;
    }

    sign = (pstPirisDev->dest_pos - pstPirisDev->src_pos < 0) ? -1 : 1;
    pstPirisDev->src_pos += sign;

    // close iris: 0->1->2->3->0;    open iris: 3->2->1->0->3
    pstPirisDev->phase = (pstPirisDev->phase + MAX_MOTOR_PAHSE + sign) % MAX_MOTOR_PAHSE;
    bits = pstPirisDev->phase_tbl[pstPirisDev->phase];

    PIRIS_DRV_Write(bits);

    if (pstPirisDev->dest_pos == pstPirisDev->src_pos)
    {
        complete(&piris_comp);
    }

    pstPirisDev->timer.expires = jiffies + HZ / pstPirisDev->pps;
    if (!timer_pending(&pstPirisDev->timer))
    {
        add_timer(&pstPirisDev->timer);
    }

    //printk("%s, pos :%d\n", __FUNCTION__, pstPirisDev->src_pos);
    spin_unlock_irqrestore(&pstPirisDev->lock, u32Flags);

    return ;
}


static int hi_piris_isp_register(void)
{
    ISP_PIRIS_CALLBACK_S stPirisCb = {0};

    stPirisCb.pfn_piris_gpio_update = (HI_S32 (*)(HI_S32))piris_gpio_update;
    if (CKFN_ISP_RegisterPirisCallBack())
    {
        CALL_ISP_RegisterPirisCallBack(0, &stPirisCb);
    }
    else
    {
        printk("register piris_gpio_write_callback to isp failed, hi_piris init is failed!\n");
        return -1;
    }

    return 0;
}


static int hi_piris_isp_unregister(void)
{
    ISP_PIRIS_CALLBACK_S stPirisCb = {0};

    if (CKFN_ISP_RegisterPirisCallBack())
    {
        CALL_ISP_RegisterPirisCallBack(0, &stPirisCb);
    }
    else
    {
        printk("unregister piris_gpio_write_callback to isp failed!\n");
        return -1;
    }

    return 0;
}


/* module init and exit */
static int __init piris_init(void)
{
    int ret;

    reg_pirisI_base_va = ioremap_nocache(PIRISI_ADRESS_BASE, 0x10000);
    if (NULL == reg_pirisI_base_va)
    {
        printk("reg_pirisI_base_va ioremap failed.\n");
        return -1;
    }

    p_piris_dev = kmalloc(sizeof(PIRIS_DEV), GFP_KERNEL);

    if (!p_piris_dev)
    {
        printk("p_piris_dev kmalloc failed.\n");
        iounmap(reg_pirisI_base_va);
        return -1;
    }
    memset(p_piris_dev, 0x0, sizeof(PIRIS_DEV));

    spin_lock_init(&p_piris_dev->lock);
    sema_init(&p_piris_dev->sem, 1);
    init_completion(&piris_comp);

    // init timer
    init_timer(&p_piris_dev->timer);
    p_piris_dev->timer.function = piris_timer_cb;
    p_piris_dev->timer.data = (unsigned long)p_piris_dev;
    p_piris_dev->timer.expires = jiffies + HZ; /* one second */
    p_piris_dev->phase_tbl = motor_phase_tbl;

    ret = misc_register(&gstPirisDev);

    if (ret != 0)
    {
        printk("register piris device failed with %#x!\n", ret);
        del_timer(&p_piris_dev->timer);
        kfree(p_piris_dev);
        iounmap(reg_pirisI_base_va);
        return -1;
    }

    hi_piris_isp_register();

    printk(KERN_INFO "load piris.ko ...OK!\n");
    return 0;
}

static void __exit piris_exit(void)
{
    hi_piris_isp_unregister();
    misc_deregister(&gstPirisDev);
    del_timer(&p_piris_dev->timer);
    kfree(p_piris_dev);

    iounmap(reg_pirisI_base_va);

    printk(KERN_INFO "unload piris.ko ...OK!\n");
}


module_init(piris_init);
module_exit(piris_exit);

MODULE_DESCRIPTION("piris driver");
MODULE_LICENSE("GPL");
MODULE_AUTHOR("hisilicon");

