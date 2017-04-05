# Toolkit definitions for
# WindRiver's Tornado tool (win32 host platform)
#
# In *PSS projects it is used to build vxWorks targets
#
# Tornado based on GCC suite
#

export TOOL=gnu
COMP_POSTFIX="$CPU_TOOL"

export TOOL=gnu
export TOOL_FAMILY=gnu
export TOOLKIT_MAKE_RULES=tornado
unset G

export COMP_PREFIX
export PREFIX=$COMP_PREFIX
export POSTFIX=$COMP_POSTFIX
COMPILER=${PREFIX}${G}cc${POSTFIX}
export AS=${PREFIX}as${POSTFIX}
export CC=${PREFIX}${G}cc${POSTFIX}
export AR=${PREFIX}ar${POSTFIX}
export LD=${PREFIX}ld${POSTFIX}
export NM=${PREFIX}nm${POSTFIX}
export CPP=${PREFIX}cpp${POSTFIX}
export STRIP=${PREFIX}strip${POSTFIX}
export OBJCOPY=${PREFIX}objcopy${POSTFIX}
export GNU_CROSS=yes


export XCOMP_ROOT_PATH_SUFFIX

export GCC_VER=`$COMPILER -v 2>&1 | awk '/gcc version/ {print $3}'`

case $CPU_BOARD in
    ARM5181)
        ;;
    ARM5281)
        ;;
esac

export WIND_HOST_TYPE=x86-win32
win32_add_paths "$WIND_BASE\\host\\$WIND_HOST_TYPE\\bin"
export COMPILER=GNU
