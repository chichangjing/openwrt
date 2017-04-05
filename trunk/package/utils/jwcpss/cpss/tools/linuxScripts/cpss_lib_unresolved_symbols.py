#!/usr/bin/env python

######################################################################
# check_mainPpDrv_lib.py
#
# DESCRIPTION:
#       Check list of externals
#
# INPUTS:
#       path to mainPpDrv.a
#       path to the list of allowed externals
#
# OUTPUTS:
#       stdout
#
# FILE REVISION NUMBER:
#       $Revision: $
#####################################################################

import os,sys,re

def Usage():
    print "Usage: "+sys.argv[0]+" mainPpDrv.a [externals_list]"
    sys.exit(0)

externals_list = dict()
sym_U = dict()
re_U = re.compile(r"\s+U\s+(\S+)$")
sym_defined = dict()
re_defined = re.compile(r"[\da-fA-F]+\s+[TRBC]\s+(\S+)$")

# GCC's external
sym_defined['__stack_chk_fail'] = 1

if __name__ == '__main__':
    if len(sys.argv) < 2:
        Usage()
    # load list of allowed symbols
    if len(sys.argv) > 2:
        f = open(sys.argv[2],"r")
        for line in f.readlines():
            externals_list[line.strip()] = 1
        f.close()
    # load list of symbols from library
    f = os.popen("nm "+sys.argv[1])
    for line in f.readlines():
        m = re_U.match(line)
        if m != None:
            sym_U[m.group(1)] = 1
        m = re_defined.match(line)
        if m != None:
            sym_defined[m.group(1)] = 1
    f.close()

    undefined = list()
    for sym in sym_U.keys():
        if sym in sym_defined or sym in externals_list:
            continue
        undefined.append(sym)

    undefined.sort()

    print "\n".join(undefined)

