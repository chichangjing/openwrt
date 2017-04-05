#include "base.h"
#include "ring.h"
#include "rpp_in.h"
#include "rpp_to.h"
#include "machine.h"

#define STATES {\
                CHOOSE(RESERVE),\
                CHOOSE(BALLOT_ITTVM),\
                CHOOSE(BALLOT_ITTVP),\
                CHOOSE(BALLOT_PSSV),\
                CHOOSE(BALLOT_FAIL),\
                CHOOSE(BALLOT_FIN)\
}

#define GET_STATE_NAME RPP_ballot_get_state_name 
#include "choose.h"

#define _ballot_fwd port_fwd

// forwarding or loopback ballot packet
static void _ballot_fwd_or_back(rppPort_t *this, RPP_PDU_T *pdu,NODE_ID_T *minid)
{
    rppPort_t *peer = this->peer;
    // if peer port pass auth determine direction for sending ballot packet
    if(peer->neighber_vaild)
    {
        memcpy(&peer->fwding,pdu,sizeof(RPP_PDU_T));                        
        memcpy(&peer->fwding.body.msg.ballot.id,minid,sizeof(NODE_ID_T));
        tx_forwarding(peer);
    }
    else
    {
        memcpy(&this->fwding,pdu,sizeof(RPP_PDU_T));                        
        this->fwding.body.msg.ballot.type = MSG_BALLOT_LOOPBACK;
        memcpy(&this->fwding.body.msg.ballot.id,minid,sizeof(NODE_ID_T));
        memcpy(&this->fwding.body.msg.ballot.endpoint,RPP_OUT_get_mac(),MAC_LEN);
        tx_forwarding(this);
    }
}

static void _ballot_role_compute(rppPort_t *port,eMsgBallotType type)
{
    NODE_ID_T   *nodeId   = RPP_OUT_get_nodeid(port->owner);
    rppPort_t   *peer     = port->peer;
    BALLOT_ID_T *master_id;
    eNodeType   role;

    // if peer port pass auth determine unilateral or bilateral compute
    if(peer->neighber_vaild)
    {
        // do nothing for port which pass auth but ballot has not finished
        if(!ballot_if_fin(peer) || !ballot_if_fin(port))
            return;
             
        role = (memcmp(nodeId,&port->ballot_id,sizeof(NODE_ID_T)) || memcmp(nodeId,&peer->ballot_id,sizeof(NODE_ID_T)));
        master_id = port_min_id(port_min_id(nodeId,&port->ballot_id),&peer->ballot_id);
    }
    else
    {
        role = memcmp(nodeId,&port->ballot_id,sizeof(NODE_ID_T));
        master_id = port_min_id(nodeId,&port->ballot_id);
    }
    
    node_role_handler(port->owner,role,master_id,(type == MSG_BALLOT_LOOPBACK)?0:1);
    //log_info("this is a %s node",(role == NODE_TYPE_MASTER)?"master":"transit");
}

void ballot_ittv_minus(rppPort_t *port)
{
    port->flag.f_ballot = BALLOT_ITTVM;
}

Bool ballot_if_fin(rppPort_t *port)
{
    return (port->flag.f_ballot == BALLOT_FIN);
}

// endpoint to endpoint,it means ring will into health without by ballot
void ballot_p2p(rppPort_t *port)
{
    rppPort_t *peer     = port->peer;
    if(ballot_if_fin(peer) != True)
    {
        LOG_ERROR("p2p trig,but peer port %d has not finish ballot",peer->port_index);
        return;
    }

    memcpy(&port->ballot_id,&peer->ballot_id,sizeof(NODE_ID_T));
    port->flag.f_ballot = BALLOT_FIN;
    node_deal_p2p(port->owner,peer,port->neighber_mac);
}

int ballot_packet_parse(rppPort_t *this, RPP_PDU_T *pdu)
{
    tRMsgBallot *ballot = &pdu->body.msg.ballot;
    rppRing_t *ring     = this->owner;
    rppPort_t *peer     = this->peer;

    if(!this->neighber_vaild)
    {
        LOG_ERROR("port %d get ballot,but neighber is invalid",this->port_index);
        return 0;
    }
    
    if(!memcmp(pdu->hdr.src_mac,RPP_OUT_get_mac(),MAC_LEN))
    {
        log_info("port %d recieve a packet for self ballot loopback",this->port_index);
        rppPort_t *owner = port_get_owner(ring,ballot->port);

        // mode will be sync with each node in ring
        if(ballot->mode == RPP_COMPAT)
            ring_set_mode(ring,RPP_COMPAT);

        // recv self ballot packet means one port finish ballot
        memcpy(&owner->ballot_id,&ballot->id,sizeof(NODE_ID_T));
        RPP_OUT_close_timer(&owner->ballot_timer);
        owner->flag.f_ballot = BALLOT_FIN;

        // only edge node will record endpoint from ballot by self
        if(ring_get_mode(ring) == RPP_ENHANCED && ballot->type == MSG_BALLOT_LOOPBACK && !peer->neighber_vaild)
        {
            if(*(unsigned int *)ballot->endpoint == 0)
            {
                LOG_ERROR("endpoint in ballot packet error");
                return 0;
            }
            memcpy(ring->endpoint,ballot->endpoint,MAC_LEN);
        }

        // trying to compute node role
        _ballot_role_compute(this,ballot->type);
    }
    else
    {
        NODE_ID_T *minId;
        NODE_ID_T *nodeId = RPP_OUT_get_nodeid(ring);
        switch(ballot->type)
        {
            case MSG_BALLOT_INITIATIVE:
                log_info("port %d recieve a packet for ballot initiative[%02x:%02x:%02x:%02x:%02x:%02x]",this->port_index,\
                     pdu->hdr.src_mac[0],pdu->hdr.src_mac[1],pdu->hdr.src_mac[2],pdu->hdr.src_mac[3],pdu->hdr.src_mac[4],pdu->hdr.src_mac[5]);
                minId = port_min_id(nodeId,&ballot->id);
                _ballot_fwd_or_back(this,pdu,minId);

                /**<    ++ballot packet maybe cause this-port into passive ballot,--ballot packet maybe cause both ports into passive ballot */
                if(ballot_if_fin(this) && (memcmp(pdu->hdr.src_mac,this->neighber_mac,MAC_LEN) || ballot->sub_type == 1))
                    this->flag.f_ballot = BALLOT_PSSV;
                if(ballot_if_fin(peer) && (ballot->sub_type == 1))
                    peer->flag.f_ballot = BALLOT_PSSV;
                break;
            case MSG_BALLOT_PASSIVE:
                log_info("port %d recieve a packet for ballot passive",this->port_index);
                minId = port_min_id(nodeId,&ballot->id);
                _ballot_fwd_or_back(this,pdu,minId);
                break;
            case MSG_BALLOT_LOOPBACK:
                log_info("port %d recieve a packet for ballot loopback",this->port_index);
                _ballot_fwd(this,pdu);
                break;
            default:
                return -5;
        }
    }

    return 0;
}

void ballot_timer_handler(struct uloop_timeout *timer)
{
    rppPort_t *port = rpp_get_port_addr(timer,ballot_timer);

    switch(port->ballot->state)
    {
        case BALLOT_ITTVM:
            tx_ballot_ittv_m(port);
            break;
        case BALLOT_ITTVP:
            tx_ballot_ittv_p(port);
            break;
        case BALLOT_PSSV:
            tx_ballot_pssv(port);
            break;
        default:
            LOG_ERROR("current ballot machine state = %d is not correct",port->ballot->state);
            return;
    }

    RPP_OUT_set_timer(timer,1000 * RPP_OUT_get_time(port->owner,BALLOT_TIME));

	ring_update (port->owner);
}

void RPP_ballot_enter_state(STATE_MACH_T* this)
{
    register rppPort_t *port = this->owner.port;

    switch(this->state)
    {
        case BEGIN:
            RPP_OUT_close_timer(&port->ballot_timer);
            port->flag.f_ballot = 0;
            break;
        case BALLOT_ITTVM:
        case BALLOT_ITTVP:
            RPP_OUT_set_timer(&port->ballot_timer,500);
            log_info("  \033[1;33m<ballot machine> port %d in initiative%s status!\033[0m",\
                     port->port_index,(this->state == BALLOT_ITTVP)?"++":"--");
            break;
        case BALLOT_PSSV:
            RPP_OUT_set_timer(&port->ballot_timer,500);
            log_info("  \033[1;33m<ballot machine> port %d in passive status!\033[0m",port->port_index);
            break;
        case BALLOT_FAIL:
            log_info("  \033[1;33m<ballot machine> port %d in fail status!\033[0m",port->port_index);
            break;
        case BALLOT_FIN:
            log_info("  \033[1;33m<ballot machine> port %d in finish status!\033[0m",port->port_index);
            break;
    }
}

Bool RPP_ballot_check_conditions(STATE_MACH_T* this)
{
    register rppPort_t *port    = this->owner.port;

    switch(this->state)
    {
        case BEGIN:
            // if get flag of ballot-finish first,need not to ballot
            if(port->flag.f_ballot == BALLOT_FIN)
                return RPP_hop_2_state(this,BALLOT_FIN);
            if(port->neighber_vaild) 
                return RPP_hop_2_state(this,BALLOT_ITTVP);
            break;
        case BALLOT_ITTVM:
            if(port->flag.f_ballot == BALLOT_FIN)
                return RPP_hop_2_state(this,BALLOT_FIN);
            if(port->flag.f_ballot == BALLOT_FAIL)
                return RPP_hop_2_state(this,BALLOT_FAIL);
            if(!port->neighber_vaild)
                return RPP_hop_2_state(this,BEGIN);
            break;
        case BALLOT_ITTVP:
            if(port->flag.f_ballot == BALLOT_FIN)
                return RPP_hop_2_state(this,BALLOT_FIN);
            if(port->flag.f_ballot == BALLOT_FAIL)
                return RPP_hop_2_state(this,BALLOT_FAIL);
            if(port->flag.f_ballot == BALLOT_ITTVM)
                return RPP_hop_2_state(this,BALLOT_ITTVM);
            if(!port->neighber_vaild)
                return RPP_hop_2_state(this,BEGIN);
            break;
        case BALLOT_PSSV:
            if(port->flag.f_ballot == BALLOT_FIN)
                return RPP_hop_2_state(this,BALLOT_FIN);
            if(port->flag.f_ballot == BALLOT_FAIL)
                return RPP_hop_2_state(this,BALLOT_FAIL);
            if(port->flag.f_ballot == BALLOT_ITTVM)
                return RPP_hop_2_state(this,BALLOT_ITTVM);
            if(!port->neighber_vaild)
                return RPP_hop_2_state(this,BEGIN);
            break;
        case BALLOT_FAIL:
            return RPP_hop_2_state(this,BEGIN);
        case BALLOT_FIN:
            if(port->flag.f_ballot == BALLOT_PSSV)
                return RPP_hop_2_state(this,BALLOT_PSSV);
            if(port->flag.f_ballot == BALLOT_ITTVM)
                return RPP_hop_2_state(this,BALLOT_ITTVM);
            if(!port->neighber_vaild)
                return RPP_hop_2_state(this,BEGIN);
            break;
        default:
            LOG_ERROR("ballot machine in error status!\n");
            break;
    }

    return False;
}

