#!/usr/bin/env python
#
# cmdFileTransferClient.c
#
# DESCRIPTION:
#       File transfer client program
#
# Usage:
#    python cmdFileTransferClient.py [-v] [-q] [-l] [-u] [-z] [-d] [-o _name_] _board_ip_ [filename...]
#         -v        verbose
#         -q        quiet operations, check exit status
#         -l        list files
#         -p        push files (default mode)
#         -d        delete files on board
#         -g        get files from board
#         -o _name_ target file name
#         -z        upload file with compression
#
# Example:
#    python cmdFileTransferClient.py -z 127.0.0.1 cpssAPI.xml
#
#

import socket
import sys
import getopt

opt_verbose = False

def nvl(a,b):
    if a != None:
        return a
    return b

class FileTransferClient:
    NET_FLUSH_TIMEOUT = 1
    NET_WAITSTRING_TIMEOUT = 60
    CMD_FS_PORT = 12346

    def __init__(self, address):
        self.address = address
        self.connected = False
        self.verbose = False
        self.state = "ok"
        self.compress = False

    def cleanstate(self):
        self.state = "ok"

    def waitString(self,timeout):
        self.s.settimeout(timeout)
        self.buf = ""
        while True:
            try:
                chunk = self.s.recv(1)
            except socket.timeout:
                self.state = "timeout"
                return False
            if chunk == "\n":
                break
            self.buf += chunk
        if self.buf.endswith("\r"):
            self.buf = self.buf[:-1]
        return True

        
    def wait_ack(self, failmsg=None, errmsg=None):
        if not self.waitString(self.NET_WAITSTRING_TIMEOUT) or len(self.buf) < 3:
            if self.state == "ok":
                self.state = "badack"
            print nvl(failmsg, "failed to wait command ack")
            return False
        if self.verbose:
            print self.buf
        if self.buf[0] != "2":
            if not self.verbose:
                print nvl(errmsg,self.buf)
            self.state = "errack"
            return False
        return True

    def send_cmd(self,cmd):
        if self.verbose:
            print "sending command \""+cmd+"\""
        self.s.send(cmd+"\n")

    def connect(self):
        self.cleanstate()
        if self.connected:
            return True
        self.s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.s.connect((self.address, self.CMD_FS_PORT))
        if self.wait_ack(failmsg="failed to wait greeting message",errmsg="service is not ready"):
            self.connected = True
        else:
            self.s.close
            del self.s
        return self.connected

    def do_ls(self, path):
        self.cleanstate()
        if path == None:
            self.send_cmd("ls")
        else:
            self.send_cmd("ls "+path)
        if not self.wait_ack():
            return False
        self.listing = ""
        # responce should be 2xx-....
        if self.buf[3] != "-":
            if not self.verbose:
                print self.buf
            return True
        # save responce code
        tbuf = self.buf[:3]+" "
        # print till line startting with the same responce code
        # following by space
        while True:
            if not self.waitString(None):
                break
            if self.verbose:
                print self.buf
            if self.buf[0] != " ":
                if self.buf.startswith(tbuf):
                    # this is the last line of reply
                    self.listing += self.buf[4:]
                    return True
                else:
                    # line starts with wrong code, return error
                    if not self.verbose:
                        print self.buf
                    return False
            else:
                # delete leading space
                self.listing += self.buf[1:] + "\n"
        return False

    def do_upload(self,data,target_filename):
        self.cleanstate()
        cmd = "push"
        if self.compress:
            cmd = "pushz"
        self.send_cmd(cmd+" %d "%len(data)+target_filename)

        if not self.wait_ack():
            return False
        # board is ready to receive file, send it
        self.s.settimeout(None)
        self.s.send(data)

        if self.verbose:
            print "waiting final ack..."
        return self.wait_ack()

    def do_get(self,filename):
        self.cleanstate()
        self.send_cmd("get "+filename);
        if not self.wait_ack():
            return False
        # get file size from buf
        filesize = int(self.buf[4:].split()[0])
        self.s.settimeout(None)
        received = 0
        self.filedata=''
        while received < filesize:
            chunk = self.s.recv(filesize-received)
            self.filedata += chunk
            received += len(chunk)
        if self.verbose:
            print "waiting final ack..."
        return self.wait_ack()

    def do_delete(self,filename):
        self.cleanstate()
        self.send_cmd("del "+filename);
        return self.wait_ack()


def Usage():
    print "Usage: "+sys.argv[0]+ \
          " [-v] [-q] [-l] [-u] [-z] [-d] [-o _name_] _board_ip_ [filename...]"
    print "\t-v        verbose"
    print "\t-q        quiet operations, check exit status"
    print "\t-l        list files"
    print "\t-p        push files (default mode)"
    print "\t-d        delete files on board"
    print "\t-g        get files from board"
    print "\t-o _name_ target file name"
    print "\t-z        upload file with compression"
    sys.exit(0)

if __name__ == '__main__':
    mode = "upload"
    target_filename = None
    opts,rest = getopt.getopt(sys.argv[1:], "vqludgo:z?")
    cli = FileTransferClient(None)

    for (opt,val) in opts:
        if opt == '-v':
            cli.verbose = True
        elif opt == '-q':
            cli.quiet = True
        elif opt == '-l':
            mode = "list"
        elif opt == '-u':
            mode = "upload"
        elif opt == '-d':
            mode = "delete"
        elif opt == '-g':
            mode = "get"
        elif opt == '-o':
            target_filename = val
        elif opt == '-z':
            cli.compress = True
        elif opt == '-?':
            Usage()

    if len(rest) < 1:
        Usage()

    cli.address = rest[0]

    if not cli.connect():
        sys.exit(1)

    if mode == "list":
        cli.do_ls(None)
        if not cli.verbose:
            print cli.listing
        sys.exit(0)

    if mode == "delete":
        for fname in rest[1:]:
            cli.do_delete(fname)
        sys.exit(0)

    if mode == "upload":
        # read source file
        for fname in rest[1:]:
            f = open(fname, "rb")
            data = f.read()
            f.close()
            tname = fname
            if target_filename != None:
                tname = target_filename
            cli.do_upload(data, tname)
        sys.exit(0)

    if mode == "get":
        for fname in rest[1:]:
            if cli.do_get(fname):
                tname = fname
                if target_filename != None:
                    tname = target_filename
                f = open(tname, "wb")
                f.write(cli.filedata)
                f.close()
        sys.exit(0)
