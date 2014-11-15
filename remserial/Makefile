#
# Copyright (C) 2006-2014 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=remserial
PKG_VERSION:=1.4
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://lpccomp.bc.ca/remserial/
PKG_MD5SUM:=

include $(INCLUDE_DIR)/package.mk

define Package/remserial
  SECTION:=net
  CATEGORY:=Network
  TITLE:=Bridge between a TCP/IP network and serial ports
  URL:=http://lpccomp.bc.ca/remserial/
  SUBMENU:=SERIAL
endef

CONFIGURE_ARGS += \

CONFIGURE_VARS += \
	LIBS="$(TARGET_LDFLAGS)" \

define Build/Compile
	$(call Build/Compile/Default,remserial)
endef

define Package/remserial/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/remserial $(1)/usr/sbin/
endef

$(eval $(call BuildPackage,remserial))
