. $tool_build/host/Linux.sh

echo "CC_RELEASE_STREAM     - $CC_RELEASE_STREAM"
echo "CC_CURRENT_BASE_LINE  - $CC_CURRENT_BASE_LINE"
echo "CC_CURRENT_CHANGE_SET - $CC_CURRENT_CHANGE_SET"

#########################################################
##### START default CPSS paths
#########################################################
    ELDK_PPC_PATH=/swtools/devtools/eldk/ppc/usr/bin
    ELDK_ARM_PATH=/swtools/devtools/eldk/arm/usr/bin
    GNUEABI_ARM_PATH=/swtools/devtools/gnueabi/arm_le/arm-linux-gnueabi/bin
    GNUEABI_ARM_BE_PATH=/swtools/devtools/gnueabi/arm_be/armeb-linux-gnueabi/bin
    GNUEABI_ARMV7_PATH=/swtools/devtools/gnueabi/arm_le/arm-none-linux-gnueabi-versions/armv7-marvell-linux-gnueabi-softfp/bin
    GNUEABI_ARMV7_4_6_4_PATH=/swtools/devtools/gnueabi/arm_le/arm-none-linux-gnueabi-versions/armv7-marvell-linux-gnueabi-softfp_i686/bin:
    PATH=$PATH:$GNUEABI_ARMV7_4_6_4_PATH:$GNUEABI_ARM_PATH:$GNUEABI_ARM_BE_PATH:$ELDK_PPC_PATH:$GNUEABI_ARMV7_PATH
    # TODO: set default paths
    export LSP_USER_BASE=${LSP_USER_BASE:-/home/serg/tmp/Marvell/lsp}
    export LINUX_DIST_PATH=${LINUX_DIST_PATH:-/home/serg/tmp/Marvell/linux}


    # TODO
    export CPSS_PATH=${CPSS_PATH:-/cpss/SW/prestera}
#    export CPSS_PATH=${CPSS_PATH:-/home/serg/tmp/Marvell/cpss}

    # TODO

    if [ $FAMILY = "DX_ALL" ];then 
       family_ext="DX"
       if [ $PRODUCT_TYPE = "CPSS_ENABLER_SHARED" ];then
          family_ext="DX_SHARED" 
       fi
    elif [ $FAMILY = "PM" ];then 
          family_ext="PM"
    elif [ $FAMILY = "EXMXPM" ];then 
          family_ext="PM"
    fi   

    echo "OBJ_NETWORK_ROOT=/swdev/fileril103/Objects/cpss/$CC_RELEASE_STREAM/$CC_CURRENT_BASE_LINE/linux/$family_ext"
    OBJ_NETWORK_ROOT="/swdev/fileril103/Objects/cpss/$CC_RELEASE_STREAM/$CC_CURRENT_BASE_LINE/linux/$family_ext"
#    OBJ_NETWORK_ROOT=/home/serg/tmp/Marvell/wrk_o/objects

    # TODO
#    comp_dir=/home/serg/tmp/Marvell/tmp
#    family_subdir=${FAMILY:-NONE}
#    test "$XBAR_TYPE" = "DUNE" && family_subdir=${family_subdir}_Dune
#    if [ "$COMPILATION_ROOT" = "" -o "x$COMPILATION_ROOT" = "x-" ]; then
#        #TODO
#        COMPILATION_ROOT="$comp_dir/${family_subdir}_${CC_CURRENT_CHANGE_SET}"
#    fi
#########################################################
##### END default CPSS paths
#########################################################

host_pre_build()
{

    test -d $COMPILATION_ROOT || mkdir -p $COMPILATION_ROOT

    if [ -d $COMPILATION_ROOT/common/objTemp/$CPU_DIR ]; then
        info_message "Objects already copied, skip copying from server"
        return 0
    fi

    if [ "$NO_COPY_OBJECTS" = "TRUE" ]; then
        info_message "No copy of objects from server"
        return 0
    fi

    if [ -d $OBJ_NETWORK_ROOT/$linux_objects_ext ]; then
        echo "cp -rf -p $OBJ_NETWORK_ROOT/$linux_objects_ext/*  $COMPILATION_ROOT"
        cp -rf -p $OBJ_NETWORK_ROOT/$linux_objects_ext/*  $COMPILATION_ROOT  || true

        ############################
        # dependency correction
        ############################
        chmod 777 $CPSS_PATH/tools/linuxScripts/fix_depfiles.py
        chmod -R 777 $COMPILATION_ROOT
        echo "$CPSS_PATH/tools/linuxScripts/fix_depfiles.py $COMPILATION_ROOT $RELEASE_DEP $CPSS_PATH" 
        $CPSS_PATH/tools/linuxScripts/fix_depfiles.py $COMPILATION_ROOT $RELEASE_DEP $CPSS_PATH
    fi



}
