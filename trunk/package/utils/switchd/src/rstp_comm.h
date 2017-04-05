/************************************************************************
 * RSTP library - Rapid Spanning Tree (802.1t, 802.1w)
 * Copyright (C) 2001-2003 Optical Access
 * Author: Alex Rozin
 *
 * This file is part of RSTP library.
 *
 * RSTP library is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published by the
 * Free Software Foundation; version 2.1
 *
 * RSTP library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser
 * General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with RSTP library; see the file COPYING.  If not, write to the Free
 * Software Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 * 02111-1307, USA.
 **********************************************************************/

/* External management communication API definitions */

#ifndef _RSTP_COMM_H__
#define _RSTP_COMM_H__

#include <sys/types.h>
#include <string.h>

#define NAME_LEN    20

typedef struct tagBITMAP
{
    unsigned long part0;     /*  Least Significant part */
} BITMAP_T;


typedef enum {
  STP_DISABLED,
  STP_ENABLED,
} UID_STP_MODE_T;

typedef struct {
  unsigned short  prio;
  unsigned char   addr[6];
} UID_BRIDGE_ID_T;

typedef struct {
  char      vlan_name[NAME_LEN]; /* name of the VLAN, key of the bridge */
  char      action; /* 1-create, 0- delete */
} UID_STP_BR_CTRL_T;

#define BR_CFG_STATE        (1L << 0)
#define BR_CFG_PRIO         (1L << 1)
#define BR_CFG_AGE          (1L << 2)
#define BR_CFG_HELLO        (1L << 3)
#define BR_CFG_DELAY        (1L << 4)
#define BR_CFG_FORCE_VER    (1L << 5)
#define BR_CFG_AGE_MODE     (1L << 6)
#define BR_CFG_AGE_TIME     (1L << 7)
#define BR_CFG_HOLD_TIME    (1L << 8)
#define BR_CFG_ALL BR_CFG_STATE     | \
                   BR_CFG_PRIO      | \
                   BR_CFG_AGE       | \
                   BR_CFG_HELLO     | \
                   BR_CFG_DELAY     | \
                   BR_CFG_FORCE_VER | \
                   BR_CFG_AGE_MODE  | \
                   BR_CFG_AGE_TIME  | \
                   BR_CFG_HOLD_TIME

typedef struct {
  /* service data */
  unsigned long     field_mask; /* which fields to change */
  UID_STP_MODE_T    stp_enabled;
  char              vlan_name[NAME_LEN]; /* name of the VLAN, key of the bridge */

  /* protocol data */
  int           bridge_priority;
  int           max_age;
  int           hello_time;
  int           forward_delay;
  int           force_version;
  int           hold_time;
} UID_STP_CFG_T;

typedef struct {
  /* service data */
  char              vlan_name[NAME_LEN]; /* name of the VLAN, key of the bridge */
  unsigned long     vlan_id;
  UID_STP_MODE_T    stp_enabled;

  /* protocol data */
  UID_BRIDGE_ID_T   designated_root;
  unsigned long     root_path_cost;

  unsigned long     timeSince_Topo_Change; /* 14.8.1.1.3.b: TBD */
  unsigned long     Topo_Change_Count;     /* 14.8.1.1.3.c: TBD */
  unsigned char     Topo_Change;           /* 14.8.1.1.3.d: TBD */

  unsigned short    root_port;
  int               max_age;
  int               hello_time;
  int               forward_delay;
  UID_BRIDGE_ID_T   bridge_id;
} UID_STP_STATE_T;

typedef enum {
  UID_PORT_DISABLED = 0,
  UID_PORT_DISCARDING,
  UID_PORT_LEARNING,
  UID_PORT_FORWARDING,
  UID_PORT_NON_STP
} RSTP_PORT_STATE;

typedef unsigned short  UID_PORT_ID;

typedef enum {
  P2P_FORCE_TRUE,
  P2P_FORCE_FALSE,
  P2P_AUTO,
} ADMIN_P2P_T;

#define STP_DBG 1

#define PT_CFG_STATE    (1L << 0)
#define PT_CFG_COST     (1L << 1)
#define PT_CFG_PRIO     (1L << 2)
#define PT_CFG_P2P      (1L << 3)
#define PT_CFG_EDGE     (1L << 4)
#define PT_CFG_MCHECK   (1L << 5)
#define PT_CFG_NON_STP  (1L << 6)
#ifdef STP_DBG
#define PT_CFG_DBG_SKIP_RX (1L << 16)
#define PT_CFG_DBG_SKIP_TX (1L << 17)
#endif

#define PT_CFG_ALL PT_CFG_STATE  | \
                   PT_CFG_COST   | \
                   PT_CFG_PRIO   | \
                   PT_CFG_P2P    | \
                   PT_CFG_EDGE   | \
                   PT_CFG_MCHECK | \
                   PT_CFG_NON_STP                  

#define ADMIN_PORT_PATH_COST_AUTO   0

typedef struct {
  /* service data */
  unsigned long field_mask; /* which fields to change */
  BITMAP_T      port_bmp;   
  char          vlan_name[NAME_LEN]; /* name of the VLAN, key of the bridge */

  /* protocol data */
  int           port_priority;
  unsigned long admin_port_path_cost; /* ADMIN_PORT_PATH_COST_AUTO - auto sence */
  ADMIN_P2P_T   admin_point2point;
  unsigned char admin_edge;
  unsigned char admin_non_stp; /* 1- doesn't participate in STP, 1 - regular */
#ifdef STP_DBG
  unsigned int	skip_rx;
  unsigned int	skip_tx;
#endif

} UID_STP_PORT_CFG_T;

typedef struct {
  /* service data */
  char              vlan_name[NAME_LEN]; /* name of the VLAN, key of the bridge */
  unsigned int      port_no; /* key of the entry */

  /* protocol data */
  UID_PORT_ID       port_id;
  RSTP_PORT_STATE   state;
  unsigned long     path_cost;

  UID_BRIDGE_ID_T   designated_root;
  unsigned long     designated_cost;
  UID_BRIDGE_ID_T   designated_bridge;
  UID_PORT_ID       designated_port;

#if 0
  int               infoIs;
  unsigned short    handshake_flags;
#endif

  unsigned long     rx_cfg_bpdu_cnt;
  unsigned long     rx_rstp_bpdu_cnt;
  unsigned long     rx_tcn_bpdu_cnt;
  int               fdWhile;      /* 17.15.1 */
  int               helloWhen;    /* 17.15.2 */
  int               mdelayWhile;  /* 17.15.3 */
  int               rbWhile;      /* 17.15.4 */
  int               rcvdInfoWhile;/* 17.15.5 */
  int               rrWhile;      /* 17.15.6 */
  int               tcWhile;      /* 17.15.7 */
  int               txCount;      /* 17.18.40 */
  int               lnkWhile;

  unsigned long     uptime;       /* 14.8.2.1.3.a */
  unsigned long     oper_port_path_cost;
  unsigned char     role;
  unsigned char     oper_point2point;
  unsigned char     oper_edge;
  unsigned char     oper_stp_neigb;
  unsigned char     top_change_ack;
  unsigned char     tc;
} UID_STP_PORT_STATE_T;

int ctl_rstp_init(void);
int ctl_rstp_send(int cmd, void *inbuf, int lin, void *outbuf, int *lout, int *res);
int cmd_show_bridge(UID_STP_STATE_T* uid_state, UID_STP_CFG_T* uid_cfg);
int cmd_show_portdetail(int port, UID_STP_PORT_CFG_T* portCfg, UID_STP_PORT_STATE_T* portState);

int cmd_rstp(int enable);
int cmd_set_bridge_prio(unsigned long value);
int cmd_set_bridge_hello(unsigned long value);
int cmd_set_bridge_maxage(unsigned long value);
int cmd_set_bridge_fdelay(unsigned long value);

int cmd_set_port_prio(int port_index, unsigned long value);
int cmd_set_port_pathcost(int port_index, unsigned long value);
int cmd_set_port_edge(int port_index, unsigned long  value);
int cmd_set_port_nonstp(int port_index, unsigned long value);


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
#if 1
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
#define set_port_trace_ARGS       (unsigned long *portmap, int mach_index, int enableflag)
#define set_port_trace_COPY_IN    ({ in->portmap = *portmap; in->mach_index = mach_index; in->enableflag = enableflag;})
#define set_port_trace_COPY_OUT   ({ (void)0; })
#define set_port_trace_CALL       (&in->portmap, in->mach_index, in->enableflag)
struct set_port_trace_IN {
    unsigned long portmap;
    int mach_index;
    int enableflag;
};
struct set_port_trace_OUT {
};

#endif

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
  int r = ctl_rstp_send(CMD_CODE_ ## name, in, sizeof(*in), out, &l, &res); \
  if (r || res) LOG_NORMAL("Got return code %d, %d", r, res); \
  if (r) return r; \
  if (res) return res; \
  name ## _COPY_OUT; \
  return r; \
}








#endif

