#
# Copyright (C) 2007-2014 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=mt7601u-hisi

PKG_VERSION:=3.0.0.4_20130913
PKG_RELEASE:=1
##PKG_SOURCE_URL:=http://www.mediatek.com/AmazonS3/Downloads/linux/
#PKG_SOURCE_URL:=http://cdn-cw.mediatek.com/Downloads/linux/
#PKG_MD5SUM:=5f440dccc8bc952745a191994fc34699

#PKG_SOURCE:=DPO_MT7601U_LinuxSTA_$(PKG_VERSION).tar.bz2
PKG_BUILD_DIR:=$(KERNEL_BUILD_DIR)/DPO_MT7601U_LinuxSTA_$(PKG_VERSION)
PKG_BUILD_PARALLEL:=1

PKG_MAINTAINER:=Luca Olivetti <luca@ventoso.org>
PKG_LICENSE:=GPL-2.0+

include $(INCLUDE_DIR)/package.mk

WMENU:=Wireless Drivers

define KernelPackage/mt7601u-hisi
	SUBMENU:=Wireless Drivers
	TITLE:=Driver for MT7601U wireless adapters
	FILES:=$(PKG_BUILD_DIR)/os/linux/mt7601Usta.$(LINUX_KMOD_SUFFIX)
#	DEPENDS:=+wireless-tools +hostapd-common-old @USB_SUPPORT
	AUTOLOAD:=$(call AutoProbe,mt7601Usta)
	MENU:=1
endef

define KernelPackage/mt7601u-hisi/config
  if PACKAGE_kmod-mt7601u-hisi
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

define KernelPackage/mt7601u-hisi/description
  This package contains a driver for usb wireless adapters based on the mediatek MT7601U
endef


MAKE_OPTS:= \
	ARCH="$(LINUX_KARCH)" \
	CROSS_COMPILE="$(TARGET_CROSS)" \
	SUBDIRS="$(PKG_BUILD_DIR)" \
	EXTRA_CFLAGS="$(EXTRA_CFLAGS)" \
	$(EXTRA_KCONFIG)

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)/
endef


MT7601U_CHIPSET = 7601U
MT7601U_MODULE = $(word 1, $(MT7601U_CHIPSET))

MT7601U_MAKE_OPTS += CROSS_COMPILE=$(TARGET_CROSS)
MT7601U_MAKE_OPTS += PLATFORM=HI3518C
MT7601U_MAKE_OPTS += CHIPSET=$(MT7601U_CHIPSET)
MT7601U_MAKE_OPTS += LINUX_SRC=$(LINUX_DIR)
MT7601U_MAKE_OPTS += TARGET=LINUX
MT7601U_MAKE_OPTS += MODULE=$(MT7601U_MODULE)
MT7601U_MAKE_OPTS += OSABL=NO
MT7601U_MAKE_OPTS += RT28xx_MODE=STA
MT7601U_MAKE_OPTS += RELEASE=DPO


define Build/Compile
#	$(MAKE) -C "$(LINUX_DIR)" \
#	$(MAKE_OPTS) \
#	RTDEBUG=$(CONFIG_PACKAGE_MT7601_DEBUG)
#	modules
	$(MAKE) $(MT7601U_MAKE_OPTS) -C $(PKG_BUILD_DIR) LINUX_DIR=$(LINUX_DIR) KERNEL_CROSS=$(KERNEL_CROSS) ARCH=$(LINUX_KARCH) RTDEBUG=$(CONFIG_PACKAGE_MT7601_DEBUG) -Wunused-variable
endef

define KernelPackage/mt7601u-hisi/install
	$(INSTALL_DIR) $(1)/etc/Wireless/RT2870STA/
	$(INSTALL_DIR) $(1)/lib/modules/$(LINUX_VERSION)
	$(INSTALL_DIR) $(1)/lib/wifi
	$(CP) $(PKG_BUILD_DIR)/RT2870STA.dat $(1)/etc/Wireless/RT2870STA/
	$(INSTALL_DATA) ./files/wext.sh $(1)/lib/wifi
	$(INSTALL_DATA) ./files/wpa_supplicant.sh $(1)/lib/wifi

endef

$(eval $(call KernelPackage,mt7601u-hisi))
