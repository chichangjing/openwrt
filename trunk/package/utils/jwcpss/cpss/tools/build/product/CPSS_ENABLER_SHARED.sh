#

. $tool_build/product/cpss_common.inc
export LINUX_CPSS_TARGET=CPSS_ENABLER
export MAKE_TARGET_LINK=all
export SHARED_MEMORY=1
export CPSS_SPLIT_LIBRARIES=1
export INCLUDE_SHMT=1
PROJECT_DEFS="$PROJECT_DEFS SHARED_MEMORY"

if [ -f $CPSS_PATH/luaCLI_standalone/gtTopMake ]; then
    export LUACLI_STANDALONE=yes
fi
