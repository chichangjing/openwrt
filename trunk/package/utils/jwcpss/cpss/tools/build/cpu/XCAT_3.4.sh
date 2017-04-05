#
# Target CPU settings

ARCH=arm
export CPU_TOOL=arm
export CPU_FAMILY=CPU_ARM
export CPU_DIR=ARM926EJ
export CPU=ARMARCH5
export ARM_CPU=926EJ
export ARM_MMU=926E
export ARM_CACHE=926E
export KERNEL_SYSCALL_TABLE_EXPORTED=yes
export XCAT_DRV=EXISTS
PROJECT_DEFS="$PROJECT_DEFS XCAT_DRV OS_MEM_64_BIT_ALIGNED"
if [ "$PRODUCT_TYPE" != "POC" ]
then
   export DRAGONITE_TYPE=A1
   PROJECT_DEFS="$PROJECT_DEFS DRAGONITE_TYPE_A1"
fi

echo "export linux_objects_ext=xcat_3.4"
export linux_objects_ext="xcat_3.4"

#
DEFAULT_MANAGEMENT_BUS=PCI_SMI
DEFAULT_TOOLKIT_Linux=mv5sft
DEFAULT_TOOLKIT_vxWorks=tornado
CPU_PRE_TOOLKIT_mv5sft=cpu_pre_toolkit_mv5sft
CPU_PRE_TOOLKIT_marvell_v5sft=cpu_pre_toolkit_marvell_v5sft
CPU_PRE_TOOLKIT_tornado=cpu_pre_toolkit_tornado
#
DEFAULT_LINUX_LSP_NAME=xcat_lsp_3_4
DEFAULT_LINUX_KERNEL_VERSION=3.4
DEFAULT_LINUX_CONFIG=kirkwood_xcat_le_defconfig
LINUX_CRFS_ARCH_ARG=""
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
cpu_pre_toolkit_mv5sft()
{
    LINUX_CRFS_SCRIPT_NAME=crfs_arm_mv5sft.sh
}
cpu_pre_toolkit_marvell_v5sft()
{
    LINUX_CRFS_SCRIPT_NAME=crfs_arm_marvell_v5sft.sh
}
linux_2_6_cr_jffs2_params()
{
  UIMAGE_PATH="${DIST}"
	UIMAGE_NO_COPY=TRUE
  CR_JFFS2_FILE_NAME="cr_jffs2_img_arm_mv5sft.sh"
  CR_JFFS2_PARAMS="${DIST}/arch/${CPU_TOOL}/boot/uImage ${DIST}/rootfs little_endian ${IMAGE_PATH}"
}
