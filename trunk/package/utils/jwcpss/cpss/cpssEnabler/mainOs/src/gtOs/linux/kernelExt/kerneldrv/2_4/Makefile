# -*-makefile-*-
HOME_ROMFS	= $(DIST_HOME)/rootfs
MODULES_DIR	= $(HOME_ROMFS)/lib/modules
DEVICES_DIR	= $(HOME_ROMFS)/dev

O_TARGET := mvKernelExt.o
# flag kernel patches
ifeq (yes,$(KERNEL_SYSCALL_TABLE_EXPORTED))
	EXTRA_CFLAGS += -DMVKERNELEXT_SYSCALLS
endif
ifeq (yes,$(KERNEL_TASK_STRUCT_EXTENDED))
	EXTRA_CFLAGS += -DMVKERNELEXT_TASK_STRUCT
endif

ifneq ($(KERNELRELEASE),)
# call from kernel build system

obj-y := mv_KernelExt.o

obj-m := $(O_TARGET)

include $(TOPDIR)/Rules.make

else

	# import PREFIX and POSTFIX
	ifneq (,$(wildcard $(USER_BASE)/gtDefsTopMake.mk))
		C_FILE_LIST=NONE
		C_EXCLUDE_FILE_LIST=NONE
		include $(USER_BASE)/gtDefsTopMake.mk
	endif

all: default

default: clean
	$(MAKE) -C $(DIST_HOME) SUBDIRS=$(shell pwd) modules
	@echo -e "\tstripping debug information from $(O_TARGET)"
	@$(PREFIX)strip$(POSTFIX) -g $(O_TARGET)
ifneq (,$(wildcard $(HOME_ROMFS)))
	@echo -e "\tinstalling module to $(HOME_ROMFS)"
	@sh inst_mod.sh $(O_TARGET) $(HOME_ROMFS)
endif
endif

clean:
	rm -rf *.o .*.o.flags *~ core .depend .*.cmd *.ko *.mod.c .tmp_versions

depend .depend dep:
	$(CC) $(CFLAGS) -M *.c > .depend


ifeq (.depend,$(wildcard .depend))
include .depend
endif
