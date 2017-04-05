#!/usr/bin/python
#------------------------------------------------------------------------------
#  File name: luaCLI_GetCommands.py
#------------------------------------------------------------------------------
# Usage:
#   luaCLI_GetCommands.py [-o outfile] <scripts_dir>
#       -o outfile        Output file
#
# Example
#   python luaCLI_GetCommands.py -o /tmp/cmds_list_dx.xml /lua/lua_cli/scripts
#
#--------------------------------------------------------------------------
#  File version: 1 $
#--------------------------------------------------------------------------

import os, re, sys, getopt
from xml.dom import minidom

reAddCommand = re.compile(r'CLI_addCommand\s*\(\s*"([^"][^"]*)"\s*,\s*"([^"][^"]*)"',re.M)
reAddCommand2 = re.compile(r'CLI_addCommand\s*\(\s*\{([^\}]*)\}\s*,\s*"([^"][^"]*)"',re.M)
reModeListSplit = re.compile(r'"\s*,\s*"',re.M)

commands = dict()
def command(mode, command):
    global commands
    if mode not in commands:
        commands[mode] = list()
    if command not in commands[mode]:
        commands[mode].append(command)

def do_addCommand(m):
    command(m.group(1), m.group(2))

def do_addCommand2(m):
    cmd = m.group(2)
    mds = m.group(1).strip()
    if not mds.startswith('"') or not mds.endswith('"'):
        return
    mds = mds[1:-1]
    for m in  reModeListSplit.split(mds):
        command(m, cmd)

def scan_file(fname):
    f=open(fname)
    content=f.read()
    f.close()
    reAddCommand.sub(do_addCommand, content)
    reAddCommand2.sub(do_addCommand2, content)

if __name__ == '__main__':
    #TODO: root dir
    of = sys.stdout
    ofname = "<STDOUT>"
    is_file = False

    opts,rest = getopt.getopt(sys.argv[1:], "o:?")
    for (opt,val) in opts:
        if opt == '-o':
            of = open(val, "w")
            is_file = True
        elif opt == '-?':
            print "Usage: "+sys.argv[0]+" [-o outfile] <scripts_dir>"
            print "    -o outfile        Output file"
            sys.exit(0)

    if len(rest) < 1:
        print "No scripts_dir argument passed"
        sys.exit(1)
    scripts_dir = rest[0]

    for path, subdirs, files in os.walk(scripts_dir):
        for name in files:
            if not name.endswith(".lua"):
                continue
            # .lua file
            scan_file(os.path.join(path, name))

    # generate xml
    doc = minidom.getDOMImplementation().createDocument(None,'CLIcommands',None)
    for mode, cmds in commands.items():
        m = doc.createElement("Mode")
        m.setAttribute("name", mode)
        doc.documentElement.appendChild(m)
        for cmd in cmds:
            c = doc.createElement("Command")
            c.setAttribute("cmd", cmd)
            m.appendChild(c)
    
    doc.appendChild(doc.createComment(" "+" ".join(sys.argv)+" "))
    doc.writexml(of, "", " ", "\n", "utf-8")
    if is_file:
        of.close()

