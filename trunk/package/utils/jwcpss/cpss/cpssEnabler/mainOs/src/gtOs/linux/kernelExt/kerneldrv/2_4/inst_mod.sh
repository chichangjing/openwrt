#!/bin/sh
# parameters: 

module_file=$1
home_romfs=$2

test -d $home_romfs || exit 0

################################################
#
# 1. Copy module to $home_romfs/lib/modules
#
################################################
echo "	cp $module_file $home_romfs/lib/modules"
cp $module_file $home_romfs/lib/modules


################################################
#
# 2. Check if device node exists.
#    Create node if it not exists yet
#
################################################
cd $home_romfs/dev
if [ \! -c mvKernelExt ]
then
	echo pwd=`pwd`
	echo mknod mvKernelExt c 253 0
	sudo mknod mvKernelExt c 253 0
fi


################################################
#
# 3. Check if $home_romfs/etc/init.sh loads module
#    Add module load commands if necessary
#
################################################
cd $home_romfs/etc
grep -q $module_file init.sh || {
	echo "patching init.sh"
	sed '/telnetd/ a\
\
 if test -e /lib/modules/'$module_file'\
 then\
   test -c /dev/mvKernelExt || mknod /dev/mvKernelExt c 253 0\
   insmod /lib/modules/'$module_file'\
 fi' init.sh >init.sh.$$
	cat init.sh.$$ >init.sh
	rm init.sh.$$
}
