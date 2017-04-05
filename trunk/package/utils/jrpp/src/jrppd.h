#ifndef _JRPPD_H_
#define _JRPPD_H_

#include <net/if.h>


#include <joyware.h>

#include "rpp_mgmt.h"
#include "rpp_pdu.h"
#include "statmch.h"
#include "port.h"
#include "ring.h"


#define MAC_LEN     6
#define PORT_NUM    (CONFIG_JWS_PORT_TOTAL_NR + 1)  // port number(contain cpu port)
#define PORT_CPU_H  CONFIG_JWS_PORT_CPU_SEQ         // physical cpu-port
#define PORT_CPU_L  PORT_NUM                            // logical cpu-port
#define MAX_LED_RING    (CONFIG_JWS_PORT_OPTICAL_NR/2)  // max ring id which can control led

typedef enum {
    COMMON_PORT,
    RSTP_PORT,
    JRPP_PORT,
    IEEE802_1X
}ePortRole;

/*******************************************************************************************
 *                  common loop protect protocol
 *******************************************************************************************/
#define MAX_KNOT_ELM (PORT_NUM - 1)
typedef struct {
    struct list_head node;
    struct port_s *knot_map[MAX_KNOT_ELM];
    uint8_t num;
}knot_t;

/*******************************************************************************************
 *                  rapid ring protect protocol
 *******************************************************************************************/
typedef struct {
    RING_STATE_T    enable; 
	uint16_t        ring_id;
	NODE_ID_T       node_id;
	TIME_VALUES_T   times;

    rppRing_t       rpp_ring;

    struct list_head node;
}ring_t;

struct port_s {
    uint32_t lport;
    ePortRole role;
    uint32_t link;

    int (*cb)(struct port_s *port,uint32_t flag);
    struct uloop_timeout t;
    knot_t *pknot;

    int *opt_link;
};

typedef struct {
    RPP_STATE_T running;
    char        if_name[IFNAMSIZ];
    char        mac[MAC_LEN];
    uint16_t    ring_num;

    struct port_s mports[PORT_NUM];
    uint8_t h2lport[PORT_NUM];
    uint8_t l2hport[PORT_NUM];

    struct list_head ring;
}rpp_t;

#endif


