#include <linux/rtnetlink.h>
#include <netinet/in.h>
#include <unistd.h>
#include <syslog.h>
#include <string.h>

#include <libubus.h>
#include <joyware.h>

#include "crpp.h"
#include "libepoll.h"
#include "librtnl.h"
#include "rawsock.h"
#include "unixsock.h"
#include "notify.h"
#include "pool.h"
#include "interface.h"
#include "pdu.h"


/**<    name rule: JW.year|month.01(main ver).01(sub ver) */
#define VERSION "JW.201612.02.01"    
#define MODULE_NAME "crppd"

static crpp_t *pCrpp = NULL;
static struct ubus_context *ctx;
static struct blob_buf b;
static struct sock_filter crpp_filter[] = {
    { 0x20, 0, 0, 0x00000002 },
    { 0x15, 0, 3, 0x2a000006 },
    { 0x28, 0, 0, 0x00000000 },
    { 0x15, 0, 1, 0x00000da4 },
    { 0x6,  0, 0, 0x00040000 },
    { 0x6,  0, 0, 0x00000000 },
};

static int usage(const char *prog)
{
	LOG_ERROR("Usage: %s [options]\n"
		"Options:\n"
		"\t-d <level>\tEnable debug messages\n"
		"\t-u <level>\tSelect channel to commit log\n"
		"\n", prog);
	return 1;
}

static const struct blobmsg_policy ret_policy = {
    .name = "ret",
    .type = BLOBMSG_TYPE_STRING
};
static void callback_ret(struct ubus_request *req,int type, struct blob_attr *msg)
{
    struct blob_attr *tb[1];
    blobmsg_parse(&ret_policy, 1, tb, blob_data(msg), blob_len(msg));
    if (tb[0] == NULL)
    {
        LOG_ERROR("invalid arguement");
        return;
    }
    char *ret = blobmsg_get_string(tb[0]);
    if(ret == NULL)
        LOG_ERROR("invalid arguement");
    else
        LOG_NORMAL("ret = %s",ret);
}

static int ubus_set_ifstp(int lport,int state)
{
    uint32_t id;

    if (ubus_lookup_id(ctx, "port_config", &id))
    {
        LOG_ERROR("ubus object \"port_config\" cannot find");
        return -1;
    }

    void *ret_array1,*ret_array2;
    void *ret_table1,*ret_table2;
    blob_buf_init(&b, 0);
    ret_array1 = blobmsg_open_array(&b, "set_args");
    ret_table1 = blobmsg_open_table(&b, NULL);
    char buf[8];
    sprintf(buf,"port-%d",lport);
    ret_array2 = blobmsg_open_array(&b, buf);
    ret_table2 = blobmsg_open_table(&b, NULL);

    sprintf(buf,"%d",state);
    blobmsg_add_string(&b, "dot1d_state", buf);

    blobmsg_close_table(&b, ret_table2);
    blobmsg_close_array(&b, ret_array2);
    blobmsg_close_table(&b, ret_table1);
    blobmsg_close_array(&b, ret_array1);
    ubus_invoke(ctx, id, "port_config_set", b.head, callback_ret, 0, 3000);

    return 0;
}

static int crpp_init(void)
{
    pCrpp = (crpp_t *)calloc(1,sizeof(crpp_t));
    if(pCrpp == NULL)
    {
        LOG_ERROR("calloc crpp error");
        return -1;
    }
    pCrpp->state = CRPP_ENABLED;
    
    if(bridge_init(pCrpp) < 0)
    {
        LOG_ERROR("bridge info init error");
        return -1;
    }

    struct sock_fprog prog = {
        .len = ARRAY_SIZE(crpp_filter),
        .filter = crpp_filter,
    };

    pCrpp->raw = rawsock_init(&prog);
    if(pCrpp->raw == NULL)
    {
        LOG_ERROR("rawsock init error");
        return -1;
    }
    LOG_NORMAL("rawsock init OK");

    port_init(pCrpp);
    LOG_NORMAL("port-pool init OK");

    pCrpp->nl = rtnl_init(RTMGRP_LINK);
    if(pCrpp->nl == NULL)
    {
        LOG_ERROR("rtnl init error");
        return -1;
    }
    LOG_NORMAL("rtnl init OK");

    if(rtnl_get_link(pCrpp->nl->fd) < 0)
        LOG_ERROR("send request for get port link status error");

    notify_init(&pCrpp->notify_cb);
    LOG_NORMAL("notify init OK");

    // build physical-port <--> logical-port table
    struct jws_port_info port_info[PORT_NUM];
    jws_port_info_get(port_info);
    uint8_t idx,hport;
    for(idx = 0;idx < PORT_NUM;idx++)
    {
        hport = port_info[idx].physical_seq;
        pCrpp->l2hport[idx] = hport;
        pCrpp->h2lport[hport] = idx;
    }

    return 0;
}

void crpp_recv(uint8_t *data,uint32_t len)
{
    //LOG_NORMAL("get a crpp packet,len = %d,src_mac = [%02x:%02x:%02x:%02x:%02x:%02x]",len,data[6],data[7],data[8],data[9],data[10],data[11]);
    if(!(data[PDU_DSA_OFFSET] & DSA_TAG_BIT))
    {
        unsigned char *offset = data + PDU_DSA_OFFSET + 2;
        do{
            *offset = *(offset + 4); 
        }while(++offset != (data + CRPP_PDU_SLEN));
        if(len != CRPP_PDU_LLEN)
        {
            LOG_ERROR("rpp pdu(no dsa-tag) len = %d error !\n",len);
            return;
        }
    }
    else
    {
        if(len != CRPP_PDU_SLEN)
        {
            LOG_ERROR("rpp pdu(dsa-tag) len = %d error !\n",len);
            return;
        }
    }

    crpppdu_t *pdu = (crpppdu_t *)data;

    uint8_t rl_port;
    if(crpp_h2lport(pdu->hdr.dsa[1] >> 3,&rl_port))
    {
        LOG_ERROR("get logical port fail!\n");
        return;
    }

    if(pdu->body.type != PACKET_DETECT)
    {
        LOG_ERROR("msg type  = %d undefine!\n",pdu->body.type);
        return;
    }

    if(memcmp(pdu->hdr.src,pCrpp->mac,MAC_LEN))
    {
        port_forward(rl_port,pCrpp->port_pool,pdu);
    }
    else
    {
        uint8_t tl_port = pdu->body.port_id;
        port_knot(&pCrpp->port_pool[rl_port - 1],&pCrpp->port_pool[tl_port - 1]);
    }
}

void crpp_notify_link(uint32_t lport,ePortLinkState link)
{
    if(lport >= PORT_CPU_L || lport <= 0)
        return;

    struct port_s *port = &pCrpp->port_pool[lport - 1];
    if(link != port->link)
    {
        port->link = link;

        if(port->role != COMMON_PORT)
            return;
        
        LOG_NORMAL("port %d link %s",lport,(link == LINK_UP)?"up":"down");
        if(port->link == LINK_DOWN)
            port_inactive(port,lport);
        else
            port_active(port,lport);
    }
}

int crpp_set_port_state(int lport,int fd,ePortStp state)
{
    if(fd && rtnl_set_ifstp(fd,lport,state,NULL) < 0)
    {
        LOG_ERROR("set bridge port %d state fail",lport);
        return -1;
    }

    if(ubus_set_ifstp(lport,(state == BLOCKING)?2:4) < 0)
    {
        LOG_ERROR("set physical port %d state fail",lport);
        return -1;
    }

    return 0;
}

int crpp_h2lport(uint8_t hport, uint8_t *lport)
{
    if(hport == PORT_CPU_H || hport >= PORT_NUM)
        return -1;

    *lport = pCrpp->h2lport[hport];

    return 0;
}

int crpp_l2hport(uint8_t lport,uint8_t *hport)
{
    if(lport >= PORT_NUM)
        return -1;

    *hport = pCrpp->l2hport[lport];

    return 0;
}

int main(int argc, char **argv)
{
	int ch;
    int level,channel;
    
	while ((ch = getopt(argc, argv, "vu:d:s:")) != -1) {
		switch (ch) {
		case 'd':
			level = atoi(optarg);
            log_threshold(level);
			break;
		case 'u':
			channel = atoi(optarg);
            log_set_channel(channel);
			break;
        case 'v':
            printf("crppd version: %s\n",VERSION);
            return 0;
		default:
			return usage(argv[0]);
		}
	}

    log_init(MODULE_NAME,LOG_DAEMON);
    //openlog("crppd",LOG_CONS | LOG_PID,LOG_LOCAL1);

    LOG_NORMAL("crpp starting...");
    if(daemon(0,0) < 0)
    {
        LOG_ERROR("daemon error");
        return -1;
    }

    if(uloop_init() < 0)
    {
        LOG_ERROR("create epoll fail");
        return -1;
    }

    ctx = ubus_connect("/var/run/ubus.sock");
    if(ctx == NULL)
    {
        LOG_ERROR("ubus connect to server fail");
        return -1;
    }
    ubus_add_uloop(ctx);
    
	if(crpp_init() < 0)
    {
        LOG_ERROR("crpp init error");
        return -1;
    }
    LOG_NORMAL("crpp init OK");
    LOG_NORMAL("");

    uloop_run();
	return 0;
}
