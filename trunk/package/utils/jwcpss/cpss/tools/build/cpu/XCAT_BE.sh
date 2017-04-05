#
# Target CPU settings

. $tool_build/cpu/XCAT.sh

export CPU_DIR=ARM926EJ_BE
export ENDIAN=BE

echo "export linux_objects_ext=xcat_be"
export linux_objects_ext="xcat_be"


DEFAULT_TOOLKIT_Linux=gnueabi_be
CPU_PRE_TOOLKIT_gnueabi_be=cpu_pre_toolkit_gnueabi_be
CPU_PRE_TOOLKIT_marvell_v5sft_be=cpu_pre_toolkit_marvell_v5sft_be
#
if [ $TARGET_OS = Linux ]
then
    DEFAULT_LINUX_CONFIG=mv88fxcat_be_defconfig
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
cpu_pre_toolkit_tornado()
{
    ##
    ## WIND_BASE can be set by user
    ##
   if [ -z $WIND_BASE ]
   then
      export WIND_BASE='c:\TornadoARM'
      echo "WIND_BASE - $WIND_BASE"
   else   
      ### check if exists WIND_BASE defined by user 
      if [ -d $WIND_BASE ]
      then
         echo "WIND_BASE - $WIND_BASE"
      else
         echo "Wrong WIND_BASE - $WIND_BASE"
         exit 0
      fi
   fi

   export CC_ARCH=-march=armv5
   export CC_ARCH_DIAB=-tARMV5ES:vxworks55
   export USE_GNUEABI_BE=TRUE
   
   if [ "$TOOLKIT" = "tornado_diab" ]; then
      export PROJ_BSP_DIR="$BSP_CONFIG_DIR"'\xcat_bsp_DIAB_BE'
      export CPU_DIR=ARM926EJ_DIAB_BE
	  export TOOL=diabbe
   else
      export PROJ_BSP_DIR="$BSP_CONFIG_DIR"'\xcat_bsp_BE'
      export CPU_DIR=ARM926EJ_BE
   fi     

}
cpu_pre_toolkit_marvell_v5sft_be()
{
    LINUX_CRFS_SCRIPT_NAME=crfs_arm_marvell_v5sft.sh
}
cpu_pre_toolkit_gnueabi_be()
{
    LINUX_CRFS_SCRIPT_NAME=crfs_arm_gnueabi.sh
}
linux_2_6_cr_jffs2_params()
{
    UIMAGE_PATH="${DIST}/arch/${CPU_TOOL}/boot"
	UIMAGE_NO_COPY=TRUE	
    CR_JFFS2_FILE_NAME="cr_jffs2_img_arm_gnueabi.sh"
    CR_JFFS2_PARAMS="${DIST}/arch/${CPU_TOOL}/boot/uImage ${DIST}/rootfs big_endian ${IMAGE_PATH}"
}
