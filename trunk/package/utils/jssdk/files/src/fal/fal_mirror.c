
#include "sw.h"
#include "fal_mirror.h"
#include "hsl_api.h"
#include "sal_sys.h"

static inline sw_error_t _fal_mirror_dest_port_set(a_uint32_t dev_id, fal_port_t port_id)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->mirror_dest_port_set)
        return SW_NOT_SUPPORTED;

    rv = p_api->mirror_dest_port_set(dev_id, port_id);
    return rv;

}

static inline sw_error_t _fal_mirror_dest_port_get(a_uint32_t dev_id, fal_port_t *port_id)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->mirror_dest_port_get)
        return SW_NOT_SUPPORTED;

    rv = p_api->mirror_dest_port_get(dev_id, port_id);
    return rv;

}

static inline sw_error_t _fal_mirror_source_port_mode_set(a_uint32_t dev_id,fal_port_t source_port,fal_mirror_mode_t mode)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->mirror_source_port_mode_set)
        return SW_NOT_SUPPORTED;

    rv = p_api->mirror_source_port_mode_set(dev_id, source_port, mode);
    return rv;

}

static inline sw_error_t _fal_mirror_source_port_mode_get(a_uint32_t dev_id,fal_port_t source_port,fal_mirror_mode_t * mode)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->mirror_source_port_mode_get)
        return SW_NOT_SUPPORTED;

    rv = p_api->mirror_source_port_mode_get(dev_id, source_port, mode);
    return rv;

}




sw_error_t	fal_mirror_dest_port_set(a_uint32_t dev_id, fal_port_t port_id)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_mirror_dest_port_set(dev_id, port_id);
    FAL_API_UNLOCK;
    return rv;

}

sw_error_t	fal_mirror_dest_port_get(a_uint32_t dev_id, fal_port_t *port_id)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_mirror_dest_port_get(dev_id, port_id);
    FAL_API_UNLOCK;
    return rv;

}

sw_error_t	fal_mirror_source_port_mode_set(a_uint32_t dev_id,fal_port_t source_port,fal_mirror_mode_t mode)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_mirror_source_port_mode_set(dev_id, source_port, mode);
    FAL_API_UNLOCK;
    return rv;

}

sw_error_t	fal_mirror_source_port_mode_get(a_uint32_t dev_id,fal_port_t source_port,fal_mirror_mode_t * mode)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_mirror_source_port_mode_get(dev_id, source_port, mode);
    FAL_API_UNLOCK;
    return rv;

}


