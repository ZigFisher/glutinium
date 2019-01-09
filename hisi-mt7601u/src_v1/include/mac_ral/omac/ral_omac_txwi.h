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


#ifndef __RAL_OMAC_TXWI_H__
#define __RAL_OMAC_TXWI_H__


#include "rtmp_type.h"


#ifdef RT_BIG_ENDIAN
typedef	struct GNU_PACKED _TXWI_OMAC {
	/* Word 0 */
	UINT32		PHYMODE:2;
	UINT32		iTxBF:1; /* iTxBF enable */
	UINT32		Sounding:1; /* Sounding enable */
	UINT32		eTxBF:1; /* eTxBF enable */
	UINT32		STBC:2;	/*channel bandwidth 20MHz or 40 MHz */
	UINT32		ShortGI:1;
	UINT32		BW:1;	/*channel bandwidth 20MHz or 40 MHz */
	UINT32		MCS:7;
	
	UINT32		rsv:1;
	UINT32		TXRPT:1;
	UINT32		Autofallback:1; /* TX rate auto fallback disable */
	UINT32		NDPSndBW:1; /* NDP sounding BW */
	UINT32		NDPSndRate:2; /* 0 : MCS0, 1: MCS8, 2: MCS16, 3: reserved */
	UINT32		txop:2;
	UINT32		MpduDensity:3;
	UINT32		AMPDU:1;
	
	UINT32		TS:1;
	UINT32		CFACK:1;
	UINT32		MIMOps:1;	/* the remote peer is in dynamic MIMO-PS mode */
	UINT32		FRAG:1;		/* 1 to inform TKIP engine this is a fragment. */
	/* Word 1 */
	UINT32		PacketId:4;
	UINT32		MPDUtotalByteCnt:12;
	UINT32		wcid:8;
	UINT32		BAWinSize:6;
	UINT32		NSEQ:1;
	UINT32		ACK:1;
	/* Word 2 */
	UINT32		IV;
	/* Word 3 */
	UINT32		EIV;

	/* Word 4 */
	/* For Expert Antenna */
	UINT32		Reserved:11;
	UINT32		CCP:1;
	UINT32		TxPwrAdj:4;
	UINT32		TxStreamMode:8;
	UINT32		EncodedAntID:8;
}	TXWI_OMAC, *PTXWI_OMAC;
#else
typedef	struct GNU_PACKED _TXWI_OMAC {
	/* Word	0 */
	/* ex: 00 03 00 40 means txop = 3, PHYMODE = 1 */
	UINT32		FRAG:1;		/* 1 to inform TKIP engine this is a fragment. */
	UINT32		MIMOps:1;	/* the remote peer is in dynamic MIMO-PS mode */
	UINT32		CFACK:1;
	UINT32		TS:1;
		
	UINT32		AMPDU:1;
	UINT32		MpduDensity:3;
	UINT32		txop:2;	/*FOR "THIS" frame. 0:HT TXOP rule , 1:PIFS TX ,2:Backoff, 3:sifs only when previous frame exchange is successful. */
	UINT32		NDPSndRate:2; /* 0 : MCS0, 1: MCS8, 2: MCS16, 3: reserved */
	UINT32		NDPSndBW:1; /* NDP sounding BW */
	UINT32		Autofallback:1; /* TX rate auto fallback disable */
	UINT32		TXRPT:1;
	UINT32		rsv:1;
	
	UINT32		MCS:7;
	UINT32		BW:1;	/*channel bandwidth 20MHz or 40 MHz */
	UINT32		ShortGI:1;
	UINT32		STBC:2;	/* 1: STBC support MCS =0-7,   2,3 : RESERVE */
	UINT32		eTxBF:1; /* eTxBF enable */
	UINT32		Sounding:1; /* Sounding enable */
	UINT32		iTxBF:1; /* iTxBF enable */
	UINT32		PHYMODE:2;  
	/* Word1 */
	/* ex:  1c ff 38 00 means ACK=0, BAWinSize=7, MPDUtotalByteCnt = 0x38 */
	UINT32		ACK:1;
	UINT32		NSEQ:1;
	UINT32		BAWinSize:6;
	UINT32		wcid:8;
	UINT32		MPDUtotalByteCnt:12;
	UINT32		PacketId:4;
	/*Word2 */
	UINT32		IV;
	/*Word3 */
	UINT32		EIV;

}	TXWI_OMAC, *PTXWI_OMAC;
#endif


#define TxWIMPDUByteCnt	TXWI_O.MPDUtotalByteCnt
#define TxWIWirelessCliID	TXWI_O.wcid
#define TxWIFRAG			TXWI_O.FRAG
#define TxWICFACK			TXWI_O.CFACK
#define TxWITS				TXWI_O.TS
#define TxWIAMPDU			TXWI_O.AMPDU
#define TxWIACK				TXWI_O.ACK
#define TxWITXOP			TXWI_O.txop
#define TxWINSEQ			TXWI_O.NSEQ
#define TxWIBAWinSize		TXWI_O.BAWinSize
#define TxWIShortGI			TXWI_O.ShortGI
#define TxWISTBC			TXWI_O.STBC
#define TxWIBW				TXWI_O.BW
#define TxWIMCS				TXWI_O.MCS
#define TxWIPHYMODE		TXWI_O.PHYMODE
#define TxWIMIMOps			TXWI_O.MIMOps
#define TxWIMpduDensity		TXWI_O.MpduDensity

#endif /* __RAL_OMAC_TXWI_H__ */

