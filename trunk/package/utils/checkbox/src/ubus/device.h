#ifndef __DEVICE_H__
#define __DEVICE_H__
#include "../caselib/cs_type.h"

#define GE_20208Z_PORTNUM 10

struct global_device{
    struct global_ring * global_ring;
};

struct dev_port{
    char * name;
    cs_u32 idx;
    cs_u8 mode;
    cs_u8 admin;
    cs_u8 status;
    cs_u8 duplex;
    cs_u8 speed;
    cs_u8 type;
    struct dev_ring * dev_ring;
};

struct global_port{
    cs_u32 count;
    struct dev_port dev_port[0];
};

struct dev_ring{
    cs_u32 idx;
    cs_u32 ring_id;
    struct dev_port * master;
    struct dev_port * slave;
    cs_u8 priority;
    cs_u8 mode;
};

struct global_ring{
    cs_u8 mode;
    cs_u32 count;
    struct dev_ring dev_ring[0];
};

struct dev_mcast{
    cs_u32 idx;
    cs_mac mac;
    cs_u32 port;
};

struct global_mcast{
    cs_u8 mode;
    cs_u32 count;
    struct dev_mcast dev_mcast[0];
};

struct dev_isot{
    struct dev_port * dev_port;
    cs_u32 idx;
    cs_u32 mask;
};

struct global_isot{
    cs_u32 count;
    struct dev_isot dev_isot[0];
};

struct dev_vlan{
    cs_u32 idx;
    cs_u32 vlan_id;
    cs_u32 utag_list;
    cs_u32 tag_list;
    char name[64];
};

struct global_vlan{
    cs_u8 mode;
    cs_u32 count;
    cs_u32 untag_list;
    struct dev_vlan dev_vlan[0];
};

#define global_port_len(__num) (sizeof(struct global_port) + __num * sizeof(struct dev_port))
#define global_ring_len(__num) (sizeof(struct global_ring) + __num * sizeof(struct dev_ring))
#define global_mcast_len(__num) (sizeof(struct global_mcast) + __num * sizeof(struct dev_mcast))
#define global_isot_len(__num) (sizeof(struct global_isot) + __num * sizeof(struct dev_isot))
#define global_vlan_len(__num) (sizeof(struct global_vlan) + __num * sizeof(struct dev_vlan))

extern struct global_port * global_port;
extern struct global_ring * global_ring;
extern struct global_mcast * global_mcast;
extern struct global_isot * global_isot;
extern struct global_vlan * global_vlan;

char * id2ring(cs_u32 id);
char * id2idx(cs_u32 id);
char * id2port(cs_u32 id);

cs_u32 ring2id(char * ring);
cs_u32 idx2id(char * idx);
cs_u32 port2id(char * port);

bool _exsit_ring(cs_u32 ring_id);

bool UsedIn_ring(cs_u32 port);

void dev_port_spec(cs_u32 port_num);
void dev_ring_spec(cs_u32 ring_num);
void dev_mcast_spec(cs_u32 mcast_num);
void dev_isot_spec(cs_u32 isot_num);
void dev_vlan_spec(cs_u32 vlan_num);
void device_init(void);


cs_u32 ringid2idx(cs_u32 ring_id);
cs_u32 mac2idx(cs_mac mac);
cs_u32 vlan2idx(cs_u32 vlan);

bool is_manag_mcast(cs_mac mac);

#endif
