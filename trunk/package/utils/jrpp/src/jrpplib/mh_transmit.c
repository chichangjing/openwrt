

/* Port Transmit state machine  */
  
#include "base.h"
#include "ring.h"
#include "rpp_in.h"
#include "rpp_to.h"
#include "machine.h"

#define STATES {                  \
	CHOOSE(TRANSMIT_INIT),        \
	CHOOSE(IDLE),                 \
	CHOOSE(TRANSMIT_AUTH),    \
	CHOOSE(TRANSMIT_BALLOT),      \
	CHOOSE(TRANSMIT_FORWARD),      \
	CHOOSE(TRANSMIT_HELLO),      \
	CHOOSE(TRANSMIT_COMPLETE),      \
	CHOOSE(TRANSMIT_REPORT),      \
	CHOOSE(TRANSMIT_NODEDOWN),      \
	CHOOSE(TRANSMIT_COMMCMD)      \
}

#define GET_STATE_NAME RPP_transmit_get_state_name
#include "choose.h"


static RPP_PDU_T pdu_packet  = {
	{	/* RPP_HEADER_T, 30 bytes */
		{0x0D, 0xA4, 0x2A, 0x00, 0x00, 0x05},       /* dst_mac */
        //{0x01, 0x80, 0xc2, 0x00, 0x00, 0x00},
		{0x00, 0x00, 0x00, 0x00, 0x00, 0x00},       /* --src_mac */ 
		{0x60, 0x00, 0xef, 0xfe},                   /* dsa */ 
		//{0x81, 0x00},                               /* ether_type: 802.1Q */
		//{0xef, 0xfe},                               /* vlan_tag: prio: 7, vid:4094 */
		{0x00, 0x48},                               /* len8023 */
		 0xaa, 0xaa, 0x03,                          /* dsap, ssap, llc */
		{0x0c, 0xa4, 0x2a},                         /* obtelecom */
		{0x00, 0xbb},                               /* jrpp's pid */
	},
	{{	/* RPP_BODY_T, 28 bytes + 36 bytes = 64 bytes */
		{0x99, 0x0b},                               /* res1[2] */
		{0x00, 0x40},                               /* length[2] */
		 0x01,                                      /* version: old platform = 1; new platform = 2*/
		 0x00,                                      /* --packet_type */
		{0x00,0x00},                                /* --domain_id[2] */
		{0x00,0x00},                                /* --ring_id[2] */
		{0x00,0x00},                                /* res2[2] */
		{0x00, 0x00, 0x00, 0x00, 0x00, 0x00},       /* --sys_mac */
		{0x00,0x00},                                /* --hello_time[2]; */
		{0x00,0x00},                                /* --fail_time[2] */
		 0x00,                                      /* --lport_id */
		 0x00,                                      /* ring_level */
		{0x00,0x00},                                /* --hello_req[2] */ 
		{0x00,0x00},                                /* res3[2] */
        {{0x00, 0x00, 0x00, 0x00, 0x00, 0x00,        /* --message_data[36] */
		 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,		
		 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}},
     }},
};

static void rpp_fill_body(rppPort_t *port,unsigned char type)
{
    register rppRing_t *ring = port->owner;
	register unsigned short ring_id;
	unsigned char hport;
		
	RPP_OUT_l2hport(port->port_index,&hport);
	ring_id = RPP_OUT_get_ringid(ring);
    
    pdu_packet.hdr.dsa_tag[1]   = hport << 3;
	pdu_packet.body.type        = type;
    *(unsigned short *)pdu_packet.body.domain_id    = htons(ring_id);
    *(unsigned short *)pdu_packet.body.ring_id      = htons(ring_id);
	pdu_packet.body.port_id     = port->port_index;
    
    if(type == PACKET_HELLO)
    {
        *(unsigned short *)pdu_packet.body.hello_seq    = htons(++ring->hello_syn);
        *(unsigned short *)pdu_packet.body.hello_time   = htons(RPP_OUT_get_time(ring,HELLO_TIME));
        *(unsigned short *)pdu_packet.body.fail_time    = htons(RPP_OUT_get_time(ring,FAIL_TIME));

        // statistic block points is an enhanced feature
        if(ring_get_mode(ring) == RPP_ENHANCED)
            pdu_packet.body.res3[0]                         = (port->stp_st == BLOCKING);
    }
    else
    {
        *(unsigned short *)pdu_packet.body.hello_seq    = 0;
        *(unsigned short *)pdu_packet.body.hello_time   = 0;
        *(unsigned short *)pdu_packet.body.fail_time    = 0;
        pdu_packet.body.res3[0]                         = 0;
    }
}

// 发认证报文
static int rpp_txAuth (STATE_MACH_T* this)
{
    register rppPort_t      *port = this->owner.port;
    register tRMsgAuth      *auth = &pdu_packet.body.msg.auth;

    memset(&pdu_packet.body.msg,0,RPP_MSG_DATA_SIZE);
    rpp_fill_body(port,PACKET_AUTHENTICATION);

    if(port->flag.f_tx & F_TX_AUTH_REQ)
    {
	    auth->type      = MSG_AUTH_REQ;
        port->flag.f_tx &= ~F_TX_AUTH_REQ;
    }
    else if(port->flag.f_tx & F_TX_AUTH_TACK)
    {
	    auth->type      = MSG_AUTH_ACK;
        port->flag.f_tx &= ~F_TX_AUTH_TACK;
    }

    return RPP_OUT_tx_pdu (port->port_index, (unsigned char *)&pdu_packet, sizeof(RPP_PDU_T));
}

static int rpp_txBallot (STATE_MACH_T* this)
{
    register rppPort_t      *port   = this->owner.port;
	register tRMsgBallot    *ballot = &pdu_packet.body.msg.ballot;

    memset(&pdu_packet.body.msg,0,RPP_MSG_DATA_SIZE);
    rpp_fill_body(port,PACKET_BALLOT);

    if(port->flag.f_tx & F_TX_BLT_ITTV_PLUS)
    {
        ballot->type        = MSG_BALLOT_INITIATIVE;
        ballot->sub_type    = 0;
        port->flag.f_tx     &= ~F_TX_BLT_ITTV_PLUS;
    }
    else if(port->flag.f_tx & F_TX_BLT_ITTV_MINUS)
    {
        ballot->type        = MSG_BALLOT_INITIATIVE;
        ballot->sub_type    = 1;
        port->flag.f_tx     &= ~F_TX_BLT_ITTV_MINUS;
    }
    else if(port->flag.f_tx & F_TX_BALLOT_PSSV)
    {
        ballot->type = MSG_BALLOT_PASSIVE;
        port->flag.f_tx &= ~F_TX_BALLOT_PSSV;
    }
	        
    ballot->mode = ring_get_mode(port->owner);
    ballot->port = port->port_index;
    memcpy(&ballot->id,RPP_OUT_get_nodeid(port->owner),sizeof(NODE_ID_T));

    return RPP_OUT_tx_pdu (port->port_index, (unsigned char *)&pdu_packet, sizeof(RPP_PDU_T));
}

static int rpp_txHello (STATE_MACH_T* this)
{
    register rppPort_t  *port   = this->owner.port;
    register tRMsgHello *hello  = &pdu_packet.body.msg.hello;

    memset(&pdu_packet.body.msg,0,RPP_MSG_DATA_SIZE);
    rpp_fill_body(port,PACKET_HELLO);
    
    struct timeval time;
    gettimeofday(&time,NULL);
    *(unsigned int *)hello->tick    = htonl(time.tv_sec * 1000 + time.tv_usec / 1000);
    hello->master_secondary_state   = port->owner->secondary->stp_st;
    hello->txport_state             = port->stp_st;
    hello->block_line_num[0]        = 0;
    hello->block_line_num[1]        = 0;
    hello->ring_state               = port->owner->status;

    port->flag.f_tx &= ~F_TX_HELLO;

    //log_info("  \033[1;33m<transmit machine> port %d in send hello packet(%d) status\033[0m",port->port_index,port->owner->hello_syn);
    return RPP_OUT_tx_pdu (port->port_index, (unsigned char *)&pdu_packet, sizeof(RPP_PDU_T));
}

static int rpp_txComplete(STATE_MACH_T* this)
{
    register rppPort_t  *port       = this->owner.port;
    register tRMsgComplete *compl   = &pdu_packet.body.msg.complete;

    memset(&pdu_packet.body.msg,0,RPP_MSG_DATA_SIZE);
    rpp_fill_body(port,PACKET_COMPLETE);
    
    if(port->flag.complete.type == 0)
    {
        LOG_ERROR("tmp buffer in port %d is empty",port->port_index);
        return -1;    
    }

    memcpy(compl,&port->flag.complete,sizeof(tRMsgComplete));
    memset(&port->flag.complete,0,sizeof(tRMsgComplete));
    port->flag.f_tx &= ~F_TX_COMPLETE;

    return RPP_OUT_tx_pdu (port->port_index, (unsigned char *)&pdu_packet, sizeof(RPP_PDU_T));
}

static int rpp_txReport(STATE_MACH_T* this)
{
    register rppPort_t  *port       = this->owner.port;
    register tRMsgReport *report    = &pdu_packet.body.msg.report;

    memset(&pdu_packet.body.msg,0,RPP_MSG_DATA_SIZE);
    rpp_fill_body(port,PACKET_REPORT);
    
    if(port->flag.down.type == 0)
    {
        LOG_ERROR("tmp buffer in port %d is empty",port->port_index);
        return -1;    
    }

    memcpy(report,&port->flag.down,sizeof(tRMsgReport));
    memset(&port->flag.down,0,sizeof(tRMsgReport));
    port->flag.f_tx &= ~F_TX_REPORT;

    log_info("  \033[1;33m<transmit machine> port %d in send report %s packet status\033[0m",\
             port->port_index,(report->type == MSG_ENDPOINT_TO_ENDPOINT)?"Ep2Ep":"Linkdown");
    return RPP_OUT_tx_pdu (port->port_index, (unsigned char *)&pdu_packet, sizeof(RPP_PDU_T));
}

static int rpp_txNodeDown(STATE_MACH_T* this)
{
    register rppPort_t  *port   = this->owner.port;
    register tRMsgNodeDown *ndown = &pdu_packet.body.msg.nodedown;

    memset(&pdu_packet.body.msg,0,RPP_MSG_DATA_SIZE);
    rpp_fill_body(port,PACKET_NODEDOWN);

    if(port->flag.ndown.type == 0)
    {
        LOG_ERROR("tmp buffer in port %d is empty",port->port_index);
        return -1;    
    }

    memcpy(ndown,&port->flag.ndown,sizeof(tRMsgNodeDown));
    memset(&port->flag.ndown,0,sizeof(tRMsgNodeDown));
    port->flag.f_tx &= ~F_TX_NODEDOWN;

    return RPP_OUT_tx_pdu (port->port_index, (unsigned char *)&pdu_packet, sizeof(RPP_PDU_T));
}

static int rpp_txCommcmd(STATE_MACH_T* this)
{
    register rppPort_t  *port   = this->owner.port;
    register tRMsgCmd *cmd      = &pdu_packet.body.msg.cmd;

	memcpy(pdu_packet.hdr.src_mac,RPP_OUT_get_mac(),MAC_LEN);
    memset(&pdu_packet.body.msg,0,RPP_MSG_DATA_SIZE);
    rpp_fill_body(port,PACKET_COMMAND);

    if(port->owner->cmd_req.cmd_msg.type == 0)
    {
        LOG_ERROR("cmd buffer in port %d is empty",port->port_index);
        return -1;    
    }

    memcpy(cmd,&port->owner->cmd_req.cmd_msg,sizeof(tRMsgCmd));
    if(!--port->owner->cmd_req.ref)
        memset(&port->owner->cmd_req.cmd_msg,0,sizeof(tRMsgCmd));
    port->flag.f_tx &= ~F_TX_CMD;

    return RPP_OUT_tx_pdu (port->port_index, (unsigned char *)&pdu_packet, sizeof(RPP_PDU_T));
}

static int rpp_txForwarding(STATE_MACH_T* this)
{
    register rppPort_t  *port   = this->owner.port;
	unsigned char hport;
		
	RPP_OUT_l2hport(port->port_index,&hport);
    port->fwding.hdr.dsa_tag[0] = 0x60;
    port->fwding.hdr.dsa_tag[1] = hport << 3;
	port->fwding.body.port_id   = port->port_index;
    port->flag.f_tx             &= ~F_TX_FORWARDING;

    if(port->fwding.body.type != PACKET_HELLO)
        log_info("  \033[1;33m<transmit machine> port %d in send forwarding packet status\033[0m",port->port_index);
    return RPP_OUT_tx_pdu (port->port_index, (unsigned char *)&port->fwding, sizeof(RPP_PDU_T));
}

void RPP_transmit_enter_state (STATE_MACH_T* this)
{
    register rppPort_t *port = this->owner.port;

	switch (this->state) 
    {
		case BEGIN:
            port->flag.f_tx = 0;
	        memcpy(pdu_packet.hdr.src_mac,RPP_OUT_get_mac(),MAC_LEN);
	        memcpy(pdu_packet.body.sys_mac,RPP_OUT_get_mac(),MAC_LEN);
		case IDLE:
			break;
		case TRANSMIT_AUTH:
			rpp_txAuth(this);
            log_info("  \033[1;33m<transmit machine> port %d in send auth packet status\033[0m",port->port_index);
			break;		
		case TRANSMIT_BALLOT:
			rpp_txBallot(this);
            log_info("  \033[1;33m<transmit machine> port %d in send ballot packet status\033[0m",port->port_index);
			break;		
        case TRANSMIT_FORWARD:
			rpp_txForwarding(this);
            //log_info("  \033[1;33m<transmit machine> port %d in send forwarding packet status\033[0m",port->port_index);
			break;		
        case TRANSMIT_HELLO:
			rpp_txHello(this);
            //log_info("  \033[1;33m<transmit machine> port %d in send hello packet status\033[0m",port->port_index);
            break;
        case TRANSMIT_COMPLETE:
			rpp_txComplete(this);
            log_info("  \033[1;33m<transmit machine> port %d in send complete packet status\033[0m",port->port_index);
            break;
        case TRANSMIT_REPORT:
			rpp_txReport(this);
            break;
        case TRANSMIT_NODEDOWN:
			rpp_txNodeDown(this);
            log_info("  \033[1;33m<transmit machine> port %d in send nodedown packet status\033[0m",port->port_index);
            break;
        case TRANSMIT_COMMCMD:
			rpp_txCommcmd(this);
            log_info("  \033[1;33m<transmit machine> port %d in send commcmd packet status\033[0m",port->port_index);
            break;
	}
}


Bool RPP_transmit_check_conditions (STATE_MACH_T* this)
{
    register rppPort_t *port = this->owner.port;

	switch (this->state) 
    {
		case BEGIN:
			return RPP_hop_2_state (this, IDLE);
		case IDLE:
			if(port->flag.f_tx & (F_TX_AUTH_REQ | F_TX_AUTH_TACK))
				return RPP_hop_2_state (this, TRANSMIT_AUTH);
			if(port->flag.f_tx & (F_TX_BLT_ITTV_MINUS | F_TX_BLT_ITTV_PLUS | F_TX_BALLOT_PSSV))
				return RPP_hop_2_state (this, TRANSMIT_BALLOT);
			if(port->flag.f_tx & F_TX_FORWARDING) 
				return RPP_hop_2_state (this, TRANSMIT_FORWARD);
			if(port->flag.f_tx & F_TX_HELLO) 
				return RPP_hop_2_state (this, TRANSMIT_HELLO);
            if(port->flag.f_tx & F_TX_COMPLETE)
				return RPP_hop_2_state (this, TRANSMIT_COMPLETE);
            if(port->flag.f_tx & F_TX_REPORT)
				return RPP_hop_2_state (this, TRANSMIT_REPORT);
            if(port->flag.f_tx & F_TX_NODEDOWN)
				return RPP_hop_2_state (this, TRANSMIT_NODEDOWN);
            if(port->flag.f_tx & F_TX_CMD)
				return RPP_hop_2_state (this, TRANSMIT_COMMCMD);
		    break;
        case TRANSMIT_AUTH:
            return RPP_hop_2_state(this,IDLE);
        case TRANSMIT_BALLOT:
            return RPP_hop_2_state(this,IDLE);
        case TRANSMIT_FORWARD:
            return RPP_hop_2_state(this,IDLE);
        case TRANSMIT_HELLO:
            return RPP_hop_2_state(this,IDLE);
        case TRANSMIT_COMPLETE:
            return RPP_hop_2_state(this,IDLE);
        case TRANSMIT_REPORT:
            return RPP_hop_2_state(this,IDLE);
        case TRANSMIT_NODEDOWN:
            return RPP_hop_2_state(this,IDLE);
        case TRANSMIT_COMMCMD:
            return RPP_hop_2_state(this,IDLE);
	}
	return False;
}

