#
# required environment:
#   LINUX_DIST_PATH
#   LSP_USER_BASE
#   LINUX_KERNEL_VERSION  (or DEFAULT_LINUX_KERNEL_VERSION from CPU defs)
#   LINUX_LSP_NAME        (or DEFAULT_LINUX_LSP_NAME from CPU defs)
#   LINUX_BUILD_KERNEL    set to YES to build kernel
#
#   UIMAGE_NO_COPY        don't copy uImage to rootfs
#
#   LSP_KERNEL_TYPE       uImage or zImage, default zImage
#   LSP_KERNEL_CONFIG     oldconfig or config file name (3.4+)
#   LSP_KERNEL_DTB_NAME   .DTB file name
#   DONT_CREATE_ROOTFS    Don't build busybox when == "YES" (for LSP devel)
#

#----------------------------------------------------------------
# Function os_linux_patch_kernel
#
# Description:
#       Prepare kernel to build:
#       1. Create directory $LINUX_DIST_PATH/$CPU_BOARD/$LINUX_LSP_NAME
#       2. Extract kernel from tarball (located in $LINUX_DIST_PATH)
#       3. Apply LSP
#
os_linux_patch_kernel()
{
    echo " >> os_linux_patch_kernel "
    # extract and patch kernel if required
    cd $LINUX_DIST_PATH
    PACKAGE=linux-$LINUX_KERNEL_VERSION

    # APPLY LSP_SUBDIR
    if [ "$LSP_SUBDIR" != "" -a -d $LSP_USER_BASE/$LSP_SUBDIR/$LINUX_LSP_NAME ]
    then
        LSP_USER_BASE=$LSP_USER_BASE/$LSP_SUBDIR
    fi

    if [ -f $DIST/.linux_config_done ]
    then
        # check LSP match
        lsp="`find ${LSP_USER_BASE}/${LINUX_LSP_NAME}/. -type f -print0 | sort -z |
            xargs -0 cat | md5sum | cut -d ' ' -f 1`"
        if [ "$lsp" = "`cat $DIST/.linux_config_done`" ]
        then
            rm -f $DIST/rootfs/lib/modules/mv*.ko
            rm -f $DIST/rootfs/usr/bin/appDemo
            rm -f $DIST/rootfs/usr/bin/appImage
            info_message -e "END configLinux  -> was done before"
            return 0
        fi
    fi

    # kernel not yet compiled or lsp doesn't match
    trace_message -e "\trm -rf "$DIST
    rm -rf $DIST/.[^.]* $DIST/*

    os_linux_extract_tarball $LINUX_DIST_PATH/$PACKAGE $LINUX_DIST_PATH/$CPU_BOARD/$LINUX_LSP_NAME
    if [ $? -ne 0 ]; then
        return 1
    fi

    cd $LINUX_DIST_PATH

    #Dist O.K ?
    if [ ! -d $DIST ]; then
      error_message -e "\tDISTRIBUTION NAME ?"
      return 1
    fi

    #Apply the LSP
    #trace_message  "cp -rf --reply=yes ${LSP_USER_BASE}/${LINUX_LSP_NAME}/* ${DIST}"
    #cp -rf --reply=yes ${LSP_USER_BASE}/${LINUX_LSP_NAME}/* ${DIST}
    trace_message  "yes | cp -rfL ${LSP_USER_BASE}/${LINUX_LSP_NAME}/* ${DIST}"
    yes | cp -rfL ${LSP_USER_BASE}/${LINUX_LSP_NAME}/* ${DIST}
    if [ $? -ne 0 ]; then
        error_message -e "\tCopy LSP failed"
        return 1
    else
        info_message -e "\tCopy LSP...done"
    fi

    ## patch kernel
    #if [ -f $tool_build/os/Linux_patch_kernel.sh ]
    #then
    #    sh $tool_build/os/Linux_patch_kernel.sh
    #    if [ -f $DIST/kernel_patched_env.sh ]
    #    then
    #        . $DIST/kernel_patched_env.sh
    #    fi
    #fi

    if [ "$LINUX_CONFIG_NOPRESTERA" = "YES" ]; then
        info_message -e "Disable MV_INCLUDE_PRESTERA"
        echo "CONFIG_MV_INCLUDE_PRESTERA=n" >> ${DIST}/arch/${CPU_TOOL}/configs/${DEFCONFIG}
        echo "CONFIG_MV_INCLUDE_PRESTERA_PCI=n" >> ${DIST}/arch/${CPU_TOOL}/configs/${DEFCONFIG}
        echo "CONFIG_MV_INCLUDE_DRAGONITE_XCAT=n" >> ${DIST}/arch/${CPU_TOOL}/configs/${DEFCONFIG}
    fi
    if [ "$LSP_KERNEL_CONFIG" = "oldconfig" ]; then
        # make oldconfig, copy our configuration
        T cp ${DIST}/arch/${CPU_TOOL}/configs/${DEFCONFIG} ${DIST}/.config
        if [ $? -ne 0 ]; then
            error_message -e "\tCopy old config failed"
            return 1
        else
            info_message -e "\tCopy old config...done"
        fi
    fi

    return 0
}

#----------------------------------------------------------------
# Function os_linux_kernel_is_already_built
#
# Description:
#       Check if kernel alredy built
#
# Returns
#       0 if yes
#       1 if not built
#
os_linux_kernel_is_already_built()
{
    echo " >> os_linux_kernel_is_already_built "
    if [ -f $DIST/lsp_specific_settings.sh ]; then
        . $DIST/lsp_specific_settings.sh
    fi
    if [ -f $DIST/.linux_config_done ]
    then
      info_message -e "\t$DIST/.linux_config_done exists; kernel is already built"
      return 0
    fi
    info_message -e "\t$DIST/.linux_config_done NOT exists; kernel is NOT built"
    return 1
}

#----------------------------------------------------------------
# Function os_pre_build_int
#
# Description:
#       Build kernel and rootfs for target:
#       1. Build uImage/zImage
#       2. Build .dtb file if required
#       3. if zImage build uImage based on .dtb and zImage
#
os_pre_build_int()
{
    if [ "${LINUX_BUILD_KERNEL}" != YES ]
    then
        echo " >>> os_pre_build : LINUX_BUILD_KERNEL - $LINUX_BUILD_KERNEL : no build linux kernel <<<"
        return 0
    fi

    info_message -e "BEGIN buildLinux"

    #os_linux_patch_kernel || return 1

    #os_linux_kernel_is_already_built && return 0

    if [ "$LSP_KERNEL_CONFIG" = "oldconfig" ]; then
        config=oldconfig
    else
        config=${DEFCONFIG}
        rm -f ${DIST}/.config
    fi
    if [ "$LSP_KERNEL_CONFIG" != "oldconfig" ]; then
        #clean
        I make -C ${DIST} mrproper
    fi
    #now make it
    I make ${MAKE_FLAGS} -C ${DIST} $config
    if [ $? -ne 0 ]
    then
        error_message -e "\tmake config $config failed"
        return 1
    else
        info_message -e "\tmake condig $config...done"
    fi
    echo 11111
     mkdir -p ${DIST}/rootfs/{dev,usr/bin,lib/modules}
    # now build kernel
    make ${MAKE_FLAGS} -C ${DIST} $LSP_KERNEL_TYPE $mkopts
    if [ $? -ne 0 ]; then
        error_message -e "\tmake $LSP_KERNEL_TYPE failed 111"
        return 1
    else
        info_message -e "\tmake $LSP_KERNEL_TYPE...done"
    fi
    if [ "$LSP_KERNEL_DTB_NAME" != "none" ]; then
        I make ${MAKE_FLAGS} -C ${DIST} $LSP_KERNEL_DTB_NAME $mkopts
        if [ $? -ne 0 ]; then
            error_message -e "\tmake $LSP_KERNEL_DTB_NAME failed"
            return 1
        fi
    fi
    echo 22222
    os_pre_build_mk_uImage $DIST $DIST

    # save md5 of lsp to make sure compilation successful
    find ${LSP_USER_BASE}/${LINUX_LSP_NAME}/. -type f -print0 | sort -z |
        xargs -0 cat | md5sum | cut -d ' ' -f 1 > $DIST/.linux_config_done

    info_message -e "END   buildLinux"
    return 1
}


#----------------------------------------------------------------
# Function os_post_build_int
#
# Description:
#       Build image to burn on target flash
#
os_post_build_int()
{
    linux_2_6_cr_jffs2_params

    if [ -f  ${UIMAGE_PATH}/ep3041a.dtb ]; then
      T cp ${UIMAGE_PATH}/ep3041a.dtb ${DIST}/rootfs
    fi

    if [ "${UIMAGE_NO_COPY}" = "TRUE" ]
    then
        info_message -e "\tuImage not included in rootfs."
    else
        T cp ${UIMAGE_PATH}/uImage ${DIST}/rootfs
        if [ "$LSP_KERNEL_DTB_NAME" != "none" ]; then
            T cp ${DIST}/arch/arm/boot/dts/$LSP_KERNEL_DTB_NAME ${DIST}/rootfs/
        fi
    fi

    if [ "$DRAGONITE_TYPE" = "A1" ]; then
        dragoniteFwPath=${CPSS_PATH}/cpssEnabler/mainSysConfig/src/appDemo/dragonite/firmware
        if [ -e ${dragoniteFwPath}/drigonite_fw.bin ]
        then
            chmod 777 ${DIST}/rootfs/etc/drigonite_fw.bin
            info_message "Dragonite FW copy to rootfs:"
            I cp ${dragoniteFwPath}/drigonite_fw.bin ${DIST}/rootfs/etc
        else
            info_message -e "\t${dragoniteFwPath}/drigonite_fw.bin not found."
        fi
    fi

    chmod a+x ${DIST}/scripts/${CR_JFFS2_FILE_NAME}

    I ${DIST}/scripts/${CR_JFFS2_FILE_NAME} ${CR_JFFS2_PARAMS}

    if [ $? -ne 0 ] # test result
    then
        error_message -e "\tCreate jffs2_img failed"
        return 1
    fi

    info_message "END buildImage"

    # copy uImage and rootfs to $TARGET_DIR_ROOTFS
    if [ -d "$TARGET_DIR_ROOTFS" ]; then

        if [ -f  ${DIST}/arch/${CPU_TOOL}/boot/uImage ]; then
            I cp ${DIST}/arch/${CPU_TOOL}/boot/uImage $TARGET_DIR_ROOTFS
        fi

        if [ -f  ${DIST}/arch/${CPU_TOOL}/boot/images/uImage ]; then
           I cp ${DIST}/arch/${CPU_TOOL}/boot/images/uImage $TARGET_DIR_ROOTFS
        fi

        if [ -f  ${UIMAGE_PATH}/ep3041a.dtb ]; then
            I cp ${UIMAGE_PATH}/uImage.ep3041a $TARGET_DIR_ROOTFS
            I cp ${UIMAGE_PATH}/ep3041a.dtb $TARGET_DIR_ROOTFS
        fi
        I tar -czf $TARGET_DIR_ROOTFS/rootfs.tgz -C $DIST/rootfs .
    fi


    return 0
}
