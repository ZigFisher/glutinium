#
# Copyright (C) 2008-2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=aprx
PKG_REV:=HEAD
PKG_VERSION:=2.8.2-BG5HHP
PKG_RELEASE:=

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:=https://github.com/shawnchain/aprx/trunk
#PKG_SOURCE_URL:=https://github.com/shawnchain/aprx/tags/2.8.2
PKG_SOURCE_SUBDIR:=$(PKG_NAME)-$(PKG_VERSION)
PKG_SOURCE_VERSION:=$(PKG_REV)
PKG_SOURCE_PROTO:=svn

include $(INCLUDE_DIR)/package.mk

define Package/aprx
  SECTION:=net
  CATEGORY:=Network
  TITLE:=APRS RX only I-gate
  URL:=http://ham.zmailer.org/oh2mqk/aprx/
  DEPENDS:=+libpthread
endef

define Package/aprx/description
	This daemon listens for traffic on the specified serial interfaces.
	It then forwards appropriate packets to APRS-IS servers.
endef

CONFIGURE_ARGS += \
	--with-embedded \

define Package/aprx/install
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/etc/init.d/aprx.init $(1)/etc/init.d/aprx
	$(INSTALL_DIR) $(1)/etc
	$(INSTALL_CONF) ./files/etc/aprx.conf $(1)/etc/aprx.conf
	$(INSTALL_CONF) $(PKG_BUILD_DIR)/aprx.conf $(1)/etc/aprx.conf.sample
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/aprx $(1)/usr/sbin/	
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/aprx-stat $(1)/usr/sbin/
	$(INSTALL_BIN) ./files/etc/update-aprs-config $(1)/usr/sbin/	
endef

define Package/aprx/conffiles
/etc/aprx.conf
endef

define Package/aprx/postinst
#!/bin/sh
if [ -e /etc/openwrt_release ]; then
	/etc/init.d/aprx enable || :
fi
endef

$(eval $(call BuildPackage,aprx,+libpthread))
