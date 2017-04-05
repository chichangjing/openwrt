# mv_pmc8245.sh
# Target CPU settings

ARCH=82xx
export CPU_TOOL=ppc
export CPU_FAMILY=PPC_CPU
export CPU=PPC603
export CPU_DIR=PPC603
export PMC8245_DRV=EXISTS
export ENDIAN=BE
#
DEFAULT_MANAGEMENT_BUS=PCI
DEFAULT_TOOLKIT_Linux=eldk
DEFAULT_TOOLKIT_vxWorks=tornado
CPU_PRE_TOOLKIT_tornado=cpu_pre_toolkit_tornado
CPU_PRE_TOOLKIT_tornado_diab=cpu_pre_toolkit_tornado_diab
CPU_FLAGS="-mstrict-align"
#
if [ $TARGET_OS = Linux ]
then
    DEFAULT_LINUX_KERNEL_VERSION=2.6.22.18
    DEFAULT_LINUX_LSP_NAME=mv_pmc8245
    DEFAULT_LINUX_CONFIG=mv_pmc8245_defconfig
    LSP_SUBDIR=${LSP_SUBDIR:-mv_pmc8245_lsp_2_6}
    LINUX_CRFS_SCRIPT_NAME=crfs_ppc.sh
    LINUX_CRFS_ARCH_ARG=$ARCH
fi
#
# per toolkit settings
# executed _after_ toolkit choosen,
# overrides defaults
cpu_pre_toolkit_tornado()
{
    ##
    ## WIND_BASE can be set by user
    ##
    if [ -z $WIND_BASE ]
    then
       export WIND_BASE='c:\Tornado'
       echo "WIND_BASE - $WIND_BASE"
    else   
       ### check if exists WIND_BASE defined by user 
       if [ -d $WIND_BASE ]
       then
          echo "WIND_BASE - $WIND_BASE"
       else
          echo "Wrong WIND_BASE - $WIND_BASE"
          exit 0
       fi
    fi
    
    export PROJ_BSP_DIR="$BSP_CONFIG_DIR"'\mv_pmc8245'
    export CC_ARCH=-mcpu=603
    export CC_ARCH_DIAB=-tPPC603FH:vxworks55
}
cpu_pre_toolkit_tornado_diab()
{
    export WIND_BASE='c:\Tornado_2_2'
    export PROJ_BSP_DIR="$BSP_CONFIG_DIR\\mv_pmc8245_Tornado2.2.1"
    export CC_ARCH=-mcpu=603
    export CC_ARCH_DIAB=-tPPC603FH:vxworks55
    export CPU_DIR=PPC603_DIAB
}
linux_2_6_cr_jffs2_params()
{
    CR_JFFS2_FILE_NAME="cr_jffs2_img_${CPU_TOOL}.sh"
    CR_JFFS2_PARAMS="${IMAGE_PATH} ${DIST}/rootfs ${TOOL_CHAIN_PATH} 256"
    UIMAGE_PATH="${DIST}/arch/${CPU_TOOL}/boot/images"
}
