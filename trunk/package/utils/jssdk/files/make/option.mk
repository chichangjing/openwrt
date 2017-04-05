
# ***********************************************************
#    Copyright (C) 2016 Joyware
# ***********************************************************

ifeq (TRUE, $(IN_PORTCONTROL))
  MODULE_CFLAG += -DIN_PORTCONTROL
endif

ifeq (TRUE, $(IN_MIB))
  MODULE_CFLAG += -DIN_MIB
endif

ifeq (TRUE, $(IN_RATE))
  MODULE_CFLAG += -DIN_RATE
endif

ifeq (TRUE, $(IN_QOS))
  MODULE_CFLAG += -DIN_QOS
endif

ifeq (TRUE, $(IN_MIRROR))
  MODULE_CFLAG += -DIN_MIRROR
endif

ifeq (TRUE, $(IN_PORTVLAN))
  MODULE_CFLAG += -DIN_PORTVLAN
endif

ifeq (TRUE, $(IN_VLAN))
  MODULE_CFLAG += -DIN_VLAN
endif

ifeq (TRUE, $(IN_FDB))
  MODULE_CFLAG += -DIN_FDB
endif

ifeq (TRUE, $(API_LOCK))
  MODULE_CFLAG += -DAPI_LOCK
endif

ifeq (TRUE, $(DEBUG_ON))
  MODULE_CFLAG += -g 
endif

ifeq (TRUE, $(IN_TRUNK))
  MODULE_CFLAG += -DIN_TRUNK
endif

ifeq (TRUE, $(IN_MCAST))
  MODULE_CFLAG += -DIN_MCAST
endif

MODULE_CFLAG += -DVERSION=\"$(JSSDK_BUILD_VERSION)\" -DBUILD_DATE=\"$(JSSDK_BUILD_DATE)\" -DCPU=\"$(CPU)\" -DOS=\"$(OS)\"

MODULE_INC += -I$(PRJ_PATH)/include \
              -I$(PRJ_PATH)/include/common \
              -I$(PRJ_PATH)/include/api \
              -I$(PRJ_PATH)/include/fal \
              -I$(PRJ_PATH)/include/hsl \
              -I$(PRJ_PATH)/include/sal \
              -I$(PRJ_PATH)/include/sal/os \
              -I$(PRJ_PATH)/include/sal/sd \
              -I$(PRJ_PATH)/include/init \
              -I$(KERNEL_DIR)/include

ifneq (,$(findstring MARVELL_88E6097, $(SUPPORT_CHIP)))
  MODULE_INC   += -I$(PRJ_PATH)/include/hsl/mv88e6097 \
                  -I$(PRJ_PATH)/include/dsdt \
                  -I$(PRJ_PATH)/include/dsdt/h/driver \
                  -I$(PRJ_PATH)/include/dsdt/h/msApi \
                  -I$(PRJ_PATH)/include/dsdt/h/platform \
                  -I$(PRJ_PATH)/include/dsdt/h/porting
  MODULE_CFLAG += -DMARVELL_88E6097 -DLINUX
#-DDEBUG_QD
endif

ifneq (,$(findstring MARVELL_3236, $(SUPPORT_CHIP)))
  MODULE_INC   += -I$(PRJ_PATH)/include/hsl/mv3236 \
                  -I$(PRJ_PATH)/include/hsl/mv3236/cpsslib \
                  -I$(STAGING_DIR)/usr/include/jwcpss/mainPpDrv/h\
                  -I$(STAGING_DIR)/usr/include/jwcpss/common/h\
                  -I$(STAGING_DIR)/usr/include/jwcpss/mainOs/h\
                  
  MODULE_CFLAG += -DMARVELL_3236 -DLINUX -DCPU_LE
  LIBFLAGS +=--whole-archive  $(TOPDIR)/package/utils/jwcpss/compilation_root/cpssEnabler/libs/ARMV7/mainSysConfig.a --no-whole-archive
#-DDEBUG_QD
endif

ifeq (SHELL, $(MODULE_TYPE))
  ifeq (TRUE, $(KERNEL_MODE))
    MODULE_CFLAG += -static
  else
    MODULE_CFLAG += -static -DUSER_MODE
  endif

  MODULE_INC += -I$(PRJ_PATH)/include/shell
endif

ifneq (TRUE, $(KERNEL_MODE))
  ifneq (SHELL, $(MODULE_TYPE))
    MODULE_CFLAG += -DUSER_MODE
  endif
endif

CPU_CFLAGS += $(CFLAGS) $(MODULE_INC) $(MODULE_CFLAG) 

