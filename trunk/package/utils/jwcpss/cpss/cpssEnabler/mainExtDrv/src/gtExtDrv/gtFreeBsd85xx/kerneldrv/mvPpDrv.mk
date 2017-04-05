SRCS	=prestera.c presteraIrq.c
KMOD	=mvPpDrv
TARGET	=mvPpDrv.ko

SYSDIR       =${KERNEL_TREE}/src/sys
MACHINE_ARCH =powerpc
MACHINE      =powerpc
OBJDIR       =${KERNEL_TREE}/obj-${MACHINE_ARCH}
CPUTYPE      =e500
CC           =ppc-fbsd-e500-gcc --no-mv-frontend
LD           =ppc-fbsd-e500-ld --no-mv-frontend
NM           =ppc-fbsd-e500-nm
OBJCOPY      =ppc-fbsd-e500-objcopy

## there is system bug that make's global variable .TARGETS getting value = w 
## in addition to real targets needed. here is the rule fixing that
w:
	@echo "bug fixing of .TARGETS w"

## this target appeares when you try to compile with option -s
ws:
	@echo "bug fixing of .TARGETS ws"

s:
	@echo "bug fixing of .TARGETS s"

.include <${KERNEL_TREE}/src/share/mk/bsd.kmod.mk>

CFLAGS +=-DNO_WERROR -DMV_DRIVERS -Wall -Wno-error -I${OBJDIR}/sys/GENERIC -I${SYSDIR}/powerpc/e500/LSP
CFLAGS +=-I${SYSDIR}/powerpc/include -I./include
CFLAGS +=-I$(USER_BASE)/mainPpDrv/h -I$(USER_BASE)/common/h -I$(USER_BASE)/cpssEnabler/mainExtDrv/h

