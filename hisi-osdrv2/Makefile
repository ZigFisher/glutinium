#
# Copyright (C) 2006-2018 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

# 2018.06.09
# В будущем сделать зависимость от платформы Hisilicon и конкретной версии ядра
# Включить сборку из исходников himm, некоторых модулей ябра и библиотек сенсоров


include $(TOPDIR)/rules.mk

PKG_NAME:=hisi-osdrv2
PKG_VERSION:=3.4.35
PKG_RELEASE:=0.1

PKG_LICENSE:=MIT
PKG_LICENSE_FILES:=HISILICON COPYING

include $(INCLUDE_DIR)/package.mk

define Package/$(PKG_NAME)/default
	SECTION:=multimedia
	CATEGORY:=Multimedia
	MAINTAINER:=Igor Zalatov <flyrouter@gmail.com>
	TITLE:=Hisilicon OSDRV
	#SUBMENU:=Hisilicon OSDRV-1 for HI35_16C_18E_V200 with kernel 3.0.8
	SUBMENU:=Hisilicon OSDRV-2 for HI35_16C_18E_V200 with kernel 3.4.35
	#SUBMENU:=Hisilicon OSDRV-3 for HI35_16C_18E_V200 with kernel 3.18.20
	URL:=http://zftlab.org
	DEPENDS:=+busybox +dropbear
endef

define Package/$(PKG_NAME)/Default/description
	Hisilicon OSDRV kernel modules, drivers, libs and tools
endef

define Package/$(PKG_NAME)-base
$(call Package/$(PKG_NAME)/default)
	TITLE+= - base package
endef

define Package/$(PKG_NAME)-binary
$(call Package/$(PKG_NAME)/default)
	TITLE+= - binary package
	DEPENDS:=+$(PKG_NAME)-base +$(PKG_NAME)-kmod +$(PKG_NAME)-lib +$(PKG_NAME)-sensor +libpthread
endef

define Package/$(PKG_NAME)-kmod
$(call Package/$(PKG_NAME)/default)
	TITLE+= - kmod package
endef

define Package/$(PKG_NAME)-lib
$(call Package/$(PKG_NAME)/default)
	TITLE+= - lib package
endef

define Package/$(PKG_NAME)-sensor
$(call Package/$(PKG_NAME)/default)
	TITLE+= - sensor package
endef

define Package/$(PKG_NAME)-tool
$(call Package/$(PKG_NAME)/default)
	TITLE+= - tool package
endef

define Build/Prepare
endef

define Build/Configure
endef

define Build/Compile
endef

define Build/InstallDev
	$(INSTALL_DIR) $(STAGING_DIR)/usr/include/$(PKG_NAME)
	$(CP) ./include/* $(STAGING_DIR)/usr/include/$(PKG_NAME)/
	$(INSTALL_DIR) $(STAGING_DIR)/usr/lib
	$(CP) ./lib/*.so $(STAGING_DIR)/usr/lib/
endef

define Package/$(PKG_NAME)-base/install
	$(INSTALL_DIR) $(1)/etc/hisi
	$(INSTALL_BIN) ./script/* $(1)/etc/hisi/
endef

define Package/$(PKG_NAME)-binary/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(CP) ./binary/hijpg $(1)/usr/bin/
	$(CP) ./binary/himm $(1)/usr/bin/
	$(CP) ./binary/rtsp-hisi $(1)/usr/bin/
endef

define Package/$(PKG_NAME)-kmod/install
	$(INSTALL_DIR) $(1)/lib/modules/$(PKG_VERSION)
	$(CP) ./kmod/*.ko $(1)/lib/modules/$(PKG_VERSION)
endef

define Package/$(PKG_NAME)-lib/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) ./lib/lib*.so $(1)/usr/lib/
endef

define Package/$(PKG_NAME)-sensor/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) ./sensor/libsns_*.so $(1)/usr/lib/
endef

define Package/$(PKG_NAME)-tool/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/$(PKG_NAME)-tool/himm $(1)/usr/bin/
endef

$(eval $(call BuildPackage,$(PKG_NAME)-base))
$(eval $(call BuildPackage,$(PKG_NAME)-binary))
$(eval $(call BuildPackage,$(PKG_NAME)-kmod))
$(eval $(call BuildPackage,$(PKG_NAME)-lib))
$(eval $(call BuildPackage,$(PKG_NAME)-sensor))
#$(eval $(call BuildPackage,$(PKG_NAME)-tool))
