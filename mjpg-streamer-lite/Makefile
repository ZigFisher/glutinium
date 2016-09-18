# 
# Copyright (C) 2015 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

PKG_NAME:=mjpg-streamer-lite
PKG_VERSION:=2.0.1a
PKG_RELEASE:=1
PKG_MAINTAINER:=Igor Zalatov <flyrouter@gmail.com>

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=https://github.com/ZigFisher/mjpg-streamer.git
PKG_SOURCE_VERSION:=fb23fa28071e4c20adf7cd06e6207eecb0d120c3
PKG_SOURCE_SUBDIR:=$(PKG_NAME)-$(PKG_VERSION)
PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz

PKG_LICENSE:=GPL-2.0
PKG_LICENSE_FILES:=LICENSE


include $(INCLUDE_DIR)/package.mk

define Package/mjpg-streamer-lite
	SECTION:=multimedia
	CATEGORY:=Multimedia
	TITLE:=ExtraLight application for Linux-UVC compatible webcams
	DEPENDS:=+libpthread
endef

define Package/mjpg-streamer-lite/install
	$(INSTALL_DIR) $(1)/etc/config
	$(CP) ./files/mjpg-streamer.config $(1)/etc/config/mjpg-streamer
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/mjpg-streamer.init $(1)/etc/init.d/mjpg-streamer
	$(INSTALL_DIR) $(1)/etc/hotplug.d/usb
	$(INSTALL_DATA) ./files/mjpg-streamer.hotplug $(1)/etc/hotplug.d/usb/20-mjpg-streamer
	$(INSTALL_DIR) $(1)/usr/bin
	#$(INSTALL_BIN) $(PKG_BUILD_DIR)/flypeek $(1)/usr/bin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/mjpg_streamer $(1)/usr/bin/
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_BUILD_DIR)/plugins/input_file/input_file.so $(1)/usr/lib
	$(CP) $(PKG_BUILD_DIR)/plugins/input_uvc/input_uvc.so $(1)/usr/lib
	$(CP) $(PKG_BUILD_DIR)/plugins/output_file/output_file.so $(1)/usr/lib
	$(CP) $(PKG_BUILD_DIR)/plugins/output_http/output_http.so $(1)/usr/lib
	$(CP) $(PKG_BUILD_DIR)/plugins/output_rtsp/output_rtsp.so $(1)/usr/lib
	$(INSTALL_DIR) $(1)/www/webcam
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/www/stream_simple.html $(1)/www/webcam/index.html
	#$(INSTALL_DATA) $(PKG_BUILD_DIR)/www/* $(1)/www/webcam/
	$(CP) ./files/cool_girl.jpg $(1)/www/webcam/
endef

$(eval $(call BuildPackage,mjpg-streamer-lite))
