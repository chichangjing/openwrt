#!/bin/bash

export CPSS_PATH=/home/serg/tmp/Marvell/cpss

export DEBUG_INFO=D_ON
export LUACLI_STANDALONE=yes
#export INCLUDE_ENHANCED_UTF=YES
$CPSS_PATH/Build.sh \
	-u UTF_YES \
	-l \
	Linux i386 LinuxWM \
	DX simulation CPSS_ENABLER_SHARED \
	/home/serg/tmp/Marvell/objsShared -
