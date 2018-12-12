#ifndef __HP_FPN_H__
#define __HP_FPN_H__

#include "hp_common.h"


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* End of #ifdef __cplusplus */

#define ALIGN_BACK(x, a)              ((a) * (((x) / (a))))

typedef struct st_fpn_attr
{
	SIZE_S stPicSize;
	VB_BLK VbBlk;    
    VIDEO_FRAME_INFO_S stVideoFrame;
    HI_U32 u32FrmSize;       
    ISP_FPN_TYPE_E  enFpnType;  
}FPN_ATTR_S;

extern HI_S32 FPN_init(const IPC_VEDIO_S* pstIpcVedio);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif

