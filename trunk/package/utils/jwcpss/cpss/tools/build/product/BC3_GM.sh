#
# Golden Mode library for Bobcat3
#
# Please note: 4th and 5th parameter of build sh changed:
#	4th parameter: chip_name (BC3)
#   5th parameter: mode { RELEASE | DEBUG | PURIFY }
#
# Example:
#   Build.sh Linux i386 LinuxWM BC3 DEBUG BC3_GM $OBJS_DIR
#

export GM_ROOT=$GM_PATH
export GM_PROJECT=Bobcat3Gm
export GM_KERNEL_DIR=verifierKernel/Verifier_gm_krnl_3_1_1
export GM_KERNEL_RELEASE_DIR=verifierKernel
export GM_SILICON=Sip5
export GM=gm
export GM_DIR=$GM_ROOT/$GM
#export GM_OUTPUT_DIR="$COMPILATION_ROOT"
export PROJECT=$GM_PROJECT
export BUILD_SH_ENV=YES

if [ \! -z "$ZSH_NAME" ]; then
    set -o shwordsplit
fi
OS_DEFS_FIX=""
for d in $OS_DEFS; do
    case $d in
        LINUX)
            OS_DEFS_FIX="$OS_DEFS_FIX $d NLMPLATFORM_$d LINUX_SIM"
            ;;
        UNIX)
            OS_DEFS_FIX="$OS_DEFS_FIX NLMPLATFORM_$d"
            ;;
        WIN32)
            OS_DEFS_FIX="$OS_DEFS_FIX $d VRF_$d"
            ;;
        *)
            OS_DEFS_FIX="$OS_DEFS_FIX $d"
            ;;
    esac
done
OS_DEFS="$OS_DEFS_FIX"


PROJECT_DEFS="$PROJECT_DEFS VRF_KRNL_STRICT VRF_GM_KERNEL VRF_GOLDEN_MODEL"
PROJECT_DEFS="$PROJECT_DEFS VRF_NO_RSLVD ENABLE_RECOVERY GM_ALT_STRING GM_ALT_CLSDATA NO_COV"

if [ \! -z "$ZSH_NAME" ]; then
    set -o noshwordsplit
fi


export USER_BASE=$GM_PATH
export USER_MAKE=gmTopMake
export DONT_LINK=YES

export PREFIX=$FAMILY
export MODE=$SIMULATION
case $MODE in
    DEBUG)
        export DEBUG_INFO=D_ON
        export GM_LIB_NAME=${GM_PROJECT}_gm_dbg
        ;;
    RELEASE)
        export GM_LIB_NAME=${GM_PROJECT}_gm
        ;;
    PURIFY)
        export GM_LIB_NAME=${GM_PROJECT}_gm_purify
        ;;
esac

SIM_INCLUDES="$SIM_INCLUDES simulation/simGeneral/h"
SIM_INCLUDES="$SIM_INCLUDES simulation/simOs/h"
export SIM_INCLUDES
