#ifndef __HP_VENC_H__
#define __HP_VENC_H__


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* End of #ifdef __cplusplus */
typedef struct{

	VENC_CHN vencChnNum;
	HI_BOOL bGetStreamProcThreadStart;
}ST_VENC_ATTR;

extern HI_S32 VENC_init(const IPC_VEDIO_S* pstIpcVedio);
extern HI_S32 VENC_exit(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif
