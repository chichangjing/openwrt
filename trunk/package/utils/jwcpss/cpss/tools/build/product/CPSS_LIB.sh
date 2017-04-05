#

. $tool_build/product/cpss_common.inc
export MAKE_TARGET_LINK=cpss_lib

export CPSS_LIB_ONLY=YES
export LINUX_CPSS_TARGET=NO_CPSS_ENABLER

###DONT_LINK=YES
export LINUX_BUILD_KERNEL=NO

# Disable kernel-side PP driver
# This driver maps PP resources to kernel address space
# and perform R/W access with address completion
PROJECT_DEFS="$PROJECT_DEFS NO_KERN_PP_DRV"
export NO_KERN_PP_DRV=YES
