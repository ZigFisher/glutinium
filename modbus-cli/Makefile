#
# Copyright (C) 2006-2015 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=modbus-cli
PKG_VERSION:=0.3.1
PKG_RELEASE:=1
PKG_MAINTAINER:=ZigFisher

PKG_LICENSE:=GPL-2.0
PKG_LICENSE_FILES:=LICENSE

include $(INCLUDE_DIR)/package.mk

TARGET_CFLAGS=-I$(STAGING_DIR)/usr/include/modbus
TARGET_LDFLAGS=-L$(STAGING_DIR)/usr/include/modbus
PKG_BUILD_DEPENDS:=libmodbus

define Package/$(PKG_NAME)
	SECTION:=utils
	CATEGORY:=Utilities
	SUBMENU:=ZFT Lab.
	TITLE:=A simple application to send Modbus commands from the CLI
	MAINTAINER:=https://dev.maestro-wireless.eu/kb/modbus-cli-package/
	DEPENDS:=+libmodbus
endef

define Package/$(PKG_NAME)/description
	A simple application to send Modbus commands from the CLI
endef

define Build/Prepare
	$(INSTALL_DIR) $(PKG_BUILD_DIR)
	$(INSTALL_DATA) ./src/* $(PKG_BUILD_DIR)/
endef

define Build/Compile
	$(TARGET_CC) $(TARGET_CFLAGS) $(TARGET_LDFLAGS) -O -g -D LINUX \
		-o $(PKG_BUILD_DIR)/$(PKG_NAME) $(PKG_BUILD_DIR)/modbus-cli.c
endef

define Package/$(PKG_NAME)/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/$(PKG_NAME) $(1)/usr/bin/
endef

$(eval $(call BuildPackage,$(PKG_NAME)))
