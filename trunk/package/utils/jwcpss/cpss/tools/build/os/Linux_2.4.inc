# NOT TESTED YET
# required environment:
#   LINUX_DIST_PATH
#   LSP_USER_BASE
#   LINUX_KERNEL_VERSION  (or DEFAULT_LINUX_KERNEL_VERSION from CPU defs)
#   LINUX_LSP_NAME        (or DEFAULT_LINUX_LSP_NAME from CPU defs)
#   LINUX_BUILD_KERNEL    set to YES to build kernel
# 
#

os_pre_build()
{
    if [ "${LINUX_BUILD_KERNEL}" != YES ]
    then
        return 0
    fi

    # build linux kernel
    info_message -e "BEGIN configLinux"

    os_linux_patch_kernel || return 1
    os_linux_kernel_is_already_built && return 0

    #now make it
    info_message  "make ${MAKE_FLAGS} -C ${DIST} oldconfig"
    make ${MAKE_FLAGS} -C ${DIST} oldconfig
    if [ $? -ne 0 ]
    then
        error_message -e "\tmake oldconfig failed"
        return 1
    else
        info_message -e "\tmake oldconfig...done"
    fi

    #Now make dep. This for linux versions < 2.6
    info_message  "make ${MAKE_FLAGS} -C ${DIST} dep"
    make ${MAKE_FLAGS} -C ${DIST} dep
    if [ $? -ne 0 ]
    then
        error_message -e "\tmake dep failed"
        return 1
    else
        info_message -e "\tmake dep...done"
    fi

    os_linux_make_rootfs || return 1

    chmod a+x ${DIST}/scripts/crfs.sh
    #Usage: crfs.sh <ROOTFS_PATH> <TOOL_CHAIN_PATH> <ARCH> <BUSYBOX_PATH>.
    info_message ${DIST}"/scripts/crfs.sh "${DIST}"/rootfs "${COMPILER}" "${ARCH}" "${LINUX_DIST_PATH}
    ${DIST}/scripts/crfs.sh ${DIST}/rootfs ${TOOL_CHAIN_PATH} ${ARCH} ${LINUX_DIST_PATH}
    if [ $? -ne 0 ]
    then
        error_message -e "\tROOTFS CREATION FAILED"
        return 1
    fi

    # save md5 of lsp to make sure compilation successful
    find ${LSP_USER_BASE}/${LINUX_LSP_NAME}/. -type f -print0 | sort -z |
        xargs -0 cat | md5sum | cut -d ' ' -f 1 > $DIST/.linux_config_done

    info_message -e "END   configLinux"
    return 0
}

os_post_build()
{
    # create image

    ##building
    rm -f zimage.initrd.*

    # Create initrd
    rm -f "$LINUX_DIST_PATH"/arch/"$CPU_TOOL"/boot/images/zImage.initrd.*

    chmod a+x ${DIST}/scripts/create_initrd.sh

    info_message -e "${DIST}/scripts/create_initrd.sh "$DIST"/rootfs "$DIST"/arch/"$CPU_TOOL"/boot/images"
    ${DIST}/scripts/create_initrd.sh "$DIST"/rootfs "$DIST"/arch/"$CPU_TOOL"/boot/images

    if [ $? -ne 0 ] # test result
    then
        error_message -e "\tCreate initrd faild"
        return 1
    fi

    #Create the zImage
    make ${MAKE_FLAGS} -C "$DIST" zImage.initrd
    if [ $? -ne 0 ] # test result
    then
        error_message -e "\tCreate the zImage faild"
        return 1
    fi

    info_message "image copy -> cp "$DIST"/arch/"$CPU_TOOL"/boot/images/zImage.initrd.* ${IMAGE_PATH}"
    cp "$DIST"/arch/"$CPU_TOOL"/boot/images/zImage.initrd.* ${IMAGE_PATH}

    return 0
}
