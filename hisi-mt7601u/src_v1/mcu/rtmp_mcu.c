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


#include	"rt_config.h"

INT MCUBurstWrite(PRTMP_ADAPTER pAd, UINT32 Offset, UINT32 *Data, UINT32 Cnt)
{
	RTUSBMultiWrite_nBytes(pAd, Offset, Data, Cnt * 4, 64); 
}

INT MCURandomWrite(PRTMP_ADAPTER pAd, RTMP_REG_PAIR *RegPair, UINT32 Num)
{
	UINT32 Index;
	
	for (Index = 0; Index < Num; Index++)
		RTMP_IO_WRITE32(pAd, RegPair->Register, RegPair->Value);
}

VOID ChipOpsMCUHook(PRTMP_ADAPTER pAd, enum MCU_TYPE MCUType)
{

	RTMP_CHIP_OP *pChipOps = &pAd->chipOps;


	if (MCUType == M8051) 
	{
		pChipOps->sendCommandToMcu = RtmpAsicSendCommandToMcu;
		pChipOps->BurstWrite = MCUBurstWrite;
		pChipOps->RandomWrite = MCURandomWrite;
	}

#ifdef CONFIG_ANDES_SUPPORT
	if (MCUType == ANDES) 
	{

#ifdef RTMP_USB_SUPPORT
		pChipOps->loadFirmware = USBLoadFirmwareToAndes;
#endif
		//pChipOps->sendCommandToMcu = AsicSendCmdToAndes;
		pChipOps->Calibration = AndesCalibrationOP;
		pChipOps->BurstWrite =  AndesBurstWrite;
		pChipOps->BurstRead = AndesBurstRead;
		pChipOps->RandomRead = AndesRandomRead;
		pChipOps->RFRandomRead = AndesRFRandomRead;
		pChipOps->ReadModifyWrite = AndesReadModifyWrite;
		pChipOps->RFReadModifyWrite = AndesRFReadModifyWrite;
		pChipOps->RandomWrite = AndesRandomWrite;
		pChipOps->RFRandomWrite = AndesRFRandomWrite;
		pChipOps->PwrSavingOP = AndesPwrSavingOP;
	}
#endif
}
