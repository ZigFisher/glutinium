/******************************************************************************

  Copyright (C), 2001-2011, Hisilicon Tech. Co., Ltd.

 ******************************************************************************
  File Name     : stat.c
  Version       : Initial Draft
  Author        : Hisilicon multimedia software group
  Created       : 2005/11/29
  Last Modified :
  Description   : this file for stat usage
  Function List :
  History       :
  1.Date        : 2005/11/29
    Author      : qushen
    Modification: Created file

******************************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
    
#include "hi.h"

#include "stat.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#define WRITE_LOG_STAT  WRITE_LOG_DEBUG

#ifdef STAT

U64 gStat[E_STAT_ITEM_END];
TimeVal_t timeval[STAT_TYPE_MAX];


VOID stat_calc(STAT_ITEM_E t, U32 value)
{
    gStat[t] += value;
}

VOID stat_init()
{
    memset(gStat, 0, sizeof(gStat));
}
VOID stat_time_start(STAT_TYPE_E t)
{
    time(&timeval[t].StartTime);
}

VOID stat_rtp_discarded(STAT_ITEM_E t, U16 seq, U16 prev_seq)
{
    short losenum = 0;

    losenum = seq - prev_seq;
    if ( losenum != 1 )
    {
        if ( t == E_VDec_Rtp_Discarded )
        {
            WRITE_LOG_DEBUG("Video: ");
        }
        else if ( t == E_Voice_Rtp_Discarded )
        {
            WRITE_LOG_DEBUG("Voice: ");
        }
        WRITE_LOG_DEBUG("RB: cur_seq: %hu, prev_seq: %hu, losenum: %d\n", seq, prev_seq, losenum-1);
        if(losenum > 1)
        {
            gStat[t] += (losenum-1);
        }
        
    }    
}


VOID stat_print(STAT_TYPE_E t)
{
    int   sec = 0;
    float fps = 0;
    if (timeval[t].StartTime > 0)
    {
        time(&timeval[t].EndTime);
        sec = timeval[t].EndTime - timeval[t].StartTime;
        if ( t == STAT_VIDEOENC )
        {
            fps = gStat[E_VEnc_SendFrames]/(sec);
        }
        else if ( t == STAT_VIDEODEC )
        {
            fps = gStat[E_VDec_RecvFrames]/(sec);
        }
    }
    if ( t == STAT_VIDEOENC )
    {
        WRITE_LOG_NORMAL("==VE times:%i sec. TotalFrame: %llu, Packets:%llu, fps: %6.2f\n", 
            sec, gStat[E_VEnc_RecvPackets], gStat[E_VEnc_SendFrames], fps);
        
        WRITE_LOG_NORMAL("  VE RTP Packets: Sent Packets: %llu, Bytes: %llu\n", 
            gStat[E_VEnc_Rtp_SendPackets], gStat[E_VEnc_Rtp_SendBytes]);
    }
    else if ( t == STAT_VIDEODEC )
    {
        WRITE_LOG_NORMAL("==VD times:%i sec. TotalFrame: %llu, Packets:%llu, fps: %6.2f\n", 
            sec, gStat[E_VDec_RecvFrames], gStat[E_VDec_SendPackets], fps);

        WRITE_LOG_NORMAL("  VD RTP Packets: Received Packets: %llu, Bytes: %llu, Discarded: %llu\n", 
            gStat[E_VDec_Rtp_RecvPackets], gStat[E_VDec_Rtp_RecvBytes], gStat[E_VDec_Rtp_Discarded]);
    }
    else if ( t == STAT_VOICE )
    {
        WRITE_LOG_NORMAL("==VO times:%i sec. TotalFrame: %llu, Packets:%llu, ResendTimes: %llu.\n", 
            sec, gStat[E_Voice_SendPackets], gStat[E_Voice_RecvFrames], gStat[E_Voice_ResendTimes]);

        WRITE_LOG_NORMAL("  VO RTP Packets: Received: %llu, Bytes: %llu, Discarded: %llu.\n", 
            gStat[E_Voice_Rtp_RecvPackets], gStat[E_Voice_Rtp_RecvBytes], gStat[E_Voice_Rtp_Discarded]);
        
        WRITE_LOG_NORMAL("  VO RTP Packets: Sent: %llu, Bytes: %llu.\n", 
            gStat[E_Voice_Rtp_SendPackets], gStat[E_Voice_Rtp_SendBytes]);
    }
 
}

VOID stat_print_venc(int argc, char* argv[])
{
    stat_print(STAT_VIDEOENC);
}

VOID stat_print_vdec(int argc, char* argv[])
{
    stat_print(STAT_VIDEODEC);
}

VOID stat_print_voice(int argc, char* argv[])
{
    stat_print(STAT_VOICE);
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif


