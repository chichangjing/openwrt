# host definitions Marvell internal environment
# (WIN32)
#
# $Revision: 6 $

. $tool_build/host/win32_marvell_dev.inc


if [ "$CC_VIEW_TYPE" != "DeveloperView" ]; then
    echo "CC_VIEW_TYPE - $CC_VIEW_TYPE"
    error_message "Wrong view type"
#    exit 1
fi

#########################################################
##### START default CPSS paths
#########################################################
    if [ "$CPSS_PATH" = "" ]; then
        CPSS_PATH="$DRIVE\\cpss\\SW\\prestera"
    fi
    case "${FAMILY:-NONE}" in
        # EX
        EX_ALL | EX_TG | EX_RD)  family_subdir=Ex ;;
        NONE)                    family_subdir=Ex; FAMILY=EX_TG ;;
        # DX
        DX_ALL)                  family_subdir=Dx ;;
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
    OBJ_NETWORK_ROOT="F:\\Objects\\cpss\\$CC_RELEASE_STREAM\\$CC_CURRENT_BASE_LINE\\vxWorks\\$family_subdir"
    OBJ_EXTENSIONS_NETWORK_ROOT="F:\\Objects\\cpss\\$CC_RELEASE_STREAM\\$CC_CURRENT_BASE_LINE\\vxWorks"
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

    check_create_dir "$COMPILATION_ROOT"

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
        DESTINATION_OBJ_LIB_DIR_NAME="$TMP_DESTINATION_OBJ_LIB_DIR_NAME"
        
        if win32_path_exists "$COMPILATION_ROOT\\common\\libs\\$OBJ_LIB_DIR_NAME" ; then
            info_message ""
            info_message "compilation root already hold files from server -- action will not copy files"
            info_message ""
        else
        
            for A in \
                common cpssEnabler mainExtUtils mainFaDrv mainGaltisWrapper \
                mainPpDrv mainTmDrv mainUT mainXbarDrv simulation mainLuaWrapper
            do
                copy_baseline_objects $A $OBJ_LIB_DIR_NAME $DESTINATION_OBJ_LIB_DIR_NAME
            done
            
			# copy VC debug info
			if [ "$TOOL" = "visual" ]
			then
				xcopy "$OBJ_NETWORK_ROOT\\application\\work\\$CPU_DIR\\*.pdb" "$COMPILATION_ROOT\\application\\work\\$CPU_DIR\\" /E /H /R /Y /Q
				xcopy "$OBJ_NETWORK_ROOT\\application\\work\\$CPU_DIR\\*.idb" "$COMPILATION_ROOT\\application\\work\\$CPU_DIR\\" /E /H /R /Y /Q
			fi	
			
            # copy SAND files if needed
            if [ "$XBAR_TYPE" = "DUNE" ]; then
                for A in mainFxDrv ; do
                    copy_baseline_objects $A $OBJ_LIB_DIR_NAME $DESTINATION_OBJ_LIB_DIR_NAME
                done
            fi
            
            # extention library has the customers directories --> the list is %%A parameter
            # the libraries within every customr listed in %%B parameter
          if [ "$INC_EXTENSION_SYMBOL" = "YES" ]; then
                A=C001
                for B in cpssEnabler mainGaltisWrapper mainPpDrv mainUT ; do
                     copy_baseline_extension_objects $A $B $OBJ_LIB_DIR_NAME $DESTINATION_OBJ_LIB_DIR_NAME
                done
            fi
            
          if [ "$INC_EXTENSION_SYMBOL1" = "YES" ]; then
                A=C002
                for B in cpssEnabler mainGaltisWrapper mainPpDrv mainUT ; do
                    copy_baseline_extension_objects $A $B $OBJ_LIB_DIR_NAME $DESTINATION_OBJ_LIB_DIR_NAME
                done
            fi

           if [ "$INC_EXTENSION_SYMBOL_G3" = "YES" ]; then
                A=G003
                for B in cpssEnabler mainGaltisWrapper mainPpDrv mainUT ; do
                     copy_baseline_extension_objects $A $B $OBJ_LIB_DIR_NAME $DESTINATION_OBJ_LIB_DIR_NAME
                done
            fi
        fi
        
    fi

    win32_marvell_BSP_copy || return 1
    return 0
}
