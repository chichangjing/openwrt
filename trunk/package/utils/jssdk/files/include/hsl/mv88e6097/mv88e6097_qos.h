
#ifndef _MV88E6097_QOS_H
#define _MV88E6097_QOS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "fal/fal_qos.h"


sw_error_t mv88e6097_qos_init(a_uint32_t dev_id);
sw_error_t mv88e6097_qos_port_prio_map_rule_set(a_uint32_t dev_id, fal_port_t port_id, fal_qos_map_t qos_map);
sw_error_t mv88e6097_qos_port_prio_map_rule_get(a_uint32_t dev_id, fal_port_t port_id, fal_qos_map_t *qos_map);
sw_error_t mv88e6097_qos_port_schedule_mode_set(a_uint32_t dev_id, fal_port_t port_id, fal_qos_schedule_mode_t schedule_mode);
sw_error_t mv88e6097_qos_port_schedule_mode_get(a_uint32_t dev_id, fal_port_t port_id, fal_qos_schedule_mode_t *schedule_mode);
sw_error_t mv88e6097_qos_tag_prio_map_set(a_uint32_t dev_id,fal_tag_map_t *tag_mapping);
sw_error_t mv88e6097_qos_ip_prio_map_set(a_uint32_t dev_id,fal_ip_map_t *ip_mapping);
sw_error_t mv88e6097_qos_port_prio_assign_mode_set(a_uint32_t dev_id, fal_port_t port_id, fal_qos_assign_mode_t assign_mode);
sw_error_t mv88e6097_qos_port_default_prio_set(a_uint32_t dev_id, fal_port_t port_id, fal_qos_prio_t priority);
sw_error_t mv88e6097_qos_show_status(a_uint32_t dev_id);



#ifdef __cplusplus
}
#endif
#endif

