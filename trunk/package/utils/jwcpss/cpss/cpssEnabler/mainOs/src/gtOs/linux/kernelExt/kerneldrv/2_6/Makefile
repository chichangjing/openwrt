# -*-makefile-*-
HOME_ROMFS	= $(DIST_HOME)/rootfs
MODULES_DIR	= $(HOME_ROMFS)/lib/modules
DEVICES_DIR	= $(HOME_ROMFS)/dev

TARGET = mvKernelExt
# flag kernel patches
ifeq (yes,$(KERNEL_SYSCALL_TABLE_EXPORTED))
	EXTRA_CFLAGS += -DMVKERNELEXT_SYSCALLS
endif
ifeq (yes,$(KERNEL_TASK_STRUCT_EXTENDED))
	EXTRA_CFLAGS += -DMVKERNELEXT_TASK_STRUCT
endif
ifeq (1, $(LINUX_SIM))
	EXTRA_CFLAGS += -DLINUX_SIM
endif
ifeq (arm, $(CPU_TOOL))
	EXTRA_CFLAGS += -DCPU_ARM
endif
ifneq (,$(filter 3.%,$(KERNELRELEASE)))
	MV_REALTIME_SCEDULLING_REQUIRED = yes
endif
ifneq (,$(MV_REALTIME_SCEDULLING_REQUIRED))
	EXTRA_CFLAGS += -DENABLE_REALTIME_SCHEDULING_POLICY
endif


ifneq ($(KERNELRELEASE),)
# call from kernel build system

obj-m := ${TARGET}.o
${TARGET}-objs := mv_KernelExt.o

else

	# import PREFIX and POSTFIX
	ifneq (,$(wildcard $(USER_BASE)/gtDefsTopMake.mk))
		C_FILE_LIST=NONE
		C_EXCLUDE_FILE_LIST=NONE
		include $(USER_BASE)/gtDefsTopMake.mk
	endif

all: default

default: clean
	$(MAKE) -C $(DIST_HOME) M=$(shell pwd) modules
	@echo -e "\tstripping debug information from $(TARGET).ko"
	@$(PREFIX)strip$(POSTFIX) -g $(TARGET).ko
ifneq (,$(wildcard $(HOME_ROMFS)))
	@echo -e "\tinstalling module to $(HOME_ROMFS)"
	@sh inst_mod.sh $(TARGET).ko $(HOME_ROMFS)
endif
endif

clean:
	rm -rf *.o *~ core .depend .*.cmd *.ko *.mod.c .tmp_versions

depend .depend dep:
	$(CC) $(EXTRA_CFLAGS) -M *.c > .depend


ifeq (.depend,$(wildcard .depend))
include .depend
endif
