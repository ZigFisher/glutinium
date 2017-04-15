#
# Copyright (C) 2006-2014 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=i2c-telemetry
PKG_RELEASE:=2

include $(INCLUDE_DIR)/package.mk

define Package/i2c-telemetry
  SECTION:=utils
  CATEGORY:=Utilities
  DEPENDS:=@GPIO_SUPPORT +kmod-i2c-core
  TITLE:=Simple I2C telemetry system
  MAINTAINER:=http://flymon.net
endef

define Package/i2c-telemetry/description
  Simple I2C telemetry system.
endef

define Build/Prepare
	$(INSTALL_DIR) $(PKG_BUILD_DIR)
	$(INSTALL_DATA) ./src/*.c ./src/*.h $(PKG_BUILD_DIR)/
endef

define Build/Compile
	$(TARGET_CC) $(TARGET_CFLAGS) -Wall -o $(PKG_BUILD_DIR)/am2321 $(PKG_BUILD_DIR)/am2321.c
	$(TARGET_CC) $(TARGET_CFLAGS) -Wall -o $(PKG_BUILD_DIR)/bh1750 $(PKG_BUILD_DIR)/bh1750.c $(PKG_BUILD_DIR)/smbus.c
	$(TARGET_CC) $(TARGET_CFLAGS) -Wall -o $(PKG_BUILD_DIR)/bmp085 $(PKG_BUILD_DIR)/bmp085.c $(PKG_BUILD_DIR)/smbus.c
	$(TARGET_CC) $(TARGET_CFLAGS) -Wall -o $(PKG_BUILD_DIR)/ds1621 $(PKG_BUILD_DIR)/ds1621.c
	$(TARGET_CC) $(TARGET_CFLAGS) -Wall -o $(PKG_BUILD_DIR)/ina219 $(PKG_BUILD_DIR)/ina219.c
	$(TARGET_CC) $(TARGET_CFLAGS) -Wall -o $(PKG_BUILD_DIR)/lm75 $(PKG_BUILD_DIR)/lm75.c
	$(TARGET_CC) $(TARGET_CFLAGS) -Wall -o $(PKG_BUILD_DIR)/pcf8591 $(PKG_BUILD_DIR)/pcf8591d.c
	#$(TARGET_CC) $(TARGET_CFLAGS) -Wall -o $(PKG_BUILD_DIR)/pcf8591 $(PKG_BUILD_DIR)/pcf8591.c
	$(TARGET_CC) $(TARGET_CFLAGS) -Wall -o $(PKG_BUILD_DIR)/sht21 $(PKG_BUILD_DIR)/sht21.c
	$(TARGET_CC) $(TARGET_CFLAGS) -Wall -o $(PKG_BUILD_DIR)/ads1115 $(PKG_BUILD_DIR)/ads1115.c
endef

define Package/i2c-telemetry/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/am2321 $(1)/usr/sbin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/bh1750 $(1)/usr/sbin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/bmp085 $(1)/usr/sbin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/ds1621 $(1)/usr/sbin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/ina219 $(1)/usr/sbin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/lm75 $(1)/usr/sbin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/pcf8591 $(1)/usr/sbin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/sht21 $(1)/usr/sbin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/ads1115 $(1)/usr/sbin/
	#$(INSTALL_BIN) ./files/narodmon.sh $(1)/usr/sbin/
	#$(INSTALL_BIN) ./files/thingspeak.sh $(1)/usr/sbin/
endef

$(eval $(call BuildPackage,i2c-telemetry))
