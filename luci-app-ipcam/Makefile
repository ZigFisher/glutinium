#
# Copyright (C) 2011-2014 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=luci-app-ipcam
PKG_VERSION:=2020-09-23
PKG_RELEASE:=0.1
PKG_MAINTAINER:=Igor Zalatov (ZFT Lab.) <flyrouter@gmail.com>

include $(INCLUDE_DIR)/package.mk

PKG_LICENSE:=GPL-2.0
PKG_LICENSE_FILES:=LICENSE
PKG_BUILD_DEPENDS:=busybox

define Package/$(PKG_NAME)
	SECTION:=openipc
	CATEGORY:=OpenIPC
	SUBMENU:=Other
	TITLE:=Minimalistic extension from the OpenIPC team
	MAINTAINER:=Igor Zalatov (ZFT Lab.) <flyrouter@gmail.com>
	URL:=http://alpha.openipc.org/firmware
	DEPENDS:=+busybox
endef

define Package/$(PKG_NAME)/description
  Minimalistic extension from the OpenIPC team
endef

define Build/Prepare
endef

define Build/Compile
endef

define Package/$(PKG_NAME)/install
	$(INSTALL_DIR) $(1)/usr/lib/lua/luci
	$(CP) ./luasrc/* $(1)/usr/lib/lua/luci/
endef

$(eval $(call BuildPackage,$(PKG_NAME)))
