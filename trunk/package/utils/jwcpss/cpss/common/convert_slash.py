#!/usr/bin/python
# convert_slash.py
import logging, sys, getopt, os,string, glob

########################################################################################
def del_old_file_V01(filepath):

    if os.path.isfile(filepath):
       try:
           os.remove(filepath)
           #print 'removed:', filepath
       except OSError:
           pass

######################################################################33

def Usage():
    print "Usage:"
    print "    "+sys.argv[0]+" INPUT_NAME INPUT_PATH CONVERT_PATH_SLASH"
    print "Example:"
    print "    "+sys.argv[0]+" CONFIG_IMAGES c:\conf_img3.sh c:\Git\cpss.super\cpss\tools\common"
    sys.exit(0)

if __name__ == '__main__':

    opts, parm = getopt.getopt(sys.argv[1:], "?")
    for (opt,val) in opts:
        if opt == "-?":
            Usage()
        else:
            Usage()
    #print 'len(parm)=', len(parm)  
    if len(parm) < 3:
        Usage()

    i=0
    INPUT_NAME         = parm[i]; i=i+1
    INPUT_PATH         = parm[i]; i=i+1
    CONVERT_PATH_SLASH = parm[i]; i=i+1

    OUT_PATH = INPUT_PATH.replace('\\','/')

    #print 'INPUT_NAME        =', INPUT_NAME          
    #print 'INPUT_PATH        =', INPUT_PATH          
    #print 'CONVERT_PATH_SLASH=', CONVERT_PATH_SLASH  
    #print 'OUT_PATH          =', OUT_PATH          

    del_old_file_V01(CONVERT_PATH_SLASH)

    f_cs_config_bat = open(CONVERT_PATH_SLASH, "w")

    file_line_bat =  'SET '+INPUT_NAME +'='+ OUT_PATH; f_cs_config_bat.write(file_line_bat+'\n');  # print 'convert_slash.py',file_line_bat

    f_cs_config_bat.close()
