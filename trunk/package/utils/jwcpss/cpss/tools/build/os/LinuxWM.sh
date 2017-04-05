#
# Linux White Mode 
#
# File version : 2
#

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

# os definitions
export OS_TARGET=Linux
export OS_RUN=linux
export OS_TARGET_RELEASE=2

#eval LIB_BASE="/swdev/fileril103/Objects/cpss/bin/lib"
#export LIB_BASE="/swdev/fileril103/Objects/cpss/bin/lib"
echo "LIB_BASE for WM=${LIB_BASE}"

OS_DEFS="$OS_DEFS LINUX UNIX"


##############################################
# check the host linux station CPU wordsize
##############################################
linux_host_word_size=`uname -m`

echo "-->> linux_host_word_size - $linux_host_word_size"

if [ ${linux_host_word_size} = ${linux_host_word_size%_64} ];then
   export IS_64BIT_OS="NO"
else
   export IS_64BIT_OS="YES"
fi

echo "-->> IS_64BIT_OS - $IS_64BIT_OS"


