
#ifndef _MV88E6097_VLAN_H
#define _MV88E6097_VLAN_H

#ifdef __cplusplus
extern "C" {
#endif

#include "fal/fal_vlan.h"



sw_error_t mv88e6097_vlan_init(a_uint32_t dev_id);
sw_error_t mv88e6097_vlan_mode_set(a_uint32_t dev_id, fal_vlan_mode_t mode);
sw_error_t mv88e6097_vlan_mode_get(a_uint32_t dev_id, fal_vlan_mode_t *mode);
sw_error_t mv88e6097_port_based_vlan_set(a_uint32_t dev_id, fal_port_t port_id, fal_vlan_members_t *vlan_members, a_uint32_t member_size);
sw_error_t mv88e6097_8021q_vlan_row_create(a_uint32_t dev_id, fal_vlan_id_t vlan_id, fal_vlan_members_t *port_members, a_uint32_t member_size, fal_vlan_egress_tag_t *egress_tag);
sw_error_t mv88e6097_8021q_vlan_row_destroy(a_uint32_t dev_id, fal_vlan_id_t vlan_id);
sw_error_t mv88e6097_8021q_vlan_show_status(a_uint32_t dev_id);
sw_error_t mv88e6097_8021q_vlan_row_status(a_uint32_t dev_id, fal_vlan_id_t *vlan_id, fal_vlan_egress_tag_t *egress_tag);


#ifdef __cplusplus
}
#endif
#endif

