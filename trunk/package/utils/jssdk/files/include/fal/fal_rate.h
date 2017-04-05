
#ifndef _FAL_RATE_H
#define _FAL_RATE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "common/sw.h"
#include "fal/fal_type.h"


typedef a_uint32_t fal_rate_bucket_t;
typedef a_uint32_t fal_rate_speed_t;
typedef a_uint32_t fal_rate_mask_t;

	

sw_error_t fal_rate_bucket_egrl_set(a_uint32_t dev_id,fal_port_t port_id,fal_rate_speed_t speed,fal_enable_t enable);
sw_error_t fal_rate_bucket_egrl_get(a_uint32_t dev_id,fal_port_t port_id,fal_rate_speed_t *speed,fal_enable_t *enable);
sw_error_t fal_rate_bucket_pirl_set(a_uint32_t dev_id,fal_port_t port_id,fal_rate_bucket_t bucket_id,fal_rate_mask_t type_mask,fal_rate_speed_t speed,fal_enable_t enable);
sw_error_t fal_rate_bucket_pirl_get(a_uint32_t dev_id,fal_port_t port_id,fal_rate_bucket_t bucket_id,fal_rate_mask_t *type_mask,fal_rate_speed_t *speed,fal_enable_t *enable);





#ifdef __cplusplus
}
#endif

#endif

