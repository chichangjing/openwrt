#include <net/if.h>
#include <netinet/in.h>
#include <string.h>

#include "pool.h"
#include "libepoll.h"
#include "encapuci.h"
#include "utils.h"
#include "pdu.h"
#include "rawsock.h"

#define uci_dump_option_string(ctx,sct,key,value) do{\
    char *tmp = uci_lookup_option_string(ctx,sct,key);\
    value = (tmp == NULL)?tmp:strdup(tmp);\
    }while(0)

LIST_HEAD(knot_h);

static crpppdu_t packet = {
    {
        {0x0d,0xa4,0x2a,0x00,0x00,0x06},    // dst mac 
        {0x00,0x00,0x00,0x00,0x00,0x00},    // src mac
        {0x60,0x00,0x00,0x01},              // dsa tag
        {0x00,0x2e},                        // 802.3 len
        0xaa,0xaa,0x03,                     // dsap ssap llc
        {0x0c,0xa4,0x2a},                   // joyware oui
        {0x00,0xbc},                        // crpp's field
    },
    {
        0x01,                               // version
        0x00,                               // packet type
        0x00,                               // port id
        {0x00,0x00,0x00},                   // reserve
        {{0x00,0x00,0x00,0x00,0x00,0x00,     // message data
        0x00,0x00,0x00,0x00,0x00,0x00,
        0x00,0x00,0x00,0x00,0x00,0x00,
        0x00,0x00,0x00,0x00,0x00,0x00,
        0x00,0x00,0x00,0x00,0x00,0x00,
        0x00,0x00}},
    }
};

static int knot_creat(struct port_s *rl_p,struct port_s *tl_p)
{
    knot_t *knot_rl = rl_p->pknot;
    knot_t *knot_tl = tl_p->pknot;
    if(knot_tl != NULL)
    {
        if(knot_rl == knot_tl)
            return 0;
        else if(knot_rl == NULL)
        {
            knot_tl->knot_map[knot_tl->num++] = rl_p;
            rl_p->pknot = knot_tl;
            return 0;
        }
        else
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
    else
    {
        if(knot_rl == knot_tl)
        {
            knot_t *knot = (knot_t *)calloc(1,sizeof(knot_t));
            knot->knot_map[knot->num++] = rl_p;
            knot->knot_map[knot->num++] = tl_p;
            rl_p->pknot = knot;
            tl_p->pknot = knot;
            list_add_tail(&knot->node,&knot_h);
        }
        else
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
    LOG_NORMAL("knot delete contain port %d",lport);
    int idx;
    for(idx = 0;idx < knot->num;idx++)
    {
        if(knot->knot_map[idx]->lport == lport)
        {
            knot->knot_map[idx]->pknot = NULL;
            memmove(knot->knot_map + idx,knot->knot_map + idx + 1,sizeof(int) * (knot->num - idx - 1));
            break;
        }
    }
    if(--knot->num == 1)
    {
        port_active(knot->knot_map[0],knot->knot_map[0]->lport);
        knot->knot_map[0]->pknot = NULL;
        list_del(&knot->node);
        free(knot);
    }

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
                LOG_NORMAL("port %d is belong to jrpp",lport);
                pool[lport - 1].role = JRPP_PORT;
            }
        }

        lport_str = uci_lookup_option_string(ctx,sct,"secondary_port");
        if(lport_str != NULL)
        {
            lport = atoi(lport_str);
            if(PORT_IF_VALID(lport))
            {
                LOG_NORMAL("port %d is belong to jrpp",lport);
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

    if(strcmp("yes",state) == 0)
    {
        const char *lport_str; 
        uint32_t lport;

        lport_str = uci_lookup_option_string(ctx,sct,"portid");
        if(lport_str != NULL)
        {
            lport = atoi(lport_str);
            if(PORT_IF_VALID(lport))
            {
                LOG_NORMAL("port %d is belong to rstp",lport);
                pool[lport - 1].role = RSTP_PORT;
            }
        }
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
            //LOG_NORMAL("section name = %s",elt->name);
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
        .state          = "rstpd.@admin[0].rstp",
        .section_type   = "port",
        .option_cb      = _port_role_rstp
    };
    _port_load_config(ctx,&rstparg,pool);

    uci_free_context(ctx);
}

static int _port_circ_detect(struct uloop_fd *raw,uint32_t lport)
{
    uint8_t hport;
    MsgDetect_t *detect = &packet.body.msg.detect;

    if(crpp_l2hport(lport,&hport) < 0)
        return -1;

    packet.hdr.dsa[1]   = hport << 3;
    packet.body.type    = PACKET_DETECT;
    packet.body.port_id = lport;

    struct timeval time;
    gettimeofday(&time,NULL);
    *(unsigned int *)detect->tick   = htonl(time.tv_sec * 1000 + time.tv_usec / 1000);
    *(unsigned int *)detect->ttl    = htonl(CRPP_MAX_AGE);

    return rawsock_send(raw->fd,(uint8_t *)&packet,CRPP_PDU_SLEN);
}


static void _port_timeout_handler(struct uloop_timeout *timer)
{
    struct port_s *pt = container_of(timer,struct port_s,t);
    LOG_NORMAL("port %d timeout",pt->lport);

    struct uloop_fd *nl = GET_NL_HANDLER(pt,pt->lport);
    crpp_set_port_state(pt->lport,nl->fd,FORWARDING);
}

int port_init(crpp_t *p)
{
    // pdu init
    memcpy(packet.hdr.src,p->mac,MAC_LEN);

    int port;
    for(port = 1;port < PORT_NUM;port++)
    {
        p->port_pool[port - 1].lport = port;
        p->port_pool[port - 1].role  = COMMON_PORT;
        p->port_pool[port - 1].link  = LINK_DOWN;
    }

    _port_get_role(p->port_pool);
    LOG_NORMAL("port get role OK");

    return 0;
}

int port_active(struct port_s *p,uint32_t lport)
{
    struct uloop_fd *raw = GET_RAW_HANDLER(p,lport);
    if(_port_circ_detect(raw,lport) < 0)
    {
        LOG_ERROR("port %d send loop detect packet fail",lport);
        return -1;
    }

    struct uloop_timeout *timer = &p->t;
    timer->cb = _port_timeout_handler; 
    uloop_timeout_set(timer,CRPP_DETECT_TIME);

    return 0;
}

int port_inactive(struct port_s *p,uint32_t lport)
{
    if(p->pknot != NULL)
        knot_del(p->pknot,lport);

    crpp_set_port_state(lport,0,BLOCKING);

    return 0;
}

int port_forward(uint8_t s_port,struct port_s *start_addr,crpppdu_t *pdu)
{
    struct uloop_fd *raw = GET_RAW_HANDLER(start_addr,1);
    MsgDetect_t *detect = &pdu->body.msg.detect;
    uint32_t ttl = ntohl(*(unsigned int *)detect->ttl);

    if(--ttl == 0)
        return 0;
    *(unsigned int *)detect->ttl = htonl(ttl);

    uint8_t hport,lport = 1;

    for(lport = 1;lport < PORT_NUM;lport++)
    {
        if(lport == s_port || (start_addr + (lport - 1))->link == LINK_DOWN)
            continue;

        if(crpp_l2hport(lport,&hport) < 0)
            return -1;

        pdu->hdr.dsa[0]     = 0x60;
        pdu->hdr.dsa[1]     = hport << 3;

        if(rawsock_send(raw->fd,(uint8_t *)pdu,CRPP_PDU_SLEN) < 0)
            LOG_ERROR("port %d forwarding packet fail",lport);
        else
            LOG_NORMAL("port %d forwarding packet ok",lport);
    }

    return 0;
}

int port_knot(struct port_s *rl_p,struct port_s *tl_p)
{
    timer_del(&tl_p->t);
    knot_creat(rl_p,tl_p);
    LOG_ERROR("port %d recv self detect packet sended by port %d",rl_p->lport,tl_p->lport);

    return 0;
}

