#
# Copyright (C) 2006-2014 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=vtun
PKG_VERSION:=3.0.2
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=@SF/vtun
PKG_MD5SUM:=d3d8bc4d58886498a1c338670eab9315

include $(INCLUDE_DIR)/package.mk

define Package/vtun-lite
  SECTION:=net
  CATEGORY:=Network
  DEPENDS:=+kmod-tun
  TITLE:=VPN tunneling daemon (no ssl/lzo/zlib)
  URL:=http://vtun.sourceforge.net
  SUBMENU:=VPN
endef

define Package/vtun-lite/conffiles
	/etc/vtund.conf
endef

CONFIGURE_ARGS += \
	--disable-lzo \
	--disable-zlib \
	--disable-ssl \

CONFIGURE_VARS += \
	LIBS="$(TARGET_LDFLAGS)" \

define Build/Compile
	$(call Build/Compile/Default,vtund)
endef

define Package/vtun-lite/install
	$(INSTALL_DIR) $(1)/etc
	$(INSTALL_CONF) ./files/vtund.conf $(1)/etc/
	$(INSTALL_CONF) ./files/vtund.dat $(1)/etc/
	$(INSTALL_BIN) ./files/vtund.sh $(1)/etc/
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/vtund $(1)/usr/sbin/
endef

$(eval $(call BuildPackage,vtun-lite))
