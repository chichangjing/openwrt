#ifndef JWSUCITRAPCONF_H
#define JWSUCITRAPCONF_H

struct trap_cfg
{
    unsigned int port_id;
    unsigned int port_en;
    unsigned int tx_enable;
    unsigned int tx_percentage;
    unsigned int rx_enable;
    unsigned int rx_percentage;
};

/* function declarations */
int trapCfgLoad(void);
int trapCfgDisplay(void);

int getPowerSupplyCfg(enum PW_ALARM_STATUS *powEnable);
char * pow_status_print(enum PWSTATUS pwStatus);
int trap_get_cfg_entry(int port_id, struct trap_cfg *trap);
int trap_display_cfg_table(struct trap_cfg *trapCfg);

#endif /* JWSUCITRAPCONF_H */