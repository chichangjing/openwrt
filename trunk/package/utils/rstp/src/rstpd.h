
#ifndef _RSTPD_H_
#define _RSTPD_H_

#include <net/if.h>
#include "base.h"
#include "stpm.h"

/****************************************************
 *                  Micro define
 ****************************************************/ 
#define STP_MAX_PORT_NUM		32
#define STP_DEFAULT_VLAN_ID     0
#define STP_DEFAULT_VLAN_NAME   "vlan-none"

/* Stp admin mode */
#define STP_MODE_DISABLED		0
#define STP_MODE_ENABLED		1

/* Stp running status */
#define STP_DOWN				0
#define STP_RUNNING				1

/* Port disable/enable */
#define STP_PORT_DISABLED   	0
#define STP_PORT_ENABLED		1

/* Port link status */
#define STP_PORT_LINKDOWN   	0
#define STP_PORT_LINKUP     	1

/* Port duplex status */
#define STP_PORT_DUPLEX_HALF	0
#define STP_PORT_DUPLEX_FULL	1

/****************************************************
 *                  Bridge Parameters
 ****************************************************/ 
typedef struct {
	STPM_T*         stpm;
	int             brIndex;
	char            brName[IFNAMSIZ];
	int             portCount;
	int            *portList;
	
	/* bridge config */
	UID_STP_MODE_T  stp_enabled;
	int             bridge_priority;
	int             max_age;
	int             hello_time;
	int             forward_delay;
	int             force_version;
	int             hold_time;	
} stpBridge_t;

/****************************************************
 *                  Port Parameters
 ****************************************************/
typedef struct {
	PORT_T*         port;
    int             ifIndex;
	char            ifName[IFNAMSIZ];
	unsigned char   portEnable;
	int             linkStatus;
	int             speed;
	int             duplex;
	
	/* port config */
	int             port_priority;
	int             admin_port_path_cost;
	ADMIN_P2P_T     admin_point2point;
	unsigned char   admin_edge;
	unsigned char   admin_non_stp;	
} stpPort_t;

typedef struct {
	int             running;
	stpBridge_t     bridge;
	stpPort_t      *ports;
} rstp_t;

#endif

