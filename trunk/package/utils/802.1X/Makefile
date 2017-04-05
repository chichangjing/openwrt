#
# Copyright (C) 2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:= nas
PKG_VERSION:=1.0
PKG_RELEASE:=1

PKG_BUILD_DIR := $(BUILD_DIR)/$(PKG_NAME)

include $(INCLUDE_DIR)/package.mk

define Package/nas
  SECTION:=utils
  CATEGORY:=Utilities
  TITLE:=OBtelecom industry 802.1X Authenticator processor
  DEPENDS:=+jshn +libjson-c +libblobmsg-json +libubox +libubus +libjwsconvert +libuci +libjwf
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)/
endef

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) \
		CC="$(TARGET_CC)" \
		CFLAGS="$(TARGET_CFLAGS) -Wall" \
		LDFLAGS="$(TARGET_LDFLAGS)"
endef

define Package/nas/install
	$(INSTALL_DIR) $(1)/etc/config
	$(INSTALL_DATA) ./files/nas.conf $(1)/etc/config/nas
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/nas.init $(1)/etc/init.d/nas
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/nas $(1)/usr/sbin/
endef

$(eval $(call BuildPackage,nas))
