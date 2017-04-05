
#ifndef _RPP_PORT_H_
#define _RPP_PORT_H_

#include <libubox/uloop.h>

#include "statmch.h"

#define F_MASK      0xFFFF

#define F_TX_AUTH_REQ       0x0001    
#define F_TX_AUTH_TACK      0x0002    
#define F_TX_BLT_ITTV_MINUS 0x0004    
#define F_TX_BLT_ITTV_PLUS  0x0008
#define F_TX_BALLOT_PSSV    0x0010    
#define F_TX_FORWARDING     0x0020     
#define F_TX_HELLO          0x0040 
#define F_TX_COMPLETE       0x0080 
#define F_TX_REPORT         0x0100 
#define F_TX_NODEDOWN       0x0200 
#define F_TX_CMD            0x0400 

typedef struct {
    unsigned char   f_auth;       // flag for auth machine
    unsigned char   f_ballot;     // flag for ballot machine
    unsigned short  f_tx;         // flag for transmit machine
    union {
        tRMsgReport down;
        tRMsgComplete complete; 
        tRMsgNodeDown ndown;
    };
}mach_flag_t;

typedef struct rppPort{
    ePortLinkState  link_st;    
    unsigned char   port_index;
    PORT_ROLE_T     port_role;
    BALLOT_ID_T     ballot_id; 
    ePortStpState   stp_st;
    unsigned char   auth_count;  
    mach_flag_t     flag;       
    RPP_PDU_T       fwding;     // buffer used for forwarding packet

    struct uloop_timeout auth_timer;
    struct uloop_timeout ballot_timer;

    unsigned char neighber_vaild;
    unsigned char neighber_mac[MAC_LEN];
    unsigned char neighber_port;

    STATE_MACH_T *auth;         
    STATE_MACH_T *ballot;       
    STATE_MACH_T *transmit;     
    STATE_MACH_T *machines;
    
    struct rppRing  *owner;     
    struct rppPort  *peer;
}rppPort_t;

int port_get_state(rppPort_t *,RPP_PORT_STATE_T *);
void port_link_change(rppPort_t *,ePortLinkState);
int port_start(rppPort_t *);
int port_stop(rppPort_t *);
rppPort_t *port_get_peer(rppPort_t *);
rppPort_t *port_get_owner(struct rppRing *,unsigned char);
NODE_ID_T *port_min_id(NODE_ID_T *,NODE_ID_T *);
void port_fwd(rppPort_t *, RPP_PDU_T *);
int port_set_stp(rppPort_t *,ePortStpState);

static inline void port_reset(rppPort_t *port)
{
    memset(&port->ballot_id,0,sizeof(BALLOT_ID_T));
    memset(&port->flag,0,sizeof(mach_flag_t));
    port->auth_count        = 0;
    port->neighber_vaild    = 0;
}

static inline Bool timer_get_status(struct uloop_timeout *timer)
{
    return timer->pending;
}
#endif

