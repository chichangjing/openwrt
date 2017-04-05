#!/usr/bin/env python
import string
import time
import os

export_path = '/sys/class/gpio/export'
unexport_path = '/sys/class/gpio/unexport'
direction_path = '/sys/class/gpio/gpio%d/direction'
value_path = '/sys/class/gpio/gpio%d/value'

def init_gpio(gpio):
    gpio_nr = '%d'%gpio

    if os.path.exists(direction_path %gpio):
        fp = open(unexport_path, 'w')
        fp.write(gpio_nr)
        fp.close()

    fp = open(export_path, 'w')
    fp.write(gpio_nr)
    fp.close()

    fp = open(direction_path %gpio, 'w')
    fp.write('in')
    fp.close()

    fp = open(value_path %gpio, 'r')
    return fp


def restore_conf():
    os.system('mount -t jffs2 /dev/mtdblock4 /overlay')
    os.system('cp -f /usr/share/defconf.tgz /overlay/sysupgrade.tgz')
    os.system('sync')
    os.system('umount /overlay')
    os.system('reboot')


if __name__ == "__main__":
    rst_button_fp = init_gpio(118)
    btn_pressed_sec = 0

    while True:
        var = rst_button_fp.read(10)
        #print 'button counter %d' %btn_pressed_sec
        #print 'var length %d' %len(var)
        if var[0] == '0':
            #print 'button pressed\n'
            btn_pressed_sec = btn_pressed_sec + 1
            rst_button_fp.seek(0, 0)
            time.sleep(1)
            if btn_pressed_sec > 5:
                restore_conf()
            continue

        btn_pressed_sec = 0
        time.sleep(1)
        rst_button_fp.seek(0, 0)

