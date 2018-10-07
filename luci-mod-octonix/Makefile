# Copyright (C) 2018 octonix.net
#
# This is free software, licensed under the GNU General Public License v3.
# See /LICENSE for more information.

include $(TOPDIR)/rules.mk

PKG_NAME:=luci-mod-octonix
PKG_VERSION:=$(shell cat ../VERSION)
PKG_RELEASE:=1
PKG_BUILD_DIR := $(BUILD_DIR)/$(PKG_NAME)/$(PKG_SOURCE_SUBDIR)
PKG_LICENSE:=GPL-3.0

include $(INCLUDE_DIR)/package.mk

define Package/luci-mod-octonix
	SECTION:=luci
	CATEGORY:=LuCI
	SUBMENU:=2. Modules
	TITLE:=LuCI Octonix module
	DEPENDS:=+luci-base +libiwinfo-lua
	PKGARCH:=all
	MAINTAINER:=Igor Zalatov <flyrouter@gmail.com>
	URL:=http://octonix.net
endef

define Package/luci-mod-octonix/description
	Octonix configuration Web UI.
endef

define Build/Compile
endef

define Package/luci-mod-octonix/conffiles
/etc/config/luci_octonix
endef

define Package/luci-mod-octonix/install
	$(INSTALL_DIR) \
		$(1)/etc/config \
		$(1)/usr/lib/lua/luci/controller/octonix \
		$(1)/usr/lib/lua/luci/model/cbi/octonix \
		$(1)/usr/lib/lua/luci/view/octonix \
		$(1)/usr/sbin

	$(INSTALL_CONF) \
		luci_octonix.config \
		$(1)/etc/config/luci_octonix

	$(CP) -r luasrc/controller/octonix/* \
		$(1)/usr/lib/lua/luci/controller/octonix/

	$(CP) -r luasrc/model/cbi/octonix/* \
		$(1)/usr/lib/lua/luci/model/cbi/octonix/

	$(CP) -r luasrc/view/octonix/* \
		$(1)/usr/lib/lua/luci/view/octonix/

	$(INSTALL_BIN) \
		sbin/octonix-passwd.lua \
		$(1)/usr/sbin/octonix-passwd
endef

$(eval $(call BuildPackage,luci-mod-octonix))
