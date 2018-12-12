#ifndef __HP_VI_H__
#define __HP_VI_H__

#include "hi_mipi.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* End of #ifdef __cplusplus */

typedef struct{
	ISP_DEV IspDev;
	VI_DEV ViDev;
	HI_U32 u32ChnNum;
	WDR_MODE_E  enWDRMode;
	combo_dev_attr_t *pstcomboDevAttr;
	ISP_PUB_ATTR_S *pstIspPubAttr;
	VI_DEV_ATTR_S * pstViDevAttr;
	VI_CHN_ATTR_S * pstChnAttr;
}VI_ATTR_S;

extern HI_S32 VI_exit(void);
extern HI_S32 VI_init(const IPC_VEDIO_S* pstIpcVedio);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif
