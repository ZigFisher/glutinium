#
# Copyright (C) 2006-2009 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=etherdump
PKG_VERSION:=2.11
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://psydev.syw4e.info/new/etherdump/
PKG_MD5SUM:=

include $(INCLUDE_DIR)/package.mk

define Package/etherdump
  SECTION:=net
  CATEGORY:=Network
  TITLE:=Etherdump is an extremely small packet sniffer
  URL:=http://freecode.com/projects/etherdump
endef

CONFIGURE_ARGS += \

CONFIGURE_VARS += \
	LIBS="$(TARGET_LDFLAGS)" \

define Build/Compile
	$(MAKE) $(PKG_JOBS) -C $(PKG_BUILD_DIR) \
		$(TARGET_CONFIGURE_OPTS) \
		LD="$(TARGET_CC)"
endef

define Package/etherdump/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/etherdump $(1)/usr/bin/
endef

$(eval $(call BuildPackage,etherdump))
