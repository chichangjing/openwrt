#
# Make definitions for Linux/FreeBSD/etc native GCC
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

LDFLAGS=-r -X -N
export LDFLAGS

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
#############################################
# build WM simulation on linux platform - 
# old (32 bits) or new (64 bits)
#############################################
ifeq (YES,$(IS_64BIT_OS))                          # linux host platform - 64 bit
   ifeq (32,$(WORDSIZE_REQUIRED))                  # build 32 bit on 64 bits platform
      CFLAGS2 = -ansi  -fno-builtin -funroll-loops -m32
      LDFLAGS += -melf_i386 
   else                                            # build 64 bit on 64 bits platform
      CFLAGS2 = -ansi  -fno-builtin -funroll-loops -m64 -D__WORDSIZE=64
   endif
else   # old linux host platform 32 bit
   CFLAGS2 = -ansi -fno-builtin -funroll-loops
endif
CXXFLAGS2 = -pthread

ifeq (YES,$(THREAT_WARNINGS_AS_ERRORS))
#enforce C89-style variable declarations in gcc
CFLAGS2 += -Werror=declaration-after-statement
#forbids variable len arrays, C89-style, prevent compilation fail with VC10
CFLAGS2 += -Werror=vla
CFLAGS2 += -Werror
endif

#Shared library approach requires Position-Independent Code
ifeq (1,$(SHARED_MEMORY))
  CFLAGS2 += -fPIC -DPIC
endif

CFLAGS_OPT = -O2 

BUILD_MAP_FILE= -Map 

