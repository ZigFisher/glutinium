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


#ifndef __RT3290_H__
#define __RT3290_H__

#ifdef RT3290

#error "For RT3290, you should define the compile flag -DRTMP_PCI_SUPPORT"

#error "For RT3290, you should define the compile flag -DRTMP_MAC_PCI"

#error "For RT3290, you should define the compile flag -DRTMP_RF_RW_SUPPORT"

#error "For RT3290, you should define the compile flag -DRT30xx"

#include "chip/mac_pci.h"
#include "chip/rt30xx.h"

struct _RTMP_ADAPTER;


#define NIC3290_PCIe_DEVICE_ID 0x3290


#define RT3290_CHECK_SW_EEP_BUSY(pAd)	\
{	\
	UINT32 _val, _cnt = 0;	\
	do {	\
		RTMP_IO_FORCE_READ32(pAd, WLAN_FUN_INFO, &_val);	\
		if ((_val & 0x80000000) == 0 || (_val == 0xffffffff))	\
			break;	\
		_cnt++;	\
		DBGPRINT_ERR(("RT3290: EEP is busy!!!! BusyCnt%d : fail\n",  _cnt));	\
		RTMPusecDelay(500);	\
	} while (_cnt<300);	\
}	\

#define RT3290_CURRENT_LEAKAGE(_pAd,_A,_e)	\
{	\
	if (IS_RT3290(_pAd))	\
	{	\
		UINT32 btFunInfo, _val = 0;	\
		RTMP_IO_FORCE_READ32(_pAd, _A, &_val);	\
		if (_e)	\
			_val &= ~(EESK|EEDI);		\
		else	\
		{	\
			_val &= ~(EESK);		\
			_val |= EEDI;			\
		}	\
		RT3290_CHECK_SW_EEP_BUSY(_pAd);	\
		RTMP_IO_FORCE_READ32(_pAd, BT_FUN_INFO, &btFunInfo);	\
		btFunInfo |= 0x80000000;	\
		RTMP_IO_FORCE_WRITE32(_pAd, BT_FUN_INFO, btFunInfo);	\
		\
		RTMP_IO_FORCE_WRITE32(_pAd, _A, _V);	\
		\
		btFunInfo &= ~(0x80000000);	\
		RTMP_IO_FORCE_WRITE32(_pAd, BT_FUN_INFO, btFunInfo);	\
	}	\
}


VOID MlmeAntSelection(
	IN struct _RTMP_ADAPTER *pAd,
	IN ULONG	 AccuTxTotalCnt,
	IN ULONG	 TxErrorRatio,
	IN ULONG TxSuccess,
	IN CHAR Rssi);

INT RT3290_eeprom_access_grant(
	IN struct _RTMP_ADAPTER *pAd,
	IN BOOLEAN bGetCtrl);
	
VOID RTMP_MAC_PWRSV_EN(
	IN struct _RTMP_ADAPTER *pAd,
	IN BOOLEAN EnterIdle,
	IN BOOLEAN use40M);

VOID RTMPEnableWlan(
	IN struct _RTMP_ADAPTER *pAd,
	IN BOOLEAN bOn,
	IN BOOLEAN bResetWLAN);

VOID RT3290_Init(
	IN struct _RTMP_ADAPTER *pAd);

#endif /* RT3290 */

#endif /* __RT5390_H__ */

