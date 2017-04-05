# MSYS_2_6_32.sh
# Target CPU settings

DEFAULT_MANAGEMENT_BUS=PCI_SMI_I2C

export CPU=ARMARCH7
export ARCH=arm
export CPU_TOOL=arm
export CPU_FAMILY=CPU_ARM
export CPU_DIR=ARMV7
export ARM_CPU=MSYS_2_6_32
export CROSS_COMPILE=arm-marvell-linux-gnueabi-

#    echo "export linux_objects_ext=MSYS"
#    export linux_objects_ext="MSYS"

echo "export linux_objects_ext=MSYS_2_6_32"
export linux_objects_ext="MSYS_2_6_32"

#
DEFAULT_TOOLKIT_Linux=marvell_v7sft
CPU_PRE_TOOLKIT_marvell_v7sft=cpu_pre_toolkit_marvell_v7sft

DEFAULT_LINUX_LSP_NAME=msys_lsp_2_6_32
DEFAULT_LINUX_KERNEL_VERSION=2.6.32.59
DEFAULT_LINUX_CONFIG=msys_bc2_cpss_defconfig
LSP_KERNEL_CONFIG=$DEFAULT_LINUX_CONFIG

###############################
# Set CPSS LOG_ENABLE
###############################
if [ -z $API_LOG_ENABLE ]; then
    export LOG_ENABLE="L_ON"
fi
# per toolkit settings
# executed _after_ toolkit choosen,
# overrides defaults
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
