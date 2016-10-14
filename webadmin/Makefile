#
# Copyright (C) 2006-2014 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=webadmin
PKG_VERSION:=2016-10-14
PKG_RELEASE:=1
PKG_MAINTAINER:=Igor Zalatov <flyrouter@gmail.com>
PKG_LICENSE:=GPL-2.0

include $(INCLUDE_DIR)/package.mk

define Package/webadmin
	SECTION:=admin
	CATEGORY:=Administration
	TITLE:=FlyRouter v.2 WEB administration interface
	MAINTAINER:=http://flyrouter.net
	#DEPENDS:=
endef

define Build/Configure
endef

define Build/Compile
endef

define Package/webadmin/install
	$(INSTALL_DIR)  $(1)/www
	$(INSTALL_BIN)  ./files/www/* $(1)/www/
	#$(CP)  ./files/www/* $(1)/www/
endef

$(eval $(call BuildPackage,webadmin))

