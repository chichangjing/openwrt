

#include "sw.h"
#include "ssdk_init.h"
#include "sal_drv.h"
#include "sw_api.h"

mdio_reg_set ssdk_mdio_set    = NULL;
mdio_reg_get ssdk_mdio_get    = NULL;

sw_error_t sal_mdio_reg_set(a_uint32_t dev_id, a_uint32_t phy, a_uint32_t reg, a_uint16_t data)
{
    sw_error_t rv = SW_OK;

    if (NULL != ssdk_mdio_set) {
        rv = ssdk_mdio_set(dev_id, phy, reg, data);
    } else {
        return SW_NOT_SUPPORTED;
    }

    return rv;
}

sw_error_t sal_mdio_reg_get(a_uint32_t dev_id, a_uint32_t phy, a_uint32_t reg, a_uint16_t * data)
{
    sw_error_t rv = SW_OK;

    if (NULL != ssdk_mdio_get) {
        rv = ssdk_mdio_get(dev_id, phy, reg, data);
    } else {
        return SW_NOT_SUPPORTED;
    }

    return rv;
}

sw_error_t sal_drv_init(a_uint32_t dev_id, ssdk_init_cfg * cfg)
{
    if (NULL != cfg->reg_func.mdio_set) {
        ssdk_mdio_set = cfg->reg_func.mdio_set;
    }

    if (NULL != cfg->reg_func.mdio_get) {
        ssdk_mdio_get = cfg->reg_func.mdio_get;
    }

    return SW_OK;
}



