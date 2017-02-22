#
# Copyright (C) 2006-2014 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=ralink-wifi-mt76x8
PKG_VERSION:=2.0.1
PKG_RELEASE:=1
PKG_LICENSE:=GPL-2.0

include $(INCLUDE_DIR)/package.mk

define Package/ralink-wifi-mt76x8
  SECTION:=base
  CATEGORY:=Extra packages
  DEPENDS:=+wireless-tools +maccalc
  MAINTAINER:=Zig Fisher <flyrouter@gmail.com>
  TITLE:=Ralink APSoC WiFi SoftAP driver
  URL:=https://github.com/ZigFisher/Glutinium
endef

define Package/ralink-wifi-mt76x8/description
	Ralink APSoC WiFi driver (binary)
endef

define Build/Configure
endef

define Build/Compile
endef

define Package/ralink-wifi-mt76x8/install
	$(INSTALL_DIR) $(1)/lib/modules/$(LINUX_VERSION) $(1)/etc/Wireless/RT2860/
	$(CP) -rf ./files/* $(1)/
	$(CP) ./modules/* $(1)/lib/modules/$(LINUX_VERSION)
endef

$(eval $(call BuildPackage,ralink-wifi-mt76x8))

