#
# Copyright (C) 2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=darkice
PKG_VERSION:=1.3
PKG_RELEASE:=5

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=https://sourceforge.net/projects/darkice/files/
PKG_MD5SUM:= a4d3a1007bfd22dfa02c3ca4ae88c63c

PKG_FIXUP:=autoreconf
PKG_INSTALL:=1

include $(INCLUDE_DIR)/package.mk

define Package/$(PKG_NAME)
	SECTION:=multimedia
	CATEGORY:=Multimedia
	SUBMENU:=Streaming
	TITLE:=DarkIce is a live audio streamer
	DEPENDS:=+librt +libpthread +libstdcpp
	MAINTAINER:=Artur Wronowski <arteqw@gmail.com>
endef

define Package/darkice/description
	DarkIce is a live audio streamer. It records audio from an audio interface (e.g. sound card), encodes it and sends it to a streaming server.
endef

CONFIGURE_ARGS += \
	--without-aacplus \
	--without-alsa \
	--without-faac \
	--without-jack \
	--without-lame \
	--without-samplerate \
	--without-twolame \
	--with-vorbis \
	--with-vorbis-prefix="$(STAGING_DIR)/usr/lib" \

define Package/$(PKG_NAME)/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/$(PKG_NAME) $(1)/usr/sbin/
	$(INSTALL_DIR) $(1)/etc
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/etc/darkice.cfg $(1)/etc/
endef

define Package/$(PKG_NAME)/conffiles
	/etc/darkice.cfg
endef

$(eval $(call BuildPackage,$(PKG_NAME)))
