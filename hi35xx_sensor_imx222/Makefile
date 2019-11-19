#
# Copyright (C) 2011-2014 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=hi35xx_sensor_imx222
PKG_VERSION:=20191119
PKG_RELEASE:=1
PKG_MAINTAINER:=Igor Zalatov (ZFT Lab.) <flyrouter@gmail.com>

include $(INCLUDE_DIR)/package.mk

PKG_LICENSE:=MIT
PKG_LICENSE_FILES:=LICENSE

PKG_BUILD_DEPENDS:=hisi-osdrv2

define Package/$(PKG_NAME)
	SECTION:=openipc
	CATEGORY:=OpenIPC
	SUBMENU:=Sensors
	TITLE:=IMX222 sensor driver for HI35xx SoC
	MAINTAINER:=<dev@openipc.org>
	DEPENDS:=@(TARGET_hi35xx) +hisi-osdrv2-lib
endef

define Package/$(PKG_NAME)/description
  IMX222 sensor driver for HI35xx SoC
endef

define Build/Prepare
	$(INSTALL_DIR) $(PKG_BUILD_DIR)
	$(INSTALL_DATA) ./src/*.c $(PKG_BUILD_DIR)/
endef

define Build/Compile
	$(TARGET_CC) $(TARGET_CFLAGS) -Wall -fPIC -O2 -lm -shared -I$(STAGING_DIR)/usr/include/hisi-osdrv2 \
		-o $(PKG_BUILD_DIR)/libsns_imx222.so $(PKG_BUILD_DIR)/imx222_cmos.c $(PKG_BUILD_DIR)/imx222_sensor_ctl.c
endef

define Package/$(PKG_NAME)/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/*.so $(1)/usr/lib/
endef

$(eval $(call BuildPackage,$(PKG_NAME)))






