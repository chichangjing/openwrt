#
# Copyright (C) 2016 Joyware
#

include $(TOPDIR)/rules.mk

PKG_NAME:=rstp
PKG_VERSION:=1.0
PKG_RELEASE:=1

PKG_BUILD_DIR := $(BUILD_DIR)/$(PKG_NAME)

include $(INCLUDE_DIR)/package.mk

define Package/rstp
  SECTION:=utils
  CATEGORY:=Utilities
  TITLE:=Rapid Spanning Tree Protocol
  DEPENDS:=+libjssdk_us
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


SERIAL_NUMBER=$(subst ",,$(CONFIG_JWS_SERIAL_NUM))
CONF_FILE=./files/rstpd.$(SERIAL_NUMBER)

define Package/rstp/install
	$(INSTALL_DIR) $(1)/etc/config
	$(INSTALL_DATA) $(CONF_FILE) $(1)/etc/config/rstpd
	$(INSTALL_DIR) $(1)/etc/rstp

	ln -sf /var/run/rstpd.conf $(1)/etc/rstp/
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/rstpd.init $(1)/etc/init.d/rstpd
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/{rstpd,rstpctl} $(1)/usr/sbin/
endef

$(eval $(call BuildPackage,rstp))
