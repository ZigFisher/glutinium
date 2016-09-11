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

define Package/flyscripts/default
  SECTION:=base
  CATEGORY:=Extra packages
  DEPENDS:=+comgt
  MAINTAINER:=Zig Fisher <flyrouter@gmail.com>
  TITLE:=Some FlyRouter scripts
  URL:=https://github.com/ZigFisher/Glutinium/tree/master/flyscripts
endef

define Package/flyscripts_all
$(call Package/flyscripts/default)
  TITLE:=Some FlyRouter scripts
endef

define Package/flyscripts_octonix_bricket_base
$(call Package/flyscripts/default)
  TITLE:=Custom project settings
endef

define Package/flyscripts_rkeeper_wr841n_base
$(call Package/flyscripts/default)
  TITLE:=Custom project settings
endef

define Package/flyscripts_switron_rt2257_base
$(call Package/flyscripts/default)
  TITLE:=Custom project settings
endef

define Package/flyscripts_zftlab_bricket_base
$(call Package/flyscripts/default)
  TITLE:=Custom project settings
endef

define Build/Compile
endef

define Build/Configure
endef

define Package/flyscripts_all/install
	$(INSTALL_DIR)  $(1)/etc
	$(INSTALL_BIN)  ./files/etc/rc.local.example $(1)/etc/
	$(INSTALL_BIN)  ./files/etc/ser2net.conf $(1)/etc/
	$(INSTALL_DIR)  $(1)/etc/hotplug.d/iface
	$(INSTALL_BIN)  ./files/etc/hotplug.d/iface/12-vtun $(1)/etc/hotplug.d/iface/
	$(INSTALL_DIR)  $(1)/etc/hotplug.d/net
	$(INSTALL_BIN)  ./files/etc/hotplug.d/net/37-vtun.example $(1)/etc/hotplug.d/net/
	$(INSTALL_DIR)  $(1)/etc/hotplug.d/usb
	$(INSTALL_BIN)  ./files/etc/hotplug.d/usb/30-com-* $(1)/etc/hotplug.d/usb/
	$(INSTALL_BIN)  ./files/etc/hotplug.d/usb/30-gps-* $(1)/etc/hotplug.d/usb/
	$(INSTALL_BIN)  ./files/etc/hotplug.d/usb/30-modem-* $(1)/etc/hotplug.d/usb/
	$(INSTALL_DIR)  $(1)/usr/sbin
	$(INSTALL_BIN)  ./files/usr/sbin/service_flypeek_usb_yandex.sh $(1)/usr/sbin/
	$(INSTALL_DIR)  $(1)/lib/netifd/proto
	$(INSTALL_BIN)  ./files/lib/netifd/proto/3g.sh $(1)/lib/netifd/proto/
endef

define Package/flyscripts_octonix_bricket_base/install
	$(INSTALL_DIR)  $(1)/etc
	$(INSTALL_BIN)  ./files/etc/banner/banner__octonix_bricket_base $(1)/etc/banner
	$(INSTALL_DIR)  $(1)/etc/uci-defaults
	$(INSTALL_BIN)  ./files/etc/uci-defaults/55_octonix_bricket_base $(1)/etc/uci-defaults/
endef

define Package/flyscripts_zftlab_bricket_base/install
	$(INSTALL_DIR)  $(1)/etc
	$(INSTALL_BIN)  ./files/etc/banner/banner__zftlab_bricket_base $(1)/etc/banner
	$(INSTALL_DIR)  $(1)/etc/uci-defaults
	$(INSTALL_BIN)  ./files/etc/uci-defaults/55_zftlab_bricket_base $(1)/etc/uci-defaults/
endef

$(eval $(call BuildPackage,flyscripts_all))
$(eval $(call BuildPackage,flyscripts_octonix_bricket_base))
$(eval $(call BuildPackage,flyscripts_rkeeper_wr841n_base))
$(eval $(call BuildPackage,flyscripts_switron_rt2257_base))
$(eval $(call BuildPackage,flyscripts_zftlab_bricket_base))
