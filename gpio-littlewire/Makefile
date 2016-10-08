#
# Copyright (C) 2008 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=gpio-littlewire
PKG_VERSION:=0.1
PKG_RELEASE:=1
PKG_MAINTAINER:=Igor Zalatov <flyrouter@gmail.com>
PKG_LICENSE:=GPL-2.0
PKG_LICENSE_FILES:=LICENSE

include $(INCLUDE_DIR)/package.mk

define KernelPackage/gpio-littlewire
	SUBMENU:=Other modules
	TITLE:=Little Wire USB device GPIO driver
	MAINTAINER:=https://www.mail-archive.com/linux-gpio@vger.kernel.org/msg01353.html
	FILES:=$(PKG_BUILD_DIR)/gpio-littlewire.ko
	AUTOLOAD:=$(call AutoLoad,90,gpio-littlewire)
	DEPENDS:=@GPIO_SUPPORT +kmod-usb-core
endef

define KernelPackage/gpio-littlewire/description
	Little Wire USB device GPIO driver
endef

include $(INCLUDE_DIR)/kernel-defaults.mk

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	cp src/Makefile src/gpio-littlewire.c $(PKG_BUILD_DIR)/
endef

define Build/Compile
	$(MAKE) $(KERNEL_MAKEOPTS) SUBDIRS="$(PKG_BUILD_DIR)" modules
endef

define KernelPackage/gpio-littlewire/install
	#$(INSTALL_DIR) $(1)/usr/bin
	#$(INSTALL_DATA) ./files/littlewire.sh $(1)/usr/bin/littlewire.sh
	#$(INSTALL_BIN) ./files/littlewire.init $(1)/etc/init.d/littlewire
endef

$(eval $(call KernelPackage,gpio-littlewire))

