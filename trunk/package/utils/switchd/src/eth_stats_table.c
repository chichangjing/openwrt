/* *< 中威文件编码格式为gb2312,cp936 */
/* * $Id$ $DateTime$
 * * @file eth_stats_table.c
 * * @brief This is a table of the switch's port ethernet statistics.
 * * @version 0.0.1
 * * @since 0.0.1
 * * @author code<songyaofei@joyware.com>
 * * @date 2016-04-25
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
    ETH_STATS_GET_KEY,
    __ETH_STATS_GET_MAX
};

enum {
    ETH_STATS_SET_KEY,
    __ETH_STATS_SET_MAX
};

static const struct blobmsg_policy eth_stats_get_policy[] = {
    [ETH_STATS_GET_KEY] = {.name = "get_args", .type = BLOBMSG_TYPE_ARRAY},
};

static const struct blobmsg_policy eth_stats_set_policy[] = {
    [ETH_STATS_SET_KEY] = {.name = "set_args", .type = BLOBMSG_TYPE_ARRAY},
};

enum {
    ETH_RX_BYTES,
    ETH_RX_UCAST_PKTS,
    ETH_RX_BCAST_PKTS,
    ETH_RX_MCAST_PKTS,
    ETH_RX_PAUSE_PKTS,
    ETH_TX_BYTES,
    ETH_TX_UCAST_PKTS,
    ETH_TX_BCAST_PKTS,
    ETH_TX_MCAST_PKTS,
    ETH_TX_PAUSE_PKTS,
    ETH_CLEAR_SELECT,
    ETH_CLEAR,
    __ETH_STATS_TBL_MAX,
};

static struct{
    fal_mib_info_t mib_info;
    int clear_f;
}eth_statis[CONFIG_JWS_PORT_TOTAL_NR];

static char res_c[2] = "1";


/* *
 * * @brief             get rx bytes
 * * @param[port_idx]   port index.
 * * @retval 0:         OK
 * * @retval -1:        ERROR
 * */
static int ethStatisticGetRxBytes(struct blob_buf *buf, int port_idx)
{
    char rx_bytes_s[20];

    sprintf(rx_bytes_s,"%llu",((unsigned long long)eth_statis[port_idx-1].mib_info.RxGoodByteHi << 32 | (unsigned long long)eth_statis[port_idx-1].mib_info.RxGoodByteLo));
    //LOG_NORMAL("This is port [%d], rx bytes [%s]\n",port_idx,rx_bytes_s);
    blobmsg_add_string(buf, "rx_bytes", rx_bytes_s);

    return 0;
}

/* *
 * * @brief             get rx unicast packets
 * * @param[port_idx]   port index.
 * * @retval 0:         OK
 * * @retval -1:        ERROR
 * */
static int ethStatisticGetRxUcastPkts(struct blob_buf *buf, int port_idx)
{
    char rx_ucast_pkts_s[12];

    sprintf(rx_ucast_pkts_s,"%u",eth_statis[port_idx-1].mib_info.RxUnicastPkts);
    //LOG_NORMAL("This is port %d, rx ucast packets [%s]\n",port_idx,rx_ucast_pkts_s);
    blobmsg_add_string(buf, "rx_unicast_packets", rx_ucast_pkts_s);

    return 0;
}

/* *
 * * @brief             get rx broadcast packets
 * * @param[port_idx]   port index.
 * * @retval 0:         OK
 * * @retval -1:        ERROR
 * */
static int ethStatisticGetRxBcastPkts(struct blob_buf *buf, int port_idx)
{
    char rx_bcast_pkts_s[12];

    sprintf(rx_bcast_pkts_s,"%u",eth_statis[port_idx-1].mib_info.RxBroadcastPkts);
    //LOG_NORMAL("This is port %d, rx bcast packets [%s]\n",port_idx,rx_bcast_pkts_s);
    blobmsg_add_string(buf, "rx_broadcast_packets", rx_bcast_pkts_s);

    return 0;
}

/* *
 * * @brief             get rx multicast packets
 * * @param[port_idx]   port index.
 * * @retval 0:         OK
 * * @retval -1:        ERROR
 * */
static int ethStatisticGetRxMcastPkts(struct blob_buf *buf, int port_idx)
{
    char rx_mcast_pkts_s[12];

    sprintf(rx_mcast_pkts_s,"%u",eth_statis[port_idx-1].mib_info.RxMulticastPkts);
    //LOG_NORMAL("This is port %d, rx mcast packets [%s]\n",port_idx,rx_mcast_pkts_s);
    blobmsg_add_string(buf, "rx_multicast_packets", rx_mcast_pkts_s);

    return 0;
}

/* *
 * * @brief             get rx pause packets
 * * @param[port_idx]   port index.
 * * @retval 0:         OK
 * * @retval -1:        ERROR
 * */
static int ethStatisticGetRxPausePkts(struct blob_buf *buf, int port_idx)
{
    char rx_pause_pkts_s[12];

    sprintf(rx_pause_pkts_s,"%u",eth_statis[port_idx-1].mib_info.RxPausePkts);
    //LOG_NORMAL("This is port %d, rx pause packets [%s]\n",port_idx,rx_pause_pkts_s);
    blobmsg_add_string(buf, "rx_pause_packets", rx_pause_pkts_s);

    return 0;
}

/* *
 * * @brief             get tx bytes
 * * @param[port_idx]   port index.
 * * @retval 0:         OK
 * * @retval -1:        ERROR
 * */
static int ethStatisticGetTxBytes(struct blob_buf *buf, int port_idx)
{
    char tx_bytes_s[20];
    
    sprintf(tx_bytes_s,"%llu",((unsigned long long)eth_statis[port_idx-1].mib_info.TxByteHi<< 32 | (unsigned long long)eth_statis[port_idx-1].mib_info.TxByteLo));
    //LOG_NORMAL("This is port [%d], tx bytes [%s]\n",port_idx,tx_bytes_s);
    blobmsg_add_string(buf, "tx_bytes", tx_bytes_s);

    return 0;
}

/* *
 * * @brief             get tx unicast packets
 * * @param[port_idx]   port index.
 * * @retval 0:         OK
 * * @retval -1:        ERROR
 * */
static int ethStatisticGetTxUcastPkts(struct blob_buf *buf, int port_idx)
{
    char tx_ucast_pkts_s[12];

    sprintf(tx_ucast_pkts_s,"%u",eth_statis[port_idx-1].mib_info.TxUnicastPkts);
    //LOG_NORMAL("This is port %d, tx ucast packets [%s]\n",port_idx,tx_ucast_pkts_s);
    blobmsg_add_string(buf, "tx_unicast_packets", tx_ucast_pkts_s);

    return 0;
}

/* *
 * * @brief             get tx broadcast packets
 * * @param[port_idx]   port index.
 * * @retval 0:         OK
 * * @retval -1:        ERROR
 * */
static int ethStatisticGetTxBcastPkts(struct blob_buf *buf, int port_idx)
{
    char tx_bcast_pkts_s[12];

    sprintf(tx_bcast_pkts_s,"%u",eth_statis[port_idx-1].mib_info.TxBroadcastPkts);
    //LOG_NORMAL("This is port %d, tx bcast packets [%s]\n",port_idx,tx_bcast_pkts_s);
    blobmsg_add_string(buf, "tx_broadcast_packets", tx_bcast_pkts_s);

    return 0;
}

/* *
 * * @brief             get tx multicast packets
 * * @param[port_idx]   port index.
 * * @retval 0:         OK
 * * @retval -1:        ERROR
 * */
static int ethStatisticGetTxMcastPkts(struct blob_buf *buf, int port_idx)
{
    char tx_mcast_pkts_s[12];

    sprintf(tx_mcast_pkts_s,"%u",eth_statis[port_idx-1].mib_info.TxMulticastPkts);
    //LOG_NORMAL("This is port %d, tx mcast packets [%s]\n",port_idx,tx_mcast_pkts_s);
    blobmsg_add_string(buf, "tx_multicast_packets", tx_mcast_pkts_s);

    return 0;
}

/* *
 * * @brief             get tx pause packets
 * * @param[port_idx]   port index.
 * * @retval 0:         OK
 * * @retval -1:        ERROR
 * */
static int ethStatisticGetTxPausePkts(struct blob_buf *buf, int port_idx)
{
    char tx_pause_pkts_s[12];

    sprintf(tx_pause_pkts_s,"%u",eth_statis[port_idx-1].mib_info.TxPausePkts);
    //LOG_NORMAL("This is port %d, tx pause packets [%s]\n",port_idx,tx_pause_pkts_s);
    blobmsg_add_string(buf, "tx_pause_packets", tx_pause_pkts_s);

    return 0;
}

/* *
 * * @brief             get clear select 
 * * @param[port_idx]   port index.
 * * @retval 0:         OK
 * * @retval -1:        ERROR
 * */
static int ethStatisticGetClearSelect(struct blob_buf *buf, int port_idx)
{
    char clear_select_s[2];

    sprintf(clear_select_s,"%u",eth_statis[port_idx-1].clear_f);
    //LOG_NORMAL("This is port %d, get clear select = [%s]\n",port_idx,clear_select_s);
    blobmsg_add_string(buf, "clear_select", clear_select_s);

    return 0;
}

/* *
 * * @brief             set clear select 
 * * @param[port_idx]   port index.
 * * @param[p]          FAL_ENABLE/FAL_DISABLE.
 * * @retval 0:         OK
 * * @retval -1:        ERROR
 * */
static int ethStatisticSetClearSelect(int port_idx,void *p)
{
    int clear_select = atoi((char *)p);

    if(clear_select != FAL_ENABLE && clear_select != FAL_DISABLE)
    {
        LOG_ERROR("set clear select arguement invalid!\n");
        return -1;
    }
    if(clear_select == eth_statis[port_idx-1].clear_f)
    {
        LOG_NORMAL("same arguement,do nothing\n");
        return 0;
    }

    eth_statis[port_idx-1].clear_f = clear_select;

    char buf[128];
    struct uci_ptr ptr = {
        .package = "switchd",
        .section = "ethernet_stats",
        .option = "clear_select",
    };

    sprintf(buf,"port1:%d port2:%d port3:%d port4:%d port5:%d port6:%d port7:%d port8:%d port9:%d port10:%d",\
            eth_statis[0].clear_f,eth_statis[1].clear_f,eth_statis[2].clear_f,eth_statis[3].clear_f,eth_statis[4].clear_f,\
            eth_statis[5].clear_f,eth_statis[6].clear_f,eth_statis[7].clear_f,eth_statis[8].clear_f,eth_statis[9].clear_f);
    ubus_collect_config(&ptr,buf,UCI_TYPE_STRING,DO_SET);

    LOG_NORMAL("port [%d] set clear select = [%d] OK\n",port_idx,clear_select);

    return 0;
}

/* *
 * * @brief     clear 
 * * @param[p]  FAL_ENABLE/FAL_DISABLE.
 * * @retval    0: OK
 * * @retval    -1: ERROR
 * * @note      normaly clear_mask is equal 1 just clear all selected,if clear_mask is not equal 1,it means a bit mask format[0-CONFIG_JWS_PORT_TOTAL_NR]
 * */
static int ethStatisticSetClear(void *p)
{
    int ret = 0;
    int port_idx;
    unsigned int clear_mask = atoi((char *)p);    

    if(clear_mask != 1)
    {
        int clear_select;

        LOG_NORMAL("this is bit mask format\n");
        for(port_idx = 1;port_idx <= CONFIG_JWS_PORT_TOTAL_NR;port_idx++)
        {
            clear_select = (clear_mask >> port_idx) & 0x01;
            if(clear_select)
            {
                ret = fal_mib_info_clear(0,port_idx);
                if(ret)
                {
                    LOG_ERROR("mib info get error = %d\n",ret);
                    memcpy(res_c,"1",2);
                    return -1;
                }

                LOG_NORMAL("mib info port [%d] clear OK\n",port_idx);
            }
            ethStatisticSetClearSelect(port_idx,clear_select?"1":"2");
        }
    }
    else
    {
        for(port_idx = 1;port_idx <= CONFIG_JWS_PORT_TOTAL_NR;port_idx++)
        {
            if(eth_statis[port_idx-1].clear_f == FAL_ENABLE)
            {
                ret = fal_mib_info_clear(0,port_idx);
                if(ret)
                {
                    LOG_ERROR("mib info get error = %d\n",ret);
                    memcpy(res_c,"1",2);
                    return -1;
                }
                LOG_NORMAL("mib info port [%d] clear OK\n",port_idx);
            }
        }
    }

    memcpy(res_c,"2",2);
    return 0;
}

static const struct jw_switch_policy eth_stats_tbl[] = {
    [ETH_RX_BYTES]       = {.name = "rx_bytes", .get_handler = ethStatisticGetRxBytes}, 
    [ETH_RX_UCAST_PKTS]  = {.name = "rx_unicast_packets", .get_handler = ethStatisticGetRxUcastPkts},
    [ETH_RX_BCAST_PKTS]  = {.name = "rx_broadcast_packets", .get_handler = ethStatisticGetRxBcastPkts},
    [ETH_RX_MCAST_PKTS]  = {.name = "rx_multicast_packets", .get_handler = ethStatisticGetRxMcastPkts},
    [ETH_RX_PAUSE_PKTS]  = {.name = "rx_pause_packets", .get_handler = ethStatisticGetRxPausePkts},
    [ETH_TX_BYTES]       = {.name = "tx_bytes", .get_handler = ethStatisticGetTxBytes},
    [ETH_TX_UCAST_PKTS]  = {.name = "tx_unicast_packets", .get_handler = ethStatisticGetTxUcastPkts},
    [ETH_TX_BCAST_PKTS]  = {.name = "tx_broadcast_packets", .get_handler = ethStatisticGetTxBcastPkts},
    [ETH_TX_MCAST_PKTS]  = {.name = "tx_multicast_packets", .get_handler = ethStatisticGetTxMcastPkts},
    [ETH_TX_PAUSE_PKTS]  = {.name = "tx_pause_packets", .get_handler = ethStatisticGetTxPausePkts},
    [ETH_CLEAR_SELECT] = {.name = "clear_select", .get_handler = ethStatisticGetClearSelect, .set_handler = ethStatisticSetClearSelect},
    [ETH_CLEAR] = {.name = "clear", .set_ext_handler = ethStatisticSetClear},
}; 

/* *
 * * @brief         parse port ethernet statistic to get config list
 * * @param[lvl3]   level 3 message structure.
 * * @retval 0:     OK
 * * @retval -1:    ERROR
 * */
static int ethStatisticParsePortEthStatsGet(struct blob_attr *lvl3, struct blob_buf *buf)
{	
    const char *name    = blobmsg_name(lvl3);
    int idx             = atoi(name + strlen(PORT_IDX));

    if (idx < 1 || idx > CONFIG_JWS_PORT_TOTAL_NR) 
    {
        LOG_ERROR("invalid index %d\n", idx);
        return -1;
    }
 
    int ret;
    
    /**********< get port statistic form lower */
    ret = fal_mib_info_get(0,idx,&eth_statis[idx-1].mib_info);
    if(ret)
    {
        LOG_ERROR("mib info get error = %d\n",ret);
        return -1;
    }
    
    void *ret_array;

    ret_array= blobmsg_open_array(buf, name);
    foreach_json_array_get(idx,lvl3,buf,eth_stats_tbl, __ETH_STATS_TBL_MAX);
    blobmsg_close_array(buf, ret_array);	

    return 0;
}

/* *
 * * @brief         parse port ethernet statistic to clear port data
 * * @param[lvl3]   point to level 3 message structure.
 * * @retval 0:     OK
 * * @retval -1:    ERROR
 * */
static int ethStatisticParsePortEthStatsSet(struct blob_attr *lvl3)
{
    const char *name    = blobmsg_name(lvl3);
    int idx             = atoi(name + strlen(PORT_IDX));

    if (idx < 1 || idx > CONFIG_JWS_PORT_TOTAL_NR) 
    {
        LOG_ERROR("invalid index %d\n", idx);
        return -1;
    }

    int ret;
    ret = foreach_json_array_set(idx,lvl3,eth_stats_tbl, __ETH_STATS_TBL_MAX);
    if(ret)
    {
        LOG_ERROR("foreach index %d error!\n",idx);
        return -1;
    }
    
    return 0;
}

/* *
 * * @brief         handle function for the method of "eth_stats_get"
 * * @retval 0:     OK
 * * @retval -1:    ERROR
 * */
static int ethStatisticEthStatsGetHandle(struct ubus_context *ctx, struct ubus_object *obj,
                struct ubus_request_data *req, const char *method,
                struct blob_attr *msg)
{
    struct blob_attr *tb[__ETH_STATS_GET_MAX];    
	struct blob_attr *lvl1 = NULL;  

    blobmsg_parse(eth_stats_get_policy, __ETH_STATS_GET_MAX, tb, blob_data(msg), blob_len(msg));
	lvl1 = tb[ETH_STATS_GET_KEY];     
    if (!lvl1) 
	{
        LOG_ERROR("invalid params in %s\n", __func__);
        return -1;
    }

	if (blobmsg_type(lvl1) != BLOBMSG_TYPE_ARRAY) 
	{
        LOG_ERROR("blobmsg type is not array\n");
        return -1;
    }

    struct blob_attr *lvl2 = NULL; 
    int rem;
    void *ret_array;
    void *ret_table;
		
	blob_buf_init(&b, 0);
    ret_array = blobmsg_open_array(&b, "ret"); 
    
    
    blobmsg_for_each_attr(lvl2,lvl1, rem) 
    {
        switch (blobmsg_type(lvl2)) 
        {
            case BLOBMSG_TYPE_TABLE:
            {
                struct blob_attr *lvl3 = blobmsg_data(lvl2);
                
                if (blobmsg_type(lvl3) ==  BLOBMSG_TYPE_ARRAY) 
                {
                    ret_table = blobmsg_open_table(&b, NULL);
                    ethStatisticParsePortEthStatsGet(lvl3, &b);
                    blobmsg_close_table(&b, ret_table);
                } 
                else 
                {
                    LOG_ERROR("level 3 message type is not avaiable!\n");
                }
            }
            break;
            case BLOBMSG_TYPE_STRING:
            {
                char *value = blobmsg_get_string(lvl2);
                
                if(!strcmp(value,"clear"))
                {
                    ret_table = blobmsg_open_table(&b, NULL);
                    blobmsg_add_string(&b, "clear", res_c);
                    blobmsg_close_table(&b, ret_table);
                }
            }
                break;
            default:
                LOG_ERROR("level 2 message type is not avaiable!\n");
                break;
        }
    }

    blobmsg_close_array(&b, ret_array);

	ubus_send_reply(ctx, req, b.head);

	return 0;
}

/* *
 * * @brief         handle function for the method of "eth_stats_set"
 * * @retval 0:     OK
 * * @retval -1:    ERROR
 * */
static int ethStatisticEthStatsSetHandle(struct ubus_context *ctx, struct ubus_object *obj,
                struct ubus_request_data *req, const char *method,
                struct blob_attr *msg)
{
    struct blob_attr *tb[__ETH_STATS_SET_MAX];    
	struct blob_attr *lvl1 = NULL;  
    int ret = -1;

    blobmsg_parse(eth_stats_set_policy, __ETH_STATS_SET_MAX, tb, blob_data(msg), blob_len(msg));
	lvl1 = tb[ETH_STATS_SET_KEY];     
    if (!lvl1) 
	{
        LOG_ERROR("invalid params in %s\n", __func__);
        goto end;
    }

	if (blobmsg_type(lvl1) != BLOBMSG_TYPE_ARRAY) 
	{
        LOG_ERROR("blobmsg type is not array\n");
        goto end;
    }

    struct blob_attr *lvl2 = NULL; 
    int rem = 0;
		
	blob_buf_init(&b, 0);

    blobmsg_for_each_attr(lvl2, lvl1, rem) 
    {
        if(blobmsg_type(lvl2) == BLOBMSG_TYPE_TABLE)
        {
            struct blob_attr *lvl3 = blobmsg_data(lvl2);

            switch(blobmsg_type(lvl3))
            {
                case BLOBMSG_TYPE_STRING:
                {
                    struct jw_switch_policy *policy_p;
                    const char *name  = blobmsg_name(lvl3);
                    char *value = blobmsg_get_string(lvl3);

                    policy_p = (struct jw_switch_policy *)jw_switchd_get_context(name, eth_stats_tbl, __ETH_STATS_TBL_MAX);
                    if (policy_p && policy_p->set_ext_handler) 
                    {
                        ret = policy_p->set_ext_handler(value);
                        if(ret)
                            goto end;
                    }
                    else
                    {
                        LOG_ERROR("policy check not pass!\n");
                    }
                }
                    break;
                case BLOBMSG_TYPE_ARRAY:
                    ret = ethStatisticParsePortEthStatsSet(lvl3);
                    if(ret)
                        goto end;
                    break;
                default:
                    LOG_ERROR("level 3 message type is not available!\n");
                    break;
            }
        }
        else
        { 
            LOG_ERROR("level 2 message type is not available!\n");
            break;
        }
    }

end:
    blobmsg_add_string(&b, "ret", ret?"-1":"0");
	ubus_send_reply(ctx, req, b.head);

	return UBUS_STATUS_OK;

}

static const struct ubus_method eth_stats_methods[] = {
	UBUS_METHOD("eth_stats_get", ethStatisticEthStatsGetHandle,eth_stats_get_policy),
    UBUS_METHOD("eth_stats_set",ethStatisticEthStatsSetHandle,eth_stats_set_policy),
};


static struct ubus_object_type eth_stats_object_type =
	UBUS_OBJECT_TYPE("eth_stats", eth_stats_methods);

static struct ubus_object eth_stats_object = {
	.name = "eth_stats",
	.type = &eth_stats_object_type,
	.methods = eth_stats_methods,
	.n_methods = ARRAY_SIZE(eth_stats_methods),
};

/* *
 * * @brief             load ethernet statistic arguement - clear_select from UCI file
 * * @param[*p]         buffer to be parse 
 * * @retval 0:         OK
 * * @retval -1:        ERROR
 * */
static int ethStatisticLoadClearSelect(struct uci_ptr *ptr)
{
    char *string = strdup(ptr->o->v.string);
    char *pstr;
    int clear_select;
    int port;

    while((pstr = strsep(&string," ")) != NULL)
    {
        if(strncmp(pstr,"port",strlen("port")))
        {
            LOG_ERROR("clear select format in UCI file error!\n"); 
            return -1;
        }

        port = atoi(pstr + 4);
        if(port < 1 || port > CONFIG_JWS_PORT_TOTAL_NR)
        {
            LOG_ERROR("clear select arguement-port index in UCI file error!\n"); 
            return -1;
        }

        char *pdelim = strchr(pstr,':');
        if(pdelim == NULL)
        {
            LOG_ERROR("clear select format in UCI file error!\n"); 
            return -1;
        }

        clear_select = atoi(pdelim + 1);
        if(clear_select != FAL_ENABLE && clear_select != FAL_DISABLE)
        {
            LOG_ERROR("clear select arguement is invalid!\n"); 
            return -1;
        }

        eth_statis[port-1].clear_f = clear_select;        
    }
    free(string);

    LOG_NORMAL("ethernet statistic get clear select arguement OK\n");

    return 0;
}

static UciConfig_S ethUci[] = {
    {.name = "clear_select",.type = UCI_TYPE_STRING,.ubusGet = ethStatisticLoadClearSelect},
};

/* *
 * * @brief             load ethernet statistic configure
 * * @retval 0:         OK
 * * @retval -1:        ERROR
 * */
static int ethStatisticCfgLoad(void)
{
    struct uci_context *ctx;

    ctx = uci_alloc_context();

    char name[64]; 
    int ret;
    struct uci_ptr ptr;

    sprintf(name,"switchd.ethernet_stats.%s",ethUci[0].name);
    ret = load_config(name,ethUci[0].type,&ptr,ctx);
    if(ret)
    {
        uci_free_context(ctx);

        if(ethUci[0].type == UCI_TYPE_LIST) return 0;

        LOG_ERROR("load config %s fail!\n",name); 
        return -1;
    }

    ret = ethUci[0].ubusGet(&ptr);
    if(ret)
    {
        uci_free_context(ctx);

        LOG_ERROR("ubus set arguement %d fail!\n",name); 
        return -1;
    }

    uci_free_context(ctx);

    return 0;
}


/* *
 * * @brief add object "eth_stats" to ubus
 * */
void ubus_init_eth_stats(struct ubus_context *ctx)
{
    int ret;

	_ctx = ctx;

    ret = ethStatisticCfgLoad();
    if(ret)
    {
        LOG_ERROR("Init ethernet statistic configure fail!\n"); 
        return;
    }

	ret = ubus_add_object(ctx, &eth_stats_object);
	if (ret)
		LOG_ERROR("Failed to add object [eth_stats]: %s\n", ubus_strerror(ret));
}
