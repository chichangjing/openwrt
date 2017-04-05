
# ***********************************************************
#    Copyright (C) 2016 Joyware
# ***********************************************************

LOC_DIR=src/shell
LIB=SHELL

include $(PRJ_PATH)/make/config.mk
include $(PRJ_PATH)/make/components.mk

SRC_LIST=$(wildcard *.c)
ifeq (,$(findstring SHELL, $(COMPONENTS)))
all: dep obj
else
all: dep obj lib
endif

include $(PRJ_PATH)/make/defs.mk
include $(PRJ_PATH)/make/target.mk

ifeq (TRUE, $(API_LOCK))
    PT_LIB=-lpthread
else
    PT_LIB=
endif

lib:
ifneq (,$(findstring MARVELL_3236, $(SUPPORT_CHIP)))
	echo "run here2"
	$(CC) $(OBJ_FILE) $(JSSDK_BIN_DIR)/$(JSSDK_US_MOD).a -o $(DST_DIR)/$(JSSDK_SHELL_OBJ) $(PT_LIB) $(TOPDIR)/package/utils/jwcpss/compilation_root/cpssEnabler/libs/ARMV7/mainSysConfig.a
else
	echo "run here"
	$(CC) $(CPU_CFLAGS) $(OBJ_FILE) $(JSSDK_BIN_DIR)/$(JSSDK_US_MOD).a -o $(DST_DIR)/$(JSSDK_SHELL_OBJ) $(PT_LIB)
endif 
#  -Wl,--verbose=99 
