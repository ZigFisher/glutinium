#
# Copyright (C) 2014-2019 OpenIPC.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk


PKG_NAME:=hi35xx-rtl8188fu
PKG_VERSION:=2020-01-01
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=https://github.com/kelebek333/rtl8188fu.git
PKG_SOURCE_VERSION:=a60758937803680454ed21945216354b25e5f6d5
#PKG_SOURCE_URL:=https://github.com/ulli-kroll/rtl8188fu.git
#PKG_SOURCE_VERSION:=fab4f5db4580d9a920f99c64dceaa5711589559d
#PKG_SOURCE_URL:=https://github.com/lutmm/rtl8188fu.git
#PKG_SOURCE_VERSION:=75e3f56d61ba713ce9ee9e0aadf00523e90c2d85
PKG_SOURCE_SUBDIR:=$(PKG_NAME)-$(PKG_VERSION)

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_SOURCE_SUBDIR)
#PKG_BUILD_PARALLEL:=1

PKG_MAINTAINER:=Igor Zalatov (ZFT Lab.) <flyrouter@gmail.com>
PKG_LICENSE:=GPL-2.0
PKG_LICENSE_FILES:=LICENSE


include $(INCLUDE_DIR)/package.mk

define Package/$(PKG_NAME)/Default
	URL:=https://github.com/ulli-kroll/rtl8188fu
	MAINTAINER:=Hans Ulli Kroll
endef

define KernelPackage/$(PKG_NAME)
$(call Package/$(PKG_NAME)/Default)
	SUBMENU:=Wireless Drivers
	DEPENDS:=@USB_SUPPORT +@DRIVER_WEXT_SUPPORT +kmod-usb-core +wireless-tools +hostapd-common-old
	TITLE:=Driver for RTL8188FU wireless adapters (HI35xx)
	FILES:=$(PKG_BUILD_DIR)/$(PKG_NAME).$(LINUX_KMOD_SUFFIX)
	VERSION:=$(LINUX_VERSION)+$(PKG_VERSION)-$(PKG_RELEASE)
	AUTOLOAD:=$(call AutoLoad,21,$(PKG_NAME))
endef

define KernelPackage/$(PKG_NAME)/description
	Driver for RTL8188FU wireless adapters (HI35xx)
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
	$(MAKE) -C $(PKG_BUILD_DIR) LINUX_DIR=$(LINUX_DIR) \
		$(MAKE_OPTS) modules
	$(CP) $(PKG_BUILD_DIR)/rtl8188fu.$(LINUX_KMOD_SUFFIX) \
		$(PKG_BUILD_DIR)/$(PKG_NAME).$(LINUX_KMOD_SUFFIX)
endef

define Package/$(PKG_NAME)-firmware/install
	$(INSTALL_DIR) $(1)/lib/firmware/rtlwifi
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/firmware/rtl8188fufw.bin $(1)/lib/firmware/rtlwifi
	$(INSTALL_DIR) $(1)/lib/wifi
	$(INSTALL_DATA) ./files/wext.sh $(1)/lib/wifi
endef

$(eval $(call KernelPackage,$(PKG_NAME)))
$(eval $(call BuildPackage,$(PKG_NAME)-firmware))
