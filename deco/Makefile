#
# Copyright (C) 2011-2014 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=deco
PKG_VERSION:=39
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)$(PKG_VERSION).tgz
PKG_SOURCE_URL:=@SF/deco
PKG_MD5SUM:=f77f60e8be0cae1f814cba1ef61bf4d0
PKG_CAT:=zcat
PKG_MAINTAINER:=Igor Zalatov (ZFT Lab.) <flyrouter@gmail.com>

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)$(PKG_VERSION)

include $(INCLUDE_DIR)/package.mk

define Package/$(PKG_NAME)
  SECTION:=utils
  CATEGORY:=Utilities
  DEPENDS:=+libncurses
  TITLE:=Text-based, full featured file manager
  URL:=http://deco.sourceforge.net/
endef

define Package/$(PKG_NAME)/description
    A clone of Norton Commander for Unix. Text-based, full featured 
    file manager.
endef
  
define Build/Compile
    $(MAKE) -C $(PKG_BUILD_DIR)
endef

define Package/$(PKG_NAME)/install
    $(INSTALL_DIR) $(1)/usr/bin
    $(INSTALL_BIN) $(PKG_BUILD_DIR)/$(PKG_NAME) $(1)/usr/bin/
    $(INSTALL_DIR) $(1)/usr/lib/deco
    $(INSTALL_DATA) $(PKG_BUILD_DIR)/menu $(1)/usr/lib/$(PKG_NAME)/menu
    $(INSTALL_DATA) $(PKG_BUILD_DIR)/profile $(1)/usr/lib/$(PKG_NAME)/profile
endef

$(eval $(call BuildPackage,$(PKG_NAME)))
