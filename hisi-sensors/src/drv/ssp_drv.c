/*  ssp_sony.c
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
 *      21-April-2006 create this file
 *      21- June-2016 support multi device, and adapt for HuaweiLite
 */

#include <linux/kernel.h>
#include <linux/spinlock.h>
#include <linux/delay.h>

#ifndef __HuaweiLite__
#include <linux/version.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/errno.h>
#include <linux/fcntl.h>
#include <linux/mm.h>
#include <linux/proc_fs.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/init.h>
#include <asm/uaccess.h>
#include <asm/io.h>

#include <linux/miscdevice.h>

#include <linux/proc_fs.h>
#include <linux/poll.h>

#include <asm/bitops.h>
#include <asm/uaccess.h>
#include <asm/irq.h>

#include <linux/moduleparam.h>
#include <linux/ioport.h>
#include <linux/interrupt.h>

#define SSP_DBG_ERR     KERN_ALERT
#define SSP_DBG_INFO    KERN_DEBUG
#define SSP_TRACE(level, fmt, ...) printk(level fmt, ##__VA_ARGS__)

#else

#ifndef __iomem
#define __iomem
#endif

#define SSP_DBG_ERR     "err"
#define SSP_DBG_INFO    "inf"
#define SSP_TRACE(level, fmt, ...)   dprintf(fmt, ##__VA_ARGS__)
#endif

#include "sensor.h"
#include "hi_comm_isp.h"

#define DEV_NAME "ssp"
#define SSP_DEV_NUM         2

#define  ssp_readw(addr,ret)			(ret =(*(volatile unsigned int *)(addr)))
#define  ssp_writew(addr,value)			((*(volatile unsigned int *)(addr)) = (value))

#define  HI_REG_READ(addr,ret)			(ret =(*(volatile unsigned int *)(addr)))
#define  HI_REG_WRITE(addr,value)		((*(volatile unsigned int *)(addr)) = (value))	

#define SSP_BASE	0x12120000
#define SSP_SIZE	0x10000	             // 64KB
#define SSP_INT		41                  // Interrupt No.

static void __iomem *reg_ssp_base_va[SSP_DEV_NUM];
#define IO_ADDRESS_VERIFY(x, spi_no) (reg_ssp_base_va[spi_no] + (x))

/* SSP register definition .*/
#define SSP_CR0(ssp_no)     IO_ADDRESS_VERIFY(0x00, ssp_no)
#define SSP_CR1(ssp_no)     IO_ADDRESS_VERIFY(0x04, ssp_no)
#define SSP_DR(ssp_no)      IO_ADDRESS_VERIFY(0x08, ssp_no)
#define SSP_SR(ssp_no)      IO_ADDRESS_VERIFY(0x0C, ssp_no)
#define SSP_CPSR(ssp_no)    IO_ADDRESS_VERIFY(0x10, ssp_no)
#define SSP_IMSC(ssp_no)    IO_ADDRESS_VERIFY(0x14, ssp_no)
#define SSP_RIS(ssp_no)     IO_ADDRESS_VERIFY(0x18, ssp_no)
#define SSP_MIS(ssp_no)     IO_ADDRESS_VERIFY(0x1C, ssp_no)
#define SSP_ICR(ssp_no)     IO_ADDRESS_VERIFY(0x20, ssp_no)
#define SSP_DMACR(ssp_no)   IO_ADDRESS_VERIFY(0x24, ssp_no)

//unsigned int ssp_dmac_rx_ch,ssp_dmac_tx_ch;

static spinlock_t g_stSspSonyLock;
#define SSP_SPIN_LOCK_INIT()    spin_lock_init(&g_stSspSonyLock)
#define SSP_SPIN_LOCK(flags)    spin_lock_irqsave(&g_stSspSonyLock, flags)
#define SSP_SPIN_UNLOCK(flags)  spin_unlock_irqrestore(&g_stSspSonyLock, flags)

#define SSP_GET_FUNCTION(num, func) sensor_ssp_##func_##num

static void hi_ssp_disable(unsigned int ssp_no)
{
    int ret = 0;
    ssp_readw(SSP_CR1(ssp_no), ret);
    ret = ret & (~(0x1 << 1));
    ssp_writew(SSP_CR1(ssp_no), ret);
}

/*
 * set SSP frame form routine.
 *
 * @param framemode: frame form
 * 00: Motorola SPI frame form.
 * when set the mode,need set SSPCLKOUT phase and SSPCLKOUT voltage level.
 * 01: TI synchronous serial frame form
 * 10: National Microwire frame form
 * 11: reserved
 * @param sphvalue: SSPCLKOUT phase (0/1)
 * @param sp0: SSPCLKOUT voltage level (0/1)
 * @param datavalue: data bit
 * 0000: reserved    0001: reserved    0010: reserved    0011: 4bit data
 * 0100: 5bit data   0101: 6bit data   0110:7bit data    0111: 8bit data
 * 1000: 9bit data   1001: 10bit data  1010:11bit data   1011: 12bit data
 * 1100: 13bit data  1101: 14bit data  1110:15bit data   1111: 16bit data
 *
 * @return value: 0--success; -1--error.
 *
 */
static int hi_ssp_set_frameform(unsigned int ssp_no, unsigned char framemode,unsigned char spo,unsigned char sph,unsigned char datawidth)
{
    int ret = 0;
    ssp_readw(SSP_CR0(ssp_no), ret);
    if(framemode > 3)
    {
        SSP_TRACE(SSP_DBG_ERR, "set frame parameter err.\n");
        return -1;
    }
    ret = (ret & 0xFFCF) | (framemode << 4);
    if((ret & 0x30) == 0)
    {
        if(spo > 1)
        {
            SSP_TRACE(SSP_DBG_ERR, "set spo parameter err.\n");
            return -1;
        }
        if(sph > 1)
        {
            SSP_TRACE(SSP_DBG_ERR, "set sph parameter err.\n");
            return -1;
        }
        ret = (ret & 0xFF3F) | (sph << 7) | (spo << 6);
    }
    if((datawidth > 16) || (datawidth < 4))
    {
        SSP_TRACE(SSP_DBG_ERR, "set datawidth parameter err.\n");
        return -1;
    }
    ret = (ret & 0xFFF0) | (datawidth -1);
    ssp_writew(SSP_CR0(ssp_no), ret);
    return 0;
}

/*
 * set SSP serial clock rate routine.
 *
 * @param scr: scr value.(0-255,usually it is 0)
 * @param cpsdvsr: Clock prescale divisor.(2-254 even)
 *
 * @return value: 0--success; -1--error.
 *
 */
static int hi_ssp_set_serialclock(unsigned int ssp_no, unsigned char scr,unsigned char cpsdvsr)
{
    int ret = 0;
    ssp_readw(SSP_CR0(ssp_no), ret);
    ret = (ret & 0xFF) | (scr << 8);
    ssp_writew(SSP_CR0(ssp_no), ret);
    if((cpsdvsr & 0x1))
    {
        SSP_TRACE(SSP_DBG_ERR, "set cpsdvsr parameter err.\n");
        return -1;
    }
    ssp_writew(SSP_CPSR(ssp_no), cpsdvsr);
    return 0;
}

static int hi_ssp_alt_mode_set(unsigned int ssp_no, int enable)
{
	int ret = 0;
	
	ssp_readw(SSP_CR1(ssp_no), ret);
	if (enable)
	{
		ret = ret & (~0x40);
	}
	else
	{
	    ret = (ret & 0xFF) | 0x40;
	}
	ssp_writew(SSP_CR1(ssp_no),ret);

    return 0;
}


static unsigned int hi_ssp_is_fifo_empty(unsigned int ssp_no, int bSend)
{
    int ret = 0;
    ssp_readw(SSP_SR(ssp_no), ret);

    if (bSend)
    {
        if((ret & 0x1) == 0x1) /* send fifo */
            return 1;
        else
            return 0;
    }
    else
    {
        if((ret & 0x4) == 0x4) /* receive fifo */
            return 0;
        else
            return 1;
    }
}

static void spi_enable(unsigned int ssp_no)
{
    // little endian
    HI_REG_WRITE(SSP_CR1(ssp_no), 0x52);
}

static void spi_disable(unsigned int ssp_no)
{
    HI_REG_WRITE(SSP_CR1(ssp_no), 0x50);
}


// Fsspclkout = Fsspclk/(cpsdvsr*(1+scr))
static int hi_ssp_init_cfg(unsigned int ssp_no)
{
	unsigned char framemode = 0;
	unsigned char spo = 1;
	unsigned char sph = 1;
	unsigned char datawidth = 8 ;
	int ret;

#ifdef HI_FPGA
	unsigned char scr = 1;
	unsigned char cpsdvsr = 2;
#else
	unsigned char scr = 1;  //8
	unsigned char cpsdvsr = 8;
#endif
	
	spi_disable(ssp_no);

	ret = hi_ssp_set_frameform(ssp_no, framemode, spo, sph, datawidth);
    if(ret < 0)
	{
     return ret;
	}

	ret = hi_ssp_set_serialclock(ssp_no, scr, cpsdvsr);
	if(ret < 0)
	{
     return ret;
	}

	// altasens mode, which CS won't be pull high between 16bit data transfer
	hi_ssp_alt_mode_set(ssp_no, 0);

	//hi_ssp_enable(ssp_no);

    return 0;
}


unsigned short ssp_read_alt(unsigned int ssp_no, void *pSensorData)
{
	unsigned int ret = 0;
	unsigned short value = 0;
    unsigned short dontcare = 0x00;
    unsigned long flags;
	unsigned short devaddr, addr;
	unsigned int devaddr_byte_num, regaddr_byte_num, data_byte_num;
	unsigned int len;
	
	ISP_SSP_DATA_S *pstSspData = HI_NULL;
	
	pstSspData = (ISP_SSP_DATA_S *)pSensorData;
	
	devaddr = pstSspData->u32DevAddr & 0xff;
	addr = pstSspData->u32RegAddr;
	devaddr_byte_num = pstSspData->u32DevAddrByteNum;
	regaddr_byte_num = pstSspData->u32RegAddrByteNum;
	data_byte_num = pstSspData->u32DataByteNum;

	len = devaddr_byte_num + regaddr_byte_num + data_byte_num;

    SSP_SPIN_LOCK(flags);

    spi_enable(ssp_no);
	
	if (0 != devaddr_byte_num)
	{
		ssp_writew(SSP_DR(ssp_no), devaddr);
	}

    if (0 != regaddr_byte_num)
    {
		if (2 == regaddr_byte_num)
		{
			unsigned char addr_h, addr_l;

			addr_h = (addr >> 8) & 0xff;
			addr_l = (addr & 0xff);
			ssp_writew(SSP_DR(ssp_no), addr_h);
			ssp_writew(SSP_DR(ssp_no), addr_l);
		}
		else
		{
			ssp_writew(SSP_DR(ssp_no), (addr & 0xff));
		}
    }

	if (0 != data_byte_num)
	{
		ssp_writew(SSP_DR(ssp_no), dontcare);
	}
 
    while (len--)
	{
		while(hi_ssp_is_fifo_empty(ssp_no, 0)){};
		ssp_readw(SSP_DR(ssp_no), ret);
	}

    spi_disable(ssp_no);
	value = (unsigned short)(ret & 0xff);

    SSP_SPIN_UNLOCK(flags);
	
    return value;
}

int ssp_write_alt(unsigned int ssp_no, void *pSensorData)
{
	unsigned int ret;
    unsigned long flags;
	unsigned short devaddr, addr, data;
	unsigned int devaddr_byte_num, regaddr_byte_num, data_byte_num;
	unsigned int len;
	
	ISP_SSP_DATA_S *pstSspData = HI_NULL;
	
	pstSspData = (ISP_SSP_DATA_S *)pSensorData;
	
	devaddr = pstSspData->u32DevAddr & 0xff;
	addr = pstSspData->u32RegAddr;
	data = pstSspData->u32Data & 0xff;
	devaddr_byte_num = pstSspData->u32DevAddrByteNum;
	regaddr_byte_num = pstSspData->u32RegAddrByteNum;
	data_byte_num = pstSspData->u32DataByteNum;
	
	len = devaddr_byte_num + regaddr_byte_num + data_byte_num;

    SSP_SPIN_LOCK(flags);

	spi_enable(ssp_no);

	if (0 != devaddr_byte_num)
	{
		ssp_writew(SSP_DR(ssp_no), devaddr);
	}

    if (0 != regaddr_byte_num)
    {
		if (2 == regaddr_byte_num)
		{
			unsigned char addr_h, addr_l;

			addr_h = (addr >> 8) & 0xff;
			addr_l = (addr & 0xff);
			ssp_writew(SSP_DR(ssp_no), addr_h);
			ssp_writew(SSP_DR(ssp_no), addr_l);
		}
		else
		{
			ssp_writew(SSP_DR(ssp_no), (addr & 0xff));
		}
    }

	if (0 != data_byte_num)
	{
		ssp_writew(SSP_DR(ssp_no), data);
	}
  
	// wait receive fifo has data
	while (len--)
	{
		while(hi_ssp_is_fifo_empty(ssp_no, 0)){};
		ssp_readw(SSP_DR(ssp_no), ret);
	}
	
	spi_disable(ssp_no);
    
    SSP_SPIN_UNLOCK(flags);
    
	return 0;
}

int sensor_ssp_write_0(HI_VOID *pSensorData)
{
	return ssp_write_alt(0, pSensorData);
}

int sensor_ssp_write_1(HI_VOID *pSensorData)
{
	return ssp_write_alt(1, pSensorData);
}

int sensor_ssp_read_0(HI_VOID *pSensorData)
{
	return ssp_read_alt(0, pSensorData);
}

int sensor_ssp_read_1(HI_VOID *pSensorData)
{
	return ssp_read_alt(1, pSensorData);
}

HI_VOID ssp_get_ops(HI_U32 u32BusNum, SENSOR_CTRL_OPS_S *pstCtrlOps)
{
	if (0 == u32BusNum)
	{
		pstCtrlOps->write = sensor_ssp_write_0;
		pstCtrlOps->read  = sensor_ssp_read_0;
	}
	else
	{
		pstCtrlOps->write = sensor_ssp_write_1;
		pstCtrlOps->read  = sensor_ssp_read_1;
	}
}

/*
 * initializes SSP interface routine.
 *
 * @return value:0--success.
 *
 */
HI_S32 ssp_drv_init(HI_U32 u32BusNum)
{
    int ret;
	
	if (u32BusNum >= SSP_DEV_NUM)
	{
		return -1;
	}
    
	reg_ssp_base_va[u32BusNum] = ioremap_nocache((unsigned long)(SSP_BASE + SSP_SIZE * u32BusNum), (unsigned long)SSP_SIZE);
	if (!reg_ssp_base_va[u32BusNum])
	{
		SSP_TRACE(SSP_DBG_ERR, "Kernel: ioremap ssp base failed!\n");
	    return -ENOMEM;
	}

	ret = hi_ssp_init_cfg(u32BusNum);
	if (ret)
	{
		SSP_TRACE(SSP_DBG_ERR, "Debug: ssp initial failed!\n");
		return -1;
	}
    
    SSP_SPIN_LOCK_INIT();
    SSP_TRACE(SSP_DBG_INFO, "Kernel: ssp initial ok!\n");

    return 0;
}

HI_VOID ssp_drv_exit(HI_U32 u32BusNum)
{
	if (u32BusNum >= SSP_DEV_NUM)
	{
		return;
	}

    hi_ssp_disable(u32BusNum);

    iounmap((void*)reg_ssp_base_va[u32BusNum]);
}
