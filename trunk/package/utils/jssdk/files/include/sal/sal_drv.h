
#ifndef _SAL_DRV_H_
#define _SAL_DRV_H_

#ifdef __cplusplus
extern "C" {
#endif

sw_error_t sal_mdio_reg_set(a_uint32_t dev_id, a_uint32_t phy, a_uint32_t reg, a_uint16_t data);
sw_error_t sal_mdio_reg_get(a_uint32_t dev_id, a_uint32_t phy, a_uint32_t reg, a_uint16_t * data);
sw_error_t sal_drv_init(a_uint32_t dev_id, ssdk_init_cfg * cfg);

#ifdef __cplusplus
}
#endif
#endif


