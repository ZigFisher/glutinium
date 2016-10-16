#
# Copyright (C) 2006-2015 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=pzem004t
PKG_VERSION:=2016-10-14
PKG_RELEASE:=1
PKG_MAINTAINER:=Armand Huqi

PKG_LICENSE:=GPL-2.0
PKG_LICENSE_FILES:=LICENSE

include $(INCLUDE_DIR)/package.mk

define Package/pzem004t
	SECTION:=utils
	CATEGORY:=Utilities
	TITLE:=A simple util to read/write data PZEM-004T
	MAINTAINER:=https://github.com/alabamas/PZEM-004T
	DEPENDS:=+libcurl
endef

define Package/pzem004t/description
	A simple util to read/write data PZEM-004T
endef

define Build/Prepare
	$(INSTALL_DIR) $(PKG_BUILD_DIR)
	$(INSTALL_DATA) ./src/* $(PKG_BUILD_DIR)/
endef

define Build/Compile
	$(TARGET_CC) $(TARGET_CFLAGS) $(TARGET_LDFLAGS) -lcurl -O -g -D LINUX \
		-o $(PKG_BUILD_DIR)/pzem004t $(PKG_BUILD_DIR)/pzem004t.c
endef

define Package/pzem004/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/pzem004t $(1)/usr/bin/
endef

$(eval $(call BuildPackage,pzem004t))
