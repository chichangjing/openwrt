#
# Make definitions for INTEL64
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
LIBEXT=a

ifeq (BE, $(ENDIAN))
  LDFLAGS=-EB -r -X -N
else
  LDFLAGS=-EL -r -X -N
endif
export LDFLAGS

CFLAGS1 += -B$(COMPILER_ROOT)

#Add a C definition to distiguish between different OS (Added for FreeBSD)
ifdef OS_TARGET
  CFLAGS1 =  -Wall $(CC_ARCH) -D$(OS_TARGET) -DOS_TARGET=$(OS_TARGET) -DOS_TARGET_RELEASE=$(OS_TARGET_RELEASE)
else
  CFLAGS1 =  -Wall $(CC_ARCH) -DOS_TARGET=$(OS_RUN)
endif

ifeq (YES,$(SO_LIB))
  CFLAGS1 += -fPIC
endif

#CFLAGS2 = -ansi -pedantic -fvolatile -fno-builtin -funroll-loops
CFLAGS2 = -ansi -fno-builtin -funroll-loops
ifeq (3, $(GCCVER))
  CFLAGS2 += -fvolatile
endif
CFLAGS2 += -D__WORDSIZE=64

CFLAGS_OPT = -O2 

BUILD_MAP_FILE= -Map 

#Shared library approach requires Position-Independent Code
ifeq (1,$(SHARED_MEMORY))
  CFLAGS2 += -fPIC -DPIC
endif

