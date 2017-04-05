#ifndef _CRPP_H_
#define _CRPP_H_

#include <uci.h>

typedef struct {
    const char *package;
    const char *state;
    const char *section_type;
    int (*option_cb)(struct uci_context *ctx,struct uci_section *sct,struct port_s pool[]);
}expect_argc_t;

#define PORT_IF_VALID(lport) (lport < PORT_CPU_L)
#define CRPP_MAX_AGE 16
#define CRPP_DETECT_TIME (CRPP_MAX_AGE * 2) // ms 

int crpp_init(struct port_s *pool,char *mac);
int crpp_port_detect(struct port_s *p,uint32_t lport);
int port_inactive(struct port_s *p,uint32_t lport);
int port_forward(uint8_t s_port,struct port_s *p,RPP_PDU_T *pdu);
int port_knot(struct port_s *rl_p,struct port_s *tl_p);
#endif
