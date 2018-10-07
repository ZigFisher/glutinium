# Copyright (C) 2018 octonix.net
#
# This is free software, licensed under the GNU General Public License v3.
# See /LICENSE for more information.

include $(TOPDIR)/rules.mk

PKG_NAME:=luci-theme-octonix
PKG_VERSION:=$(shell cat ../VERSION)
PKG_RELEASE:=1
PKG_BUILD_DIR := $(BUILD_DIR)/$(PKG_NAME)/$(PKG_SOURCE_SUBDIR)
PKG_LICENSE:=GPL-3.0

LUCI_DEFAULTS:=$(notdir $(wildcard ${CURDIR}/root/etc/uci-defaults/*))

include $(INCLUDE_DIR)/package.mk

define Package/luci-theme-octonix
	SECTION:=luci
	CATEGORY:=LuCI
	SUBMENU:=4. Themes
	TITLE:=Octonix theme
	DEPENDS:=+luci-theme-bootstrap
	PKGARCH:=all
	MAINTAINER:=Igor Zalatov <flyrouter@gmail.com>
	URL:=http://octonix.net
endef

define Package/luci-theme-octonix/description
	Octonix theme for LuCI, based on the bootstrap theme.
endef

define Build/Compile
endef

define Package/luci-theme-octonix/install
	$(INSTALL_DIR) \
		$(1)/usr/lib/lua/luci/view/themes/octonix \
		$(1)/www/luci-static/octonix \
		$(1)/etc/uci-defaults

	$(CP) -r luasrc/view/themes/octonix/* \
		$(1)/usr/lib/lua/luci/view/themes/octonix/

	$(CP) -r htdocs/luci-static/octonix/* \
		$(1)/www/luci-static/octonix/

	$(CP) -pR root/etc/uci-defaults/* \
		$(1)/etc/uci-defaults/
endef

ifneq ($(LUCI_DEFAULTS),)
define Package/$(PKG_NAME)/postinst
[ -n "$${IPKG_INSTROOT}" ] || {$(foreach script,$(LUCI_DEFAULTS),
	(. /etc/uci-defaults/$(script)) && rm -f /etc/uci-defaults/$(script))
	exit 0
}
endef
endif

$(eval $(call BuildPackage,luci-theme-octonix))
