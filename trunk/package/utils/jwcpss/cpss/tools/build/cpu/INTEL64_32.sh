#
# INTEL64 CPU support
# 64-bit kernel
# 32-bit applications
#
# File version: 1

export CPU=i386
export CPU_TOOL=x86
export CPU_DIR=INTEL64_32
export LIB_1=32
#if [ -d /usr/lib/x86_64-linux-gnu/. ]; then
#  # Ubuntu: override
#  LIB_1=/x86_64-linux-gnu
#fi
export INTEL64_DRV=EXISTS
PROJECT_DEFS="$PROJECT_DEFS INTEL64_DRV PHYSADDR_IS_64BIT KERNEL_IS_64BIT"

export CPU_FAMILY=INTEL64_CPU
#export WORDSIZE_REQUIRED="64"
#export IS_64BIT_OS=YES
DEFAULT_TOOLKIT_Linux=intel64_32
CPU_PRE_TOOLKIT_intel64_32=cpu_pre_toolkit_intel64_32
DEFAULT_MANAGEMENT_BUS=PCI_SMI

if [ $TARGET_OS = Linux ]
then
  DEFAULT_LINUX_LSP_NAME=intel64_lsp
  DEFAULT_LINUX_KERNEL_VERSION=2.6.35.13
  DEFAULT_LINUX_CONFIG=mv_intel64_defconfig
  LINUX_CRFS_ARCH_ARG=""
fi
#
###############################
# Set CPSS LOG_ENABLE
###############################
if [ -z $API_LOG_ENABLE ]; then
    export LOG_ENABLE="L_ON"
fi

cpu_pre_toolkit_intel64_32()
{
  LINUX_CRFS_SCRIPT_NAME=crfs_intel64.sh
}

linux_2_6_cr_jffs2_params()
{
  UIMAGE_PATH="${DIST}"
	UIMAGE_NO_COPY=TRUE
  CR_JFFS2_FILE_NAME="cr_jffs2_image_intel64.sh"
  CR_JFFS2_PARAMS="${DIST} ${IMAGE_PATH}"
}
