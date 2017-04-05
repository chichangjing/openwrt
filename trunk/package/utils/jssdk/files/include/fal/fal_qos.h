
#ifndef _FAL_QOS_H
#define _FAL_QOS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "common/sw.h"
#include "fal/fal_type.h"



typedef enum {
	FAL_QOS_IP_MAPPING	= 1,
	FAL_QOS_TAG_MAPPING = 2,
	FAL_QOS_MAP_END		= 3
}fal_qos_map_t;

typedef enum {
	FAL_QOS_SR_MODE		= 1,
	FAL_QOS_WRR_MODE	= 2,
	FAL_QOS_MODE_END	= 3
}fal_qos_schedule_mode_t;

typedef enum {
	FAL_QOS_USE_DEFAULT_ASSIGN_MODE  = 1,
	FAL_QOS_USE_TAG_ASSIGN_MODE		 = 2,
	FAL_QOS_USE_IP_ASSIGN_MODE		 = 3,
	FAL_QOS_USE_TAG_IP_ASSIGN_MODE	 = 4,
	FAL_QOS_ASSIGN_MODE_END			 = 5
}fal_qos_assign_mode_t;

typedef enum {
	FAL_QOS_PRIO_LOWEST  = 1,
	FAL_QOS_PRIO_LOW	 = 2,
	FAL_QOS_PRIO_HIGH 	 = 3,
	FAL_QOS_PRIO_HIGHEST = 4,
	FAL_QOS_PRIO_END	 = 5
}fal_qos_prio_t;

typedef struct {
	a_uint8_t tag_map[8];
} fal_tag_map_t;

typedef struct {
	a_uint8_t ip_map[64];
} fal_ip_map_t;
	


sw_error_t fal_qos_port_prio_map_rule_set(a_uint32_t dev_id, fal_port_t port_id, fal_qos_map_t qos_map);
sw_error_t fal_qos_port_prio_map_rule_get(a_uint32_t dev_id, fal_port_t port_id, fal_qos_map_t *qos_map);
sw_error_t fal_qos_port_schedule_mode_set(a_uint32_t dev_id, fal_port_t port_id, fal_qos_schedule_mode_t schedule_mode);
sw_error_t fal_qos_port_schedule_mode_get(a_uint32_t dev_id, fal_port_t port_id, fal_qos_schedule_mode_t *schedule_mode);
sw_error_t fal_qos_tag_prio_map_set(a_uint32_t dev_id,fal_tag_map_t *tag_mapping);
sw_error_t fal_qos_ip_prio_map_set(a_uint32_t dev_id,fal_ip_map_t *ip_mapping);
sw_error_t fal_qos_port_prio_assign_mode_set(a_uint32_t dev_id, fal_port_t port_id, fal_qos_assign_mode_t assign_mode);
sw_error_t fal_qos_port_default_prio_set(a_uint32_t dev_id, fal_port_t port_id, fal_qos_prio_t priority);
sw_error_t fal_qos_show_status(a_uint32_t dev_id);




#ifdef __cplusplus
}
#endif

#endif

