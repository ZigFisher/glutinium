/******************************************************************************

  Copyright (C), 2001-2011, Huawei Tech. Co., Ltd.

 ******************************************************************************
  File Name     : sample_ist.c
  Version       : Initial Draft
  Author        : c00298856
  Created       : 2015/9/21
  Last Modified :
  Description   : a Isp Sync Task sample
  Function List :
              SampleIst_Close
              SampleIst_Ioctl
              SampleIst_Open
              sample_ist_exit
              sample_ist_init
  History       :
  1.Date        : 2015/9/21
    Author      : c00298856
    Modification: Created file

******************************************************************************/

#include <linux/module.h>
#include <linux/errno.h>
#include <linux/miscdevice.h>
#include <linux/fcntl.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/proc_fs.h>
#include <linux/workqueue.h>

#include <asm/uaccess.h>
#include <asm/io.h>

#include "sample_ist.h"
#include "hi_common.h"
#include "isp_ext.h"

#define MAX_TEST_NODES 6

HI_S32 sync_call_back(HI_U64 u64Data);

ISP_SYNC_TASK_NODE_S syncNode[MAX_TEST_NODES] = {
    {
        .enMethod = ISP_SYNC_TSK_METHOD_HW_IRQ,
        .pfnIspSyncTskCallBack = sync_call_back,
        .u64Data = 0,
        .pszId = "hw_0"
    },
    {
        .enMethod = ISP_SYNC_TSK_METHOD_HW_IRQ,
        .pfnIspSyncTskCallBack = sync_call_back,
        .u64Data = 1,
        .pszId = "hw_1"
    },
    {
        .enMethod = ISP_SYNC_TSK_METHOD_TSKLET,
        .pfnIspSyncTskCallBack = sync_call_back,
        .u64Data = 2,
        .pszId = "tsklt_0"
    },

    {
        .enMethod = ISP_SYNC_TSK_METHOD_TSKLET,
        .pfnIspSyncTskCallBack = sync_call_back,
        .u64Data = 3,
        .pszId = "tsklt_1"
    },
    {
        .enMethod = ISP_SYNC_TSK_METHOD_WORKQUE,
        .pfnIspSyncTskCallBack = sync_call_back,
        .u64Data = 4,
        .pszId = "wq_0"
    },
    {
        .enMethod = ISP_SYNC_TSK_METHOD_WORKQUE,
        .pfnIspSyncTskCallBack = sync_call_back,
        .u64Data = 5,
        .pszId = "wq_1"
    } };

HI_S32 sync_call_back(HI_U64 u64Data)
{
    int data = u64Data;

    printk("%d\n", data);
    return 0;
}


//------------------------------------------------------------------------------------------


/* file operation */

int SampleIst_Open(struct inode* inode, struct file* file)
{
    return 0 ;

}

int SampleIst_Close(struct inode* inode, struct file* file)
{
    return 0;
}

static long SampleIst_Ioctl(struct file* file, unsigned int cmd, unsigned long arg)
{
    int __user* argp = (int __user*)arg;
    int node_index = *argp;

    if (node_index >= MAX_TEST_NODES)
    {
        return -1;
    }

    switch (cmd)
    {
        case SAMPLE_IST_ADD_NODE:
            hi_isp_sync_task_register(0, &syncNode[node_index]);
        break;

        case SAMPLE_IST_DEL_NODE:
            if (HI_FAILURE == hi_isp_sync_task_unregister(0, &syncNode[node_index]))
            {
                printk("del node err %d\n", node_index);
            }
        break;

        default:
        {
            printk("invalid ioctl command!\n");
            return -ENOIOCTLCMD;
        }
    }

    return 0 ;
}

static struct file_operations sample_ist_fops =
{
    .owner      = THIS_MODULE,
    .unlocked_ioctl = SampleIst_Ioctl  ,
    .open       = SampleIst_Open   ,
    .release    = SampleIst_Close  ,
};

static struct miscdevice sample_ist_dev =
{
    .minor   = MISC_DYNAMIC_MINOR,
    .name    = "sample_ist"    ,
    .fops    = &sample_ist_fops,
};


/* module init and exit */

static int __init sample_ist_init(void)
{
    int  i, ret;

    ret = misc_register(&sample_ist_dev);
    if (ret != 0)
    {
        printk("register sample_ist device failed with %#x!\n", ret);
        return -1;
    }

    for (i = 0; i < MAX_TEST_NODES; i++)
    {
        hi_isp_sync_task_register(0, &syncNode[i]);
    }

	printk(KERN_INFO "load sample_ist.ko ...OK!\n");
    return 0;
}

static void __exit sample_ist_exit(void)
{
    int i;

    misc_deregister(&sample_ist_dev);

    for (i = 0; i < MAX_TEST_NODES; i++)
    {
        hi_isp_sync_task_unregister(0, &syncNode[i]);
    }
	printk(KERN_INFO "unload sample_ist.ko ...OK!\n");
}


module_init(sample_ist_init);
module_exit(sample_ist_exit);

MODULE_DESCRIPTION("sample of isp sync task Driver");
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Hisilicon");

