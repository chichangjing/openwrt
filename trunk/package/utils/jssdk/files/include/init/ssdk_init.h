
#ifndef _SSDK_INIT_H_
#define _SSDK_INIT_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "common/sw.h"

typedef sw_error_t (*mdio_reg_set) (a_uint32_t dev_id, a_uint32_t phy_addr, a_uint32_t reg, a_uint16_t data);
typedef sw_error_t (*mdio_reg_get) (a_uint32_t dev_id, a_uint32_t phy_addr, a_uint32_t reg, a_uint16_t *data);

typedef enum {
    HSL_MDIO = 1,
    HSL_SPI
} hsl_access_mode;

typedef struct {
    mdio_reg_set    mdio_set;
    mdio_reg_get    mdio_get;
} hsl_reg_func;

typedef struct {
    hsl_access_mode reg_mode;
    hsl_reg_func    reg_func;
	a_bool_t		en_init_chip;
} ssdk_init_cfg;

#if defined MARVELL_88E6097
#define def_init_cfg  {.reg_mode = HSL_MDIO};
#elif defined(MARVELL_3236)
#define def_init_cfg  {.reg_mode = HSL_MDIO};/*FIXME*/
#endif

typedef struct {
    a_bool_t in_portcontrol;
    a_bool_t in_mirror;	
    a_bool_t in_qos;
    a_bool_t in_rate;
    a_bool_t in_mib;	
    a_bool_t in_fdb;
    a_bool_t in_portvlan;
    a_bool_t in_vlan;
    a_bool_t in_misc;	
    a_bool_t in_stp;
    a_bool_t in_igmp;	
} ssdk_features;

#define CFG_STR_SIZE 20
typedef struct {
    a_uint8_t build_ver[CFG_STR_SIZE];
    a_uint8_t build_date[CFG_STR_SIZE];
    a_uint8_t chip_type[CFG_STR_SIZE];
    a_uint8_t cpu_type[CFG_STR_SIZE];
    a_uint8_t os_info[CFG_STR_SIZE];
    ssdk_features features;
    ssdk_init_cfg init_cfg;
} ssdk_cfg_t;

sw_error_t ssdk_init(a_uint32_t dev_id, ssdk_init_cfg *cfg);

#ifdef __cplusplus
}
#endif
#endif

