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
PKG_MAINTAINER:=Igor Zalatov <flyrouter@gmail.com>

PKG_LICENSE:=GPL-2.0
PKG_LICENSE_FILES:=LICENSE

include $(INCLUDE_DIR)/package.mk

define Package/$(PKG_NAME)
	SECTION:=net
	CATEGORY:=Network
	MAINTAINER:=Igor Zalatov <flyrouter@gmail.com>
	TITLE:=Extra small and simple WEB-interface for embedded system
	URL:=https://github.com/ZigFisher/Glutinium/tree/master/microbe-web
	DEPENDS:=+busybox +haserl
endef

define Package/$(PKG_NAME)/description
	Extra small and simple WEB-interface for embedded system
endef

define Build/Configure
endef

define Build/Compile
endef

define Package/$(PKG_NAME)/install
	$(INSTALL_DIR)  $(1)/etc
	$(CP) ./files/etc/* $(1)/etc/
	$(INSTALL_DIR)  $(1)/www
	$(CP) ./files/www/* $(1)/www/
endef

$(eval $(call BuildPackage,microbe-web))

