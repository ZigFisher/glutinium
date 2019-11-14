#
# Copyright (C) 2011-2014 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=hi_chip_info
PKG_VERSION:=2019-02-13
PKG_MAINTAINER:=Chertov Maxim <chertovmv@gmail.com>

PKG_LICENSE:=MIT
PKG_LICENSE_FILES:=LICENSE

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=https://github.com/chertov/hi_chip_info.git
PKG_SOURCE_VERSION:=3f8c60912f59799020792c362b3bc0a6477de1ee

PKG_SOURCE_SUBDIR:=$(PKG_NAME)-$(PKG_VERSION)-$(PKG_SOURCE_VERSION)
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION)-$(PKG_SOURCE_VERSION).tar.gz
PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)-$(PKG_SOURCE_VERSION)

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/cmake.mk

define Package/$(PKG_NAME)
	SECTION:=openipc
	CATEGORY:=OpenIPC
	SUBMENU:=Tools
	TITLE:=Tool for get info from Hisilicon SoC's
	MAINTAINER:=chertovmv@gmail.com
	DEPENDS:=@(TARGET_hi35xx)
endef

define Package/$(PKG_NAME)/description
    Tool for get info from Hisilicon SoC's
endef

define Package/$(PKG_NAME)/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/$(PKG_NAME) $(1)/usr/bin/

endef

$(eval $(call BuildPackage,$(PKG_NAME)))
