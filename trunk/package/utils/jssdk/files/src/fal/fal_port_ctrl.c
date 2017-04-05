
#include "sw.h"
#include "fal_port_ctrl.h"
#include "hsl_api.h"
#include "sal_sys.h"

static inline sw_error_t _fal_port_type_get(a_uint32_t dev_id, fal_port_t port_id, fal_port_type_t *type_info, fal_port_comb_t *comb_port)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->port_type_get)
        return SW_NOT_SUPPORTED;

    rv = p_api->port_type_get(dev_id, port_id, type_info,comb_port);
    return rv;
}

static inline sw_error_t _fal_port_link_status_get(a_uint32_t dev_id, fal_port_t port_id, fal_port_link_status_t *status)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->port_link_status_get)
        return SW_NOT_SUPPORTED;

    rv = p_api->port_link_status_get(dev_id, port_id, status);
    return rv;
}

static inline sw_error_t _fal_port_admin_mode_set(a_uint32_t dev_id, fal_port_t port_id, fal_enable_t enable)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->port_admin_mode_set)
        return SW_NOT_SUPPORTED;

    rv = p_api->port_admin_mode_set(dev_id, port_id, enable);
    return rv;
}

static inline sw_error_t _fal_port_admin_mode_get(a_uint32_t dev_id, fal_port_t port_id, fal_enable_t *enable)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->port_admin_mode_get)
        return SW_NOT_SUPPORTED;

    rv = p_api->port_admin_mode_get(dev_id, port_id, enable);
    return rv;
}

static inline sw_error_t _fal_port_physical_mode_set(a_uint32_t dev_id, fal_port_t port_id, fal_port_physical_mode_t mode)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->port_physical_mode_set)
        return SW_NOT_SUPPORTED;

    rv = p_api->port_physical_mode_set(dev_id, port_id, mode);
    return rv;
}

static inline sw_error_t _fal_port_physical_mode_get(a_uint32_t dev_id, fal_port_t port_id, fal_port_physical_mode_t *mode)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->port_physical_mode_get)
        return SW_NOT_SUPPORTED;

    rv = p_api->port_physical_mode_get(dev_id, port_id, mode);
    return rv;
}

static inline sw_error_t _fal_port_physical_status_get(a_uint32_t dev_id, fal_port_t port_id, fal_port_physical_status_t *status)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->port_physical_status_get)
        return SW_NOT_SUPPORTED;

    rv = p_api->port_physical_status_get(dev_id, port_id, status);
    return rv;
}

static inline sw_error_t _fal_port_speed_get(a_uint32_t dev_id, fal_port_t port_id, fal_port_speed_t *speed)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->port_speed_get)
        return SW_NOT_SUPPORTED;

    rv = p_api->port_speed_get(dev_id, port_id, speed);
    return rv;
}

static inline sw_error_t _fal_port_duplex_get(a_uint32_t dev_id, fal_port_t port_id, fal_port_duplex_t *duplex)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->port_duplex_get)
        return SW_NOT_SUPPORTED;

    rv = p_api->port_duplex_get(dev_id, port_id, duplex);
    return rv;
}

static inline sw_error_t _fal_port_flowctrl_mode_set(a_uint32_t dev_id, fal_port_t port_id, fal_enable_t enable)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->port_flowctrl_mode_set)
        return SW_NOT_SUPPORTED;

    rv = p_api->port_flowctrl_mode_set(dev_id, port_id, enable);
    return rv;
}

static inline sw_error_t _fal_port_flowctrl_mode_get(a_uint32_t dev_id, fal_port_t port_id, fal_enable_t *enable)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->port_flowctrl_mode_get)
        return SW_NOT_SUPPORTED;

    rv = p_api->port_flowctrl_mode_get(dev_id, port_id, enable);
    return rv;
}

static inline sw_error_t _fal_port_dot1d_state_set(a_uint32_t dev_id, fal_port_t port_id, fal_port_dot1d_state_t state)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->port_dot1d_state_set)
        return SW_NOT_SUPPORTED;

    rv = p_api->port_dot1d_state_set(dev_id, port_id, state);
    return rv;
}

static inline sw_error_t _fal_port_dot1d_state_get(a_uint32_t dev_id, fal_port_t port_id, fal_port_dot1d_state_t *state)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->port_dot1d_state_get)
        return SW_NOT_SUPPORTED;

    rv = p_api->port_dot1d_state_get(dev_id, port_id, state);
    return rv;
}

static inline sw_error_t _fal_port_test_show_all(a_uint32_t dev_id)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->port_test_show_all)
        return SW_NOT_SUPPORTED;

    rv = p_api->port_test_show_all(dev_id);
    return rv;
}

static inline sw_error_t _fal_port_pcs_status_get(a_uint32_t dev_id,fal_port_t port_id,a_uint32_t *pcs_status)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->port_pcs_status_get)
        return SW_NOT_SUPPORTED;

    rv = p_api->port_pcs_status_get(dev_id, port_id, pcs_status);
    return rv;	
}

static inline sw_error_t _fal_port_comb_type_set(a_uint32_t dev_id,fal_port_t port_id,fal_port_comb_type_t comb_type)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->port_comb_type_set)
        return SW_NOT_SUPPORTED;

    rv = p_api->port_comb_type_set(dev_id, port_id, comb_type);
    return rv;	
}

static inline sw_error_t _fal_port_comb_type_get(a_uint32_t dev_id,fal_port_t port_id,fal_port_comb_type_t *comb_type)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->port_comb_type_get)
        return SW_NOT_SUPPORTED;

    rv = p_api->port_comb_type_get(dev_id, port_id, comb_type);
    return rv;	
}



/**
 * @brief      Set port type on a particular port.
 * @param[in]  dev_id    - device id
 * @param[in]  port_id   - port id
 * @param[out] type      - port type
 * @return     SW_OK or error code
 */
sw_error_t fal_port_type_get(a_uint32_t dev_id, fal_port_t port_id, fal_port_type_t *type_info, fal_port_comb_t *comb_port)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_port_type_get(dev_id, port_id, type_info, comb_port);
    FAL_API_UNLOCK;
    return rv;
}

sw_error_t fal_port_link_status_get(a_uint32_t dev_id, fal_port_t port_id, fal_port_link_status_t *status)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_port_link_status_get(dev_id, port_id, status);
    FAL_API_UNLOCK;
    return rv;
}

sw_error_t fal_port_admin_mode_set(a_uint32_t dev_id, fal_port_t port_id, fal_enable_t enable)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_port_admin_mode_set(dev_id, port_id, enable);
    FAL_API_UNLOCK;
    return rv;
}

sw_error_t fal_port_admin_mode_get(a_uint32_t dev_id, fal_port_t port_id, fal_enable_t *enable)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_port_admin_mode_get(dev_id, port_id, enable);
    FAL_API_UNLOCK;
    return rv;
}

sw_error_t fal_port_physical_mode_set(a_uint32_t dev_id, fal_port_t port_id, fal_port_physical_mode_t mode)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_port_physical_mode_set(dev_id, port_id, mode);
    FAL_API_UNLOCK;
    return rv;
}

sw_error_t fal_port_physical_mode_get(a_uint32_t dev_id, fal_port_t port_id, fal_port_physical_mode_t *mode)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_port_physical_mode_get(dev_id, port_id, mode);
    FAL_API_UNLOCK;
    return rv;
}

sw_error_t fal_port_physical_status_get(a_uint32_t dev_id, fal_port_t port_id, fal_port_physical_status_t *status)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_port_physical_status_get(dev_id, port_id, status);
    FAL_API_UNLOCK;
    return rv;
}

sw_error_t fal_port_speed_get(a_uint32_t dev_id, fal_port_t port_id, fal_port_speed_t *speed)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_port_speed_get(dev_id, port_id, speed);
    FAL_API_UNLOCK;
    return rv;
}

sw_error_t fal_port_duplex_get(a_uint32_t dev_id, fal_port_t port_id, fal_port_duplex_t *duplex)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_port_duplex_get(dev_id, port_id, duplex);
    FAL_API_UNLOCK;
    return rv;
}

sw_error_t fal_port_flowctrl_mode_set(a_uint32_t dev_id, fal_port_t port_id, fal_enable_t enable)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_port_flowctrl_mode_set(dev_id, port_id, enable);
    FAL_API_UNLOCK;
    return rv;
}

sw_error_t fal_port_flowctrl_mode_get(a_uint32_t dev_id, fal_port_t port_id, fal_enable_t *enable)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_port_flowctrl_mode_get(dev_id, port_id, enable);
    FAL_API_UNLOCK;
    return rv;
}

sw_error_t fal_port_dot1d_state_set(a_uint32_t dev_id, fal_port_t port_id, fal_port_dot1d_state_t state)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_port_dot1d_state_set(dev_id, port_id, state);
    FAL_API_UNLOCK;
    return rv;
}

sw_error_t fal_port_dot1d_state_get(a_uint32_t dev_id, fal_port_t port_id, fal_port_dot1d_state_t *state)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_port_dot1d_state_get(dev_id, port_id, state);
    FAL_API_UNLOCK;
    return rv;
}

sw_error_t fal_port_test_show_all(a_uint32_t dev_id)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_port_test_show_all(dev_id);
    FAL_API_UNLOCK;
    return rv;
}

sw_error_t fal_port_pcs_status_get(a_uint32_t dev_id,fal_port_t port_id,a_uint32_t *pcs_status)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_port_pcs_status_get(dev_id, port_id, pcs_status);
    FAL_API_UNLOCK;
    return rv;	
}

sw_error_t fal_port_comb_type_set(a_uint32_t dev_id,fal_port_t port_id,fal_port_comb_type_t comb_type)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_port_comb_type_set(dev_id, port_id, comb_type);
    FAL_API_UNLOCK;
    return rv;	
}

sw_error_t fal_port_comb_type_get(a_uint32_t dev_id,fal_port_t port_id,fal_port_comb_type_t *comb_type)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_port_comb_type_get(dev_id, port_id, comb_type);
    FAL_API_UNLOCK;
    return rv;	
}


