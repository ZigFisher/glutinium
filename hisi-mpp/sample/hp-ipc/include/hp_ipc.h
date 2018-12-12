#ifndef __HP_IPC_H__
#define __HP_IPC_H__

#include "hp_common.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* End of #ifdef __cplusplus */

typedef struct{
	PIC_SIZE_E e_Size;
	HI_FLOAT	f32FrameRate;

}FARMAT_S;

extern HI_S32 IPC_Init(HI_U32 u32StreamNum, FARMAT_S *stFmt_a);
extern HI_S32 IPC_exit(void);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif
