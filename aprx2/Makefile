#
# Copyright (C) 2008-2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=aprx
PKG_VERSION:=2017-09-18
PKG_MAINTAINER:=Igor Zalatov <flyrouter@gmail.com>

PKG_LICENSE:=MIT
PKG_LICENSE_FILES:=LICENSE

PKG_SOURCE_PROTO:=git
#PKG_SOURCE_URL:=https://github.com/shawnchain/aprx.git
PKG_SOURCE_URL:=https://github.com/PhirePhly/aprx.git
PKG_SOURCE_VERSION:=13c6e7950210c501f71a0d12bb5c780f37e21b9b

PKG_SOURCE_SUBDIR:=$(PKG_NAME)-$(PKG_VERSION)-$(PKG_SOURCE_VERSION)
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION)-$(PKG_SOURCE_VERSION).tar.gz
PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)-$(PKG_SOURCE_VERSION)

include $(INCLUDE_DIR)/package.mk

define Package/aprx
  SECTION:=net
  CATEGORY:=Network
  TITLE:=APRS RX only I-gate
  URL:=http://ham.zmailer.org/oh2mqk/aprx/
endef

define Package/aprx/description
	This daemon listens for traffic on the specified serial interfaces.
	It then forwards appropriate packets to APRS-IS servers.
	Google Group: https://groups.google.com/forum/#!forum/aprx-software
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
endef

define Package/aprx/conffiles
/etc/aprx.conf
endef

$(eval $(call BuildPackage,aprx))
