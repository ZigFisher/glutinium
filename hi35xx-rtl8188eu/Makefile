#
# Copyright (C) 2014 Bruno Randolf <br1@einfach.org>
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=hi35xx-rtl8188eu
PKG_VERSION:=2019-12-29
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=https://github.com/lwfinger/rtl8188eu.git
PKG_SOURCE_SUBDIR:=$(PKG_NAME)-$(PKG_VERSION)
PKG_SOURCE_VERSION:=4930848febecd66a1c4b0737954a3c98984e9ea6

PKG_MAINTAINER:=Igor Zalatov (ZFT Lab.) <flyrouter@gmail.com>
PKG_LICENSE:=GPL-2.0
PKG_LICENSE_FILES:=LICENSE

include $(INCLUDE_DIR)/package.mk

define Package/$(PKG_NAME)/Default
	URL:=https://github.com/lwfinger/rtl8188eu
	MAINTAINER:=lwfinger
endef

define KernelPackage/$(PKG_NAME)
$(call Package/$(PKG_NAME)/Default)
	SUBMENU:=Wireless Drivers
	DEPENDS:=@USB_SUPPORT +@DRIVER_WEXT_SUPPORT +kmod-usb-core
	TITLE:=Driver for RTL8188EU wireless adapters (HI35xx)
	FILES:=$(PKG_BUILD_DIR)/$(PKG_NAME).$(LINUX_KMOD_SUFFIX)
	VERSION:=$(LINUX_VERSION)+$(PKG_VERSION)-$(PKG_RELEASE)
	AUTOLOAD:=$(call AutoLoad,21,$(PKG_NAME))
endef

define KernelPackage/$(PKG_NAME)/description
	Driver for RTL8188EU wireless adapters (HI35xx)
endef

define Package/$(PKG_NAME)-firmware
$(call Package/$(PKG_NAME)/Default)
	SECTION:=firmware
	CATEGORY:=Firmware
	TITLE:=Firmware for RTL8188EU adapters (HI35xx)
endef

define Package/$(PKG_NAME)-firmware/description
	Firmware for RTL8188EU adapters (HI35xx)
endef

MAKE_OPTS:= \
	ARCH="$(LINUX_KARCH)" \
	CROSS_COMPILE="$(TARGET_CROSS)" \
	SUBDIRS="$(PKG_BUILD_DIR)" \
	KSRC="$(LINUX_DIR)"

define Build/Configure
endef

define Build/Compile
	$(MAKE) -C "$(LINUX_DIR)" \
		$(MAKE_OPTS) \
		modules
	$(CP) $(PKG_BUILD_DIR)/8188eu.$(LINUX_KMOD_SUFFIX) \
		$(PKG_BUILD_DIR)/$(PKG_NAME).$(LINUX_KMOD_SUFFIX)
endef

define Package/$(PKG_NAME)-firmware/install
	$(INSTALL_DIR) $(1)/lib/firmware/rtlwifi
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/rtl8188eufw.bin $(1)/lib/firmware/rtlwifi
	$(INSTALL_DIR) $(1)/lib/wifi
	$(INSTALL_DATA) ./files/wext.sh $(1)/lib/wifi
	$(INSTALL_DATA) ./files/wpa_supplicant.sh $(1)/lib/wifi
endef

$(eval $(call KernelPackage,$(PKG_NAME)))
$(eval $(call BuildPackage,$(PKG_NAME)-firmware))
