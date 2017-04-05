
#include "sw.h"
#include "hsl.h"
#include "hsl_dev.h"
#include "hsl_api.h"

/**
 * @brief Init fal layer.
 * @details Comments:
 *   This operation will init fal layer and hsl layer
 * @param[in] dev_id device id
 * @param[in] cfg configuration for initialization
 * @return SW_OK or error code
 */
sw_error_t fal_init(a_uint32_t dev_id, ssdk_init_cfg *cfg)
{
    sw_error_t rv;
    HSL_DEV_ID_CHECK(dev_id);

    rv = hsl_api_init(dev_id);
    SW_RTN_ON_ERROR(rv);    

    rv = hsl_dev_init(dev_id, cfg);
    SW_RTN_ON_ERROR(rv); 

    return rv;
}

static inline sw_error_t _fal_reset(a_uint32_t dev_id)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->dev_reset)
        return SW_NOT_SUPPORTED; 

    rv = p_api->dev_reset(dev_id);
    return rv;
}

static inline sw_error_t _fal_mode_tag(a_uint32_t dev_id)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->dev_mode_tag)
        return SW_NOT_SUPPORTED; 

    rv = p_api->dev_mode_tag(dev_id);
    return rv;
}

static inline sw_error_t _fal_ssdk_cfg(a_uint32_t dev_id, ssdk_cfg_t *ssdk_cfg)
{    
    sw_error_t rv;
    HSL_DEV_ID_CHECK(dev_id);

    rv = hsl_ssdk_cfg(dev_id, ssdk_cfg);

    return rv;
}

sw_error_t fal_cleanup(void)
{
    sw_error_t rv;

    rv = hsl_dev_cleanup();
    SW_RTN_ON_ERROR(rv);

    return SW_OK;
}

/**
 * @brief Reset fal layer.
 * @details Comments:
 *   This operation will reset fal layer and hsl layer
 * @param[in] dev_id device id
 * @return SW_OK or error code
 */
sw_error_t fal_reset(a_uint32_t dev_id)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_reset(dev_id);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief tag mode set in fal layer.
 * @details Comments:
 *   This operation will reset fal layer and hsl layer
 * @param[in] dev_id device id
 * @return SW_OK or error code
 */
 
sw_error_t fal_mode_tag(a_uint32_t dev_id)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_mode_tag(dev_id);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get SSDK config infomation.
 * @param[in] dev_id device id
 * @param[out] ssdk_cfg SSDK config infomation
 * @return SW_OK or error code
 */
sw_error_t fal_ssdk_cfg(a_uint32_t dev_id, ssdk_cfg_t *ssdk_cfg)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_ssdk_cfg(dev_id, ssdk_cfg);
    FAL_API_UNLOCK;
    return rv;
}


