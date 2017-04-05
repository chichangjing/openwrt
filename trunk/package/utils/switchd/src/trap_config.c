/* *< 中威文件编码格式为gb2312,cp936 */
/* * $Id$ $DateTime$
 * * @file trap_config.c
 * * @brief This is a table of the switch's trap configure.
 * * @version 0.0.1
 * * @since 0.0.1
 * * @author code<songyaofei@josware.com>
 * * @date 2016-06-24
 * Created it
 * */
/* *****************************************************************
 * *@note
 * Copyright 2012, OB Telecom Electronics Co., Ltd.
 * ALL RIGHTS RESERVED
 * Permission is hereby granted to licensees of OB Telecom Electronics
 * Co., Ltd. products to use or abstract this computer program for
 * the sole purpose of implementing a product based on OB Telecom
 * Electronics Co., Ltd. products. No other rights to reproduce, use,
 * or disseminate this computer program,whether in part or in whole,
 * are granted. OB Telecom Electronics Co., Ltd. makes no
 * representation or warranties with respect to the performance of
 * this computer program, and specifically disclaims any
 * responsibility for any damages, special or consequential,
 * connected with the use of this program.
 * For details, see http://obtelecom.com
 * *******************************************************************/
#include <sys/utsname.h>
#include <sys/sysinfo.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>

#include "switchd.h"

static struct blob_buf b;
static struct ubus_context *_ctx;

enum {
    TRAP_GET_KEY,
    __TRAP_GET_MAX,
};

enum {
    TRAP_SET_KEY,
    __TRAP_SET_MAX
};

static const struct blobmsg_policy trap_get_policy[] = {
    [TRAP_GET_KEY] = {.name = "get_args", .type = BLOBMSG_TYPE_ARRAY},
};

static const struct blobmsg_policy trap_set_policy[] = {
    [TRAP_SET_KEY] = {.name = "set_args", .type = BLOBMSG_TYPE_ARRAY},
};

enum {
    TRAP_POWER,
    TRAP_PORT,
    TRAP_PORT_TX_SH_ENABLE,
    TRAP_PORT_TX_SH_PER,
    TRAP_PORT_RX_SH_ENABLE,
    TRAP_PORT_RX_SH_PER,
    __TRAP_TBL_MAX
};

static struct{
    int power;
    struct{
        int st;
        int tx_sh_st;
        int tx_sh_per;
        int rx_sh_st;
        int rx_sh_per;
    }port[CONFIG_JWS_PORT_TOTAL_NR];
}trap;

/* *
 * * @brief         get power supply trap mode
 * * @retval 0:     OK
 * * @retval -1:    ERROR
 * */
static int trapGetPowerMode(struct blob_buf *buf)
{
    char mode_s[2];
    
    sprintf(mode_s,"%d",trap.power);
    blobmsg_add_string(buf, "power_trap", mode_s);
    LOG_NORMAL("get power trap mode = [%s] OK\n",mode_s);

    return 0;
}

/* *
 * * @brief         set power supply trap mode
 * * @retval 0:     OK
 * * @retval -1:    ERROR
 * */
static int trapSetPowerMode(void *p)
{
    int mode = atoi((char *)p);

    if(mode != FAL_DISABLE && mode != FAL_ENABLE)
    {
        LOG_ERROR("invalid arguement!\n");
        return -1;
    }

    if(mode == trap.power)
    {
        LOG_NORMAL("same arguement,do nothing\n");
        return 0;
    }
    
    trap.power = mode;

    char buf[2];
    struct uci_ptr ptr = {
        .package = "switchd",
        .section = "trap",
        .option = "power_trap",
    };

    sprintf(buf,"%d",mode);
    ubus_collect_config(&ptr,buf,UCI_TYPE_STRING,DO_SET);

    LOG_NORMAL("set power trap mode = [%d] OK\n", mode);

    return 0;
}

/* *
 * * @brief             get port trap mode
 * * @param[port_idx]   port index.
 * * @retval 0:         OK
 * * @retval -1:        ERROR
 * */
static int trapGetPortMode(struct blob_buf *buf,int port_idx)
{
    char mode_s[2];

    sprintf(mode_s,"%d",trap.port[port_idx-1].st);
    blobmsg_add_string(buf,"port_trap",mode_s);
    LOG_NORMAL("get port [%d] trap mode = %s OK\n",port_idx,mode_s);

    return 0;
}

/* *
 * * @brief             set port trap mode
 * * @param[port_idx]   port index.
 * * @retval 0:         OK
 * * @retval -1:        ERROR
 * */
static int trapSetPortMode(int port_idx,void *p)
{
    int mode = atoi((char *)p);

    if(mode != FAL_ENABLE && mode != FAL_DISABLE)
    {
        LOG_ERROR("invalid arguement!\n");
        return -1;
    }

    trap.port[port_idx-1].st = mode;

    char buf[128];
    struct uci_ptr ptr = {
        .package = "switchd",
        .section = "trap",
        .option = "port_trap",
    };

    sprintf(buf,"port1:%d port2:%d port3:%d port4:%d port5:%d port6:%d port7:%d port8:%d port9:%d port10:%d",\
            trap.port[0].st,trap.port[1].st,trap.port[2].st,trap.port[3].st,trap.port[4].st,\
            trap.port[5].st,trap.port[6].st,trap.port[7].st,trap.port[8].st,trap.port[9].st);
    ubus_collect_config(&ptr,buf,UCI_TYPE_STRING,DO_SET);

    LOG_NORMAL("set port [%d] trap mode = [%d] OK\n",port_idx,mode);
    return 0;
}

/* *
 * * @brief             get tx port shreshold enable 
 * * @param[port_idx]   port index.
 * * @retval 0:         OK
 * * @retval -1:        ERROR
 * */
static int trapGetPortTxShEnable(struct blob_buf *buf,int port_idx)
{
    char mode_s[2];

    sprintf(mode_s,"%d",trap.port[port_idx-1].tx_sh_st);
    blobmsg_add_string(buf,"port_tx_shreshold_enable",mode_s);
    LOG_NORMAL("get port [%d] tx shresshold enable = %s OK\n",port_idx,mode_s);

    return 0;
}

/* *
 * * @brief             set tx port shreshold enable
 * * @param[port_idx]   port index.
 * * @retval 0:         OK
 * * @retval -1:        ERROR
 * */
static int trapSetPortTxShEnable(int port_idx,void *p)
{
    int mode = atoi((char *)p);

    if(mode != FAL_ENABLE && mode != FAL_DISABLE)
    {
        LOG_ERROR("invalid arguement!\n");
        return -1;
    }

    trap.port[port_idx-1].tx_sh_st = mode;

    char buf[128];
    struct uci_ptr ptr = {
        .package = "switchd",
        .section = "trap",
        .option = "port_tx_shreshold_enable",
    };

    sprintf(buf,"port1:%d port2:%d port3:%d port4:%d port5:%d port6:%d port7:%d port8:%d port9:%d port10:%d",\
            trap.port[0].tx_sh_st,trap.port[1].tx_sh_st,trap.port[2].tx_sh_st,trap.port[3].tx_sh_st,trap.port[4].tx_sh_st,\
            trap.port[5].tx_sh_st,trap.port[6].tx_sh_st,trap.port[7].tx_sh_st,trap.port[8].tx_sh_st,trap.port[9].tx_sh_st);
    ubus_collect_config(&ptr,buf,UCI_TYPE_STRING,DO_SET);

    LOG_NORMAL("set port [%d] tx shreshold enable = [%d] OK\n",port_idx,mode);
    return 0;
}

/* *
 * * @brief             get tx port shreshold percentage 
 * * @param[port_idx]   port index.
 * * @retval 0:         OK
 * * @retval -1:        ERROR
 * */
static int trapGetPortTxShPerc(struct blob_buf *buf,int port_idx)
{
    char perc_s[2];

    sprintf(perc_s,"%d",trap.port[port_idx-1].tx_sh_per);
    blobmsg_add_string(buf,"port_tx_shreshold_perc",perc_s);
    LOG_NORMAL("get port [%d] tx shreshold percentage = %s OK\n",port_idx,perc_s);

    return 0;
}

/* *
 * * @brief             set tx port shreshold percentage
 * * @param[port_idx]   port index.
 * * @retval 0:         OK
 * * @retval -1:        ERROR
 * */
static int trapSetPortTxShPerc(int port_idx,void *p)
{
    if(trap.port[port_idx-1].tx_sh_st != FAL_ENABLE)
    {
        LOG_ERROR("the feature of port tx shreshold status is disable!\n");
        return -1;
    }

    int perc = atoi((char *)p);

    if(perc < 1 || perc > 99)
    {
        LOG_ERROR("invalid arguement!\n");
        return -1;
    }

    trap.port[port_idx-1].tx_sh_per = perc;

    char buf[128];
    struct uci_ptr ptr = {
        .package = "switchd",
        .section = "trap",
        .option = "port_tx_shreshold_per",
    };

    sprintf(buf,"port1:%d port2:%d port3:%d port4:%d port5:%d port6:%d port7:%d port8:%d port9:%d port10:%d",\
            trap.port[0].tx_sh_per,trap.port[1].tx_sh_per,trap.port[2].tx_sh_per,trap.port[3].tx_sh_per,trap.port[4].tx_sh_per,\
            trap.port[5].tx_sh_per,trap.port[6].tx_sh_per,trap.port[7].tx_sh_per,trap.port[8].tx_sh_per,trap.port[9].tx_sh_per);
    ubus_collect_config(&ptr,buf,UCI_TYPE_STRING,DO_SET);

    LOG_NORMAL("set port [%d] tx shreshold percentage = [%d] OK\n",port_idx,perc);
    return 0;
}

/* *
 * * @brief             get rx port shreshold enable 
 * * @param[port_idx]   port index.
 * * @retval 0:         OK
 * * @retval -1:        ERROR
 * */
static int trapGetPortRxShEnable(struct blob_buf *buf,int port_idx)
{
    char mode_s[2];

    sprintf(mode_s,"%d",trap.port[port_idx-1].rx_sh_st);
    blobmsg_add_string(buf,"port_rx_shreshold_enable",mode_s);
    LOG_NORMAL("get port [%d] rx shresshold enable = %s OK\n",port_idx,mode_s);

    return 0;
}

/* *
 * * @brief             set rx port shreshold enable
 * * @param[port_idx]   port index.
 * * @retval 0:         OK
 * * @retval -1:        ERROR
 * */
static int trapSetPortRxShEnable(int port_idx,void *p)
{
    int mode = atoi((char *)p);

    if(mode != FAL_ENABLE && mode != FAL_DISABLE)
    {
        LOG_ERROR("invalid arguement!\n");
        return -1;
    }

    trap.port[port_idx-1].rx_sh_st = mode;

    char buf[128];
    struct uci_ptr ptr = {
        .package = "switchd",
        .section = "trap",
        .option = "port_rx_shreshold_enable",
    };

    sprintf(buf,"port1:%d port2:%d port3:%d port4:%d port5:%d port6:%d port7:%d port8:%d port9:%d port10:%d",\
            trap.port[0].rx_sh_st,trap.port[1].rx_sh_st,trap.port[2].rx_sh_st,trap.port[3].rx_sh_st,trap.port[4].rx_sh_st,\
            trap.port[5].rx_sh_st,trap.port[6].rx_sh_st,trap.port[7].rx_sh_st,trap.port[8].rx_sh_st,trap.port[9].rx_sh_st);
    ubus_collect_config(&ptr,buf,UCI_TYPE_STRING,DO_SET);

    LOG_NORMAL("set port [%d] rx shreshold enable = [%d] OK\n",port_idx,mode);
    return 0;
}

/* *
 * * @brief             get rx port shreshold percentage 
 * * @param[port_idx]   port index.
 * * @retval 0:         OK
 * * @retval -1:        ERROR
 * */
static int trapGetPortRxShPerc(struct blob_buf *buf,int port_idx)
{
    char perc_s[2];

    sprintf(perc_s,"%d",trap.port[port_idx-1].rx_sh_per);
    blobmsg_add_string(buf,"port_rx_shreshold_perc",perc_s);
    LOG_NORMAL("get port [%d] rx shreshold percentage = %s OK\n",port_idx,perc_s);

    return 0;
}

/* *
 * * @brief             set rx port shreshold percentage
 * * @param[port_idx]   port index.
 * * @retval 0:         OK
 * * @retval -1:        ERROR
 * */
static int trapSetPortRxShPerc(int port_idx,void *p)
{
    if(trap.port[port_idx-1].rx_sh_st != FAL_ENABLE)
    {
        LOG_ERROR("the feature of port rx shreshold status is disable!\n");
        return -1;
    }

    int perc = atoi((char *)p);

    if(perc < 1 || perc > 99)
    {
        LOG_ERROR("invalid arguement!\n");
        return -1;
    }

    trap.port[port_idx-1].rx_sh_per = perc;

    char buf[128];
    struct uci_ptr ptr = {
        .package = "switchd",
        .section = "trap",
        .option = "port_rx_shreshold_per",
    };

    sprintf(buf,"port1:%d port2:%d port3:%d port4:%d port5:%d port6:%d port7:%d port8:%d port9:%d port10:%d",\
            trap.port[0].rx_sh_per,trap.port[1].rx_sh_per,trap.port[2].rx_sh_per,trap.port[3].rx_sh_per,trap.port[4].rx_sh_per,\
            trap.port[5].rx_sh_per,trap.port[6].rx_sh_per,trap.port[7].rx_sh_per,trap.port[8].rx_sh_per,trap.port[9].rx_sh_per);
    ubus_collect_config(&ptr,buf,UCI_TYPE_STRING,DO_SET);

    LOG_NORMAL("set port [%d] rx shreshold percentage = [%d] OK\n",port_idx,perc);
    return 0;
}

static const struct jw_switch_policy trap_tbl[] = {
    [TRAP_POWER] = {.name = "power_trap", .get_ext_handler = trapGetPowerMode, .set_ext_handler = trapSetPowerMode}, 
    [TRAP_PORT] = {.name = "port_trap", .get_handler = trapGetPortMode, .set_handler = trapSetPortMode},
    [TRAP_PORT_TX_SH_ENABLE] = {.name = "port_tx_shreshold_enable", .get_handler = trapGetPortTxShEnable, .set_handler = trapSetPortTxShEnable},
    [TRAP_PORT_TX_SH_PER] = {.name = "port_tx_shreshold_perc", .get_handler = trapGetPortTxShPerc, .set_handler = trapSetPortTxShPerc},
    [TRAP_PORT_RX_SH_ENABLE] = {.name = "port_rx_shreshold_enable", .get_handler = trapGetPortRxShEnable, .set_handler = trapSetPortRxShEnable},
    [TRAP_PORT_RX_SH_PER] = {.name = "port_rx_shreshold_perc", .get_handler = trapGetPortRxShPerc, .set_handler = trapSetPortRxShPerc},
};

/* *
 * * @brief         parse port trap get
 * * @param[lvl3]   level 3 message structure.
 * * @retval 0:     OK
 * * @retval -1:    ERROR
 * */
static int trapParsePortGet(struct blob_attr *lvl3, struct blob_buf *buf)
{
    const char *name    = blobmsg_name(lvl3);
    int idx     = atoi(name + 5);

    if (idx < 1 || idx > CONFIG_JWS_PORT_TOTAL_NR) 
    {
        LOG_ERROR("invalid port index!\n");
        return -1;
    }
    
    void *ret_array;

    ret_array= blobmsg_open_array(buf, name);
    foreach_json_array_get(idx,lvl3,buf,trap_tbl, __TRAP_TBL_MAX);
    blobmsg_close_array(buf, ret_array);	

    return 0;
}

/* *
 * * @brief         parse port trap set
 * * @param[lvl3]   level 3 message structure.
 * * @retval 0:     OK
 * * @retval -1:    ERROR
 * */
static int trapParsePortSet(struct blob_attr *lvl3)
{
    const char *name    = blobmsg_name(lvl3);
    int idx     = atoi(name + 5);

    if (idx < 1 || idx > CONFIG_JWS_PORT_TOTAL_NR) 
    {
        LOG_ERROR("invalid port index!\n");
        return -1;
    }

    /**<    foreach array of level 3 message  */
    int ret;
    ret = foreach_json_array_set(idx,lvl3,trap_tbl,__TRAP_TBL_MAX);
    if(ret)
    {
        LOG_ERROR("foreach index %d error!\n",idx);
        return -1;
    }

    return 0;
}

/* *
 * * @brief         method "trap_get" handler
 * * @retval 0:     OK
 * * @retval -1:    ERROR
 * */
static int trapGetHandler(struct ubus_context *ctx, struct ubus_object *obj,
                struct ubus_request_data *req, const char *method,
                struct blob_attr *msg)
{
    struct blob_attr *tb[__TRAP_GET_MAX];    
	struct blob_attr *lvl1 = NULL;  

    blobmsg_parse(trap_get_policy, __TRAP_GET_MAX, tb, blob_data(msg), blob_len(msg));
	lvl1 = tb[TRAP_GET_KEY];     
    if (!lvl1) 
	{
        LOG_ERROR("invalid params in %s\n",__func__);
        return UBUS_STATUS_INVALID_ARGUMENT;
    }

	if (blobmsg_type(lvl1) != BLOBMSG_TYPE_ARRAY) 
	{
        LOG_ERROR("level 1 message type is not array\n");
        return -1;
    }
    
    struct blob_attr *lvl2 = NULL;
    void *ret_array;
    void *ret_table;

    int rem;
    
	blobmsg_buf_init(&b);
    ret_array = blobmsg_open_array(&b, "ret"); 

    blobmsg_for_each_attr(lvl2,lvl1, rem) 
    {
        switch (blobmsg_type(lvl2))
        {
            case BLOBMSG_TYPE_STRING:
            {
                struct jw_switch_policy *policy_p;
                char *name = blobmsg_get_string(lvl2);

                policy_p = (struct jw_switch_policy *)jw_switchd_get_context(name, trap_tbl, __TRAP_TBL_MAX);
                if (policy_p && policy_p->get_ext_handler) 
                {
                    ret_table = blobmsg_open_table(&b, NULL);
                    policy_p->get_ext_handler(&b);
                    blobmsg_close_table(&b, ret_table);	
                }
                else
                {
                    LOG_ERROR("policy check not pass!\n");
                }
            }
                break;
            case BLOBMSG_TYPE_TABLE:
            {
                struct blob_attr *lvl3 = blobmsg_data(lvl2);

                if(blobmsg_type(lvl3) == BLOBMSG_TYPE_ARRAY)
                {
                    ret_table = blobmsg_open_table(&b, NULL);
                    trapParsePortGet(lvl3,&b);
                    blobmsg_close_table(&b, ret_table);	
                }
                else
                {
                    LOG_NORMAL("level 3 message type is not available!\n");
                }
            }
                break;
            default:
                LOG_NORMAL("level 2 message type is not available!\n");
                break;
        }
    }
    
    blobmsg_close_array(&b, ret_array);	
    ubus_send_reply(ctx, req, b.head);

	return UBUS_STATUS_OK;

}

/* *
 * * @brief         method "trap_set" handler
 * * @retval 0:     OK
 * * @retval -1:    ERROR
 * */
static int trapSetHandler(struct ubus_context *ctx, struct ubus_object *obj,
                struct ubus_request_data *req, const char *method,
                struct blob_attr *msg)
{
    struct blob_attr *tb[__TRAP_SET_MAX];    
	struct blob_attr *lvl1 = NULL;  
    int ret = -1;

    blobmsg_parse(trap_set_policy, __TRAP_SET_MAX, tb, blob_data(msg), blob_len(msg));
	lvl1 = tb[TRAP_SET_KEY];     
    if (!lvl1) 
	{
        LOG_ERROR("invalid params in %s\n",__func__);
        goto end;
    }

	if (blobmsg_type(lvl1) != BLOBMSG_TYPE_ARRAY) 
	{
        LOG_ERROR("level 1 message type is not array\n");
        goto end;
    }

    struct blob_attr *lvl2 = NULL;
    int rem;

	blob_buf_init(&b, 0);

    blobmsg_for_each_attr(lvl2, lvl1, rem) 
    {
        if (blobmsg_type(lvl2) == BLOBMSG_TYPE_TABLE) 
        {
            struct blob_attr *lvl3 = blobmsg_data(lvl2);
            
            switch(blobmsg_type(lvl3))
            {
                case BLOBMSG_TYPE_STRING:
                {
                    struct jw_switch_policy *policy_p;
                    const char *name  = blobmsg_name(lvl3);
                    char *value = blobmsg_get_string(lvl3);

                    policy_p = (struct jw_switch_policy *)jw_switchd_get_context(name, trap_tbl, __TRAP_TBL_MAX);
                    if (policy_p && policy_p->set_ext_handler) 
                    {
                        ret = policy_p->set_ext_handler(value);
                        if(ret)
                            goto end;
                    }
                    else
                    {
                        LOG_ERROR("policy check not pass!\n");
                        ret = -1;
                        goto end;
                    }
                }
                    break;
                case BLOBMSG_TYPE_ARRAY:
                    ret = trapParsePortSet(lvl3);
                    if(ret)
                        goto end;
                    break;
                default:
                    LOG_ERROR("level 3 message type is not available!\n");
                    ret = -1;
                    goto end;
            }
        }
        else
        {
            LOG_ERROR("level 2 message type is not available!\n");
            ret = -1;
            goto end;
        }
    }

end:
    blobmsg_add_string(&b, "ret", ret?"-1":"0");
	ubus_send_reply(ctx, req, b.head);

	return UBUS_STATUS_OK;

}


static const struct ubus_method trap_methods[] = {
	UBUS_METHOD("trap_get", trapGetHandler, trap_get_policy),
    UBUS_METHOD("trap_set", trapSetHandler, trap_set_policy),
};


static struct ubus_object_type trap_object_type =
	UBUS_OBJECT_TYPE("trap", trap_methods);


static struct ubus_object trap_object = {
	.name = "trap",
	.type = &trap_object_type,
	.methods = trap_methods,
	.n_methods = ARRAY_SIZE(trap_methods),
};

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
        LOG_ERROR("invalid arguement!\n"); 
        return -1;
    }


    trap.power = power;
    LOG_NORMAL("trap load power trap mode = [%d] OK\n", power);
    
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
            LOG_ERROR("port trap format in UCI file error!\n"); 
            return -1;
        }

        port = atoi(pstr + 4);
        if(port < 1 || port > CONFIG_JWS_PORT_TOTAL_NR)
        {
            LOG_ERROR("port trap arguement - port index in UCI file error!\n"); 
            return -1;
        }

        char *pdelim = strchr(pstr,':');
        if(pdelim == NULL)
        {
            LOG_ERROR("port trap format in UCI file error!\n"); 
            return -1;
        }

        port_trap = atoi(pdelim + 1);
        if(port_trap != FAL_DISABLE && port_trap != FAL_ENABLE)
        {
            LOG_ERROR("invalid arguement!\n");
            return -1;
        }

        trap.port[port-1].st = port_trap;
    }
    free(string);

    LOG_NORMAL("trap load port trap OK\n");
    
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
            LOG_ERROR("port tx shreshold enalbe format in UCI file error!\n"); 
            return -1;
        }

        port = atoi(pstr + 4);
        if(port < 1 || port > CONFIG_JWS_PORT_TOTAL_NR)
        {
            LOG_ERROR("port tx shreshold enalbe arguement - port index in UCI file error!\n"); 
            return -1;
        }

        char *pdelim = strchr(pstr,':');
        if(pdelim == NULL)
        {
            LOG_ERROR("port tx shreshold enalbe format in UCI file error!\n"); 
            return -1;
        }

        sh_enable = atoi(pdelim + 1);
        if(sh_enable != FAL_DISABLE && sh_enable != FAL_ENABLE)
        {
            LOG_ERROR("invalid arguement!\n");
            return -1;
        }

        trap.port[port-1].tx_sh_st = sh_enable;
    }
    free(string);

    LOG_NORMAL("trap load port tx shreshold enable OK\n");
    
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
            LOG_ERROR("port tx shreshold percentage format in UCI file error!\n"); 
            return -1;
        }

        port = atoi(pstr + 4);
        if(port < 1 || port > CONFIG_JWS_PORT_TOTAL_NR)
        {
            LOG_ERROR("port tx shreshold percentage arguement - port index in UCI file error!\n"); 
            return -1;
        }

        char *pdelim = strchr(pstr,':');
        if(pdelim == NULL)
        {
            LOG_ERROR("port tx shreshold percentage format in UCI file error!\n"); 
            return -1;
        }

        perc = atoi(pdelim + 1);
        if(perc < 1 || perc > 99)
        {
            LOG_ERROR("invalid arguement!\n");
            return -1;
        }

        trap.port[port-1].tx_sh_per = perc;
    }
    free(string);

    LOG_NORMAL("trap load port tx shreshold percentage OK\n");
    
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
            LOG_ERROR("port rx shreshold enalbe format in UCI file error!\n"); 
            return -1;
        }

        port = atoi(pstr + 4);
        if(port < 1 || port > CONFIG_JWS_PORT_TOTAL_NR)
        {
            LOG_ERROR("port rx shreshold enalbe arguement - port index in UCI file error!\n"); 
            return -1;
        }

        char *pdelim = strchr(pstr,':');
        if(pdelim == NULL)
        {
            LOG_ERROR("port rx shreshold enalbe format in UCI file error!\n"); 
            return -1;
        }

        sh_enable = atoi(pdelim + 1);
        if(sh_enable != FAL_DISABLE && sh_enable != FAL_ENABLE)
        {
            LOG_ERROR("invalid arguement!\n");
            return -1;
        }

        trap.port[port-1].rx_sh_st = sh_enable;
    }
    free(string);

    LOG_NORMAL("trap load port rx shreshold enable OK\n");
    
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
            LOG_ERROR("port rx shreshold percentage format in UCI file error!\n"); 
            return -1;
        }

        port = atoi(pstr + 4);
        if(port < 1 || port > CONFIG_JWS_PORT_TOTAL_NR)
        {
            LOG_ERROR("port rx shreshold percentage arguement - port index in UCI file error!\n"); 
            return -1;
        }

        char *pdelim = strchr(pstr,':');
        if(pdelim == NULL)
        {
            LOG_ERROR("port rx shreshold percentage format in UCI file error!\n"); 
            return -1;
        }

        perc = atoi(pdelim + 1);
        if(perc < 1 || perc > 99)
        {
            LOG_ERROR("invalid arguement!\n");
            return -1;
        }

        trap.port[port-1].rx_sh_per = perc;
    }
    free(string);

    LOG_NORMAL("trap load port rx shreshold percentage OK\n");
    
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

static int trapCfgLoad(void)
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

            LOG_ERROR("load config %s fail!\n",name); 
            return -1;
        }
        
        ret = trapUci[i].ubusGet(&ptr);
        if(ret)
        {
            uci_free_context(ctx);

            LOG_ERROR("ubus get arguement %s fail!\n",name); 
            return -1;
        }
    }



    uci_free_context(ctx);

    return 0;
}

void ubus_init_trap(struct ubus_context *ctx)
{
	int ret;

	_ctx = ctx;

    ret = trapCfgLoad();
    if(ret)
    {
		LOG_ERROR("trap load configure from UCI file fail\n");
        return;
    }

	ret = ubus_add_object(ctx, &trap_object);
    
	if (ret)
    {
		LOG_ERROR("Failed to add object [trap]: %s\n", ubus_strerror(ret));
    }
}
