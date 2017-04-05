######################################################################################
#
# Utils to scan and analyze c code 
#
# $Revision: 9 $
#
######################################################################################

import re
import sys
import os, fnmatch, string
from pprint import *

def all_files(root, patterns='*', single_level=False, yield_folders=False):
    # Expand patterns from semicolon-separated string to list
    patterns = patterns.split(';')
    for path, subdirs, files in os.walk(root, followlinks=True):
        if yield_folders:
            files.extend(subdirs)
        files.sort( )
        for name in files:
            for pattern in patterns:
                if fnmatch.fnmatch(name, pattern):
                    yield os.path.join(path, name)
                    break
        if single_level:
            break

def all_dirs(root, patterns):
    patterns = patterns.split(';')
    for path, subdirs, files in os.walk(root):
        subdirs.sort( )
        for name in subdirs:
            for pattern in patterns:
                if fnmatch.fnmatch(name, pattern):
                    yield os.path.join(path, name)
                    break


# function declarations/definitions scan
_gParseTree = list()

_reSignatureParams = re.compile(r"^\s*(IN|OUT|INOUT)?\s+([_a-zA-Z0-9]+)\s+(\*{0,2}[_a-zA-Z0-9]+(?:\[[^\]]*\])?)", re.M)
_reCommentedSignature = re.compile(r"(/\*[^*]*\*+(?:[^/*][^*]*\*+)*/)\s*(static\s+|extern\s+)?([_a-zA-Z0-9]+)\s+([_a-zA-Z0-9]+)\s*\((.*?)\);?",\
                                  re.M | re.DOTALL)
_reFunctionTextSignature = re.compile(r"^}", re.M | re.DOTALL)
_reChop = re.compile(r"\s*$", re.M)
_reBodyStart = re.compile(r"\s*{",re.M)

def _normalize_param(param, comment):
    temp = list(param)
    #if(param[2][0] == '*' and param[2].endswith("Ptr")):
    #    temp[2] = param[2][:-3]
    return temp

def _do_match(m):
    ddd = dict()
    # ignore function pointer typedefs
    if m.group(3) == 'typedef':
        return m.end(0)
    # ignore 'LUA_API void  (lua_pushnil) (lua_State *L);'
    if m.group(4) == 'void':
        return m.end(0)
    ddd['comment'] = _reChop.sub("",m.group(1))
    ddd['name'] = m.group(4)
    ddd['type'] = m.group(3)
    if m.group(2) != None:
        ddd['modifier'] = m.group(2).strip()
    ddd['line'] = "%d" % (m.string.count('\n',0,m.start(0))+1)
    params = re.findall(_reSignatureParams, m.group(5))
    ddd['params'] = [_normalize_param(entry, "") for entry in params]
    _gParseTree.append(ddd)
    if m.group(0)[-1:] != ';':
        m2 = _reBodyStart.match(m.string[m.end(0):])
        if m2 != None:
            e = m2.end(0)-1
            defi = _reFunctionTextSignature.search(m.string[m.end(0)+e:])
            if defi != None:
                ddd['body'] = defi.string[0:defi.end(0)]
                ddd['bodyline'] = m.string.count('\n',0,m.end(0)+e)+1
                return m.end(0)+e+defi.end(0)
    #return m.group(0)
    return m.end(0)

def scan_c_src(source):
    del _gParseTree[:]
    #_reCommentedSignature.sub(_do_match, source)
    pos = 0
    while True:
        m = _reCommentedSignature.search(source,pos)
        if m == None:
            break
        pos = _do_match(m)
    return _gParseTree



#
# Scan structures from given file
#

_reRmComment = re.compile(r"(/\*[^*]*\*+(?:[^/*][^*]*\*+)*/)", re.M | re.DOTALL)
_reRmCpp = re.compile(r"^\s*#.*$",re.M)
_reif0 = re.compile(r"^\s*#\s*(if\s+0\s*|ifdef\s+undef\s*)$",re.M)
_reendif = re.compile(r"^\s*#\s*endif.*$",re.M)
_reGroup = re.compile(r"[}{]", 0)
_reStructHeader = re.compile(r"([Tt]ypedef:?\s+)?struct(\s+(\S+))?\s*\{(\d+)\{", re.M|re.DOTALL)
_reStructFooter0 = re.compile(r"[^;]*;", re.M|re.DOTALL)
_reStructFooter = re.compile(r"\s*(\S+)?\s*;", re.M|re.DOTALL)
_reStructFooter1 = re.compile(r"\s*(\S+)\s*,\s*\*(\S+)?\s*;", re.M|re.DOTALL)
_reStructFiels = re.compile(r"\s*(struct\s+)?(\S+)\s*(\*)?\s+(\*)?\s*([^;]+)\s*;",re.M)
_reRmSpaces = re.compile(r"\s+",re.M)

_level = 0
_structs = dict()

def _make_nesting(m):
    global _level
    if m.group(0) == '{':
        _level = _level+1
        return "{%d{" % (_level - 1)
    _level = _level-1
    return "}%d}" % _level

def _parse_fields(name_s, name_t, body):
    global _structs
    names = list()
    pos = 0
    while True:
        m = _reStructFiels.search(body,pos)
        if m == None:
            break
        name = _reRmSpaces.sub("",m.group(5))
        if m.group(2) in _structs:
            delim="."
            if m.group(3) != None or m.group(4) != None:
                    delim = "->"
            for field in _structs[m.group(2)][:]:
                names.append(name+delim+field)
        else:
            names.append(name)
        pos = m.end(0)
    if name_s != None:
        _structs[name_s] = names
    if name_t != None:
        _structs[name_t] = names
            

def _parse_structs(str):
    pos = 0
    while True:
        m = _reStructHeader.search(str,pos)
        if m == None:
            break
        bodyend = str.find('}'+m.group(4)+'}', m.end(0))
        if bodyend == -1:
            break
        m0 = _reStructFooter0.search(str,bodyend+3)
        if m0 == None:
            break;
        m1 = _reStructFooter1.search(str,bodyend+3)
        if m1 == None or m1.end(0) > m0.end(0):
            m1 = _reStructFooter.search(str,bodyend+3)
            if m1 == None:
                break
        #
        body = _parse_structs(str[m.end(0):bodyend])
        _parse_fields(m.group(3), m1.group(1), body)
        b = str[:pos]
        e = str[m0.end(0):]
        if m.group(1) != None or m1.group(1) == None:
            #typedef struct {} xxx; or struct aaa {};
            str = b+e
        else:
            if m.group(3) == None:
                str = b + "XXX" + " " + m1.group(1) + ";"
            else:
                str = b + m.group(3) + " " + m1.group(1) + ";"
            pos = len(str)
            str += e
    return str

def readfile(fname):
    f=open(fname)
    data=f.read()
    f.close()
    return data

def remove_comments(body):
    body = _reRmComment.sub("",body)
    pos = 0
    b = ''
    while True:
        m = _reif0.search(body,pos)
        if m == None:
            b += body[pos:]
            break
        m1 = _reendif.search(body,m.end(0))
        if m1 == None:
            b += body[pos:m.end(0)]
            pos = m.end(0)
            continue
        b += body[pos:m.start(0)]
        pos = m1.end(0)
    return _reRmCpp.sub("",b)

def scan_structs(filename):
    global _level
    global _structs
    _structs = dict()
    _level = 0
    header=readfile(filename)
    header = remove_comments(header)
    header = _reGroup.sub(_make_nesting, header)
    _parse_structs(header)
    return _structs



_reApplicableDevicesStart=re.compile(r"APPLICABLE DEVICES:",re.IGNORECASE)
_reApplicableDevicesNorm1=re.compile(r"\s+")
_reApplicableDevicesNorm2=re.compile(r"^\s*")
_reApplicableDevicesNorm3=re.compile(r"\s*[\,]*\s*$")
def parse_cpss_applicable_devices(comment,applicableDevices):
    m = _reApplicableDevicesStart.search(comment)
    if m == None:
        return None,None
    f = comment.find("\n",m.end(0))
    if f < 0:
        return None,None
    description = comment[m.end(0):f]

    description = _reApplicableDevicesNorm1.sub(" ",description)
    description = _reApplicableDevicesNorm2.sub("",description)
    description = _reApplicableDevicesNorm3.sub("",description)
    if description == '':
        if comment[f+1:f+7] == '*     ':
            f1 = comment.find("\n",f+7)
            description = comment[f+7:f1]
            description = _reApplicableDevicesNorm1.sub(" ",description)
            description = _reApplicableDevicesNorm2.sub("",description)
            description = _reApplicableDevicesNorm3.sub("",description)
            if description == '':
                return None,None
    
    if description in applicableDevices:
        return description, applicableDevices[description]

    return description,None


######################################################################################
# scan structures/enums
#
_reCommentedTypedef = re.compile(\
                r"(/\*[^*]*\*+(?:[^/*][^*]*\*+)*/)\s*typedef\s+(enum|struct|union)(\s+[a-zA-Z_][a-zA-Z_0-9]*)?\s*\{",\
                re.M | re.DOTALL)
_reEnumValues = re.compile(r"\s*([^\s,]+)\s*(=[^,]*)?,",re.M)

######################################################################################
# scan structures body 
#
# returns all nested union/structure names and all field names 
# (it useful to check structure description)
# 

####pattern to detect complex nested structure begin
_reComplexNestedStructBegin = re.compile(r"\s*(struct|union)\s*\{",\
                              re.M | re.DOTALL)

####pattern to detect simple structure fields
#group 1 - field type
#group 2 - field name
_reSimpleStructFields = re.compile(r"(\w+)\s*\*?\s+\*?\s*([^;\s]+)\s*;")

def analyze_struct_body(structBody):

    result = dict()

    posComplexStruct = 0
    while True:
        complexStructObject = _reComplexNestedStructBegin.search(structBody, \
                                                                 posComplexStruct)
        if complexStructObject == None:
            break

        #save type of nested object
        nestedType = complexStructObject.group(1)

        #search end of nested object
        p = complexStructObject.end(0)
        nl = 1
        while True:
            pe = structBody.find("}", p)
            ps = structBody.find("{", p)
            if ps > 0 and ps < pe:
                nl = nl + 1
                p = ps + 1
                continue
            if pe < 0:
                print 'Error in code of structure (brace missed)'
                return
            nl = nl - 1
            p = pe + 1
            if nl == 0:
                break

        #save nested body
        nestedBody = structBody[complexStructObject.end(0):p-2]

        #search nested name
        nestedNameObject = _reStructFooter.search(structBody, p)

        if nestedNameObject == None:
            print 'Error in code of structure (structure name missed)'
            return 

        #correct end of complex struct
        posComplexStruct = nestedNameObject.end(0)
        nestedName = nestedNameObject.group(1)

        if nestedType not in result:
            result[nestedType] = list()

        #save object name to result dict
        if nestedName not in result[nestedType]:
            result[nestedType].append(nestedName)

        #save end of current complex structure position
        posComplexStruct = complexStructObject.end(0)

    #################search all fields
    nestedType = 'fields'
    if nestedType not in result:
        result[nestedType] = list()

    posSimpleFields = 0
    while True:
        simpleFieldObject = _reSimpleStructFields.search(structBody, \
                                                         posSimpleFields)
        if simpleFieldObject == None:
            break

        #save fields name
        nestedName = simpleFieldObject.group(2)

        if nestedName == None:
            print 'Error: cannot determine simple field name'
            print simpleFieldObject.group(0)
            print

        if nestedName not in result[nestedType]:
            result[nestedType].append(nestedName)

        #save end of current field position
        posSimpleFields = simpleFieldObject.end(0)

    return result


def scan_struct_descriptions(source):
    del _gParseTree[:]
    pos = 0
    while True:
        m = _reCommentedTypedef.search(source,pos)
        if m == None:
            break
        p = m.end(0)
        nl = 1
        while True:
            pe = source.find("}", p)
            ps = source.find("{", p)
            if ps > 0 and ps < pe:
                nl = nl + 1
                p = ps + 1
                continue
            if pe < 0:
                # notify error
                return _gParseTree
            nl = nl - 1
            p = pe + 1
            if nl == 0:
                break
        ddd = dict()

        ddd['comment'] = _reChop.sub("",m.group(1))
        ddd['type'] = m.group(2)
        ddd['line'] = "%d" % (m.string.count('\n',0,m.start(0))+1)
        body = remove_comments(source[m.end(0):p-2])
        ddd['body'] = body
        m1 = _reStructFooter.search(source, p)
        if m1 == None:
            # notify error
            return _gParseTree
        m2 = _reStructFooter1.search(source,p,m1.end(0))
        if m2 != None and m2.start(0) < m1.start(0):
            m1 = m2
        pos = m1.end(0)
        ddd['name'] = m1.group(1)
        if ddd['type'] == 'struct':
            ddd['fields'] = list()
            #pos1 = 0
            #while True:
                #m = _reStructFiels.search(body,pos1)
                #if m == None:
                    #break

                    #print m.group(0)
                    #print
                    #print m.group(1)
                    #print m.group(2)
                    #print m.group(3)
                    #print m.group(4)
                    #print m.group(5)

                #check for recursive structures
                #name = _reRmSpaces.sub("",m.group(5))
                #if m.group(1) != None:

                    #ddd['recursive'] = True
                    #call structure parser
            recursive_stc_info = analyze_struct_body(body)

            #save result to dict
            #clear fields first
            ddd['fields'] = list()

            for keys in recursive_stc_info:
                if keys not in ddd:
                    ddd[keys] = list()
                ddd[keys] = recursive_stc_info[keys] 

            #if ddd['name'] == 'CPSS_DXCH_PCL_ACTION_REDIRECT_STC':
                #pprint(ddd) 
            #break

                #ddd['fields'].append(name)
                #pos1 = m.end(0)

        if ddd['type'] == 'enum':
            ddd['values'] = list()
            pos1 = 0
            body += ","
            while True:
                m = _reEnumValues.search(body,pos1)
                if m == None:
                    break
                name = _reRmSpaces.sub("",m.group(1))
                ddd['values'].append(name)
                pos1 = m.end(0)
        _gParseTree.append(ddd)
    return _gParseTree
