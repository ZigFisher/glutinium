#
# Copyright (C) 2014-2019 OpenIPC.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=hi35xx-rtl8188eu

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=https://github.com/lwfinger/rtl8188eu.git
PKG_REV:=4930848febecd66a1c4b0737954a3c98984e9ea6
PKG_VERSION:=2019-12-29
PKG_RELEASE:=1
PKG_LICENSE:=GPL-2.0
PKG_MAINTAINER:=Igor Zalatov (ZFT Lab.) <flyrouter@gmail.com>

PKG_SOURCE_VERSION:=$(PKG_REV)
PKG_SOURCE_SUBDIR:=$(PKG_NAME)-$(PKG_VERSION)
PKG_SOURCE:=$(PKG_SOURCE_SUBDIR).tar.gz
#PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_SOURCE_SUBDIR)
PKG_BUILD_DIR:=$(KERNEL_BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)


include $(INCLUDE_DIR)/package.mk




















define Package/$(PKG_NAME)/Default
  URL:=https://github.com/lwfinger/rtl8188eu
  MAINTAINER:=lwfinger
endef

define KernelPackage/$(PKG_NAME)
$(call Package/$(PKG_NAME)/Default)
  SUBMENU:=Wireless Drivers
  DEPENDS:=@USB_SUPPORT +@DRIVER_WEXT_SUPPORT +kmod-usb-core
  TITLE:=Driver for Realtek RTL8188EU (for HI35xx)
  FILES:=$(PKG_BUILD_DIR)/$(PKG_NAME).$(LINUX_KMOD_SUFFIX)
  VERSION:=$(LINUX_VERSION)+$(PKG_VERSION)-$(PKG_RELEASE)
  AUTOLOAD:=$(call AutoLoad,21,rtl8188eu)
endef

define KernelPackage/$(PKG_NAME)/description
Driver for Realtek RTL8188EU (for HI35xx)
endef

define Package/$(PKG_NAME)-firmware
$(call Package/$(PKG_NAME)/Default)
  SECTION:=firmware
  CATEGORY:=Firmware
  TITLE:=Firmware for Realtek RTL8188EU (for HI35xx)
endef

define Package/$(PKG_NAME)-firmware/description
Firmware for Realtek RTL8188EU  (for HI35xx)
endef

MAKE_RTL8188EU_ARGS:= \
	ARCH="$(LINUX_KARCH)" \
	CROSS_COMPILE="$(TARGET_CROSS)" \
	SUBDIRS="$(PKG_BUILD_DIR)" \
	KSRC="$(LINUX_DIR)"

define Build/Configure
endef

ifneq ($(DEVELOPER)$(CONFIG_PACKAGE_kmod-rtl8188eu),)
	BUILD_RTL8188EU := $(MAKE) -C "$(PKG_BUILD_DIR)" $(MAKE_RTL8188EU_ARGS) modules
endif

define Build/Compile
	$(BUILD_RTL8188EU)
endef

define Package/$(PKG_NAME)-firmware/install
	$(INSTALL_DIR) $(1)/lib/firmware/rtlwifi
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/firmware/rtl8188eufw.bin $(1)/lib/firmware/rtlwifi
	$(INSTALL_DIR) $(1)/lib/wifi
	$(INSTALL_DATA) ./files/wext.sh $(1)/lib/wifi
	$(INSTALL_DATA) ./files/wpa_supplicant.sh $(1)/lib/wifi
endef

$(eval $(call KernelPackage,$(PKG_NAME)))
$(eval $(call BuildPackage,$(PKG_NAME)-firmware))
