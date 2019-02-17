include $(TOPDIR)/rules.mk

PKG_NAME:=hub-ctrl
PKG_VERSION:=1.0
PKG_RELEASE:=1

PKG_SOURCE:=hub-ctrl.c
PKG_SOURCE_URL:=http://www.gniibe.org/oitoite/ac-power-control-by-USB-hub

include $(INCLUDE_DIR)/package.mk

define Package/hub-ctrl
  SECTION:=net
  CATEGORY:=Utilities
  TITLE:=Manipulate USB HUB port control
  URL:=http://www.gniibe.org/development/ac-power-control-by-USB-hub/index
  DEPENDS:=+libusb
  MAINTAINER:=Gateworks Corporation <support@gateworks.com>
endef

define Package/hub-ctrl/description
	Tool for manipulating USB HUB port control via libusb
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) $(DL_DIR)/$(PKG_SOURCE) $(PKG_BUILD_DIR)
endef

define Build/Configure
endef

define Build/Compile
	$(TARGET_CC) $(TARGET_CFLAGS) -I$(STAGING_DIR)/usr/include \
		-L$(STAGING_DIR)/usr/lib -lusb \
		-o $(PKG_BUILD_DIR)/hub-ctrl $(PKG_BUILD_DIR)/hub-ctrl.c
endef

define Package/hub-ctrl/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/hub-ctrl $(1)/usr/sbin/
endef

$(eval $(call BuildPackage,hub-ctrl))
