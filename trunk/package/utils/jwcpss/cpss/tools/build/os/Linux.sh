#
# required environment:
#   LINUX_DIST_PATH
#   LSP_USER_BASE
#   LSP_SUBDIR            try $LSP_USER_BASE/$LSP_SUBDIR/$LINUX_LSP_NAME first
#                         Otherwise use $LSP_USER_BASE/$LINUX_LSP_NAME
#   LINUX_KERNEL_VERSION  (or DEFAULT_LINUX_KERNEL_VERSION from CPU defs)
#   LINUX_LSP_NAME        (or DEFAULT_LINUX_LSP_NAME from CPU defs)
#   LINUX_BUILD_KERNEL    set to YES to build kernel. Default is YES
#
#   UIMAGE_NO_COPY        don't copy uImage to rootfs
#
#   LSP_KERNEL_TYPE       uImage or zImage, default zImage
#   LSP_KERNEL_CONFIG     oldconfig or config file name (3.4+)
#   LSP_KERNEL_DTB_NAME   .DTB file name
#   LSP_KERNEL_UDTB_NAME  uImage name for zImage + .dtb
#

#----------------------------------------------------------------
if [ "${APP_DEMO_ONLY}" = "YES" -o  "${CPSS_LIB}" = "YES" ] ; then
   LINUX_BUILD_KERNEL=NO
fi
LINUX_BUILD_KERNEL=${LINUX_BUILD_KERNEL:-YES}

LSP_KERNEL_TYPE=${LSP_KERNEL_TYPE:-uImage}
LSP_KERNEL_CONFIG=${LSP_KERNEL_CONFIG:-oldconfig}
LSP_KERNEL_DTB_NAME=${LSP_KERNEL_DTB_NAME:-none}
LSP_KERNEL_UDTB_NAME=${LSP_KERNEL_UDTB_NAME:-none}

#----------------------------------------------------------------
# Function os_linux_extract_tarball
#
# Description:
#       Extract tarball file to target directory
#       Create target directory if required
#
# Inputs:
#   $1  tarball name (without extension)
#   $2  target directory
#
os_linux_extract_tarball()
{
    if [ -f $1.tar.bz2 ]; then
         EXT=.tar.bz2
         COMPRESSION=j
    elif [ -f $1.tar.xz ]; then
        EXT=.tar.xz
        COMPRESSION=J
    elif [ -f $1.tar.gz ]; then
        EXT=.tar.gz
        COMPRESSION=z
    else
       error_message -e "\tLinux kernel zip not found"
       error_message -e "Please put linux kernel zip file version `basename $1` to directory `dirname $1`"
       return 1
    fi
    if [ \! -d $2 ]; then
        mkdir -p $2
        if [ $? -ne 0 ]; then
            error_message "can't mkdir $1"
            return 1
        fi
    fi

    T tar -C $2 -x${COMPRESSION}f $1${EXT}
    if [ $? -ne 0 ]; then
      error_message -e "\tTAR failed"
      return 1
    else
      info_message -e "\ttar ...done"
    fi
    return 0
}

#----------------------------------------------------------------
# Function os_linux_make_rootfs
#
# Description:
#       Create rootfs for target
#
os_linux_make_rootfs()
{
    # Validate that Busybox tarball exists
    if [ ! -f busybox-1.01.tar.bz2 -a ! -f busybox-1.01.tar.gz -a ! -f busybox-1.2.1.tar.bz2 -a ! -f busybox-1.2.1.tar.gz ]
    then
        error_message -e "\tBusybox tarball missing"
        return 1
    else
        trace_message -e "\tBusybox tarball found"
    fi
 
    T mkdir ${DIST}/rootfs
    if [ $? -ne 0 ]
    then
        error_message -e "\tfailed make rootfs directory"
        return 1
    else
        info_message -e "\tmake rootfs...done"
    fi
    return 0
}

#----------------------------------------------------------------
# Function os_pre_build_patch_init
#
# Description:
#       patch init.sh to create environment variable with LSP revision
#
os_pre_build_patch_init()
{
    root=$1
    # patch init.sh to create environment variable with LSP revision
    grep -q '#LSP_VERS' $root/etc/init.sh && return

    sed -i '/test -e \/usr\/bin\/appDemo/ i \
#LSP_VERS \
dmesg -s 32768 |grep -i LSP > /dev/shm/LSP_VERS.TXT \
' $root/etc/init.sh

    sed -i '/mount.*\/dev\/shm/ i \
 test -d /dev/shm || mkdir /dev/shm' $root/etc/init.sh
}

#----------------------------------------------------------------
# Function os_pre_build_mk_uImage
#
# Description:
#       make uImage from zImage
#
os_pre_build_mk_uImage()
{
    srcs="$1"
    objs="$2"
    if [        "$LSP_KERNEL_TYPE" = "zImage" \
                -a "$LSP_KERNEL_DTB_NAME" != "none" \
                -a -f ${srcs}/scripts/mkuboot.sh \
       ]; then
        echo "=============================================================================="
        echo "---------- making uImage from zImage -----------------------------------------"

        for udtb in ${LSP_KERNEL_UDTB_NAME[*]}; do
            tmpfile=$objs/zImage_$udtb
            target_uImage=uImage_$udtb
            rm -f $tmpfile
            cat ${objs}/arch/arm/boot/zImage ${objs}/arch/arm/boot/dts/$LSP_KERNEL_DTB_NAME > $tmpfile
            /bin/bash ${srcs}/scripts/mkuboot.sh \
                    -A arm -O linux -T kernel -C none -a 0x00008000 -e 0x00008000 \
                    -n "Linux-$udtb" \
                    -d $tmpfile ${objs}/arch/arm/boot/$target_uImage
        done

        echo
        echo "=============================================================================="
    fi
}

#----------------------------------------------------------------
os_link()
{
    # compile project
    I ${MAKE} ${MAKE_FLAGS} -C ${USER_BASE} -f Makefile ${MAKE_TARGET_LINK:-application}
    if [ $? -ne 0 ] # test result
    then
        error_message -e "\tfailed to build $USER_BASE"
        return 1
    fi
    return 0
}
#----------------------------------------------------------------

# os definitions
export OS_RUN=linux
export OS_TARGET=Linux
export OS_TARGET_RELEASE=2
LINUX_KERNEL_VERSION=${LINUX_KERNEL_VERSION:-$DEFAULT_LINUX_KERNEL_VERSION}
LINUX_LSP_NAME=${LINUX_LSP_NAME:-$DEFAULT_LINUX_LSP_NAME}
info_message "\t DIST ${LINUX_DIST_PATH} ${CPU_BOARD} ${LINUX_LSP_NAME} ${LINUX_KERNEL_VERSION}"
export DIST=$LINUX_DIST_PATH/$CPU_BOARD/$LINUX_LSP_NAME/linux-${LINUX_KERNEL_VERSION}
export LINUX_SRC_DIST=${DIST}
export DIST_HOME=${DIST}
export LINUX_BASE=${DIST}


case $LINUX_KERNEL_VERSION in
    2.4.*)
        KPATCHLEVEL=4
        ;;
    *)
        KPATCHLEVEL=6
        ;;
esac
export KPATCHLEVEL

if [ -z "$DEFAULT_LINUX_CONFIG" ]
then
    error_message "DEFAULT_LINUX_CONFIG not defined in $tool_build/cpu/$CPU_BOARD.sh"
    exit 1
fi
DEFCONFIG=${DEFAULT_LINUX_CONFIG}
OS_DEFS="$OS_DEFS LINUX UNIX"

#. $tool_build/os/Linux_${KNUMBER}.inc
if [ $KPATCHLEVEL = 4 ]; then
    . $tool_build/os/Linux_2.4.inc
else
if [ "$UNI_DIR" != "" ]; then
    LINUX_SRC_DIST=$LINUX_DIST_PATH/uni/$LINUX_LSP_NAME
    . $tool_build/os/Linux_build_uni.sh
else
    echo 1
    . $tool_build/os/Linux_build_lsp.sh
fi

#----------------------------------------------------------------
# Function os_pre_build
#
# Description:
#       Build kernel and rootfs for target:
#       1. Build uImage/zImage
#       2. Build .dtb file if required
#       3. if zImage build uImage based on .dtb and zImage
#
os_pre_build()
{
    if [ "${LINUX_BUILD_KERNEL}" != YES ]
    then
        echo " >>> os_pre_build : LINUX_BUILD_KERNEL - $LINUX_BUILD_KERNEL : no build linux kernel <<<"
        return 0
    fi

    info_message -e "BEGIN buildLinux"

    mkopts=""
    if [ "${FAST_KERNEL_COMP}" = "TRUE" ] ; then
        mkopts="-j 20"
    fi

    export KCPPFLAGS="-DSHARED_MEMORY"
    os_pre_build_int
    return $?
}

#----------------------------------------------------------------
# Function os_post_build
#
# Description:
#       Build image to burn on target flash
#
os_post_build()
{
    if [ "${LINUX_BUILD_KERNEL}" != YES ]
    then
       echo " >>> os_post_build : LINUX_BUILD_KERNEL - $LINUX_BUILD_KERNEL : no build linux kernel <<<"
       if [ "${CPSS_LIB_ONLY}" = "YES" ]
       then
            echo " >>> os_post_build : CPSS_LIB_ONLY - $CPSS_LIB_ONLY : copy cpss lib to $IMAGE_PATH <<<"
            echo "cp ${COMPILATION_ROOT}/cpssDrvLib.a  ${IMAGE_PATH} "
            #cp ${IMAGE_PATH} ${COMPILATION_ROOT}/cpssDrvLib.a
            cp ${COMPILATION_ROOT}/cpssDrvLib.a  ${IMAGE_PATH}
       fi
       return 0
    fi

    if [ "${APP_DEMO_ONLY}" = "YES" ]
    then
        info_message "AppDemo created"
        return 0
    fi

    info_message "BEGIN buildImage"

    os_post_build_int

    return $?
}

fi
