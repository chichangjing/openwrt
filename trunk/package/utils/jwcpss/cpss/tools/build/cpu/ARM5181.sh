#
# Target CPU settings

# mandatory params
export CPU=ARMARCH5
export CPU_TOOL=arm
export CPU_FAMILY=CPU_ARM
export CPU_DIR=ARM5181

export ARM5181_DRV=EXISTS
PROJECT_DEFS="$PROJECT_DEFS ARM5181_DRV"

export ARM_CPU=5181
export ARM_MMU=5181
export ARM_CACHE=5181

# for Linux 2.4 build
ARCH=arm
#
DEFAULT_MANAGEMENT_BUS=PCI_SMI
DEFAULT_TOOLKIT_Linux=gnueabi
DEFAULT_TOOLKIT_vxWorks=tornado
CPU_PRE_TOOLKIT_gnueabi=cpu_pre_toolkit_gnueabi
CPU_PRE_TOOLKIT_eldk=cpu_pre_toolkit_eldk
CPU_PRE_TOOLKIT_tornado=cpu_pre_toolkit_tornado
#
if [ $TARGET_OS = Linux ]
then
    DEFAULT_LINUX_KERNEL_VERSION=2.6.22.18
    DEFAULT_LINUX_LSP_NAME=88fxx81_prpmc
    DEFAULT_LINUX_CONFIG=db88f5181_defconfig
    LSP_SUBDIR=${LSP_SUBDIR:-88fxx81_prpmc_lsp_2_6}
    LINUX_CRFS_ARCH_ARG=""
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
      export WIND_BASE='c:\TornadoARM'
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

    export PROJ_BSP_DIR="$BSP_CONFIG_DIR"'\db_88f5181_prpmc\swControllers\VxBsp'
    export CC_ARCH=-march=armv5
    export CC_ARCH_DIAB=-tARMV5LN:vxworks55
    export ARM_CPU=5181
    export ARM_MMU=5181
    export ARM_CACHE=5181
}
cpu_pre_toolkit_eldk()
{
    LINUX_CRFS_SCRIPT_NAME=crfs_arm.sh
}
cpu_pre_toolkit_gnueabi()
{
    LINUX_CRFS_SCRIPT_NAME=crfs_arm_gnueabi.sh
}
linux_2_6_cr_jffs2_params()
{
    UIMAGE_PATH="${DIST}/arch/${CPU_TOOL}/boot"
    CR_JFFS2_FILE_NAME="cr_jffs2_img_arm_gnueabi.sh"
    CR_JFFS2_PARAMS="${IMAGE_PATH} ${DIST}/rootfs 256"
}
