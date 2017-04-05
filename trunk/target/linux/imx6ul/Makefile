#
# Copyright (C) 2013-2014 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

ARCH:=arm
BOARD:=imx6ul
BOARDNAME:=Freescale i.MX 6 Ultra Lite
#FEATURES:=audio display fpu gpio pcie rtc usb usbgadget squashfs targz nand ubifs
FEATURES:=fpu gpio rtc usb usbgadget squashfs targz nand ubifs
CPU_TYPE:=cortex-a7
CPU_SUBTYPE:=neon
MAINTAINER:=Xu Xin <xuxin@obtelecom.com>

KERNEL_PATCHVER:=3.14

include $(INCLUDE_DIR)/target.mk

KERNELNAME:=zImage dtbs

DEFAULT_PACKAGES += uboot-envtools

$(eval $(call BuildTarget))
