#
# Copyright (C) 2010-2015 OpenWrt.org
#

. /lib/imx6.sh

platform_check_image() {
	local board="jws2100"

	case "$board" in
	jws2100)
        echo "JoyWare check image ..."
		return 0;
		;;
	esac

	echo "Sysupgrade is not yet supported on $board."
	return 1
}

platform_pre_upgrade() {
	local board=$(imx6_board_name)
    #echo "platform_pre_upgrade ... Testing ..."
    #return 1

	case "$board" in
	jws2100)
        echo "Pre upgrade in jws2100 ..."
		#suppose upgarde uboot and dtb here ?
        #or just check the file existen ?
		;;
	esac
}

uboot_path="openwrt-imx6ul-mx6ul_14x14_evk-u-boot.imx"
dtb_path="imx6ul-joyware.dtb"
kernel_path="openwrt-imx6ul-zImage"
rootfs_path="openwrt-imx6ul-generic-jffs2-64k.img"
platform_do_upgrade() {
    local board=$(imx6_board_name)

    case "$board" in
    jws2100)
        echo "platform do upgrade"
        if [ -f "/tmp/images/$uboot_path" ]; then
            echo "upgrade uboot $uboot_path"
            dd if=/dev/mtd0 of=/tmp/bootpart
            dd if="/tmp/images/$uboot_path" of=/tmp/bootpart bs=1k seek=4 count=444 conv=notrunc
            bootpart_up="yes"
            echo "uboot" > /tmp/status
        fi
        if [ -f "/tmp/images/$dtb_path" ]; then
            echo "upgrade dtb $dtb_path"
            if [ ! -e "/tmp/bootpart" ]; then
                echo "backup boot partition"
                dd if=/dev/mtd0 of=/tmp/bootpart
            fi
            dd if="/tmp/images/$dtb_path" of=/tmp/bootpart bs=1k seek=448 count=64 conv=notrunc
            bootpart_up="yes"
            echo "dtb" > /tmp/status
        fi
        if [ "$bootpart_up"x == "yes"x ]; then
            echo "upgrading boot partition"
            #/usr/sbin/flash_eraseall /dev/mtd0
            /usr/sbin/flashcp /tmp/bootpart /dev/mtd0 -v
        fi
        if [ -f "/tmp/images/$kernel_path" ]; then
            echo "upgrade kernel $kernel_path"
            echo "kernel" > /tmp/status
            /usr/sbin/flash_eraseall -j /dev/mtd1
            /usr/sbin/flashcp "/tmp/images/$kernel_path" /dev/mtd1 -v
        fi
        if [ -f "/tmp/images/$rootfs_path" ]; then
            echo "upgrade rootfs $rootfs_path"
            echo "rootfs" > /tmp/status
            /usr/sbin/flash_eraseall -j /dev/mtd2
            /usr/sbin/flashcp "/tmp/images/$rootfs_path" /dev/mtd2 -v
        fi
        echo "done" > /tmp/status
        ;;
    esac
}
