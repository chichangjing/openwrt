
#ifndef _CTL_SOCKET_H_
#define _CTL_SOCKET_H_

#include <sys/types.h>
#include <string.h>
#include "ctl_func.h"

struct ctl_msg_hdr {
	int cmd;
	int lin;
	int lout;
	int res;
};

#define set_socket_address(sa, string) \
 do {\
  (sa)->sun_family = AF_UNIX; \
  memset((sa)->sun_path, 0, sizeof((sa)->sun_path)); \
  strcpy((sa)->sun_path + 1, (string)); \
 } while (0)

#define CTL_SERVER_SOCK_NAME "/var/run/jrppd.sock"

/***********************************************************************************
                             CTL Commands 
 ***********************************************************************************/
/* Enable/Disable JRPP */
#define CMD_CODE_enable_jrpp      101
#define enable_jrpp_ARGS          (int enable)
#define enable_jrpp_COPY_IN       ({ in->enable = enable; })
#define enable_jrpp_COPY_OUT      ({ (void)0; })
#define enable_jrpp_CALL          (in->enable)
struct enable_jrpp_IN {
	int enable; 
};
struct enable_jrpp_OUT {
};

/* Add a JRPP ring */
#define CMD_CODE_add_ring         102
#define add_ring_ARGS             (int ring_id)
#define add_ring_COPY_IN          ({ in->ring_id = ring_id; })
#define add_ring_COPY_OUT         ({ (void)0; })
#define add_ring_CALL             (in->ring_id)
struct add_ring_IN {
	int ring_id; 
};
struct add_ring_OUT {
};

/*******************************************************************/
/* Delete a JRPP ring */
#define CMD_CODE_del_ring         103
#define del_ring_ARGS             (int ring_id)
#define del_ring_COPY_IN          ({ in->ring_id = ring_id; })
#define del_ring_COPY_OUT         ({ (void)0; })
#define del_ring_CALL             (in->ring_id)
struct del_ring_IN {
	int ring_id; 
};
struct del_ring_OUT {
};

/*******************************************************************/
/* Add ringport for JRPP ring */
#define CMD_CODE_add_ringport     104
#define add_ringport_ARGS         (int ring_id, int primary_port, int secondary_port)
#define add_ringport_COPY_IN      ({ in->ring_id = ring_id; in->primary_port = primary_port; in->secondary_port = secondary_port;})
#define add_ringport_COPY_OUT     ({ (void)0; })
#define add_ringport_CALL         (in->ring_id, in->primary_port, in->secondary_port)
struct add_ringport_IN {
	int ring_id;
	int primary_port;
	int secondary_port;
};
struct add_ringport_OUT {
};

/*******************************************************************/
/* Delete ringport for JRPP ring */
#define CMD_CODE_del_ringport     105
#define del_ringport_ARGS         (int ring_id)
#define del_ringport_COPY_IN      ({ in->ring_id = ring_id; })
#define del_ringport_COPY_OUT     ({ (void)0; })
#define del_ringport_CALL         (in->ring_id)
struct del_ringport_IN {
	int ring_id;
};
struct del_ringport_OUT {
};

/*******************************************************************/
/* Enable/disable a JRPP ring */
#define CMD_CODE_enable_ring      106
#define enable_ring_ARGS          (int ring_id, int enable)
#define enable_ring_COPY_IN       ({ in->ring_id = ring_id; in->enable = enable; })
#define enable_ring_COPY_OUT      ({ (void)0; })
#define enable_ring_CALL          (in->ring_id, in->enable)
struct enable_ring_IN {
	int ring_id;
	int enable;
};
struct enable_ring_OUT {
};

/*******************************************************************/
/* Set ring config */
#define CMD_CODE_set_ring_config   107
#define set_ring_config_ARGS       (int ring_id, RPP_RING_CFG_T *ring_cfg)
#define set_ring_config_COPY_IN    ({ in->ring_id = ring_id; in->ring_cfg = *ring_cfg; })
#define set_ring_config_COPY_OUT   ({ (void)0; })
#define set_ring_config_CALL       (in->ring_id, &in->ring_cfg)
struct set_ring_config_IN {
	int ring_id;
	RPP_RING_CFG_T ring_cfg;
};
struct set_ring_config_OUT {
};

/*******************************************************************/
/* Get ring state */
#define CMD_CODE_get_ring_state   108
#define get_ring_state_ARGS       (int ring_id, RPP_RING_STATE_T *ring_state)
#define get_ring_state_COPY_IN    ({ in->ring_id = ring_id; })
#define get_ring_state_COPY_OUT   ({ *ring_state = out->ring_state; })
#define get_ring_state_CALL       (in->ring_id, &out->ring_state)
struct get_ring_state_IN {
	int ring_id;
};
struct get_ring_state_OUT {
	RPP_RING_CFG_T ring_cfg;
	RPP_RING_STATE_T ring_state;
};

/*******************************************************************/
/* Set Debug Level */
#define CMD_CODE_set_debug_level  109
#define set_debug_level_ARGS      (int level)
#define set_debug_level_COPY_IN   ({ in->level = level; })
#define set_debug_level_COPY_OUT  ({ (void)0; })
#define set_debug_level_CALL      (in->level)
struct set_debug_level_IN {
	int level;
};
struct set_debug_level_OUT {
};

/*  Get ring topo */
#define CMD_CODE_get_ring_topo  110
#define get_ring_topo_ARGS      (int ring_id, RPP_RING_TOPO_T *ring_topo)
#define get_ring_topo_COPY_IN   ({ in->ring_id = ring_id; })
#define get_ring_topo_COPY_OUT  ({ *ring_topo = out->ring_topo; })
#define get_ring_topo_CALL      (in->ring_id, &out->ring_topo)
struct get_ring_topo_IN {
    int ring_id;
};
struct get_ring_topo_OUT {
    RPP_RING_TOPO_T ring_topo;
};

/*  update ports link status */
#define CMD_CODE_update_ports      111
#define update_ports_ARGS          (void)
#define update_ports_COPY_IN       ({ (void)0; })
#define update_ports_COPY_OUT      ({ (void)0; })
#define update_ports_CALL          ()
struct update_ports_IN {
};
struct update_ports_OUT {
};

/* Enable/disable a topo*/
#define CMD_CODE_set_topo   112
#define set_topo_ARGS       (int ring_id, int state)
#define set_topo_COPY_IN    ({ in->ring_id = ring_id; in->state = state; })
#define set_topo_COPY_OUT   ({ (void)0; })
#define set_topo_CALL       (in->ring_id, in->state)
struct set_topo_IN {
	int ring_id;
	int state;
};
struct set_topo_OUT {
};

/***********************************************************************************
 ***********************************************************************************/
 
/* General case part in ctl command server switch */
#define SERVER_MESSAGE_CASE(name) \
case CMD_CODE_ ## name : do { \
  if (0) log_info("CTL command " #name); \
  struct name ## _IN in0, *in = &in0; \
  struct name ## _OUT out0, *out = &out0; \
  if (sizeof(*in) != lin || sizeof(*out) != (outbuf?*lout:0)) { \
    log_info("Bad sizes lin %d != %zd or lout %d != %zd", lin, sizeof(*in), lout?*lout:0, sizeof(*out)); \
    return -1; \
  } \
  memcpy(in, inbuf, lin); \
  int r = CTL_ ## name name ## _CALL; \
  if (r) return r; \
  if (outbuf) memcpy(outbuf, out, *lout); \
  return r; \
} while (0)

/* Wraper for the control functions in the control command client */
#define CLIENT_SIDE_FUNCTION(name) \
int CTL_ ## name name ## _ARGS \
{ \
  struct name ## _IN in0, *in=&in0; \
  struct name ## _OUT out0, *out = &out0; \
  int l = sizeof(*out); \
  name ## _COPY_IN; \
  int res = 0; \
  int r = ctl_client_send(CMD_CODE_ ## name, in, sizeof(*in), out, &l, &res); \
  if (r || res) log_info("Got return code %d, %d", r, res); \
  if (r) return r; \
  if (res) return res; \
  name ## _COPY_OUT; \
  return r; \
}

#endif


