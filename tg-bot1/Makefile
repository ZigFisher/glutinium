#
# Copyright (C) 2006-2017 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=tg-bot1
PKG_VERSION:=2017-11-26
PKG_RELEASE:=4
PKG_LICENSE:=GPL-2.0

include $(INCLUDE_DIR)/package.mk

define Package/tg-bot1
	SECTION:=net
	CATEGORY:=Network
	MAINTAINER:=Zig Fisher <flyrouter@gmail.com>
	TITLE:=TG-Bot1 - simple Telegram bot for OpenWrt based devices
	URL:=https://github.com/ZigFisher/Glutinium/tree/master/tg-bot1
	DEPENDS:=+libcurl
endef

define Package/tg-bot1/description
	TG-Bot1 - simple Telegram bot for OpenWrt based devices
endef

define Build/Configure
endef

define Build/Compile
endef

define Package/tg-bot1/install
	$(INSTALL_DIR)  $(1)/etc/config
	$(INSTALL_BIN)  ./files/etc/config/telegram $(1)/etc/config/
	$(INSTALL_DIR)  $(1)/etc/init.d
	$(INSTALL_BIN)  ./files/etc/init.d/telegram $(1)/etc/init.d/
	$(INSTALL_DIR)  $(1)/usr/bin
	$(INSTALL_BIN)  ./files/usr/bin/tg_relay.sh $(1)/usr/bin/
	$(INSTALL_BIN)  ./files/usr/bin/tg_reboot.sh $(1)/usr/bin/
	$(INSTALL_DIR)  $(1)/usr/sbin
	$(INSTALL_BIN)  ./files/usr/sbin/telegram_bot $(1)/usr/sbin/
endef

$(eval $(call BuildPackage,tg-bot1))



