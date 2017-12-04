#
# Copyright (C) Felix Fietkau <nbd@nbd.name>
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=gpio-watch
PKG_VERSION:=2017-06-08
PKG_RELEASE:=1
PKG_MAINTAINER:=larsks
PKG_LICENSE:=GPL-2.0

include $(INCLUDE_DIR)/package.mk

define Package/$(PKG_NAME)
	SECTION:=utils
	CATEGORY:=Utilities
	TITLE:=GPIO monitoring tool
	MAINTAINER:=https://github.com/larsks/gpio-watch
	DEPENDS:=+librt
endef

define Package/$(PKG_NAME)/description
	GPIO monitoring tool
endef

define Build/Prepare
	$(INSTALL_DIR) $(PKG_BUILD_DIR)
	$(INSTALL_DATA) ./src/* $(PKG_BUILD_DIR)/
endef

define Build/Compile
	$(TARGET_CC) $(TARGET_CFLAGS) $(TARGET_LDFLAGS) -O -g -D LINUX \
		-o $(PKG_BUILD_DIR)/gpio-watch $(PKG_BUILD_DIR)/gpio-watch.c $(PKG_BUILD_DIR)/gpio.c $(PKG_BUILD_DIR)/fileutil.c
endef

define Package/$(PKG_NAME)/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/gpio-watch $(1)/usr/sbin/
endef

$(eval $(call BuildPackage,gpio-watch))
