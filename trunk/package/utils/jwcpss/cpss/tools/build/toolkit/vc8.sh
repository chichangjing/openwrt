# Toolkit definitions for
# Microsoft Visual C++ 8
#

# call "C:\Program Files\Microsoft Visual Studio 8\VC\bin\vcvars32.bat"
win32_call_bat_file VCVARS32 "C:\\Program Files\\Microsoft Visual Studio 8\\VC\\bin\\vcvars32.bat"
export VC_VER=8
CPU_DIR=VC8

#export OS_INCLUDE='/I "C:/Program Files/Microsoft Visual Studio 8/VC/include"'
#export COMPILER_ROOT='C:/Program Files/Microsoft Visual Studio 8/VC/Bin'
#export OS_ROOT='"C:/Program Files/Microsoft Visual Studio 8/VC/Lib"'
if [ \! -z "$ZSH_NAME" ]; then
 i="${VCINSTALLDIR:gs.\\./.}"
elif [ \! -z "$BASH" ]; then
 i="${VCINSTALLDIR//\\//}"
fi
export OS_INCLUDE='/I "'"$i"'/include" /I "'"$i"'/PlatformSDK/include"'
export COMPILER_ROOT="$i/Bin"
export OS_ROOT="\"$i/Lib\""

export TOOL=visual
export TOOLKIT_MAKE_RULES=vc8

export CPU_TOOL=intel
export BIN_BS=128
export BIN_SKIP=1
export WM_USED=USED

unset PROJ_BSP_DIR

######################################################
#  SHOST_LIB_DIR - used to access simulation libs 
#                  in MakefileWin.mak
######################################################
export SHOST_LIB_DIR="VC"

export AS=
export CC=cl
export AR=lib
export LD=link
export CPP=cl
export NM=
export LDFLAGS=""

