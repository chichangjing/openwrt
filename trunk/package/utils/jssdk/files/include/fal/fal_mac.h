
#ifndef _FAL_MAC_H
#define _FAL_MAC_H

#ifdef __cplusplus
extern "C" {
#endif

#include "common/sw.h"
#include "fal/fal_type.h"
#include "common/list.h"



typedef struct {
	a_uint8_t mac[6];
} fal_fdb_mac_t;

typedef a_uint32_t fal_mac_aging_t;

typedef enum {
	FAL_MAC_WHITELIST	= 1,
    FAL_MAC_BLACKLIST	= 2,
    FAL_MAC_LIST_END 	= 3
} fal_fdb_black_white_t;


typedef struct{
	struct list_head node;
	fal_fdb_mac_t ptr;
	fal_port_t port_id;
	a_uint32_t portVec;
	//fal_fdb_black_white_t permission;
}FdbCommit_S;

typedef struct{
	struct list_head node;
	fal_fdb_mac_t ptr;
	a_uint32_t PortVec;
}FdbPortVec_S;

/* 802.1x enmu */
typedef enum {
	FAL_MAC_DYNAMIC		= 0x1,
	FAL_MAC_STATIC		= 0xF
}fal_atu_status;

sw_error_t fal_mac_aging_timeout_set(a_uint32_t dev_id, fal_mac_aging_t aging_time);
sw_error_t fal_mac_secure_port_enable_set(a_uint32_t dev_id, fal_port_t port_id, fal_enable_t enable);
sw_error_t fal_mac_row_create(a_uint32_t dev_id, fal_port_t port_id, fal_fdb_mac_t *mac_addr);
sw_error_t fal_mac_row_destroy(a_uint32_t dev_id, fal_fdb_mac_t *mac_addr, fal_port_t port_id);
sw_error_t fal_mac_addr_table_show(a_uint32_t dev_id);
sw_error_t fal_mac_addr_table_clear(a_uint32_t dev_id);
sw_error_t fal_mac_show_blacklist(a_uint32_t dev_id);
sw_error_t fal_mac_flush_port(a_uint32_t dev_id, fal_port_t port_id);
/* use for 802.1x feature */
sw_error_t fal_8021x_port_auth_set(a_uint32_t dev_id, fal_port_t port_id, fal_enable_t enable);
sw_error_t fal_8021x_mac_bind(a_uint32_t dev_id, fal_port_t port_id, fal_atu_status atu_status, fal_fdb_mac_t *macAddress);

#ifdef __cplusplus
}
#endif

#endif

