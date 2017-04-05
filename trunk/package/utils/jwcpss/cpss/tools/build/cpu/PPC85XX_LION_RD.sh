# PQ3.sh
# or gda8548 ?
# Target CPU settings

ARCH=85xx
export CPU_TOOL=ppc
export CPU_FAMILY=PPC_CPU
export CPU=PPC85XX
export CPU_DIR=PPC85XX
export GDA8548_DRV=EXISTS
if [ $TARGET_OS != "FreeBSD" ]; then
	CPU_FLAGS="-meabi -mstrict-align"
fi
PROJECT_DEFS="$PROJECT_DEFS GDA8548_DRV"
export ENDIAN=BE
#
DEFAULT_MANAGEMENT_BUS=PCI
DEFAULT_TOOLKIT_Linux=eldk
DEFAULT_TOOLKIT_FreeBSD=fbsd
DEFAULT_TOOLKIT_vxWorks=workbench26
CPU_PRE_TOOLKIT_workbench26=cpu_pre_toolkit_workbench26
CPU_PRE_TOOLKIT_workbench26_diab=cpu_pre_toolkit_workbench26
#
if [ $TARGET_OS = Linux ]
then
    DEFAULT_LINUX_KERNEL_VERSION=2.6.22.18
    DEFAULT_LINUX_LSP_NAME=gda8548_lsp
    DEFAULT_LINUX_CONFIG=mv_lion_mpc8544_defconfig
    LINUX_CRFS_SCRIPT_NAME=crfs_gda8548.sh
    LINUX_CRFS_ARCH_ARG=$ARCH
fi
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
cpu_pre_toolkit_workbench26()
{
   export CC_ARCH=-mcpu=8540
   export CC_ARCH_DIAB=-tPPCE500FS:vxworks65
   export BOARD=LION_RD

   if [ "$TOOLKIT" = "wb_diab" -o "$TOOLKIT" = "workbench26_diab" ]; then
      export PROJ_BSP_DIR=export PROJ_BSP_DIR="$BSP_CONFIG_DIR\\lion_mpc8544_DIAB_bsp"
   else
      export PROJ_BSP_DIR="$BSP_CONFIG_DIR\\lion_mpc8544_bsp"
   fi
}

linux_2_6_cr_jffs2_params()
{
    CR_JFFS2_FILE_NAME="cr_jffs2_img_lion_mpc8544.sh"
    CR_JFFS2_PARAMS="${IMAGE_PATH} ${DIST}/rootfs ${TOOL_CHAIN_PATH} 256"
    UIMAGE_PATH="${DIST}/arch/${CPU_TOOL}/boot/images"
}
