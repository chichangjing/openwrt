#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <syslog.h>
#include <signal.h>
#include <time.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <linux/un.h>
#include <linux/types.h>
#include <linux/filter.h>
#include <linux/if_packet.h>
#include <linux/if_ether.h>
#include <linux/if_bridge.h>
#include <linux/sockios.h>
#include <net/if.h>


#include "jssdk_mode.h"
#include "fal/fal.h"
#include "jws_convert.h"

#include "ring.h"
#include "rpp_in.h"
#include "rpp_to.h"

#include "log.h"
#include "utils.h"
#include "packet.h"
#include "jrpp_if.h"
#include "jrppd.h"
#include "crpp.h"
#include "librtnl.h"
#include "ctl_func.h"

rpp_t *pRpp = NULL;
	
RPP_RING_CFG_T default_ring_cfg = {
	.node_priority = 0,
    .auth_time = 2,
	.hello_time = 1,
	.fail_time =  3,
	.ballot_time = 2,
};

static int crpp_pdu_rcv(RPP_PDU_T *pdu,uint8_t rl_port)
{
    struct port_s *port = &pRpp->mports[rl_port - 1];

    if(port->role != COMMON_PORT)
        return -1;

    if(pdu->body.type != PACKET_DETECT)
    {
        LOG_ERROR("msg type  = %d undefine!\n",pdu->body.type);
        return -1;
    }

    if(memcmp(pdu->hdr.src_mac,pRpp->mac,MAC_LEN))
        port_forward(rl_port,pRpp->mports,pdu);
    else
    {
        uint8_t tl_port = pdu->body.port_id;
        port_knot(port,&pRpp->mports[tl_port - 1]);
    }

    return 0;
}

// set ring state to /etc/config/jrpp
static int rpp_save_ring(int ring_id,int state)
{
    FILE *file,*file_new;
    char name[12]   = {0};
    char line[64]     = {0};

    sprintf(name,"ring%d",ring_id);
    file = fopen("/etc/config/jrppd","r");
    if(file == NULL)
        return -1;
    file_new = fopen("/etc/config/jrppd.bak","w");
    if(file == NULL)
        goto close_out;

    while(fgets(line,sizeof(line),file))
    {
        if(state == RING_ENABLED && strstr(line,"jrpp") != NULL)
            strcpy(line,"\toption jrpp on\n");

        fputs(line,file_new);
        if(strstr(line,name))
        {
            while(fgets(line,sizeof(line),file))
            {
                if(!strncmp(line,"config",strlen("config")))
                {
                    goto free_out;
                }

                if(strstr(line,"admin") != NULL)
                {
                    sprintf(line,"\toption admin %s\n",(state == RING_ENABLED)?"enable":"disable");
                    fputs(line,file_new);
                    while(fgets(line,sizeof(line),file) != NULL)
                        fputs(line,file_new);

                    fclose(file);
                    fclose(file_new);
                    remove("/etc/config/jrppd");
                    rename("/etc/config/jrppd.bak","/etc/config/jrppd");
                    system("ubus call ring_config ring_config_get '{\"get_args\":[\"reload\"]}'");
                    return 0;
                }

                fputs(line,file_new);
            }

            goto free_out;
        }
    }

free_out:
    fclose(file_new);
    remove("/etc/config/jrppd.bak");
close_out:
    fclose(file);
    return -1;
}

/* 根据key索引对应的环(flag: 1- 按环号索引；0- 按端口号索引)
 * 索引时覆盖所有环(包括处于禁止状态的)
 */
static ring_t *rpp_find_ring(int key,int flag)
{
    ring_t *pRing,*pRing_bak;  
    list_for_each_entry_safe(pRing, pRing_bak, &pRpp->ring, node) 
    {
        if(flag && pRing->ring_id == key)   
            return pRing;
        else if(!flag && RPP_IN_port_valid(&pRing->rpp_ring,key))
            return pRing;
    }

    return NULL;
}

static int rpp_cmd_pdu_cb(rppRing_t *rppRing,int cbtype)
{

    return 0;
}

static int rpp_pdu_rcv(RPP_PDU_T *pdu,uint8_t lport)
{
    // check if recv-port belong to rpp
    ring_t *pRing = rpp_find_ring(lport,0);
    if(pRing == NULL)
    {
        LOG_ERROR("logic port %d is not in any ring",lport);
		return -1;
    }

    // check if recv-ringid is valid
    if(pRing->ring_id != ntohs(*(unsigned short *)pdu->body.ring_id))
    {
        LOG_ERROR("ring id = %d does not match[%d]!\n",ntohs(*(unsigned short *)pdu->body.ring_id),pRing->ring_id);
        return -1;
    }

    // parse common command
    if(pdu->body.type == PACKET_COMMAND)
    {
        RPP_IN_cmd_msg(&pRing->rpp_ring,lport,pdu,rpp_cmd_pdu_cb);
        return -1;
    }

    // check if the rpp or ring is vaild
    if(pRpp->running != RPP_ENABLED || pRing->enable != RING_ENABLED)
    {
        LOG_ERROR("rpp ring#%d is not enabled!\n",pRing->ring_id);
        return -1;
    }

    if(RPP_IN_rx_pdu(&pRing->rpp_ring,lport,pdu))
    {
        LOG_ERROR("deal with rx packet fail\n");
        return -1;
    }
    
    return 0;
}

int rpp_init(void)
{
    pRpp = (rpp_t *)calloc(1,sizeof(rpp_t));        
    if(pRpp == NULL)
    {
        LOG_ERROR("calloc rpp fail!\n");
        return -1;
    }

    pRpp->running   = RPP_DISABLED;
    INIT_LIST_HEAD(&pRpp->ring);

    init_ring_led();
    RPP_OUT_led_set(NULL,LED_CLOSE);

    if(get_bridge_name(pRpp->if_name))
    {
        LOG_ERROR("get bridge name fail!\n");
        return -1;
    }
    if(get_if_mac(pRpp->if_name,pRpp->mac))
    {
        LOG_ERROR("get mac address fail!\n");
        return -1;
    }

    struct jws_port_info port_info[PORT_NUM];
    jws_port_info_get(port_info);
    uint8_t idx,hport;
    for(idx = 0;idx < PORT_NUM;idx++)
    {
        hport = port_info[idx].physical_seq;
        pRpp->l2hport[idx] = hport;
        pRpp->h2lport[hport] = idx;
    }

#if 1
    crpp_init(pRpp->mports,pRpp->mac);
#endif

    return 0;
}

void br_pdu_rcv(unsigned char *data, int len)
{
    // ensure uniform format
    if(!(data[PDU_DSA_OFFSET] & DSA_TAG_BIT))
    {
        unsigned char *offset = data + PDU_DSA_OFFSET + 2;
        do{
            *offset = *(offset + 4);
        }while(++offset != (data + PDU_SLEN));
        
        if(len != PDU_LLEN)
        {
            LOG_ERROR("rpp pdu(no dsa-tag) len = %d error !\n",len);
            RPP_IN_check_pdu_header((RPP_PDU_T *)data,len);
            return;
        }
    }
    else
    {
        if(len != PDU_SLEN)
        {
            LOG_ERROR("rpp pdu(dsa-tag) len = %d error !\n",len);
            RPP_IN_check_pdu_header((RPP_PDU_T *)data,len);
            return;
        }
    }

    RPP_HEADER_T *hdr = (RPP_HEADER_T *)data;

    // get logical port
    uint8_t lport;
    if(RPP_OUT_h2lport(hdr->dsa_tag[1] >> 3,&lport))
    {
        LOG_ERROR("get logical port fail!\n");
        return;
    }

    switch(*(uint16_t *)hdr->pid)
    {
        case 0xbb00:
            rpp_pdu_rcv((RPP_PDU_T *)data,lport);
            break;
        case 0xbc00:
            crpp_pdu_rcv((RPP_PDU_T *)data,lport);
            break;
        default:
            LOG_ERROR("pid = 0x%x is not define",*(uint16_t *)hdr->pid);
            return;
    }
    
}

void br_notify(int lport, uint32_t link)
{
    if(lport >= PORT_CPU_L || lport <= 0)
        return;

    struct port_s *port = &pRpp->mports[lport - 1];
    if(link != port->link)
    {
        port->link = link;

        switch(port->role)
        {
            case COMMON_PORT:
                if(port->opt_link != NULL)
                    *port->opt_link = link;

                if(link == LINK_DOWN)
                    port_inactive(port,lport);
                else
                    crpp_port_detect(port,lport);
                break;
            case JRPP_PORT:
                {
                ring_t *pRing = rpp_find_ring(lport,0);
                if(pRing == NULL)
                    return;

                if(pRing->enable != RING_ENABLED)
                    return;

                RPP_IN_port_link_check(&pRing->rpp_ring,lport,link);
                }
                break;
            case RSTP_PORT:
                log_info("rstp port,ignore...");
                break;
            case IEEE802_1X:
                log_info("ieee802.1x port,ignore...");
                break;
        }
    }
}

void sig_kill_handler(int signo)
{
    log_info("----------------- recv SIGTERM signal = %d",signo);
    ring_t *pRing,*pRing_bak;  
    list_for_each_entry_safe(pRing, pRing_bak, &pRpp->ring, node) 
    {
        if(pRing->enable == RING_ENABLED)
        {
            RPP_IN_ring_disable(&pRing->rpp_ring,MSG_NODEDOWN_RPP_DIS);
            pRing->enable = RING_DISABLED;
        }
    }
    
    _exit(0);
}

int RPP_OUT_flush_ports(uint8_t p1,uint8_t p2)
{
    log_info("flush ports");
    system("echo 0 > /sys/class/net/br-lan/bridge/flush");
    if(fal_mac_flush_port(0,p1) != SW_OK || fal_mac_flush_port(0,p2) != SW_OK)
    {
        LOG_ERROR("flush port %d and port %d error!",p1,p2);
        return -1;
    }

	return 0;
}

int RPP_OUT_h2lport(uint8_t hport, uint8_t *lport)
{
    if(hport == PORT_CPU_H || hport >= PORT_NUM)
        return -1;

    *lport = pRpp->h2lport[hport];

    return 0;
}

int RPP_OUT_l2hport(uint8_t lport, uint8_t *hport)
{
    if(lport >= PORT_NUM)
        return -1;

    *hport = pRpp->l2hport[lport];
    return 0;
}

// 设置端口stp
int RPP_OUT_set_port_stp(int port_index, ePortStpState state,ePortLinkState link)
{
    fal_port_dot1d_state_t dot1d_state;
    uint8_t br_state;

	switch(state) 
    {
		case FORWARDING:
			dot1d_state = FAL_PORT_DOT1D_STATE_FORWARDING;
            br_state    = BR_STATE_FORWARDING;
			break;
		case BLOCKING:
			dot1d_state = FAL_PORT_DOT1D_STATE_BLOCKING;
            br_state    = BR_STATE_BLOCKING;
			break;
		default:
            LOG_ERROR("state = %d is forbidden",state);
            return -1;
	}

	if(link == LINK_UP && rtnl_set_state(port_index, br_state,NULL)) 
    {
		LOG_ERROR("set kernel bridge state fail");	
		return -1;
	}
    if(fal_port_dot1d_state_set(0, port_index, dot1d_state) != SW_OK)
    {
		LOG_ERROR("set jssdk dot1d state fail");	
		return -1;
    }

	return 0;
}

// 获取MAC
const uint8_t *RPP_OUT_get_mac (void)
{
    return (uint8_t *)pRpp->mac;
}

// 发送报文
int RPP_OUT_tx_pdu (int port_index, unsigned char* pdu, size_t pdu_len)
{
    if(pdu_len != PDU_SLEN)
    {
		LOG_ERROR("rpp pdu len = %d is error,expect 90",pdu_len);	
		return -1;
    }

    pdu_pkt_send(port_index,pdu,pdu_len);
	return 0;
}

// 添加定时器(秒->毫秒)
int RPP_OUT_set_timer(struct uloop_timeout *timer, int msec)
{
    return uloop_timeout_set(timer,msec);   
}

int RPP_OUT_close_timer(struct uloop_timeout *timer)
{
    return uloop_timeout_cancel(timer);
}

// 获取定时器值
uint8_t RPP_OUT_get_time(rppRing_t *rppRing,TIMER_ID id)
{
    ring_t *ring = container_of(rppRing,ring_t,rpp_ring);

    if(id == AUTH_TIME)
        return ring->times.auth;
    if(id == BALLOT_TIME)
        return ring->times.ballot;
    if(id == FAIL_TIME)
        return ring->times.fail;
    if(id == HELLO_TIME)
        return ring->times.hello;

    return 0;
}

// 获取环id
uint16_t RPP_OUT_get_ringid(rppRing_t *rppRing)
{
    ring_t *ring = container_of(rppRing,ring_t,rpp_ring);

    return ring->ring_id;
}

NODE_ID_T *RPP_OUT_get_nodeid(rppRing_t *rppRing)
{
    ring_t *ring = container_of(rppRing,ring_t,rpp_ring);
    return &ring->node_id;
}

RING_STATE_T RPP_OUT_get_ringstate(rppRing_t *rppRing)
{
    ring_t *ring = container_of(rppRing,ring_t,rpp_ring);
    return ring->enable; 
}

int RPP_OUT_led_set(rppRing_t *rppRing,int value)
{
    if(rppRing == NULL)
        return ring_led_set(value);

    ring_t *ring = container_of(rppRing,ring_t,rpp_ring);
    if(ring->ring_id <= MAX_LED_RING)
        return ring_led_set(value);

    return 0;
}

int RPP_OUT_cmd_rsp(unsigned char *buf,unsigned int len,char *un,int fd,int type)
{
    struct msghdr msg;
    struct iovec iov[2];
	int mhdr[4] = {type,sizeof(int),len,0};

    msg.msg_name        = un;
    msg.msg_namelen     = sizeof(struct sockaddr_un);
    msg.msg_iov         = iov;
    msg.msg_iovlen      = 2;
    msg.msg_control     = NULL;
    msg.msg_controllen  = 0;
    iov[0].iov_base     = mhdr;
    iov[0].iov_len      = sizeof(mhdr);
    iov[1].iov_base     = buf;
    iov[1].iov_len      = len;

	int l = sendmsg(fd, &msg, MSG_NOSIGNAL);
    if(l != iov[0].iov_len + iov[1].iov_len)
    {
        LOG_ERROR("l = %d is error");
        return -1;
    }
    
    return 0;
}

int RPP_OUT_set_ring_state(int ring_id,int state,FROM_T from)
{
    int r = 0;
    ring_t *pRing = rpp_find_ring(ring_id,1);
    if(pRing == NULL)
    {
		LOG_ERROR("can't find ring #%d\n", ring_id);
		return -1;
    }

    if(pRing->rpp_ring.primary == NULL || pRing->rpp_ring.secondary == NULL)
    {
        LOG_ERROR("arguements for ring#%d is not completely\n", ring_id);
        return -1;
    }

    if(pRing->enable == state)
        return 0;

    if(state == RING_ENABLED)
    {
	    r = RPP_IN_ring_enable(&pRing->rpp_ring);
    }
    else	
    { 
        r = RPP_IN_ring_disable(&pRing->rpp_ring,(from == FROM_CLI_TRY)?MSG_NODEDOWN_RING_DIS:MSG_NODEDOWN_TOPO_DIS);
    }

    if(r) 
    {
		LOG_ERROR("rpp ring#%d %s failed\n",ring_id,(state == RING_ENABLED)?"enable":"disable");
		return -1;
	}
	
    pRing->enable = state;

    if(from != FROM_CLI_TRY)
    {
        if(state == RING_ENABLED)
            pRpp->running = RPP_ENABLED;
        
        if(rpp_save_ring(ring_id,state) < 0)
        {
            LOG_ERROR("rpp save ring#%d error",ring_id);
            return -1;
        }
    }

    // update port-pool
    RPP_RING_STATE_T status;
    RPP_IN_ring_get_state(&pRing->rpp_ring,&status);
    pRpp->mports[status.primary.port_no - 1].role   = (state == RING_ENABLED)?JRPP_PORT:COMMON_PORT;
    pRpp->mports[status.primary.port_no - 1].link   = LINK_DOWN;
    pRpp->mports[status.secondary.port_no - 1].role = (state == RING_ENABLED)?JRPP_PORT:COMMON_PORT;
    pRpp->mports[status.secondary.port_no - 1].link = LINK_DOWN;

    return CTL_update_ports();
}

/****************************************************************************************************
 *                                  API for CLI
 * *************************************************************************************************/
int CTL_enable_jrpp(int enable)
{
    pRpp->running = enable;

	log_info("Ring Protect Protocol is %s\n",(enable == RPP_ENABLED)?"enable":"disable");

	return 0;
}

int CTL_enable_ring(int ring_id, int enable) 
{
    if(pRpp->running != RPP_ENABLED)
    {
        LOG_ERROR("rpp is disabled\n");
        return 0;
    }

    if(RPP_OUT_set_ring_state(ring_id,enable,FROM_CLI_TRY) < 0)
        return -1;

	log_info("ring#%d %s success\n",ring_id,(enable == RING_ENABLED)?"enable":"disable");

	return 0;
}

int CTL_add_ring(int ring_id) 
{
    if(rpp_find_ring(ring_id,1) != NULL)
    {
        LOG_ERROR("warning: ring #%d already added", ring_id);
        return -1;
    }

    ring_t *pRing = (ring_t *)calloc(1,sizeof(ring_t));
    list_add_tail(&pRing->node,&pRpp->ring);
    pRing->enable   = RING_DISABLED;
    pRing->ring_id  = ring_id;
    pRing->times.auth   = default_ring_cfg.auth_time;
    pRing->times.ballot = default_ring_cfg.ballot_time;
    pRing->times.fail   = default_ring_cfg.fail_time;
    pRing->times.hello  = default_ring_cfg.hello_time;
    pRing->node_id.prio = default_ring_cfg.node_priority;
    memcpy(pRing->node_id.addr,pRpp->mac,MAC_LEN);
    
    if(RPP_IN_ring_create(&pRing->rpp_ring))
    {
        LOG_ERROR("create rpp ring fail!\n");
        return -1;
    }
    
	log_info("add ring#%d OK\n",ring_id);
	return 0;
}

int CTL_del_ring(int ring_id) 
{
    ring_t *pRing = rpp_find_ring(ring_id,1);
    if(pRing == NULL)
    {
        LOG_ERROR("ring #%d is not exist", ring_id);
        return -1;
    }

    if(pRing->enable == RING_ENABLED)
    {
        LOG_ERROR("ring #%d must be closed before delete", ring_id);
        return -1;
    }

	return 0;
}

int CTL_add_ringport(int ring_id, int p_port, int s_port) 
{
    if(rpp_find_ring(p_port,0) || rpp_find_ring(s_port,0))
    {
		LOG_ERROR("port %d or %d has been used for rpp!\n", p_port,s_port);
		return -1;
    }

    ring_t *pRing = rpp_find_ring(ring_id,1);
    if(pRing == NULL)
    {
        LOG_ERROR("ring #%d is not exist", ring_id);
        return -1;
    }

    if(RPP_IN_port_create(&pRing->rpp_ring,p_port,s_port))
    {
        LOG_ERROR("rpp ring#%d add primary port %d and secondary port %d fail!\n",ring_id,p_port,s_port);
        return -1;
    }

    // set opt_link point to port's link_st
    pRpp->mports[p_port - 1].opt_link = (int *)pRing->rpp_ring.primary;
    pRpp->mports[s_port - 1].opt_link = (int *)pRing->rpp_ring.secondary;

	log_info("ring#%d add port success\n",ring_id);
	return 0;
}

int CTL_del_ringport(int ring_id) 
{
    ring_t *pRing = rpp_find_ring(ring_id,1);
    if(pRing == NULL)
    {
        LOG_ERROR("ring #%d is not exist", ring_id);
        return -1;
    }

    if(RPP_IN_port_delete(&pRing->rpp_ring))
    {
        LOG_ERROR("delete rpp ring#%d ports fail!\n",ring_id);
        return -1;
    }

    // clear opt_link
    pRpp->mports[pRing->rpp_ring.primary->port_index - 1].opt_link = NULL;
    pRpp->mports[pRing->rpp_ring.secondary->port_index - 1].opt_link = NULL;

	return 0;
}

int CTL_set_ring_config(int ring_id, RPP_RING_CFG_T *ring_cfg) 
{
    ring_t *pRing = rpp_find_ring(ring_id,1);
    if(pRing == NULL)
    {
		LOG_ERROR("can't find ring #%d", ring_id);
		return -1;
    }
    if(pRing->enable == RING_ENABLED)
    {
		LOG_ERROR("please close ring#%d before set parameter", ring_id);
		return -1;
    }

    if(ring_cfg->field_mask & RING_CFG_PRIO)
    {
        pRing->node_id.prio = ring_cfg->node_priority;
    }
    if(ring_cfg->field_mask & RING_CFG_FAIL)
    {
        pRing->times.fail   = ring_cfg->fail_time;
    }
    if(ring_cfg->field_mask & RING_CFG_BALLOT)
    {
        pRing->times.ballot = ring_cfg->ballot_time;
    }
    if(ring_cfg->field_mask & RING_CFG_HELLO)
    {
        pRing->times.hello  = ring_cfg->hello_time;
    }
    if(ring_cfg->field_mask & RING_CFG_AUTH)
    {
        pRing->times.auth   = ring_cfg->auth_time;
    }

	log_info("ring#%d set config success\n",ring_id);
	return 0;
}

int CTL_get_ring_state(int ring_id, RPP_RING_STATE_T *state) 
{
    memset(state,0,sizeof(RPP_RING_STATE_T));

    state->rpp_state = pRpp->running;
    if(pRpp->running != RPP_ENABLED)
        return 0;

    ring_t *pRing = rpp_find_ring(ring_id,1);
    if(pRing == NULL)
    {
		LOG_ERROR("can't find ring #%d", ring_id);
		return -1;
    }

    state->ring_state= pRing->enable;
    memcpy(&state->node_id,&pRing->node_id,sizeof(NODE_ID_T));
	if(RPP_IN_ring_get_state(&pRing->rpp_ring, state)) 
    {
		LOG_ERROR(" get ring state failed");
		return -1;
	}

	return 0;
}

int CTL_set_debug_level(int level)
{
	log_info("CTL_set_debug_level, level=%d\n", level);
    log_threshold(level);

	return 0;
}

int CTL_get_ring_topo_s(int ring_id,char *un,int fd)
{
    ring_t *pRing = rpp_find_ring(ring_id,1);
    if(pRing == NULL)
    {
        LOG_ERROR("can't find ring #%d", ring_id);
        return -1;
    }

    RPP_IN_ring_get_topo(&pRing->rpp_ring,un,fd);

    return 0;
}

int CTL_update_ports(void)
{
    return rtnl_get_link();
}

// attention! make sure ring is in fault status calling settopo cmd
int CTL_set_topo(int ring_id,int state)
{
    ring_t *pRing = rpp_find_ring(ring_id,1);
    if(RPP_IN_ring_set_topo(&pRing->rpp_ring,state) < 0)
        return -1;

    if(RPP_OUT_set_ring_state(ring_id,state,FORM_CLI_FORCE) < 0)
        return -1;

    return 0;
}

