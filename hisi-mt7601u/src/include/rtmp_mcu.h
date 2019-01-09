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


#ifndef __RTMP_MCU_H__
#define __RTMP_MCU_H__

enum MCU_TYPE {
	SWMCU,
	M8051,
	ANDES,
};


struct _RTMP_ADAPTER;

typedef void (*CMD_RSP_HANDLER)(struct _RTMP_ADAPTER *pAd, UCHAR *Data);

/*
 * CMD Unit (8051, Andes, ...,and etc)
 */
struct CMD_UNIT {
	union {
		struct {
			UCHAR Command;
			UCHAR Token;
			UCHAR Arg0;
			UCHAR Arg1;
		} MCU51;
		struct {
			UINT8 Type;
			USHORT CmdPayloadLen;
			PUCHAR CmdPayload;
			USHORT RspPayloadLen;
			PUCHAR RspPayload;
			ULONG Timeout;
			BOOLEAN NeedRsp;
			BOOLEAN NeedWait;
			CMD_RSP_HANDLER CmdRspHdler;
		} ANDES;
	} u;
};


struct MCU_CTRL {
	UCHAR CmdSeq;
	NDIS_SPIN_LOCK CmdRspEventListLock;
	DL_LIST CmdRspEventList;
};


struct CMD_RSP_EVENT {
	DL_LIST List;
	UCHAR CmdSeq;	
	UINT32 Timeout;
	BOOLEAN NeedWait;
	PVOID	AckDone;
	UCHAR **RspPayload;
	USHORT *RspPayloadLen;
};

VOID ChipOpsMCUHook(struct _RTMP_ADAPTER *pAd, enum MCU_TYPE MCUType);
VOID MCUCtrlInit(struct _RTMP_ADAPTER *pAd);
VOID MCUCtrlExit(struct _RTMP_ADAPTER *pAd);

#endif 
