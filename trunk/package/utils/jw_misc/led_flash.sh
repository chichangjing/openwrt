#!/bin/sh
opinon() {
    N=$1
    if [ ! -d /sys/class/gpio/gpio$N ] ; then
        echo "$N" > /sys/class/gpio/export
    fi
    echo "out" > /sys/class/gpio/gpio$N/direction

    while true
    do  
        echo "1" > /sys/class/gpio/gpio$N/value
        sleep 1
        echo "0" > /sys/class/gpio/gpio$N/value
    done
}

pin_init() {
    N=$1
    if [ ! -d /sys/class/gpio/gpio$N ] ; then
        echo "$N" > /sys/class/gpio/export
    fi
    echo $2 > /sys/class/gpio/gpio$N/direction
}

led_on(){
    echo "1" > /sys/class/gpio/gpio$1/value
}


led_off() {
    echo "0" > /sys/class/gpio/gpio$1/value
}


pin_status() {
    echo pin $1 $2 status detected:
    cat /sys/class/gpio/gpio$1/value
}

devmem 0x020e0134 32 0x05
devmem 0x020e0130 32 0x05
devmem 0x020e01f4 32 0x05
devmem 0x020e01fc 32 0x05
devmem 0x020e01ec 32 0x05
devmem 0x020e01f0 32 0x05
devmem 0x020e01e4 32 0x05
devmem 0x020e019c 32 0x05
devmem 0x020e01e8 32 0x05
devmem 0x020e0110 32 0x05

#L10
pin_init 105 out
#run
pin_init 76 out
#alarm
pin_init 75 out
#ring
pin_init 121 out
#NC
pin_init 122 out
#K-OUT
pin_init 119 out
#power
pin_init 120 in
pin_init 117 in
#rst
pin_init 118 in
#power 1
pin_init 67 in

while true
do
    led_on 76
    led_on 75
    led_on 121
    led_on 122
    led_on 119
    led_on 105
    sleep 1
    led_off 76
    led_off 75
    led_off 121
    led_off 122
    led_off 119
    led_off 105
    sleep 1
    #power_status 120
    #power_status 117
    pin_status 120 pow_a
    pin_status 117 pow_b
    pin_status 118 reset_button
    pin_status 67 pow_c
done

