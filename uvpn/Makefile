#
# Copyright (C) 2011-2014 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=uvpn
PKG_VERSION:=2019-10-17
PKG_RELEASE:=0.1
PKG_MAINTAINER:=@mAX3773 and @FlyRouter

include $(INCLUDE_DIR)/package.mk

PKG_LICENSE:=GPL-2.0
PKG_LICENSE_FILES:=LICENSE

define Package/$(PKG_NAME)
	SECTION:=openipc
	CATEGORY:=OpenIPC
	SUBMENU:=VPN
	TITLE:=µVPN - miniature private network
	MAINTAINER:=uvpn@openipc.org
endef

define Package/$(PKG_NAME)/description
  µVPN - miniature private network
endef

define Build/Prepare
endef

define Build/Compile
endef

define Package/$(PKG_NAME)/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) ./binary/$(PKG_NAME) $(1)/usr/sbin/
	$(INSTALL_DIR) $(1)/etc/config
	$(INSTALL_CONF) ./files/uvpn.config $(1)/etc/config/uvpn
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/uvpn.init $(1)/etc/init.d/uvpn

endef

$(eval $(call BuildPackage,$(PKG_NAME)))


