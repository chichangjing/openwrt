
#include "base.h"
#include "ring.h"
#include "rpp_to.h"
#include "machine.h"

static int _rpp_ring_init_machine (STATE_MACH_T* this)
{
	this->state = BEGIN;
	(*(this->enter_state)) (this);
	return 0;
}

static int _rpp_ring_iterate_machines (rppRing_t *this, int (*iter_callb)(STATE_MACH_T*), Bool exit_on_non_zero_ret)
{
	register STATE_MACH_T   *stater;
	int iret, mret = 0;

	/* state machines per port */
    for (stater = this->primary->machines; stater; stater = stater->next) 
    {
        iret = (*iter_callb) (stater);
        if (exit_on_non_zero_ret && iret)
            return iret;
        else
            mret += iret;
    }

    for (stater = this->secondary->machines; stater; stater = stater->next) 
    {
        iret = (*iter_callb) (stater);
        if (exit_on_non_zero_ret && iret)
            return iret;
        else
            mret += iret;
    }
    
    /* state machines per ring */
	for (stater = this->machines; stater; stater = stater->next) {
		iret = (*iter_callb) (stater);
		if (exit_on_non_zero_ret && iret)
			return iret;
		else
			mret += iret;
	}

	return mret;
}

static int _ring_settopo_handler(rppRing_t *ring,tRMsgCmd *cmd)
{
    static RPP_RING_TOPO_T topo;
    static Bool chain = False;

    if(!topo.num)
    {
        topo.node_ret[0].ret = 0;
        memcpy(topo.node_ret[0].mac,RPP_OUT_get_nodeid(ring)->addr,MAC_LEN);
        topo.num = 1;
        if(ring->primary->link_st == LINK_DOWN || ring->secondary->link_st == LINK_DOWN)
            chain = True;
    }
    if(cmd != NULL)
    {
        if(cmd->type != CMD_RING_SET_RSP)
        {
            LOG_ERROR("type in cmd = %d does not match %s",cmd->type,__func__);
            return -1;
        }
    
        log_info("set topo handler get one: [%02x:%02x:%02x:%02x:%02x:%02x]",\
                 cmd->ctl_mac[0],cmd->ctl_mac[1],cmd->ctl_mac[2],cmd->ctl_mac[3],cmd->ctl_mac[4],cmd->ctl_mac[5]);

        topo.node_ret[topo.num].ret = cmd->rsp_ret;
        memcpy(topo.node_ret[topo.num].mac,cmd->ctl_mac,MAC_LEN);
        topo.num++;
        switch(cmd->rsp_node)
        { 
            case CMD_RET_NODE_NOT_LAST:
                return 0;
            case CMD_RET_LAST_NODE_CHAIN:
                if(!chain)
                {
                    chain = True;
                    return 0;
                }
                else
                {
                    chain = False;
                    break;
                }
            case CMD_RET_LAST_NODE_RING:
                break;
            default:
                LOG_ERROR("rsp node type = %d undefine",cmd->rsp_node);
                break;
        }
    }
    else
    {
        log_info("cmd time out");
        chain = False;
    }

    log_info("%d node info get",topo.num);
    RPP_OUT_cmd_rsp((unsigned char *)&topo,1 + topo.num * sizeof(RPP_NODE_RET_T),ring->cmd_req.u_name,ring->cmd_req.ctl_fd,112);
    RPP_OUT_close_timer(&ring->cmd_req.cmd_timer);
    topo.num = 0;
    return 0;
}

static int _ring_gettopo_handler(rppRing_t *ring,tRMsgCmd *cmd)
{
    static RPP_RING_TOPO_T topo;
    static Bool chain = False;
    
    if(!topo.num)
    {
        topo.node[0].state      = RPP_OUT_get_ringstate(ring); 
        topo.node[0].rpp_mode   = ring->rpp_mode;
        topo.node[0].status     = ring->status;
        topo.node[0].node_role  = ring->node_role;
        memcpy(&topo.node[0].node_id,RPP_OUT_get_nodeid(ring),sizeof(NODE_ID_T));
        memcpy(&topo.node[0].master_id,&ring->master_id,sizeof(NODE_ID_T));

        topo.node[0].primary.port_no    = ring->primary->port_index;
        topo.node[0].primary.role       = ring->primary->port_role;
        topo.node[0].primary.stp        = ring->primary->stp_st;
        if(ring->primary->neighber_vaild)
            memcpy(topo.node[0].primary.neighber_mac,ring->primary->neighber_mac,MAC_LEN); 
        topo.node[0].secondary.port_no  = ring->secondary->port_index;
        topo.node[0].secondary.role     = ring->secondary->port_role;
        topo.node[0].secondary.stp      = ring->secondary->stp_st;
        if(ring->secondary->neighber_vaild)
            memcpy(topo.node[0].secondary.neighber_mac,ring->secondary->neighber_mac,MAC_LEN); 

        topo.num = 1;
        if(ring->primary->link_st == LINK_DOWN || ring->secondary->link_st == LINK_DOWN)
            chain = True;
    }
    if(cmd != NULL)
    {
        if(cmd->type != CMD_RING_TOPO_RSP)
        {
            LOG_ERROR("type in cmd = %d does not match %s",cmd->type,__func__);
            return -1;
        }
    
        log_info("ring topo handler get one: [%02x:%02x:%02x:%02x:%02x:%02x]",\
                 cmd->ctl_mac[0],cmd->ctl_mac[1],cmd->ctl_mac[2],cmd->ctl_mac[3],cmd->ctl_mac[4],cmd->ctl_mac[5]);

        // use this method to avoid duplication temporary
        int i;
        for(i = topo.num;i >= 0;i--)
        {
            if(!memcmp(topo.node[i].node_id.addr,cmd->ctl_mac,MAC_LEN))
            {
                log_info("duplication node rsp,end current req");
                goto end_req;
            }
        }

        
        topo.node[topo.num].state           = (cmd->rsp_node_status.ring_info & RSP_NODE_STATE_BIT) >> 6;
        topo.node[topo.num].rpp_mode        = (cmd->rsp_node_status.ring_info & RSP_NODE_MODE_BIT)  >> 5;
        topo.node[topo.num].status          = (cmd->rsp_node_status.ring_info & RSP_NODE_STATUS_BIT)>> 4;
        topo.node[topo.num].node_role       = (cmd->rsp_node_status.ring_info & RSP_NODE_ROLE_BIT)  >> 3;
        topo.node[topo.num].node_id.prio    = cmd->rsp_node_status.ring_info & RSP_NODE_PRIO_BITS;
        memcpy(topo.node[topo.num].node_id.addr,cmd->ctl_mac,MAC_LEN);
        memcpy(&topo.node[topo.num].master_id,&cmd->rsp_node_status.master_id,sizeof(NODE_ID_T));

        topo.node[topo.num].primary.port_no    = cmd->rsp_node_status.port[0].port_no;
        topo.node[topo.num].primary.role       = (cmd->rsp_node_status.port[0].port_info & RSP_PORT_ROLE_BIT) >> 2;
        topo.node[topo.num].primary.stp        = cmd->rsp_node_status.port[0].port_info & RSP_PORT_STP_BITS;
        memcpy(topo.node[topo.num].primary.neighber_mac,cmd->rsp_node_status.port[0].neigbor_mac,MAC_LEN); 
        topo.node[topo.num].secondary.port_no  = cmd->rsp_node_status.port[1].port_no;
        topo.node[topo.num].secondary.role     = (cmd->rsp_node_status.port[1].port_info & RSP_PORT_ROLE_BIT) >> 2;
        topo.node[topo.num].secondary.stp      = cmd->rsp_node_status.port[1].port_info & RSP_PORT_STP_BITS;
        memcpy(topo.node[topo.num].secondary.neighber_mac,cmd->rsp_node_status.port[1].neigbor_mac,MAC_LEN); 
        topo.num++;
        switch(cmd->rsp_node)
        { 
            case CMD_RET_NODE_NOT_LAST:
                return 0;
            case CMD_RET_LAST_NODE_CHAIN:
                if(!chain)
                {
                    chain = True;
                    return 0;
                }
                else
                {
                    chain = False;
                    break;
                }
            case CMD_RET_LAST_NODE_RING:
                break;
            default:
                LOG_ERROR("rsp node type = %d undefine",cmd->rsp_node);
                break;
        }
    }
    else
    {
        log_info("cmd time out");
        chain = False;
    }
    
end_req:
    log_info("%d node info get",topo.num);
    RPP_OUT_cmd_rsp((unsigned char *)&topo,1 + topo.num * sizeof(RPP_NODE_SIMPL_T),ring->cmd_req.u_name,ring->cmd_req.ctl_fd,110);
    RPP_OUT_close_timer(&ring->cmd_req.cmd_timer);
    memset(&topo,0,sizeof(RPP_RING_TOPO_T));
    return 0;
}

int ring_start (rppRing_t *ring)
{
    port_start(ring->primary);
    port_start(ring->secondary);

    ring_reset(ring);
    memcpy(ring->endpoint,RPP_OUT_get_mac(),MAC_LEN);
	_rpp_ring_iterate_machines (ring, _rpp_ring_init_machine, False);

	return 0;
}

int ring_stop (rppRing_t *ring,eMsgNodeDownType type)
{
    port_stop(ring->primary);
    port_stop(ring->secondary);
    // ring must be in fault status when stop,later resolve it
    node_deal_down(ring,type);

    return 0;
}

int ring_update (rppRing_t *this)
{
	register Bool need_state_change;
	register int  number_of_loops = 0;

	need_state_change = False; 

	for (;;) 
    {
		need_state_change = _rpp_ring_iterate_machines (this, RPP_check_condition, True);

		if (! need_state_change) 
			return number_of_loops;

		number_of_loops++;
		number_of_loops += _rpp_ring_iterate_machines (this, RPP_change_state, False);
	}

	return number_of_loops;
}

int ring_set_state(rppRing_t *ring,RING_STATUS_T state)
{
    ring->status = state;
		return RPP_OUT_led_set(ring,(state == RPP_HEALTH)?LED_OPEN:LED_CLOSE);
}

void ring_set_topo(rppRing_t *ring,char *un,int fd,unsigned char state)
{
    if(timer_get_status(&ring->cmd_req.cmd_timer) == True)
    {
        LOG_ERROR("cmd timer is running,abandon new cmd");
        return;
    }

    if(fd)
    {
        memcpy(ring->cmd_req.u_name,un,109);
        ring->cmd_req.ctl_fd    = fd;
        ring->cmd_req.cmd_func  = _ring_settopo_handler;
        
        RPP_OUT_set_timer(&ring->cmd_req.cmd_timer,3000);
    }
    ring->cmd_req.ref       = 0;
    
    if(ring->primary->link_st == LINK_UP)
    {
        ring_build_cmd_req(ring,CMD_RING_SET_REQ,&state,sizeof(unsigned char));
        tx_cmd(ring->primary);
    }
    if(ring->secondary->link_st == LINK_UP)
    {
        ring_build_cmd_req(ring,CMD_RING_SET_REQ,&state,sizeof(unsigned char));
        tx_cmd(ring->secondary);
    }
}

void ring_get_topo(rppRing_t *ring,char *un,int fd)
{
    if(timer_get_status(&ring->cmd_req.cmd_timer) == True)
    {
        LOG_ERROR("cmd timer is running,abandon new cmd");
        return;
    }
        
    memcpy(ring->cmd_req.u_name,un,109);
    ring->cmd_req.ctl_fd    = fd;
    ring->cmd_req.cmd_func  = _ring_gettopo_handler;
    ring->cmd_req.ref       = 0;

    if(ring->status == RPP_HEALTH)
    {
        ring_build_cmd_req(ring,CMD_RING_TOPO_REQ,NULL,0);
        tx_cmd(ring->primary);
    }
    else
    {
        if(ring->primary->link_st == LINK_UP)
        {
            ring_build_cmd_req(ring,CMD_RING_TOPO_REQ,NULL,0);
            tx_cmd(ring->primary);
        }
        if(ring->secondary->link_st == LINK_UP)
        {
            ring_build_cmd_req(ring,CMD_RING_TOPO_REQ,NULL,0);
            tx_cmd(ring->secondary);
        }
    }
    RPP_OUT_set_timer(&ring->cmd_req.cmd_timer,3000);
}

void ring_build_cmd_req(rppRing_t *ring,eMsgCmdCode type,unsigned char *data,int len)
{
    tRMsgCmd *cmd = &ring->cmd_req.cmd_msg;

    cmd->type  = type;
    if(data != NULL)
        memcpy(cmd->data,data,len);

    ring->cmd_req.ref++;
}

void ring_build_cmd_rsp(rppRing_t *ring,rppPort_t *peer,eMsgCmdCode type,unsigned char *req_mac,void *data)
{
    tRMsgCmd *cmd = &ring->cmd_req.cmd_msg;

    if(cmd->type)
        LOG_ERROR("cmd buffer is not empty!");

    cmd->type  = type;
    memcpy(cmd->ctl_mac,req_mac,MAC_LEN);
    if(ring->status == RPP_HEALTH)
    {
        if(memcmp(req_mac,peer->neighber_mac,MAC_LEN))
            cmd->rsp_node = CMD_RET_NODE_NOT_LAST;
        else
            cmd->rsp_node = CMD_RET_LAST_NODE_RING;
    }
    else
    {
        if(peer->link_st == LINK_UP)
            cmd->rsp_node = CMD_RET_NODE_NOT_LAST;
        else
            cmd->rsp_node = CMD_RET_LAST_NODE_CHAIN;
    }

    switch(type)
    {
        case CMD_RING_SET_RSP:
            if(data == NULL)
            {
                LOG_ERROR("data lost!"); 
                return;
            }
            cmd->rsp_ret = *(int *)data;
            break;
        case CMD_RING_TOPO_RSP:
            cmd->rsp_node_status.ring_info  = (unsigned char)RPP_OUT_get_ringstate(ring) << 6;
            cmd->rsp_node_status.ring_info  |= (unsigned char)ring->rpp_mode    << 5;
            cmd->rsp_node_status.ring_info  |= (unsigned char)ring->status      << 4;
            cmd->rsp_node_status.ring_info  |= (unsigned char)ring->node_role   << 3;
            cmd->rsp_node_status.ring_info  |= RPP_OUT_get_nodeid(ring)->prio;
            memcpy(&cmd->rsp_node_status.master_id,&ring->master_id,sizeof(NODE_ID_T));

            RPP_PORT_STATE_T status;
            if(ring->primary != NULL)
            {
                port_get_state(ring->primary,&status);
                cmd->rsp_node_status.port[0].port_no    = status.port_no; 
                cmd->rsp_node_status.port[0].port_info  = (unsigned char)status.role << 3; 
                cmd->rsp_node_status.port[0].port_info  |= (unsigned char)status.dot1d_state; 
                memcpy(cmd->rsp_node_status.port[0].neigbor_mac,status.neigbor_mac,MAC_LEN); 
            }
            if(ring->secondary != NULL)
            {
                port_get_state(ring->secondary,&status);
                cmd->rsp_node_status.port[1].port_no    = status.port_no; 
                cmd->rsp_node_status.port[1].port_info  = (unsigned char)status.role << 3; 
                cmd->rsp_node_status.port[1].port_info  |= (unsigned char)status.dot1d_state; 
                memcpy(cmd->rsp_node_status.port[1].neigbor_mac,status.neigbor_mac,MAC_LEN); 
            }
            break;
        default:
            return;
    }
    ring->cmd_req.ref++;
}

void ring_build_complete_req(rppRing_t *ring,eMsgCompleteType type)
{
    tRMsgComplete *complete = &ring->primary->flag.complete;

    if(complete->type)
        LOG_ERROR("port %d complete buffer is not empty!",ring->primary->port_index);

    complete->type  = type;
}

void ring_build_linkdown_req(rppPort_t *this,rppPort_t *peer)
{
    tRMsgReport *down = &peer->flag.down;

    if(down->type)
        LOG_ERROR("port %d linkdown buffer is not empty!",peer->port_index);

    down->ext_neighbor_port = this->neighber_port;
    memcpy(down->ext_neighbor_mac,this->neighber_mac,MAC_LEN);

    if(this->stp_st == FORWARDING)
    {
        port_set_stp(peer,FORWARDING);
        down->type = MSG_LINKDOWN_REQ_FLUSH_FDB;
        
        RPP_OUT_flush_ports(this->port_index,peer->port_index);
    } 
    else
    {
        down->type = MSG_LINKDOWN_REQ_UPDATE_NODE;
    }
}

void ring_build_nodedown_req(rppRing_t *ring,rppPort_t *port,eMsgNodeDownType type)
{
    tRMsgNodeDown *down = &port->flag.ndown;

    if(down->type)
        LOG_ERROR("port %d nodedown buffer is not empty!",port->port_index);

    down->type = type;
}

