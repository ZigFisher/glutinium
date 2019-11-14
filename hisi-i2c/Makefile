#
# Copyright (C) 2011-2014 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=hisi-i2c
PKG_VERSION:=2019-02-03
PKG_RELEASE:=0.1
PKG_MAINTAINER:=Igor Zalatov (ZFT Lab.) <flyrouter@gmail.com>

include $(INCLUDE_DIR)/package.mk

PKG_LICENSE:=GPL-2.0
PKG_LICENSE_FILES:=LICENSE
#PKG_BUILD_DEPENDS:=busybox

define Package/$(PKG_NAME)
	SECTION:=openipc
	CATEGORY:=OpenIPC
	SUBMENU:=Tools
	TITLE:=I2C read/write tools for hisilicon platform
	MAINTAINER:=hisilicon
	DEPENDS:=@(TARGET_hi35xx) +hisi-osdrv2-base +hisi-osdrv2-kmod
endef

define Package/$(PKG_NAME)/description
  I2C read/write tools for hisilicon platform
endef

define Build/Prepare
	$(INSTALL_DIR) $(PKG_BUILD_DIR)
	$(INSTALL_DATA) ./src/*.c $(PKG_BUILD_DIR)/
endef

define Build/Compile
	$(TARGET_CC) $(TARGET_CFLAGS) -Wall \
		-o $(PKG_BUILD_DIR)/i2c_read $(PKG_BUILD_DIR)/i2c_read.c
	$(TARGET_CC) $(TARGET_CFLAGS) -Wall \
		-o $(PKG_BUILD_DIR)/i2c_write $(PKG_BUILD_DIR)/i2c_write.c
endef

define Package/$(PKG_NAME)/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/{i2c_read,i2c_write} $(1)/usr/bin/
endef

$(eval $(call BuildPackage,$(PKG_NAME)))


