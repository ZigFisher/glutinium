#
# Copyright (C) 2006-2014 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=micronucleus
PKG_VERSION:=2015-07-29
PKG_RELEASE:=1
PKG_MAINTAINER:=Igor Zalatov <flyrouter@gmail.com>

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=https://github.com/micronucleus/micronucleus.git
PKG_SOURCE_VERSION:=0fb6c10fa92b3bc3b5e217f1369d7325faba145c
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION)-$(PKG_SOURCE_VERSION).tar.gz
PKG_SOURCE_SUBDIR:=$(PKG_NAME)-$(PKG_VERSION)-$(PKG_SOURCE_VERSION)
PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)-$(PKG_SOURCE_VERSION)

PKG_LICENSE:=GPL-2.0
PKG_LICENSE_FILES:=LICENSE

include $(INCLUDE_DIR)/package.mk

define Package/micronucleus
	SECTION:=utils
	CATEGORY:=Utilities
	TITLE:=Micronucleus - ATTiny usb bootloader
	MAINTAINER:=https://github.com/micronucleus/micronucleus
	DEPENDS:=+libusb-compat
endef

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR)/commandline \
	CC="$(TARGET_CC)" \
	CFLAGS="$(TARGET_CFLAGS) -I$(STAGING_DIR)/usr/include -I$(PKG_BUILD_DIR)/commandline/library -DLINUX=1" \
	LDFLAGS="$(TARGET_LDFLAGS)"
endef

define Package/micronucleus/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/commandline/{micronucleus} $(1)/usr/bin
endef

$(eval $(call BuildPackage,micronucleus))

