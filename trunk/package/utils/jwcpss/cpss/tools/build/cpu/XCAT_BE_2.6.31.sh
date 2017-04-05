#
# Target CPU settings

. $tool_build/cpu/XCAT_2.6.31.sh

DEFAULT_TOOLKIT_Linux=mv5sft_be
CPU_PRE_TOOLKIT_mv5sft_be=cpu_pre_toolkit_mv5sft_be
CPU_PRE_TOOLKIT_marvell_v5sft_be=cpu_pre_toolkit_marvell_v5sft_be
export ENDIAN=BE
#
DEFAULT_LINUX_CONFIG=mv88fxcat_be_defconfig
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
cpu_pre_toolkit_mv5sft_be()
{
    LINUX_CRFS_SCRIPT_NAME=crfs_arm_mv5sft.sh
}
cpu_pre_toolkit_marvell_v5sft_be()
{
    LINUX_CRFS_SCRIPT_NAME=crfs_arm_marvell_v5sft.sh
}
linux_2_6_cr_jffs2_params()
{
    UIMAGE_PATH="${DIST}/arch/${CPU_TOOL}/boot"
	UIMAGE_NO_COPY=TRUE	
    CR_JFFS2_FILE_NAME="cr_jffs2_img_arm_mv5sft.sh"
    CR_JFFS2_PARAMS="${DIST}/arch/${CPU_TOOL}/boot/uImage ${DIST}/rootfs big_endian ${IMAGE_PATH}"
}
