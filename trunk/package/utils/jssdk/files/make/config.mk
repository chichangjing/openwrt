
# ***********************************************************
#    Copyright (C) 2016 Joyware
# ***********************************************************

include $(PRJ_PATH)/config

ifndef CPU
  CPU=arm
endif

ifndef OS
  OS=linux
endif

ifeq ($(product_type),mv3336)
 	CHIP_TYPE = MARVELL_3236
endif 

ifndef CHIP_TYPE
  $(error CHIP_TYPE isn't defined!)
else
  ifeq (MARVELL_88E6097, $(CHIP_TYPE))
    SUPPORT_CHIP = MARVELL_88E6097
  endif

ifeq (MARVELL_3236, $(CHIP_TYPE))
   SUPPORT_CHIP = MARVELL_3236
endif
  
  ifndef SUPPORT_CHIP
    $(error defined CHIP_TYPE isn't supported!)
  endif
endif

ifeq (TRUE, $(KERNEL_MODE))
  RUNMODE=km
else
  RUNMODE=um
endif

DEBUG_ON=FALSE
JSSDK_SHELL_OBJ=jssdk_sh
JSSDK_US_MOD=libjssdk_us
JSSDK_KS_MOD=libjssdk_ks
JSSDK_BLD_DIR=$(PRJ_PATH)/build/$(OS)_$(CPU)
JSSDK_BIN_DIR=$(PRJ_PATH)/build/bin

VER=1.0.0
JSSDK_BUILD_VERSION=$(VER)
JSSDK_BUILD_DATE=$(shell date -u  +%F-%T)
ifeq ($(product_type),mv3336)
HSL_LIB_OBJ=libhsllib
endif