#
# required environment:
#   BSP_CONFIG_DIR          (Fx: Z:\cpss\Bsp\config)
# 
#

if [ "$DRAGONITE_TYPE" = "A1" ]
then
    dragoniteFwPath=${CPSS_PATH}/cpssEnabler/mainSysConfig/src/appDemo/dragonite/firmware
    if [ -e ${dragoniteFwPath}/drigonite_fw.bin ]
    then
	info_message "Dragonite FW translation to assembly format:"
	info_message "binToAsm ${dragoniteFwPath}/drigonite_fw.bin >${dragoniteFwPath}/drigonite_fw.s"
	binToAsm ${dragoniteFwPath}/drigonite_fw.bin >${dragoniteFwPath}/drigonite_fw.s
    fi
fi   

os_link()
{
   if [ "$ARM_CPU" = "ARMADAXP" -a "$PRODUCT_TYPE" = "CPSS" ];then
      echo ">>> No link for PSS WB33 ARMADAXP <<<"
      return 0
   fi
   

    # link project
    info_message "make -C $PROJ_BSP_DIR vxWorks.st"
    #make -C "$PROJ_BSP_DIR" vxWorks.st
    make -C "$PROJ_BSP_DIR" vxWorks.st.bin

    #####################################################################
    # build bz2 file for PSS and CPSS (PRODUCT_TYPE exists in PSS only)
    #####################################################################
    
    if [ "$PRODUCT_TYPE" = "CPSS" ]; then  
       echo -E "cmd /c $PSS_PATH\\tools\\bin\\bzip2.exe -vfkz1 $PROJ_BSP_DIR\\vxWorks.st"
       cmd /c "$PSS_PATH\\tools\\bin\\bzip2.exe -vfkz1 $PROJ_BSP_DIR\\vxWorks.st" >/dev/null 2>&1
    else
       echo -E "cmd /c $CPSS_PATH\\tools\\bin\\bzip2.exe -vfkz1 $PROJ_BSP_DIR\\vxWorks.st"
       cmd /c "$CPSS_PATH\\tools\\bin\\bzip2.exe -vfkz1 $PROJ_BSP_DIR\\vxWorks.st" >/dev/null 2>&1
    fi
    
    if [ -f "`win32_path_to_posix $PROJ_BSP_DIR`"/vxWorks.st ]
    then
       echo "***************************"
       echo "****    SUCCESS !!!    ****"
       echo "***************************"
    fi
    return 0
}

os_post_build()
{   
    # copy vxWorks.st
    if [ -z "$IMAGE_PATH" -o "$IMAGE_PATH" = "-" ]; then
        return 0
    fi

    mkdir -p "$IMAGE_PATH" >/dev/null 2>&1
    info_message "copy $PROJ_BSP_DIR\\vxWorks.st $IMAGE_PATH"
    cmd /c "copy $PROJ_BSP_DIR\\vxWorks.st $IMAGE_PATH" >/dev/null 2>&1

    # copy bz2 file to image.zip for both CPSS and PSS
    info_message "copy $PROJ_BSP_DIR\\vxWorks.st.bz2 $IMAGE_PATH"
    cmd /c "copy $PROJ_BSP_DIR\\vxWorks.st.bz2 $IMAGE_PATH" >/dev/null 2>&1        
    
    return 0
}

# os definitions
export OS_RUN=VXWORKS
OS_DEFS="$OS_DEFS VXWORKS FLAT_MEMORY EMBEDDED"
