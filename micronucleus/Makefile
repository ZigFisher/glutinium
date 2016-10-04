#
# Copyright (C) 2006-2014 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=micronucleus
PKG_RELEASE:=2.03

include $(INCLUDE_DIR)/package.mk

define Package/micronucleus
  SECTION:=utils
  CATEGORY:=Utilities
  DEPENDS:=+libusb-compat
  TITLE:=This commandline application for the Micronucleus bootloader
  MAINTAINER:=http://zftlab.org
endef

define Package/micronucleus/description
  This commandline application for the Micronucleus bootloader
endef

define Build/Prepare
	$(INSTALL_DIR) $(PKG_BUILD_DIR)
	$(INSTALL_DATA) ./src/*.c ./src/*.h $(PKG_BUILD_DIR)/
endef

define Build/Compile
	$(TARGET_CC) $(TARGET_CFLAGS) $(TARGET_LDFLAGS) -Wall -lusb -o $(PKG_BUILD_DIR)/micronucleus $(PKG_BUILD_DIR)/micronucleus.c $(PKG_BUILD_DIR)/micronucleus_lib.c $(PKG_BUILD_DIR)/littleWire_util.c
endef

define Package/homes-smart/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/micronucleus $(1)/usr/sbin/
endef

$(eval $(call BuildPackage,micronucleus))

