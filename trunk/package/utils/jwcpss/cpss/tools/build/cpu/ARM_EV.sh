# ??
# Target CPU settings

ARCH=arm
export CPU_TOOL=arm
export CPU_FAMILY=CPU_ARM
export CPU=ARMARCH5
export CPU_DIR=ARM926ES
export ARM_CPU=946ES
export ARM_MMU=946E
export ARM_CACHE=946E
#
DEFAULT_MANAGEMENT_BUS=SMI
DEFAULT_TOOLKIT_vxWorks=tornado
CPU_PRE_TOOLKIT_tornado=cpu_pre_toolkit_tornado
#
#
# per toolkit settings
# executed _after_ toolkit choosen,
# overrides defaults
cpu_pre_toolkit_tornado()
{
    ##
    ## WIND_BASE can be set by user
    ##
    if [ -z $WIND_BASE ]
    then
       export WIND_BASE='c:\TornadoARM'
       echo "WIND_BASE - $WIND_BASE"
    else   
       ### check if exists WIND_BASE defined by user 
       if [ -d $WIND_BASE ]
       then
          echo "WIND_BASE - $WIND_BASE"
       else
          echo "Wrong WIND_BASE - $WIND_BASE"
          exit 0
       fi
    fi

    export PROJ_BSP_DIR="$BSP_CONFIG_DIR\\88e6218\\BSP"
    export CC_ARCH=-march=armv5
    export CC_ARCH_DIAB=-tARMV5LN:vxworks55
}
