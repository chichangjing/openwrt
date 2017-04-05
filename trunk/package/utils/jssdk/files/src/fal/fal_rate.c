
#include "sw.h"
#include "fal_rate.h"
#include "hsl_api.h"
#include "sal_sys.h"

static inline sw_error_t _fal_rate_bucket_egrl_set(a_uint32_t dev_id, fal_port_t port_id, fal_rate_speed_t speed, fal_enable_t enable)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->rate_bucket_egrl_set)
        return SW_NOT_SUPPORTED;

    rv = p_api->rate_bucket_egrl_set(dev_id, port_id, speed, enable);
    return rv;
}

static inline sw_error_t _fal_rate_bucket_egrl_get(a_uint32_t dev_id, fal_port_t port_id, fal_rate_speed_t *speed, fal_enable_t * enable)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->rate_bucket_egrl_get)
        return SW_NOT_SUPPORTED;

    rv = p_api->rate_bucket_egrl_get(dev_id, port_id, speed, enable);
    return rv;
}

static inline sw_error_t _fal_rate_bucket_pirl_set(a_uint32_t dev_id,fal_port_t port_id,fal_rate_bucket_t bucket_id,fal_rate_mask_t type_mask,fal_rate_speed_t speed,fal_enable_t enable)
{
	sw_error_t rv;
	hsl_api_t *p_api;

	SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

	if (NULL == p_api->rate_bucket_pirl_set)
		return SW_NOT_SUPPORTED;

	rv = p_api->rate_bucket_pirl_set(dev_id, port_id, bucket_id, type_mask, speed, enable);
	return rv;
}

static inline sw_error_t _fal_rate_bucket_pirl_get(a_uint32_t dev_id,fal_port_t port_id,fal_rate_bucket_t bucket_id,fal_rate_mask_t * type_mask,fal_rate_speed_t * speed,fal_enable_t * enable)
{
	sw_error_t rv;
	hsl_api_t *p_api;

	SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

	if (NULL == p_api->rate_bucket_pirl_get)
		return SW_NOT_SUPPORTED;

	rv = p_api->rate_bucket_pirl_get(dev_id, port_id, bucket_id, type_mask, speed, enable);
	return rv;
}





sw_error_t	fal_rate_bucket_egrl_set(a_uint32_t dev_id,fal_port_t port_id,fal_rate_speed_t speed,fal_enable_t enable)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_rate_bucket_egrl_set(dev_id, port_id, speed, enable);
    FAL_API_UNLOCK;
    return rv;
}

sw_error_t	fal_rate_bucket_egrl_get(a_uint32_t dev_id,fal_port_t port_id,fal_rate_speed_t * speed,fal_enable_t *enable)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_rate_bucket_egrl_get(dev_id, port_id, speed, enable);
    FAL_API_UNLOCK;
    return rv;
}

sw_error_t	fal_rate_bucket_pirl_set(a_uint32_t dev_id,fal_port_t port_id,fal_rate_bucket_t bucket_id,fal_rate_mask_t type_mask,fal_rate_speed_t speed,fal_enable_t enable)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_rate_bucket_pirl_set(dev_id, port_id, bucket_id, type_mask, speed, enable);
    FAL_API_UNLOCK;
    return rv;
}

sw_error_t	fal_rate_bucket_pirl_get(a_uint32_t dev_id,fal_port_t port_id,fal_rate_bucket_t bucket_id,fal_rate_mask_t * type_mask,fal_rate_speed_t * speed,fal_enable_t * enable)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_rate_bucket_pirl_get(dev_id, port_id, bucket_id, type_mask, speed, enable);
    FAL_API_UNLOCK;
    return rv;
}




