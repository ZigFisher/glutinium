# 
# Copyright (C) 2015 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

PKG_NAME:=flypeek
PKG_VERSION:=0.01
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
include $(INCLUDE_DIR)/cmake.mk

define Package/flypeek
	SECTION:=multimedia
	CATEGORY:=Multimedia
	TITLE:=FlyPeek - application for Linux-UVC compatible webcams
	DEPENDS:=+libjpeg
endef

define Package/flypeek/install
	$(INSTALL_DIR) $(1)/etc/config
	$(CP) ./files/mjpg-streamer.config $(1)/etc/config/mjpg-streamer
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/mjpg-streamer.init $(1)/etc/init.d/
	$(INSTALL_DIR) $(1)/etc/hotplug.d/usb
	$(INSTALL_DATA) ./files/mjpg-streamer.hotplug $(1)/etc/hotplug.d/usb/20-mjpg-streamer
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/flypeek $(1)/usr/bin/
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_BUILD_DIR)/input_file.so $(1)/usr/lib
	$(CP) $(PKG_BUILD_DIR)/input_uvc.so $(1)/usr/lib
	$(CP) $(PKG_BUILD_DIR)/output_file.so $(1)/usr/lib
	$(CP) $(PKG_BUILD_DIR)/output_http.so $(1)/usr/lib
endef

$(eval $(call BuildPackage,flypeek))
