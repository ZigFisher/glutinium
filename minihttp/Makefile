#
# Copyright (C) 2011-2014 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=minihttp
PKG_VERSION:=2019-11-11
PKG_MAINTAINER:=Chertov Maxim <chertovmv@gmail.com>

PKG_LICENSE:=MIT
PKG_LICENSE_FILES:=LICENSE

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=https://github.com/chertov/hi_minihttp.git
PKG_SOURCE_VERSION:=9ce9da7dcbb5adc3c8b66e2978045ee491651c41

PKG_SOURCE_SUBDIR:=$(PKG_NAME)-$(PKG_VERSION)-$(PKG_SOURCE_VERSION)
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION)-$(PKG_SOURCE_VERSION).tar.gz
PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)-$(PKG_SOURCE_VERSION)
PKG_BUILD_DEPENDS:=hisi-osdrv2

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/cmake.mk

define Package/$(PKG_NAME)
	SECTION:=openipc
	CATEGORY:=OpenIPC
	SUBMENU:=Multimedia
	TITLE:=Best samples for Hisilicon devices
	MAINTAINER:=chertovmv@gmail.com
	DEPENDS:=@(TARGET_hi35xx) +libpthread +hisi-osdrv2-base +hisi-osdrv2-kmod +hisi-osdrv2-lib +libevent2 +lame-lib
endef

define Package/$(PKG_NAME)/description
    Best samples for Hisilicon devices
endef

define Package/$(PKG_NAME)/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/$(PKG_NAME) $(1)/usr/bin/
	$(INSTALL_DIR) $(1)/etc
	#$(CP) $(PKG_BUILD_DIR)/$(PKG_NAME).ini $(1)/etc/
	$(INSTALL_CONF) ./files/$(PKG_NAME).ini $(1)/etc/

endef

$(eval $(call BuildPackage,$(PKG_NAME)))
