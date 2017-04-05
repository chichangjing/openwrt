#include <net/if.h>
#include <netinet/in.h>
#include <string.h>

#include "jrppd.h"
#include "crpp.h"
#include "encapuci.h"
#include "utils.h"
#include "ubus_client.h"
#include "librtnl.h"
#include "packet.h"

#include "rpp_to.h"

#define uci_dump_option_string(ctx,sct,key,value) do{\
    char *tmp = uci_lookup_option_string(ctx,sct,key);\
    value = (tmp == NULL)?tmp:strdup(tmp);\
    }while(0)

LIST_HEAD(knot_h);

static RPP_PDU_T packet = {
    .hdr = {
        {0x0d,0xa4,0x2a,0x00,0x00,0x05},    // dst mac 
        {0x00,0x00,0x00,0x00,0x00,0x00},    // src mac
        {0x60,0x00,0x00,0x01},              // dsa tag
        {0x00,0x2e},                        // 802.3 len
        0xaa,0xaa,0x03,                     // dsap ssap llc
        {0x0c,0xa4,0x2a},                   // joyware oui
        {0x00,0xbc},                        // crpp's field
    },
    .body_crpp = {
        .ver        = 0x01,                               // version
        .type       = 0x00,                               // packet type
        .port_id    = 0x00,                               // port id
    }
};

static int knot_creat(struct port_s *rl_p,struct port_s *tl_p)
{
    knot_t *knot_rl = rl_p->pknot;
    knot_t *knot_tl = tl_p->pknot;
    if(knot_tl != NULL)     // already has a knot for tx-port
    {
        if(knot_rl == knot_tl)      // do nothing if rx-port and tx-port are already in a same knot
            return 0;
        else if(knot_rl == NULL)    // add rx-port into the knot which tx-port belong
        {
            knot_tl->knot_map[knot_tl->num++] = rl_p;
            rl_p->pknot = knot_tl;
            return 0;
        }
        else    // merge two different knots
        {
            int idx;
            for(idx = 0;idx < knot_rl->num;idx++)
            {
                knot_tl->knot_map[knot_tl->num++] = knot_rl->knot_map[idx];
                knot_rl->knot_map[idx]->pknot = knot_tl;
            }
            list_del(&knot_rl->node);
            free(knot_rl);

            return 0;
        }
    }
    else                    // tx-port does not has a knot so far
    {
        if(knot_rl == knot_tl)  // create a new knot if niether rx nor tx port has a knot
        {
            knot_t *knot = (knot_t *)calloc(1,sizeof(knot_t));
            if(knot == NULL)
                return -1;
            knot->knot_map[knot->num++] = rl_p;
            if(rl_p->lport != tl_p->lport)
            {
                knot->knot_map[knot->num++] = tl_p;
                tl_p->pknot = knot;
            }
            rl_p->pknot = knot;
            list_add_tail(&knot->node,&knot_h);
        }
        else                    // add tx-port into the knot which rx-port belong
        {
            knot_rl->knot_map[knot_rl->num++] = tl_p;
            tl_p->pknot = knot_rl;
            return 0;
        }
    }

    return 0;
}


static int knot_del(knot_t *knot,uint8_t lport)
{
    int idx;
    for(idx = 0;idx < knot->num;idx++)
    {
        if(knot->knot_map[idx]->lport == lport)
        {
            knot->knot_map[idx]->pknot = NULL;
            memmove(knot->knot_map + idx,knot->knot_map + idx + 1,sizeof(int) * (knot->num - idx - 1));

            if(--knot->num == 1)
            {
                crpp_port_detect(knot->knot_map[0],knot->knot_map[0]->lport);
                knot->knot_map[0]->pknot = NULL;
                list_del(&knot->node);
                free(knot);
            }
            else if(knot->num == 0)
            {
                list_del(&knot->node);
                free(knot);
            }
            
            log_info("knot remove common port %d OK",lport);
            return 0;
        }
    }

    LOG_ERROR("no matching knot member for port %d",lport);
    return -1;
}

int crpp_set_port_state(int lport,int link,ePortStpState state)
{
    if(link == LINK_UP && rtnl_set_state(lport,(state == BLOCKING)?BR_STATE_BLOCKING:BR_STATE_FORWARDING,NULL) < 0)
    {
        LOG_ERROR("set bridge port %d state fail",lport);
        return -1;
    }

    if(ubus_set_ifstp(lport,(state == BLOCKING)?2:4) < 0)
    {
        LOG_ERROR("set switch port %d state fail",lport);
       return -1; 
    }

    log_info("common port %d set %s OK",lport,(state == BLOCKING)?"blocking":"forwarding");
    return 0;
}

static int _port_role_jrpp(struct uci_context *ctx,struct uci_section *sct,struct port_s pool[])
{
    const char *state = uci_lookup_option_string(ctx,sct,"admin");
    if(state == NULL)
    {
        LOG_ERROR("value for jrpp.%s.admin not find",sct->e.name);
        return -1;
    }

    if(strcmp("enable",state) == 0)
    {
        const char *lport_str; 
        uint32_t lport;

        lport_str = uci_lookup_option_string(ctx,sct,"primary_port");
        if(lport_str != NULL)
        {
            lport = atoi(lport_str);
            if(PORT_IF_VALID(lport))
            {
                log_info("port %d is belong to jrpp",lport);
                pool[lport - 1].role = JRPP_PORT;
            }
        }

        lport_str = uci_lookup_option_string(ctx,sct,"secondary_port");
        if(lport_str != NULL)
        {
            lport = atoi(lport_str);
            if(PORT_IF_VALID(lport))
            {
                log_info("port %d is belong to jrpp",lport);
                pool[lport - 1].role = JRPP_PORT;
            }
        }
    }

    return 0;
}

static int _port_role_rstp(struct uci_context *ctx,struct uci_section *sct,struct port_s pool[])
{
    const char *state = uci_lookup_option_string(ctx,sct,"portnonstp");
    if(state == NULL)
    {
        LOG_ERROR("value for rstp.%s.admin not find",sct->e.name);
        return -1;
    }

    if(strcmp("no",state) == 0)
    {
        const char *lport_str; 
        uint32_t lport;

        lport_str = uci_lookup_option_string(ctx,sct,"portid");
        if(lport_str != NULL)
        {
            lport = atoi(lport_str);
            if(PORT_IF_VALID(lport))
            {
                log_info("port %d is belong to rstp",lport);
                pool[lport - 1].role = RSTP_PORT;
            }
        }
    }

    return 0;
}

static int _port_role_nas(struct uci_context *ctx,struct uci_section *sct,struct port_s pool[])
{
    const char *ifname = uci_lookup_option_string(ctx,sct,"ifname");
    if(ifname == NULL)
    {
        LOG_ERROR("option for nas.%s.ifname not find",sct->e.name);
        return -1;
    }

    uint32_t lport;

    lport = atoi(ifname + 4);
    if(PORT_IF_VALID(lport))
    {
        log_info("port %d is belong to ieee802.1x",lport);
        pool[lport - 1].role = IEEE802_1X;
    }

    return 0;
}


static int _port_load_config(struct uci_context *ctx,expect_argc_t *argc,struct port_s pool[])
{
    struct uci_package *pkg;

    if(UCI_OK != uci_load(ctx,argc->package,&pkg))
        return -1;

    struct uci_ptr ptr;

    if(uci_load_config(argc->state,UCI_TYPE_STRING,&ptr,ctx) == 0 && strcmp(ptr.o->v.string,"on") == 0)
    {
        struct uci_section *sct;
        struct uci_element *elt;

        uci_foreach_element(&pkg->sections,elt)
        {
            sct = uci_to_section(elt);
            //log_info("section name = %s",elt->name);
            if(strcmp(sct->type,argc->section_type) == 0)
            {
                argc->option_cb(ctx,sct,pool);
            }
        }
    }

    return 0;
}

static void _port_get_role(struct port_s pool[])
{
    struct uci_context *ctx; 
    ctx = uci_alloc_context();
    
    expect_argc_t jrpparg = {
        .package        = "/etc/config/jrppd",
        .state          = "jrppd.admin.jrpp",
        .section_type   = "ring",
        .option_cb      = _port_role_jrpp
    };
    _port_load_config(ctx,&jrpparg,pool);

    expect_argc_t rstparg = {
        .package        = "/etc/config/rstpd",
        .state          = "rstpd.admin.rstp",
        .section_type   = "port",
        .option_cb      = _port_role_rstp
    };
    _port_load_config(ctx,&rstparg,pool);

    expect_argc_t ieee8021xparg = {
        .package        = "/etc/config/nas",
        .state          = "nas.admin.802_1x",
        .section_type   = "iface",
        .option_cb      = _port_role_nas
    };
    _port_load_config(ctx,&ieee8021xparg,pool);

    uci_free_context(ctx);
}

static int _port_circ_detect(uint32_t lport)
{
    uint8_t hport;
    MsgDetect_t *detect = &packet.body_crpp.msg.detect;

    if(RPP_OUT_l2hport(lport,&hport) < 0)
        return -1;

    packet.hdr.dsa_tag[1]   = hport << 3;
    packet.body.type    = PACKET_DETECT;
    packet.body.port_id = lport;

    struct timeval time;
    gettimeofday(&time,NULL);
    *(unsigned int *)detect->tick   = htonl(time.tv_sec * 1000 + time.tv_usec / 1000);
    *(unsigned int *)detect->ttl    = htonl(CRPP_MAX_AGE);

    pdu_pkt_send(lport,(uint8_t *)&packet,PDU_SLEN);
    return 0;
}


static void _port_timeout_handler(struct uloop_timeout *timer)
{
    struct port_s *pt = container_of(timer,struct port_s,t);
    log_info("common port %d loop detect timeout",pt->lport);

    crpp_set_port_state(pt->lport,pt->link,FORWARDING);
}

int crpp_init(struct port_s *pool,char *mac)
{
    // pdu init
    memcpy(packet.hdr.src_mac,mac,MAC_LEN);

    int lport;
    for(lport = 1;lport < PORT_NUM;lport++)
    {
        pool[lport - 1].lport = lport;
        pool[lport - 1].role  = COMMON_PORT;
        pool[lport - 1].link  = LINK_DOWN;

        if(ubus_set_ifstp(lport,2) < 0)
        {
            LOG_ERROR("set switch port %d state fail",lport);
           return -1; 
        }
    }

    _port_get_role(pool);
    log_info("all ports get role OK");

    return 0;
}

int crpp_port_detect(struct port_s *p,uint32_t lport)
{
    if(_port_circ_detect(lport) < 0)
    {
        LOG_ERROR("port %d send loop detect packet fail",lport);
        return -1;
    }
    log_info("common port %d send loop detect packet OK",lport);

    struct uloop_timeout *timer = &p->t;
    timer->cb = _port_timeout_handler; 
    uloop_timeout_set(timer,CRPP_DETECT_TIME);

    return 0;
}

int port_inactive(struct port_s *p,uint32_t lport)
{
    if(p->pknot != NULL)
        knot_del(p->pknot,lport);

    crpp_set_port_state(lport,p->link,BLOCKING);

    return 0;
}

int port_forward(uint8_t s_port,struct port_s *start_addr,RPP_PDU_T *pdu)
{
    MsgDetect_t *detect = &pdu->body_crpp.msg.detect;
    uint32_t ttl = ntohl(*(unsigned int *)detect->ttl);

    if(--ttl == 0)
        return 0;
    *(unsigned int *)detect->ttl = htonl(ttl);

    uint8_t hport,lport = 1;

    for(lport = 1;lport < PORT_NUM;lport++)
    {
        if(lport == s_port || (start_addr + (lport - 1))->link == LINK_DOWN)
            continue;

        if(RPP_OUT_l2hport(lport,&hport) < 0)
            return -1;

        pdu->hdr.dsa_tag[0]     = 0x60;
        pdu->hdr.dsa_tag[1]     = hport << 3;

		pdu_pkt_send(lport,(uint8_t *)pdu,PDU_SLEN);
        //log_info("port %d forwarding packet ok",lport);
    }

    return 0;
}

int port_knot(struct port_s *rl_p,struct port_s *tl_p)
{
    uloop_timeout_cancel(&tl_p->t);
    knot_creat(rl_p,tl_p);
    LOG_ERROR("port %d recv self detect packet sended by port %d",rl_p->lport,tl_p->lport);

    return 0;
}

