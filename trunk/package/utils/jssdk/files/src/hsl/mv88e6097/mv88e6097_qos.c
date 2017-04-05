 
#include "sw.h"
#include "sal_sys.h"
#include "hsl.h"
#include "hsl_dev.h"
#include "hsl_port_prop.h"
#include "mv88e6097_qos.h"
#include "mv88e6097_port_ctrl.h"
#include "mv88e6097_reg.h"
#include "msApi.h"
#include "gtHwCntl.h"
#include "gtDrvSwRegs.h"

#include "joyware.h"


extern GT_QD_DEV *pJwDev;
static a_uint32_t Tag_Ip_Assign_Mode[CONFIG_JWS_PORT_TOTAL_NR];




static inline sw_error_t _mv88e6097_qos_port_prio_map_rule_set(a_uint32_t dev_id,fal_port_t port_id,fal_qos_map_t qos_map)
{
	a_uint32_t i;	
	GT_LPORT hport;
	GT_BOOL mode;
	GT_STATUS status;

	i = port_id-1;
	SW_RTN_ON_ERROR(map_lport2hport(port_id, (a_uint32_t *)&hport));

	if(Tag_Ip_Assign_Mode[i] == 1) {
		
		switch(qos_map) {
			case FAL_QOS_IP_MAPPING:
				mode = GT_FALSE;
				if((status = gqosSetPrioMapRule(pJwDev, hport, mode)) != GT_OK) {
					printf("gqosSetPrioMapRule Failed\n");
					return SW_FAIL;
				}				
				break;

			case FAL_QOS_TAG_MAPPING:
				mode = GT_TRUE;
				if((status = gqosSetPrioMapRule(pJwDev, hport, mode)) != GT_OK) {
					printf("gqosSetPrioMapRule Failed\n");
					return SW_FAIL;
				}				
				break;

			default: 
				break;
			
		}
	}else{
		printf("Qos port priority map rule set Failed, This mode must in Both Tag and Ip mode!\n");
		return SW_FAIL;
	}

	return SW_OK;
}

static inline sw_error_t _mv88e6097_qos_port_prio_map_rule_get(a_uint32_t dev_id,fal_port_t port_id,fal_qos_map_t * qos_map)
{
	GT_LPORT hport;
	GT_BOOL mode;
	GT_STATUS status;

	SW_RTN_ON_ERROR(map_lport2hport(port_id, (a_uint32_t *)&hport));

	if((status = gqosGetPrioMapRule(pJwDev, hport, &mode)) != GT_OK) {
		printf("gqosGetPrioMapRule Failed\n");
		return SW_FAIL;
	}

	if(mode == GT_TRUE){
		*qos_map = FAL_QOS_TAG_MAPPING;
	}else if(mode == GT_FALSE){
		*qos_map = FAL_QOS_IP_MAPPING;
	}

	return SW_OK;
}

static inline sw_error_t _mv88e6097_qos_port_schedule_mode_set(a_uint32_t dev_id,fal_port_t port_id,fal_qos_schedule_mode_t schedule_mode)
{
	GT_LPORT hport;
	GT_STATUS status;
	GT_PORT_SCHED_MODE mode;
	
	SW_RTN_ON_ERROR(map_lport2hport(port_id, (a_uint32_t *)&hport));

	switch(schedule_mode){
		case FAL_QOS_WRR_MODE:
			mode = GT_PORT_SCHED_WEIGHTED_RRB;
			if((status = gprtSetPortSched(pJwDev, hport, mode)) != GT_OK) {
				 printf("gprtSetPortSched Failed\n");
				 return SW_FAIL;
			}
			break;

		case FAL_QOS_SR_MODE:
			mode = GT_PORT_SCHED_STRICT_PRI;
			if((status = gprtSetPortSched(pJwDev, hport, mode)) != GT_OK) {
				 printf("gprtSetPortSched Failed\n");
				 return SW_FAIL;
			}
			break;

		default: 
			break;
	}

	return SW_OK;
}

static inline sw_error_t _mv88e6097_qos_port_schedule_mode_get(a_uint32_t dev_id,fal_port_t port_id,fal_qos_schedule_mode_t * schedule_mode)
{
	GT_LPORT hport;
	GT_STATUS status;
	GT_PORT_SCHED_MODE mode;
	
	SW_RTN_ON_ERROR(map_lport2hport(port_id, (a_uint32_t *)&hport));
 
	if((status = gprtGetPortSched(pJwDev, hport, &mode)) != GT_OK) {
		 printf("gprtGetPortSched Failed\n");
		 return SW_FAIL;
	 }

	if(mode == GT_PORT_SCHED_WEIGHTED_RRB){
		*schedule_mode = FAL_QOS_WRR_MODE;
	}else if(mode == GT_PORT_SCHED_STRICT_PRI){
		*schedule_mode = FAL_QOS_SR_MODE;
	}

	return SW_OK;
}

static inline sw_error_t _mv88e6097_qos_tag_prio_map_set(a_uint32_t dev_id,fal_tag_map_t *tag_mapping)
{
	GT_STATUS status;
	GT_U8 priority;
	GT_U8 queue_num;

	for(priority=0; priority<8; priority++) {   	
		queue_num = tag_mapping->tag_map[priority];
		/* ieee802.3ac tag priority mapping set */
		if((status = gcosSetUserPrio2Tc(pJwDev, priority, queue_num)) != GT_OK) {
			printf("gcosSetUserPrio2Tc Failed\n");
			return SW_FAIL;
		}
		//printf("%s, priority = %d, queue_num = %d\n",__FUNCTION__,priority,queue_num);
	} 
	
	return SW_OK;
}

static inline sw_error_t _mv88e6097_qos_ip_prio_map_set(a_uint32_t dev_id,fal_ip_map_t *ip_mapping)
{
	GT_U8 priority;
	GT_U8 queue_num;
	GT_STATUS status;

	for(priority=0; priority<64; priority++) {   	
		queue_num = ip_mapping->ip_map[priority];
		/* IPv4/IPv6 priority mapping set */
		if((status = gcosSetDscp2Tc(pJwDev, priority, queue_num)) != GT_OK) {
			printf("gcosSetDscp2Tc Failed\n");
			return SW_FAIL;
		}
		//printf("%s, priority = %d, queue_num = %d\n",__FUNCTION__,priority,queue_num);
	} 

	return SW_OK;
}

static inline sw_error_t _mv88e6097_qos_port_prio_assign_mode_set(a_uint32_t dev_id,fal_port_t port_id,fal_qos_assign_mode_t assign_mode)
{
	GT_STATUS status;
	GT_LPORT hport;
	a_uint32_t i;

	if(port_id > (CONFIG_JWS_PORT_TOTAL_NR))
		return SW_BAD_PARAM;
	
	i = port_id-1;
	SW_RTN_ON_ERROR(map_lport2hport(port_id, (a_uint32_t *)&hport));

	switch(assign_mode) {
		case FAL_QOS_USE_DEFAULT_ASSIGN_MODE:
			Tag_Ip_Assign_Mode[i] = 0;
			/* default assign mode: use default priority, disable tag and ip assign mode */
			if((status = gqosUserPrioMapEn(pJwDev, hport, GT_FALSE)) != GT_OK) {
				printf("gqosUserPrioMapEn Failed\n");
				return SW_FAIL;
			}

			if((status = gqosIpPrioMapEn(pJwDev, hport, GT_FALSE)) != GT_OK) {
				printf("gqosIpPrioMapEn Failed\n");
				return SW_FAIL;
			}
			break;
			
		case FAL_QOS_USE_TAG_ASSIGN_MODE:	
			Tag_Ip_Assign_Mode[i] = 0;
			/* enable tag assign mode,disable ip assign mode */
			if((status = gqosUserPrioMapEn(pJwDev, hport, GT_TRUE)) != GT_OK) {
				printf("gqosUserPrioMapEn Failed\n");
				return SW_FAIL;
			}

			if((status = gqosIpPrioMapEn(pJwDev, hport, GT_FALSE)) != GT_OK) {
				printf("gqosIpPrioMapEn Failed\n");
				return SW_FAIL;
			}

			/* Set default priority to 0 */
			if((status = gcosSetPortDefaultTc(pJwDev, hport, 0)) != GT_OK) {
				printf("gcosSetPortDefaultTc Failed\n");
				return SW_FAIL;
			}
			break;

		case FAL_QOS_USE_IP_ASSIGN_MODE:
			Tag_Ip_Assign_Mode[i] = 0;
			/* enable ip assign mode, disable tag addign mode */
			if((status = gqosUserPrioMapEn(pJwDev, hport, GT_FALSE)) != GT_OK) {
				printf("gqosUserPrioMapEn Failed\n");
				return SW_FAIL;
			}
			
			if((status = gqosIpPrioMapEn(pJwDev, hport, GT_TRUE)) != GT_OK) {
				printf("gqosIpPrioMapEn Failed\n");
				return SW_FAIL;
			}

			/* Set default priority to 0 */
			if((status = gcosSetPortDefaultTc(pJwDev, hport, 0)) != GT_OK) {
				printf("gcosSetPortDefaultTc Failed\n");
				return SW_FAIL;
			}
			break;

		case FAL_QOS_USE_TAG_IP_ASSIGN_MODE:
			/* Use tag ip assign mode init in _mv88e6097_qos_port_prio_map_rule_set function */
			Tag_Ip_Assign_Mode[i] = 1;
			/* initialize tag-ip assign mode, both ip and tag should be enable, default tag mapping GT_TRUE */
			if((status = gqosUserPrioMapEn(pJwDev, hport, GT_TRUE)) != GT_OK) {
				printf("gqosUserPrioMapEn Failed\n");
				return SW_FAIL;
			}
			
			if((status = gqosIpPrioMapEn(pJwDev, hport, GT_TRUE)) != GT_OK) {
				printf("gqosIpPrioMapEn Failed\n");
				return SW_FAIL;
			}

			/* default when both ip and tag assign mode enable, use tag mode GT_TRUE */
			if((status = gqosSetPrioMapRule(pJwDev, hport, GT_TRUE)) != GT_OK) {
				printf("gqosSetPrioMapRule Failed\n");
				return SW_FAIL;
			}

			/* Set default priority to 0 */
			if((status = gcosSetPortDefaultTc(pJwDev, hport, 0)) != GT_OK) {
				printf("gcosSetPortDefaultTc Failed\n");
				return SW_FAIL;
			}
			break;

		default: 
			break;

	}
	
	return SW_OK;
}

static inline sw_error_t _mv88e6097_qos_port_default_prio_set(a_uint32_t dev_id,fal_port_t port_id,fal_qos_prio_t priority)
{
	GT_STATUS status;
	GT_LPORT hport;
	GT_U8 priority_level;
	
	if(port_id > (CONFIG_JWS_PORT_TOTAL_NR))
		return SW_BAD_PARAM;

	SW_RTN_ON_ERROR(map_lport2hport(port_id, (a_uint32_t *)&hport));

	switch(priority) {
		case FAL_QOS_PRIO_LOWEST:
			priority_level = 0;
			if((status = gcosSetPortDefaultTc(pJwDev, hport, priority_level * 2)) != GT_OK) {
				printf("gcosSetPortDefaultTc Failed\n");
				return SW_FAIL;
			}
			break;

		case FAL_QOS_PRIO_LOW:
			priority_level = 1;
			if((status = gcosSetPortDefaultTc(pJwDev, hport, priority_level * 2)) != GT_OK) {
				printf("gcosSetPortDefaultTc Failed\n");
				return SW_FAIL;
			}
			break;

		case FAL_QOS_PRIO_HIGH:
			priority_level = 2;
			if((status = gcosSetPortDefaultTc(pJwDev, hport, priority_level * 2)) != GT_OK) {
				printf("gcosSetPortDefaultTc Failed\n");
				return SW_FAIL;
			}
			break;

		case FAL_QOS_PRIO_HIGHEST:
			priority_level = 3;
			if((status = gcosSetPortDefaultTc(pJwDev, hport, priority_level * 2)) != GT_OK) {
				printf("gcosSetPortDefaultTc Failed\n");
				return SW_FAIL;
			}
			break;

		default: 
			break;
		
	}
		
	return SW_OK;
}

static inline sw_error_t _mv88e6097_qos_show_status(a_uint32_t dev_id)
{
	GT_LPORT hport, lport, i;
	GT_BOOL MapRuleMode;
	GT_BOOL IpEnable, TagEnable;
	GT_STATUS status;
	GT_PORT_SCHED_MODE SchedMode;
	GT_U8 priority;

	printf("\n");
	printf("  QoS Status Information Display :\n");
	printf("  ==============================================\n");
	printf("  Port  AssignMode  MapRule  Priority   Schedule\n");
	printf("  ==============================================\n");
	
	for(i=0; i<CONFIG_JWS_PORT_TOTAL_NR; i++){
		
		lport = i+1;
		SW_RTN_ON_ERROR(map_lport2hport(lport, (a_uint32_t *)&hport));

		if((status = gqosGetUserPrioMapEn(pJwDev, hport, &TagEnable)) != GT_OK) {
			 printf("gprtGetPortSched Failed\n");
			 return SW_FAIL;
		}

		if((status = gqosGetIpPrioMapEn(pJwDev, hport, &IpEnable)) != GT_OK) {
			 printf("gprtGetPortSched Failed\n");
			 return SW_FAIL;
		}

		if((status = gqosGetPrioMapRule(pJwDev, hport, &MapRuleMode)) != GT_OK) {
			printf("gqosGetPrioMapRule Failed\n");
			return SW_FAIL;
		}
	
		if((status = gcosGetPortDefaultTc(pJwDev, hport, &priority)) != GT_OK) {
			 printf("gprtGetPortSched Failed\n");
			 return SW_FAIL;
		}

		if((status = gprtGetPortSched(pJwDev, hport, &SchedMode)) != GT_OK) {
			 printf("gprtGetPortSched Failed\n");
			 return SW_FAIL;
		}

		printf("   %02d    %-8s   %-6s    %-9s  %-4s\r\n", (int)lport, 	
			((TagEnable == GT_TRUE) && (IpEnable == GT_TRUE))? "Tag Ip" : \
			((TagEnable == GT_TRUE) && (IpEnable == GT_FALSE))? "Tag" : \
			((TagEnable == GT_FALSE) && (IpEnable == GT_TRUE))? "Ip" : "Default",
			(MapRuleMode == GT_TRUE)? "Tagmap" : "Ipmap",
			(priority == ((FAL_QOS_PRIO_LOWEST-1) * 2))? "Lowest" : \
			(priority == ((FAL_QOS_PRIO_LOW-1) * 2))? "Low" : \
			(priority == ((FAL_QOS_PRIO_HIGH-1) * 2))? "High" : \
			(priority == ((FAL_QOS_PRIO_HIGHEST-1)* 2))? "Highest" : "UN",
			(SchedMode == GT_PORT_SCHED_WEIGHTED_RRB)? "WRR" : "SR");
		
	}
	
	return SW_OK;
}






sw_error_t mv88e6097_qos_port_prio_map_rule_set(a_uint32_t dev_id,fal_port_t port_id,fal_qos_map_t qos_map)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _mv88e6097_qos_port_prio_map_rule_set(dev_id, port_id, qos_map);
    HSL_API_UNLOCK;
	
    return rv;
}

sw_error_t mv88e6097_qos_port_prio_map_rule_get(a_uint32_t dev_id,fal_port_t port_id,fal_qos_map_t * qos_map)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _mv88e6097_qos_port_prio_map_rule_get(dev_id, port_id, qos_map);
    HSL_API_UNLOCK;
	
    return rv;
}

sw_error_t mv88e6097_qos_port_schedule_mode_set(a_uint32_t dev_id,fal_port_t port_id,fal_qos_schedule_mode_t schedule_mode)
{
	sw_error_t rv;

    HSL_API_LOCK;
    rv = _mv88e6097_qos_port_schedule_mode_set(dev_id, port_id, schedule_mode);
    HSL_API_UNLOCK;
	
    return rv;
}

sw_error_t mv88e6097_qos_port_schedule_mode_get(a_uint32_t dev_id,fal_port_t port_id,fal_qos_schedule_mode_t * schedule_mode)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _mv88e6097_qos_port_schedule_mode_get(dev_id, port_id, schedule_mode);
    HSL_API_UNLOCK;
	
    return rv;
}

sw_error_t mv88e6097_qos_tag_prio_map_set(a_uint32_t dev_id,fal_tag_map_t *tag_mapping)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _mv88e6097_qos_tag_prio_map_set(dev_id, tag_mapping);
    HSL_API_UNLOCK;
	
    return rv;
}

sw_error_t mv88e6097_qos_ip_prio_map_set(a_uint32_t dev_id,fal_ip_map_t *ip_mapping)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _mv88e6097_qos_ip_prio_map_set(dev_id, ip_mapping);
    HSL_API_UNLOCK;
	
    return rv;
}

sw_error_t mv88e6097_qos_port_prio_assign_mode_set(a_uint32_t dev_id,fal_port_t port_id,fal_qos_assign_mode_t assign_mode)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _mv88e6097_qos_port_prio_assign_mode_set(dev_id, port_id, assign_mode);
    HSL_API_UNLOCK;
	
    return rv;
}

sw_error_t mv88e6097_qos_port_default_prio_set(a_uint32_t dev_id,fal_port_t port_id,fal_qos_prio_t priority)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _mv88e6097_qos_port_default_prio_set(dev_id, port_id, priority);
    HSL_API_UNLOCK;
	
    return rv;
}

sw_error_t mv88e6097_qos_show_status(a_uint32_t dev_id)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _mv88e6097_qos_show_status(dev_id);
    HSL_API_UNLOCK;
	
    return rv;
}



sw_error_t mv88e6097_qos_init(a_uint32_t dev_id)
{
    hsl_api_t *p_api;
	hsl_dev_t *pdev = NULL;

    pdev = hsl_dev_ptr_get(dev_id);
    if (pdev == NULL)
        return SW_NOT_INITIALIZED;
	
    HSL_DEV_ID_CHECK(dev_id);
	
    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));


	p_api->qos_port_prio_map_rule_set = mv88e6097_qos_port_prio_map_rule_set;
	p_api->qos_port_prio_map_rule_get = mv88e6097_qos_port_prio_map_rule_get;
	p_api->qos_port_schedule_mode_set = mv88e6097_qos_port_schedule_mode_set;	
	p_api->qos_port_schedule_mode_get = mv88e6097_qos_port_schedule_mode_get;
	p_api->qos_tag_prio_map_set = mv88e6097_qos_tag_prio_map_set;
	p_api->qos_ip_prio_map_set = mv88e6097_qos_ip_prio_map_set;
	p_api->qos_port_prio_assign_mode_set = mv88e6097_qos_port_prio_assign_mode_set;
	p_api->qos_port_default_prio_set = mv88e6097_qos_port_default_prio_set;
	p_api->qos_show_status = mv88e6097_qos_show_status;

	return SW_OK;
}


