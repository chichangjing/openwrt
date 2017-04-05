#ifndef __RMI_REG_H__
#define __RMI_REG_H__

#include "Rmu.h"

typedef struct _rmi_reg_get{
    Rmi_mac da;
    Rmi_mac sa;
    unsigned char tag[4];
    unsigned char eth_type[2];

    u_int16_t req_format;
    u_int16_t res;
    u_int16_t req_code;
    u_int16_t req_data;
    u_int16_t pad;
}Reg_GET;

u_int32_t regs_dump(u_int32_t dev);
void show_regs(void);

u_int32_t rmi_reg_set(u_int32_t dev, u_int32_t phy_addr, u_int32_t reg, u_int16_t data);
u_int32_t rmi_reg_get(u_int32_t dev, u_int32_t phy_addr, u_int32_t reg, u_int16_t * data);

#endif
