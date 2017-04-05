#!/usr/bin/python
#
# CPSS Api to Lua genwrapper description
#
# $Revision: 1$
#
###########################################################################################

import os, re, sys, getopt
from scan_utils import *

cpssRoot = "x:/cpss/SW/prestera"
cpssRoot = "/home/serg/tmp/Marvell/cpss"

_reArrayParameter = re.compile(r"(\w+)\[\w*\]")

def Usage():
    print "Usage: gen_lua_funcdescr.py [-o outfile] [-C CPSS_ROOT] name [name...]"
    print "    -o outfile        Output file"
    print "    -C CPSS_ROOT      override defalt CPSS_ROOT ("+cpssRoot+")"
    sys.exit(0)

#########################################################################################
# Main Function
# Return zero if program works without errors, one - otherwise.
#
if __name__ == '__main__':

    of = sys.stdout
    ofname = "<STDOUT>"
    is_file = False


    opts, required_APIs = getopt.getopt(sys.argv[1:], "o:C:?")
    for (opt,val) in opts:
        if opt == '-o':
            of = open(val, "w")
            ofname = val
            is_file = True
        elif opt == '-C':
            cpssRoot = val

        elif opt == '-?':
            Usage()

    if len(required_APIs) == 0:
        Usage()


    for srcpath in all_files(cpssRoot,"*.h"):
        source = readfile(srcpath)
        for decl in scan_c_src(source):
            name = decl['name']

            if name in required_APIs:
                required_APIs.remove(name)
                #print decl
                # print description
                print >>of, "cpssAPIs[\""+name+"\"] = {"
                for i in range(len(decl['params'])):
                    d=","
                    if i+1 == len(decl['params']):
                        d = ""
                    param = decl["params"][i]
                    pdir = param[0]
                    ptype = param[1]
                    pname = param[2]
                    # handle pointers
                    if ptype.endswith("*"):
                        ptype = ptype[:-1].strip()
                    if pname.startswith("*"):
                        pname = ptype[1:].strip()
                    # handle arrays
                    m = _reArrayParameter.match(pname)
                    if m != None:
                        pname = m.group(1).strip()
                    
                    print >>of, "  { \""+pdir+"\", \""+ptype+"\", \""+pname+"\" }"+d
                print >>of, "}"
                if len(required_APIs) == 0:
                    exit(0)
    exit(0)
