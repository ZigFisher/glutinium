#
# Copyright (C) 2006-2009 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=httping
PKG_VERSION:=1.5.2
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tgz
PKG_SOURCE_URL:=http://www.vanheusden.com/httping
PKG_MD5SUM:=49f72b124afb8a60f152ec674a162ad6

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_INSTALL:=1

include $(INCLUDE_DIR)/package.mk

define Package/httping
  SECTION:=net
  CATEGORY:=Network
  TITLE:=Httping is like 'ping' but for http-requests
  URL:=http://www.vanheusden.com/httping/
endef

define Package/httping/description
	Give it an url, and it'll show you how long it takes to connect, send a
	request and retrieve the reply (only the headers). Be aware that the
	transmission across the network also takes time!
endef

TARGET_CFLAGS += -D_GNU_SOURCE
MAKE_FLAGS += SSL="no"
TARGET_CFLAGS += -DNO_SSL

define Package/httping/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/httping $(1)/usr/bin/
endef

$(eval $(call BuildPackage,httping))
