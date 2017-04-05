# Toolkit definitions for
# Emdebian - Embedded Debian Project
#
# Official web site:
#  	http://www.emdebian.org
#

case $CPU_BOARD in
    PPC603 | PPC604 | PPC85XX )
        COMP_PREFIX=powerpc-linux-gnu-
        XCOMP_ROOT_PATH_SUFFIX=powerpc-linux-gnu
        ;;
    ARM_EV | ARM_RD | ARM_SP | ARM_GM | ARM5181 | ARM5281
        COMP_PREFIX=arm-linux-gnueabi-
        XCOMP_ROOT_PATH_SUFFIX=arm-linux-gnueabi
        ;;
    *)
        error_message "CPU $CPU_BOARD is unsuppoted by $TOOLKIT toolkit"
        exit 1
        ;;
esac

export TOOL=gnu
export TOOL_FAMILY=gnu
export TOOLKIT_MAKE_RULES=embeddian

export COMP_PREFIX
export PREFIX=$COMP_PREFIX
export POSTFIX=$COMP_POSTFIX
if which ${PREFIX}gcc${POSTFIX} >/dev/null 2>/dev/null ; then
	export G=g
fi
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
export XCOMP_ROOT_PATH=/usr/$XCOMP_ROOT_PATH_SUFFIX
