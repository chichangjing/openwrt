#
# MIPS64 CPU support
#
# File version: 1

export CPU=MIPS64
export CPU_TOOL=mips
export CPU_DIR=MIPS64
export CPU_FAMILY=MIPS_CPU
export WORDSIZE_REQUIRED="64"
export ENDIAN=BE
CPU_FLAGS="-mlong-calls -mips3 -mgp32 -mfp32 -non_shared -G 0"
DEFAULT_TOOLKIT_Linux=mips64
CPU_PRE_TOOLKIT_mips64=cpu_pre_toolkit_mips64
DEFAULT_MANAGEMENT_BUS=PCI_SMI
PROJECT_DEFS="$PROJECT_DEFS PHYSADDR_IS_64BIT KERNEL_IS_64BIT"

#
if [ $TARGET_OS = Linux ]
then
  DEFAULT_LINUX_LSP_NAME=mips64_lsp
  DEFAULT_LINUX_KERNEL_VERSION=2.6.27.7
  DEFAULT_LINUX_CONFIG=mv_mips64_defconfig
  LINUX_CRFS_ARCH_ARG=""
fi
#

cpu_pre_toolkit_mips64()
{
  LINUX_CRFS_SCRIPT_NAME=crfs_mips64.sh
}

linux_2_6_cr_jffs2_params()
{
  UIMAGE_PATH="${DIST}"
	UIMAGE_NO_COPY=TRUE
  CR_JFFS2_FILE_NAME="cr_jffs2_image_mips64.sh"
  CR_JFFS2_PARAMS="${IMAGE_PATH} ${DIST}/rootfs"
}
