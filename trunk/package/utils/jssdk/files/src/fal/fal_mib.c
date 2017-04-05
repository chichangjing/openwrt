
#include "sw.h"
#include "fal_mib.h"
#include "hsl_api.h"

static sw_error_t _fal_mib_info_get(a_uint32_t dev_id, fal_port_t port_id, fal_mib_info_t *mib_Info)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->mib_info_get)
        return SW_NOT_SUPPORTED;

    rv = p_api->mib_info_get(dev_id, port_id, mib_Info);
    return rv;
}

static sw_error_t _fal_mib_info_clear(a_uint32_t dev_id, fal_port_t port_id)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->mib_info_clear)
        return SW_NOT_SUPPORTED;

    rv = p_api->mib_info_clear(dev_id, port_id);
    return rv;
}

/**
 * @brief Get mib infomation on particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out] mib_info mib infomation
 * @return SW_OK or error code
 */
sw_error_t fal_mib_info_get(a_uint32_t dev_id, fal_port_t port_id, fal_mib_info_t *mib_Info)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_mib_info_get(dev_id, port_id, mib_Info);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Mib test show infomation on particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @return SW_OK or error code
 */
sw_error_t fal_mib_info_clear(a_uint32_t dev_id, fal_port_t port_id)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_mib_info_clear(dev_id, port_id);
    FAL_API_UNLOCK;
    return rv;
}


