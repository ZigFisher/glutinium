#
# Copyright (C) 2011-2014 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=ipc_chip_info
PKG_VERSION:=2020-latest
PKG_MAINTAINER:=Dmitry Ilyin <dima@doty.ru>

PKG_LICENSE:=MIT
PKG_LICENSE_FILES:=LICENSE

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=https://github.com/OpenIPC/ipc_chip_info.git
PKG_SOURCE_VERSION:=852d0d49df3f632bcb18b6bebcb8d115a5bad43f

PKG_SOURCE_SUBDIR:=$(PKG_NAME)-$(PKG_VERSION)-$(PKG_SOURCE_VERSION)
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION)-$(PKG_SOURCE_VERSION).tar.gz
PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)-$(PKG_SOURCE_VERSION)

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/cmake.mk

define Package/$(PKG_NAME)
	SECTION:=openipc
	CATEGORY:=OpenIPC
	SUBMENU:=Tools
	TITLE:=New tool for get info from IPCamera SoC's
	MAINTAINER:=dima@doty.ru
	DEPENDS:=@(TARGET_hi35xx)
endef

define Package/$(PKG_NAME)/description
    New tool for get info from IPCamera (Hisi) SoC's
endef

define Package/$(PKG_NAME)/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/$(PKG_NAME) $(1)/usr/bin/

endef

$(eval $(call BuildPackage,$(PKG_NAME)))
