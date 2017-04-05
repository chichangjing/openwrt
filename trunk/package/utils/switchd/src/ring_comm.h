
#ifndef _RING_COMM_H_
#define _RING_COMM_H_

#include <sys/types.h>
#include <string.h>

#ifndef MAC_LEN
#define MAC_LEN     6
#endif

#define M_NAME_LEN  32
#define MAX_NODE_NUM 100

typedef enum {
	RPP_DISABLED = 0,
	RPP_ENABLED,
} RPP_STATE_T;

typedef enum {
    RPP_COMPAT,
    RPP_ENHANCED
}RPP_MODE_T;

typedef struct {
    unsigned char prio;
    unsigned char addr[MAC_LEN];
}BALLOT_ID_T,NODE_ID_T;

/*****************************************************************
        Ring configuration and state management
 *****************************************************************/
#define DEFAULT_RING_PRIO    0
#define DEFAULT_RING_HELLO   1
#define DEFAULT_RING_FAIL    3

typedef enum {
    RING_DISABLED = 0,
    RING_ENABLED
} RING_STATE_T;

typedef enum {
	RPP_FAULT = 0,
	RPP_HEALTH,
} RING_STATUS_T;

typedef enum {
	NODE_TYPE_MASTER			= 0,
	NODE_TYPE_TRANSIT			= 1,
} eNodeType;

// 环配置参数标记位
#define RING_CFG_STATE       (1L << 0)
#define RING_CFG_PRIO        (1L << 1)
#define RING_CFG_HELLO       (1L << 2)
#define RING_CFG_FAIL        (1L << 3)
#define RING_CFG_BALLOT      (1L << 4)
#define RING_CFG_AUTH        (1L << 5)
#define RING_CFG_ALL        RING_CFG_STATE     | \
                            RING_CFG_PRIO      | \
                            RING_CFG_HELLO     | \
                            RING_CFG_BALLOT    | \
                            RING_CFG_FAIL      | \
                            RING_CFG_AUTH

/*****************************************************************
        Port configuration and state management
 *****************************************************************/
typedef enum {
	PRIMARY_PORT = 0,
	SECONDARY_PORT,
} PORT_ROLE_T;

#if 0
typedef enum {
	RPP_PORT_DISABLED = 0,
	RPP_PORT_BLOCKING,
	RPP_PORT_FORWARDING,
} PORT_STATE_T;
#endif

typedef enum {
    DISABLED					= 0,
    BLOCKING					= 1,
    LEARNING					= 2,
    FORWARDING					= 3,
    STP_UNKOWN					= 4
} ePortStpState;

typedef enum {
    LINK_DOWN					= 2,
    LINK_UP						= 1
} ePortLinkState;

#define PORT_CFG_ROLE        (1L << 0)
#define PORT_CFG_ALL        PORT_CFG_ROLE

typedef struct {
	unsigned long   field_mask;

	/* protocol data */
	PORT_ROLE_T     port_role;
} RPP_PORT_CFG_T;

typedef struct {
	int             port_no;

	/* protocol data */
    PORT_ROLE_T     role;
	ePortStpState   dot1d_state;
    ePortLinkState  link_state;

    char            m_auth_st[M_NAME_LEN];
    char            m_ballot_st[M_NAME_LEN];
    char            m_tx_st[M_NAME_LEN];
	
	int             neigbor_port_no;
	unsigned char   neigbor_mac[MAC_LEN];

} RPP_PORT_STATE_T;


/*****************************************************************
        Ring configuration and state management
 *****************************************************************/
typedef struct {
	unsigned long   field_mask;     // 环配置参数标记字段
    RPP_STATE_T     rpp_state;      // 环网全局使能/禁止
	RING_STATE_T    ring_state;     // 环使能/禁止,暂时不用
	/* protocol data */
	unsigned char   node_priority;  // 节点优先级
	int             hello_time;        
	int             fail_time;         
	int             ballot_time;
    int             auth_time;
} RPP_RING_CFG_T;

typedef struct {
    RPP_STATE_T     rpp_state;      // 环网全局使能/禁止
    RPP_MODE_T      rpp_mode;       // 环网模式
	RING_STATE_T    ring_state;     // 环使能/禁止
	NODE_ID_T       node_id;      
	BALLOT_ID_T     master_id;    
	RING_STATUS_T   ring_status;    // 环状态
    eNodeType       node_role;
    unsigned short  hello_seq;
    unsigned char   endpoint[MAC_LEN];

    char            m_node_st[M_NAME_LEN];

    RPP_PORT_STATE_T primary;
    RPP_PORT_STATE_T secondary;
} RPP_RING_STATE_T;

typedef struct {
	unsigned char   port_no;
    unsigned char   role;
	unsigned char   stp;

	unsigned char   neighber_mac[6];
} RPP_PORT_SIMPL_T;

typedef struct {
    unsigned char   state;       // 环使能/禁止
    unsigned char   rpp_mode;       // 环网模式
	NODE_ID_T       node_id;      
	BALLOT_ID_T     master_id;    
	unsigned char   status;    // 环状态
    unsigned char   node_role;

    RPP_PORT_SIMPL_T primary;
    RPP_PORT_SIMPL_T secondary;
} RPP_NODE_SIMPL_T;

typedef struct {
    unsigned char       num;
    RPP_NODE_SIMPL_T    node[MAX_NODE_NUM];
}RPP_RING_TOPO_T;

#define MAX_RING_ID     0xFFFF
#define MAX_PORT_NUM    10


int ctl_ring_init(void);
int ctl_ring_send(int cmd, void *inbuf, int lin, void *outbuf, int *lout, int *res);
int cmd_show_ring(int ring_id, RPP_RING_STATE_T state);
int cmd_show_topo(int ring_id, RPP_RING_TOPO_T* topo);
int cmd_set_topo(int ring_id, int state);
int cmd_jrpp(int value);
int cmd_add_ring(int ring_id);
int cmd_del_ring(int ring_id);
int cmd_add_ringport(int port_id, int primary_port, int secondary_port);
int cmd_set_nodeprio(int ring_id, char *prio_val);
int cmd_set_ringmode(int ring_id, char *ring_mode);

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
  int r = ctl_ring_send(CMD_CODE_ ## name, in, sizeof(*in), out, &l, &res); \
  if (r || res) LOG_NORMAL("Got return code %d, %d\n", r, res); \
  if (r) return r; \
  if (res) return res; \
  name ## _COPY_OUT; \
  return r; \
}

#endif


