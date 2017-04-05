#
# Make definitions for Tornado
#

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

LDFLAGS= -X -N
export LDFLAGS

ifeq (ppc, $(CPU_TOOL))
    ifneq (3, $(GCCVER))
        CPU_FLAGS += -mlongcall
    endif
endif

ifeq (arm, $(CPU_TOOL))
  CPU_FLAGS = -DARMEL -DCPU_$(ARM_CPU) -DARMMMU=ARMMMU_$(ARM_MMU) -DARMCACHE=ARMCACHE_$(ARM_CACHE)
  ifeq (BE,$(ENDIAN))
    CC_ARCH += -mbig-endian
  else
    CPU_FLAGS += -DLE
  endif
  #Do we compile for thumb ?
  ifeq  ($(findstring _T,$(CPU)),_T)
    # definitions for Thumb mode
    CPU_FLAGS += -mthumb -mthumb-interwork
  endif
endif

#Add a C definition to distiguish between different OS (Added for FreeBSD)
ifdef OS_TARGET
  CFLAGS1 =  -Wall $(CC_ARCH) -D$(OS_TARGET) -DOS_TARGET=$(OS_TARGET) -DOS_TARGET_RELEASE=$(OS_TARGET_RELEASE)
else
  CFLAGS1 =  -Wall $(CC_ARCH) -DOS_TARGET=$(OS_RUN)
endif

#CFLAGS2 = -ansi -pedantic -fvolatile -fno-builtin -funroll-loops
CFLAGS2 = -ansi -fno-builtin -funroll-loops
ifeq (3, $(GCCVER))
  CFLAGS2 += -fvolatile
endif
ifeq (MIPS64, $(CPU))	
  CFLAGS2 += -D__WORDSIZE=64
endif	

ifeq (PPC603, $(CPU))
  CFLAGS_OPT = -O0 
else
  ifeq (PPC860, $(CPU))
    CFLAGS_OPT = -O0 
  else
    CFLAGS_OPT = -O2 
  endif
endif

BUILD_MAP_FILE= -Map 


PROJ_BSP   = $(subst \,/,$(PROJ_BSP_DIR))
VX_ROOT    = $(subst \,/,$(WIND_BASE))
OS_CFLAGS  = -I$(VX_ROOT)/target/h -I$(VX_ROOT)/target/config/all -I$(PROJ_BSP)

COMPILER_ROOT = $(VX_ROOT)/host/x86-win32/lib/gcc-lib/
OS_ROOT       = $(VX_ROOT)
GNULIBROOT    = $(VX_ROOT)/host/$(WIND_HOST_TYPE)


# fix LDFLAGS possible changed in defs.bsp

ifeq (,$(filter -EB -EL,$(LDFLAGS)))
  ifeq (BE,$(ENDIAN))
    LDFLAGS:=-EB $(LDFLAGS)
  else
    LDFLAGS:=-EL $(LDFLAGS)
  endif
endif
