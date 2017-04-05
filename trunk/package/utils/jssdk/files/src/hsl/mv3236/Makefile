LOC_DIR=src/hsl/mv3236
LIB=HSL

include $(PRJ_PATH)/make/config.mk

SRC_LIST=mv3236_reg_access.c mv3236_init.c

ifeq (TRUE, $(IN_PORTCONTROL))
  SRC_LIST += mv3236_port_ctrl.c
endif

ifeq (TRUE, $(IN_MIB))
  SRC_LIST += mv3236_mib.c
endif

ifeq (TRUE, $(IN_MIRROR))
  SRC_LIST += mv3236_mirror.c
endif

ifeq (TRUE, $(IN_TRUNK))
  SRC_LIST += mv3236_trunk.c
endif

ifeq (TRUE, $(IN_RATE))
  SRC_LIST += mv3236_rate.c
endif

ifeq (TRUE, $(IN_MCAST))
  SRC_LIST += mv3236_multicast.c
endif

ifeq (TRUE, $(IN_VLAN))
  SRC_LIST += mv3236_vlan.c
endif

ifeq (TRUE, $(IN_QOS))
  SRC_LIST += mv3236_qos.c
endif

ifeq (TRUE, $(IN_FDB))
  SRC_LIST += mv3236_mac.c
endif

ifeq (linux, $(OS))
  ifeq (KSLIB, $(MODULE_TYPE))
    ifneq (TRUE, $(KERNEL_MODE))
      SRC_LIST=mv3236_reg_access.c mv3236_init.c
    endif
  endif

endif

ifeq (, $(findstring MARVELL_3236, $(SUPPORT_CHIP)))
  SRC_LIST=
endif


include $(PRJ_PATH)/make/components.mk
include $(PRJ_PATH)/make/defs.mk

include $(PRJ_PATH)/make/target.mk

all: dep obj