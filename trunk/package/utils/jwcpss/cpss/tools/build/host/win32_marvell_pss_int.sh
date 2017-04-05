############################################################################ 
# win32_marvell_pss_int.sh
#
# DESCRIPTION
# host definitions Marvell internal environment
# for integrator 
# integrator :
#     - does not copy object files from server as win32_marvell_pss_dev.sh does
#     - does not check view type as win32_marvell_pss_dev.sh does
#     - copy BSP source but not objects
#     - write "token.$toolkit" file to compilation root 
#       for success/no success identification  
#
# FILE REVISION NUMBER:
#       $Revision: 1 $
##############################################################################

. $tool_build/host/win32_marvell_dev.inc

# check default PSS paths
#
    if [ "$PSS_PATH" = "" ]; then
        PSS_PATH="$DRIVE\\pss\\SW\\prestera"
    fi
    case "${FAMILY:-NONE}" in
        # EX
        EX)  family_subdir=EX ;;
        # DX
        DX)  family_subdir=DXCPSS ;;
        DXDPSS)  family_subdir=DXDPSS ;;
        EXMXPM)  family_subdir=EXMXPMCPSS ;;
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
    case $releasetype in
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



