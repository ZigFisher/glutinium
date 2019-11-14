#
# Copyright (C) 2006-2018 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=hisi-sample
PKG_VERSION:=0.1
PKG_RELEASE:=1
PKG_MAINTAINER:=Chertov Maxim <chertovmv@gmail.com>

PKG_LICENSE:=GPL-2.0
PKG_LICENSE_FILES:=LICENSE

PKG_BUILD_DEPENDS:=hisi-osdrv2

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/cmake.mk

define Package/$(PKG_NAME)
	SECTION:=openipc
	CATEGORY:=OpenIPC
	SUBMENU:=Unused
	TITLE:=Best samples for Hisilicon devices
	MAINTAINER:=chertovmv@gmail.com
	DEPENDS:=@(TARGET_hi35xx) +libpthread +hisi-osdrv2-base +hisi-osdrv2-kmod +hisi-osdrv2-lib +hisi-osdrv2-sensor
endef

define Package/$(PKG_NAME)/description
	Best samples for Hisilicon devices
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)/
endef

define Package/$(PKG_NAME)/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/rtsp-hisi $(1)/usr/bin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/hijpg $(1)/usr/bin/
endef

$(eval $(call BuildPackage,$(PKG_NAME)))
