
#include "sw.h"
#include "fal_qos.h"
#include "hsl_api.h"
#include "sal_sys.h"


static inline sw_error_t _fal_qos_port_prio_map_rule_set(a_uint32_t dev_id,fal_port_t port_id,fal_qos_map_t qos_map)
{
	sw_error_t rv;
	hsl_api_t *p_api;

	SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

	if (NULL == p_api->qos_port_prio_map_rule_set)
		return SW_NOT_SUPPORTED;

	rv = p_api->qos_port_prio_map_rule_set(dev_id, port_id, qos_map);
	return rv;
}

static inline sw_error_t _fal_qos_port_prio_map_rule_get(a_uint32_t dev_id,fal_port_t port_id,fal_qos_map_t * qos_map)
{
	sw_error_t rv;
	hsl_api_t *p_api;

	SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

	if (NULL == p_api->qos_port_prio_map_rule_get)
		return SW_NOT_SUPPORTED;

	rv = p_api->qos_port_prio_map_rule_get(dev_id, port_id, qos_map);
	return rv;
}

static inline sw_error_t _fal_qos_port_schedule_mode_set(a_uint32_t dev_id,fal_port_t port_id,fal_qos_schedule_mode_t schedule_mode)
{
	sw_error_t rv;
	hsl_api_t *p_api;

	SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

	if (NULL == p_api->qos_port_schedule_mode_set)
		return SW_NOT_SUPPORTED;

	rv = p_api->qos_port_schedule_mode_set(dev_id, port_id, schedule_mode);
	return rv;
}

static inline sw_error_t _fal_qos_port_schedule_mode_get(a_uint32_t dev_id,fal_port_t port_id,fal_qos_schedule_mode_t * schedule_mode)
{
	sw_error_t rv;
	hsl_api_t *p_api;

	SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

	if (NULL == p_api->qos_port_schedule_mode_get)
		return SW_NOT_SUPPORTED;

	rv = p_api->qos_port_schedule_mode_get(dev_id, port_id, schedule_mode);
	return rv;
}

static inline sw_error_t _fal_qos_tag_prio_map_set(a_uint32_t dev_id,fal_tag_map_t *tag_mapping)
{
	sw_error_t rv;
	hsl_api_t *p_api;

	SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

	if (NULL == p_api->qos_tag_prio_map_set)
		return SW_NOT_SUPPORTED;

	rv = p_api->qos_tag_prio_map_set(dev_id, tag_mapping);
	return rv;
}

static inline sw_error_t _fal_qos_ip_prio_map_set(a_uint32_t dev_id,fal_ip_map_t *ip_mapping)
{
	sw_error_t rv;
	hsl_api_t *p_api;

	SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

	if (NULL == p_api->qos_ip_prio_map_set)
		return SW_NOT_SUPPORTED;

	rv = p_api->qos_ip_prio_map_set(dev_id, ip_mapping);
	return rv;
}

static inline sw_error_t _fal_qos_port_prio_assign_mode_set(a_uint32_t dev_id,fal_port_t port_id,fal_qos_assign_mode_t assign_mode)
{
	sw_error_t rv;
	hsl_api_t *p_api;

	SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

	if (NULL == p_api->qos_port_prio_assign_mode_set)
		return SW_NOT_SUPPORTED;

	rv = p_api->qos_port_prio_assign_mode_set(dev_id, port_id, assign_mode);
	return rv;
}

static inline sw_error_t _fal_qos_port_default_prio_set(a_uint32_t dev_id,fal_port_t port_id,fal_qos_prio_t priority)
{
	sw_error_t rv;
	hsl_api_t *p_api;

	SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

	if (NULL == p_api->qos_port_default_prio_set)
		return SW_NOT_SUPPORTED;

	rv = p_api->qos_port_default_prio_set(dev_id, port_id, priority);
	return rv;
}

static inline sw_error_t _fal_qos_show_status(a_uint32_t dev_id)
{
	sw_error_t rv;
	hsl_api_t *p_api;

	SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

	if (NULL == p_api->qos_show_status)
		return SW_NOT_SUPPORTED;

	rv = p_api->qos_show_status(dev_id);
	return rv;
}





sw_error_t	fal_qos_port_prio_map_rule_set(a_uint32_t dev_id,fal_port_t port_id,fal_qos_map_t qos_map)
{
	sw_error_t rv;

	FAL_API_LOCK;
	rv = _fal_qos_port_prio_map_rule_set(dev_id, port_id, qos_map);
	FAL_API_UNLOCK;
	return rv;

}

sw_error_t	fal_qos_port_prio_map_rule_get(a_uint32_t dev_id,fal_port_t port_id,fal_qos_map_t * qos_map)
{
	sw_error_t rv;

	FAL_API_LOCK;
	rv = _fal_qos_port_prio_map_rule_get(dev_id, port_id, qos_map);
	FAL_API_UNLOCK;
	return rv;

}

sw_error_t	fal_qos_port_schedule_mode_set(a_uint32_t dev_id,fal_port_t port_id,fal_qos_schedule_mode_t schedule_mode)
{
	sw_error_t rv;

	FAL_API_LOCK;
	rv = _fal_qos_port_schedule_mode_set(dev_id, port_id, schedule_mode);
	FAL_API_UNLOCK;
	return rv;

}

sw_error_t	fal_qos_port_schedule_mode_get(a_uint32_t dev_id,fal_port_t port_id,fal_qos_schedule_mode_t * schedule_mode)
{
	sw_error_t rv;

	FAL_API_LOCK;
	rv = _fal_qos_port_schedule_mode_get(dev_id, port_id, schedule_mode);
	FAL_API_UNLOCK;
	return rv;

}

sw_error_t	fal_qos_tag_prio_map_set(a_uint32_t dev_id,fal_tag_map_t *tag_mapping)
{
	sw_error_t rv;

	FAL_API_LOCK;
	rv = _fal_qos_tag_prio_map_set(dev_id, tag_mapping);
	FAL_API_UNLOCK;
	return rv;

}

sw_error_t	fal_qos_ip_prio_map_set(a_uint32_t dev_id,fal_ip_map_t *ip_mapping)
{
	sw_error_t rv;

	FAL_API_LOCK;
	rv = _fal_qos_ip_prio_map_set(dev_id, ip_mapping);
	FAL_API_UNLOCK;
	return rv;

}

sw_error_t	fal_qos_port_prio_assign_mode_set(a_uint32_t dev_id,fal_port_t port_id,fal_qos_assign_mode_t assign_mode)
{
	sw_error_t rv;

	FAL_API_LOCK;
	rv = _fal_qos_port_prio_assign_mode_set(dev_id, port_id, assign_mode);
	FAL_API_UNLOCK;
	return rv;

}

sw_error_t	fal_qos_port_default_prio_set(a_uint32_t dev_id,fal_port_t port_id,fal_qos_prio_t priority)
{
	sw_error_t rv;

	FAL_API_LOCK;
	rv = _fal_qos_port_default_prio_set(dev_id, port_id, priority);
	FAL_API_UNLOCK;
	return rv;

}

sw_error_t	fal_qos_show_status(a_uint32_t dev_id)
{
	sw_error_t rv;

	FAL_API_LOCK;
	rv = _fal_qos_show_status(dev_id);
	FAL_API_UNLOCK;
	return rv;

}





