#!/usr/bin/env python
import csv

conf_file_path = './.config'
product_info_file_path = './scripts/jws_product_info.csv'
header_file_name = 'joyware.h'

head_templat = '#ifndef __JOYWARE_H__\n#define __JOYWARE_H__\n\n\n'
foot_templat = '#endif\n'

enum_port_type_templat = \
'''enum jws_port_type {
    JWS_PORT_TYPE_MANAGE_E = 0,
    JWS_PORT_TYPE_FIBER_E,
    JWS_PORT_TYPE_ETHER_E,
    JWS_PORT_TYPE_COMB_E,
    JWS_PORT_TYPE_LINK_E,
    JWS_PORT_TYPE_MAX_E,
};\n\n'''
    
enum_port_speed_templat = \
'''enum jws_port_speed {
    JWS_PORT_SPEED_MEGA_E = 0,
    JWS_PORT_SPEED_GIGA_E,
    JWS_PORT_SPEED_MAX_E,
};\n\n'''

struct_templat = \
'''struct jws_port_info {
    int chip;
    int type;
    int speed;
    int physical_seq;
    int logic_seq;
};\n\n'''

func_templat = \
'''static inline int jws_port_info_get(struct jws_port_info *p) {
    struct jws_port_info jws_port_table[CONFIG_JWS_PORT_TOTAL_NR + 1] = {\n\t%s};

    memcpy(p, jws_port_table, sizeof(struct jws_port_info) * (CONFIG_JWS_PORT_TOTAL_NR + 1));
    return 0;
}\n\n'''


midfix_list = ['SERIAL_NUMBER ', 'HARDWARE_VERSION ', 'PRODUCT_TYPE ', 'PORT_TOTAL_NR ', 'PORT_OPTICAL_NR ', 'PORT_ETHER_NR ', 'PORT_CPU_SEQ ']
prefix_common = '#define CONFIG_JWS_'
prefix_cpu = prefix_common + 'CPU ' 


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


def get_cpu_by_config():
    conf_fp = open(conf_file_path)

    while True:
        line = conf_fp.readline()

        if not line:
            break

        if line[0] == '#' or line[0] == '\n':
            continue

        if 'CONFIG_JWS_CPU' == line[0:14]:
            conf_fp.close()
            return line.split('=')[0].split('_')[3]

    conf_fp.close()
    return None


def get_product_info_by_uid(uid):
    fp = open(product_info_file_path, 'rb')
    product_bf = csv.reader(fp)

    for item in product_bf:
        #print item[0], uid[1:-2] 
        if len(item) > 3 and item[0].upper() == uid[1:-2].upper():
            fp.close()
            return item

    fp.close()
    return None


def config_2_header(kv_list):
    for kv in kv_list:
        if kv == None:
            continue
        if kv[0] == 'CONFIG_JWS_BOARD':
            board_type = kv[1]
        elif kv[0] == 'CONFIG_JWS_PORT_TOTAL_NR':
            ports_total = kv[1]

        fp.write(prefix_config + kv[0] + '  ' + kv[1] + '\n')

    if ports_v2p_tbl.has_key(board_type[1:-1]):
        port_map_list = ports_v2p_tbl[board_type[1:-1]]

    ln = '{'
    for seq in port_map_list:
        ln = ln + str(seq) + ','
    ln = ln + '};'
    fp.write(prefix_port_view %ports_total + ln)


keyword_translate_dict = {'G':'JWS_PORT_SPEED_GIGA_E', 'M':'JWS_PORT_SPEED_MEGA_E', \
        'F':'JWS_PORT_TYPE_FIBER_E', 'E':'JWS_PORT_TYPE_ETHER_E',\
        'C':'JWS_PORT_TYPE_MANAGE_E', 'L':'JWS_PORT_TYPE_LINK_E',\
        'D':'JWS_PORT_TYPE_COMB_E'}
def get_port_info_by_symbol(sym):
    sym_list = sym.split(':')
    speed = keyword_translate_dict[sym_list[0][0].upper()]
    ptype = keyword_translate_dict[sym_list[0][1].upper()]

    chip = sym_list[1]
    physical_seq = sym_list[2]

    return (chip, ptype, speed, physical_seq)


def export_to_header(cpu, pdif):
    fp = open(header_file_name, 'w+')
    fp.write(head_templat)

    fp.write(prefix_cpu + '"' + cpu + '"\n')

    i = 0
    for l in midfix_list:
        if i < 3:
            fp.write(prefix_common + l + '"' + pdif[i] + '"\n')
        else:
            fp.write(prefix_common + l + pdif[i] + '\n')
        i = i + 1

    fp.write('\n')
    fp.write(enum_port_type_templat)
    fp.write(enum_port_speed_templat)
    fp.write(struct_templat)

    port_map_list = pdif[i].split('-')
    logic_seq = 0
    pia = ''
    for p in port_map_list:
        pi = get_port_info_by_symbol(p)
        pinfo_str = '{' + pi[0] + ',' + pi[1] + ',' + pi[2] + ',' + pi[3] + ',' + str(logic_seq) + '},\\\n\t'
        pia = pia + pinfo_str
        logic_seq = logic_seq + 1

    fp.write(func_templat %pia)
    fp.write('\n\n')

    fp.write(foot_templat)
    fp.close()


if __name__ == "__main__":
    uid = get_uid_by_config()
    cpu = get_cpu_by_config()

    product_info = get_product_info_by_uid(uid)
    export_to_header(cpu, product_info)
