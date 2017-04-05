#
# INTEL64 CPU support
#
# File version: 5

export CPU=INTEL64
export CPU_TOOL=x86
export CPU_DIR=INTEL64
export LIB_1=64
if [ -d /usr/lib/x86_64-linux-gnu/. ]; then
  # Ubuntu: override
  LIB_1=/x86_64-linux-gnu
fi
export INTEL64_DRV=EXISTS
PROJECT_DEFS="$PROJECT_DEFS INTEL64_DRV PHYSADDR_IS_64BIT KERNEL_IS_64BIT"

## Since CPSS is built with host gcc for this CPU it will be linked
## with system glibc. Currently it is glibc 2.19
## On INTEL64 glibc 2.13 installed, so it is impossible to run appDemo there
## Solution: create a static-linked appDemo
##
## Uncomment next two lines to run appDemo on board with an old glibc
#export STATIC_LINKED_APPDEMO=YES
#PROJECT_DEFS="$PROJECT_DEFS STATIC_LINKED_APPDEMO"

export CPU_FAMILY=INTEL64_CPU
export WORDSIZE_REQUIRED="64"
export IS_64BIT_OS=YES
DEFAULT_TOOLKIT_Linux=intel64
CPU_PRE_TOOLKIT_intel64=cpu_pre_toolkit_intel64
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

cpu_pre_toolkit_intel64()
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
