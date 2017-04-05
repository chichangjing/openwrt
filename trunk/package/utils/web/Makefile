#
# Copyright (C) 2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=jwweb
PKG_VERSION:=1.0
PKG_RELEASE:=1

PKG_BUILD_DIR := $(BUILD_DIR)/$(PKG_NAME)

include $(INCLUDE_DIR)/package.mk

define Package/jwweb
  SECTION:=utils
  CATEGORY:=Utilities
  TITLE:=Joyware customized web
  DEPENDS:=+luci
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./* $(PKG_BUILD_DIR)/
endef

define Build/Configure
endef

define Build/Compile
endef


define Package/jwweb/install
	$(INSTALL_DIR) $(1)/usr/lib/lua/luci/controller/admin
	$(INSTALL_DIR) $(1)/usr/lib/lua/luci/model/cbi/admin_ethernet/switchConfig
	$(INSTALL_DIR) $(1)/usr/lib/lua/luci/view/admin_ethernet/switchConfig
	$(INSTALL_DIR) $(1)/usr/lib/lua/luci/view/admin_ethernet/maintenance
	$(INSTALL_DIR) $(1)/usr/lib/lua/luci/view/admin_ethernet/system
	$(INSTALL_DIR) $(1)/usr/lib/lua/luci/view/admin_ethernet/traps
	$(INSTALL_BIN) ./controller/*  $(1)/usr/lib/lua/luci/controller/admin
	$(INSTALL_BIN) ./module/switchConfig/*  $(1)/usr/lib/lua/luci/model/cbi/admin_ethernet/switchConfig/
	$(INSTALL_BIN) ./view/switchConfig/*  $(1)/usr/lib/lua/luci/view/admin_ethernet/switchConfig/
	$(INSTALL_BIN) ./view/maintenance/*  $(1)/usr/lib/lua/luci/view/admin_ethernet/maintenance/
	$(INSTALL_BIN) ./view/system/*  $(1)/usr/lib/lua/luci/view/admin_ethernet/system/
	$(INSTALL_BIN) ./luci/*  $(1)/usr/lib/lua/luci/
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) ./jwcheck.sh  $(1)/usr/sbin/
endef

$(eval $(call BuildPackage,jwweb))
