
#ifndef _MV88E6097_MULTICAST_H
#define _MV88E6097_MULTICAST_H

#ifdef __cplusplus
extern "C" {
#endif

#include "fal/fal_multicast.h"
#include "list.h"



typedef struct{
    struct list_head node;
	a_uint32_t lportvec;
    fal_multicast_addr_t ptr;
}MacCommit_S;



sw_error_t mv88e6097_multicast_init(a_uint32_t dev_id);

sw_error_t mv88e6097_multicast_enable_set(a_uint32_t dev_id, fal_enable_t enable);
sw_error_t mv88e6097_multicast_enable_get(a_uint32_t dev_id, fal_enable_t *enable);
sw_error_t mv88e6097_multicast_row_create(a_uint32_t dev_id, fal_multicast_members_t *multicast_members, fal_multicast_addr_t *multicast_addr, a_uint32_t members_size);
sw_error_t mv88e6097_multicast_row_destroy(a_uint32_t dev_id, fal_multicast_members_t *multicast_members, fal_multicast_addr_t *multicast_addr, a_uint32_t members_size);
sw_error_t mv88e6097_mgmt_frame_set(a_uint32_t dev_id, fal_multicast_addr_t *mgmt_addr, fal_enable_t enable);



#ifdef __cplusplus
}
#endif
#endif

