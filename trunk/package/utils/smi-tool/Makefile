#
# Copyright (C) 2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=smi-tool
PKG_VERSION:=1.0
PKG_RELEASE:=1

PKG_BUILD_DIR := $(BUILD_DIR)/$(PKG_NAME)

include $(INCLUDE_DIR)/package.mk

define Package/smi-tool
  SECTION:=utils
  CATEGORY:=Utilities
  TITLE:=Joyware industry switcher smi interface access library and tool
  DEPENDS:=
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)/
endef

define Build/Configure
endef

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) \
		CC="$(TARGET_CC)" \
		LD="$(TARGET_CROSS)ld" \
		CFLAGS="$(filter-out -Werror=%,$(TARGET_CFLAGS)) -Wall" \
		LDFLAGS="$(TARGET_LDFLAGS)"
endef

define Package/smi-tool/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/smi-tool $(1)/usr/sbin/
endef

$(eval $(call BuildPackage,smi-tool))
