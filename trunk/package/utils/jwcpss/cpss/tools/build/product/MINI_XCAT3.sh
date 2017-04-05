# mini xcat3 application

export USER_BASE=$CPSS_PATH
export USER_MAKE=presteraTopMake

export MINI_XCAT3=YES
export BUILD_ONLY=miniXcat3

MAKE_TARGET_LINK=miniXcat3.bin
APP_DEMO_ONLY=yes
export LINUX_BUILD_KERNEL=NO

os_post_build()
{
    return 0
}

. $tool_build/product/simulation.inc

