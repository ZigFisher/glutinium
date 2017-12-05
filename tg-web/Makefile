#
# Copyright (C) 2006-2017 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=tg-web
PKG_VERSION:=2017-11-30
PKG_RELEASE:=1
PKG_LICENSE:=GPL-2.0

include $(INCLUDE_DIR)/package.mk

define Package/tg-web
	SECTION:=net
	CATEGORY:=Network
	MAINTAINER:=Zig Fisher <flyrouter@gmail.com>
	TITLE:=TG-Web - a minimalistic web interface for bots and automation systems
	URL:=https://github.com/ZigFisher/Glutinium/tree/master/tg-web
	DEPENDS:=+haserl
endef

define Package/tg-web/description
	TG-Web - a minimalistic web interface for bots and automation systems
endef

define Build/Configure
endef

define Build/Compile
endef

define Package/tg-web/install
	$(INSTALL_DIR)  $(1)/etc
	$(CP) ./files/etc/* $(1)/etc/
<------>$(INSTALL_DIR)  $(1)/www
<------>$(CP) ./files/www/* $(1)/www/
	#
	#$(INSTALL_DIR)  $(1)/etc/init.d
	#$(INSTALL_BIN)  ./files/etc/init.d/test $(1)/etc/init.d/
	#$(INSTALL_DIR)  $(1)/usr/bin
	#$(INSTALL_BIN)  ./files/usr/bin/test $(1)/usr/bin/
	3$(INSTALL_DIR)  $(1)/usr/sbin
	#$(INSTALL_BIN)  ./files/usr/sbin/test $(1)/usr/sbin/
endef

$(eval $(call BuildPackage,tg-web))

