/******************************************************************************

  Copyright (C), 2001-2011, Hisilicon Tech. Co., Ltd.

 ******************************************************************************
  File Name     : piris_ext.h
  Version       : Initial Draft
  Author        : Hisilicon multimedia software group
  Created       : 2015/01/28
  Description   :
  History       :
  1.Date        : 2015/01/28
    Author      : h00191408
    Modification: Created file

******************************************************************************/
#ifndef __PIRIS_EXT_H__
#define __PIRIS_EXT_H__

#include "hi_type.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

typedef struct hiISP_PIRIS_CALLBACK_S
{
    HI_S32  (*pfn_piris_gpio_update)(HI_S32 s32Pos);
} ISP_PIRIS_CALLBACK_S;

typedef struct hiISP_PIRIS_EXPORT_FUNC_S
{
    HI_S32  (*pfnISPRegisterPirisCallBack) (HI_S32 IspDev, ISP_PIRIS_CALLBACK_S* pstPirisCb);
} ISP_PIRIS_EXPORT_FUNC_S;

extern ISP_PIRIS_EXPORT_FUNC_S g_stIspPirisExpFunc;

#define CKFN_ISP_RegisterPirisCallBack()\
    (NULL != g_stIspPirisExpFunc.pfnISPRegisterPirisCallBack)
#define CALL_ISP_RegisterPirisCallBack(IspDev,pstPirisCb)\
    g_stIspPirisExpFunc.pfnISPRegisterPirisCallBack(IspDev,pstPirisCb)

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif
