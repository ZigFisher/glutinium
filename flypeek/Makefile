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
PKG_SOURCE_URL:=https://github.com/PRINTR3D/mjpg-streamer-fixed.git
PKG_SOURCE_VERSION:=bb4d51d0067f5ef78d2ea55a7e1723ec1502e127
PKG_SOURCE_SUBDIR:=$(PKG_NAME)-$(PKG_VERSION)
PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_SOURCE_VERSION)
PKG_SOURCE:=$(PKG_NAME)-$(PKG_SOURCE_VERSION).tar.gz

PKG_LICENSE:=GPL-2.0
PKG_LICENSE_FILES:=LICENSE

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/cmake.mk

define Package/flypeek
	SECTION:=multimedia
	CATEGORY:=Multimedia
	TITLE:=FlyPeek - package for video monitoring and creating snapshots
	#DEPENDS:=+libv4l
endef

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR)
	$(STRIP) $(PKG_BUILD_DIR)/$(PKG_NAME)
endef

define Package/flypeek/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/flypeek $(1)/usr/bin/
	#$(INSTALL_DIR) $(1)/etc/config
	#$(CP) ./files/flypeek.config $(1)/etc/config/flypeek
	#$(INSTALL_DIR) $(1)/etc/init.d
	#$(INSTALL_BIN) ./files/flypeek.init $(1)/etc/init.d/
endef

$(eval $(call BuildPackage,flypeek))

