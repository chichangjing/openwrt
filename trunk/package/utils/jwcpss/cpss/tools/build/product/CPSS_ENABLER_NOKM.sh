# CPSS enabler without kernel module

. $tool_build/product/CPSS_ENABLER.sh
export LINUX_NOKM="YES"
PROJECT_DEFS="$PROJECT_DEFS LINUX_NOKM"
