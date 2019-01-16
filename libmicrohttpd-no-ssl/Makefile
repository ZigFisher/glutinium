#
# Copyright (C) 2010-2014 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=libmicrohttpd-no-ssl
PKG_VERSION:=0.9.38
PKG_RELEASE:=1.2

PKG_SOURCE:=libmicrohttpd-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=@GNU/libmicrohttpd
PKG_MD5SUM:=b72efbfe7f290846015d460aea7091fe

PKG_MAINTAINER:=Martijn Zilverschoon <martijn@friedzombie.com>

PKG_LICENSE:=LGPL-2.1
PKG_LICENSE_FILES:=COPYING

#PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_BUILD_DIR:=$(BUILD_DIR)/libmicrohttpd-$(PKG_VERSION)
PKG_INSTALL:=1

include $(INCLUDE_DIR)/package.mk

define Package/$(PKG_NAME)
	SECTION:=libs
	CATEGORY:=Libraries
	TITLE:=GNU libmicrohttpd library without SSL
	URL:=http://www.gnu.org/software/libmicrohttpd/
	DEPENDS:=+libpthread
endef

define Package/$(PKG_NAME)/description
	GNU libmicrohttpd is a small C library that is supposed to make it easy
	to run an HTTP server as part of another application.
endef

CONFIGURE_ARGS+= \
	--disable-curl \
	--disable-spdy \
	--disable-https \
	--without-openssl \
	--without-gnutls

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include/
	$(INSTALL_DATA) $(PKG_INSTALL_DIR)/usr/include/* $(1)/usr/include/
	$(INSTALL_DIR) $(1)/usr/lib/
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/*.{so*,a,la} $(1)/usr/lib/
	$(INSTALL_DIR) $(1)/usr/lib/pkgconfig/
	$(INSTALL_DATA) $(PKG_INSTALL_DIR)/usr/lib/pkgconfig/* $(1)/usr/lib/pkgconfig/
endef

define Package/$(PKG_NAME)/install
	$(INSTALL_DIR) $(1)/usr/lib/
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/*.so* $(1)/usr/lib/
endef

$(eval $(call BuildPackage,$(PKG_NAME)))
