#
# Copyright (C) 2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=jwupgrade
PKG_VERSION:=1.0
PKG_RELEASE:=1

PKG_BUILD_DIR := $(BUILD_DIR)/$(PKG_NAME)

include $(INCLUDE_DIR)/package.mk

define Package/jwupgrade
  SECTION:=utils
  CATEGORY:=Utilities
  TITLE:=Joyware privite protocol upragde process
  DEPENDS:=
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./jwupgrade.py $(PKG_BUILD_DIR)
	$(CP) ./jwsetmac.py $(PKG_BUILD_DIR)
	$(CP) ./jwps.sh $(PKG_BUILD_DIR)
	$(CP) ./jwimage_check.py $(PKG_BUILD_DIR)
	$(CP) ./jwgpio.py $(PKG_BUILD_DIR)
	$(CP) ./getuci.py $(PKG_BUILD_DIR)
	$(CP) ./tunnelclient.py $(PKG_BUILD_DIR)
	$(CP) ./tunneld $(PKG_BUILD_DIR)
	$(CP) ./defconf.tgz $(PKG_BUILD_DIR)
endef

define Build/Configure
endef

define Build/Compile
endef

define Package/jwupgrade/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/jwupgrade.py $(1)/usr/sbin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/jwsetmac.py $(1)/usr/sbin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/jwps.sh $(1)/usr/sbin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/jwimage_check.py $(1)/usr/sbin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/jwgpio.py $(1)/usr/sbin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/getuci.py $(1)/usr/sbin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/tunnelclient.py $(1)/usr/sbin/
	$(INSTALL_DIR) $(1)/usr/share
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/defconf.tgz $(1)/usr/share
	$(INSTALL_DIR) $(1)/etc/config
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/tunneld $(1)/etc/config
endef

$(eval $(call BuildPackage,jwupgrade))
