/* *< 中威文件编码格式为gb2312,cp936 */
/* * $Id$ $DateTime$
 * * @file port_aggregation.c
 * * @brief This is a table of the switch's port aggregation configuration.
 * * @version 0.0.1
 * * @since 0.0.1
 * * @author code<songyaofei@obtelecom.com>
 * * @date 2016-04-29
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
    PORT_AGGR_GET_KEY,
    __PORT_AGGR_GET_MAX,
};

enum {
    PORT_AGGR_SET_KEY,
    __PORT_AGGR_SET_MAX,
};

enum {
    PORT_AGGR_CLEAR_KEY,
    __PORT_AGGR_CLEAR_MAX,
};

static const struct blobmsg_policy port_aggr_get_policy[] = {
    [PORT_AGGR_GET_KEY] = {.name = "get_args", .type = BLOBMSG_TYPE_ARRAY},
};

static const struct blobmsg_policy port_aggr_set_policy[] = {
    [PORT_AGGR_SET_KEY] = {.name = "set_args", .type = BLOBMSG_TYPE_ARRAY},
};

static const struct blobmsg_policy port_aggr_clear_policy[] = {
    [PORT_AGGR_SET_KEY] = {.name = "clear_args", .type = BLOBMSG_TYPE_ARRAY},
};

enum {
    AGGR_SWITCH,
    AGGR_LOAD_BALANCE,
    AGGR_GROUP_NUM,
    AGGR_GROUP_ID,
    AGGR_GROUP_MEMBER,
    __PORT_AGGR_TBL_MAX,
};

#define AGGR_MAX_CHANNEL_ID 8
#define AGGR_MAX_CHANNEL_ID_VAL 15

typedef struct{
    int idx;
    int flag;
    fal_trunk_id_t id;
    fal_trunk_members_t memb;
    int mem_num;
    struct list_head node;
}aggrList_S;

/******< port aggregation manager*/
static struct{
    fal_trunk_mode_t aggr_sw;
    fal_trunk_load_balance_t aggr_lb;
    int aggr_num;
    struct list_head entry;
}aggr;


/* *
 * * @brief         delete specified list option in UCI 
 * * @retval 0:     OK
 * * @retval -1:    ERROR
 * */
static void aggrDeleteUciEntry(aggrList_S *ag)
{
    char buf[32];
    char memb_s[3*MV88E6097_MAX_MEMBERS_SIZE];
    struct uci_ptr ptr = {
        .package = "switchd",
        .section = "aggregation",
        .option = "entry",
    };

    sprintf(buf,"%d:",ag->id);

    int i;
    for(i = 0;i < ag->mem_num;i++)
    {
        sprintf(memb_s + 3*i,"%02x-",ag->memb.members[i]);
    }
    memb_s[i * 3 - 1] = 0;

    LOG_NORMAL("delete one entry: id = %d , members = %s OK\n",ag->id,memb_s);

    strcat(buf,memb_s);
    ubus_collect_config(&ptr,buf,UCI_TYPE_LIST,DO_DEL);
}

/* *
 * * @brief         delete specified entry(contain driver and uci) 
 * * @retval 0:     OK
 * * @retval -1:    ERROR
 * */
static int aggrDeleteEntry(aggrList_S *ag)
{
    int ret, i, j;

    /**<    delete specified entry in driver  */
    ret = fal_trunk_row_destroy(0,ag->id);
    if(ret)
    {
        LOG_ERROR("multicast destory old entry in driver fail!\n");
        return -1;
    }
    for(i = 0; i < ag->mem_num; i++){//set port status
        for(j = 0; j < 8; j++){
            if((ag->memb.members[i] << j) & 0x80 ){
               if(get_port_status(j+1+(i*8)) == PORT_STATUS_AGGREGATION){ 
                    set_port_status(j+1+(i*8), PORT_STATUS_NORMAL);
               }
            }
        }
    }

    /**<    delete specified entry in UCI file  */
    aggrDeleteUciEntry(ag);
    
    return 0;
}

static int aggrCheckPortStatus(fal_trunk_members_t* memb, int num)
{
    int res,i,j;
    for(i = 0; i < num; i++){//判断 端口 当前状态
        for(j = 0; j < 8; j++){
            if((memb->members[i] << j) & 0x80 ){ 
                res = get_port_status(j+1+(i*8));
                if((res != PORT_STATUS_NORMAL)&&(res != PORT_STATUS_AGGREGATION)){
                    LOG_ERROR("port cur status fail!\n");
                    return -2;
                }
            }
        }
    }
    return 0;
}
static void aggrSetPortStatus(fal_trunk_members_t* memb, int num)
{
    int i, j;
    for(i = 0; i < num; i++){//set port status
        for(j = 0; j < 8; j++){
            if((memb->members[i] << j) & 0x80 ){ 
                set_port_status(j+1+(i*8), PORT_STATUS_AGGREGATION);
            }
        }
    }
}

static int aggrProcessPortStatus(fal_trunk_mode_t aggr_sw, int num)
{
    /**< check port status*/
    int i, ret;
    if(aggr_sw == FAL_TRUNK_MODE_DISABLE){ //aggregation disable
        for(i=1; i<= CONFIG_JWS_PORT_TOTAL_NR; i++){
            if(get_port_status(i) == PORT_STATUS_AGGREGATION){// set port from aggr to nomal
                set_port_status(i,PORT_STATUS_NORMAL);
            }
        }
    }

    int flag = 0;
    /**<    set every entry if status is enable  */
    if((aggr_sw == FAL_TRUNK_MODE_STATIC) && num)
    {
        aggrList_S *ag,*ag_bak;
        list_for_each_entry_safe(ag, ag_bak, &aggr.entry, node)
        {
            if(flag){
                ag->idx--; 
            }
            if(!ag->mem_num)
                continue;

            //判断成员端口当前状态
            ret = aggrCheckPortStatus(&ag->memb, ag->mem_num);
            if(ret){//端口状态不对
                //删除节点
                flag = 1;
                aggrDeleteEntry(ag);
                aggr.aggr_num--;
                list_del(&ag->node);
                free(ag);
                continue;
            }
            ret = fal_trunk_row_create(0,ag->id,&ag->memb,ag->mem_num);
            if(ret)
            {
                LOG_ERROR("set new entry error!\n");
                return -1;
            }
            //设置端口 status          
            aggrSetPortStatus(&ag->memb, ag->mem_num);
        }
    }
    return 0;
}

/* *
 * * @brief     port aggregation get mode select
 * * @retval    0: OK
 * * @retval    -1: ERROR
 * */
static int aggrGetSwitch(struct blob_buf *buf)
{
    char aggr_sw_s[2];

    sprintf(aggr_sw_s,"%d",aggr.aggr_sw);
    blobmsg_add_string(buf, "mode_select", aggr_sw_s);
    LOG_NORMAL("get aggregation mode select = [%s]\n",aggr_sw_s);
    
    return 0;
}

/* *
 * * @brief         port aggregation set mode select
 * * @param[*p]     1-disable 2-static.
 * * @retval 0:     OK
 * * @retval -1:    ERROR
 * */
static int aggrSetSwitch(void *p)
{
    fal_trunk_mode_t aggr_sw = atoi((char *)p);
    sw_error_t ret;

    if(aggr_sw == aggr.aggr_sw)
    {
        LOG_NORMAL("same arguement,do nothing\n");
        return 0;
    }
    else if(aggr_sw != FAL_TRUNK_MODE_STATIC && aggr_sw != FAL_TRUNK_MODE_DISABLE)
    {
        LOG_ERROR("invalid arguement!\n");
        return -1;
    }
    
    ret = fal_trunk_pag_mode_set(0,aggr_sw);
    if(ret)
    {
        LOG_ERROR("set aggregation mode select fail!\n");
        return -1;
    }

    aggr.aggr_sw = aggr_sw;

    char buf[2];
    struct uci_ptr ptr = {
        .package = "switchd",
        .section = "aggregation",
        .option = "mode_select",
    };

    sprintf(buf,"%d",aggr_sw);
    ubus_collect_config(&ptr,buf,UCI_TYPE_STRING,DO_SET);

    aggrProcessPortStatus(aggr_sw, aggr.aggr_num);
    LOG_NORMAL("port aggregation set mode select [%d] OK!\n", aggr_sw);
 
    return 0;
}

/* *
 * * @brief         port aggregation get load balance method 
 * * @retval 0:     OK
 * * @retval -1:    ERROR
 * */
static int aggrGetLoadBalance(struct blob_buf *buf)
{
    char load_balance_s[2];

    sprintf(load_balance_s,"%d",aggr.aggr_lb);
    blobmsg_add_string(buf, "load_balance", load_balance_s);
    LOG_NORMAL("get load balance = [%s]\n",load_balance_s);

    return 0;
}

/* *
 * * @brief         port aggregation set load balance method
 * * @param[*p]     1-xor 2-hash.
 * * @retval 0:     OK
 * * @retval -1:    ERROR
 * */
static int aggrSetLoadBalance(void *p)
{
    int load_balance= atoi((char *)p);
    sw_error_t res;

    if(aggr.aggr_sw != FAL_TRUNK_MODE_STATIC)
    {
        LOG_ERROR("the feature of port aggregation is disable!\n");
        return -1;
    }
    else if(load_balance == aggr.aggr_lb)
    {
        LOG_NORMAL("same arguement,do nothing\n");
        return 0;
    }

    res = fal_trunk_load_balance_set(0,load_balance);
    if(res)
    {
        LOG_ERROR("port aggregation balance error = %d!\n",res);
        return -1;
    }
    
    aggr.aggr_lb = load_balance;
    
    char buf[2];
    struct uci_ptr ptr = {
        .package = "switchd",
        .section = "aggregation",
        .option = "load_balance",
    };

    sprintf(buf,"%d",load_balance);
    ubus_collect_config(&ptr,buf,UCI_TYPE_STRING,DO_SET);

    LOG_NORMAL("port aggregation set load balance [%d] OK!\n", load_balance);
 
    return 0;
}

/* *
 * * @brief         port aggregation get channel number
 * * @retval 0:     OK
 * * @retval -1:    ERROR
 * * @note:         upper limit 999 temporary
 * */
static int aggrGetChannelNum(struct blob_buf *buf)
{
    char channel_num_s[4];

    sprintf(channel_num_s,"%d",aggr.aggr_num);
    LOG_NORMAL("get channel num = [%s]\n",channel_num_s);
    blobmsg_add_string(buf, "channel_num", channel_num_s);

    return 0;
}

/* *
 * * @brief         port aggregation get one channel
 * * @param[id]     channel id saved in structure "aggr"
 * * @retval 0:     OK
 * * @retval -1:    ERROR
 * */
static int aggrGetChannelId(struct blob_buf *buf,int idx)
{
    char id_s[2];

    aggrList_S *ag,*ag_bak;
    list_for_each_entry_safe(ag, ag_bak, &aggr.entry, node)
    {
        if(idx == ag->idx)
        {
            sprintf(id_s,"%d",ag->id);
            blobmsg_add_string(buf, "channel_id", id_s);
            LOG_NORMAL("aggregation get index %d channel id = [%s] OK\n",idx,id_s);
            
            return 0;
        }
    }
            
    blobmsg_add_string(buf, "channel_id", "-1");
    LOG_ERROR("no match index [%d], something error!\n",idx);
    
    return -1;
}

/* *
 * * @brief         port aggregation set channel id
 * * @param[idx]    aggregation index
 * * @retval 0:     OK
 * * @retval -1:    ERROR
 * */
static int aggrSetChannelId(int idx,void *p)
{
    int id_new   = atoi((char *)p);
    
    if(id_new < 1 || id_new > AGGR_MAX_CHANNEL_ID_VAL)
    {
        LOG_ERROR("invalid arguement!\n");
        return -1;
    }

    /**<    foreach every node  */
    aggrList_S *ag,*ag_bak;
    aggrList_S *a = NULL;
    list_for_each_entry_safe(ag, ag_bak, &aggr.entry, node)
    {
        if(idx == ag->idx)
        {
            a = ag;       
        }
        else if(id_new == ag->id)
        {
            a = NULL;
            LOG_ERROR("channel id duplicate!\n");
            break;
        }
    }

    if(a == NULL)
    {
        LOG_ERROR("something error while foreach!\n");
        return -1;
    }

    if(id_new == a->id)
    {
        LOG_NORMAL("same arguement,do nothing\n");
        return 0;
    }        

    /**<    destory old entry if exist  */
    if(a->mem_num)  
    {
        if(a->id >= 1 && a->id <= AGGR_MAX_CHANNEL_ID_VAL)
        {
            if(aggrDeleteEntry(a))
            {
                LOG_ERROR("aggregation delete entry fail!\n");
                return -1;
            }
        }
        
        a->flag = 1;
    }

    a->id = id_new;
    LOG_NORMAL("index [%d] set channel id = %d OK!\n",idx,id_new);
    return 0;
}

/* *
 * * @brief         port aggregation get one channel
 * * @param[id]     channel id saved in structure "aggr"
 * * @retval 0:     OK
 * * @retval -1:    ERROR
 * */
static int aggrGetChannelMember(struct blob_buf *buf,int idx)
{
    char memb_s[3*MV88E6097_MAX_MEMBERS_SIZE];

    aggrList_S *ag,*ag_bak;
    list_for_each_entry_safe(ag, ag_bak, &aggr.entry, node)
    {
        if(idx == ag->idx)
        {
            if(!ag->mem_num)
            {
                blobmsg_add_string(buf, "channel_member", "0");
                LOG_WARN("index [%d] aggregation channel members has not been set!\n",idx);
                return 0;
            }

            int i;
            for(i = 0;i < ag->mem_num;i++)
            {
                sprintf(memb_s + 3*i,"%02x-",ag->memb.members[i]);
            }

            memb_s[i * 3 - 1] = 0;
                     
            blobmsg_add_string(buf, "channel_member", memb_s);
            LOG_NORMAL("index [%d] get aggregation channel members OK\n",idx);
            return 0;
        }
    }

    blobmsg_add_string(buf, "channel_member", "-1");
    LOG_ERROR("no match index [%d], something error!\n",idx);
    return -1;   
}

/* *
 * * @brief         port aggregation set mode select
 * * @param[*p]     1-disable 2-static.
 * * @retval 0:     OK
 * * @retval -1:    ERROR
 * * @note          aggr structure initialize here
 * */
static int aggrSetChannelMember(int idx,void *p)
{
    int num, i, j, res;


    fal_trunk_members_t memb_new;

    num = JW_ObjStrToArray(MV88E6097_MAX_MEMBERS_SIZE,memb_new.members,p);
    if(num == -1)
    {
        LOG_ERROR("analyse aggregation channel members fail!\n");
        return -1;
    }
    int ret;
    //check port 端口状态
    ret = aggrCheckPortStatus(&memb_new, num);
    if(ret){
        LOG_ERROR("check aggregation channel members  port status fail!\n");
        return ret; 
    }
    
    fal_port_type_t type;
	fal_port_comb_t comb_port;
    int mode = 0, mode1;
    //解析member成员
    for(i = 0; i < num; i++){//比较同一聚合组是否包含光口与电口
        for(j = 0; j < 8; j++){
            if((memb_new.members[i] << j) & 0x80 ){ 
                res = fal_port_type_get(0,j+1+(i*8),&type,&comb_port);
                if(res){
                    LOG_ERROR(" get port %d type fail\n", j+1+(i*8));
                    return -1;
                }
                mode1 = type;//当前type
                if(mode == 0){
                    mode = mode1;//首个type
                }
                else if(mode1 != mode){
                    LOG_ERROR("invalid members, FE and GX forbidden in one trunk\n");
                    return -1;
                }
            }
        }
    }

    aggrList_S *ag,*ag_bak, *a = NULL;
    list_for_each_entry_safe(ag, ag_bak, &aggr.entry, node)
    {
        if(idx == ag->idx){
            a = ag;
            continue;
        }else{
            for(i = 0; i < num; i++){//比较端口是否在不同聚合组
                if(memb_new.members[i] & ag->memb.members[i]){
                    LOG_ERROR("invalid members\n");
                    return -1;
                }
            }
        }
    }
    if(a != NULL)
    {
        if(!memcmp(&memb_new,&a->memb,sizeof(fal_trunk_members_t)))
        {
            LOG_NORMAL("same arguement,nothing change\n");
            return 0;
        }

        if(a->id >= 1 && a->id <= AGGR_MAX_CHANNEL_ID_VAL)
        {
            if(a->mem_num)
            {
                /**<    delete specified entry in UCI file  */
                aggrDeleteUciEntry(a);
            }

            a->flag = 1;
        }

        a->mem_num = num;
        memcpy(&a->memb,&memb_new,sizeof(fal_trunk_members_t));
        //set port status
        aggrSetPortStatus(&memb_new, num);

        LOG_NORMAL("set channel members OK!\n");
        return 0;
    }

    LOG_ERROR("no match index [%d], something error!\n",idx);
    return -1;
}

static const struct jw_switch_policy port_aggr_tbl[] = {
    [AGGR_SWITCH] = {.name = "mode_select", .get_ext_handler = aggrGetSwitch, .set_ext_handler = aggrSetSwitch}, 
    [AGGR_LOAD_BALANCE] = {.name = "load_balance", .get_ext_handler = aggrGetLoadBalance, .set_ext_handler = aggrSetLoadBalance},
    [AGGR_GROUP_NUM] = {.name = "channel_num", .get_ext_handler = aggrGetChannelNum},
    [AGGR_GROUP_ID] = {.name = "channel_id", .get_handler = aggrGetChannelId, .set_handler = aggrSetChannelId},
    [AGGR_GROUP_MEMBER] = {.name = "channel_member", .get_handler = aggrGetChannelMember, .set_handler = aggrSetChannelMember},
};

/* *
 * * @brief         port aggregation parse recv form upper
 * * @param[lvl3]   level 3 message
 * * @retval 0:     OK
 * * @retval -1:    ERROR
 * */
static int aggrParseChannelGet(struct blob_attr *lvl3, struct blob_buf *buf)
{
    const char *name  = blobmsg_name(lvl3);
    int idx     = atoi(name + 4);

    if (idx < 1 || idx > aggr.aggr_num) 
    {
        LOG_ERROR("invalid index %d\n", idx);
        return -1;
    }
    
    void *ret_array;

    ret_array= blobmsg_open_array(buf, name);
    foreach_json_array_get(idx,lvl3,buf,port_aggr_tbl, __PORT_AGGR_TBL_MAX);
    blobmsg_close_array(buf, ret_array);	

    return 0; 
}

/* *
 * * @brief         port aggregation parse recv form upper
 * * @param[lvl3]   level 3 message
 * * @retval 0:     OK
 * * @retval -1:    ERROR
 * */
static int aggrParseChannelSet(struct blob_attr *lvl3)
{
    if(aggr.aggr_sw != FAL_TRUNK_MODE_STATIC)
    {
        LOG_ERROR("the feature of aggregation is disable!\n");
        return -1;
    }

    const char *name  = blobmsg_name(lvl3);
    int idx     = atoi(name + 4);

    // note: idx=aggr.aggr_num+1 mean adding a new aggregation channel
    if (idx < 1 || idx > (aggr.aggr_num + 1)) 
    {
        LOG_ERROR("invalid index %d\n", idx);
        return -1;
    }

    /**<    add new index/entry  */
    if(idx == (aggr.aggr_num + 1))
    {
        aggr.aggr_num++;
        aggrList_S *ag = (aggrList_S *)calloc(1,sizeof(aggrList_S));
        list_add_tail(&ag->node,&aggr.entry);
        ag->idx = aggr.aggr_num;
        ag->id = -1;
    }

    /**<    foreach array of level 3 message  */
    int ret;
    ret = foreach_json_array_set(idx,lvl3,port_aggr_tbl, __PORT_AGGR_TBL_MAX);
    if(ret)
    {
        LOG_ERROR("foreach index %d error!\n",idx);
        return -1;
    }

    /**<    create each multicast row which has been changed  */
    aggrList_S *ag,*ag_bak;
    list_for_each_entry_safe(ag, ag_bak, &aggr.entry, node)
    {
        if(ag->flag)
        {
            ret = fal_trunk_row_create(0,ag->id,&ag->memb,ag->mem_num);
            if(ret)
            {
                /**<    add free memory later  */
                LOG_ERROR("set new mac address error!\n");
                ag->flag = 0;
                return -1;
            }
            ag->flag = 0;

            char buf[32];
            char memb_s[3*MV88E6097_MAX_MEMBERS_SIZE];
            struct uci_ptr ptr = {
                .package = "switchd",
                .section = "aggregation",
                .option = "entry",
            };

            sprintf(buf,"%d:",ag->id);

            int i;
            for(i = 0;i < ag->mem_num;i++)
            {
                sprintf(memb_s + 3*i,"%02x-",ag->memb.members[i]);
            }
            memb_s[i * 3 - 1] = 0;

            LOG_NORMAL("set one entry: id = %d , members = %s OK\n",ag->id,memb_s);

            strcat(buf,memb_s);
            ubus_collect_config(&ptr,buf,UCI_TYPE_LIST,DO_SET);
        }
    }

    return 0; 
}

/* *
 * * @brief         port aggregation parse recv form upper
 * * @param[lvl2]   level 2 message
 * * @retval 0:     OK
 * * @retval -1:    ERROR
 * */
static int aggrParseChannelClear(struct blob_attr *lvl2)
{
    char *value = blobmsg_get_string(lvl2);
    int idx     = atoi(value + 4);
    int flag = 0;

    if (idx < 1 || idx > aggr.aggr_num) 
    {
        LOG_ERROR("invalid index %d\n", idx);
        return -1;
    }

    aggrList_S *ag,*ag_bak;
    list_for_each_entry_safe(ag, ag_bak, &aggr.entry, node)
    {
        if(flag)
        {
            ag->idx--;
        }
        else if(idx == ag->idx)
        {
            if(ag->mem_num && (ag->id >= 1 && ag->id <= AGGR_MAX_CHANNEL_ID_VAL)) 
            {
                if(aggrDeleteEntry(ag))
                {
                    LOG_ERROR("aggregation delete entry fail!\n");
                    return -1;
                }
            }

            flag = 1;
            aggr.aggr_num--;
            list_del(&ag->node);
            free(ag);
        }
    }

    if(flag)
    {
        LOG_NORMAL("destroy one aggregation entry which index = %d OK\n",idx);
        return 0;
    }

    LOG_ERROR("no match index [%d], something error!\n",idx);
    return -1;
}

/* *
 * * @brief         method "port_aggr_get" handler
 * * @retval 0:     OK
 * * @retval -1:    ERROR
 * */
static int aggrGetHandler(struct ubus_context *ctx, struct ubus_object *obj,
                struct ubus_request_data *req, const char *method,
                struct blob_attr *msg)
{
    struct blob_attr *tb[__PORT_AGGR_GET_MAX];    
	struct blob_attr *lvl1 = NULL;  

    blobmsg_parse(port_aggr_get_policy, __PORT_AGGR_GET_MAX, tb, blob_data(msg), blob_len(msg));
	lvl1 = tb[PORT_AGGR_GET_KEY];     
    if (!lvl1) 
	{
        LOG_ERROR("invalid params in %s\n",__func__);
        return -1;
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
    
	blob_buf_init(&b, 0);
    ret_array = blobmsg_open_array(&b, "ret"); 

    blobmsg_for_each_attr(lvl2,lvl1, rem) 
    {
        switch (blobmsg_type(lvl2))
        {
            case BLOBMSG_TYPE_STRING:
            {
                struct jw_switch_policy *policy_p;
                char *name = blobmsg_get_string(lvl2);

                 
                policy_p = (struct jw_switch_policy *)jw_switchd_get_context(name, port_aggr_tbl, __PORT_AGGR_TBL_MAX);
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
                    aggrParseChannelGet(lvl3,&b);
                    blobmsg_close_table(&b, ret_table);	
                }
                else
                {
                    LOG_ERROR("level 3 message type is not available!\n");
                }
            }
                break;
            default:
                LOG_ERROR("level 2 message type is not available!\n");
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
static int aggrSetHandler(struct ubus_context *ctx, struct ubus_object *obj,
                struct ubus_request_data *req, const char *method,
                struct blob_attr *msg)
{
    struct blob_attr *tb[__PORT_AGGR_GET_MAX];    
	struct blob_attr *lvl1 = NULL;  
    int ret = -1;

    blobmsg_parse(port_aggr_set_policy, __PORT_AGGR_SET_MAX, tb, blob_data(msg), blob_len(msg));
	lvl1 = tb[PORT_AGGR_SET_KEY];     
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

                    policy_p = (struct jw_switch_policy *)jw_switchd_get_context(name, port_aggr_tbl, __PORT_AGGR_TBL_MAX);
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
                    ret = aggrParseChannelSet(lvl3);
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
        }
    }

end:
    blobmsg_add_string(&b, "ret", ret?"-1":"0");
	ubus_send_reply(ctx, req, b.head);

	return UBUS_STATUS_OK;
}

/* *
 * * @brief         method "port_aggr_clear" handler
 * * @retval 0:     OK
 * * @retval -1:    ERROR
 * */
static int aggrClearHandler(struct ubus_context *ctx, struct ubus_object *obj,
                struct ubus_request_data *req, const char *method,
                struct blob_attr *msg)
{
    struct blob_attr *tb[__PORT_AGGR_CLEAR_MAX];    
	struct blob_attr *lvl1 = NULL;  
    int ret = -1;

    blobmsg_parse(port_aggr_clear_policy, __PORT_AGGR_CLEAR_MAX, tb, blob_data(msg), blob_len(msg));
	lvl1 = tb[PORT_AGGR_CLEAR_KEY];     
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
                        
            ret = aggrParseChannelClear(lvl2);
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
 
static const struct ubus_method port_aggr_methods[] = {
	UBUS_METHOD("port_aggr_get", aggrGetHandler, port_aggr_get_policy),
	UBUS_METHOD("port_aggr_set", aggrSetHandler, port_aggr_set_policy),
	UBUS_METHOD("port_aggr_clear", aggrClearHandler, port_aggr_clear_policy),
};


static struct ubus_object_type port_aggr_object_type =
	UBUS_OBJECT_TYPE("port_aggr", port_aggr_methods);


static struct ubus_object port_aggr_object = {
	.name = "port_aggr",
	.type = &port_aggr_object_type,
	.methods = port_aggr_methods,
	.n_methods = ARRAY_SIZE(port_aggr_methods),
};

/* *
 * * @brief             load aggr arguement - status from UCI file
 * * @param[*p]         buffer to be parse 
 * * @retval 0:         OK
 * * @retval -1:        ERROR
 * */
static int aggrLoadMode(struct uci_ptr *ptr)
{
    int status = atoi(ptr->o->v.string);
    int ret;

    if(status != FAL_TRUNK_MODE_STATIC && status != FAL_TRUNK_MODE_DISABLE)
    {
        LOG_ERROR("invalid arguement!\n"); 
        return -1;
    }

    ret = fal_trunk_pag_mode_set(0,status);
    if(ret)
    {
        LOG_ERROR("set aggregation status fail!\n");
        return -1;
    }

    aggr.aggr_sw = status;
    LOG_NORMAL("aggr load status = [%d] OK\n", status);
    
    return 0;
}

/* *
 * * @brief             load aggr arguement - load balance from UCI file
 * * @param[*p]         buffer to be parse 
 * * @retval 0:         OK
 * * @retval -1:        ERROR
 * */
static int aggrLoadBalance(struct uci_ptr *ptr)
{
    int balance = atoi(ptr->o->v.string);
    int ret;

    if(balance != FAL_TRUNK_LOAD_BALANCE_XOR && balance != FAL_TRUNK_LOAD_BALANCE_HASH)
    {
        LOG_ERROR("invalid arguement!\n"); 
        return -1;
    }

    if(aggr.aggr_sw == FAL_TRUNK_MODE_STATIC)
    {
        ret = fal_trunk_load_balance_set(0,balance);
        if(ret)
        {
            LOG_ERROR("set aggregation load balance fail!\n");
            return -1;
        }
    }

    aggr.aggr_lb = balance;
    LOG_NORMAL("aggr load balance = [%d] OK\n", balance);
    
    return 0;
}

/* *
 * * @brief             load aggr arguement - entry list from UCI file
 * * @param[*p]         buffer to be parse 
 * * @retval 0:         OK
 * * @retval -1:        ERROR
 * */
static int aggrLoadEntry(struct uci_ptr *ptr)
{
//    int ret;
    int count = 0;

    struct uci_element *elt;

    uci_foreach_element(&ptr->o->v.list,elt)
    {
        /**<    separate channel id and members  */
        char *pdelim = strchr(elt->name,':');
        if(pdelim == NULL)
        {
            LOG_ERROR("aggr entry list format in UCI file error!\n"); 
            return -1;
        }

        pdelim[0] = 0;

        /**<    check if channel and members are valid  */
        int id = atoi(elt->name);
        if(id < 0 || id > AGGR_MAX_CHANNEL_ID)
        {
            LOG_ERROR("parse aggr entry channel id format in UCI file error!\n"); 
            return -1;
        }

        fal_trunk_members_t memb;
        int num = JW_ObjStrToArray(MV88E6097_MAX_MEMBERS_SIZE,memb.members,pdelim + 1);
        if(num == -1)
        {
            LOG_ERROR("parse aggr entry members fail!\n");
            return -1;
        }

#if 0   /**<  移到链表中处理*/
        /**<    set entry to driver  */

        if(aggr.aggr_sw == FAL_TRUNK_MODE_STATIC)
        {
            ret = fal_trunk_row_create(0,id,&memb,num);
            if(ret)
            {
                LOG_ERROR("set new entry error!\n");
                return -1;
            }
        }
#endif
        /**<    link entry to list  */
        aggrList_S *ag = (aggrList_S *)calloc(1,sizeof(aggrList_S));
        list_add_tail(&ag->node,&aggr.entry);

        ag->id = id;
        memcpy(&ag->memb,&memb,sizeof(fal_trunk_members_t));
        ag->mem_num = num;
        ag->idx = ++count;
    }

    aggr.aggr_num = count;
#if 0
    /**<    set every entry if status is enable  */
    if(aggr.aggr_sw == FAL_TRUNK_MODE_STATIC && aggr.aggr_num){
        /**<   查询链表*/
        aggrList_S *ag,*ag_bak;
        list_for_each_entry_safe(ag, ag_bak, &aggr.entry, node)
        {
            if(!ag->mem_num)
                continue;

            //判断成员端口当前状态
            ret = aggrCheckPortStatus(&ag->memb, ag->mem_num);
            if(ret){//端口状态不对
                //删除节点
                ag->idx--;
                aggrDeleteEntry(ag);
                aggr.aggr_num--;
                list_del(&ag->node);
                free(ag);
                continue;
            }
            ret = fal_trunk_row_create(0,ag->id,&ag->memb,ag->mem_num);
            if(ret)
            {
                LOG_ERROR("set new entry error!\n");
                return -1;
            }
            //设置端口 status          
            aggrSetPortStatus(&ag->memb, ag->mem_num);
        }
    }
#endif

    LOG_NORMAL("aggr load entry OK\n");
    
    return 0;
}
static UciConfig_S aggrUci[] = {
    {.name = "mode_select",     .type = UCI_TYPE_STRING,    .ubusGet = aggrLoadMode},
    {.name = "load_balance",    .type = UCI_TYPE_STRING,    .ubusGet = aggrLoadBalance},
    {.name = "entry",           .type = UCI_TYPE_LIST,      .ubusGet = aggrLoadEntry},
};

static int aggrCfgLoad(void)
{   
    memset(&aggr,0,sizeof(aggr));
    INIT_LIST_HEAD(&aggr.entry);

    struct uci_context *ctx = uci_alloc_context();

    char name[64]; 
    int ret;
    int i;
    struct uci_ptr ptr;

    for(i = 0;i < sizeof(aggrUci)/sizeof(UciConfig_S);i++)
    {
        sprintf(name,"switchd.aggregation.%s",aggrUci[i].name);
        memset(&ptr, 0, sizeof(struct uci_ptr));
        ret = load_config(name,aggrUci[i].type,&ptr,ctx);
        if(ret)
        {

            if(aggrUci[i].type == UCI_TYPE_LIST) continue;

            LOG_ERROR("load config %s fail!\n",name); 
            uci_free_context(ctx);
            return -1;
        }
        
        ret = aggrUci[i].ubusGet(&ptr);
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

void ubus_init_port_aggr(struct ubus_context *ctx)
{
	int ret;

	_ctx = ctx;
    
    ret = aggrCfgLoad();
    if(ret)
    {
		LOG_ERROR("aggr load configure from UCI file fail\n");
        return;
    }

    ret = aggrProcessPortStatus(aggr.aggr_sw, aggr.aggr_num);
    if(ret)
    {
		LOG_ERROR("aggr load configure from UCI file fail\n");
        return;
    }

	ret = ubus_add_object(ctx, &port_aggr_object);
	if (ret)
		LOG_ERROR("Failed to add object [port_aggr]: %s\n", ubus_strerror(ret));
}
