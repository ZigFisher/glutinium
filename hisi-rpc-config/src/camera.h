/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   camera.h
 * Author: ussh
 *
 * Created on 6 декабря 2018 г., 13:01
 */

#ifndef CAMERA_H
#define CAMERA_H

#include <iostream>
#include <stdio.h>
#include <cstring>

#include <sys/types.h>
#include <sys/select.h>
#include <sys/socket.h>
#include "picojson.h"

#include <hi_type.h>
#include <hi_math.h>
#include <hi_defines.h>
#include <hi_common.h>
#include <hi_comm_sys.h>
#include <hi_comm_3a.h>
#include <hi_af_comm.h>
#include <hi_comm_isp.h>
#include <hi_comm_vi.h>
#include <hi_comm_vpss.h>
#include <hi_comm_vb.h>
#include <hi_comm_video.h>
#include <hi_comm_venc.h>
#include <hi_sns_ctrl.h>
#include <hi_ae_comm.h>
#include <hi_mipi.h>
#include <mpi_sys.h>
#include <mpi_isp.h>
#include <mpi_vb.h>
#include <mpi_vi.h>
#include <mpi_venc.h>
#include <mpi_vpss.h>
#include <mpi_awb.h>
#include <mpi_ae.h>
#include <mpi_af.h>

#ifdef DEBUG
#define DBG(...) fprintf(stderr, " DBG(%s, %s(), %d): ", __FILE__, __FUNCTION__, __LINE__); fprintf(stderr, __VA_ARGS__)
#define MJPG_PRT(fmt...)   \
    do {\
        printf("[%s]-%d: ", __FUNCTION__, __LINE__);\
        printf(fmt);\
       }while(0)
#else
#define DBG(...)
#define MJPG_PRT(...)
#endif



typedef struct hiADPT_SCENEAUTO_WB_S
{
    HI_BOOL bManulEnable;
    HI_U16  u16Rgain;
    HI_U16  u16Grgain;
    HI_U16  u16Gbgain;
    HI_U16  u16Bgain;
} ADPT_SCENEAUTO_WB_S;
std::string ReturnError(std::string func, std::string exec_func, HI_S32 s32Ret);
std::string ReturnError(std::string func, std::string custom_error);
std::string ReturnSuccess(std::string func, std::string exec_func, HI_S32 s32Ret);
std::string AwbGetOpt(void);
std::string AwbSetOpt(const std::string& root);
std::string AEGetOpt(void);
std::string AESetOpt(const std::string& root);
std::string CcmGetOpt(void);
std::string CcmSetOpt(const std::string& root);
std::string SaturationGetOpt(void);
std::string SaturationSetOpt(const std::string& root);
std::string ColorToneGetOpt(void);
std::string ColorToneSetOpt(const std::string& root);
std::string GammaGetOpt(void);
std::string GammaSetOpt(const std::string& root);
std::string DRCGetOpt(void);
std::string DRCSetOpt(const std::string& root);

#endif /* CAMERA_H */

