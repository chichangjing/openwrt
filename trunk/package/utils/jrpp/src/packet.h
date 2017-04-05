
#ifndef _PDU_PACKET_H_
#define _PDU_PACKET_H_

void pdu_pkt_send(int ifindex, const unsigned char *data, int len);
int pdu_sock_init(void);

#endif


