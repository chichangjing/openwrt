LOC_DIR=src/hsl/mv88e6097
LIB=HSL

include $(PRJ_PATH)/make/config.mk

SRC_LIST=mv88e6097_reg_access.c mv88e6097_init.c

ifeq (TRUE, $(IN_PORTCONTROL))
  SRC_LIST += mv88e6097_port_ctrl.c
endif

ifeq (TRUE, $(IN_MIB))
  SRC_LIST += mv88e6097_mib.c
endif

ifeq (TRUE, $(IN_MIRROR))
  SRC_LIST += mv88e6097_mirror.c
endif

ifeq (TRUE, $(IN_TRUNK))
  SRC_LIST += mv88e6097_trunk.c
endif

ifeq (TRUE, $(IN_RATE))
  SRC_LIST += mv88e6097_rate.c
endif

ifeq (TRUE, $(IN_MCAST))
  SRC_LIST += mv88e6097_multicast.c
endif

ifeq (TRUE, $(IN_VLAN))
  SRC_LIST += mv88e6097_vlan.c
endif

ifeq (TRUE, $(IN_QOS))
  SRC_LIST += mv88e6097_qos.c
endif

ifeq (TRUE, $(IN_FDB))
  SRC_LIST += mv88e6097_mac.c
endif

ifeq (linux, $(OS))
  ifeq (KSLIB, $(MODULE_TYPE))
    ifneq (TRUE, $(KERNEL_MODE))
      SRC_LIST=mv88e6097_reg_access.c mv88e6097_init.c
    endif
  endif

endif

ifeq (, $(findstring MARVELL_88E6097, $(SUPPORT_CHIP)))
  SRC_LIST=
endif


include $(PRJ_PATH)/make/components.mk
include $(PRJ_PATH)/make/defs.mk

include $(PRJ_PATH)/make/target.mk

all: dep obj