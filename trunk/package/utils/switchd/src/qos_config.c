/* *< 中威文件编码格式为gb2312,cp936 */
/* * $Id$ $DateTime$
 * * @file qos_config.c
 * * @brief This is a table of the switch's qos configure.
 * * @version 0.0.1
 * * @since 0.0.1
 * * @author code<songyaofei@joyware.com>
 * * @date 2016-06-08
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
#include "fal/fal_qos.h"

static struct blob_buf b;
static struct ubus_context *_ctx;

enum {
    QOS_CFG_GET_KEY,
    __QOS_CFG_GET_MAX,
};

enum {
    QOS_CFG_SET_KEY,
    __QOS_CFG_SET_MAX,
};

static const struct blobmsg_policy qos_cfg_get_policy[] = {
    [QOS_CFG_GET_KEY] = {.name = "get_args", .type = BLOBMSG_TYPE_ARRAY},
};

static const struct blobmsg_policy qos_cfg_set_policy[] = {
    [QOS_CFG_SET_KEY] = {.name = "set_args", .type = BLOBMSG_TYPE_ARRAY},
};

enum {
    QOS_PRIORITY_ASSIGNMENT,
    QOS_PRIORITY_MAP_RULE,
    QOS_DEFAULT_PRIORITY_LEVEL,
    QOS_SCHEDULE_MODE,
    QOS_IP_PRIORITY_QUEUE_ID,
    QOS_TAG_PRIORITY_QUEUE_ID,
    __QOS_CFG_TBL_MAX,
};

#define IP_MAP_NUM  64
#define TAG_MAP_NUM 8

static struct{
    struct{
        fal_qos_assign_mode_t prio_assign;
        fal_qos_map_t map_rule;
        fal_qos_prio_t prio_level;
        fal_qos_schedule_mode_t schedule_mode;
    }portCfg[CONFIG_JWS_PORT_TOTAL_NR];
    fal_ip_map_t imap;
    int ip_flag;
    fal_tag_map_t tmap;
    int tag_flag;
}qosCfg;

/* *
 * * @brief         get qos port priority assignment mode
 * * @param[idx]    port index
 * * @retval 0:     OK
 * * @retval -1:    ERROR
 * */
static int qosGetPrioAssign(struct blob_buf *buf,int idx)
{
    char prio_assign_s[2];

    sprintf(prio_assign_s,"%d",qosCfg.portCfg[idx-1].prio_assign);
    LOG_NORMAL("Get port [%d] priority assignment mode = [%s] OK\n",idx,prio_assign_s);
    blobmsg_add_string(buf, "port_priority_assignment", prio_assign_s);

    return 0;
}

/* *
 * * @brief         set qos port priority assignment mode
 * * @param[idx]    port index
 * * @retval 0:     OK
 * * @retval -1:    ERROR
 * */
static int qosSetPrioAssign(int idx,void *p)
{
    fal_qos_assign_mode_t mode = atoi((char *)p);
    sw_error_t res;

    if(mode == qosCfg.portCfg[idx-1].prio_assign)
    {
        LOG_NORMAL("same qos config arguement\n");
        return 0;
    }
    else if(mode < FAL_QOS_USE_DEFAULT_ASSIGN_MODE || mode > FAL_QOS_USE_TAG_IP_ASSIGN_MODE)
    {
        LOG_ERROR("invalid arguement!\n");
        return -1;
    }
    
    res = fal_qos_port_prio_assign_mode_set(0,idx,mode);
    if(res)
    {
        LOG_ERROR("set qos port priority assignment mode fail!\n");
        return -1;
    }

    qosCfg.portCfg[idx-1].prio_assign = mode;

    char buf[128];
    struct uci_ptr ptr = {
        .package = "switchd",
        .section = "qos",
        .option = "prio_assign",
    };

    sprintf(buf,"port1:%d port2:%d port3:%d port4:%d port5:%d port6:%d port7:%d port8:%d port9:%d port10:%d",\
           qosCfg.portCfg[0].prio_assign,qosCfg.portCfg[1].prio_assign,qosCfg.portCfg[2].prio_assign,qosCfg.portCfg[3].prio_assign,\
           qosCfg.portCfg[4].prio_assign,qosCfg.portCfg[5].prio_assign,qosCfg.portCfg[6].prio_assign,qosCfg.portCfg[7].prio_assign,\
           qosCfg.portCfg[8].prio_assign,qosCfg.portCfg[9].prio_assign);
    ubus_collect_config(&ptr,buf,UCI_TYPE_STRING,DO_SET);

    /**<    map rule comes to force if assignment mode is tag-ip  */
    if(mode == FAL_QOS_USE_TAG_IP_ASSIGN_MODE)
    {
        res = fal_qos_port_prio_map_rule_set(0,idx,qosCfg.portCfg[idx-1].map_rule);
        if(res)
        {
            LOG_ERROR("set qos port [%d] priority map rule fail!\n",idx);
            return -1;
        }      
    }

    LOG_NORMAL("set port [%d] qos priority assignment mode = [%d] OK\n",idx,mode);
 
    return 0;
}

/* *
 * * @brief         get qos port priority map rule
 * * @param[idx]    port index
 * * @retval 0:     OK
 * * @retval -1:    ERROR
 * */
static int qosGetPrioMapRule(struct blob_buf *buf,int idx)
{
    char prio_map_rule_s[2];

    sprintf(prio_map_rule_s,"%d",qosCfg.portCfg[idx-1].map_rule);
    LOG_NORMAL("Get port [%d] priority map rule = [%s] OK\n",idx,prio_map_rule_s);
    blobmsg_add_string(buf, "port_priority_map_rule", prio_map_rule_s);

    return 0;
}

/* *
 * * @brief         set qos port priority map rule
 * * @param[idx]    port index
 * * @retval 0:     OK
 * * @retval -1:    ERROR
 * */
static int qosSetPrioMapRule(int idx,void *p)
{
    fal_qos_map_t map = atoi((char *)p);
    sw_error_t res;

    if(map == qosCfg.portCfg[idx-1].map_rule)
    {
        LOG_NORMAL("same qos config arguement\n");
        return 0;
    }
    else if(map != FAL_QOS_IP_MAPPING && map != FAL_QOS_TAG_MAPPING)
    {
        LOG_ERROR("invalid arguement!\n");
        return -1;
    }
    else if(qosCfg.portCfg[idx-1].prio_assign != FAL_QOS_USE_TAG_IP_ASSIGN_MODE)
    {
        LOG_ERROR("port [%d] priority assignment mode is not tag & ip,forbidden!\n",idx);
        return -1;
    }
    
    res = fal_qos_port_prio_map_rule_set(0,idx,map);
    if(res)
    {
        LOG_ERROR("set qos port priority map rule fail!\n");
        return -1;
    }

    qosCfg.portCfg[idx-1].map_rule= map;

    char buf[128];
    struct uci_ptr ptr = {
        .package = "switchd",
        .section = "qos",
        .option = "map_rule",
    };

    sprintf(buf,"port1:%d port2:%d port3:%d port4:%d port5:%d port6:%d port7:%d port8:%d port9:%d port10:%d",\
           qosCfg.portCfg[0].map_rule,qosCfg.portCfg[1].map_rule,qosCfg.portCfg[2].map_rule,qosCfg.portCfg[3].map_rule,\
           qosCfg.portCfg[4].map_rule,qosCfg.portCfg[5].map_rule,qosCfg.portCfg[6].map_rule,qosCfg.portCfg[7].map_rule,\
           qosCfg.portCfg[8].map_rule,qosCfg.portCfg[9].map_rule);
    ubus_collect_config(&ptr,buf,UCI_TYPE_STRING,DO_SET);

    LOG_NORMAL("set port [%d] qos priority map rule = [%d] OK\n",idx,map);

    return 0;
}

/* *
 * * @brief         get qos port default priority level
 * * @param[idx]    port index
 * * @retval 0:     OK
 * * @retval -1:    ERROR
 * */
static int qosGetDefaultPrioLevel(struct blob_buf *buf,int idx)
{
    char deflt_prio_level_s[2];

    sprintf(deflt_prio_level_s,"%d",qosCfg.portCfg[idx-1].prio_level);
    LOG_NORMAL("Get port [%d] default priority level = [%s] OK\n",idx,deflt_prio_level_s);
    blobmsg_add_string(buf,"port_default_priority_level",deflt_prio_level_s);

    return 0;
}

/* *
 * * @brief         set qos port default priority level
 * * @param[idx]    port index
 * * @retval 0:     OK
 * * @retval -1:    ERROR
 * */
static int qosSetDefaultPrioLevel(int idx,void *p)
{
    fal_qos_prio_t prio_lev = atoi((char *)p);
    sw_error_t res;

    if(prio_lev == qosCfg.portCfg[idx-1].prio_level)
    {
        LOG_NORMAL("same qos config arguement\n");
        return 0;
    }
    else if(prio_lev < FAL_QOS_PRIO_LOWEST || prio_lev > FAL_QOS_PRIO_HIGHEST)
    {
        LOG_ERROR("invalid arguement!\n");
        return -1;
    }
    
    res = fal_qos_port_default_prio_set(0,idx,prio_lev);
    if(res)
    {
        LOG_ERROR("set qos default port level fail!\n");
        return -1;
    }

    qosCfg.portCfg[idx-1].prio_level = prio_lev;

    char buf[128];
    struct uci_ptr ptr = {
        .package = "switchd",
        .section = "qos",
        .option = "default_level",
    };

    sprintf(buf,"port1:%d port2:%d port3:%d port4:%d port5:%d port6:%d port7:%d port8:%d port9:%d port10:%d",\
           qosCfg.portCfg[0].prio_level,qosCfg.portCfg[1].prio_level,qosCfg.portCfg[2].prio_level,qosCfg.portCfg[3].prio_level,\
           qosCfg.portCfg[4].prio_level,qosCfg.portCfg[5].prio_level,qosCfg.portCfg[6].prio_level,qosCfg.portCfg[7].prio_level,\
           qosCfg.portCfg[8].prio_level,qosCfg.portCfg[9].prio_level);
    ubus_collect_config(&ptr,buf,UCI_TYPE_STRING,DO_SET);

    LOG_NORMAL("set port [%d] qos default port level = [%d] OK\n",idx,prio_lev);

    return 0;
}

/* *
 * * @brief         get qos port schedule mode
 * * @param[idx]    port index
 * * @retval 0:     OK
 * * @retval -1:    ERROR
 * */
static int qosGetScheduleMode(struct blob_buf *buf,int idx)
{
    char schedule_mode_s[2];

    sprintf(schedule_mode_s,"%d",qosCfg.portCfg[idx-1].schedule_mode);
    LOG_NORMAL("Get port [%d] schedule mode = [%s] OK\n",idx,schedule_mode_s);
    blobmsg_add_string(buf,"port_schedule_mode",schedule_mode_s);

    return 0;
}

/* *
 * * @brief         set qos port schedule mode
 * * @param[idx]    port index
 * * @retval 0:     OK
 * * @retval -1:    ERROR
 * */
static int qosSetScheduleMode(int idx,void *p)
{
    fal_qos_schedule_mode_t schedule_mode = atoi((char *)p);
    sw_error_t res;

    if(schedule_mode == qosCfg.portCfg[idx-1].schedule_mode)
    {
        LOG_NORMAL("same qos config arguement\n");
        return 0;
    }
    else if(schedule_mode != FAL_QOS_SR_MODE && schedule_mode != FAL_QOS_WRR_MODE)
    {
        LOG_ERROR("invalid arguement!\n");
        return -1;
    }
    
    res = fal_qos_port_schedule_mode_set(0,idx,schedule_mode);
    if(res)
    {
        LOG_ERROR("set qos port schedule mode fail!\n");
        return -1;
    }

    qosCfg.portCfg[idx-1].schedule_mode = schedule_mode;

    char buf[128];
    struct uci_ptr ptr = {
        .package = "switchd",
        .section = "qos",
        .option = "schedule",
    };

    sprintf(buf,"port1:%d port2:%d port3:%d port4:%d port5:%d port6:%d port7:%d port8:%d port9:%d port10:%d",\
           qosCfg.portCfg[0].schedule_mode,qosCfg.portCfg[1].schedule_mode,qosCfg.portCfg[2].schedule_mode,qosCfg.portCfg[3].schedule_mode,\
           qosCfg.portCfg[4].schedule_mode,qosCfg.portCfg[5].schedule_mode,qosCfg.portCfg[6].schedule_mode,qosCfg.portCfg[7].schedule_mode,\
           qosCfg.portCfg[8].schedule_mode,qosCfg.portCfg[9].schedule_mode);
    ubus_collect_config(&ptr,buf,UCI_TYPE_STRING,DO_SET);

    LOG_NORMAL("set port [%d] qos port schedule mode = [%d] OK\n",idx,schedule_mode);

    return 0;
}

/* *
 * * @brief         get qos 8021.p priority queue id
 * * @param[idx]    8021.p index
 * * @retval 0:     OK
 * * @retval -1:    ERROR
 * */
static int qosGet8021pPrioQueueId(struct blob_buf *buf,int idx)
{
    char tag_map_s[2];

    sprintf(tag_map_s,"%d",qosCfg.tmap.tag_map[idx]);
    LOG_NORMAL("Get 8021p priority index [%d] queue id = [%s] OK\n",idx,tag_map_s);
    blobmsg_add_string(buf,"8021p_priority_queue_id",tag_map_s);

    return 0;
}

/* *
 * * @brief         set qos 8021.p priority queue id
 * * @param[idx]    8021.p index
 * * @retval 0:     OK
 * * @retval -1:    ERROR
 * */
static int qosSet8021pPrioQueueId(int idx,void *p)
{
    int queue_id = atoi((char *)p);

    if(queue_id == qosCfg.tmap.tag_map[idx])
    {
        LOG_NORMAL("same qos config arguement\n");
        return 0;
    }
    else if(queue_id < 0 || queue_id > 3)
    {
        LOG_ERROR("invalid arguement!\n");
        return -1;
    }
    
    qosCfg.tmap.tag_map[idx] = queue_id;
    qosCfg.tag_flag++;

    char buf[128];
    struct uci_ptr ptr = {
        .package = "switchd",
        .section = "qos",
        .option = "8021p_map",
    };

    sprintf(buf,"8021p0:%d 8021p1:%d 8021p2:%d 8021p3:%d 8021p4:%d 8021p5:%d 8021p6:%d 8021p7:%d",\
           qosCfg.tmap.tag_map[0],qosCfg.tmap.tag_map[1],qosCfg.tmap.tag_map[2],qosCfg.tmap.tag_map[3],\
           qosCfg.tmap.tag_map[4],qosCfg.tmap.tag_map[5],qosCfg.tmap.tag_map[6],qosCfg.tmap.tag_map[7]);
    ubus_collect_config(&ptr,buf,UCI_TYPE_STRING,DO_SET);

    LOG_NORMAL("set 802.1p priority index [%d] queue id = [%d] OK\n",idx,queue_id);

    return 0;
}

/* *
 * * @brief         get qos DSCP priority queue id
 * * @param[idx]    DSCP index
 * * @retval 0:     OK
 * * @retval -1:    ERROR
 * */
static int qosGetDscpPrioQueueId(struct blob_buf *buf,int idx)
{
    char ip_map_s[2];

    sprintf(ip_map_s,"%d",qosCfg.imap.ip_map[idx]);
    LOG_NORMAL("Get DSCP priority index [%d] queue id = [%s] OK\n",idx,ip_map_s);
    blobmsg_add_string(buf,"dscp_priority_queue_id",ip_map_s);

    return 0;
}

static const char *dscp_map[4][2] = {
    {"dscp_map0","dscp0:%d dscp1:%d dscp2:%d dscp3:%d dscp4:%d dscp5:%d dscp6:%d dscp7:%d \
dscp8:%d dscp9:%d dscp10:%d dscp11:%d dscp12:%d dscp13:%d dscp14:%d dscp15:%d"},
    {"dscp_map1","dscp16:%d dscp17:%d dscp18:%d dscp19:%d dscp20:%d dscp21:%d dscp22:%d dscp23:%d \
dscp24:%d dscp25:%d dscp26:%d dscp27:%d dscp28:%d dscp29:%d dscp30:%d dscp31:%d"},
    {"dscp_map2","dscp32:%d dscp33:%d dscp34:%d dscp35:%d dscp36:%d dscp37:%d dscp38:%d dscp39:%d \
dscp40:%d dscp41:%d dscp42:%d dscp43:%d dscp44:%d dscp45:%d dscp46:%d dscp47:%d"},
    {"dscp_map3","dscp48:%d dscp49:%d dscp50:%d dscp51:%d dscp52:%d dscp53:%d dscp54:%d dscp55:%d \
dscp56:%d dscp57:%d dscp58:%d dscp59:%d dscp60:%d dscp61:%d dscp62:%d dscp63:%d"}
};

/* *
 * * @brief         set qos DSCP priority queue id
 * * @param[idx]    DSCP index
 * * @retval 0:     OK
 * * @retval -1:    ERROR
 * */
static int qosSetDscpPrioQueueId(int idx,void *p)
{
    int queue_id = atoi((char *)p);

    if(queue_id == qosCfg.imap.ip_map[idx])
    {
        LOG_NORMAL("same qos config arguement\n");
        return 0;
    }
    else if(queue_id < 0 || queue_id > 3)
    {
        LOG_ERROR("invalid arguement!\n");
        return -1;
    }
    
    qosCfg.imap.ip_map[idx] = queue_id;
    qosCfg.ip_flag++;
    
    unsigned char row = idx/16;
    char buf[256];
    struct uci_ptr ptr = {
        .package = "switchd",
        .section = "qos",
        .option = dscp_map[row][0],
    };

    sprintf(buf,dscp_map[row][1],\
           qosCfg.imap.ip_map[16*row],qosCfg.imap.ip_map[16*row+1],qosCfg.imap.ip_map[16*row+2],qosCfg.imap.ip_map[16*row+3],\
           qosCfg.imap.ip_map[16*row+4],qosCfg.imap.ip_map[16*row+5],qosCfg.imap.ip_map[16*row+6],qosCfg.imap.ip_map[16*row+7],\
           qosCfg.imap.ip_map[16*row+8],qosCfg.imap.ip_map[16*row+9],qosCfg.imap.ip_map[16*row+10],qosCfg.imap.ip_map[16*row+11],\
           qosCfg.imap.ip_map[16*row+12],qosCfg.imap.ip_map[16*row+13],qosCfg.imap.ip_map[16*row+14],qosCfg.imap.ip_map[16*row+15]);
    ubus_collect_config(&ptr,buf,UCI_TYPE_STRING,DO_SET);

    LOG_NORMAL("set DSCP priority index [%d] queue id = [%d] OK\n",idx,queue_id);

    return 0;
}


static const struct jw_switch_policy qos_cfg_tbl[] = {
    [QOS_PRIORITY_ASSIGNMENT] = {.name = "port_priority_assignment", .get_handler = qosGetPrioAssign, .set_handler = qosSetPrioAssign}, 
    [QOS_PRIORITY_MAP_RULE] = {.name = "port_priority_map_rule", .get_handler = qosGetPrioMapRule, .set_handler = qosSetPrioMapRule},
    [QOS_DEFAULT_PRIORITY_LEVEL] = {.name = "port_default_priority_level", .get_handler = qosGetDefaultPrioLevel, .set_handler = qosSetDefaultPrioLevel}, 
    [QOS_SCHEDULE_MODE] = {.name = "port_schedule_mode", .get_handler = qosGetScheduleMode, .set_handler = qosSetScheduleMode},
    [QOS_TAG_PRIORITY_QUEUE_ID] = {.name = "8021p_priority_queue_id", .get_handler = qosGet8021pPrioQueueId, .set_handler = qosSet8021pPrioQueueId},
    [QOS_IP_PRIORITY_QUEUE_ID] = {.name = "dscp_priority_queue_id", .get_handler = qosGetDscpPrioQueueId, .set_handler = qosSetDscpPrioQueueId},
};

#define PORT_IDX    "port-"
#define TAG_IDX     "8021p-"
#define IP_IDX      "dscp-"

static int qosCheckKey(const char *key)
{
    int idx = -1;

    if(!strncmp(key,PORT_IDX,strlen(PORT_IDX)))
    {
        idx = atoi(key + strlen(PORT_IDX));            
        if(idx < 1 || idx > CONFIG_JWS_PORT_TOTAL_NR)
        {
            idx = -1;
        }
    }
    else if(!strncmp(key,IP_IDX,strlen(IP_IDX)))
    {
        idx = atoi(key + strlen(IP_IDX));            
        if(idx < 0 || idx >= IP_MAP_NUM)
        {
            idx = -1;
        }
    }
    else if(!strncmp(key,TAG_IDX,strlen(TAG_IDX)))
    {
        idx = atoi(key + strlen(TAG_IDX));            
        if(idx < 0 || idx >= TAG_MAP_NUM)
        {
            idx = -1;
        }
    }

    return idx;
}

/* *
 * * @brief         parse QOS configuration get
 * * @param[lvl3]   level 3 message structure.
 * * @retval 0:     OK
 * * @retval -1:    ERROR
 * */
static int qosParseCfgGet(struct blob_attr *lvl3, struct blob_buf *buf)
{
    const char *name    = blobmsg_name(lvl3);
    int idx             = qosCheckKey(name);

    if (idx < 0) 
    {
        LOG_ERROR("QOS Check Key error!\n");
        return -1;
    }
    
    void *ret_array;

    ret_array= blobmsg_open_array(buf, name);
    foreach_json_array_get(idx,lvl3,buf,qos_cfg_tbl, __QOS_CFG_TBL_MAX);
    blobmsg_close_array(buf, ret_array);	

    return 0;
}

/* *
 * * @brief         parse QOS configuration set
 * * @param[lvl3]   level 3 message structure.
 * * @retval 0:     OK
 * * @retval -1:    ERROR
 * */
static int qosParseCfgSet(struct blob_attr *lvl3)
{
    const char *name    = blobmsg_name(lvl3);
    int idx             = qosCheckKey(name);

    if (idx < 0) 
    {
        LOG_ERROR("QOS Check Key error!\n");
        return -1;
    }

    /**<    foreach array of level 3 message  */
    int ret;
    ret = foreach_json_array_set(idx,lvl3,qos_cfg_tbl, __QOS_CFG_TBL_MAX);
    if(ret)
    {
        LOG_ERROR("foreach index %d error!\n",idx);
        return -1;
    }

    return 0;
}


/* *
 * * @brief         method "qos_config_get" handler
 * * @retval 0:     OK
 * * @retval -1:    ERROR
 * */
static int qosCfgGetHandler(struct ubus_context *ctx, struct ubus_object *obj,
                struct ubus_request_data *req, const char *method,
                struct blob_attr *msg)
{
    struct blob_attr *tb[__QOS_CFG_GET_MAX];    
	struct blob_attr *lvl1 = NULL;  

    blobmsg_parse(qos_cfg_get_policy, __QOS_CFG_GET_MAX, tb, blob_data(msg), blob_len(msg));
	lvl1 = tb[QOS_CFG_GET_KEY];     
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
        if(blobmsg_type(lvl2) == BLOBMSG_TYPE_TABLE)
        {
            struct blob_attr *lvl3 = blobmsg_data(lvl2);

            if(blobmsg_type(lvl3) == BLOBMSG_TYPE_ARRAY)
            {
                ret_table = blobmsg_open_table(&b, NULL);
                qosParseCfgGet(lvl3,&b);
                blobmsg_close_table(&b, ret_table);	
            }
            else
            {
                LOG_ERROR("level 3 message type is not available!\n");
            }
        }
        else
        {
            LOG_ERROR("level 2 message type is not available!\n");
        }
    }
    
    blobmsg_close_array(&b, ret_array);	
    ubus_send_reply(ctx, req, b.head);

	return UBUS_STATUS_OK;

}

/* *
 * * @brief method "qos_config_set" handler
 * * @retval 0: OK
 * * @retval -1: ERROR
 * */
static int qosCfgSetHandler(struct ubus_context *ctx, struct ubus_object *obj,
                struct ubus_request_data *req, const char *method,
                struct blob_attr *msg)
{
    struct blob_attr *tb[__QOS_CFG_SET_MAX];    
	struct blob_attr *lvl1 = NULL;  
    int ret = -1;

    blobmsg_parse(qos_cfg_set_policy, __QOS_CFG_SET_MAX, tb, blob_data(msg), blob_len(msg));
	lvl1 = tb[QOS_CFG_SET_KEY];     
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
            
            if(blobmsg_type(lvl3) == BLOBMSG_TYPE_ARRAY)
            {
                ret = qosParseCfgSet(lvl3);
                if(ret)
                    goto end;
            }
            else
            {
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

    if(qosCfg.ip_flag)
    {
        ret = fal_qos_ip_prio_map_set(0,&qosCfg.imap);
        if(ret)
        {
            LOG_ERROR("set qos priority ip map fail!\n");
            goto end;
        }

        qosCfg.ip_flag = 0;
    }
    else if(qosCfg.tag_flag)
    {
        ret = fal_qos_tag_prio_map_set(0,&qosCfg.tmap);
        if(ret)
        {
            LOG_ERROR("set qos priority tag map fail!\n");
            goto end;
        }

        qosCfg.tag_flag = 0;
    }

end:
    blobmsg_add_string(&b, "ret", ret?"-1":"0");
	ubus_send_reply(ctx, req, b.head);

	return UBUS_STATUS_OK;

}


static const struct ubus_method qos_cfg_methods[] = {
	UBUS_METHOD("qos_config_get", qosCfgGetHandler, qos_cfg_get_policy),
	UBUS_METHOD("qos_config_set", qosCfgSetHandler, qos_cfg_set_policy),
};


static struct ubus_object_type qos_cfg_object_type =
	UBUS_OBJECT_TYPE("qos_config", qos_cfg_methods);


static struct ubus_object qos_cfg_object = {
	.name = "qos_config",
	.type = &qos_cfg_object_type,
	.methods = qos_cfg_methods,
	.n_methods = ARRAY_SIZE(qos_cfg_methods),
};

/* *
 * * @brief             load qos arguement - priority assignment from UCI file
 * * @param[*p]         buffer to be parse 
 * * @retval 0:         OK
 * * @retval -1:        ERROR
 * */
static int qosLoadPrioAssignMode(struct uci_ptr *ptr)
{
    char *string = strdup(ptr->o->v.string);
    char *pstr;
    int mode;
    int port;
    int ret;

    while((pstr = strsep(&string," ")) != NULL)
    {
        if(strncmp(pstr,"port",strlen("port")))
        {
            LOG_ERROR("qos priority assignemt format in UCI file error!\n"); 
            return -1;
        }

        port = atoi(pstr + 4);
        if(port < 1 || port > CONFIG_JWS_PORT_TOTAL_NR)
        {
            LOG_ERROR("qos priority assignemt arguement - port index in UCI file error!\n"); 
            return -1;
        }

        char *pdelim = strchr(pstr,':');
        if(pdelim == NULL)
        {
            LOG_ERROR("qos priority assignemt format in UCI file error!\n"); 
            return -1;
        }

        mode = atoi(pdelim + 1);
        if(mode < FAL_QOS_USE_DEFAULT_ASSIGN_MODE || mode > FAL_QOS_USE_TAG_IP_ASSIGN_MODE)
        {
            LOG_ERROR("invalid arguement!\n");
            return -1;
        }

        ret = fal_qos_port_prio_assign_mode_set(0,port,mode);
        if(ret)
        {
            LOG_ERROR("set qos port [%d] priority assignment mode fail!\n",port);
            return -1;
        }

        qosCfg.portCfg[port-1].prio_assign = mode;
    }
    free(string);

    LOG_NORMAL("qos load priority assignemt mode OK\n");
    
    return 0;
}

/* *
 * * @brief             load qos arguement - map rule from UCI file
 * * @param[*p]         buffer to be parse 
 * * @retval 0:         OK
 * * @retval -1:        ERROR
 * */
static int qosLoadMapRule(struct uci_ptr *ptr)
{
    char *string = strdup(ptr->o->v.string);
    char *pstr;
    int map;
    int port;
    int ret;

    while((pstr = strsep(&string," ")) != NULL)
    {
        if(strncmp(pstr,"port",strlen("port")))
        {
            LOG_ERROR("qos priority assignemt format in UCI file error!\n"); 
            return -1;
        }

        port = atoi(pstr + 4);
        if(port < 1 || port > CONFIG_JWS_PORT_TOTAL_NR)
        {
            LOG_ERROR("qos priority assignemt arguement - port index in UCI file error!\n"); 
            return -1;
        }

        char *pdelim = strchr(pstr,':');
        if(pdelim == NULL)
        {
            LOG_ERROR("qos priority assignemt format in UCI file error!\n"); 
            return -1;
        }

        map = atoi(pdelim + 1);
        if(map != FAL_QOS_IP_MAPPING && map != FAL_QOS_TAG_MAPPING)
        {
            LOG_ERROR("invalid arguement!\n");
            return -1;
        }
        
        if(qosCfg.portCfg[port-1].prio_assign == FAL_QOS_USE_TAG_IP_ASSIGN_MODE)
        {
            ret = fal_qos_port_prio_map_rule_set(0,port,map);
            if(ret)
            {
                LOG_ERROR("set qos port [%d] priority map rule fail!\n",port);
                return -1;
            }
        }

        qosCfg.portCfg[port-1].map_rule = map;
    }
    free(string);

    LOG_NORMAL("qos load map rule OK\n");
    
    return 0;
}

/* *
 * * @brief             load qos arguement - default priority level from UCI file
 * * @param[*p]         buffer to be parse 
 * * @retval 0:         OK
 * * @retval -1:        ERROR
 * */
static int qosLoadDefaultLevel(struct uci_ptr *ptr)
{
    char *string = strdup(ptr->o->v.string);
    char *pstr;
    int level;
    int port;
    int ret;

    while((pstr = strsep(&string," ")) != NULL)
    {
        if(strncmp(pstr,"port",strlen("port")))
        {
            LOG_ERROR("qos priority assignemt format in UCI file error!\n"); 
            return -1;
        }

        port = atoi(pstr + 4);
        if(port < 1 || port > CONFIG_JWS_PORT_TOTAL_NR)
        {
            LOG_ERROR("qos priority assignemt arguement - port index in UCI file error!\n"); 
            return -1;
        }

        char *pdelim = strchr(pstr,':');
        if(pdelim == NULL)
        {
            LOG_ERROR("qos priority assignemt format in UCI file error!\n"); 
            return -1;
        }

        level = atoi(pdelim + 1);
        if(level < FAL_QOS_PRIO_LOWEST || level > FAL_QOS_PRIO_HIGHEST)
        {
            LOG_ERROR("invalid arguement!\n");
            return -1;
        }
        
        ret = fal_qos_port_default_prio_set(0,port,level);
        if(ret)
        {
            LOG_ERROR("set qos port [%d] default level fail!\n",port);
            return -1;
        }

        qosCfg.portCfg[port-1].prio_level = level;
    }
    free(string);

    LOG_NORMAL("qos load default priority OK\n");
    
    return 0;
}

/* *
 * * @brief             load qos arguement - schedule mode from UCI file
 * * @param[*p]         buffer to be parse 
 * * @retval 0:         OK
 * * @retval -1:        ERROR
 * */
static int qosLoadSchedule(struct uci_ptr *ptr)
{
    char *string = strdup(ptr->o->v.string);
    char *pstr;
    int schedule_mode;
    int port;
    int ret;

    while((pstr = strsep(&string," ")) != NULL)
    {
        if(strncmp(pstr,"port",strlen("port")))
        {
            LOG_ERROR("qos schedule mode format in UCI file error!\n"); 
            return -1;
        }

        port = atoi(pstr + 4);
        if(port < 1 || port > CONFIG_JWS_PORT_TOTAL_NR)
        {
            LOG_ERROR("qos schedule mode arguement - port index in UCI file error!\n"); 
            return -1;
        }

        char *pdelim = strchr(pstr,':');
        if(pdelim == NULL)
        {
            LOG_ERROR("qos schedule mode format in UCI file error!\n"); 
            return -1;
        }

        schedule_mode = atoi(pdelim + 1);
        if(schedule_mode != FAL_QOS_SR_MODE && schedule_mode != FAL_QOS_WRR_MODE)
        {
            LOG_ERROR("invalid arguement!\n");
            return -1;
        }
        
        ret = fal_qos_port_schedule_mode_set(0,port,schedule_mode);
        if(ret)
        {
            LOG_ERROR("set qos port [%d] schedule mode fail!\n",port);
            return -1;
        }   

        qosCfg.portCfg[port-1].schedule_mode = schedule_mode;
    }
    free(string);

    LOG_NORMAL("qos load schedule mode OK\n");
    
    return 0;
}

/* *
 * * @brief             load qos arguement - 8021p map from UCI file
 * * @param[*p]         buffer to be parse 
 * * @retval 0:         OK
 * * @retval -1:        ERROR
 * */
static int qosLoad8021pMap(struct uci_ptr *ptr)
{
    char *string = strdup(ptr->o->v.string);
    char *pstr;
    int map;
    int index;
    int ret;

    while((pstr = strsep(&string," ")) != NULL)
    {
        if(strncmp(pstr,"8021p",strlen("8021p")))
        {
            LOG_ERROR("qos 8021p map format in UCI file error!\n"); 
            return -1;
        }

        index = atoi(pstr + strlen("8021p"));
        if(index < 0 || index>= TAG_MAP_NUM)
        {
            LOG_ERROR("qos 8021p map arguement - index in UCI file error!\n"); 
            return -1;
        }

        char *pdelim = strchr(pstr,':');
        if(pdelim == NULL)
        {
            LOG_ERROR("qos 8021p map format in UCI file error!\n"); 
            return -1;
        }

        map = atoi(pdelim + 1);
        
        qosCfg.tmap.tag_map[index] = map; 
    }
    free(string);

    ret = fal_qos_tag_prio_map_set(0,&qosCfg.tmap);
    if(ret)
    {
        LOG_ERROR("set qos 8021p map fail!\n");
        return -1;
    }
    qosCfg.tag_flag = 0;
    
    LOG_NORMAL("qos load 8021p map OK\n");
    
    return 0;
}

/* *
 * * @brief             load qos arguement - dscp map from UCI file
 * * @param[*p]         buffer to be parse 
 * * @retval 0:         OK
 * * @retval -1:        ERROR
 * */
static int qosLoadDscpMap(struct uci_ptr *ptr)
{
    char *string = strdup(ptr->o->v.string);
    char *pstr;
    int map;
    int index;
    int ret;

    while((pstr = strsep(&string," ")) != NULL)
    {
        if(strncmp(pstr,"dscp",strlen("dscp")))
        {
            LOG_ERROR("qos dscp map format in UCI file error!\n"); 
            return -1;
        }

        index = atoi(pstr + strlen("dscp"));
        if(index < 0 || index>= IP_MAP_NUM)
        {
            LOG_ERROR("qos dscp map arguement - index in UCI file error!\n"); 
            return -1;
        }

        char *pdelim = strchr(pstr,':');
        if(pdelim == NULL)
        {
            LOG_ERROR("qos dscp map format in UCI file error!\n"); 
            return -1;
        }

        map = atoi(pdelim + 1);
        
        qosCfg.imap.ip_map[index] = map; 
    }
    free(string);

    ret = fal_qos_ip_prio_map_set(0,&qosCfg.imap);
    if(ret)
    {
        LOG_ERROR("set qos priority ip map fail!\n");
        return -1;
    }
    qosCfg.ip_flag = 0;

    LOG_NORMAL("qos load dscp map OK\n");
    
    return 0;
}

static UciConfig_S qosUci[] = {
    {.name = "prio_assign",.ubusGet = qosLoadPrioAssignMode},
    {.name = "map_rule",.ubusGet = qosLoadMapRule},
    {.name = "default_level",.ubusGet = qosLoadDefaultLevel},
    {.name = "schedule",.ubusGet = qosLoadSchedule},
    {.name = "8021p_map",.ubusGet = qosLoad8021pMap},
    {.name = "dscp_map0",.ubusGet = qosLoadDscpMap},
    {.name = "dscp_map1",.ubusGet = qosLoadDscpMap},
    {.name = "dscp_map2",.ubusGet = qosLoadDscpMap},
    {.name = "dscp_map3",.ubusGet = qosLoadDscpMap},
};

/* *
 * * @brief             load qos configure
 * * @retval 0:         OK
 * * @retval -1:        ERROR
 * */
static int qosCfgLoad(void)
{
    struct uci_context *ctx;

    ctx = uci_alloc_context();

    char name[64]; 
    int ret;
    int i;
    struct uci_ptr ptr;

    for(i = 0;i < sizeof(qosUci)/sizeof(UciConfig_S);i++)
    {
        sprintf(name,"switchd.qos.%s",qosUci[i].name);
        ret = load_config(name,qosUci[i].type,&ptr,ctx);
        if(ret)
        {
            uci_free_context(ctx);

            if(qosUci[i].type == UCI_TYPE_LIST) continue;

            LOG_ERROR("load config %s fail!\n",name); 
            return -1;
        }
        
        ret = qosUci[i].ubusGet(&ptr);
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

void ubus_init_qos_config(struct ubus_context *ctx)
{
	int ret;

	_ctx = ctx;
    ret = qosCfgLoad();
    if(ret)
    {
		LOG_ERROR("Init QOS configuration fail!\n");
        return;
    }

	ret = ubus_add_object(ctx, &qos_cfg_object);
	if (ret)
		LOG_ERROR("Failed to add object qos config: %s\n", ubus_strerror(ret));
}
