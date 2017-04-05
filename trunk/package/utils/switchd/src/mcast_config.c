/* *< 中威文件编码格式为gb2312,cp936 */
/* * $Id$ $DateTime$
 * * @file mcast_conofig.c
 * * @brief This is a table of the switch's multicast configurable information.
 * * @version 0.0.1
 * * @since 0.0.1
 * * @author code<songyaofei@joyware.com>
 * * @date 2016-05-16
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
#include <ctype.h>

#include "switchd.h"

static struct blob_buf b;
static struct ubus_context *_ctx;

enum {
    MCAST_CFG_GET_KEY,
    __MCAST_CFG_GET_MAX,
};

enum {
    MCAST_CFG_SET_KEY,
    __MCAST_CFG_SET_MAX,
};

enum {
    MCAST_CFG_CLEAR_KEY,
    __MCAST_CFG_CLEAR_MAX,
};

static const struct blobmsg_policy mcast_cfg_get_policy[] = {
    [MCAST_CFG_GET_KEY] = {.name = "get_args", .type = BLOBMSG_TYPE_ARRAY},
};

static const struct blobmsg_policy mcast_cfg_set_policy[] = {
    [MCAST_CFG_SET_KEY] = {.name = "set_args", .type = BLOBMSG_TYPE_ARRAY},
};

static const struct blobmsg_policy mcast_cfg_clear_policy[] = {
    [MCAST_CFG_CLEAR_KEY] = {.name = "clear_args", .type = BLOBMSG_TYPE_ARRAY},
};

enum {
    MST_ENABLE_STATUS,
    MSG_NUM,
    MST_MAC_ADDRESS,
    MST_GROUP_MEMBERS,
    __MCAST_CFG_TBL_MAX,
};

typedef struct{
    int idx;
    int flag;
    fal_multicast_addr_t mac_addr;
    fal_multicast_members_t memb;
    int mem_num;
    struct list_head node;
}mcastList_S;

/******< mcast config manager*/
static struct{
    int status;
    int num;
    struct list_head entry;
}mcastCfg;

/* *
 * * @brief             parse if the format of mac is correct
 * * @param[*mac_addr]  buffer to store mac
 * * @param[*p]         buffer to be parsed
 * * @retval 0:         OK
 * * @retval -1:        ERROR
 * */
static int mcastAnalyseMac(fal_multicast_addr_t *mac_addr,char *p)
{
    int i;

    for(i = 0;i < 6;i++)
    {
        if(i < 5 && p[i*3 + 2] != '-')
        {
            LOG_ERROR("analyse \'-\' error!\n");
            return -1;
        }

        if(isxdigit(p[i*3]) && isxdigit(p[i*3 + 1])) 
        {
            mac_addr->mac[i] = JW_CharToHex(p[i*3]) * 16 + JW_CharToHex(p[i*3 + 1]);
        }
        else {
            LOG_ERROR("analyse mac error!\n");
            return -1;
        }
    }

    if(p[17])
    {
        LOG_ERROR("analyse EOF error!\n");
        return -1;
    }
    if((mac_addr->mac[0] == strtol("0d", NULL, 16)) &&(mac_addr->mac[1] == strtol("a4", NULL, 16)) &&(mac_addr->mac[2] == strtol("2a", NULL, 16)) 
       &&(mac_addr->mac[3] == strtol("00", NULL, 16)) &&(mac_addr->mac[4] == strtol("00", NULL, 16)) &&(mac_addr->mac[5] == strtol("05", NULL, 16)))
    {//
        LOG_ERROR("set mac error, this mac has not set!\n");
        return -1; 
    }
    
    if((mac_addr->mac[0] == strtol("01", NULL, 16)) &&(mac_addr->mac[1] == strtol("00", NULL, 16)) &&(mac_addr->mac[2] == strtol("5e", NULL, 16)) 
       &&(mac_addr->mac[3] == strtol("64", NULL, 16)) &&(mac_addr->mac[4] == strtol("e4", NULL, 16)) &&(mac_addr->mac[5] == strtol("e4", NULL, 16)))
    {//
        LOG_ERROR("set mac error, this mac has not set!\n");
        return -1; 
    }
    
    if((mac_addr->mac[0] == strtol("01", NULL, 16)) &&(mac_addr->mac[1] == strtol("80", NULL, 16)) &&(mac_addr->mac[2] == strtol("c2", NULL, 16)) 
       &&(mac_addr->mac[3] == strtol("00", NULL, 16)) &&(mac_addr->mac[4] == strtol("00", NULL, 16)))
    {//
        if((mac_addr->mac[5] >= strtol("00", NULL, 16)) &&(mac_addr->mac[5] <= strtol("2f", NULL, 16))){
            LOG_ERROR("set mac error, this mac has not set!\n");
            return -1; 
        }
    }
    return 0; 
}

/* *
 * * @brief         delete specified list option in UCI 
 * * @retval 0:     OK
 * * @retval -1:    ERROR
 * */
static void mcastDeleteUciEntry(mcastList_S *mc)
{
    char buf[32];
    char memb_s[3*MV88E6097_MAX_MEMBERS_SIZE];
    struct uci_ptr ptr = {
        .package = "switchd",
        .section = "multicast",
        .option = "entry",
    };

    sprintf(buf,"%02x-%02x-%02x-%02x-%02x-%02x:",mc->mac_addr.mac[0],mc->mac_addr.mac[1],mc->mac_addr.mac[2],\
            mc->mac_addr.mac[3],mc->mac_addr.mac[4],mc->mac_addr.mac[5]);

    int i;
    for(i = 0;i < mc->mem_num;i++)
    {
        sprintf(memb_s + 3*i,"%02x-",mc->memb.members[i]);
    }
    memb_s[i * 3 - 1] = 0;

    LOG_NORMAL("set one entry: mac = %s , members = %s OK\n",buf,memb_s);

    strcat(buf,memb_s);
    ubus_collect_config(&ptr,buf,UCI_TYPE_LIST,DO_DEL);
}

/* *
 * * @brief         delete specified entry(contain driver and uci) 
 * * @retval 0:     OK
 * * @retval -1:    ERROR
 * */
static int mcastDeleteEntry(mcastList_S *mc)
{
    int ret;

    /**<    delete specified entry in driver  */
    ret = fal_multicast_row_destroy(0,&mc->memb,&mc->mac_addr,mc->mem_num);
    if(ret)
    {
        LOG_ERROR("multicast destory old entry in driver fail!\n");
        return -1;
    }

    /**<    delete specified entry in UCI file  */
    mcastDeleteUciEntry(mc);
    
    return 0;
}

/* *
 * * @brief         get multicast status 
 * * @retval 0:     OK
 * * @retval -1:    ERROR
 * */
static int mcastGetStatus(struct blob_buf *buf)
{
    char status_s[2];

    sprintf(status_s,"%d",mcastCfg.status);
    blobmsg_add_string(buf, "enable_status", status_s);
    LOG_NORMAL("get multicast status = [%s] OK\n",status_s);

    return 0;
}

/* *
 * * @brief         set multicast status 
 * * @retval 0:     OK
 * * @retval -1:    ERROR
 * */
static int mcastSetStatus(void *p)
{
    fal_enable_t status = atoi((char *)p);
    sw_error_t ret;

    /**<    check arguement  */
    if(status == mcastCfg.status)
    {
        LOG_NORMAL("same arguement,do nothing\n");
        return 0;
    }
    if(status != FAL_DISABLE && status != FAL_ENABLE)
    {
        LOG_ERROR("invalid arguement!\n");
        return -1;
    }
    
    /**<    set multicast status  */
    ret = fal_multicast_enable_set(0,status);
    if(ret)
    {
        LOG_ERROR("set multicast status fail!\n");
        return -1;
    }

    mcastCfg.status = status;

    char buf[2];
    struct uci_ptr ptr = {
        .package = "switchd",
        .section = "multicast",
        .option = "status",
    };

    sprintf(buf,"%d",status);
    ubus_collect_config(&ptr,buf,UCI_TYPE_STRING,DO_SET);

    /**<    set every entry if status is enable  */
    if(status == FAL_ENABLE && mcastCfg.num)
    {
        mcastList_S *mc,*mc_bak;
        list_for_each_entry_safe(mc, mc_bak, &mcastCfg.entry, node)
        {
            if(!mc->mem_num || IS_EMPTY_MAC(mc->mac_addr.mac))
                continue;

            ret = fal_multicast_row_create(0,&mc->memb,&mc->mac_addr,mc->mem_num);
            if(ret)
            {
                LOG_ERROR("set new mac address error!\n");
                return -1;
            }          
        }
    }
    
    LOG_NORMAL("set multicast status = [%d] OK\n",status);
    return 0;
}

/* *
 * * @brief         get multicast entry number
 * * @retval 0:     OK
 * * @retval -1:    ERROR
 * * @note:         upper limit 999 temporary
 * */
static int mcastGetNum(struct blob_buf *buf)
{
    char num_s[4];
    
    sprintf(num_s,"%d",mcastCfg.num);
    blobmsg_add_string(buf, "group_num", num_s);
    LOG_NORMAL("get multicast group num = [%s] OK\n",num_s);

    return 0;
}

/* *
 * * @brief         get mac address of one multicast entry
 * * @param[idx]    entry index 
 * * @retval 0:     OK
 * * @retval -1:    ERROR
 * */
static int mcastGetMacAddress(struct blob_buf *buf,int idx)
{
    mcastList_S *mc,*mc_bak;
    list_for_each_entry_safe(mc, mc_bak, &mcastCfg.entry, node)
    {
        if(idx == mc->idx)
        {
            char mac_s[18];

            sprintf(mac_s,"%02x-%02x-%02x-%02x-%02x-%02x",mc->mac_addr.mac[0],mc->mac_addr.mac[1],mc->mac_addr.mac[2],\
                    mc->mac_addr.mac[3],mc->mac_addr.mac[4],mc->mac_addr.mac[5]);
            blobmsg_add_string(buf, "mac_address", mac_s);
            LOG_NORMAL("mcast get index %d mac address = [%s] OK\n",idx,mac_s);
            
            return 0;
        }
    }

    blobmsg_add_string(buf,"mac_address","-1");
    LOG_ERROR("no match index [%d], something error!\n",idx);

    return -1;
}


/* *
 * * @brief         set mac address for one entry in multicast config 
 * * @param[idx]    entry index
 * * @param[*p]     buffer to be parsed
 * * @retval 0:     OK
 * * @retval -1:    ERROR
 * */
static int mcastSetMacAddress(int idx,void *p)
{
    fal_multicast_addr_t mac_new;
    sw_error_t ret;
    
    /**<    check arguement  */
    ret = mcastAnalyseMac(&mac_new,(char *)p);
    if(ret)
    {
        LOG_ERROR("analyse multicast mac address fail!\n");
        return -1;
    }

    /**<    foreach every node  */
    mcastList_S *mc,*mc_bak;
    mcastList_S *m = NULL;
    list_for_each_entry_safe(mc, mc_bak, &mcastCfg.entry, node)
    {
        if(idx == mc->idx)
        {
            m = mc;
        }
        else if(!memcmp(&mac_new,&mc->mac_addr,sizeof(fal_multicast_addr_t)))
        {
            m = NULL;
            LOG_ERROR("mac address duplicate!\n");
            break;       
        }
    }
    
    if(m == NULL)
    {
        LOG_ERROR("something error while foreach!\n");
        return -1;   
    }

    fal_multicast_addr_t *mac_old = &m->mac_addr;
    if(!memcmp(&mac_new,mac_old,sizeof(fal_multicast_addr_t)))
    {
        LOG_NORMAL("same arguement,do nothing\n");
        return 0;
    }        

    /**<    destory old entry if exist  */
    if(m->mem_num)  
    {
        if(!IS_EMPTY_MAC(mac_old->mac))
        {
            if(mcastDeleteEntry(m))
            {
                LOG_ERROR("mcast delete entry fail!\n");
                return -1;
            }
        }

        m->flag = 1;
    }

    memcpy(&m->mac_addr,&mac_new,sizeof(fal_multicast_addr_t));

    LOG_NORMAL("index [%d] set mac address OK!\n",idx);
    return 0;

 
}

/* *
 * * @brief         get group members for one entry in multicast config 
 * * @param[idx]    entry index
 * * @retval 0:     OK
 * * @retval -1:    ERROR
 * */
static int mcastGetGroupMembers(struct blob_buf *buf,int idx)
{
    char memb_s[3*MV88E6097_MAX_MEMBERS_SIZE];

    mcastList_S *mc,*mc_bak;
    list_for_each_entry_safe(mc, mc_bak, &mcastCfg.entry, node)
    {
        if(idx == mc->idx)
        {
            if(!mc->mem_num)
            {
                blobmsg_add_string(buf, "group_members", "0");
                LOG_WARN("index [%d] multicast configure group members has not been set!\n",idx);
                return 0;
            }

            int i;
            for(i = 0;i < mc->mem_num;i++)
            {
                sprintf(memb_s + 3*i,"%02x-",mc->memb.members[i]);
            }

            memb_s[i * 3 - 1] = 0;
                     
            blobmsg_add_string(buf, "group_members", memb_s);
            LOG_NORMAL("index [%d] get multicast group members = [%s] OK\n",idx,memb_s);
            return 0;
        }
    }

    blobmsg_add_string(buf, "group_members", "-1");
    LOG_ERROR("no match index [%d], something error!\n",idx);
    return -1;
}

/* *
 * * @brief         set group members for one entry in multicast config 
 * * @param[idx]    entry index
 * * @param[*p]     buffer to be parsed
 * * @retval 0:     OK
 * * @retval -1:    ERROR
 * */
static int mcastSetGroupMembers(int idx,void *p)
{
    int num;

    fal_multicast_members_t memb_new;

    num = JW_ObjStrToArray(MV88E6097_MAX_MEMBERS_SIZE,memb_new.members,p);
    if(num == -1)
    {
        LOG_ERROR("analyse multicast group members fail!\n");
        return -1;
    }

    mcastList_S *mc,*mc_bak;
    list_for_each_entry_safe(mc, mc_bak, &mcastCfg.entry, node)
    {
        if(idx == mc->idx)
        {
            fal_multicast_members_t *memb_old = &mc->memb;
            fal_multicast_addr_t *mac_addr = &mc->mac_addr;
                
            if(!memcmp(&memb_new,memb_old,sizeof(fal_multicast_members_t)))
            {
                LOG_NORMAL("same arguement,nothing change\n");
                return 0;
            }

            if(!IS_EMPTY_MAC(mac_addr->mac))
            {
                if(mc->mem_num)
                {
                    /**<    delete specified entry in UCI file  */
                    mcastDeleteUciEntry(mc);
                }

                mc->flag = 1;
            }

            
            mc->mem_num = num;
            memcpy(&mc->memb,&memb_new,sizeof(fal_multicast_members_t));

            LOG_NORMAL("mcast set group members OK!\n");
            return 0;
        }
    }
    
    LOG_ERROR("no match index [%d], something error!\n",idx);
    return -1;
}

static const struct jw_switch_policy mcast_cfg_tbl[] = {
    [MST_ENABLE_STATUS] = {.name = "enable_status", .get_ext_handler = mcastGetStatus, .set_ext_handler = mcastSetStatus}, 
    [MSG_NUM] = {.name = "group_num", .get_ext_handler = mcastGetNum},
    [MST_MAC_ADDRESS] = {.name = "mac_address", .get_handler = mcastGetMacAddress, .set_handler = mcastSetMacAddress},
    [MST_GROUP_MEMBERS] = {.name = "group_members", .get_handler = mcastGetGroupMembers, .set_handler = mcastSetGroupMembers},
};

/* *
 * * @brief         multicast config parse recv form upper
 * * @param[lvl3]   level 3 message
 * * @retval 0:     OK
 * * @retval -1:    ERROR
 * */
static int mcastParseGet(struct blob_attr *lvl3, struct blob_buf *buf)
{
    const char *name  = blobmsg_name(lvl3);
    int idx     = atoi(name + 4);

    if (idx < 1 || idx > mcastCfg.num) 
    {
        LOG_ERROR("invalid index %d\n", idx);
        return -1;
    }
    
    void *ret_array;

    ret_array= blobmsg_open_array(buf, name);
    foreach_json_array_get(idx,lvl3,buf,mcast_cfg_tbl,__MCAST_CFG_TBL_MAX);
    blobmsg_close_array(buf, ret_array);	
        
    return 0; 
}

/* *
 * * @brief         multicast config parse recv form upper
 * * @param[lvl3]   level 3 message
 * * @retval 0:     OK
 * * @retval -1:    ERROR
 * */
static int mcastParseSet(struct blob_attr *lvl3)
{
    if(mcastCfg.status != FAL_ENABLE)
    {
        LOG_ERROR("the status of multicast-config is disable!\n");
        return -1;
    }

    const char *name  = blobmsg_name(lvl3);
    int idx     = atoi(name + 4);

    if (idx < 1 || idx > (mcastCfg.num + 1)) 
    {
        LOG_ERROR("invalid index %d\n", idx);
        return -1;
    }

    /**<    add new index/entry  */
    if(idx == (mcastCfg.num + 1))
    {
        mcastCfg.num++;
        mcastList_S *mc = (mcastList_S *)calloc(1,sizeof(mcastList_S));
        list_add_tail(&mc->node,&mcastCfg.entry);
        mc->idx = mcastCfg.num;
    }

    /**<    foreach array of level 3 message  */
    int ret;
    ret = foreach_json_array_set(idx,lvl3,mcast_cfg_tbl,__MCAST_CFG_TBL_MAX);
    if(ret)
    {
        LOG_ERROR("foreach index %d error!\n",idx);
        return -1;
    }

    /**<    create each multicast row which has been changed  */
    mcastList_S *mc,*mc_bak;
    list_for_each_entry_safe(mc, mc_bak, &mcastCfg.entry, node)
    {
        if(mc->flag)
        {   
            int j = 0, memVal = 0;

            for(j = 0; j < mc->mem_num; j++)
            {
                memVal += mc->memb.members[j];
            }
            if(memVal)
                ret = fal_multicast_row_create(0,&mc->memb,&mc->mac_addr,mc->mem_num);
            else
                ret = fal_multicast_row_destroy(0,&mc->memb,&mc->mac_addr,mc->mem_num);
            if(ret)
            {
                /**<    add free memory later  */
                LOG_ERROR("set new mac address error!\n");
                mc->flag = 0;
                return -1;
            }
            mc->flag = 0;

            char buf[32];
            char memb_s[3*MV88E6097_MAX_MEMBERS_SIZE];
            struct uci_ptr ptr = {
                .package = "switchd",
                .section = "multicast",
                .option = "entry",
            };

            sprintf(buf,"%02x-%02x-%02x-%02x-%02x-%02x:",mc->mac_addr.mac[0],mc->mac_addr.mac[1],mc->mac_addr.mac[2],\
                    mc->mac_addr.mac[3],mc->mac_addr.mac[4],mc->mac_addr.mac[5]);

            int i;
            for(i = 0;i < mc->mem_num;i++)
            {
                sprintf(memb_s + 3*i,"%02x-",mc->memb.members[i]);
            }
            memb_s[i * 3 - 1] = 0;

            LOG_NORMAL("set one entry: mac = %s , members = %s OK\n",buf,memb_s);

            strcat(buf,memb_s);
            ubus_collect_config(&ptr,buf,UCI_TYPE_LIST,DO_SET);

        }
    }

    return 0; 
}

/* *
 * * @brief         parse multicast config clear operation from upper
 * * @param[lvl2]   level 2 message
 * * @retval 0:     OK
 * * @retval -1:    ERROR
 * */
static int mcastParseClear(struct blob_attr *lvl2)
{
    char *value = blobmsg_get_string(lvl2);
    int idx     = atoi(value + 4);
    int flag = 0;

    if (idx < 1 || idx > mcastCfg.num) 
    {
        LOG_ERROR("invalid index %d\n", idx);
        return -1;
    }
    
    mcastList_S *mc,*mc_bak;
    list_for_each_entry_safe(mc, mc_bak, &mcastCfg.entry, node)
    {
        if(flag)
        {
            mc->idx--;
        }
        else if(idx == mc->idx)
        {
            /**<    delete specified entry  */
            if(mc->mem_num && !IS_EMPTY_MAC(mc->mac_addr.mac))
            {
                if(mcastDeleteEntry(mc))
                {
                    LOG_ERROR("mcast delete entry fail!\n");
                    return -1;
                }
            }
            
            flag = 1;
            mcastCfg.num--;
            list_del(&mc->node);
            free(mc);
        }
    }

    if(flag)
    {
        LOG_NORMAL("destroy one multicast entry which index = %d OK\n",idx);
        return 0;
    }

    LOG_ERROR("no match index [%d], something error!\n",idx);
    return -1;
}


/* *
 * * @brief         method "mcast_config_get" handler
 * * @retval 0:     OK
 * * @retval -1:    ERROR
 * */
static int mcastGetHandler(struct ubus_context *ctx, struct ubus_object *obj,
                struct ubus_request_data *req, const char *method,
                struct blob_attr *msg)
{
    struct blob_attr *tb[__MCAST_CFG_GET_MAX];    
	struct blob_attr *lvl1 = NULL;  

    blobmsg_parse(mcast_cfg_get_policy, __MCAST_CFG_GET_MAX, tb, blob_data(msg), blob_len(msg));
	lvl1 = tb[MCAST_CFG_GET_KEY];     
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

                policy_p = (struct jw_switch_policy *)jw_switchd_get_context(name, mcast_cfg_tbl, __MCAST_CFG_TBL_MAX);
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
                    mcastParseGet(lvl3,&b);
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
 * * @brief method "mcast_config_set" handler
 * * @retval 0: OK
 * * @retval -1: ERROR
 * */
static int mcastSetHandler(struct ubus_context *ctx, struct ubus_object *obj,
                struct ubus_request_data *req, const char *method,
                struct blob_attr *msg)
{
    struct blob_attr *tb[__MCAST_CFG_SET_MAX];    
	struct blob_attr *lvl1 = NULL;  
    int ret = -1;

    blobmsg_parse(mcast_cfg_set_policy, __MCAST_CFG_SET_MAX, tb, blob_data(msg), blob_len(msg));
	lvl1 = tb[MCAST_CFG_SET_KEY];     
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

                    policy_p = (struct jw_switch_policy *)jw_switchd_get_context(name, mcast_cfg_tbl, __MCAST_CFG_TBL_MAX);
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
                    ret = mcastParseSet(lvl3);
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
 * * @fn mcastClearHandler
 * * @brief method "port_aggr_clear" handler
 * * @retval 0: OK
 * * @retval -1: ERROR
 * */
static int mcastClearHandler(struct ubus_context *ctx, struct ubus_object *obj,
                struct ubus_request_data *req, const char *method,
                struct blob_attr *msg)
{
    struct blob_attr *tb[__MCAST_CFG_CLEAR_MAX];    
	struct blob_attr *lvl1 = NULL;  
    int ret = -1;

    blobmsg_parse(mcast_cfg_clear_policy, __MCAST_CFG_CLEAR_MAX, tb, blob_data(msg), blob_len(msg));
	lvl1 = tb[MCAST_CFG_CLEAR_KEY];     
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
                        
            ret = mcastParseClear(lvl2);
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

static const struct ubus_method mcast_cfg_methods[] = {
	UBUS_METHOD("mcast_config_get", mcastGetHandler, mcast_cfg_get_policy),
	UBUS_METHOD("mcast_config_set", mcastSetHandler, mcast_cfg_set_policy),
	UBUS_METHOD("mcast_config_clear", mcastClearHandler, mcast_cfg_clear_policy),
};


static struct ubus_object_type mcast_cfg_object_type =
	UBUS_OBJECT_TYPE("mcast_config", mcast_cfg_methods);


static struct ubus_object mcast_cfg_object = {
	.name = "mcast_config",
	.type = &mcast_cfg_object_type,
	.methods = mcast_cfg_methods,
	.n_methods = ARRAY_SIZE(mcast_cfg_methods),
};

/* *
 * * @brief             load mcast arguement - status from UCI file
 * * @param[*p]         buffer to be parse 
 * * @retval 0:         OK
 * * @retval -1:        ERROR
 * */
static int mcastLoadStatus(struct uci_ptr *ptr)
{
    int status = atoi(ptr->o->v.string);
    int ret;

    if(status != FAL_DISABLE && status != FAL_ENABLE)
    {
        LOG_ERROR("invalid arguement!\n"); 
        return -1;
    }

    ret = fal_multicast_enable_set(0,status);
    if(ret)
    {
        LOG_ERROR("set multicast status fail!\n");
        return -1;
    }

    mcastCfg.status = status;
    LOG_NORMAL("mcast load status = [%d] OK\n", status);
    
    return 0;
}

/* *
 * * @brief             load mcast arguement - entry list from UCI file
 * * @param[*p]         buffer to be parse 
 * * @retval 0:         OK
 * * @retval -1:        ERROR
 * */
static int mcastLoadEntry(struct uci_ptr *ptr)
{
    int ret;
    int count = 0;

    struct uci_element *elt;

    uci_foreach_element(&ptr->o->v.list,elt)
    {
        /**<    separate mac address and members  */
        char *pdelim = strchr(elt->name,':');
        if(pdelim == NULL)
        {
            LOG_ERROR("mcast entry list format in UCI file error!\n"); 
            return -1;
        }

        pdelim[0] = 0;

        /**<    check if mac address and members are valid  */
        fal_multicast_addr_t mac_new;
        ret = mcastAnalyseMac(&mac_new,elt->name);
        if(ret)
        {
            LOG_ERROR("parse mcast entry mac address format in UCI file error!\n"); 
            return -1;
        }

        fal_multicast_members_t memb_new;
        int num = JW_ObjStrToArray(MV88E6097_MAX_MEMBERS_SIZE,memb_new.members,pdelim + 1);
        if(num == -1)
        {
            LOG_ERROR("parse mcast entry members fail!\n");
            return -1;
        }

        /**<    set entry to driver  */
        if(mcastCfg.status == FAL_ENABLE)
        {
            ret = fal_multicast_row_create(0,&memb_new,&mac_new,num);
            if(ret)
            {
                LOG_ERROR("set new mac address error!\n");
                return -1;
            }
        }

        /**<    link entry to list  */
        mcastList_S *mc = (mcastList_S *)calloc(1,sizeof(mcastList_S));
        list_add_tail(&mc->node,&mcastCfg.entry);

        memcpy(&mc->mac_addr,&mac_new,sizeof(fal_multicast_addr_t));
        memcpy(&mc->memb,&memb_new,sizeof(fal_multicast_members_t));
        mc->mem_num = num;
        mc->idx = ++count;
    }

    mcastCfg.num = count;

    LOG_NORMAL("mcast load entry OK\n");
    
    return 0;
}

static UciConfig_S mcastUci[] = {
    {.name = "status",  .type = UCI_TYPE_STRING,    .ubusGet = mcastLoadStatus},
    {.name = "entry",   .type = UCI_TYPE_LIST,      .ubusGet = mcastLoadEntry},
};

static int mcastCfgLoad(void)
{   
    memset(&mcastCfg,0,sizeof(mcastCfg));
    INIT_LIST_HEAD(&mcastCfg.entry);

    struct uci_context *ctx = uci_alloc_context();

    char name[64]; 
    int ret;
    int i;
    struct uci_ptr ptr;

    for(i = 0;i < sizeof(mcastUci)/sizeof(UciConfig_S);i++)
    {
        sprintf(name,"switchd.multicast.%s",mcastUci[i].name);
        memset(&ptr, 0, sizeof(struct uci_ptr));
        ret = load_config(name,mcastUci[i].type,&ptr,ctx);
        if(ret)
        {
            if(mcastUci[i].type == UCI_TYPE_LIST) continue;

            LOG_ERROR("load config %s fail!\n",name); 
            uci_free_context(ctx);
            return -1;
        }
        
        ret = mcastUci[i].ubusGet(&ptr);
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

void ubus_init_mcast_config(struct ubus_context *ctx)
{
	int ret;

	_ctx = ctx;

    ret = mcastCfgLoad();
    if(ret)
    {
		LOG_ERROR("mcast load configure from UCI file fail\n");
        return;
    }

	ret = ubus_add_object(ctx, &mcast_cfg_object);
	if (ret)
		LOG_ERROR("Failed to add object [mcast_config]: %s\n", ubus_strerror(ret));
}
