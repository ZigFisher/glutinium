# Copyright (C) 2016 openwisp.org
#
# This is free software, licensed under the GNU General Public License v3.
# See /LICENSE for more information.

include $(TOPDIR)/rules.mk

PKG_NAME:=luci-theme-openwisp
PKG_VERSION:=$(shell cat ../VERSION)
PKG_RELEASE:=1
PKG_BUILD_DIR := $(BUILD_DIR)/$(PKG_NAME)/$(PKG_SOURCE_SUBDIR)
PKG_LICENSE:=GPL-3.0

LUCI_DEFAULTS:=$(notdir $(wildcard ${CURDIR}/root/etc/uci-defaults/*))

include $(INCLUDE_DIR)/package.mk

define Package/luci-theme-openwisp
	SECTION:=luci
	CATEGORY:=LuCI
	SUBMENU:=4. Themes
	TITLE:=OpenWISP theme
	DEPENDS:=+luci-theme-bootstrap
	PKGARCH:=all
	MAINTAINER:=Federico Capoano <f.capoano@cineca.it>
	URL:=http://openwisp.org
endef

define Package/luci-theme-openwisp/description
	OpenWISP theme for LuCI, based on the bootstrap theme.
endef

define Build/Compile
endef

define Package/luci-theme-openwisp/install
	$(INSTALL_DIR) \
		$(1)/usr/lib/lua/luci/view/themes/openwisp \
		$(1)/www/luci-static/openwisp \
		$(1)/etc/uci-defaults

	$(CP) -r luasrc/view/themes/openwisp/* \
		$(1)/usr/lib/lua/luci/view/themes/openwisp/

	$(CP) -r htdocs/luci-static/openwisp/* \
		$(1)/www/luci-static/openwisp/

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

$(eval $(call BuildPackage,luci-theme-openwisp))
