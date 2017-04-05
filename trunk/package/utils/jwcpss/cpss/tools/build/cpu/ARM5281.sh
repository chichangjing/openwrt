# Orion2.sh
# or db_88f5281_mng ?
# Target CPU settings

. $tool_build/cpu/ARM5181.sh

DEFAULT_MANAGEMENT_BUS=PCI_SMI_I2C
export CPU_DIR=ARM5281

CPU_PRE_TOOLKIT_workbench26=cpu_pre_toolkit_workbench26
CPU_PRE_TOOLKIT_workbench26_diab=cpu_pre_toolkit_workbench26


if [ $TARGET_OS = Linux ]
then
    DEFAULT_LINUX_CONFIG=db88f5281_mng_defconfig
fi
#
# per toolkit settings
# executed _after_ toolkit choosen,
# overrides defaults
cpu_pre_toolkit_tornado()
{
    export ARM_CPU=5281
    export ARM_MMU=5281
    export ARM_CACHE=5281

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

    export PROJ_BSP_DIR="$BSP_CONFIG_DIR"'\db_88f5281_mng\swControllers\VxBsp'
    export CC_ARCH=-march=armv5
    export CC_ARCH_DIAB=-tARMV5LN:vxworks55
}

cpu_pre_toolkit_workbench26()
{
    export ARM_CPU=5281W
    export ARM_MMU=5281W
    export ARM_CACHE=5281W

    if [ "$TOOLKIT" = "wb_diab" -o "$TOOLKIT" = "workbench26_diab" ]; then
       export PROJ_BSP_DIR="$BSP_CONFIG_DIR"'\db_88f5281_mng_WB_DIAB\swControllers\VxBsp'
       export CPU_DIR=ARM5281W_DIAB
    else
       export PROJ_BSP_DIR="$BSP_CONFIG_DIR"'\db_88f5281_mng_WB\swControllers\VxBsp'
       export CPU_DIR=ARM5181W       
    fi
    

    export CC_ARCH=-march=armv5
    export CC_ARCH_DIAB=-tARMV5LN:vxworks55

    info_message " -->cpu_pre_toolkit_workbench26() >> PROJ_BSP_DIR - $PROJ_BSP_DIR"

}
