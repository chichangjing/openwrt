#
# required environment:
# 
# Version number: $7

os_pre_link()
{
    # prepare to link
    if [ -f simulationLibsCopy.bat ]
    then
        simulationLibsCopy.bat
    fi
    return 0
}

os_link()
{
   if [ -f "`win32_path_to_posix $PROJ_BSP_DIR`"/appDemoSim.exe ]; then
      # file exists, try to remove
      rm "`win32_path_to_posix $PROJ_BSP_DIR`"/appDemoSim.exe
      if [ $? != 0 ]; then
           # failed to remove, error
           echo "appDemoSim is still runing?"
         return 1
         fi
    fi
    # link project
    info_message "gmake -C ${USER_BASE} -f MakefileWin.mak application"
    #info_message "PROJ_BSP_DIR - $PROJ_BSP_DIR"
    gmake -C ${USER_BASE} -f MakefileWin.mak application

    if [ -f "`win32_path_to_posix $PROJ_BSP_DIR`"/appDemoSim.exe ]
    then
          echo "***************************"
          echo "****    SUCCESS !!!    ****"
          echo "***************************"
    fi
    return 0
}

os_post_build()
{

    ###################################### 
    # copy VC appDemoSim
    # no copy VC images with GM lib
    ##################################### 
    if [ -z "$IMAGE_PATH" -o "$IMAGE_PATH" = "-" ]; then
        return 0
    fi

    mkdir -p "$IMAGE_PATH" >/dev/null 2>&1
    info_message "copy $PROJ_BSP_DIR\\appDemoSim.exe $IMAGE_PATH"

    cmd /c "copy $PROJ_BSP_DIR\\appDemoSim.exe $IMAGE_PATH" >/dev/null 2>&1
    cmd /c "copy $PROJ_BSP_DIR\\appDemoSim.map $IMAGE_PATH" >/dev/null 2>&1
    cmd /c "copy $PROJ_BSP_DIR\\appDemoSim.pdb $IMAGE_PATH" >/dev/null 2>&1


    return 0
}

# os definitions
export OS_RUN=win32
unset RTOS_ON_SIM
unset DISTRIBUTED_SIMULATION_ROLE
export ASIC_SIMULATION=EXISTS


###########################################
#  set target directory for VC GM build
###########################################
if [ "$FORCE_GM_LION_USE" = "EXISTS" ];then
   export DESTINATION_BSP_DIR="$COMPILATION_ROOT\\application\\work\\${TOOLKIT}_gm_lion"
elif [ "$FORCE_GM_LION2_USE" = "EXISTS" ];then
   export DESTINATION_BSP_DIR="$COMPILATION_ROOT\\application\\work\\${TOOLKIT}_gm_lion2" 
elif [ "$FORCE_GM_XCAT2_USE" = "EXISTS" ];then
   export DESTINATION_BSP_DIR="$COMPILATION_ROOT\\application\\work\\${TOOLKIT}_gm_xcat2"
elif [ "$FORCE_GM_PUMA3_USE" = "EXISTS" ];then
   export DESTINATION_BSP_DIR="$COMPILATION_ROOT\\application\\work\\${TOOLKIT}_gm_puma3"
elif [ "$FORCE_GM_BOBCAT2_USE" = "EXISTS" ];then
   export DESTINATION_BSP_DIR="$COMPILATION_ROOT\\application\\work\\${TOOLKIT}_gm_bobcat2"
elif [ "$FORCE_GM_BOBCAT3_USE" = "EXISTS" ];then
   export DESTINATION_BSP_DIR="$COMPILATION_ROOT\\application\\work\\${TOOLKIT}_gm_bobcat3"
elif [ "$FORCE_GM_BOBCAT2_B0_USE" = "EXISTS" ];then
   export DESTINATION_BSP_DIR="$COMPILATION_ROOT\\application\\work\\${TOOLKIT}_gm_bobcat2_b0"
elif [ "$FORCE_GM_BOBK_CAELUM_USE" = "EXISTS" ];then
   export DESTINATION_BSP_DIR="$COMPILATION_ROOT\\application\\work\\${TOOLKIT}_gm_bobk_caelum"
elif [ "$FORCE_SUB20_USE" = "EXISTS" ];then
   export DESTINATION_BSP_DIR="$COMPILATION_ROOT\\application\\work\\${TOOLKIT}_sub20"
else
   export DESTINATION_BSP_DIR="$COMPILATION_ROOT\\application\\work\\${TOOLKIT}"

   if [ "$WORDSIZE_REQUIRED" = "64" ]; then
      export DESTINATION_BSP_DIR="$COMPILATION_ROOT\\application\\work\\${TOOLKIT}_64"
   else
      export DESTINATION_BSP_DIR="$COMPILATION_ROOT\\application\\work\\${TOOLKIT}"
   fi

fi




export PROJ_BSP_DIR="$DESTINATION_BSP_DIR"
OS_DEFS="$OS_DEFS WIN32 $CPU"

