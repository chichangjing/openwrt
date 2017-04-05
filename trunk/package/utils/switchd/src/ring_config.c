/* *< 中威文件编码格式为gb2312,cp936 */
/* * $Id$ $DateTime$
 * * @file ring_config.c
 * * @brief This is a table of the switch's port rate limite.
 * * @version 0.0.1
 * * @since 0.0.1
 * * @author
 * * @date 2016-08-11
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
 * For details, see http://joyware.com
 * *******************************************************************/

#include <sys/utsname.h>
#include <sys/sysinfo.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>

#include "switchd.h"
#include "ring_comm.h"

static struct blob_buf b;
static struct ubus_context *_ctx;

typedef struct{
    int idx;
    int flag;
    int ring_id;
    int primary_port;
    int secondary_port;
    int priority;
    int admin;
    char ring_name[16];
    struct list_head node;
}ringList_S;

static struct{
    int status;
    int num;
    struct list_head entry;
}ringCfg;

struct getPara{
    char *name;
    struct uci_ptr ptr;
};

//过滤策略
enum {
    RING_CFG_GET_KEY,
    __RING_CFG_GET_MAX
};

enum {
    RING_CFG_SET_KEY,
    __RING_CFG_SET_MAX
};

enum {
    RING_CFG_CLEAR_KEY,
    __RING_CFG_CLEAR_MAX
};

static const struct blobmsg_policy ring_config_get_policy[] = {
    [RING_CFG_GET_KEY] = {.name = "get_args", .type = BLOBMSG_TYPE_ARRAY},
};

static const struct blobmsg_policy ring_config_set_policy[] = {
    [RING_CFG_SET_KEY] = {.name = "set_args", .type = BLOBMSG_TYPE_ARRAY},
};

static const struct blobmsg_policy ring_config_clear_policy[] = {
    [RING_CFG_CLEAR_KEY] = {.name = "clear_args", .type = BLOBMSG_TYPE_ARRAY},
};

enum {
    JRPP_ADMIN_STATUS,
    JRPP_RING_NUM,
    JRPP_RING_NAME,
//    JRPP_RING_ID,
    JRPP_RING_PRIMARY,
    JRPP_RING_SECOND,
    JRPP_RING_PRIORITY,
    JRPP_RING_ADMIN,
    JRPP_RING_RELOAD,
    JRPP_RING_TOPO,
    __JRPP_CFG_TBL_MAX,
};

#define JRPP_PRIORITY_VALUE1 "low"
#define JRPP_PRIORITY_VALUE2 "medium"
#define JRPP_PRIORITY_VALUE3 "high"


#define JRPP_ST_ADMIN_VALUE1 "enable"
#define JRPP_ST_ADMIN_VALUE2 "disable"


static int ringCfgLoad(void);
    
char del_name[16];
char set_name[16];

/* *
 * * @brief         delete specified section in UCI 
 * * @retval 0:     OK
 * * @retval -1:    ERROR
 * */
static int jrppDeleteUciSection(ringList_S *ri)
{
    //int ret;
    if(get_port_status(ri->primary_port) == PORT_STATUS_RING){
        set_port_status(ri->primary_port, PORT_STATUS_NORMAL);
    }
    if(get_port_status(ri->secondary_port) == PORT_STATUS_RING){
        set_port_status(ri->secondary_port, PORT_STATUS_NORMAL);
    }
    sprintf(del_name, "%s", ri->ring_name);
    
    /*ret = cmd_del_ring(atoi(del_name + 4));
    if(ret){
        LOG_ERROR("del ring %d failed\n",atoi(del_name + 4));
        return -1;
    }*/
    struct uci_ptr ptr = {
        .package = "jrppd",
        .section = strdup(del_name),
    };
    LOG_NORMAL("package = %s, section = %s\n", ptr.package,ptr.section);

    ubus_collect_config(&ptr,NULL,UCI_TYPE_STRING,DO_DEL);
    return 0;
}

static int jrppCheckPortStatus(int primaryPort, int secondPort)
{
    if((get_port_status(primaryPort) != PORT_STATUS_NORMAL) && (get_port_status(primaryPort) != PORT_STATUS_RING)
        && (get_port_status(secondPort) != PORT_STATUS_NORMAL) && (get_port_status(secondPort) != PORT_STATUS_RING))
    {
        LOG_ERROR("check port cur status fail!\n");
        return -2;
    }

    return 0;
}

static void jrppSetPortStatus(int primaryPort, int secondPort)
{
    set_port_status(primaryPort, PORT_STATUS_RING);
    set_port_status( secondPort, PORT_STATUS_RING);
}

static void jrppProcessFreeEntry()
{
    int i = 0;
    /**< 1.端口类型更改为空闲*/
    for(i = 1; i <= CONFIG_JWS_PORT_TOTAL_NR; i++){
        if(get_port_status(i) == PORT_STATUS_RING){
            set_port_status(i, PORT_STATUS_NORMAL);
        }
    }

    /**< 2.清空链表*/
    ringList_S *ri,*ri_bak;
    list_for_each_entry_safe(ri, ri_bak, &ringCfg.entry, node)
    {
        /**< 删除节点*/
     //   jrppDeleteUciSection(ri);
        ringCfg.num--;
        list_del(&ri->node);
        free(ri);
    }
    ringCfg.num = 0;

}
static void jrppProcessPortStatus(int status, int num)
{
    int i, ret;
    /**< when jrpp admin status is disable trans port status from ring to normal*/
    if(status == FAL_DISABLE){
        for(i = 1; i <= CONFIG_JWS_PORT_TOTAL_NR; i++){
            if(get_port_status(i) == PORT_STATUS_RING){
                set_port_status(i, PORT_STATUS_NORMAL);
            }
        }
    } 
    int flag = 0;
    /**< set port status if jrpp admin enabl*/
    if((status == FAL_ENABLE) && (num)){
        /** < 遍历链表*/ 
        ringList_S *ri,*ri_bak;
        list_for_each_entry_safe(ri, ri_bak, &ringCfg.entry, node)
        {
            if(flag)
            {
                ri->idx--;
            }
            /**<  check ring port status*/
            ret = jrppCheckPortStatus(ri->primary_port, ri->secondary_port);
            if(ret){ /**< 端口状态不对*/
                /**< 删除节点*/
                flag = 1;
                jrppDeleteUciSection(ri);
                ringCfg.num--;
                list_del(&ri->node);
                free(ri);
                continue;
            }
            /**< set ring port status*/
            jrppSetPortStatus(ri->primary_port, ri->secondary_port);
        }
    }
}
/* *
 * * @brief         get jrpp admin status
 * * @retval 0:     OK
 * * @retval -1:    ERROR
 * */
static int jrppGetAdminStatus(struct blob_buf *buf)
{
    char admin_st_s[2];

    sprintf(admin_st_s,"%d",ringCfg.status);
    blobmsg_add_string(buf, "jrpp_status", admin_st_s);
    LOG_NORMAL("get jrpp admin status = [%s] OK\n",admin_st_s);

    return 0;
}

/* *
 * * @brief         set jrpp admin status
 * * @retval 0:     OK
 * * @retval -1:    ERROR
 * */
static int jrppSetAdminStatus(void *p)
{
    int ret;
    int status = atoi((char *)p);

    if(status != FAL_DISABLE && status != FAL_ENABLE)
    {
        LOG_ERROR("invalid arguement!\n");
        return -1;
    }

    if(status == ringCfg.status)
    {
        LOG_NORMAL("same arguement,do nothing\n");
        return 0;
    }
/*  
    int value = 0;
    switch(status){
        case FAL_ENABLE:
            value = 1;
            break;
        case FAL_DISABLE:
            value = 0;
            break;
        default:
            break;
    }
    ret = cmd_jrpp(value);
    if(ret){
        LOG_ERROR("jrpp set status failed\n");
        return -1;
    }
*/    
    ringCfg.status = status;

     struct uci_ptr ptr = {
        .package = "jrppd",
        .section = "admin",
        .option  = "jrpp",
    };
    
    char buf[8];
    sprintf(buf, "%s", (status == FAL_ENABLE)?"on":"off");
    ubus_collect_config(&ptr,buf,UCI_TYPE_STRING,DO_SET);

    if(status == FAL_DISABLE){ 
        ringList_S *ri,*ri_bak;
        list_for_each_entry_safe(ri, ri_bak, &ringCfg.entry, node)
        {
            if(ri->priority && ri->ring_name && ri->secondary_port && ri->primary_port)
            {
                ri->admin = FAL_DISABLE;
                char ribuf[8];
                sprintf(set_name, "%s", ri->ring_name);
                struct uci_ptr ptr1 = {
                    .package = "jrppd",
                    .section = strdup(set_name),
                    .option = "admin",
                };

                sprintf(ribuf,"%s",JRPP_ST_ADMIN_VALUE2);
                ubus_collect_config(&ptr1,ribuf,UCI_TYPE_STRING,DO_SET);

            }
        }
    }
   
    jrppProcessPortStatus(ringCfg.status, ringCfg.num);


    LOG_NORMAL("jrpp set status = [%d] OK\n", status);

    return 0;
}

/* *
 * * @brief         get ring section nember
 * * @retval 0:     OK
 * * @retval -1:    ERROR
 * */
static int jrppGetRingNum(struct blob_buf *buf)
{
    char num_s[2];

    sprintf(num_s,"%d",ringCfg.num);
    blobmsg_add_string(buf, "ring_num", num_s);
    LOG_NORMAL("jrpp get ring section num = [%s] OK\n",num_s);

    return 0;
}

/* *
 * * @brief         reload ring uci
 * * @retval 0:     OK
 * * @retval -1:    ERROR
 * */
static int jrppGetRingReload(struct blob_buf *buf)
{
    char status_s[2];
    int ret;
    
    jrppProcessFreeEntry();
    ret = ringCfgLoad();
    jrppProcessPortStatus(ringCfg.status, ringCfg.num);

    sprintf(status_s,"%d",ret);
    blobmsg_add_string(buf, "reload", status_s);
    LOG_NORMAL("jrpp reload ring uci = [%s] OK\n",status_s);
    return 0;
}

/* *
 * * @brief         get jrpp ring name
 * * @retval 0:     OK
 * * @retval -1:    ERROR
 * */
static int jrppGetRingName(struct blob_buf *buf, int idx)
{
    ringList_S *ri,*ri_bak;
    list_for_each_entry_safe(ri, ri_bak, &ringCfg.entry, node)
    {
        if(idx == ri->idx)
        {
            char ring_name_s[16];

            sprintf(ring_name_s,"%s",ri->ring_name);
            blobmsg_add_string(buf, "ring_name", ring_name_s);
            LOG_NORMAL("jrpp get idx %d ring_name = [%s] OK\n",idx, ring_name_s);
            
            return 0;
        }
    }

    blobmsg_add_string(buf,"ring_name","-1");
    LOG_ERROR("no match idx [%d] ring_name, something error!\n",idx);

    return -1;
}

/* *
 * * @brief         set jrpp ring name
 * * @retval 0:     OK
 * * @retval -1:    ERROR
 * */
static int jrppSetRingName(int idx, void *p)
{
    char* ringName = (char *)p;
    int ringId     = atoi(ringName + 4);
    /*
       if( ringId< 0 || ringId > MAX_PORT_PRIO_NUM)
       {
       LOG_ERROR("invalid arguement!\n");
       return -1;
       }*/
    ringList_S *ri,*ri_bak;
    ringList_S *r = NULL;
    list_for_each_entry_safe(ri, ri_bak, &ringCfg.entry, node)
    {
        if(idx == ri->idx) 
        {
            r = ri;
        }
        else if(ringId == atoi(ri->ring_name + 4)) //环网name唯一
        {
            r = NULL;
            LOG_ERROR("ring_name duplicate!\n");
            break;
        }
    }
    if(r == NULL)
    {
        LOG_ERROR("something error while eachwhile\n");
        return -1;
    }
    if(!memcmp(&r->ring_name, ringName, strlen(ringName)))
    {
        LOG_NORMAL("same arguement,do nothing\n");
        return 0;
    }
    
    if(r->priority && r->primary_port && r->secondary_port && r->admin)
    {
        if(r->ring_name)
        {
            /**<    delete specified entry in UCI file  */
            jrppDeleteUciSection(r);
        }

        r->flag = 1;
    }
    memcpy(&r->ring_name, ringName, strlen(ringName));
    LOG_NORMAL("jrpp set idx [%d] ring_name = [%s] OK\n",idx, ringName);
    
    return 0;
}

#if 0
/* *
 * * @brief         get jrpp ring id
 * * @retval 0:     OK
 * * @retval -1:    ERROR
 * */
static int jrppGetRingId(struct blob_buf *buf, int idx)
{
    ringList_S *ri,*ri_bak;
    list_for_each_entry_safe(ri, ri_bak, &ringCfg.entry, node)
    {
        if(idx == ri->idx)
        {
            char ring_id_s[2];

            sprintf(ring_id_s,"%d",ri->ring_id);
            blobmsg_add_string(buf, "ring_id", ring_id_s);
            LOG_NORMAL("jrpp get ring %d ring_id = [%s] OK\n",idx, ring_id_s);
            
            return 0;
        }
    }

    blobmsg_add_string(buf,"ring_id","-1");
    LOG_ERROR("no match ring [%d] id, something error!\n",idx);

    return -1;
}

/* *
 * * @brief         set jrpp ring id
 * * @retval 0:     OK
 * * @retval -1:    ERROR
 * */
static int jrppSetRingId(int idx, void *p)
{
    int ringId = atoi((char *)p);
    /*
       if( ringId< 0 || ringId > MAX_PORT_PRIO_NUM)
       {
       LOG_ERROR("invalid arguement!\n");
       return -1;
       }*/
    ringList_S *ri,*ri_bak;
    ringList_S *r = NULL;
    list_for_each_entry_safe(ri, ri_bak, &ringCfg.entry, node)
    {
        if(idx == ri->idx) 
        {
            r = ri;
        }
        else if(ringId == ri->ring_id) //环网ID唯一
        {
            r = NULL;
            LOG_ERROR("ring_id duplicate!\n");
            break;
        }
    }
    if(r == NULL)
    {
        LOG_ERROR("something error while eachwhile\n");
        return -1;
    }
    if(ringId == r->ring_id)
    {
        LOG_NORMAL("same arguement,do nothing\n");
        return 0;
    }
    r->ring_id = ringId;

    char sect[8];
    sprintf(sect,"%s",r->ring_name);

    char buf[2];
    struct uci_ptr ptr = {
        .package = "jrppd",
        .section = r->ring_name,
        .option = "ring_id",
    };

    sprintf(buf,"%d",ringId);
    ubus_collect_config(&ptr,buf,UCI_TYPE_STRING,DO_SET);

    LOG_NORMAL("jrpp set idx[%d] [%s] ring_id = [%d] OK\n",idx, sect, ringId);
    return 0;
}
#endif

/* *
 * * @brief         get jrpp ring primary_port
 * * @retval 0:     OK
 * * @retval -1:    ERROR
 * */
static int jrppGetRingPrimary(struct blob_buf *buf, int idx)
{
    ringList_S *ri,*ri_bak;
    list_for_each_entry_safe(ri, ri_bak, &ringCfg.entry, node)
    {
        if(idx == ri->idx)
        {
            char ring_primary_s[2];

            sprintf(ring_primary_s,"%d",ri->primary_port);
            blobmsg_add_string(buf, "primary_port", ring_primary_s);
            LOG_NORMAL("jrpp get ring %d primary_port = [%s] OK\n",idx, ring_primary_s);
            
            return 0;
        }
    }

    blobmsg_add_string(buf,"primary_port","-1");
    LOG_ERROR("no match ring [%d] primary_port, something error!\n",idx);

    return -1;
}

/* *
 * * @brief         set jrpp ring primary_port
 * * @retval 0:     OK
 * * @retval -1:    ERROR
 * */
static int jrppSetRingPrimary(int idx, void *p)
{
    int primaryPort = atoi((char *)p);
    /**< check port cur status*/
    int port_status = get_port_status(primaryPort);
    if((port_status != PORT_STATUS_NORMAL) && (port_status != PORT_STATUS_RING)){
        LOG_ERROR("set port %d cur port status fail\n", primaryPort);
        return -2;
    }
    ringList_S *ri,*ri_bak;
    list_for_each_entry_safe(ri, ri_bak, &ringCfg.entry, node)
    {
        if(idx == ri->idx) 
        {
            int primaryPort_old = ri->primary_port;
            if(primaryPort == primaryPort_old){
                LOG_NORMAL("same arguement,do nothing\n");
                return 0;
            }
            if(ri->priority && ri->ring_name && ri->secondary_port && ri->admin)
            {
                if(ri->primary_port)
                {
                    /**<    delete specified entry in UCI file  */
                    jrppDeleteUciSection(ri);
                }

                ri->flag = 1;
            }
            ri->primary_port = primaryPort;
            set_port_status(primaryPort, PORT_STATUS_RING);
            LOG_NORMAL("jrpp set idx [%d] primary_port = [%d] OK\n", idx, primaryPort);
            return 0;
        }
        else{
            if((primaryPort == ri->primary_port) || (primaryPort == ri->secondary_port)){
                LOG_ERROR("port[%d] has been set ring port already, set port error\n",primaryPort);
                return -1;
            }
        }
    }

    LOG_ERROR("no match ring [%d] , something error\n",idx);
    return -1;
}

/* *
 * * @brief         get jrpp ring secondary_port
 * * @retval 0:     OK
 * * @retval -1:    ERROR
 * */
static int jrppGetRingSecondary(struct blob_buf *buf, int idx)
{
    ringList_S *ri,*ri_bak;
    list_for_each_entry_safe(ri, ri_bak, &ringCfg.entry, node)
    {
        if(idx == ri->idx)
        {
            char ring_secondary_s[2];

            sprintf(ring_secondary_s,"%d",ri->secondary_port);
            blobmsg_add_string(buf, "secondary_port", ring_secondary_s);
            LOG_NORMAL("jrpp get ring %d secondary_port = [%s] OK\n",idx, ring_secondary_s);
            
            return 0;
        }
    }

    blobmsg_add_string(buf,"secondary_port","-1");
    LOG_ERROR("no match ring [%d] secondary_port, something error!\n",idx);

    return -1;
}

/* *
 * * @brief         set jrpp ring secondary_port
 * * @retval 0:     OK
 * * @retval -1:    ERROR
 * */
static int jrppSetRingSecondary(int idx, void *p)
{
    int secondPort = atoi((char *)p);
    
    /**< check port cur status*/
    int port_status = get_port_status(secondPort);
    if((port_status != PORT_STATUS_NORMAL) && (port_status != PORT_STATUS_RING)){
        LOG_ERROR("set port %d cur port status fail\n", secondPort);
        return -2;
    }
    ringList_S *ri,*ri_bak;
    list_for_each_entry_safe(ri, ri_bak, &ringCfg.entry, node)
    {
        if(idx == ri->idx) 
        {
            int secondPort_old = ri->secondary_port;
            if(secondPort == secondPort_old){
                LOG_NORMAL("same arguement,do nothing\n");
                return 0;
            }
            if(ri->priority && ri->ring_name && ri->primary_port && ri->admin)
            {
                if(ri->secondary_port)
                {
                    /**<    delete specified entry in UCI file  */
                    jrppDeleteUciSection(ri);
                }

                ri->flag = 1;
            }
            ri->secondary_port = secondPort;
            set_port_status(secondPort, PORT_STATUS_RING);
            LOG_NORMAL("jrpp set idx [%d] secondary_port = [%d] OK\n",idx, secondPort);
            return 0;
        }
        else{
            if((secondPort == ri->primary_port) || (secondPort == ri->secondary_port)){
                LOG_ERROR("port[%d] has been set ring port already, set port error\n",secondPort);
                return -1;
            }
        }
    }

    LOG_ERROR("no match ring [%d] , something error\n",idx);
    return -1;
}

/* *
 * * @brief         get jrpp ring priority
 * * @retval 0:     OK
 * * @retval -1:    ERROR
 * */
static int jrppGetRingPriority(struct blob_buf *buf, int idx)
{
    ringList_S *ri,*ri_bak;
    list_for_each_entry_safe(ri, ri_bak, &ringCfg.entry, node)
    {
        if(idx == ri->idx)
        {
            char ring_priority_s[2];

            sprintf(ring_priority_s,"%d",ri->priority);
            blobmsg_add_string(buf, "priority", ring_priority_s);
            LOG_NORMAL("jrpp get ring %d priority = [%s] OK\n",idx, ring_priority_s);
            
            return 0;
        }
    }

    blobmsg_add_string(buf,"priority","-1");
    LOG_ERROR("no match ring [%d] priority, something error!\n",idx);

    return -1;
}

/* *
 * * @brief         set jrpp ring priority
 * * @retval 0:     OK
 * * @retval -1:    ERROR
 * */
static int jrppSetRingPriority(int idx, void *p)
{
    int priorityVal = atoi((char *)p);
    if(priorityVal != 1 && priorityVal != 2 && priorityVal != 3){
        LOG_ERROR("invalid arguement!\n");
        return -1;
    }
    
    ringList_S *ri,*ri_bak;
    list_for_each_entry_safe(ri, ri_bak, &ringCfg.entry, node)
    {
        if(idx == ri->idx) 
        {
            int priority_old = ri->priority;
            if(priorityVal == priority_old){
                LOG_NORMAL("same arguement,do nothing\n");
                return 0;
            }
            if(ri->primary_port && ri->ring_name && ri->secondary_port && ri->admin)
            {
                if(ri->priority)
                {
                    /**<    delete specified entry in UCI file  */
                    jrppDeleteUciSection(ri);
                }

                ri->flag = 1;
            }
            ri->priority = priorityVal;
            LOG_NORMAL("jrpp set idx [%d] priority = [%d] OK\n",idx, priorityVal);
            return 0;
        }
    }

    LOG_ERROR("no match ring [%d] , something error\n",idx);
    return -1;
}

/* *
 * * @brief         get jrpp ring admin
 * * @retval 0:     OK
 * * @retval -1:    ERROR
 * */
static int jrppGetRingAdmin(struct blob_buf *buf, int idx)
{
    ringList_S *ri,*ri_bak;
    list_for_each_entry_safe(ri, ri_bak, &ringCfg.entry, node)
    {
        if(idx == ri->idx)
        {
            char ring_admin_s[2];

            sprintf(ring_admin_s,"%d",ri->admin);
            blobmsg_add_string(buf, "admin", ring_admin_s);
            LOG_NORMAL("jrpp get ring %d admin = [%s] OK\n",idx, ring_admin_s);
            
            return 0;
        }
    }

    blobmsg_add_string(buf,"admin","-1");
    LOG_ERROR("no match ring [%d] admin, something error!\n",idx);

    return -1;
}

/* *
 * * @brief         set jrpp ring admin 
 * * @retval 0:     OK
 * * @retval -1:    ERROR
 * */
static int jrppSetRingAdmin(int idx, void *p)
{
    int adminVal = atoi((char *)p);
    if(adminVal != FAL_ENABLE && adminVal != FAL_DISABLE){
        LOG_ERROR("invalid arguement!\n");
        return -1;
    }
    
    ringList_S *ri,*ri_bak;
    list_for_each_entry_safe(ri, ri_bak, &ringCfg.entry, node)
    {
        if(idx == ri->idx) 
        {
            int admin_old = ri->admin;
            if(adminVal == admin_old){
                LOG_NORMAL("same arguement,do nothing\n");
                return 0;
            }
            if(ri->priority && ri->ring_name && ri->secondary_port && ri->primary_port)
            {
                if(ri->admin)
                {
                    /**<    delete specified entry in UCI file  */
                    jrppDeleteUciSection(ri);
                }

                ri->flag = 1;
            }
            ri->admin = adminVal;
            LOG_NORMAL("jrpp set idx [%d] admin = [%d] OK\n",idx, adminVal);
            return 0;
        }
    }


    LOG_ERROR("no match ring [%d] , something error\n",idx);
    return -1;
}

/* *
 * * @brief         get jrpp ring topo
 * * @retval 0:     OK
 * * @retval -1:    ERROR
 * */
static int jrppGetRingTopo(struct blob_buf *buf, int idx)
{
    int ret;
    RPP_RING_TOPO_T topo;
    ret = cmd_show_topo(idx, &topo);
    if(ret){
        LOG_ERROR("no match ring [%d] topo, something error!\n",idx);
        return -1;
    }

    char value[8];
    sprintf(value,"%d",topo.num);
    blobmsg_add_string(buf,"topo_num",value);
    int i;
    void *ret_table;
    char nodeId[48];
    for(i = 0; i < topo.num; i++){
        ret_table = blobmsg_open_table(&b, NULL);
        
        sprintf(nodeId,"%d-%02x:%02x:%02x:%02x:%02x:%02x",topo.node[i].node_id.prio,\
                topo.node[i].node_id.addr[0],topo.node[i].node_id.addr[1],topo.node[i].node_id.addr[2],\
                topo.node[i].node_id.addr[3],topo.node[i].node_id.addr[4],topo.node[i].node_id.addr[5]);
        blobmsg_add_string(buf,"NodeId",nodeId);
        blobmsg_add_string(buf,"RingState",(topo.node[i].state == RING_ENABLED)?"Enabled":"Disabled");
        blobmsg_add_string(buf,"RingMode",(topo.node[i].rpp_mode == RPP_ENHANCED)?"Enhanced":"Compactible");
        blobmsg_add_string(buf,"Status", (topo.node[i].status == RPP_HEALTH)?"Health":"Fault");
        blobmsg_add_string(buf,"NodeRole",(topo.node[i].node_role == NODE_TYPE_MASTER)?"Master":"Transit");
        sprintf(nodeId,"%d-%02x:%02x:%02x:%02x:%02x:%02x",topo.node[i].master_id.prio,\
                topo.node[i].master_id.addr[0],topo.node[i].master_id.addr[1],topo.node[i].master_id.addr[2],\
                topo.node[i].master_id.addr[3],topo.node[i].master_id.addr[4],topo.node[i].master_id.addr[5]);
        blobmsg_add_string(buf,"MasterId",nodeId);
        sprintf(nodeId,"Port-%d[%s * %02x:%02x:%02x:%02x:%02x:%02x]",topo.node[i].primary.port_no,\
                (topo.node[i].primary.stp == FORWARDING)?"Forwarding":"Blocking",\
                topo.node[i].primary.neighber_mac[0],topo.node[i].primary.neighber_mac[1],topo.node[i].primary.neighber_mac[2],\
                topo.node[i].primary.neighber_mac[3],topo.node[i].primary.neighber_mac[4],topo.node[i].primary.neighber_mac[5]);
        blobmsg_add_string(buf,"PrimaryPortInfo",nodeId);
        sprintf(nodeId,"Port-%d[%s * %02x:%02x:%02x:%02x:%02x:%02x]",topo.node[i].secondary.port_no,\
                (topo.node[i].secondary.stp == FORWARDING)?"Forwarding":"Blocking",\
                topo.node[i].secondary.neighber_mac[0],topo.node[i].secondary.neighber_mac[1],topo.node[i].secondary.neighber_mac[2],\
                topo.node[i].secondary.neighber_mac[3],topo.node[i].secondary.neighber_mac[4],topo.node[i].secondary.neighber_mac[5]);
        blobmsg_add_string(buf,"SecondPortInfo",nodeId);

        blobmsg_close_table(&b, ret_table); 
    }
    LOG_NORMAL("jrpp get ring %d topo_num = [%d] OK\n",idx, topo.num);

    return 0;
}

/* *
 * * @brief         set jrpp ring topo 
 * * @retval 0:     OK
 * * @retval -1:    ERROR
 * */
static int jrppSetRingTopo(int idx, void *p)
{   
    int ret;
    int stateVal = atoi((char *)p);
    if(stateVal != RING_ENABLED && stateVal != RING_DISABLED){
        LOG_ERROR("invalid arguement!\n");
        return -1;
    }
    ret = cmd_set_topo(idx, stateVal);
    if(ret){
        LOG_ERROR("set ring[%d] topo error\n",idx);
        return -1;
    }
    char ringId[8];
    sprintf(ringId,"ring%d", idx);
    //同步更改环状态
    ringList_S *ri,*ri_bak;
    list_for_each_entry_safe(ri, ri_bak, &ringCfg.entry, node)
    {
        if(!memcmp(&ri->ring_name, ringId, strlen(ringId)))
        {
            //更改状态
            ri->admin = (stateVal == RING_ENABLED)?FAL_ENABLE:FAL_DISABLE;
        }
    }

    LOG_NORMAL("jrpp set ring[%d] topo = [%s] OK\n",idx, (stateVal == RING_ENABLED)?"enable":"disable");

    return 0;
}

static const struct jw_switch_policy jrpp_cfg_tbl[] = {
    [JRPP_ADMIN_STATUS] = {.name = "jrpp_status",    .get_ext_handler = jrppGetAdminStatus,   .set_ext_handler = jrppSetAdminStatus   }, 
    [JRPP_RING_NUM]     = {.name = "ring_num",       .get_ext_handler = jrppGetRingNum }, 
    [JRPP_RING_NAME]    = {.name = "ring_name",      .get_handler     = jrppGetRingName,      .set_handler     = jrppSetRingName      }, 
  //[JRPP_RING_ID]      = {.name = "ring_id",        .get_handler     = jrppGetRingId,        .set_handler     = jrppSetRingId        }, 
    [JRPP_RING_PRIMARY] = {.name = "primary_port",   .get_handler     = jrppGetRingPrimary,   .set_handler     = jrppSetRingPrimary   }, 
    [JRPP_RING_SECOND]  = {.name = "secondary_port", .get_handler     = jrppGetRingSecondary, .set_handler     = jrppSetRingSecondary }, 
    [JRPP_RING_PRIORITY]= {.name = "priority",       .get_handler     = jrppGetRingPriority,  .set_handler     = jrppSetRingPriority  }, 
    [JRPP_RING_ADMIN]   = {.name = "admin",          .get_handler     = jrppGetRingAdmin,     .set_handler     = jrppSetRingAdmin     }, 
    [JRPP_RING_RELOAD]  = {.name = "reload",         .get_ext_handler = jrppGetRingReload}, 
    [JRPP_RING_TOPO]    = {.name = "topo",           .get_handler     = jrppGetRingTopo,      .set_handler     = jrppSetRingTopo }, 
//    [JRPP_RING_STATUS]  = {.name = "ring_status",    .get_handler     = jrppGetRingStatus }, 
};

/* *
 * * @brief         parse jrpp ring get
 * * @param[lvl3]   level 3 message structure.
 * * @retval 0:     OK
 * * @retval -1:    ERROR
 * */
static int jrppParseRingGet(struct blob_attr *lvl3, struct blob_buf *buf)
{
    const char *name    = blobmsg_name(lvl3);
    int idx     = atoi(name + 4);

/*    if (idx < 1 || idx > CONFIG_JWS_PORT_TOTAL_NR) 
    {
        LOG_ERROR("invalid port index!\n");
        return -1;
    }*/
    
    void *ret_array;

    ret_array= blobmsg_open_array(buf, name);
    foreach_json_array_get(idx,lvl3,buf,jrpp_cfg_tbl, __JRPP_CFG_TBL_MAX);
    blobmsg_close_array(buf, ret_array);    

    return 0;
}

/* *
 * * @brief         parse jrpp ring set
 * * @param[lvl3]   level 3 message structure.
 * * @retval 0:     OK
 * * @retval -1:    ERROR
 * */
static int jrppParseRingSet(struct blob_attr *lvl3)
{
    const char *name    = blobmsg_name(lvl3);
    int idx     = atoi(name + 4);
/*
    if (idx < 1 || idx > CONFIG_JWS_PORT_TOTAL_NR) 
    {
        LOG_ERROR("invalid port index!\n");
        return -1;
    }
*/
    if(ringCfg.status != FAL_ENABLE)
    {
        LOG_ERROR("the feature of jrpp is disable!\n");
    }

    /**<    add new index/entry  */
    if(idx == (ringCfg.num + 1))
    {
        ringCfg.num++;
        ringList_S *rin = (ringList_S *)calloc(1,sizeof(ringList_S));
        list_add_tail(&rin->node,&ringCfg.entry);
        rin->idx = ringCfg.num;
    }

    /**<    foreach array of level 3 message  */
    int ret;
    //int ring_id;
    ret = foreach_json_array_set(idx,lvl3,jrpp_cfg_tbl,__JRPP_CFG_TBL_MAX);
    if(ret)
    {
        LOG_ERROR("foreach ring %d error!\n",idx);
        return -1;
    }
    
    /**<    create each multicast row which has been changed  */
    ringList_S *ri,*ri_bak;
    list_for_each_entry_safe(ri, ri_bak, &ringCfg.entry, node)
    {
        if(ri->flag)
        {
            ri->flag = 0;
            char buf[8];
            sprintf(set_name, "%s", ri->ring_name);
            //ring_id = atoi(set_name + 4);
            //ring_name
           /*   ret = cmd_add_ring(ring_id); 
            if(ret){
                LOG_ERROR("add or set ring %d failed\n", ring_id);
                return -1;
            }
            */
            struct uci_ptr ptr = {
                .package = "jrppd",
                .section = strdup(set_name),
            };
            sprintf(buf,"%s","ring");

            ubus_collect_config(&ptr,buf,UCI_TYPE_STRING,DO_SET);
            
            //primary_porta
            /*ret = cmd_add_ringport(ring_id, ri->primary_port, ri->secondary_port); 
            if(ret){
                LOG_ERROR("add or set ring %d port  failed\n", ring_id);
                return -1;
            }*/
            
            ptr.option = "primary_port";

            sprintf(buf,"%d",ri->primary_port);
            ubus_collect_config(&ptr,buf,UCI_TYPE_STRING,DO_SET);
            
            //secondary_port
            ptr.option = "secondary_port";

            sprintf(buf,"%d",ri->secondary_port);
            ubus_collect_config(&ptr,buf,UCI_TYPE_STRING,DO_SET);
            
            //priority
            ptr.option = "priority";

            switch(ri->priority){
                case 1:
                    sprintf(buf,"%s",JRPP_PRIORITY_VALUE1);
                    break;
                case 2:
                    sprintf(buf,"%s",JRPP_PRIORITY_VALUE2);
                    break;
                case 3:
                    sprintf(buf,"%s",JRPP_PRIORITY_VALUE3);
                    break;
                default:
                    break;
            }

            /*ret = cmd_set_nodeprio(ring_id, buf); 
            if(ret){
                LOG_ERROR("add or set ring %d prio failed\n", ring_id);
                return -1;
            }*/
            ubus_collect_config(&ptr,buf,UCI_TYPE_STRING,DO_SET);

            //admin
            ptr.option = "admin";

            sprintf(buf,"%s",(ri->admin == FAL_ENABLE) ? JRPP_ST_ADMIN_VALUE1:JRPP_ST_ADMIN_VALUE2);
            /*ret = cmd_set_ringmode(ring_id, buf); 
            if(ret){
                LOG_ERROR("add or set ring %d  mode failed\n", ring_id);
                return -1;
            }*/
            ubus_collect_config(&ptr,buf,UCI_TYPE_STRING,DO_SET);

            LOG_NORMAL("jrpp set one section: %s OK\n", ri->ring_name);
        }
    }

    return 0;
}

/* *
 * * @brief         parse ring config clear operation from upper
 * * @param[lvl2]   level 2 message
 * * @retval 0:     OK
 * * @retval -1:    ERROR
 * */
static int jrppParseRingClear(struct blob_attr *lvl2)
{
    char *value = blobmsg_get_string(lvl2);
    int idx     = atoi(value + 4);
    int flag = 0;
/*
    if (idx < 1 || idx > mcastCfg.num) 
    {
        LOG_ERROR("invalid index %d\n", idx);
        return -1;
    }
*/    
    ringList_S *ri,*ri_bak;
    list_for_each_entry_safe(ri, ri_bak, &ringCfg.entry, node)
    {
        if(flag)
        {
            ri->idx--;
        }
        else if(idx == ri->idx)
        {
            /**<    delete specified entry  */
            if(ri->ring_name)
            {
                if(jrppDeleteUciSection(ri))
                {
                    LOG_ERROR("ring delete section fail!\n");
                    return -1;
                }
            }
            
            flag = 1;
            ringCfg.num--;
            list_del(&ri->node);
            free(ri);
        }
    }

    if(flag)
    {
        LOG_NORMAL("destroy one ring section which index = %d OK\n",idx);
        return 0;
    }

    LOG_ERROR("no match index [%d], something error!\n",idx);
    return -1;
}


//getHandler
static int ringGetHandler(struct ubus_context *ctx, struct ubus_object *obj,
                struct ubus_request_data *req, const char *method,
                struct blob_attr *msg)
{
    struct blob_attr *tb[__RING_CFG_GET_MAX];    
    struct blob_attr *lvl1 = NULL;  
    //校验信息
    /**
     * 从data BLOGMSG串中根据policy策略过滤，得到的结果存储在tb BLOGATTR数组中
     */
    blobmsg_parse(ring_config_get_policy, __RING_CFG_GET_MAX, tb, blob_data(msg), blob_len(msg));
    lvl1 = tb[RING_CFG_GET_KEY];     
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
    //返回数组包含ret
    ret_array = blobmsg_open_array(&b, "ret"); 
    blobmsg_for_each_attr(lvl2,lvl1, rem) //遍历
    {
        switch (blobmsg_type(lvl2))
        {
            case BLOBMSG_TYPE_STRING: //只针对string类型  全局变量
            {
                struct jw_switch_policy *policy_p;
                char *name = blobmsg_get_string(lvl2);

                policy_p = (struct jw_switch_policy *)jw_switchd_get_context(name, jrpp_cfg_tbl, __JRPP_CFG_TBL_MAX);
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
            case BLOBMSG_TYPE_TABLE: //table类型    环内参数
            {
                struct blob_attr *lvl3 = blobmsg_data(lvl2);

                if(blobmsg_type(lvl3) == BLOBMSG_TYPE_ARRAY)
                {
                    ret_table = blobmsg_open_table(&b, NULL);
                    jrppParseRingGet(lvl3,&b);
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

//setHandler
static int ringSetHandler(struct ubus_context *ctx, struct ubus_object *obj,
                struct ubus_request_data *req, const char *method,
                struct blob_attr *msg)
{
    struct blob_attr *tb[__RING_CFG_SET_MAX];    
    struct blob_attr *lvl1 = NULL;  
    int ret = -1;

    blobmsg_parse(ring_config_set_policy, __RING_CFG_SET_MAX, tb, blob_data(msg), blob_len(msg));
    lvl1 = tb[RING_CFG_SET_KEY];     
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

                    policy_p = (struct jw_switch_policy *)jw_switchd_get_context(name, jrpp_cfg_tbl, __JRPP_CFG_TBL_MAX);
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
                    ret = jrppParseRingSet(lvl3);
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

//clearHandler
static int ringClearHandler(struct ubus_context *ctx, struct ubus_object *obj,
                struct ubus_request_data *req, const char *method,
                struct blob_attr *msg)
{
    struct blob_attr *tb[__RING_CFG_CLEAR_MAX];    
    struct blob_attr *lvl1 = NULL;  
    int ret = -1;

    blobmsg_parse(ring_config_clear_policy, __RING_CFG_CLEAR_MAX, tb, blob_data(msg), blob_len(msg));
    lvl1 = tb[RING_CFG_CLEAR_KEY];     
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
                        
            ret = jrppParseRingClear(lvl2);
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

//ubus结构参数
static struct ubus_method ring_config_methods[] = {
    UBUS_METHOD("ring_config_get", ringGetHandler, ring_config_get_policy),
    UBUS_METHOD("ring_config_set", ringSetHandler, ring_config_set_policy),
    UBUS_METHOD("ring_config_clear", ringClearHandler, ring_config_clear_policy),
};

static struct ubus_object_type ring_object_type = 
    UBUS_OBJECT_TYPE("ring_config", ring_config_methods);

static struct ubus_object ring_object = {
    .name = "ring_config",
    .type = &ring_object_type,
    .methods = ring_config_methods,
    .n_methods = ARRAY_SIZE(ring_config_methods),
};

static int jrppLoadState(struct uci_ptr *ptr)
{
    int state;

    if(!strcmp(ptr->o->v.string,"on"))
    {
        state = FAL_ENABLE;
    }
    else if(!strcmp(ptr->o->v.string,"off"))
    {
        state = FAL_DISABLE;
    }
    else
    {
        LOG_ERROR("invalid arguement!\n"); 
        return -1;
    }
    
    ringCfg.status = state;

    LOG_NORMAL("ring load status = %d & %s OK!\n",ringCfg.status, ptr->o->v.string);
    return 0;
}

#if 0
static int jrppLoadRingId(struct uci_ptr *ptr)
{
    static int count = 0;
    int id = atoi(ptr->o->v.string);
    const char* ringName = ptr->section;
//    memcpy(ringName, ptr->section, strlen(ptr->section));

/*    if(prio < 0 || prio > MAX_PORT_PRIO_NUM)
    {
        LOG_ERROR("invalid arguement!\n");
        return -1;
    }
*/
    /**<    link entry to list  */
    ringList_S *ri = (ringList_S *)calloc(1,sizeof(ringList_S));
    list_add_tail(&ri->node,&ringCfg.entry);
    
    memcpy(&ri->ring_id, &id, sizeof(id));
    memcpy(&ri->ring_name, ringName, strlen(ringName));
    ri->idx = ++count;
//    LOG_NORMAL("ring idx = %d !\n", ri->idx);
//    free(ringName);
    LOG_NORMAL("jrpp load [%s] ring_id = %d OK!\n", ringName, id);
    return 0;
}
#endif

static int jrppLoadRingPrimary(struct uci_ptr *ptr)
{
    const char* ringName = ptr->section;
    int port = atoi(ptr->o->v.string);


/*    if(prio < 0 || prio > MAX_PORT_PRIO_NUM)
    {
        LOG_ERROR("invalid arguement!\n");
        return -1;
    }
*/
    /**<    link entry to list  */
    ringList_S *ri, *ri_bak;
    list_for_each_entry_safe(ri, ri_bak, &ringCfg.entry, node)
    {
        if(!memcmp(&ri->ring_name, ringName, strlen(ringName))){
            memcpy(&ri->primary_port, &port, sizeof(port));
            LOG_NORMAL("jrpp load  [%s] primary_port = %d OK!\n", ringName, port);
            return 0;
        }
    }

    LOG_ERROR("jrpp load [%s] primary_port fail!\n", ringName);
    return -1;
}

static int jrppLoadRingSecond(struct uci_ptr *ptr)
{
    const char* ringName = ptr->section;
    int port = atoi(ptr->o->v.string);


/*    if(prio < 0 || prio > MAX_PORT_PRIO_NUM)
    {
        LOG_ERROR("invalid arguement!\n");
        return -1;
    }
*/
    /**<    link entry to list  */
    ringList_S *ri, *ri_bak;
    list_for_each_entry_safe(ri, ri_bak, &ringCfg.entry, node)
    {
        if(!memcmp(&ri->ring_name, ringName, strlen(ringName))){
            memcpy(&ri->secondary_port, &port, sizeof(port));
            LOG_NORMAL("jrpp load [%s] secondary_port = %d OK!\n", ringName, port);
            return 0;
        }
    }

    LOG_ERROR("jrpp load [%s] secondary_port fail!\n", ringName);
    return -1;
}



static int jrppLoadRingPriority(struct uci_ptr *ptr)
{
    const char* ringName = ptr->section;
    char*  p;
    p = ptr->o->v.string;
    int mode;

    if(!strncmp(p, JRPP_PRIORITY_VALUE1, strlen(JRPP_PRIORITY_VALUE1)))
    {
        mode = 1; 
    }
    else if(!strncmp(p, JRPP_PRIORITY_VALUE2, strlen(JRPP_PRIORITY_VALUE2)))
    {
        mode = 2;
    }
    else
    {
        mode = 3;
    }


    /**<    link entry to list  */
    ringList_S *ri, *ri_bak;
    list_for_each_entry_safe(ri, ri_bak, &ringCfg.entry, node)
    {
        if(!memcmp(&ri->ring_name, ringName, strlen(ringName))){
            memcpy(&ri->priority, &mode, sizeof(int));
            LOG_NORMAL("jrpp load [%s] priority = %s OK!\n", ringName, p);
            return 0;
        }
    }
    

    LOG_ERROR("jrpp load [%s] priority fail!\n", ringName);
    return -1;
}


static int jrppLoadRingAdmin(struct uci_ptr *ptr)
{
    const char* ringName = ptr->section;
    char*  p;
    p = ptr->o->v.string;
    int mode;

    if(!strncmp(p, JRPP_ST_ADMIN_VALUE1, strlen(JRPP_ST_ADMIN_VALUE1)))
    {
        mode = FAL_ENABLE; 
    }
    else
    {
        mode = FAL_DISABLE;
    }

    /**<    link entry to list  */
    ringList_S *ri, *ri_bak;
    list_for_each_entry_safe(ri, ri_bak, &ringCfg.entry, node)
    {
        if(!memcmp(&ri->ring_name, ringName, strlen(ringName))){
            memcpy(&ri->admin, &mode, sizeof(int));
            LOG_NORMAL("jrpp load [%s] admin = %s OK!\n", ringName, p);
            return 0;
        }
    }
    

    LOG_ERROR("jrpp load [%s] priority fail!\n", ringName);
    return -1;
}

typedef struct{
    char *name;
    enum uci_option_type type;
    int (*ubusGet)(struct getPara *);
}UciRingConfig_S;

static UciConfig_S ringUci[] = {
    {.name = "admin.jrpp",          .type = UCI_TYPE_STRING,    .ubusGet = jrppLoadState},
//    {.name = "ring_id",                 .type = UCI_TYPE_STRING,    .ubusGet = jrppLoadRingId},
    {.name = "primary_port",            .type = UCI_TYPE_STRING,    .ubusGet = jrppLoadRingPrimary},
    {.name = "secondary_port",          .type = UCI_TYPE_STRING,    .ubusGet = jrppLoadRingSecond},
    {.name = "priority",                .type = UCI_TYPE_STRING,    .ubusGet = jrppLoadRingPriority},
    {.name = "admin",                   .type = UCI_TYPE_STRING,    .ubusGet = jrppLoadRingAdmin},
};

//
static int ringCfgLoad(void)
{

    memset(&ringCfg, 0, sizeof(ringCfg));
    INIT_LIST_HEAD(&ringCfg.entry);

    int ret, i;
    char name[64];
    struct uci_ptr ptr;
    int count = 0;

    struct uci_context *ctx = uci_alloc_context();
    struct uci_package *package;
    struct uci_element *elt;
    uci_load(ctx, "jrppd", &package);

    if(package == NULL){
        LOG_ERROR("/etc/config have no jrppd!\n");
        return -1;
    }

    uci_foreach_element(&package->sections, elt)
    {
        if(!strncmp(elt->name,"ring",strlen("ring")))//按环网个数增加节点个数
        {                    
            /**<    link entry to list  */
            ringList_S *ri = (ringList_S *)calloc(1,sizeof(ringList_S));
            list_add_tail(&ri->node,&ringCfg.entry);

            memcpy(&ri->ring_name, elt->name, strlen(elt->name));
            ri->idx = ++count;
            ringCfg.num = count;
        }
        for(i = 0; i < sizeof(ringUci)/sizeof(UciConfig_S); i++)//遍历
        {

            if(!strncmp(elt->name,"ring",strlen("ring")) && (i !=0))
            {                    
                sprintf(name,"jrppd.%s.%s",elt->name, ringUci[i].name);
            }
            else if(!(strncmp(elt->name,"admin",strlen("admin")))&& (i == 0))
            {
                sprintf(name,"jrppd.%s",ringUci[i].name);
            }
            else
            {
                continue;
            }
            memset(&ptr, 0 , sizeof(ptr));
            ret = load_config(name,ringUci[i].type,&ptr,ctx);//读配置文件
            if(ret)
            {
                if(ringUci[i].type == UCI_TYPE_LIST) continue;

                LOG_ERROR("load config %s fail!\n",name); 
                uci_free_context(ctx);
                return -1;
            }
            ret = ringUci[i].ubusGet(&ptr);//执行操作
            if(ret)
            {
                uci_free_context(ctx);

                LOG_ERROR("ubus get arguement %s fail!\n",name); 
                return -1;
            }
        }
    }
    uci_free_context(ctx);
    return 0;
}

//入口函数
void ubus_init_ring(struct ubus_context *ctx)
{
    int ret;
    _ctx = ctx;

    //读写配置文件内容
    ret = ringCfgLoad();
    if(ret){
        LOG_ERROR("Init ring configuration from UCI file fail!\n");
        return;
    }
    /**< 处理端口状态*/
    jrppProcessPortStatus(ringCfg.status, ringCfg.num);
    //往ubus里加载ring模块
    ret = ubus_add_object(ctx, &ring_object);
    if (ret)
        LOG_ERROR("Failed to add object [ring_config]: %s\n", ubus_strerror(ret));
    ctl_ring_init();
  //  cmd_show_ring(1);
}
