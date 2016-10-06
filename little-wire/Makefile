#
# Copyright (C) 2006-2014 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=little-wire
PKG_RELEASE:=2

include $(INCLUDE_DIR)/package.mk

define Package/little-wire
	SECTION:=utils
	CATEGORY:=Utilities
	TITLE:=Little-Wire components for attiny45 USB Digispark
	MAINTAINER:=https://zftlab.org
	DEPENDS:=+libusb-compat
endef

define Package/little-wire/description
	Little-Wire bootloader, firmware and example utils for attiny45 USB Digispark
endef

define Build/Prepare
	$(INSTALL_DIR) $(PKG_BUILD_DIR)
	$(INSTALL_DATA) ./src/* $(PKG_BUILD_DIR)/
	$(INSTALL_DATA) ./firmware/digispark_t85_littlewire_v13.hex $(PKG_BUILD_DIR)/
endef

define Build/Compile
#	$(TARGET_CC) $(TARGET_CFLAGS) $(TARGET_LDFLAGS) -Wall -lusb -O -g -D LINUX \
#		-o $(PKG_BUILD_DIR)/micronucleus $(PKG_BUILD_DIR)/micronucleus.c $(PKG_BUILD_DIR)/micronucleus_lib.c $(PKG_BUILD_DIR)/littleWire_util.c
	
	$(TARGET_CC) $(TARGET_CFLAGS) $(TARGET_LDFLAGS) -lusb -O -g -D LINUX \
		-o adc $(PKG_BUILD_DIR)/littleWire_util.c $(PKG_BUILD_DIR)/littleWire.c $(PKG_BUILD_DIR)/opendevice.c $(PKG_BUILD_DIR)/adc.c
	
	$(TARGET_CC) $(TARGET_CFLAGS) $(TARGET_LDFLAGS) -lusb -O -g -D LINUX \
		-o onewire $(PKG_BUILD_DIR)/littleWire_util.c $(PKG_BUILD_DIR)/littleWire.c $(PKG_BUILD_DIR)/opendevice.c $(PKG_BUILD_DIR)/onewire.c
endef

define Package/little-wire/install
	$(INSTALL_DIR) $(1)/usr/bin
#	$(INSTALL_BIN) $(PKG_BUILD_DIR)/micronucleus $(1)/usr/bin/
	$(INSTALL_DIR) $(1)/usr/share/little-wire
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/digispark_t85_littlewire_v13.hex $(1)/usr/share/little-wire/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/adc $(1)/usr/bin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/onewire $(1)/usr/bin/
endef

$(eval $(call BuildPackage,little-wire))

