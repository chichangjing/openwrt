#
# untested
#
# required environment:
#
#
#   FSP_SOURCE - path to FSP. In this directory:
#                        Makefile
#                        scripts/
#                        src/
#                        compiler_frontend/
#   KERNEL_TREE    - In this directory:
#                        kernel.bin
#                        tar of rootfs_fbsd_e500
# 
#

os_pre_build()
{
    if [ "$PRODUCT_TYPE" = "CPSS_IPLIB" ]; then
       # fsp is not required for this target
       return 0
    fi
    
    local fsp=yes

    test -f $FSP_SOURCE/scripts/cr_fbsd_image.sh || fsp=no
    if [ $fsp != yes ]; then
        error_message -e "FSP_SOURCE directory is not correct"
        return 1
    fi
    
    test -f $KERNEL_TREE/kernel.bin || fsp=no
    test -f $KERNEL_TREE/rootfs_fbsd_e500.tar.gz || fsp=no
    if [ $fsp != yes ]; then
        error_message -e "KERNEL_TREE directory is not correct"
        return 1
    fi
    
    if [ -d ${COMPILATION_ROOT} ]; then
        rm -rf ${COMPILATION_ROOT}
    fi
    mkdir -p ${COMPILATION_ROOT}
    tar xzf $KERNEL_TREE/rootfs_fbsd_e500.tar.gz -C $COMPILATION_ROOT
    info_message "tar xzf $KERNEL_TREE/rootfs_fbsd_e500.tar.gz -C $COMPILATION_ROOT"
    test -d $COMPILATION_ROOT/rootfs_fbsd_e500 || fsp=no   
    if [ $fsp != yes ]; then
        error_message -e "failed to unpack rootfs_fbsd_e500"
        return 1
    fi

    return 0
}


os_link()
{
    # compile project
    info_message "${MAKE} ${MAKE_FLAGS} -C ${USER_BASE} -f Makefile ${MAKE_TARGET_LINK:-application}"
    ${MAKE} ${MAKE_FLAGS} -C ${USER_BASE} -f Makefile ${MAKE_TARGET_LINK:-application}
    if [ $? -ne 0 ] # test result
    then
        error_message -e "\tfailed to build $USER_BASE"
        return 1
    fi
    return 0
}

os_post_build()
{
    info_message -e "cp $COMPILATION_ROOT/appDemo $DEST_ROOT_PATH/usr/bin"
    cp $COMPILATION_ROOT/appDemo $DEST_ROOT_PATH/usr/bin
    info_message "stripping $DEST_ROOT_PATH/usr/bin"
    ${COMP_PREFIX}strip $DEST_ROOT_PATH/usr/bin/appDemo
    info_message -e "cp $COMPILATION_ROOT/mvPpDrv.ko $DEST_ROOT_PATH/lib/modules"
    cp $COMPILATION_ROOT/mvPpDrv.ko $DEST_ROOT_PATH/lib/modules

    ##Build flash burnable image 
##    chmod a+x ${KERNEL_TREE}/scripts/cr_fbsd_image.sh
    export CRFS_FBSD_NO_INCLUDE="MYROOTFS" ##used by cr_fbsd_image.sh

    if [ -d ${COMPILATION_ROOT}/dev_image ]; then
        rm -rf ${COMPILATION_ROOT}/dev_image
    fi
    mkdir -p ${COMPILATION_ROOT}/dev_image
    ${FSP_SOURCE}/scripts/cr_fbsd_image.sh \
        ${KERNEL_TREE}/kernel.bin \
        ${DEST_ROOT_PATH} \
        ${COMPILATION_ROOT}/dev_image
    # copy image to destination dir
    if [ "x$IMAGE_PATH" != "x-" -a -f ${COMPILATION_ROOT}/dev_image/fbsd.image ]; then
        if [ ! -d $IMAGE_PATH ]; then
            mkdir -p $IMAGE_PATH 
            if [ ! -d $IMAGE_PATH ]; then
                error_message "failed to create directory $IMAGE_PATH"
                return 1
            fi
        fi
        cp ${COMPILATION_ROOT}/dev_image/fbsd.image $IMAGE_PATH
    fi
}

# os definitions
export OS_RUN=FreeBSD
export OS_TARGET=FreeBSD
export OS_TARGET_RELEASE=`uname -r`
export OS_TARGET_RELEASE=${OS_TARGET_RELEASE%%.*}
OS_DEFS="$OS_DEFS FREEBSD UNIX LINUX_NPTL"

export DEST_ROOT_PATH=${COMPILATION_ROOT}/rootfs_fbsd_e500
