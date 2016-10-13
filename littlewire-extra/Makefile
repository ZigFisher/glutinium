#
# Copyright (C) 2006-2014 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=littlewire-extra
PKG_VERSION:=2016-10-14
PKG_RELEASE:=1
PKG_MAINTAINER:=Igor Zalatov <flyrouter@gmail.com>

PKG_LICENSE:=GPL-2.0
PKG_LICENSE_FILES:=LICENSE

include $(INCLUDE_DIR)/package.mk

define Package/littlewire-extra
	SECTION:=utils
	CATEGORY:=Utilities
	TITLE:=Little Wire experimental utilites
	MAINTAINER:=https://zftlab.org
	DEPENDS:=+libusb-compat
endef

define Package/littlewire-extra/description
	Little Wire experimental utilites
endef

define Build/Prepare
	$(INSTALL_DIR) $(PKG_BUILD_DIR)
	$(INSTALL_DATA) ./src/* $(PKG_BUILD_DIR)/
endef

define Build/Compile
	$(TARGET_CC) $(TARGET_CFLAGS) $(TARGET_LDFLAGS) -lusb -O -g -D LINUX \
		-o octonix_gpio $(PKG_BUILD_DIR)/littleWire_util.c $(PKG_BUILD_DIR)/littleWire.c $(PKG_BUILD_DIR)/opendevice.c $(PKG_BUILD_DIR)/octonix_gpio.c
	
endef

define Package/littlewire-extra/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/octonix_gpio0 $(1)/usr/bin/
endef

$(eval $(call BuildPackage,littlewire-extra))
