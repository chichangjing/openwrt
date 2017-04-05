
#ifndef _RSTP_IF_H_
#define _RSTP_IF_H_

int init_bridge_ops(void);
int br_info_init(void);
void br_one_second(void);
void br_get_configuration(void);
int br_set_state(int ifindex, int brstate);
int br_notify(int if_index, int newlink, unsigned flags);
void br_bpdu_rcv(int if_index, const unsigned char *data, int len);
int br_notify(int if_index, int newlink, unsigned flags);

#endif


