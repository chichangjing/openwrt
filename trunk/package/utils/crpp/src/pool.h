#ifndef _POOL_H_
#define _POOL_H_

#include <uci.h>

#include "crpp.h"
#include "pdu.h"

typedef struct {
    const char *package;
    const char *state;
    const char *section_type;
    int (*option_cb)(struct uci_context *ctx,struct uci_section *sct,struct port_s pool[]);
}expect_argc_t;


int port_init(crpp_t *p);
int port_active(struct port_s *p,uint32_t lport);
int port_inactive(struct port_s *p,uint32_t lport);
int port_forward(uint8_t s_port,struct port_s *p,crpppdu_t *pdu);
int port_knot(struct port_s *rl_p,struct port_s *tl_p);
#endif
