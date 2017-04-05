
#ifndef _JRPP_IF_H_
#define _JRPP_IF_H_

int rpp_init(void);
void br_notify(int if_index, uint32_t link);
void br_pdu_rcv(unsigned char *data, int len);
void sig_kill_handler(int signo);
#endif


