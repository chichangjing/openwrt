#!/bin/sh

test -d h || mkdir h
cd h
CMDH=../../cpssEnabler/mainCmd/h/cmdShell
for f in FS/cmdFS.h os/cmdStream.h lua/cmdLuaCommon.h lua/genwrapper_API.h;do ln -s $CMDH/$f .;done
cd ../src
CMDS=../../cpssEnabler/mainCmd/src/cmdShell
ln -s $CMDS/FS .
ln -s $CMDS/lua .
