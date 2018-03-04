#
# Copyright (C) 2006-2015 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=mii-diag
PKG_VERSION:=2018-03-04
PKG_RELEASE:=1
PKG_MAINTAINER:=Igor Zalatov <flyrouter@gmail.com>

PKG_LICENSE:=GPL-2.0
PKG_LICENSE_FILES:=LICENSE

include $(INCLUDE_DIR)/package.mk

define Package/$(PKG_NAME)
	SECTION:=utils
	CATEGORY:=Utilities
	TITLE:=Network adapter control and monitoring
	MAINTAINER:=https://github.com/oe-lite/base/tree/master/recipes/net-tools/files
endef

define Package/$(PKG_NAME)/description
	Network adapter control and monitoring
endef

define Build/Prepare
	$(INSTALL_DIR) $(PKG_BUILD_DIR)
	$(INSTALL_DATA) ./src/* $(PKG_BUILD_DIR)/
endef

define Build/Compile
	$(TARGET_CC) $(TARGET_CFLAGS) $(TARGET_LDFLAGS) -O -g -D LINUX \
		-o $(PKG_BUILD_DIR)/mii-diag $(PKG_BUILD_DIR)/mii-diag.c $(PKG_BUILD_DIR)/libmii.c
endef

define Package/$(PKG_NAME)/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/mii-diag $(1)/usr/bin/
endef

$(eval $(call BuildPackage,$(PKG_NAME)))
