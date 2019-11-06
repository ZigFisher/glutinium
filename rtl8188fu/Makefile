#
# Copyright (C) 2014-2019 OpenIPC.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=rtl8188fu

PKG_SOURCE_PROTO:=git
#PKG_SOURCE_URL:=https://github.com/ulli-kroll/rtl8188fu.git
#PKG_REV:=fab4f5db4580d9a920f99c64dceaa5711589559d
PKG_SOURCE_URL:=https://github.com/kelebek333/rtl8188fu.git
PKG_REV:=a60758937803680454ed21945216354b25e5f6d5
#PKG_SOURCE_URL:=https://github.com/lutmm/rtl8188fu.git
#PKG_REV:=75e3f56d61ba713ce9ee9e0aadf00523e90c2d85
PKG_VERSION:=2019-10-05
PKG_RELEASE:=0.3
PKG_LICENSE:=GPL-2.0
PKG_MAINTAINER:=Igor Zalatov (ZFT Lab.) <flyrouter@gmail.com>

PKG_SOURCE_VERSION:=$(PKG_REV)
PKG_SOURCE_SUBDIR:=$(PKG_NAME)-$(PKG_VERSION)
PKG_SOURCE:=$(PKG_SOURCE_SUBDIR).tar.gz
PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_SOURCE_SUBDIR)


include $(INCLUDE_DIR)/package.mk

define Package/$(PKG_NAME)/Default
  URL:=https://github.com/ulli-kroll/rtl8188fu
  MAINTAINER:=Hans Ulli Kroll
endef

define KernelPackage/$(PKG_NAME)
$(call Package/$(PKG_NAME)/Default)
  SUBMENU:=Wireless Drivers
  DEPENDS:=@USB_SUPPORT +@DRIVER_WEXT_SUPPORT +kmod-usb-core
  TITLE:=Driver for Realtek RTL8188FU
  FILES:=$(PKG_BUILD_DIR)/$(PKG_NAME).$(LINUX_KMOD_SUFFIX)
  VERSION:=$(LINUX_VERSION)+$(PKG_VERSION)-$(PKG_RELEASE)
  AUTOLOAD:=$(call AutoLoad,21,rtl8188fu)
endef

define KernelPackage/$(PKG_NAME)/description
Driver for Realtek RTL8188FU
endef

define Package/$(PKG_NAME)-firmware
$(call Package/$(PKG_NAME)/Default)
  SECTION:=firmware
  CATEGORY:=Firmware
  TITLE:=Firmware for Realtek RTL8188FU
endef

define Package/$(PKG_NAME)-firmware/description
Firmware for Realtek RTL8188FU
endef

MAKE_RTL8188FU_ARGS:= \
	ARCH="$(LINUX_KARCH)" \
	CROSS_COMPILE="$(TARGET_CROSS)" \
	SUBDIRS="$(PKG_BUILD_DIR)" \
	KSRC="$(LINUX_DIR)"

define Build/Configure
endef

ifneq ($(DEVELOPER)$(CONFIG_PACKAGE_kmod-rtl8188fu),)
	BUILD_RTL8188FU := $(MAKE) -C "$(PKG_BUILD_DIR)" $(MAKE_RTL8188FU_ARGS) modules
endif

define Build/Compile
	$(BUILD_RTL8188FU)
endef

define Package/$(PKG_NAME)-firmware/install
	$(INSTALL_DIR) $(1)/lib/firmware/rtlwifi
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/firmware/rtl8188fufw.bin $(1)/lib/firmware/rtlwifi
	$(INSTALL_DIR) $(1)/lib/wifi
	$(INSTALL_DATA) ./files/wext.sh $(1)/lib/wifi
	$(INSTALL_DATA) ./files/wpa_supplicant.sh $(1)/lib/wifi
endef

$(eval $(call KernelPackage,$(PKG_NAME)))
$(eval $(call BuildPackage,$(PKG_NAME)-firmware))
