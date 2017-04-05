# Toolkit definitions for
# Wind River Diab Compiler
#
# In *PSS projects it is used to build vxWorks targets
#
# formerly known as the Diab Compiler, combines industry-leading
# optimization technology with the flexibility and control needed
# to fully take advantage of today's complex embedded processors
# 
# URL:
#    http://windriver.com/products/development_suite/wind_river_compiler/
#

if [ "$ENDIAN" = "BE" ]
then
	export TOOL=diabbe
else
	export TOOL=diab
fi

COMP_POSTFIX="$CPU_TOOL"
export TOOL_FAMILY=diab
export TOOLKIT_MAKE_RULES=tornado_diab

# Diab toolchain addition
export WIND_HOST_TYPE=x86-win32
export DIABLIB="$WIND_BASE\\host\\diab"
win32_add_paths "$DIABLIB\\WIN32\\bin"
win32_add_paths "$WIND_BASE\\host\\$WIND_HOST_TYPE\\bin"
export COMPILER=DIAB
TOOLKIT_MAKE_RULES=tornado_diab

export COMP_PREFIX
export PREFIX=$COMP_PREFIX
export POSTFIX=$COMP_POSTFIX
export AS=${PREFIX}as${POSTFIX}
export CC=${PREFIX}${G}cc${POSTFIX}
export AR=${PREFIX}ar${POSTFIX}
export LD=${PREFIX}ld${POSTFIX}
export NM=${PREFIX}nm${POSTFIX}
export CPP=${PREFIX}cpp${POSTFIX}
export STRIP=${PREFIX}strip${POSTFIX}
export OBJCOPY=${PREFIX}objcopy${POSTFIX}


case $CPU_BOARD in
    ARM5181)
        ;;
    ARM5281)
        ;;
esac
