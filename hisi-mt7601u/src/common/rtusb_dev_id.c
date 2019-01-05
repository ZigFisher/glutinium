/*
 *************************************************************************
 * Ralink Tech Inc.
 * 5F., No.36, Taiyuan St., Jhubei City,
 * Hsinchu County 302,
 * Taiwan, R.O.C.
 *
 * (c) Copyright 2002-2010, Ralink Technology, Inc.
 *
 * This program is free software; you can redistribute it and/or modify  *
 * it under the terms of the GNU General Public License as published by  *
 * the Free Software Foundation; either version 2 of the License, or     *
 * (at your option) any later version.                                   *
 *                                                                       *
 * This program is distributed in the hope that it will be useful,       *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 * GNU General Public License for more details.                          *
 *                                                                       *
 * You should have received a copy of the GNU General Public License     *
 * along with this program; if not, write to the                         *
 * Free Software Foundation, Inc.,                                       *
 * 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 *                                                                       *
 *************************************************************************/


#define RTMP_MODULE_OS

/*#include "rt_config.h"*/
#include "rtmp_comm.h"
#include "rt_os_util.h"
#include "rt_os_net.h"


/* module table */
USB_DEVICE_ID rtusb_dev_id[] = {
#ifdef RT6570
	{USB_DEVICE(0x148f,0x6570)}, /* Ralink 6570 */
#endif /* RT6570 */
	{USB_DEVICE(0x148f, 0x7650)}, /* MT7650 */
#ifdef MT7601U
	{USB_DEVICE(0x148f,0x6370)}, /* Ralink 6370 */
	{USB_DEVICE(0x148f,0x7601)}, /* MT 6370 */
    {USB_DEVICE(0x148f,0x760b)}, /* 360 wifi */
    {USB_DEVICE(0x2a5f,0x1000)}, /* Tencent wifi QQ */
    {USB_DEVICE(0x2955,0x1003)}, /* nuomi wifi */
#endif /* MT7601U */
	{ }/* Terminating entry */
};

INT const rtusb_usb_id_len = sizeof(rtusb_dev_id) / sizeof(USB_DEVICE_ID);
MODULE_DEVICE_TABLE(usb, rtusb_dev_id);
