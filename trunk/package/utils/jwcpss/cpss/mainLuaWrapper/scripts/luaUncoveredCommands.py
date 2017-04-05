#!/usr/local/bin/python
#
# DESCRIPTION:
#   Create html report for uncovered Lua commands from Lua UT's log
#
# PARAMETERS: 
#   srcipt runs with following parameters
#               1) input logfile name
#               2) output html file name
#
# USE:
#   python luaUncoveredCommands.py logfile.txt logfile.htm
#
#
# $Revision: 1 $

import re
import sys
from pprint import *

#------------------------------------------------------------------------------
# create list of uncovered lua commands in UT
# in:
#   lines - list of log file strings
# out:
#   output dictionary 
#         contextDict[context] = {'commandsTotal' , 'commandsUncovered' , 'uncoveredList':[]}
#   totalCommandsNum 
#          total number of lua commands
#
def parseLuaCommands(lines):
    
    inProgress = False
    context = ""
    commandsList = []
    performedCommands = []
    luaUncoveredCommands = []

    # result of parsing
    totalCommandsNum = 0
    contextDict = {}

    # scan thru all lines
    for line in lines:

        # skip too short lines
        if len(line) < 2:
            continue
        
        # skip system logs
        if "LINK CHANGED" in line:
            continue

        # list of performed commands
        if line.startswith("Performing: ("):
            m = re.match(r"(Performing: \()(?P<context_name>\w+)(\)\# )(?P<command>.+)", line)
            if m:
                pContext = m.group('context_name')
                pCommand = m.group('command')
                pCommand = pCommand.replace(' ', '->')
                performedCommands.append(pContext + '.' + pCommand)
            continue


        if (not inProgress) and ("Console(debug)# show cli commands all" in line):
            inProgress = True
            continue

        if not inProgress:
            continue

        if inProgress and line.endswith(":"):
            context = line[:-1]
            continue
        
        if inProgress and context == "":
            continue
        
        if inProgress and "Console(debug)#" in line:
            inProgress = False
            break
        
        
        tmpLine = line.strip()
        commandsList.append(context+"."+tmpLine)
        
        if not contextDict.has_key(context):
            contextDict[context] = {'commandsTotal' : 1, 'commandsUncovered' : 0, 'uncoveredList' : []}
        else:
            contextDict[context]['commandsTotal'] += 1 
            
        totalCommandsNum = totalCommandsNum + 1


    for command in commandsList:
        inUse = False
        for pCommand in performedCommands:
            if pCommand.startswith(command):
                inUse  = True
                break
        if not inUse:
            luaUncoveredCommands.append(command)


    for command in luaUncoveredCommands:
        commandArray = command.split(".", 1)
        contextDict[commandArray[0]]['uncoveredList'].append(commandArray[1]) 
        contextDict[commandArray[0]]['commandsUncovered'] += 1 

    return contextDict, totalCommandsNum

#------------------------------------------------------------------------------
# generate report of uncovered lua commands
#
# in:
#   inFileName      - input(log) file name
#   outFileName     - output(html) file name
#   luaUncoveredCommands - list of uncovered commands
#   totalCommandsNum - total number of lua commands
#
def generateLuaUncoveredCommandsReport(inFileName, outFileName, contextDict, totalCommandsNum):

    # try create output file
    try:
        outFile = open(outFileName, 'w')
    except IOError:
        print "Error: cannot create output html report file: " + outFileName + "\n"
        sys.exit(2)

    # print a report title
    outFile.write('<head><title>Uncovered commands report for %s</title></head>' % inFileName)
    
    uncoveredCommandsNum = 0
    
    # print embedded javaScript body
    outFile.write('\n'                                             \
                  '<script><!--\n'                                 \
                  ' function show_hide(idName)\n'                  \
                  ' {\n'                                           \
                  '    var id = document.getElementById(idName);\n'\
                  '    if (id.style.display == "none")\n'          \
                  '    {\n'                                        \
                  '        id.style.display =  "table-row-group";\n' \
                  '    }\n'                                        \
                  '    else\n'                                     \
                  '    {\n'                                        \
                  '        id.style.display = "none";\n'           \
                  '    }\n'                                        \
                  ' }\n'                                           \
                  '--></script>\n'\
                  '\n')
    # reports body style
    outFile.write('<body><p style="font-family: Tahoma, Verdana, Helv, serif;">\n')
    # print reports name
    outFile.write('<br><b>Uncovered commands report for %s </b><br><br>\n' % inFileName)
    # start of table header
    outFile.write('<br><table border><tr><th>LUA CONTEXT</th><th>UNCOVERED</th><th>TOTAL</th></tr> \n')


    # iterate over context______________________________________________________
    for context in contextDict:

        uncoveredCommandsNum += contextDict[context]['commandsUncovered']
        
        outFile.write('<tbody style="cursor: pointer; cursor: hand;" onclick="show_hide(\'tb_%s\')">\n' % context)
        outFile.write('<tr><td><font color=#DD3333><li type="square">%s</li></font></td>' \
                          '<td align="right"><font color=#DD3333>%d</font></td>'\
                          '<td align="right">%d</td></tr>\n' %
                          (context, contextDict[context]['commandsUncovered'], contextDict[context]['commandsTotal']) )
        outFile.write('</tbody>\n')                              
        outFile.write('<tbody id="tb_%s" style="display:none">\n' % context)                                                  

        for commandLine in contextDict[context]['uncoveredList']:
            outFile.write('<tr><td><small><font color=#993333>&nbsp;-&nbsp;%s</font></small></td>' \
                          '<td align="right">&nbsp</td>'\
                          '<td align="right">&nbsp</td></tr>\n' % commandLine)

        outFile.write('</tbody>\n')


    # write output report footer________________________________________________
    outFile.write('<tr><th>Total:</th>'\
                      '<th align="right">%d</th>'\
                      '<th align="right">%d</th></tr>\n' %
                      (uncoveredCommandsNum,  totalCommandsNum) )
    
    # close table
    outFile.write("</table><br>\n")
    # end of report
    outFile.write("</p></body>")
    # close outFile
    outFile.close()
    return

#------------------------------------------------------------------------------
# main module with import feature
#
def akaMain(argv):
    
    argc = len(argv)
    if argc < 3:
        print "\nBad parameters.\nUse: python luaUncoveredCommands.py inLogFile.txt outReportFile.htm\n"
        sys.exit(2)

    inFileName  = argv[1]
    outFileName = argv[2]

    # try open and read input log
    try:
        inFile = open(inFileName, 'r')
        lines = inFile.readlines()
        inFile.close()
    except IOError:
        print "IO Error: cannot to read log file: " + inFileName + " \n"
        sys.exit(2)

    print "Generate report ", outFileName, " .. ",

    #remove newline symbols and trailing whitespaces
    for i in range(len(lines)):
        lines[i] = lines[i][:-1].strip()

    contextDict, totalCommandsNum = parseLuaCommands(lines)
    generateLuaUncoveredCommandsReport(inFileName,  outFileName, 
                                       contextDict, totalCommandsNum)

    print " finished.\nReport completed.\n"

#------------------------------------------------------------------------------
# main module stub to prevent autoexecute
#
if __name__ == '__main__':
    akaMain(sys.argv)
    sys.exit(0)

