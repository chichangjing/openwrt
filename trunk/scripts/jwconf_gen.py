#!/usr/bin/python
#coding=utf-8
import os
import tarfile
import shutil

conf_dir_path = './tmp/etc/'
conf_file_path = './.config'
tar_file = './tmp/defconf.tgz'

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
            return value[1:-2]

    conf_fp.close()
    return None

#conffile_list = [['network', './package/network/config/netifd/netconf/', 'config/network'], \
conffile_list = [['switchd', './package/utils/switchd/files/etc/config/', 'config/switchd'], \
        ['jrppd.conf', './package/utils/jrpp/files/', 'config/jrppd'], \
        ['rstpd', './package/utils/rstp/files/', 'config/rstpd'], \
        ['snmpd.conf', './feeds/packages/net/net-snmp/files/', 'config/snmpd'], \
        ['lldpd.config', './package/network/services/lldpd/files/', 'config/lldpd'], \
        ['lldpcli.config', './package/network/services/lldpd/files/', 'lldpd.conf'], \
        ['network', './package/network/config/netifd/netconf/', 'config/network'], \
        ['rc.local', './package/base-files/files/etc/', 'rc.local']]


if __name__ == '__main__':
    if os.path.exists(conf_dir_path) == True:
        shutil.rmtree(conf_dir_path)

    if os.path.isfile(tar_file) == True:
        os.remove(tar_file)

    os.makedirs(conf_dir_path + 'config/')

    uid = get_uid_by_config()
    #print uid

    for i in conffile_list:
        file_name = i[0]
        file_path = i[1]
        final_name = i[2]

        file_name_suffix = file_name + '.' + uid
        if os.path.exists(file_path + file_name_suffix):
            #print '%s exist!'%file_name_suffix
            src_file = file_path + file_name_suffix
        elif os.path.exists(file_path + file_name):
            #print '%s exist!'%file_name
            src_file = file_path + file_name
        else:
            print 'no %s file exist!'

        dst_file = conf_dir_path + final_name
        shutil.copyfile(src_file, dst_file) 

    tar = tarfile.open(tar_file , 'w:gz')

    joyware_sid_path = conf_dir_path + 'config/joyware.sid'
    if os.path.isfile(joyware_sid_path):
        os.remove(joyware_sid_path)
    fp = open(joyware_sid_path, 'w+')
    fp.write(uid)
    fp.close()

    for r, d, fs in os.walk(conf_dir_path):
        root_ = os.path.relpath(r, start='./tmp')
        for f in fs:
            full_path = os.path.join(r, f)
            #tar.add(fullpath)
            tar.add(full_path, arcname = os.path.join(root_, f))

    tar.close()

    dst_tar_file = './package/utils/jwupgrade/'
    shutil.copy(tar_file, dst_tar_file)

