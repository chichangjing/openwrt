#------------------------------------------------------------------------------
#  File name: gen_xml_funcdescr.py
#------------------------------------------------------------------------------
# Usage:
#TODO
#   1. Compile Linux appDemo / appDemoSim with DEBUG_INFO=ON
#   2. Get type info using GDB:
#      $ gdb appDemoSim > type_info_file <<eof
#      > info types
#      > quit
#      > eof
#   3. Run script:
#      python gen_xml_funcdescr.py \
#           -i type_info_file_dx \
#           -C /home/serg/tmp/Marvell/cpss \
#           -F dxCh \
#           -o cpssAPI.xml
#   4. Check generated file
#
#--------------------------------------------------------------------------
#  File version: 2 $
#--------------------------------------------------------------------------

import pprint
pp = pprint.PrettyPrinter(indent=2,width=100)
import re, os, getopt, sys, time

cpssRoot = "/home/serg/tmp/Marvell/cpss"

integer_types = [ "int", "char", "short", "long" ]
float_types = [ "float", "double" ]

reName = re.compile(r"}\s*([a-zA-Z_]\w*);\r?\n?$")
reTypedefInt = re.compile(r'typedef (signed |unsigned |short |long )*(int|char|short|long) ([a-zA-Z_][a-zA-Z_0-9]*);\r?\n?$')
reStruct_field = re.compile(r"^\s*([a-zA-Z_]\w*)\s+([a-zA-Z_]\w*)(\s*\[(\d+)\])?;\r?\n?$")
reStruct_comprex_field = re.compile(r"^\s*(struct|union)\s*\{\s*\.\.\.\s*\}\s*([a-zA-Z_]\w*);$")
reStruct_comprex_field2 = re.compile(r"^\s*(struct|union)\s+[a-zA-Z_]\w*\s+([a-zA-Z_]\w*);$")
reEnumItem = re.compile(r"\b([a-zA-Z_][a-zA-Z_0-9]*)\b(\s*=\s*\d+)?\s*,?")
_reArrayParameter = re.compile(r"(\w+)\[\w*\]")
_reCommentSection = re.compile(r'([A-Z\s]+):$')
_reParamDescr = re.compile(r' {0,12}([a-zA-Z_][a-zA-Z_0-9]*) +- +(.*)$')

reStructEmbedStart = re.compile(r"\s*(struct|union)(\s+[a-zA-Z_]\w*)?\s*{", 0)
reStructEmbedMember = re.compile(r"\b([a-zA-Z_]\w*)\b\s*\*?\s*\b([a-zA-Z_]\w*)\s*;",0)
reStructEmbedEnd = re.compile(r"}\s*([a-zA-Z_]\w*)\s*;",0)

all_structs = dict()
all_defs = dict() # [type] = "struct" || "enum" || "int"


def scan_type_info_from_gdb(file_name):
    f = open(file_name,"r")
    lines = f.readlines()
    f.close()

    def line_type(line):
        if line.startswith("File "):
            return "file"
        if line.startswith("typedef enum {"):
            return "enum"
        if line.startswith("typedef struct {"):
            return "struct"
        if line.startswith("typedef union {"):
            return "union"
        if reTypedefInt.match(line) != None:
            return "int"
        if line.startswith("typedef "):
            return "none"
        if line.startswith("enum "):
            return "none"
        if line.startswith("struct "):
            return "none"
        if line.strip() == "":
            return "none"
        return ""

    def apply_decl(collected):
        if len(collected) == 0:
            return
        ltype = line_type(collected[0])
        global reName
        global all_structs
        global all_defs
        m = reName.search(collected[-1])
        if m != None:
            name = m.group(1)
            if name not in all_defs:
                all_defs[name] = ltype
                if ltype in ["struct", "union"]:
                    all_structs[name] = collected

    status = "none"
    collected = list()
    for line in lines:
        ltype = line_type(line)
        if status in ["enum", "struct", "union" ] and ltype == "":
            collected.append(line)
            continue
        if status in ["enum", "struct", "union" ]:
            # get name from lines
            apply_decl(collected)
            collected = list()
            status = ""
        if ltype in ["enum", "struct", "union" ]:
            status = ltype
            collected.append(line)
            continue
        if ltype == "int":
            m = reTypedefInt.match(line)
            all_defs[m.group(3)] = "int"





# list of wrappers
defs = dict()


def type_classify(t):
    global all_defs
    global integer_types
    if t == "bool" or t == "GT_BOOL":
        return "bool"
    if t in integer_types:
        return "int"
    if t in float_types:
        return "float"
    if t in all_defs:
        return all_defs[t]
    return "UNKNOWN"



# remove 
def lstrip_list(lines):
    min_indent = None
    for line in lines:
        if line.strip() == "":
            continue
        indent = len(line) - len(line.lstrip())
        if min_indent == None:
            min_indent = indent
        if min_indent > indent:
            min_indent = indent
    r = list()
    for line in lines:
        if line.strip() == "":
            r.append("")
            continue
        r.append(line[min_indent:])
    return "\n".join(r).rstrip().decode("windows-1250").encode("utf-8")




#####################
### Main
#####################

from scan_utils import *

#from xml.dom.minidom import getDOMImplementation
from xml.dom import minidom
xml_impl = minidom.getDOMImplementation()

def all_files_x(roots, patterns='*', single_level=False, yield_folders=False):
    retlist=list()
    for root in roots:
        for f in all_files(root, patterns, single_level, yield_folders):
            retlist.append(f)
    return retlist

if __name__ == '__main__':
    gdb_type_info = list()
    of = sys.stdout
    ofname = "<STDOUT>"
    is_file = False
    families = list()
    add_descriptions = False
    add_comments = False
    bad_struct_fname = "<STDOUT>"
    manual_structs = dict()
    mapping = dict()
    structures_required = list()
    banList = list()

    opts,rest = getopt.getopt(sys.argv[1:], "i:o:A:C:F:DcB:M:m:?")
    for (opt,val) in opts:
        if opt == '-i':
            gdb_type_info.append(val)
        elif opt == '-o':
            of = open(val, "w")
            ofname = val
            is_file = True
        elif opt == "-B":
            bad_struct_fname = val
        elif opt == "-M":
            mdoc = minidom.parse(val)
            top_elem = mdoc.documentElement
            for e in top_elem.getElementsByTagName("Structures"):
                for s in e.getElementsByTagName("Structure"):
                    ns = s.cloneNode(0)
                    for m in s.getElementsByTagName("Member"):
                        t = m.getAttribute("type").encode("ascii")
                        if m.getAttribute("class") == "struct" and t not in all_defs:
                            if t not in structures_required:
                                structures_required.append(t)
                        ns.appendChild(m.cloneNode(0))
                    t = ns.getAttribute("name").encode("ascii")
                    manual_structs[t] = ns
                    all_defs[t] = "struct"
                    if ns.getAttribute("special") == "unused":
                        all_defs[t] = "unusable"
        elif opt == "-A":
            #load file with list of banned API's
            
            # try to open input file
            try:
                banListFile = open(val, 'r')
            except IOError:
                print "Error: cannot open file: " + val + "\n"
                sys.exit(2)

            for line in banListFile.readlines():
                banList.append(line.strip())
            banListFile.close()

        elif opt == '-m':
            # load structure names mapping
            pf = open(val, "r")
            for line in pf.readlines():
                line = line.strip()
                if line.startswith("#"):
                    continue
                ll = line.split()
                if len(ll) == 2:
                    mapping[ll[0]] = ll[1]
            pf.close()

        elif opt == "-C":
            cpssRoot = val
        elif opt == "-F":
            families.append(val)
        elif opt == "-D":
            add_descriptions = True
        elif opt == "-c":
            add_comments = True

        elif opt == '-?':
            print "Usage: gen_xml_funcdescr.py -i gdb_type_info [-o outfile] [-r] [-p file] [-F family]"
            print "    -i gdb_type_info  an output of gdb command \"info types\""
            print "    -o outfile        Output file"
            print "    -C path           Scan CPSS sources from different path"
            print "    -F family         generate for given families (dxCh,exMxPm,etc)"
            print "    -B fname          Save unparsed structures to this file"
            print "    -M fname          Load manual structure definitions from this file"
            print "    -m map_file       field mapping file   STRUCT_NAME.fpath luaName"
            print "    -D                Add description fields"
            print "    -A fname          Load list of banned API's"
            sys.exit(0)

    if len(gdb_type_info) < 1:
        print "*** gdb_type_info parameter required"
        sys.exit(0)

    for type_info_file in gdb_type_info:
        scan_type_info_from_gdb(type_info_file)

    doc = xml_impl.createDocument(None,'CpssFunctionPrototypes',None)
    # list of all declarations
    decls = dict()
    scanned_structs = dict()

    search_dirs = [
        os.path.join(cpssRoot,"mainPpDrv","h"),
        os.path.join(cpssRoot,"common","h")
    ]
    # scan CPSS header files for structures/unions prototypes
    for srcpath in all_files_x(search_dirs,"*.h"):
        # skip uneeded families
        family='none'
        for f in [ "dxCh", "dxSal", "exMx", "exMxPm" ]:
            if srcpath.find(os.path.sep+f+os.path.sep) >= 0:
                family = f
        if family != 'none' and family not in families:
            continue
        # skip private headers
        if srcpath.find('private'+os.path.sep) >= 0:
            continue
        if srcpath.find(os.path.sep+'prv') >= 0:
            continue
        source = readfile(srcpath)
        for decl in scan_struct_descriptions(source):
            nm = decl["name"]
            if nm in scanned_structs:
                continue
            if nm not in all_defs:
                all_defs[nm] = decl["type"]
            if decl["type"] not in ["struct", "union"]:
                continue
            if 'body' not in decl:
                continue
            s = decl['body']
            pos = 0
            stack = [ [] ]
            while True:
                m1 = reStructEmbedStart.search(s,pos)
                m2 = reStructEmbedEnd.search(s,pos)
                if m1 == None and m2 == None:
                    nxt = len(s)
                elif m1 != None and m2 != None:
                    if m1.start(0) < m2.start(0):
                        nxt = m1.start(0)
                    else:
                        nxt = m2.start(0)
                elif m2 == None:
                    nxt = m1.start(0)
                else:
                    nxt = m2.start(0)
                sub = s[pos:nxt]
                pos1 = 0
                while True:
                    m3 = reStructEmbedMember.search(sub,pos1)
                    if m3 == None:
                        break
                    stack[-1].append([ m3.group(2), m3.group(1) ])
                    pos1 = m3.end(0)

                if m1 != None and m1.start(0) == nxt:
                    # start
                    if m1.group(1) == "union":
                        decl["has_unions"] = True
                    stack.append([])
                    pos = m1.end(0)
                elif m2 != None:
                    # end
                    if len(stack) < 2:
                        break # smth is wrong
                    for x in stack[-1]:
                        stack[-2].append([ m2.group(1)+"."+x[0], x[1] ])
                    del stack[len(stack)-1]
                    pos = m2.end(0)
                else:
                    break
            if len(stack) != 1:
                continue

            decl['flds'] = stack[-1]
            scanned_structs[nm] = decl


    # scan CPSS header files for function prototypes
    for srcpath in all_files_x(search_dirs, "*.h"):
        # skip uneeded families
        family='none'
        for f in [ "dxCh", "dxSal", "exMx", "exMxPm" ]:
            if srcpath.find(os.path.sep+f+os.path.sep) >= 0:
                family = f
        if family != 'none' and family not in families:
            continue
        # skip private headers
        if srcpath.find('private'+os.path.sep) >= 0:
            continue
        if srcpath.find(os.path.sep+'prv') >= 0:
            continue
        source = readfile(srcpath)
        for decl in scan_c_src(source):
            if 'name' not in decl:
                continue
            name = decl['name']
            # skip private functions
            if name.startswith(("prv","pvr")):
                continue
            # skip banned API's
            if name in banList:
                continue
            if decl['type'] != "GT_STATUS":
                #print name+" doesn't return GT_STATUS"
                continue
            decl['param_descr'] = dict()
            if 'comment' in decl:
                # parse comment
                c = decl['comment']
                c = c.replace("\n*", "\n")
                c = c.replace("\n *", "\n")
                desc = None
                state = None
                lparam_name = None
                lparam_descr = None
                def complete_param_descr():
                    global decl
                    global lparam_name, lparam_descr
                    if lparam_name != None:
                        decl['param_descr'][lparam_name] = lstrip_list(lparam_descr)
                    lparam_name = None
                    lparam_descr = None

                for line in c.split("\n"):
                    line = line.rstrip()
                    v = line.replace("\t"," ").replace("  ", " ").strip()
                    m = _reCommentSection.match(v)
                    if m != None:
                        if state == "INPUTS" or state == "OUTPUTS":
                            complete_param_descr()
                        state = m.group(1).strip()
                        continue
                    if state == "DESCRIPTION":
                        if desc == None:
                            desc = list()
                        desc.append(line)
                    if state == "INPUTS" or state == "OUTPUTS":
                        m = _reParamDescr.match(line)
                        if m != None:
                            complete_param_descr()
                            lparam_name = m.group(1)
                            lparam_descr = list()
                            lparam_descr.append(m.group(2))
                        elif lparam_name != None:
                            lparam_descr.append(line.strip())
                            
                if desc != None:
                    decl['descr'] = lstrip_list(desc)
                    #print "descr='"+decl['descr']+"'"
                #print "param_descr=",decl['param_descr']
            skip = False
            # handle parameters
            decl['aparams'] = list()
            for param in decl['params']:
                pdir = param[0]
                ptype = param[1]
                pname = param[2]
                # handle wrong parameter types
                if not pdir in [ "IN", "OUT", "INOUT" ]:
                    skip = True
                # handle pointers
                if ptype.endswith("*"):
                    ptype = ptype[:-1].strip()
                if pname.startswith("*"):
                    pname = pname[1:].strip()
                aparam = {'dir' : pdir, 'type': ptype, 'name' : pname }
                aparam['class'] = type_classify(ptype)
                if pname in decl['param_descr']:
                    aparam['descr'] = decl['param_descr'][pname]
                if pname.endswith(("Ptr","Prt")):
                    aparam['name'] = pname[:-3]
                decl['aparams'].append(aparam)
                # handle arrays
                m = _reArrayParameter.match(pname)
                if m != None:
                    skip = True
                    break
                if pdir in [ "IN", "INOUT" ]:
                    if type_classify(ptype) not in ["bool", "int", "float", "enum", "struct"]:
                        #print "=> ptype='"+ptype+"' cl='"+type_classify(ptype)+"'",param
                        skip = True
                        break
                if ptype.endswith("*") or pname.startswith("*"):
                    skip = True
                    
            if skip:
                continue
            x = doc.createElement("Function")
            x.setAttribute("name", name)
            x.setAttribute("type", decl['type'])
            # add comment
            if add_descriptions and 'descr' in decl:
                descr = doc.createElement("Description")
                descr.appendChild(doc.createTextNode(decl['descr']))
                x.appendChild(descr)
            if add_comments and 'comment' in decl:
                descr = doc.createElement("Comment")
                comment = decl['comment']
                # fix bad characters (cp1250 chars)
                comment = comment.replace('\x90','&nbsp')
                comment = comment.replace('\x91','`')
                comment = comment.replace('\x92','\'')
                comment = comment.replace('\x93','"')
                comment = comment.replace('\x94','"')
                comment = comment.replace('\x95','*')
                comment = comment.replace('\x96','-')
                comment = comment.replace('\x97','-')
                descr.appendChild(doc.createCDATASection(comment))
                x.appendChild(descr)
            params = doc.createElement("Params")
            for p in decl['aparams']:
                param = doc.createElement("Param")
                param.setAttribute("dir", p['dir'])
                param.setAttribute("type", p['type'])
                param.setAttribute("name", p['name'])
                param.setAttribute("class", p['class'])
                if p['class'] == "struct" and p['dir'] in ["IN","INOUT"]:
                    if p['type'] not in structures_required:
                        structures_required.append(p['type'])
                if p['type'] in manual_structs:
                    _m = manual_structs[p['type']]
                    if _m.hasAttribute("special"):
                        param.setAttribute("special", _m.getAttribute("special"))
                if add_descriptions and 'descr' in p:
                    descr = doc.createElement("Description")
                    descr.appendChild(doc.createTextNode(p['descr']))
                    param.appendChild(descr)
                params.appendChild(param)
            x.appendChild(params)
            decls[name] = x

    #pp.pprint(scanned_structs)

    structs = dict()
    #TODO: add manual structs for external file
    for t,c in manual_structs.items():
        structs[t] = c
    bad_structs = dict()

    while len(structures_required) > 0:
        for t in structures_required[:]:
            #TODO: skip manually implemented
            if t in structs or t in bad_structs:
                structures_required.remove(t)
                continue
            if t not in structures_required:
                continue
            if t not in all_structs and t not in scanned_structs:
                print "xxx: type="+t
                structures_required.remove(t)
                continue
            if t not in all_structs and t in scanned_structs:
                c = scanned_structs[t]
                x = doc.createElement("Structure")
                x.setAttribute("name", t)
                for fld in c['flds']:
                    fname = fld[0]
                    ftype = fld[1]
                    fcls = type_classify(ftype)
                    handled = True
                    if fcls in ["bool", "int", "float", "enum", "struct", "union"]:
                        member = doc.createElement("Member")
                        member.setAttribute("name", fname)
                        member.setAttribute("class", fcls)
                        member.setAttribute("type", ftype)
                        x.appendChild(member)
                        if fcls in ["struct","union"] and ftype not in structs and ftype not in bad_structs:
                            if ftype not in structures_required:
                                structures_required.append(ftype)
                        if fcls == "union":
                            x.setAttribute("special","union")
                if c["type"] == "union":
                    x.setAttribute("special","UNION")
                structs[t] = x
                structures_required.remove(t)
            else:
                c = all_structs[t]
                if t not in structures_required:
                    continue
                #TODO: skip manually implemented
                if len(c) > 2:
                    x = doc.createElement("Structure")
                    x.setAttribute("name", t)
                    errors = False
                    for line in c[1:-1]:
                        line = line.strip()
                        m = reStruct_field.match(line)
                        error = ""
                        if m != None:
                            ftype = m.group(1)
                            fname = m.group(2)
                            # do mapping
                            if t+"."+fname in mapping:
                                fname = mapping[t+"."+fname]
                            fcls = type_classify(m.group(1)) 
                            if fcls in ["bool", "int", "float", "enum", "struct", "union"]:
                                member = doc.createElement("Member")
                                member.setAttribute("name", fname)
                                member.setAttribute("class", fcls)
                                member.setAttribute("type", ftype)
                                if m.group(4) != None:
                                    member.setAttribute("array", m.group(4))
                                x.appendChild(member)
                                if fcls in ["struct", "union"] and ftype not in structs and ftype not in bad_structs:
                                    if ftype not in structures_required:
                                        structures_required.append(ftype)
                                if fcls == "union":
                                    x.setAttribute("special","union")
                            else:
                                error = "cannot identify type class"
                        else:
                            handled = False
                            if t in scanned_structs:
                                m = reStruct_comprex_field.match(line)
                                if m == None:
                                    m = reStruct_comprex_field2.match(line)
                                if m != None:
                                    prefix = m.group(2)+"."
                                    for fld in scanned_structs[t]['flds']:
                                        if fld[0].startswith(prefix):
                                            fname = fld[0]
                                            ftype = fld[1]
                                            # do mapping
                                            if t+"."+fname in mapping:
                                                fname = prefix+mapping[t+"."+fname]
                                            fcls = type_classify(ftype)
                                            handled = True
                                            if fcls in ["bool", "int", "float", "enum", "struct", "union"]:
                                                member = doc.createElement("Member")
                                                member.setAttribute("name", fname)
                                                member.setAttribute("class", fcls)
                                                member.setAttribute("type", ftype)
                                                x.appendChild(member)
                                                if fcls in ["struct","union"] and ftype not in structs and ftype not in bad_structs:
                                                    if ftype not in structures_required:
                                                        structures_required.append(ftype)
                                                if fcls == "union":
                                                    x.setAttribute("special","union")
                            if handled and 'has_unions' in scanned_structs[t]:
                                x.setAttribute("special","union")

                            if not handled:
                                error = "unhandled line"
                        if error != "":
                            x.appendChild(doc.createComment(" error: "+error+" "+line.rstrip()+" "))
                            errors = True
                    #x.writexml(sys.stdout,"", " ", "\n")
                    if t in scanned_structs and scanned_structs[t]["type"] == "union":
                        x.setAttribute("special", "UNT")
                    if not errors:
                        structs[t] = x
                    else:
                        bad_structs[t] = x
                    structures_required.remove(t)

    # Now sort nodes and save xml to file
    top_elem = doc.documentElement

    all_funcs = decls.keys()
    all_funcs.sort()
    x = doc.createElement("Functions")
    for f in all_funcs:
        x.appendChild(decls[f])
    x.setAttribute("count", "%d"%len(all_funcs))
    top_elem.appendChild(x)
    struct_names = structs.keys()
    if len(struct_names) > 0:
        struct_names.sort()
        x = doc.createElement("Structures")
        for t in struct_names:
            x.appendChild(structs[t])
        x.setAttribute("count", "%d"%len(struct_names))
        top_elem.appendChild(x)

    doc.appendChild(doc.createComment(" "+" ".join(sys.argv)+" "))
    doc.appendChild(doc.createComment(" Generated "+time.ctime()+" "))
    doc.writexml(of,"", " ", "\n", "utf-8")

    if is_file:
        of.close()

    if len(bad_structs) > 0:
        if bad_struct_fname != "<STDOUT>":
            bad_struct_f = open(bad_struct_fname, "w")
        else:
            bad_struct_f = sys.stdout

        bdoc = xml_impl.createDocument(None,'CpssFunctionPrototypes',None)
        top_elem = bdoc.documentElement
        x = doc.createElement("Structures")
        for t,v in bad_structs.items():
            x.appendChild(v)
        x.setAttribute("count", "%d"%len(bad_structs))
        top_elem.appendChild(x)
        bdoc.writexml(bad_struct_f,"", " ", "\n", "utf-8")

        if bad_struct_fname != "<STDOUT>":
            bad_struct_f.close()


