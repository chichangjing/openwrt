# ARM78200.sh
# Target CPU settings

. $tool_build/cpu/ARM78100.sh

export CPU_DIR=ARM78200
export ARM_CPU=78200

echo "export linux_objects_ext=dd_lsp"
export linux_objects_ext="dd_lsp"

#
DEFAULT_TOOLKIT_Linux=mv5sft
CPU_PRE_TOOLKIT_mv5sft=cpu_pre_toolkit_mv5sft
CPU_PRE_TOOLKIT_marvell_v5sft=cpu_pre_toolkit_marvell_v5sft
CPU_PRE_TOOLKIT_tornado_diab=cpu_pre_toolkit_tornado
#
if [ $TARGET_OS = Linux ]
then
    DEFAULT_LINUX_LSP_NAME=dd_lsp
    DEFAULT_LINUX_KERNEL_VERSION=2.6.31.8
    DEFAULT_LINUX_CONFIG=mv78200_defconfig
fi
#
###############################
# Set CPSS LOG_ENABLE
###############################
if [ -z $API_LOG_ENABLE ]; then
    export LOG_ENABLE="L_ON"
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

    if [ "$TOOLKIT" = "tornado_diab" ]; then
       export PROJ_BSP_DIR="$BSP_CONFIG_DIR"'\dd_bsp_DIAB'
       export CPU_DIR=ARM78200_DIAB
    else
       export PROJ_BSP_DIR="$BSP_CONFIG_DIR"'\dd_bsp'
       export CPU_DIR=ARM78200      
    fi 	

    export CC_ARCH=-march=armv5
    export CC_ARCH_DIAB=-tARMV5LS:vxworks55
}

cpu_pre_toolkit_mv5sft()
{
    LINUX_CRFS_SCRIPT_NAME=crfs_arm_mv5sft.sh
}
cpu_pre_toolkit_marvell_v5sft()
{
    LINUX_CRFS_SCRIPT_NAME=crfs_arm_marvell_v5sft.sh
}
cpu_pre_toolkit_gnueabi()
{
    LINUX_CRFS_SCRIPT_NAME=crfs_arm_gnueabi.sh
}
linux_2_6_cr_jffs2_params()
{
    UIMAGE_PATH="${DIST}/arch/${CPU_TOOL}/boot"
	UIMAGE_NO_COPY=TRUE
    CR_JFFS2_FILE_NAME="cr_jffs2_img_discoduo.sh"
    CR_JFFS2_PARAMS="${DIST}/arch/${CPU_TOOL}/boot/uImage ${DIST}/rootfs little_endian ${IMAGE_PATH}"
}