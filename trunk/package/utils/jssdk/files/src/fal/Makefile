LOC_DIR=src/fal
LIB=FAL

include $(PRJ_PATH)/make/config.mk

SRC_LIST=fal_init.c fal_reg_access.c

ifeq (TRUE, $(IN_PORTCONTROL))
  SRC_LIST += fal_port_ctrl.c
endif

ifeq (TRUE, $(IN_MIB))
  SRC_LIST += fal_mib.c
endif

ifeq (TRUE, $(IN_FDB))
  SRC_LIST += fal_mac.c
endif

ifeq (TRUE, $(IN_MIRROR))
  SRC_LIST += fal_mirror.c
endif

ifeq (TRUE, $(IN_MISC))
  SRC_LIST += fal_misc.c
endif



ifeq (TRUE, $(IN_PORTVLAN))
  SRC_LIST += fal_portvlan.c
endif

ifeq (TRUE, $(IN_QOS))
  SRC_LIST += fal_qos.c
endif

ifeq (TRUE, $(IN_RATE))
  SRC_LIST += fal_rate.c
endif

ifeq (TRUE, $(IN_STP))
  SRC_LIST += fal_stp.c
endif

ifeq (TRUE, $(IN_VLAN))
  SRC_LIST += fal_vlan.c
endif

ifeq (TRUE, $(IN_TRUNK))
  SRC_LIST += fal_trunk.c
endif

ifeq (TRUE, $(IN_MCAST))
  SRC_LIST += fal_multicast.c
endif

include $(PRJ_PATH)/make/components.mk
include $(PRJ_PATH)/make/defs.mk
include $(PRJ_PATH)/make/target.mk

all: dep obj