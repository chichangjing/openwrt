####################################################################
# Workbench DIAB Definitions
####################################################################

#start of include
FIS= -I
#end of include
FIE=
FD=-D
FO=-o
FC=-c
FG=-g
OBJ=o
ASM=s
LIBEXT=lib

#ifeq (ppc, $(CPU_TOOL))
#    ifneq (3, $(GCCVER))
#        CPU_FLAGS += -mlongcall
#    endif
#endif

#ifeq (arm, $(CPU_TOOL))
#  CPU_FLAGS = -DARMEL -DCPU_$(ARM_CPU) -DARMMMU=ARMMMU_$(ARM_MMU) -DARMCACHE=ARMCACHE_$(ARM_CACHE)
#  ifeq (BE,$(ENDIAN))
#    CC_ARCH += -mbig-endian
#  else
#    CPU_FLAGS += -DLE
#  endif
#  #Do we compile for thumb ?
#  ifeq  ($(findstring _T,$(CPU)),_T)
#    # definitions for Thumb mode
#    CPU_FLAGS += -mthumb -mthumb-interwork
#  endif
#endif

WORKBENCH_TOOL_INCLUDE = $(FIS)C:/Workbench_3.3/vxworks-6.9/target/h/wrn/coreip$(FIE) \
		   $(FIS)C:/Workbench_3.3/vxworks-6.9/target/h/types$(FIE) \
		   $(FIS)C:/Workbench_3.3/vxworks-6.9/target/h/arch/ppc$(FIE)

PROJ_BSP   = $(subst \,/,$(PROJ_BSP_DIR))
VX_ROOT    = $(subst \,/,$(WIND_BASE))
OS_CFLAGS  = -I$(VX_ROOT)/target/h -I$(VX_ROOT)/target/config/all -I$(PROJ_BSP)

ifeq (workbench, $(TOOL_ENV))
  OS_CFLAGS+= -I$(WIND_HOME)\components\ip_net2-6.9\ipcom\config
endif

COMPILER_ROOT = $(VX_ROOT)/host/x86-win32/lib/gcc-lib/
OS_ROOT       = $(VX_ROOT)
GNULIBROOT    = $(VX_ROOT)/host/$(WIND_HOST_TYPE)

TGT_DIR = $(WIND_BASE)/target
include $(TGT_DIR)/h/make/defs.bsp

# fix LDFLAGS possible changed in defs.bsp

ifeq (,$(filter -EB -EL,$(LDFLAGS)))
  ifeq (BE,$(ENDIAN))
    LDFLAGS:=-EB $(LDFLAGS)
  else
    LDFLAGS:=-EL $(LDFLAGS)
  endif
endif

ifeq (ppc, $(CPU_TOOL))
  LDFLAGS    = $(CC_ARCH_DIAB) -X
endif


ifeq (arm, $(CPU_TOOL))
  ifeq (diab, $(TOOL))
    LDFLAGS    = $(CC_ARCH_DIAB) -X
  endif
endif


#Add a C definition to distiguish between different OS (Added for FreeBSD)
ifdef OS_TARGET
  CFLAGS1 = $(CC_ARCH_DIAB) -DOS_TARGET=$(OS_TARGET)
else
  CFLAGS1 = $(CC_ARCH_DIAB) -DOS_TARGET=$(OS_RUN)
endif

CFLAGS2 = -ansi -fvolatile -fno-builtin

# suppress: warning 1606 - conditional expression or part of it is always true/false
# warning 1772 - possible redundant expression
CFLAGS2 += -ei1606,1772

#CFLAGS2 = -Xenum-is-best -Xeieio
ifeq (ppc, $(CPU_TOOL))
  #CFLAGS2 = -Xeieio
  # suppress: warning 1606 - conditional expression or part of it is always true/false
  # warning 1772 - possible redundant expression
  CFLAGS2 = -ei1606,1772
endif
#CFLAGS_OPT = 

BUILD_MAP_FILE= -m2 > 
