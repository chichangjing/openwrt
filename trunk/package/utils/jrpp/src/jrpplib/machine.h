#ifndef _MACHINE_H_
#define _MACHINE_H_

void RPP_node_enter_state(STATE_MACH_T* this);
Bool RPP_node_check_conditions(STATE_MACH_T* this);
char *RPP_node_get_state_name(int);
void fail_timer_handler(struct uloop_timeout *timer);
void hello_timer_handler(struct uloop_timeout *timer);
void cmd_timer_handler(struct uloop_timeout *timer);
int node_parse_hello_packet(rppPort_t *this, RPP_PDU_T *pdu);
int node_parse_report_packet(rppPort_t *this, RPP_PDU_T *pdu);
int node_parse_complete_packet(rppPort_t *this, RPP_PDU_T *pdu);
int node_parse_down_packet(rppPort_t *this, RPP_PDU_T *pdu);
int node_parse_cmd_packet(rppPort_t *this, RPP_PDU_T *pdu,int (*if_cb)(rppRing_t *r,int type));
void node_deal_link_down(rppRing_t *ring,rppPort_t *this);
void node_deal_down(rppRing_t *ring,eMsgNodeDownType type);
void node_deal_p2p(rppRing_t *,rppPort_t *,unsigned char *);
void node_role_handler(rppRing_t *,eNodeType,NODE_ID_T *,Bool);

void RPP_auth_enter_state(STATE_MACH_T* this);
Bool RPP_auth_check_conditions(STATE_MACH_T* this);
char *RPP_auth_get_state_name(int);
void auth_timer_handler(struct uloop_timeout *timer);
int auth_packet_parse(rppPort_t *this, RPP_PDU_T *pdu);
void auth_start(rppPort_t *port);
void auth_stop(rppPort_t *port);

void RPP_ballot_enter_state(STATE_MACH_T*);
Bool RPP_ballot_check_conditions(STATE_MACH_T*);
char *RPP_ballot_get_state_name(int);
Bool RPP_ballot_force(STATE_MACH_T*);
void ballot_timer_handler(struct uloop_timeout *);
int ballot_packet_parse(rppPort_t *, RPP_PDU_T *);
Bool ballot_if_fin(rppPort_t *);
void ballot_ittv_minus(rppPort_t *);
void ballot_p2p(rppPort_t *port);

void RPP_transmit_enter_state (STATE_MACH_T* this);  
Bool RPP_transmit_check_conditions (STATE_MACH_T* this);
char* RPP_transmit_get_state_name (int state);
void RPP_transmit_balllot_patch(Bool force);

#define tx_auth_req(p)          (p->flag.f_tx |= F_TX_AUTH_REQ)
#define tx_auth_ack(p)          (p->flag.f_tx |= F_TX_AUTH_TACK)
#define tx_ballot_ittv_m(p)     (p->flag.f_tx |= F_TX_BLT_ITTV_MINUS)
#define tx_ballot_ittv_p(p)     (p->flag.f_tx |= F_TX_BLT_ITTV_PLUS)
#define tx_ballot_pssv(p)       (p->flag.f_tx |= F_TX_BALLOT_PSSV)
#define tx_forwarding(p)        (p->flag.f_tx |= F_TX_FORWARDING)
#define tx_report(p)            (p->flag.f_tx |= F_TX_REPORT)
#define tx_hello(p)             (p->flag.f_tx |= F_TX_HELLO)
#define tx_complete(p)          (p->flag.f_tx |= F_TX_COMPLETE)
#define tx_nodedown(p)          (p->flag.f_tx |= F_TX_NODEDOWN)
#define tx_cmd(p)               (p->flag.f_tx |= F_TX_CMD)

#define RPP_machine_get_state(name,addr) RPP_ ## name ## _get_state_name(addr->state)
#endif
