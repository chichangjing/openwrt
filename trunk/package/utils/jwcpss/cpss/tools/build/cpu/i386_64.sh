#
# i386_64 CPU support
#
# File version: 2


. $tool_build/cpu/i386.sh

DEFAULT_TOOLKIT_win32=vc10
OS_DEFS="$OS_DEFS IS_64BIT_OS"
export CPU_DIR=i386_64
export WORDSIZE_REQUIRED="64"
export IS_64BIT_OS=YES

echo "export linux_objects_ext=i386_64"
export linux_objects_ext="i386_64"
#
###############################
# Set CPSS LOG_ENABLE
###############################
if [ -z $API_LOG_ENABLE ]; then
    export LOG_ENABLE="L_ON"
fi

