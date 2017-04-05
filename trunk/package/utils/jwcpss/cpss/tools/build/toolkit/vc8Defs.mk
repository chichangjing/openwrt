#
# Make definitions for Microsoft Visual C++ 8
#

#start of include
FIS = /I "
#end of include
FIE = "
FD=/D
FO=/Fo
FC=/c
FG=/Zi
OBJ=obj
ASM=asm
LIBEXT=lib


OS_DEFS += _VISUAL
#OS_INCLUDE = /I "C:/Program Files/Microsoft Visual Studio 8/VC/include" 
#COMPILER_ROOT = C:/Program Files/Microsoft Visual Studio 8/VC/Bin
#OS_ROOT = "C:/Program Files/Microsoft Visual Studio 8/VC/Lib"

CFLAGS1 = /nologo /DWIN32 /D_WIN32 /DNDEBUG /D_MBCS /D_VISUALC /DIMAGE_HELP_SHELL /DAPPLICATION_SYMBOL_TABLE /D_CONSOLE /D_LIB
CFLAGS2 =

CFLAGS_OPT = /Od /Gm /EHsc /RTC1 /ZI /MTd

ifneq (dep,$(MAKECMDGOALS))
### add this flag only on compilation and not on make depend to avoid next warnings:
### makedepend: warning:  cannot open "/Zp1"
  ifeq (simulation, $(PROJ_NAME))
    CFLAGS_OPT += /Zp1
  endif
  
   CFLAGS1 += $(DBG_WARNING_LEVEL)  
endif

BUILD_MAP_FILE=/Fm
