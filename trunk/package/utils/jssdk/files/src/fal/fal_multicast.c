
#include "sw.h"
#include "fal_multicast.h"
#include "hsl_api.h"
#include "sal_sys.h"


static inline sw_error_t _fal_multicast_enable_set(a_uint32_t dev_id, fal_enable_t enable)
{
	sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->multicast_enable_set)
        return SW_NOT_SUPPORTED;

    rv = p_api->multicast_enable_set(dev_id, enable);
    return rv;
}

static inline sw_error_t _fal_multicast_enable_get(a_uint32_t dev_id, fal_enable_t *enable)
{
	sw_error_t rv;
	hsl_api_t *p_api;

	SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

	if (NULL == p_api->multicast_enable_get)
		return SW_NOT_SUPPORTED;

	rv = p_api->multicast_enable_get(dev_id, enable);
	return rv;

}

static inline sw_error_t _fal_multicast_row_create(a_uint32_t dev_id, fal_multicast_members_t *multicast_members, fal_multicast_addr_t *multicast_addr, a_uint32_t members_size)
{
	sw_error_t rv;
	hsl_api_t *p_api;

	SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

	if (NULL == p_api->multicast_row_create)
		return SW_NOT_SUPPORTED;

	rv = p_api->multicast_row_create(dev_id, multicast_members, multicast_addr, members_size);
	return rv;
}

static inline sw_error_t _fal_multicast_row_destroy(a_uint32_t dev_id, fal_multicast_members_t *multicast_members, fal_multicast_addr_t *multicast_addr, a_uint32_t members_size)
{
	sw_error_t rv;
	hsl_api_t *p_api;

	SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

	if (NULL == p_api->multicast_row_destroy)
		return SW_NOT_SUPPORTED;

	rv = p_api->multicast_row_destroy(dev_id, multicast_members, multicast_addr, members_size);
	return rv;
}

static inline sw_error_t _fal_mgmt_frame_set(a_uint32_t dev_id,fal_multicast_addr_t *mgmt_addr,fal_enable_t enable)
{
	sw_error_t rv;
	hsl_api_t *p_api;

	SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

	if (NULL == p_api->mgmt_frame_set)
		return SW_NOT_SUPPORTED;

	rv = p_api->mgmt_frame_set(dev_id, mgmt_addr, enable);
	return rv;
}




sw_error_t	fal_multicast_enable_set(a_uint32_t dev_id, fal_enable_t enable)
{
	sw_error_t rv;
	
	FAL_API_LOCK;
	rv = _fal_multicast_enable_set(dev_id, enable);
	FAL_API_UNLOCK;
	return rv;
	
}

sw_error_t	fal_multicast_enable_get(a_uint32_t dev_id, fal_enable_t *enable)
{
	sw_error_t rv;
	
	FAL_API_LOCK;
	rv = _fal_multicast_enable_get(dev_id, enable);
	FAL_API_UNLOCK;
	return rv;

}

sw_error_t	fal_multicast_row_create(a_uint32_t dev_id, fal_multicast_members_t *multicast_members, fal_multicast_addr_t *multicast_addr, a_uint32_t members_size)
{
	sw_error_t rv;

	FAL_API_LOCK;
	rv = _fal_multicast_row_create(dev_id, multicast_members, multicast_addr, members_size);
	FAL_API_UNLOCK;
	return rv;
}

sw_error_t	fal_multicast_row_destroy(a_uint32_t dev_id, fal_multicast_members_t *multicast_members, fal_multicast_addr_t *multicast_addr, a_uint32_t members_size)
{
	sw_error_t rv;
	
	FAL_API_LOCK;
	rv = _fal_multicast_row_destroy(dev_id, multicast_members, multicast_addr, members_size);
	FAL_API_UNLOCK;
	return rv;
}

sw_error_t	fal_mgmt_frame_set(a_uint32_t dev_id,fal_multicast_addr_t *mgmt_addr,fal_enable_t enable)
{
	sw_error_t rv;
	
	FAL_API_LOCK;
	rv = _fal_mgmt_frame_set(dev_id, mgmt_addr, enable);
	FAL_API_UNLOCK;
	return rv;
}


