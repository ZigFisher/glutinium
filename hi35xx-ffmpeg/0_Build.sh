#!/bin/sh
#
# https://gist.github.com/widgetii/43188b185f2c2180b994e89c74741fe2

wget http://ffmpeg.org/releases/ffmpeg-4.2.tar.gz
tar xvfz ffmpeg-4.2.tar.gz
cd ffmpeg-4.2

config_full() {
  ARCH=arm
  CROSS=arm-openwrt-linux-
  STAGING_DIR=/mnt/raid/OpenWrt/OpenIPC/staging_dir
  PATH=$PATH:/mnt/raid/OpenWrt/OpenIPC/staging_dir/toolchain-arm_arm926ej-s_gcc-4.8-linaro_uClibc-0.9.33.2_eabi/bin
  # Size ~ 2270072
  ./configure \
    --enable-cross-compile --cross-prefix=${CROSS} --arch=arm --target-os=linux \
    --disable-iconv --disable-debug --enable-small \
    --disable-armv6 --disable-armv6t2 --disable-neon --disable-vfp \
    --disable-ffprobe \
    --disable-decoders --enable-decoder=aac,h264,pcm_u8 \
    --disable-encoders --enable-encoder=aac \
    --disable-parsers --enable-parser=aac,h264 \
    --disable-demuxers --enable-demuxer=aac,h264 --enable-demuxer=rtp,rtsp,sdp \
    --disable-muxers --enable-muxer=flv \
    --disable-protocols --enable-protocol=http,rtp,rtsp,rtmp,file \
    --disable-filters --enable-filter=anullsrc,aresample \
    --disable-bsfs --enable-bsf=aac_adtstoasc \
    --disable-indevs --enable-indev=lavfi \
    --disable-outdevs
}

config_mini() {
  ARCH=arm
  CROSS=arm-openwrt-linux-
  STAGING_DIR=/mnt/raid/OpenWrt/OpenIPC/staging_dir
  PATH=$PATH:/mnt/raid/OpenWrt/OpenIPC/staging_dir/toolchain-arm_arm926ej-s_gcc-4.8-linaro_uClibc-0.9.33.2_eabi/bin
  # Size ~ 2143096
  ./configure \
    --enable-cross-compile --cross-prefix=${CROSS} --arch=arm --target-os=linux \
    --disable-iconv --disable-debug --enable-small \
    --disable-armv6 --disable-armv6t2 --disable-neon --disable-vfp \
    --disable-ffprobe \
    --disable-swresample \
    --disable-avdevice \
    --disable-decoders \
    --disable-encoders --enable-decoder=aac,h264 \
    --disable-parsers --enable-parser=aac,h264 \
    --disable-demuxers --enable-demuxer=rtp,rtsp,sdp \
    --disable-muxers --enable-demuxer=aac,h264 --enable-muxer=flv \
    --disable-protocols --enable-protocol=http,rtp,rtsp,rtmp,file \
    --disable-filters \
    --disable-bsfs --enable-bsf=aac_adtstoasc \
    --disable-indevs \
    --disable-outdevs
}

config_mimi_dbell() {
  ARCH=arm
  CROSS=arm-openwrt-linux-
  STAGING_DIR=/mnt/raid/OpenWrt/OpenIPC/staging_dir
  PATH=$PATH:/mnt/raid/OpenWrt/OpenIPC/staging_dir/toolchain-arm_arm926ej-s_gcc-4.8-linaro_uClibc-0.9.33.2_eabi/bin
  # Size ~ ?
  ./configure \
    --enable-cross-compile --cross-prefix=${CROSS} --arch=arm --target-os=linux \
    --disable-iconv --disable-debug --enable-small \
    --disable-armv6 --disable-armv6t2 --disable-neon --disable-vfp \
\
    --enable-shared --enable-static --enable-pthreads --enable-zlib \
    --disable-runtime-cpudetect --disable-doc --enable-gpl --enable-version3 \
    --disable-altivec --disable-amd3dnow --disable-amd3dnowext --disable-mmx --disable-mmxext \
    --disable-sse --disable-sse2 --disable-sse3 --disable-ssse3 --disable-sse4 --disable-sse42 \
    --disable-avx --disable-xop --disable-fma3 --disable-fma4 --disable-avx2 \
    --disable-inline-asm --disable-yasm --disable-mips32r2 --disable-mipsdspr1 \
    --disable-mipsdspr2 --disable-mipsfpu --disable-dxva2 --disable-lzma --disable-vaapi \
    --disable-vda --disable-vdpau --disable-outdevs
}

config_cortex() {
  ARCH=arm
  CROSS=arm-openwrt-linux-
  STAGING_DIR=/mnt/raid/OpenWrt/OpenIPC/staging_dir
  PATH=$PATH:/mnt/raid/OpenWrt/OpenIPC/staging_dir/toolchain-arm_cortex-a9_gcc-4.8-linaro_uClibc-0.9.33.2_eabi/bin
  # Size ~ 2143096
  ./configure \
    --enable-cross-compile --cross-prefix=${CROSS} --arch=arm --target-os=linux \
    --disable-iconv --disable-debug --enable-small \
    --disable-ffprobe \
    --disable-swresample \
    --disable-avdevice \
    --disable-decoders \
    --disable-encoders --enable-decoder=aac,h264 \
    --disable-parsers --enable-parser=aac,h264 \
    --disable-demuxers --enable-demuxer=rtp,rtsp,sdp \
    --disable-muxers --enable-demuxer=aac,h264 --enable-muxer=flv \
    --disable-protocols --enable-protocol=http,rtp,rtsp,rtmp,file \
    --disable-filters \
    --disable-bsfs --enable-bsf=aac_adtstoasc \
    --disable-indevs \
    --disable-outdevs
#   --disable-armv6 --disable-armv6t2 --disable-neon --disable-vfp \
}

config_mipsel() {
  ARCH=mipsel
  CROSS=mipsel-openwrt-linux-
  STAGING_DIR=/mnt/raid/OpenWrt/chaos_calmer/staging_dir
  PATH=$PATH:/mnt/raid/OpenWrt/chaos_calmer/staging_dir/toolchain-mipsel_24kec+dsp_gcc-4.8-linaro_uClibc-0.9.33.2/bin
  # Size ~ 2143096
  ./configure \
    --enable-cross-compile --cross-prefix=${CROSS} --arch=mipsel --target-os=linux \
    --disable-iconv --disable-debug --enable-small \
    --disable-ffprobe \
    --disable-swresample \
    --disable-avdevice \
    --disable-decoders \
    --disable-encoders --enable-decoder=aac,h264 \
    --disable-parsers --enable-parser=aac,h264 \
    --disable-demuxers --enable-demuxer=rtp,rtsp,sdp \
    --disable-muxers --enable-demuxer=aac,h264 --enable-muxer=flv \
    --disable-protocols --enable-protocol=http,rtp,rtsp,rtmp,file \
    --disable-filters \
    --disable-bsfs --enable-bsf=aac_adtstoasc \
    --disable-indevs \
    --disable-outdevs
#   --disable-armv6 --disable-armv6t2 --disable-neon --disable-vfp \
}

#config_full
#config_mini
config_mimi_dbell
#config_cortex
#config_mipsel
make -j$(($(nproc)+1))
  