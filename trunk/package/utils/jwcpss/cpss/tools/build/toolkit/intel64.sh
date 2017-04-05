# toolkit defs (on vvenus111)

case `uname -m` in
    i?86)
        # cross-compilation on 32bit host
        export ARCH=x86_64
        export CC_ARCH=-m64
        export LDEMULATION=elf_x86_64
        ;;
esac

COMP_PREFIX=
XCOMP_ROOT_PATH_SUFFIX=

# common definitions for all GNU cross compilers

export TOOL=gnu
export TOOL_FAMILY=gnu
export TOOLKIT_MAKE_RULES=intel64
if which ${PREFIX}gcc${POSTFIX} >/dev/null 2>/dev/null ; then
    export G=g
fi
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

#TOOLKIT_PATH=`which $COMPILER`
#TOOLKIT_PATH=`dirname $TOOLKIT_PATH`
#export COMPILER_ROOT=$TOOLKIT_PATH
#export TOOL_CHAIN_PATH=$TOOLKIT_PATH
#TOOLKIT_PATH=`dirname $TOOLKIT_PATH`
#export XCOMP_INC_PATH=$TOOLKIT_PATH
#export XCOMP_ROOT_PATH=
