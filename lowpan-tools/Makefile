# 
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.

include $(TOPDIR)/rules.mk

PKG_NAME:=lowpan-tools
PKG_VERSION:=git
PKG_REV:=a1d9615adde6d1a568813c24a128273ed755af04
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION)-$(PKG_REV).tar.gz
PKG_SOURCE_URL:=git://linux-zigbee.git.sourceforge.net/gitroot/linux-zigbee/linux-zigbee
PKG_SOURCE_PROTO:=git
PKG_SOURCE_SUBDIR:=$(PKG_NAME)-$(PKG_VERSION)
PKG_SOURCE_VERSION:=$(PKG_REV)

include $(INCLUDE_DIR)/package.mk

define Package/lowpan-tools
    TITLE:=a set of utils to manage the Linux LoWPAN stack
    SECTION:=network
    CATEGORY:=Network
    URL:=http://sourceforge.net/apps/trac/linux-zigbee/
    DEPENDS:=+libnl-3
endef

define Package/lowpan-tools/description
This is a set of utils to manage the Linux LoWPAN stack.
The LoWPAN stack aims for IEEE 802.15.4-2003 (and for lesser extent
IEEE 802.15.4-2006) compatibility.

If you want to learn more, please go to http://linux-zigbee.sf.net/
endef

define Build/Prepare
	$(call Build/Prepare/Default)
	(cd $(PKG_BUILD_DIR) && ./autogen.sh --help)
endef

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include
	$(CP) $(PKG_BUILD_DIR)/include/{ieee802154.h,nl802154.h} $(1)/usr/include
endef

define Package/lowpan-tools/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/src/{iz,izattach,izchat,izcoordinator} $(1)/usr/sbin/
endef

$(eval $(call BuildPackage,lowpan-tools))
