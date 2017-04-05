#!/bin/sh


echo " ************ devmem show **********"
devmem 0x020c8110 32
devmem 0x020c8120 32
devmem 0x020c8130 32
devmem 0x020c8140 32
devmem 0x020c8150 32
devmem 0x020c8160 32
devmem 0x020c8164 32
devmem 0x020c8168 32
devmem 0x020c816c 32
devmem 0x020c8170 32
devmem 0x020c8174 32
devmem 0x020c8178 32
devmem 0x020c817c 32
devmem 0x020c8270 32
devmem 0x020c8274 32
devmem 0x020c8278 32
devmem 0x020c827c 32


echo " ************ devmem config **********"

devmem 0x020c8120 32 0x00011775
echo " >>> 1"
sleep 1
devmem 0x020c8140 32 0x004c200a
echo " >>> 2"
sleep 1
devmem 0x020c8150 32 0x24008088
echo " >>> 3"
sleep 1
devmem 0x020c8160 32 0x80030000
echo " >>> 4"
sleep 1
devmem 0x020c8164 32 0x80030000
echo " >>> 5"
sleep 1
devmem 0x020c8168 32 0x80030000
echo " >>> 6"
sleep 1
devmem 0x020c816c 32 0x80030000
echo " >>> 7"

echo " ************ devmem show after config **********"
devmem 0x020c8110 32
devmem 0x020c8120 32
devmem 0x020c8130 32
devmem 0x020c8140 32
devmem 0x020c8150 32
devmem 0x020c8160 32
devmem 0x020c8164 32
devmem 0x020c8168 32
devmem 0x020c816c 32
devmem 0x020c8170 32
devmem 0x020c8174 32
devmem 0x020c8178 32
devmem 0x020c817c 32
devmem 0x020c8270 32
devmem 0x020c8274 32
devmem 0x020c8278 32
devmem 0x020c827c 32


echo "************ done ***********"
