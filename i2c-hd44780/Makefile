#
# Copyright (C) 2007 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# $Id: $

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=i2c-hd44780
PKG_VERSION:=0.1
PKG_RELEASE:=2

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)

include $(INCLUDE_DIR)/package.mk

MAKEFLAGS_KMOD:= -C "$(LINUX_DIR)" \
  CROSS_COMPILE="$(TARGET_CROSS)" \
  ARCH="$(LINUX_KARCH)" \
  PATH="$(TARGET_PATH)" \
  SUBDIRS="$(PKG_BUILD_DIR)"

define KernelPackage/$(PKG_NAME)
  SUBMENU:=Other modules
  DEPENDS:=
  TITLE:=Kernel driver for hd44780-based LCD display
  URL:=https://github.com/lucidm/lcdi2c
  FILES:=$(PKG_BUILD_DIR)/lcdi2c.$(LINUX_KMOD_SUFFIX)
  VERSION:=$(LINUX_VERSION)+$(PKG_VERSION)-$(PKG_RELEASE)
  AUTOLOAD:=$(call AutoLoad,80,hd44780)
endef

define KernelPackage/$(PKG_NAME)/description
	Kernel driver for access to hd44780-based display via i2c
endef


define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)/
endef

define Build/Compile
	$(MAKE) $(MAKEFLAGS_KMOD) \
		modules
endef

$(eval $(call KernelPackage,i2c-hd44780))
