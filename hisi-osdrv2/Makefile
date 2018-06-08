#
# Copyright (C) 2007-2018 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=hisi-osdrv2
PKG_VERSION:=3.4.35
PKG_RELEASE:=0.1
PKG_MAINTAINER:=Igor Zalatov <flyrouter@gmail.com>

PKG_LICENSE:=MIT
PKG_LICENSE_FILES:=HISILICON COPYING

PKG_BUILD_PARALLEL:=1

PKG_INSTALL:=1

PKG_BUILD_DIR := $(BUILD_DIR)/$(PKG_NAME)

include $(INCLUDE_DIR)/package.mk

define Package/hisi-osdrv2/Default
	SECTION:=multimedia
	CATEGORY:=Multimedia
	TITLE+=Hisilicon osdrv2
	URL:=http://zftlab.org/
endef

define Package/hisi-osdrv2/Default/description
	Hisilicon kernel modules, drivers and tools
endef


define Package/hisi-osdrv2-tools
$(call Package/hisi-osdrv2/Default)
	SECTION:=multimedia
	CATEGORY:=Multimedia
	DEPENDS:=+busybox +dropbear
	TITLE+= (tools)
	SUBMENU:=Hisilicon
endef

define Package/hisi-osdrv2-tools/description
$(call Package/hisi-osdrv2/Default/description)
	This package contains the...
endef

define Package/hisi-osdrv2-libs
$(call Package/hisi-osdrv2/Default)
	SECTION:=multimedia
	CATEGORY:=Multimedia
	TITLE+= (library)
	DEPENDS:=+busybox +dropbear
	SUBMENU:=Hisilicon
endef

define Package/hisi-osdrv2-libs/description
$(call Package/hisi-osdrv2/Default/description)
	This package contains the...
endef

define Package/hisi-osdrv2-sensors
$(call Package/hisi-osdrv2/Default)
	SECTION:=multimedia
	CATEGORY:=Multimedia
	TITLE+= (sensors)
	DEPENDS:=+busybox +dropbear
	SUBMENU:=Hisilicon
endef

define Package/hisi-osdrv2-sensors/description
$(call Package/hisi-osdrv2/Default/description)
	This package contains the...
endef


define Build/Prepare
	$(INSTALL_DIR) $(PKG_BUILD_DIR)
	$(CP) ./tools/* $(PKG_BUILD_DIR)/
endef

define Build/Compile
	$(TARGET_CC) $(TARGET_CFLAGS) -Os $(PKG_BUILD_DIR)/himm.c -o $(PKG_BUILD_DIR)/$(PKG_NAME)
endef


define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include/hisi-osdrv2
	$(CP) ./include/* $(1)/usr/include/hisi-osdrv2/
endef

define Package/hisi-osdrv2-libs/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) ./libs/*.so $(1)/usr/lib/
endef

define Package/hisi-osdrv2-sensors/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) ./sensors/libsns_*.so $(1)/usr/lib/
endef

define Package/hisi-osdrv2-tools/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/hisi-osdrv2-tools/himm $(1)/usr/sbin/
endef

$(eval $(call BuildPackage,hisi-osdrv2-libs))
$(eval $(call BuildPackage,hisi-osdrv2-sensors))
$(eval $(call BuildPackage,hisi-osdrv2-tools))
