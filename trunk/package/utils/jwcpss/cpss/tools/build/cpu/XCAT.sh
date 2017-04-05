#
# Target CPU settings

# mandatory params

export CPU=ARMARCH5
export CPU_TOOL=arm
export CPU_FAMILY=CPU_ARM
export CPU_DIR=ARM926EJ

echo "export linux_objects_ext=xcat"
export linux_objects_ext="xcat"

echo "linux_objects_ext - $linux_objects_ext"

export XCAT_DRV=EXISTS
PROJECT_DEFS="$PROJECT_DEFS XCAT_DRV"

if [ "$PRODUCT_TYPE" != "POC" ]
then
   export DRAGONITE_TYPE=A1
   PROJECT_DEFS="$PROJECT_DEFS DRAGONITE_TYPE_A1"
fi

ARCH=arm

export ARM_CPU=926EJ
export ARM_MMU=926E
export ARM_CACHE=926E
#

if [ "$PSS_PRODUCT_TYPE" = "POC" -o "$PSS_PRODUCT_TYPE" = "PSS" ]
then
   DEFAULT_MANAGEMENT_BUS=PCI
else
   DEFAULT_MANAGEMENT_BUS=PCI_SMI_I2C
fi


DEFAULT_TOOLKIT_Linux=gnueabi
DEFAULT_TOOLKIT_vxWorks=tornado
CPU_PRE_TOOLKIT_gnueabi=cpu_pre_toolkit_gnueabi
CPU_PRE_TOOLKIT_marvell_v5sft=cpu_pre_toolkit_marvell_v5sft
CPU_PRE_TOOLKIT_tornado=cpu_pre_toolkit_tornado
CPU_PRE_TOOLKIT_tornado_diab=cpu_pre_toolkit_tornado
#
if [ $TARGET_OS = Linux ]
then
    DEFAULT_LINUX_LSP_NAME=xcat_lsp
    DEFAULT_LINUX_KERNEL_VERSION=2.6.22.18
    DEFAULT_LINUX_CONFIG=mv88fxcat_le_defconfig
    LINUX_CRFS_ARCH_ARG=""
fi
#
#
###############################
# Set CPSS LOG_ENABLE
###############################
if [ -z $API_LOG_ENABLE ]; then
    export LOG_ENABLE="L_ON"
fi
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

   export CC_ARCH=-march=armv5
   export CC_ARCH_DIAB=-tARMV5LS:vxworks55
     
   if [ "$TOOLKIT" = "tornado_diab" ]; then
      export PROJ_BSP_DIR="$BSP_CONFIG_DIR"'\xcat_bsp_DIAB'
      export CPU_DIR=ARM926EJ_DIAB
   else
      export PROJ_BSP_DIR="$BSP_CONFIG_DIR"'\xcat_bsp'
      export CPU_DIR=ARM926EJ      
   fi   
}
cpu_pre_toolkit_gnueabi()
{
    LINUX_CRFS_SCRIPT_NAME=crfs_arm_gnueabi.sh
}
cpu_pre_toolkit_marvell_v5sft()
{
    LINUX_CRFS_SCRIPT_NAME=crfs_arm_marvell_v5sft.sh
}
linux_2_6_cr_jffs2_params()
{
    UIMAGE_PATH="${DIST}/arch/${CPU_TOOL}/boot"
	UIMAGE_NO_COPY=TRUE
    CR_JFFS2_FILE_NAME="cr_jffs2_img_arm_gnueabi.sh"
    CR_JFFS2_PARAMS="${DIST}/arch/${CPU_TOOL}/boot/uImage ${DIST}/rootfs little_endian ${IMAGE_PATH}"
}
