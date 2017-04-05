#!/usr/bin/env python
import subprocess
import string
import os
import random


def validate_mac_addr(mac):
    mac_sp = mac.split(':')
    #simple check mac addr format
    if len(mac_sp) == 6:
        return True
    return False


def get_mac_random():
    mac_list = ['0c', 'a4', '2a', 'ff']
    for i in range(1,3):
        randstr = "".join(random.sample("0123456789abcdef", 2))
        mac_list.append(randstr)
        random_mac = ":".join(mac_list)

    return random_mac


def set_mac_addr(mac):
    if mac == 'ff:ff:ff:ff:ff:ff' or mac == '00:00:00:00:00:00':
        mac = get_mac_random()
        print mac

    netdev_list = os.listdir('/sys/class/net/')
    for dev in netdev_list:
        if dev == 'lo':
            continue
        os.system('ifconfig %s down'%dev)
        os.system('ifconfig %s hw ether %s'%(dev, mac))
        os.system('ifconfig %s up'%dev)


if __name__ == "__main__":
    output = subprocess.check_output(['jw_setmac'])
    output_line = output.split('\n', 3)

    os.system('mount -t jffs2 /dev/mtdblock4 /overlay')

    if os.path.isfile('/overlay/sysupgrade.tgz'):
        os.system('sysupgrade -r /overlay/sysupgrade.tgz')
        os.system('rm -f /overlay/sysupgrade.tgz')
        print 'Restore sysupgrade.tgz'

    if os.path.isfile('/overlay/network'):
        os.system('mv /overlay/network /etc/config/')
        os.system('rm -f /overlay/network')
        print 'Restore network config'


    os.system('umount /overlay')

    for line in output_line:
        line_sp = line.split('=')
        key = string.strip(line_sp[0])
        value = string.strip(line_sp[1])

        if key == 'mac':
            mac = value[1:-1]
            #print mac
            if not validate_mac_addr(mac):
                print 'invalid mac addr format'
                break
            set_mac_addr(mac)
            break


