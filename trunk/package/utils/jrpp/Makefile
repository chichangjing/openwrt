#
# Copyright (C) 2016 Joyware
#

include $(TOPDIR)/rules.mk

PKG_NAME:=jrpp
PKG_VERSION:=1.3
PKG_RELEASE:=1

PKG_BUILD_DIR := $(BUILD_DIR)/$(PKG_NAME)

include $(INCLUDE_DIR)/package.mk

define Package/jrpp
  SECTION:=utils
  CATEGORY:=Utilities
  TITLE:=Joyware Ring Protect Protocol
  DEPENDS:=+jshn +libjson-c +libblobmsg-json +libubox +libubus +libjssdk_us +libubox +libjwsconvert +libuci
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)/
endef

define Build/Configure
endef

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) \
	STAGING_DIR="$(STAGING_DIR)" \
	CFLAGS="$(TARGET_CFLAGS) -Wall" \
	LDFLAGS="$(TARGET_LDFLAGS)"
endef

define Build/InstallDev
endef

define Package/jrpp/install
	$(INSTALL_DIR) $(1)/etc/config
	$(INSTALL_DATA) ./files/jrppd.conf $(1)/etc/config/jrppd
	$(INSTALL_DIR) $(1)/etc/jrpp
	ln -sf /var/run/jrppd.conf $(1)/etc/jrpp/
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/jrppd.init $(1)/etc/init.d/jrppd
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/{jrppd,jrppctl} $(1)/usr/sbin/
endef

$(eval $(call BuildPackage,jrpp))
