#
# FreeBSD White Mode 
#

os_link()
{
    # compile project
    info_message "${MAKE} ${MAKE_FLAGS} -C ${USER_BASE} -f Makefile ${MAKE_TARGET_LINK:-application}"
    ${MAKE} ${MAKE_FLAGS} -C ${USER_BASE} -f Makefile ${MAKE_TARGET_LINK:-application}
    if [ $? -ne 0 ] # test result
    then
        error_message -e "\tfailed to build $CPSS_PATH"
        return 1
    fi
    return 0
}

# os definitions
export OS_TARGET=FreeBSD
export OS_RUN=FreeBSD
export OS_TARGET_RELEASE=`uname -r`
export OS_TARGET_RELEASE=${OS_TARGET_RELEASE%%.*}
OS_DEFS="$OS_DEFS FREEBSD UNIX LINUX_NPTL"
