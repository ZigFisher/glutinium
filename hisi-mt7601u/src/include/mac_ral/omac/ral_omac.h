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


#ifndef __RAL_OMAC_H__
#define __RAL_OMAC_H__

#include "rtmp_type.h"

#include "mac_ral/omac/ral_omac_txwi.h"
#include "mac_ral/omac/ral_omac_rxwi.h"

/*
	TXINFO
*/

/*
	TXINFO fields defintion:
	
	USBDMATxPktLen[b0~b14]:
		Total bytes of all sub-frame. ONLY for USB bulk Aggregation
	IPOffset[b15~b18]:
		Start byte of IP packet. The base address is from TXINFO.
		0: header will be parsed by hardware.
		This field is like backdoor.
		For AMSDU, this field is useless

	TCPOffset[b19~b23]:
		Start byte of TCP packet from IP packet The base address is IP header.
		0: header will be parsed by hardware.
		This field is like backdoor.
		For AMSDU, this field is useless

	WIV[b24]:
		Wireless Info Valid. 
		1: if Driver already fill WI
		0: if DMA needs to copy WI to correctposition

	QSEL[b25~b26]:
		Select on-chip FIFO ID for 2nd-stage output scheduler.
		0:MGMT, 1:HCCA 2:EDCA

	SwUseLastRound[b27]:
		Software used for USB-based chipset, reserved for other interfaces.

	uso[b28]:
		UDP checksum enable. 
		1: indicate this packet needs to do UDP checksum

	cso[b29]:
		Checksum offload. 
		1: indicate this packet needs to do checksum

	USBDMANextVLD[b30]:
		Used for USB-based chipset, reserved for other interfaces.
		Used ONLY in USB bulk Aggregation, host driver info DMA current 
		frame is not he last frame in current Tx queue

	USBDMATxburst[b31]:
		force DMA transmit frame from current selected endpoint
*/
#ifdef RT_BIG_ENDIAN
typedef	struct _TXINFO_OMAC {
	UINT32 USBDMATxburst:1;
	UINT32 USBDMANextVLD:1;
	UINT32 cso:1;
	UINT32 uso:1;
#ifdef USB_BULK_BUF_ALIGMENT
	UINT32 bFragLasAlignmentsectiontRound:1;
#else
	UINT32 SwUseLastRound:1;
#endif /* USB_BULK_BUF_ALIGMENT */
	UINT32 QSEL:2;
	UINT32 WIV:1;
	UINT32 TCPOffset:5;
	UINT32 IPOffset:4;
	UINT32 USBDMATxPktLen:15;
}TXINFO_OMAC;
#else
typedef	struct _TXINFO_OMAC {
	UINT32 USBDMATxPktLen:15;
	UINT32 IPOffset:4;
	UINT32 TCPOffset:5;
	UINT32 WIV:1;
	UINT32 QSEL:2;
#ifdef USB_BULK_BUF_ALIGMENT
	UINT32 bFragLasAlignmentsectiontRound:1;
#else
	UINT32 SwUseLastRound:1;
#endif /* USB_BULK_BUF_ALIGMENT */
	UINT32 uso:1;
	UINT32 cso:1;
	UINT32 USBDMANextVLD:1;
	UINT32 USBDMATxburst:1;
}TXINFO_OMAC;
#endif /* RT_BIG_ENDIAN */


#define TxInfoWIV			txinfo_omac.WIV
#define TxInfoQSEL			txinfo_omac.QSEL
#define TxInfoPktLen			txinfo_omac.USBDMATxPktLen
#define TxInfoSwLstRnd		txinfo_omac.SwUseLastRound
#define TxInfoUDMATxburst	txinfo_omac.USBDMATxburst
#define TxInfoUDMANextVld	txinfo_omac.USBDMANextVLD
#define TxInfoIPOffset		txinfo_omac.IPOffset
#define TxInfoTCPOffset		txinfo_omac.TCPOffset
#define TxInfoCSO			txinfo_omac.cso
#define TxInfoUSO			txinfo_omac.uso




/* ================================================================================= */
/* Register format */
/* ================================================================================= */

#define GPIO_CTRL_CFG	0x0228
#define MCU_CMD_CFG	0x022c


#define PAIRWISE_KEY_TABLE_BASE     0x4000      /* 32-byte * 256-entry =  -byte */
#define HW_KEY_ENTRY_SIZE           0x20

#define PAIRWISE_IVEIV_TABLE_BASE     0x6000      /* 8-byte * 256-entry =  -byte */
#define MAC_IVEIV_TABLE_BASE     0x6000      /* 8-byte * 256-entry =  -byte */
#define HW_IVEIV_ENTRY_SIZE   8

#define MAC_WCID_ATTRIBUTE_BASE     0x6800      /* 4-byte * 256-entry =  -byte */
#define HW_WCID_ATTRI_SIZE   4

#define SHARED_KEY_TABLE_BASE       0x6c00      /* 32-byte * 16-entry = 512-byte */
#define SHARED_KEY_MODE_BASE       0x7000      /* 32-byte * 16-entry = 512-byte */

#define HW_SHARED_KEY_MODE_SIZE   4
#define SHAREDKEYTABLE			0
#define PAIRWISEKEYTABLE			1

/* This resgiser is ONLY be supported for RT3883 or later.
   It conflicted with BCN#0 offset of previous chipset. */
#define WAPI_PN_TABLE_BASE		0x7800		
#define WAPI_PN_ENTRY_SIZE   		8

#endif /* __RAL_OMAC_H__ */

