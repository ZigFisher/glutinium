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

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/cmake.mk

define Package/$(PKG_NAME)
	SECTION:=multimedia
	CATEGORY:=Multimedia
	TITLE:=Some samples for Hisilicon devices
	SUBMENU:=Hisilicon OSDRV-2 for HI35_16C_18E_V200 with kernel 3.4.35
	MAINTAINER:=chertovmv@gmail.com
	DEPENDS:=+hisi-osdrv2-lib +libpthread
	#DEPENDS:=+libstdcpp +libyaml-cpp +libpthread  +hidrv
endef

define Package/$(PKG_NAME)/description
	Some samples for Hisilicon devices
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
