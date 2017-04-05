
#ifndef _FAL_PORTCTRL_H_
#define _FAL_PORTCTRL_H_

#ifdef __cplusplus
extern "c" {
#endif

#include "common/sw.h"
#include "fal/fal_type.h"


typedef enum {
    FAL_PORT_TYPE_CPU	 = 0, 
    FAL_PORT_TYPE_FE         = 1, 
    FAL_PORT_TYPE_FX         = 2, 
    FAL_PORT_TYPE_GE         = 3, 
    FAL_PORT_TYPE_GX         = 4,
    FAL_PORT_TYPE_COMB		 = 5,
    FAL_PORT_TYPE_END        = 6
} fal_port_type_t;

typedef enum {
    FAL_PORT_LINK_STATUS_UP      = 1,
    FAL_PORT_LINK_STATUS_DOWN    = 2,
    FAL_PORT_LINK_STATUS_END     = 3
} fal_port_link_status_t;

typedef enum {
    FAL_PORT_ADMIN_MODE_ENABLE   = 1,
    FAL_PORT_ADMIN_MODE_DISABLE  = 2, 		 
    FAL_PORT_ADMIN_MODE_END      = 3
} fal_port_admin_mode_t;

typedef enum {
	FAL_PORT_PHYSICAL_MODE_AUTONEG        = 1,
    FAL_PORT_PHYSICAL_MODE_S10_HALF       = 2,
    FAL_PORT_PHYSICAL_MODE_S10_FULL       = 3, 		 
    FAL_PORT_PHYSICAL_MODE_S100_HALF      = 4,
    FAL_PORT_PHYSICAL_MODE_S100_FULL      = 5,
    FAL_PORT_PHYSICAL_MODE_S1000_FULL     = 6,
    FAL_PORT_PHYSICAL_MODE_S1000_HALF	  = 7,
    FAL_PORT_PHYSICAL_MODE_COPPER_FIBER_AUTO = 8,
    FAL_PORT_PHYSICAL_MODE_FIBER_AUTO	  = 9,
    FAL_PORT_PHYSICAL_MODE_END            = 10
} fal_port_physical_mode_t;

typedef enum {
	FAL_PORT_PHYSICAL_STATUS_UNKOWN       = 1,
    FAL_PORT_PHYSICAL_STATUS_S10_HALF     = 2,
    FAL_PORT_PHYSICAL_STATUS_S10_FULL     = 3, 		 
    FAL_PORT_PHYSICAL_STATUS_S100_HALF    = 4,
    FAL_PORT_PHYSICAL_STATUS_S100_FULL    = 5,
    FAL_PORT_PHYSICAL_STATUS_S1000_FULL   = 6,
    FAL_PORT_PHYSICAL_STATUS_S1000_HALF   = 7,
    FAL_PORT_PHYSICAL_STATUS_END          = 8
} fal_port_physical_status_t;

typedef enum {
    FAL_PORT_SPEED_10        = 1,
	FAL_PORT_SPEED_100       = 2,
	FAL_PORT_SPEED_1000      = 3,
    FAL_PORT_SPEED_END       = 4
} fal_port_speed_t;

typedef enum {
	FAL_PORT_DUPLEX_HALF     = 1,
    FAL_PORT_DUPLEX_FULL     = 2,
    FAL_PORT_DUPLEX_END      = 3
} fal_port_duplex_t;

typedef enum {
    FAL_PORT_FLOWCTRL_MODE_ENABLE     = 1,
    FAL_PORT_FLOWCTRL_MODE_DISABLE    = 2,
    FAL_PORT_FLOWCTRL_MODE_END        = 3
} fal_port_flowctrl_mode_t;

typedef enum {
    FAL_PORT_DOT1D_STATE_DISABLED	= 1,
    FAL_PORT_DOT1D_STATE_BLOCKING	= 2,
    FAL_PORT_DOT1D_STATE_LEARNING	= 3,
    FAL_PORT_DOT1D_STATE_FORWARDING	= 4,
    FAL_PORT_DOT1D_STATE_END		= 5
} fal_port_dot1d_state_t;

typedef enum {
	FAL_PORT_NOT_COMB_PORT	= 0,
	FAL_PORT_COMB_PORT		= 1,
	FAL_PORT_COMB_END		= 2
}fal_port_comb_t;

typedef enum {
	FAL_PORT_COMB_FORCE_1000BASE_X	= 1,
	FAL_PORT_COMB_FORCE_100BASE_FX  = 2,
	FAL_PORT_COMB_FORCE_COPPER		= 3,
	FAL_PORT_COMB_AUTO_FIBER_COPPER = 4,
	FAL_PORT_COMB_FORCE_100M_COPPER = 5,
	FAL_PORT_COMB_FORCE_END			= 6
}fal_port_comb_type_t;

typedef struct {
	unsigned long	lport;     /* logic port */
	unsigned long	hport;	   /* hardware port */
	fal_port_type_t type;      /* port type */
} fal_port_map_t;

sw_error_t fal_port_type_get(a_uint32_t dev_id, fal_port_t port_id, fal_port_type_t *type_info, fal_port_comb_t *comb_port);
sw_error_t fal_port_link_status_get(a_uint32_t dev_id, fal_port_t port_id, fal_port_link_status_t *status);
sw_error_t fal_port_admin_mode_set(a_uint32_t dev_id, fal_port_t port_id, fal_enable_t enable);
sw_error_t fal_port_admin_mode_get(a_uint32_t dev_id, fal_port_t port_id, fal_enable_t *enable);
sw_error_t fal_port_physical_mode_set(a_uint32_t dev_id, fal_port_t port_id, fal_port_physical_mode_t mode);
sw_error_t fal_port_physical_mode_get(a_uint32_t dev_id, fal_port_t port_id, fal_port_physical_mode_t *mode);
sw_error_t fal_port_physical_status_get(a_uint32_t dev_id, fal_port_t port_id, fal_port_physical_status_t *status);
sw_error_t fal_port_speed_get(a_uint32_t dev_id, fal_port_t port_id, fal_port_speed_t *speed);
sw_error_t fal_port_duplex_get(a_uint32_t dev_id, fal_port_t port_id, fal_port_duplex_t *duplex);
sw_error_t fal_port_flowctrl_mode_set(a_uint32_t dev_id, fal_port_t port_id, fal_enable_t enable);
sw_error_t fal_port_flowctrl_mode_get(a_uint32_t dev_id, fal_port_t port_id, fal_enable_t *enable);
sw_error_t fal_port_dot1d_state_get(a_uint32_t dev_id, fal_port_t port_id, fal_port_dot1d_state_t *state);
sw_error_t fal_port_dot1d_state_set(a_uint32_t dev_id, fal_port_t port_id, fal_port_dot1d_state_t state);
sw_error_t fal_port_test_show_all(a_uint32_t dev_id);
sw_error_t fal_port_pcs_status_get(a_uint32_t dev_id, fal_port_t port_id, a_uint32_t *pcs_status);
sw_error_t fal_port_comb_type_set(a_uint32_t dev_id, fal_port_t port_id, fal_port_comb_type_t comb_type);
sw_error_t fal_port_comb_type_get(a_uint32_t dev_id, fal_port_t port_id, fal_port_comb_type_t *comb_type);

#ifdef __cplusplus
}
#endif
#endif

