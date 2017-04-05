
#ifndef _MV88E6097_MAC_H
#define _MV88E6097_MAC_H

#ifdef __cplusplus
extern "C" {
#endif

#include "fal/fal_mac.h"
#include "msApi.h"




sw_error_t fdb_atu_int_disable();
sw_error_t fdb_atu_int_enable();
sw_error_t fdb_atu_clear_violation(GT_QD_DEV * pJwDev,GT_ATU_OPERATION atuOp,GT_U8 * port,GT_U32 * intCause);
sw_error_t fdb_atu_get_violation(GT_QD_DEV * pJwDev,GT_ATU_OPERATION atuOp,GT_ATU_ENTRY * entry);
sw_error_t fdb_atu_add_mac(unsigned char hport,GT_ATU_UC_STATE ucState,char * MacAddr);
sw_error_t fdb_atu_delete_mac(a_uint8_t * Mac);
sw_error_t fdb_mac_row_create(fal_port_t port_id,a_uint8_t * mac_addr);


sw_error_t mv88e6097_mac_init(a_uint32_t dev_id);
sw_error_t mv88e6097_mac_aging_timeout_set(a_uint32_t dev_id,fal_mac_aging_t aging_time);
sw_error_t mv88e6097_mac_secure_port_enable_set(a_uint32_t dev_id,fal_port_t port_id,fal_enable_t enable);
sw_error_t mv88e6097_mac_row_create(a_uint32_t dev_id,fal_port_t port_id,fal_fdb_mac_t * mac_addr);
sw_error_t mv88e6097_mac_row_destroy(a_uint32_t dev_id,fal_fdb_mac_t * mac_addr, fal_port_t port_id);
sw_error_t mv88e6097_mac_addr_table_show(a_uint32_t dev_id);
sw_error_t mv88e6097_mac_addr_table_clear(a_uint32_t dev_id);
sw_error_t mv88e6097_mac_show_blacklist(a_uint32_t dev_id);
sw_error_t mv88e6097_mac_flush_port(a_uint32_t dev_id, fal_port_t port_id);
/* 802.1x authentication feature */
sw_error_t mv88e6097_8021x_port_auth_set(a_uint32_t dev_id,fal_port_t port_id,fal_enable_t enable);
sw_error_t mv88e6097_8021x_mac_bind(a_uint32_t dev_id,fal_port_t port_id,fal_atu_status atu_status,fal_fdb_mac_t * macAddress);


#ifdef __cplusplus
}
#endif
#endif

