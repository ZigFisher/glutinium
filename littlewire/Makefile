#
# Copyright (C) 2006-2014 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=littlewire
PKG_VERSION:=2014-11-30
PKG_RELEASE:=1
PKG_MAINTAINER:=Cezary Jackiewicz <cezary@eko.one.pl>

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=https://github.com/littlewire/Little-Wire.git
PKG_SOURCE_VERSION:=86dcd06a6aaaa4d90657b1ae5976812140be5c61
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION)-$(PKG_SOURCE_VERSION).tar.gz
PKG_SOURCE_SUBDIR:=$(PKG_NAME)-$(PKG_VERSION)
PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)

PKG_LICENSE:=GPL-2.0
PKG_LICENSE_FILES:=LICENSE

include $(INCLUDE_DIR)/package.mk

define Package/littlewire
	SECTION:=utils
	CATEGORY:=Utilities
	TITLE:=Little-Wire components for attiny45 USB Digispark
	MAINTAINER:=http://eko.one.pl
	DEPENDS:=+libusb-compat
endef

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR)/software \
	CC="$(TARGET_CC)" CFLAGS="$(TARGET_CFLAGS) LDFLAGS="$(TARGET_LDFLAGS)" \
	-DLINUX=1" -I$(STAGING_DIR)/usr/include -I$(PKG_BUILD_DIR)/software/library
endef

define Package/littlewire/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/software/{adc,blink,blink_ws2812} $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/software/{button,debugConsole,fade_ws2812,hardwarePWM} $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/software/{i2c_blinkM,i2c_nunchuck,lwbuttond,onewire} $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/software/{rgb_cycle_ws2812,servo,softPWM,spi_LTC1448} $(1)/usr/bin
endef

$(eval $(call BuildPackage,littlewire))

