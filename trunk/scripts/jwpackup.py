#!/usr/bin/python
#coding=utf-8
import sys 
import os
import getopt
import hashlib
from xml.dom import minidom
import time
import tarfile

conf_file_path = './.config'

def get_file_md5(path):
    fp = open(path, 'r')
    mm = hashlib.md5()
    mm.update(fp.read())
    return mm.hexdigest()


def get_file_date(f):
    fs = os.stat(f)
    return str(fs.st_mtime)


def get_file_size(f):
    fs = os.stat(f)
    return str(fs.st_size)
    

def usage():
    print '''
    -u [u-boot file]
    -d [dtb file]
    -k [kernel file]
    -r [rootfs file]
    -c [config file]
    -f [restore config to factory]'''


def pat_file_node(doc, parent, n, arg):
    date = get_file_date(arg)
    version = '1.0' #fixed now
    md5 = get_file_md5(arg)
    name = arg

    file_node = doc.createElement(n)
    parent.appendChild(file_node)

    #print 'date: %s, md5 = %s, name = %s' %(date, md5, name)
    version_node = doc.createElement('version')
    file_node.appendChild(version_node)
    date_node = doc.createElement('date')
    file_node.appendChild(date_node)
    name_node = doc.createElement('name')
    file_node.appendChild(name_node)
    md5_node = doc.createElement('md5')
    file_node.appendChild(md5_node)

    filename_text_node = doc.createTextNode(arg)
    name_node.appendChild(filename_text_node)
    version_text_node = doc.createTextNode(version)
    version_node.appendChild(version_text_node)
    date_text_node = doc.createTextNode(date)
    date_node.appendChild(date_text_node)
    md5_text_node = doc.createTextNode(md5)
    md5_node.appendChild(md5_text_node)


def tar_files(flist):
    tf = tarfile.open('./images.tar.gz', 'w:gz')
    for f in flist:
        tf.add(f)
    tf.close()

import csv

conf_file_path = './.config'
product_info_file_path = './scripts/jws_product_info.csv'
def get_uid_by_config():
    conf_fp = open(conf_file_path)

    while True:
        line = conf_fp.readline()

        if not line:
            break

        if line[0] == '#' or line[0] == '\n':
            continue

        try:
            key, value = line.split('=')
        except:
            continue

        if 'CONFIG_JWS_SERIAL_NUM' == key:
            conf_fp.close()
            return value

    conf_fp.close()
    return None


def get_product_info_by_uid(uid):
    fp = open(product_info_file_path, 'rb')
    product_bf = csv.reader(fp)

    for item in product_bf:
        if len(item) > 3 and item[0].upper() == uid[1:-2].upper():
            fp.close()
            return item

    fp.close()
    return None


uid = get_uid_by_config()
hardware_version =get_product_info_by_uid(uid)[1]

import pysvn
import datetime
import shutil

client = pysvn.Client()
entry = client.info('./')
today = datetime.date.today()

image_name = hardware_version + '-%4d%02d%02d-B%d-' \
        %(today.year, today.month, today.day, entry.commit_revision.number) + uid[1:-2]



def gen_header(arg_dict):
    doc = minidom.Document()
    root_node = doc.createElement('upgrade')
    doc.appendChild(root_node)

    flist = []

    for p in arg_dict:
        #print p, arg_dict[p]
        if p == 'factory':
            factory_node = doc.createElement('factory')
            root_node.appendChild(factory_node)
            factory_text_node = doc.createTextNode('true')
            factory_node.appendChild(factory_text_node)
        elif p == 'sid':
            sid_node = doc.createElement('sid')
            root_node.appendChild(sid_node)
            sid_text_node = doc.createTextNode(arg_dict[p])
            sid_node.appendChild(sid_text_node)
        else:
            pat_file_node(doc, root_node, p, arg_dict[p])
            flist.append(arg_dict[p])

    tar_files(flist)

    tar_size = get_file_size('./images.tar.gz')
    #print tar_size
    tar_size_node = doc.createElement('size')
    root_node.appendChild(tar_size_node)
    size_text_node = doc.createTextNode(tar_size)
    tar_size_node.appendChild(size_text_node)

    tar_time = time.asctime(time.localtime())
    #print tar_time

    time_node = doc.createElement('date')
    root_node.appendChild(time_node)
    time_text_node = doc.createTextNode(tar_time)
    time_node.appendChild(time_text_node)

    f = open('./header.xml', 'w')
    doc.writexml(f)
    f.close()

    cmd_zero_header = 'dd if=/dev/zero of=./%s bs=1k count=4' %image_name
    cmd_header_with_zero = 'dd if=./header.xml of=./%s bs=1k conv=notrunc' %image_name
    cmd_append_tar = 'dd if=./images.tar.gz of=./%s bs=1k seek=4 oflag=append' %image_name
    os.system(cmd_zero_header)
    os.system(cmd_header_with_zero)
    os.system(cmd_append_tar)

    if os.path.exists('./bin/imx6ul/%s'%image_name):
        os.remove('./bin/imx6ul/%s'%image_name)
    shutil.move('./%s'%image_name, './bin/imx6ul/')
    


if __name__ == '__main__':
    try:
        opts, args = getopt.getopt(sys.argv[1:], "u:d:k:r:c:s:f", ["help", ])
    except getopt.GetoptError:
        usage()
        sys.exit(2)

    path_dict = {}
    path_dict['sid'] = uid[1:-2]

    for opt, arg in opts:
        if opt == '-u':
            path_dict['uboot'] = arg
        elif opt == '-d':
            path_dict['dtb'] = arg
        elif opt == '-k':
            path_dict['kernel'] = arg
        elif opt == '-r':
            path_dict['rootfs'] = arg
        elif opt == '-c':
            path_dict['confile'] = arg
        elif opt == '-f':
            path_dict['factory'] = 'true'
        #elif opt == '-s':
        #    path_dict['sid'] = arg
        else:
            usage()
            sys.exit(0)

    gen_header(path_dict)


