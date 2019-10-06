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
PKG_RELEASE:=1.0.4.1

PKG_LICENSE:=MIT
PKG_LICENSE_FILES:=HISILICON COPYING

include $(INCLUDE_DIR)/package.mk

define Package/$(PKG_NAME)/default
	SECTION:=openipc
	CATEGORY:=OpenIPC
	MAINTAINER:=Igor Zalatov <flyrouter@gmail.com>
	TITLE:=Hisilicon OSDRV-2
	SUBMENU:=Hisilicon OSDRV-2 with kernel 3.4.35
	URL:=http://zftlab.org
	DEPENDS:=@(TARGET_hisilicon_armv5tej_hi3516cv200||TARGET_hisilicon_armv5tej_hi3518ev200||TARGET_hisilicon_armv5tej_hi3518ev201) +libstdcpp
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
	DEPENDS:=+$(PKG_NAME)-base +$(PKG_NAME)-kmod +$(PKG_NAME)-lib +$(PKG_NAME)-sensor +libpthread
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
	$(INSTALL_DIR) $(1)/usr/bin
	$(CP) ./script/* $(1)/usr/bin/
endef

define Package/$(PKG_NAME)-binary/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(CP) ./binary/* $(1)/usr/bin/
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
	$(CP) ./sensor/libsns_ar0130.so $(1)/usr/lib/
	$(CP) ./sensor/libsns_imx222.so $(1)/usr/lib/
	$(CP) ./sensor/libsns_imx323.so $(1)/usr/lib/
	$(CP) ./sensor/libsns_jxf22.so $(1)/usr/lib/
	$(CP) ./sensor/libsns_sc1135.so $(1)/usr/lib/
	$(CP) ./sensor/libsns_sc2135.so $(1)/usr/lib/
	$(INSTALL_DIR) $(1)/etc/sensors
	$(CP) ./sensor/config/* $(1)/etc/sensors
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
