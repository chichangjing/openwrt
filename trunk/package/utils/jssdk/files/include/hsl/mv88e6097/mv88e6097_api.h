
#ifndef _MV88E6097_API_H_
#define _MV88E6097_API_H_

#ifdef __cplusplus
extern "C" {
#endif

#ifdef IN_PORTCONTROL
#define PORTCONTROL_API \
    SW_API_DEF(SW_API_PT_DUPLEX_GET, mv88e6097_port_duplex_get), \
    SW_API_DEF(SW_API_PT_DUPLEX_SET, mv88e6097_port_duplex_set), \
    SW_API_DEF(SW_API_PT_SPEED_GET, mv88e6097_port_speed_get), \
    SW_API_DEF(SW_API_PT_SPEED_SET, mv88e6097_port_speed_set), \
    SW_API_DEF(SW_API_PT_AN_GET, mv88e6097_port_autoneg_status_get), \
    SW_API_DEF(SW_API_PT_AN_ENABLE, mv88e6097_port_autoneg_enable), \
    SW_API_DEF(SW_API_PT_AN_RESTART, mv88e6097_port_autoneg_restart),

#define PORTCONTROL_API_PARAM \
    SW_API_DESC(SW_API_PT_DUPLEX_GET)  \
    SW_API_DESC(SW_API_PT_DUPLEX_SET)  \
    SW_API_DESC(SW_API_PT_SPEED_GET)  \
    SW_API_DESC(SW_API_PT_SPEED_SET)  \
    SW_API_DESC(SW_API_PT_AN_GET)  \
    SW_API_DESC(SW_API_PT_AN_ENABLE)  \
    SW_API_DESC(SW_API_PT_AN_RESTART)  
#else
#define PORTCONTROL_API
#define PORTCONTROL_API_PARAM
#endif

#define REG_API \
    SW_API_DEF(SW_API_REG_GET, mv88e6097_reg_get), \
    SW_API_DEF(SW_API_REG_SET, mv88e6097_reg_set), \
    SW_API_DEF(SW_API_REG_FIELD_GET, mv88e6097_reg_field_get), \
    SW_API_DEF(SW_API_REG_FIELD_SET, mv88e6097_reg_field_set),

#define REG_API_PARAM \
    SW_API_DESC(SW_API_REG_GET)  \
    SW_API_DESC(SW_API_REG_SET)  \
    SW_API_DESC(SW_API_REG_FIELD_GET) \
    SW_API_DESC(SW_API_REG_FIELD_SET)

#define SSDK_API \
    SW_API_DEF(SW_API_SWITCH_RESET, mv88e6097_reset), \
    SW_API_DEF(SW_API_SSDK_CFG, hsl_ssdk_cfg), \
    PORTCONTROL_API \
    REG_API \
    SW_API_DEF(SW_API_MAX, NULL)

#define SSDK_PARAM  \
    SW_PARAM_DEF(SW_API_SWITCH_RESET, SW_UINT32, 4, SW_PARAM_IN, "Dev ID"), \
    SW_PARAM_DEF(SW_API_SSDK_CFG, SW_UINT32, 4, SW_PARAM_IN, "Dev ID"), \
    SW_PARAM_DEF(SW_API_SSDK_CFG, SW_SSDK_CFG, sizeof(ssdk_cfg_t), SW_PARAM_PTR|SW_PARAM_OUT, "ssdk configuration"), \
    PORTCONTROL_API_PARAM \
    REG_API_PARAM \
    SW_PARAM_DEF(SW_API_MAX, SW_UINT32, 4, SW_PARAM_IN, "Dev ID"),

#ifdef __cplusplus
}
#endif
#endif

