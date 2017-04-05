# Toolkit definitions for
# Wind River Workbench
#
# In *PSS projects it is used to build vxWorks targets
#
# Wind River Workbench is a collection of tools that accelerates
# time-to-market for developers building devices with VxWorks
# and Wind River Linux
#
# URL:
#    http://windriver.com/products/workbench/
#

export TOOL=gnu
export TOOL_ENV=workbench
export TOOLKIT_MAKE_RULES=workbench33
export COMPILER=GNU

export WIND_DIR="C:\\Workbench_3.3"
export WRSD_LICENSE_FILE='C:/Workbench_3.3/license'
export WIND_PREFERRED_PACKAGES=vxworks-6.9
export WIND_HOME="C:\\Workbench_3.3"
export WIND_PLATFORM=vxworks-6.9
export WIND_BASE="$WIND_HOME\\$WIND_PREFERRED_PACKAGES"
export WIND_USR="$WIND_BASE\\target\\usr"
export WIND_HOST_TYPE=x86-win32

export WIND_TOOLS="$WIND_HOME\\workbench-3.3"
export WIND_GNU_PATH="$WIND_HOME\\gnu\\4.3.3-vxworks-6.9"

export LD_LIBRARY_PATH="$WIND_BASE\\host\\$WIND_HOST_TYPE\\lib;$WIND_BASE\\wrwb\\platform\\eclipse\\$WIND_HOST_TYPE\\bin;$WIND_BASE\\$WIND_HOST_TYPE\\lib;$WIND_BASE\\foundation\\$WIND_HOST_TYPE\\lib"


export COMP_IPNET2=ip_net2-6.9
export WIND_COMPONENTS_LIBPATHS="$WIND_HOME\\components\\obj\\$WIND_PLATFORM\\krnl\\lib"
export WIND_COMPONENTS_INCLUDES="$WIND_HOME\\components\\$COMP_IPNET2\\ipcom\\include;$WIND_HOME\\components\\$COMP_IPNET2\\ipcom\\config;$WIND_HOME\\components\\$COMP_IPNET2\\ipcom\\port\\vxworks\\config;"

win32_add_paths "$WIND_BASE\\host\\$WIND_HOST_TYPE\\bin"
COMP_POSTFIX="$CPU_TOOL"

export TOOL_FAMILY=gnu
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

export COMPILER=GNU

win32_add_paths "$WIND_TOOLS\\$WIND_HOST_TYPE\\bin"
win32_add_paths "$WIND_BASE"
win32_add_paths "$WIND_BASE\\host\\$WIND_HOST_TYPE\\bin"
win32_add_paths "$WIND_TOOLS\\foundation\\$WIND_HOST_TYPE\\bin"

