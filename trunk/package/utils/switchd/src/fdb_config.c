/* *< 中威文件编码格式为gb2312,cp936 */
/* * $Id$ $DateTime$
 * * @file fdb_config.c
 * * @brief This is a table of the switch's fdb configure.
 * * @version 0.0.1
 * * @since 0.0.1
 * * @author code<songyaofei@joyware.com>
 * * @date 2016-07-1
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
    FDB_GET_KEY,
    __FDB_GET_MAX,
};

enum {
    FDB_SET_KEY,
    __FDB_SET_MAX
};

enum {
    FDB_CLEAR_KEY,
    __FDB_CLEAR_MAX
};

static const struct blobmsg_policy fdb_get_policy[] = {
    [FDB_GET_KEY] = {.name = "get_args", .type = BLOBMSG_TYPE_ARRAY},
};

static const struct blobmsg_policy fdb_set_policy[] = {
    [FDB_SET_KEY] = {.name = "set_args", .type = BLOBMSG_TYPE_ARRAY},
};

static const struct blobmsg_policy fdb_clear_policy[] = {
    [FDB_CLEAR_KEY] = {.name = "clear_args", .type = BLOBMSG_TYPE_ARRAY},
};

enum {
    FDB_AGING_TIME,
    FDB_PORT_STATUS,
    FDB_ENTRY_NUM,
    FDB_ENTRY_PORT,
    FDB_ENTRY_MAC,
    __FDB_TBL_MAX
};

#define FDB_MIN_AGING_TIME 10
#define FDB_MAX_AGING_TIME 3825

typedef struct{
    int idx;
    int flag;
    int port;
    fal_fdb_mac_t mac_addr;
    struct list_head node;
}fdbList_S;

static struct{
    int aging_time;
    int status[CONFIG_JWS_PORT_TOTAL_NR];
    int num;
    struct list_head entry;     // list for valid entrys
    struct list_head entry_v;   // list for invalid entrys
}fdb;

/* *
 * * @brief         get fdb aging time 
 * * @retval 0:     OK
 * * @retval -1:    ERROR
 * */
static int fdbGetAgingTime(struct blob_buf *buf)
{
    char age_s[2];

    sprintf(age_s,"%d",fdb.aging_time);
    blobmsg_add_string(buf, "aging_time", age_s);
    LOG_NORMAL("get fdb aging time = [%s] OK\n",age_s);

    return 0;
}

/* *
 * * @brief         set fdb aging time 
 * * @retval 0:     OK
 * * @retval -1:    ERROR
 * */
static int fdbSetAgingTime(void *p)
{
    int age = atoi((char *)p);
    sw_error_t ret;

    /**<    check arguement  */
    if(age == fdb.aging_time)
    {
        LOG_NORMAL("same arguement,do nothing\n");
        return 0;
    }
    if(age < FDB_MIN_AGING_TIME || age > FDB_MAX_AGING_TIME)
    {
        LOG_ERROR("invalid arguement!\n");
        return -1;
    }
    
    /**<    set fdb status  */
    ret = fal_mac_aging_timeout_set(0,age);
    if(ret)
    {
        LOG_ERROR("set fdb aging time fail!\n");
        return -1;
    }

    fdb.aging_time = age;

    char buf[8];
    struct uci_ptr ptr = {
        .package = "switchd",
        .section = "fdb",
        .option = "aging_time",
    };

    sprintf(buf,"%d",age);
    ubus_collect_config(&ptr,buf,UCI_TYPE_STRING,DO_SET);

    LOG_NORMAL("set fdb aging time = [%d] OK\n",age);
    return 0;
}

/* *
 * * @brief         get fdb entry number
 * * @retval 0:     OK
 * * @retval -1:    ERROR
 * * @note:         upper limit 999 temporary
 * */
static int fdbGetEntryNum(struct blob_buf *buf)
{
    char num_s[4];
    
    sprintf(num_s,"%d",fdb.num);
    blobmsg_add_string(buf, "entry_num", num_s);
    LOG_NORMAL("get fdb entry num = [%s] OK\n",num_s);

    return 0;
}

/* *
 * * @brief             get fdb port status 
 * * @param[port_idx]   port index.
 * * @retval 0:         OK
 * * @retval -1:        ERROR
 * */
static int fdbGetPortStatus(struct blob_buf *buf, int port_idx)
{
    char status_s[2];

    sprintf(status_s,"%d",fdb.status[port_idx-1]);
    blobmsg_add_string(buf, "status", status_s);
    LOG_NORMAL("get fdb port %d status OK\n",port_idx);

    return 0;
}

/* *
 * * @brief             set fdb port status
 * * @param[port_idx]   port index.
 * * @param[p]          FAL_ENABLE/FAL_DISABLE.
 * * @retval 0:         OK
 * * @retval -1:        ERROR
 * */
static int fdbSetPortStatus(int port_idx,void *p)
{
    int status = atoi((char *)p);

    if(status != FAL_ENABLE && status != FAL_DISABLE)
    {
        LOG_ERROR("invalid arguement!\n");
        return -1;
    }
    if(status == fdb.status[port_idx-1])
    {
        LOG_NORMAL("same arguement,do nothing\n");
        return 0;
    }

    int ret = fal_mac_secure_port_enable_set(0,port_idx,status);
    if(ret)
    {
        LOG_ERROR("set port %d status = %d fail!\n",port_idx,status); 
        return -1;
    }

    fdb.status[port_idx-1] = status;

    char buf[128];
    struct uci_ptr ptr = {
        .package = "switchd",
        .section = "fdb",
        .option = "port_status",
    };

    sprintf(buf,"port1:%d port2:%d port3:%d port4:%d port5:%d port6:%d port7:%d port8:%d port9:%d port10:%d",\
            fdb.status[0],fdb.status[1],fdb.status[2],fdb.status[3],fdb.status[4],\
            fdb.status[5],fdb.status[6],fdb.status[7],fdb.status[8],fdb.status[9]);
    ubus_collect_config(&ptr,buf,UCI_TYPE_STRING,DO_SET);

    /**<    do diffrent policy according to status  */
    if(status == FAL_ENABLE)
    {
        fdbList_S *fb,*fb_bak;
        list_for_each_entry_safe(fb, fb_bak, &fdb.entry_v, node)
        {
            if(port_idx != fb->port)
                continue;

            ret = fal_mac_row_create(0,fb->port,&fb->mac_addr);
            if(ret)
            {
                LOG_ERROR("fdb create new entry error!\n");
                return -1;
            }          

            list_move_tail(&fb->node,&fdb.entry);
            fb->idx = ++fdb.num;
        }
    }
    else
    {
        int cnt = 0;
        fdbList_S *fb,*fb_bak;
        list_for_each_entry_safe(fb, fb_bak, &fdb.entry, node)
        {
            if(port_idx != fb->port)
            {
                fb->idx -= cnt;
                continue;
            }

            cnt++;
            
            if(!IS_EMPTY_MAC(fb->mac_addr.mac) && fb->port > 0 && fb->port <= CONFIG_JWS_PORT_TOTAL_NR)
            {
                list_move_tail(&fb->node,&fdb.entry_v);
            }
            else
            {
                list_del(&fb->node);
                free(fb);
            }
        }

        fdb.num -= cnt;
    }

    LOG_NORMAL("set port [%d] status = [%d] OK\n",port_idx,status);

    return 0;
}

/* *
 * * @brief         get fdb entry port
 * * @param[idx]    entry index.
 * * @retval 0:     OK
 * * @retval -1:    ERROR
 * */
static int fdbGetEntryPort(struct blob_buf *buf, int idx)
{
    char port_s[2];

    fdbList_S *fb,*fb_bak;
    list_for_each_entry_safe(fb, fb_bak, &fdb.entry, node)
    {
        if(idx == fb->idx)
        {
            sprintf(port_s,"%d",fb->port);
            blobmsg_add_string(buf, "port", port_s);
            LOG_NORMAL("fdb get index %d port = [%s] OK\n",idx,port_s);
            
            return 0;
        }
    }
            
    blobmsg_add_string(buf, "port", "-1");
    LOG_ERROR("no match index [%d], something error!\n",idx);
    
    return -1;
}

/* *
 * * @brief         set fdb entry port 
 * * @param[idx]    entry index.
 * * @param[p]      port [1-10]    
 * * @retval 0:     OK
 * * @retval -1:    ERROR
 * */
static int fdbSetEntryPort(int idx,void *p)
{
    int port = atoi((char *)p);
    
    if(port < 1 || port > CONFIG_JWS_PORT_TOTAL_NR)
    {
        LOG_ERROR("invalid arguement!\n");
        return -1;
    }

    if(fdb.status[port-1] != FAL_ENABLE)
    {
        LOG_ERROR("the feature of port %d in fdb is disable!\n",port);
        return -1;
    }

    /**<    foreach every node  */
    fdbList_S *fb,*fb_bak;
    list_for_each_entry_safe(fb, fb_bak, &fdb.entry, node)
    {
        if(idx == fb->idx)
        {
            if(fb->port > 0 && fb->port <= CONFIG_JWS_PORT_TOTAL_NR)
            {
                LOG_ERROR("arguement is exist,can not be modified!\n");
                return -1;
            }        

            if(!IS_EMPTY_MAC(fb->mac_addr.mac)) 
            {
                fb->flag = 1;
            }

            fb->port = port;
            LOG_NORMAL("fdb set index [%d] port = %d OK!\n",idx,port);
            return 0;
        }
    }

    LOG_ERROR("no match index [%d], something error!\n",idx);
    return -1;
}


/* *
 * * @brief         get fdb entry mac 
 * * @param[idx]    entry index.
 * * @retval 0:     OK
 * * @retval -1:    ERROR
 * */
static int fdbGetEntryMac(struct blob_buf *buf, int idx)
{
    fdbList_S *fb,*fb_bak;
    list_for_each_entry_safe(fb, fb_bak, &fdb.entry, node)
    {
        if(idx == fb->idx)
        {
            char mac_s[18];

            sprintf(mac_s,"%02x-%02x-%02x-%02x-%02x-%02x",fb->mac_addr.mac[0],fb->mac_addr.mac[1],fb->mac_addr.mac[2],\
                    fb->mac_addr.mac[3],fb->mac_addr.mac[4],fb->mac_addr.mac[5]);
            blobmsg_add_string(buf, "mac_address", mac_s);
            LOG_NORMAL("fdb get index %d mac address = [%s] OK\n",idx,mac_s);
            
            return 0;
        }
    }

    blobmsg_add_string(buf,"mac_address","-1");
    LOG_ERROR("no match index [%d], something error!\n",idx);

    return -1;
}


/* *
 * * @brief             set fdb port 
 * * @param[idx]   entry index.
 * * @retval 0:         OK
 * * @retval -1:        ERROR
 * */
static int fdbSetEntryMac(int idx,void *p)
{
    fal_fdb_mac_t mac_new;
    sw_error_t ret;
    
    /**<    check arguement  */
    ret = parse_mac(mac_new.mac,(char *)p);
    if(ret)
    {
        LOG_ERROR("analyse fdb mac address fail!\n");
        return -1;
    }

    /**<    foreach every node  */
    fdbList_S *fb,*fb_bak;
    list_for_each_entry_safe(fb, fb_bak, &fdb.entry, node)
    {
        if(idx == fb->idx)
        {
            if(!IS_EMPTY_MAC(fb->mac_addr.mac))
            {
                LOG_ERROR("arguement is exist,can not be modified!\n");
                break;
            }

            /**<    set node flag = 1 if all of this node's arguement are set  */
            if(fb->port > 0 && fb->port <= CONFIG_JWS_PORT_TOTAL_NR)
            {
                fb->flag = 1;
            }
            
            memcpy(&fb->mac_addr,&mac_new,sizeof(fal_fdb_mac_t));

            LOG_NORMAL("fdb set index [%d] mac address OK!\n",idx);
            return 0;
        }
    }

    LOG_ERROR("something error while foreach index %d!\n",idx);
    return -1;
}


static const struct jw_switch_policy fdb_tbl[] = {
    [FDB_AGING_TIME] = {.name = "aging_time", .get_ext_handler = fdbGetAgingTime, .set_ext_handler = fdbSetAgingTime}, 
    [FDB_PORT_STATUS] = {.name = "status", .get_handler = fdbGetPortStatus, .set_handler = fdbSetPortStatus}, 
    [FDB_ENTRY_NUM] = {.name = "entry_num", .get_ext_handler = fdbGetEntryNum}, 
    [FDB_ENTRY_PORT] = {.name = "port", .get_handler = fdbGetEntryPort, .set_handler = fdbSetEntryPort}, 
    [FDB_ENTRY_MAC] = {.name = "mac_address", .get_handler = fdbGetEntryMac, .set_handler = fdbSetEntryMac}, 
};

static int fdbCheckKey(const char *key,int action)
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
    else if(!strncmp(key,ENTRY_IDX,strlen(ENTRY_IDX)))
    {
        idx = atoi(key + strlen(ENTRY_IDX));            
        if(idx < 1 || idx > (fdb.num + action))
        {
            idx = -1;
        }
    }

    return idx;
}


/* *
 * * @brief         fdb config parse recv form upper
 * * @param[lvl3]   level 3 message
 * * @retval 0:     OK
 * * @retval -1:    ERROR
 * */
static int fdbParseGet(struct blob_attr *lvl3, struct blob_buf *buf)
{
    const char *name    = blobmsg_name(lvl3);
    int idx             = fdbCheckKey(name,0);

    if (idx < 0) 
    {
        LOG_ERROR("fdb Check Key error!\n");
        return -1;
    }

    void *ret_array;

    ret_array= blobmsg_open_array(buf, name);
    foreach_json_array_get(idx,lvl3,buf,fdb_tbl,__FDB_TBL_MAX);
    blobmsg_close_array(buf, ret_array);	
        
    return 0; 
}

/* *
 * * @brief         fdb config parse recv form upper
 * * @param[lvl3]   level 3 message
 * * @retval 0:     OK
 * * @retval -1:    ERROR
 * */
static int fdbParseSet(struct blob_attr *lvl3)
{
    const char *name    = blobmsg_name(lvl3);
    int idx             = fdbCheckKey(name,1);

    if (idx < 0) 
    {
        LOG_ERROR("fdb Check Key error!\n");
        return -1;
    }

    /**<    add new index/entry  */
    if(!strncmp(name,ENTRY_IDX,strlen(ENTRY_IDX)) && idx == (fdb.num + 1))
    {
        fdb.num++;
        fdbList_S *fb = (fdbList_S *)calloc(1,sizeof(fdbList_S));
        list_add_tail(&fb->node,&fdb.entry);
        fb->idx = fdb.num;
    }

    /**<    foreach array of level 3 message  */
    int ret;
    ret = foreach_json_array_set(idx,lvl3,fdb_tbl,__FDB_TBL_MAX);
    if(ret)
    {
        LOG_ERROR("foreach index %d error!\n",idx);
        return -1;
    }

    /**<    create each fdb row which has been changed  */
    fdbList_S *fb,*fb_bak;
    list_for_each_entry_safe(fb, fb_bak, &fdb.entry, node)
    {
        if(fb->flag)
        {
            ret = fal_mac_row_create(0,fb->port,&fb->mac_addr);
            if(ret)
            {
                /**<    add free memory later  */
                LOG_ERROR("fdb set new entry error!\n");
                return -1;
            }
            fb->flag = 0;

            char buf[32];
            struct uci_ptr ptr = {
                .package = "switchd",
                .section = "fdb",
                .option = "entry",
            };

            sprintf(buf,"port%d:%02x-%02x-%02x-%02x-%02x-%02x",fb->port,\
                    fb->mac_addr.mac[0],fb->mac_addr.mac[1],fb->mac_addr.mac[2],\
                    fb->mac_addr.mac[3],fb->mac_addr.mac[4],fb->mac_addr.mac[5]);

            ubus_collect_config(&ptr,buf,UCI_TYPE_LIST,DO_SET);
        }
    }

    return 0; 
}

/* *
 * * @brief         parse fdb config clear operation from upper
 * * @param[lvl2]   level 2 message
 * * @retval 0:     OK
 * * @retval -1:    ERROR
 * */
static int fdbParseClear(struct blob_attr *lvl2)
{
    char *value = blobmsg_get_string(lvl2);
    int idx     = atoi(value + 4);
    int res;
    int flag = 0;

    if (idx < 1 || idx > fdb.num) 
    {
        LOG_ERROR("invalid index %d\n", idx);
        return -1;
    }
    
    fdbList_S *fb,*fb_bak;
    list_for_each_entry_safe(fb, fb_bak, &fdb.entry, node)
    {
        if(flag)
        {
            fb->idx--;
        }
        else if(idx == fb->idx)
        {
            /**<    delete specified entry  */
            fal_fdb_mac_t *mac_addr = &fb->mac_addr;
            fal_port_t port_id = fb->port;

            if(!IS_EMPTY_MAC(mac_addr->mac) && (fb->port > 0 && fb->port <= CONFIG_JWS_PORT_TOTAL_NR))
            {
                res = fal_mac_row_destroy(0, mac_addr, port_id);
                if(res)
                {
                    LOG_ERROR("destroy one fdb entry which index = [%d] fail!\n", idx);
                    return -1;
                }

                /**<    delete specified entry in UCI file  */
                char buf[32];
                struct uci_ptr ptr = {
                    .package = "switchd",
                    .section = "fdb",
                    .option = "entry",
                };

                sprintf(buf,"port%d:%02x-%02x-%02x-%02x-%02x-%02x",fb->port,\
                        fb->mac_addr.mac[0],fb->mac_addr.mac[1],fb->mac_addr.mac[2],\
                        fb->mac_addr.mac[3],fb->mac_addr.mac[4],fb->mac_addr.mac[5]);
                ubus_collect_config(&ptr,buf,UCI_TYPE_LIST,DO_DEL);
            }
            
            flag = 1;
            fdb.num--;
            list_del(&fb->node);
            free(fb);
        }
    }

    if(flag)
    {
        LOG_NORMAL("destroy one fdb entry which index = %d OK\n",idx);
        return 0;
    }

    LOG_ERROR("no match index [%d], something error!\n",idx);
    return -1;
}


/* *
 * * @brief         method "fdb_get" handler
 * * @retval 0:     OK
 * * @retval -1:    ERROR
 * */
static int fdbGetHandler(struct ubus_context *ctx, struct ubus_object *obj,
                struct ubus_request_data *req, const char *method,
                struct blob_attr *msg)
{
    struct blob_attr *tb[__FDB_GET_MAX];    
	struct blob_attr *lvl1 = NULL;  

    blobmsg_parse(fdb_get_policy, __FDB_GET_MAX, tb, blob_data(msg), blob_len(msg));
	lvl1 = tb[FDB_GET_KEY];     
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

                policy_p = (struct jw_switch_policy *)jw_switchd_get_context(name, fdb_tbl, __FDB_TBL_MAX);
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
                    fdbParseGet(lvl3,&b);
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
 * * @brief method "fdb_set" handler
 * * @retval 0: OK
 * * @retval -1: ERROR
 * */
static int fdbSetHandler(struct ubus_context *ctx, struct ubus_object *obj,
                struct ubus_request_data *req, const char *method,
                struct blob_attr *msg)
{
    struct blob_attr *tb[__FDB_SET_MAX];    
	struct blob_attr *lvl1 = NULL;  
    int ret = -1;

    blobmsg_parse(fdb_set_policy, __FDB_SET_MAX, tb, blob_data(msg), blob_len(msg));
	lvl1 = tb[FDB_SET_KEY];     
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

                    policy_p = (struct jw_switch_policy *)jw_switchd_get_context(name, fdb_tbl, __FDB_TBL_MAX);
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
                    ret = fdbParseSet(lvl3);
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

/* *
 * * @fn fdbClearHandler
 * * @brief method "port_fdb_clear" handler
 * * @retval 0: OK
 * * @retval -1: ERROR
 * */
static int fdbClearHandler(struct ubus_context *ctx, struct ubus_object *obj,
                struct ubus_request_data *req, const char *method,
                struct blob_attr *msg)
{
    struct blob_attr *tb[__FDB_CLEAR_MAX];    
	struct blob_attr *lvl1 = NULL;  
    int ret = -1;

    blobmsg_parse(fdb_clear_policy, __FDB_CLEAR_MAX, tb, blob_data(msg), blob_len(msg));
	lvl1 = tb[FDB_CLEAR_KEY];     
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
        if (blobmsg_type(lvl2) == BLOBMSG_TYPE_STRING) 
        {
                        
            ret = fdbParseClear(lvl2);
        }
        else
        {
            LOG_ERROR("level 2 message type is not available!\n");
        }
    }

end:
    blobmsg_add_string(&b, "ret", ret?"-1":"0");
	ubus_send_reply(ctx, req, b.head);

	return UBUS_STATUS_OK;
}


static const struct ubus_method fdb_methods[] = {
	UBUS_METHOD("fdb_get", fdbGetHandler, fdb_get_policy),
	UBUS_METHOD("fdb_set", fdbSetHandler, fdb_set_policy),
	UBUS_METHOD("fdb_clear", fdbClearHandler, fdb_clear_policy),
};

static struct ubus_object_type fdb_object_type =
	UBUS_OBJECT_TYPE("fdb", fdb_methods);

static struct ubus_object fdb_object = {
	.name = "fdb",
	.type = &fdb_object_type,
	.methods = fdb_methods,
	.n_methods = ARRAY_SIZE(fdb_methods),
};

/* *
 * * @brief             load fdb arguement - aging time from UCI file
 * * @param[*p]         buffer to be parse 
 * * @retval 0:         OK
 * * @retval -1:        ERROR
 * */
static int fdbLoadAgingTime(struct uci_ptr *ptr)
{
    int age = atoi(ptr->o->v.string);
    int ret;

    if(age < FDB_MIN_AGING_TIME || age > FDB_MAX_AGING_TIME)
    {
        LOG_ERROR("invalid arguement!\n"); 
        return -1;
    }

    ret = fal_mac_aging_timeout_set(0,age);
    if(ret)
    {
        LOG_ERROR("set fdb aging time fail!\n");
        return -1;
    }

    fdb.aging_time = age;
    LOG_NORMAL("fdb load aging time = [%d] OK\n", age);
    
    return 0;
}

/* *
 * * @brief             load fdb arguement - port status from UCI file
 * * @param[*p]         buffer to be parse 
 * * @retval 0:         OK
 * * @retval -1:        ERROR
 * */
static int fdbLoadPortStatus(struct uci_ptr *ptr)
{
    char *string = strdup(ptr->o->v.string);
    char *pstr;

    while((pstr = strsep(&string," ")) != NULL)
    {
        if(strncmp(pstr,"port",strlen("port")))
        {
            LOG_ERROR("port status format in UCI file error!\n"); 
            return -1;
        }

        int port = atoi(pstr + 4);
        if(port < 1 || port > CONFIG_JWS_PORT_TOTAL_NR)
        {
            LOG_ERROR("port status arguement-port index in UCI file error!\n"); 
            return -1;
        }

        char *pdelim = strchr(pstr,':');
        if(pdelim == NULL)
        {
            LOG_ERROR("port status format in UCI file error!\n"); 
            return -1;
        }

        int status = atoi(pdelim + 1);
        if(status != FAL_ENABLE && status != FAL_DISABLE)
        {
            LOG_ERROR("port status arguement is invalid!\n"); 
            return -1;
        }
        
        int ret = fal_mac_secure_port_enable_set(0,port,status);
        if(ret)
        {
            LOG_ERROR("set port %d status = %d fail!\n",port,status); 
            return -1;
        }

        fdb.status[port-1] = status;        
    }
    free(string);

    LOG_NORMAL("fdb load port status OK\n");

    return 0;
}

/* *
 * * @brief             load fdb arguement - entry list from UCI file
 * * @param[*p]         buffer to be parse 
 * * @retval 0:         OK
 * * @retval -1:        ERROR
 * */
static int fdbLoadEntry(struct uci_ptr *ptr)
{
    int ret;
    int count = 0;

    struct uci_element *elt;

    uci_foreach_element(&ptr->o->v.list,elt)
    {
        /**<    separate port index and mac  */
        char *pdelim = strchr(elt->name,':');
        if(pdelim == NULL)
        {
            LOG_ERROR("fdb entry list format in UCI file error!\n"); 
            return -1;
        }

        *pdelim++ = 0;

        /**<    check if port index and mac are valid  */
        if(strncmp(elt->name,"port",strlen("port")))
        {
            LOG_ERROR("fdb entry format in UCI file error!\n"); 
            return -1;
        }

        int port = atoi(elt->name + 4);
        if(port < 1 || port > CONFIG_JWS_PORT_TOTAL_NR)
        {
            LOG_ERROR("fdb entry arguement-port index in UCI file error!\n"); 
            return -1;
        }

        fal_fdb_mac_t mac_addr;
        ret = parse_mac(mac_addr.mac,pdelim);
        if(ret)
        {
            LOG_ERROR("parse fdb entry mac format in UCI file error!\n"); 
            return -1;
        }

        /**<    set entry to driver  */
        if(fdb.status[port-1] == FAL_ENABLE)
        {
            ret = fal_mac_row_create(0,port,&mac_addr);
            if(ret)
            {
                LOG_ERROR("set new entry error!\n");
                return -1;
            }
            LOG_NORMAL("fdb creat entry :port = %d,mac_addr = %s OK\n",port,pdelim);
        }

        /**<    link entry to list  */
        fdbList_S *fb = (fdbList_S *)calloc(1,sizeof(fdbList_S));
        if(fdb.status[port-1] == FAL_ENABLE)
        {
            list_add_tail(&fb->node,&fdb.entry);
            fb->idx = ++count;
        }
        else
        {
            list_add_tail(&fb->node,&fdb.entry_v);
        }

        memcpy(&fb->mac_addr,&mac_addr,sizeof(fal_fdb_mac_t));
        fb->port = port;
    }

    fdb.num = count;

    LOG_NORMAL("fdb load entry OK\n");
    
    return 0;
}

static UciConfig_S fdbUci[] = {
    {.name = "aging_time",  .type = UCI_TYPE_STRING,    .ubusGet = fdbLoadAgingTime},
    {.name = "port_status", .type = UCI_TYPE_STRING,    .ubusGet = fdbLoadPortStatus},
    {.name = "entry",       .type = UCI_TYPE_LIST,      .ubusGet = fdbLoadEntry},
};

static int fdbCfgLoad(void)
{
    memset(&fdb,0,sizeof(fdb));
    INIT_LIST_HEAD(&fdb.entry);
    INIT_LIST_HEAD(&fdb.entry_v);

    struct uci_context *ctx = uci_alloc_context();
    
    char name[64]; 
    int ret;
    int i;
    struct uci_ptr ptr;

    for(i = 0;i < sizeof(fdbUci)/sizeof(UciConfig_S);i++)
    {
        sprintf(name,"switchd.fdb.%s",fdbUci[i].name);
        memset(&ptr, 0, sizeof(struct uci_ptr));
        ret = load_config(name,fdbUci[i].type,&ptr,ctx);
        if(ret)
        {
            if(fdbUci[i].type == UCI_TYPE_LIST) continue;

            LOG_ERROR("load config %s fail!\n",name); 
            uci_free_context(ctx);
            return -1;
        }
        
        ret = fdbUci[i].ubusGet(&ptr);
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

void ubus_init_fdb(struct ubus_context *ctx)
{
	int ret;

	_ctx = ctx;

    ret = fdbCfgLoad();
    if(ret)
    {
		LOG_ERROR("fdb load configure from UCI file fail\n");
        return;
    }

	ret = ubus_add_object(ctx, &fdb_object);
    
	if (ret)
    {
		LOG_ERROR("Failed to add object [fdb]: %s\n", ubus_strerror(ret));
    }
}

extern sw_error_t fdb_mac_row_create(fal_port_t port_id, a_uint8_t * mac_addr);

int ubus_poll_fdb_mac_create(void)
{
	int ret;
	fdbList_S *fb,*fb_bak;
	
	list_for_each_entry_safe(fb, fb_bak, &fdb.entry, node)
	{
        
	/*	LOG_NORMAL("###########switchd mac (%02x:%02x:%02x:%02x:%02x:%02x)########### \r\n",
			fb->mac_addr.mac[0],fb->mac_addr.mac[1],fb->mac_addr.mac[2],fb->mac_addr.mac[3],fb->mac_addr.mac[4],fb->mac_addr.mac[5]);*/
		ret = fdb_mac_row_create(fb->port, fb->mac_addr.mac);
		if(ret)
        {
            LOG_ERROR("ubus poll fdb_mac_create error!\n");
            return -1;
        }
	}

    return 0;
}

int ubus_is_fdb_port(int lport)
{
    return (fdb.status[lport - 1] == FAL_ENABLE);
}

