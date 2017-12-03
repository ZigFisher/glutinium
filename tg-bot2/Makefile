#
# Copyright (C) 2006-2015 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=tg-bot2
PKG_VERSION:=2015-08-07
PKG_RELEASE:=1
PKG_MAINTAINER:=shinmai-rookie

PKG_LICENSE:=GPL-2.0
PKG_LICENSE_FILES:=LICENSE

include $(INCLUDE_DIR)/package.mk

define Package/tg-bot2
	SECTION:=net
	CATEGORY:=Network
	TITLE:=TG-Bot2 - simple Telegram bot for OpenWrt based devices
	MAINTAINER:=https://github.com/shinmai-rookie/rous-mola-bot-telegram
	DEPENDS:=+libcurl
endef

define Package/$(PKG_NAME)/description
	TG-Bot2 - simple Telegram bot for OpenWrt based devices
endef

define Package/$(PKG_NAME)/Build/Prepare
	$(INSTALL_DIR) $(PKG_BUILD_DIR)
	$(INSTALL_DATA) ./src/* $(PKG_BUILD_DIR)/
endef

define Package/$(PKG_NAME)/Build/Compile
	$(TARGET_CC) $(TARGET_CFLAGS) $(TARGET_LDFLAGS) -O -g -D LINUX -lcurl \
		-o $(PKG_BUILD_DIR)/tg-bot2 $(PKG_BUILD_DIR)/rousbot.c $(PKG_BUILD_DIR)/message.c
	#CFLAGS=-ansi -Wall -Wextra -pedantic -Wno-unused-parameter `curl-config --cflags`
endef

define Package/$(PKG_NAME)/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/tg-bot2 $(1)/usr/bin/
endef

$(eval $(call BuildPackage,tg-bot2))
