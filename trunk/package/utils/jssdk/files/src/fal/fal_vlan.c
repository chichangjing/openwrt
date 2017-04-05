
#include "sw.h"
#include "fal_vlan.h"
#include "hsl_api.h"
#include "sal_sys.h"


static inline sw_error_t _fal_vlan_mode_set(a_uint32_t dev_id,fal_vlan_mode_t mode)
{
	sw_error_t rv;
	hsl_api_t *p_api;

	SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

	if (NULL == p_api->vlan_mode_set)
		return SW_NOT_SUPPORTED;

	rv = p_api->vlan_mode_set(dev_id, mode);
	return rv;
}

static inline sw_error_t _fal_vlan_mode_get(a_uint32_t dev_id,fal_vlan_mode_t *mode)
{
	sw_error_t rv;
	hsl_api_t *p_api;

	SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

	if (NULL == p_api->vlan_mode_get)
		return SW_NOT_SUPPORTED;

	rv = p_api->vlan_mode_get(dev_id, mode);
	return rv;
}

static inline sw_error_t _fal_port_based_vlan_set(a_uint32_t dev_id,fal_port_t port_id,fal_vlan_members_t *vlan_members, a_uint32_t member_size)
{
	sw_error_t rv;
	hsl_api_t *p_api;

	SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

	if (NULL == p_api->port_based_vlan_set)
		return SW_NOT_SUPPORTED;

	rv = p_api->port_based_vlan_set(dev_id, port_id, vlan_members, member_size);
	return rv;
}

static inline sw_error_t _fal_8021q_vlan_row_create(a_uint32_t dev_id,fal_vlan_id_t vlan_id,fal_vlan_members_t * port_members, a_uint32_t member_size,fal_vlan_egress_tag_t * egress_tag)
{
	sw_error_t rv;
	hsl_api_t *p_api;

	SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

	if (NULL == p_api->ieee8021q_vlan_row_create)
		return SW_NOT_SUPPORTED;

	rv = p_api->ieee8021q_vlan_row_create(dev_id, vlan_id, port_members, member_size, egress_tag);
	return rv;
}

static inline sw_error_t _fal_8021q_vlan_row_destroy(a_uint32_t dev_id,fal_vlan_id_t vlan_id)
{
	sw_error_t rv;
	hsl_api_t *p_api;

	SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

	if (NULL == p_api->ieee8021q_vlan_row_destroy)
		return SW_NOT_SUPPORTED;

	rv = p_api->ieee8021q_vlan_row_destroy(dev_id, vlan_id);
	return rv;
}

static inline sw_error_t _fal_8021q_vlan_show_status(a_uint32_t dev_id)
{
	sw_error_t rv;
	hsl_api_t *p_api;

	SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

	if (NULL == p_api->ieee8021q_vlan_show_status)
		return SW_NOT_SUPPORTED;

	rv = p_api->ieee8021q_vlan_show_status(dev_id);
	return rv;
}

static inline sw_error_t _fal_8021q_vlan_row_status(a_uint32_t dev_id,fal_vlan_id_t * vlan_id,fal_vlan_egress_tag_t * egress_tag)
{
	sw_error_t rv;
	hsl_api_t *p_api;

	SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

	if (NULL == p_api->ieee8021q_vlan_row_status)
		return SW_NOT_SUPPORTED;

	rv = p_api->ieee8021q_vlan_row_status(dev_id, vlan_id, egress_tag);
	return rv;
}






sw_error_t	fal_vlan_mode_set(a_uint32_t dev_id,fal_vlan_mode_t mode)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_vlan_mode_set(dev_id, mode);
    FAL_API_UNLOCK;
    return rv;
}

sw_error_t	fal_vlan_mode_get(a_uint32_t dev_id,fal_vlan_mode_t *mode)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_vlan_mode_get(dev_id, mode);
    FAL_API_UNLOCK;
    return rv;
}

sw_error_t	fal_port_based_vlan_set(a_uint32_t dev_id,fal_port_t port_id,fal_vlan_members_t *vlan_members, a_uint32_t member_size)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_port_based_vlan_set(dev_id, port_id, vlan_members, member_size);
    FAL_API_UNLOCK;
    return rv;
}

sw_error_t	fal_8021q_vlan_row_create(a_uint32_t dev_id,fal_vlan_id_t vlan_id,fal_vlan_members_t *port_members, a_uint32_t member_size,fal_vlan_egress_tag_t *egress_tag)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_8021q_vlan_row_create(dev_id, vlan_id, port_members, member_size, egress_tag);
    FAL_API_UNLOCK;
    return rv;
}

sw_error_t	fal_8021q_vlan_row_destroy(a_uint32_t dev_id,fal_vlan_id_t vlan_id)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_8021q_vlan_row_destroy(dev_id, vlan_id);
    FAL_API_UNLOCK;
    return rv;
}

sw_error_t	fal_8021q_vlan_show_status(a_uint32_t dev_id)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_8021q_vlan_show_status(dev_id);
    FAL_API_UNLOCK;
    return rv;
}

sw_error_t	fal_8021q_vlan_row_status(a_uint32_t dev_id,fal_vlan_id_t * vlan_id,fal_vlan_egress_tag_t * egress_tag)
{
	sw_error_t rv;

	FAL_API_LOCK;
	rv = _fal_8021q_vlan_row_status(dev_id, vlan_id, egress_tag);
	FAL_API_UNLOCK;
	return rv;

}




