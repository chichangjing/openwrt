#
# Make definitions for toolkit
#

#start of include
FIS= -I
#end of include
FIE=
FD=<TEMPLATE: define preprocessor macro for $(CC), -D for gcc>
FO=<TEMPLATE: set output file name flag for $(CC), -o for gcc>
FC=<TEMPLATE: compile only flag for $(CC), -c for gcc>
FG=<TEMPLATE: enable debug flag for $(CC), -g for gcc>
OBJ=<TEMPLATE: object file ext, o for linux/freebsd/vxworks, obj for bc,visual>
ASM=<TEMPLATE: assembler file ext, s for linux/freebsd/vxworks, asm for bc,visual>
LIBEXT=<TEMPLATE: library file ext, a for linux/freebsd/vxworks, lib for bc,visual>


# optional:
#
#OS_DEFS += <TEMPLATE: list of macroses>
#    example: OS_DEFS += WIN32

#OS_INCLUDE = <TEMPLATE: include directive>

#LDFLAGS = <TEMPLATE: linker flags>
export LDFLAGS


####################################################################

# Compiler flags
CFLAGS1 = -DWIN32 -D_WIN32 -DNDEBUG -D_MBCS /D_BORLAND -DIMAGE_HELP_SHELL -DAPPLICATION_SYMBOL_TABLE -D_CONSOLE -D_LIB
# ???
CFLAGS2 = 
# Compiler flags for optimized compilation
CFLAGS_OPT = -c -DI486 -5 -Od -o -r- -X -a1 -R -v -pc -y -q -w- -wbef -wbig -wccc -wcpr -wdpu -wdup -wdsz -weas -weff -wias -wext \
			-whid -wibc -will -winl -wlin -wlvc -wmpc -wmpd -wnak -wnci -wnfc -wnod -wnst -wntd -wnvf -wobi -wofp -wovl -wpch -wpia \
			-wpro -wrch -wret -wrng -wrpt -wrvl -wsus -wvoi -wzdi

