# Toolkit definitions for
# Borland C++ 5.5 Free Command-line Tools
#
# Can be downloaded from
#    ftp://ftpd.codegear.com/download/bcppbuilder/freecommandLinetools.exe
# Debugger:
#    ftp://ftpd.codegear.com/download/bcppbuilder/TurboDebugger.exe

export TOOL=builder
export CPU=BC
export CPU_DIR=BC
export TOOLKIT_MAKE_RULES=bc

# search for Borland directory
if [ \! -z "$ZSH_NAME" ]; then
  for i in $path
  do
    if [ -f "$i/bcc32.exe" ]; then
      OS_INCLUDE="-I\"${i%/*}/Include\""
      export OS_INCLUDE
    fi
  done
fi

export CPU_TOOL=intel
export BIN_BS=128
export BIN_SKIP=1
export WM_USED=USED

unset WIND_BASE
unset CC_ARCH
unset PROJ_BSP_DIR

######################################################
#  SHOST_LIB_DIR - used to access simulation libs 
#                  in MakefileWin.mak
######################################################
export SHOST_LIB_DIR=BC

export AS=tasm32
export CC=bcc32
case $PRODUCT_TYPE in
    CPSS*)
        export AR="tlib /u /C /P512"
        ;;
    *)
        export AR="tlib /u /C /P2048"
        ;;
esac
export LD=ilink32
export CPP=bcc32
export NM=

export LDFLAGS="/Gn /q /c /v /m /M /s c0x32"
