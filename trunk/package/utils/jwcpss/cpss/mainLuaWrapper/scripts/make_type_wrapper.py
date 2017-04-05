#------------------------------------------------------------------------------
#  File name: make_type_wrapper.py
#------------------------------------------------------------------------------
# Usage:
#   1. Compile Linux appDemo / appDemoSim with DEBUG_INFO=ON
#   2. Get type info using GDB:
#      $ gdb appDemoSim > type_info_file <<eof
#      > info types
#      > quit
#      > eof
#   3. Create optional file with already defined types already_implemented.lst
#      jn the following form
#          struct  STRUCTURE_NAME1
#          struct  STRUCTURE_NAME2
#          enum    ENUM_NAME3
#      (first use only)
# TODO: mapping file
#   4. Run script:
#      python make_type_wrapper.py \
#           -i type_info_file \
#           -p already_implemented.lst 
#           -r \
#           -o try.c \
#           CPSS_GEN_CFG_DEV_INFO_STC CPSS_INTERFACE_INFO_STC CPSS_DRAM_FREQUENCY_ENT
#   5. Check generated file
#   6. add types listed at the begin of file to already_implemented.lst
#
#--------------------------------------------------------------------------
#  File version: 2 $
#--------------------------------------------------------------------------

import re, os, getopt, sys, time


integer_types = [ "int", "char", "short" ]
float_types = [ "float", "double" ]

reName = re.compile(r"}\s*([a-zA-Z_]\w*);\r?\n?$")
reTypefefInt = re.compile(r'typedef (.* )?(int|char|short) ([a-zA-Z_][a-zA-Z_0-9]*);\r?\n?$')
reStruct_field = re.compile(r"^\s*([a-zA-Z_]\w*)\s+([a-zA-Z_]\w*)(\s*\[(\d+|[a-zA-Z_][a-zA-Z_0-9]*)\])?;\r?\n?$")
reStruct_comprex_field = re.compile(r"^\s*(struct|union)\s*\{\s*\.\.\.\s*\}\s*([a-zA-Z_]\w*);$")
reStruct_comprex_field2 = re.compile(r"^\s*(struct|union)\s+[a-zA-Z_]\w*\s+([a-zA-Z_]\w*);$")
reEnumItem = re.compile(r"\b([a-zA-Z_][a-zA-Z_0-9]*)\b(\s*=\s*\d+)?\s*,?")

reStructEmbedStart = re.compile(r"\s*(struct|union)(\s+[a-zA-Z_]\w*)?\s*{", 0)
reStructEmbedMember = re.compile(r"\b([a-zA-Z_]\w*)\b\s*\*?\s*\b([a-zA-Z_]\w*)\s*;",0)
reStructEmbedEnd = re.compile(r"}\s*([a-zA-Z_]\w*)\s*;",0)

all_structs = dict()
all_unions = dict()
all_enums = dict()
all_defs = dict() # [type] = "struct" || "enum" || "union"
scanned_structs = dict()
mapping = dict() # structname.path => luaName

includes = [ 'extUtils/luaCLI/luaCLIEngine_genWrapper.h' ]

# hardcode this because CPSS_VERSION_INFO_STC also defined
# in mainFaDrv/h/cpssFa/generic/version/gtFaVersion.h
# GT_U64 defined in mainPpDrv/src/cpss/generic/labServices/ddr/ddr3libv2/h/Os/gtOs/gtEnvDep.h
include_map = {
    'CPSS_VERSION_INFO_STC': 'cpss/generic/version/cpssGenVersion.h',
    'GT_U64': 'cpss/extServices/os/gtOs/gtOs.h'
}


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
        if reTypefefInt.match(line) != None:
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
        global all_unions
        global all_enums
        global all_defs
        m = reName.search(collected[-1])
        if m != None:
            name = m.group(1)
            if name not in all_defs:
                if ltype == "struct":
                    all_structs[name] = collected
                    all_defs[name] = "struct"
                if ltype == "enum":
                    all_enums[name] = collected
                    all_defs[name] = "enum"
                if ltype == "union":
                    all_unions[name] = collected
                    all_defs[name] = "union"


    status = "none"
    collected = list()
    for line in lines:
        ltype = line_type(line)
        if status in [ "enum", "struct", "union"]:
            if ltype == "":
                collected.append(line)
                continue
            if status != "enum" and line.strip() == "":
                continue
            # get name from lines
            apply_decl(collected)
            collected = list()
            status = ""
        if ltype in [ "enum", "struct", "union"]:
            status = ltype
            collected.append(line)
            continue
        if ltype == "int":
            m = reTypefefInt.match(line)
            all_defs[m.group(3)] = "int"





# list of types already implemented (just refer them
types_already_implemented = dict() # [name] = "struct" || "enum"
# list of all required types (with recursive ones)
decls = dict()
# list of wrappers
defs = dict()


# list of required types (from cmd line)
types_required = list()

def type_classify(t):
    global all_defs
    global integer_types
    if t == "bool" or t == "GT_BOOL":
        return "BOOL"
    if t in integer_types:
        return "NUMBER"
    if t in float_types:
        return "FLOAT"
    if t in types_already_implemented:
        if types_already_implemented[t] == "struct":
            return "STRUCT"
        if types_already_implemented[t] == "struct_custom":
            return "STRUCT_CUSTOM"
        if types_already_implemented[t] == "array_type":
            return "ARRAY_TYPE"
        if types_already_implemented[t] == "array_type_custom":
            return "ARRAY_TYPE_CUSTOM"
        if types_already_implemented[t] == "enum":
            return "ENUM"
    if t in all_defs:
        if all_defs[t] == "enum":
            return "ENUM"
        if all_defs[t] == "struct":
            return "STRUCT"
        if all_defs[t] == "int":
            return "NUMBER"
    if t in scanned_structs:
        if scanned_structs[t]['type'] == "struct":
            return "STRUCT"
    return "UNKNOWN"

def to_c_field(t,s,fpath,fname,ftype,fcls):
    global mapping
    if t+"."+fpath not in mapping:
        # no mapping
        if fcls == "BOOL":
            return "    F_BOOL("+s+", -1, "+fname+");\n"
        if fcls in [ "NUMBER", "FLOAT", "ENUM", "STRUCT", "STRUCT_CUSTOM", "ARRAY_TYPE", "ARRAY_TYPE_CUSTOM"]:
            return "    F_"+fcls+"("+s+", -1, "+fname+", "+ftype+");\n"
        return ""
    # do mapping here
    newname = mapping[t+"."+fpath]
    if fcls == "BOOL":
        return "    F_BOOL_N("+s+", -1, "+fname+", "+newname+");\n"
    if fcls in [ "NUMBER", "FLOAT", "ENUM", "STRUCT", "STRUCT_CUSTOM", "ARRAY_TYPE", "ARRAY_TYPE_CUSTOM"]:
        return "    F_"+fcls+"_N("+s+", -1, "+fname+", "+newname+", "+ftype+");\n"
    return ""

def to_lua_field(t,s,idx,fpath,fname,ftype,fcls):
    global mapping
    if fcls.endswith("_CUSTOM"):
        fcls = fcls[:-7]
    if t+"."+fpath not in mapping:
        if fcls == "BOOL":
            return "    FO_BOOL("+s+", "+idx+", "+fname+");\n"
        if fcls in [ "NUMBER", "FLOAT", "ENUM", "STRUCT", "ARRAY_TYPE"]:
            return "    FO_"+fcls+"("+s+", "+idx+", "+fname+", "+ftype+");\n"
        return ""
    # do mapping here
    newname = mapping[t+"."+fpath]
    if fcls == "BOOL":
        return "    FO_BOOL_N("+s+", "+idx+", "+fname+", "+newname+");\n"
    if fcls in [ "NUMBER", "FLOAT", "ENUM", "STRUCT", "ARRAY_TYPE"]:
        return "    FO_"+fcls+"_N("+s+", "+idx+", "+fname+", "+newname+", "+ftype+");\n"
    return ""

def make_struct_wrapper(t):
    # scan struct fields, add fiels types to types_required if not in decls
    global types_required
    global scanned_structs
    global mapping
    to_c = 'void prv_lua_to_c_'+t+'(\n    lua_State *L,\n    '+t+' *val\n)\n{\n'
    to_c0 = to_c
    to_l = 'void prv_c_to_lua_'+t+'(\n    lua_State *L,\n    '+t+' *val\n)\n{\n'
    to_l += '    int t;\n    lua_newtable(L);\n    t = lua_gettop(L);\n'
    if all_defs[t] == "struct":
        strings = all_structs[t]
    else:
        strings = all_unions[t]
    if len(strings) > 2:
        state = None
        for fline in strings[1:-1]:
            line = fline.strip()
            if state != None:
                text += fline+'\n'
                cnt += line.find('{')
                if cnt > 0:
                    state = 'su{'
                cnt -= line.find('}')
                if cnt == 0 and state == 'su{':
                    state = 'su{}'
                if state == 'su{}' and ';' in line:
                    error_text  = '#ifndef _VISUALC\n'
                    error_text += '# warning "TBD manually"\n'
                    error_text += '#else\n'
                    error_text += '# pragma message ( "TBD manually" )\n'
                    error_text += '#endif\n'
                    error_text += '#if 0\n'+text+'#endif\n'
                    to_c += error_text
                    to_l += error_text
                    state = None
                continue

            m = reStruct_field.match(line)
            error = ""
            if m != None:
                ftype = m.group(1)
                fname = m.group(2)
                fcls = type_classify(m.group(1)) 
                if m.group(4) == None:
                    # Not an array
                    if fcls in [ "BOOL", "NUMBER", "FLOAT", "ENUM", "STRUCT", "STRUCT_CUSTOM", "ARRAY_TYPE", "ARRAY_TYPE_CUSTOM" ]:
                        to_c += to_c_field(t, "val", fname,fname, ftype, fcls)
                        to_l += to_lua_field(t, "val", "t",fname,fname,ftype,fcls)
                    else:
                        error = "cannot identify type class"
                else:
                    # array
                    to_c += "    F_ARRAY_START(val, -1, "+fname+");\n"
                    to_l += "    FO_ARRAY_START(val, t, "+fname+");\n"
                    if fcls in [ "NUMBER", "FLOAT", "STRUCT", "ENUM", "BOOL" ]:
                        to_c += "    {\n"
                        to_c += "        int idx;\n"
                        to_c += "        for (idx = 0; idx < "+m.group(4)+"; idx++) {\n"
                        to_c += "            F_ARRAY_"+fcls+"(val, "+fname+", idx, "+ftype+");\n"
                        to_c += "        }\n"
                        to_c += "    }\n"
                        #
                        to_l += "    {\n"
                        to_l += "        int idx;\n"
                        to_l += "        for (idx = 0; idx < "+m.group(4)+"; idx++) {\n"
                        to_l += "            FO_ARRAY_"+fcls+"(val, "+fname+", idx, "+ftype+");\n"
                        to_l += "        }\n"
                        to_l += "    }\n"
                    else:
                        error = "can't handle "+fcls+" for array"
                    to_c += "    F_ARRAY_END(val, -1, "+fname+");\n"
                    to_l += "    FO_ARRAY_END(val, t, "+fname+");\n"
            elif t in scanned_structs:
                m = reStruct_comprex_field.match(line.strip())
                if m == None:
                    m = reStruct_comprex_field2.match(line.strip())
                if m != None:
                    #TODO: more levels
                    subf = m.group(2)
                    valid = True
                    for fld in scanned_structs[t]['flds']:
                        if fld[0].startswith(subf+"."):
                            if fld[0].find(".",len(subf)+1) >= 0:
                                valid = False
                                break
                            fcls = type_classify(fld[1])
                            if fcls not in [ "NUMBER", "FLOAT", "STRUCT", "ENUM", "BOOL" ]:
                                valid = False
                    if valid:
                        # mapping
                        subfNm = subf
                        if t+"."+subf in mapping:
                            subfNm = mapping[t+"."+subf]
                        to_c += "    lua_getfield(L, -1, \""+subfNm+"\");\n"
                        to_c += "    if (lua_istable(L, -1))\n"
                        to_c += "    {\n"
                        to_l += "    {\n"
                        to_l += "        int t1;\n"
                        to_l += "        lua_newtable(L);\n"
                        to_l += "        t1 = lua_gettop(L);\n"
                        for fld in scanned_structs[t]['flds']:
                            if fld[0].startswith(subf+"."):
                                fname = fld[0][len(subf)+1:] 
                                fcls = type_classify(fld[1])
                                to_c += "    "+to_c_field(t, "&(val->"+subf+")", fld[0], fname, fld[1], fcls)
                                to_l += "        "+to_lua_field(t, "&(val->"+subf+")", "t1",fld[0], fname,fld[1],fcls)
                        to_c += "    }\n"
                        to_c += "    lua_pop(L, 1);\n"
                        to_l += "        lua_setfield(L, t, \""+subfNm+"\");\n"
                        to_l += "    }\n"
                    else:
                        error = "unhandled line"
                else:
                    error = "unhandled line"
            else:
                error = "unhandled line"
            if line.strip().startswith(('struct','union')) and error == "unhandled line":
                state = 'su'
                text = fline+'\n'
                cnt = line.count('{')
                if cnt > 0:
                    state = 'su{'
                cnt -= line.count('};')
                continue
            if error == "":
                # add enum/struct to gen_list if not defined yet 
                if fcls in ["ENUM", "STRUCT", "STRUCT_CUSTOM", "ARRAY_TYPE", "ARRAY_TYPE_CUSTOM" ]:
                    if ftype not in decls:
                        if ftype in types_already_implemented:
                            decls[ftype] = types_already_implemented[ftype]
                        elif ftype not in types_required:
                            types_required.append(ftype)
            if error != "":
                error_text  = '#ifndef _VISUALC\n'
                error_text += '# warning "TBD manually"\n'
                error_text += '#else\n'
                error_text += '# pragma message ( "TBD manually" )\n'
                error_text += '#endif\n'
                error_text += '#if 0\n    '+error+'\n'+line+'\n#endif\n'
                to_c += error_text
                to_l += error_text
    if all_defs[t] == "union":
        to_c = to_c0
        to_c += "    /* just do nothing */\n"
    to_c += '}\n'
    to_l += '    lua_settop(L, t);\n}\n'
    return to_c+'\n'+to_l+'add_mgm_type('+t+');\n'

def parse_union_members(t):
    # scan union members, add fiels types to types_required if not in decls
    global types_required
    if len(all_unions[t]) < 3:
        return
    for line in all_unions[t][1:-1]:
        m = reStruct_field.match(line)
        error = ""
        if m != None:
            ftype = m.group(1)
            fname = m.group(2)
            fcls = type_classify(m.group(1)) 
        else:
            error = "unhandled line"
        if error == "":
            # add enum/struct to gen_list if not defined yet 
            if fcls == "ENUM" or fcls == "STRUCT":
                if ftype not in decls:
                    if ftype not in types_required:
                        types_required.append(ftype)

def make_enum_wrapper(t):
    # TODO
    result = 'enumDescr enumDescr_'+t+'[] = {\n'
    for line in all_enums[t]:
        # get part of line in { }
        f = line.find('{')
        if f >= 0:
            line = line[f+1:]
        f = line.rfind('}')
        if f >= 0:
            line = line[:f] + ","

        p = 0
        while True:
            m = reEnumItem.search(line, p)
            if m == None:
                break
            result += '    ENUM_ENTRY('+m.group(1)+'),\n'
            p = m.end(0)
    result += '    { 0, NULL }\n'
    result += '};\n'
    result += 'add_mgm_enum('+t+');\n'
    return result




#####################
### Main
#####################
#types_already_implemented = { "CPSS_PORTS_BMP_STC" : "struct" }
#types_required = ["CPSS_GEN_CFG_DEV_INFO_STC", "CPSS_INTERFACE_INFO_STC", "CPSS_DRAM_FREQUENCY_ENT" ]


if __name__ == '__main__':
    gdb_type_info = list()
    of = sys.stdout
    ofname = "<STDOUT>"
    is_file = False
    scan_cpss_api = False
    # Generate declarations for missing recursive types
    recursion = True
    generate_wrappers_for_all_types = False
    excludelist = list()
    family_only = 'ALL'


    opts, types_required = getopt.getopt(sys.argv[1:], "i:o:rNp:M:C:ATX:F:?")
    for (opt,val) in opts:
        if opt == '-i':
            gdb_type_info.append(val)
        elif opt == '-o':
            of = open(val, "w")
            ofname = val
            is_file = True
        elif opt == '-r':
            recursion = True
        elif opt == '-N':
            recursion = False
        elif opt == '-p':
            # load list of already implemented types
            pf = open(val, "r")
            for line in pf.readlines():
                ll = line.split()
                if len(ll) > 1 and (ll[0] in ['enum', "struct", "struct_custom", "array_type", "array_type_custom"]):
                    types_already_implemented[ll[1]] = ll[0]
            pf.close()

        elif opt == '-M':
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

        elif opt == "-A":
            scan_cpss_api = True
        elif opt == "-C":
            scan_cpss_api = True
            cpssRoot = val
        elif opt == "-T":
            generate_wrappers_for_all_types = True
        elif opt == "-X":
            pf = open(val, "r")
            for line in pf.readlines():
                if not line.startswith('#') and line.strip() != '':
                    print "add to exclude '"+line.strip()+"'"
                    excludelist.append(line.strip())
            pf.close
        elif opt == '-F':
            family_only = val

        elif opt == '-?':
            print "Usage: make_type_wrapper.py -i gdb_type_info [-o outfile] [-r] [-p file] [-F family]"
            print "    -i gdb_type_info  an output of gdb command \"info types\""
            print "    -o outfile        Output file"
            print "    -r                Enable recursion"
            print "    -N                Disable recursion"
            print "    -p file           The file with list of already defined struct/enums"
            print "    -C path           Scan CPSS sources from different path"
            print "    -A                Scan CPSS sources for APIs"
            print "    -T                Generate wrappers for all mainPpDrv/, common/ types"
            print "    -X exclude_list   file with types to be ignored with -T"
            print "    -F family         when used with -T generate for given family only (dxCh,exMxPm,NONE)"
            print "    -M map_file       field mapping file   STRUCT_NAME.fpath luaName"
            sys.exit(0)

    if len(gdb_type_info) < 1:
        print "*** gdb_type_info parameter required"
        sys.exit(0)

    # scan CPSS header files for typedefs
    if scan_cpss_api:
        from scan_utils import *

        for srcpath in all_files(cpssRoot,"*.h"):
            # scan files in /h/ only
            if not srcpath.startswith(cpssRoot):
                continue
            incpath = srcpath[len(cpssRoot):].replace('\\', '/')
            file_is_mainPpDrv_common = False
            if incpath.startswith("/mainPpDrv/") or incpath.startswith("/common/"):
                file_is_mainPpDrv_common = True
            f = incpath.find('/h/')
            if f < 0:
                continue
            incpath = incpath[f+3:]
            if incpath.startswith("cpss/extServices"):
                file_is_mainPpDrv_common = False
            if incpath.endswith("/cpssExtMacDrv.h"):
                file_is_mainPpDrv_common = False
            # for given famuly only (if specified)
            family = "NONE"
            if incpath.startswith("cpss/dxCh"):
                family = "dxCh"
            if incpath.startswith("cpss/dxSal"):
                family = "dxSal"
            if incpath.startswith("cpss/exMx"):
                family = "exMx"
            if incpath.startswith("cpss/exMxPm"):
                family = "exMxPm"
            if incpath.startswith("cpssFa"):
                family = "cpssFa"
            if incpath.startswith("mainFxDrv"):
                family = "mainFxDrv"
            if incpath.startswith("mainTmDrv"):
                family = "mainTmDrv"
            # incpath = path for #include <>
            source = readfile(srcpath)
            for decl in scan_struct_descriptions(source):
                if 'name' not in decl:
                    continue
                name = decl['name']
                if name not in include_map:
                    include_map[name] = incpath
                if 'body' in decl and name not in scanned_structs:
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
                    scanned_structs[name] = decl

                if generate_wrappers_for_all_types:
                    if not file_is_mainPpDrv_common:
                        continue
                    if not name.startswith("CPSS"):
                        continue
                    if name.endswith("_FUNC"):
                        continue
                    if name in excludelist:
                        continue
                    if family_only != 'ALL' and family != family_only:
                        continue
                    if name not in types_required:
                        types_required.append(name)

    for type_info_file in gdb_type_info:
        scan_type_info_from_gdb(type_info_file)

    while len(types_required) > 0:
        types_required.sort()
        for t in types_required[:]:
            if t in types_already_implemented:
                decls[t] = types_already_implemented[t]
            else:
                # add implementation
                if t not in all_defs:
                    # get information from source scan
                    if t in scanned_structs:
                        decl = scanned_structs[t]
                        if decl['type'] in ['struct','union']:
                            arr = []
                            arr.append('typedef '+decl['type']+' {')
                            for line in decl['body'].split("\n"):
                                if line.strip() != '':
                                    arr.append(line)
                            arr.append('} '+t+';')
                            if decl['type'] == 'struct':
                                all_structs[t] = arr
                            else:
                                all_unions[t] = arr
                            all_defs[t] = decl['type']
                if t not in all_defs:
                    if generate_wrappers_for_all_types:
                        types_required.remove(t)
                        continue
                    print "*** Can't find info for type "+t
                else:
                    if all_defs[t] == "enum":
                        # scan enum, add array into defs[t]
                        defs[t] = make_enum_wrapper(t)
                        decls[t] = "enum"
                    if all_defs[t] == "struct":
                        defs[t] = make_struct_wrapper(t)
                        decls[t] = "struct"
                    if all_defs[t] == "union":
                        parse_union_members(t)
                        decls[t] = "union"
                        defs[t] = make_struct_wrapper(t)
                types_already_implemented[t] = decls[t]
            types_required.remove(t)
        if recursion == False:
            break
    for t in types_required[:]:
        if t not in decls:
            if t in all_defs:
                if all_defs[t] in [ "enum", "struct" ]:
                    decls[t] = all_defs[t]
            elif t in types_already_implemented:
                decls[t] = types_already_implemented[t]


    #print "all_defs=",all_defs
    #print "types_already_implemented=",types_already_implemented
    #print "decls=",decls
    #print "defs=",defs

    # add includes for all used types
    for k in decls.keys():
        if k in include_map:
            if include_map[k] not in includes:
                includes.append(include_map[k])
    for k in defs.keys():
        if k in include_map:
            if include_map[k] not in includes:
                includes.append(include_map[k])

    print >>of, "/*************************************************************************"
    print >>of, "* "+ofname
    print >>of, "*"
    if generate_wrappers_for_all_types:
        print >>of, "*       WARNING!!! this is a generated file, please don't edit it manually"
        print >>of, "*       See COMMENTS for command line"
        print >>of, "*"
    print >>of, "* DESCRIPTION:"
    print >>of, "*       A lua type wrapper" 
    print >>of, "*       It implements support for the following types:"
    for k in defs.keys():
        print >>of, "*           %-8s"%decls[k] + k
    print >>of, "*"
    print >>of, "* DEPENDENCIES:"
    print >>of, "*"
    print >>of, "* COMMENTS:"
    print >>of, "*       Generated at "+time.ctime()
    print >>of, "*       "+" ".join(sys.argv)
    print >>of, "*"
    print >>of, "* FILE REVISION NUMBER:"
    print >>of, "*       $Revision: 15 $"
    print >>of, "**************************************************************************/"
    if not scan_cpss_api:
        print >>of, "/* Add feature specific includes. here */"
        print >>of, ""
    for k in includes:
        print >>of, "#include <"+k+">"
    print >>of, ""

    print >>of, "/***** declarations ********/\n"
    for k, v in decls.items():
        if v == 'enum' or v == 'struct':
            print >>of, 'use_prv_'+v+'('+k+');'
        if v == 'struct_custom':
            print >>of, 'use_prv_struct('+k+');'
        if v in ['array_type', 'array_type_custom']:
            print >>of, 'use_prv_array_type('+k+');'

    print >>of, "\n/***** declarations ********/\n"
    for k, v in defs.items():
        print >>of, v
    if is_file:
        of.close()
