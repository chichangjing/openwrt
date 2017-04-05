
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

#define CTL_SERVER_SOCK_NAME "/var/run/rstpd.sock"

/***********************************************************************************
                             CTL Commands 
 ***********************************************************************************/

/* Enable/Disable RSTP */
#define CMD_CODE_enable_rstp       101
#define enable_rstp_ARGS           (int enable)
#define enable_rstp_COPY_IN        ({ in->enable = enable; })
#define enable_rstp_COPY_OUT       ({ (void)0; })
#define enable_rstp_CALL           (in->enable)
struct enable_rstp_IN {
	int enable; 
};
struct enable_rstp_OUT {
};

/*******************************************************************/
/* Get bridge state */
#define CMD_CODE_get_bridge_state  102
#define get_bridge_state_ARGS      (UID_STP_CFG_T *cfg, UID_STP_STATE_T *state)
#define get_bridge_state_COPY_IN   ({ (void)0; })
#define get_bridge_state_COPY_OUT  ({ *cfg = out->cfg; *state = out->state; })
#define get_bridge_state_CALL      (&out->cfg, &out->state)
struct get_bridge_state_IN {
};
struct get_bridge_state_OUT {
	UID_STP_CFG_T cfg;
	UID_STP_STATE_T state;
};

/*******************************************************************/
/* Set bridge config */
#define CMD_CODE_set_bridge_config 103
#define set_bridge_config_ARGS     (UID_STP_CFG_T *cfg)
#define set_bridge_config_COPY_IN  ({in->cfg = *cfg; })
#define set_bridge_config_COPY_OUT ({ (void)0; })
#define set_bridge_config_CALL     (&in->cfg)
struct set_bridge_config_IN {
	UID_STP_CFG_T cfg;
};
struct set_bridge_config_OUT {
};

/*******************************************************************/
/* Get port state */
#define CMD_CODE_get_port_state    104
#define get_port_state_ARGS        (int port_index, UID_STP_PORT_CFG_T *cfg, UID_STP_PORT_STATE_T *state)
#define get_port_state_COPY_IN     ({ in->port_index = port_index; })
#define get_port_state_COPY_OUT    ({ *cfg = out->cfg; *state = out->state; })
#define get_port_state_CALL        (in->port_index, &out->cfg, &out->state)
struct get_port_state_IN {
	int port_index;
};
struct get_port_state_OUT {
	UID_STP_PORT_CFG_T cfg;
	UID_STP_PORT_STATE_T state;
};

/*******************************************************************/
/* Set port config */
#define CMD_CODE_set_port_config   105
#define set_port_config_ARGS       (int port_index, UID_STP_PORT_CFG_T *cfg)
#define set_port_config_COPY_IN    ({ in->port_index = port_index; in->cfg = *cfg; })
#define set_port_config_COPY_OUT   ({ (void)0; })
#define set_port_config_CALL       (in->port_index, &in->cfg)
struct set_port_config_IN {
	int port_index;
	UID_STP_PORT_CFG_T cfg;
};
struct set_port_config_OUT {
};

/*******************************************************************/
/* Set Debug Level */
#define CMD_CODE_set_debug_level   106
#define set_debug_level_ARGS       (int level)
#define set_debug_level_COPY_IN    ({ in->level = level; })
#define set_debug_level_COPY_OUT   ({ (void)0; })
#define set_debug_level_CALL       (in->level)
struct set_debug_level_IN {
	int level;
};
struct set_debug_level_OUT {
};

/*******************************************************************/
/* Set port trace */
#define CMD_CODE_set_port_trace   107
#define set_port_trace_ARGS       (BITMAP_T *portmap, int mach_index, int enableflag)
#define set_port_trace_COPY_IN    ({ in->portmap = *portmap; in->mach_index = mach_index; in->enableflag = enableflag;})
#define set_port_trace_COPY_OUT   ({ (void)0; })
#define set_port_trace_CALL       (&in->portmap, in->mach_index, in->enableflag)
struct set_port_trace_IN {
	BITMAP_T portmap;
	int mach_index;
	int enableflag;
};
struct set_port_trace_OUT {
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


