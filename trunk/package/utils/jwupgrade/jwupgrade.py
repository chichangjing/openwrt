#!/usr/bin/env python
#-*- coding: utf-8 -*-
#import sys
import time
import struct 
from ctypes import create_string_buffer  
import socket
import thread
import fcntl
import uuid
import os
from xml.dom import minidom
import tarfile
import string
import subprocess
import copy
import serial
import select
import getuci
import tunnelclient

cmd_table = {0x20001:'login', 0xf000001:'file_open', 0xf000002:'file_close', 0xf000004:'file_write', \
        0x9b:'upgrade_check', 0x98:'upgrade', 0x1dd00:'discovery_hello', 0x1dd01:'discovery_state', \
        0x1dd02:'discovery_setparam', 0x00:'system_hello', 0x95:'export', 0x96:'import', \
        0x97:'restore',0x93:'reboot',0x1000001:'tunnel_open'}

header_table = {'verson':0, 'product':0, 'sequence':0, 'command':0}
upstate_header_table = {'verson':0, 'product':0, 'sequence':0, 'command':0}
multi_header_table = {'verson':0, 'product':0, 'sequence':0, 'command':0}
fp_table = {}

g_thread_lock = thread.allocate()

tunnel_num = 64
readputs = []

#tunnel session data
tunnelSession = {'sessionID':0,'tunnelID':0, 'protocol':0, 'clientIp':0, 'clientPort':0,
                 'link':0, 'dock':0, 'sock':0, 'serialFd':0, 'readFdSet':readputs,
                  'state':0}
g_tunnelSession = [copy.deepcopy(tunnelSession) for i in range(tunnel_num)]
g_channelFlag = [0 for i in range(tunnel_num)]
g_ID = [0 for i in range(tunnel_num)]
g_tunnelSessionCount = 0

#login session node
UM_SESSION_NUM = 64
loginSession = {'used':0,'sockfd':0}
g_loginSession = [copy.deepcopy(loginSession) for i in range(UM_SESSION_NUM)]

#串口信息
serial_path_name = ['/dev/ttymxc2', '/dev/ttymxc3', '/dev/ttymxc4', '/dev/ttymxc6']
g_serialFd = [0 for i in range(len(serial_path_name))]

#stream state
STREAM_STATE_IDLE,STREAM_STATE_READY,STREAM_STATE_START,STREAM_STATE_STOP,STREAM_STATE_CLOSE = range(5)

tunnel_header_table = {'mark':0xBF9D1FDB, 'type':0, 'channel':0, 'device':0}


def uart_init():
    for i in range(len(serial_path_name)):
        g_serialFd[i] = serial.Serial(port=serial_path_name[i], baudrate=115200, bytesize=8, parity='N', stopbits=1, xonxoff=False)

def get_ip_address(ifname):
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    return socket.inet_ntoa(fcntl.ioctl(s.fileno(),0x8915, struct.pack('256s', ifname[:15]))[20:24])


def get_file_size(f):
    fs = os.stat(f)
    return fs.st_size

#sick of me
def get_mac_address(): 
    mac=uuid.UUID(int = uuid.getnode()).hex[-12:] 
    mac_h = 0x00
    mac_l = 0x00
    for m in range(0, 3, 2):
        mac_h = (mac_h << 8) | int(mac[m:m+2], 16)
    for n in range(4, 11, 2):
        mac_l = (mac_l << 8) | int(mac[n:n+2], 16)
    mac_s = ":".join([mac[e:e+2] for e in range(0,11,2)])
    return mac_s, mac_h, mac_l


def get_product_info():
    output = subprocess.check_output(['jwboardinfo'])
    output_line = output.split('\n')

    for line in output_line:
        line_sp = line.split('=')
        try:
            key = string.strip(line_sp[0])
            value = string.strip(line_sp[1])
            print key,value
        except:
            break

        if key == 'uid':
            uid = value
        elif key == 'product_type':
            ptype = value
        elif key == 'hardware_version':
            hver = value

    print uid, ptype, hver

    verfile = open('/etc/openwrt_version', 'r')
    version = verfile.read()
 #   print version
    verfile.close()

    return (ptype, uid, hver, version)

netdevice_name='br-lan'
#local_ip = get_ip_address(netdevice_name)
local_ip = '0.0.0.0'
local_mac, mac_h, mac_l = get_mac_address()
local_port = 3721
multicast_group = '228.228.228.228'
local_mask = '255.255.255.0'
product_info = get_product_info()


def send_with_header(conn, data, length=0, err=0):
    header_fmt = "ccbbihhiii40x"
    header_len = struct.calcsize(header_fmt);

    buf = create_string_buffer(header_len + len(data))
    if length == 0:
        data_len = 56 + len(data)
    else:
        data_len = length + 56

    struct.pack_into(header_fmt, buf, 0, 'O', 'B', 10, 0, data_len, 2, err, 0, header_table['sequence'], header_table['command'])

    buf[64:] = data
    try:
        conn.send(buf)
    except socket.error, e:
        print socket.error
        pass
"""
def send_with_error(conn,  error):
    header_fmt = "ccbbihhiii40x"
    header_len = struct.calcsize(header_fmt);

    buf = create_string_buffer(header_len)

    struct.pack_into(header_fmt, buf, 0, 'O', 'B', 10, 0, 56, 2, error, 0, header_table['sequence'], header_table['command'])

    try:
        conn.send(buf)
    except socket.error, e:
        print socket.error
        pass
"""
def send_with_header_upstate(conn, data, length=0, err=0):
    header_fmt = "ccbbihhiii40x"
    header_len = struct.calcsize(header_fmt);

    buf = create_string_buffer(header_len + len(data))
    if length == 0:
        data_len = 56 + len(data)
    else:
        data_len = length + 56

    struct.pack_into(header_fmt, buf, 0, 'O', 'B', 10, 0, data_len, 2, err, 0, upstate_header_table['sequence'], upstate_header_table['command'])

    buf[64:] = data
    try:
        conn.send(buf)
    except socket.error, e:
        print socket.error
        pass


def unsupport_reply(conn, data):
    try:
        conn.recv(1024)
    except socket.error, e:
        print socket.error
        pass

    reply_fmt = 'I'
    reply_buf = create_string_buffer(struct.calcsize(reply_fmt))
    struct.pack_into(reply_fmt, reply_buf, 0, 0)

    send_with_header(conn, reply_buf)


def reply_multicast(conn, data, peer):
    header_fmt = "ccBBihhiii40x"
    header_len = struct.calcsize(header_fmt);

    buf = create_string_buffer(header_len + len(data))
    struct.pack_into(header_fmt, buf, 0, 'O', 'B', 10, 0, 56 + len(data), 2, 0, 0, multi_header_table['sequence'], multi_header_table['command'])

    buf[64:] = data
    try:
        conn.sendto(buf, (multicast_group, local_port))
    except:
        print 'reply multicast fail:'


def system_get_version(conn, data):
    reply_fmt = 'I32s32s32sII'
    product = 0x0000000e
    module = product_info[0]
    serial = product_info[1]
    hardware_ver = product_info[2]

    reply_buf = create_string_buffer(struct.calcsize(reply_fmt))
    struct.pack_into(reply_fmt, reply_buf, 0, product, module, hardware_ver, serial, socket.ntohl(mac_h), socket.ntohl(mac_l))

    send_with_header(conn, reply_buf)


def system_set_version(conn, data):
    reply_fmt = 'I32s32s32sII'
    product = 0x0000000e
    module = product_info[0]
    serial = product_info[1]
    hardware_ver = product_info[2]
    
    product, module, hardware_ver, serial, mac_h, mac_l = struct.unpack_from('I32s32s32sII', data, 0);
    #print (product, module, hardware_ver, serial, mac_h, mac_l)
    #print hex(mac_h), hex(mac_l)
    mac_h = socket.ntohl(mac_h)
    mac_l = socket.ntohl(mac_l)

    mac_s = '%08x%08x' %(mac_h, mac_l)
    mac_s_fmt = ":".join([mac_s[e:e+2] for e in range(4, 15, 2)])

    g_thread_lock.acquire()
    os.system('jw_setmac %s' %mac_s_fmt)

    reply_fmt = 'i'
    reply_buf = create_string_buffer(struct.calcsize(reply_fmt))
    struct.pack_into(reply_fmt, reply_buf, 0, 0x0)

    send_with_header(conn, reply_buf)
    if local_mac != mac_s:
        os.system('reboot -f')
    else:
        print 'mac not change'
    g_thread_lock.release()


def discovery_hello(conn, data, peer):
    reply_fmt = 'IIIBBHIII32s32s32s132s'
    product = 0x0000000e
    ver = 10
    crypt = 1
    port = local_port
    addr = socket.ntohl(struct.unpack('I', socket.inet_aton(get_ip_address(netdevice_name)))[0])
    netmask = socket.ntohl(struct.unpack('I', socket.inet_aton(local_mask))[0])
    gateway = socket.ntohl(struct.unpack('I', socket.inet_aton(get_ip_address(netdevice_name)))[0]) & 0xffffff00 | 0x01
    module = product_info[0]
    serial = product_info[1]
    version = product_info[3]

    reply_buf = create_string_buffer(struct.calcsize(reply_fmt))
    struct.pack_into(reply_fmt, reply_buf, 0, product, socket.ntohl(mac_h), socket.ntohl(mac_l), ver, crypt, port, addr,\
            netmask, gateway, module, version, serial, '\0')

    reply_multicast(conn, reply_buf, peer)


def discovery_modinfo(conn, data, peer):
    mac = data[66:72]
    device_type, set_type, user, passwd, ip, mask, gate = struct.unpack_from('HH16s32sIII', data, 8 + 64);

    mac_s = ":".join(['%02x'%(ord(mac[n])) for n in range(0,6,1)])
    #print mac_s

    ip_str = socket.inet_ntoa(struct.pack('I', ip))
    mask_str = socket.inet_ntoa(struct.pack('I', mask))
    gate_str = socket.inet_ntoa(struct.pack('I', gate))

    reply_fmt = 'i'
    reply_buf = create_string_buffer(struct.calcsize(reply_fmt))

    if set_type != 1 or mac_s != local_mac:
        struct.pack_into(reply_fmt, reply_buf, 0, -1)
        reply_multicast(conn, reply_buf, peer)
        return

    os.system('uci set network.lan.ipaddr=%s'%ip_str)
    os.system('uci set network.lan.netmask=%s'%mask_str)
    os.system('uci set network.lan.gateway=%s'%gate_str)
    os.system('uci commit')
    os.system('/etc/init.d/network stop')
    os.system('/etc/init.d/network start')                                                           
    os.system('reboot -f')

def system_hello(conn, data):
    reply_fmt = 'IIIBBHI'
    product = 0x00000011
    ver = 10
    crypt = 1
    port = 3721
    addr = socket.ntohl(struct.unpack('I', socket.inet_aton(get_ip_address(netdevice_name)))[0])

    reply_buf = create_string_buffer(struct.calcsize(reply_fmt))
    struct.pack_into(reply_fmt, reply_buf, 0, product, socket.ntohl(mac_h), socket.ntohl(mac_l), ver, crypt, port, addr)

    send_with_header(conn, reply_buf)


def login_process(conn, data):
    user, passwd, actor = struct.unpack_from('16s32si', data, 0);
    print (user, passwd, actor)
    sessionidx = -1

    #TODO: check user and passwd
    for i in range(3, UM_SESSION_NUM):
        if g_loginSession[i]['used'] == 0:
            sessionidx = i 
            break
    if sessionidx < 0:
        print 'umSessionLogin reach the login user limit'
        return

    g_loginSession[sessionidx]['used'] = 1
    g_loginSession[sessionidx]['sockfd'] = conn
    role = 'switch'

    login_reply_fmt = 'i16s'
    login_reply_len = struct.calcsize(login_reply_fmt)
    reply_buf = create_string_buffer(login_reply_len)
    struct.pack_into(login_reply_fmt, reply_buf, 0, sessionidx, role)
    
    send_with_header(conn, reply_buf)

def logout_process(conn, data):
    session = struct.unpack_from('I', data, 0);

    sessionidx = session[0]

    reply_buf = ''

    send_with_header(conn, reply_buf)
     
 #   conn.close()    
    g_loginSession[sessionidx]['used'] = 0
    g_loginSession[sessionidx]['sockfd'] = 0


def tunnel_get_idle_stream():
    for i in range(tunnel_num):
        if g_tunnelSession[i]['state'] == 0:
            tunnel = g_tunnelSession[i]
            return tunnel
    print 'all tunnel is going'
    return
#print tunnel_get_stream()
def tunnel_get_pession_by_id(tunnelId):
    for i in range(tunnel_num):
        if g_tunnelSession[i]['tunnelID'] == tunnelId:
            tunnel = g_tunnelSession[i]
            return tunnel
    print 'this tunnel has no exit'
    return

def tunnel_get_pession_by_sock(conn):
    for i in range(tunnel_num):
        if g_tunnelSession[i]['sock'] == conn:
            tunnel = g_tunnelSession[i]
            return tunnel
    print 'this tunnel has no exit'
    return

def tunnel_get_idle_id():
    for i in range(tunnel_num):
        if g_ID[i] == 0:
            id = i + 1
            return id
    print 'all tunnel is going'
    return 0

def tunnel_get_sessionid(conn):
    for i in range(UM_SESSION_NUM):
        if g_loginSession[i]['sockfd'] == conn:
            return i
    print 'this session has no exit'
    return 0

def tunnel_get_serialFd(dock):
    return g_serialFd[dock-1]

def tunnel_open_process(conn, data):
    link,dock,res,protocol,ip,port = struct.unpack_from('BB2sIII', data, 0);
    print (link, dock, ip,port)
        
    global g_tunnelSessionCount

    #TODO: check the same tunnel
    if protocol != 3:
        if g_channelFlag[dock -1 ] == dock:
            print 'the tunnel is exit'
            send_with_error(conn, 1) 
            return -1
        g_channelFlag[dock - 1] = dock

    #TODO: get idle tunnel session
    psession = tunnel_get_idle_stream()
    if psession:
        psession['sessionID'] = tunnel_get_sessionid(conn)
        psession['protocol'] = protocol
        psession['link'] = link
        psession['dock'] = dock
        psession['tunnelID'] = tunnel_get_idle_id()
        g_ID[psession['tunnelID']-1] = 1
        g_tunnelSessionCount += 1
        psession['serialFd'] = tunnel_get_serialFd(dock)
        
    addr = socket.ntohl(struct.unpack('I', socket.inet_aton(local_ip))[0])
    #addr = socket.ntohl(struct.unpack('I', socket.inet_aton(ip))[0])
    rcvPort = 3720

    tunnel_reply_fmt = 'IIBB2sIII'
    tunnel_reply_len = struct.calcsize(tunnel_reply_fmt)
    reply_buf = create_string_buffer(tunnel_reply_len)
    struct.pack_into(tunnel_reply_fmt, reply_buf, 0, psession['sessionID'], psession['tunnelID'],psession['link'],psession['dock'],res,protocol,addr,rcvPort)

    send_with_header(conn, reply_buf)

def tunnel_free_stream(psession):
    if psession['sock'] > 0:
        psession['sock'].close()
        psession['sock'] = 0
    psession['dock'] = 0
    psession['link'] = 0
    psession['protocol'] = 0
    psession['sessionID'] = 0
    psession['tunnelID'] = 0
    psession['serialFd'] = 0
    psession['state'] = STREAM_STATE_CLOSE
    psession['clientIp'] = 0
    psession['clientPort'] = 0
    psession['readFdSet'] = []


def tunnel_close_process(conn,data):
    tunnelid = struct.unpack_from('I', data, 0);
    tunnelId = tunnelid[0]
    print (tunnelId)
    
    global g_tunnelSessionCount

    #TODO: get pession and change state
    psession = tunnel_get_pession_by_id(tunnelId)
    if(psession):
        g_channelFlag[psession['dock'] - 1] = 0
        g_ID[psession['tunnelID']-1] = 0

        if psession['state'] != STREAM_STATE_CLOSE:
            tunnel_free_stream(psession)
        g_tunnelSessionCount -= 1

        if psession['state'] == STREAM_STATE_CLOSE:
            psession['state'] = STREAM_STATE_IDLE

    stop_reply_fmt = 'i'
    stop_reply_len = struct.calcsize(stop_reply_fmt)
    reply_buf = create_string_buffer(stop_reply_len)
    struct.pack_into(stop_reply_fmt, reply_buf, 0, tunnelId)

    send_with_header(conn, reply_buf)


def tunnel_list_process(conn, data):

    srcAddr = socket.ntohl(struct.unpack('I', socket.inet_aton(local_ip))[0])
    srcPort = 3720
    time = 0
    res = ''
    
    count = g_tunnelSessionCount
    tunnel_reply_fmt = 'I'
    tunnel_reply_len = struct.calcsize(tunnel_reply_fmt)

    tunnel_struct_fmt = 'IIBB2sIIIIIII'
    tunnel_struct_len = struct.calcsize(tunnel_struct_fmt)

    reply_buf = create_string_buffer(tunnel_reply_len + tunnel_struct_len * count)

    struct.pack_into(tunnel_reply_fmt, reply_buf, 0,count)
    for i in range(count): 
        struct.pack_into(tunnel_struct_fmt, reply_buf, tunnel_reply_len + tunnel_struct_len * i, g_tunnelSession[i]['tunnelID'],g_tunnelSession[i]['sessionID'],\
                g_tunnelSession[i]['link'],g_tunnelSession[i]['dock'],res,g_tunnelSession[i]['protocol'],srcAddr, srcPort,\
                g_tunnelSession[i]['clientIp'],g_tunnelSession[i]['clientPort'],g_tunnelSession[i]['state'],time)
    
    send_with_header(conn, reply_buf)

def tunnel_stream_start(tunnelId, sock, addr):
    pession = tunnel_get_pession_by_id(tunnelId)
    if(pession):
        if sock > 0:
            pession['sock'] = sock
        if addr:
            pession['clientIp']   =socket.ntohl(struct.unpack('I', socket.inet_aton(addr[0]))[0])
            pession['clientPort'] = addr[1]
        pession['state'] = STREAM_STATE_START

def tunnel_start_process(conn, data):
    tunnelid = struct.unpack_from('I', data, 0);
    tunnelId = tunnelid[0]
    print (tunnelId)

    #TODO: get pession and change state
    tunnel_stream_start(tunnelId,-1,'')
#    pession = tunnel_get_pession_by_id(tunnelId)
#    if(pession):
#        pession['state'] = STREAM_STATE_START

    start_reply_fmt = 'i'
    start_reply_len = struct.calcsize(start_reply_fmt)
    reply_buf = create_string_buffer(start_reply_len)
    struct.pack_into(start_reply_fmt, reply_buf, 0, tunnelId)

    send_with_header(conn, reply_buf)

def tunnel_stop_process(conn, data):
    tunnelid = struct.unpack_from('I', data, 0);
    tunnelId = tunnelid[0]
    print (tunnelId)

    #TODO: get pession and change state
    psession = tunnel_get_pession_by_id(tunnelId)
    if(psession):
        psession['state'] = STREAM_STATE_STOP

    stop_reply_fmt = 'i'
    stop_reply_len = struct.calcsize(stop_reply_fmt)
    reply_buf = create_string_buffer(stop_reply_len)
    struct.pack_into(stop_reply_fmt, reply_buf, 0, tunnelId)

    send_with_header(conn, reply_buf)



def file_read_process(conn, data):
    fwrite_fmt = 'ii'
    fid, length = struct.unpack_from(fwrite_fmt, data, 0)

    g_thread_lock.acquire()
    fp = fp_table[fid]
    
    sbuf = bytearray(length)

    data = fp.read(length)
    sbuf[:len(data)] = data
    g_thread_lock.release()

    reply_fmt = 'ii'
    reply_buf = create_string_buffer(struct.calcsize(reply_fmt))
    struct.pack_into(reply_fmt, reply_buf, 0, 65, len(data))

    send_with_header(conn, reply_buf, length + 8)
    conn.sendall(sbuf)


def file_write_process(conn, data):
    fwrite_fmt = 'iix'
    fid, length = struct.unpack_from(fwrite_fmt, data, 0)

    g_thread_lock.acquire()
    fp = fp_table[fid]
    #print length
    #print len(data)

    if length < (len(data) - 8):
        remain_size = 0
        fp.write(data[8:length+8])
        null_size = 131080 - len(data)
        #print 'null_size %s' %null_size
    else:
        remain_size = length - len(data) + 8
        fp.write(data[8:])
        null_size = 131072 - length

    while remain_size:
        if remain_size > 1024:
            size = 1024
        else:
            size = remain_size

        try:
            fslice = conn.recv(size)
        except:
            return

        fp.write(fslice)
        remain_size = remain_size - len(fslice)
    fp.flush()
    g_thread_lock.release()

    fwrite_reply_fmt = 'ii'
    reply_buf = create_string_buffer(struct.calcsize(fwrite_reply_fmt))
    struct.pack_into(fwrite_reply_fmt, reply_buf, 0, 65, length)

    #TODO: here is not my fault, client do things not right
    while null_size:
        if null_size > 1024:
            size = 1024
        else:
            size = null_size
        null = conn.recv(size)
        null_size = null_size - len(null)

    send_with_header(conn, reply_buf)
    

def file_close_process(conn, data):
    fid = struct.unpack_from('i', data, 0)[0]

    g_thread_lock.acquire()
    fp = fp_table[fid]
    fp.close()
    g_thread_lock.release()

    fclose_reply_fmt = 'i'
    reply_buf = create_string_buffer(struct.calcsize(fclose_reply_fmt))
    struct.pack_into(fclose_reply_fmt, reply_buf, 0, fid)

    send_with_header(conn, reply_buf)


def file_open_process(conn, data):
    schema, path, mode, size= struct.unpack_from('i128sII', data, 0)
    path = path.rstrip('\0')

    g_thread_lock.acquire()
    fp = open(path, 'a+')
    fp_table[65] = fp
    
    file_size = get_file_size(path)
    g_thread_lock.release()

    fopen_reply_fmt = 'ii'
    reply_buf = create_string_buffer(struct.calcsize(fopen_reply_fmt))
    #FIXME: now fileId and size fixed 1, 0
    struct.pack_into(fopen_reply_fmt, reply_buf, 0, 65, file_size)

    send_with_header(conn, reply_buf)


def upgrade_check(conn, data):
    path = struct.unpack_from('128s', data, 0)[0]
    path = path.rstrip('\0')

    uc_reply_fmt = 'i'
    reply_buf = create_string_buffer(struct.calcsize(uc_reply_fmt))
    struct.pack_into(uc_reply_fmt, reply_buf, 0, 0)
    #TODO: check head part of config file is complete
    #      or maybe could check system version also
    #      now error info loged ing common log file

    send_with_header(conn, reply_buf)

def upgrade_clean(path):
    print path
    os.system("rm -rf %s" %path)
    os.system("rm -rf /tmp/images")

def do_upgrade(conn, data):
    path = struct.unpack_from('128s', data, 0)[0]
    path = path.rstrip('\0')

    up_reply_fmt = 'i'
    reply_buf = create_string_buffer(struct.calcsize(up_reply_fmt))
    struct.pack_into(up_reply_fmt, reply_buf, 0, 0x00)

    g_thread_lock.acquire()
    # 1, split file into header + tar
    # 2, uncompress tar, get serveral file
    # 3, verify image version, md5 etc.
    cut_header_cmd = 'dd if=%s of=/tmp/p1 bs=1k count=4'%path
    cut_body_cmd = 'dd if=%s of=/tmp/p2 bs=1k skip=4'%path
    os.system(cut_header_cmd)
    os.system(cut_body_cmd)

    fp = open('/tmp/p1', 'r')
    fstr = fp.read(4096)
    fstr = fstr.rstrip('\0')
    wfp = open('/tmp/header.xml', 'w+')
    wfp.write(fstr)
    fp.close()
    wfp.close()

    tar = tarfile.open('/tmp/p2')
    names = tar.getnames()
    for na in names:
        tar.extract(na, path='/tmp/images')

    #parse xml and check
    doc = minidom.parse('/tmp/header.xml')
    root_node = doc.documentElement
    child_nodes = root_node.childNodes

    size_node = root_node.getElementsByTagName('size')
    p2_size_xml = int(size_node[0].childNodes[0].nodeValue)
    p2_size_real = int(get_file_size('/tmp/p2'))

    sid_node = root_node.getElementsByTagName('sid')
    sid_xml = sid_node[0].childNodes[0].nodeValue
    sid_real = product_info[1] 
    if sid_xml.upper() != sid_real.upper():
        upgrade_clean(path)
        struct.pack_into(up_reply_fmt, reply_buf, 0, 0x01)
        send_with_header(conn, reply_buf, 0x00, 0x01)
        print 'sid not match'
        g_thread_lock.release()
        return
    
    if p2_size_xml != p2_size_real:
        upgrade_clean(path)
        struct.pack_into(up_reply_fmt, reply_buf, 0, 0x01)
        send_with_header(conn, reply_buf, 0x00, 0x01)
        print 'upgrade failed'
        g_thread_lock.release()
        return 

    first_child_node =  root_node.firstChild
    #print first_child_node.nodeName
    nt = ('uboot', 'dtb', 'kernel', 'rootfs')
    nd = {}
    for n in child_nodes:
        if n.nodeName in nt:
            name = n.nodeName
            nd[n.nodeName] = {}
            sub_children = n.childNodes
            for s in sub_children:
                nd[n.nodeName][s.nodeName] = s.firstChild.nodeValue
                #print (s.nodeName, s.firstChild.nodeValue)
        else:
            print n.firstChild.nodeValue

    for d in nd:
        if os.path.exists('/tmp/images/' + os.path.split(nd[d]['name'])[1]):
            print 'file existen'
            #TODO: check md5
        else:
            print 'file not existen'
            continue
    
    struct.pack_into(up_reply_fmt, reply_buf, 0, 0x00)
    send_with_header(conn, reply_buf)
    os.system('sysupgrade -v %s'%path)
    g_thread_lock.release()


global stage
stage = 0
state_tbl = {'uboot':(5, 40), 'dtb':(5, 50), 'kernel':(5, 60), 'rootfs':(5, 80), 'done':(100, 100)}
def upgrade_state(conn, data):
    global stage
    reply_fmt = 'II'

    g_thread_lock.acquire()
    if stage < 5:
        stage = stage + 1
        percent = stage * 5
    else:
        if os.path.isfile('/tmp/status'):
            fps = open('/tmp/status', 'r')
            state_str = fps.read(128)
            print state_str[0:-1]
            stage, percent = state_tbl[state_str[0:-1]]
        else:
            percent = stage * 5
    g_thread_lock.release()
    
    reply_buf = create_string_buffer(struct.calcsize(reply_fmt))
    struct.pack_into(reply_fmt, reply_buf, 0, stage, percent)

    send_with_header_upstate(conn, reply_buf)


def config_export(conn, data):
    reply_fmt = '128s'
    reply_buf = create_string_buffer(struct.calcsize(reply_fmt))

    export_path = '/tmp/jwexport.tar.gz'
    export_cmd = 'sysupgrade -b %s' %export_path

    g_thread_lock.acquire()
    os.system(export_cmd)
    g_thread_lock.release()

    struct.pack_into(reply_fmt, reply_buf, 0, export_path)
    send_with_header(conn, reply_buf)


def config_import(conn, data):
    path = struct.unpack_from('128s', data, 0)[0]
    path = path.rstrip('\0')

    import_cmd = 'sysupgrade -r %s' %path
    g_thread_lock.acquire()
    ret = os.system(import_cmd)
    g_thread_lock.release()

    reply_fmt = 'i'
    reply_buf = create_string_buffer(struct.calcsize(reply_fmt))
    struct.pack_into(reply_fmt, reply_buf, 0, ret)

    if ret != 0:
        send_with_header(conn, reply_buf, 0, 0x01)
    else:
        send_with_header(conn, reply_buf)
    #os.system('reboot -f')

def system_reboot(conn, data):
    reply_fmt = 'i'
    reply_buf = create_string_buffer(struct.calcsize(reply_fmt))
    struct.pack_into(reply_fmt, reply_buf, 0, 0x0)
    send_with_header(conn, reply_buf)
    print 'reboot system after 3 seconds later'
    time.sleep(3)
    os.system('reboot -f')

def config_restore(conn, data):
    print 'empty func'


def dispatch_handler(conn, data, cmd):
    #print cmd_table[cmd]   #FIXME: delete after debug, cause fatal when cmd not in table

    if cmd  == 0x20001:
        login_process(conn, data)
    elif cmd  == 0x20002:
        logout_process(conn, data)
    elif cmd == 0x1000001:
        tunnel_open_process(conn, data)
    elif cmd == 0x1000002:
        tunnel_start_process(conn, data)
    elif cmd == 0x1000003:
        tunnel_stop_process(conn, data)
    elif cmd == 0x1000004:
        tunnel_close_process(conn, data)
    elif cmd == 0x1000007:
        tunnel_list_process(conn, data)
    elif cmd == 0xf000001:
        file_open_process(conn, data)
    elif cmd == 0xf000003:
        file_read_process(conn, data)
    elif cmd == 0xf000004:
        file_write_process(conn, data)
    elif cmd == 0xf000002:
        file_close_process(conn, data)
    elif cmd == 0x9b:
        upgrade_check(conn, data)
    elif cmd == 0x98:
        do_upgrade(conn, data)
    #elif cmd == 0x99:
    #    upgrade_state(conn, data)
    elif cmd == 0x95:
        config_export(conn, data)
    elif cmd == 0x96:
        config_import(conn, data)
    elif cmd == 0x97:
        config_restore(conn, data)
    elif cmd == 0x00:
        system_hello(conn, data)
    elif cmd == 0x2:
        system_get_version(conn, data)
    elif cmd == 0x4:
        system_set_version(conn, data)
    elif cmd == 0x93:
        system_reboot(conn, data)
    else:
        print 'undefind cmd %x' %cmd
        unsupport_reply(conn, data)
    #FIXME: logout not handle yet 


def multicast_dispatch_handler(conn, data, cmd, addr):
    if cmd == 0x1dd00:
        discovery_hello(conn, data, addr)
    elif cmd == 0x1dd02:
        print 'edit device info'
        discovery_modinfo(conn, data, addr)


def upstate_dispatch_handler(conn, data, cmd):
    if cmd == 0x99:
        upgrade_state(conn, data)


def obcp_handler(connection, buf):
    header_fmt = '2sBBIHHIII40x'
    try:
        mark, version, crypt, length, mode, error, product, sequence, command = struct.unpack_from(header_fmt, buf, 0)
    except: 
        print struct.error
        return
    header_table['version'] = version
    header_table['product'] = product
    header_table['sequence'] = sequence
    header_table['command'] = command

    print (mark, version, crypt, length,  mode, error, product, sequence, hex(command))
    if length > 56:
        data = connection.recv(length - 56)
    else:
        data = buf
    dispatch_handler(connection, data, command)


def obcp_upstate_handler(connection, buf):
    header_fmt = '2sBBIHHIII40x'
    try:
        mark, version, crypt, length, mode, error, product, sequence, command = struct.unpack_from(header_fmt, buf, 0)
    except: 
        print struct.error
        return
    upstate_header_table['version'] = version
    upstate_header_table['product'] = product
    upstate_header_table['sequence'] = sequence
    upstate_header_table['command'] = command

    print (mark, version, crypt, length, mode, error, product, sequence, hex(command))
    if length > 56:
        data = connection.recv(length - 56)
    else:
        data = buf
    upstate_dispatch_handler(connection, data, command)


def multicast_handler(connection, buf, addr):
    header_fmt = '2sBBIHHIII40x'
    mark, version, crypt, length, mode, error, product, sequence, command = struct.unpack_from(header_fmt, buf, 0)
    multi_header_table['version'] = version
    multi_header_table['product'] = product
    multi_header_table['sequence'] = sequence
    multi_header_table['command'] = command

    print (mark, version, crypt, length, mode, error, product, sequence, hex(command))
    
    #ignore ret pack
    if mode == 0x02:
        print 'mode ret'
        return

    '''print length
    if length > 56:
        print 'recvfrom'
        data, client = connection.recvfrom(length - 56)
        print len(data)
    else:
        print 'usring buf'
        data = buf'''

    multicast_dispatch_handler(connection, buf, command, addr)


def discovery_thread():
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM, socket.IPPROTO_UDP)
    #allow multiple sockets to use the same PORT number
    sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    sock.bind((multicast_group, local_port))
    sock.setsockopt(socket.IPPROTO_IP, socket.IP_MULTICAST_TTL, 255)

    status = sock.setsockopt(socket.IPPROTO_IP, socket.IP_ADD_MEMBERSHIP,
        socket.inet_aton(multicast_group) + socket.inet_aton(local_ip));

    while True:
        try:
            buf, addr = sock.recvfrom(312)
            print addr
        except socket.error, e:
            pass
        else:
            if buf[0:2] == 'OB':
                multicast_handler(sock, buf, addr)

    sock.close()


def upgrade_status_thread():
    s = socket.socket(socket.AF_INET,socket.SOCK_STREAM)
    s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    
    s.bind((local_ip, 1000))
    s.listen(1)

    while True:
        print 'upgrade_status_thead: waiting ....'
        conn, addr = s.accept()
        print 'upstate connection accepting'
        while True:
            try:
                buf = conn.recv(64)
            except:
                print 'recv upgrade statue fail'
                conn.close()
                break

            print 'upgrade status connection'
            if len(buf) == 0:
                break
            elif buf[0:2] =='OB':
                obcp_upstate_handler(conn, buf)

def tunnel_tickets_process(conn,data,addr):
    datatype,tunnelId,sessionId = struct.unpack_from('III', data, 0);
    print (datatype,tunnelId,sessionId)

    #TODO:
    tunnel_stream_start(tunnelId,conn,addr)

def tunnel_obfp_process(connection, buf, addr):
    header_fmt = 'IBBHIIIII'
    try:
        mark, ftype, channel, device, length, sequence, timestamp, ticks, fformat = struct.unpack_from(header_fmt, buf, 0)
    except: 
        print struct.error
        return

    #tunnel_header_table['mark'] = mark
    tunnel_header_table['type'] = ftype
    tunnel_header_table['channel'] = channel
    tunnel_header_table['device'] = device

    print (hex(mark), ftype, channel, device, length, sequence, timestamp, ticks, fformat)
    if length > 0:
        pSession = tunnel_get_pession_by_sock(connection)
        data = ''
        if pSession and pSession['protocol'] == 1:
            data = connection.recv(length)
        if pSession and pSession['protocol'] == 2:
            data = buf[28:28+length]
        if pSession and pSession['protocol'] == 3:
            data = buf[28:28+length]
        if pSession and  STREAM_STATE_START == pSession['state']:
            print data
            pSession['serialFd'].write(data)

def tunnel_tcp_server_stream_thread():
    s = socket.socket(socket.AF_INET,socket.SOCK_STREAM)
    s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    
    s.bind((local_ip, 3720))
    s.listen(1)

    while True:
        conn, addr = s.accept()
        while True:
            try:
                buf = conn.recv(28)
                print len(buf)
            except:
                print 'recv tunnel fail'
                conn.close()
                break

            if len(buf) == 0:
                break
            elif len(buf) == 12:
                tunnel_tickets_process(conn, buf, addr)
            else:
                tunnel_obfp_process(conn, buf, addr)
    s.close()


def tunnel_udp_server_stream_thread():
    s = socket.socket(socket.AF_INET,socket.SOCK_DGRAM)
    s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    
    s.bind((local_ip, 3720))

    while True:
        buf,addr = s.recvfrom(1024)
        #print (buf,addr)
        print len(buf)

        if len(buf) == 0:
            break
        elif len(buf) == 12:
            tunnel_tickets_process(s, buf, addr)
        else:
            tunnel_obfp_process(s, buf, addr)
    s.close()

def tunnel_mul_server_stream_thread():
    mul_local_port,multicast_ip,mul_remote_port = getuci.get_tunneld_para(4)
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM, socket.IPPROTO_UDP)
    #allow multiple sockets to use the same PORT number
    sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    print(multicast_ip, int(mul_local_port))
    sock.bind((multicast_ip, int(mul_local_port)))
    #sock.bind(('230.230.230.230', 8001))
    sock.setsockopt(socket.IPPROTO_IP, socket.IP_MULTICAST_TTL, 255)

    status = sock.setsockopt(socket.IPPROTO_IP, socket.IP_ADD_MEMBERSHIP,\
        socket.inet_aton(multicast_ip) + socket.inet_aton(local_ip));

    while True:
        try:
            buf, addr = sock.recvfrom(312)
            print (buf,addr)
        except socket.error, e:
            pass
        else:
            if len(buf) == 0:
                break
            elif len(buf) == 12:
                tunnel_tickets_process(sock, buf, addr)
            else:
                tunnel_obfp_process(sock, buf, addr)

    sock.close()

def tunnel_send_frame(pSession, data):
    header_fmt = 'IBBHIIIII'
    header_len = struct.calcsize(header_fmt);

    buf = create_string_buffer(header_len + len(data))

    struct.pack_into(header_fmt, buf, 0, tunnel_header_table['mark'], 0, tunnel_header_table['channel'],0,len(data),0,0,0,0)
      
    buf[28:] = data
    if pSession['protocol'] == 1:
        try:
            pSession['sock'].send(buf)
        except socket.error, e:
            print socket.error
            pass
    if pSession['protocol'] == 2:
        try:
            pSession['sock'].sendto(buf,(socket.inet_ntoa(struct.pack('I',socket.htonl(pSession['clientIp']))),3720))
        except socket.error, e:
            print socket.error
            pass
    if pSession['protocol'] == 3:
        mul_local_port,multicast_ip,mul_remote_port = getuci.get_tunneld_para(4)
        try:
            pSession['sock'].sendto(buf,(multicast_ip, int(mul_remote_port)))
        except socket.error, e:
            print socket.error
            pass


def tunnel_process_thread(*para):
    pSession = para[0]
    while True:
        pSession['readFdSet'] = [] 
        pSession['readFdSet'].append(pSession['serialFd']);
        pSession['readFdSet'].append(pSession['sock']);
        try:
            rs, ws, es = select.select(pSession['readFdSet'],[],[], 5)
            #print pSession['serialFd']
        except select.error,e:
            print select.error
            continue
        for r in rs:
            if r is pSession['serialFd']:
                data = ''
                while pSession['serialFd'].inWaiting() > 0:
                    data += pSession['serialFd'].read(1)
                print data
                if STREAM_STATE_START == pSession['state']:
                    tunnel_send_frame(pSession, data)

def tunnel_thread_init(pSession):
    para = (pSession,)
    try:
        thread.start_new_thread(tunnel_process_thread, (para))
    except:
        print 'fail to create thread 875'


def tunnel_server_stream_init():
    uart_init()
    global g_tunnelSessionCount
    g_tunnelSessionCount = 0

    for i in range(tunnel_num):
        tunnel_thread_init(g_tunnelSession[i])

def session_thread(conn, addr):
    while True:
        try:
            buf = conn.recv(64)
        except:
            break

        if len(buf) == 0:
            break
        elif buf[0:2] =='OB':
            obcp_handler(conn, buf)

    thread.exit()

MODE_TCP_SERVER,MODE_TCP_CLIENT,MODE_UDP_SERVER,MODE_UDP_CLIENT,MODE_MCAST_SERVER,MODE_MCAST_CLIENT = range(6)
PROTOCOL_TCP,PROTOCL_UDP,PROTOCOL_MCAST = range(3)

if __name__ == '__main__':
    s = socket.socket(socket.AF_INET,socket.SOCK_STREAM)
    s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)

    s.bind((local_ip, local_port))
    s.listen(5)

    try:
        thread.start_new_thread(discovery_thread, ())
        thread.start_new_thread(upgrade_status_thread, ())
    except:
        print 'fail to create thead'

    #读配置文件
    able = getuci.get_tunneld_able()
    if able == 'on':
        #读要运行的模式
        mode = getuci.get_tunneld_mode()
        #判断工作模式
        if mode == MODE_TCP_SERVER:
            tunnel_server_stream_init()
            try:
                thread.start_new_thread(tunnel_tcp_server_stream_thread, ())
            except:
                print 'fail to create thead'
        elif mode == MODE_UDP_SERVER:
            tunnel_server_stream_init()
            try:
                thread.start_new_thread(tunnel_udp_server_stream_thread, ())
            except:
                print 'fail to create thead'
        elif mode == MODE_MCAST_SERVER:
            tunnel_server_stream_init()
            try:
                thread.start_new_thread(tunnel_mul_server_stream_thread, ())
            except:
                print 'fail to create thead'

        elif mode == MODE_TCP_CLIENT or mode == MODE_UDP_CLIENT or mode == MODE_MCAST_CLIENT:
            if mode == MODE_TCP_CLIENT:
                val = PROTOCOL_TCP
            elif mode == MODE_UDP_CLIENT:
                val = PROTOCL_UDP
            elif mode == MODE_MCAST_CLIENT:
                val = PROTOCOL_MCAST
            tunnelclient.tunnel_client_init(val)


    while True:
        print 'waiting for connection ...'
        conn,addr = s.accept()
        thread.start_new_thread(session_thread, (conn, addr))

