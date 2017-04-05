# 
# i386 CPU support
#
# File version: 2
#

# mandatory params

export CPU=i386
export CPU_TOOL=intel
export CPU_FAMILY=INTEL_CPU
export CPU_DIR=i386
export linux_objects_ext="i386"


#
DEFAULT_MANAGEMENT_BUS=PCI_SMI
DEFAULT_TOOLKIT_Linux=gnu
DEFAULT_TOOLKIT_LinuxWM=gnu
DEFAULT_TOOLKIT_FreeBSDWM=gnu
DEFAULT_TOOLKIT_win32=vc
#

export WORDSIZE_REQUIRED="32"
#
###############################
# Set CPSS LOG_ENABLE
###############################
if [ -z $API_LOG_ENABLE ]; then
    export LOG_ENABLE="L_ON"
fi

