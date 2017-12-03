#
# Copyright (C) Felix Fietkau <nbd@nbd.name>
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=gpio-watch
PKG_RELEASE:=1

PKG_FLAGS:=nonshared

include $(INCLUDE_DIR)/package.mk

define Package/gpio-watch
  DEPENDS:=+librt
  SECTION:=utils
  CATEGORY:=Utilities
  TITLE:=GPIO monitoring tool
endef

define Package/$(PKG_NAME)/Build/Compile
	$(TARGET_CC) $(TARGET_CFLAGS) $(TARGET_LDFLAGS) -o $(PKG_BUILD_DIR)/gpio-watch ./src/gpio-watch.c
endef

define Package/$(PKG_NAME)/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/gpio-watch $(1)/usr/bin/
endef

$(eval $(call BuildPackage,gpio-watch))
