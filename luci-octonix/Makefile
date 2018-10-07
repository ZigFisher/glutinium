# Copyright (C) 2018 octonix.net
#
# This is free software, licensed under the GNU General Public License v3.
# See /LICENSE for more information.

include $(TOPDIR)/rules.mk

PKG_NAME:=luci-octonix
PKG_VERSION:=$(shell cat ../VERSION)
PKG_RELEASE:=1
PKG_BUILD_DIR := $(BUILD_DIR)/$(PKG_NAME)/$(PKG_SOURCE_SUBDIR)
PKG_LICENSE:=GPL-3.0

include $(INCLUDE_DIR)/package.mk

define Package/luci-octonix/common
	SECTION:=luci
	CATEGORY:=LuCI
	SUBMENU:=1. Collections
	PKGARCH:=all
	MAINTAINER:=Igor Zalatov <flyrouter@gmail.com>
	URL:=http://octonix.net
endef

define Package/luci-octonix
	$(call Package/luci-octonix/common)
	TITLE:=LuCI Octonix collection
	DEPENDS:=+uhttpd +uhttpd-mod-ubus \
	         +luci-mod-octonix +luci-theme-octonix
endef

define Build/Compile
endef

define Package/luci-octonix/install
endef

$(eval $(call BuildPackage,luci-octonix))
