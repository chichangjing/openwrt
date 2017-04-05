#

. $tool_build/product/PSS.sh
maindrv=mainDrv/src/prestera
classifier="$maindrv/coreApi/classifier"
case $FAMILY in
    EXMXPM)
        BUILD_ONLY="$mainDrv/coreExMx/ipv4"
        ;;
    EX)
        BUILD_ONLY="$classifier $maindrv/coreExMx/ip $maindrv/coreExMx/ipv4 $maindrv/coreExMx/ipv6"
        ;;
    *)
        echo "No IP libraries for family $FAMILY"
        exit 0
esac

export BUILD_IPLIB=YES
LINUX_BUILD_KERNEL=NO
DONT_LINK=YES

export BUILD_ONLY
