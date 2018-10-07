# Copyright (C) 2016 openwisp.org
#
# This is free software, licensed under the GNU General Public License v3.
# See /LICENSE for more information.

include $(TOPDIR)/rules.mk

PKG_NAME:=luci-openwisp
PKG_VERSION:=$(shell cat ../VERSION)
PKG_RELEASE:=1
PKG_BUILD_DIR := $(BUILD_DIR)/$(PKG_NAME)/$(PKG_SOURCE_SUBDIR)
PKG_LICENSE:=GPL-3.0

include $(INCLUDE_DIR)/package.mk

define Package/luci-openwisp/common
	SECTION:=luci
	CATEGORY:=LuCI
	SUBMENU:=1. Collections
	PKGARCH:=all
	MAINTAINER:=Federico Capoano <f.capoano@cineca.it>
	URL:=http://openwisp.org
endef

define Package/luci-openwisp
	$(call Package/luci-openwisp/common)
	TITLE:=LuCI OpenWISP collection
	DEPENDS:=+uhttpd +uhttpd-mod-ubus \
	         +luci-mod-openwisp +luci-theme-openwisp
endef

define Package/luci-openwisp/ssl
	$(call Package/luci-openwisp/common)
	TITLE:=LuCI OpenWISP collection with HTTPS support ($(2))
	VARIANT:=$(1)
	DEPENDS:=+luci-openwisp $(3) +px5g
endef

Package/luci-openwisp-openssl=$(call Package/luci-openwisp/ssl,openssl,OpenSSL,+libustream-openssl)
Package/luci-openwisp-mbedtls=$(call Package/luci-openwisp/ssl,mbedtls,mbedTLS,+libustream-mbedtls)
Package/luci-openwisp-polarssl=$(call Package/luci-openwisp/ssl,polarssl,PolarSSL,+libustream-polarssl)
Package/luci-openwisp-cyassl=$(call Package/luci-openwisp/ssl,cyassl,cyaSSL,+libustream-cyassl)

define Build/Compile
endef

define Package/luci-openwisp/install
endef

define Package/luci-openwisp-openssl/install
endef

define Package/luci-openwisp-mbedtls/install
endef

define Package/luci-openwisp-polarssl/install
endef

define Package/luci-openwisp-cyassl/install
endef

$(eval $(call BuildPackage,luci-openwisp))
$(eval $(call BuildPackage,luci-openwisp-openssl))
$(eval $(call BuildPackage,luci-openwisp-mbedtls))
$(eval $(call BuildPackage,luci-openwisp-polarssl))
$(eval $(call BuildPackage,luci-openwisp-cyassl))
