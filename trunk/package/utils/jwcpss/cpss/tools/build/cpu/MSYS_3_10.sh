# MSYS_3_10.sh
# Internal CPU on AC3, BC2, BobK
# Currently .dbb file generated for BobK
# Target CPU settings

DEFAULT_MANAGEMENT_BUS=PCI_SMI_I2C

export CPU=ARMARCH7
export CPU_TOOL=arm
export CPU_FAMILY=CPU_ARM
export CPU_DIR=ARMV7
export ARM_CPU=MSYS

echo "export linux_objects_ext=MSYS"
export linux_objects_ext="MSYS"
export ARCH=arm
export CROSS_COMPILE=arm-marvell-linux-gnueabi-

#
DEFAULT_TOOLKIT_Linux=marvell_v7sft
CPU_PRE_TOOLKIT_marvell_v7sft=cpu_pre_toolkit_marvell_v7sft
#
if [ $TARGET_OS = Linux ]
then
    #DEFAULT_LINUX_LSP_NAME=armada385/kernel
    DEFAULT_LINUX_LSP_NAME=3.10/kernel
    DEFAULT_LINUX_KERNEL_VERSION=3.10.70
    DEFAULT_LINUX_CONFIG=mvebu_lsp_defconfig
    LSP_KERNEL_TYPE=zImage
    LSP_KERNEL_CONFIG=$DEFAULT_LINUX_CONFIG
    #LSP_KERNEL_DTB_NAME=(msys-bobk-caelum-db.dtb msys-bobk-cetus-db.dtb msys-bc2-db.dtb)
    #LSP_KERNEL_UDTB_NAME=(msys_caelum msys_cetus msys_bc2)
    LSP_KERNEL_DTB_NAME[0]=msys-bobk-caelum-db.dtb
    LSP_KERNEL_UDTB_NAME[0]=msys_caelum

    LSP_KERNEL_DTB_NAME[1]=msys-bobk-cetus-db.dtb
    LSP_KERNEL_UDTB_NAME[1]=msys_cetus

    LSP_KERNEL_DTB_NAME[2]=msys-bc2-db.dtb
    LSP_KERNEL_UDTB_NAME[2]=msys_bc2

    export UNI_DIR=uni
fi
#
#
###############################
# Set CPSS LOG_ENABLE
###############################
if [ -z $API_LOG_ENABLE ]; then
    export LOG_ENABLE="L_ON"
fi
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
