# Toolkit definitions for
# Linux/FreeBSD native(system) GCC
#
# This toolkit used to build simulation app to be
# run on the same host
#

export TOOL=gnu
export TOOL_FAMILY=gnu
export TOOLKIT_MAKE_RULES=host_gcc

export XCOMP_INC_PATH=/
export XCOMP_ROOT_PATH=/

export AS=as
export CC=gcc
export CXX=g++
export AR=ar
export LD=ld
export NM=nm
export CPP=cpp
export STRIP=strip
export OBJCOPY=objcopy

export GCC_VER=`gcc -v 2>&1 | awk '/gcc version/ {print $3}'`

#When 64bit application built use /usr/lib64 instead of /usr/lib (if exists)
if [ "$IS_64BIT_OS" == "YES" ]; then
    if [ -d /usr/lib/x86_64-linux-gnu/. ]; then
        export LIB_1=/x86_64-linux-gnu
    elif [ -d /usr/lib64/. ]; then
        export LIB_1=64
    fi
fi
