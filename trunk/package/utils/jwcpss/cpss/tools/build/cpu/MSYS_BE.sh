# MSYS.sh
# Target CPU settings

. $tool_build/cpu/MSYS.sh

export CPU_DIR=CPU_ARM_BE
export ENDIAN=BE

echo "export linux_objects_ext=MSYS_BE"
export linux_objects_ext="MSYS_BE"
export ARCH=arm
export CROSS_COMPILE=armeb-marvell-linux-gnueabi-

#
DEFAULT_TOOLKIT_Linux=marvell_v7sft_be
CPU_PRE_TOOLKIT_marvell_v7sft_be=cpu_pre_toolkit_marvell_v7sft_be

DEFAULT_LINUX_CONFIG=msys_be_cpss_defconfig
LSP_KERNEL_CONFIG=$DEFAULT_LINUX_CONFIG

# per toolkit settings
# executed _after_ toolkit choosen,
# overrides defaults
cpu_pre_toolkit_marvell_v7sft_be()
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
