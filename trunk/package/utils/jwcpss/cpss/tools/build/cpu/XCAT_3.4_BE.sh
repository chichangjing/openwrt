#
# Target CPU settings

. $tool_build/cpu/XCAT_3.4.sh

DEFAULT_TOOLKIT_Linux=mv5sft_be
CPU_PRE_TOOLKIT_mv5sft_be=cpu_pre_toolkit_mv5sft_be
export ENDIAN=BE

#
DEFAULT_LINUX_CONFIG=kirkwood_xcat_be_defconfig
#
#
###############################
# Set CPSS LOG_ENABLE
###############################
if [ -z $API_LOG_ENABLE ]; then
    export LOG_ENABLE="L_ON"
fi
# per toolkit settings
# executed _after_ toolkit choosen,
# overrides defaults
cpu_pre_toolkit_mv5sft_be()
{
    LINUX_CRFS_SCRIPT_NAME=crfs_arm_mv5sft.sh
}
