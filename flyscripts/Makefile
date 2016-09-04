#
# Copyright (C) 2006-2014 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=flyscripts
PKG_VERSION:=2.0.1
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=@SF/vtun
PKG_MD5SUM:=d3d8bc4d58886498a1c338670eab9315

PKG_INSTALL:=1

include $(INCLUDE_DIR)/package.mk

define Package/flyscripts
  SECTION:=net
  CATEGORY:=Network
  TITLE:=VPN tunneling daemon (no ssl/lzo/zlib)
  URL:=
endef

define Build/Install
	$(call Build/Install/Default,)
endef

define Package/flyscripts/install
	$(INSTALL_DIR)  $(1)/etc
	$(INSTALL_BIN)  ./files/etc/rc.local.example $(1)/etc/rc.local.example
	$(INSTALL_DIR)  $(1)/etc/hotplug.d/net
	$(INSTALL_BIN)  ./files/etc/hotplug.d/net/37-vtun.example $(1)/etc/hotplug.d/net/37-vtun.example
	$(INSTALL_CONF) ./files/vtund.dat $(1)/etc/
endef

$(eval $(call BuildPackage,flyscripts))
