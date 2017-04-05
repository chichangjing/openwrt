#!/usr/bin/env bash
#
# Build script top level wrapper.
#
# This script should be located at the top of PSS/CPSS projects
#
# It just redirects to tools/build/main.sh to allow
# sharing between PSS and CPSS
#

# detect build scripts path:
# resolve full path to this script and add tools/build
# to get absolute path to build script dir
case $0 in
    */*)
        tool_build="${0%/*}"
        ;;
    *\\*)
        tool_build="${0%\\*}"
        ;;
    *)
        tool_build=.
        ;;
esac
tool_build=`(cd "$tool_build";pwd)`/tools/build

. $tool_build/main.sh
