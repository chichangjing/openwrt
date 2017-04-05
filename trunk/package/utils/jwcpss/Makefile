#
# Copyright (C) 2016 Joyware
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=jwcpss
PKG_VERSION:=1.0.0
PKG_RELEASE:=1

PKG_BUILD_DIR := $(BUILD_DIR)/$(PKG_NAME)

include $(INCLUDE_DIR)/package.mk

define Package/libcpss
    SECTION:=utils
    CATEGORY:=Utilities
    TITLE:=OBtelecom Marvel 3336/3236 cpss sdk lib
    DEPENDS:=
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	mkdir -p $(PKG_BUILD_DIR)/mainPpDrv
	mkdir -p $(PKG_BUILD_DIR)/common
	mkdir -p $(PKG_BUILD_DIR)/cpssEnabler/mainOs/
	mkdir -p $(PKG_BUILD_DIR)/cpssEnabler/mainExtDrv/
	
	$(CP) ./cpss/mainPpDrv/h $(PKG_BUILD_DIR)/mainPpDrv
	$(CP) ./cpss/common/h $(PKG_BUILD_DIR)/common
	$(CP) ./cpss/cpssEnabler/mainOs/h $(PKG_BUILD_DIR)/cpssEnabler/mainOs/
	$(CP) ./cpss/cpssEnabler/mainExtDrv/h $(PKG_BUILD_DIR)/cpssEnabler/mainExtDrv/
endef

define Build/Configure
endef

define Build/Compile
	./build_only_app.sh
endef

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include/$(PKG_NAME)
	$(CP) $(PKG_BUILD_DIR)/{mainPpDrv,common,cpssEnabler} $(1)/usr/include/$(PKG_NAME)/
	$(INSTALL_DIR) $(1)/usr/lib
#	$(CP) -d compilation_root/libcpss.so.0       $(1)/usr/lib/libcpss.so.0
	$(CP) -d compilation_root/libcpss.so.0.0.0   $(1)/usr/lib/libcpss.so
	$(CP) -d compilation_root/libhelper.so.0.0.0   $(1)/usr/lib/libhelper.so
#	$(CP) -d compilation_root/libhelper.so.0.0.0 $(1)/usr/lib/libhelper.so.0.0.0
#	$(CP) -d compilation_root/libhelper.so.0     $(1)/usr/lib/libhelper.so.0 
#	$(CP) -d compilation_root/libhelper.so       $(1)/usr/lib/libhelper.so
endef

define Package/libcpss/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(INSTALL_BIN) compilation_root/libcpss.so.0.0.0   $(1)/usr/lib/libcpss.so
#	$(INSTALL_BIN) compilation_root/libcpss.so.0         $(1)/usr/lib/libcpss.so.0
#	$(INSTALL_BIN) compilation_root/libcpss.so.0.0.0   $(1)/usr/lib/libcpss.so.0.0.0
#	$(INSTALL_BIN) compilation_root/libhelper.so.0.0.0 $(1)/usr/lib/libhelper.so.0.0.0
#	$(INSTALL_BIN) compilation_root/libhelper.so.0     $(1)/usr/lib/libhelper.so.0 
	$(INSTALL_BIN) compilation_root/libhelper.so.0.0.0       $(1)/usr/lib/libhelper.so

	ln -s $(1)/usr/lib/libhelper.so $(1)/usr/lib/libhelper.so.0

	ln -s $(1)/usr/lib/libcpss.so $(1)/usr/lib/libcpss.so.0
endef

$(eval $(call BuildPackage,libcpss))
