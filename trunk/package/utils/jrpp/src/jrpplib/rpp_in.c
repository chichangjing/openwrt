

/* This file contains API from OS to the JRPP library */

#include "base.h"
#include "ring.h"
#include "rpp_in.h"
#include "rpp_to.h"
#include "machine.h"

// 处理环网端口link状态改变情况
int RPP_IN_port_link_check(rppRing_t *this,int lport,int link_status)
{
    rppPort_t *port = port_get_owner(this,lport);
    port_link_change(port,link_status);
    
    ring_update (this);

    return 0;
}

// 检查主副端口是否合法
int RPP_IN_port_valid(rppRing_t *rppRing,int port)
{
    if(rppRing->primary != NULL)
    {
        if(rppRing->primary->port_index == port)
            return 1;
    }
    if(rppRing->secondary != NULL)
    {
        if(rppRing->secondary->port_index == port)
            return 1;
    }

    return 0;
}

int RPP_IN_ring_create (rppRing_t *this)
{
    RPP_CRITICAL_START;  

    this->status                = RPP_FAULT;
    this->node_role             = NODE_TYPE_TRANSIT;
    this->fail_timer.cb         = fail_timer_handler;
    this->hello_timer.cb        = hello_timer_handler;
    this->cmd_req.cmd_timer.cb  = cmd_timer_handler;

    RPP_STATE_MACH_IN_LIST(node);

    RPP_CRITICAL_END;

    return 0;
}

int RPP_IN_ring_delete (int ring_id)
{
	RPP_CRITICAL_START;  
	
	RPP_CRITICAL_END;

	return 0;
}

int RPP_IN_ring_enable (rppRing_t *this)
{
	RPP_CRITICAL_START;  
    
    ring_start(this);   
	ring_update(this);
 
	RPP_CRITICAL_END;

	return 0;
}

int RPP_IN_ring_disable (rppRing_t *this,eMsgNodeDownType type)
{
	RPP_CRITICAL_START;  

    if(this->status == RPP_HEALTH)
    {
        LOG_ERROR("make ring in fault status when disable a ring");
        return -1;
    }

    ring_stop(this,type);
	ring_update(this);
    
	RPP_CRITICAL_END;

	return 0;
}

int RPP_IN_port_create(rppRing_t *ring,int p_port,int s_port)
{
    if(ring->primary != NULL || ring->secondary != NULL)
    {
        LOG_ERROR("please delete old ports before add new port");
        return -1;
    }
    rppPort_t *this;

    this = (rppPort_t *)calloc(2,sizeof(rppPort_t));
    if(this == NULL)
    {
	    return -1;
    }

    RPP_CRITICAL_START;  

    RPP_STATE_MACH_IN_LIST(auth);
    RPP_STATE_MACH_IN_LIST(ballot);
    RPP_STATE_MACH_IN_LIST(transmit);
    this->owner             = ring;
    this->peer              = this + 1;
    this->port_role         = PRIMARY_PORT;
    this->auth_timer.cb     = auth_timer_handler;
    this->ballot_timer.cb   = ballot_timer_handler;
    this->link_st           = LINK_DOWN;
    this->port_index        = p_port;
    ring->primary   = this;

    this++;
    
    RPP_STATE_MACH_IN_LIST(auth);
    RPP_STATE_MACH_IN_LIST(ballot);
    RPP_STATE_MACH_IN_LIST(transmit);
    this->owner             = ring;
    this->peer              = this - 1;
    this->port_role         = SECONDARY_PORT;
    this->auth_timer.cb     = auth_timer_handler;
    this->ballot_timer.cb   = ballot_timer_handler;
    this->link_st           = LINK_DOWN;
    this->port_index        = s_port;
    ring->secondary = this;

    RPP_CRITICAL_END;

	return 0;
}

int RPP_IN_port_delete(rppRing_t *ring)
{
    rppPort_t *port = ring->secondary;

    RPP_state_mach_delete(port->auth);
    RPP_state_mach_delete(port->ballot);
    RPP_state_mach_delete(port->transmit);

    port = ring->primary;

    RPP_state_mach_delete(port->auth);
    RPP_state_mach_delete(port->ballot);
    RPP_state_mach_delete(port->transmit);

    free(port);

    ring->primary   = NULL;
    ring->secondary = NULL;

    return 0;
}

int RPP_IN_port_get_cfg (int ring_id, int port_index, RPP_PORT_CFG_T* port_cfg)
{
	return 0;
}

int RPP_IN_ring_get_state(rppRing_t *ring,RPP_RING_STATE_T *state)
{
    state->rpp_mode     = ring->rpp_mode;
    state->node_role    = ring->node_role;
    state->ring_status  = ring->status;
    state->hello_seq    = ring->hello_syn;
    memcpy(&state->master_id,&ring->master_id,sizeof(NODE_ID_T));
    strcpy(state->m_node_st,RPP_machine_get_state(node,ring->node));

    if(ring->primary != NULL)
        port_get_state(ring->primary,&state->primary);
    if(ring->secondary != NULL)
        port_get_state(ring->secondary,&state->secondary);

    return 0;
}

int RPP_IN_ring_get_topo(rppRing_t *ring,char *un,int fd)
{
    ring_get_topo(ring,un,fd);
    ring_update (ring);
    return 0;
}

int RPP_IN_ring_set_topo(rppRing_t *ring,RING_STATE_T state)
{
    if(ring->status == RPP_HEALTH)
    {
        LOG_ERROR("make ring in fault status when calling setopo cmd");
        return -1;
    }

    ring_set_topo(ring,NULL,0,state);
    ring_update (ring);
    return 0;
}

int RPP_IN_check_pdu_header (RPP_PDU_T* pdu, size_t len)
{
    if(len < sizeof(RPP_HEADER_T))
	    return -1;

    if(pdu->hdr.pid[0] != 0x00 || pdu->hdr.pid[1] != 0xbb)
    {
        LOG_ERROR("rpp header error");
        return -1;
    }

    if(len >= 36 && len <= 54)
    {
        LOG_ERROR("ring#%d message type = %d packet has error",ntohs(*(unsigned short *)pdu->body.ring_id),pdu->body.type);
        if(pdu->body.type == PACKET_HELLO)
        {
            LOG_ERROR("hello seq = %d will be abandon",ntohs(*(unsigned short *)pdu->body.hello_seq));
        }
        return -1;
    }

    return -1;
}

int RPP_IN_cmd_msg(rppRing_t *ring, int lport, RPP_PDU_T *pdu,int (*if_cb)(rppRing_t *r,int type))
{
    rppPort_t *port = port_get_owner(ring,lport);
    if(node_parse_cmd_packet(port,pdu,if_cb) < 0)
    {
        LOG_ERROR("node pase command packet error");
        return -1;
    }

	ring_update(ring);
    return 0;
}

int RPP_IN_rx_pdu (rppRing_t *ring, int port_index, RPP_PDU_T *pdu)
{
    rppPort_t *port = port_get_owner(ring,port_index);

    if(port->link_st != LINK_UP)
    {
        LOG_ERROR("port %d is in link-down status");
        return -1;
    }
    
    int ret;
    switch(pdu->body.type)
    {
        case PACKET_AUTHENTICATION:
            ret = auth_packet_parse(port,pdu);
            break;
        case PACKET_BALLOT:
            ret = ballot_packet_parse(port,pdu);   
            break;
        case PACKET_HELLO:
            ret = node_parse_hello_packet(port,pdu);   
            break;
        case PACKET_COMPLETE:
            ret = node_parse_complete_packet(port,pdu);
            break;
        case PACKET_REPORT:
            ret = node_parse_report_packet(port,pdu);
            break;
        case PACKET_NODEDOWN:
            ret = node_parse_down_packet(port,pdu);
            break;
        default:
            LOG_ERROR("message type = %d in packet is undefine",pdu->body.type);
            return -1;
    }

	ring_update(ring);
	return ret;
}

const char* RPP_IN_get_error_explanation (int rpp_err_no)
{
#define CHOOSE(a) #a
	static char* jrpp_error_names[] = JRPP_ERRORS;
#undef CHOOSE

	if (rpp_err_no < RPP_OK) {
		return "Too small error code :(";
	}
	if (rpp_err_no >= RPP_LAST_DUMMY) {
		return "Too big error code :(";
	}

	return jrpp_error_names[rpp_err_no];
}

