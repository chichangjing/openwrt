# ARMADAXP_BE.sh
# Target CPU settings

. $tool_build/cpu/ARMADAXP.sh

export CPU_DIR=ARMV7_BE

#
DEFAULT_TOOLKIT_Linux=mv7sft_be
CPU_PRE_TOOLKIT_marvell_v7sft_be=cpu_pre_toolkit_marvell_v7sft_be
CPU_PRE_TOOLKIT_workbench33=cpu_pre_toolkit_workbench33
CPU_PRE_TOOLKIT_workbench33_diab=cpu_pre_toolkit_workbench33
export ENDIAN=BE

#
if [ $TARGET_OS = Linux ]
then
    DEFAULT_LINUX_LSP_NAME=armadaxp_lsp
    DEFAULT_LINUX_KERNEL_VERSION=2.6.31.8
    DEFAULT_LINUX_CONFIG=armadaxp_be_defconfig
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
    export ARM_CPU=ARMADAXP
    export ARM_MMU=ARMADAXP
    export ARM_CACHE=ARMADAXP

    if [ "$TOOLKIT" = "wb_diab" -o "$TOOLKIT" = "workbench33_diab" ]; then
       export PROJ_BSP_DIR="$BSP_CONFIG_DIR"'\armadaxp_bsp_DIAB_BE\'
       export CPU_DIR=ARMV7_DIAB_BE
    else
       export PROJ_BSP_DIR="$BSP_CONFIG_DIR"'\armadaxp_bsp_BE\'
       export CPU_DIR=ARMV7_BE       
    fi
    

    export CC_ARCH=-march=armv7-a
    export CC_ARCH_DIAB=-tARMV7LS:vxworks69
    export USE_GNUEABI_BE=TRUE
    export ENDIAN=BE
	
    info_message " -->cpu_pre_toolkit_workbench33() >> PROJ_BSP_DIR - $PROJ_BSP_DIR"

}

cpu_pre_toolkit_marvell_v7sft_be()
{
    LINUX_CRFS_SCRIPT_NAME=crfs_arm_mv7sft.sh
}
linux_2_6_cr_jffs2_params()
{
    UIMAGE_PATH="${DIST}/arch/${CPU_TOOL}/boot"
	UIMAGE_NO_COPY=TRUE
    CR_JFFS2_FILE_NAME="cr_jffs2_img.sh"
    CR_JFFS2_PARAMS="${DIST}/arch/${CPU_TOOL}/boot/uImage ${DIST}/rootfs big_endian ${IMAGE_PATH}"
}