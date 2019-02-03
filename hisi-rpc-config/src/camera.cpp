/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   camera.cpp
 * Author: ussh
 * 
 * Created on 6 декабря 2018 г., 13:01
 */

#include "camera.h"

picojson::value hibool_to_json(HI_BOOL val) {
    return picojson::value(static_cast<bool> (val));
}

picojson::value hiint_to_json(HI_U8 val) {
    return picojson::value(static_cast<double> (val));
}

picojson::value hiint_to_json(HI_U16 val) {
    return picojson::value(static_cast<double> (val));
}

picojson::value hiint_to_json(HI_U32 val) {
    return picojson::value(static_cast<double> (val));
}

picojson::value hiint_to_json(HI_S8 val) {
    return picojson::value(static_cast<double> (val));
}

picojson::value hiint_to_json(HI_S16 val) {
    return picojson::value(static_cast<double> (val));
}

picojson::value hiint_to_json(HI_S32 val) {
    return picojson::value(static_cast<double> (val));
}

picojson::value op_type_e_to_json(ISP_OP_TYPE_E input) {
    std::map<ISP_OP_TYPE_E, std::string> OP_TYPE_E;
    OP_TYPE_E[OP_TYPE_AUTO] = "OP_TYPE_AUTO";
    OP_TYPE_E[OP_TYPE_MANUAL] = "OP_TYPE_MANUAL";
    OP_TYPE_E[OP_TYPE_BUTT] = "OP_TYPE_BUTT";
    picojson::value sv(OP_TYPE_E[input]);
    return sv;
}

ISP_OP_TYPE_E op_type_e_to_type(picojson::value input) {
    std::map<std::string, ISP_OP_TYPE_E> OP_TYPE_E;
    OP_TYPE_E[std::string("OP_TYPE_AUTO")] = OP_TYPE_AUTO;
    OP_TYPE_E[std::string("OP_TYPE_MANUAL")] = OP_TYPE_MANUAL;
    OP_TYPE_E[std::string("OP_TYPE_BUTT")] = OP_TYPE_BUTT;
    return OP_TYPE_E[input.get<std::string>()];
}

picojson::value awb_alg_type_e_to_json(ISP_AWB_ALG_TYPE_E input) {
    std::map<ISP_AWB_ALG_TYPE_E, std::string> AWB_ALG_TYPE_E;
    AWB_ALG_TYPE_E[AWB_ALG_LOWCOST] = "AWB_ALG_LOWCOST";
    AWB_ALG_TYPE_E[AWB_ALG_ADVANCE] = "AWB_ALG_ADVANCE";
    AWB_ALG_TYPE_E[AWB_ALG_BUTT] = "AWB_ALG_BUTT";
    picojson::value sv(AWB_ALG_TYPE_E[input]);
    return sv;
}

ISP_AWB_ALG_TYPE_E awb_alg_type_e_to_type(picojson::value input) {
    std::map<std::string, ISP_AWB_ALG_TYPE_E> AWB_ALG_TYPE_E;
    AWB_ALG_TYPE_E[std::string("AWB_ALG_LOWCOST")] = AWB_ALG_LOWCOST;
    AWB_ALG_TYPE_E[std::string("AWB_ALG_ADVANCE")] = AWB_ALG_ADVANCE;
    AWB_ALG_TYPE_E[std::string("AWB_ALG_BUTT")] = AWB_ALG_BUTT;
    return AWB_ALG_TYPE_E[input.get<std::string>()];
}

picojson::value ae_mode_e_to_json(ISP_AE_MODE_E input) {
    std::map<ISP_AE_MODE_E, std::string> AE_MODE_E;
    AE_MODE_E[AE_MODE_SLOW_SHUTTER] = "AE_MODE_SLOW_SHUTTER";
    AE_MODE_E[AE_MODE_FIX_FRAME_RATE] = "AE_MODE_FIX_FRAME_RATE";
    AE_MODE_E[AE_MODE_BUTT] = "AE_MODE_BUTT";
    picojson::value sv(AE_MODE_E[input]);
    return sv;
}

ISP_AE_MODE_E ae_mode_e_to_type(picojson::value input) {
    std::map<std::string, ISP_AE_MODE_E> AE_MODE_E;
    AE_MODE_E[std::string("AE_MODE_SLOW_SHUTTER")] = AE_MODE_SLOW_SHUTTER;
    AE_MODE_E[std::string("AE_MODE_FIX_FRAME_RATE")] = AE_MODE_FIX_FRAME_RATE;
    AE_MODE_E[std::string("AE_MODE_BUTT")] = AE_MODE_BUTT;
    return AE_MODE_E[input.get<std::string>()];
}

picojson::value ae_strategy_e_to_json(ISP_AE_STRATEGY_E input) {
    std::map<ISP_AE_STRATEGY_E, std::string> AE_STRATEGY_E;
    AE_STRATEGY_E[AE_EXP_HIGHLIGHT_PRIOR] = "AE_EXP_HIGHLIGHT_PRIOR";
    AE_STRATEGY_E[AE_EXP_LOWLIGHT_PRIOR] = "AE_EXP_LOWLIGHT_PRIOR";
    AE_STRATEGY_E[AE_STRATEGY_MODE_BUTT] = "AE_STRATEGY_MODE_BUTT";
    picojson::value sv(AE_STRATEGY_E[input]);
    return sv;
}

ISP_AE_STRATEGY_E ae_strategy_e_to_type(picojson::value input) {
    std::map<std::string, ISP_AE_STRATEGY_E> AE_STRATEGY_E;
    AE_STRATEGY_E[std::string("AE_EXP_HIGHLIGHT_PRIOR")] = AE_EXP_HIGHLIGHT_PRIOR;
    AE_STRATEGY_E[std::string("AE_EXP_LOWLIGHT_PRIOR")] = AE_EXP_LOWLIGHT_PRIOR;
    AE_STRATEGY_E[std::string("AE_STRATEGY_MODE_BUTT")] = AE_STRATEGY_MODE_BUTT;
    return AE_STRATEGY_E[input.get<std::string>()];
}

picojson::value antiflicker_mode_e_to_json(ISP_ANTIFLICKER_MODE_E input) {
    std::map<ISP_ANTIFLICKER_MODE_E, std::string> ANTIFLICKER_MODE_E;
    ANTIFLICKER_MODE_E[ISP_ANTIFLICKER_NORMAL_MODE] = "ISP_ANTIFLICKER_NORMAL_MODE";
    ANTIFLICKER_MODE_E[ISP_ANTIFLICKER_AUTO_MODE] = "ISP_ANTIFLICKER_AUTO_MODE";
    ANTIFLICKER_MODE_E[ISP_ANTIFLICKER_MODE_BUTT] = "ISP_ANTIFLICKER_MODE_BUTT";
    picojson::value sv(ANTIFLICKER_MODE_E[input]);
    return sv;
}

ISP_ANTIFLICKER_MODE_E antiflicker_mode_e_to_type(picojson::value input) {
    std::map<std::string, ISP_ANTIFLICKER_MODE_E> ANTIFLICKER_MODE_E;
    ANTIFLICKER_MODE_E[std::string("ISP_ANTIFLICKER_NORMAL_MODE")] = ISP_ANTIFLICKER_NORMAL_MODE;
    ANTIFLICKER_MODE_E[std::string("ISP_ANTIFLICKER_AUTO_MODE")] = ISP_ANTIFLICKER_AUTO_MODE;
    ANTIFLICKER_MODE_E[std::string("ISP_ANTIFLICKER_MODE_BUTT")] = ISP_ANTIFLICKER_MODE_BUTT;
    return ANTIFLICKER_MODE_E[input.get<std::string>()];
}

picojson::value ae_range_to_json(ISP_AE_RANGE_S input) {
    picojson::object ret;
    ret["u32Max"] = picojson::value(static_cast<double> (input.u32Max));
    ret["u32Min"] = picojson::value(static_cast<double> (input.u32Min));
    return picojson::value(ret);
}

std::string ReturnError(std::string func, std::string exec_func, HI_S32 s32Ret) {
    char numstr[255];
    sprintf(numstr, "{\"error\": \"%s: %s failed with error %#x\"}",
            func.c_str(), exec_func.c_str(), s32Ret);
    fprintf(stderr, "%s\n", numstr);
    return numstr;
}

std::string ReturnError(std::string func, std::string custom_error) {
    char numstr[255];
    sprintf(numstr, "{\"error\": \"%s\"}",
            func.c_str(), custom_error);
    fprintf(stderr, "%s\n", numstr);
    return numstr;
}

std::string ReturnSuccess(std::string func, std::string exec_func, HI_S32 s32Ret) {
    char numstr[255];
    sprintf(numstr, "{\"success\": \"%s: %s success with code %#x\"}",
            func.c_str(), exec_func.c_str(), s32Ret);
    fprintf(stderr, "%s\n", numstr);
    return numstr;
}

std::string CcmSetOpt(const std::string& root) {
    picojson::value Jroot;
    std::string err = picojson::parse(Jroot, root);
//    std::cout << root << std::endl;
    if (!err.empty()) {
        return ReturnError(__func__, err);
    }
    if (!Jroot.is<picojson::object>()) {
        return ReturnError(__func__, "JSON is not an object");
    }
    HI_S32 s32Ret = HI_SUCCESS;
    ISP_DEV IspDev = 0;

    ISP_COLORMATRIX_ATTR_S stCCMAttr;

    s32Ret = HI_MPI_ISP_GetCCMAttr(IspDev, &stCCMAttr);
    if (HI_SUCCESS != s32Ret) {
        return ReturnError(__func__, "HI_MPI_ISP_GetCCMAttr", s32Ret);
    }
    stCCMAttr.enOpType = op_type_e_to_type(Jroot.get("enOpType"));
    if (stCCMAttr.enOpType == OP_TYPE_AUTO) {
        ISP_COLORMATRIX_AUTO_S MstAuto;
        MstAuto.bISOActEn = static_cast<HI_BOOL> (Jroot.get("stAuto").get("bISOActEn").get<bool>());
        MstAuto.bTempActEn = static_cast<HI_BOOL> (Jroot.get("stAuto").get("bTempActEn").get<bool>());
        MstAuto.u16HighColorTemp = static_cast<HI_U16> (Jroot.get("stAuto").get("u16HighColorTemp").get<double>());
        MstAuto.u16LowColorTemp = static_cast<HI_U16> (Jroot.get("stAuto").get("u16LowColorTemp").get<double>());
        MstAuto.u16MidColorTemp = static_cast<HI_U16> (Jroot.get("stAuto").get("u16MidColorTemp").get<double>());
                
        picojson::array au16HighCCM = Jroot.get("stAuto").get("au16HighCCM").get<picojson::array>();
        int i = 0;
        for (picojson::array::iterator iter = au16HighCCM.begin(); iter != au16HighCCM.end(); ++iter) {
            MstAuto.au16HighCCM[i] = static_cast<HI_U16> ((*iter).get<double>());
            i++;
        }
        
        picojson::array au16LowCCM = Jroot.get("stAuto").get("au16LowCCM").get<picojson::array>();
        i = 0;
        for (picojson::array::iterator iter = au16LowCCM.begin(); iter != au16LowCCM.end(); ++iter) {
            MstAuto.au16LowCCM[i] = static_cast<HI_U16> ((*iter).get<double>());
            i++;
        }
        
        picojson::array au16MidCCM = Jroot.get("stAuto").get("au16MidCCM").get<picojson::array>();
        i = 0;
        for (picojson::array::iterator iter = au16MidCCM.begin(); iter != au16MidCCM.end(); ++iter) {
            MstAuto.au16MidCCM[i] = static_cast<HI_U16> ((*iter).get<double>());
            i++;
        }
        stCCMAttr.stAuto = MstAuto;
    } else if (stCCMAttr.enOpType == OP_TYPE_MANUAL) {
        ISP_COLORMATRIX_MANUAL_S MstManual;
        picojson::array au16CCM = Jroot.get("stManual").get("au16CCM").get<picojson::array>();
        int i = 0;
        for (picojson::array::iterator iter = au16CCM.begin(); iter != au16CCM.end(); ++iter) {
            MstManual.au16CCM[i] = static_cast<HI_U16> ((*iter).get<double>());
            i++;
        }
        MstManual.bSatEn = static_cast<HI_BOOL> (Jroot.get("stManual").get("bSatEn").get<bool>());
        stCCMAttr.stManual = MstManual;
    } 
    s32Ret = HI_MPI_ISP_SetCCMAttr(IspDev, &stCCMAttr);
    if (HI_SUCCESS != s32Ret) {
        return ReturnError(__func__, "HI_MPI_ISP_SetCCMAttr", s32Ret);
    }

    return ReturnSuccess(__func__, "HI_MPI_ISP_SetCCMAttr", s32Ret);
}

std::string CcmGetOpt(void) {
    HI_S32 s32Ret = HI_SUCCESS;
    ISP_DEV IspDev = 0;
    ISP_COLORMATRIX_ATTR_S stCCMAttr;

    s32Ret = HI_MPI_ISP_GetCCMAttr(IspDev, &stCCMAttr);
    if (HI_SUCCESS != s32Ret) {
        return ReturnError(__func__, "HI_MPI_ISP_GetCCMAttr", s32Ret);
    }

    picojson::object Jroot;
    picojson::object JstAuto;
    picojson::object JstManual;
    picojson::array PAau16CCM;
    picojson::array PAau16HighCCM;
    picojson::array PAau16MidCCM;
    picojson::array PAau16LowCCM;

    Jroot["enOpType"] = op_type_e_to_json(stCCMAttr.enOpType);

    JstManual["bSatEn"] = hibool_to_json(stCCMAttr.stManual.bSatEn);
    for (int i = 0; i < 9; i++) {
        PAau16CCM.push_back(picojson::value(static_cast<double> (stCCMAttr.stManual.au16CCM[i])));
    }

    JstManual["au16CCM"] = picojson::value(PAau16CCM);
    Jroot["stManual"] = picojson::value(JstManual);
    JstAuto["bISOActEn"] = hibool_to_json(stCCMAttr.stAuto.bISOActEn);
    JstAuto["bTempActEn"] = hibool_to_json(stCCMAttr.stAuto.bTempActEn);
    JstAuto["u16HighColorTemp"] = picojson::value(static_cast<double> (stCCMAttr.stAuto.u16HighColorTemp));
    JstAuto["u16MidColorTemp"] = picojson::value(static_cast<double> (stCCMAttr.stAuto.u16MidColorTemp));
    JstAuto["u16LowColorTemp"] = picojson::value(static_cast<double> (stCCMAttr.stAuto.u16LowColorTemp));

    for (int i = 0; i < 9; i++) {
        PAau16HighCCM.push_back(picojson::value(static_cast<double> (stCCMAttr.stAuto.au16HighCCM[i])));
    }
    JstAuto["au16HighCCM"] = picojson::value(PAau16HighCCM);
    for (int i = 0; i < 9; i++) {
        PAau16MidCCM.push_back(picojson::value(static_cast<double> (stCCMAttr.stAuto.au16MidCCM[i])));
    }
    JstAuto["au16MidCCM"] = picojson::value(PAau16MidCCM);
    for (int i = 0; i < 9; i++) {
        PAau16LowCCM.push_back(picojson::value(static_cast<double> (stCCMAttr.stAuto.au16LowCCM[i])));
    }
    JstAuto["au16LowCCM"] = picojson::value(PAau16LowCCM);
    Jroot["stAuto"] = picojson::value(JstAuto);
    return picojson::value(Jroot).serialize();
}

std::string SaturationSetOpt(const std::string& root) {
    picojson::value Jroot;
    std::string err = picojson::parse(Jroot, root);
//    std::cout << root << std::endl;
    if (!err.empty()) {
        return ReturnError(__func__, err);
    }
    if (!Jroot.is<picojson::object>()) {
        return ReturnError(__func__, "JSON is not an object");
    }
    HI_S32 s32Ret = HI_SUCCESS;
    ISP_DEV IspDev = 0;

    ISP_SATURATION_ATTR_S stSatAttr;

    s32Ret = HI_MPI_ISP_GetSaturationAttr(IspDev, &stSatAttr);
    if (HI_SUCCESS != s32Ret) {
        return ReturnError(__func__, "HI_MPI_ISP_GetSaturationAttr", s32Ret);
    }
    stSatAttr.enOpType = op_type_e_to_type(Jroot.get("enOpType"));
    if (stSatAttr.enOpType == OP_TYPE_AUTO) {
        picojson::array au8Sat = Jroot.get("stAuto").get("au8Sat").get<picojson::array>();
        int i = 0;
        for (picojson::array::iterator iter = au8Sat.begin(); iter != au8Sat.end(); ++iter) {
            stSatAttr.stAuto.au8Sat[i] = static_cast<HI_U8> ((*iter).get<double>());
            i++;
        }
        i = 0;
        for (picojson::array::iterator iter = au8Sat.begin(); iter != au8Sat.end(); ++iter) {
            stSatAttr.stAuto.au8Sat[i] = static_cast<HI_U8> ((*iter).get<double>());
            i++;
        }
    } else if (stSatAttr.enOpType == OP_TYPE_MANUAL) {
        stSatAttr.stManual.u8Saturation = 
                static_cast<HI_U8> (Jroot.get("stManual").get("u8Saturation").get<double>());
    }
    s32Ret = HI_MPI_ISP_SetSaturationAttr(IspDev, &stSatAttr);
    if (HI_SUCCESS != s32Ret) {
        return ReturnError(__func__, "HI_MPI_ISP_SetSaturationAttr", s32Ret);
    }

    return ReturnSuccess(__func__, "HI_MPI_ISP_SetSaturationAttr", s32Ret);
    
}

std::string SaturationGetOpt(void) {
    HI_S32 s32Ret = HI_SUCCESS;
    ISP_DEV IspDev = 0;
    ISP_SATURATION_ATTR_S stSatAttr;

    s32Ret = HI_MPI_ISP_GetSaturationAttr(IspDev, &stSatAttr);
    if (HI_SUCCESS != s32Ret) {
        return ReturnError(__func__, "HI_MPI_ISP_GetSaturationAttr", s32Ret);
    }

    picojson::object Jroot;
    picojson::object JstAuto;
    picojson::object JstManual;
    picojson::array PAau8Sat;
    for (int i = 0; i < ISP_AUTO_ISO_STENGTH_NUM; i++) {
        PAau8Sat.push_back(hiint_to_json(stSatAttr.stAuto.au8Sat[i]));
    }

    JstManual["u8Saturation"] = hiint_to_json(stSatAttr.stManual.u8Saturation);
    JstAuto["au8Sat"] = picojson::value(PAau8Sat);
    Jroot["enOpType"] = op_type_e_to_json(stSatAttr.enOpType);
    Jroot["stAuto"] = picojson::value(JstAuto);
    Jroot["stManual"] = picojson::value(JstManual);
    return picojson::value(Jroot).serialize();
}

std::string ColorToneSetOpt(const std::string& root) {
    picojson::value Jroot;
    std::string err = picojson::parse(Jroot, root);
//    std::cout << root << std::endl;
    if (!err.empty()) {
        return ReturnError(__func__, err);
    }
    if (!Jroot.is<picojson::object>()) {
        return ReturnError(__func__, "JSON is not an object");
    }
    HI_S32 s32Ret = HI_SUCCESS;
    ISP_DEV IspDev = 0;

    ISP_COLOR_TONE_ATTR_S stCTAttr;

    s32Ret = HI_MPI_ISP_GetColorToneAttr(IspDev, &stCTAttr);
    if (HI_SUCCESS != s32Ret) {
        return ReturnError(__func__, "HI_MPI_ISP_SetColorToneAttr", s32Ret);
    }
    stCTAttr.u16BlueCastGain = static_cast<HI_U16> (Jroot.get("u16BlueCastGain").get<double>());
    stCTAttr.u16GreenCastGain = static_cast<HI_U16> (Jroot.get("u16GreenCastGain").get<double>());
    stCTAttr.u16RedCastGain = static_cast<HI_U16> (Jroot.get("u16RedCastGain").get<double>());
    
    s32Ret = HI_MPI_ISP_SetColorToneAttr(IspDev, &stCTAttr);
    if (HI_SUCCESS != s32Ret) {
        return ReturnError(__func__, "HI_MPI_ISP_SetColorToneAttr", s32Ret);
    }

    return ReturnSuccess(__func__, "HI_MPI_ISP_SetColorToneAttr", s32Ret);
}

std::string ColorToneGetOpt(void) {
    HI_S32 s32Ret = HI_SUCCESS;
    ISP_DEV IspDev = 0;
    ISP_COLOR_TONE_ATTR_S stCTAttr;

    s32Ret = HI_MPI_ISP_GetColorToneAttr(IspDev, &stCTAttr);
    if (HI_SUCCESS != s32Ret) {
        return ReturnError(__func__, "HI_MPI_ISP_SetColorToneAttr", s32Ret);
    }

    picojson::object Jroot;
    Jroot["u16RedCastGain"] = hiint_to_json(stCTAttr.u16RedCastGain);
    Jroot["u16GreenCastGain"] = hiint_to_json(stCTAttr.u16GreenCastGain);
    Jroot["u16BlueCastGain"] = hiint_to_json(stCTAttr.u16BlueCastGain);
    return picojson::value(Jroot).serialize();
}

std::string AwbGetOpt(void) {
    HI_S32 s32Ret = HI_SUCCESS;
    ISP_DEV IspDev = 0;
    ISP_WB_ATTR_S stWBAttr;

    s32Ret = HI_MPI_ISP_GetWBAttr(IspDev, &stWBAttr);
    if (HI_SUCCESS != s32Ret) {
        return ReturnError(__func__, "HI_MPI_ISP_GetWBAttr", s32Ret);
    }

    picojson::object Jroot;
    picojson::object JstAuto;
    picojson::object JstManual;
    picojson::object JstCTLimit;
    picojson::object JstCbCrTrack;
    picojson::object JstLumaHist;
    JstCTLimit["bEnable"] = hibool_to_json(stWBAttr.stAuto.stCTLimit.bEnable);
    JstCTLimit["enOpType"] = op_type_e_to_json(stWBAttr.stAuto.stCTLimit.enOpType);
    JstCTLimit["u16HighBgLimit"] = hiint_to_json(stWBAttr.stAuto.stCTLimit.u16HighBgLimit);
    JstCTLimit["u16HighRgLimit"] = hiint_to_json(stWBAttr.stAuto.stCTLimit.u16HighRgLimit);
    JstCTLimit["u16LowBgLimit"] = hiint_to_json(stWBAttr.stAuto.stCTLimit.u16LowBgLimit);
    JstCTLimit["u16LowRgLimit"] = hiint_to_json(stWBAttr.stAuto.stCTLimit.u16LowRgLimit);

    picojson::array as32CurvePara;
    for (int i = 0; i < 6; i++) {
        as32CurvePara.push_back(hiint_to_json(stWBAttr.stAuto.as32CurvePara[i]));
    }
    picojson::array au16StaticWB;
    for (int i = 0; i < 4; i++) {
        au16StaticWB.push_back(hiint_to_json(stWBAttr.stAuto.au16StaticWB[i]));
    }

    JstCbCrTrack["bEnable"] = hibool_to_json(stWBAttr.stAuto.stCbCrTrack.bEnable);

    picojson::array au16CbMax;

    for (int i = 0; i < ISP_AUTO_ISO_STENGTH_NUM; i++) {
        au16CbMax.push_back(hiint_to_json(stWBAttr.stAuto.stCbCrTrack.au16CbMax[i]));
    }
    JstCbCrTrack["au16CbMax"] = picojson::value(au16CbMax);

    picojson::array au16CbMin;

    for (int i = 0; i < ISP_AUTO_ISO_STENGTH_NUM; i++) {
        au16CbMin.push_back(hiint_to_json(stWBAttr.stAuto.stCbCrTrack.au16CbMin[i]));
    }
    JstCbCrTrack["au16CbMin"] = picojson::value(au16CbMin);

    picojson::array au16CrMax;

    for (int i = 0; i < ISP_AUTO_ISO_STENGTH_NUM; i++) {
        au16CrMax.push_back(hiint_to_json(stWBAttr.stAuto.stCbCrTrack.au16CrMax[i]));
    }
    JstCbCrTrack["au16CrMax"] = picojson::value(au16CrMax);

    picojson::array au16CrMin;

    for (int i = 0; i < ISP_AUTO_ISO_STENGTH_NUM; i++) {
        au16CrMin.push_back(hiint_to_json(stWBAttr.stAuto.stCbCrTrack.au16CrMin[i]));
    }
    JstCbCrTrack["au16CrMin"] = picojson::value(au16CrMin);

    picojson::array au16HistWt;
    for (int i = 0; i < 6; i++) {
        au16HistWt.push_back(hiint_to_json(stWBAttr.stAuto.stLumaHist.au16HistWt[i]));
    }

    picojson::array au8HistThresh;
    for (int i = 0; i < 6; i++) {
        au8HistThresh.push_back(hiint_to_json(stWBAttr.stAuto.stLumaHist.au8HistThresh[i]));
    }

    JstLumaHist["bEnable"] = hibool_to_json(stWBAttr.stAuto.stLumaHist.bEnable);
    JstLumaHist["au16HistWt"] = picojson::value(au16HistWt);
    JstLumaHist["au8HistThresh"] = picojson::value(au8HistThresh);
    JstLumaHist["enOpType"] = op_type_e_to_json(stWBAttr.stAuto.stLumaHist.enOpType);
    JstAuto["as32CurvePara"] = picojson::value(as32CurvePara);
    JstAuto["au16StaticWB"] = picojson::value(au16StaticWB);
    JstAuto["bEnable"] = hibool_to_json(stWBAttr.stAuto.bEnable);
    JstAuto["bGainNormEn"] = hibool_to_json(stWBAttr.stAuto.bGainNormEn);
    JstAuto["bShiftLimitEn"] = hibool_to_json(stWBAttr.stAuto.bShiftLimitEn);
    JstAuto["enAlgType"] = awb_alg_type_e_to_json(stWBAttr.stAuto.enAlgType);
    JstAuto["stCTLimit"] = picojson::value(JstCTLimit);
    JstAuto["stCbCrTrack"] = picojson::value(JstCbCrTrack);
    JstAuto["stLumaHist"] = picojson::value(JstLumaHist);
    JstAuto["u16HighColorTemp"] = hiint_to_json(stWBAttr.stAuto.u16HighColorTemp);
    JstAuto["u16LowColorTemp"] = hiint_to_json(stWBAttr.stAuto.u16LowColorTemp);
    JstAuto["u16RefColorTemp"] = hiint_to_json(stWBAttr.stAuto.u16RefColorTemp);
    JstAuto["u16Speed"] = hiint_to_json(stWBAttr.stAuto.u16Speed);
    JstAuto["u16ZoneSel"] = hiint_to_json(stWBAttr.stAuto.u16ZoneSel);
    JstAuto["u8BGStrength"] = hiint_to_json(stWBAttr.stAuto.u8BGStrength);
    JstAuto["u8RGStrength"] = hiint_to_json(stWBAttr.stAuto.u8RGStrength);
    JstAuto["u8ShiftLimit"] = hiint_to_json(stWBAttr.stAuto.u8ShiftLimit);
    Jroot["stAuto"] = picojson::value(JstAuto);
    Jroot["enOpType"] = op_type_e_to_json(stWBAttr.enOpType);
    JstManual["u16Rgain"] = hiint_to_json(stWBAttr.stManual.u16Rgain);
    JstManual["u16Grgain"] = hiint_to_json(stWBAttr.stManual.u16Grgain);
    JstManual["u16Gbgain"] = hiint_to_json(stWBAttr.stManual.u16Gbgain);
    JstManual["u16Bgain"] = hiint_to_json(stWBAttr.stManual.u16Bgain);
    Jroot["stManual"] = picojson::value(JstManual);
    Jroot["bByPass"] = hibool_to_json(stWBAttr.bByPass);
    return picojson::value(Jroot).serialize();
}

std::string GammaGetOpt(void) {
    HI_S32 s32Ret = HI_SUCCESS;
    ISP_DEV IspDev = 0;
    ISP_GAMMA_ATTR_S stGammaAttr;

    s32Ret = HI_MPI_ISP_GetGammaAttr(IspDev, &stGammaAttr);
    if (HI_SUCCESS != s32Ret) {
        return ReturnError(__func__, "HI_MPI_ISP_GetGammaAttr", s32Ret);
    }
    picojson::object Jroot;
    Jroot["bEnable"] = hibool_to_json(stGammaAttr.bEnable);
    
    std::map<ISP_GAMMA_CURVE_TYPE_E, std::string> GAMMA_CURVE_TYPE_E;
    GAMMA_CURVE_TYPE_E[ISP_GAMMA_CURVE_DEFAULT] = "ISP_GAMMA_CURVE_DEFAULT";
    GAMMA_CURVE_TYPE_E[ISP_GAMMA_CURVE_SRGB] = "ISP_GAMMA_CURVE_SRGB";
    GAMMA_CURVE_TYPE_E[ISP_GAMMA_CURVE_USER_DEFINE] = "ISP_GAMMA_CURVE_USER_DEFINE";
    GAMMA_CURVE_TYPE_E[ISP_GAMMA_CURVE_BUTT] = "ISP_GAMMA_CURVE_BUTT";
    
    Jroot["enCurveType"] = picojson::value(GAMMA_CURVE_TYPE_E[stGammaAttr.enCurveType]);
    picojson::array PAu16Table;
    for (int i = 0; i < GAMMA_NODE_NUM; i++) {
        PAu16Table.push_back(hiint_to_json(stGammaAttr.u16Table[i]));
    }
    Jroot["u16Table"] = picojson::value(PAu16Table);
    return picojson::value(Jroot).serialize();
}

std::string GammaSetOpt(const std::string& root) {
    picojson::value Jroot;
    std::string err = picojson::parse(Jroot, root);
//    std::cout << root << std::endl;
    if (!err.empty()) {
        return ReturnError(__func__, err);
    }
    if (!Jroot.is<picojson::object>()) {
        return ReturnError(__func__, "JSON is not an object");
    }

    HI_S32 s32Ret = HI_SUCCESS;
    ISP_DEV IspDev = 0;

    ISP_GAMMA_ATTR_S stGammaAttr;

    s32Ret = HI_MPI_ISP_GetGammaAttr(IspDev, &stGammaAttr);
    if (HI_SUCCESS != s32Ret) {
        return ReturnError(__func__, "HI_MPI_ISP_GetGammaAttr", s32Ret);
    }
    stGammaAttr.bEnable = static_cast<HI_BOOL> (Jroot.get("bEnable").get<bool>());
    std::map<std::string, ISP_GAMMA_CURVE_TYPE_E> GAMMA_CURVE_TYPE_E;
    GAMMA_CURVE_TYPE_E[std::string("ISP_GAMMA_CURVE_DEFAULT")] = ISP_GAMMA_CURVE_DEFAULT;
    GAMMA_CURVE_TYPE_E[std::string("ISP_GAMMA_CURVE_SRGB")] = ISP_GAMMA_CURVE_SRGB;
    GAMMA_CURVE_TYPE_E[std::string("ISP_GAMMA_CURVE_USER_DEFINE")] = ISP_GAMMA_CURVE_USER_DEFINE;
    GAMMA_CURVE_TYPE_E[std::string("ISP_GAMMA_CURVE_BUTT")] = ISP_GAMMA_CURVE_BUTT;
    stGammaAttr.enCurveType = GAMMA_CURVE_TYPE_E[Jroot.get("enCurveType").get<std::string>()];
    picojson::array u16Table = Jroot.get("u16Table").get<picojson::array>();
    int i = 0;
    for (picojson::array::iterator iter = u16Table.begin(); iter != u16Table.end(); ++iter) {
        stGammaAttr.u16Table[i] = static_cast<HI_U16> ((*iter).get<double>());
        i++;
    }
    s32Ret = HI_MPI_ISP_SetGammaAttr(IspDev, &stGammaAttr);
    if (HI_SUCCESS != s32Ret) {
        return ReturnError(__func__, "HI_MPI_ISP_SetGammaAttr", s32Ret);
    }

    return ReturnSuccess(__func__, "HI_MPI_ISP_SetGammaAttr", s32Ret);
}

std::string DRCGetOpt(void) {
    HI_S32 s32Ret = HI_SUCCESS;
    ISP_DEV IspDev = 0;
    ISP_DRC_ATTR_S stDRC;

    s32Ret = HI_MPI_ISP_GetDRCAttr(IspDev, &stDRC);
    if (HI_SUCCESS != s32Ret) {
        return ReturnError(__func__, "HI_MPI_ISP_GetDRCAttr", s32Ret);
    }
    picojson::object Jroot;
    picojson::object JstAuto;
    picojson::object JstManual;
    
    Jroot["bEnable"] = hibool_to_json(stDRC.bEnable);
    Jroot["u16BrightGainLmt"] = hiint_to_json(stDRC.u16BrightGainLmt);
    Jroot["u16DarkGainLmtC"] = hiint_to_json(stDRC.u16DarkGainLmtC);
    Jroot["u16DarkGainLmtY"] = hiint_to_json(stDRC.u16DarkGainLmtY);
    Jroot["u8Asymmetry"] = hiint_to_json(stDRC.u8Asymmetry);
    Jroot["u8LocalMixingThres"] = hiint_to_json(stDRC.u8LocalMixingThres);
    Jroot["u8RangeVar"] = hiint_to_json(stDRC.u8RangeVar);
    Jroot["u8SecondPole"] = hiint_to_json(stDRC.u8SecondPole);
    Jroot["u8SpatialVar"] = hiint_to_json(stDRC.u8SpatialVar);
    Jroot["u8Stretch"] = hiint_to_json(stDRC.u8Stretch);
    picojson::array PAau16ColorCorrectionLut;
    for (int i = 0; i < COLORCORRECTIONLUT_NODE_NUMBER; i++) {
        PAau16ColorCorrectionLut.push_back(hiint_to_json(stDRC.au16ColorCorrectionLut[i]));
    }
    Jroot["au16ColorCorrectionLut"] = picojson::value(PAau16ColorCorrectionLut);
    Jroot["enOpType"] = op_type_e_to_json(stDRC.enOpType);
    JstAuto["u8LocalMixingBright"] = hiint_to_json(stDRC.stAuto.u8LocalMixingBright);
    JstAuto["u8LocalMixingDark"] = hiint_to_json(stDRC.stAuto.u8LocalMixingDark);
    JstAuto["u8Strength"] = hiint_to_json(stDRC.stAuto.u8Strength);
    
    JstManual["u8LocalMixingBright"] = hiint_to_json(stDRC.stManual.u8LocalMixingBright);
    JstManual["u8LocalMixingDark"] = hiint_to_json(stDRC.stManual.u8LocalMixingDark);
    JstManual["u8Strength"] = hiint_to_json(stDRC.stManual.u8Strength);
    
    Jroot["stAuto"] = picojson::value(JstAuto);
    Jroot["stManual"] = picojson::value(JstManual);
    return picojson::value(Jroot).serialize();
}

std::string DRCSetOpt(const std::string& root) {
    picojson::value Jroot;
    std::string err = picojson::parse(Jroot, root);
//    std::cout << root << std::endl;
    if (!err.empty()) {
        return ReturnError(__func__, err);
    }
    if (!Jroot.is<picojson::object>()) {
        return ReturnError(__func__, "JSON is not an object");
    }

    HI_S32 s32Ret = HI_SUCCESS;
    ISP_DEV IspDev = 0;

    ISP_DRC_ATTR_S stDRC;

    s32Ret = HI_MPI_ISP_GetDRCAttr(IspDev, &stDRC);
    if (HI_SUCCESS != s32Ret) {
        return ReturnError(__func__, "HI_MPI_ISP_GetDRCAttr", s32Ret);
    }
    stDRC.bEnable = static_cast<HI_BOOL> (Jroot.get("bEnable").get<bool>());
    stDRC.u16BrightGainLmt = static_cast<HI_U16> (Jroot.get("u16BrightGainLmt").get<double>());
    stDRC.u16DarkGainLmtC = static_cast<HI_U16> (Jroot.get("u16DarkGainLmtC").get<double>());
    stDRC.u16DarkGainLmtY = static_cast<HI_U16> (Jroot.get("u16DarkGainLmtY").get<double>());
    stDRC.u8Asymmetry = static_cast<HI_U8> (Jroot.get("u8Asymmetry").get<double>());
    stDRC.u8LocalMixingThres = static_cast<HI_U8> (Jroot.get("u8LocalMixingThres").get<double>());
    stDRC.u8RangeVar = static_cast<HI_U8> (Jroot.get("u8RangeVar").get<double>());
    stDRC.u8SecondPole = static_cast<HI_U8> (Jroot.get("u8SecondPole").get<double>());
    stDRC.u8SpatialVar = static_cast<HI_U8> (Jroot.get("u8SpatialVar").get<double>());
    stDRC.u8Stretch = static_cast<HI_U8> (Jroot.get("u8Stretch").get<double>());
    stDRC.enOpType = op_type_e_to_type(Jroot.get("enOpType"));
    if (stDRC.enOpType == OP_TYPE_AUTO) {
        stDRC.stAuto.u8LocalMixingBright = static_cast<HI_U8> (Jroot.get("stAuto").get("u8LocalMixingBright").get<double>());
        stDRC.stAuto.u8LocalMixingDark = static_cast<HI_U8> (Jroot.get("stAuto").get("u8LocalMixingDark").get<double>());
        stDRC.stAuto.u8Strength = static_cast<HI_U8> (Jroot.get("stAuto").get("u8Strength").get<double>());
    } else if (stDRC.enOpType == OP_TYPE_MANUAL) {
        stDRC.stManual.u8LocalMixingBright = static_cast<HI_U8> (Jroot.get("stManual").get("u8LocalMixingBright").get<double>());
        stDRC.stManual.u8LocalMixingDark = static_cast<HI_U8> (Jroot.get("stManual").get("u8LocalMixingDark").get<double>());
        stDRC.stManual.u8Strength = static_cast<HI_U8> (Jroot.get("stManual").get("u8Strength").get<double>());
    }
    s32Ret = HI_MPI_ISP_SetDRCAttr(IspDev, &stDRC);
    if (HI_SUCCESS != s32Ret) {
        return ReturnError(__func__, "HI_MPI_ISP_SetDRCAttr", s32Ret);
    }

    return ReturnSuccess(__func__, "HI_MPI_ISP_SetDRCAttr", s32Ret);
}

std::string AEGetOpt(void) {
    HI_S32 s32Ret = HI_SUCCESS;
    ISP_DEV IspDev = 0;
    ISP_EXPOSURE_ATTR_S stExpAttr;

    s32Ret = HI_MPI_ISP_GetExposureAttr(IspDev, &stExpAttr);
    if (HI_SUCCESS != s32Ret) {
        return ReturnError(__func__, "HI_MPI_ISP_GetExposureAttr", s32Ret);
    }

    picojson::object Jroot;
    picojson::object JstAuto;
    picojson::object JstManual;
    picojson::object JstAEDelayAttr;
    picojson::object JstAntiflicker;
    picojson::object JstSubflicker;

    Jroot["bByPass"] = hibool_to_json(stExpAttr.bByPass);
    Jroot["bAERouteExValid"] = hibool_to_json(stExpAttr.bAERouteExValid);
    Jroot["bHistStatAdjust"] = hibool_to_json(stExpAttr.bHistStatAdjust);
    Jroot["u8AERunInterval"] = hiint_to_json(stExpAttr.u8AERunInterval);
    Jroot["enOpType"] = op_type_e_to_json(stExpAttr.enOpType);
    
    picojson::array PAau8Weight;
    for (int i = 0; i < AE_ZONE_ROW; i++) {
        for (int j = 0; j < AE_ZONE_COLUMN; j++) {
            PAau8Weight.push_back(hiint_to_json(stExpAttr.stAuto.au8Weight[i][j]));
        }
    }
    JstAuto["au8Weight"] = picojson::value(PAau8Weight);
    JstAuto["bManualExpValue"] = hibool_to_json(stExpAttr.stAuto.bManualExpValue);
    JstAuto["enAEMode"] = ae_mode_e_to_json(stExpAttr.stAuto.enAEMode);
    JstAuto["enAEStrategyMode"] = ae_strategy_e_to_json(stExpAttr.stAuto.enAEStrategyMode);
    JstAEDelayAttr["u16BlackDelayFrame"] = hiint_to_json(stExpAttr.stAuto.stAEDelayAttr.u16BlackDelayFrame);
    JstAEDelayAttr["u16WhiteDelayFrame"] = hiint_to_json(stExpAttr.stAuto.stAEDelayAttr.u16WhiteDelayFrame);
    JstAuto["stAEDelayAttr"] = picojson::value(JstAEDelayAttr);
    JstAuto["stAGainRange"] = ae_range_to_json(stExpAttr.stAuto.stAGainRange);
    JstAntiflicker["bEnable"] = hibool_to_json(stExpAttr.stAuto.stAntiflicker.bEnable);
    JstAntiflicker["u8Frequency"] = hiint_to_json(stExpAttr.stAuto.stAntiflicker.u8Frequency);
    JstAntiflicker["enMode"] = antiflicker_mode_e_to_json(stExpAttr.stAuto.stAntiflicker.enMode);
    JstAuto["stAntiflicker"] = picojson::value(JstAntiflicker);
    JstAuto["stDGainRange"] = ae_range_to_json(stExpAttr.stAuto.stDGainRange);
    JstAuto["stExpTimeRange"] = ae_range_to_json(stExpAttr.stAuto.stExpTimeRange);
    JstSubflicker["bEnable"] = hibool_to_json(stExpAttr.stAuto.stSubflicker.bEnable);
    JstSubflicker["u8LumaDiff"] = hiint_to_json(stExpAttr.stAuto.stSubflicker.u8LumaDiff);
    JstAuto["stSubflicker"] = picojson::value(JstSubflicker);
    JstAuto["stSysGainRange"] = ae_range_to_json(stExpAttr.stAuto.stSysGainRange);
    JstAuto["u16BlackSpeedBias"] = hiint_to_json(stExpAttr.stAuto.u16BlackSpeedBias);
    JstAuto["u16EVBias"] = hiint_to_json(stExpAttr.stAuto.u16EVBias);
    JstAuto["u16HistRatioSlope"] = hiint_to_json(stExpAttr.stAuto.u16HistRatioSlope);
    JstAuto["u32ExpValue"] = hiint_to_json(stExpAttr.stAuto.u32ExpValue);
    JstAuto["u32GainThreshold"] = hiint_to_json(stExpAttr.stAuto.u32GainThreshold);
    JstAuto["u8Compensation"] = hiint_to_json(stExpAttr.stAuto.u8Compensation);
    JstAuto["u8MaxHistOffset"] = hiint_to_json(stExpAttr.stAuto.u8MaxHistOffset);
    JstAuto["u8Speed"] = hiint_to_json(stExpAttr.stAuto.u8Speed);
    JstAuto["u8Tolerance"] = hiint_to_json(stExpAttr.stAuto.u8Tolerance);
    JstAuto["stISPDGainRange"] = ae_range_to_json(stExpAttr.stAuto.stISPDGainRange);

    JstManual["u32AGain"] = hiint_to_json(stExpAttr.stManual.u32AGain);
    JstManual["u32DGain"] = hiint_to_json(stExpAttr.stManual.u32DGain);
    JstManual["u32ExpTime"] = hiint_to_json(stExpAttr.stManual.u32ExpTime);
    JstManual["u32ISPDGain"] = hiint_to_json(stExpAttr.stManual.u32ISPDGain);
    JstManual["enAGainOpType"] = op_type_e_to_json(stExpAttr.stManual.enAGainOpType);
    JstManual["enDGainOpType"] = op_type_e_to_json(stExpAttr.stManual.enDGainOpType);
    JstManual["enExpTimeOpType"] = op_type_e_to_json(stExpAttr.stManual.enExpTimeOpType);
    JstManual["enISPDGainOpType"] = op_type_e_to_json(stExpAttr.stManual.enISPDGainOpType);

    Jroot["stAuto"] = picojson::value(JstAuto);
    Jroot["stManual"] = picojson::value(JstManual);
    return picojson::value(Jroot).serialize();
}

std::string AESetOpt(const std::string& root) {
    picojson::value Jroot;
    std::string err = picojson::parse(Jroot, root);
//    std::cout << root << std::endl;
    if (!err.empty()) {
        return ReturnError(__func__, err);
    }
    if (!Jroot.is<picojson::object>()) {
        return ReturnError(__func__, "JSON is not an object");
    }

    HI_S32 s32Ret = HI_SUCCESS;
    ISP_DEV IspDev = 0;

    ISP_EXPOSURE_ATTR_S stExpAttr;

    s32Ret = HI_MPI_ISP_GetExposureAttr(IspDev, &stExpAttr);
    if (HI_SUCCESS != s32Ret) {
        return ReturnError(__func__, "HI_MPI_ISP_GetExposureAttr", s32Ret);
    }
    
    stExpAttr.bByPass = static_cast<HI_BOOL> (Jroot.get("bByPass").get<bool>());
    stExpAttr.bAERouteExValid = static_cast<HI_BOOL> (Jroot.get("bAERouteExValid").get<bool>());
    stExpAttr.bHistStatAdjust = static_cast<HI_BOOL> (Jroot.get("bHistStatAdjust").get<bool>());
    stExpAttr.enOpType = op_type_e_to_type(Jroot.get("enOpType"));
    stExpAttr.u8AERunInterval = static_cast<HI_U8> (Jroot.get("u8AERunInterval").get<double>());
    if (stExpAttr.enOpType == OP_TYPE_AUTO) {
        ISP_AE_ATTR_S MstAuto;
        MstAuto.bManualExpValue = static_cast<HI_BOOL> (Jroot.get("stAuto").get("bManualExpValue").get<bool>());
        MstAuto.u16BlackSpeedBias = static_cast<HI_U16> (Jroot.get("stAuto").get("u16BlackSpeedBias").get<double>());
        MstAuto.u16EVBias = static_cast<HI_U16> (Jroot.get("stAuto").get("u16EVBias").get<double>());
        MstAuto.u16HistRatioSlope = static_cast<HI_U16> (Jroot.get("stAuto").get("u16HistRatioSlope").get<double>());
        MstAuto.u32ExpValue = static_cast<HI_U32> (Jroot.get("stAuto").get("u32ExpValue").get<double>());
        MstAuto.u32GainThreshold = static_cast<HI_U32> (Jroot.get("stAuto").get("u32GainThreshold").get<double>());
        MstAuto.u8Compensation = static_cast<HI_U8> (Jroot.get("stAuto").get("u8Compensation").get<double>());
        MstAuto.u8MaxHistOffset = static_cast<HI_U8> (Jroot.get("stAuto").get("u8MaxHistOffset").get<double>());
        MstAuto.u8Speed = static_cast<HI_U8> (Jroot.get("stAuto").get("u8Speed").get<double>());
        MstAuto.u8Tolerance = static_cast<HI_U8> (Jroot.get("stAuto").get("u8Tolerance").get<double>());

        picojson::array au8Weight = Jroot.get("stAuto").get("au8Weight").get<picojson::array>();
        for (int i = 0, k = 0; i < AE_ZONE_ROW; i++) {
            for (int j = 0; j < AE_ZONE_COLUMN; j++) {
                MstAuto.au8Weight[i][j] = static_cast<HI_U8> ((au8Weight[k++]).get<double>());
            }
        }
        MstAuto.enAEMode = ae_mode_e_to_type(Jroot.get("stAuto").get("enAEMode"));
        MstAuto.enAEStrategyMode = ae_strategy_e_to_type(Jroot.get("stAuto").get("enAEStrategyMode"));
        MstAuto.stAEDelayAttr.u16BlackDelayFrame =
                static_cast<HI_U16> (Jroot.get("stAuto").get("stAEDelayAttr").get("u16BlackDelayFrame").get<double>());
        MstAuto.stAEDelayAttr.u16WhiteDelayFrame =
                static_cast<HI_U16> (Jroot.get("stAuto").get("stAEDelayAttr").get("u16WhiteDelayFrame").get<double>());
        MstAuto.stAGainRange.u32Max =
                static_cast<HI_U32> (Jroot.get("stAuto").get("stAGainRange").get("u32Max").get<double>());
        MstAuto.stAGainRange.u32Min =
                static_cast<HI_U32> (Jroot.get("stAuto").get("stAGainRange").get("u32Min").get<double>());
        MstAuto.stAntiflicker.bEnable =
                static_cast<HI_BOOL> (Jroot.get("stAuto").get("stAntiflicker").get("bEnable").get<bool>());
        MstAuto.stAntiflicker.u8Frequency =
                static_cast<HI_U8> (Jroot.get("stAuto").get("stAntiflicker").get("u8Frequency").get<double>());
        MstAuto.stAntiflicker.enMode =
                antiflicker_mode_e_to_type(Jroot.get("stAuto").get("stAntiflicker").get("enMode"));

        MstAuto.stDGainRange.u32Max =
                static_cast<HI_U32> (Jroot.get("stAuto").get("stDGainRange").get("u32Max").get<double>());
        MstAuto.stDGainRange.u32Min =
                static_cast<HI_U32> (Jroot.get("stAuto").get("stDGainRange").get("u32Min").get<double>());


        MstAuto.stExpTimeRange.u32Max =
                static_cast<HI_U32> (Jroot.get("stAuto").get("stExpTimeRange").get("u32Max").get<double>());
        MstAuto.stExpTimeRange.u32Min =
                static_cast<HI_U32> (Jroot.get("stAuto").get("stExpTimeRange").get("u32Min").get<double>());

        MstAuto.stISPDGainRange.u32Max =
                static_cast<HI_U32> (Jroot.get("stAuto").get("stISPDGainRange").get("u32Max").get<double>());
        MstAuto.stISPDGainRange.u32Min =
                static_cast<HI_U32> (Jroot.get("stAuto").get("stISPDGainRange").get("u32Min").get<double>());

        MstAuto.stExpTimeRange.u32Max =
                static_cast<HI_U32> (Jroot.get("stAuto").get("stExpTimeRange").get("u32Max").get<double>());
        MstAuto.stExpTimeRange.u32Min =
                static_cast<HI_U32> (Jroot.get("stAuto").get("stExpTimeRange").get("u32Min").get<double>());

        MstAuto.stISPDGainRange.u32Max =
                static_cast<HI_U32> (Jroot.get("stAuto").get("stISPDGainRange").get("u32Max").get<double>());
        MstAuto.stISPDGainRange.u32Min =
                static_cast<HI_U32> (Jroot.get("stAuto").get("stISPDGainRange").get("u32Min").get<double>());

        MstAuto.stSubflicker.bEnable =
                static_cast<HI_BOOL> (Jroot.get("stAuto").get("stSubflicker").get("bEnable").get<bool>());

        MstAuto.stSysGainRange.u32Max =
                static_cast<HI_U32> (Jroot.get("stAuto").get("stSysGainRange").get("u32Max").get<double>());
        MstAuto.stSysGainRange.u32Min =
                static_cast<HI_U32> (Jroot.get("stAuto").get("stSysGainRange").get("u32Min").get<double>());
        stExpAttr.stAuto = MstAuto;
    } else if (stExpAttr.enOpType == OP_TYPE_MANUAL) {
        ISP_ME_ATTR_S MstManual;
        MstManual.enAGainOpType = op_type_e_to_type(Jroot.get("stManual").get("enAGainOpType"));
        MstManual.enDGainOpType = op_type_e_to_type(Jroot.get("stManual").get("enDGainOpType"));
        MstManual.enExpTimeOpType = op_type_e_to_type(Jroot.get("stManual").get("enExpTimeOpType"));
        MstManual.enISPDGainOpType = op_type_e_to_type(Jroot.get("stManual").get("enISPDGainOpType"));

        MstManual.u32AGain = static_cast<HI_U32> (Jroot.get("stManual").get("u32AGain").get<double>());
        MstManual.u32DGain = static_cast<HI_U32> (Jroot.get("stManual").get("u32DGain").get<double>());
        MstManual.u32ExpTime = static_cast<HI_U32> (Jroot.get("stManual").get("u32ExpTime").get<double>());
        MstManual.u32ISPDGain = static_cast<HI_U32> (Jroot.get("stManual").get("u32ISPDGain").get<double>());
        stExpAttr.stManual = MstManual;
    }

    s32Ret = HI_MPI_ISP_SetExposureAttr(IspDev, &stExpAttr);
    if (HI_SUCCESS != s32Ret) {
        return ReturnError(__func__, "HI_MPI_ISP_SetExposureAttr", s32Ret);
    }

    return ReturnSuccess(__func__, "HI_MPI_ISP_SetExposureAttr", s32Ret);
}

std::string AwbSetOpt(const std::string& root) {
    picojson::value Jroot;
    std::string err = picojson::parse(Jroot, root);
//    std::cout << root << std::endl;
    if (!err.empty()) {
        return ReturnError(__func__, err);
    }
    if (!Jroot.is<picojson::object>()) {
        return ReturnError(__func__, "JSON is not an object");
    }

    ISP_WB_ATTR_S stWBAttr;

    HI_S32 s32Ret = HI_SUCCESS;
    ISP_DEV IspDev = 0;

    s32Ret = HI_MPI_ISP_GetWBAttr(IspDev, &stWBAttr);
    if (HI_SUCCESS != s32Ret) {
        return ReturnError(__func__, "HI_MPI_ISP_GetWBAttr", s32Ret);
    }

    stWBAttr.bByPass = static_cast<HI_BOOL> (Jroot.get("bByPass").get<bool>());
    stWBAttr.enOpType = op_type_e_to_type(Jroot.get("enOpType"));
    if (stWBAttr.enOpType == OP_TYPE_AUTO) {
        stWBAttr.stAuto.bEnable = static_cast<HI_BOOL> (Jroot.get("stAuto").get("bEnable").get<bool>());
        picojson::array as32CurvePara_list = Jroot.get("stAuto").get("as32CurvePara").get<picojson::array>();
        int i = 0;
        for (picojson::array::iterator iter = as32CurvePara_list.begin(); iter != as32CurvePara_list.end(); ++iter) {
            stWBAttr.stAuto.as32CurvePara[i] = static_cast<HI_S32> ((*iter).get<double>());
            i++;
        }

        picojson::array au16StaticWB_list = Jroot.get("stAuto").get("au16StaticWB").get<picojson::array>();
        i = 0;
        for (picojson::array::iterator iter = au16StaticWB_list.begin(); iter != au16StaticWB_list.end(); ++iter) {
            stWBAttr.stAuto.au16StaticWB[i] = static_cast<HI_U16> ((*iter).get<double>());
            i++;
        }

        stWBAttr.stAuto.bGainNormEn = static_cast<HI_BOOL> (Jroot.get("stAuto").get("bGainNormEn").get<bool>());
        stWBAttr.stAuto.bShiftLimitEn = static_cast<HI_BOOL> (Jroot.get("stAuto").get("bShiftLimitEn").get<bool>());
        stWBAttr.stAuto.enAlgType = awb_alg_type_e_to_type(Jroot.get("stAuto").get("enAlgType"));
        stWBAttr.stAuto.stCTLimit.bEnable = static_cast<HI_BOOL> (Jroot.get("stAuto").get("stCTLimit").get("bEnable").get<bool>());
        stWBAttr.stAuto.stCTLimit.enOpType = op_type_e_to_type(Jroot.get("stAuto").get("stCTLimit").get("enOpType"));
        stWBAttr.stAuto.stCTLimit.u16HighBgLimit = static_cast<HI_U16> (Jroot.get("stAuto").get("stCTLimit").get("u16HighBgLimit").get<double>());
        stWBAttr.stAuto.stCTLimit.u16HighRgLimit = static_cast<HI_U16> (Jroot.get("stAuto").get("stCTLimit").get("u16HighRgLimit").get<double>());
        stWBAttr.stAuto.stCTLimit.u16LowBgLimit = static_cast<HI_U16> (Jroot.get("stAuto").get("stCTLimit").get("u16LowBgLimit").get<double>());
        stWBAttr.stAuto.stCTLimit.u16LowRgLimit = static_cast<HI_U16> (Jroot.get("stAuto").get("stCTLimit").get("u16LowRgLimit").get<double>());

        picojson::array au16CbMax_list = Jroot.get("stAuto").get("stCbCrTrack").get("au16CbMax").get<picojson::array>();
        i = 0;
        for (picojson::array::iterator iter = au16CbMax_list.begin(); iter != au16CbMax_list.end(); ++iter) {
            stWBAttr.stAuto.stCbCrTrack.au16CbMax[i] = static_cast<HI_U16> ((*iter).get<double>());
            i++;
        }

        picojson::array au16CbMin_list = Jroot.get("stAuto").get("stCbCrTrack").get("au16CbMin").get<picojson::array>();
        i = 0;
        for (picojson::array::iterator iter = au16CbMin_list.begin(); iter != au16CbMin_list.end(); ++iter) {
            stWBAttr.stAuto.stCbCrTrack.au16CbMin[i] = static_cast<HI_U16> ((*iter).get<double>());
            i++;
        }

        picojson::array au16CrMax_list = Jroot.get("stAuto").get("stCbCrTrack").get("au16CrMax").get<picojson::array>();
        i = 0;
        for (picojson::array::iterator iter = au16CrMax_list.begin(); iter != au16CrMax_list.end(); ++iter) {
            stWBAttr.stAuto.stCbCrTrack.au16CrMax[i] = static_cast<HI_U16> ((*iter).get<double>());
            i++;
        }

        picojson::array au16CrMin_list = Jroot.get("stAuto").get("stCbCrTrack").get("au16CrMin").get<picojson::array>();
        i = 0;
        for (picojson::array::iterator iter = au16CrMin_list.begin(); iter != au16CrMin_list.end(); ++iter) {
            stWBAttr.stAuto.stCbCrTrack.au16CrMin[i] = static_cast<HI_U16> ((*iter).get<double>());
            i++;
        }

        stWBAttr.stAuto.stCbCrTrack.bEnable = static_cast<HI_BOOL> (Jroot.get("stAuto").get("stCbCrTrack").get("bEnable").get<bool>());

        picojson::array au16HistWt_list = Jroot.get("stAuto").get("stLumaHist").get("au16HistWt").get<picojson::array>();
        i = 0;
        for (picojson::array::iterator iter = au16HistWt_list.begin(); iter != au16HistWt_list.end(); ++iter) {
            stWBAttr.stAuto.stLumaHist.au16HistWt[i] = static_cast<HI_U16> ((*iter).get<double>());
            i++;
        }

        picojson::array au8HistThresh_list = Jroot.get("stAuto").get("stLumaHist").get("au8HistThresh").get<picojson::array>();
        i = 0;
        for (picojson::array::iterator iter = au8HistThresh_list.begin(); iter != au8HistThresh_list.end(); ++iter) {
            stWBAttr.stAuto.stLumaHist.au8HistThresh[i] = static_cast<HI_U8> ((*iter).get<double>());
            i++;
        }

        stWBAttr.stAuto.stLumaHist.bEnable = static_cast<HI_BOOL> (Jroot.get("stAuto").get("stLumaHist").get("bEnable").get<bool>());
        stWBAttr.stAuto.stLumaHist.enOpType = op_type_e_to_type(Jroot.get("stAuto").get("stLumaHist").get("enOpType"));

        stWBAttr.stAuto.u16HighColorTemp = static_cast<HI_U16> (Jroot.get("stAuto").get("u16HighColorTemp").get<double>());
        stWBAttr.stAuto.u16LowColorTemp = static_cast<HI_U16> (Jroot.get("stAuto").get("u16LowColorTemp").get<double>());
        stWBAttr.stAuto.u16RefColorTemp = static_cast<HI_U16> (Jroot.get("stAuto").get("u16RefColorTemp").get<double>());
        stWBAttr.stAuto.u16Speed = static_cast<HI_U16> (Jroot.get("stAuto").get("u16Speed").get<double>());
        stWBAttr.stAuto.u16ZoneSel = static_cast<HI_U16> (Jroot.get("stAuto").get("u16ZoneSel").get<double>());
        stWBAttr.stAuto.u8BGStrength = static_cast<HI_U8> (Jroot.get("stAuto").get("u8BGStrength").get<double>());
        stWBAttr.stAuto.u8RGStrength = static_cast<HI_U8> (Jroot.get("stAuto").get("u8RGStrength").get<double>());
        stWBAttr.stAuto.u8ShiftLimit = static_cast<HI_U8> (Jroot.get("stAuto").get("u8ShiftLimit").get<double>());

    } else if (stWBAttr.enOpType == OP_TYPE_MANUAL) {
        stWBAttr.stManual.u16Bgain = static_cast<HI_U16> (Jroot.get("stManual").get("u16Bgain").get<double>());
        stWBAttr.stManual.u16Gbgain = static_cast<HI_U16> (Jroot.get("stManual").get("u16Gbgain").get<double>());
        stWBAttr.stManual.u16Grgain = static_cast<HI_U16> (Jroot.get("stManual").get("u16Grgain").get<double>());
        stWBAttr.stManual.u16Rgain = static_cast<HI_U16> (Jroot.get("stManual").get("u16Rgain").get<double>());

        fprintf(stderr, std::to_string(static_cast<HI_U16> (Jroot.get("stManual").get("u16Bgain").get<double>())).c_str());
        fprintf(stderr, std::to_string(static_cast<HI_U16> (Jroot.get("stManual").get("u16Gbgain").get<double>())).c_str());
        fprintf(stderr, std::to_string(static_cast<HI_U16> (Jroot.get("stManual").get("u16Grgain").get<double>())).c_str());
        fprintf(stderr, std::to_string(static_cast<HI_U16> (Jroot.get("stManual").get("u16Rgain").get<double>())).c_str());

    }
    s32Ret = HI_MPI_ISP_SetWBAttr(IspDev, &stWBAttr);
    if (HI_SUCCESS != s32Ret) {
        return ReturnError(__func__, "HI_MPI_ISP_SetWBAttr", s32Ret);
    }

    return ReturnSuccess(__func__, "HI_MPI_ISP_SetWBAttr", s32Ret);
}