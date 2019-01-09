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


#ifdef RT_CFG80211_SUPPORT

#include "rt_config.h"

#define RT_CFG80211_DEBUG /* debug use */
#define CFG80211CB			(pAd->pCfg80211_CB)

#ifdef RT_CFG80211_DEBUG
#define CFG80211DBG(__Flg, __pMsg)		DBGPRINT(__Flg, __pMsg)
#else
#define CFG80211DBG(__Flg, __pMsg)
#endif /* RT_CFG80211_DEBUG */




INT CFG80211DRV_IoctlHandle(
	IN	VOID					*pAdSrc,
	IN	RTMP_IOCTL_INPUT_STRUCT	*wrq,
	IN	INT						cmd,
	IN	USHORT					subcmd,
	IN	VOID					*pData,
	IN	ULONG					Data)
{
	PRTMP_ADAPTER pAd = (PRTMP_ADAPTER)pAdSrc;


	switch(cmd)
	{
		case CMD_RTPRIV_IOCTL_80211_START:
		case CMD_RTPRIV_IOCTL_80211_END:
			/* nothing to do */
			break;

		case CMD_RTPRIV_IOCTL_80211_CB_GET:
			*(VOID **)pData = (VOID *)(pAd->pCfg80211_CB);
			break;

		case CMD_RTPRIV_IOCTL_80211_CB_SET:
			pAd->pCfg80211_CB = pData;
			break;

		case CMD_RTPRIV_IOCTL_80211_CHAN_SET:
			if (CFG80211DRV_OpsSetChannel(pAd, pData) != TRUE)
				return NDIS_STATUS_FAILURE;
			break;

		case CMD_RTPRIV_IOCTL_80211_VIF_CHG:
			if (CFG80211DRV_OpsChgVirtualInf(pAd, pData, Data) != TRUE)
				return NDIS_STATUS_FAILURE;
			break;

		case CMD_RTPRIV_IOCTL_80211_SCAN:
			CFG80211DRV_OpsScan(pAd);
			break;

		case CMD_RTPRIV_IOCTL_80211_IBSS_JOIN:
			CFG80211DRV_OpsJoinIbss(pAd, pData);
			break;

		case CMD_RTPRIV_IOCTL_80211_STA_LEAVE:
			CFG80211DRV_OpsLeave(pAd);
			break;

		case CMD_RTPRIV_IOCTL_80211_STA_GET:
			if (CFG80211DRV_StaGet(pAd, pData) != TRUE)
				return NDIS_STATUS_FAILURE;
			break;

		case CMD_RTPRIV_IOCTL_80211_KEY_ADD:
			CFG80211DRV_KeyAdd(pAd, pData);
			break;

		case CMD_RTPRIV_IOCTL_80211_KEY_DEFAULT_SET:
#ifdef CONFIG_STA_SUPPORT
			pAd->StaCfg.DefaultKeyId = Data; /* base 0 */
#endif /* CONFIG_STA_SUPPORT */
			break;

		case CMD_RTPRIV_IOCTL_80211_CONNECT_TO:
			CFG80211DRV_Connect(pAd, pData);
			break;

#ifdef RFKILL_HW_SUPPORT
		case CMD_RTPRIV_IOCTL_80211_RFKILL:
		{
			UINT32 data = 0;
			BOOLEAN active;

			/* Read GPIO pin2 as Hardware controlled radio state */
			RTMP_IO_READ32(pAd, GPIO_CTRL_CFG, &data);
			active = !!(data & 0x04);

			if (!active)
			{
				RTMPSetLED(pAd, LED_RADIO_OFF);
				*(UINT8 *)pData = 0;
			}
			else
				*(UINT8 *)pData = 1;
		}
			break;
#endif /* RFKILL_HW_SUPPORT */

		case CMD_RTPRIV_IOCTL_80211_REG_NOTIFY_TO:
			CFG80211DRV_RegNotify(pAd, pData);
			break;

		case CMD_RTPRIV_IOCTL_80211_UNREGISTER:
			CFG80211_UnRegister(pAd, pData);
			break;

		case CMD_RTPRIV_IOCTL_80211_BANDINFO_GET:
		{
			CFG80211_BAND *pBandInfo = (CFG80211_BAND *)pData;
			CFG80211_BANDINFO_FILL(pAd, pBandInfo);
		}
			break;

		case CMD_RTPRIV_IOCTL_80211_SURVEY_GET:
			CFG80211DRV_SurveyGet(pAd, pData);
			break;

		case CMD_RTPRIV_IOCTL_80211_EXTRA_IES_SET:
			CFG80211DRV_OpsExtraIesSet(pAd);	
			break;

		case CMD_RTPRIV_IOCTL_80211_REMAIN_ON_CHAN_SET: 		
			CFG80211DRV_OpsRemainOnChannel(pAd, pData, Data);					
			break;				
		case CMD_RTPRIV_IOCTL_80211_CANCEL_REMAIN_ON_CHAN_SET:
			CFG80211DRV_OpsCancelRemainOnChannel(pAd, Data);
			break;

		case CMD_RTPRIV_IOCTL_80211_MGMT_FRAME_REG:
			if (Data)
				pAd->Cfg80211ProbeReqCount++;
			else 
			{
				pAd->Cfg80211ProbeReqCount--;	
			}
	
			if (pAd->Cfg80211ProbeReqCount > 0)
				pAd->Cfg80211RegisterProbeReqFrame = TRUE;
			else 
				pAd->Cfg80211RegisterProbeReqFrame = FALSE;
			
			DBGPRINT(RT_DEBUG_ERROR, ("pAd->Cfg80211RegisterProbeReqFrame=%d[%d]\n",pAd->Cfg80211RegisterProbeReqFrame, pAd->Cfg80211ProbeReqCount));
			break;

		case CMD_RTPRIV_IOCTL_80211_ACTION_FRAME_REG:
			if (Data)
				pAd->Cfg80211ActionCount++;
			else
				pAd->Cfg80211ActionCount--;

			if (pAd->Cfg80211ActionCount > 0)
				pAd->Cfg80211RegisterActionFrame = TRUE;
			else
				pAd->Cfg80211RegisterActionFrame = FALSE;

			DBGPRINT(RT_DEBUG_TRACE, ("pAd->Cfg80211RegisterActionFrame=%d [%d]\n",pAd->Cfg80211RegisterActionFrame, pAd->Cfg80211ActionCount));
			break;

		case CMD_RTPRIV_IOCTL_80211_CHANNEL_LOCK:
			//pAd->CommonCfg.CentralChannel = Data;
			//DBGPRINT(RT_DEBUG_TRACE, ("CMD_RTPRIV_IOCTL_80211_CHANNEL_LOCK %d\n", Data));
			if (pAd->CommonCfg.Channel != Data)
			{
				pAd->CommonCfg.Channel= Data;
				AsicSwitchChannel(pAd, pAd->CommonCfg.Channel, FALSE);
				AsicLockChannel(pAd, pAd->CommonCfg.Channel);
			}
			break;

		case CMD_RTPRIV_IOCTL_80211_MGMT_FRAME_SEND:
			/* send a managment frame */
			pAd->TxStatusInUsed = TRUE;
			pAd->TxStatusSeq = pAd->Sequence;
			if (pData != NULL) 
			{
#ifdef WFD_SUPPORT
				if (pAd->StaCfg.WfdCfg.bSuppInsertWfdIe)
				{
					PP2P_PUBLIC_FRAME	pFrame = (PP2P_PUBLIC_FRAME)pData;
					ULONG	WfdIeLen = 0, WfdIeBitmap = 0;
				
					switch (pFrame->p80211Header.FC.SubType) 
					{
						case SUBTYPE_BEACON:
						case SUBTYPE_PROBE_REQ:
						case SUBTYPE_ASSOC_REQ:
						case SUBTYPE_REASSOC_REQ:
							WfdIeBitmap = (0x1 << SUBID_WFD_DEVICE_INFO) | (0x1 << SUBID_WFD_ASSOCIATED_BSSID) |
								(0x1 << SUBID_WFD_COUPLED_SINK_INFO);
							break;	

						case SUBTYPE_ASSOC_RSP:
						case SUBTYPE_REASSOC_RSP:
							WfdIeBitmap = (0x1 << SUBID_WFD_DEVICE_INFO) | (0x1 << SUBID_WFD_ASSOCIATED_BSSID) |
								(0x1 << SUBID_WFD_COUPLED_SINK_INFO) | (0x1 << SUBID_WFD_SESSION_INFO);
							break;	

						case SUBTYPE_PROBE_RSP:
							WfdIeBitmap = (0x1 << SUBID_WFD_DEVICE_INFO) | (0x1 << SUBID_WFD_ASSOCIATED_BSSID) |
								(0x1 << SUBID_WFD_COUPLED_SINK_INFO) | (0x1 << SUBID_WFD_SESSION_INFO);
							break;	

						case SUBTYPE_ACTION:
							if ((pFrame->Category == CATEGORY_PUBLIC) &&
								(pFrame->Action == ACTION_WIFI_DIRECT))
							{
								switch (pFrame->Subtype) 
								{
									case GO_NEGOCIATION_REQ:
									case GO_NEGOCIATION_RSP:
									case GO_NEGOCIATION_CONFIRM:
									case P2P_PROVISION_REQ:
										WfdIeBitmap = (0x1 << SUBID_WFD_DEVICE_INFO) | (0x1 << SUBID_WFD_ASSOCIATED_BSSID) |
											(0x1 << SUBID_WFD_COUPLED_SINK_INFO);
										break;
				
									case P2P_INVITE_REQ:
									case P2P_INVITE_RSP:
									case P2P_PROVISION_RSP:
										WfdIeBitmap = (0x1 << SUBID_WFD_DEVICE_INFO) | (0x1 << SUBID_WFD_ASSOCIATED_BSSID) |
											(0x1 << SUBID_WFD_COUPLED_SINK_INFO) | (0x1 << SUBID_WFD_SESSION_INFO);
										break;
								}
							}
							break;
					}
				
					if (WfdIeBitmap > 0)
					{
						PUCHAR		pOutBuffer;
						NDIS_STATUS   NStatus;
						
						NStatus = MlmeAllocateMemory(pAd, &pOutBuffer);  /* Get an unused nonpaged memory */
						if (NStatus != NDIS_STATUS_SUCCESS)
							DBGPRINT(RT_DEBUG_ERROR, ("%s: Allocate memory fail!!!\n", __FUNCTION__));
						else
						{
							memcpy(pOutBuffer, pData, Data);
							WfdMakeWfdIE(pAd, WfdIeBitmap, pOutBuffer + Data, &WfdIeLen);
							Data += WfdIeLen;
							
							if (pAd->pTxStatusBuf != NULL)
								os_free_mem(NULL, pAd->pTxStatusBuf);
							
							os_alloc_mem(NULL, (UCHAR **)&pAd->pTxStatusBuf, Data);
							if (pAd->pTxStatusBuf != NULL)
							{
								NdisCopyMemory(pAd->pTxStatusBuf, pOutBuffer, Data);
								pAd->TxStatusBufLen = Data;
							}
							else
							{
								DBGPRINT(RT_DEBUG_TRACE, ("YF_TX_STATUS: MEM ALLOC ERROR\n"));
								MlmeFreeMemory(pAd, pOutBuffer);
								return NDIS_STATUS_FAILURE;
							}
							MiniportMMRequest(pAd, 0, pOutBuffer, Data);
						}
					}
				}
				else
#endif /* WFD_SUPPORT */
				{
					if (pAd->pTxStatusBuf != NULL)
						os_free_mem(NULL, pAd->pTxStatusBuf);
		
					os_alloc_mem(NULL, (UCHAR **)&pAd->pTxStatusBuf, Data);
					if (pAd->pTxStatusBuf != NULL)
					{
						NdisCopyMemory(pAd->pTxStatusBuf, pData, Data);
						pAd->TxStatusBufLen = Data;
					}
					else
					{
						DBGPRINT(RT_DEBUG_TRACE, ("YF_TX_STATUS: MEM ALLOC ERROR\n"));
						return NDIS_STATUS_FAILURE;
					}
					// pAd->pTxStatusBuf
					// pAd->TxStatusBufLen = Data
					//DBGPRINT(RT_DEBUG_TRACE, ("YF_TX_STATUS: send %d\n", pAd->TxStatusSeq));
					MiniportMMRequest(pAd, 0, pData, Data);
					//DBGPRINT(RT_DEBUG_TRACE, ("YF_TX_STATUS: sent %d\n", pAd->TxStatusSeq));
				}
			}
			break;

		case CMD_RTPRIV_IOCTL_80211_REMAIN_ON_CHAN_DUR_TIMER_INIT:
			DBGPRINT(RT_DEBUG_TRACE, ("ROC TIMER INIT\n"));
			RTMPInitTimer(pAd, &pAd->Cfg80211RemainOnChannelDurationTimer, GET_TIMER_FUNCTION(RemainOnChannelTimeout), pAd, FALSE);
			break;

		case CMD_RTPRIV_IOCTL_80211_CHANNEL_LIST_SET:
			DBGPRINT(RT_DEBUG_TRACE, ("CMD_RTPRIV_IOCTL_80211_CHANNEL_LIST_SET: %d\n", Data));
			UINT32 *pChanList = (UINT32 *) pData;

			if (pChanList != NULL) 
			{
			
				if (pAd->pCfg80211ChanList != NULL)
					os_free_mem(NULL, pAd->pCfg80211ChanList);

				os_alloc_mem(NULL, (UINT32 **)&pAd->pCfg80211ChanList, sizeof(UINT32 *) * Data);
				if (pAd->pCfg80211ChanList != NULL)
				{
					NdisCopyMemory(pAd->pCfg80211ChanList, pChanList, sizeof(UINT32 *) * Data);
					pAd->Cfg80211ChanListLan = Data;
				}
				else
				{
					return NDIS_STATUS_FAILURE;
				}
			}
			
			break;

		case CMD_RTPRIV_IOCTL_80211_BEACON_SET:
			CFG80211DRV_OpsBeaconSet(pAd, pData, 0);			
			break;
		
		case CMD_RTPRIV_IOCTL_80211_BEACON_ADD:
			CFG80211DRV_OpsBeaconSet(pAd, pData, 1);
			break;
			
		case CMD_RTPRIV_IOCTL_80211_BEACON_DEL:
#ifdef WFD_SUPPORT
			pAd->StaCfg.WfdCfg.bSuppGoOn = FALSE;
#endif /* WFD_SUPPORT */
			break;

		case CMD_RTPRIV_IOCTL_80211_CHANGE_BSS_PARM:
			CFG80211DRV_OpsChangeBssParm(pAd, pData);
			break;

		case CMD_RTPRIV_IOCTL_80211_AP_PROBE_RSP:
			if (pData != NULL)
			{
				if (pAd->pCfg80211RrobeRsp != NULL)
					os_free_mem(NULL, pAd->pCfg80211RrobeRsp);

				os_alloc_mem(NULL, (UCHAR **)&pAd->pCfg80211RrobeRsp, Data);
				if (pAd->pCfg80211RrobeRsp != NULL)
				{
					NdisCopyMemory(pAd->pCfg80211RrobeRsp, pData, Data);
					pAd->Cfg80211AssocRspLen = Data;
				}
				else
				{
					DBGPRINT(RT_DEBUG_TRACE, ("YF_AP: MEM ALLOC ERROR\n"));
					return NDIS_STATUS_FAILURE;
				}
				
			}
			else
				return NDIS_STATUS_FAILURE;
			break;

		case CMD_RTPRIV_IOCTL_80211_PORT_SECURED:
			CFG80211_StaPortSecured(pAd, pData, Data);
			break;

		case CMD_RTPRIV_IOCTL_80211_AP_STA_DEL:
			CFG80211_ApStaDel(pAd, pData);
			break;
		case CMD_RTPRIV_IOCTL_80211_BITRATE_SET:
		//	pAd->CommonCfg.PhyMode = PHY_11AN_MIXED;
		//	RTMPSetPhyMode(pAd,  pAd->CommonCfg.PhyMode);
			//Set_WirelessMode_Proc(pAd, PHY_11AGN_MIXED);
			break;
#ifdef RT_P2P_SPECIFIC_WIRELESS_EVENT
		case CMD_RTPRIV_IOCTL_80211_SEND_WIRELESS_EVENT:
			CFG80211_SendWirelessEvent(pAd, pData);
			break;
#endif /* RT_P2P_SPECIFIC_WIRELESS_EVENT */
		default:
			return NDIS_STATUS_FAILURE;
	}

	return NDIS_STATUS_SUCCESS;
}

static VOID CFG80211DRV_DisableApInterface(
        VOID                                            *pAdOrg)
{
	UINT32 Value;
	PRTMP_ADAPTER pAd = (PRTMP_ADAPTER)pAdOrg;
	pAd->ApCfg.MBSSID[MAIN_MBSSID].bBcnSntReq = FALSE;

        /* Disable pre-tbtt interrupt */
        RTMP_IO_READ32(pAd, INT_TIMER_EN, &Value);
        Value &=0xe;
        RTMP_IO_WRITE32(pAd, INT_TIMER_EN, Value);

        if (!INFRA_ON(pAd))
        {
                /* Disable piggyback */
                RTMPSetPiggyBack(pAd, FALSE);
                AsicUpdateProtect(pAd, 0,  (ALLN_SETPROTECT|CCKSETPROTECT|OFDMSETPROTECT), TRUE, FALSE);

        }

        if (!RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_NIC_NOT_EXIST))
        {
                /*RTMP_ASIC_INTERRUPT_DISABLE(pAd); */
                AsicDisableSync(pAd);

#ifdef LED_CONTROL_SUPPORT
                /* Set LED */
                RTMPSetLED(pAd, LED_LINK_DOWN);
#endif /* LED_CONTROL_SUPPORT */
        }

#ifdef RTMP_MAC_USB
        /* For RT2870, we need to clear the beacon sync buffer. */
        RTUSBBssBeaconExit(pAd);
#endif /* RTMP_MAC_USB */
	
}

VOID CFG80211DRV_OpsChangeBssParm(
        VOID                                            *pAdOrg,
        VOID                                            *pData)
{
	PRTMP_ADAPTER pAd = (PRTMP_ADAPTER)pAdOrg;
	CMD_RTPRIV_IOCTL_80211_BSS_PARM *pBssInfo;
	BOOLEAN TxPreamble;

	CFG80211DBG(RT_DEBUG_TRACE, ("%s\n", __FUNCTION__));

	pBssInfo = (CMD_RTPRIV_IOCTL_80211_BSS_PARM *)pData;	

	/* Short Preamble */
	if (pBssInfo->use_short_preamble != -1)
	{
		CFG80211DBG(RT_DEBUG_TRACE, ("%s: ShortPreamble %d\n", __FUNCTION__, pBssInfo->use_short_preamble));
        	pAd->CommonCfg.TxPreamble = (pBssInfo->use_short_preamble == 0 ? Rt802_11PreambleLong : Rt802_11PreambleShort);	
		TxPreamble = (pAd->CommonCfg.TxPreamble == Rt802_11PreambleLong ? 0 : 1);
		MlmeSetTxPreamble(pAd, (USHORT)pAd->CommonCfg.TxPreamble);			
	}
	
	/* CTS Protection */
	if (pBssInfo->use_cts_prot != -1)
	{
		CFG80211DBG(RT_DEBUG_TRACE, ("%s: CTS Protection %d\n", __FUNCTION__, pBssInfo->use_cts_prot));
	}
	
	/* Short Slot */
	if (pBssInfo->use_short_slot_time != -1)
	{
		CFG80211DBG(RT_DEBUG_TRACE, ("%s: Short Slot %d\n", __FUNCTION__, pBssInfo->use_short_slot_time));
	}
}

BOOLEAN CFG80211DRV_OpsSetChannel(
	VOID						*pAdOrg,
	VOID						*pData)
{
	PRTMP_ADAPTER pAd = (PRTMP_ADAPTER)pAdOrg;
	CMD_RTPRIV_IOCTL_80211_CHAN *pChan;
	UINT8 ChanId;
	UINT8 IfType;
	UINT8 ChannelType;
	STRING ChStr[5] = { 0 };
#ifdef DOT11_N_SUPPORT
	UCHAR BW_Old;
	BOOLEAN FlgIsChanged;
#endif /* DOT11_N_SUPPORT */


	/* init */
	pChan = (CMD_RTPRIV_IOCTL_80211_CHAN *)pData;
	ChanId = pChan->ChanId;
	IfType = pChan->IfType;
	ChannelType = pChan->ChanType;

#ifdef DOT11_N_SUPPORT
	if (IfType != RT_CMD_80211_IFTYPE_MONITOR)
	{
		/* get channel BW */
		FlgIsChanged = FALSE;
		BW_Old = pAd->CommonCfg.RegTransmitSetting.field.BW;
	
		/* set to new channel BW */
		if (ChannelType == RT_CMD_80211_CHANTYPE_HT20)
		{
			pAd->CommonCfg.RegTransmitSetting.field.BW = BW_20;
			FlgIsChanged = TRUE;
		}
		else if ((ChannelType == RT_CMD_80211_CHANTYPE_HT40MINUS) ||
				(ChannelType == RT_CMD_80211_CHANTYPE_HT40PLUS))
		{
			/* not support NL80211_CHAN_HT40MINUS or NL80211_CHAN_HT40PLUS */
			/* i.e. primary channel = 36, secondary channel must be 40 */
			pAd->CommonCfg.RegTransmitSetting.field.BW = BW_40;
			FlgIsChanged = TRUE;
		} /* End of if */
	
		CFG80211DBG(RT_DEBUG_ERROR, ("80211> New BW = %d\n",
					pAd->CommonCfg.RegTransmitSetting.field.BW));
	
		/* change HT/non-HT mode (do NOT change wireless mode here) */
		if (((ChannelType == RT_CMD_80211_CHANTYPE_NOHT) &&
			(pAd->CommonCfg.HT_Disable == 0)) ||
			((ChannelType != RT_CMD_80211_CHANTYPE_NOHT) &&
			(pAd->CommonCfg.HT_Disable == 1)))
		{
			if (ChannelType == RT_CMD_80211_CHANTYPE_NOHT)
				pAd->CommonCfg.HT_Disable = 1;
			else
				pAd->CommonCfg.HT_Disable = 0;
			/* End of if */
	
			FlgIsChanged = TRUE;
			CFG80211DBG(RT_DEBUG_ERROR, ("80211> HT Disable = %d\n",
						pAd->CommonCfg.HT_Disable));
		} /* End of if */
	}
	else
	{
		/* for monitor mode */
		FlgIsChanged = TRUE;
		pAd->CommonCfg.HT_Disable = 0;
		pAd->CommonCfg.RegTransmitSetting.field.BW = BW_40;
	} /* End of if */

	if (FlgIsChanged == TRUE)
		SetCommonHT(pAd);
	/* End of if */
#endif /* DOT11_N_SUPPORT */

	/* switch to the channel */
	sprintf(ChStr, "%d", ChanId);
	if (Set_Channel_Proc(pAd, ChStr) == FALSE)
	{
		CFG80211DBG(RT_DEBUG_ERROR, ("80211> Change channel fail!\n"));
	} /* End of if */

#ifdef CONFIG_STA_SUPPORT
#ifdef DOT11_N_SUPPORT
	if ((IfType == RT_CMD_80211_IFTYPE_STATION) && (FlgIsChanged == TRUE))
	{
		/*
			1. Station mode;
			2. New BW settings is 20MHz but current BW is not 20MHz;
			3. New BW settings is 40MHz but current BW is 20MHz;

			Re-connect to the AP due to BW 20/40 or HT/non-HT change.
		*/
		Set_SSID_Proc(pAd, (PSTRING)pAd->CommonCfg.Ssid);
	} /* End of if */
#endif /* DOT11_N_SUPPORT */

	if (IfType == RT_CMD_80211_IFTYPE_ADHOC)
	{
		/* update IBSS beacon */
		MlmeUpdateTxRates(pAd, FALSE, 0);
		MakeIbssBeacon(pAd);
		AsicEnableIbssSync(pAd);

		Set_SSID_Proc(pAd, (PSTRING)pAd->CommonCfg.Ssid);
	} /* End of if */

	if (IfType == RT_CMD_80211_IFTYPE_MONITOR)
	{
		/* reset monitor mode in the new channel */
		Set_NetworkType_Proc(pAd, "Monitor");
		RTMP_IO_WRITE32(pAd, RX_FILTR_CFG, pChan->MonFilterFlag);
	} /* End of if */
#endif /* CONFIG_STA_SUPPORT */

	return TRUE;
}


BOOLEAN CFG80211DRV_OpsChgVirtualInf(
	VOID						*pAdOrg,
	VOID						*pFlgFilter,
	UINT8						IfType)
{
#ifdef CONFIG_STA_SUPPORT
	PRTMP_ADAPTER pAd = (PRTMP_ADAPTER)pAdOrg;
	UINT32 FlgFilter = *(UINT32 *)pFlgFilter;


	/* change type */
	if (IfType == RT_CMD_80211_IFTYPE_ADHOC)
		Set_NetworkType_Proc(pAd, "Adhoc");
	else if (IfType == RT_CMD_80211_IFTYPE_STATION)
		Set_NetworkType_Proc(pAd, "Infra");
	else if (IfType == RT_CMD_80211_IFTYPE_MONITOR)
	{
		/* set packet filter */
		Set_NetworkType_Proc(pAd, "Monitor");

		if (FlgFilter != 0)
		{
			UINT32 Filter;


			RTMP_IO_READ32(pAd, RX_FILTR_CFG, &Filter);

			if ((FlgFilter & RT_CMD_80211_FILTER_FCSFAIL) == \
												RT_CMD_80211_FILTER_FCSFAIL)
			{
				Filter = Filter & (~0x01);
			}
			else
				Filter = Filter | 0x01;
			/* End of if */
	
			if ((FlgFilter & RT_CMD_80211_FILTER_PLCPFAIL) == \
												RT_CMD_80211_FILTER_PLCPFAIL)
			{
				Filter = Filter & (~0x02);
			}
			else
				Filter = Filter | 0x02;
			/* End of if */
	
			if ((FlgFilter & RT_CMD_80211_FILTER_CONTROL) == \
												RT_CMD_80211_FILTER_CONTROL)
			{
				Filter = Filter & (~0xFF00);
			}
			else
				Filter = Filter | 0xFF00;
			/* End of if */
	
			if ((FlgFilter & RT_CMD_80211_FILTER_OTHER_BSS) == \
												RT_CMD_80211_FILTER_OTHER_BSS)
			{
				Filter = Filter & (~0x08);
			}
			else
				Filter = Filter | 0x08;
			/* End of if */

			RTMP_IO_WRITE32(pAd, RX_FILTR_CFG, Filter);
			*(UINT32 *)pFlgFilter = Filter;
		} /* End of if */

		return TRUE; /* not need to set SSID */
	} /* End of if */

	pAd->StaCfg.bAutoReconnect = TRUE;

	CFG80211DBG(RT_DEBUG_ERROR, ("80211> SSID = %s\n", pAd->CommonCfg.Ssid));
	Set_SSID_Proc(pAd, (PSTRING)pAd->CommonCfg.Ssid);
#endif /* CONFIG_STA_SUPPORT */

	return TRUE;
}


BOOLEAN CFG80211DRV_OpsScan(
	VOID						*pAdOrg)
{
	PRTMP_ADAPTER pAd = (PRTMP_ADAPTER)pAdOrg;


	if (pAd->FlgCfg80211Scanning == TRUE)
		return FALSE; /* scanning */
	/* End of if */

	/* do scan */
	pAd->FlgCfg80211Scanning = TRUE;
	return TRUE;
}

/* REF: ap_connect.c ApMakeBssBeacon */
BOOLEAN CFG80211DRV_OpsBeaconSet(
        VOID                                            *pAdOrg,
        VOID                                            *pData,
	BOOLEAN                                          isAdd)
{
	CFG80211DBG(RT_DEBUG_TRACE, ("80211> CFG80211DRV_OpsBeaconSet ==> %d\n", isAdd));
        PRTMP_ADAPTER pAd = (PRTMP_ADAPTER)pAdOrg;
        CMD_RTPRIV_IOCTL_80211_BEACON *pBeacon;
        PTXWI_STRUC    pTxWI = &pAd->BeaconTxWI;
        HTTRANSMIT_SETTING      BeaconTransmit;   /* MGMT frame PHY rate setting when operatin at Ht rate. */
        BCN_TIME_CFG_STRUC csr9;
        UCHAR  *ptr;
        UINT  i;
        UINT32 longValue;
        UINT8 TXWISize = pAd->chipCap.TXWISize;
	UINT32 rx_filter_flag;
	BOOLEAN TxPreamble, SpectrumMgmt = FALSE;
	BOOLEAN	bWmmCapable = FALSE;
	UCHAR	BBPR1 = 0, BBPR3 = 0;
	INT idx;
	ULONG offset;

	CFG80211DBG(RT_DEBUG_TRACE, ("80211> CFG80211DRV_OpsBeaconSet ==> \n"));
	pBeacon = (CMD_RTPRIV_IOCTL_80211_BEACON *)pData;

#ifdef WFD_SUPPORT
	if (pAd->StaCfg.WfdCfg.bSuppInsertWfdIe)
	{
		ULONG TmpLen, WfdIeBitmap;

		ptr = pBeacon->beacon + pBeacon->beacon_len;
		WfdIeBitmap = (0x1 << SUBID_WFD_DEVICE_INFO) | (0x1 << SUBID_WFD_ASSOCIATED_BSSID) |
			(0x1 << SUBID_WFD_COUPLED_SINK_INFO);
		WfdMakeWfdIE(pAd, WfdIeBitmap, ptr, &TmpLen);
		pBeacon->beacon_len += TmpLen;
	}
#endif /* WFD_SUPPORT */

	if (isAdd)
	{
		rx_filter_flag = APNORMAL;
		RTMP_IO_WRITE32(pAd, RX_FILTR_CFG, rx_filter_flag);     /* enable RX of DMA block */
	
		pAd->ApCfg.BssidNum = 1;
		pAd->MacTab.MsduLifeTime = 20; /* default 5 seconds */
		pAd->ApCfg.MBSSID[MAIN_MBSSID].bBcnSntReq = TRUE;

#ifdef INF_AMAZON_SE
		printk("YF DEBUG: INF_AMAZON_SE\n");
		for (i = 0; i < NUM_OF_TX_RING; i++)
		{
			pAd->BulkOutDataSizeLimit[i]=24576;
		}
#endif /* INF_AMAZON_SE  */
	
		AsicDisableSync(pAd);

		if (pAd->CommonCfg.PhyMode >= PHY_11ABGN_MIXED)
		{
			if (pAd->CommonCfg.Channel > 14)
				pAd->ApCfg.MBSSID[MAIN_MBSSID].PhyMode = PHY_11AN_MIXED;
			else
				pAd->ApCfg.MBSSID[MAIN_MBSSID].PhyMode = PHY_11BGN_MIXED;
		}
		else
		{
			if (pAd->CommonCfg.Channel > 14)
				pAd->ApCfg.MBSSID[MAIN_MBSSID].PhyMode = PHY_11A;
			else
				pAd->ApCfg.MBSSID[MAIN_MBSSID].PhyMode = PHY_11BG_MIXED;
		}

		TxPreamble = (pAd->CommonCfg.TxPreamble == Rt802_11PreambleLong ? 0 : 1);	
	}

	PMULTISSID_STRUCT pMbss = &pAd->ApCfg.MBSSID[MAIN_MBSSID];

	const UCHAR *ssid_ie = NULL;
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,33))
	ssid_ie = cfg80211_find_ie(WLAN_EID_SSID, pBeacon->beacon+36, pBeacon->beacon_len-36);
#endif
	NdisZeroMemory(pMbss->Ssid, pMbss->SsidLen);
	if (ssid_ie == NULL) 
	{
		printk("YF Debug: SSID Not Found In Packet\n");
		NdisMoveMemory(pMbss->Ssid, "P2P_Linux_AP", 12);
		pMbss->SsidLen = 12;
	}
	else
	{
		pMbss->SsidLen = ssid_ie[1];
		NdisCopyMemory(pMbss->Ssid, ssid_ie+2, pMbss->SsidLen);
		printk("YF Debug: SSID: %s, %d\n", pMbss->Ssid, pMbss->SsidLen);
	}
	
	if (isAdd)
	{
		//if (pMbss->bWmmCapable)
		//{
        		bWmmCapable = FALSE;
			pMbss->bWmmCapable = FALSE;
		//}

		pMbss->MSSIDDev = pAd->net_dev;
		COPY_MAC_ADDR(pMbss->Bssid, pAd->CurrentAddress);
		printk("AP BSSID %02x:%02x:%02x:%02x:%02x:%02x\n", PRINT_MAC(pAd->CurrentAddress));
		
		/* GO always use WPA2PSK / AES */
		pMbss->AuthMode = Ndis802_11AuthModeWPA2PSK;
 		pMbss->WepStatus = Ndis802_11Encryption3Enabled;
		pMbss->WscSecurityMode = WPA2PSKAES;
		pMbss->GroupKeyWepStatus = pMbss->WepStatus;
		pMbss->CapabilityInfo =
			CAP_GENERATE(1, 0, (pMbss->WepStatus != Ndis802_11EncryptionDisabled), TxPreamble, pAd->CommonCfg.bUseShortSlotTime, SpectrumMgmt);

		RTMPMakeRSNIE(pAd, Ndis802_11AuthModeWPA2PSK, Ndis802_11Encryption3Enabled, MAIN_MBSSID);

#ifdef DOT11_N_SUPPORT
		RTMPSetPhyMode(pAd,  pAd->CommonCfg.PhyMode);
		SetCommonHT(pAd);

		if ((pAd->CommonCfg.PhyMode >= PHY_11ABGN_MIXED) && (pAd->Antenna.field.TxPath == 2))
		{
			RTMP_BBP_IO_READ8_BY_REG_ID(pAd, BBP_R1, &BBPR1);
			BBPR1 &= (~0x18);
			BBPR1 |= 0x10;
			RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R1, BBPR1);
		}
		else
#endif /* DOT11_N_SUPPORT */
		{
			RTMP_BBP_IO_READ8_BY_REG_ID(pAd, BBP_R1, &BBPR1);
			BBPR1 &= (~0x18);
			RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R1, BBPR1);
		}
	
		/* Receiver Antenna selection, write to BBP R3(bit4:3) */
		RTMP_BBP_IO_READ8_BY_REG_ID(pAd, BBP_R3, &BBPR3);
		BBPR3 &= (~0x18);
		if(pAd->Antenna.field.RxPath == 3)
		{
			BBPR3 |= (0x10);
		}
		else if(pAd->Antenna.field.RxPath == 2)
		{
			BBPR3 |= (0x8);
		}
		else if(pAd->Antenna.field.RxPath == 1)
		{
			BBPR3 |= (0x0);
		}
		RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R3, BBPR3);

		if(!OPSTATUS_TEST_FLAG(pAd, fOP_STATUS_MEDIA_STATE_CONNECTED))
		{
			if ((pAd->CommonCfg.PhyMode > PHY_11G) || bWmmCapable)
			{
				/* EDCA parameters used for AP's own transmission */
				pAd->CommonCfg.APEdcaParm.bValid = TRUE;
				pAd->CommonCfg.APEdcaParm.Aifsn[0] = 3;
				pAd->CommonCfg.APEdcaParm.Aifsn[1] = 7;
				pAd->CommonCfg.APEdcaParm.Aifsn[2] = 1;
				pAd->CommonCfg.APEdcaParm.Aifsn[3] = 1;

				pAd->CommonCfg.APEdcaParm.Cwmin[0] = 4;
				pAd->CommonCfg.APEdcaParm.Cwmin[1] = 4;
				pAd->CommonCfg.APEdcaParm.Cwmin[2] = 3;
				pAd->CommonCfg.APEdcaParm.Cwmin[3] = 2;

				pAd->CommonCfg.APEdcaParm.Cwmax[0] = 6;
				pAd->CommonCfg.APEdcaParm.Cwmax[1] = 10;
				pAd->CommonCfg.APEdcaParm.Cwmax[2] = 4;
				pAd->CommonCfg.APEdcaParm.Cwmax[3] = 3;

				pAd->CommonCfg.APEdcaParm.Txop[0]  = 0;
				pAd->CommonCfg.APEdcaParm.Txop[1]  = 0;
				pAd->CommonCfg.APEdcaParm.Txop[2]  = 94;	/*96; */
				pAd->CommonCfg.APEdcaParm.Txop[3]  = 47;	/*48; */
				AsicSetEdcaParm(pAd, &pAd->CommonCfg.APEdcaParm);

				/* EDCA parameters to be annouced in outgoing BEACON, used by WMM STA */
				pAd->ApCfg.BssEdcaParm.bValid = TRUE;
				pAd->ApCfg.BssEdcaParm.Aifsn[0] = 3;
				pAd->ApCfg.BssEdcaParm.Aifsn[1] = 7;
				pAd->ApCfg.BssEdcaParm.Aifsn[2] = 2;
				pAd->ApCfg.BssEdcaParm.Aifsn[3] = 2;

				pAd->ApCfg.BssEdcaParm.Cwmin[0] = 4;
				pAd->ApCfg.BssEdcaParm.Cwmin[1] = 4;
				pAd->ApCfg.BssEdcaParm.Cwmin[2] = 3;
				pAd->ApCfg.BssEdcaParm.Cwmin[3] = 2;

				pAd->ApCfg.BssEdcaParm.Cwmax[0] = 10;
				pAd->ApCfg.BssEdcaParm.Cwmax[1] = 10;
				pAd->ApCfg.BssEdcaParm.Cwmax[2] = 4;
				pAd->ApCfg.BssEdcaParm.Cwmax[3] = 3;
	
				pAd->ApCfg.BssEdcaParm.Txop[0]  = 0;
				pAd->ApCfg.BssEdcaParm.Txop[1]  = 0;
				pAd->ApCfg.BssEdcaParm.Txop[2]  = 94;	/*96; */
				pAd->ApCfg.BssEdcaParm.Txop[3]  = 47;	/*48; */
			}
			else
			{
				AsicSetEdcaParm(pAd, NULL);
			}
		}

#ifdef DOT11_N_SUPPORT
		if (pAd->CommonCfg.PhyMode < PHY_11ABGN_MIXED)
		{
			/* Patch UI */
			pAd->CommonCfg.HtCapability.HtCapInfo.ChannelWidth = BW_20;
		}

		/* init */
		if (pAd->CommonCfg.bRdg)
		{	
			RTMP_SET_FLAG(pAd, fRTMP_ADAPTER_RDG_ACTIVE);
			AsicEnableRDG(pAd);
		}
		else	
		{
			RTMP_CLEAR_FLAG(pAd, fRTMP_ADAPTER_RDG_ACTIVE);
			AsicDisableRDG(pAd);
		}	
#endif /* DOT11_N_SUPPORT */

		//AsicSetBssid(pAd, pAd->CurrentAddress); 
		AsicSetMcastWC(pAd);
		
		/* In AP mode,  First WCID Table in ASIC will never be used. To prevent it's 0xff-ff-ff-ff-ff-ff, Write 0 here. */
		/* p.s ASIC use all 0xff as termination of WCID table search. */
		RTMP_IO_WRITE32(pAd, MAC_WCID_BASE, 0x00);
		RTMP_IO_WRITE32(pAd, MAC_WCID_BASE+4, 0x0);

		/* reset WCID table */
		for (idx=2; idx<255; idx++)
		{
			offset = MAC_WCID_BASE + (idx * HW_WCID_ENTRY_SIZE);	
			RTMP_IO_WRITE32(pAd, offset, 0x0);
			RTMP_IO_WRITE32(pAd, offset+4, 0x0);
		}

		pAd->MacTab.Content[0].Addr[0] = 0x01;
		pAd->MacTab.Content[0].HTPhyMode.field.MODE = MODE_OFDM;
		pAd->MacTab.Content[0].HTPhyMode.field.MCS = 3;

		AsicBBPAdjust(pAd);
		//MlmeSetTxPreamble(pAd, (USHORT)pAd->CommonCfg.TxPreamble);	
	
		{
			ULONG	Addr4;
			UINT32	regValue;
			PUCHAR pP2PBssid = &pAd->CurrentAddress[0];
		
			Addr4 = (ULONG)(pP2PBssid[0])	    | 
				(ULONG)(pP2PBssid[1] << 8)  | 
				(ULONG)(pP2PBssid[2] << 16) |
				(ULONG)(pP2PBssid[3] << 24);
			RTMP_IO_WRITE32(pAd, MAC_BSSID_DW0, Addr4);
	
			Addr4 = 0;

			Addr4 = (ULONG)(pP2PBssid[4]) | (ULONG)(pP2PBssid[5] << 8);
			RTMP_IO_WRITE32(pAd, MAC_BSSID_DW1, Addr4);
	
			RTMP_IO_READ32(pAd, MAC_BSSID_DW1, &regValue);
			regValue &= 0x0000FFFF;

			regValue |= (1 << 16);

			if (pAd->chipCap.MBSSIDMode == MBSSID_MODE1)
				regValue |= (1 << 21);
			RTMP_IO_WRITE32(pAd, MAC_BSSID_DW1, regValue);		
		}
	

#ifdef RTMP_MAC_USB
		printk("YF DEBUG: RTUSBBssBeaconInit\n");
        	RTUSBBssBeaconInit(pAd);
#endif /* RTMP_MAC_USB */
	}

	UCHAR apcliIdx, apidx = MAIN_MBSSID;

	//pAd->ApCfg.MBSSID[MAIN_MBSSID].PhyMode = PHY_11BGN_MIXED;


	printk("YF DEBUG: Beacon Len %d\n", pBeacon->beacon_len);
	printk("YF DEBUG: Beacon Interval %d\n", pBeacon->interval);
        BeaconTransmit.word = 0;

        RTMPWriteTxWI(pAd, pTxWI, FALSE, FALSE, TRUE, FALSE, FALSE, TRUE, 0, BSS0Mcast_WCID,
                pBeacon->beacon_len, PID_MGMT, 0, 0,IFS_HTTXOP, FALSE, &BeaconTransmit);

        ptr = (PUCHAR)&pAd->BeaconTxWI;
#ifdef RT_BIG_ENDIAN
        RTMPWIEndianChange(ptr, TYPE_TXWI);
#endif

        for (i=0; i<TXWISize; i+=4)  /* 16-byte TXWI field */
        {
                longValue =  *ptr + (*(ptr+1)<<8) + (*(ptr+2)<<16) + (*(ptr+3)<<24);
                RTMP_IO_WRITE32(pAd, pAd->BeaconOffset[0] + i, longValue);
                ptr += 4;
        }

        /* update BEACON frame content. start right after the 16-byte TXWI field. */
        ptr = pBeacon->beacon;
#ifdef RT_BIG_ENDIAN
        RTMPFrameEndianChange(pAd, ptr, DIR_WRITE, FALSE);
#endif

        for (i= 0; i< pBeacon->beacon_len; i+=4)
        {
                longValue =  *ptr + (*(ptr+1)<<8) + (*(ptr+2)<<16) + (*(ptr+3)<<24);
                RTMP_IO_WRITE32(pAd, pAd->BeaconOffset[0] + TXWISize + i, longValue);
                ptr += 4;
        }

	if (isAdd)
	{
		/* Enable Bss Sync*/
		RTMP_IO_READ32(pAd, BCN_TIME_CFG, &csr9.word);
        	csr9.field.BeaconInterval = (pBeacon->interval) << 4; /* ASIC register in units of 1/16 TU*/
        	csr9.field.bTsfTicking = 1;
        	csr9.field.TsfSyncMode = 3;
	        csr9.field.bTBTTEnable = 1;
        	csr9.field.bBeaconGen = 1;
	        RTMP_IO_WRITE32(pAd, BCN_TIME_CFG, csr9.word);

		pAd->P2pCfg.bSentProbeRSP = TRUE;

#ifdef RTMP_MAC_USB
		/*
		 * Support multiple BulkIn IRP,
	 	 * the value on pAd->CommonCfg.NumOfBulkInIRP may be large than 1.
		 */
	
		UCHAR num_idx;

		for(num_idx=0; num_idx < pAd->CommonCfg.NumOfBulkInIRP; num_idx++)
		{
			RTUSBBulkReceive(pAd);
			printk("RTUSBBulkReceive!\n" );
		}
	
#endif /* RTMP_MAC_USB */
	}

#ifdef WFD_SUPPORT
	pAd->StaCfg.WfdCfg.bSuppGoOn = TRUE;
#endif /* WFD_SUPPORT */
		
	return TRUE;

}

BOOLEAN CFG80211DRV_OpsExtraIesSet(
	VOID						*pAdOrg)
{
	PRTMP_ADAPTER pAd = (PRTMP_ADAPTER)pAdOrg;
	
	CFG80211_CB *pCfg80211_CB = pAd->pCfg80211_CB;
	UINT ie_len = pCfg80211_CB->pCfg80211_ScanReq->ie_len;
    CFG80211DBG(RT_DEBUG_ERROR, ("80211> CFG80211DRV_OpsExtraIesSet ==> %d\n", ie_len)); 

	if (pAd->StaCfg.pWpsProbeReqIe)
	{	
		os_free_mem(NULL, pAd->StaCfg.pWpsProbeReqIe);
		pAd->StaCfg.pWpsProbeReqIe = NULL;
	}

	pAd->StaCfg.WpsProbeReqIeLen = 0;

	CFG80211DBG(RT_DEBUG_ERROR, ("80211> is_wpa_supplicant_up ==> %d\n", pAd->StaCfg.WpaSupplicantUP)); 
	os_alloc_mem(pAd, (UCHAR **)&(pAd->StaCfg.pWpsProbeReqIe), ie_len);
	if (pAd->StaCfg.pWpsProbeReqIe)
	{
		memcpy(pAd->StaCfg.pWpsProbeReqIe, pCfg80211_CB->pCfg80211_ScanReq->ie, ie_len);
		pAd->StaCfg.WpsProbeReqIeLen = ie_len;
		//hex_dump("WpsProbeReqIe", pAd->StaCfg.pWpsProbeReqIe, pAd->StaCfg.WpsProbeReqIeLen);
		DBGPRINT(RT_DEBUG_TRACE, ("Set::RT_OID_WPS_PROBE_REQ_IE, WpsProbeReqIeLen = %d!!\n",
					pAd->StaCfg.WpsProbeReqIeLen));
	}
	else
	{
		CFG80211DBG(RT_DEBUG_ERROR, ("80211> CFG80211DRV_OpsExtraIesSet ==> allocate fail. \n")); 
		return FALSE;
	}
	return TRUE;
}


BOOLEAN CFG80211DRV_OpsJoinIbss(
	VOID						*pAdOrg,
	VOID						*pData)
{
#ifdef CONFIG_STA_SUPPORT
	PRTMP_ADAPTER pAd = (PRTMP_ADAPTER)pAdOrg;
	CMD_RTPRIV_IOCTL_80211_IBSS *pIbssInfo;


	pIbssInfo = (CMD_RTPRIV_IOCTL_80211_IBSS *)pData;
	pAd->StaCfg.bAutoReconnect = TRUE;

	pAd->CommonCfg.BeaconPeriod = pIbssInfo->BeaconInterval;
	Set_SSID_Proc(pAd, (PSTRING)pIbssInfo->pSsid);
#endif /* CONFIG_STA_SUPPORT */
	return TRUE;
}


BOOLEAN CFG80211DRV_OpsLeave(
	VOID						*pAdOrg)
{
#ifdef CONFIG_STA_SUPPORT
	PRTMP_ADAPTER pAd = (PRTMP_ADAPTER)pAdOrg;


	pAd->StaCfg.bAutoReconnect = FALSE;
	pAd->FlgCfg80211Connecting = FALSE;
	LinkDown(pAd, FALSE);
#endif /* CONFIG_STA_SUPPORT */
	return TRUE;
}


BOOLEAN CFG80211DRV_StaGet(
	VOID						*pAdOrg,
	VOID						*pData)
{
	PRTMP_ADAPTER pAd = (PRTMP_ADAPTER)pAdOrg;
	CMD_RTPRIV_IOCTL_80211_STA *pIbssInfo;


	pIbssInfo = (CMD_RTPRIV_IOCTL_80211_STA *)pData;


#ifdef CONFIG_STA_SUPPORT
{
	HTTRANSMIT_SETTING PhyInfo;
	ULONG DataRate = 0;
	UINT32 RSSI;


	/* fill tx rate */
    if ((!WMODE_CAP_N(pAd->CommonCfg.PhyMode)) ||
	 (pAd->MacTab.Content[BSSID_WCID].HTPhyMode.field.MODE <= MODE_OFDM))
	{
		PhyInfo.word = pAd->StaCfg.HTPhyMode.word;
	}
    else
		PhyInfo.word = pAd->MacTab.Content[BSSID_WCID].HTPhyMode.word;
	/* End of if */

	getRate(PhyInfo, &DataRate);

	if ((PhyInfo.field.MODE == MODE_HTMIX) ||
		(PhyInfo.field.MODE == MODE_HTGREENFIELD))
	{
		if (PhyInfo.field.BW)
			pIbssInfo->TxRateFlags |= RT_CMD_80211_TXRATE_BW_40;
		/* End of if */
		if (PhyInfo.field.ShortGI)
			pIbssInfo->TxRateFlags |= RT_CMD_80211_TXRATE_SHORT_GI;
		/* End of if */

		pIbssInfo->TxRateMCS = PhyInfo.field.MCS;
	}
	else
	{
		pIbssInfo->TxRateFlags = RT_CMD_80211_TXRATE_LEGACY;
		pIbssInfo->TxRateMCS = DataRate*10; /* unit: 100kbps */
	} /* End of if */

	/* fill signal */
	RSSI = (pAd->StaCfg.RssiSample.AvgRssi0 +
			pAd->StaCfg.RssiSample.AvgRssi1 +
			pAd->StaCfg.RssiSample.AvgRssi2) / 3;
	pIbssInfo->Signal = RSSI;
}
#endif /* CONFIG_STA_SUPPORT */

	return TRUE;
}


BOOLEAN CFG80211DRV_KeyAdd(
	VOID						*pAdOrg,
	VOID						*pData)
{
#ifdef CONFIG_STA_SUPPORT
	PRTMP_ADAPTER pAd = (PRTMP_ADAPTER)pAdOrg;
	CMD_RTPRIV_IOCTL_80211_KEY *pKeyInfo;


	pKeyInfo = (CMD_RTPRIV_IOCTL_80211_KEY *)pData;

	if (pKeyInfo->KeyType == RT_CMD_80211_KEY_WEP)
	{
		switch(pKeyInfo->KeyId)
		{
			case 1:
			default:
				Set_Key1_Proc(pAd, (PSTRING)pKeyInfo->KeyBuf);
				break;

			case 2:
				Set_Key2_Proc(pAd, (PSTRING)pKeyInfo->KeyBuf);
				break;

			case 3:
				Set_Key3_Proc(pAd, (PSTRING)pKeyInfo->KeyBuf);
				break;

			case 4:
				Set_Key4_Proc(pAd, (PSTRING)pKeyInfo->KeyBuf);
				break;
		} /* End of switch */
	}
	else
		Set_WPAPSK_Proc(pAd, (PSTRING)pKeyInfo->KeyBuf);
#endif /* CONFIG_STA_SUPPORT */

	return TRUE;
}


BOOLEAN CFG80211DRV_Connect(
	VOID						*pAdOrg,
	VOID						*pData)
{
#ifdef CONFIG_STA_SUPPORT
	PRTMP_ADAPTER pAd = (PRTMP_ADAPTER)pAdOrg;
	CMD_RTPRIV_IOCTL_80211_CONNECT *pConnInfo;
	UCHAR SSID[NDIS_802_11_LENGTH_SSID];
	UINT32 SSIDLen;


	pConnInfo = (CMD_RTPRIV_IOCTL_80211_CONNECT *)pData;

	/* change to infrastructure mode if we are in ADHOC mode */
	Set_NetworkType_Proc(pAd, "Infra");

	/* set authentication mode */
	if (pConnInfo->WpaVer == 2)
	{
		if (pConnInfo->FlgIs8021x == TRUE)
			Set_AuthMode_Proc(pAd, "WPA2");
		else
			Set_AuthMode_Proc(pAd, "WPA2PSK");
		/* End of if */
	}
	else if (pConnInfo->WpaVer == 1)
	{
		if (pConnInfo->FlgIs8021x == TRUE)
			Set_AuthMode_Proc(pAd, "WPA");
		else
			Set_AuthMode_Proc(pAd, "WPAPSK");
		/* End of if */
	}
	else if (pConnInfo->FlgIsAuthOpen == FALSE)
		Set_AuthMode_Proc(pAd, "SHARED");
	else
		Set_AuthMode_Proc(pAd, "OPEN");
	/* End of if */

	CFG80211DBG(RT_DEBUG_ERROR,
				("80211> AuthMode = %d\n", pAd->StaCfg.AuthMode));

	/* set encryption mode */
	if (pConnInfo->PairwiseEncrypType & RT_CMD_80211_CONN_ENCRYPT_CCMP)
		Set_EncrypType_Proc(pAd, "AES");
	else if (pConnInfo->PairwiseEncrypType & RT_CMD_80211_CONN_ENCRYPT_TKIP)
		Set_EncrypType_Proc(pAd, "TKIP");
	else if (pConnInfo->PairwiseEncrypType & RT_CMD_80211_CONN_ENCRYPT_WEP)
	{
		Set_EncrypType_Proc(pAd, "WEP");
	}
	else if (pConnInfo->GroupwiseEncrypType & RT_CMD_80211_CONN_ENCRYPT_CCMP)
		Set_EncrypType_Proc(pAd, "AES");
	else if (pConnInfo->GroupwiseEncrypType & RT_CMD_80211_CONN_ENCRYPT_TKIP)
		Set_EncrypType_Proc(pAd, "TKIP");
	else
		Set_EncrypType_Proc(pAd, "NONE");
	/* End of if */

	CFG80211DBG(RT_DEBUG_ERROR,
				("80211> EncrypType = %d\n", pAd->StaCfg.WepStatus));

	/* set channel: STATION will auto-scan */

	/* set WEP key */
	if (pConnInfo->pKey &&
		((pConnInfo->GroupwiseEncrypType | pConnInfo->PairwiseEncrypType) &
												RT_CMD_80211_CONN_ENCRYPT_WEP))
	{
		UCHAR KeyBuf[50];

		/* reset AuthMode and EncrypType */
		Set_AuthMode_Proc(pAd, "SHARED");
		Set_EncrypType_Proc(pAd, "WEP");

		/* reset key */
#ifdef RT_CFG80211_DEBUG
		hex_dump("KeyBuf=", (UINT8 *)pConnInfo->pKey, pConnInfo->KeyLen);
#endif /* RT_CFG80211_DEBUG */

		pAd->StaCfg.DefaultKeyId = pConnInfo->KeyIdx; /* base 0 */
		if (pConnInfo->KeyLen >= sizeof(KeyBuf))
			return FALSE;
		/* End of if */
		memcpy(KeyBuf, pConnInfo->pKey, pConnInfo->KeyLen);
		KeyBuf[pConnInfo->KeyLen] = 0x00;

		CFG80211DBG(RT_DEBUG_ERROR,
					("80211> pAd->StaCfg.DefaultKeyId = %d\n",
					pAd->StaCfg.DefaultKeyId));

		switch(pConnInfo->KeyIdx)
		{
			case 1:
			default:
				Set_Key1_Proc(pAd, (PSTRING)KeyBuf);
				break;

			case 2:
				Set_Key2_Proc(pAd, (PSTRING)KeyBuf);
				break;

			case 3:
				Set_Key3_Proc(pAd, (PSTRING)KeyBuf);
				break;

			case 4:
				Set_Key4_Proc(pAd, (PSTRING)KeyBuf);
				break;
		} /* End of switch */
	} /* End of if */

	/* TODO: We need to provide a command to set BSSID to associate a AP */

	/* re-set SSID */
	pAd->StaCfg.bAutoReconnect = TRUE;
	pAd->FlgCfg80211Connecting = TRUE;

	SSIDLen = pConnInfo->SsidLen;
	if (SSIDLen > NDIS_802_11_LENGTH_SSID)
		SSIDLen = NDIS_802_11_LENGTH_SSID;
	/* End of if */

	memset(&SSID, 0, sizeof(SSID));
	memcpy(SSID, pConnInfo->pSsid, SSIDLen);
	Set_SSID_Proc(pAd, (PSTRING)SSID);
	CFG80211DBG(RT_DEBUG_ERROR, ("80211> SSID = %s\n", SSID));
#endif /* CONFIG_STA_SUPPORT */

	return TRUE;
}


VOID CFG80211DRV_RegNotify(
	VOID						*pAdOrg,
	VOID						*pData)
{
	PRTMP_ADAPTER pAd = (PRTMP_ADAPTER)pAdOrg;
	CMD_RTPRIV_IOCTL_80211_REG_NOTIFY *pRegInfo;


	pRegInfo = (CMD_RTPRIV_IOCTL_80211_REG_NOTIFY *)pData;

	/* keep Alpha2 and we can re-call the function when interface is up */
	pAd->Cfg80211_Alpha2[0] = pRegInfo->Alpha2[0];
	pAd->Cfg80211_Alpha2[1] = pRegInfo->Alpha2[1];

	/* apply the new regulatory rule */
	if (RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_START_UP))
	{
		/* interface is up */
		CFG80211_RegRuleApply(pAd, pRegInfo->pWiphy, (UCHAR *)pRegInfo->Alpha2);
	}
	else
	{
		CFG80211DBG(RT_DEBUG_ERROR, ("crda> interface is down!\n"));
	} /* End of if */
}


VOID CFG80211DRV_SurveyGet(
	VOID						*pAdOrg,
	VOID						*pData)
{
	PRTMP_ADAPTER pAd = (PRTMP_ADAPTER)pAdOrg;
	CMD_RTPRIV_IOCTL_80211_SURVEY *pSurveyInfo;


	pSurveyInfo = (CMD_RTPRIV_IOCTL_80211_SURVEY *)pData;

	pSurveyInfo->pCfg80211 = pAd->pCfg80211_CB;

#ifdef AP_QLOAD_SUPPORT
	pSurveyInfo->ChannelTimeBusy = pAd->QloadLatestChannelBusyTimePri;
	pSurveyInfo->ChannelTimeExtBusy = pAd->QloadLatestChannelBusyTimeSec;
#endif /* AP_QLOAD_SUPPORT */
}


VOID CFG80211_UnRegister(
	IN VOID						*pAdOrg,
	IN VOID						*pNetDev)
{
	PRTMP_ADAPTER pAd = (PRTMP_ADAPTER)pAdOrg;


	/* sanity check */
	if (pAd->pCfg80211_CB == NULL)
		return;
	/* End of if */

	CFG80211OS_UnRegister(pAd->pCfg80211_CB, pNetDev);
	pAd->pCfg80211_CB = NULL;
	pAd->CommonCfg.HT_Disable = 0;
}


/*
========================================================================
Routine Description:
	Parse and handle country region in beacon from associated AP.

Arguments:
	pAdCB			- WLAN control block pointer
	pVIE			- Beacon elements
	LenVIE			- Total length of Beacon elements

Return Value:
	NONE

Note:
========================================================================
*/
VOID CFG80211_BeaconCountryRegionParse(
	IN VOID						*pAdCB,
	IN NDIS_802_11_VARIABLE_IEs	*pVIE,
	IN UINT16					LenVIE)
{
	PRTMP_ADAPTER pAd = (PRTMP_ADAPTER)pAdCB;
	UCHAR *pElement = (UCHAR *)pVIE;
	UINT32 LenEmt;


	while(LenVIE > 0)
	{
		pVIE = (NDIS_802_11_VARIABLE_IEs *)pElement;

		if (pVIE->ElementID == IE_COUNTRY)
		{
			/* send command to do regulation hint only when associated */
			RTEnqueueInternalCmd(pAd, CMDTHREAD_REG_HINT_11D,
								pVIE->data, pVIE->Length);
			break;
		} /* End of if */

		LenEmt = pVIE->Length + 2;

		if (LenVIE <= LenEmt)
			break; /* length is not enough */
		/* End of if */

		pElement += LenEmt;
		LenVIE -= LenEmt;
	} /* End of while */
} /* End of CFG80211_BeaconCountryRegionParse */


/*
========================================================================
Routine Description:
	Hint to the wireless core a regulatory domain from driver.

Arguments:
	pAd				- WLAN control block pointer
	pCountryIe		- pointer to the country IE
	CountryIeLen	- length of the country IE

Return Value:
	NONE

Note:
	Must call the function in kernel thread.
========================================================================
*/
VOID CFG80211_RegHint(
	IN VOID						*pAdCB,
	IN UCHAR					*pCountryIe,
	IN ULONG					CountryIeLen)
{
	PRTMP_ADAPTER pAd = (PRTMP_ADAPTER)pAdCB;


	CFG80211OS_RegHint(CFG80211CB, pCountryIe, CountryIeLen);
} /* End of CFG80211_RegHint */


/*
========================================================================
Routine Description:
	Hint to the wireless core a regulatory domain from country element.

Arguments:
	pAdCB			- WLAN control block pointer
	pCountryIe		- pointer to the country IE
	CountryIeLen	- length of the country IE

Return Value:
	NONE

Note:
	Must call the function in kernel thread.
========================================================================
*/
VOID CFG80211_RegHint11D(
	IN VOID						*pAdCB,
	IN UCHAR					*pCountryIe,
	IN ULONG					CountryIeLen)
{
	/* no regulatory_hint_11d() in 2.6.32 */
	PRTMP_ADAPTER pAd = (PRTMP_ADAPTER)pAdCB;


	CFG80211OS_RegHint11D(CFG80211CB, pCountryIe, CountryIeLen);
} /* End of CFG80211_RegHint11D */


/*
========================================================================
Routine Description:
	Apply new regulatory rule.

Arguments:
	pAdCB			- WLAN control block pointer
	pWiphy			- Wireless hardware description
	pAlpha2			- Regulation domain (2B)

Return Value:
	NONE

Note:
	Can only be called when interface is up.

	For general mac80211 device, it will be set to new power by Ops->config()
	In rt2x00/, the settings is done in rt2x00lib_config().
========================================================================
*/
VOID CFG80211_RegRuleApply(
	IN VOID						*pAdCB,
	IN VOID						*pWiphy,
	IN UCHAR					*pAlpha2)
{
	PRTMP_ADAPTER pAd = (PRTMP_ADAPTER)pAdCB;
	VOID *pBand24G, *pBand5G;
	UINT32 IdBand, IdChan, IdPwr;
	UINT32 ChanNum, ChanId, Power, RecId, DfsType;
	BOOLEAN FlgIsRadar;
	ULONG IrqFlags;
#ifdef DFS_SUPPORT	
	RADAR_DETECT_STRUCT	*pRadarDetect;
#endif /* DFS_SUPPORT */


	CFG80211DBG(RT_DEBUG_ERROR, ("crda> CFG80211_RegRuleApply ==>\n"));

	/* init */
	pBand24G = NULL;
	pBand5G = NULL;

	if (pAd == NULL)
		return;

	RTMP_IRQ_LOCK(&pAd->irq_lock, IrqFlags);

	/* zero first */
	NdisZeroMemory(pAd->ChannelList,
					MAX_NUM_OF_CHANNELS * sizeof(CHANNEL_TX_POWER));

	/* 2.4GHZ & 5GHz */
	RecId = 0;
#ifdef DFS_SUPPORT	
	pRadarDetect = &pAd->CommonCfg.RadarDetect;
#endif /* DFS_SUPPORT */

	/* find the DfsType */
	DfsType = CE;

	pBand24G = NULL;
	pBand5G = NULL;

	if (CFG80211OS_BandInfoGet(CFG80211CB, pWiphy, &pBand24G, &pBand5G) == FALSE)
		return;

#ifdef AUTO_CH_SELECT_ENHANCE
#ifdef EXT_BUILD_CHANNEL_LIST
	if ((pAlpha2[0] != '0') && (pAlpha2[1] != '0'))
	{
		UINT32 IdReg;

		if (pBand5G != NULL)
		{
			for(IdReg=0; ; IdReg++)
			{
				if (ChRegion[IdReg].CountReg[0] == 0x00)
					break;
				/* End of if */
	
				if ((pAlpha2[0] == ChRegion[IdReg].CountReg[0]) &&
					(pAlpha2[1] == ChRegion[IdReg].CountReg[1]))
				{
					if (pAd->CommonCfg.DfsType != MAX_RD_REGION)
						DfsType = pAd->CommonCfg.DfsType;
					else
					DfsType = ChRegion[IdReg].DfsType;
	
					CFG80211DBG(RT_DEBUG_ERROR,
								("crda> find region %c%c, DFS Type %d\n",
								pAlpha2[0], pAlpha2[1], DfsType));
					break;
				} /* End of if */
			} /* End of for */
		} /* End of if */
	} /* End of if */
#endif /* EXT_BUILD_CHANNEL_LIST */
#endif /* AUTO_CH_SELECT_ENHANCE */

	for(IdBand=0; IdBand<2; IdBand++)
	{
		if (((IdBand == 0) && (pBand24G == NULL)) ||
			((IdBand == 1) && (pBand5G == NULL)))
		{
			continue;
		} /* End of if */

		if (IdBand == 0)
		{
			CFG80211DBG(RT_DEBUG_ERROR, ("crda> reset chan/power for 2.4GHz\n"));
		}
		else
		{
			CFG80211DBG(RT_DEBUG_ERROR, ("crda> reset chan/power for 5GHz\n"));
		} /* End of if */

		ChanNum = CFG80211OS_ChanNumGet(CFG80211CB, pWiphy, IdBand);

		for(IdChan=0; IdChan<ChanNum; IdChan++)
		{
			if (CFG80211OS_ChanInfoGet(CFG80211CB, pWiphy, IdBand, IdChan,
									&ChanId, &Power, &FlgIsRadar) == FALSE)
			{
				/* the channel is not allowed in the regulatory domain */
				/* get next channel information */
				continue;
			}

			if (!WMODE_CAP_2G(pAd->CommonCfg.PhyMode))
			{
				/* 5G-only mode */
				if (ChanId <= CFG80211_NUM_OF_CHAN_2GHZ)
					continue;
			}

			if (!WMODE_CAP_5G(pAd->CommonCfg.PhyMode))
			{
				/* 2.4G-only mode */
				if (ChanId > CFG80211_NUM_OF_CHAN_2GHZ)
					continue;
			}

			for(IdPwr=0; IdPwr<MAX_NUM_OF_CHANNELS; IdPwr++)
			{
				if (ChanId == pAd->TxPower[IdPwr].Channel)
				{
					/* init the channel info. */
					NdisMoveMemory(&pAd->ChannelList[RecId],
									&pAd->TxPower[IdPwr],
									sizeof(CHANNEL_TX_POWER));

					/* keep channel number */
					pAd->ChannelList[RecId].Channel = ChanId;

					/* keep maximum tranmission power */
					pAd->ChannelList[RecId].MaxTxPwr = Power;

					/* keep DFS flag */
					if (FlgIsRadar == TRUE)
						pAd->ChannelList[RecId].DfsReq = TRUE;
					else
						pAd->ChannelList[RecId].DfsReq = FALSE;
					/* End of if */

					/* keep DFS type */
					pAd->ChannelList[RecId].RegulatoryDomain = DfsType;

					/* re-set DFS info. */
					pAd->CommonCfg.RDDurRegion = DfsType;

					CFG80211DBG(RT_DEBUG_ERROR,
								("Chan %03d:\tpower %d dBm, "
								"DFS %d, DFS Type %d\n",
								ChanId, Power,
								((FlgIsRadar == TRUE)?1:0),
								DfsType));

					/* change to record next channel info. */
					RecId ++;
					break;
				} /* End of if */
			} /* End of for */
		} /* End of for */
	} /* End of for */

	pAd->ChannelListNum = RecId;
	RTMP_IRQ_UNLOCK(&pAd->irq_lock, IrqFlags);

	CFG80211DBG(RT_DEBUG_ERROR, ("crda> Number of channels = %d\n", RecId));
} /* End of CFG80211_RegRuleApply */


/*
========================================================================
Routine Description:
	Inform us that a scan is got.

Arguments:
	pAdCB				- WLAN control block pointer

Return Value:
	NONE

Note:
	Call RT_CFG80211_SCANNING_INFORM, not CFG80211_Scaning
========================================================================
*/
VOID CFG80211_Scaning(
	IN VOID							*pAdCB,
	IN UINT32						BssIdx,
	IN UINT32						ChanId,
	IN UCHAR						*pFrame,
	IN UINT32						FrameLen,
	IN INT32						RSSI)
{
#ifdef CONFIG_STA_SUPPORT
	PRTMP_ADAPTER pAd = (PRTMP_ADAPTER)pAdCB;
	VOID *pCfg80211_CB = pAd->pCfg80211_CB;
	BOOLEAN FlgIsNMode;
	UINT8 BW;


	CFG80211DBG(RT_DEBUG_ERROR, ("80211> CFG80211_Scaning ==>\n"));

	if (!RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_INTERRUPT_IN_USE))
	{
		DBGPRINT(RT_DEBUG_TRACE, ("80211> Network is down!\n"));
		return;
	} /* End of if */

	/*
		In connect function, we also need to report BSS information to cfg80211;
		Not only scan function.
	*/
	if ((pAd->FlgCfg80211Scanning == FALSE) &&
		(pAd->FlgCfg80211Connecting == FALSE))
	{
		return; /* no scan is running */
	} /* End of if */

	/* init */
	/* Note: Can not use local variable to do pChan */
	if (WMODE_CAP_N(pAd->CommonCfg.PhyMode))
		FlgIsNMode = TRUE;
	else
		FlgIsNMode = FALSE;

	if (pAd->CommonCfg.RegTransmitSetting.field.BW == BW_20)
		BW = 0;
	else
		BW = 1;

	CFG80211OS_Scaning(pCfg80211_CB,
						ChanId,
						pFrame,
						FrameLen,
						RSSI,
						FlgIsNMode,
						BW);
#endif /* CONFIG_STA_SUPPORT */
} /* End of CFG80211_Scaning */


/*
========================================================================
Routine Description:
	Inform us that scan ends.

Arguments:
	pAdCB			- WLAN control block pointer
	FlgIsAborted	- 1: scan is aborted

Return Value:
	NONE

Note:
========================================================================
*/
VOID CFG80211_ScanEnd(
	IN VOID						*pAdCB,
	IN BOOLEAN					FlgIsAborted)
{
#ifdef CONFIG_STA_SUPPORT
	PRTMP_ADAPTER pAd = (PRTMP_ADAPTER)pAdCB;


	if (!RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_INTERRUPT_IN_USE))
	{
		DBGPRINT(RT_DEBUG_TRACE, ("80211> Network is down!\n"));
		return;
	} /* End of if */

	if (pAd->FlgCfg80211Scanning == FALSE)
	{
		DBGPRINT(RT_DEBUG_TRACE, ("80211> No scan is running!\n"));
		return; /* no scan is running */
	} /* End of if */

	if (FlgIsAborted == TRUE)
		FlgIsAborted = 1;
	else
		FlgIsAborted = 0;
	/* End of if */

	CFG80211OS_ScanEnd(CFG80211CB, FlgIsAborted);

	pAd->FlgCfg80211Scanning = FALSE;
#endif /* CONFIG_STA_SUPPORT */
} /* End of CFG80211_ScanEnd */


/*
========================================================================
Routine Description:
	Inform CFG80211 about association status.

Arguments:
	pAdCB			- WLAN control block pointer
	pBSSID			- the BSSID of the AP
	pReqIe			- the element list in the association request frame
	ReqIeLen		- the request element length
	pRspIe			- the element list in the association response frame
	RspIeLen		- the response element length
	FlgIsSuccess	- 1: success; otherwise: fail

Return Value:
	None

Note:
========================================================================
*/
VOID CFG80211_ConnectResultInform(
	IN VOID						*pAdCB,
	IN UCHAR					*pBSSID,
	IN UCHAR					*pReqIe,
	IN UINT32					ReqIeLen,
	IN UCHAR					*pRspIe,
	IN UINT32					RspIeLen,
	IN UCHAR					FlgIsSuccess)
{
	PRTMP_ADAPTER pAd = (PRTMP_ADAPTER)pAdCB;


	CFG80211DBG(RT_DEBUG_ERROR, ("80211> CFG80211_ConnectResultInform ==>\n"));

	CFG80211OS_ConnectResultInform(CFG80211CB,
								pBSSID,
								pReqIe,
								ReqIeLen,
								pRspIe,
								RspIeLen,
								FlgIsSuccess);

	pAd->FlgCfg80211Connecting = FALSE;
} /* End of CFG80211_ConnectResultInform */


/*
========================================================================
Routine Description:
	Re-Initialize wireless channel/PHY in 2.4GHZ and 5GHZ.

Arguments:
	pAdCB			- WLAN control block pointer

Return Value:
	TRUE			- re-init successfully
	FALSE			- re-init fail

Note:
	CFG80211_SupBandInit() is called in xx_probe().
	But we do not have complete chip information in xx_probe() so we
	need to re-init bands in xx_open().
========================================================================
*/
BOOLEAN CFG80211_SupBandReInit(
	IN VOID						*pAdCB)
{
	PRTMP_ADAPTER pAd = (PRTMP_ADAPTER)pAdCB;
	CFG80211_BAND BandInfo;


	CFG80211DBG(RT_DEBUG_ERROR, ("80211> re-init bands...\n"));

	/* re-init bands */
	NdisZeroMemory(&BandInfo, sizeof(BandInfo));
	CFG80211_BANDINFO_FILL(pAd, &BandInfo);

	return CFG80211OS_SupBandReInit(CFG80211CB, &BandInfo);
} /* End of CFG80211_SupBandReInit */


INT CFG80211_StaPortSecured(
	IN VOID                                         *pAdCB,
	IN UCHAR 					*pMac,
	IN UINT						flag)
{
	PRTMP_ADAPTER pAd = (PRTMP_ADAPTER)pAdCB;
	MAC_TABLE_ENTRY *pEntry;

	pEntry = MacTableLookup(pAd, pMac);
	if (!pEntry)
	{
		printk("Can't find pEntry in CFG80211_StaPortSecured\n");
	}
	else
	{
		if (flag)
		{
			printk("AID:%d, PortSecured\n", pEntry->Aid);
			pEntry->PrivacyFilter = Ndis802_11PrivFilterAcceptAll;
			pEntry->WpaState = AS_PTKINITDONE;
			pEntry->PortSecured = WPA_802_1X_PORT_SECURED;	
		}
		else
		{
			printk("AID:%d, PortNotSecured\n", pEntry->Aid);
			pEntry->PrivacyFilter = Ndis802_11PrivFilter8021xWEP;
			pEntry->PortSecured = WPA_802_1X_PORT_NOT_SECURED;	
		}	
	}
	
	return 0;
}

INT CFG80211_ApStaDel(
	IN VOID                                         *pAdCB,
	IN UCHAR                                        *pMac)
{
	PRTMP_ADAPTER pAd = (PRTMP_ADAPTER)pAdCB;
	MAC_TABLE_ENTRY *pEntry;
	
	if (pMac == NULL)
	{
		MacTableReset(pAd);
	}
	else
	{
		pEntry = MacTableLookup(pAd, pMac);
		if (pEntry)
		{
		//	MlmeDeAuthAction(pAd, pEntry, 2, FALSE);
		}
		else
			printk("Can't find pEntry in ApStaDel\n");
	}
}

//CMD_RTPRIV_IOCTL_80211_KEY_DEFAULT_SET:
INT CFG80211_setDefaultKey(
	IN VOID                                         *pAdCB,
	IN UINT 					Data
)
{
	PRTMP_ADAPTER pAd = (PRTMP_ADAPTER)pAdCB;
	CFG80211_CB *p80211CB = pAd->pCfg80211_CB;

        if (p80211CB->pCfg80211_Wdev->iftype == RT_CMD_80211_IFTYPE_AP)
	//if (pAd->VifNextMode == RT_CMD_80211_IFTYPE_AP)
        {
		 printk("Set Ap Default Key: %d\n", Data);
        	pAd->ApCfg.MBSSID[MAIN_MBSSID].DefaultKeyId = Data;
        }
        else
#ifdef CONFIG_STA_SUPPORT
        {
		printk("Set Sta Default Key: %d\n", Data);
                pAd->StaCfg.DefaultKeyId = Data; /* base 0 */
        }
#endif /* CONFIG_STA_SUPPORT */
	return 0;	

}

#ifdef RT_P2P_SPECIFIC_WIRELESS_EVENT
INT CFG80211_SendWirelessEvent(
	IN VOID                                         *pAdCB,
	IN UCHAR 					*pMacAddr)
{
	PRTMP_ADAPTER pAd = (PRTMP_ADAPTER)pAdCB;

	P2pSendWirelessEvent(pAd, RT_P2P_CONNECTED, NULL, pMacAddr);
	
	return 0;
}
#endif /* RT_P2P_SPECIFIC_WIRELESS_EVENT */


#endif /* RT_CFG80211_SUPPORT */

/* End of cfg80211drv.c */
