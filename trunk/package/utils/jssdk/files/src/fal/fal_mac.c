
#include "sw.h"
#include "fal_mac.h"
#include "hsl_api.h"
#include "sal_sys.h"



static inline sw_error_t _fal_mac_aging_timeout_set(a_uint32_t dev_id,fal_mac_aging_t aging_time)
{
	sw_error_t rv;
	hsl_api_t *p_api;

	SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

	if (NULL == p_api->mac_aging_timeout_set)
		return SW_NOT_SUPPORTED;

	rv = p_api->mac_aging_timeout_set(dev_id, aging_time);
	return rv;
}

static inline sw_error_t _fal_mac_secure_port_enable_set(a_uint32_t dev_id,fal_port_t port_id,fal_enable_t enable)
{
	sw_error_t rv;
	hsl_api_t *p_api;

	SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

	if (NULL == p_api->mac_secure_port_enable_set)
		return SW_NOT_SUPPORTED;

	rv = p_api->mac_secure_port_enable_set(dev_id, port_id, enable);
	return rv;
}

static inline sw_error_t _fal_mac_row_create(a_uint32_t dev_id,fal_port_t port_id,fal_fdb_mac_t * mac_addr)
{
	sw_error_t rv;
	hsl_api_t *p_api;

	SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

	if (NULL == p_api->mac_row_create)
		return SW_NOT_SUPPORTED;

	rv = p_api->mac_row_create(dev_id, port_id, mac_addr);
	return rv;
}

static inline sw_error_t _fal_mac_row_destroy(a_uint32_t dev_id,fal_fdb_mac_t * mac_addr, fal_port_t port_id)
{
	sw_error_t rv;
	hsl_api_t *p_api;

	SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

	if (NULL == p_api->mac_row_destroy)
		return SW_NOT_SUPPORTED;

	rv = p_api->mac_row_destroy(dev_id, mac_addr, port_id);
	return rv;
}

static inline sw_error_t _fal_mac_addr_table_show(a_uint32_t dev_id)
{
	sw_error_t rv;
	hsl_api_t *p_api;

	SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

	if (NULL == p_api->mac_addr_table_show)
		return SW_NOT_SUPPORTED;

	rv = p_api->mac_addr_table_show(dev_id);
	return rv;
}

static inline sw_error_t _fal_mac_addr_table_clear(a_uint32_t dev_id)
{
	sw_error_t rv;
	hsl_api_t *p_api;

	SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

	if (NULL == p_api->mac_addr_table_clear)
		return SW_NOT_SUPPORTED;

	rv = p_api->mac_addr_table_clear(dev_id);
	return rv;
}

static inline sw_error_t _fal_mac_show_blacklist(a_uint32_t dev_id)
{
	sw_error_t rv;
	hsl_api_t *p_api;

	SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

	if (NULL == p_api->mac_show_blacklist)
		return SW_NOT_SUPPORTED;

	rv = p_api->mac_show_blacklist(dev_id);
	return rv;
}

static inline sw_error_t _fal_mac_flush_port(a_uint32_t dev_id, fal_port_t port_id)
{
	sw_error_t rv;
	hsl_api_t *p_api;

	SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

	if (NULL == p_api->mac_flush_port)
		return SW_NOT_SUPPORTED;

	rv = p_api->mac_flush_port(dev_id, port_id);
	return rv;
}

static inline sw_error_t _fal_8021x_port_auth_set(a_uint32_t dev_id,fal_port_t port_id,fal_enable_t enable)
{
	sw_error_t rv;
	hsl_api_t *p_api;

	SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

	if (NULL == p_api->ieee8021x_port_auth_set)
		return SW_NOT_SUPPORTED;

	rv = p_api->ieee8021x_port_auth_set(dev_id, port_id, enable);
	return rv;
}

static inline sw_error_t _fal_8021x_mac_bind(a_uint32_t dev_id,fal_port_t port_id,fal_atu_status atu_status,fal_fdb_mac_t * macAddress)
{
	sw_error_t rv;
	hsl_api_t *p_api;

	SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

	if (NULL == p_api->ieee8021x_mac_bind)
		return SW_NOT_SUPPORTED;

	rv = p_api->ieee8021x_mac_bind(dev_id, port_id, atu_status, macAddress);
	return rv;
}




sw_error_t	fal_mac_aging_timeout_set(a_uint32_t dev_id,fal_mac_aging_t aging_time)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_mac_aging_timeout_set(dev_id, aging_time);
    FAL_API_UNLOCK;
    return rv;
}

sw_error_t	fal_mac_secure_port_enable_set(a_uint32_t dev_id,fal_port_t port_id,fal_enable_t enable)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_mac_secure_port_enable_set(dev_id, port_id, enable);
    FAL_API_UNLOCK;
    return rv;
}

sw_error_t	fal_mac_row_create(a_uint32_t dev_id,fal_port_t port_id,fal_fdb_mac_t * mac_addr)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_mac_row_create(dev_id, port_id, mac_addr);
    FAL_API_UNLOCK;
    return rv;
}

sw_error_t	fal_mac_row_destroy(a_uint32_t dev_id,fal_fdb_mac_t * mac_addr, fal_port_t port_id)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_mac_row_destroy(dev_id, mac_addr, port_id);
    FAL_API_UNLOCK;
    return rv;
}

sw_error_t	fal_mac_addr_table_show(a_uint32_t dev_id)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_mac_addr_table_show(dev_id);
    FAL_API_UNLOCK;
    return rv;
}

sw_error_t	fal_mac_addr_table_clear(a_uint32_t dev_id)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_mac_addr_table_clear(dev_id);
    FAL_API_UNLOCK;
    return rv;
}

sw_error_t	fal_mac_show_blacklist(a_uint32_t dev_id)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_mac_show_blacklist(dev_id);
    FAL_API_UNLOCK;
    return rv;
}

sw_error_t	fal_mac_flush_port(a_uint32_t dev_id, fal_port_t port_id)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_mac_flush_port(dev_id, port_id);
    FAL_API_UNLOCK;
    return rv;
}

sw_error_t	fal_8021x_port_auth_set(a_uint32_t dev_id,fal_port_t port_id,fal_enable_t enable)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_8021x_port_auth_set(dev_id, port_id, enable);
    FAL_API_UNLOCK;
    return rv;
}


sw_error_t	fal_8021x_mac_bind(a_uint32_t dev_id,fal_port_t port_id,fal_atu_status atu_status,fal_fdb_mac_t * macAddress)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_8021x_mac_bind(dev_id, port_id, atu_status, macAddress);
    FAL_API_UNLOCK;
    return rv;
}



