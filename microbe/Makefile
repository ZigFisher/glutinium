#
# Copyright (C) 2006-2017 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=microbe
PKG_VERSION:=2017-11-30
PKG_RELEASE:=1
PKG_MAINTAINER:=ZigFisher

PKG_LICENSE:=GPL-2.0
PKG_LICENSE_FILES:=LICENSE

include $(INCLUDE_DIR)/package.mk

define Package/$(PKG_NAME)
	SECTION:=net
	CATEGORY:=Network
	MAINTAINER:=Zig Fisher <flyrouter@gmail.com>
	TITLE:=Microbe - a selection of scripts for routers with a small amount of flash memory
	URL:=https://github.com/ZigFisher/Glutinium/tree/master/microbe
	DEPENDS:=+busybox
endef

define Package/$(PKG_NAME)/description
	Microbe - a selection of scripts for routers with a small amount of flash memory
endef

define Build/Configure
endef

define Build/Compile
endef

define Package/$(PKG_NAME)/install
	$(INSTALL_DIR)  $(1)/etc
	$(INSTALL_BIN)  ./files/etc/inittab $(1)/etc/inittab
	#$(INSTALL_DIR)  $(1)/etc/config
	#$(INSTALL_BIN)  ./files/etc/config/test $(1)/etc/config/
	#$(INSTALL_DIR)  $(1)/etc/init.d
	#$(INSTALL_BIN)  ./files/etc/init.d/test $(1)/etc/init.d/
	#$(INSTALL_DIR)  $(1)/usr/bin
	#$(INSTALL_BIN)  ./files/usr/bin/test $(1)/usr/bin/
	#$(INSTALL_DIR)  $(1)/usr/sbin
	#$(INSTALL_BIN)  ./files/usr/sbin/test $(1)/usr/sbin/
endef

$(eval $(call BuildPackage,microbe))

