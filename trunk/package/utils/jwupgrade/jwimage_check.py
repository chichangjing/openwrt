#!/usr/bin/env python

from xml.dom import minidom
import os
import subprocess
import string

fp_p1 = open('/tmp/p1', 'r')
str_p1 = fp_p1.read(4096)
header_xml = str_p1.rstrip('\0')
fp_xml = open('/tmp/header.xml', 'w+')
fp_xml.write(header_xml)
fp_p1.close()
fp_xml.close()

#parse xml and check
doc = minidom.parse('/tmp/header.xml')
root_node = doc.documentElement
child_nodes = root_node.childNodes

first_child_node =  root_node.firstChild
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
    elif n.nodeName == 'sid':
        sid = n.firstChild.nodeValue
    else:
        continue


image_md5 = subprocess.check_output(['md5sum', '/tmp/images/openwrt-imx6ul-generic-jffs2-64k.img'])
file_md5 = image_md5.split(' ')[0]
if file_md5 != nd['rootfs']['md5']:
    print 'false'
    exit(0)


def get_mach_sid():
    output = subprocess.check_output(['jwboardinfo'])
    output_line = output.split('\n')

    for line in output_line:
        line_sp = line.split('=')
	try:
            key = string.strip(line_sp[0])
            value = string.strip(line_sp[1])
            if key == 'uid':
                return value
	except:
	    break

    return None


mach_sid = get_mach_sid()

#print nd['rootfs']['md5']
#print sid

#print image_md5
#print mach_sid

if mach_sid != sid:
    print 'false'
else:
    print 'true'

exit(0)

