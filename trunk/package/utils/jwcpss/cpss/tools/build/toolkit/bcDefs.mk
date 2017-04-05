#
# Make definitions for Borland cmdline tools
#

#start of include
FIS= -I
#end of include
FIE=
FD=-D
FO=-o
FC=-c
FG=-v
OBJ=obj
ASMEXT=asm
LIBEXT=lib

# optional:
#
OS_DEFS += _BORLAND
ifeq (,$(OS_INCLUDE))
OS_INCLUDE = -I"c:\Program Files\Borland\CBuilder5\Include" 
endif

####################################################################
#   Borland C++ Builder 5 Definitions
####################################################################

# Compiler flags
CFLAGS1 = -g255 -DWIN32 -D_WIN32 -DNDEBUG -D_MBCS /D_BORLAND -DIMAGE_HELP_SHELL -DAPPLICATION_SYMBOL_TABLE -D_CONSOLE -D_LIB -DLE
CFLAGS2 = 
# Compiler flags for optimized compilation
CFLAGS_OPT = -DI486 -5 -Od -r- -X -a1 -R -v -pc -y -q -w- -wbef -wbig -wccc -wcpr -wdpu -wdup -wdsz -weas -weff -wias -wext \
			-whid -wibc -will -winl -wlin -wlvc -wmpc -wmpd -wnak -wnci -wnfc -wnod -wnst -wntd -wnvf -wobi -wofp -wovl -wpch -wpia \
			-wpro -wrch -wret -wrng -wrpt -wrvl -wsus -wvoi -wzdi

