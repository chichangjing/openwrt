#include <stdlib.h>
#include <stdio.h>
#include "device.h"
#include "device_info.h"
#include "ubus_call.h"
#include "../caselib/cs_type.h"

struct global_port * global_port;
struct global_ring * global_ring;
struct global_mcast * global_mcast;
struct global_isot * global_isot;
struct global_vlan * global_vlan;

char * id2ring(cs_u32 id){
    static char ring[16];
    memset(ring, 0, sizeof(ring));
    sprintf(ring, "ring %d", id);
    return ring;
}

cs_u32 ring2id(char * ring){
    return str2cs32(ring+4);
}

char * id2idx(cs_u32 id){
    static char idx[16];
    memset(idx, 0, sizeof(idx));
    sprintf(idx, "idx-%d", id);
    return idx;
}

cs_u32 idx2id(char * idx){
    return str2cs32(idx+4);
}

char * id2port(cs_u32 id){
    static char port[16];
    memset(port, 0, sizeof(port));
    sprintf(port, "port-%d", id);
    return port;
}

cs_u32 port2id(char * port){
    return str2cs32(port+5);
}

static void dev_port_init(cs_u32 port_num){
    global_port = (struct global_port *)malloc(global_port_len(port_num));
    memset(global_port, 0, global_port_len(port_num));
    global_port->count = port_num;
    int i=0;
    for(;i<_PORT_MAX;i++){
        global_port->dev_port[i].idx = i;
        global_port->dev_port[i].name = port_name[2][i] ? port_name[2][i]:NULL;
    }
}

void dev_port_spec(cs_u32 port_num){
    global_port = realloc(global_port, global_port_len(port_num));
    memset(global_port, 0, global_port_len(port_num));
    global_port->count= port_num;
}

static void dev_ring_init(cs_u32 ring_num){
    global_ring = (struct global_ring *)malloc(global_ring_len(ring_num));
    memset(global_ring, 0, global_ring_len(ring_num));
    global_ring->count = ring_num;
}

void dev_ring_spec(cs_u32 ring_num){
    global_ring = realloc(global_ring, global_ring_len(ring_num));
    memset(global_ring, 0, global_ring_len(ring_num));
    global_ring->count= ring_num;
}

static void dev_mcast_init(cs_u32 mcast_num){
    global_mcast = (struct global_mcast *)malloc(global_mcast_len(mcast_num));
    memset(global_mcast, 0, global_mcast_len(mcast_num));
    global_mcast->count = mcast_num;
}

void dev_mcast_spec(cs_u32 mcast_num){
    global_mcast = realloc(global_mcast, global_mcast_len(mcast_num));
    memset(global_mcast, 0, global_mcast_len(mcast_num));
    global_mcast->count= mcast_num;
}

static void dev_isot_init(cs_u32 isot_num){
    global_isot = (struct global_isot *)malloc(global_isot_len(isot_num));
    memset(global_isot, 0, global_isot_len(isot_num));
    global_isot->count = isot_num;
}

void dev_isot_spec(cs_u32 isot_num){
    global_isot = realloc(global_isot, global_isot_len(isot_num));
    memset(global_isot, 0, global_isot_len(isot_num));
    global_isot->count= isot_num;
}

static void dev_vlan_init(cs_u32 vlan_num){
    global_vlan = (struct global_vlan *)malloc(global_vlan_len(vlan_num));
    memset(global_vlan, 0, global_vlan_len(vlan_num));
    global_vlan->count = vlan_num;
}

void dev_vlan_spec(cs_u32 vlan_num){
    global_vlan = realloc(global_vlan, global_vlan_len(vlan_num));
    memset(global_vlan, 0, global_vlan_len(vlan_num));
    global_vlan->count= vlan_num;
}


bool _exsit_ring(cs_u32 ring_id){
    cs_u32 i =0;
    for(;i<global_ring->count; i++){
        if(global_ring->dev_ring[i].ring_id
                && global_ring->dev_ring[i].idx
                &&ring_id == global_ring->dev_ring[i].ring_id){
            return true;
        }
    }
    return false;
}

cs_u32 ringid2idx(cs_u32 ring_id){
    cs_u32 i =0;
    for(;i<global_ring->count; i++){
        if(global_ring->dev_ring[i].ring_id
                && global_ring->dev_ring[i].idx
                &&ring_id == global_ring->dev_ring[i].ring_id){
            return global_ring->dev_ring[i].idx;
        }
    }
    return 0;
}

cs_u32 mac2idx(cs_mac mac){
    cs_u32 i =0;
    for(;i<global_mcast->count; i++){
        if(global_mcast->dev_mcast[i].idx
                && !memcmp(&mac, &global_mcast->dev_mcast[i].mac, sizeof(cs_mac))){
            return global_mcast->dev_mcast[i].idx;
        }
    }
    return 0;
}

cs_u32 vlan2idx(cs_u32 vlan){
    cs_u32 i =0;
    for(;i<global_vlan->count; i++){
        if(global_vlan->dev_vlan[i].idx
                && vlan == global_vlan->dev_vlan[i].vlan_id){
            return global_vlan->dev_vlan[i].idx;
        }
    }
    return 0;
}

/*
 * ubus call ring_config ring_config_get '{"get_args":["jrpp_status"]}'
 * ubus call ring_config ring_config_get '{"get_args":["ring_num"]}'
 * ubus call ring_config ring_config_get '{"get_args":[{"idx-1":["ring_name", "primary_port","secondary_port","priority","admin"]}]}'
 *
 * */

bool UsedIn_ring(cs_u32 port){
    cs_u32 i=0;
    if(_JRPP_MODE_ENABLE==global_ring->mode){
        for(;i<global_ring->count; i++){
            if(global_ring->dev_ring[i].ring_id
                    &&_RING_MODE_ENABLE == global_ring->dev_ring[i].mode
                    &&((global_ring->dev_ring[i].master
                            && port == global_ring->dev_ring[i].master->idx)
                        ||(global_ring->dev_ring[i].slave
                            && port == global_ring->dev_ring[i].slave->idx))){
                printf(" %% Port %d is used in ring %d! Please check the configuration to continue!\n", \
                        port, global_ring->dev_ring[i].ring_id);
                return true;
            }
        }
    }
    return false;
}

void device_init(void){
    dev_port_init(_PORT_MAX);
    dev_ring_init(2);
    dev_mcast_init(0);
    dev_isot_init(_PORT_MAX);
    dev_vlan_init(0);
}

bool is_manag_mcast(cs_mac mac){
    char manag_mcast[4][6] = {
            {0x01, 0x08, 0xc2, 0x00, 0x00, 0x00},
            {0x01, 0x08, 0xc2, 0x00, 0x00, 0x2f},
            {0x0d, 0xa4, 0x2a, 0x00, 0x00, 0x05},
            {0x01, 0x00, 0x5e, 0x64, 0xe4, 0xe4},
    };
    cs_u8 i=0;
    for(;i<sizeof(manag_mcast)/(sizeof(manag_mcast[0])); i++)
        if(!memcmp(&mac, manag_mcast[i], sizeof(manag_mcast[0])))
            return true;
    return false;
}
