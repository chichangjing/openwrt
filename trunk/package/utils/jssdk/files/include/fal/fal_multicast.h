
#ifndef _FAL_MULTICAST_H_
#define _FAL_MULTICAST_H_

#ifdef __cplusplus
extern "c" {
#endif

#include "common/sw.h"
#include "fal/fal_type.h"


//typedef a_uint8_t fal_multicast_members_t;

typedef struct {
	a_uint8_t members[MV88E6097_MAX_MEMBERS_SIZE];
} fal_multicast_members_t;

//typedef a_uint32_t fal_multicast_addr_t;

typedef struct {
    a_uint8_t mac[6];
} fal_multicast_addr_t;



sw_error_t	fal_multicast_enable_set(a_uint32_t dev_id, fal_enable_t enable);
sw_error_t	fal_multicast_enable_get(a_uint32_t dev_id, fal_enable_t *enable);
sw_error_t	fal_multicast_row_create(a_uint32_t dev_id, fal_multicast_members_t *multicast_members, fal_multicast_addr_t *multicast_addr, a_uint32_t members_size);
sw_error_t	fal_multicast_row_destroy(a_uint32_t dev_id,fal_multicast_members_t *multicast_members, fal_multicast_addr_t *multicast_addr, a_uint32_t members_size);
sw_error_t	fal_mgmt_frame_set(a_uint32_t dev_id, fal_multicast_addr_t *mgmt_addr, fal_enable_t enable);

#ifdef __cplusplus
}
#endif
#endif

