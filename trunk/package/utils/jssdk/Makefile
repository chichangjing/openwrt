#
# Copyright (C) 2016 Joyware
#
include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=jssdk
PKG_VERSION:=1.0.0
PKG_RELEASE:=1

PKG_BUILD_DIR := $(BUILD_DIR)/$(PKG_NAME)

ifeq ($(CONFIG_JWS_CPU_MV3336),y)
product_type:=mv3336
export product_type
endif

include $(INCLUDE_DIR)/package.mk

define Package/libjssdk_us
    SECTION:=utils
    CATEGORY:=Utilities
    TITLE:=Joyware Switch Software Development Kit
ifeq ($(CONFIG_JWS_CPU_MV3336),y)
    DEPENDS:=+libcpss +libhelper
endif
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./files/* $(PKG_BUILD_DIR)/
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
	$(CP) $(PKG_BUILD_DIR)/include/{fal,common,sal,init} $(1)/usr/include/$(PKG_NAME)/
	$(CP) $(PKG_BUILD_DIR)/board/jssdk_mode.h $(1)/usr/include/$(PKG_NAME)/
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_BUILD_DIR)/build/bin/libjssdk_us.{a,so*} $(1)/usr/lib/
endef

define Package/libjssdk_us/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/build/bin/libjssdk_us.so $(1)/usr/lib/
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/build/bin/jssdk_sh $(1)/usr/sbin/
endef

$(eval $(call BuildPackage,libjssdk_us))
