#

. $tool_build/product/POC.sh
cpss_lib="mainDrv/src/prestera/coreCpss/cpss"
maindrv=mainDrv/src/prestera
classifier="$maindrv/coreApi/classifier"
case $FAMILY in
    EXMXPM)
        BUILD_ONLY="$cpss_lib/exMxPm/exMxPmGen/ipLpmEngine $mainDrv/coreExMx/ipv4 $classifier"
        ;;
    EX)
        BUILD_ONLY="$cpss_lib/exMx/exMxGen/ipLpmEngine $classifier"
        ;;
    *)
        echo "No IP libraries for family $FAMILY"
        exit 1
esac

export BUILD_IPLIB=YES
LINUX_BUILD_KERNEL=NO
DONT_LINK=YES

export BUILD_ONLY
