# Toolkit definitions for
# Microsoft Visual C++ 10
#

# call vcvars32.bat
if win32_path_exists "c:\Program Files (x86)\Microsoft Visual Studio 10.0\VC\bin"
then
	if [ "$WORDSIZE_REQUIRED" = "64" ]; then
		win32_call_bat_file VCVARS32 "c:\Program Files (x86)\Microsoft Visual Studio 10.0\VC\vcvarsall.bat" amd64
	else
		win32_call_bat_file VCVARS32 "c:\Program Files (x86)\Microsoft Visual Studio 10.0\VC\bin\vcvars32.bat"
	fi
else
	if win32_path_exists "c:\Program Files\Microsoft Visual Studio 10.0\VC\bin"
	then
		if [ "$WORDSIZE_REQUIRED" = "64" ]; then
			win32_call_bat_file VCVARS32 "c:\Program Files\Microsoft Visual Studio 10.0\VC\vcvarsall.bat" amd64
		else
			win32_call_bat_file VCVARS32 "c:\Program Files\Microsoft Visual Studio 10.0\VC\bin\vcvars32.bat"
		fi
	else
		echo
		echo
		echo "Microsoft Visual Studio 10 not found. Exiting."
		echo
		echo
		exit
	fi
fi

if [ \! -z "$ZSH_NAME" ]; then
 i="${VCINSTALLDIR:gs.\\./.}"
 w="${WindowsSdkDir:gs.\\./.}"
elif [ \! -z "$BASH" ]; then
 i="${VCINSTALLDIR//\\//}"
 i="${WindowsSdkDir//\\//}"
fi
export OS_INCLUDE='/I "'"$i"'/Include" /I "'"$w"'/Include"'
#export COMPILER_ROOT="$i/Bin"
#export OS_ROOT="\"$i/Lib\""

export VC_VER=10


if [ "$WORDSIZE_REQUIRED" = "64" ]; then
   CPU_DIR=VC10_64
elif [ "$FORCE_GM_PUMA3_USE" = "EXISTS" ];then
   CPU_DIR=VC10_GM_PUMA3 
elif [ "$FORCE_GM_BOBCAT2_USE" = "EXISTS" ];then
   CPU_DIR=VC10_GM_BOBCAT2 
elif [ "$FORCE_GM_BOBCAT2_B0_USE" = "EXISTS" ];then
   CPU_DIR=VC10_GM_BOBCAT2_B0 
elif [ "$FORCE_GM_BOBK_CAELUM_USE" = "EXISTS" ];then
   CPU_DIR=VC10_GM_BOBK_CAELUM 
elif [ "$FORCE_GM_BOBCAT3_USE" = "EXISTS" ];then
   CPU_DIR=VC10_GM_BOBCAT3 
else
   CPU_DIR=VC10
fi


######################################################
#  SHOST_LIB_DIR - used to access simulation libs 
#                  in MakefileWin.mak
######################################################
if [ "$WORDSIZE_REQUIRED" = "64" ]; then
  export SHOST_LIB_DIR="VC10_64bit"
else
  export SHOST_LIB_DIR="VC10"
  export IMPL_PRINT_CALL_STACK=EXISTS
fi

export TOOL=visual
export TOOLKIT_MAKE_RULES=vc10

export CPU_TOOL=intel
export BIN_BS=128
export BIN_SKIP=1
export WM_USED=USED

unset PROJ_BSP_DIR

export AS=
export CC=cl
export CXX=cl
export AR=lib
export LD=link
export CPP=cl
export NM=
export LDFLAGS=""

