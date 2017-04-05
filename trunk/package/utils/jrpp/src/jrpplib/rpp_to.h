
/* This file contains API from JRPP library to OS */

#ifndef _RPP_OUT_H_
#define _RPP_OUT_H_

typedef enum {
    AUTH_TIME,
    BALLOT_TIME,
    FAIL_TIME,
    HELLO_TIME
}TIMER_ID;

#define rpp_get_port_addr(p,member)   container_of(p,rppPort_t,member)
#define rpp_get_ring_addr(p,member)   container_of(p,rppRing_t,member)



const unsigned char *RPP_OUT_get_mac (void);
unsigned char RPP_OUT_get_time(rppRing_t *,TIMER_ID); 
unsigned short RPP_OUT_get_ringid(rppRing_t *);
NODE_ID_T *RPP_OUT_get_nodeid(rppRing_t *);
RING_STATE_T RPP_OUT_get_ringstate(rppRing_t *);
int RPP_OUT_flush_ports(unsigned char,unsigned char);
int RPP_OUT_set_port_stp(int, ePortStpState,ePortLinkState);
int RPP_OUT_tx_pdu (int, unsigned char*, size_t);
int RPP_OUT_set_timer(struct uloop_timeout *, int);
int RPP_OUT_close_timer(struct uloop_timeout *);
int RPP_OUT_l2hport(unsigned char,unsigned char *);
int RPP_OUT_h2lport(unsigned char,unsigned char *);
int RPP_OUT_led_set(rppRing_t *,int);
int RPP_OUT_cmd_rsp(unsigned char *,unsigned int,char *,int,int);
int RPP_OUT_set_ring_state(int,int,FROM_T);
#endif /* _RPP_OUT_H_ */

