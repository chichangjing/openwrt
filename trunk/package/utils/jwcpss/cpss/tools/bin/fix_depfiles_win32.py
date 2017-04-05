#!/usr/bin/python
######################################################################################
#
# Util to fix CPSS paths in dependency files
# This utility removes system headers from .dep headers
#
######################################################################################

import string, re
import sys, os
import fnmatch
import getopt

reExt = re.compile("[A-Z]:(/Progra|.*/Borland|.*/Micros)", re.I)

def Usage():
    print "Usage:"
    print "    "+sys.argv[0]+" objs_dir"
    print "Example:"
    print "    "+sys.argv[0]+" x:\\cpss\\objs\\dx_sim"
    sys.exit(0)

if __name__ == '__main__':
    opts, paths = getopt.getopt(sys.argv[1:], "?")
    for (opt,val) in opts:
        if opt == "-?":
            Usage()
        else:
            Usage()
    if len(paths) < 1:
        Usage()
    objs_dir = paths[0]

    # walk in objs_dir
    for root, dirs, files in os.walk(objs_dir):
        for filename in files:
            filepath = os.path.join(root,filename)
            if not filepath.endswith(".dep"):
                continue
            # fix single .dep file
            f = open(filepath)
            src = f.read()
            f.close()
            dst = ''
            for line in src.split("\n"):
                line = line.strip()
                if line == "":
                    dst += "\n"
                    continue
                words = line.split()
                dline = words[0]
                if not dline.endswith(":"):
                    dst += line + "\n"
                    continue
                for word in words[1:]:
                    if reExt.match(word) == None:
                        dline += " " + word
                if dline != words[0]:
                    dst += dline + "\n"
            if dst != src:
                # if file changed
                print filepath
                # save original timestamps
                stat_res = os.stat(filepath)
                f = open(filepath, "w")
                f.write(dst)
                f.close()
                # set timestamps to original ones
                os.utime(filepath, (stat_res.st_atime,stat_res.st_mtime))



