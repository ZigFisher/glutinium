#
# Copyright (C) 2006-2018 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=megatec
PKG_VERSION:=2018-10-01
PKG_RELEASE:=1
PKG_MAINTAINER:=Igor Zalatov <flyrouter@gmail.com>
PKG_LICENSE:=GPL-2.0
PKG_LICENSE_FILES:=LICENSE

include $(INCLUDE_DIR)/package.mk

define Package/megatec
	SECTION:=utils
	CATEGORY:=Utilities
	TITLE:=Simple utility for Megatec Protocol UPS
	MAINTAINER:=https://habr.com/post/397637/
endef

define Package/megatec/description
	Simple utility for Megatec Protocol UPS
	Idea and basic c++ source file get from https://habr.com/post/397637/ (Russian)
endef

define Build/Prepare
	$(INSTALL_DIR) $(PKG_BUILD_DIR)
	$(INSTALL_DATA) ./src/* $(PKG_BUILD_DIR)/
endef

define Build/Compile
	$(TARGET_CC) $(TARGET_CFLAGS) $(TARGET_LDFLAGS) -std=c99 -O -g -D LINUX \
		-o $(PKG_BUILD_DIR)/megatec $(PKG_BUILD_DIR)/megatec_v3.c
endef

define Package/megatec/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/megatec $(1)/usr/bin/
endef

$(eval $(call BuildPackage,megatec))
