#
# Copyright (C) 2006-2014 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=homes-smart
PKG_RELEASE:=2

include $(INCLUDE_DIR)/package.mk

define Package/homes-smart
  SECTION:=utils
  CATEGORY:=Utilities
  DEPENDS:=+libusb-1.0
  TITLE:=Some utils for Homes-smarT project based on USBasp device
  MAINTAINER:=http://zftlab.org
endef

define Package/homes-smart/description
  Some utils for Homes-smarT project based on USBasp device.
endef

define Build/Prepare
	$(INSTALL_DIR) $(PKG_BUILD_DIR)
	$(INSTALL_DATA) ./src/*.c ./src/*.h $(PKG_BUILD_DIR)/
endef

define Build/Compile
	#$(TARGET_CC) $(TARGET_CFLAGS) -Wall -lusb-1.0 -o $(PKG_BUILD_DIR)/usbgpio $(PKG_BUILD_DIR)/usbgpio.c $(PKG_BUILD_DIR)/opendevice.c
	$(TARGET_CC) $(TARGET_CFLAGS) -Wall -lusb-1.0 -o $(PKG_BUILD_DIR)/usbtemp $(PKG_BUILD_DIR)/usbtemp.c
endef

define Package/homes-smart/install
	$(INSTALL_DIR) $(1)/usr/sbin
	#$(INSTALL_BIN) $(PKG_BUILD_DIR)/usbgpio $(1)/usr/sbin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/usbtemp $(1)/usr/sbin/
endef

$(eval $(call BuildPackage,homes-smart))
