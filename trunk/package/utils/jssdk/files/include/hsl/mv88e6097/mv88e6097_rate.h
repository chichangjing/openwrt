
#ifndef _MV88E6097_RATE_H
#define _MV88E6097_RATE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "fal/fal_rate.h"


#define MV88E6097_MAX_BUCKET_NUM 5

sw_error_t mv88e6097_rate_limit_init(a_uint32_t dev_id);
sw_error_t mv88e6097_rate_bucket_egrl_set(a_uint32_t dev_id,fal_port_t port_id,fal_rate_speed_t speed,fal_enable_t enable);
sw_error_t mv88e6097_rate_bucket_egrl_get(a_uint32_t dev_id,fal_port_t port_id,fal_rate_speed_t *speed,fal_enable_t *enable);
sw_error_t mv88e6097_rate_bucket_pirl_set(a_uint32_t dev_id,fal_port_t port_id,fal_rate_bucket_t bucket_id,fal_rate_mask_t type_mask,fal_rate_speed_t speed,fal_enable_t enable);
sw_error_t mv88e6097_rate_bucket_pirl_get(a_uint32_t dev_id,fal_port_t port_id,fal_rate_bucket_t bucket_id,fal_rate_mask_t *type_mask,fal_rate_speed_t *speed,fal_enable_t *enable);



#ifdef __cplusplus
}
#endif
#endif

