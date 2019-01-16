set(CMAKE_SYSTEM_NAME Linux)
# set(CMAKE_SYSTEM_PROCESSOR arm)

# set(CMAKE_SYSROOT /home/ussh/work/ARM/CAMERA/hi3518/WRTED/camopenwrt_docker/hi35xx-br/output/host/arm-buildroot-linux-uclibcgnueabi/sysroot)
# set(CMAKE_SYSROOT /home/ussh/work/ARM/CAMERA/hi3518/WRTED/camopenwrt_docker/UCLIBC/crosstool-NG/arm-unknown-linux-uclibcgnueabi/arm-unknown-linux-uclibcgnueabi/sysroot)
#set(CMAKE_STAGING_PREFIX /home/devel/stage)

# set(tools /home/ussh/work/ARM/CAMERA/hi3518/WRTED/camopenwrt_docker/hi35xx-br/output/host)
# set(tools /home/ussh/work/ARM/CAMERA/hi3518/WRTED/camopenwrt_docker/UCLIBC/crosstool-NG/arm-unknown-linux-uclibcgnueabi)
# set(CMAKE_C_COMPILER ${tools}/bin/arm-buildroot-linux-uclibcgnueabi-gcc)
# set(CMAKE_CXX_COMPILER ${tools}/bin/arm-buildroot-linux-uclibcgnueabi-g++)
# set(CMAKE_C_COMPILER ${tools}/bin/arm-unknown-linux-uclibcgnueabi-gcc)
# set(CMAKE_CXX_COMPILER ${tools}/bin/arm-unknown-linux-uclibcgnueabi-g++)

cmake_minimum_required(VERSION 2.8.11)
project(xsonrpc)

# Version number
set(XSONRPC_VERSION_MAJOR 0)
set(XSONRPC_VERSION_MINOR 1)
set(XSONRPC_VERSION "${XSONRPC_VERSION_MAJOR}.${XSONRPC_VERSION_MINOR}")

# Options
option(BUILD_EXAMPLES "Build xsonrpc examples" OFF)
option(BUILD_TESTS "Build xsonrpc unit tests" OFF)
option(BUILD_CLIENT "Build client support" OFF)

include(CheckCXXCompilerFlag)
include(CheckFunctionExists)
include(FindPackageMessage)

# libmicrohttpd
find_path(MICROHTTPD_INCLUDE_DIR microhttpd.h)
# set(MICROHTTPD_INCLUDE_DIR "/home/ussh/work/ARM/CAMERA/hi3518/WRTED/camopenwrt_docker/hi35xx-br/package/hisi-rpc-config/libmicrohttpd/build_uclibc-0.9/include")
# mark_as_advanced(MICROHTTPD_INCLUDE_DIR)
find_library(MICROHTTPD_LIBRARY microhttpd)
# set(MICROHTTPD_LIBRARY "/home/ussh/work/ARM/CAMERA/hi3518/WRTED/camopenwrt_docker/hi35xx-br/package/hisi-rpc-config/libmicrohttpd/build_uclibc-0.9/lib")
# mark_as_advanced(MICROHTTPD_LIBRARY)

if (NOT MICROHTTPD_INCLUDE_DIR OR NOT MICROHTTPD_LIBRARY)
  message(SEND_ERROR "Required library microhttpd not found")
endif()

if(BUILD_CLIENT)
  # cURL
  #find_package(CURL REQUIRED)
  set(CURL_INCLUDE_DIR "/home/ussh/work/ARM/CAMERA/hi3518/WRTED/camopenwrt_docker/hi35xx-br/package/hisi-rpc-config/curl/build_uclibc-0.9/include")
  mark_as_advanced(CURL_INCLUDE_DIR)
  #find_library(MICROHTTPD_LIBRARY microhttpd)
  set(CURL_LIBRARY "/home/ussh/work/ARM/CAMERA/hi3518/WRTED/camopenwrt_docker/hi35xx-br/package/hisi-rpc-config/curl/build_uclibc-0.9/lib")
  mark_as_advanced(CURL_LIBRARY)
endif(BUILD_CLIENT)


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

# integer_sequence for c++11 compiler
if (NOT COMPILER_SUPPORTS_CXX14)
  include_directories("3pp/integer_seq")
endif()
install(FILES "3pp/integer_seq/integer_seq.h" DESTINATION "include/xsonrpc")

# Public headers
set(XSONRPC_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")
install(DIRECTORY "include/xsonrpc" DESTINATION "include"
  FILES_MATCHING PATTERN "*.h")

# Main library
add_subdirectory("src")

# Unit tests
if (BUILD_TESTS)
  enable_testing()
  add_subdirectory("test")
endif()

# Examples
if (BUILD_EXAMPLES)
  add_subdirectory("examples")
else()
  add_subdirectory("examples" EXCLUDE_FROM_ALL)
endif()
