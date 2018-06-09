#
# Copyright (C) 2006-2018 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=hisi-osdrv2
PKG_VERSION:=3.4.35
PKG_RELEASE:=0.1

PKG_LICENSE:=MIT
PKG_LICENSE_FILES:=HISILICON COPYING

include $(INCLUDE_DIR)/package.mk

define Package/hisi-osdrv2/default
	SECTION:=multimedia
	CATEGORY:=Multimedia
	MAINTAINER:=Igor Zalatov <flyrouter@gmail.com>
	#TITLE:=Hisilicon OSDRV kernel modules, drivers, libs and tools
	TITLE:=Hisilicon OSDRV
	#SUBMENU:=Hisilicon OSDRV (1) for HI35_16C_18E_V200 with kernel 3.0.8
	SUBMENU:=Hisilicon OSDRV (2) for HI35_16C_18E_V200 with kernel 3.4.35
	#SUBMENU:=Hisilicon OSDRV (3)for HI35_16C_18E_V200 with kernel 3.18.20
	URL:=http://zftlab.org
	DEPENDS:=+busybox +dropbear
endef

define Package/hisi-osdrv2-base
$(call Package/hisi-osdrv2/default)
	TITLE+= - base package
endef

define Package/hisi-osdrv2-binary
$(call Package/hisi-osdrv2/default)
	TITLE+= - binary package
endef

define Package/hisi-osdrv2-kmod
$(call Package/hisi-osdrv2/default)
	TITLE+= - kmod package
endef

define Package/hisi-osdrv2-lib
$(call Package/hisi-osdrv2/default)
	TITLE+= - lib package
endef

define Package/hisi-osdrv2-sensor
$(call Package/hisi-osdrv2/default)
	TITLE+= - sensor package
endef

define Package/hisi-osdrv2-tool
$(call Package/hisi-osdrv2/default)
	TITLE+= - tool package
endef

#PKG_BUILD_DIR := $(BUILD_DIR)/$(PKG_NAME)

define Build/Configure
endef

define Build/Compile
endef


#define Build/InstallDev
#	$(INSTALL_DIR) $(1)/usr/include/hisi-osdrv2
#	$(CP) ./include/* $(1)/usr/include/hisi-osdrv2/
#endef

define Package/hisi-osdrv2-base/install
	$(INSTALL_DIR) $(1)/etc $(1)/etc/config
	#$(INSTALL_BIN) ./files/etc/rc.local.example $(1)/etc/
endef

define Package/hisi-osdrv2-binary/install
	$(INSTALL_DIR) $(1)/usr/bin
endef

define Package/hisi-osdrv2-kmod/install
	$(INSTALL_DIR) $(1)/lib/modules/$(PKG_VERSION)
	$(CP) ./kmod/*.ko $(1)/lib/modules/$(PKG_VERSION)
endef

define Package/hisi-osdrv2-lib/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) ./lib/lib*.so $(1)/usr/lib/
endef

define Package/hisi-osdrv2-sensor/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) ./sensor/libsns_*.so $(1)/usr/lib/
endef

define Package/hisi-osdrv2-tool/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/hisi-osdrv2-tool/himm $(1)/usr/bin/
endef

#$(eval $(call BuildPackage,hisi-osdrv2-base))
#$(eval $(call BuildPackage,hisi-osdrv2-binary))
$(eval $(call BuildPackage,hisi-osdrv2-kmod))
$(eval $(call BuildPackage,hisi-osdrv2-lib))
$(eval $(call BuildPackage,hisi-osdrv2-sensor))
#$(eval $(call BuildPackage,hisi-osdrv2-tool))
