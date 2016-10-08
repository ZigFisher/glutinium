#
# Copyright (C) 2006-2014 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=rcswitch
PKG_VERSION:=2016-10-08
PKG_RELEASE:=2
PKG_MAINTAINER:=Igor Zalatov <flyrouter@gmail.com>
#PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)

PKG_LICENSE:=GPL-2.0
PKG_LICENSE_FILES:=LICENSE

include $(INCLUDE_DIR)/package.mk

define Package/homes-smart
  SECTION:=utils
  CATEGORY:=Utilities
  TITLE:=Some utils for control RCswitch devices
  MAINTAINER:=https://github.com/apiel/rcswitch
  DEPENDS:=+libpthread
endef

define Package/rcswitch/description
	Some utils for control RCswitch devices
endef

define Build/Prepare
	$(INSTALL_DIR) $(PKG_BUILD_DIR)
	$(INSTALL_DATA) ./src/* $(PKG_BUILD_DIR)/
endef

define Build/Compile
	#g++ -o rc rc.cpp RCSwitch.cpp WProgram.cpp -std=c++0x -pthread
	$(TARGET_CXX) $(TARGET_CXXFLAGS) $(TARGET_LDFLAGS) -o rc $(PKG_BUILD_DIR)/rc.cpp $(PKG_BUILD_DIR)/RCSwitch.cpp $(PKG_BUILD_DIR)/WProgram.cpp -pthread
endef

define Package/rcswitch/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/rc $(1)/usr/bin/
endef

$(eval $(call BuildPackage,rcswitch))
