#
# Copyright (C) 2006-2018 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#


include $(TOPDIR)/rules.mk

PKG_NAME:=hisi-osdrv1
PKG_VERSION:=3.0.8
PKG_RELEASE:=1.0.B.0

PKG_LICENSE:=MIT
PKG_LICENSE_FILES:=HISILICON COPYING

include $(INCLUDE_DIR)/package.mk

define Package/$(PKG_NAME)/default
	SECTION:=openipc_old
	CATEGORY:=OpenIPC_old
	MAINTAINER:=Igor Zalatov <flyrouter@gmail.com>
	TITLE:=Hisilicon OSDRV-1
	SUBMENU:=HiSilicon related
	URL:=http://zftlab.org
	DEPENDS:=@(TARGET_hi35xx_16cv100||TARGET_hi35xx_18av100||TARGET_hi35xx_18cv100||TARGET_hi35xx_18ev100) +libstdcpp
endef

define Package/$(PKG_NAME)/Default/description
	Hisilicon OSDRV kernel modules, drivers and libs
endef

define Package/$(PKG_NAME)-base
$(call Package/$(PKG_NAME)/default)
	TITLE+= - base package
endef

define Package/$(PKG_NAME)-binary
$(call Package/$(PKG_NAME)/default)
	TITLE+= - binary package
	DEPENDS:=+$(PKG_NAME)-lib +libpthread +libevent2 +lame-lib
endef

define Package/$(PKG_NAME)-kmod
$(call Package/$(PKG_NAME)/default)
	TITLE+= - kmod package
endef

define Package/$(PKG_NAME)-lib
$(call Package/$(PKG_NAME)/default)
	TITLE+= - lib package
endef

define Package/$(PKG_NAME)-majestic
$(call Package/$(PKG_NAME)/default)
	TITLE+= - tiny but powerful IPC streaming software
	DEPENDS:=+$(PKG_NAME)-lib +lame-lib +libevent2 +libjson-c +libpthread
endef

define Package/$(PKG_NAME)-sensor
$(call Package/$(PKG_NAME)/default)
	TITLE+= - sensor package
	DEPENDS:=+libstdcpp
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
	$(INSTALL_BIN) ./script/hisi-* $(1)/usr/bin/
endef

define Package/$(PKG_NAME)-binary/install
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./script/ipcam.init $(1)/etc/init.d/ipcam
	#$(CP) ./script/ircut_demo $(1)/usr/bin/
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) ./script/load_hisilicon $(1)/usr/bin/
	$(INSTALL_DIR) $(1)/etc/sensors
	$(CP) ./binary/etc/sensors/* $(1)/etc/sensors/
	$(INSTALL_DIR) $(1)/usr/lib/fonts
	$(CP) ./binary/lib/fonts/* $(1)/usr/lib/fonts/
	$(INSTALL_DIR) $(1)/usr/lib/sensors
	$(CP) ./binary/lib/sensors/* $(1)/usr/lib/sensors/
endef

define Package/$(PKG_NAME)-kmod/install
	$(INSTALL_DIR) $(1)/lib/modules/$(PKG_VERSION)
	$(CP) ./kmod/*.ko $(1)/lib/modules/$(PKG_VERSION)
endef

define Package/$(PKG_NAME)-lib/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) ./lib/lib*.so $(1)/usr/lib/
endef

define Package/$(PKG_NAME)-majestic/install
	wget -v -O $(DL_DIR)/majestic.hi3516cv100.master.tar.bz2 http://openipc.s3-eu-west-1.amazonaws.com/majestic.hi3516cv100.master.tar.bz2
	tar -C $(PKG_BUILD_DIR) -xvjf $(DL_DIR)/majestic.hi3516cv100.master.tar.bz2
	$(INSTALL_DIR) $(1)/etc
	$(CP) $(PKG_BUILD_DIR)/majestic.yaml $(1)/etc/
	$(INSTALL_DIR) $(1)/usr/bin
	$(CP) $(PKG_BUILD_DIR)/majestic $(1)/usr/bin/
endef

define Package/$(PKG_NAME)-sensor/install
	#$(INSTALL_DIR) $(1)/usr/lib
	#$(CP) ./sensor/libsns_imx122.so $(1)/usr/lib/
	#$(CP) ./sensor/libsns_ov9712.so $(1)/usr/lib/
	$(INSTALL_DIR) $(1)/etc/sensors
	#$(CP) ./sensor/config/* $(1)/etc/sensors
endef

$(eval $(call BuildPackage,$(PKG_NAME)-base))
$(eval $(call BuildPackage,$(PKG_NAME)-binary))
$(eval $(call BuildPackage,$(PKG_NAME)-kmod))
$(eval $(call BuildPackage,$(PKG_NAME)-lib))
$(eval $(call BuildPackage,$(PKG_NAME)-majestic))
$(eval $(call BuildPackage,$(PKG_NAME)-sensor))
