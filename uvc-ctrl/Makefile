#
# Copyright (C) 2016 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=uvc-ctrl
PKG_RELEASE:=1

PKG_BUILD_DIR := $(BUILD_DIR)/$(PKG_NAME)

include $(INCLUDE_DIR)/package.mk

define Package/uvc-ctrl
	SECTION:=multimedia
	CATEGORY:=Multimedia
	TITLE:=Utility for set and display uvc camera control option
	URL:=https://github.com/allanbian1017/uvc-ctrl
	DEPENDS:=+libv4l
endef

define Package/uvc-ctrl/description
	Utility for set and display uvc camera control option.
endef

define Build/Prepare
	$(INSTALL_DIR) $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)/
endef

define Build/Compile
	$(TARGET_CC) $(TARGET_CFLAGS) -Os $(PKG_BUILD_DIR)/uvc-ctrl.c -o $(PKG_BUILD_DIR)/$(PKG_NAME)
endef

define Package/uvc-ctrl/install
	$(INSTALL_DIR) $(1)/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/uvc-ctrl $(1)/sbin/
endef

$(eval $(call BuildPackage,uvc-ctrl))
