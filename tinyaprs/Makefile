#
# Copyright (C) 2008-2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=tinyaprs
PKG_REV:=HEAD
PKG_VERSION:=0.5.4
PKG_RELEASE:=

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:=https://github.com/shawnchain/tinyaprs-gate/trunk
#PKG_SOURCE_URL:=https://github.com/shawnchain/tinyaprs-gate/tags/1.0
PKG_SOURCE_SUBDIR:=$(PKG_NAME)-$(PKG_VERSION)
PKG_SOURCE_VERSION:=$(PKG_REV)
PKG_SOURCE_PROTO:=svn

include $(INCLUDE_DIR)/package.mk

define Package/tinyaprs
  SECTION:=net
  CATEGORY:=Network
  TITLE:=Tiny APRS RX only I-gate by BG5HHP
  URL:=https://github.com/shawnchain/tinyaprs-gate/
  DEPENDS:=
endef

define Package/tinyaprs/description
	This daemon listens for traffic on the specified serial interfaces.
	It then forwards appropriate packets to APRS-IS servers.
endef

#TARGET_CFLAGS = -std=c99

define Package/tinyaprs/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/src/tinyaprs $(1)/usr/sbin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/src/tinyaprs-util $(1)/usr/sbin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/src/tinyaprs-cfg $(1)/usr/sbin/
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/etc/init.d/tinyaprs.init $(1)/etc/init.d/tinyaprs
endef

define Package/tinyaprs/conffiles
endef

define Package/tinyaprs/postinst
#!/bin/sh
if [ -e /etc/openwrt_release ]; then
	/etc/init.d/tinyaprs disable || :
	if [ -e /etc/init.d/aprx ]; then
		/etc/init.d/aprx disable || :
		/etc/init.d/aprx stop || :
	fi
fi
exit 0
endef

$(eval $(call BuildPackage,tinyaprs))
