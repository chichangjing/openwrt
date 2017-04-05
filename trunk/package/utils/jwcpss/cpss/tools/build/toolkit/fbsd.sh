# Toolkit definitions for
#	powerpc-juniper_11_03_07-eabi
#
# Used to build freebsd version for PPC85XX
#

case $CPU_BOARD in
    PPC85XX )
        COMP_PREFIX=ppc-fbsd-e500-
        XCOMP_ROOT_PATH_SUFFIX=ppc-fbsd-e500
		export MACHINE_ARCH=powerpc
		export 
        ;;
    *)
        error_message "CPU $CPU_BOARD is unsuppoted by $TOOLKIT toolkit"
        exit 1
        ;;
esac

export TOOL=gnu
export TOOL_FAMILY=gnu
export TOOLKIT_MAKE_RULES=fbsd
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
export LD="NO_MV_FRONTEND=yes ${PREFIX}ld${POSTFIX}"
export NM=${PREFIX}nm${POSTFIX}
export CPP=${PREFIX}cpp${POSTFIX}
export STRIP=${PREFIX}strip${POSTFIX}
export OBJCOPY=${PREFIX}objcopy${POSTFIX}
export GNU_CROSS=yes


export XCOMP_ROOT_PATH_SUFFIX

export GCC_VER=`$COMPILER -v 2>&1 | awk '/gcc version/ {print $3}'`

TOOLKIT_PATH=`which $COMPILER`
if [ $? != 0 ]; then
	echo "**********************************************"
	echo "**********************************************"
	echo "!!! $COMPILER not found in \$PATH"
	echo "!!! Make sure you have set correct PATH value"
	exit 1
fi
TOOLKIT_PATH=`dirname $TOOLKIT_PATH`
TOOLKIT_PATH=`dirname $TOOLKIT_PATH`
export COMPILER_ROOT=$TOOLKIT_PATH
TOOLKIT_PATH=`dirname $TOOLKIT_PATH`
export TOOL_CHAIN_PATH=$TOOLKIT_PATH
export XCOMP_INC_PATH=$TOOLKIT_PATH
export XCOMP_ROOT_PATH=$TOOLKIT_PATH/$XCOMP_ROOT_PATH_SUFFIX
