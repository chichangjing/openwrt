/*
 * Copyright (C) 2013 Felix Fietkau <nbd@openwrt.org>
 * Copyright (C) 2013 John Crispin <blogic@openwrt.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version 2.1
 * as published by the Free Software Foundation
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef __SWITCHD_H
#define __SWITCHD_H

#include <libubox/uloop.h>
#include <libubox/utils.h>
#include <libubus.h>

#include <stdio.h>
#include <syslog.h>

#include <init/ssdk_init.h>

#include "fal/fal_port_ctrl.h" 
#include "fal/fal_mirror.h"
#include "fal/fal_trunk.h"
#include "fal/fal_mib.h" 
#include "fal/fal_rate.h"
#include "fal/fal_multicast.h"
#include "fal/fal_vlan.h"
#include "fal/fal_mac.h"
#include "sal/os/aos_types.h"
#include "fal/fal_type.h"
#include "fal/fal_reg_access.h"

#include <jws_convert.h>
 
#include "log.h"
#include "joyware.h"

#include <uci.h>
#include <json-c/json.h>

#define __init __attribute__((constructor))
#define PORT_NUM 10

#define ALIGN_ARG 3   
#define ALIGN(len) (((len) + (1 << ALIGN_ARG) - 1) & ~((1 << ALIGN_ARG) - 1))
#define SET_BIT_ALL(val,len) while(len--){val |= (1 << len);}
#define SET_PORT_MASK(val,len,idx) ({int len_t = len;\
                                    SET_BIT_ALL(val,len_t);\
                                    val &= ~(1 << (len - idx));\
                                    val <<= (ALIGN(len) - len);})
#define IS_EMPTY_MAC(mac) ((mac[0] + mac[1] + mac[2] + mac[3] + mac[4] + mac[5]) == 0) 
#define PORT_IDX    "port-"
#define ENTRY_IDX   "idx-"
#define OCTET_NUM   (CONFIG_JWS_PORT_TOTAL_NR/8 + ((CONFIG_JWS_PORT_TOTAL_NR%8)?1:0))


extern char *ubus_socket;
extern int upgrade_running;

struct jw_switch_policy {
    char *name;
    enum blobmsg_type type;
    union {
        int(*get_handler)(struct blob_buf *buf, int port_idx);
        int(*get_ext_handler)(struct blob_buf *buf);
    };
    union {
        int(*set_handler)(int port_idx, void *var);
        int(*set_ext_handler)(void *var);
    };
};

typedef struct{
    char *name;
    enum uci_option_type type;
    int (*ubusGet)(struct uci_ptr *);
}UciConfig_S;

typedef enum{
    DO_SET,
    DO_DEL, 
}UciDo_S;

typedef struct{
    struct list_head node;
    struct uci_ptr ptr;
    enum uci_option_type type;
    UciDo_S action;
}UciCommit_S;

typedef struct{
    struct list_head node;
    int (*func)(void);
}ExtCommit_S;

typedef enum{                   /**< 端口运行状态*/       
    PORT_STATUS_NORMAL = 0,     /**< 端口运行正常状态*/ 
    PORT_STATUS_MIRROR,         /**< 端口运行镜像状态*/ 
    PORT_STATUS_AGGREGATION,    /**< 端口运行聚合状态*/ 
    PORT_STATUS_RSTP,           /**< 端口运行RSPT状态*/ 
    PORT_STATUS_RING,           /**< 端口运行环网状态*/ 
}CurPortStatus;

typedef struct{
    int status;
}PortStatus;

 typedef struct{
	fal_port_comb_t port_comb;
    fal_port_comb_type_t comb_type;
    fal_port_physical_mode_t phy_mode;
}PortConfig;

const void *jw_switchd_get_context(const char *name, const struct jw_switch_policy *policy_tbl, int item_max);
int JW_CharToHex(char);
int JW_ObjStrToArray(int max,unsigned char [],char *);
int load_config(char *,enum uci_option_type,struct uci_ptr *,struct uci_context *);
int set_config(struct uci_context *,struct uci_ptr *,enum uci_option_type);
int del_config(struct uci_context *ctx,struct uci_ptr *ptr,enum uci_option_type type);
struct json_object *json_search_element(struct json_object *object,const char *name);
int if_valid_ip(const char *str);
int foreach_json_array_set(int,struct blob_attr *,const struct jw_switch_policy *,int);
void foreach_json_array_get(int,struct blob_attr *,struct blob_buf *,const struct jw_switch_policy *,int);
int parse_mac(unsigned char mac[],char *p);
int get_port_status(int idx);
void set_port_status(int idx, int status);
int get_port_type(int port);
int getHandlerProcess(struct blob_attr *lvl1,struct blob_buf *b, const struct jw_switch_policy *info_tbl, int max);
int setHandlerProcess(struct blob_attr *lvl1,struct blob_buf *b, const struct jw_switch_policy *info_tbl, int max);
int checkFpgaVersion();
    
void switchd_connect_ubus(void);
void switchd_reconnect_ubus(int reconnect);
void ubus_init_boardinfo(struct ubus_context *ctx);

int ubus_is_ifname(char *ifname);
int ubus_is_fdb_port(int lport);

void switchd_state_next(void);
//void switchd_state_ubus_connect(void);
//void switchd_shutdown(int event);
void switchd_signal(void);
void switchd_signal_preinit(void);

void ubus_collect_config(struct uci_ptr *,char *,enum uci_option_type,UciDo_S);
void ubus_collect_config_ext(int (*func)(void));
int ubus_commit_config(void);
int ubus_reset_config(void);
void ubus_init_eth_stats(struct ubus_context *ctx);
void ubus_init_port_config(struct ubus_context *ctx);
void ubus_init_port_mirror(struct ubus_context *ctx);
void ubus_init_rate_limit(struct ubus_context *ctx);
void ubus_init_sysinfo(struct ubus_context *ctx);
void ubus_init_port_aggr(struct ubus_context *ctx);
void ubus_init_mcast_config(struct ubus_context *ctx);
void ubus_init_vlan_config(struct ubus_context *ctx);
void ubus_init_qos_config(struct ubus_context *ctx);
void ubus_init_lldp_config(struct ubus_context *ctx);
void ubus_init_network(struct ubus_context *ctx);
void ubus_init_trap(struct ubus_context *ctx);
void ubus_init_snmp(struct ubus_context *ctx);
void ubus_init_fdb(struct ubus_context *ctx);
void ubus_init_rstp(struct ubus_context *ctx);
void ubus_init_ring(struct ubus_context *ctx);
void ubus_init_gpio(struct ubus_context *ctx);
//void ubus_init_tunnel(struct ubus_context *ctx);
void ubus_init_8021x(struct ubus_context *ctx);

int gpioInterruptInit(void);

#endif
