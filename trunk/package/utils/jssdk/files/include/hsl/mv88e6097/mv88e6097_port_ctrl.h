
#ifndef _MV88E6097_PORT_CTRL_H
#define _MV88E6097_PORT_CTRL_H

#ifdef __cplusplus
extern "C" {
#endif

#include "fal/fal_port_ctrl.h"





sw_error_t map_lport2hport(a_uint32_t lport, a_uint32_t *hport);
sw_error_t map_hport2lport(a_uint32_t hport, a_uint32_t *lport);
a_uint8_t shift_high_low_bit(a_uint8_t data);

sw_error_t comb_port_fiber_link_status_detect(fal_port_t lport, unsigned int *media_status);
sw_error_t comb_port_led_mode_select(fal_port_t lport,unsigned int media_link);

sw_error_t mv88e6097_port_type_get(a_uint32_t dev_id, fal_port_t port_id, fal_port_type_t *type_info, fal_port_comb_t *comb_port);
sw_error_t mv88e6097_port_link_status_get(a_uint32_t dev_id, fal_port_t port_id, fal_port_link_status_t *status);
sw_error_t mv88e6097_port_admin_mode_set(a_uint32_t dev_id, fal_port_t port_id, fal_enable_t enable);
sw_error_t mv88e6097_port_admin_mode_get(a_uint32_t dev_id, fal_port_t port_id, fal_enable_t *enable);
sw_error_t mv88e6097_port_physical_mode_set(a_uint32_t dev_id, fal_port_t port_id, fal_port_physical_mode_t mode);
sw_error_t mv88e6097_port_physical_mode_get(a_uint32_t dev_id, fal_port_t port_id, fal_port_physical_mode_t *mode);
sw_error_t mv88e6097_port_physical_status_get(a_uint32_t dev_id, fal_port_t port_id, fal_port_physical_status_t *status);
sw_error_t mv88e6097_port_flowctrl_mode_set(a_uint32_t dev_id, fal_port_t port_id, fal_enable_t enable);
sw_error_t mv88e6097_port_flowctrl_mode_get(a_uint32_t dev_id, fal_port_t port_id, fal_enable_t *enable);
sw_error_t mv88e6097_port_test_show_all(a_uint32_t dev_id);
sw_error_t mv88e6097_port_ctrl_init(a_uint32_t dev_id);
sw_error_t mv88e6097_port_pcs_status_get(a_uint32_t dev_id,fal_port_t port_id,a_uint32_t *pcs_status);
sw_error_t mv88e6097_port_comb_type_set(a_uint32_t dev_id,fal_port_t port_id,fal_port_comb_type_t comb_type);
sw_error_t mv88e6097_port_comb_type_get(a_uint32_t dev_id,fal_port_t port_id,fal_port_comb_type_t *comb_type);

#ifdef __cplusplus
}
#endif
#endif

