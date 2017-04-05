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

#ifndef __CRPP_H
#define __CRPP_H

#include <net/if.h>

#include <libubox/uloop.h>
#include <libubox/utils.h>
#include <json-c/json.h>

#include "utils.h"

#define __init __attribute__((constructor))
#define PORT_NUM 11     // port number (contain cpu port)
#define PORT_CPU_H  9   // physical cpu-port
#define PORT_CPU_L  11  // logical cpu-port 
#define PORT_IF_VALID(lport) (lport < PORT_CPU_L)
#define MAC_LEN 6


#define IS_EMPTY_MAC(mac) ((mac[0] + mac[1] + mac[2] + mac[3] + mac[4] + mac[5]) == 0) 
#define PORT_IDX    "port-"
#define ENTRY_IDX   "idx-"
#define OCTET_NUM   (PORT_NUM/8 + ((PORT_NUM%8)?1:0))

typedef enum {
    CRPP_ENABLED,
    CRPP_DISABLED
}eCrppState;

typedef enum {
    COMMON_PORT,
    RSTP_PORT,
    JRPP_PORT
}ePortRole;

typedef enum {
    LINK_DOWN,
    LINK_UP
}ePortLinkState;

typedef enum {
    DISABLED,
    LISTENING,
    LEARNING,
    FORWARDING,
    BLOCKING
}ePortStp;


#define MAX_KNOT_ELM (PORT_NUM - 1)
typedef struct {
    struct list_head node;
    struct port_s *knot_map[MAX_KNOT_ELM];
    uint8_t num;
}knot_t;

struct port_s {
    uint32_t lport;
    uint32_t link;
    ePortRole role;
    int (*cb)(struct port_s *port,uint32_t flag);
    struct uloop_timeout t;
    knot_t *pknot;
};

typedef struct {
    char if_name[IFNAMSIZ];
    char mac[MAC_LEN];
    eCrppState state;  
    struct uloop_fd *nl;
    struct uloop_fd *raw;
    void (*notify_cb)(struct uloop_fd **h);
    struct port_s port_pool[PORT_NUM];
    uint8_t h2lport[PORT_NUM];
    uint8_t l2hport[PORT_NUM];
}crpp_t;

#define GET_CRPP(addr,port) container_of((addr - (port - 1)),crpp_t,port_pool[0])
#define GET_NL_HANDLER(addr,port) GET_CRPP(addr,port)->nl
#define GET_RAW_HANDLER(addr,port) GET_CRPP(addr,port)->raw

void crpp_recv(uint8_t *data,uint32_t len);
void crpp_notify_link(uint32_t lport,ePortLinkState link);
int crpp_set_port_state(int lport,int fd,ePortStp state);
int crpp_l2hport(uint8_t lport,uint8_t *hport);
int crpp_h2lport(uint8_t hport, uint8_t *lport);
#endif
