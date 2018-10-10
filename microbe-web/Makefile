#
# Copyright (C) 2006-2017 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=microbe-web
PKG_VERSION:=2018-10-10
PKG_RELEASE:=1
PKG_MAINTAINER:=ZigFisher

PKG_LICENSE:=GPL-2.0
PKG_LICENSE_FILES:=LICENSE

include $(INCLUDE_DIR)/package.mk

define Package/$(PKG_NAME)
	SECTION:=net
	CATEGORY:=Network
	MAINTAINER:=Zig Fisher <flyrouter@gmail.com>
	TITLE:=Microbe-WEB - a selection of scripts for routers with a small amount of flash memory
	URL:=https://github.com/ZigFisher/Glutinium/tree/master/microbe-web
	DEPENDS:=+busybox +haserl
endef

define Package/$(PKG_NAME)/description
	Microbe-WEB - a selection of scripts for routers with a small amount of flash memory
endef

define Build/Configure
endef

define Build/Compile
endef

define Package/$(PKG_NAME)/install
	$(INSTALL_DIR)  $(1)/etc/init.d
	$(INSTALL_BIN)  ./files/etc/init.d/httpd $(1)/etc/init.d/
	$(INSTALL_DIR)  $(1)/www
	$(INSTALL_BIN)  ./files/www/index.html $(1)/www/
	$(INSTALL_DIR)  $(1)/www/assets/css
	$(INSTALL_BIN)  ./files/www/assets/css/base.css $(1)/www/assets/css/
	$(INSTALL_DIR)  $(1)/www/assets/img
	$(INSTALL_BIN)  ./files/www/assets/img/favicon.ico $(1)/www/assets/img/
	$(INSTALL_DIR)  $(1)/www/assets/img
	$(INSTALL_BIN)  ./files/www/assets/img/octonix.jpg $(1)/www/assets/img/
	$(INSTALL_DIR)  $(1)/www/assets/js
	$(INSTALL_BIN)  ./files/www/assets/js/jquery-1.7.2.min.js $(1)/www/assets/js/
	$(INSTALL_DIR)  $(1)/www/cgi-bin
	$(INSTALL_BIN)  ./files/www/cgi-bin/octonix.cgi $(1)/www/cgi-bin/
endef

$(eval $(call BuildPackage,microbe-web))

