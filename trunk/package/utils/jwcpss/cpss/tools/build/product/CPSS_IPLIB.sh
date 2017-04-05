#

. $tool_build/product/cpss_common.inc
case $FAMILY in
    EXMXPM)
        BUILD_ONLY="mainPpDrv/src/cpss/exMxPm/exMxPmGen/ipLpmEngine"
        ;;
    EX)
        BUILD_ONLY="mainPpDrv/src/cpss/exMx/exMxGen/ipLpmEngine"
        ;;
    *)
        echo "No IP libraries for family $FAMILY"
        exit 0
esac

LINUX_BUILD_KERNEL=NO
DONT_LINK=YES

export BUILD_ONLY
