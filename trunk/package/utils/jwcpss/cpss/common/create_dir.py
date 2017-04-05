#!/usr/bin/python

import os
import time
import re
import sys
#import hashlib 

import filecmp
import os.path
import shutil

import getopt, fnmatch, string, glob, subprocess
import commands

if __name__ == '__main__':

    opts, parm = getopt.getopt(sys.argv[1:], "?")

    i=0

    folder_DST = parm[i]; i=i+1
    print 'folder_DST = ',folder_DST

    if not (os.path.exists(folder_DST)) :
        os.makedirs(folder_DST)
        print 'os.makedirs ',folder_DST

    sys.exit(0)

    #try: 
    #    os.makedirs(folder_DST)
    #    print 'os.makedirs ',folder_DST
    #
    #except OSError:
    #    if not os.path.isdir(folder_DST):
    #        raise

