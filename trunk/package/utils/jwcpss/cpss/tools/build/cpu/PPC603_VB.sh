# rd_ex126_vb48g.sh
# Target CPU settings

ARCH=82xx
export CPU_TOOL=ppc
export CPU_FAMILY=PPC_CPU
export CPU=PPC603
export CPU_DIR=PPC603
export ENDIAN=BE
#
DEFAULT_MANAGEMENT_BUS=PCI_I2C
DEFAULT_TOOLKIT_vxWorks=tornado
CPU_PRE_TOOLKIT_tornado=cpu_pre_toolkit_tornado
CPU_PRE_TOOLKIT_tornado_diab=cpu_pre_toolkit_tornado_diab
CPU_FLAGS="-mstrict-align"
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
       export WIND_BASE='c:\Tornado'
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

    
    export PROJ_BSP_DIR="$BSP_CONFIG_DIR"'\rd_ex126_vb48g'
    export CC_ARCH=-mcpu=603
    export CC_ARCH_DIAB=-tPPC603FH:vxworks55
}
cpu_pre_toolkit_tornado_diab()
{
    ##
    ## WIND_BASE can be set by user
    ##
    if [ -z $WIND_BASE ]
    then
       export WIND_BASE='c:\Tornado_2_2'
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
    
    export PROJ_BSP_DIR="$BSP_CONFIG_DIR\\rd_ex126_vb48g_Tornado2.2.1"
    export CC_ARCH=-mcpu=603
    export CC_ARCH_DIAB=-tPPC603FH:vxworks55
}
