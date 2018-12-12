#ifndef __HP_VPSS_H__
#define __HP_VPSS_H__


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* End of #ifdef __cplusplus */
typedef struct{
	VPSS_GRP VpssGrp;
  	VPSS_CHN VpssChnNum;
  	VPSS_GRP_ATTR_S *pstVpssGrpAttr;
  	VPSS_CHN_ATTR_S *pstVpssChnAttr;
  	VPSS_CHN_MODE_S *pstVpssChnMode;
}VPSS_ATTR_S;


extern HI_S32 VPSS_exit(void);
extern HI_S32 VPSS_init(const IPC_VEDIO_S* pstIpcVedio);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif
