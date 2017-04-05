
#ifndef _CTL_FUNC_H_
#define _CTL_FUNC_H_

#include <rpp_mgmt.h>

#define MAX_RING_ID     0xFFFF
#define MAX_PORT_NUM    10


int CTL_enable_jrpp(int enable);
int CTL_add_ring(int ring_id);
int CTL_del_ring(int ring_id);
int CTL_add_ringport(int ring_id, int primary_port, int secondary_port);
int CTL_del_ringport(int ring_id);
int CTL_enable_ring(int ring_id, int enable);
int CTL_set_ring_config(int ring_id, RPP_RING_CFG_T *ring_cfg);
int CTL_get_ring_state(int ring_id, RPP_RING_STATE_T *ring_state);
int CTL_set_debug_level(int level);
int CTL_get_ring_topo(int ring_id, RPP_RING_TOPO_T *ring_topo);
int CTL_get_ring_topo_s(int,char *,int);
int CTL_set_topo(int ring_id,int state);
int CTL_update_ports(void);
#endif

