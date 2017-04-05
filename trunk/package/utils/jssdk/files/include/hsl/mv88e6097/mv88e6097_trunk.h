
#ifndef _MV88E6097_TRUNK_H
#define _MV88E6097_TRUNK_H

#ifdef __cplusplus
extern "C" {
#endif

#include "fal/fal_trunk.h"


typedef struct {
	fal_trunk_id_t	Trunkid;
	a_uint32_t 		TrunkPortNum;
	a_uint32_t		TrunkMemberMask;
} trunk_rec;


typedef struct{
	struct list_head node;
	a_uint8_t	TrunkCount; 
	trunk_rec 	TrunkGroupRec;
}TrunkCommit_S;



a_uint32_t trunk_find_group(fal_port_t lport);
sw_error_t trunk_add_number(fal_trunk_id_t trunk_id,fal_port_t lport);
sw_error_t trunk_delete_number(fal_trunk_id_t trunk_id,fal_port_t lport);
sw_error_t trunk_link_status_change(fal_port_t lport,fal_port_link_status_t link_status);

sw_error_t mv88e6097_port_trunk_init(a_uint32_t dev_id);
sw_error_t mv88e6097_trunk_pag_mode_set(a_uint32_t dev_id, fal_trunk_mode_t pag_mode);
sw_error_t mv88e6097_trunk_pag_mode_get(a_uint32_t dev_id, fal_trunk_mode_t *pag_mode);
sw_error_t mv88e6097_trunk_load_balance_set(a_uint32_t dev_id, fal_trunk_load_balance_t mode);
sw_error_t mv88e6097_trunk_load_balance_get(a_uint32_t dev_id, fal_trunk_load_balance_t *mode);
sw_error_t mv88e6097_trunk_id_set(a_uint32_t dev_id, fal_trunk_id_t trunk_id);
sw_error_t mv88e6097_trunk_members_set(a_uint32_t dev_id, fal_trunk_members_t *trunk_members);
sw_error_t mv88e6097_trunk_row_create(a_uint32_t dev_id, fal_trunk_id_t trunk_id, fal_trunk_members_t *trunk_members, a_uint32_t member_size);
sw_error_t mv88e6097_trunk_row_destroy(a_uint32_t dev_id, fal_trunk_id_t trunk_id);
sw_error_t mv88e6097_trunk_show_row_status(a_uint32_t dev_id, fal_trunk_id_t trunk_id);

#define MAX_TRUNK_GROUP_ID			15
#define MAX_TRUNK_GROUP_PORT_NUM	8	


#ifdef __cplusplus
}
#endif
#endif

