# Toolkit definitions for
# Microsoft Visual C++ 6
#

# call "c:\Program Files\Microsoft Visual Studio\VC98\Bin\VCVARS32.BAT"
win32_call_bat_file VCVARS32 "c:\Program Files\Microsoft Visual Studio\VC98\Bin\VCVARS32.BAT"
export VC_VER=6

#export OS_INCLUDE='/I "C:/Program Files/Microsoft Visual Studio/VC98/Include"'
#export COMPILER_ROOT='C:/Program Files/Microsoft Visual Studio/VC98/Bin'
#export OS_ROOT='"C:/Program Files/Microsoft Visual Studio/VC98/Lib"'
if [ \! -z "$ZSH_NAME" ]; then
 i="${MSVCDir:gs.\\./.}"
elif [ \! -z "$BASH" ]; then
 i="${MSVCDir//\\//}"
fi
export OS_INCLUDE='/I "'"$i"'/include"'
export COMPILER_ROOT="$i/Bin"
export OS_ROOT="\"$i/Lib\""

# set directory name for object file dir

if [ "$FORCE_GM_LION_USE" = "EXISTS" ];then
   CPU_DIR=VC_GM_LION
elif [ "$FORCE_GM_LION2_USE" = "EXISTS" ];then
   CPU_DIR=VC_GM_LION2
elif [ "$FORCE_GM_XCAT2_USE" = "EXISTS" ];then
   CPU_DIR=VC_GM_XCAT2
elif [ "$FORCE_GM_PUMA3_USE" = "EXISTS" ];then
   CPU_DIR=VC_GM_PUMA3
elif [ "$FORCE_SUB20_USE" = "EXISTS" ];then
   CPU_DIR=VC_SUB20
else
   CPU_DIR=VC
fi

######################################################
#  SHOST_LIB_DIR - used to access simulation libs 
#                  in MakefileWin.mak
######################################################
export SHOST_LIB_DIR="VC"

export IMPL_PRINT_CALL_STACK=EXISTS

export TOOL=visual
export TOOLKIT_MAKE_RULES=vc6

export CPU_TOOL=intel
export BIN_BS=128
export BIN_SKIP=1
export WM_USED=USED

unset PROJ_BSP_DIR

export AS=
export CC=cl
export AR=lib
export LD=link
export CPP=cl
export NM=
export LDFLAGS=""


