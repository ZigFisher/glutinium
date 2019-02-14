#
# Copyright (C) 2011-2014 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=estgb
PKG_VERSION:=1.2.5
PKG_RELEASE:=2019-02-11
PKG_MAINTAINER:=Igor Zalatov <flyrouter@gmail.com>

include $(INCLUDE_DIR)/package.mk

PKG_LICENSE:=GPL-2.0
PKG_LICENSE_FILES:=LICENSE
PKG_BUILD_DEPENDS:=libcurl libjson-c

define Package/$(PKG_NAME)
	SECTION:=utils
	CATEGORY:=Utilities
	TITLE:=Simply Sender TeleGram Bot based on telebot
	MAINTAINER:=Denis G Dugushkin <dentall@mail.ru>
	DEPENDS:=+libcurl +libjson-c
endef

define Package/$(PKG_NAME)/description
  Simply Sender TeleGram Bot based on telebot
endef

define Build/Prepare
	$(INSTALL_DIR) $(PKG_BUILD_DIR)
	$(INSTALL_DATA) ./src/* $(PKG_BUILD_DIR)/
endef

define Build/Compile
	$(TARGET_CC) $(TARGET_CFLAGS) $(TARGET_LDFLAGS) -Wall -ljson-c -lcurl \
		-I$(PKG_BUILD_DIR) -I$(STAGING_DIR)/usr/include/json-c -I$(PKG_BUILD_DIR) -I$(STAGING_DIR)/usr/include/curl \
		-L$(STAGING_DIR)/usr/lib -o $(PKG_BUILD_DIR)/$(PKG_NAME) $(PKG_BUILD_DIR)/$(PKG_NAME).c \
		$(PKG_BUILD_DIR)/telebot.c $(PKG_BUILD_DIR)/telebot-core.c $(PKG_BUILD_DIR)/telebot-parser.c
endef

define Package/$(PKG_NAME)/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/$(PKG_NAME) $(1)/usr/bin/
endef

$(eval $(call BuildPackage,$(PKG_NAME)))
