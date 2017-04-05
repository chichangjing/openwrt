#!/usr/bin/env python
#-*- coding: utf-8 -*-
import commands

workmode = ['tcp_server', 'tcp_client', 'udp_server', 'udp_client', 'mcast_server', 'mcast_client']
MODE_TCP_SERVER,MODE_TCP_CLIENT,MODE_UDP_SERVER,MODE_UDP_CLIENT,MODE_MCAST_SERVER,MODE_MCAST_CLIENT = range(6)

#获得全局使能状态
def get_tunneld_able():
    tunneld = commands.getoutput('uci get tunneld.admin.tunneld')
    return tunneld

#获得当前运行模式
def get_tunneld_mode():
    mode = commands.getoutput('uci get tunneld.network.mode')
    for i in range(len(workmode)):
        if workmode[i] == mode:
            return i

#获得当前模式下相关参数
def get_tunneld_para(mode):
    if mode == MODE_TCP_SERVER: 
        return '3720','0','3721'
    elif mode == MODE_TCP_CLIENT:
        remoteip = commands.getoutput('uci get tunneld.network.tcpServerIp')
        return '3720',remoteip,'3721'
    elif mode == MODE_UDP_SERVER or mode == MODE_UDP_CLIENT:
        localport = commands.getoutput('uci get tunneld.network.udpLocalPort')
        remoteip = commands.getoutput('uci get tunneld.network.udpRemoteIp')
        remoteport = commands.getoutput('uci get tunneld.network.udpRemotePort')
        return localport,remoteip,remoteport
    elif mode == MODE_MCAST_SERVER or mode == MODE_MCAST_CLIENT:
        localport = commands.getoutput('uci get tunneld.network.MulLocalPort')
        remoteip = commands.getoutput('uci get tunneld.network.MulRemoteIp')
        remoteport = commands.getoutput('uci get tunneld.network.MulRemotePort')
        return localport,remoteip,remoteport

#获得串口参数
def get_serial_para(com):
    status   = commands.getoutput('uci get tunneld.com'+str(com)+'.status')
    mode     = commands.getoutput('uci get tunneld.com'+str(com)+'.mode')
    baudrate = commands.getoutput('uci get tunneld.com'+str(com)+'.baudrate')
    bytesize = commands.getoutput('uci get tunneld.com'+str(com)+'.bytesize')
    parity   = commands.getoutput('uci get tunneld.com'+str(com)+'.parity')
    stopbit  = commands.getoutput('uci get tunneld.com'+str(com)+'.stopbit')
    return status,mode,baudrate,bytesize,parity,stopbit

def set_serial_para(com, para):
    status = commands.getoutput('uci set tunneld.com'+str(com)+'.status='+str(para))
    status = commands.getoutput('uci commit tunneld')
    return status
    















