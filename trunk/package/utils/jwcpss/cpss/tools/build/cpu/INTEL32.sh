#
# INTEL_32 CPU support
#
# x86 CPU for BM (hardware PP)
#
# File version: 1

. $tool_build/cpu/i386.sh
export CPU_DIR=INTEL32

export LINUX_BUILD_KERNEL=NO
# stub
export DEFAULT_LINUX_KERNEL_VERSION=2.6.31.8
export DEFAULT_LINUX_CONFIG=none

export linux_objects_ext=INTEL32
#
###############################
# Set CPSS LOG_ENABLE
###############################
if [ -z $API_LOG_ENABLE ]; then
    export LOG_ENABLE="L_ON"
fi

