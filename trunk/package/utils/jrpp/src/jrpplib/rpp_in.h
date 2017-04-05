
#ifndef _RPP_IN_H_
#define _RPP_IN_H_

/* This file contains API from OS to the JRPP library */

/**********************************************************************************
 *         Ring/port:  create/delete/start/stop
 **********************************************************************************/
int RPP_IN_ring_create (rppRing_t *);
int RPP_IN_ring_delete (int ring_id);
int RPP_IN_ring_enable (rppRing_t *);
int RPP_IN_ring_disable (rppRing_t *,eMsgNodeDownType);
int RPP_IN_port_create (rppRing_t *,int,int);
int RPP_IN_port_delete (rppRing_t *);

/**********************************************************************************
 *         Ring/port:  get/set management
 **********************************************************************************/
int RPP_IN_ring_get_cfg (int ring_id, RPP_RING_CFG_T* ring_cfg);
int RPP_IN_port_get_cfg (int ring_id, int port_index, RPP_PORT_CFG_T* port_cfg);

int RPP_IN_ring_get_state(rppRing_t *,RPP_RING_STATE_T *);
int RPP_IN_ring_get_topo(rppRing_t *,char *,int);
int RPP_IN_ring_set_topo(rppRing_t *,RING_STATE_T);
/**********************************************************************************
 *         Other functions
 **********************************************************************************/
int RPP_IN_port_link_check(rppRing_t *,int ,int );
int RPP_IN_port_valid(rppRing_t *,int);
int RPP_IN_check_pdu_header (RPP_PDU_T* pdu, size_t len);
int RPP_IN_rx_pdu (rppRing_t *, int , RPP_PDU_T *);
int RPP_IN_cmd_msg(rppRing_t *,int , RPP_PDU_T *,int (*if_cb)(rppRing_t *r,int type));

const char* RPP_IN_get_error_explanation (int err_no);



#ifdef __LINUX__
#define RPP_INIT_CRITICAL
#define RPP_CRITICAL_START
#define RPP_CRITICAL_END
#else
extern void RPP_OUT_sem_init (void);
extern void RPP_OUT_sem_take (void);
extern void RPP_OUT_sem_give (void);
#define RPP_INIT_CRITICAL      RPP_OUT_sem_init ()
#define RPP_CRITICAL_START     RPP_OUT_sem_take ()
#define RPP_CRITICAL_END       RPP_OUT_sem_give ()
#endif

#endif

