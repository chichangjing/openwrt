# -*-makefile-*-
HOME_ROMFS	= $(DIST_HOME)/rootfs
MODULES_DIR	= $(HOME_ROMFS)/lib/modules


CFLAGS += -I./include -I$(USER_BASE)/mainPpDrv/h -I$(USER_BASE)/common/h
#CFLAGS += -I$(XCOMP_ROOT_PATH)/ppc_82xx/usr/include
CFLAGS += -I$(XCOMP_INC_PATH)/ppc_82xx/usr/include

EXTRA_FLAGS += CFLAGS

TARGET = mvPpDrv

# Kernel module should share PP device for multiple users with shared memory
ifeq (1,$(SHARED_MEMORY))
	CFLAGS += -DSHARED_MEMORY
endif

ifneq ($(KERNELRELEASE),)
# call from kernel build system

# disable debug info
CFLAGS:=$(filter-out -g,$(CFLAGS))

obj-m := ${TARGET}.o
${TARGET}-objs := prestera.o presteraIrq.o

else

default:
	$(MAKE) -C $(DIST_HOME) M=$(shell pwd) modules
	echo -e "\tcp "$(TARGET)".ko "$(MODULES_DIR)
	cp $(TARGET).ko $(MODULES_DIR)
endif

clean:
	rm -rf *.o *~ core .depend .*.cmd *.ko *.mod.c .tmp_versions

depend .depend dep:
	$(CC) $(CFLAGS) -M *.c > .depend


ifeq (.depend,$(wildcard .depend))
include .depend
endif
