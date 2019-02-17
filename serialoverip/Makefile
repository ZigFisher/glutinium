include $(TOPDIR)/rules.mk

PKG_NAME:=serialoverip
PKG_VERSION:=1.0
PKG_RELEASE:=1


include $(INCLUDE_DIR)/package.mk

define Package/serialoverip
  SECTION:=net
  CATEGORY:=Network
  TITLE:=SerialOverIP
  MAINTAINER:=Gateworks Corporation <support@gateworks.com>
endef

define Package/serialoverip/description
	Tool for transport of serial interfaces over UDP/IP and supporting Multicast.
	Usefull for accessing a distant serial device by a local program.
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) -r ./src/* $(PKG_BUILD_DIR)/
endef

define Build/Configure
endef

define Build/Compile
	$(TARGET_CC) $(TARGET_CFLAGS) \
		-o $(PKG_BUILD_DIR)/serialoverip $(PKG_BUILD_DIR)/serialoverip.c
endef


define Package/serialoverip/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/serialoverip $(1)/usr/sbin/
	$(CP) -r ./files/* $(1)/
endef

$(eval $(call BuildPackage,serialoverip))
