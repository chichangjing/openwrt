
#ifndef _FAL_TRUNK_H_
#define _FAL_TRUNK_H_

#ifdef __cplusplus
extern "c" {
#endif

#include "common/sw.h"
#include "fal/fal_type.h"

typedef enum {
	FAL_TRUNK_LOAD_BALANCE_XOR   = 1,
	FAL_TRUNK_LOAD_BALANCE_HASH  = 2,
	FAL_TRUNK_LOAD_BALANCE_END   = 3
} fal_trunk_load_balance_t;

typedef enum {
	FAL_TRUNK_MODE_DISABLE	= 1,
	FAL_TRUNK_MODE_STATIC	= 2,
	FAL_TRUNK_MODE_END		= 3
} fal_trunk_mode_t;

typedef a_uint32_t fal_trunk_id_t;


typedef struct {
	a_uint8_t members[MV88E6097_MAX_MEMBERS_SIZE];
} fal_trunk_members_t;




sw_error_t	fal_trunk_pag_mode_set(a_uint32_t dev_id, fal_trunk_mode_t pag_mode);
sw_error_t	fal_trunk_pag_mode_get(a_uint32_t dev_id, fal_trunk_mode_t *pag_mode);
sw_error_t	fal_trunk_load_balance_set(a_uint32_t dev_id, fal_trunk_load_balance_t mode);
sw_error_t	fal_trunk_load_balance_get(a_uint32_t dev_id, fal_trunk_load_balance_t *mode);
sw_error_t	fal_trunk_id_set(a_uint32_t dev_id, fal_trunk_id_t trunk_id);
sw_error_t	fal_trunk_members_set(a_uint32_t dev_id, fal_trunk_members_t *trunk_members);
sw_error_t	fal_trunk_row_create(a_uint32_t dev_id, fal_trunk_id_t trunk_id, fal_trunk_members_t *trunk_members, a_uint32_t member_size);
sw_error_t	fal_trunk_row_destroy(a_uint32_t dev_id, fal_trunk_id_t trunk_id);
sw_error_t	fal_trunk_show_row_status(a_uint32_t dev_id, fal_trunk_id_t trunk_id);

#ifdef __cplusplus
}
#endif
#endif

