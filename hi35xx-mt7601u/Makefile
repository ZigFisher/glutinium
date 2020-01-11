#
# Copyright (C) 2014-2019 OpenIPC.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk


PKG_NAME:=hi35xx-mt7601u
PKG_VERSION:=3.0.0.4_20130913
PKG_RELEASE:=1

PKG_SOURCE:=DPO_MT7601U_LinuxSTA_$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:=http://flyrouter.net/downloads/software/ipcam/archive/
PKG_MD5SUM:=5f440dccc8bc952745a191994fc34699
PKG_SOURCE_SUBDIR:=DPO_MT7601U_LinuxSTA_$(PKG_VERSION)

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_SOURCE_SUBDIR)
#PKG_BUILD_PARALLEL:=1

PKG_MAINTAINER:=Igor Zalatov (ZFT Lab.) <flyrouter@gmail.com>
PKG_LICENSE:=GPL-2.0
PKG_LICENSE_FILES:=LICENSE


include $(INCLUDE_DIR)/package.mk

define KernelPackage/$(PKG_NAME)
	SUBMENU:=Wireless Drivers
	DEPENDS:=+@USB_SUPPORT +@DRIVER_WEXT_SUPPORT +kmod-usb-core +wireless-tools +hostapd-common-old
	TITLE:=Driver for MT7601U wireless adapters (HI35xx)
	FILES:=$(PKG_BUILD_DIR)/$(PKG_NAME).$(LINUX_KMOD_SUFFIX)
	VERSION:=$(LINUX_VERSION)+$(PKG_VERSION)-$(PKG_RELEASE)
	AUTOLOAD:=$(call AutoLoad,21,$(PKG_NAME))
	MENU:=1
endef

define KernelPackage/$(PKG_NAME)/config
  if PACKAGE_kmod-$(PKG_NAME)
	config PACKAGE_$(PKG_NAME)_debug
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

MAKE_OPTS:= \
	ARCH="$(LINUX_KARCH)" \
	CROSS_COMPILE="$(TARGET_CROSS)" \
	SUBDIRS="$(PKG_BUILD_DIR)" \
	KSRC="$(LINUX_DIR)" \
	CHIPSET="7601U" \
	OSABL=NO \
	RELEASE=DPO \
	RT28xx_MODE=STA \
	RTDEBUG=$(CONFIG_PACKAGE_HI35XX-MT7601U_DEBUG)

define Build/Configure
endef

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) LINUX_DIR=$(LINUX_DIR) \
		$(MAKE_OPTS)
	$(CP) $(PKG_BUILD_DIR)/os/linux/mt7601Usta.$(LINUX_KMOD_SUFFIX) \
		$(PKG_BUILD_DIR)/$(PKG_NAME).$(LINUX_KMOD_SUFFIX)
endef

define KernelPackage/$(PKG_NAME)/install
	$(INSTALL_DIR) $(1)/etc/Wireless/RT2870STA/
	$(INSTALL_DIR) $(1)/lib/modules/$(LINUX_VERSION)
	$(INSTALL_DIR) $(1)/lib/wifi
	$(CP) $(PKG_BUILD_DIR)/RT2870STA.dat $(1)/etc/Wireless/RT2870STA/
	$(INSTALL_DATA) ./files/wext.sh $(1)/lib/wifi
endef

$(eval $(call KernelPackage,$(PKG_NAME)))
