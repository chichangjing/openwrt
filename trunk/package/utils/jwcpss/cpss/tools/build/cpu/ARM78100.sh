# ARM78100.sh
# Target CPU settings - Big Endian

ARCH=arm
export CPU_TOOL=arm
export CPU_FAMILY=CPU_ARM
export CPU_DIR=ARM78100
export CPU=ARMARCH5
export ARM_CPU=78100
#
DEFAULT_MANAGEMENT_BUS=PCI
DEFAULT_TOOLKIT_Linux=gnueabi_be
DEFAULT_TOOLKIT_vxWorks=tornado
CPU_PRE_TOOLKIT_gnueabi_be=cpu_pre_toolkit_gnueabi_be
CPU_PRE_TOOLKIT_tornado=cpu_pre_toolkit_tornado
#
if [ $TARGET_OS = Linux ]
then
    DEFAULT_LINUX_LSP_NAME=mv78100_amc_lsp_2_6
    DEFAULT_LINUX_KERNEL_VERSION=2.6.22.18
    DEFAULT_LINUX_CONFIG=db78xx0_amc_be_defconfig
    LINUX_CRFS_ARCH_ARG=""
fi
#
# per toolkit settings
# executed _after_ toolkit choosen,
# overrides defaults
cpu_pre_toolkit_tornado()
{
    echo "***************************************************************"
    echo "TORNADO settins UNKNOWN in $tool_build/cpu/ARM78100.sh"
    echo "Please fill and remove these lines"
    exit 1
    export WIND_BASE='c:\TornadoARM'
    export PROJ_BSP_DIR="$BSP_CONFIG_DIR"'\mv78100_amc_bspBE'
    export CC_ARCH=-march=armv5
    export CC_ARCH_DIAB=-tARMV5LN:vxworks55
	export USE_GNUEABI_BE=TRUE
    export ENDIAN=BE
}
cpu_pre_toolkit_gnueabi_be()
{
    LINUX_CRFS_SCRIPT_NAME=crfs_arm_gnueabi.sh
}
linux_2_6_cr_jffs2_params()
{
    UIMAGE_PATH="${DIST}/arch/${CPU_TOOL}/boot"
    CR_JFFS2_FILE_NAME="cr_jffs2_img_discoduo.sh"
    CR_JFFS2_PARAMS="${DIST}/rootfs/uImage ${DIST}/rootfs big_endian ${IMAGE_PATH}"
}
