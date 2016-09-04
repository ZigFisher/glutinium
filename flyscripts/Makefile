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
PKG_LICENSE:=GPL-2.0

include $(INCLUDE_DIR)/package.mk

define Package/flyscripts
  SECTION:=utils
  CATEGORY:=Utilities
  MAINTAINER:=Zig Fisher <flyrouter@gmail.com>
  TITLE:=Some FlyRouter scripts
  URL:=https://github.com/ZigFisher/Glutinium/tree/master/flyscripts
endef

define Package/flyscripts/description
 Some FlyRouter scripts
endef

define Build/Compile
endef

define Build/Configure
endef

define Package/flyscripts/install
	$(INSTALL_DIR)  $(1)/etc
	$(INSTALL_BIN)  ./files/etc/rc.local.example $(1)/etc/rc.local.example
	$(INSTALL_DIR)  $(1)/etc/hotplug.d/net
	$(INSTALL_BIN)  ./files/etc/hotplug.d/net/37-vtun.example $(1)/etc/hotplug.d/net/
	$(INSTALL_DIR)  $(1)/etc/hotplug.d/usb
	$(INSTALL_BIN)  ./files/etc/hotplug.d/usb/30-com-* $(1)/etc/hotplug.d/usb/
	$(INSTALL_BIN)  ./files/etc/hotplug.d/usb/30-gps-* $(1)/etc/hotplug.d/usb/
	$(INSTALL_BIN)  ./files/etc/hotplug.d/usb/30-modem-* $(1)/etc/hotplug.d/usb/
	$(INSTALL_DIR)  $(1)/usr/sbin
	$(INSTALL_BIN)  ./files/usr/sbin/service_flypeek_usb_yandex.sh $(1)/usr/sbin/
endef

$(eval $(call BuildPackage,flyscripts))
