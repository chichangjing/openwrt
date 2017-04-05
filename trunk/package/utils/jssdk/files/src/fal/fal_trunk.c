
#include "sw.h"
#include "fal_trunk.h"
#include "hsl_api.h"
#include "sal_sys.h"


static inline sw_error_t _fal_trunk_pag_mode_set(a_uint32_t dev_id,fal_trunk_mode_t pag_mode)
{
	sw_error_t rv;
	hsl_api_t *p_api;

	SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

	if (NULL == p_api->trunk_pag_mode_set)
		return SW_NOT_SUPPORTED;

	rv = p_api->trunk_pag_mode_set(dev_id, pag_mode);
	return rv;
}

static inline sw_error_t _fal_trunk_pag_mode_get(a_uint32_t dev_id,fal_trunk_mode_t *pag_mode)
{
	sw_error_t rv;
	hsl_api_t *p_api;

	SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

	if (NULL == p_api->trunk_pag_mode_get)
		return SW_NOT_SUPPORTED;

	rv = p_api->trunk_pag_mode_get(dev_id, pag_mode);
	return rv;
}

static inline sw_error_t _fal_trunk_load_balance_set(a_uint32_t dev_id, fal_trunk_load_balance_t mode)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->trunk_load_balance_set)
        return SW_NOT_SUPPORTED;

    rv = p_api->trunk_load_balance_set(dev_id, mode);
    return rv;
}

static inline sw_error_t _fal_trunk_load_balance_get(a_uint32_t dev_id, fal_trunk_load_balance_t *mode)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->trunk_load_balance_get)
        return SW_NOT_SUPPORTED;

    rv = p_api->trunk_load_balance_get(dev_id, mode);
    return rv;
}

static inline sw_error_t  _fal_trunk_id_set(a_uint32_t dev_id, fal_trunk_id_t trunk_id)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->trunk_id_set)
        return SW_NOT_SUPPORTED;

    rv = p_api->trunk_id_set(dev_id, trunk_id);
    return rv;
}

static inline sw_error_t _fal_trunk_members_set(a_uint32_t dev_id, fal_trunk_members_t *trunk_members)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->trunk_members_set)
        return SW_NOT_SUPPORTED;

    rv = p_api->trunk_members_set(dev_id, trunk_members);
    return rv;
}

static inline sw_error_t _fal_trunk_row_create(a_uint32_t dev_id, fal_trunk_id_t trunk_id, fal_trunk_members_t *trunk_members, a_uint32_t member_size)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->trunk_row_create)
        return SW_NOT_SUPPORTED;

    rv = p_api->trunk_row_create(dev_id, trunk_id, trunk_members, member_size);
    return rv;
}

static inline sw_error_t _fal_trunk_row_destroy(a_uint32_t dev_id, fal_trunk_id_t trunk_id)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->trunk_row_destroy)
        return SW_NOT_SUPPORTED;

    rv = p_api->trunk_row_destroy(dev_id, trunk_id);
    return rv;
}

static inline sw_error_t _fal_trunk_show_row_status(a_uint32_t dev_id, fal_trunk_id_t trunk_id)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->trunk_show_row_status)
        return SW_NOT_SUPPORTED;

    rv = p_api->trunk_show_row_status(dev_id, trunk_id);
    return rv;
}





sw_error_t	fal_trunk_pag_mode_set(a_uint32_t dev_id,fal_trunk_mode_t pag_mode)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_trunk_pag_mode_set(dev_id, pag_mode);
    FAL_API_UNLOCK;
    return rv;	
}

sw_error_t	fal_trunk_pag_mode_get(a_uint32_t dev_id,fal_trunk_mode_t *pag_mode)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_trunk_pag_mode_get(dev_id, pag_mode);
    FAL_API_UNLOCK;
    return rv;	
}

sw_error_t	fal_trunk_load_balance_set(a_uint32_t dev_id, fal_trunk_load_balance_t mode)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_trunk_load_balance_set(dev_id, mode);
    FAL_API_UNLOCK;
    return rv;	
}

sw_error_t	fal_trunk_load_balance_get(a_uint32_t dev_id, fal_trunk_load_balance_t *mode)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_trunk_load_balance_get(dev_id, mode);
    FAL_API_UNLOCK;
    return rv;
}

sw_error_t	fal_trunk_id_set(a_uint32_t dev_id, fal_trunk_id_t trunk_id)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_trunk_id_set(dev_id, trunk_id);
    FAL_API_UNLOCK;
    return rv;
}

sw_error_t	fal_trunk_members_set(a_uint32_t dev_id, fal_trunk_members_t *trunk_members)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_trunk_members_set(dev_id, trunk_members);
    FAL_API_UNLOCK;
    return rv;
}

sw_error_t	fal_trunk_row_create(a_uint32_t dev_id, fal_trunk_id_t trunk_id, fal_trunk_members_t *trunk_members, a_uint32_t member_size)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_trunk_row_create(dev_id, trunk_id, trunk_members, member_size);
    FAL_API_UNLOCK;
    return rv;
}

sw_error_t	fal_trunk_row_destroy(a_uint32_t dev_id, fal_trunk_id_t trunk_id)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_trunk_row_destroy(dev_id, trunk_id);
    FAL_API_UNLOCK;
    return rv;
}

sw_error_t	fal_trunk_show_row_status(a_uint32_t dev_id, fal_trunk_id_t trunk_id)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_trunk_show_row_status(dev_id, trunk_id);
    FAL_API_UNLOCK;
    return rv;
}

