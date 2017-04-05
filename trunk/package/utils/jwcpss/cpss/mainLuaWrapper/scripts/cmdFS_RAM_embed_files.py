#!/usr/bin/env python

######################################################################
# cmdFS_RAM_embed_files.py
#
# DESCRIPTION:
#       This script generate C include file to embed files into
#        cmdFS_RAM (reference script)
#       Usage:
#       cmdFS_RAM_embed_files.py [-o fname] list_file
#       cmdFS_RAM_embed_files.py [-o fname] [-x fname] -D dir
#       cmdFS_RAM_embed_files.py [-o fname] -F file_to_embed [file_to_embed]
#          -o fname    - output file. Default cmdFS_RAM_embedded_data.inc
#          -x fname    - exclude list file
#          -D          - the argument is a directory. All files from this
#                        directory will be embedded
#          -F          - The argument(s) are file names. These files
#                        will be embedded into cmdFS_RAM root directory
#          -S size     - the size of single file
#          -z ext      - compress files with given extension
#          -E suffix   - use suffix for CMDFS_RAM_INIT_EMBEDDED()
#          -I prefix   - only files which prefix match one of listed
#          -X prefix   - exclude files which prefix match one of listed
#
#
# INPUTS:
#       listing file -  file with two columns:
#            1. host file name (relative or absolute)
#            2. cmdFS file name (use / as path sep)
#
#            # Sample file list for embedding into cmdFS_RAM
#            #
#            # source file name                              target name
#            # ----------------                              -----------
#            ../lua/luaCLI/scripts/cmdLuaCLIDefs.lua         cmdLuaCLIDefs.lua
#            ../lua/luaCLI/scripts/genwrapper_API.lua        genwrapper_API.lua
#            ../lua/luaCLI/sample/bit_wise_test.lua          test/bit_wise.lua
#
#       exclude list file - the list of cmdFS file names to be excluded
#            # Sample exclude file list for embedding into cmdFS_RAM
#            #
#            # target name
#            # -----------
#            test/bit_wise.lua
#
# OUTPUTS:
#       cmdFS_RAM_embed_files.inc
#
# RETURNS:
#       none
#
#
# EXAMPLES:
#      - under Windows
#         cd X:\cpss\SW\prestera\mainLuaWrapper\data\dxCh
#         cmdFS_RAM_embed_files.py -o cmdFS_RAM_embed_files.c -z .lua -z .xml -S 1000000 -X examples -D X:\cpss\SW\prestera\lua_cli\scripts\
#
# COMMENTS:
#
# FILE REVISION NUMBER:
#       $Revision: $
#####################################################################

import os,sys,getopt,time,zlib

def Usage():
    print "Usage:"
    print sys.argv[0]+" [-o fname] list_file"
    print sys.argv[0]+" [-o fname] [-x fname] -D dir"
    print sys.argv[0]+" [-o fname] [-x fname] -F file_to_embed [file_to_embed]"
    print "   -o fname    - output file. Default cmdFS_RAM_embedded_data.inc"
    print "   -x fname    - exclude list file"
    print "   -D          - the argument is a directory. All files from this"
    print "                 directory will be embedded (The directory path relative to dir)"
    print "   -F          - The argument(s) are file names. These files"
    print "                 will be embedded into cmdFS_RAM root directory"
    print "   -S size     - the size of single file"
    print "   -z ext      - compress files with given extension"
    print "   -E suffix   - use suffix for CMDFS_RAM_INIT_EMBEDDED()"
    print "   -I prefix   - only files which prefix match one of listed"
    print "   -X prefix   - exclude files which prefix match one of listed"
    sys.exit(0)


if __name__ == '__main__':
    outfile_name = "cmdFS_RAM_embed_files.inc"
    exclude_list = list()
    compress_ext = list()
    embed_entire_directory = False
    embed_files = False
    prefix_include = list()
    prefix_exclude = list()
    use_prefix_include = False
    use_prefix_exclude = False
    size_limit = None
    total_size = 0
    total_size_uncompressed = 0
    init_function = "CMDFS_RAM_INIT_EMBEDDED"
    opts, files = getopt.getopt(sys.argv[1:], "?o:x:DS:z:E:FI:X:")
    for (opt,val) in opts:
        if opt == "-o":
            outfile_name = val
        elif opt == "-x":
            if os.access(val, os.R_OK):
                f = open(val,"r")
                for line in f.readlines():
                    if line.startswith("#"):
                        continue
                    exclude_list.append(line.split()[0])
                f.close()
            else:
                print >>sys.stderr, "exclude list file \""+val+"\" doesn't exists or not readable"
                sys.exit(1)
        elif opt == "-D":
            embed_entire_directory = True
        elif opt == "-F":
            embed_files = True
        elif opt == "-S":
            size_limit = int(val)
        elif opt == "-z":
            compress_ext.append(val)
        elif opt == "-E":
            init_function += val
        elif opt == "-I":
            use_prefix_include = True
            prefix_include.append(val)
        elif opt == "-X":
            use_prefix_exclude = True
            prefix_exclude.append(val)
        else:
            Usage()

    if len(files) < 1:
        Usage()

    # use .c if size_limit
    if size_limit != None:
        if outfile_name.endswith(".inc"):
            outfile_name = outfile_name[:-3]+"c"
        elif not outfile_name.endswith(".c"):
            print >>sys.stderr, "Output file must have extension .c"
            sys.exit(1)

    if embed_entire_directory:
        list_lines = list()
        for srcdir in files:
            if srcdir.endswith(os.path.sep):
                srcdir = srcdir[:-len(os.path.sep)]
            for root, dirs, files in os.walk(srcdir):
                for filename in files:
                    sourcename = os.path.join(root,filename)
                    targetname = sourcename
                    if targetname.startswith(srcdir+os.path.sep):
                        targetname = targetname[len(srcdir+os.path.sep):]
                    if os.path.sep != '/':
                        targetname = targetname.replace(os.path.sep, '/')
                    if use_prefix_include and not targetname.startswith(tuple(prefix_include)):
                        continue
                    if use_prefix_exclude and targetname.startswith(tuple(prefix_exclude)):
                        continue
                    list_lines.append(sourcename + " " + targetname);
    elif embed_files:
        list_lines = list()
        for nm in files:
            list_lines.append(nm + " " + os.path.basename(nm))
        print list_lines
    else:
        # use list file
        if files[0] == "-":
            f = sys.stdin
        else:
            if not os.access(files[0], os.R_OK):
                print >>sys.stderr, "list file \""+files[0]+"\" doesn't exists or not readable"
                sys.exit(1)
            f = open(files[0],"r")
        list_lines = f.readlines()
        f.close()

    # create output file
    def open_output(name):
        global of, lst, listing,cont
        of = open(name,"w")
        print "opened output file "+name+"..."
        print >>of, "/* this file is generated at "+time.ctime()
        print >>of, " * script parameters:"
        print >>of, " *    "+" \\\n *        ".join(sys.argv)
        print >>of, " */"
        print >>of, ""
        if size_limit != None:
            print >>of,"#define CMDFS_RAM_TYPES_ONLY"
            print >>of,"#include \"cmdFS_RAM.h\""
            print >>of, ""
        cont = None
        lst = list()
        listing = ""
    open_output(outfile_name)

    n = 1
    if size_limit == None:
        funcname = "static void "+init_function
    else:
        funcname = "void "+init_function
        next_file_index = 1
    def output_file_flush(next_c = None,next_chunk_follow = None):
        global of, lst, listing, funcname,cont
        # create macro to register files
        if next_c != None:
            if next_chunk_follow != True:
                print >>of, "void "+next_c+"(void);"
            else:
                print >>of, "void "+next_c+"(CMD_FS_CHUNK_STC* prev_chunk);"
        if cont == None:
            print >>of, funcname + "(void)"
        else:
            print >>of, funcname + "(CMD_FS_CHUNK_STC* prev_chunk)"
        print >>of, "{"
        if len(lst) > 0:
            print >>of, "    CMD_FS_INODE_STC* inode;"
        if cont != None:
            print >>of, "    prev_chunk->next = (CMD_FS_CHUNK_STC*)&"+cont+";"
        for i in lst:
            print >>of, "    inode = cmdFS_RAM_newFileEntry(\""+i["filename"].encode("string-escape")+"\");"
            print >>of, "    inode->nrefs++;"
            print >>of, "    inode->mtime = "+"%dUL"%i["mtime"]+";"
            print >>of, "    inode->data = (CMD_FS_CHUNK_STC*)&"+i["dataname"]+";"
            if i["compr"]:
                print >>of, "    inode->flags |= CMD_FS_FLAGS_COMPRESSED;"
                print >>of, "    inode->size = %d;"%i["datasize"]
        if next_c != None:
            if next_chunk_follow == None:
                print >>of, "    "+next_c+"();"
            elif len(lst) > 0:
                print >>of, "    "+next_c+"(inode->data);"
            else:
                print >>of, "    "+next_c+"((CMD_FS_CHUNK_STC*)&"+cont+");"
        print >>of, "}"
        print >>of, ""
        print >>of, "/* File list:"
        print >>of, listing,
        print >>of, "*/"
        of.close()
    # iteract through list file
    list_lines.sort()
    for line in list_lines:
        line = line.strip()
        if line.startswith("#") or line == "":
            continue
        a = line.split()
        if len(a) < 2:
            continue
        if a[1] in exclude_list:
            continue
        # read source
        if not os.access(a[0], os.R_OK):
            print >>sys.stderr, "file \""+a[0]+"\" doesn't exists or not readable"
            continue

        # open next file
        data_compressed = False
        for ext in compress_ext:
            if a[0].endswith(ext):
                data_compressed = True
        f = open(a[0],"rb")
        data = f.read()
        f.close()
        data_size = len(data)
        total_size_uncompressed += data_size
        if data_compressed:
            data = zlib.compress(data,9)
        total_size += len(data)
        # check if we need to flush
        if size_limit != None:
            need_flush = False
            if of.tell() > size_limit:
                need_flush = True
            if len(lst) > 0 and of.tell() + len(data)*5 > size_limit * 1.2:
                need_flush = True
            if need_flush:
                output_file_flush(init_function+"%d"%next_file_index)
                open_output(outfile_name[:-2]+"%d.c"%next_file_index)
                funcname = "void "+init_function+"%d"%next_file_index
                next_file_index = next_file_index + 1

        # add dump
        print "embed file "+a[0]+" with name \""+a[1]+"\""
        def write_chunk(comment,data):
            global n,of
            data_name = "emb_file_%d"%n
            n = n + 1
            print >>of, "/* "+comment+" */"
            print >>of, "static struct {"
            print >>of, "    size_t chunksize;"
            print >>of, "    void* next;"
            print >>of, "    unsigned char data[%d];"%len(data)
            print >>of, "} "+data_name+" = { %d, NULL, {"%len(data)
            # data -> hex array
            s = ""
            for i in range(len(data)):
                s += "0x%02x"%ord(data[i])
                if i+1 < len(data):
                    s += ","
                if len(s) > 79:
                    print >>of, "    "+s
                    s = ""
            if s != "":
                print >>of, "    "+s
            # end of data -> hexarray
            print >>of, "}};"
            print >>of, ""

            return data_name

        need_split = False
        if size_limit != None:
            if len(data)*5 > size_limit * 1.2:
                need_split = True
        if not need_split:
            data_name = write_chunk(a[1], data)
            item = {
                "dataname" : data_name,
                "compr"    : data_compressed,
                "datasize" : data_size,
                "filename" : a[1],
                "mtime"    : os.stat(a[0]).st_mtime }
            lst.append(item)
            # add line to listing
            listing += time.ctime(item["mtime"])+" %10d "%len(data)+a[1]+"\n"
        else:
            First_chunk = True
            chunk_size = size_limit / 5
            while len(data) > chunk_size:
                comment = a[1]
                if not First_chunk:
                    comment += " (continue)"
                data_name = write_chunk(comment,data[:chunk_size])
                if First_chunk:
                    item = {
                        "dataname" : data_name,
                        "compr"    : data_compressed,
                        "datasize" : data_size,
                        "filename" : a[1],
                        "mtime"    : os.stat(a[0]).st_mtime }
                    lst.append(item)
                    # add line to listing
                    listing += time.ctime(item["mtime"])+" %10d "%len(data)+a[1]+"\n"
                else:
                    cont = data_name
                    listing += a[1]+" (continue)\n"
                data = data[chunk_size:]
                output_file_flush(init_function+"%d"%next_file_index,True)
                open_output(outfile_name[:-2]+"%d.c"%next_file_index)
                funcname = "void "+init_function+"%d"%next_file_index
                next_file_index = next_file_index + 1
                First_chunk = False
            data_name = write_chunk(a[1]+" (end)",data)
            cont = data_name
            listing += a[1]+" (end)\n"






    # create macro to register files
    listing += "\n total        %d\n"%total_size
    listing += " uncompressed %d\n"%total_size_uncompressed
    print ""
    print "total        %d"%total_size
    print "uncompressed %d"%total_size_uncompressed
    output_file_flush()



