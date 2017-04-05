
#ifndef _FAL_VLAN_H
#define _FAL_VLAN_H

#ifdef __cplusplus
extern "C" {
#endif

#include "common/sw.h"
#include "fal/fal_type.h"
#include "common/list.h"



typedef enum {
	FAL_VLAN_MODE_PORT_BASED = 1,
	FAL_VLAN_MODE_8021Q		 = 2,
	FAL_VLAN_MODE_END		 = 3
}fal_vlan_mode_t;

typedef struct {
	a_uint8_t members[MV88E6097_MAX_MEMBERS_SIZE];
} fal_vlan_members_t;

typedef struct {
	a_uint8_t members[MV88E6097_MAX_MEMBERS_SIZE];
} fal_vlan_egress_tag_t;


typedef a_uint16_t fal_vlan_id_t;


	

sw_error_t fal_vlan_mode_set(a_uint32_t dev_id, fal_vlan_mode_t mode);
sw_error_t fal_vlan_mode_get(a_uint32_t dev_id, fal_vlan_mode_t *mode);
sw_error_t fal_port_based_vlan_set(a_uint32_t dev_id, fal_port_t port_id, fal_vlan_members_t *vlan_members, a_uint32_t member_size);
sw_error_t fal_8021q_vlan_row_create(a_uint32_t dev_id, fal_vlan_id_t vlan_id, fal_vlan_members_t *port_members, a_uint32_t member_size, fal_vlan_egress_tag_t *egress_tag);
sw_error_t fal_8021q_vlan_row_destroy(a_uint32_t dev_id, fal_vlan_id_t vlan_id);
sw_error_t fal_8021q_vlan_show_status(a_uint32_t dev_id);
sw_error_t fal_8021q_vlan_row_status(a_uint32_t dev_id, fal_vlan_id_t *vlan_id, fal_vlan_egress_tag_t *egress_tag);




#ifdef __cplusplus
}
#endif

#endif

