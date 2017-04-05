# -*-makefile-*-

# call from kernel build system
TARGET = mvDmaDrv

ifneq ($(KERNELRELEASE),)

obj-m := ${TARGET}.o
${TARGET}-objs := driver.o

else
#
# CPSS make
HOME_ROMFS	= $(DIST_HOME)/rootfs
MODULES_DIR	= $(HOME_ROMFS)/lib/modules
INIT_SH     = $(HOME_ROMFS)/etc/init.sh
ifneq (ARMARCH7.ARMADA_390, $(CPU).$(ARM_CPU))
XTRA_RULE=-a ! -f /proc/mvPP
endif

all: default

default:
	$(MAKE) -C $(DIST_HOME) M=$(shell pwd) modules
	@echo -e "\tstripping debug information from $(TARGET).ko"
	$(PREFIX)strip$(POSTFIX) -g $(TARGET).ko
	cp $(TARGET).ko $(MODULES_DIR)
	grep -q mvDmaDrv $(INIT_SH) || sed -i \
		-e '/date/ i\' \
		-e ' # module to allocate DMAble memory\' \
		-e ' test -f /lib/modules/$(TARGET).ko $(XTRA_RULE) &&\' \
		-e '    insmod /lib/modules/$(TARGET).ko\' \
		-e '' \
		$(INIT_SH)

clean:
	rm -rf *.o *~ core .depend .*.cmd *.ko *.mod.c .tmp_versions

depend .depend dep:
	$(CC) $(EXTRA_CFLAGS) -M *.c > .depend

ifeq (.depend,$(wildcard .depend))
include .depend
endif

endif
