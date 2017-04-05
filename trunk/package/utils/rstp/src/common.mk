#
# Copyright (C) 2016 Joyware
#

ifndef PLATFORM
	PLATFORM = ARM_IMX6UL
endif

ifeq (ARM_IMX6UL, $(PLATFORM))
  CROSS_COMPILE = arm-openwrt-linux-muslgnueabi-
else
  $(error PLATFORM isn't support!)
endif

MAKE = make
CC = $(CROSS_COMPILE)gcc
LD = $(CROSS_COMPILE)ld
AR = $(CROSS_COMPILE)gcc-ar
STRIP = $(CROSS_COMPILE)strip
RANLIB = $(CROSS_COMPILE)ranlib
