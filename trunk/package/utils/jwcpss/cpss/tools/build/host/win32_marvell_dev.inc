# host definitions Marvell internal environment
# (WIN32)
#
# Environment variables:
#
#   DRIVE   - drive where pss/cpss projects located. Default X:
#
#       Version number - $ 17

. $tool_build/host/win32.sh

#####################################################################################
check_create_dir() {
    win32_path_exists "$1" || cmd /c "mkdir $1"
}

#####################################################################################
copy_baseline_objects() {

   if [ ! -d "$OBJ_NETWORK_ROOT\\$1" ]; then
   
      return
   fi

   if [[ -z "$3" ]]
   then
        check_create_dir "$COMPILATION_ROOT\\$1\\libs\\$2"
        xcopy "$OBJ_NETWORK_ROOT\\$1\\libs\\$2\\*.*" "$COMPILATION_ROOT\\$1\\libs\\$2\\" /E /H /R /Y /Q

   else
        check_create_dir "$COMPILATION_ROOT\\$1\\libs\\$3"
        xcopy "$OBJ_NETWORK_ROOT\\$1\\libs\\$2\\*.*" "$COMPILATION_ROOT\\$1\\libs\\$3\\" /E /H /R /Y /Q

   fi

    if [ "$NO_COPY_OBJECTS" = "TRUE" ]; then
        echo "No copy of objects from server"
        return
    fi

   if [[ -z "$3" ]]
    check_create_dir "$COMPILATION_ROOT\\$1\\objTemp\\$2"
    xcopy "$OBJ_NETWORK_ROOT\\$1\\objTemp\\$2\\*.*" "$COMPILATION_ROOT\\$1\\objTemp\\$2\\"  /E /H /R /Y /Q   if [[ -z "$3" ]]
   then
        check_create_dir "$COMPILATION_ROOT\\$1\\objTemp\\$2"
        xcopy "$OBJ_NETWORK_ROOT\\$1\\objTemp\\$2\\*.*" "$COMPILATION_ROOT\\$1\\objTemp\\$2\\"  /E /H /R /Y /Q
   else
        check_create_dir "$COMPILATION_ROOT\\$1\\objTemp\\$3"
        xcopy "$OBJ_NETWORK_ROOT\\$1\\objTemp\\$2\\*.*" "$COMPILATION_ROOT\\$1\\objTemp\\$3\\"  /E /H /R /Y /Q
   fi

   # #SET RELEASE_DEP=X:/cpss/SW/prestera
   # ECHO "RELEASE_DEP     = $RELEASE_DEP"
   # $CPSS_PATH\\tools\\linuxScripts\\fix_depfiles.py "$COMPILATION_ROOT\\$1\\objTemp\\$2" "$RELEASE_DEP" "$CPSS_PATH"

   chmod 777 $CPSS_PATH\\tools\\linuxScripts\\fix_depfiles.py
   echo "pithon $CPSS_PATH/tools/linuxScripts/fix_depfiles.py $COMPILATION_ROOT\\$1\\objTemp\\$2 $RELEASE_DEP $CPSS_PATH" 
   #c://Python27//python.exe $CPSS_PATH/tools/linuxScripts/fix_depfiles.py $COMPILATION_ROOT\\$1\\objTemp\\$2 $RELEASE_DEP $CPSS_PATH  
   python.exe $CPSS_PATH/tools/linuxScripts/fix_depfiles.py $COMPILATION_ROOT\\$1\\objTemp\\$2 $RELEASE_DEP $CPSS_PATH  
   #$CPSS_PATH/tools/linuxScripts/fix_depfiles.py $COMPILATION_ROOT\\$1\\objTemp\\$2 $RELEASE_DEP $CPSS_PATH  

}

#####################################################################################
copy_baseline_extension_objects() {

    check_create_dir "$COMPILATION_ROOT\\extension\\$1\\$2\\libs\\$3"
    xcopy "$OBJ_EXTENSIONS_NETWORK_ROOT\\extensions\\$1\\$family_subdir\\extension\\$1\\$2\\libs\\$3\\*.*" "$COMPILATION_ROOT\\extension\\$1\\$2\\libs\\$3\\" /E /H /R /Y /Q

    if [ "$NO_COPY_OBJECTS" = "TRUE" ]; then
        echo "No copy of objects from server"
        return
    fi

    check_create_dir "$COMPILATION_ROOT\\extension\\$1\\$2\\objTemp\\$3"
    xcopy "$OBJ_EXTENSIONS_NETWORK_ROOT\\extensions\\$1\\$family_subdir\\extension\\$1\\$2\\objTemp\\$3\\*.*" "$COMPILATION_ROOT\\extension\\$1\\$2\\objTemp\\$3\\"  /E /H /R /Y /Q

    chmod 777 $CPSS_PATH\\tools\\linuxScripts\\fix_depfiles.py
    echo "pithon $CPSS_PATH/tools/linuxScripts/fix_depfiles.py $COMPILATION_ROOT\\extension\\$1\\$2\\objTemp\\$3 $RELEASE_DEP $CPSS_PATH" 
    #c://Python27//python.exe $CPSS_PATH/tools/linuxScripts/fix_depfiles.py $COMPILATION_ROOT\\extension\\$1\\$2\\objTemp\\$3 $RELEASE_DEP $CPSS_PATH  
    python.exe $CPSS_PATH/tools/linuxScripts/fix_depfiles.py $COMPILATION_ROOT\\extension\\$1\\$2\\objTemp\\$3 $RELEASE_DEP $CPSS_PATH  
    #$CPSS_PATH/tools/linuxScripts/fix_depfiles.py $COMPILATION_ROOT\\extension\\$1\\$2\\objTemp\\$3 $RELEASE_DEP $CPSS_PATH  

}


#####################################################################################
win32_marvell_BSP_detect_paths()
{
    makefile_location_type=generic
               
    # BSP/GM locations
    case ${CPU_BOARD}_${TOOLKIT} in
        i386_vc)
            CC_BSP_DIR="$DRIVE\\pss\\SW\\application\\work\\vc"
            GM_DLL_VC_DIR="$USER_BASE\\simulation\\libs\\GM\\vc6"
            GM_DLL_VC_ROOT="$GM_DLL_ROOT\\vc6"
             
            if [ "$FORCE_GM_LION_USE" = "EXISTS" ];then
                BSP_NETWORK_DIR="$OBJ_NETWORK_ROOT\\application\\work\\vc_gm_lion"
                DESTINATION_BSP_DIR="$COMPILATION_ROOT\\application\\work\\vc_gm_lion"
                TMP_OBJ_LIB_DIR_NAME="VC_GM_LION"
            elif [ "$FORCE_GM_LION2_USE" = "EXISTS" ];then
                BSP_NETWORK_DIR="$OBJ_NETWORK_ROOT\\application\\work\\vc_gm_lion2"
                DESTINATION_BSP_DIR="$COMPILATION_ROOT\\application\\work\\vc_gm_lion2"
                TMP_OBJ_LIB_DIR_NAME="VC_GM_LION2"
            elif [ "$FORCE_GM_XCAT2_USE" = "EXISTS" ];then
                BSP_NETWORK_DIR="$OBJ_NETWORK_ROOT\\application\\work\\vc_gm_xcat2"
                DESTINATION_BSP_DIR="$COMPILATION_ROOT\\application\\work\\vc_gm_xcat2"
                TMP_OBJ_LIB_DIR_NAME="VC_GM_XCAT2"
            elif [ "$FORCE_GM_PUMA3_USE" = "EXISTS" ];then
                BSP_NETWORK_DIR="$OBJ_NETWORK_ROOT\\application\\work\\vc_gm_puma3"
                DESTINATION_BSP_DIR="$COMPILATION_ROOT\\application\\work\\vc_gm_puma3"
                TMP_OBJ_LIB_DIR_NAME="VC_GM_PUMA3"
            elif [ "$FORCE_SUB20_USE" = "EXISTS" ];then
                BSP_NETWORK_DIR="$OBJ_NETWORK_ROOT\\application\\work\\vc_sub20"
                DESTINATION_BSP_DIR="$COMPILATION_ROOT\\application\\work\\vc_sub20"
                TMP_OBJ_LIB_DIR_NAME="VC_SUB20"
            else
                TMP_OBJ_LIB_DIR_NAME="VC"
            fi
            ;;
        i386_vc8)
            OPTIONS="$OPTIONS -T vc8"		
            CC_BSP_DIR="$DRIVE\\pss\\SW\\application\\work\\vc8"
            BSP_NETWORK_DIR="$OBJ_NETWORK_ROOT\\application\\work\\vc8"
            DESTINATION_BSP_DIR="$COMPILATION_ROOT\\application\\work\\vc8"
            GM_DLL_VC_DIR="$USER_BASE\\simulation\\libs\\GM\\vc8"
            GM_DLL_VC_ROOT="$GM_DLL_ROOT\\vc8"
            TMP_OBJ_LIB_DIR_NAME="VC8"
            ;;
        i386_vc10)
            OPTIONS="$OPTIONS -T vc10"		
            CC_BSP_DIR="$DRIVE\\pss\\SW\\application\\work\\vc10"
            BSP_NETWORK_DIR="$OBJ_NETWORK_ROOT\\application\\work\\vc10"
            DESTINATION_BSP_DIR="$COMPILATION_ROOT\\application\\work\\vc10"
            GM_DLL_VC_DIR="$USER_BASE\\simulation\\libs\\GM\\vc10"
            GM_DLL_VC_ROOT="$GM_DLL_ROOT\\vc10"
            TMP_OBJ_LIB_DIR_NAME="VC10"


            if [ "$FORCE_GM_PUMA3_USE" = "EXISTS" ];then
                BSP_NETWORK_DIR="$OBJ_NETWORK_ROOT\\application\\work\\vc10_gm_puma3"
                DESTINATION_BSP_DIR="$COMPILATION_ROOT\\application\\work\\vc10_gm_puma3"
                TMP_OBJ_LIB_DIR_NAME="VC10_GM_PUMA3"
            elif [ "$FORCE_GM_BOBCAT2_USE" = "EXISTS" ];then
                BSP_NETWORK_DIR="$OBJ_NETWORK_ROOT\\application\\work\\vc10_gm_bobcat2"
                DESTINATION_BSP_DIR="$COMPILATION_ROOT\\application\\work\\vc10_gm_bobcat2"
                TMP_OBJ_LIB_DIR_NAME="VC10_GM_BOBCAT2"
            elif [ "$FORCE_GM_BOBCAT3_USE" = "EXISTS" ];then
                BSP_NETWORK_DIR="$OBJ_NETWORK_ROOT\\application\\work\\vc10_gm_bobcat3"
                DESTINATION_BSP_DIR="$COMPILATION_ROOT\\application\\work\\vc10_gm_bobcat3"
                TMP_OBJ_LIB_DIR_NAME="VC10_GM_BOBCAT3"
            elif [ "$FORCE_GM_BOBCAT2_B0_USE" = "EXISTS" ];then
                BSP_NETWORK_DIR="$OBJ_NETWORK_ROOT\\application\\work\\vc10_gm_bobcat2_b0"
                DESTINATION_BSP_DIR="$COMPILATION_ROOT\\application\\work\\vc10_gm_bobcat2_b0"
                TMP_OBJ_LIB_DIR_NAME="VC10_GM_BOBCAT2_B0"
            elif [ "$FORCE_GM_BOBK_CAELUM_USE" = "EXISTS" ];then

                BSP_NETWORK_DIR="$OBJ_NETWORK_ROOT\\application\\work\\vc10_gm_bobcat2_b0" ### TBD BSP_NETWORK_DIR="vc10_gm"
                DESTINATION_BSP_DIR="$COMPILATION_ROOT\\application\\work\\vc10_gm_bobk_caelum"

                TMP_OBJ_LIB_DIR_NAME="VC10_GM_BOBCAT2_B0" ### TBD TMP_OBJ_LIB_DIR_NAME="VC10_GM"
                TMP_DESTINATION_OBJ_LIB_DIR_NAME="VC10_GM_BOBK_CAELUM" ### TBD TMP_DESTINATION_OBJ_LIB_DIR_NAME="VC10_GM"
            fi    
            ;;		
        i386_64_vc10)
            OPTIONS="$OPTIONS -T vc10"		
            CC_BSP_DIR="$DRIVE\\pss\\SW\\application\\work\\vc10_64"
            BSP_NETWORK_DIR="$OBJ_NETWORK_ROOT\\application\\work\\vc10_64"
            DESTINATION_BSP_DIR="$COMPILATION_ROOT\\application\\work\\vc10_64"
            GM_DLL_VC_DIR="$USER_BASE\\simulation\\libs\\GM\\vc10_64"
            GM_DLL_VC_ROOT="$GM_DLL_ROOT\\vc10_64"
            TMP_OBJ_LIB_DIR_NAME="VC10_64"
            ;;		
        i386_bc)
            OPTIONS="$OPTIONS -T bc"
            CC_BSP_DIR="$DRIVE\\pss\\SW\\application\\work\\bc"
            BSP_NETWORK_DIR="$OBJ_NETWORK_ROOT\\application\\work\\bc"
            DESTINATION_BSP_DIR="$COMPILATION_ROOT\\application\\work\\bc"
            GM_DLL_BC_DIR=""
            GM_DLL_BC_ROOT=""
            TMP_OBJ_LIB_DIR_NAME="BC"
            ;;

        PPC604_tornado)
            TMP_OBJ_LIB_DIR_NAME="PPC604"
            CC_BSP_DIR="$DRIVE\\tornado\\target\\EV_64260A\\config\\EV-64260A-750cx-PCI"
            UNPACKED_BSP_DIR="$CPSS_PATH\\config\\EV-64260A-750cx-PCI"    
            BSP_NETWORK_DIR="$OBJ_NETWORK_ROOT\\config\\EV-64260A-750cx-PCI"
            DESTINATION_BSP_DIR="$COMPILATION_ROOT\\config\\EV-64260A-750cx-PCI"
            ;;
        PPC603_tornado)
            TMP_OBJ_LIB_DIR_NAME="PPC603"
            CC_BSP_DIR="$DRIVE\\tornado\\target\\mv_pmc8245_bsp\\config\\mv_pmc8245"
            UNPACKED_BSP_DIR="$CPSS_PATH\\config\\mv_pmc8245"    
            BSP_NETWORK_DIR="$OBJ_NETWORK_ROOT\\config\\mv_pmc8245"
            DESTINATION_BSP_DIR="$COMPILATION_ROOT\\config\\mv_pmc8245"
            ;;
        PPC603_wb_diab | PPC603_tornado_diab)
            TMP_OBJ_LIB_DIR_NAME="PPC603"
            CC_BSP_DIR="$DRIVE\\tornado\\target\\mv_pmc8245_bsp\\config\\mv_pmc8245_Tornado2.2.1"
            UNPACKED_BSP_DIR="$CPSS_PATH\\config\\mv_pmc8245_Tornado2.2.1"    
            BSP_NETWORK_DIR="$OBJ_NETWORK_ROOT\\config\\mv_pmc8245_Tornado2.2.1"
            DESTINATION_BSP_DIR="$COMPILATION_ROOT\\config\\mv_pmc8245_Tornado2.2.1"
            ;;
        PPC85XX_workbench26 | PPC85XX_wb_diab | PPC85XX_workbench26_diab )
            TMP_OBJ_LIB_DIR_NAME="PPC85XX"
            CC_BSP_DIR="$DRIVE\\tornado\\target\\gda8548_bsp\\config\\GDA8548_6.5"
            UNPACKED_BSP_DIR="$CPSS_PATH\\config\\GDA8548_6.5"    
            BSP_NETWORK_DIR="$OBJ_NETWORK_ROOT\\config\\GDA8548_6.5"
            DESTINATION_BSP_DIR="$COMPILATION_ROOT\\config\\GDA8548_6.5"
            if [ "$TOOLKIT" = "wb_diab" -o "$TOOLKIT" = "workbench26_diab" ]; then
                TMP_OBJ_LIB_DIR_NAME="PPC85XX_DIAB"
                DESTINATION_BSP_DIR="$COMPILATION_ROOT\\config\\GDA8548_6.5_DIAB"
                BSP_NETWORK_DIR="$OBJ_NETWORK_ROOT\\config\\GDA8548_6.5_DIAB"
            fi
            ;;
        PPC85XX_LION_RD_workbench26 | PPC85XX_LION_RD_wb_diab | PPC85XX_LION_RD_workbench26_diab )
            TMP_OBJ_LIB_DIR_NAME="PPC85XX"
            CC_BSP_DIR="$DRIVE\\tornado\\target\\lion_mpc8544_bsp\\config"
            UNPACKED_BSP_DIR="$CPSS_PATH\\config\\lion_mpc8544_bsp"
            BSP_NETWORK_DIR="$OBJ_NETWORK_ROOT\\config\\lion_mpc8544_bsp"
            DESTINATION_BSP_DIR="$COMPILATION_ROOT\\config\\lion_mpc8544_bsp"
            if [ "$TOOLKIT" = "wb_diab" -o "$TOOLKIT" = "workbench26_diab" ]; then
                TMP_OBJ_LIB_DIR_NAME="mpc8544_DIAB"
                DESTINATION_BSP_DIR="$COMPILATION_ROOT\\config\\lion_mpc8544_DIAB_bsp"
                BSP_NETWORK_DIR="$OBJ_NETWORK_ROOT\\config\\lion_mpc8544_DIAB_bsp"
            fi
            ;;
        ARMADAXP_workbench33 | ARMADAXP_BE_workbench33)
            TMP_OBJ_LIB_DIR_NAME="ARMV7"
            CC_BSP_DIR="$DRIVE\\tornado\\target\\armadaxp_bsp\\config"
            UNPACKED_BSP_DIR="$CPSS_PATH\\config\\armadaxp_bsp"
            BSP_NETWORK_DIR="$OBJ_NETWORK_ROOT\\config\\armadaxp_bsp"
            DESTINATION_BSP_DIR="$COMPILATION_ROOT\\config\\armadaxp_bsp"
            if [ "$CPU_BOARD" = ARMADAXP_BE ]; then
                TMP_OBJ_LIB_DIR_NAME="${TMP_OBJ_LIB_DIR_NAME}_BE"
                BSP_NETWORK_DIR="${BSP_NETWORK_DIR}_BE"
                DESTINATION_BSP_DIR="${DESTINATION_BSP_DIR}_BE"
            fi			
            ;;
        ARMADAXP_workbench33_diab | ARMADAXP_BE_workbench33_diab)
            TMP_OBJ_LIB_DIR_NAME="ARMV7_DIAB"
            CC_BSP_DIR="$DRIVE\\tornado\\target\\armadaxp_bsp\\config"
            UNPACKED_BSP_DIR="$CPSS_PATH\\config\\armadaxp_bsp_DIAB"
            BSP_NETWORK_DIR="$OBJ_NETWORK_ROOT\\config\\armadaxp_bsp_DIAB"
            DESTINATION_BSP_DIR="$COMPILATION_ROOT\\config\\armadaxp_bsp_DIAB"
            if [ "$CPU_BOARD" = ARMADAXP_BE ]; then
                TMP_OBJ_LIB_DIR_NAME="${TMP_OBJ_LIB_DIR_NAME}_BE"
                BSP_NETWORK_DIR="${BSP_NETWORK_DIR}_BE"
                DESTINATION_BSP_DIR="${DESTINATION_BSP_DIR}_BE"
            fi			
            ;;				
        XCAT_workbench33 | XCAT_BE_workbench33)
            TMP_OBJ_LIB_DIR_NAME="ARMV5"
            CC_BSP_DIR="$DRIVE\\tornado\\target\\xcat_bsp\\config"
            UNPACKED_BSP_DIR="$CPSS_PATH\\config\\xcat_bsp"
            BSP_NETWORK_DIR="$OBJ_NETWORK_ROOT\\config\\xcat_bsp"
            DESTINATION_BSP_DIR="$COMPILATION_ROOT\\config\\xcat_bsp"
            if [ "$CPU_BOARD" = XCAT_BE ]; then
                TMP_OBJ_LIB_DIR_NAME="${TMP_OBJ_LIB_DIR_NAME}_BE"
                BSP_NETWORK_DIR="${BSP_NETWORK_DIR}_BE"
                DESTINATION_BSP_DIR="${DESTINATION_BSP_DIR}_BE"
            fi			
            ;;
        XCAT_workbench33_diab | XCAT_BE_workbench33_diab)
            TMP_OBJ_LIB_DIR_NAME="ARMV5_DIAB"
            CC_BSP_DIR="$DRIVE\\tornado\\target\\xcat_bsp\\config"
            UNPACKED_BSP_DIR="$CPSS_PATH\\config\\xcat_bsp_DIAB"
            BSP_NETWORK_DIR="$OBJ_NETWORK_ROOT\\config\\xcat_bsp_DIAB"
            DESTINATION_BSP_DIR="$COMPILATION_ROOT\\config\\xcat_bsp_DIAB"
            if [ "$CPU_BOARD" = XCAT_BE ]; then
                TMP_OBJ_LIB_DIR_NAME="${TMP_OBJ_LIB_DIR_NAME}_BE"
                BSP_NETWORK_DIR="${BSP_NETWORK_DIR}_BE"
                DESTINATION_BSP_DIR="${DESTINATION_BSP_DIR}_BE"
            fi			
            ;;				
        ARM_EV_tornado)
            TMP_OBJ_LIB_DIR_NAME="ARM946ES"
            CC_BSP_DIR="$DRIVE\\tornado\\target\\88e6218_bsp\\config\\88e6218"
            UNPACKED_BSP_DIR="$CPSS_PATH\\config\\88e6218"    
            BSP_NETWORK_DIR="$OBJ_NETWORK_ROOT\\config\\88e6218"
            DESTINATION_BSP_DIR="$COMPILATION_ROOT\\config\\88e6218"
            makefile_location_type=ARM_EV
            ;;
        ARM_RD_tornado)
            TMP_OBJ_LIB_DIR_NAME="ARM946ES"
            CC_BSP_DIR="$DRIVE\\tornado\\target\\rd_dx24x_rdln_bsp\\config\\rd_dx24x"
            BSP_NETWORK_DIR="$OBJ_NETWORK_ROOT\\config\\rd_dx24x"
            DESTINATION_BSP_DIR="$COMPILATION_ROOT\\config\\rd_dx24x"
            makefile_location_type=ARM_RD
            ;;
        PPC603_VB_tornado)
            TMP_OBJ_LIB_DIR_NAME="PPC603"
            CC_BSP_DIR="$DRIVE\\tornado\\target\\rd_ex126_vb48g_bsp\\config\\rd_ex126_vb48g"
            UNPACKED_BSP_DIR="$CPSS_PATH\\config\\rd_ex126_vb48g"    
            BSP_NETWORK_DIR="$OBJ_NETWORK_ROOT\\config\\rd_ex126_vb48g"
            DESTINATION_BSP_DIR="$COMPILATION_ROOT\\config\\rd_ex126_vb48g"
            ;;
        ARM5181_tornado)
            TMP_OBJ_LIB_DIR_NAME="ARM5181"
            CC_BSP_DIR="$DRIVE\\tornado\\target\\db_88f5181_prpmc_bsp\\config\\db_88f5181_prpmc"
            UNPACKED_BSP_DIR="$CPSS_PATH\\config\\db_88f5181_prpmc"    
            BSP_NETWORK_DIR="$OBJ_NETWORK_ROOT\\config\\db_88f5181_prpmc"
            BSP_NETWORK_MAKE_DIR="$OBJ_NETWORK_ROOT\\config\\db_88f5181_prpmc\\swControllers\\VxBsp\\"            
            DESTINATION_BSP_DIR="$COMPILATION_ROOT\\config\\db_88f5181_prpmc"
            makefile_location_type=ARM5181
            ;;
        ARM5281_tornado)
            TMP_OBJ_LIB_DIR_NAME="ARM5281"
            CC_BSP_DIR="$DRIVE\\tornado\\target\\db_88f5281_mng_bsp\\config\\db_88f5281_mng"
            UNPACKED_BSP_DIR="$CPSS_PATH\\config\\db_88f5281_mng"    
            BSP_NETWORK_DIR="$OBJ_NETWORK_ROOT\\config\\db_88f5281_mng"
            BSP_NETWORK_MAKE_DIR="$OBJ_NETWORK_ROOT\\config\\db_88f5281_mng\\swControllers\\VxBsp\\"
            DESTINATION_BSP_DIR="$COMPILATION_ROOT\\config\\db_88f5281_mng"
            makefile_location_type=ARM5281
            ;;
        ARM5281_workbench26 | ARM5281_wb_diab | ARM5281_workbench26_diab)
            TMP_OBJ_LIB_DIR_NAME="ARM5281W"
            CC_BSP_DIR="$DRIVE\\tornado\\target\\db_88f5281_mng_bsp\\config\\db_88f5281_mng_WB"
            UNPACKED_BSP_DIR="$CPSS_PATH\\config\\db_88f5281_mng_WB"    
            BSP_NETWORK_DIR="$OBJ_NETWORK_ROOT\\config\\db_88f5281_mng_WB"
            BSP_NETWORK_MAKE_DIR="$OBJ_NETWORK_ROOT\\config\\db_88f5281_mng_WB\\swControllers\\VxBsp\\"            
            DESTINATION_BSP_DIR="$COMPILATION_ROOT\\config\\db_88f5281_mng_WB"
            makefile_location_type=ARM5281
            if [ "$TOOLKIT" = "wb_diab" -o "$TOOLKIT" = "workbench26_diab" ]; then
                TMP_OBJ_LIB_DIR_NAME="ARM5281W_DIAB"
                DESTINATION_BSP_DIR="$COMPILATION_ROOT\\config\\db_88f5281_mng_WB_DIAB"
                BSP_NETWORK_DIR="$OBJ_NETWORK_ROOT\\config\\db_88f5281_mng_WB_DIAB"
            fi
            ;;
        ARM6281_DB_xxx)
            # TODO: unknown toolkit
            TMP_OBJ_LIB_DIR_NAME="ARM5281"
            CC_BSP_DIR="$DRIVE\\tornado\\target\\db_88f6281_bp_bsp\\config\\db_88f6281_bp"
            BSP_NETWORK_DIR="$OBJ_NETWORK_ROOT\\config\\db_88f6281_bp"
            DESTINATION_BSP_DIR="$COMPILATION_ROOT\\config\\db_88f6281_bp"
            ;;
        XCAT_tornado | XCAT_BE_tornado)
            TMP_OBJ_LIB_DIR_NAME="ARM926EJ"
         ## CC_BSP_DIR="$DRIVE\\tornado\\target\\xcat_bsp\\config\\xcat_bsp"
            CC_BSP_DIR="$DRIVE\\switching_infra\\bsp\\xcat_bsp"
            UNPACKED_BSP_DIR="$CPSS_PATH\\config\\xcat_bsp"    
            BSP_NETWORK_DIR="$OBJ_NETWORK_ROOT\\config\\xcat_bsp"
            DESTINATION_BSP_DIR="$COMPILATION_ROOT\\config\\xcat_bsp"
            if [ "$CPU_BOARD" = XCAT_BE ]; then
                TMP_OBJ_LIB_DIR_NAME="ARM926EJ_BE"
                BSP_NETWORK_DIR="${BSP_NETWORK_DIR}_BE"
                DESTINATION_BSP_DIR="${DESTINATION_BSP_DIR}_BE"
            fi
            ;;
        XCAT_tornado_diab | XCAT_BE_tornado_diab)
            TMP_OBJ_LIB_DIR_NAME="ARM926EJ_DIAB"
            CC_BSP_DIR="$DRIVE\\switching_infra\\bsp\\xcat_bsp"
            UNPACKED_BSP_DIR="$CPSS_PATH\\config\\xcat_bsp_DIAB"    
            BSP_NETWORK_DIR="$OBJ_NETWORK_ROOT\\config\\xcat_bsp_DIAB"
            DESTINATION_BSP_DIR="$COMPILATION_ROOT\\config\\xcat_bsp_DIAB"
            if [ "$CPU_BOARD" = XCAT_BE ]; then
                TMP_OBJ_LIB_DIR_NAME="ARM926EJ_DIAB_BE"
                BSP_NETWORK_DIR="${BSP_NETWORK_DIR}_BE"
                DESTINATION_BSP_DIR="${DESTINATION_BSP_DIR}_BE"
            fi
            ;;              
        ARM78200_tornado | ARM78200_BE_tornado)
            TMP_OBJ_LIB_DIR_NAME="ARM78200"
            CC_BSP_DIR="$DRIVE\\switching_infra\\bsp\\dd_bsp"
            UNPACKED_BSP_DIR="$CPSS_PATH\\config\\dd_bsp"    
            BSP_NETWORK_DIR="$OBJ_NETWORK_ROOT\\config\\dd_bsp"
            DESTINATION_BSP_DIR="$COMPILATION_ROOT\\config\\dd_bsp"
            if [ "$CPU_BOARD" = ARM78200_BE ]; then
                TMP_OBJ_LIB_DIR_NAME="ARM78200_BE"
                BSP_NETWORK_DIR="${BSP_NETWORK_DIR}_BE"
                DESTINATION_BSP_DIR="${DESTINATION_BSP_DIR}_BE"
            fi
            ;;
        ARM78200_tornado_diab | ARM78200_BE_tornado_diab)
            TMP_OBJ_LIB_DIR_NAME="ARM78200_DIAB"
            CC_BSP_DIR="$DRIVE\\switching_infra\\bsp\\dd_bsp"
            UNPACKED_BSP_DIR="$CPSS_PATH\\config\\dd_bsp_DIAB"    
            BSP_NETWORK_DIR="$OBJ_NETWORK_ROOT\\config\\dd_bsp_DIAB"
            DESTINATION_BSP_DIR="$COMPILATION_ROOT\\config\\dd_bsp_DIAB"
            if [ "$CPU_BOARD" = ARM78200_BE ]; then
                TMP_OBJ_LIB_DIR_NAME="ARM78200_DIAB_BE"
                BSP_NETWORK_DIR="${BSP_NETWORK_DIR}_BE"
                DESTINATION_BSP_DIR="${DESTINATION_BSP_DIR}_BE"
            fi
            ;;			
        ARM78200RD_tornado | ARM78200RD_BE_tornado)
            TMP_OBJ_LIB_DIR_NAME="ARM78200RD"
            CC_BSP_DIR="$DRIVE\\switching_infra\\bsp\\dd_bsp"
            UNPACKED_BSP_DIR="$CPSS_PATH\\config\\dd_bsp"     
            BSP_NETWORK_DIR="$OBJ_NETWORK_ROOT\\config\\dd_rd_bsp"
            DESTINATION_BSP_DIR="$COMPILATION_ROOT\\config\\dd_rd_bsp"
            if [ "$CPU_BOARD" = ARM78200RD_BE ]; then
                TMP_OBJ_LIB_DIR_NAME="ARM78200RD_BE"
                BSP_NETWORK_DIR="${BSP_NETWORK_DIR}_BE"
                DESTINATION_BSP_DIR="${DESTINATION_BSP_DIR}_BE"
            fi
            ;;				
        PPC860_DUNE_xxx)
            # TODO: unknown toolkit
            TMP_OBJ_LIB_DIR_NAME="PPC860"
            CC_BSP_DIR="$DRIVE\\tornado\\target\\dune_mpc860_bsp\\config"
            BSP_NETWORK_DIR="$OBJ_NETWORK_ROOT\\config\\dune_mpc860_bsp"
            DESTINATION_BSP_DIR="$COMPILATION_ROOT\\config\\dune_mpc860_bsp"
            ;;
        PPC860_NEGEV_xxx)
            # TODO: unknown toolkit???
            # BSP_NETWORK_DIR at non standard place: pss\sw\dune\bsp\...
            CC_BSP_DIR="$DRIVE\\pss\\SW\\dune\\bsp\\ppc860_negev\\config\\ppc860_negev"
            BSP_NETWORK_DIR="$CC_BSP_DIR"
            DESTINATION_BSP_DIR="$COMPILATION_ROOT\\config\\ppc860_negev"
            TMP_OBJ_LIB_DIR_NAME="PPC860"
            ;;
        *)
            error_message "BSP not declared: CPU_BOARD=$CPU_BOARD TOOLKIT=$TOOLKIT"
            return 1
    esac
    return 0
}

#####################################################################################
win32_marvell_BSP_copy()
{
    # copy_bsp
    special_makefile_location=no
    
    case $makefile_location_type in
            ARM5*81*)
                special_makefile_location=yes
                if win32_path_exists "$DESTINATION_BSP_DIR\\swControllers\\VxBsp\\makefile"
                then
                    already_copied=yes
                fi
                ;;
            ARM_EV | ARM_RD)
                if win32_path_exists "$DESTINATION_BSP_DIR\\BSP\\makefile"
                then
                    already_copied=yes
                fi
                ;;
            *)
                if win32_path_exists "$DESTINATION_BSP_DIR\\makefile"
                then
                    already_copied=yes
                fi
                ;;
    esac
        
    if [ "$already_copied" != "yes" ]; then
        check_create_dir "$DESTINATION_BSP_DIR"
        if [ "$special_makefile_location" != "yes" ]; then
            BSP_NETWORK_MAKE_DIR=$BSP_NETWORK_DIR
        fi    
        if win32_path_exists "$BSP_NETWORK_MAKE_DIR\\makefile" ; then
            info_message "****    Copy BSP from obj place   ****"
            info_message "xcopy $BSP_NETWORK_DIR\\*.*   $DESTINATION_BSP_DIR\\"
            xcopy "$BSP_NETWORK_DIR\\*.*"   "$DESTINATION_BSP_DIR\\"  /E /H /R /Y /Q
        else
            info_message "****    Copy BSP from ClearCase   ****"
            xcopy "$CC_BSP_DIR\\*.*"   "$DESTINATION_BSP_DIR\\"  /E /H /R /Y /Q
        fi
    fi

    return 0
}
