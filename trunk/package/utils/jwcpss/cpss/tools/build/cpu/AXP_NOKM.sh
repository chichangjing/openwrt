# ARMADAXP_3.4.69.sh
# Target CPU settings

DEFAULT_MANAGEMENT_BUS=PCI_SMI_I2C

export CPU=ARMARCH7
export CPU_TOOL=arm
export CPU_FAMILY=CPU_ARM
export CPU_DIR=ARMV7
export ARM_CPU=ARMADAXP_3_4_69

echo "export linux_objects_ext=ARMADAXP_3.4.69"
export linux_objects_ext="ARMADAXP_3.4.69"
export ARCH=arm
export CROSS_COMPILE=arm-marvell-linux-gnueabi-

#
DEFAULT_TOOLKIT_Linux=marvell_v7sft
DEFAULT_TOOLKIT_vxWorks=workbench33
CPU_PRE_TOOLKIT_marvell_v7sft=cpu_pre_toolkit_marvell_v7sft
CPU_PRE_TOOLKIT_workbench33=cpu_pre_toolkit_workbench33
CPU_PRE_TOOLKIT_workbench33_diab=cpu_pre_toolkit_workbench33
#
if [ $TARGET_OS = Linux ]
then
    DEFAULT_LINUX_KERNEL_VERSION=3.4.69
    DEFAULT_LINUX_LSP_NAME=msys_lsp_3_4
    DEFAULT_LINUX_CONFIG=msys_bc2_cpss_defconfig
    LINUX_CONFIG_NOPRESTERA=YES
    LSP_KERNEL_CONFIG=$DEFAULT_LINUX_CONFIG
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
cpu_pre_toolkit_workbench33()
{
    export ARM_CPU=ARMADAXP_3.4.69
    export ARM_MMU=ARMADAXP_3.4.69
    export ARM_CACHE=ARMADAXP_3.4.69

    if [ "$TOOLKIT" = "wb_diab" -o "$TOOLKIT" = "workbench33_diab" ]; then
       export PROJ_BSP_DIR="$BSP_CONFIG_DIR"'\armadaxp_bsp_DIAB\'
       export CPU_DIR=ARMV7_DIAB
    else
       export PROJ_BSP_DIR="$BSP_CONFIG_DIR"'\armadaxp_bsp\'
       export CPU_DIR=ARMV7       
    fi
    

    export CC_ARCH=-march=armv7-a
    export CC_ARCH_DIAB=-tARMV7LS:vxworks69

    info_message " -->cpu_pre_toolkit_workbench33() >> PROJ_BSP_DIR - $PROJ_BSP_DIR"

}

cpu_pre_toolkit_marvell_v7sft()
{
    LINUX_CRFS_SCRIPT_NAME=crfs_arm_mv7sft.sh
}
linux_2_6_cr_jffs2_params()
{
  UIMAGE_PATH="${DIST}"
  UIMAGE_NO_COPY=TRUE
  CR_JFFS2_FILE_NAME="cr_jffs2_img_arm_mv7sft.sh"
  CR_JFFS2_PARAMS="${DIST}/arch/${CPU_TOOL}/boot/uImage ${DIST}/rootfs little_endian ${IMAGE_PATH}"
}
