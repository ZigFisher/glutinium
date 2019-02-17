#
# Copyright (C) 2015 Gateworks Corporation
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=button-hotplug-gw
PKG_RELEASE:=1

include $(INCLUDE_DIR)/package.mk

define Package/button-hotplug-gw
  SECTION:=utils
  CATEGORY:=Gateworks
  TITLE:=Hotplug button script
  MAINTAINER:=Gateworks Corporation <support@gateworks.com>
  URL:=http://trac.gateworks.com/wiki/
endef

define Package/button-hotplug-gw/description
  OpenWrt button events are dispatched through the hotplug system to
  the button subsystem. This script and default config adds a handler
  script that can track button-presses and execute handlers configured
  via uci.
endef

define Build/Configure
endef

define Build/Compile
endef

define Package/button-hotplug-gw/install
	$(INSTALL_DIR) $(1)/etc/hotplug.d/button
	$(INSTALL_BIN) ./files/hotplug $(1)/etc/hotplug.d/button/00-button
	$(INSTALL_BIN) ./files/btn_handlers.sh $(1)/etc/hotplug.d/button/btn_handlers.sh
	$(INSTALL_BIN) ./files/factory_reset $(1)/etc/hotplug.d/button/factory_reset

	$(INSTALL_DIR) $(1)/etc/uci-defaults
	$(INSTALL_BIN) ./files/uci-defaults $(1)/etc/uci-defaults/00-button
endef

$(eval $(call BuildPackage,button-hotplug-gw))
