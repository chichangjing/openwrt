############################################################################ 
# win32_marvell_cpss_int.sh
#
# DESCRIPTION
# host definitions Marvell internal environment
# for integrator 
# integrator :
#     - does not copy object files from server as win32_marvell_cpss_dev.sh does
#     - does not check view type as win32_marvell_cpss_dev.sh does
#     - copy BSP source but not objects
#     - write "token.$toolkit" file to compilation root 
#       for success/no success identification  
#
# FILE REVISION NUMBER:
#       $Revision: 4 $
##############################################################################

. $tool_build/host/win32_marvell_dev.inc

# check default CPSS paths
#
    if [ "$CPSS_PATH" = "" ]; then
        CPSS_PATH="$DRIVE\\cpss\\SW\\prestera"
    fi
    case "${FAMILY:-NONE}" in
        # EX
        EX_ALL | EX_TG | EX_RD)  family_subdir=Ex ;;
        NONE)                    family_subdir=Ex; FAMILY=EX_TG ;;
        # DX
        DX|DX_ALL)                  family_subdir=Dx ;;
        DX_SAL)                  family_subdir=DxSal ;;
        DX_CHX | DX_CH2)         family_subdir=DxCh ;;
        # ExDx
        EX_DX_ALL)               family_subdir=ExDx ;;
        EX_TG_DX_SAL)            family_subdir=ExDxSal ;;
        EX_TG_DX_CH)             family_subdir=ExDxCh ;;
        # ExMxPm
        EXMXPM)                  family_subdir=Pm ;;
        # ExMxPm , ALL DX
        EXMXPM_DX_CH)            family_subdir=PmDxCh ;;
        # BROKER
        BROKER)
            family_subdir=Broker
            error_message "Broker: not yet done"
            exit 1
            ;;
        *)
            error_message "family check failed: unknown family $FAMILY"
            exit 1
    esac

    test "$XBAR_TYPE" = "DUNE" && family_subdir=${family_subdir}_Dune
    c_temp_dtemp="C:\\temp\\dtemp"
    GM_DLL_ROOT="$c_temp_dtemp\\gm_dll"

    if [ "$COMPILATION_ROOT" = "" -o "x$COMPILATION_ROOT" = "x-" ]; then
        COMPILATION_ROOT="$c_temp_dtemp\\${family_subdir}_${CC_CURRENT_CHANGE_SET}"
    fi
    # owerwrite BSP_CONFIG_DIR
    export BSP_CONFIG_DIR="$COMPILATION_ROOT\\config"
#########################################################
##### END default CPSS paths
#########################################################

win32_marvell_BSP_copy()
{
    # copy_bsp

    case $makefile_location_type in
            ARM5*81*)
                if win32_path_exists "$DESTINATION_BSP_DIR\\swControllers\\VxBsp\\makefile"
                then
                    already_copied=yes
                fi
                if win32_path_exists "$UNPACKED_BSP_DIR\\swControllers\\VxBsp\\makefile"
                then
                   bsp_unpacked=yes
                fi
                ;;
            ARM_EV | ARM_RD)
                if win32_path_exists "$DESTINATION_BSP_DIR\\BSP\\makefile"
                then
                    already_copied=yes
                fi
                if win32_path_exists "$UNPACKED_BSP_DIR\\BSP\\makefile"
                then
                    bsp_unpacked=yes
                fi;;
            *)

               if win32_path_exists "$DESTINATION_BSP_DIR\\makefile"
                then
                    already_copied=yes
                fi
                if win32_path_exists "$UNPACKED_BSP_DIR\\makefile"
                then
                    bsp_unpacked=yes
                fi;;
    esac

    if [ "$bsp_unpacked" = "yes" ]; then
       info_message "****    Copy unpacked BSP    ****"
       xcopy "$UNPACKED_BSP_DIR\\*.*"   "$DESTINATION_BSP_DIR\\"  /E /H /R /Y /Q
       already_copied=yes
    fi

    if [ "$already_copied" != "yes" ]; then
       info_message "****    Copy BSP from ClearCase   ****"
       xcopy "$CC_BSP_DIR\\*.*"   "$DESTINATION_BSP_DIR\\"  /E /H /R /Y /Q
    fi

    return 0
}

#
# copy BSP from server
#
host_pre_build()
{
    win32_add_paths $CPSS_TOOLS_PATH'\tools\bin'
    win32_marvell_BSP_detect_paths || return 1

    check_create_dir "$COMPILATION_ROOT"

    win32_marvell_BSP_copy || return 1
    return 0
}



host_post_build()
{

   if [ -f  $PROJ_BSP_DIR/vxWorks.st ]   
   then
      touch $COMPILATION_ROOT/token.$FAMILY
   fi
    return 0
}



