#
# Copyright (C) 2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=rmi-tool
PKG_VERSION:=1.0
PKG_RELEASE:=1

PKG_BUILD_DIR := $(BUILD_DIR)/$(PKG_NAME)

include $(INCLUDE_DIR)/package.mk

define Package/rmi-tool
  SECTION:=utils
  CATEGORY:=Utilities
  TITLE:=JoyWare Industry Switch Remote Management Interface
  DEPENDS:=+jshn +libjson-c +libblobmsg-json +libubox +libubus +libpthread
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)/
endef

define Build/Configure
endef

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) \
		KERNEL_DIR="$(LINUX_DIR)" \
		BUILD_DIR="$(BUILD_DIR)" \
		STAGING_DIR="$(STAGING_DIR)" \
		CC="$(TARGET_CC)" \
		LD="$(TARGET_CROSS)ld" \
		AR="$(TARGET_AR)" \
		STRIP="$(STRIP)" \
		CFLAGS="$(TARGET_CFLAGS) -Wall"
endef

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include/$(PKG_NAME)
	$(CP) $(PKG_BUILD_DIR)/*.h $(1)/usr/include/$(PKG_NAME)/
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_BUILD_DIR)/librmi.so $(1)/usr/lib/
endef

define Package/rmi-tool/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_BUILD_DIR)/librmi.so $(1)/usr/lib/
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/rmi-tool $(1)/usr/sbin/
endef

$(eval $(call BuildPackage,rmi-tool))
