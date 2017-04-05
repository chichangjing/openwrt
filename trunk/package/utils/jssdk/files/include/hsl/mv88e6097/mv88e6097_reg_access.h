
#ifndef _MV88E6097_REG_ACCESS_H_
#define _MV88E6097_REG_ACCESS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "common/sw.h"

sw_error_t mv88e6097_reg_get(a_uint32_t dev_id, a_uint32_t reg_addr, a_uint8_t value[], a_uint32_t value_len);
sw_error_t mv88e6097_reg_set(a_uint32_t dev_id, a_uint32_t reg_addr, a_uint8_t value[], a_uint32_t value_len);
sw_error_t mv88e6097_reg_field_get(a_uint32_t dev_id, a_uint32_t reg_addr, a_uint32_t bit_offset, a_uint32_t field_len, a_uint32_t value[], a_uint32_t value_len);
sw_error_t mv88e6097_reg_field_set(a_uint32_t dev_id, a_uint32_t reg_addr, a_uint32_t bit_offset, a_uint32_t field_len, const a_uint8_t value[], a_uint32_t value_len);
sw_error_t mv88e6097_reg_access_init(a_uint32_t dev_id, hsl_access_mode mode);
sw_error_t mv88e6097_access_mode_set(a_uint32_t dev_id, hsl_access_mode mode);
sw_error_t mv88e6097_phy_reg_get(a_uint32_t dev_id,a_uint32_t phy_id,a_uint32_t regAddr,a_uint32_t *data);
sw_error_t mv88e6097_phy_reg_set(a_uint32_t dev_id,a_uint32_t phy_id,a_uint32_t regAddr,a_uint32_t data);

#ifdef __cplusplus
}
#endif
#endif

