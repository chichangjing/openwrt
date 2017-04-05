#!/usr/bin/python
#coding=utf-8
import sys 
import os
import shutil

ldir = os.walk('./')
for root, directory, files in ldir:
    #print root, directory
    try:
        di = directory.index('.svn')
    except:
        #print 'not subdir named \'.svn\''
        continue

    path = root + '/' + '.svn'
    print path
    #print di, directory[di]
    shutil.rmtree(path)
