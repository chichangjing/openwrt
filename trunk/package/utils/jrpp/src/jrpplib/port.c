#include "base.h"
#include "ring.h"
#include "port.h"
#include "rpp_in.h"
#include "rpp_to.h"
#include "machine.h"

int port_set_stp(rppPort_t *port,ePortStpState state)
{
    if(port->stp_st == state)
        return 0;

    log_info("port %d set %s",port->port_index,(state == FORWARDING)?"forwarding":"blocking");
    port->stp_st = state;
    return RPP_OUT_set_port_stp(port->port_index,state,port->link_st);
}

// 获取当前节点的另一个端口控制块
rppPort_t *port_get_peer(rppPort_t *this)
{
    struct rppRing *ring = this->owner;
    
    return (this != ring->primary)?ring->primary:ring->secondary;
}

// 根据端口号获取所属的端口控制块
rppPort_t *port_get_owner(rppRing_t *ring,unsigned char lport)
{
    return (lport== ring->primary->port_index)?ring->primary:ring->secondary;
}

// compute min ballot id from two id
NODE_ID_T *port_min_id(NODE_ID_T *id1,NODE_ID_T *id2)
{
    return (memcmp(id1,id2,sizeof(NODE_ID_T)) < 0)?id1:id2;
}

// forwarding packet
void port_fwd(rppPort_t *this, RPP_PDU_T *pdu)
{
    rppPort_t *peer = port_get_peer(this);
    if(peer->neighber_vaild)
    {
        memcpy(&peer->fwding,pdu,sizeof(RPP_PDU_T));                        
        tx_forwarding(peer);
    }
}

void port_link_change(rppPort_t *this,ePortLinkState link_status)
{

    // update link state 
    this->link_st           = link_status;

    log_info("port %d %s",this->port_index,(link_status == LINK_UP)?"LINK UP":"LINK_DOWN");
    if(link_status == LINK_DOWN)
    {
        node_deal_link_down(this->owner,this);

        // reset link down port must be at last
        port_reset(this);
    }
    else
        auth_start(this);

    // this port should set blocking after node deal link down
    port_set_stp(this,BLOCKING);
}

int port_get_state(rppPort_t *port,RPP_PORT_STATE_T *state)
{
    memset(state,0,sizeof(RPP_PORT_STATE_T));

    state->port_no      = port->port_index;
    state->role         = port->port_role;
    state->dot1d_state  = port->stp_st;
    state->link_state   = port->link_st;
    if(port->neighber_vaild)
    {
        state->neigbor_port_no = port->neighber_port;
        memcpy(state->neigbor_mac,port->neighber_mac,MAC_LEN);
    }
    strcpy(state->m_auth_st,RPP_machine_get_state(auth,port->auth));
    strcpy(state->m_ballot_st,RPP_machine_get_state(ballot,port->ballot));
    strcpy(state->m_tx_st,RPP_machine_get_state(transmit,port->transmit));
    
    return 0;
}


int port_start(rppPort_t *port)
{
    port->link_st = LINK_DOWN;
    port_reset(port);
    return port_set_stp(port,BLOCKING);
}

int port_stop(rppPort_t *port)
{
    port_reset(port);
    return port_set_stp(port,BLOCKING);
}

