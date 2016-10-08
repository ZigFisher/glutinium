#
# Copyright (C) 2008 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=gpio-rcswitch
PKG_VERSION:=0.1
PKG_RELEASE:=1
PKG_MAINTAINER:=Igor Zalatov <flyrouter@gmail.com>
PKG_LICENSE:=GPL-2.0
PKG_LICENSE_FILES:=LICENSE

include $(INCLUDE_DIR)/package.mk

define KernelPackage/gpio-rcswitch
  SUBMENU:=Other modules
  TITLE:=Kernel module for control 433Mhz based RC switches
  FILES:=$(PKG_BUILD_DIR)/gpio-rcswitch.ko
  AUTOLOAD:=$(call AutoLoad,50,gpio-rcswitch)
  DEPENDS:=@GPIO_SUPPORT @BUILD_PATENTED
endef

define KernelPackage/gpio-rcswitch/description
	Linux kernel module to control 433Mhz based RC switches through sysfs
endef

include $(INCLUDE_DIR)/kernel-defaults.mk

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	cp src/Makefile src/gpio-rcswitch.c $(PKG_BUILD_DIR)/
endef

define Build/Compile
	$(MAKE) $(KERNEL_MAKEOPTS) SUBDIRS="$(PKG_BUILD_DIR)" modules
endef

define KernelPackage/gpio-rcswitch/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_DATA) ./files/rcswitch.sh $(1)/usr/bin/rcswitch.sh
	#$(INSTALL_BIN) ./files/rcswitch.init $(1)/etc/init.d/rcswitch
endef

$(eval $(call KernelPackage,gpio-rcswitch))
