#
# Copyright (C) 2013-2014 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=u-boot
PKG_VERSION:=2015.04
PKG_RELEASE:=1

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(BUILD_VARIANT)/$(PKG_NAME)-$(PKG_VERSION)
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
#PKG_SOURCE_URL:= \
	http://mirror2.openwrt.org/sources \
	ftp://ftp.denx.de/pub/u-boot
#PKG_MD5SUM:=6d2116d1385a66e9a59742caa9d62a54

PKG_BUILD_PARALLEL:=1

include $(INCLUDE_DIR)/package.mk

define uboot/Default
  TITLE:=
  CONFIG:=
  IMAGE:=
endef

define uboot/mx6ul_14x14_evk
  TITLE:=U-Boot for Freescale i.MX6 Ultra Lite 1GB board
endef

define uboot/mx6ul_14x14_evk_ge_z20204a
  TITLE:=U-Boot for joyware switch ge-z20204a board base on Freescale i.MX6 Ultra Lite
endef

define uboot/mx6ul_14x14_evk_ge_z20505d
  TITLE:=U-Boot for joyware switch ge-z20505d board base on Freescale i.MX6 Ultra Lite
endef

define uboot/mx6ul_14x14_evk_ge_z20208d
  TITLE:=U-Boot for joyware switch ge-z20208d board base on Freescale i.MX6 Ultra Lite
endef

define uboot/mx6ul_14x14_evk_ge_z20210d
  TITLE:=U-Boot for joyware switch ge-z20210d board base on Freescale i.MX6 Ultra Lite
endef

define uboot/mx6ul_14x14_evk_ge_z30208d
  TITLE:=U-Boot for joyware switch ge-z30208d board base on Freescale i.MX6 Ultra Lite
endef

define uboot/mx6ul_14x14_evk_gd_sdi1000m
  TITLE:=U-Boot for joyware switch gd-sdi1000m board base on Freescale i.MX6 Ultra Lite
endef

UBOOTS := \
	mx6ul_14x14_evk \
	mx6ul_14x14_evk_ge_z20204a \
	mx6ul_14x14_evk_ge_z20505d \
	mx6ul_14x14_evk_ge_z20208d \
	mx6ul_14x14_evk_ge_z20210d \
	mx6ul_14x14_evk_ge_z30208d \
	mx6ul_14x14_evk_gd_sdi1000m

define Package/uboot/template
define Package/uboot-imx6ul-$(1)
  SECTION:=boot
  CATEGORY:=Boot Loaders
  DEPENDS:=@TARGET_imx6ul
  TITLE:=$(2)
  URL:=http://www.denx.de/wiki/U-Boot
  VARIANT:=$(1)
  MAINTAINER:=Luka Perkov <luka@openwrt.org>
endef
endef

define BuildUBootPackage
	$(eval $(uboot/Default))
	$(eval $(uboot/$(1)))
	$(call Package/uboot/template,$(1),$(TITLE))
endef

ifdef BUILD_VARIANT
$(eval $(call uboot/$(BUILD_VARIANT)))
UBOOT_CONFIG:=$(if $(CONFIG),$(CONFIG),$(BUILD_VARIANT))
UBOOT_IMAGE:=$(if $(IMAGE),$(IMAGE),openwrt-$(BOARD)-$(BUILD_VARIANT)-u-boot.bin)
endif

define Build/Configure
	+$(MAKE) $(PKG_JOBS) -C $(PKG_BUILD_DIR) \
		$(UBOOT_CONFIG)_config
endef

define Build/Compile
	+$(MAKE) $(PKG_JOBS) -C $(PKG_BUILD_DIR) \
		CROSS_COMPILE=$(TARGET_CROSS)
endef

define Package/uboot/install/default
	$(INSTALL_DIR) $(BIN_DIR)/uboot-$(BOARD)-$(1)
	$(CP) \
		$(PKG_BUILD_DIR)/u-boot.imx \
		$(BIN_DIR)/uboot-$(BOARD)-$(1)/openwrt-$(BOARD)-$(1)-u-boot.imx
endef

define Package/uboot/install/template
define Package/uboot-imx6ul-$(1)/install
	$(call Package/uboot/install/default,$(2))
endef
endef

$(foreach u,$(UBOOTS), \
	$(eval $(call Package/uboot/install/template,$(u),$(u))) \
)

$(foreach u,$(UBOOTS), \
	$(eval $(call BuildUBootPackage,$(u))) \
	$(eval $(call BuildPackage,uboot-imx6ul-$(u))) \
)
