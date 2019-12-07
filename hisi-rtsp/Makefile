#
# Copyright (C) 2006-2015 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=hisi-rtsp
PKG_VERSION:=2018-05-01
PKG_RELEASE:=1
PKG_MAINTAINER:=Igor Zalatov <flyrouter@gmail.com>

PKG_LICENSE:=GPL-2.0
PKG_LICENSE_FILES:=LICENSE

PKG_BUILD_DEPENDS:=hisi-osdrv2

include $(INCLUDE_DIR)/package.mk

define Package/$(PKG_NAME)
	SECTION:=openipc
	CATEGORY:=OpenIPC
	SUBMENU:=Unused
	TITLE:=Simple RTSP daemon for Hisilicon devices
	MAINTAINER:=https://github.com/xibeilang524/hi3518_h264/tree/master/rtspok_utf8
	DEPENDS:=@(TARGET_hi35xx) +libpthread +hisi-osdrv2-base +hisi-osdrv2-kmod +hisi-osdrv2-lib
endef

define Package/$(PKG_NAME)/description
	Simple RTSP daemon for Hisilicon devices
endef

define Build/Prepare
	$(INSTALL_DIR) $(PKG_BUILD_DIR)
	cp -ar ./src/* $(PKG_BUILD_DIR)/
	#$(INSTALL_DATA) ./src/* $(PKG_BUILD_DIR)/
endef

define Build/Compile
	$(TARGET_CC) $(TARGET_CFLAGS) $(TARGET_LDFLAGS) -O -g -D LINUX -lpthread -lm -ldl \
		-DSENSOR_TYPE=APTINA_AR0130_DC_720P_30FPS \
		-Dhi3518e -DHICHIP=0x3518E200 -DCHIP_ID=CHIP_HI3518E_V200 -DISP_V2 -Dhi3518ev200 \
		-DHI_ACODEC_TYPE_INNER -DHI_RELEASE -DHI_XXXX \
		-o $(PKG_BUILD_DIR)/hisi-rtsp $(PKG_BUILD_DIR)/main.c $(PKG_BUILD_DIR)/ringfifo.c $(PKG_BUILD_DIR)/rtputils.c $(PKG_BUILD_DIR)/rtspservice.c $(PKG_BUILD_DIR)/rtsputils.c \
		$(PKG_BUILD_DIR)/loadbmp.c $(PKG_BUILD_DIR)/sample_comm_audio.c $(PKG_BUILD_DIR)/sample_comm_isp.c $(PKG_BUILD_DIR)/sample_comm_sys.c $(PKG_BUILD_DIR)/sample_comm_ive.c \
		$(PKG_BUILD_DIR)/sample_comm_venc.c $(PKG_BUILD_DIR)/sample_comm_vi.c $(PKG_BUILD_DIR)/sample_comm_vo.c $(PKG_BUILD_DIR)/sample_comm_vpss.c $(PKG_BUILD_DIR)/sample_venc.c \
		-I$(PKG_BUILD_DIR)/include/ $(PKG_BUILD_DIR)/lib/libsns_ar0130.a \
		$(PKG_BUILD_DIR)/lib/libmpi.a $(PKG_BUILD_DIR)/lib/libive.a $(PKG_BUILD_DIR)/lib/libmd.a $(PKG_BUILD_DIR)/lib/libVoiceEngine.a $(PKG_BUILD_DIR)/lib/libupvqe.a  \
		$(PKG_BUILD_DIR)/lib/libisp.a  $(PKG_BUILD_DIR)/lib/lib_cmoscfg.a $(PKG_BUILD_DIR)/lib/lib_iniparser.a $(PKG_BUILD_DIR)/lib/lib_hiae.a \
		$(PKG_BUILD_DIR)/lib/lib_hiawb.a $(PKG_BUILD_DIR)/lib/lib_hiaf.a $(PKG_BUILD_DIR)/lib/lib_hidefog.a $(PKG_BUILD_DIR)/lib/libdnvqe.a
endef

#SENSOR_TYPE=APTINA_AR0130_DC_720P_30FPS
#SENSOR_TYPE=OMNIVISION_OV9712_DC_720P_30FPS


define Package/$(PKG_NAME)/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/$(PKG_NAME) $(1)/usr/sbin/
endef

$(eval $(call BuildPackage,$(PKG_NAME)))
