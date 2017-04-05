# EP3041A.sh
# ep3041a
# Target CPU settings
ARCH=freescale-2011.03
export CPU_TOOL=powerpc
export CPU_FAMILY=POWERPC_CPU
export CPU=EP3041A
export CPU_DIR=EP3041A
export ENDIAN=BE
export EP3041A_DRV=EXISTS
echo "export linux_objects_ext=ep3041a_lsp"
export linux_objects_ext="ep3041a_lsp"


if [ "$TARGET_OS" != "FreeBSD" -a "$TOOLKIT" != "workbench26_diab" ]; then
	CPU_FLAGS="-meabi -mstrict-align"
fi

PROJECT_DEFS="$PROJECT_DEFS EP3041A_DRV PHYSADDR_IS_64BIT"
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
    DEFAULT_LINUX_KERNEL_VERSION=3.0.6
    DEFAULT_LINUX_LSP_NAME=ep3041a_lsp
    DEFAULT_LINUX_CONFIG=mv_ep3041a_defconfig
    LINUX_CRFS_SCRIPT_NAME=crfs_ep3041a.sh
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

   if [ "$TOOLKIT" = "wb_diab" -o "$TOOLKIT" = "workbench26_diab" ]; then
      export PROJ_BSP_DIR=export PROJ_BSP_DIR="$BSP_CONFIG_DIR\\EP3041A_6.5_DIAB"
      export CPU_DIR=PPC85XX_DIAB
   else
      export PROJ_BSP_DIR="$BSP_CONFIG_DIR\\EP3041A_6.5"
   fi
}

linux_2_6_cr_jffs2_params()
{
    CR_JFFS2_FILE_NAME="cr_jffs2_img_ep3041a.sh"
    CR_JFFS2_PARAMS="${IMAGE_PATH} ${DIST}/rootfs ${TOOL_CHAIN_PATH} 128"
    UIMAGE_PATH="${DIST}/arch/${CPU_TOOL}/boot"
}
