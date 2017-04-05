#include "base.h"
#include "ring.h"
#include "rpp_in.h"
#include "rpp_to.h"
#include "machine.h"

#define STATES {\
                CHOOSE(NODE_COMPLETE),\
                CHOOSE(NODE_FAILED),\
                CHOOSE(NODE_LINK_UP),\
                CHOOSE(NODE_LINK_DOWN),\
                CHOOSE(NODE_PRE_FORWARDING)\
}

#define GET_STATE_NAME RPP_node_get_state_name 
#include "choose.h"

#define _node_fwd_report port_fwd
#define _node_fwd_complete port_fwd
#define _node_fwd_cmd(peer,pdu) do{\
        memcpy(&peer->fwding,pdu,sizeof(RPP_PDU_T));\
        tx_forwarding(peer);}while(0)


// forwarding hello packet
static void _node_fwd_hello(rppPort_t *port, RPP_PDU_T *pdu,unsigned short bline,unsigned char bpoints)
{
    memcpy(&port->fwding,pdu,sizeof(RPP_PDU_T));                        

    if(ring_get_mode(port->owner) == RPP_ENHANCED)
        port->fwding.body.res3[0] += bpoints;

    port->fwding.body.msg.hello.txport_state                        = port->stp_st;
    *(unsigned short *)port->fwding.body.msg.hello.block_line_num   = htons(bline);        
    tx_forwarding(port);
}


/* fail timer timeout means ring fault 
 * forwarding port authing ok if previous status is fault already
 * master node should close hello timer
 **/
void fail_timer_handler(struct uloop_timeout *timer)
{
    rppRing_t *ring = rpp_get_ring_addr(timer,fail_timer);

    log_info("this is in fail timer");

    if(ring->status == RPP_FAULT)
    {
        // forwarding认证成功的端口
        if(ballot_if_fin(ring->primary))
            port_set_stp(ring->primary,FORWARDING);
        if(ballot_if_fin(ring->secondary))
            port_set_stp(ring->secondary,FORWARDING);
    }
    else
    {
        ring_set_state(ring,RPP_FAULT);
    }

#if 0	// 这段代码的作用是fail定时器超时后主节点不再发hello报文
    if(ring->node_role == NODE_TYPE_MASTER)
        RPP_OUT_close_timer(&ring->hello_timer);
#endif

	ring_update (ring);
}

void hello_timer_handler(struct uloop_timeout *timer)
{
    rppRing_t *ring = rpp_get_ring_addr(timer,hello_timer);
    // check if node role is master
    if(ring->node_role != NODE_TYPE_MASTER)
    {
        LOG_ERROR("node role is not master,should not open hello timer[master-%02x:%02x:%02x:%02x:%02x:%02x]",\
                  ring->master_id.addr[0],ring->master_id.addr[1],ring->master_id.addr[2],\
                  ring->master_id.addr[3],ring->master_id.addr[4],ring->master_id.addr[5]);
        RPP_OUT_close_timer(&ring->hello_timer);
        return;
    }

    tx_hello(ring->primary);
    RPP_OUT_set_timer(timer,1000 * RPP_OUT_get_time(ring,HELLO_TIME));

	ring_update (ring);
}

void cmd_timer_handler(struct uloop_timeout *timer)
{
    rppRing_t *ring = rpp_get_ring_addr(container_of(timer,tCmdReq,cmd_timer),cmd_req);

    if(ring->cmd_req.cmd_func != NULL && ring->cmd_req.cmd_func(ring,NULL))
        LOG_ERROR("cmd timer handler error");
}

void node_role_handler(rppRing_t *this,eNodeType role,NODE_ID_T *master_id,Bool cycle)
{
    //log_info("current node has get a role [%s]",(role == NODE_TYPE_MASTER)?"master":"transit");
    memcpy(&this->master_id,master_id,sizeof(NODE_ID_T));
    this->switch_cnts   = 0;   
    this->hello_syn     = 0;
    if(role == NODE_TYPE_MASTER)
    {
        this->node_role     = NODE_TYPE_MASTER;

        // if cycle determine master set hello and fail timer
        if(cycle)
        {
            RPP_OUT_set_timer(&this->hello_timer,1000 *RPP_OUT_get_time(this,HELLO_TIME));
            RPP_OUT_set_timer(&this->fail_timer,1000 * RPP_OUT_get_time(this,FAIL_TIME));
        }
        else
        {
            // master node will set port which pass auth forwarding 
            if(this->primary->neighber_vaild)
                port_set_stp(this->primary,FORWARDING);
            if(this->secondary->neighber_vaild)
                port_set_stp(this->secondary,FORWARDING);
        }
    }
    else
    {
        this->node_role     =  NODE_TYPE_TRANSIT;
        RPP_OUT_set_timer(&this->fail_timer,1000 * RPP_OUT_get_time(this,FAIL_TIME));
        // transit node should close hello timer if it has opened
        if(timer_get_status(&this->hello_timer) == True)
            RPP_OUT_close_timer(&this->hello_timer);
    }
}

int node_parse_hello_packet(rppPort_t *this, RPP_PDU_T *pdu)
{
    if(!ballot_if_fin(this))
    {
        LOG_ERROR("port %d is not ready to recv hello packet",this->port_index);
        return 0;
    }

    tRMsgHello *hello           = &pdu->body.msg.hello;
    rppRing_t *ring             = this->owner;
    unsigned short hello_syn    = ntohs(*(unsigned short *)pdu->body.hello_seq);
    unsigned short blockline    = ntohs(*(unsigned short *)hello->block_line_num);

    // compute blockline
    if(hello->txport_state == BLOCKING || this->stp_st == BLOCKING)
        blockline++;

        //LOG_ERROR("port %d recieve a packet for hello(%d,%d)",this->port_index,hello_syn,pdu->body.res3[0]);

    if(ring->node_role == NODE_TYPE_MASTER)
    {
        if(hello_syn != ring->hello_syn)
        {
            LOG_ERROR("expect hello seq = %d,recv hello seq = %d[%02x:%02x:%02x:%02x:%02x:%02x]",ring->hello_syn,hello_syn,\
                         pdu->hdr.src_mac[0],pdu->hdr.src_mac[1],pdu->hdr.src_mac[2],pdu->hdr.src_mac[3],pdu->hdr.src_mac[4],pdu->hdr.src_mac[5]);
        }
        if(memcmp(RPP_OUT_get_mac(),pdu->hdr.src_mac,MAC_LEN))
        {
            LOG_ERROR("master node recv a packet for hello,but src mac[%02x:%02x:%02x:%02x:%02x:%02x] is error!",\
                     pdu->hdr.src_mac[0],pdu->hdr.src_mac[1],pdu->hdr.src_mac[2],pdu->hdr.src_mac[3],pdu->hdr.src_mac[4],pdu->hdr.src_mac[5]);
            return -1;
        }

        // statistic block points is an enhanced feature
        if(ring_get_mode(ring) == RPP_ENHANCED)
        {
            pdu->body.res3[0] += (this->stp_st == BLOCKING);
            if(pdu->body.res3[0] != 2)
            {
                LOG_ERROR("block points num = %d in current ring,while expect 2 points",pdu->body.res3[0]);
            }
        }

        switch(blockline)
        {
            case 1:
                if(ring->status == RPP_FAULT)
                {
                    ring_build_complete_req(ring,MSG_COMPLETE_UPDATE_NODE);
                    tx_complete(ring->primary);
                }
                break;
            case 0:
                LOG_ERROR("no block port in ring!");
            default:
                // 主端口forwarding,副端口blocking
                port_set_stp(ring->primary,FORWARDING);
                port_set_stp(ring->secondary,BLOCKING);

                ring_build_complete_req(ring,MSG_COMPLETE_WITH_FLUSH_FDB);
                tx_complete(ring->primary);
                RPP_OUT_flush_ports(ring->primary->port_index,ring->secondary->port_index);
                break;
        }

        // 主节点切换到健康状态
        if(ring->status == RPP_FAULT)
        {
            ring_set_state(ring,RPP_HEALTH);
        }

        // reset fail timer
        RPP_OUT_set_timer(&ring->fail_timer,1000 * RPP_OUT_get_time(ring,FAIL_TIME));
    }
    else
    {
        // transit node update hello seq
        if(hello_syn != (ring->hello_syn + 1))
        {
            LOG_ERROR("expect hello seq = %d,recv hello seq = %d[%02x:%02x:%02x:%02x:%02x:%02x]",ring->hello_syn + 1,hello_syn,\
                         pdu->hdr.src_mac[0],pdu->hdr.src_mac[1],pdu->hdr.src_mac[2],pdu->hdr.src_mac[3],pdu->hdr.src_mac[4],pdu->hdr.src_mac[5]);
        }
        ring->hello_syn = hello_syn;

        // transit node abandon hello packet if peer port does not finish ballot
        rppPort_t *peer = this->peer;
        if(!ballot_if_fin(peer))
        {
            LOG_ERROR("port %d has not finished ballot,abandon hello packet(%d)",peer->port_index,hello_syn);
            return 0;
        }

        // transit node forwarding hello packet  
        int bpoints = (this->stp_st == BLOCKING) + (peer->stp_st == BLOCKING);
        _node_fwd_hello(peer,pdu,blockline,bpoints);

        // transit update ring status by hello packet
        if(ring->status != hello->ring_state)
        {
            ring_set_state(ring,hello->ring_state);
            // if it is a switch from health to fault,must close fail timer right now,otherwise,storm maybe occured
            if(ring->status == RPP_FAULT)
                RPP_OUT_close_timer(&ring->fail_timer);
        }

        // reset fail timer if ring in health status
        if(ring->status == RPP_HEALTH)
            RPP_OUT_set_timer(&ring->fail_timer,1000 * RPP_OUT_get_time(ring,FAIL_TIME));
    }

    return 0;
}

int node_parse_complete_packet(rppPort_t *this, RPP_PDU_T *pdu)
{
    tRMsgComplete *complete = &pdu->body.msg.complete;
    rppRing_t *ring         = this->owner;
    rppPort_t *peer         = this->peer;

    // master abandon complete packet
    if(ring->node_role == NODE_TYPE_TRANSIT)
    {
        /* transit forwarding complete packet 
         * change to health ring
         */
        _node_fwd_complete(this,pdu);
        ring_set_state(ring,RPP_HEALTH);

        switch(complete->type)
        {
            case MSG_COMPLETE_UPDATE_NODE:
                log_info("port %d recieve a packet for complete(only update node)",this->port_index);
                break;
            case MSG_COMPLETE_WITH_FLUSH_FDB:
                log_info("port %d recieve a packet for complete(update node and fdb)",this->port_index);
                port_set_stp(this,FORWARDING);
                
                /**<    if peer node for peer port is master,blocking peer port,otherwise,forwarding peer port */
                if(!memcmp(peer->neighber_mac,pdu->hdr.src_mac,MAC_LEN))
                    port_set_stp(peer,BLOCKING);
                else
                    port_set_stp(peer,FORWARDING);

                // flush fdb
                RPP_OUT_flush_ports(ring->primary->port_index,ring->secondary->port_index);
                break;
            default:
                LOG_ERROR("sub type of complete report is undefine");
                return -1;
        }
    }
    else
        log_info("port %d recieve a packet for self complete",this->port_index);

    // 复位fail定时器
    RPP_OUT_set_timer(&ring->fail_timer,1000 * RPP_OUT_get_time(ring,FAIL_TIME));

    return 0;
}

int node_parse_report_packet(rppPort_t *this, RPP_PDU_T *pdu)
{
    tRMsgReport *report = &pdu->body.msg.report;
    rppRing_t *ring     = this->owner;

    log_info("port %d recieve a packet for %s report",this->port_index,(report->type == MSG_ENDPOINT_TO_ENDPOINT)?"Ep2Ep":"Linkdown");
    ring->hello_syn = 0;
    if(report->type == MSG_ENDPOINT_TO_ENDPOINT)
    {
        if(ring->status != RPP_HEALTH)
        {
            if(ring->node_role == NODE_TYPE_TRANSIT)
            {
                if(!memcmp(report->ext_neighbor_mac,RPP_OUT_get_mac(),MAC_LEN))
                {
                    LOG_ERROR("peer endpoint recv ep2ep report packet");
                    return 0;
                }
                _node_fwd_report(this,pdu);
            }
            else
            {
                if(timer_get_status(&ring->fail_timer) != True) 
                {
                    ring->switch_cnts   = 0;
                    ring->hello_syn     = 0;

                    RPP_OUT_set_timer(&ring->hello_timer,1000 *RPP_OUT_get_time(ring,HELLO_TIME));
                    RPP_OUT_set_timer(&ring->fail_timer,1000 * RPP_OUT_get_time(ring,FAIL_TIME));
                }
            }
        }
    }
    else
    {
        if(ring->status == RPP_HEALTH)
        {
            /* transit forwarding linkdown packet
             * master do nothing if linkdown is just occured on peer port of master     
             */
            if(ring->node_role == NODE_TYPE_TRANSIT)
                _node_fwd_report(this,pdu);            
            else
            {
                if(!memcmp(report->ext_neighbor_mac,RPP_OUT_get_mac(),MAC_LEN))
                    return 0;
                else
                    RPP_OUT_close_timer(&ring->hello_timer);
            }

            // recieve linkdown packet means ring fault
            ring_set_state(ring,RPP_FAULT);
            RPP_OUT_close_timer(&ring->fail_timer);
            port_set_stp(ring->primary,FORWARDING);
            port_set_stp(ring->secondary,FORWARDING);

            // flush ports if necessary
            if(report->type == MSG_LINKDOWN_REQ_FLUSH_FDB)
            {
                ring->switch_cnts++;
                RPP_OUT_flush_ports(ring->primary->port_index,ring->secondary->port_index);
            }
        }
        else
        {
            /* transit node recv linkdown packet in fault status means master node does not in current line.
             * current port should do --ballot
             */
            if(ring->node_role != NODE_TYPE_MASTER)
            {
                ballot_ittv_minus(this);
            }
        }
    }

    return 0;
}

int node_parse_down_packet(rppPort_t *this, RPP_PDU_T *pdu)
{
    tRMsgNodeDown *ndown = &pdu->body.msg.nodedown;  
    rppRing_t *ring = this->owner;

    log_info("port %d recieve a packet for nodedown",this->port_index);
    if(ring->status == RPP_HEALTH)
    {
        LOG_ERROR("recv nodedown packet but ring is health");
        return -1;
    }

    port_reset(this);
    port_set_stp(this,BLOCKING);
    switch(ndown->type)
    {
        case MSG_NODEDOWN_RING_DIS:
            auth_stop(this);
            break;
        case MSG_NODEDOWN_RPP_DIS:
            auth_start(this);
            break;
        default:
            LOG_ERROR("sub type of nodedown packet is undefine");
            return -1;
    }

    return 0;
}

int node_parse_cmd_packet(rppPort_t *this, RPP_PDU_T *pdu,int (*if_cb)(rppRing_t *r,int type))
{
    tRMsgCmd *cmd = &pdu->body.msg.cmd;
    rppRing_t *ring = this->owner;
    rppPort_t *peer = this->peer;

    log_info("port %d recieve a packet for cmd[%d]",this->port_index,cmd->type);

    if(!memcmp(pdu->hdr.src_mac,RPP_OUT_get_mac(),MAC_LEN))
    {
        LOG_ERROR("get self node rsp ,abandon");
        return 0;
    }

    switch(cmd->type)
    {
        case CMD_RING_TOPO_REQ:
            if(peer->link_st == LINK_UP && memcmp(pdu->hdr.src_mac,peer->neighber_mac,MAC_LEN))
                _node_fwd_cmd(peer,pdu);

            ring_build_cmd_rsp(ring,peer,CMD_RING_TOPO_RSP,pdu->hdr.src_mac,NULL);
            tx_cmd(this);
            break;
        case CMD_RING_SET_REQ:
            if(peer->link_st == LINK_UP && memcmp(pdu->hdr.src_mac,peer->neighber_mac,MAC_LEN))
            {
                _node_fwd_cmd(peer,pdu);
	            ring_update(ring);
            }

            RPP_OUT_set_ring_state(ntohs(*(unsigned short *)pdu->body.ring_id),cmd->ring_state,FORM_CMD_MSG);
            //ring_build_cmd_rsp(ring,peer,CMD_RING_SET_RSP,pdu->hdr.src_mac,&ret);
            //tx_cmd(this);
            break;
        case CMD_RING_SET_RSP:
        case CMD_RING_TOPO_RSP:
            if(!memcmp(cmd->ctl_mac,RPP_OUT_get_mac(),MAC_LEN))
            {
                if(timer_get_status(&ring->cmd_req.cmd_timer) == True)
                {
                    // attention! ctl_name will change to store src mac there
                    memcpy(cmd->ctl_mac,pdu->hdr.src_mac,MAC_LEN);
                    ring->cmd_req.cmd_func(ring,cmd);
                }
                else
                    LOG_ERROR("get cmd packet (%d),but timeout",cmd->type);
                return 0;
            }
            if(peer->link_st == LINK_UP)
                _node_fwd_cmd(peer,pdu);
            break;
    }

    return 0;
}

void node_deal_link_down(rppRing_t *ring,rppPort_t *this)
{
    ring->hello_syn = 0;
    RPP_OUT_close_timer(&ring->fail_timer);
    RPP_OUT_close_timer(&ring->hello_timer);

    rppPort_t *peer = this->peer;
    if(ring->status == RPP_HEALTH)
    {
        ring_set_state(ring,RPP_FAULT);
        /* master:  if link-down port is in forwarding status,
         *          block it,forwarding peer port and flush all
         * transit: if link-down port is in forwarding status,
         *          block it,forwarding peer port,fluch all and report linkdown packet with flushing fdb.
         *          otherwise,just report linkdown packet with updating node
         **/
        if(ring->node_role == NODE_TYPE_MASTER)
        {
            if(this->stp_st == FORWARDING)
            {
                ring->switch_cnts++;
                port_set_stp(peer,FORWARDING);
                RPP_OUT_flush_ports(ring->primary->port_index,ring->secondary->port_index);
            }
        }
        else
        {
            ring_build_linkdown_req(this,peer);

            tx_report(peer);
        }

        // in health ring,port link down means neighber may become endpoint
        memcpy(ring->endpoint,this->neighber_mac,MAC_LEN);
    }
    else
    {
        /* only if linkdown port and peer port has authed pass,minus-ballot will do.
         * the condition should not change to "if ballot finish"
         */
        if(this->neighber_vaild)
        {
            if(peer->neighber_vaild)
                ballot_ittv_minus(peer);
            else
                memcpy(ring->endpoint,RPP_OUT_get_mac(),MAC_LEN);
        }
    }
}

void node_deal_down(rppRing_t *ring,eMsgNodeDownType type)
{
    if(type == MSG_NODEDOWN_TOPO_DIS)
        return;
    if(ring->primary->link_st == LINK_UP)
    {
        ring_build_nodedown_req(ring,ring->primary,type);
        tx_nodedown(ring->primary);
    }
    if(ring->secondary->link_st == LINK_UP)
    {
        ring_build_nodedown_req(ring,ring->secondary,type);
        tx_nodedown(ring->secondary);
    }
}

void node_deal_p2p(rppRing_t *ring,rppPort_t *port,unsigned char *mac)
{
    if(ring->node_role == NODE_TYPE_TRANSIT)
    {
        tRMsgReport *report = &port->flag.down;
        report->type = MSG_ENDPOINT_TO_ENDPOINT;
        memcpy(report->ext_neighbor_mac,mac,MAC_LEN);

        tx_report(port);
    }
    else
    {
        ring->switch_cnts   = 0;
        ring->hello_syn     = 0;

        RPP_OUT_set_timer(&ring->hello_timer,1000 *RPP_OUT_get_time(ring,HELLO_TIME));
        RPP_OUT_set_timer(&ring->fail_timer,1000 * RPP_OUT_get_time(ring,FAIL_TIME));
    }
}

void RPP_node_enter_state(STATE_MACH_T* this)
{
    switch(this->state)
    {
        case BEGIN:
            break;
        case NODE_COMPLETE:
            log_info("  \033[1;33m<node machine> in complete status!\033[0m");
            break;
        case NODE_FAILED:
            log_info("  \033[1;33m<node machine> in failed status!\033[0m");
            break;
        case NODE_LINK_UP:
            log_info("  \033[1;33m<node machine> in link-up status!\033[0m");
            break;
        case NODE_LINK_DOWN:
            log_info("  \033[1;33m<node machine> in link-down status!\033[0m");
            break;
        case NODE_PRE_FORWARDING:
            log_info("  \033[1;33m<node machine> in pre-forwarding status!\033[0m");
            break;
    }
}

#define node_if_linkup(r) (r->node_role == NODE_TYPE_TRANSIT && \
                           (r->primary->link_st & r->secondary->link_st) == LINK_UP && \
                           (r->primary->stp_st & r->secondary->stp_st) == FORWARDING) 
#define node_if_linkdown(r) (r->node_role == NODE_TYPE_TRANSIT && \
                           (r->primary->link_st & r->secondary->link_st) != LINK_UP)
#define node_if_prefwd(r) (r->node_role == NODE_TYPE_TRANSIT && \
                           (r->primary->link_st & r->secondary->link_st) == LINK_UP && \
                           (r->primary->stp_st & r->secondary->stp_st) != FORWARDING) 

Bool RPP_node_check_conditions(STATE_MACH_T* this)
{
    register rppRing_t *ring = this->owner.ring;

    switch(this->state)
    {
        case BEGIN:
            if(ring->node_role == NODE_TYPE_MASTER && ring->status == RPP_FAULT)
                return RPP_hop_2_state(this,NODE_FAILED);
            if(node_if_prefwd(ring))
                return RPP_hop_2_state(this,NODE_PRE_FORWARDING);
            if(node_if_linkdown(ring))
                return RPP_hop_2_state(this,NODE_LINK_DOWN);
            break;
        case NODE_COMPLETE:
            if(ring->status == RPP_FAULT)
                return RPP_hop_2_state(this,NODE_FAILED);
            break;
        case NODE_FAILED:
            if(ring->node_role == NODE_TYPE_MASTER && ring->status == RPP_HEALTH)
                return RPP_hop_2_state(this,NODE_COMPLETE);
            if(node_if_linkup(ring))
                return RPP_hop_2_state(this,NODE_LINK_UP);
            if(node_if_linkdown(ring))
                return RPP_hop_2_state(this,NODE_LINK_DOWN);
            if(node_if_prefwd(ring))
                return RPP_hop_2_state(this,NODE_PRE_FORWARDING);
            break;
        case NODE_LINK_UP:
            if(ring->node_role == NODE_TYPE_MASTER && ring->status == RPP_FAULT)
                return RPP_hop_2_state(this,NODE_FAILED);
            if(node_if_linkdown(ring))
                return RPP_hop_2_state(this,NODE_LINK_DOWN);
            if(node_if_prefwd(ring))
                return RPP_hop_2_state(this,NODE_PRE_FORWARDING);
            break;
        case NODE_LINK_DOWN:
            if(ring->node_role == NODE_TYPE_MASTER && ring->status == RPP_FAULT)
                return RPP_hop_2_state(this,NODE_FAILED);
            if(node_if_linkup(ring))
                return RPP_hop_2_state(this,NODE_LINK_UP);
            if(node_if_prefwd(ring))
                return RPP_hop_2_state(this,NODE_PRE_FORWARDING);
            break;
        case NODE_PRE_FORWARDING:
            if(ring->node_role == NODE_TYPE_MASTER && ring->status == RPP_FAULT)
                return RPP_hop_2_state(this,NODE_FAILED);
            if(node_if_linkdown(ring))
                return RPP_hop_2_state(this,NODE_LINK_DOWN);
            if(node_if_linkup(ring))
                return RPP_hop_2_state(this,NODE_LINK_UP);
            break;
        default:
            LOG_ERROR("node machine in error status!\n");
            break;
    }
    return False;
} 
