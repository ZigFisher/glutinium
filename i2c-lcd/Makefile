#
# Copyright (C) 2006-2014 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=i2c-lcd
PKG_RELEASE:=2

include $(INCLUDE_DIR)/package.mk

define Package/i2c-lcd
  SECTION:=utils
  CATEGORY:=Utilities
  DEPENDS:=@TARGET_ramips @GPIO_SUPPORT +kmod-i2c-core
  TITLE:=LCD monitor for HD44780 over I2C by PCF8574 bus expander
  MAINTAINER:=http://flymon.net
endef

define Package/i2c-lcd/description
  LCD monitor for HD44780 over I2C by PCF8574 bus expander.
endef

define Build/Prepare
	$(INSTALL_DIR) $(PKG_BUILD_DIR)
	#$(INSTALL_DATA) ./src/*.c ./src/*.h $(PKG_BUILD_DIR)/
endef

define Build/Compile
	#$(TARGET_CC) $(TARGET_CFLAGS) -Wall -o $(PKG_BUILD_DIR)/i2c_HD44780 $(PKG_BUILD_DIR)/i2c_HD44780.c
endef

define Package/i2c-lcd/install
	$(INSTALL_DIR) $(1)/etc/config
	$(INSTALL_CONF) ./files/i2c_HD44780 $(1)/etc/config/i2c_HD44780
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/i2c_HD44780.init $(1)/etc/init.d/i2c_HD44780
	$(INSTALL_DIR) $(1)/usr/sbin
	#$(INSTALL_BIN) $(PKG_BUILD_DIR)/i2c_HD44780 $(1)/usr/sbin/
	$(INSTALL_BIN) ./src/i2c_HD44780 $(1)/usr/sbin/
endef

$(eval $(call BuildPackage,i2c-lcd))
