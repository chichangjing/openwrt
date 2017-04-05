 
#include "sw.h"
#include "sal_sys.h"
#include "hsl.h"
#include "hsl_dev.h"
#include "hsl_port_prop.h"
#include "mv3236_global.h"
#include "mv3236_qos.h"
#include "mv3236_port_ctrl.h"
#include "mv3236_reg.h"
#include "cpss/dxCh/dxChxGen/cos/cpssDxChCos.h"

//extern GT_QD_DEV *pJwDev;
static a_uint32_t Tag_Ip_Assign_Mode[MV3236_MAX_PORT_NUM-1];

static inline sw_error_t _mv3236_qos_port_prio_map_rule_set(a_uint32_t dev_id,fal_port_t port_id,fal_qos_map_t qos_map)
{
    SW_PRINTFL(DETAIL,"====enter====\n");

	a_uint32_t i;	
	a_uint32_t hport;
	GT_BOOL mode;
	GT_STATUS status;

	SW_PRINTFL(WARN,"may be 3336 not support this feature yet ????");
#if 0
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
#endif
	return SW_OK;
}

static inline sw_error_t _mv3236_qos_port_prio_map_rule_get(a_uint32_t dev_id,fal_port_t port_id,fal_qos_map_t * qos_map)
{
    SW_PRINTFL(DETAIL,"====enter====\n");

    a_uint32_t hport;
	GT_BOOL mode;
	GT_STATUS status;
#if 0
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
#endif
	return SW_OK;
}

static inline sw_error_t _mv3236_qos_port_schedule_mode_set(a_uint32_t dev_id,fal_port_t port_id,fal_qos_schedule_mode_t schedule_mode)
{
    SW_PRINTFL(DETAIL,"====enter====\n");

    a_uint32_t hport;
	GT_STATUS status;

	SW_RTN_ON_ERROR(map_lport2hport(port_id, (a_uint32_t *)&hport));
	 SW_PRINTFL(WARN,"TODO.====schedule_mode is only setted by profile setting not port ====\n");
#if 0
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
#endif
	return SW_OK;
}

static inline sw_error_t _mv3236_qos_port_schedule_mode_get(a_uint32_t dev_id,fal_port_t port_id,fal_qos_schedule_mode_t * schedule_mode)
{
    SW_PRINTFL(DETAIL,"====enter====\n");

    a_uint32_t hport;
	GT_STATUS status;

	SW_RTN_ON_ERROR(map_lport2hport(port_id, (a_uint32_t *)&hport));
#if 0
	if((status = gprtGetPortSched(pJwDev, hport, &mode)) != GT_OK) {
		 printf("gprtGetPortSched Failed\n");
		 return SW_FAIL;
	 }

	if(mode == GT_PORT_SCHED_WEIGHTED_RRB){
		*schedule_mode = FAL_QOS_WRR_MODE;
	}else if(mode == GT_PORT_SCHED_STRICT_PRI){
		*schedule_mode = FAL_QOS_SR_MODE;
	}
#endif
	return SW_OK;
}

static inline sw_error_t _mv3236_qos_tag_prio_map_set(a_uint32_t dev_id,fal_tag_map_t *tag_mapping)
{
    SW_PRINTFL(DETAIL,"====enter====\n");

	GT_STATUS status;
	GT_U8 priority;
	GT_U8 queue_num;
	CPSS_DXCH_COS_PROFILE_STC  cos;

	for(priority=0; priority<8; priority++) {   	
		queue_num = tag_mapping->tag_map[priority];

		if((status = cpssDxChCosUpCfiDeiToProfileMapSet(dev_id, 0, 0, priority, 0, queue_num - 1 +64)) < 0)
		{
			 SW_PRINTFL(WARN,"cpssDxChCosUpCfiDeiToProfileMapSet priority %d , cfi %d \n",priority, 0);
			 goto ERROR;
		}

		if((status = cpssDxChCosUpCfiDeiToProfileMapSet(dev_id, 0, 0, priority, 1, queue_num - 1 +64)) < 0)
		{
			 SW_PRINTFL(WARN,"cpssDxChCosUpCfiDeiToProfileMapSet priority %d , cfi %d \n",priority, 1);
			 goto ERROR;
		}

		if((status = cpssDxChCosProfileEntryGet(dev_id, queue_num - 1 +64, &cos)) < 0)
		{
			 SW_PRINTFL(WARN,"cpssDxChCosProfileEntryGet qos profile %d failed ret %d \n",priority, queue_num - 1 +64, status);
			 goto ERROR;
		}

		cos.trafficClass = queue_num;

		if((status = cpssDxChCosProfileEntrySet(dev_id, queue_num - 1 +64, &cos)) < 0)
		{
			 SW_PRINTFL(WARN,"cpssDxChCosProfileEntrySet qos profile %d failed  ret %d \n",priority, queue_num - 1 +64, status);
			 goto ERROR;
		}

	}

	return SW_OK;

ERROR:
	return SW_FAIL;
}

static inline sw_error_t _mv3236_qos_ip_prio_map_set(a_uint32_t dev_id,fal_ip_map_t *ip_mapping)
{
    SW_PRINTFL(DETAIL,"====enter====\n");
	GT_U8 priority;
	GT_U8 queue_num;
	GT_STATUS status;
	CPSS_DXCH_COS_PROFILE_STC cos;

	for(priority=0; priority<64; priority++) {

		queue_num = ip_mapping->ip_map[priority];

		if((status = cpssDxChCosProfileEntryGet(dev_id, priority, &cos)) < 0)
		{
			  SW_PRINTFL(WARN,"cpssDxChCosProfileEntryGet failed ret is %d\n", status);
			  break;
		}

		cos.trafficClass = queue_num;
		if((status = cpssDxChCosProfileEntrySet(dev_id, priority, &cos)) < 0)
		{
			  SW_PRINTFL(WARN,"cpssDxChCosProfileEntrySet failed ret is %d\n", status);
			  break;
		}

		if((status = cpssDxChCosDscpToProfileMapSet(dev_id, 0, priority, priority)) < 0)
		{
			  SW_PRINTFL(WARN,"cpssDxChCosProfileEntrySet failed ret is %d\n", status);
			  break;
		}

	} 

	return SW_OK;
}

static inline sw_error_t _mv3236_qos_port_prio_assign_mode_set(a_uint32_t dev_id,fal_port_t port_id,fal_qos_assign_mode_t assign_mode)
{
    SW_PRINTFL(DETAIL,"====enter====\n");
    GT_STATUS status;
    a_uint32_t hport;
	a_uint32_t i;

	if(port_id > (MV3236_MAX_PORT_NUM-1))
		return SW_BAD_PARAM;

	SW_RTN_ON_ERROR(map_lport2hport(port_id, (a_uint32_t *)&hport));

	if((status = cpssDxChCosPortQosTrustModeSet(dev_id, hport,  (assign_mode - 1) )) < 0)
	{
		SW_PRINTFL(WARN,"cpssDxChCosPortQosTrustModeSet failed ret is %d\n", status);
		return SW_FAIL;
	}

	return SW_OK;
}

static inline sw_error_t _mv3236_qos_port_default_prio_set(a_uint32_t dev_id,fal_port_t port_id,fal_qos_prio_t priority)
{
    SW_PRINTFL(DETAIL,"====enter====\n");

	GT_STATUS status;
	a_uint32_t hport;
	GT_U8 priority_level;
	
	if(port_id > (MV3236_MAX_PORT_NUM-1))
		return SW_BAD_PARAM;

	SW_RTN_ON_ERROR(map_lport2hport(port_id, (a_uint32_t *)&hport));

	switch(priority) {
		case FAL_QOS_PRIO_LOWEST:
			priority_level = 0;
			if((status = cpssDxChPortDefaultUPSet(dev_id, hport, priority_level * 2)) != GT_OK) {
				printf("gcosSetPortDefaultTc Failed\n");
				return SW_FAIL;
			}
			break;

		case FAL_QOS_PRIO_LOW:
			priority_level = 1;
			if((status = cpssDxChPortDefaultUPSet(dev_id, hport, priority_level * 2)) != GT_OK) {
				printf("gcosSetPortDefaultTc Failed\n");
				return SW_FAIL;
			}
			break;

		case FAL_QOS_PRIO_HIGH:
			priority_level = 2;
			if((status = cpssDxChPortDefaultUPSet(dev_id, hport, priority_level * 2)) != GT_OK) {
				printf("gcosSetPortDefaultTc Failed\n");
				return SW_FAIL;
			}
			break;

		case FAL_QOS_PRIO_HIGHEST:
			priority_level = 3;
			if((status = cpssDxChPortDefaultUPSet(dev_id, hport, priority_level * 2)) != GT_OK) {
				printf("gcosSetPortDefaultTc Failed\n");
				return SW_FAIL;
			}
			break;

		default: 
			break;
		
	}

	return SW_OK;
}

static inline sw_error_t _mv3236_qos_show_status(a_uint32_t dev_id)
{
    SW_PRINTFL(DETAIL,"====enter====\n");
#if 0
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

	for(i=0; i<MV3236_MAX_PORT_NUM-1; i++){
		
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

		printf("   %02d    %-8s   %-6s    %-9s  %-4s\r\n", lport, 	
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
#endif
	return SW_OK;
}

sw_error_t mv3236_qos_port_prio_map_rule_set(a_uint32_t dev_id,fal_port_t port_id,fal_qos_map_t qos_map)
{
    SW_PRINTFL(DETAIL,"====enter====\n");
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _mv3236_qos_port_prio_map_rule_set(dev_id, port_id, qos_map);
    HSL_API_UNLOCK;
	
    return rv;
}

sw_error_t mv3236_qos_port_prio_map_rule_get(a_uint32_t dev_id,fal_port_t port_id,fal_qos_map_t * qos_map)
{
    SW_PRINTFL(DETAIL,"====enter====\n");
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _mv3236_qos_port_prio_map_rule_get(dev_id, port_id, qos_map);
    HSL_API_UNLOCK;
	
    return rv;
}

sw_error_t mv3236_qos_port_schedule_mode_set(a_uint32_t dev_id,fal_port_t port_id,fal_qos_schedule_mode_t schedule_mode)
{
    SW_PRINTFL(DETAIL,"====enter====\n");
	sw_error_t rv;

    HSL_API_LOCK;
    rv = _mv3236_qos_port_schedule_mode_set(dev_id, port_id, schedule_mode);
    HSL_API_UNLOCK;
	
    return rv;
}

sw_error_t mv3236_qos_port_schedule_mode_get(a_uint32_t dev_id,fal_port_t port_id,fal_qos_schedule_mode_t * schedule_mode)
{
    SW_PRINTFL(DETAIL,"====enter====\n");
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _mv3236_qos_port_schedule_mode_get(dev_id, port_id, schedule_mode);
    HSL_API_UNLOCK;
	
    return rv;
}

sw_error_t mv3236_qos_tag_prio_map_set(a_uint32_t dev_id,fal_tag_map_t *tag_mapping)
{
    SW_PRINTFL(DETAIL,"====enter====\n");
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _mv3236_qos_tag_prio_map_set(dev_id, tag_mapping);
    HSL_API_UNLOCK;
	
    return rv;
}

sw_error_t mv3236_qos_ip_prio_map_set(a_uint32_t dev_id,fal_ip_map_t *ip_mapping)
{
    SW_PRINTFL(DETAIL,"====enter====\n");
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _mv3236_qos_ip_prio_map_set(dev_id, ip_mapping);
    HSL_API_UNLOCK;
	
    return rv;
}

sw_error_t mv3236_qos_port_prio_assign_mode_set(a_uint32_t dev_id,fal_port_t port_id,fal_qos_assign_mode_t assign_mode)
{
    SW_PRINTFL(DETAIL,"====enter====\n");
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _mv3236_qos_port_prio_assign_mode_set(dev_id, port_id, assign_mode);
    HSL_API_UNLOCK;
	
    return rv;
}

sw_error_t mv3236_qos_port_default_prio_set(a_uint32_t dev_id,fal_port_t port_id,fal_qos_prio_t priority)
{
    SW_PRINTFL(DETAIL,"====enter====\n");
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _mv3236_qos_port_default_prio_set(dev_id, port_id, priority);
    HSL_API_UNLOCK;
	
    return rv;
}

sw_error_t mv3236_qos_show_status(a_uint32_t dev_id)
{
    SW_PRINTFL(DETAIL,"====enter====\n");
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _mv3236_qos_show_status(dev_id);
    HSL_API_UNLOCK;
	
    return rv;
}



sw_error_t mv3236_qos_init(a_uint32_t dev_id)
{
    SW_PRINTFL(DETAIL,"====enter====\n");
    hsl_api_t *p_api;
	hsl_dev_t *pdev = NULL;

    pdev = hsl_dev_ptr_get(dev_id);
    if (pdev == NULL)
        return SW_NOT_INITIALIZED;
	
    HSL_DEV_ID_CHECK(dev_id);
	
    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));


	p_api->qos_port_prio_map_rule_set = mv3236_qos_port_prio_map_rule_set;
	p_api->qos_port_prio_map_rule_get = mv3236_qos_port_prio_map_rule_get;
	p_api->qos_port_schedule_mode_set = mv3236_qos_port_schedule_mode_set;
	p_api->qos_port_schedule_mode_get = mv3236_qos_port_schedule_mode_get;
	p_api->qos_tag_prio_map_set = mv3236_qos_tag_prio_map_set;
	p_api->qos_ip_prio_map_set = mv3236_qos_ip_prio_map_set;
	p_api->qos_port_prio_assign_mode_set = mv3236_qos_port_prio_assign_mode_set;
	p_api->qos_port_default_prio_set = mv3236_qos_port_default_prio_set;
	p_api->qos_show_status = mv3236_qos_show_status;

	return SW_OK;
}


