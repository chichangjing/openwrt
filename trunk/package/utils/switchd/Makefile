#
# Copyright (C) 2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
PKG_NAME:=switchd
PKG_VERSION:=1.0
PKG_RELEASE:=1

PKG_BUILD_DIR := $(BUILD_DIR)/$(PKG_NAME)
include $(INCLUDE_DIR)/package.mk

ifeq ($(CONFIG_JWS_CPU_MV3336),y)
product_type:=mv3336
export product_type
endif

define Package/switchd
  SECTION:=utils
  CATEGORY:=Utilities
  TITLE:=OBtelecom industry switcher ubus service processor
  
ifeq ($(product_type),mv3336)
  	DEPENDS:=+jshn +libjson-c +libblobmsg-json +libubox +libubus  +libjssdk_us +libjwsconvert +libuci +libjwf  +libcpss +libhelper
else
	DEPENDS:=+jshn +libjson-c +libblobmsg-json +libubox +libubus  +libjssdk_us +libjwsconvert +libuci +libjwf
endif
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
		CFLAGS="$(TARGET_CFLAGS) -Wall" \
		LDFLAGS="$(TARGET_LDFLAGS)"
ifeq ($(product_type),mv3336)
	CFLAGS+=-DMarvel3236
endif
endef

SERIAL_NUMBER=$(subst ",,$(CONFIG_JWS_SERIAL_NUM))
CONF_FILE=./files/etc/config/switchd.$(SERIAL_NUMBER)

define Package/switchd/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/switchd $(1)/usr/sbin/
	cp $(CONF_FILE) ./files/switchd
	$(INSTALL_DIR) $(1)/etc/config
	$(INSTALL_DATA) ./files/switchd $(1)/etc/config/
endef

$(eval $(call BuildPackage,switchd))
