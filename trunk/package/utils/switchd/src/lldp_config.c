/* *< 中威文件编码格式为gb2312,cp936 */
/* * $Id$ $DateTime$
 * * @file lldp_config.c
 * * @brief This is a table of the switch's lldp configuration.
 * * @version 0.0.1
 * * @since 0.0.1
 * * @author code<songyaofei@joyware.com>
 * * @date 2016-06-12
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
    LLDP_CFG_GET_KEY,
    __LLDP_CFG_GET_MAX,
};

enum {
    LLDP_CFG_SET_KEY,
    __LLDP_CFG_SET_MAX,
};

static const struct blobmsg_policy lldp_cfg_get_policy[] = {
    [LLDP_CFG_GET_KEY] = {.name = "get_args", .type = BLOBMSG_TYPE_ARRAY},
};

static const struct blobmsg_policy lldp_cfg_set_policy[] = {
    [LLDP_CFG_SET_KEY] = {.name = "set_args", .type = BLOBMSG_TYPE_ARRAY},
};

enum {
    LLDP_ADMIN_STATUS,
    LLDP_ENTRY_NUM,
    LLDP_LOCAL_PORT_NUM,
    LLDP_REMOTE_PORT_NUM,
    LLDP_REMOTE_IP,
    LLDP_REMOTE_MAC,
    LLDP_REMOTE_SYS_NAME,
    LLDP_REMOTE_SYS_DESCP,
    __LLDP_CFG_TBL_MAX,
};

static struct{
    int status;
    int entry_num;
    struct list_head neighbor;  
}lldpCfg;

typedef struct{
    int idx;
    char local_port[4];
    char remote_port_descp[64];
    char remote_ip[16];
    char remote_mac[20];
    char remote_sysname[64];
    char remote_sysdescp[512];
    struct list_head node;
}neighbor_S;


#define LLDP_ST_OPTION_NAME "configure lldp status"
#define LLDP_ST_OPTION_VALUE1 "rx-and-tx"
#define LLDP_ST_OPTION_VALUE2 "disable"
/* *
 * * @brief         commit lldp config
 * * @retval 0:     OK
 * * @retval -1:    ERROR
 * */
static int lldpCommitConfig(void)
{
    FILE *file_old,*file_new;
    char buf[64];

    file_old = fopen("/etc/lldpd.conf","r");
    file_new = fopen("/etc/lldpd.conf.bak","w+");
    if(file_old == NULL || file_new == NULL)
    {
        LOG_ERROR("open file :/etc/lldpd.conf or /etc/lldpd.conf.bak fail!\n");
        return -1;
    }
    while(fgets(buf,sizeof(buf),file_old) != NULL)
    {
        if(strstr(buf,LLDP_ST_OPTION_NAME) != NULL)
        {
            fprintf(file_new,"%s %s\n",LLDP_ST_OPTION_NAME,(lldpCfg.status == FAL_ENABLE)?LLDP_ST_OPTION_VALUE1:LLDP_ST_OPTION_VALUE2);           
            continue;
        }
        fputs(buf,file_new);
    }

    fclose(file_old);
    fclose(file_new);

    if(remove("/etc/lldpd.conf"))
    {
        LOG_ERROR("remove /etc/lldpd.conf fail!\n");
        return -1;
    }
    if(rename("/etc/lldpd.conf.bak","/etc/lldpd.conf"))
    {
        LOG_ERROR("rename /etc/lldpd.conf fail!\n");
        return -1;
    }

    LOG_NORMAL("lldp save arguements OK\n");
    return 0;
}

/* *
 * * @brief         get lldp admin status
 * * @retval 0:     OK
 * * @retval -1:    ERROR
 * */
static int lldpGetAdminStatus(struct blob_buf *buf)
{
    char admin_st_s[2];

    sprintf(admin_st_s,"%d",lldpCfg.status);
    blobmsg_add_string(buf, "admin_status", admin_st_s);
    LOG_NORMAL("get lldp admin status = [%s] OK\n",admin_st_s);

    return 0;
}

/* *
 * * @brief         set lldp admin status
 * * @retval 0:     OK
 * * @retval -1:    ERROR
 * */
static int lldpSetAdminStatus(void *p)
{
    int admin_st = atoi((char *)p);

    if(admin_st == lldpCfg.status)
    {
        LOG_NORMAL("same arguement,do nothing\n");
        return 0;
    }
    
    if(admin_st != FAL_ENABLE && admin_st != FAL_DISABLE)
    {
        LOG_ERROR("invalid arguement!\n");
        return -1;
    }
    
    lldpCfg.status = admin_st;

    int ret;
    ret = lldpCommitConfig();
    if(ret)
    {
        LOG_ERROR("commit /etc/lldpd.conf fail!\n");
        return -1;
    }
/*    
    //重启lldpd服务器
    system("/etc/init.d/lldpd restart");
*/
    if(admin_st == FAL_ENABLE){
        system("/etc/init.d/lldpd stop");   
        system("/etc/init.d/lldpd start");
    }
    else if(admin_st == FAL_DISABLE){
        system("/etc/init.d/lldpd stop");   
    }

    LOG_NORMAL("set lldp admin status = [%d] OK\n",admin_st);
    
    return 0;
}

static int lldpParseJsonInfo(struct json_object *object)
{
    json_type type = json_object_get_type(object);
    if(type == json_type_object)
    {
        json_object_object_foreach(object,key1,value1)
        {
            if(!strcmp(key1,"lldp") && json_object_get_type(value1) == json_type_object)
            {
                json_object_object_foreach(value1,key2,value2)
                {
                    if(!strcmp(key2,"interface"))
                    {
                        struct json_object *value3;
                        int i = 0;

                        type = json_object_get_type(value2);
                        if(type == json_type_array)
                        {
                            for(;i < json_object_array_length(value2);i++)
                            {
                                value3 = json_object_array_get_idx(value2,i); 
                                if(json_object_get_type(value3) == json_type_object)
                                {
find_port:
                                    {
                                        json_object_object_foreach(value3,key4,value4)
                                        {
                                            if((!strncmp(key4,"Fex-",strlen("Fex-")) || !strncmp(key4,"Gex-",strlen("Gex-")))\
                                               && json_object_get_type(value4) == json_type_object)
                                            {
                                                neighbor_S *ng = (neighbor_S *)calloc(1,sizeof(neighbor_S));
                                                list_add_tail(&ng->node,&lldpCfg.neighbor);

                                                strcpy(ng->local_port,key4 + 4);
                                                LOG_NORMAL("finaly result: Key = local_port, value = %s\n",key4);

                                                struct json_object *obj;
                                                const char *value;
                                                json_object_object_foreach(value4,key5,value5)
                                                {
                                                    if(!strcmp(key5,"port") && json_object_get_type(value5) == json_type_object)
                                                    {
                                                        obj = json_search_element(value5,"descr");
                                                        if(obj == NULL)
                                                        {
                                                            strcpy(ng->remote_port_descp,"none");
                                                            LOG_WARN("this neighbor does not have port description\n");
                                                        }
                                                        else
                                                        {
                                                            value = json_object_get_string(obj);
                                                            strcpy(ng->remote_port_descp,value);
                                                            LOG_NORMAL("finaly result: key = remote_port_description, value = %s\n",value);
                                                        }
#if 0
                                                        json_object_object_foreach(value5,key6,value6)
                                                        {
                                                            if(!strcmp(key6,"descr") && json_object_get_type(value6) == json_type_string)
                                                            {
                                                                const char *value7 = json_object_get_string(value6);
                                                                strcpy(ng->remote_port_descp,value7);
                                                                LOG_NORMAL("finaly result: key = remote_port_descp, value = %s\n",value7);
                                                            }
                                                        }
#endif
                                                    }
                                                    else if(!strcmp(key5,"chassis") && json_object_get_type(value5) == json_type_object)
                                                    {
                                                        json_object_object_foreach(value5,key6,value6)
                                                        {
                                                            if(!strcmp(key6,"id"))
                                                            {
                                                                strcpy(ng->remote_sysname,"none");
                                                                LOG_WARN("this neighbor does not have system name\n");
                                                            }
                                                            else
                                                            {
                                                                strcpy(ng->remote_sysname,key6);
                                                                LOG_NORMAL("finaly result: key = remote_sys_name, value = %s\n",key6);
                                                            }

                                                            obj = json_search_element(value6,"value");
                                                            value = json_object_get_string(obj);
                                                            strcpy(ng->remote_mac,value);
                                                            LOG_NORMAL("finaly result: key = remote_mac, value = %s\n",value);

                                                            obj = json_search_element(value6,"mgmt-ip");
                                                            if(obj == NULL)
                                                            {
                                                                strcpy(ng->remote_ip,"0");
                                                                LOG_WARN("this neighbor does not have ip\n");
                                                            }
                                                            else
                                                            {
                                                                if(json_object_get_type(obj) == json_type_array)
                                                                {
                                                                    value = json_object_get_string(json_object_array_get_idx(obj,0));
                                                                    strcpy(ng->remote_ip,value);
                                                                    LOG_NORMAL("finaly result: key = remote_ip, value = %s\n",value);
                                                                }
                                                                else
                                                                {
                                                                    value = json_object_get_string(obj);
                                                                    strcpy(ng->remote_ip,value);
                                                                    LOG_NORMAL("finaly result: key = remote_ip, value = %s\n",value);
                                                                }
                                                            }

                                                            obj = json_search_element(value6,"descr");
                                                            if(obj == NULL)
                                                            {
                                                                strcpy(ng->remote_sysdescp,"none");
                                                                LOG_WARN("this neighbor does not have system description\n");
                                                            }
                                                            else
                                                            {
                                                                value = json_object_get_string(obj);
                                                                strcpy(ng->remote_sysdescp,value);
                                                                LOG_NORMAL("finaly result: key = remote_sys_description, value = %s\n",value);
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                        else
                        {
                            LOG_NORMAL("only one port\n");
                            value3 = value2;
                            goto find_port; 
                        }                    
                    }
                }
            }

        } 
    }


    return 0;
}

static int lldpGetInfoFromLldpd(void)
{
    system("lldpctl -f json > /tmp/json.js");

    FILE *file;
    int size;
    char *pbuf;

    file = fopen("/tmp/json.js","r+");
    if(file == NULL)
    {
        LOG_ERROR("open file contained lldpd information fail\n");
        return -1;
    }

    fseek(file,0,SEEK_END);
    size = ftell(file);
    fseek(file,0,SEEK_SET);
    
    pbuf = (char *)malloc(size);
    int ret = fread(pbuf,sizeof(char),size,file);
    if(ret != size)
    {
        LOG_ERROR("read file :/etc/lldpd.conf fail ,ret = %d!\n",ret);
        return -1;
    }

    fclose(file);

    struct json_object *object = NULL;
    object = json_tokener_parse(pbuf);

    ret = lldpParseJsonInfo(object);
    if(ret)
    {
        LOG_ERROR("parse file :/etc/lldpd.conf fail!\n");
        return -1;
    }

    json_object_put(object);

    return 0;    
}

/* *
 * * @brief         get lldp entry num
 * * @retval 0:     OK
 * * @retval -1:    ERROR
 * */
static int lldpGetEntryNum(struct blob_buf *buf)
{
    if(lldpCfg.status != FAL_ENABLE)
    {
        blobmsg_add_string(buf, "entry_num", "0");
        LOG_WARN("function of lldp config disable!\n");
        return -1;
    }

    char num_s[2];
    sw_error_t res;
    neighbor_S *ng,*ng_bak;

    if(!list_empty(&lldpCfg.neighbor))
    {
        list_for_each_entry_safe(ng, ng_bak, &lldpCfg.neighbor, node)
        {
            list_del(&ng->node);
            free(ng);
        }
    }

    res = lldpGetInfoFromLldpd();
    if(res)
    {
        LOG_ERROR("lldp get information from lldpd fail\n");
        blobmsg_add_string(buf, "entry_num", "-1");
        return -1;
    }

    LOG_NORMAL("get lldp info from lldpd OK\n");
    int count = 0;

    list_for_each_entry_safe(ng, ng_bak, &lldpCfg.neighbor, node)
    {
        ng->idx = ++count;
    }

    lldpCfg.entry_num = count;

    sprintf(num_s,"%d",count);
    blobmsg_add_string(buf, "entry_num", num_s);
    LOG_NORMAL("get lldp entry number = [%s] OK\n",num_s);

    return 0;
}

/* *
 * * @brief         get lldp local port number
 * * @param[idx]    entry index
 * * @retval 0:     OK
 * * @retval -1:    ERROR
 * */
static int lldpGetLocalPortNumber(struct blob_buf *buf,int idx)
{
    if(lldpCfg.status != FAL_ENABLE)
    {
        blobmsg_add_string(buf, "local_port_number", "-1");
        LOG_WARN("function of lldp config disable!\n");
        return -1;
    }

    neighbor_S *ng,*ng_bak;

    list_for_each_entry_safe(ng, ng_bak, &lldpCfg.neighbor, node)
    {
        if(idx == ng->idx)
        {
            LOG_NORMAL("Get local port = [%s] OK\n",ng->local_port);
            blobmsg_add_string(buf,"local_port_number",ng->local_port);
            
            return 0;
        }
    }

    blobmsg_add_string(buf,"local_port_number","-1");
    LOG_ERROR("no match index [%d], something error!\n",idx);

    return -1;
}

/* *
 * * @brief         get lldp remote port description 
 * * @param[idx]    entry index
 * * @retval 0:     OK
 * * @retval -1:    ERROR
 * */
static int lldpGetRemotePortDescp(struct blob_buf *buf,int idx)
{
    if(lldpCfg.status != FAL_ENABLE)
    {
        blobmsg_add_string(buf, "remote_port_description", "-1");
        LOG_WARN("function of lldp config disable!\n");
        return -1;
    }

    neighbor_S *ng,*ng_bak;

    list_for_each_entry_safe(ng, ng_bak, &lldpCfg.neighbor, node)
    {
        if(idx == ng->idx)
        {
            LOG_NORMAL("Get remote port description = [%s] OK\n",ng->remote_port_descp);
            blobmsg_add_string(buf,"remote_port_description",ng->remote_port_descp);
            
            return 0;
        }
    }

    blobmsg_add_string(buf, "remote_port_description", "-1");
    LOG_ERROR("no match index [%d], something error!\n",idx);

    return -1;
}

/* *
 * * @brief         get lldp remote ip
 * * @param[idx]    entry index
 * * @retval 0:     OK
 * * @retval -1:    ERROR
 * */
static int lldpGetRemoteIp(struct blob_buf *buf,int idx)
{
    if(lldpCfg.status != FAL_ENABLE)
    {
        blobmsg_add_string(buf, "remote_ip", "-1");
        LOG_WARN("function of lldp config disable!\n");
        return -1;
    }

    neighbor_S *ng,*ng_bak;

    list_for_each_entry_safe(ng, ng_bak, &lldpCfg.neighbor, node)
    {
        if(idx == ng->idx)
        {
            LOG_NORMAL("Get remote ip = [%s] OK\n",ng->remote_ip);
            blobmsg_add_string(buf,"remote_ip",ng->remote_ip);
            
            return 0;
        }
    }

    blobmsg_add_string(buf, "remote_ip", "-1");
    LOG_ERROR("no match index [%d], something error!\n",idx);

    return -1;
}

/* *
 * * @brief         get lldp remote mac
 * * @param[idx]    entry index
 * * @retval 0:     OK
 * * @retval -1:    ERROR
 * */
static int lldpGetRemoteMac(struct blob_buf *buf,int idx)
{
    if(lldpCfg.status != FAL_ENABLE)
    {
        blobmsg_add_string(buf,"remote_mac","-1");
        LOG_WARN("function of lldp config disable!\n");
        return -1;
    }

    neighbor_S *ng,*ng_bak;

    list_for_each_entry_safe(ng, ng_bak, &lldpCfg.neighbor, node)
    {
        if(idx == ng->idx)
        {
            LOG_NORMAL("Get remote mac = [%s] OK\n",ng->remote_mac);
            blobmsg_add_string(buf,"remote_mac",ng->remote_mac);
            
            return 0;
        }
    }

    blobmsg_add_string(buf,"remote_mac","-1");
    LOG_ERROR("no match index [%d], something error!\n",idx);

    return -1;
}

/* *
 * * @brief         get lldp remote system name
 * * @param[idx]    entry index
 * * @retval 0:     OK
 * * @retval -1:    ERROR
 * */
static int lldpGetRemoteSysName(struct blob_buf *buf,int idx)
{
    if(lldpCfg.status != FAL_ENABLE)
    {
        blobmsg_add_string(buf,"remote_sys_name","-1");
        LOG_WARN("function of lldp config disable!\n");
        return -1;
    }

    neighbor_S *ng,*ng_bak;

    list_for_each_entry_safe(ng, ng_bak, &lldpCfg.neighbor, node)
    {
        if(idx == ng->idx)
        {
            LOG_NORMAL("Get remote system name = [%s] OK\n",ng->remote_sysname);
            blobmsg_add_string(buf,"remote_sys_name",ng->remote_sysname);
            
            return 0;
        }
    }

    blobmsg_add_string(buf,"remote_sys_name","-1");
    LOG_ERROR("no match index [%d], something error!\n",idx);

    return -1;
}

/* *
 * * @brief         get lldp remote system description
 * * @param[idx]    entry index
 * * @retval 0:     OK
 * * @retval -1:    ERROR
 * */
static int lldpGetRemoteSysDescp(struct blob_buf *buf,int idx)
{
    if(lldpCfg.status != FAL_ENABLE)
    {
        blobmsg_add_string(buf,"remote_sys_description","-1");
        LOG_WARN("function of lldp config disable!\n");
        return -1;
    }

    neighbor_S *ng,*ng_bak;

    list_for_each_entry_safe(ng, ng_bak, &lldpCfg.neighbor, node)
    {
        if(idx == ng->idx)
        {
            LOG_NORMAL("Get remote system description = [%s] OK\n",ng->remote_sysdescp);
            blobmsg_add_string(buf,"remote_sys_description",ng->remote_sysdescp);
            
            return 0;
        }
    }

    blobmsg_add_string(buf,"remote_sys_description","-1");
    LOG_ERROR("no match index [%d], something error!\n",idx);

    return -1;
}


static const struct jw_switch_policy lldp_cfg_tbl[] = {
    [LLDP_ADMIN_STATUS] = {.name = "admin_status", .get_ext_handler = lldpGetAdminStatus, .set_ext_handler = lldpSetAdminStatus}, 
    [LLDP_ENTRY_NUM] = {.name = "entry_num", .get_ext_handler = lldpGetEntryNum},
    [LLDP_LOCAL_PORT_NUM] = {.name = "local_port_number", .get_handler = lldpGetLocalPortNumber},
    [LLDP_REMOTE_PORT_NUM] = {.name = "remote_port_description", .get_handler = lldpGetRemotePortDescp},
    [LLDP_REMOTE_IP] = {.name = "remote_ip", .get_handler = lldpGetRemoteIp},
    [LLDP_REMOTE_MAC] = {.name = "remote_mac", .get_handler = lldpGetRemoteMac},
    [LLDP_REMOTE_SYS_NAME] = {.name = "remote_sys_name", .get_handler = lldpGetRemoteSysName},
    [LLDP_REMOTE_SYS_DESCP] = {.name = "remote_sys_description", .get_handler = lldpGetRemoteSysDescp},
};

/* *
 * * @brief         lldp config parse recv form upper
 * * @param[lvl3]   level 3 message
 * * @retval 0:     OK
 * * @retval -1:    ERROR
 * */
static int parseLldpCfgGet(struct blob_attr *lvl3, struct blob_buf *buf)
{
    const char *name  = blobmsg_name(lvl3);
    int idx     = atoi(name + 4);

    if (idx < 1 || idx > lldpCfg.entry_num) 
    {
        LOG_ERROR("invalid index %d\n", idx);
        return -1;
    }

    void *ret_array;

    ret_array= blobmsg_open_array(buf, name);
    foreach_json_array_get(idx,lvl3,buf,lldp_cfg_tbl, __LLDP_CFG_TBL_MAX);
    blobmsg_close_array(buf, ret_array);	

    return 0; 
}



/* *
 * * @brief         method "lldp_config_get" handler
 * * @retval 0:     OK
 * * @retval -1:    ERROR
 * */
static int lldpCfgGetHandler(struct ubus_context *ctx, struct ubus_object *obj,
                struct ubus_request_data *req, const char *method,
                struct blob_attr *msg)
{
    struct blob_attr *tb[__LLDP_CFG_GET_MAX];    
	struct blob_attr *lvl1 = NULL;  

    blobmsg_parse(lldp_cfg_get_policy, __LLDP_CFG_GET_MAX, tb, blob_data(msg), blob_len(msg));
	lvl1 = tb[LLDP_CFG_GET_KEY];     
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

                policy_p = (struct jw_switch_policy *)jw_switchd_get_context(name, lldp_cfg_tbl, __LLDP_CFG_TBL_MAX);
                if (policy_p && policy_p->get_ext_handler) 
                {
                    ret_table = blobmsg_open_table(&b, NULL);
                    policy_p->get_ext_handler(&b);
                    blobmsg_close_table(&b, ret_table);	
                }
                else
                {
                    LOG_NORMAL("policy check not pass!\n");
                }
            }
                break;
            case BLOBMSG_TYPE_TABLE:
            {
                struct blob_attr *lvl3 = blobmsg_data(lvl2);

                if(blobmsg_type(lvl3) == BLOBMSG_TYPE_ARRAY)
                {
                    ret_table = blobmsg_open_table(&b, NULL);
                    parseLldpCfgGet(lvl3,&b);
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
 * * @brief         method "port_aggr_set" handler
 * * @retval 0:     OK
 * * @retval -1:    ERROR
 * */
static int lldpCfgSetHandler(struct ubus_context *ctx, struct ubus_object *obj,
                struct ubus_request_data *req, const char *method,
                struct blob_attr *msg)
{
    struct blob_attr *tb[__LLDP_CFG_SET_MAX];    
	struct blob_attr *lvl1 = NULL;  
    int ret = -1;

    blobmsg_parse(lldp_cfg_set_policy, __LLDP_CFG_SET_MAX, tb, blob_data(msg), blob_len(msg));
	lvl1 = tb[LLDP_CFG_SET_KEY];     
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
            
            if(blobmsg_type(lvl3) == BLOBMSG_TYPE_STRING)
            {
                struct jw_switch_policy *policy_p;
                const char *name  = blobmsg_name(lvl3);
                char *value = blobmsg_get_string(lvl3);

                policy_p = (struct jw_switch_policy *)jw_switchd_get_context(name, lldp_cfg_tbl, __LLDP_CFG_TBL_MAX);
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

end:
    blobmsg_add_string(&b, "ret", ret?"-1":"0");
	ubus_send_reply(ctx, req, b.head);

	return UBUS_STATUS_OK;

}


static const struct ubus_method lldp_cfg_methods[] = {
	UBUS_METHOD("lldp_config_get", lldpCfgGetHandler, lldp_cfg_get_policy),
	UBUS_METHOD("lldp_config_set", lldpCfgSetHandler, lldp_cfg_set_policy),
};


static struct ubus_object_type lldp_cfg_object_type =
	UBUS_OBJECT_TYPE("lldp_config", lldp_cfg_methods);


static struct ubus_object lldp_cfg_object = {
	.name = "lldp_config",
	.type = &lldp_cfg_object_type,
	.methods = lldp_cfg_methods,
	.n_methods = ARRAY_SIZE(lldp_cfg_methods),
};

static int lldpCfgLoad(void)
{
    FILE *file;
    int size;
    char *pbuf;

    file = fopen("/etc/lldpd.conf","r");
    if(file == NULL)
    {
        LOG_ERROR("open file :/etc/lldpd.conf fail!\n");
        return -1;
    }

    fseek(file,0,SEEK_END);
    size = ftell(file);
    fseek(file,0,SEEK_SET);
    
    pbuf = (char *)malloc(size);
    int ret = fread(pbuf,sizeof(char),size,file);
    if(ret != size)
    {
        LOG_ERROR("read file :/etc/lldpd.conf fail ,ret = %d!\n",ret);
        return -1;
    }

    char *p;

    p = strstr(pbuf,LLDP_ST_OPTION_NAME);
    if(p == NULL)
    {
        LOG_ERROR("there is no option named \"configure lldp status\" in file :/etc/lldpd.conf!\n");
        return -1;
    }

    p += strlen(LLDP_ST_OPTION_NAME);

    int offset = strspn(p," "); 

    if(!offset)
    {
        LOG_ERROR("format error in file :/etc/lldpd.conf!\n");
        return -1;
    }

    if(!strncmp(p + offset,LLDP_ST_OPTION_VALUE1,strlen(LLDP_ST_OPTION_VALUE1)))
    {
        lldpCfg.status = FAL_ENABLE; 
    }
    else
    {
        lldpCfg.status = FAL_DISABLE;
    }

    fclose(file);
    LOG_NORMAL("lldp load status = %d OK\n", lldpCfg.status);
    
    return 0;
}

static int lldpInit(void)
{
    int ret;

    memset(&lldpCfg,0,sizeof(lldpCfg));

    INIT_LIST_HEAD(&lldpCfg.neighbor);

    ret = lldpCfgLoad();
    if(ret)
    {
        LOG_ERROR("lldp configure load fail!\n");
        return -1;
    }

    return 0;
}

void ubus_init_lldp_config(struct ubus_context *ctx)
{
	int ret;

	_ctx = ctx;

    ret = lldpInit();
    if(ret)
    {
        LOG_ERROR("Init lldp fail!\n");
        return;
    }

	ret = ubus_add_object(ctx, &lldp_cfg_object);
	if (ret)
		LOG_ERROR("Failed to add object [lldp_config]: %s\n", ubus_strerror(ret));
}
