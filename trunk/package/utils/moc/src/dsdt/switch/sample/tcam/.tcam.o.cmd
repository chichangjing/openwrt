cmd_drivers/net/../../../../mvl_drivers/mv_switch/../qd-dsdt/sample/tcam/tcam.o := /usr/local/arm-2007q3/bin/arm-none-linux-gnueabi-gcc -Wp,-MD,drivers/net/../../../../mvl_drivers/mv_switch/../qd-dsdt/sample/tcam/.tcam.o.d  -nostdinc -isystem /usr/local/arm-2007q3/bin/../lib/gcc/arm-none-linux-gnueabi/4.2.1/include -Iinclude  -I/mnt/3057647c-a78f-41f7-ad61-b05dbf18c4be/marvell/kernel/linux-2.6.30.10/arch/arm/include -include include/linux/autoconf.h -D__KERNEL__ -mlittle-endian -Iarch/arm/mach-ffxav/include -Wall -Wundef -Wstrict-prototypes -Wno-trigraphs -fno-strict-aliasing -fno-common -Werror-implicit-function-declaration -fno-delete-null-pointer-checks -O2 -marm -mabi=aapcs-linux -mno-thumb-interwork -funwind-tables -D__LINUX_ARM_ARCH__=5 -march=armv5te -mtune=arm9tdmi -msoft-float -Uarm -fno-stack-protector -fomit-frame-pointer -Wdeclaration-after-statement -Wno-pointer-sign -fno-strict-overflow -DMVL_DSDT_DRV -I/mnt/3057647c-a78f-41f7-ad61-b05dbf18c4be/marvell/kernel/linux-2.6.30.10/../../mvl_drivers/inc -I/mnt/3057647c-a78f-41f7-ad61-b05dbf18c4be/marvell/kernel/linux-2.6.30.10/../../mvl_drivers/mv_switch -DLINUX -I/mnt/3057647c-a78f-41f7-ad61-b05dbf18c4be/marvell/kernel/linux-2.6.30.10/../../mvl_drivers/mad/Include -I/mnt/3057647c-a78f-41f7-ad61-b05dbf18c4be/marvell/kernel/linux-2.6.30.10/../../mvl_drivers/mad/Include/h/driver -DMVL_FFXAV -I/mnt/3057647c-a78f-41f7-ad61-b05dbf18c4be/marvell/kernel/linux-2.6.30.10/../../mvl_drivers/qd-dsdt/Include -I/mnt/3057647c-a78f-41f7-ad61-b05dbf18c4be/marvell/kernel/linux-2.6.30.10/../../mvl_drivers/qd-dsdt/Include/h/msApi -I/mnt/3057647c-a78f-41f7-ad61-b05dbf18c4be/marvell/kernel/linux-2.6.30.10/../../mvl_drivers/qd-dsdt/Include/h/driver -I/mnt/3057647c-a78f-41f7-ad61-b05dbf18c4be/marvell/kernel/linux-2.6.30.10/../../mvl_drivers/qd-dsdt/Include/h/platform -I/mnt/3057647c-a78f-41f7-ad61-b05dbf18c4be/marvell/kernel/linux-2.6.30.10/../../mvl_drivers/qd-dsdt/Diag -I/mnt/3057647c-a78f-41f7-ad61-b05dbf18c4be/marvell/kernel/linux-2.6.30.10/../../mvl_drivers/qd-dsdt/sample/Include -I../rmgmt   -D"KBUILD_STR(s)=\#s" -D"KBUILD_BASENAME=KBUILD_STR(tcam)"  -D"KBUILD_MODNAME=KBUILD_STR(mv_switch)"  -c -o drivers/net/../../../../mvl_drivers/mv_switch/../qd-dsdt/sample/tcam/tcam.o drivers/net/../../../../mvl_drivers/mv_switch/../qd-dsdt/sample/tcam/tcam.c

deps_drivers/net/../../../../mvl_drivers/mv_switch/../qd-dsdt/sample/tcam/tcam.o := \
  drivers/net/../../../../mvl_drivers/mv_switch/../qd-dsdt/sample/tcam/tcam.c \
  /mnt/3057647c-a78f-41f7-ad61-b05dbf18c4be/marvell/kernel/linux-2.6.30.10/../../mvl_drivers/qd-dsdt/Include/Copyright.h \
  /mnt/3057647c-a78f-41f7-ad61-b05dbf18c4be/marvell/kernel/linux-2.6.30.10/../../mvl_drivers/qd-dsdt/sample/Include/msSample.h \
  include/linux/string.h \
    $(wildcard include/config/binary/printf.h) \
  include/linux/compiler.h \
    $(wildcard include/config/trace/branch/profiling.h) \
    $(wildcard include/config/profile/all/branches.h) \
    $(wildcard include/config/enable/must/check.h) \
    $(wildcard include/config/enable/warn/deprecated.h) \
  include/linux/compiler-gcc.h \
    $(wildcard include/config/arch/supports/optimized/inlining.h) \
    $(wildcard include/config/optimize/inlining.h) \
  include/linux/compiler-gcc4.h \
  include/linux/types.h \
    $(wildcard include/config/uid16.h) \
    $(wildcard include/config/lbd.h) \
    $(wildcard include/config/phys/addr/t/64bit.h) \
    $(wildcard include/config/64bit.h) \
  /mnt/3057647c-a78f-41f7-ad61-b05dbf18c4be/marvell/kernel/linux-2.6.30.10/arch/arm/include/asm/types.h \
  include/asm-generic/int-ll64.h \
  include/linux/posix_types.h \
  include/linux/stddef.h \
  /mnt/3057647c-a78f-41f7-ad61-b05dbf18c4be/marvell/kernel/linux-2.6.30.10/arch/arm/include/asm/posix_types.h \
  /usr/local/arm-2007q3/bin/../lib/gcc/arm-none-linux-gnueabi/4.2.1/include/stdarg.h \
  /mnt/3057647c-a78f-41f7-ad61-b05dbf18c4be/marvell/kernel/linux-2.6.30.10/arch/arm/include/asm/string.h \
  include/linux/kernel.h \
    $(wildcard include/config/preempt/voluntary.h) \
    $(wildcard include/config/debug/spinlock/sleep.h) \
    $(wildcard include/config/prove/locking.h) \
    $(wildcard include/config/printk.h) \
    $(wildcard include/config/dynamic/debug.h) \
    $(wildcard include/config/ring/buffer.h) \
    $(wildcard include/config/tracing.h) \
    $(wildcard include/config/numa.h) \
    $(wildcard include/config/ftrace/mcount/record.h) \
  include/linux/linkage.h \
  /mnt/3057647c-a78f-41f7-ad61-b05dbf18c4be/marvell/kernel/linux-2.6.30.10/arch/arm/include/asm/linkage.h \
  include/linux/bitops.h \
    $(wildcard include/config/generic/find/first/bit.h) \
    $(wildcard include/config/generic/find/last/bit.h) \
    $(wildcard include/config/generic/find/next/bit.h) \
  /mnt/3057647c-a78f-41f7-ad61-b05dbf18c4be/marvell/kernel/linux-2.6.30.10/arch/arm/include/asm/bitops.h \
    $(wildcard include/config/smp.h) \
  /mnt/3057647c-a78f-41f7-ad61-b05dbf18c4be/marvell/kernel/linux-2.6.30.10/arch/arm/include/asm/system.h \
    $(wildcard include/config/cpu/xsc3.h) \
    $(wildcard include/config/cpu/fa526.h) \
    $(wildcard include/config/cpu/sa1100.h) \
    $(wildcard include/config/cpu/sa110.h) \
    $(wildcard include/config/cpu/32v6k.h) \
  include/linux/irqflags.h \
    $(wildcard include/config/trace/irqflags.h) \
    $(wildcard include/config/irqsoff/tracer.h) \
    $(wildcard include/config/preempt/tracer.h) \
    $(wildcard include/config/trace/irqflags/support.h) \
    $(wildcard include/config/x86.h) \
  include/linux/typecheck.h \
  /mnt/3057647c-a78f-41f7-ad61-b05dbf18c4be/marvell/kernel/linux-2.6.30.10/arch/arm/include/asm/irqflags.h \
  /mnt/3057647c-a78f-41f7-ad61-b05dbf18c4be/marvell/kernel/linux-2.6.30.10/arch/arm/include/asm/ptrace.h \
    $(wildcard include/config/arm/thumb.h) \
  /mnt/3057647c-a78f-41f7-ad61-b05dbf18c4be/marvell/kernel/linux-2.6.30.10/arch/arm/include/asm/hwcap.h \
  include/asm-generic/cmpxchg-local.h \
  include/asm-generic/cmpxchg.h \
  include/asm-generic/bitops/non-atomic.h \
  include/asm-generic/bitops/fls64.h \
  include/asm-generic/bitops/sched.h \
  include/asm-generic/bitops/hweight.h \
  include/asm-generic/bitops/lock.h \
  include/linux/log2.h \
    $(wildcard include/config/arch/has/ilog2/u32.h) \
    $(wildcard include/config/arch/has/ilog2/u64.h) \
  include/linux/ratelimit.h \
  include/linux/param.h \
  /mnt/3057647c-a78f-41f7-ad61-b05dbf18c4be/marvell/kernel/linux-2.6.30.10/arch/arm/include/asm/param.h \
    $(wildcard include/config/hz.h) \
  include/linux/dynamic_debug.h \
  include/asm/byteorder.h \
  include/linux/byteorder/little_endian.h \
  include/linux/swab.h \
  include/asm/swab.h \
  include/linux/byteorder/generic.h \
  /mnt/3057647c-a78f-41f7-ad61-b05dbf18c4be/marvell/kernel/linux-2.6.30.10/arch/arm/include/asm/bug.h \
    $(wildcard include/config/bug.h) \
    $(wildcard include/config/debug/bugverbose.h) \
  include/asm-generic/bug.h \
    $(wildcard include/config/generic/bug.h) \
    $(wildcard include/config/generic/bug/relative/pointers.h) \
  include/linux/random.h \
  include/linux/ioctl.h \
  /mnt/3057647c-a78f-41f7-ad61-b05dbf18c4be/marvell/kernel/linux-2.6.30.10/arch/arm/include/asm/ioctl.h \
  include/asm-generic/ioctl.h \
  include/linux/irqnr.h \
    $(wildcard include/config/generic/hardirqs.h) \
  /mnt/3057647c-a78f-41f7-ad61-b05dbf18c4be/marvell/kernel/linux-2.6.30.10/../../mvl_drivers/qd-dsdt/Include/h/platform/gtMiiSmiIf.h \
  /mnt/3057647c-a78f-41f7-ad61-b05dbf18c4be/marvell/kernel/linux-2.6.30.10/../../mvl_drivers/qd-dsdt/Include/msApi.h \
  /mnt/3057647c-a78f-41f7-ad61-b05dbf18c4be/marvell/kernel/linux-2.6.30.10/../../mvl_drivers/qd-dsdt/Include/msApiDefs.h \
    $(wildcard include/config/avb/fpga.h) \
    $(wildcard include/config/avb/fpga/2.h) \
    $(wildcard include/config/mode.h) \
    $(wildcard include/config/ptp/mode/reg.h) \
    $(wildcard include/config/ptp/external/time/form.h) \
    $(wildcard include/config/1588/ing/act/vec.h) \
    $(wildcard include/config/1588/eg/act/vec.h) \
    $(wildcard include/config/as/ing/act/vec.h) \
    $(wildcard include/config/as/eg/act/vec.h) \
    $(wildcard include/config/pointer.h) \
    $(wildcard include/config/data/0.h) \
    $(wildcard include/config/data/1.h) \
    $(wildcard include/config/data/2.h) \
    $(wildcard include/config/data/3.h) \
    $(wildcard include/config/data.h) \
  /mnt/3057647c-a78f-41f7-ad61-b05dbf18c4be/marvell/kernel/linux-2.6.30.10/../../mvl_drivers/qd-dsdt/Include/msApiSelect.h \
  /mnt/3057647c-a78f-41f7-ad61-b05dbf18c4be/marvell/kernel/linux-2.6.30.10/../../mvl_drivers/qd-dsdt/Include/msApiTypes.h \
  /mnt/3057647c-a78f-41f7-ad61-b05dbf18c4be/marvell/kernel/linux-2.6.30.10/../../mvl_drivers/qd-dsdt/Include/h/msApi/msApiInternal.h \
    $(wildcard include/config/1.h) \
    $(wildcard include/config/2.h) \
  /mnt/3057647c-a78f-41f7-ad61-b05dbf18c4be/marvell/kernel/linux-2.6.30.10/../../mvl_drivers/qd-dsdt/Include/msApiPrototype.h \
    $(wildcard include/config/dtta.h) \
  /mnt/3057647c-a78f-41f7-ad61-b05dbf18c4be/marvell/kernel/linux-2.6.30.10/../../mvl_drivers/qd-dsdt/Include/h/driver/gtHwCntl.h \

drivers/net/../../../../mvl_drivers/mv_switch/../qd-dsdt/sample/tcam/tcam.o: $(deps_drivers/net/../../../../mvl_drivers/mv_switch/../qd-dsdt/sample/tcam/tcam.o)

$(deps_drivers/net/../../../../mvl_drivers/mv_switch/../qd-dsdt/sample/tcam/tcam.o):
