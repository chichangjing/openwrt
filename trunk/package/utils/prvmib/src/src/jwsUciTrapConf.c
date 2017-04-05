#include "jwsDevBaseInfo.h"
#include "jwsUciTrapConf.h"
#include "jwslog.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <uci.h>

#define PORT_NUM (PORT_MAX_NUM)

enum {
    FAL_ENABLE = 1,
    FAL_DISABLE = 2,
};

typedef struct{
    char *name;
    enum uci_option_type type;
    int (*ubusGet)(struct uci_ptr *);
}UciConfig_S;

static struct{
    int power;
    struct{
        int st;
        int tx_sh_st;
        int tx_sh_per;
        int rx_sh_st;
        int rx_sh_per;
    }port[PORT_NUM];
}trap;

/* *
 * * @brief :       load corresponding arguement's value from UCI file
 * * @param :       *name   - option name
 * * @param :       *type   - string or list
 * * @param :       *ptr    - UCI pointer
 * * @retval :      0       - OK
 * * @retval :      -1      - ERROR
 * */
int load_config(char *name,enum uci_option_type type,struct uci_ptr *ptr,struct uci_context *ctx)
{
    if(name == NULL)
    {
        LogWrite(ERROR, "invalid arguement!\n"); 
        return -1;    
    }
    
    char *name_bak = strdup(name);

    if(UCI_OK != uci_lookup_ptr(ctx,ptr,name_bak,true))
    {
        LogWrite(ERROR, "no matching arguement: %s!\n",name); 
        free(name_bak);
        return -1;
    }
    
    free(name_bak);

    if(ptr->o->type != type)
    {
        LogWrite(ERROR, "UCI option type is not match!\n"); 
        return -1; 
    }

    return 0;
}

/* *
 * * @brief             load trap arguement - power supply trap from UCI file
 * * @param[*p]         buffer to be parse 
 * * @retval 0:         OK
 * * @retval -1:        ERROR
 * */
static int trapLoadPowerTrap(struct uci_ptr *ptr)
{
    int power = atoi(ptr->o->v.string);

    if(power != FAL_DISABLE && power != FAL_ENABLE)
    {
        LogWrite(ERROR, "invalid arguement!\n"); 
        return -1;
    }

    trap.power = power;
    LogWrite(DEBUG, "trap load power trap mode = [%d] OK\n", power);
    
    return 0;
}

/* *
 * * @brief             load trap arguement - port status trap admin mode from UCI file
 * * @param[*p]         buffer to be parse 
 * * @retval 0:         OK
 * * @retval -1:        ERROR
 * */
static int trapLoadPortTrap(struct uci_ptr *ptr)
{
    char *string = strdup(ptr->o->v.string);
    char *pstr;
    int port_trap;
    int port;

    while((pstr = strsep(&string," ")) != NULL)
    {
        if(strncmp(pstr,"port",strlen("port")))
        {
            LogWrite(ERROR, "port trap format in UCI file error!\n"); 
            return -1;
        }

        port = atoi(pstr + 4);
        if(port < 1 || port > PORT_NUM)
        {
            LogWrite(ERROR, "port trap arguement - port index in UCI file error!\n"); 
            return -1;
        }

        char *pdelim = strchr(pstr,':');
        if(pdelim == NULL)
        {
            LogWrite(ERROR, "port trap format in UCI file error!\n"); 
            return -1;
        }

        port_trap = atoi(pdelim + 1);
        if(port_trap != FAL_DISABLE && port_trap != FAL_ENABLE)
        {
            LogWrite(ERROR, "invalid arguement!\n");
            return -1;
        }

        trap.port[port-1].st = port_trap;
    }
    free(string);

    LogWrite(DEBUG, "trap load port trap OK\n");
    
    return 0;
}

/* *
 * * @brief             load trap arguement - port tx shreshold enable from UCI file
 * * @param[*p]         buffer to be parse 
 * * @retval 0:         OK
 * * @retval -1:        ERROR
 * */
static int trapLoadPortTxShEnable(struct uci_ptr *ptr)
{
    char *string = strdup(ptr->o->v.string);
    char *pstr;
    int sh_enable;
    int port;

    while((pstr = strsep(&string," ")) != NULL)
    {
        if(strncmp(pstr,"port",strlen("port")))
        {
            LogWrite(ERROR, "port tx shreshold enalbe format in UCI file error!\n"); 
            return -1;
        }

        port = atoi(pstr + 4);
        if(port < 1 || port > PORT_NUM)
        {
            LogWrite(ERROR, "port tx shreshold enalbe arguement - port index in UCI file error!\n"); 
            return -1;
        }

        char *pdelim = strchr(pstr,':');
        if(pdelim == NULL)
        {
            LogWrite(ERROR, "port tx shreshold enalbe format in UCI file error!\n"); 
            return -1;
        }

        sh_enable = atoi(pdelim + 1);
        if(sh_enable != FAL_DISABLE && sh_enable != FAL_ENABLE)
        {
            LogWrite(ERROR, "invalid arguement!\n");
            return -1;
        }

        trap.port[port-1].tx_sh_st = sh_enable;
    }
    free(string);

    LogWrite(DEBUG, "trap load port tx shreshold enable OK\n");
    
    return 0;
}

/* *
 * * @brief             load trap arguement - port tx shreshold percentage from UCI file
 * * @param[*p]         buffer to be parse 
 * * @retval 0:         OK
 * * @retval -1:        ERROR
 * */
static int trapLoadPortTxShPer(struct uci_ptr *ptr)
{
    char *string = strdup(ptr->o->v.string);
    char *pstr;
    int perc;
    int port;

    while((pstr = strsep(&string," ")) != NULL)
    {
        if(strncmp(pstr,"port",strlen("port")))
        {
            LogWrite(ERROR, "port tx shreshold percentage format in UCI file error!\n"); 
            return -1;
        }

        port = atoi(pstr + 4);
        if(port < 1 || port > PORT_NUM)
        {
            LogWrite(ERROR, "port tx shreshold percentage arguement - port index in UCI file error!\n"); 
            return -1;
        }

        char *pdelim = strchr(pstr,':');
        if(pdelim == NULL)
        {
            LogWrite(ERROR, "port tx shreshold percentage format in UCI file error!\n"); 
            return -1;
        }

        perc = atoi(pdelim + 1);
        if(perc < 1 || perc > 99)
        {
            LogWrite(ERROR, "invalid arguement!\n");
            return -1;
        }

        trap.port[port-1].tx_sh_per = perc;
    }
    free(string);

    LogWrite(DEBUG, "trap load port tx shreshold percentage OK\n");
    
    return 0;
}

/* *
 * * @brief             load trap arguement - port rx shreshold enable from UCI file
 * * @param[*p]         buffer to be parse 
 * * @retval 0:         OK
 * * @retval -1:        ERROR
 * */
static int trapLoadPortRxShEnable(struct uci_ptr *ptr)
{
    char *string = strdup(ptr->o->v.string);
    char *pstr;
    int sh_enable;
    int port;

    while((pstr = strsep(&string," ")) != NULL)
    {
        if(strncmp(pstr,"port",strlen("port")))
        {
            LogWrite(ERROR, "port rx shreshold enalbe format in UCI file error!\n"); 
            return -1;
        }

        port = atoi(pstr + 4);
        if(port < 1 || port > PORT_NUM)
        {
            LogWrite(ERROR, "port rx shreshold enalbe arguement - port index in UCI file error!\n"); 
            return -1;
        }

        char *pdelim = strchr(pstr,':');
        if(pdelim == NULL)
        {
            LogWrite(ERROR, "port rx shreshold enalbe format in UCI file error!\n"); 
            return -1;
        }

        sh_enable = atoi(pdelim + 1);
        if(sh_enable != FAL_DISABLE && sh_enable != FAL_ENABLE)
        {
            LogWrite(ERROR, "invalid arguement!\n");
            return -1;
        }

        trap.port[port-1].rx_sh_st = sh_enable;
    }
    free(string);

    LogWrite(DEBUG, "trap load port rx shreshold enable OK\n");
    
    return 0;
}

/* *
 * * @brief             load trap arguement - port rx shreshold percentage from UCI file
 * * @param[*p]         buffer to be parse 
 * * @retval 0:         OK
 * * @retval -1:        ERROR
 * */
static int trapLoadPortRxShPer(struct uci_ptr *ptr)
{
    char *string = strdup(ptr->o->v.string);
    char *pstr;
    int perc;
    int port;

    while((pstr = strsep(&string," ")) != NULL)
    {
        if(strncmp(pstr,"port",strlen("port")))
        {
            LogWrite(ERROR, "port rx shreshold percentage format in UCI file error!\n"); 
            return -1;
        }

        port = atoi(pstr + 4);
        if(port < 1 || port > PORT_NUM)
        {
            LogWrite(ERROR, "port rx shreshold percentage arguement - port index in UCI file error!\n"); 
            return -1;
        }

        char *pdelim = strchr(pstr,':');
        if(pdelim == NULL)
        {
            LogWrite(ERROR, "port rx shreshold percentage format in UCI file error!\n"); 
            return -1;
        }

        perc = atoi(pdelim + 1);
        if(perc < 1 || perc > 99)
        {
            LogWrite(ERROR, "invalid arguement!\n");
            return -1;
        }

        trap.port[port-1].rx_sh_per = perc;
    }
    free(string);

    LogWrite(DEBUG, "trap load port rx shreshold percentage OK\n");
    
    return 0;
}

static UciConfig_S trapUci[] = {
    {.name = "power_trap",              .type = UCI_TYPE_STRING,    .ubusGet = trapLoadPowerTrap},
    {.name = "port_trap",               .type = UCI_TYPE_STRING,    .ubusGet = trapLoadPortTrap},
    {.name = "port_tx_shreshold_enable",.type = UCI_TYPE_STRING,    .ubusGet = trapLoadPortTxShEnable},
    {.name = "port_tx_shreshold_per",   .type = UCI_TYPE_STRING,    .ubusGet = trapLoadPortTxShPer},
    {.name = "port_rx_shreshold_enable",.type = UCI_TYPE_STRING,    .ubusGet = trapLoadPortRxShEnable},
    {.name = "port_rx_shreshold_per",   .type = UCI_TYPE_STRING,    .ubusGet = trapLoadPortRxShPer},
};

int trapCfgLoad(void)
{
    struct uci_context *ctx;

    ctx = uci_alloc_context();

    char name[64]; 
    int ret;
    int i;
    struct uci_ptr ptr;

    for(i = 0;i < sizeof(trapUci)/sizeof(UciConfig_S);i++)
    {
        sprintf(name,"switchd.trap.%s",trapUci[i].name);
        ret = load_config(name,trapUci[i].type,&ptr,ctx);
        if(ret)
        {
            uci_free_context(ctx);

            if(trapUci[i].type == UCI_TYPE_LIST) continue;

            LogWrite(ERROR, "load config %s fail!\n",name); 
            return -1;
        }
        
        ret = trapUci[i].ubusGet(&ptr);
        if(ret)
        {
            uci_free_context(ctx);

            LogWrite(ERROR, "ubus get arguement %s fail!\n",name); 
            return -1;
        }
    }

    uci_free_context(ctx);

    return 0;
}

int trapCfgDisplay(void)
{
	int i = 0;
	LogWrite(DEBUG, "file:%s line:%d func:%s, power:%d\n",
				__FILENAME__, __LINE__, __FUNCTION__, trap.power);

	for (i = 0; i < PORT_NUM; ++i)
	{
		LogWrite(DEBUG, "file:%s line:%d func:%s, port[%d]:|%d |%d |%d |%d |%d|\n",
				__FILENAME__, __LINE__, __FUNCTION__, i+1,
				trap.port[i].st, trap.port[i].tx_sh_st, 
				trap.port[i].tx_sh_per, trap.port[i].rx_sh_st, 
				trap.port[i].rx_sh_per);
	}
    return 0;
}

int getPowerSupplyCfg(enum PW_ALARM_STATUS *powEnable)
{
    *powEnable = trap.power;

    return 0;
}

int trap_get_cfg_entry(int port_id, struct trap_cfg *trapCfg)
{
    trapCfg->port_id = port_id + 1;

    trapCfg->port_en = trap.port[port_id].st;

    trapCfg->tx_enable = trap.port[port_id].tx_sh_st;

    trapCfg->tx_percentage = trap.port[port_id].tx_sh_per;

    trapCfg->rx_enable = trap.port[port_id].rx_sh_st;
    
    trapCfg->rx_percentage = trap.port[port_id].rx_sh_per;

    return 0;
}

int trap_display_cfg_table(struct trap_cfg *trapCfg)
{

    LogWrite(DEBUG, "file:%s line:%d func:%s, port[%d]:|%d |%d |%d |%d |%d|\n",
            __FILENAME__, __LINE__, __FUNCTION__,
            trapCfg->port_id, trapCfg->port_en, 
            trapCfg->tx_enable, trapCfg->tx_percentage, 
            trapCfg->rx_enable, trapCfg->rx_percentage);
    
    return 0;
}