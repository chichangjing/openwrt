# -*-makefile-*-
HOME_ROMFS	= $(DIST_HOME)/rootfs
MODULES_DIR	= $(HOME_ROMFS)/lib/modules


# Kernel module should share PP device for multiple users with shared memory
ifeq (1,$(SHARED_MEMORY))
	EXTRA_CFLAGS += -DSHARED_MEMORY
endif
# flag kernel patches
ifeq (yes,$(KERNEL_SYSCALL_TABLE_EXPORTED))
	EXTRA_CFLAGS += -DPRESTERA_SYSCALLS
endif

EXTRA_CFLAGS += -I./include -I$(USER_BASE)/mainPpDrv/h -I$(USER_BASE)/common/h
EXTRA_CFLAGS += -I$(USER_BASE)/cpssEnabler/mainExtDrv/h
EXTRA_CFLAGS += -I$(XCOMP_ROOT_PATH)/usr/include

EXTRA_FLAGS += EXTRA_CFLAGS

TARGET = mvPpDrv

ifneq ($(KERNELRELEASE),)
# call from kernel build system

## disable debug info
#EXTRA_CFLAGS:=$(filter-out -g,$(EXTRA_CFLAGS))

obj-m := ${TARGET}.o
${TARGET}-objs := prestera.o presteraIrq.o presteraSmi.o presteraSmiIrq.o \
	presteraPpDriverPci.o presteraPpDriverPciHalf.o presteraPpDriverPexMbus.o

else

default:
	$(MAKE) -C $(DIST_HOME) M=$(shell pwd) modules
	echo -e "\tcp "$(TARGET)".ko "$(MODULES_DIR)
	cp $(TARGET).ko $(MODULES_DIR)
endif

clean:
	rm -rf *.o *~ core .depend .*.cmd *.ko *.mod.c .tmp_versions

depend .depend dep:
	$(CC) $(EXTRA_CFLAGS) -M *.c > .depend


ifeq (.depend,$(wildcard .depend))
include .depend
endif
