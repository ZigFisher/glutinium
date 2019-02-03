#set(CMAKE_SYSTEM_NAME Linux)
#set(CMAKE_SYSTEM_PROCESSOR arm)

# set(CMAKE_SYSROOT /home/ussh/work/ARM/CAMERA/hi3518/WRTED/camopenwrt_docker/hi35xx-br/output/host/arm-buildroot-linux-uclibcgnueabi/sysroot)
#set(CMAKE_STAGING_PREFIX /home/devel/stage)

#set(tools /home/ussh/work/ARM/CAMERA/hi3518/WRTED/camopenwrt_docker/hi35xx-br/output/host)
#set(CMAKE_C_COMPILER ${tools}/bin/arm-buildroot-linux-uclibcgnueabi-gcc)
#set(CMAKE_CXX_COMPILER ${tools}/bin/arm-buildroot-linux-uclibcgnueabi-g++)

cmake_minimum_required(VERSION 2.8.11)
project(hisi-rpc-config)

# Version number
set(HISI_RPC_CONFIG_VERSION_MAJOR 0)
set(HISI_RPC_CONFIG_VERSION_MINOR 1)
set(HISI_RPC_CONFIG_VERSION "${HISI_RPC_CONFIG_VERSION_MAJOR}.${HISI_RPC_CONFIG_VERSION_MINOR}")

include(CheckCXXCompilerFlag)
include(CheckFunctionExists)
include(FindPackageMessage)

# libmicrohttpd
find_path(MICROHTTPD_INCLUDE_DIR microhttpd.h)
find_library(MICROHTTPD_LIBRARY microhttpd)

if (NOT MICROHTTPD_INCLUDE_DIR OR NOT MICROHTTPD_LIBRARY)
  message(SEND_ERROR "Required library microhttpd not found")
endif()

# himpp
set(HIMPP_INCLUDE_DIR "${CMAKE_SYSROOT}/usr/include/hi3518v200mpp")
find_library(HIMPP_LIBRARY mpi)

if (NOT HIMPP_INCLUDE_DIR)
  message(SEND_ERROR "Required includes himpp not found")
endif()

if (NOT HIMPP_LIBRARY)
  message(SEND_ERROR "Required library himpp not found")
endif()

# Compiler flags
check_cxx_compiler_flag(-Wall COMPILER_SUPPORTS_WALL)
if (COMPILER_SUPPORTS_WALL)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
endif()
check_cxx_compiler_flag(-Wextra COMPILER_SUPPORTS_WEXTRA)
if (COMPILER_SUPPORTS_WEXTRA)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wextra")
endif()

check_cxx_compiler_flag(-std=c++14 COMPILER_SUPPORTS_CXX14)
if (NOT COMPILER_SUPPORTS_CXX14)
  check_cxx_compiler_flag(-std=c++11 COMPILER_SUPPORTS_CXX11)
endif()
if (COMPILER_SUPPORTS_CXX14)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
elseif (COMPILER_SUPPORTS_CXX11)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
else()
  message(SEND_ERROR "Compiler does not support C++11")
endif()

check_function_exists(strptime HAVE_STRPTIME)
if (HAVE_STRPTIME)
  add_definitions(-DHAVE_STRPTIME)
endif()

add_subdirectory(xsonrpc)

set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/bin)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})

# The following folder will be included
include_directories("${PROJECT_SOURCE_DIR}")
include_directories("${CMAKE_SOURCE_DIR}/xsonrpc/include")
include_directories("${CMAKE_SOURCE_DIR}/xsonrpc/src")
link_directories(xsonrpc)
include_directories("${HIMPP_INCLUDE_DIR}")

set(SOURCE_FILES
  main.cpp
  camera.cpp
  server.cpp
)

add_executable (${PROJECT_NAME} ${SOURCE_FILES})
target_link_libraries(${PROJECT_NAME} xsonrpc)
target_link_libraries(${PROJECT_NAME} isp)
target_link_libraries(${PROJECT_NAME} mpi)

target_link_libraries(${PROJECT_NAME} _hiawb)
target_link_libraries(${PROJECT_NAME} _hiae)
target_link_libraries(${PROJECT_NAME} _hidefog)
target_link_libraries(${PROJECT_NAME} VoiceEngine)

target_link_libraries(${PROJECT_NAME} upvqe)
target_link_libraries(${PROJECT_NAME} dnvqe)

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION "bin")


