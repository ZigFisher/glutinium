#
# Copyright (C) 2012-2013 Vlad Moskovets vlad.moskovets@gmail.com
# Copyright (C) 2013-2014 Zig Fisher flyrouter@gmail.com
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=barcode
PKG_RELEASE:=2

include $(INCLUDE_DIR)/package.mk

define Package/barcode
  SECTION:=utils
  CATEGORY:=Utilities
  TITLE:=Utility to get data from event devices
  MAINTAINER:=https://github.com/ZigFisher/Glutinium
endef

define Package/barcode/description
 This package get data from event devices.
endef

define Build/Prepare
	$(INSTALL_DIR) $(PKG_BUILD_DIR)
	$(INSTALL_DATA) ./src/barcode.c $(PKG_BUILD_DIR)/
endef

define Build/Compile
	$(TARGET_CC) $(TARGET_CFLAGS) -Wall \
		-o $(PKG_BUILD_DIR)/barcode $(PKG_BUILD_DIR)/barcode.c
endef

define Package/barcode/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/barcode $(1)/usr/bin/
endef

$(eval $(call BuildPackage,barcode))
