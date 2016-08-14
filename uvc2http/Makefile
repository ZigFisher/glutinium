# 
# Copyright (C) 2015 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# This is free software, licensed under the GNU General Public License v2.
#
include $(TOPDIR)/rules.mk

PKG_NAME:=uvc2http
PKG_VERSION:=0.01
PKG_RELEASE:=1
PKG_MAINTAINER:=Oleg <legich55555@gmail.com>

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=https://github.com/Legich55555/uvc2http.git
PKG_SOURCE_VERSION:=518a089136156bda72d77c5469a628fe157daa1f
PKG_SOURCE:=$(PKG_NAME)-$(PKG_SOURCE_VERSION).tar.gz
PKG_SOURCE_SUBDIR:=$(PKG_NAME)-$(PKG_SOURCE_VERSION)
PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_SOURCE_VERSION)

PKG_LICENSE:=GPL-2.0
PKG_LICENSE_FILES:=LICENSE

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/cmake.mk

define Package/uvc2http
	SECTION:=multimedia
	CATEGORY:=Multimedia
	TITLE:=UVC to HTTP streamer
	DEPENDS:=+libv4l
endef

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR)
	$(STRIP) $(PKG_BUILD_DIR)/$(PKG_NAME)
endef

define Package/uvc2http/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/uvc2http $(1)/usr/bin/
	$(INSTALL_DIR) $(1)/etc/config
	$(CP) ./files/uvc2http.config $(1)/etc/config/uvc2http
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/uvc2http.init $(1)/etc/init.d/uvc2http
endef

$(eval $(call BuildPackage,uvc2http))

