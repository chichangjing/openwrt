#include "RmiRegs.h"

#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <linux/if_ether.h>


static u_int16_t datas[SMI_DEV_MAX_MV6097][SMI_REG_MAX_MV6097];
static u_int16_t devs[SMI_DEV_MAX_MV6097]={0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0xa,
        0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c};
static char * title [SMI_DEV_MAX_MV6097]={"PY00", "PY01", "PY02", "PY03", "PY04", "PY05", "PY06", "PY07", "PY08", "PY09","PY0A",
        "PT10", "PT11", "PT12", "PT13", "PT14", "PT15", "PT16", "PT17", "PT18", "PT19", "PT1A", "GL-1", "GL-2",};


u_int32_t regs_dump(u_int32_t dev){
    if(dev>0xff){
        return RMI_ERR_PARA_INVALID;
    }
    u_int32_t phy, reg;
    memset(datas, 0, sizeof(datas));
    for(phy=0;phy<SMI_DEV_MAX_MV6097;phy++){
        for(reg=0;reg<SMI_REG_MAX_MV6097;reg++){
            rmi_reg_get(dev, devs[phy], reg, &datas[phy][reg]);
        }
    }
    return 0;
}

void show_regs(void){
    u_int32_t phy, reg;
    printf("%4s ", "");
    for(phy=0;phy<SMI_DEV_MAX_MV6097;phy++){
        printf("%4s ", title[phy]);
    }
    printf("\n");
    for(reg=0;reg<SMI_REG_MAX_MV6097;reg++){
        printf("%04x ", reg);
            for(phy=0;phy<SMI_DEV_MAX_MV6097;phy++){
            printf("%04x ", (datas[phy][reg]));
        }
        printf("\n");
    }
}

u_int32_t rmi_reg_set(u_int32_t dev, u_int32_t phy_addr, u_int32_t reg, u_int16_t data){
    if(dev>0xff){
        return RMI_ERR_PARA_INVALID;
    }
    Rmi_req req = {
            .dev=(u_int8_t)(dev),
            .req_format=RES_REG_SET_FARMOT,
            .res=0x0000,
            .req_code=RES_REG_SET_CODE,
            .req_data=((1<<8)|(phy_addr<<5)|(reg)),
            .pad=data,
    };
    rmi_req_format(&req, (Rmi_req_bpu *)(rmi_buf));
    rmi_pkt_send(rmi_buf, sizeof(Rmi_req_bpu));
    rmi_pkt_rcv(rmi_buf, BUF_LEN);
    buf_unlock();
    return 0;
}

u_int32_t rmi_reg_get(u_int32_t dev, u_int32_t phy_addr, u_int32_t reg, u_int16_t * data){
    if(dev>0xff){
        return RMI_ERR_PARA_INVALID;
    }
    Rmi_req req = {
            .dev=(u_int8_t)(dev),
            .req_format=RES_REG_GET_FARMOT,
            .res=0x0000,
            .req_code=RES_REG_GET_CODE,
            .req_data=((2<<10)|(phy_addr<<5)|(reg)),
            .pad=RES_REG_GET_PADING,
    };
    rmi_req_format(&req, (Rmi_req_bpu *)(rmi_buf));
    rmi_pkt_send(rmi_buf, sizeof(Rmi_req_bpu));
    rmi_pkt_rcv(rmi_buf, BUF_LEN);
    Rmi_req_bpu * rmi_req_bpu = (Rmi_req_bpu*)(rmi_buf);
    *data=ntohs(rmi_req_bpu->pad);
    buf_unlock();
    return 0;
}
