#
# Copyright (C) 2006-2014 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=mt76x8
PKG_VERSION:=1
PKG_RELEASE:=1

include $(INCLUDE_DIR)/package.mk

define Package/mt76x8
  SECTION:=net
  CATEGORY:=Network
  TITLE:=Ralink APSoC WiFi SoftAP driver
endef

define Build/Compile
endef

define Package/mt76x8/install
	$(INSTALL_DIR) $(1)/lib/modules/$(LINUX_VERSION) $(1)/etc/Wireless/RT2860/
	$(CP) -rf ./files/* $(1)/
	$(CP) ./modules/* $(1)/lib/modules/$(LINUX_VERSION)
endef

$(eval $(call BuildPackage,mt76x8))

