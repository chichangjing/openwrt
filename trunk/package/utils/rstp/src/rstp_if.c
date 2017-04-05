#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <syslog.h>
#include <signal.h>
#include <dirent.h>
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

#include "base.h"
#include "stpm.h"
#include "stp_in.h"
#include "stp_to.h"

#include "log.h"
#include "utils.h"
#include "epoll_loop.h"
#include "packet.h"
#include "rstpd.h"
#include "rstp_if.h"

rstp_t *pRstp = NULL;

int br_restart_rstp()
{
	int r = 0;
	int port_index;
	BITMAP_T portmap;

    /* disable rstp, blocking port all*/
    for(port_index=1; port_index<=pRstp->bridge.portCount; port_index++) {
        br_set_state(port_index, BR_STATE_BLOCKING);
        if(fal_port_dot1d_state_set(0, port_index, FAL_PORT_DOT1D_STATE_BLOCKING) != SW_OK) {
            log_err("Error: fal_port_dot1d_state_set failed");
            return -1;
        }
    }
    
    /* Disable */
    if(pRstp->running == STP_DOWN) {
        log_err("Warning: RSTP isn't start\n");
        return 0;
    }
    
    pRstp->running = STP_DOWN;

    for(port_index=1; port_index<=pRstp->bridge.portCount; port_index++) {
        r = STP_IN_port_delete(STP_DEFAULT_VLAN_ID, port_index);
        if(r != 0) {
            log_err("port create failed, err=%s", STP_IN_get_error_explanation(r));
            return -1;
        }
        pRstp->ports[port_index-1].port = NULL;

        log_info("RSTP port%02d delete done", port_index);
    }

    r = STP_IN_stpm_delete(STP_DEFAULT_VLAN_ID);
    if(r != 0) {
        log_err("stpm delete failed, err=%s", STP_IN_get_error_explanation(r));
        return -1;
    }
    log_info("RSTP stpm delete done", STP_DEFAULT_VLAN_ID);
    
    pRstp->bridge.stpm = NULL;

    log_info("Now RSTP is shutdown\n");	


    /* Enable */
    if(pRstp->running == STP_RUNNING) {
        log_err("Warning: RSTP already started\n");
        return 0;
    }
    
    STP_IN_init(pRstp->bridge.portCount);
    BitmapClear(&portmap);

    r = STP_IN_stpm_create(STP_DEFAULT_VLAN_ID, STP_DEFAULT_VLAN_NAME, &portmap);
    if(r != 0) {
        log_err("stpm create failed, err=%s", STP_IN_get_error_explanation(r));
        return -1;
    }

    if((pRstp->bridge.stpm = (STPM_T *)stpapi_stpm_find(STP_DEFAULT_VLAN_ID)) == NULL) {
        log_err("stpm find failed");
        return -1;
    }
    log_info("RSTP stpm create done");
    
    for(port_index=1; port_index<=pRstp->bridge.portCount; port_index++) {
        pRstp->ports[port_index-1].linkStatus = STP_PORT_LINKDOWN;
        pRstp->ports[port_index-1].speed = 0;
        pRstp->ports[port_index-1].duplex = STP_PORT_DUPLEX_HALF;
        
        if(fal_port_dot1d_state_set(0, port_index, FAL_PORT_DOT1D_STATE_BLOCKING) != SW_OK) {
            log_err("Error: fal_port_dot1d_state_set failed");
            return -1;
        }
        
        r = STP_IN_port_create(STP_DEFAULT_VLAN_ID, port_index);
        if(r != 0) {
            log_err("stp port create failed, err=%s", STP_IN_get_error_explanation(r));
            return -1;
        }
        if((pRstp->ports[port_index-1].port = (PORT_T *)stpapi_port_find(pRstp->bridge.stpm, port_index)) == NULL) {
            log_err("stp port find failed");
            return -1;
        }
        log_info("RSTP port%02d create done", port_index);
    }
    
    pRstp->running = STP_RUNNING;
    log_info("Now RSTP is running\n");
		
	return r;
}


void update_bridge_stp_config(stpBridge_t *br, UID_STP_CFG_T *cfg)
{
	if (cfg->field_mask & BR_CFG_PRIO) {
        //sleep(1);
		br->bridge_priority = cfg->bridge_priority;
        //br_restart_rstp();
    }
	if (cfg->field_mask & BR_CFG_AGE)
		br->max_age = cfg->max_age;
	if (cfg->field_mask & BR_CFG_HELLO)
		br->hello_time = cfg->hello_time;
	if (cfg->field_mask & BR_CFG_DELAY)
		br->forward_delay = cfg->forward_delay;
	if (cfg->field_mask & BR_CFG_FORCE_VER)
		br->force_version = cfg->force_version;

}

void update_port_stp_config(stpPort_t *ifc, UID_STP_PORT_CFG_T *cfg)
{
	if (cfg->field_mask & PT_CFG_PRIO)
		ifc->port_priority = cfg->port_priority;
	if (cfg->field_mask & PT_CFG_NON_STP) {
		ifc->admin_non_stp = cfg->admin_non_stp;
        if (ifc->linkStatus) {
			br_set_state(ifc->ifIndex, BR_STATE_FORWARDING);
			if (fal_port_dot1d_state_set(0, ifc->ifIndex, FAL_PORT_DOT1D_STATE_FORWARDING) != SW_OK) {
				log_err("Error: fal_port_dot1d_state_set failed");
				return -1;
			}
        }
    }
	if (cfg->field_mask & PT_CFG_EDGE)
		ifc->admin_edge = cfg->admin_edge;
	if (cfg->field_mask & PT_CFG_COST)
		ifc->admin_port_path_cost = cfg->admin_port_path_cost;
	if (cfg->field_mask & PT_CFG_P2P)
		ifc->admin_point2point = cfg->admin_point2point;
}


int br_info_init(void)
{
	int port, port_count;
	char brName[IFNAMSIZ];
	char ifName[IFNAMSIZ];
	unsigned char mac[6];
	
	if((pRstp = (rstp_t *)calloc(1, sizeof(rstp_t))) == NULL) {
		log_err("calloc failed\n");
		return -1;
	}
	pRstp->running = STP_DOWN;
	
	/****************************************************************************
           Get bridge ifname and ifindex 
	 ****************************************************************************/
	if(get_bridge_name(brName) < 0) {
		log_err("can't found bridge\n");
		return -1;
	}
	memcpy(pRstp->bridge.brName, brName, IFNAMSIZ);
	pRstp->bridge.brIndex = if_nametoindex(brName);

	get_if_mac(pRstp->bridge.brIndex, mac);
	log_info("Bridge '%s' mac address: %02x-%02x-%02x-%02x-%02x-%02x\n", 
			pRstp->bridge.brName, mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

	/****************************************************************************
           Get bridge port numbers 
	 ****************************************************************************/
	if((get_port_count(brName, &port_count)) < 0) {
		log_err("get_port_count() failed\n");
		return -1;
	}
	if(port_count > STP_MAX_PORT_NUM) {
		log_err("error: port count is out of range\n");
		return -1;	
	}
	pRstp->bridge.portCount = port_count;
	log_info("Bridge port numbers: %d", port_count);
	
    if((pRstp->bridge.portList = (int *)calloc(pRstp->bridge.portCount, sizeof(int))) == NULL) {
        log_err("error: calloc failed\n");
        return -1;
    }
    if((pRstp->ports = (stpPort_t *)calloc(pRstp->bridge.portCount, sizeof(stpPort_t))) == NULL) {
        log_err("error: calloc failed\n");
        return -1;
    }

	for(port=1; port<=pRstp->bridge.portCount; port++) {
		/* ifIndex */
		pRstp->ports[port-1].ifIndex = port;
		/* ifName */
		if_indextoname(port, ifName);
		memcpy(pRstp->ports[port-1].ifName, ifName, IFNAMSIZ);
		/* portEnable */
		pRstp->ports[port-1].portEnable = STP_PORT_DISABLED;
		/* linkStatus */
		pRstp->ports[port-1].linkStatus = STP_PORT_LINKDOWN;
		/* speed */
		pRstp->ports[port-1].speed = 0;
		/* duplex */
		pRstp->ports[port-1].duplex = STP_PORT_DUPLEX_HALF;
	}
	
	return 0;
}


void br_bpdu_rcv(int if_index, const unsigned char *data, int len)
{
	BPDU_T *bpdu = (BPDU_T *) (data + sizeof(MAC_HEADER_T));

	if(len <= sizeof(MAC_HEADER_T) + sizeof(ETH_HEADER_T) + sizeof(BPDU_HEADER_T))
		return;
	if(pRstp->running == STP_DOWN)
		return;
	if(pRstp->ports[if_index-1].linkStatus != STP_PORT_LINKUP)
		return;
		
	switch (bpdu->hdr.bpdu_type) {
	case BPDU_RSTP:
		break;
	case BPDU_CONFIG_TYPE:
		break;
	case BPDU_TOPO_CHANGE_TYPE:
		break;
	default:
		log_info("Receive unknown bpdu type %x", bpdu->hdr.bpdu_type);
		return;
	}

#if 1 
	log_debug("\033[1;35mRx bpdu-type(%s) on port%02d\033[0m", 
			(bpdu->hdr.bpdu_type == BPDU_CONFIG_TYPE)? "CNF": \
			(bpdu->hdr.bpdu_type == BPDU_RSTP)? "RST": \
			(bpdu->hdr.bpdu_type == BPDU_TOPO_CHANGE_TYPE)? "TCN":"***", if_index);
	dump_hex((void *)data, len);
#endif

	int r = STP_IN_rx_bpdu(STP_DEFAULT_VLAN_ID, if_index, bpdu, len);
	if (r) {
		log_err("STP_IN_rx_bpdu on port %d returned %s", if_index, STP_IN_get_error_explanation(r));
	}
}

int br_notify(int if_index, int newlink, unsigned flags)
{
	int link_status;
	int speed = -1;
	int duplex = -1;
	fal_port_speed_t fal_speed;
	fal_port_duplex_t fal_duplex;
	int notify_flags = 0;
	const int NOTIFY_UP = 1, NOTIFY_SPEED = 2, NOTIFY_DUPLEX = 4;

	//if(pRstp->running == STP_DOWN)
	//	return -1;
	
	link_status = ((flags & IFF_UP) && (flags & IFF_RUNNING))? STP_PORT_LINKUP : STP_PORT_LINKDOWN;

	/* Link status is change */
	if(pRstp->ports[if_index-1].linkStatus != link_status) {
		log_info("Port%02d link changed -> link-%s", if_index, (link_status == STP_PORT_LINKUP)? "up":"down");
		
		if(link_status == STP_PORT_LINKDOWN) {
			/* Link down */
			if(pRstp->ports[if_index-1].linkStatus == STP_PORT_LINKUP) {
				pRstp->ports[if_index-1].linkStatus = STP_PORT_LINKDOWN;
				notify_flags |= NOTIFY_UP;
			}
		} else {
			/* Link up */
			fal_port_speed_get(0, if_index, &fal_speed);
			fal_port_duplex_get(0, if_index, &fal_duplex);

			speed = (fal_speed == FAL_PORT_SPEED_1000)? 1000L : (fal_speed == FAL_PORT_SPEED_100)? 100L : 10L;
			duplex = (fal_duplex == FAL_PORT_DUPLEX_FULL)? 1 : 0;
			
			if (speed != pRstp->ports[if_index-1].speed) {
				pRstp->ports[if_index-1].speed = speed;
				notify_flags |= NOTIFY_SPEED;
			}
			if (duplex != pRstp->ports[if_index-1].duplex) {
				pRstp->ports[if_index-1].duplex = duplex;
				notify_flags |= NOTIFY_DUPLEX;
			}
			if (!pRstp->ports[if_index-1].linkStatus) {
				pRstp->ports[if_index-1].linkStatus = STP_PORT_LINKUP;
				notify_flags |= NOTIFY_UP;
			}
			
			if(pRstp->running == STP_DOWN) {
				//log_info("Port%02d br_set_state -> forwarding state", if_index);
				//br_set_state(if_index, BR_STATE_FORWARDING);
			}	
		}
		
		if (notify_flags && pRstp->running) {
			if (notify_flags & NOTIFY_SPEED)
				STP_IN_changed_port_speed(if_index, speed);
			if (notify_flags & NOTIFY_DUPLEX)
				STP_IN_changed_port_duplex(if_index);
			if (notify_flags & NOTIFY_UP)
				STP_IN_enable_port(if_index, link_status);
		}
	}
		
	return 0;
}

void br_one_second(void)
{
	if (pRstp == NULL)
		return;
	
	if (pRstp->running == STP_RUNNING) {
		STP_IN_one_second();
	}

	#if 1
	/* To get information about port changes when bridge is down */
	/* But won't work so well since we will not sense deletions */
	static int count = 0;
	count++;
	if (count % 10 == 0) //60
		br_get_configuration();
	#endif
}

void stp_trace(const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	vDprintf(LOG_LEVEL_MODULE, fmt, ap);
	va_end(ap);
}

#if 1

int STP_OUT_flush_lt (IN int port_index, IN int vlan_id, LT_FLASH_TYPE_T type, char* reason)
{
	sw_error_t ret;

	if (port_index == 0) {
		log_info("Flush fdb all.\n");
		if((ret = fal_mac_addr_table_clear(0)) != SW_OK) {
			log_err("Error: fal_mac_addr_table_clear failed, err=%d\n", ret);
			return -1;			
		}
	} else if (type == LT_FLASH_ONLY_THE_PORT) {
		log_info("Flush fdb only port%d.\n", port_index);
		if((ret = fal_mac_flush_port(0, port_index)) != SW_OK) {
			log_err("Error: fal_mac_flush_port failed, err=%d\n", ret);
			return -1;
		}
	} else if (type == LT_FLASH_ALL_PORTS_EXCLUDE_THIS) {
		int port_no;
		log_info("Flush fdb all exclude port%d.\n", port_index);
		for(port_no=1; port_no<=pRstp->bridge.portCount; port_no++){
			if (port_no != port_index) {
				//log_info("Flush fdb port%d.\n", port_no);
				if((ret = fal_mac_flush_port(0, port_no)) != SW_OK) {
					log_err("Error: fal_mac_flush_port failed, err=%d\n", ret);
					return -1;
				}
			}
		}
	}
	
	return 0;
}


int STP_OUT_set_port_state (IN int port_index, IN int vlan_id, IN RSTP_PORT_STATE state)
{
	sw_error_t ret;
	fal_port_dot1d_state_t dot1d_state;
	int br_state;
	
	log_info("Port%02d stp state => %s\n",  port_index,			
			(state == UID_PORT_DISCARDING)? "blocking" : 
			(state == UID_PORT_LEARNING)? "learning" : 
			(state == UID_PORT_FORWARDING)? "forwarding" : "unkown");
	
	switch (state) {
	case UID_PORT_DISCARDING:
		dot1d_state = FAL_PORT_DOT1D_STATE_BLOCKING;
		br_state = BR_STATE_BLOCKING;
		break;
	case UID_PORT_LEARNING:
		dot1d_state = FAL_PORT_DOT1D_STATE_LEARNING;
		br_state = BR_STATE_LEARNING;
		break;
	case UID_PORT_FORWARDING:
		dot1d_state = FAL_PORT_DOT1D_STATE_FORWARDING;
		br_state = BR_STATE_FORWARDING;
		break;
	default:
		log_err("Error: STP_OUT_set_port_state: Unexpected state %d\n", state);
		return -1;
	}
	
	if(pRstp->ports[port_index-1].linkStatus == STP_PORT_LINKUP) {
		br_set_state(port_index, br_state);
	}

	if((ret = fal_port_dot1d_state_set(0, port_index, dot1d_state)) != SW_OK) {
		log_err("Error: fal_port_dot1d_state_set failed, err=%d\n", ret);
		return -1;
	}

	return 0;
}

/* 1- Up, 0- Down */ 
int STP_OUT_get_port_link_status (int port_index)
{
	log_info("Port%02d link   : link-%s\n",  port_index, (pRstp->ports[port_index-1].linkStatus == 1)? "up" : "down");	
	return pRstp->ports[port_index-1].linkStatus;
}

unsigned long STP_OUT_get_port_oper_speed (unsigned int portNo)
{
	log_info("Port%02d speed  : %d\n", portNo, pRstp->ports[portNo-1].speed);
	return pRstp->ports[portNo-1].speed;
}

/* 1- Full, 0- Half */
int STP_OUT_get_duplex (IN int port_index)
{
	log_info("Port%02d duplex : %s\n", port_index, (pRstp->ports[port_index-1].duplex == 1)? "full":"half");
	return pRstp->ports[port_index-1].duplex;
}

void STP_OUT_get_port_mac (int port_index, unsigned char *mac)
{
	get_if_mac(port_index, mac);
}

int STP_OUT_set_hardware_mode (int vlan_id, UID_STP_MODE_T mode)
{
  return STP_OK;
}


int STP_OUT_tx_bpdu (int port_index, int vlan_id, unsigned char *bpdu, size_t bpdu_len)
{
	pdu_pkt_send(port_index, bpdu, bpdu_len + sizeof(MAC_HEADER_T) + sizeof(ETH_HEADER_T));
	return 0;
}

const char *STP_OUT_get_port_name (IN int port_index)
{
  static char tmp[4];
  
  sprintf(tmp, "p%02d", (int) port_index);
  return tmp;
}

unsigned long STP_OUT_get_deafult_port_path_cost (IN unsigned int portNo)
{
  return 20000;
}

int STP_OUT_get_init_stpm_cfg (IN int vlan_id, INOUT UID_STP_CFG_T* cfg)
{
  cfg->bridge_priority = DEF_BR_PRIO;
  cfg->max_age =         DEF_BR_MAXAGE;
  cfg->hello_time =      DEF_BR_HELLOT;
  cfg->forward_delay =   DEF_BR_FWDELAY;
  cfg->force_version =   NORMAL_RSTP;

  return STP_OK;
}
  

int STP_OUT_get_init_port_cfg (IN int vlan_id, IN int port_index, INOUT UID_STP_PORT_CFG_T* cfg)
{
  cfg->port_priority =        DEF_PORT_PRIO;
  cfg->admin_non_stp =        DEF_ADMIN_NON_STP;
  cfg->admin_edge =           DEF_ADMIN_EDGE;
  cfg->admin_port_path_cost = ADMIN_PORT_PATH_COST_AUTO;
  cfg->admin_point2point =    DEF_P2P;

  return STP_OK;
}

#endif


int CTL_set_debug_level(int level)
{
	extern int log_level;

	if(level < LOG_LEVEL_MAX)
		log_level = level;
	return 0;
}



int CTL_enable_rstp(int enable)
{
	int r = 0;
	int port_index;
	BITMAP_T portmap;

	if(enable) {
		/* Enable */
		if(pRstp->running == STP_RUNNING) {
			log_err("Warning: RSTP already started\n");
			return 0;
		}

		//set_bridge_stp(pRstp->bridge.brName, STP_ENABLED);
		
		STP_IN_init(pRstp->bridge.portCount);
		BitmapClear(&portmap);
	
		r = STP_IN_stpm_create(STP_DEFAULT_VLAN_ID, STP_DEFAULT_VLAN_NAME, &portmap);
		if(r != 0) {
			log_err("stpm create failed, err=%s", STP_IN_get_error_explanation(r));
			return -1;
		}

		if((pRstp->bridge.stpm = (STPM_T *)stpapi_stpm_find(STP_DEFAULT_VLAN_ID)) == NULL) {
			log_err("stpm find failed");
			return -1;
		}
		log_info("RSTP stpm create done");
		
		for(port_index=1; port_index<=pRstp->bridge.portCount; port_index++) {
			pRstp->ports[port_index-1].linkStatus = STP_PORT_LINKDOWN;
			pRstp->ports[port_index-1].speed = 0;
			pRstp->ports[port_index-1].duplex = STP_PORT_DUPLEX_HALF;
			
			if(fal_port_dot1d_state_set(0, port_index, FAL_PORT_DOT1D_STATE_BLOCKING) != SW_OK) {
				log_err("Error: fal_port_dot1d_state_set failed");
				return -1;
			}
			
			r = STP_IN_port_create(STP_DEFAULT_VLAN_ID, port_index);
			if(r != 0) {
				log_err("stp port create failed, err=%s", STP_IN_get_error_explanation(r));
				return -1;
			}
			if((pRstp->ports[port_index-1].port = (PORT_T *)stpapi_port_find(pRstp->bridge.stpm, port_index)) == NULL) {
				log_err("stp port find failed");
				return -1;
			}
			log_info("RSTP port%02d create done", port_index);
		}
		
		pRstp->running = STP_RUNNING;
		log_info("Now RSTP is running\n");
		
		//br_get_configuration();
			
	} else {

		for(port_index=1; port_index<=pRstp->bridge.portCount; port_index++) {
			br_set_state(port_index, BR_STATE_FORWARDING);
			if(fal_port_dot1d_state_set(0, port_index, FAL_PORT_DOT1D_STATE_FORWARDING) != SW_OK) {
				log_err("Error: fal_port_dot1d_state_set failed");
				return -1;
			}
		}
		
		/* Disable */
		if(pRstp->running == STP_DOWN) {
			log_err("Warning: RSTP isn't start\n");
			return 0;
		}
		
		pRstp->running = STP_DOWN;

		for(port_index=1; port_index<=pRstp->bridge.portCount; port_index++) {
			r = STP_IN_port_delete(STP_DEFAULT_VLAN_ID, port_index);
			if(r != 0) {
				log_err("port create failed, err=%s", STP_IN_get_error_explanation(r));
				return -1;
			}
			pRstp->ports[port_index-1].port = NULL;

			log_info("RSTP port%02d delete done", port_index);
		}
	
		r = STP_IN_stpm_delete(STP_DEFAULT_VLAN_ID);
		if(r != 0) {
			log_err("stpm delete failed, err=%s", STP_IN_get_error_explanation(r));
			return -1;
		}
		log_info("RSTP stpm delete done", STP_DEFAULT_VLAN_ID);
		
		pRstp->bridge.stpm = NULL;
	
		log_info("Now RSTP is shutdown\n");	
	}
	
	return r;
}

int CTL_get_bridge_state(UID_STP_CFG_T *cfg, UID_STP_STATE_T *state)
{
	int r;

	r = STP_IN_stpm_get_state(STP_DEFAULT_VLAN_ID, state);
	if (r) {
		log_err("Error getting bridge state: %s", STP_IN_get_error_explanation(r));
		return r;
	}
	r = STP_IN_stpm_get_cfg(STP_DEFAULT_VLAN_ID, cfg);
	if (r) {
		log_err("Error getting bridge config: %s", STP_IN_get_error_explanation(r));
		return r;
	}

	return 0;
}

int CTL_set_bridge_config(UID_STP_CFG_T *cfg)
{
	int r;

	r = STP_IN_stpm_set_cfg(STP_DEFAULT_VLAN_ID, NULL, cfg);
	if (r) {
		log_err("Error setting bridge config: %s", STP_IN_get_error_explanation(r));
		return r;
	}
	
	update_bridge_stp_config(&pRstp->bridge, cfg);
	
	return 0;
}

int CTL_get_port_state(int port_index, UID_STP_PORT_CFG_T *cfg, UID_STP_PORT_STATE_T *state)
{
	int r;

	state->port_no = port_index;
	r = STP_IN_port_get_state(STP_DEFAULT_VLAN_ID, state);
	if (r) {
		log_err("Error getting port state for port %d: %s", port_index, STP_IN_get_error_explanation(r));
		return r;
	}
	r = STP_IN_port_get_cfg(STP_DEFAULT_VLAN_ID, port_index, cfg);
	if (r) {
		log_err("Error getting port config for port %d: %s", port_index, STP_IN_get_error_explanation(r));
		return r;
	}

	return 0;
}

int CTL_set_port_config(int port_index, UID_STP_PORT_CFG_T *cfg)
{
	int r;

	r = STP_IN_set_port_cfg(STP_DEFAULT_VLAN_ID, port_index, cfg);
	if (r) {
		log_err("Error setting port config for port %d: %s", port_index, STP_IN_get_error_explanation(r));
		return r;
	}

	update_port_stp_config(&pRstp->ports[port_index-1], cfg);
	
	return 0;
}

int CTL_set_port_trace(BITMAP_T *portmap, int mach_index, int enableflag)
{
	#define MACHINE_NUM	10
	char *mach_name[MACHINE_NUM] = {"pcost","edge","p2p","transmit","migrate","topoch","sttrans","roletrns","info","all"};

	if((mach_index >= MACHINE_NUM) || (mach_index < 0))
		return -1;
	
	STP_IN_dbg_set_port_trace(mach_name[mach_index], enableflag, STP_DEFAULT_VLAN_ID, portmap, 1);

	return 0;
}

