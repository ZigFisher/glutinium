cmake_minimum_required(VERSION 2.6)
project(zfttools)

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 11)

# get_cmake_property(_variableNames VARIABLES)
# list (SORT _variableNames)
# foreach (_variableName ${_variableNames})
#     message(STATUS "${_variableName}=${${_variableName}}")
# endforeach()

include_directories(
    "/Users/user/dev/rustcam/openwrt/tool/camfeed copy/hidemo/src/include/"
    ./
    ${CMAKE_PREFIX_PATH}/usr/include/hisi-osdrv2/
    ${CMAKE_PREFIX_PATH}/usr/include/hidrv/
)

link_directories(
    ${CMAKE_PREFIX_PATH}/usr/lib/hisi-osdrv2/
    ${CMAKE_PREFIX_PATH}/usr/lib/hidrv/
)

# set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
# set(CMAKE_C_FLAGS "-Os -ffunction-sections -Wl,--gc-sections -fno-asynchronous-unwind-tables -Wl,--strip-all")

set(HILIBS
    isp         # ISP_AlgRegisterDehaze
    # sns_imx222  # sensor_register_callback / sensor_unregister_callback
    _hiae       # HI_MPI_AE_Register/HI_MPI_AE_UnRegister, HI_MPI_AE_SensorRegCallBack/HI_MPI_AE_SensorUnRegCallBack
    _hiawb      # HI_MPI_AWB_Register/HI_MPI_AWB_UnRegister, HI_MPI_AWB_SensorRegCallBack/HI_MPI_AWB_SensorUnRegCallBack
    _hiaf       # HI_MPI_AF_Register/HI_MPI_AF_UnRegister
    _hidefog    # ISP_AlgRegisterDehaze
    pthread m dl
    ive         # HI_MPI_IVE_DMA, HI_MPI_IVE_Query, ISP_AlgRegisterDehaze
    md          # ISP_AlgRegisterDehaze
    mpi
    tde         # ISP_AlgRegisterDehaze
    upvqe       # HI_UPVQE_Create/HI_UPVQE_Destroy, HI_UPVQE_ReadFrame/HI_UPVQE_WriteFrame, HI_UPVQE_GetVolume/HI_UPVQE_SetVolume, HI_UPVQE_GetConfig
    dnvqe       # ISP_AlgRegisterDehaze, HI_DNVQE_Create/HI_DNVQE_Destroy, HI_DNVQE_ReadFrame/HI_DNVQE_WriteFrame, HI_DNVQE_GetConfig
    VoiceEngine # ISP_AlgRegisterDehaze, HI_VOICE_EncodeFrame/HI_VOICE_DecodeFrame, HI_VOICE_EncReset/HI_VOICE_DecReset
    # slice_trans
)

set(HISRC
    loadbmp.h
    loadbmp.c
    sample_comm.h
    sample_comm_audio.c
    sample_comm_isp.c
    sample_comm_ive.c
    sample_comm_ive.h
    sample_comm_sys.c
    sample_comm_venc.c
    sample_comm_vi.c
    sample_comm_vo.c
    sample_comm_vpss.c
    sample_venc.c
    sample_init.c
    stack.c
#    sensors.hpp
#    sensors.cpp
)
add_executable(rtsp-hisi ${HISRC}
    main.c
    ringfifo.c
    rtputils.c
    rtspservice.c
    rtsputils.c
)
set_target_properties(rtsp-hisi PROPERTIES COMPILE_DEFINITIONS RTSP_STREAM=1)
target_link_libraries(rtsp-hisi ${HILIBS})

add_executable(hijpg ${HISRC}
    jpg.c
)
target_link_libraries(hijpg ${HILIBS})

install(TARGETS rtsp-hisi RUNTIME DESTINATION /usr/bin/)
install(TARGETS hijpg RUNTIME DESTINATION /usr/bin/)
