#
# Copyright (C) 2006-2015 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=mercury206
PKG_VERSION:=2018-12-10
PKG_RELEASE:=1
PKG_MAINTAINER:=ZigFisher

PKG_LICENSE:=GPL-2.0
PKG_LICENSE_FILES:=LICENSE

include $(INCLUDE_DIR)/package.mk

define Package/$(PKG_NAME)
	SECTION:=utils
	CATEGORY:=Utilities
	TITLE:=Mercury 206 power meter RS485 communication utility
	MAINTAINER:=https://github.com/ZigFisher/Glutinium/
endef

define Package/$(PKG_NAME)/description
	Mercury 206 power meter RS485 communication utility
endef

define Build/Prepare
	$(INSTALL_DIR) $(PKG_BUILD_DIR)
	$(INSTALL_DATA) ./src/* $(PKG_BUILD_DIR)/
endef

define Build/Compile
	$(TARGET_CC) $(TARGET_CFLAGS) $(TARGET_LDFLAGS) -std=c99 -O -g -D LINUX \
		-o $(PKG_BUILD_DIR)/$(PKG_NAME) $(PKG_BUILD_DIR)/mercury206.c
endef

define Package/$(PKG_NAME)/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/$(PKG_NAME) $(1)/usr/bin/
endef

$(eval $(call BuildPackage,$(PKG_NAME)))
