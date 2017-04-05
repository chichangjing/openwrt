#!/usr/bin/python
######################################################################################
#
# Util to fix CPSS paths in dependency files and remove standard headers
#
######################################################################################

import string, re
import sys, os
import fnmatch
import getopt

def Usage():
    print "Usage:"
    print "    "+sys.argv[0]+"[-v] objs_dir cpss_dir [cpss_dir_fix]"
    print "        -v  - print fixed files to stdout"
    print "Example:"
    print "    "+sys.argv[0]+" /tmp/cpss_4.1_999 /home/serg/cpss_4.1_999"
    sys.exit(0)

if __name__ == '__main__':
    verbose = False
    opts, paths = getopt.getopt(sys.argv[1:], "v?")
    for (opt,val) in opts:
        if opt == "-v":
            verbose = True
        elif opt == "-?":
            Usage()
        else:
            Usage()
    if len(paths) < 2:
        Usage()
    objs_dir = paths[0]
    cpss_dir = paths[1]
    cpss_dir_fix = cpss_dir # old: '/cpss/SW/prestera'
    if len(paths) > 2:
        cpss_dir_fix = paths[2]

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
            dst = src.replace(cpss_dir+"/", cpss_dir_fix+"/")
            # stage2: remove all paths outside cpss
            stage2 = ''
            changed = False
            cont = False
            b = list()
            for line in dst.split("\n"):
                b.append(line)
                if cont:
                    tline += " " + line.lstrip()
                else:
                    tline = line
                if tline.endswith("\\"):
                    tline = tline[:-1].rstrip()
                    cont = True
                    continue
                cont = False
                c = tline.split()
                if len(c) < 1 or not c[0].endswith(":"):
                    stage2 += "\n".join(b) + "\n"
                    b = list()
                    continue
                b = list()
                d = list()
                d.append(c[0])
                for l in c[1:]:
                    if l.startswith(cpss_dir_fix) or not l.startswith("/"):
                        d.append(l)
                    else:
                        changed = True
                if len(d) < 2:
                    continue
                dline = ''
                for l in d:
                    if dline == '':
                        dline += l
                    else:
                        if len(dline) + len(l) > 81:
                            stage2 += dline + " \\\n"
                            dline = ""
                        dline += " " + l
                if dline != '':
                    stage2 += dline + "\n"
                
            if changed:
                dst = stage2
            if dst != src:
                # if file changed
                if verbose:
                    print filepath
                # save original timestamps
                stat_res = os.stat(filepath)
                f = open(filepath, "w")
                f.write(dst)
                f.close()
                # set timestamps to original ones
                os.utime(filepath, (stat_res.st_atime,stat_res.st_mtime))



