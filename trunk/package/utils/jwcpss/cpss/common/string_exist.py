#::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
#  string_exist.py
#::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::


#SET SUB_STRING=%1
#SET PATH_STRING=%2
#
#SET PATH=c:\Program Files (x86)\Git\bin;C:\Python27;%PATH%.

import  sys, getopt, re, os, fnmatch, string, glob, subprocess
#import path
import commands

##############################################
def del_old_file_V01(filepath):

    if os.path.isfile(filepath):
       try:
           os.remove(filepath)
           #print 'removed:', filepath
       except OSError:
           pass

##############################################
if __name__ == '__main__':
    opts, parm = getopt.getopt(sys.argv[1:], "?")

    i=0

    SUB_STRING          = parm[i]; i=i+1
    PATH_STRING         = parm[i]; i=i+1

    print 'SUB_STRING  =', SUB_STRING          
    #print 'PATH_STRING =', PATH_STRING         

    if SUB_STRING in PATH_STRING:
        ret=1

        PATH_STRING.replace(SUB_STRING,'',100)
        PATH_STRING.replace(';;',";",100)

        print '-----------------------------------------------------------'
        print 'NEW =[',PATH_STRING+']'         
        print '-----------------------------------------------------------'

    else:
        ret=0

    #print 'ret =', ret
    add_2_path='c:\\temp\\' + 'add_2_path.bat'
    del_old_file_V01(add_2_path)
    f_add_2_path = open(add_2_path, "w")

    file_line_bat = 'SET PATH='+SUB_STRING+';'+PATH_STRING ; f_add_2_path.write(file_line_bat+'\n') #; print file_line_bat

    f_add_2_path.close()
    print '******************************************************'


    
    #exit ret      
        

