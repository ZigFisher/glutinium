#
# Copyright (C) 2011-2014 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=prometheus-node-exporter-lua
PKG_VERSION:=2020.07.04
PKG_RELEASE:=1
PKG_MAINTAINER:=Etienne CHAMPETIER <champetier.etienne@gmail.com>

include $(INCLUDE_DIR)/package.mk

PKG_LICENSE:=Apache-2.0
PKG_LICENSE_FILES:=LICENSE

define Package/$(PKG_NAME)
	SECTION:=utils
	CATEGORY:=Utilities
	TITLE:=Prometheus node exporter
	DEPENDS:=+luasocket +lua +libuci-lua
endef

define Package/$(PKG_NAME)/description
  Provides node metrics as Prometheus scraping endpoint.

  This service is a lightweight rewrite in LUA of the offical Prometheus node_exporter.
endef

define Build/Prepare
endef

define Build/Configure
endef

define Build/Compile
endef

define Package/$(PKG_NAME)/install
	$(INSTALL_DIR) $(1)/etc/config
	$(INSTALL_CONF) ./files/etc/config/prometheus-node-exporter-lua $(1)/etc/config/prometheus-node-exporter-lua
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/etc/init.d/prometheus-node-exporter-lua $(1)/etc/init.d/prometheus-node-exporter-lua
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) ./files/usr/bin/prometheus-node-exporter-lua $(1)/usr/bin/prometheus-node-exporter-lua
	$(INSTALL_DIR) $(1)/usr/lib/lua/prometheus-collectors
	$(INSTALL_BIN) ./files/usr/lib/lua/prometheus-collectors/cpu.lua              $(1)/usr/lib/lua/prometheus-collectors/
	$(INSTALL_BIN) ./files/usr/lib/lua/prometheus-collectors/filefd.lua           $(1)/usr/lib/lua/prometheus-collectors/
	$(INSTALL_BIN) ./files/usr/lib/lua/prometheus-collectors/loadavg.lua          $(1)/usr/lib/lua/prometheus-collectors/
	$(INSTALL_BIN) ./files/usr/lib/lua/prometheus-collectors/meminfo.lua          $(1)/usr/lib/lua/prometheus-collectors/
	$(INSTALL_BIN) ./files/usr/lib/lua/prometheus-collectors/netdev.lua           $(1)/usr/lib/lua/prometheus-collectors/
	$(INSTALL_BIN) ./files/usr/lib/lua/prometheus-collectors/openwrt.lua          $(1)/usr/lib/lua/prometheus-collectors/
	$(INSTALL_BIN) ./files/usr/lib/lua/prometheus-collectors/time.lua             $(1)/usr/lib/lua/prometheus-collectors/
	$(INSTALL_BIN) ./files/usr/lib/lua/prometheus-collectors/uci_dhcp_host.lua    $(1)/usr/lib/lua/prometheus-collectors/
	$(INSTALL_BIN) ./files/usr/lib/lua/prometheus-collectors/uname.lua            $(1)/usr/lib/lua/prometheus-collectors/
endef

$(eval $(call BuildPackage,$(PKG_NAME)))
