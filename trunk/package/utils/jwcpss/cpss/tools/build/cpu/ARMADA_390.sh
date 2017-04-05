# ARMADA_390.sh
# Target CPU settings, linux only (now)

DEFAULT_MANAGEMENT_BUS=PCI_SMI_I2C

export CPU=ARMARCH7
export CPU_TOOL=arm
export CPU_FAMILY=CPU_ARM
export CPU_DIR=ARMV7
export ARM_CPU=ARMADA_390

export linux_objects_ext="ARMADA_390"
export ARCH=arm
export CROSS_COMPILE=arm-marvell-linux-gnueabi-

#
DEFAULT_TOOLKIT_Linux=marvell_v7sft
CPU_PRE_TOOLKIT_marvell_v7sft=cpu_pre_toolkit_marvell_v7sft

DEFAULT_LINUX_LSP_NAME=3.14/kernel
DEFAULT_LINUX_KERNEL_VERSION=3.14.33
DEFAULT_LINUX_CONFIG=mvebu_v7_lsp_defconfig
LSP_KERNEL_TYPE=zImage
LSP_KERNEL_CONFIG=$DEFAULT_LINUX_CONFIG
LSP_KERNEL_DTB_NAME=armada-39x-modular.dtb
LSP_KERNEL_UDTB_NAME=390

export UNI_DIR=uni


echo "DEFAULT_LINUX_LSP_NAME=${DEFAULT_LINUX_LSP_NAME}"
echo "DEFAULT_LINUX_KERNEL_VERSION=${DEFAULT_LINUX_KERNEL_VERSION}"
echo "DEFAULT_LINUX_CONFIG=${DEFAULT_LINUX_CONFIG}"

echo "UNI_DIR=${UNI_DIR}"

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
cpu_pre_toolkit_marvell_v7sft()
{
    LINUX_CRFS_SCRIPT_NAME=crfs_arm_mv7sft.sh
}
linux_2_6_cr_jffs2_params()
{
  UIMAGE_PATH="${DIST}"
  UIMAGE_NO_COPY=TRUE
  echo "echo hello" >$DIST/ubi_script.txt
  CR_JFFS2_FILE_NAME="cr_jffs2_img_arm_mv7sft.sh"
  CR_JFFS2_PARAMS="\
        ${DIST}/arch/${CPU_TOOL}/boot/zImage \
        ${DIST}/arch/${CPU_TOOL}/boot/dts/${1:-$LSP_KERNEL_DTB_NAME} \
        ${DIST}/rootfs \
        little_endian \
        ${IMAGE_PATH} \
        $DIST/ubi_script.txt"
}
