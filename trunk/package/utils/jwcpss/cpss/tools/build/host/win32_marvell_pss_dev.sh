# host definitions Marvell internal environment
# (WIN32)
#
#

. $tool_build/host/win32_marvell_dev.inc


if [ "$CC_VIEW_TYPE" != "DeveloperView" ]; then
    echo -E "CC_VIEW_TYPE - $CC_VIEW_TYPE"
    error_message "Wrong view type"
#    exit 1
fi

#########################################################
##### START default CPSS paths
#########################################################
    if [ "$PSS_PATH" = "" ]; then
        PSS_PATH="$DRIVE\\pss\\SW\\prestera"
    fi

    echo -E "FAMILY - $FAMILY"

    case "${FAMILY:-NONE}" in
        # EX
        EX)  family_subdir=EXCPSS ;;
        EXPSS)  family_subdir=EX ;;
        # DX
        DX) if [ "$PSS_MODE" = "PSS_DPSS" ]
            then
               family_subdir=DXDPSS
            else
               family_subdir=DX
            fi
        ;;
        #PM
        EXMXPM)  family_subdir=EXMXPM ;;
        *)
            error_message "family check failed: unknown family $FAMILY"
            exit 1
    esac

#    test "$XBAR_TYPE" = "DUNE" && family_subdir=${family_subdir}_Dune
    c_temp_dtemp="C:\\temp\\dtemp"
    GM_DLL_ROOT="$c_temp_dtemp\\gm_dll"
    OBJ_NETWORK_ROOT="F:\\Objects\\pss\\$CC_RELEASE_STREAM\\$CC_CURRENT_BASE_LINE\\vxWorks\\$family_subdir"
    if [ "$COMPILATION_ROOT" = "" -o "x$COMPILATION_ROOT" = "x-" ]; then
        COMPILATION_ROOT="$c_temp_dtemp\\${family_subdir}_${CC_CURRENT_CHANGE_SET}"
    fi
    # owerwrite BSP_CONFIG_DIR
    export BSP_CONFIG_DIR="$COMPILATION_ROOT\\config"
#########################################################
##### END default CPSS paths
#########################################################

host_pre_build()
{
    win32_add_paths $CPSS_TOOLS_PATH'\tools\bin'

    win32_marvell_BSP_detect_paths || return 1


    # copy objects from the last base line.

    echo -E "check_create_dir COMPILATION_ROOT - $COMPILATION_ROOT"
    check_create_dir "$COMPILATION_ROOT"

    echo -E "OBJ_NETWORK_ROOT - $OBJ_NETWORK_ROOT"

    if win32_path_not_exists "$OBJ_NETWORK_ROOT" ; then
        warning_message "no files found on server to copy"
        COPY_SERVER_FILES=COPY_FAIL
    #
    # in developer mode if no objects on server goto cmd prompt 
    #
       if [ "$mode" = "developer" ]
       then
          return 0
       fi

    else
        OBJ_LIB_DIR_NAME="$TMP_OBJ_LIB_DIR_NAME"
        
        if win32_path_exists "$COMPILATION_ROOT\\mainDrv\\libs\\$OBJ_LIB_DIR_NAME" ; then
            info_message ""
            info_message "compilation root already hold files from server -- action will not copy files"
            info_message ""
        else
        
            for A in \
                mainAppDemo mainCmd mainDrv mainOs mainDrvExtLib mainExtDrv \
                     mainExtLibs mainGaltisAgent mainOs mainXbarDrv simulation 
                
            do
                copy_baseline_objects $A $OBJ_LIB_DIR_NAME
            done
            
            # copy SAND files if needed
            if [ "$XBAR_TYPE" = "DUNE" ]; then
                for A in mainFxDrv ; do
                    copy_baseline_objects $A $OBJ_LIB_DIR_NAME
                done
            fi
            
            # extention library has the customers directories --> the list is %%A parameter
            # the libraries within every customr listed in %%B parameter
            for A in C001 ; do
                for B in cpssEnabler mainGaltisWrapper mainPpDrv mainUT ; do
                    copy_baseline_objects "extension\\$A\\$B" $OBJ_LIB_DIR_NAME
                done
            done
            
            for A in C002 ; do 
                for B in cpssEnabler mainGaltisWrapper mainPpDrv mainUT ; do
                    copy_baseline_objects "extension\\$A\\$B" $OBJ_LIB_DIR_NAME
                done
            done
        fi
        
    fi

    win32_marvell_BSP_copy || return 1
    return 0
}
