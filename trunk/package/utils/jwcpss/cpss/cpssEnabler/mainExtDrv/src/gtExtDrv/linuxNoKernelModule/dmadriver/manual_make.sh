#!/bin/sh
#
# A script to build mvDrvDrv.ko manually
#
# Edit next line and set $DIST to kernel's source path
DIST=/usr/src/kernels/`uname -r`

make -C $DIST M=`pwd` modules
