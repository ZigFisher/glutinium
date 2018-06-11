/******************************************************************************

  Copyright (C), 2001-2011, Hisilicon Tech. Co., Ltd.

 ******************************************************************************
  File Name     : isp_ext.h
  Version       : Initial Draft
  Author        : Hisilicon multimedia software group
  Created       : 2013/07/17
  Description   :
  History       :
  1.Date        : 2013/07/17
    Author      : n00168968
    Modification: Created file

******************************************************************************/
#ifndef __ISP_EXT_H__
#define __ISP_EXT_H__

#include "hi_type.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

typedef enum hiISP_BUS_TYPE_E
{
    ISP_BUS_TYPE_I2C = 0,
    ISP_BUS_TYPE_SSP,

    ISP_BUS_TYPE_BUTT,
} ISP_BUS_TYPE_E;

typedef struct hiISP_BUS_CALLBACK_S
{
    HI_S32  (*pfnISPWriteI2CData) (HI_U8 u8DevAddr, HI_U32 u32RegAddr,
                                   HI_U32 u32RegAddrByteNum, HI_U32 u32Data, HI_U32 u32DataByteNum);
    HI_S32  (*pfnISPWriteSSPData) (HI_U32 u32DevAddr, HI_U32 u32DevAddrByteNum,
                                   HI_U32 u32RegAddr, HI_U32 u32RegAddrByteNum, HI_U32 u32Data, HI_U32 u32DataByteNum);
} ISP_BUS_CALLBACK_S;

typedef struct hiISP_EXPORT_FUNC_S
{
    HI_S32  (*pfnISPRegisterBusCallBack) (HI_S32 IspDev, ISP_BUS_TYPE_E enType, ISP_BUS_CALLBACK_S* pstBusCb);
} ISP_EXPORT_FUNC_S;

extern ISP_EXPORT_FUNC_S g_stIspExpFunc;

#define CKFN_ISP_RegisterBusCallBack()\
    (NULL != g_stIspExpFunc.pfnISPRegisterBusCallBack)
#define CALL_ISP_RegisterBusCallBack(IspDev,enType,pstBusCb)\
    g_stIspExpFunc.pfnISPRegisterBusCallBack(IspDev,enType,pstBusCb)

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif
