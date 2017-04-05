#!/usr/bin/env python
#-*- coding: utf-8 -*-
import serial 
import socket, select
import sys
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
import threading
import copy
import getuci

#配置文件里网络模式
MODE_TCP_SERVER,MODE_TCP_CLIENT,MODE_UDP_SERVER,MODE_UDP_CLIENT,MODE_MCAST_SERVER,MODE_MCAST_CLIENT = range(6)
PROTOCOL_TCP,PROTOCL_UDP,PROTOCOL_MCAST = range(3)

def get_ip_address(ifname):
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    return socket.inet_ntoa(fcntl.ioctl(s.fileno(),0x8915, struct.pack('256s', ifname[:15]))[20:24])

local_ip = get_ip_address('br-lan')

mgrCli= {'seq':0, 'rcvSeq':0, 'rcvLen':0, 'rcvBuf':'' }


tunnel_num = 8
readputs = []

#tunnel session data
tunnelSession = {'sessionID':0,'tunnelID':0, 'protocol':0, 'serverIp':0, 'serverPort':0,
                 'link':0, 'dock':0, 'sock':0, 'serialFd':0, 'readFdSet':readputs,
                 'state':0}
g_tunnelSession = [copy.deepcopy(tunnelSession) for i in range(tunnel_num)]

#stream state
STREAM_STATE_IDLE,STREAM_STATE_READY,STREAM_STATE_START,STREAM_STATE_STOP,STREAM_STATE_CLOSE = range(5)

#串口信息
serial_path_name = ['/dev/ttymxc2', '/dev/ttymxc3', '/dev/ttymxc4', '/dev/ttymxc6']
g_serialFd = [0 for i in range(len(serial_path_name))]

def uart_init(com,baudrateVal,bytesizeVal,parityVal,stopbitVal):
    #for i in range(len(serial_path_name)):
    g_serialFd[com] = serial.Serial(port=serial_path_name[com], baudrate=int(baudrateVal), bytesize=int(bytesizeVal),\
            parity=parityVal, stopbits=int(stopbitVal), xonxoff=False)

def tunnel_get_serialFd(dock):
    return g_serialFd[dock-1]

sequence = 1

def cli_send_with_header(conn, data, command):
    header_fmt = "ccbbihhiii40x"
    header_len = struct.calcsize(header_fmt);

    buf = create_string_buffer(header_len + len(data))
    
    data_len = 56 + len(data)
       
    global sequence
    sequence += 1
    mgrCli['seq'] = sequence

    struct.pack_into(header_fmt, buf, 0, 'O', 'B', 90, 0, data_len, 1, 0, 0, sequence, command)

    buf[64:] = data
    try:
        conn.send(buf)
    except socket.error, e:
        print socket.error
        pass

def free_mgrCli():
    mgrCli['seq'] = 0
    mgrCli['rcvSeq'] = 0
    mgrCli['rcvLen'] = 0
    mgrCli['rcvBuf'] = ''

def tunnel_get_idle_stream():
    for i in range(tunnel_num):
        if g_tunnelSession[i]['state'] == 0:
            tunnel = g_tunnelSession[i]
            return tunnel
    print 'all tunnel is going'
    return

def client_login_process(conn, singal):

    login_snd_fmt = '16s32sI'
    login_snd_len = struct.calcsize(login_snd_fmt)
    snd_buf = create_string_buffer(login_snd_len)
    struct.pack_into(login_snd_fmt, snd_buf, 0, 'root', '12345', 4)
    command = 0x20001

    cli_send_with_header(conn, snd_buf, command)

    #timer = threading.Timer(1, fun_timer)
    #timer.start()
    #time.sleep(1)
    singal.wait()

    session, role = struct.unpack_from('I16s', mgrCli['rcvBuf'], 0);
    print (session, role)

    free_mgrCli()
    singal.clear()
    return session

def client_tunnel_open_process(conn, singal, pSession):
    tunnel_open_fmt = 'BB2sIII'
    tunnel_open_len = struct.calcsize(tunnel_open_fmt)
    snd_buf = create_string_buffer(tunnel_open_len)
    struct.pack_into(tunnel_open_fmt, snd_buf, 0, pSession['link'], pSession['dock'], '', pSession['protocol'], 0, 0)
    command = 0x1000001

    cli_send_with_header(conn, snd_buf, command)
    singal.wait()
    #print mgrCli

    if mgrCli['rcvLen'] == 0 :
        free_mgrCli()
        singal.clear()
        return 0,0,0,0,0
    sessionId,tunnelId,link,dock,res,protocol,ip,port = struct.unpack_from('IIBB2sIII', mgrCli['rcvBuf'], 0);
    print (sessionId,tunnelId,link,dock,res,protocol,ip,port)

    free_mgrCli()
    singal.clear()
    return 1,sessionId,tunnelId,ip,port

def client_send_tickets_process(pSession):
    sessionId = pSession['sessionID']
    tunnelId = pSession['tunnelID'] 
    ip = pSession['serverIp'] 
    port = pSession['serverPort']
    addr = socket.inet_ntoa(struct.pack('I',socket.htonl(ip)))


    tickets_fmt = "iii"
    tickets_len = struct.calcsize(tickets_fmt);

    buf = create_string_buffer(tickets_len)
    struct.pack_into(tickets_fmt, buf, 0, sessionId, tunnelId, 1)

    if pSession['protocol'] == 1:#tcp模式
        s = socket.socket(socket.AF_INET,socket.SOCK_STREAM)
        s.setsockopt(socket.SOL_SOCKET, socket.SO_KEEPALIVE, 1) #在客户端开启心跳维护
        s.connect((addr, port))
        try:
            s.send(buf)
        except socket.error, e:
            print socket.error
            pass
        return s

    elif pSession['protocol'] == 2:#udp模式
        address = (addr, port)
        print address
        s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        s.bind((local_ip,3720))
        try:
            s.sendto(buf,address)
        except socket.error, e:
            print socket.error
            pass
        return s
    elif pSession['protocol'] == 3:#组播模式
        mul_local_port,multicast_group,mul_remote_port = getuci.get_tunneld_para(MODE_MCAST_CLIENT)
        sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM, socket.IPPROTO_UDP)
        #allow multiple sockets to use the same PORT number
        sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        sock.bind((multicast_group, int(mul_local_port)))
        #sock.bind(('230.230.230.230', 8002))
        sock.setsockopt(socket.IPPROTO_IP, socket.IP_MULTICAST_TTL, 255)

        status = sock.setsockopt(socket.IPPROTO_IP, socket.IP_ADD_MEMBERSHIP,
                socket.inet_aton(multicast_group) + socket.inet_aton(local_ip));

        #sock.sendto(buf, (multicast_group, int(mul_remote_port)))
        try:
            sock.sendto(buf, (multicast_group, int(mul_remote_port)))
            #print(multicast_group, int(mul_remote_port))
        except:
            print 'reply multicast fail:'
        return sock

def tunnel_send_frame(pSession, data):
    
    header_fmt = 'IBBHIIIII'
    header_len = struct.calcsize(header_fmt);

    buf = create_string_buffer(header_len + len(data))

    struct.pack_into(header_fmt, buf, 0, 0xBF9D1FDB, 0, 1, 0,len(data),0,0,0,0)
      
    buf[28:] = data
    if pSession['protocol'] == 1:
        try:
            pSession['sock'].send(buf)
        except socket.error, e:
            print socket.error
            pass
    elif pSession['protocol'] == 2:
        pSession['sock'].sendto(buf,(socket.inet_ntoa(struct.pack('I',socket.htonl(pSession['serverIp']))), pSession['serverPort']))
    elif pSession['protocol'] == 3:
        mul_local_port,mul_remote_ip,mul_remote_port = getuci.get_tunneld_para(MODE_MCAST_CLIENT)
        #print (server_ip,int(server_port))
        pSession['sock'].sendto(buf,(mul_remote_ip, int(mul_remote_port)))


def cli_tunnel_process_thread(*para):
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
            elif r is pSession['sock']:
                #print ('11111111111111')
                if pSession['protocol'] == 1:
                    try:
                        buf = r.recv(28)
                    except:
                        print 'recv tunnel fail'
                        continue
                elif pSession['protocol'] == 2:
                    buf,addr = r.recvfrom(1024)
                    #print (buf,addr)
                elif pSession['protocol'] == 3:
                    buf,addr = r.recvfrom(1024)
                
                header_fmt = 'IBBHIIIII'
                try:
                    mark, ftype, channel, device, length, sequence, timestamp, ticks, fformat = struct.unpack_from(header_fmt, buf, 0)
                except: 
                    print struct.error
                    continue

                print (hex(mark), ftype, channel, device, length, sequence, timestamp, ticks, fformat)
                if length > 0:
                    if pSession['protocol'] == 1:
                        data = r.recv(length)
                    elif pSession['protocol'] == 2:
                        data = buf[28:28+length]
                    elif pSession['protocol'] == 3:
                        data = buf[28:28+length]
                        #data,addr = r.recvfrom(length)
                        #print(data,addr)
                    if STREAM_STATE_START == pSession['state']:
                        #print data
                        pSession['serialFd'].write(data)
            else:
                print r


def cli_tunnel_thread_init(pSession):
    para = (pSession,)
    try:
        thread.start_new_thread(cli_tunnel_process_thread, (para))
    except:
        print 'fail to create thread 875'


def cli_tunnel_stream_init():
    global g_tunnelSessionCount
    g_tunnelSessionCount = 0

    for i in range(tunnel_num):
        cli_tunnel_thread_init(g_tunnelSession[i])
    
    

def obcp_handler(connection, buf, singal):
    header_fmt = '2sBBIHHIII40x'
    try:
        mark, version, crypt, length, mode, error, product, sequence, command = struct.unpack_from(header_fmt, buf, 0)
    except: 
        print struct.error
        return

    print (mark, version, crypt, length,  mode, error, product, sequence, hex(command))
    if length > 56:
        data = connection.recv(length - 56)
    else:
        data = ''
    mgrCli['rcvSeq'] = sequence
    mgrCli['rcvLen'] = length - 56
    mgrCli['rcvBuf'] = data
    singal.set()

#    dispatch_handler(sequence, data, command)
def obcp_client_thread(*para):
    conn = para[0]
    singal = para[1]
    while True:
        try:
            buf = conn.recv(64)
        except:
            break
        if len(buf) == 0:
            break
        elif buf[0:2] =='OB':
            obcp_handler(conn, buf, singal)


def obcp_client_process():
    local_port,server_ip,server_port = getuci.get_tunneld_para(MODE_TCP_CLIENT)
    s = socket.socket(socket.AF_INET,socket.SOCK_STREAM)
    s.setsockopt(socket.SOL_SOCKET, socket.SO_KEEPALIVE, 1) #在客户端开启心跳维护
    s.connect((server_ip, int(server_port)))
    return s

def tunnel_client_init(pro_mode):
    #开启处理线程
    cli_tunnel_stream_init()
    #获得事件信号
    singal = threading.Event()
    #获得事件信号
    #获得socket fd  
    conn = obcp_client_process()
    #获得事件信号
    #开启客户端控制线程
    try:
        thread.start_new_thread(obcp_client_thread, ((conn, singal)))
    except:
        print 'fail to create thead'

    #获得事件信号
    #login
    session = client_login_process(conn, singal)
    #获得事件信号
    
    #打开缓存log文件
    filehandle = open('/tmp/tunneld.log','w+')

    #读配置文件获得相关参数
    for i in range(4):
        status,mode,baudrate,bytesize,parity,stopbit = getuci.get_serial_para(i+1)
        print (status,mode,baudrate,bytesize,parity,stopbit)
        uart_init(i,baudrate,bytesize,parity,stopbit)

        if status == 'on':
            #获得空闲session
            pSession = tunnel_get_idle_stream()

            pSession['link'] = 1
            pSession['dock'] = i+1 #远端端口从配置文件获得
            pSession['protocol'] = pro_mode #从入参参数获得
            pSession['serialFd'] = tunnel_get_serialFd(pSession['dock'])

            #create tunnel
            ret,pSession['sessionID'], pSession['tunnelID'], pSession['serverIp'], pSession['serverPort'] = client_tunnel_open_process(conn, singal, pSession)
            #将隧道建立消息储存在缓存log里
            filehandle.write(str(i+1)+':'+str(ret)+'\n')
            if ret == 0:
                #操作uci配置文件on改为off
                #set        <config>.<section>[.<option>]=<value>
                val = getuci.set_serial_para(i+1, 'off')
                #print val
                continue
            #send tickets
            pSession['sock'] = client_send_tickets_process(pSession)
            #print pSession['sock']
            pSession['state'] = STREAM_STATE_START

    #关闭log文件
    filehandle.close()

    while True:
        pass


