#
# Copyright (C) 2007-2014 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=hi35xx-mt7601u

PKG_VERSION:=3.0.0.4_20130913
PKG_RELEASE:=1
PKG_SOURCE_URL:=http://flyrouter.net/downloads/software/ipcam/archive/
PKG_MD5SUM:=5f440dccc8bc952745a191994fc34699

PKG_SOURCE:=DPO_MT7601U_LinuxSTA_$(PKG_VERSION).tar.bz2
PKG_BUILD_DIR:=$(KERNEL_BUILD_DIR)/DPO_MT7601U_LinuxSTA_$(PKG_VERSION)
PKG_BUILD_PARALLEL:=1

PKG_USE_MIPS16:=0

PKG_MAINTAINER:=Luca Olivetti <luca@ventoso.org>
PKG_LICENSE:=GPL-2.0+

include $(INCLUDE_DIR)/package.mk

WMENU:=Wireless Drivers

define KernelPackage/$(PKG_NAME)
	SUBMENU:=Wireless Drivers
	TITLE:=Driver for MT7601U wireless adapters (HI35xx)
	FILES:=$(PKG_BUILD_DIR)/os/linux/mt7601Usta.$(LINUX_KMOD_SUFFIX)
	#DEPENDS:=+wireless-tools +hostapd-common-old @USB_SUPPORT
	DEPENDS:=@USB_SUPPORT +@DRIVER_WEXT_SUPPORT +kmod-usb-core
	AUTOLOAD:=$(call AutoProbe,mt7601Usta)
	MENU:=1
endef

define KernelPackage/$(PKG_NAME)/config
  if PACKAGE_kmod-$(PKG_NAME)
	config PACKAGE_MT7601_DEBUG
		bool "Enable debug messages"
		default n
		help
		  With this option the driver will emit A LOT
		  of debugging information. 
		  The driver is quite noisy, so enable it only if
		  absolutely necessary to debug problems with this
		  driver.
  endif
endef

define KernelPackage/$(PKG_NAME)/description
  Driver for MT7601U wireless adapters (HI35xx)
endef

ifneq ($(CONFIG_BIG_ENDIAN),)
  ENDIANOPTS:=-DRT_BIG_ENDIAN
else
  ENDIANOPTS:=
endif

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) LINUX_DIR=$(LINUX_DIR) KERNEL_CROSS=$(KERNEL_CROSS) ARCH=$(LINUX_KARCH) ENDIANOPTS=$(ENDIANOPTS) RTDEBUG=$(CONFIG_PACKAGE_MT7601_DEBUG)
endef

define KernelPackage/$(PKG_NAME)/install
	$(INSTALL_DIR) $(1)/etc/Wireless/RT2870STA/
	$(INSTALL_DIR) $(1)/lib/modules/$(LINUX_VERSION)
	$(INSTALL_DIR) $(1)/lib/wifi
	$(CP) $(PKG_BUILD_DIR)/RT2870STA.dat $(1)/etc/Wireless/RT2870STA/
	$(INSTALL_DATA) ./files/wext.sh $(1)/lib/wifi
	$(INSTALL_DATA) ./files/wpa_supplicant.sh $(1)/lib/wifi
endef

$(eval $(call KernelPackage,$(PKG_NAME)))
