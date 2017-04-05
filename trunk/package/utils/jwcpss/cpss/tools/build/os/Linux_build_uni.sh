#
# Build without LSP (patched source tree available)
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
#   LSP_KERNEL_UDTB_NAME  uImage name for zImage + .dtb
#   DONT_CREATE_ROOTFS    Don't build busybox when == "YES" (for LSP devel)
#

#----------------------------------------------------------------
# Function os_linux_uni_configure_path
#
# Description:
#       Get path to UNI directory
#
os_linux_uni_configure_path()
{
    #TODO: use $LINUX_SRC_DIST ==  $LINUX_DIST_PATH/uni/$LINUX_LSP_NAME ???
    cd $LINUX_DIST_PATH

    echo "LINUX_CLONE_PATH=${LINUX_CLONE_PATH}"
    echo "CASE_BUILD_IMAGES=${CASE_BUILD_IMAGES}"
    echo " >> UNI_DIR=${UNI_DIR} "

    if [ -z ${LINUX_CLONE_PATH} ]
    then
       if [ \! -z ${UNI_DIR} ]
       then
           LSP_USER_BASE=$LSP_USER_BASE/${UNI_DIR}
       fi
    else
       if [ \! -z ${CASE_BUILD_IMAGES} ]
       then
          if [ \! -z ${UNI_DIR} ]
          then
              LSP_USER_BASE=$LSP_USER_BASE/${UNI_DIR}
          fi
       fi
    fi

    # APPLY LSP_SUBDIR
    echo " >> LSP_USER_BASE=${LSP_USER_BASE} "
    echo " >> LSP_SUBDIR=${LSP_SUBDIR} "

    if [ "$LSP_SUBDIR" != "" -a -d $LSP_USER_BASE/$LSP_SUBDIR/$LINUX_LSP_NAME ]
    then
        LSP_USER_BASE=$LSP_USER_BASE/$LSP_SUBDIR
    fi
    echo " >> LSP_USER_BASE=${LSP_USER_BASE}/${LSP_SUBDIR}"

	UNI_SRC_DIST=${LSP_USER_BASE}/${LINUX_LSP_NAME}
    export LINUX_SRC_DIST=${LSP_USER_BASE}/${LINUX_LSP_NAME}
}

#----------------------------------------------------------------
# Function os_linux_prepare_uni
#
# Description:
#       Copy kernel to build directory
#
os_linux_prepare_uni()
{
    echo " >> os_linux_prepare_uni "
	os_linux_uni_configure_path
    export LINUX_SRC_DIST=${UNI_SRC_DIST}
    DIST=$LINUX_DIST_PATH/uni/$LINUX_LSP_NAME/${DEFCONFIG}
    DIST_HOME=${DIST}

    # check if rebuild required
    config_file_uni=${LINUX_SRC_DIST}/arch/${CPU_TOOL}/configs/${DEFCONFIG}
    config_md5_uni="`cat ${config_file_uni} | md5sum | cut -d ' ' -f 1`"

    if [ -f $DIST/config_file ]; then
        config_md5="`cat ${DIST}/config_file | md5sum | cut -d ' ' -f 1`"
        if [ "${config_md5}" = "${config_md5_uni}" ]; then
            info_message -e "END os_linux_prepare_uni -> config unchanged"
            return 0
        fi
    fi
    rm -rf $DIST
    mkdir -p $DIST ||
    {
        error_message "can't mkdir $DIST"
        return 1
    }

    cp ${config_file_uni} ${DIST}/config_file

    return 0
}

#----------------------------------------------------------------
# Function os_linux_copy_kernel
#
# Description:
#       Copy kernel to build directory
#
os_linux_copy_kernel()
{

    echo " >> os_linux_copy_kernel "
	os_linux_uni_configure_path

    if [ \! -d $CPU_BOARD/$LINUX_LSP_NAME ]
    then
        mkdir -p $CPU_BOARD/$LINUX_LSP_NAME ||
        {
            error_message "can't mkdir $CPU_BOARD/$LINUX_LSP_NAME"
            return 1
        }
    fi

    if [ \! -d $DIST ]
    then
        mkdir -p $DIST ||
        {
            error_message "can't mkdir $DIST"
            return 1
        }
    fi

    # kernel not yet compiled or lsp doesn't match
    trace_message -e "\trm -rf "$DIST
    rm -rf $DIST/.[^.]* $DIST/*

    cd $CPU_BOARD/$LINUX_LSP_NAME
    if [ ! -d $LINUX_DIST_PATH ]
    then
      mkdir $LINUX_DIST_PATH
    fi
    echo " >> LINUX_DIST_PATH=${LINUX_DIST_PATH} "

    cd $LINUX_DIST_PATH

    #Dist O.K ?
    if [ ! -d $DIST ]
    then
      error_message -e "\tDISTRIBUTION NAME ?"
      return 1
    fi

    #Apply the LSP
    #trace_message  "cp -rf --reply=yes ${LSP_USER_BASE}/${LINUX_LSP_NAME}/* ${DIST}"
    #cp -rf --reply=yes ${LSP_USER_BASE}/${LINUX_LSP_NAME}/* ${DIST}

    trace_message  "yes | cp -rpfL ${LSP_USER_BASE}/${LINUX_LSP_NAME}/* ${DIST}"

    echo " >> yes | cp -rpfL ${LSP_USER_BASE}/${LINUX_LSP_NAME}/* ${DIST} "
    yes | cp -rpfL ${LSP_USER_BASE}/${LINUX_LSP_NAME}/* ${DIST}

    if [ $? -ne 0 ]
    then
        error_message -e "\tCopy kernel failed"
        return 1
    else
        info_message -e "\tCopy kernel...done"
    fi

    LINUX_SRC_DIST=$DIST
    return 0
}


#----------------------------------------------------------------
# Function os_pre_build_impl
#
# Description:
#       Build kernel and rootfs for target:
#       1. Build zImage
#       2. Build .dtb file if required
#       3. if zImage build uImage based on .dtb and zImage
#
os_pre_build_int()
{
    # build linux kernel
    #os_linux_copy_kernel || return 1
    #OBJS=
    # Build objects in serarate directory (common for MSYS_3_10 and AMC_385)
    # uni/ makefiles need to be fixed first
    os_linux_prepare_uni || return 1
    OBJS="O=${DIST}"

    #os_linux_kernel_is_already_built && return 0

    if [ \! -f $DIST/config_done ]; then
        # configure kernel
        I make ${MAKE_FLAGS} ${OBJS} -C ${LINUX_SRC_DIST} ${DEFCONFIG}
        if [ $? -ne 0 ]
        then
            error_message -e "\tmake config ${DEFCONFIG} failed"
            return 1
        else
            info_message -e "\tmake condig $config...done"
        fi

        if [ "x$DONT_CREATE_ROOTFS" != "xYES" ]; then
            # create rootfs
            os_linux_make_rootfs || return 1
            I bash ${LINUX_SRC_DIST}/scripts/${LINUX_CRFS_SCRIPT_NAME} \
                        ${DIST}/rootfs ${TOOLKIT_PATH} \
                        ${LINUX_CRFS_ARCH_ARG} ${LINUX_DIST_PATH}
            if [ $? -ne 0 ]
            then
                error_message -e "\tROOTFS CREATION FAILED"
                return 1
            fi

            os_pre_build_patch_init ${DIST}/rootfs
        else
            # Skip busybox
            mkdir -p ${DIST}/rootfs/{dev,usr/bin,lib/modules}
        fi
        touch $DIST/config_done
    fi


    # now build kernel
    I make ${MAKE_FLAGS} ${OBJS} -C ${LINUX_SRC_DIST} $LSP_KERNEL_TYPE $mkopts
    if [ $? -ne 0 ]; then
        error_message -e "\tmake $LSP_KERNEL_TYPE failed"
        return 1
    else
        info_message -e "\tmake $LSP_KERNEL_TYPE...done"
    fi
    for dtb in ${LSP_KERNEL_DTB_NAME[*]}; do
        I make ${MAKE_FLAGS} ${OBJS} -C ${LINUX_SRC_DIST} $dtb $mkopts
        if [ $? -ne 0 ]; then
            error_message -e "\tmake $dtb failed"
            return 1
        fi
    done

    os_pre_build_mk_uImage $LINUX_SRC_DIST $DIST

    #TODO mark compilation is done

    info_message -e "END   buildLinux"
    return 0
}

#----------------------------------------------------------------
# Function os_post_build_int
#
# Description:
#       Build image to burn on target flash
#
os_post_build_int()
{
    # create image
	for index in ${!LSP_KERNEL_UDTB_NAME[*]}; do
		udtb=${LSP_KERNEL_UDTB_NAME[$index]}
		dtb=${LSP_KERNEL_DTB_NAME[$index]}
		
		linux_2_6_cr_jffs2_params $dtb

		target_uImage=uImage_$udtb

		if [ "${UIMAGE_NO_COPY}" = "TRUE" ]
		then
			info_message -e "\tuImage not included in rootfs."
		else
			T cp ${UIMAGE_PATH}/$target_uImage ${DIST}/rootfs
			T cp ${DIST}/arch/arm/boot/dts/$dtb ${DIST}/rootfs/
		fi

                phy88x3240FwPath=${CPSS_PATH}/cpssEnabler/mainSysConfig/src/appDemo/phy/88x3240
                if [ -e ${phy88x3240FwPath}/88X32xx-FW.hdr ]
                then
                    if [ -e ${DIST}/rootfs/88X32xx-FW.hdr ]
                    then
                        I chmod 777 ${DIST}/rootfs/88X32xx-FW.hdr
                    fi
                    T cp ${phy88x3240FwPath}/88X32xx-FW.hdr ${DIST}/rootfs/
                    I chmod 777 ${DIST}/rootfs/88X32xx-FW.hdr                    
                else
                    info_message -e "\t${phy88x3240FwPath}/88X32xx-FW.hdr not found."
                fi

		I bash ${LINUX_SRC_DIST}/scripts/${CR_JFFS2_FILE_NAME} ${CR_JFFS2_PARAMS}
		if [ $? -ne 0 ] # test result
		then
			error_message -e "\tCreate jffs2_img failed"
			return 1
		fi

		for img in jffs2_arm.image ubifs_arm_256eb_nand_v2_5.image ubifs_arm_512eb_nand.image
		do
			if [ -f $IMAGE_PATH/$img ]; then
				mv $IMAGE_PATH/$img $IMAGE_PATH/${udtb}_$img
			fi
		done

		info_message "END buildImage"
		# TODO: rename images

		# copy uImage and rootfs to $TARGET_DIR_ROOTFS
		if [ -d "$TARGET_DIR_ROOTFS" ]; then
			if [ "$index" = "0" ]; then
				I cp ${DIST}/arch/${CPU_TOOL}/boot/zImage $TARGET_DIR_ROOTFS
				I tar -czf $TARGET_DIR_ROOTFS/rootfs.tgz -C $DIST/rootfs .
			fi
			I cp ${DIST}/arch/${CPU_TOOL}/boot/dts/$dtb $TARGET_DIR_ROOTFS
			I cp ${DIST}/arch/${CPU_TOOL}/boot/uImage_${udtb} $TARGET_DIR_ROOTFS
                        I cp ${phy88x3240FwPath}/88X32xx-FW.hdr $TARGET_DIR_ROOTFS
                        chmod 777 ${TARGET_DIR_ROOTFS}/88X32xx-FW.hdr                                            
		fi
	done

    return 0
}
