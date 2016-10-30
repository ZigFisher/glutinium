#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=libnl-3
PKG_VERSION:=3.2.21
PKG_RELEASE:=1

PKG_SOURCE:=libnl-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://www.infradead.org/~tgr/libnl/files/
PKG_MD5SUM:=6fe7136558a9071e70673dcda38545b3

PKG_BUILD_DIR=$(BUILD_DIR)/libnl-$(PKG_VERSION)

include $(INCLUDE_DIR)/package.mk

define Package/libnl-3
  SECTION:=libs
  CATEGORY:=Libraries
  TITLE:=netlink socket library
  URL:=http://people.suug.ch/~tgr/libnl/
  DEPENDS:=+libpthread
endef

define Package/libnl-3/description
 This package contains a library for applications dealing with netlink sockets
endef

TARGET_CFLAGS += -ffunction-sections $(FPIC)

define Build/Compile
	$(call Build/Compile/Default)
	make -C $(PKG_BUILD_DIR) \
		DESTDIR="$(PKG_INSTALL_DIR)" \
		install
endef

define Build/InstallDev
	$(INSTALL_DIR) $(1)
	$(CP) $(PKG_INSTALL_DIR)/* $(1)/
	mkdir -p $(1)/usr/include/libnl3
	$(CP) $(PKG_BUILD_DIR)/include/linux $(1)/usr/include/libnl3/
endef

define Package/libnl-3/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libnl-route-3.so.* $(1)/usr/lib/
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libnl-genl-3.so.* $(1)/usr/lib/
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libnl-nf-3.so.* $(1)/usr/lib/
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libnl-route-3.so.* $(1)/usr/lib/
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libnl-3.so.* $(1)/usr/lib/
endef

$(eval $(call BuildPackage,libnl-3))
