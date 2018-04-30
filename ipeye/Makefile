#
# Copyright (C) 2006-2015 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=ipeye
PKG_VERSION:=2018-04-30
PKG_RELEASE:=1
PKG_MAINTAINER:=Igor Zalatov <flyrouter@gmail.com>

PKG_LICENSE:=GPL-2.0
PKG_LICENSE_FILES:=LICENSE

include $(INCLUDE_DIR)/package.mk

define Package/$(PKG_NAME)
	SECTION:=multimedia
	CATEGORY:=Multimedia
	TITLE:=IPEYE cloud client for integrate to IPCam's
	MAINTAINER:=http://ipeye.ru/firmware/client/source/
	DEPENDS:=+libpthread
endef

define Package/$(PKG_NAME)/description
	IPEYE cloud client for integrate to IPCam's
endef

define Build/Prepare
	$(INSTALL_DIR) $(PKG_BUILD_DIR)
	$(INSTALL_DATA) ./src/* $(PKG_BUILD_DIR)/
endef

define Build/Compile
	$(TARGET_CC) $(TARGET_CFLAGS) $(TARGET_LDFLAGS) -Wall -lpthread -O -g -D LINUX  \
		-o $(PKG_BUILD_DIR)/ipeye $(PKG_BUILD_DIR)/ipeye.c
endef

define Package/$(PKG_NAME)/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/ipeye $(1)/usr/sbin/
endef

$(eval $(call BuildPackage,$(PKG_NAME)))

 

