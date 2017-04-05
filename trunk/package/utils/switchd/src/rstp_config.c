/* *< 中威文件编码格式为gb2312,cp936 */
/* * $Id$ $DateTime$
 * * @file rstp_config.c
 * * @brief This is a table of the switch's rstp configure.
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
#include "rstp_comm.h"

static struct blob_buf b;
static struct ubus_context *_ctx;


enum {
    RSTP_GET_KEY,
    __RSTP_GET_MAX,
};

enum {
    RSTP_SET_KEY,
    __RSTP_SET_MAX
};

static const struct blobmsg_policy rstp_get_policy[] = {
    [RSTP_GET_KEY] = {.name = "get_args", .type = BLOBMSG_TYPE_ARRAY},
};

static const struct blobmsg_policy rstp_set_policy[] = {
    [RSTP_SET_KEY] = {.name = "set_args", .type = BLOBMSG_TYPE_ARRAY},
};

enum {
    RSTP_STATUS,
    RSTP_ROOT_PRIO,
    RSTP_ROOT_HELLOTIME,
    RSTP_ROOT_MAX_AGE,
    RSTP_ROOT_FORWARD_DELAY,
    RSTP_ROOT_MAC_ADDRESS,
    RSTP_PRIO,
    RSTP_HELLOTIME,
    RSTP_MAX_AGE,
    RSTP_FORWARD_DELAY,
    RSTP_MAC_ADDRESS,
    RSTP_PORT_STATUS,
    RSTP_PORT_PRIO,
    RSTP_PORT_PATH_COST,
    RSTP_PORT_ATUO_CAL,
    RSTP_GET_PORT_NUM,
    RSTP_GET_PORT_NAME,
    RSTP_GET_PORT_STATE,
    RSTP_GET_PORT_PRIO,
    RSTP_GET_PORT_ROLE,
    RSTP_GET_PORT_TYPE,
    RSTP_GET_PORT_COST,
    RSTP_PORT_EDGE,
    __RSTP_TBL_MAX
};

static struct{
    int status;
    int prio;
    int hello_t;
    int max_age;
    int fwd_dly;
    int res;
    struct{
        int status;
        int prio;
        int auto_cal;
        int edge;
        unsigned long ph_cost;
    }port[CONFIG_JWS_PORT_TOTAL_NR];
}rstp;


//rstp 端口信息
typedef struct{
    int idx;    //序号
    int port;   //端口
    RSTP_PORT_STATE state;  //状态
    int prio;   //优先级
    unsigned char role;   //角色
    unsigned char portType; //类型
    unsigned char res[2];
    unsigned long cost;   //端口开销
    struct list_head node;
}portList_S;

static struct{
    int num;
    struct list_head entry;
}rstpPortCfg;


#define MAX_PRIO_NUM 0xFFFF
#define MAX_HELLO_TIME 10
#define MIN_HELLO_TIME 1
#define MAX_AGE_TIME 40
#define MIN_AGE_TIME 6
#define MAX_FW_DELAY 30
#define MIN_FW_DELAY 4
#define MAX_PORT_PRIO_NUM 0xFF
#define MAX_PORT_PATH_COST 0xFFFFFFF 
#define DEFAULT_COST 10000

#define PORT_RSTP_DISABLE '2'

static int rstpSetPortStatus(int port,void *p);

static int rstpCheckPortStatus(int Port)
{
    if((get_port_status(Port) != PORT_STATUS_NORMAL) && (get_port_status(Port) != PORT_STATUS_RSTP))
    {
        LOG_ERROR("check port cur status fail!\n");
        return -2;
    }
    return 0;
}

static void rstpProcessPortStatus(int status)
{
    int i, ret;
    /**< when jrpp admin status is disable trans port status from ring to normal*/
    if(status == FAL_DISABLE){
        for(i = 1; i <= CONFIG_JWS_PORT_TOTAL_NR; i++){
            if(get_port_status(i) == PORT_STATUS_RSTP){
                set_port_status(i, PORT_STATUS_NORMAL);
            }
        }
    } 
    /**< set port status if jrpp admin enabl*/
    if(status == FAL_ENABLE){
        for(i = 1; i <= CONFIG_JWS_PORT_TOTAL_NR; i++){
            /**<  check ring port status*/
            if(rstp.port[i-1].status == FAL_ENABLE){
                ret = rstpCheckPortStatus(i);
                if(ret){ /**< 端口状态不对*/
                    /**< 删除节点*/
                    rstpSetPortStatus(i, (void*)PORT_RSTP_DISABLE); 
                    continue;
                }
                /**< set ring port status*/
                set_port_status(i, PORT_STATUS_RSTP);
            }
        }
    }
}



/* *
 * * @brief         get rstp root priority
 * * @retval 0:     OK
 * * @retval -1:    ERROR
 * */
static int rstpGetRootPriority(struct blob_buf *buf)
{
    char prio_s[16];

    UID_STP_STATE_T uid_state;
    UID_STP_CFG_T uid_cfg;

    int res = cmd_show_bridge(&uid_state, &uid_cfg);
    if(res){
        LOG_ERROR("get rstp uid state failed!\n");
        return -1; 
    }

    sprintf(prio_s, "%ld",(unsigned long)uid_state.designated_root.prio);
    
    blobmsg_add_string(buf, "root_priority", prio_s);
    LOG_NORMAL("rstp get root priority = [%s] OK\n",prio_s);

    return 0;
}

/* *
 * * @brief         get rstp root hello time
 * * @retval 0:     OK
 * * @retval -1:    ERROR
 * */
static int rstpGetRootHelloTime(struct blob_buf *buf)
{
    char hello_s[16];

    UID_STP_STATE_T uid_state;
    UID_STP_CFG_T uid_cfg;

    int res = cmd_show_bridge(&uid_state, &uid_cfg);
    if(res){
        LOG_ERROR("get rstp uid state failed!\n");
        return -1; 
    }

    sprintf(hello_s, "%2d",(int)uid_state.hello_time);
    
    blobmsg_add_string(buf, "root_hello_time", hello_s);
    LOG_NORMAL("rstp get root hellotime = [%s] OK\n",hello_s);

    return 0;
}

/* *
 * * @brief         get rstp root max age
 * * @retval 0:     OK
 * * @retval -1:    ERROR
 * */
static int rstpGetRootMaxAge(struct blob_buf *buf)
{
    char age_s[16];

    UID_STP_STATE_T uid_state;
    UID_STP_CFG_T uid_cfg;

    int res = cmd_show_bridge(&uid_state, &uid_cfg);
    if(res){
        LOG_ERROR("get rstp uid state failed!\n");
        return -1; 
    }

    sprintf(age_s, "%2d",(int)uid_state.max_age);
    
    blobmsg_add_string(buf, "root_max_age", age_s);
    LOG_NORMAL("rstp get root maxage = [%s] OK\n",age_s);

    return 0;
}
/* *
 * * @brief         get rstp root forward delay
 * * @retval 0:     OK
 * * @retval -1:    ERROR
 * */
static int rstpGetRootForwardDelay(struct blob_buf *buf)
{
    char delay_s[16];

    UID_STP_STATE_T uid_state;
    UID_STP_CFG_T uid_cfg;

    int res = cmd_show_bridge(&uid_state, &uid_cfg);
    if(res){
        LOG_ERROR("get rstp uid state failed!\n");
        return -1; 
    }

    sprintf(delay_s, "%2d",(int)uid_state.forward_delay);
    
    blobmsg_add_string(buf, "root_forward_delay", delay_s);
    LOG_NORMAL("rstp get root forwardDelay = [%s] OK\n",delay_s);

    return 0;

}
/* *
 * * @brief         get rstp root mac addr
 * * @retval 0:     OK
 * * @retval -1:    ERROR
 * */
static int rstpGetRootMacAddr(struct blob_buf *buf)
{
    char addr_s[16];

    UID_STP_STATE_T uid_state;
    UID_STP_CFG_T uid_cfg;

    int res = cmd_show_bridge(&uid_state, &uid_cfg);
    if(res){
        LOG_ERROR("get rstp uid state failed!\n");
        return -1; 
    }

    sprintf(addr_s, "%02x%02x-%02x%02x-%02x%02x",
           (unsigned char)uid_state.designated_root.addr[0],
           (unsigned char)uid_state.designated_root.addr[1],
           (unsigned char)uid_state.designated_root.addr[2],
           (unsigned char)uid_state.designated_root.addr[3],
           (unsigned char)uid_state.designated_root.addr[4],
           (unsigned char)uid_state.designated_root.addr[5]);
    
    blobmsg_add_string(buf, "root_mac_addr", addr_s);
    LOG_NORMAL("rstp get root mac addr = [%s] OK\n",addr_s);

    return 0;
}


/* *
 * * @brief         get rstp port num
 * * @retval 0:     OK
 * * @retval -1:    ERROR
 * */
static int rstpGetPortNum(struct blob_buf *buf)
{
    memset(&rstpPortCfg, 0, sizeof(rstpPortCfg));
    INIT_LIST_HEAD(&rstpPortCfg.entry);
    char num_s[4];

    int i, role = 0;
    int count = 0;
    UID_STP_PORT_CFG_T portCfg;
    UID_STP_PORT_STATE_T portState;
    
    //清空链表
    portList_S *ri,*ri_bak;
    list_for_each_entry_safe(ri, ri_bak, &rstpPortCfg.entry, node)
    {
        list_del(&ri->node);
        free(ri);
    }
    rstpPortCfg.num = 0;
    

    for(i = 0; i < CONFIG_JWS_PORT_TOTAL_NR; i++){
        int res = cmd_show_portdetail(i+1, &portCfg, &portState);
        if(res){
            LOG_ERROR("get rstp port detail failed!\n");
            return -1; 
        }
        switch(portState.role){
            case 'A': 
            case 'B': 
            case 'R': 
            case 'D': 
                {
                    portList_S *pr = (portList_S *)calloc(1,sizeof(portList_S));
                    list_add_tail(&pr->node,&rstpPortCfg.entry);
                    pr->port = i + 1;
                    if(portState.role == 'A'){role = 1;}
                    else if(portState.role == 'B'){role = 2;}
                    else if(portState.role == 'R'){role = 3;}
                    else if(portState.role == 'D'){role = 4;}
                    pr->role = role;
                    pr->state = portState.state;
                    pr->cost = portState.path_cost;
                    pr->prio = (int)(portState.port_id >> 8);
                    pr->portType = portState.oper_edge;

                    pr->idx = ++count;
                    rstpPortCfg.num = count;
                }
                break;
            default:break;
        }
    }

    sprintf(num_s, "%2d", count);

    blobmsg_add_string(buf, "port_get_num", num_s);
    LOG_NORMAL("rstp get port num = [%s] OK\n",num_s);

    return 0;

}

/* *
 * * @brief             get port status
 * * @param[port]       port index.
 * * @retval 0:         OK
 * * @retval -1:        ERROR
 * */
static int rstpGetReadPortName(struct blob_buf *buf,int idx)
{
    portList_S *ri,*ri_bak;
    list_for_each_entry_safe(ri, ri_bak, &rstpPortCfg.entry, node)
    {
        if(idx == ri->idx)
        {
            char port_s[8];

            sprintf(port_s,"%d",ri->port);
            blobmsg_add_string(buf, "port_get_name", port_s);
            LOG_NORMAL("rstp get port name = [%s] OK\n",port_s);
            
            return 0;
        }
    }

    blobmsg_add_string(buf,"port_get_name","-1");
    LOG_ERROR("no match rstp [%d] port , something error!\n",idx);

    return -1;
}

/* *
 * * @brief             get port status
 * * @param[port]       port index.
 * * @retval 0:         OK
 * * @retval -1:        ERROR
 * */
static int rstpGetReadPortState(struct blob_buf *buf,int idx)
{
    portList_S *ri,*ri_bak;
    list_for_each_entry_safe(ri, ri_bak, &rstpPortCfg.entry, node)
    {
        if(idx == ri->idx)
        {
            char state_s[8];

            sprintf(state_s,"%d",ri->state);
            blobmsg_add_string(buf, "port_get_state", state_s);
            LOG_NORMAL("rstp get port name = [%s] OK\n",state_s);
            
            return 0;
        }
    }

    blobmsg_add_string(buf,"port_get_state","-1");
    LOG_ERROR("no match rstp [%d] state , something error!\n",idx);

    return -1;
}

/* *
 * * @brief             get port status
 * * @param[port]       port index.
 * * @retval 0:         OK
 * * @retval -1:        ERROR
 * */
static int rstpGetReadPortPrio(struct blob_buf *buf,int idx)
{
    portList_S *ri,*ri_bak;
    list_for_each_entry_safe(ri, ri_bak, &rstpPortCfg.entry, node)
    {
        if(idx == ri->idx)
        {
            char prio_s[8];

            sprintf(prio_s,"%d",ri->prio);
            blobmsg_add_string(buf, "port_get_prio", prio_s);
            LOG_NORMAL("rstp get port prio = [%s] OK\n",prio_s);
            
            return 0;
        }
    }

    blobmsg_add_string(buf,"port_get_prio","-1");
    LOG_ERROR("no match rstp [%d] prio , something error!\n",idx);

    return -1;
}

/* *
 * * @brief             get port role
 * * @param[port]       port index.
 * * @retval 0:         OK
 * * @retval -1:        ERROR
 * */
static int rstpGetReadPortRole(struct blob_buf *buf,int idx)
{
    portList_S *ri,*ri_bak;
    list_for_each_entry_safe(ri, ri_bak, &rstpPortCfg.entry, node)
    {
        if(idx == ri->idx)
        {
            char role_s[8];

            sprintf(role_s,"%d",ri->role);
            blobmsg_add_string(buf, "port_get_role", role_s);
            LOG_NORMAL("rstp get port role = [%s] OK\n",role_s);
            
            return 0;
        }
    }

    blobmsg_add_string(buf,"port_get_role","-1");
    LOG_ERROR("no match rstp [%d] role , something error!\n",idx);

    return -1;
}

/* *
 * * @brief             get port status
 * * @param[port]       port index.
 * * @retval 0:         OK
 * * @retval -1:        ERROR
 * */
static int rstpGetReadPortType(struct blob_buf *buf,int idx)
{
    portList_S *ri,*ri_bak;
    list_for_each_entry_safe(ri, ri_bak, &rstpPortCfg.entry, node)
    {
        if(idx == ri->idx)
        {
            char type_s[8];

            sprintf(type_s,"%d",ri->portType);
            blobmsg_add_string(buf, "port_get_type", type_s);
            LOG_NORMAL("rstp get port type = [%s] OK\n",type_s);
            
            return 0;
        }
    }

    blobmsg_add_string(buf,"port_get_type","-1");
    LOG_ERROR("no match rstp [%d] type , something error!\n",idx);

    return -1;
}

/* *
 * * @brief             get port status
 * * @param[port]       port index.
 * * @retval 0:         OK
 * * @retval -1:        ERROR
 * */
static int rstpGetReadPortCost(struct blob_buf *buf,int idx)
{
    portList_S *ri,*ri_bak;
    list_for_each_entry_safe(ri, ri_bak, &rstpPortCfg.entry, node)
    {
        if(idx == ri->idx)
        {
            char cost_s[8];

            sprintf(cost_s,"%ld",ri->cost);
            blobmsg_add_string(buf, "port_get_cost", cost_s);
            LOG_NORMAL("rstp get port cost = [%s] OK\n",cost_s);
            
            return 0;
        }
    }

    blobmsg_add_string(buf,"port_get_cost","-1");
    LOG_ERROR("no match rstp [%d] cost , something error!\n",idx);

    return -1;
}

/* *
 * * @brief         get rstp status
 * * @retval 0:     OK
 * * @retval -1:    ERROR
 * */
static int rstpGetStatus(struct blob_buf *buf)
{
    char status_s[2];
    
    sprintf(status_s,"%d",rstp.status);
    blobmsg_add_string(buf, "status", status_s);
    LOG_NORMAL("rstp get status = [%s] OK\n",status_s);

    return 0;
}

/* *
 * * @brief         set rstp status 
 * * @retval 0:     OK
 * * @retval -1:    ERROR
 * */
static int rstpSetStatus(void *p)
{
    int status = atoi((char *)p);

    if(status != FAL_DISABLE && status != FAL_ENABLE)
    {
        LOG_ERROR("invalid arguement!\n");
        return -1;
    }

    if(status == rstp.status)
    {
        LOG_NORMAL("same arguement,do nothing\n");
        return 0;
    }
    
    int value = 0;
    switch(status){
        case FAL_ENABLE:
            system("uci set rstpd.admin.rstp=on");
            system("uci commit rstpd");
            system("/etc/init.d/rstpd start");
            sleep(3);
            value = 1;
            break;
        case FAL_DISABLE:
            value = 0;
            break;
        default:
            break;
    }

    /*调用ctl命令*/
    int ret = cmd_rstp(value);
    if(ret){
        LOG_ERROR("set bridge  status failed!\n");
        return -1; 
    }
    if(status == FAL_DISABLE){
       system("/etc/init.d/rstpd  stop");
    }
    rstp.status = status;
     
    char buf[8];
    struct uci_ptr ptr = {
        .package = "rstpd",
        .section = "admin",
        .option =  "rstp",
    };

    sprintf(buf,"%s",(status == FAL_ENABLE)?"on":"off");
    ubus_collect_config(&ptr,buf,UCI_TYPE_STRING,DO_SET);

    /**< 处理端口状态*/
    rstpProcessPortStatus(rstp.status);
    
    LOG_NORMAL("rstp set status = [%d] OK\n", status);

    return 0;
}

/* *
 * * @brief         get rstp priority 
 * * @retval 0:     OK
 * * @retval -1:    ERROR
 * */
static int rstpGetPriority(struct blob_buf *buf)
{
    char prio_s[8];
    
    sprintf(prio_s,"%d",rstp.prio);
    blobmsg_add_string(buf, "priority", prio_s);
    LOG_NORMAL("rstp get priority = [%s] OK\n",prio_s);

    return 0;
}

/* *
 * * @brief         set rstp priority
 * * @retval 0:     OK
 * * @retval -1:    ERROR
 * */
static int rstpSetPriority(void *p)
{
    if(rstp.status != FAL_ENABLE)
    {
        LOG_ERROR("the feature of rstp is disable");
        return -1;
    }

    int prio = atoi((char *)p);

    if(prio < 0 || prio > MAX_PRIO_NUM)
    {
        LOG_ERROR("invalid arguement!\n");
        return -1;
    }

    if(prio == rstp.prio)
    {
        LOG_NORMAL("same arguement,do nothing\n");
        return 0;
    }

    /*调用ctl命令*/
    int ret = cmd_set_bridge_prio(prio);
    if(ret){
        LOG_ERROR("set bridge priority failed!\n");
        return -1; 
    }
    
    rstp.prio = prio;

    char buf[8];
    struct uci_ptr ptr = {
        .package = "rstpd",
        .section = "bridge",
        .option =  "bridgeprio",
    };

    sprintf(buf,"%d", prio);
    ubus_collect_config(&ptr,buf,UCI_TYPE_STRING,DO_SET);
    
    LOG_NORMAL("rstp set prio = [%d] OK\n", prio);
    return 0;
}

/* *
 * * @brief         get rstp hello time
 * * @retval 0:     OK
 * * @retval -1:    ERROR
 * */
static int rstpGetHelloTime(struct blob_buf *buf)
{
    char hello_s[4];
    
    sprintf(hello_s,"%d",rstp.hello_t);
    blobmsg_add_string(buf, "hello_time", hello_s);
    LOG_NORMAL("rstp get hello time = [%s] OK\n",hello_s);

    return 0;
}

/* *
 * * @brief         set rstp hello time 
 * * @retval 0:     OK
 * * @retval -1:    ERROR
 * */
static int rstpSetHelloTime(void *p)
{
    if(rstp.status != FAL_ENABLE)
    {
        LOG_ERROR("the feature of rstp is disable");
        return -1;
    }

    int hello = atoi((char *)p);

    if(hello < MIN_HELLO_TIME || hello > MAX_HELLO_TIME)
    {
        LOG_ERROR("invalid arguement!\n");
        return -1;
    }

    if(hello == rstp.hello_t)
    {
        LOG_NORMAL("same arguement,do nothing\n");
        return 0;
    }
    
    /*调用ctl命令*/
    int ret = cmd_set_bridge_hello(hello);
    if(ret){
        LOG_ERROR("set bridge hellotime failed!\n");
        return -1; 
    }

    rstp.hello_t = hello;
 
    char buf[8];
    struct uci_ptr ptr = {
        .package = "rstpd",
        .section = "bridge",
        .option =  "hello",
    };

    sprintf(buf,"%d", hello);
    ubus_collect_config(&ptr,buf,UCI_TYPE_STRING,DO_SET);

    LOG_NORMAL("rstp set hello time = [%d] OK\n", hello);
    return 0;
}

/* *
 * * @brief         get rstp max age
 * * @retval 0:     OK
 * * @retval -1:    ERROR
 * */
static int rstpGetMaxAge(struct blob_buf *buf)
{
    char age_s[4];
    
    sprintf(age_s,"%d",rstp.max_age);
    blobmsg_add_string(buf, "max_age", age_s);
    LOG_NORMAL("rstp get max age = [%s] OK\n",age_s);

    return 0;
}

/* *
 * * @brief         set rstp max age
 * * @retval 0:     OK
 * * @retval -1:    ERROR
 * */
static int rstpSetMaxAge(void *p)
{
    if(rstp.status != FAL_ENABLE)
    {
        LOG_ERROR("the feature of rstp is disable");
        return -1;
    }

    int age = atoi((char *)p);

    if(age < MIN_AGE_TIME || age > MAX_AGE_TIME)
    {
        LOG_ERROR("invalid arguement!\n");
        return -1;
    }

    if(age == rstp.max_age)
    {
        LOG_NORMAL("same arguement,do nothing\n");
        return 0;
    }
    
    /*调用ctl命令*/
    int ret = cmd_set_bridge_maxage(age);
    if(ret){
        LOG_ERROR("set bridge max age failed!\n");
        return -1; 
    }

    rstp.max_age  = age;

    char buf[8];
    struct uci_ptr ptr = {
        .package = "rstpd",
        .section = "bridge",
        .option =  "maxage",
    };

    sprintf(buf,"%d", age);
    ubus_collect_config(&ptr,buf,UCI_TYPE_STRING,DO_SET);

    LOG_NORMAL("rstp set max age = [%d] OK\n", age);
    return 0;
}

/* *
 * * @brief         get rstp mac addr
 * * @retval 0:     OK
 * * @retval -1:    ERROR
 * */
static int rstpGetMacAddr(struct blob_buf *buf)
{
    char addr_s[16];

    UID_STP_STATE_T uid_state;
    UID_STP_CFG_T uid_cfg;

    int res = cmd_show_bridge(&uid_state, &uid_cfg);
    if(res){
        LOG_ERROR("get rstp uid state failed!\n");
        return -1; 
    }

    sprintf(addr_s, "%02x%02x-%02x%02x-%02x%02x",
           (unsigned char)uid_state.bridge_id.addr[0],
           (unsigned char)uid_state.bridge_id.addr[1],
           (unsigned char)uid_state.bridge_id.addr[2],
           (unsigned char)uid_state.bridge_id.addr[3],
           (unsigned char)uid_state.bridge_id.addr[4],
           (unsigned char)uid_state.bridge_id.addr[5]);
    
    blobmsg_add_string(buf, "mac_addr", addr_s);
    LOG_NORMAL("rstp get mac addr = [%s] OK\n",addr_s);

    return 0;
}

/* *
 * * @brief         get rstp forward delay
 * * @retval 0:     OK
 * * @retval -1:    ERROR
 * */
static int rstpGetForwardDelay(struct blob_buf *buf)
{
    char delay_s[4];
    
    sprintf(delay_s,"%d",rstp.fwd_dly);
    blobmsg_add_string(buf, "forward_delay", delay_s);
    LOG_NORMAL("rstp get forward delay = [%s] OK\n",delay_s);

    return 0;
}

/* *
 * * @brief         set rstp forward delay
 * * @retval 0:     OK
 * * @retval -1:    ERROR
 * */
static int rstpSetForwardDelay(void *p)
{
    if(rstp.status != FAL_ENABLE)
    {
        LOG_ERROR("the feature of rstp is disable");
        return -1;
    }

    int delay = atoi((char *)p);

    if(delay < MIN_FW_DELAY || delay > MAX_FW_DELAY)
    {
        LOG_ERROR("invalid arguement!\n");
        return -1;
    }

    if(delay == rstp.fwd_dly)
    {
        LOG_NORMAL("same arguement,do nothing\n");
        return 0;
    }
    
    /*调用ctl命令*/
    int ret = cmd_set_bridge_fdelay(delay);
    if(ret){
        LOG_ERROR("set bridge forward delay failed!\n");
        return -1; 
    }
    rstp.fwd_dly = delay;
 
    char buf[8];
    struct uci_ptr ptr = {
        .package = "rstpd",
        .section = "bridge",
        .option =  "fdelay",
    };

    sprintf(buf,"%d", delay);
    ubus_collect_config(&ptr,buf,UCI_TYPE_STRING,DO_SET);

    LOG_NORMAL("rstp set forward delay = [%d] OK\n", delay);
    return 0;
}

/* *
 * * @brief             get port status
 * * @param[port]       port index.
 * * @retval 0:         OK
 * * @retval -1:        ERROR
 * */
static int rstpGetPortStatus(struct blob_buf *buf,int port)
{
    char status_s[2];

    sprintf(status_s,"%d",rstp.port[port-1].status);
    blobmsg_add_string(buf,"port_status",status_s);
    LOG_NORMAL("rstp get port [%d] status = %s OK\n",port,status_s);

    return 0;
}

char *port_if[CONFIG_JWS_PORT_TOTAL_NR];

/* *
 * * @brief             set port status
 * * @param[port]       port index.
 * * @retval 0:         OK
 * * @retval -1:        ERROR
 * */
static int rstpSetPortStatus(int port,void *p)
{
    int status = atoi((char *)p);

    /* *< check port cur status*/
    int port_status = get_port_status(port);
    LOG_NORMAL("port_status = %d\n",port_status);

    if(status != FAL_ENABLE && status != FAL_DISABLE)
    {
        LOG_ERROR("invalid arguement!\n");
        return -1;
    }
    
    if(status == rstp.port[port-1].status)
    {
        LOG_NORMAL("same arguement,do nothing\n");
        return 0;
    }
    if(status == FAL_ENABLE){     
        if((port_status != PORT_STATUS_NORMAL) && (port_status != PORT_STATUS_RSTP)){
            LOG_ERROR("set port %d cur port status fail\n", port);
            return -2;
        }
    }

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

    /*调用ctl命令*/
    int ret = cmd_set_port_nonstp(port, value);
    if(ret){
        LOG_ERROR("set bridge port[%d] status failed!\n", port);
        return -1; 
    }
    
    rstp.port[port-1].status = status;

    if(status == FAL_ENABLE)
        set_port_status(port, PORT_STATUS_RSTP);
    else if((status == FAL_DISABLE)&&(get_port_status(port) == PORT_STATUS_RSTP))
        set_port_status(port, PORT_STATUS_NORMAL);

    char buf[2];
    struct uci_ptr ptr = {
        .package = "rstpd",
        .section = strdup(port_if[port-1]),
        .option = "portnonstp",
    };

    sprintf(buf,"%s",(status == FAL_ENABLE)?"no":"yes");
    ubus_collect_config(&ptr,buf,UCI_TYPE_STRING,DO_SET);


    LOG_NORMAL("set port [%d] status = [%d] OK\n",port,status);
    return 0;
}

/* *
 * * @brief             get port priority 
 * * @param[port]       port index.
 * * @retval 0:         OK
 * * @retval -1:        ERROR
 * */
static int rstpGetPortPriority(struct blob_buf *buf,int port)
{
    char prio_s[4];

    sprintf(prio_s,"%d",rstp.port[port-1].prio);
    blobmsg_add_string(buf,"port_priority",prio_s);
    LOG_NORMAL("rstp get port [%d] priority = %s OK\n",port,prio_s);

    return 0;
}

/* *
 * * @brief             set port priority
 * * @param[port]       port index.
 * * @retval 0:         OK
 * * @retval -1:        ERROR
 * */
static int rstpSetPortPriority(int port,void *p)
{
    if(rstp.port[port-1].status != FAL_ENABLE)
    {
        LOG_ERROR("the feature of protocol for port %d is disable!\n",port);
        return -1;
    }

    int prio = atoi((char *)p);

    if(prio < 0 || prio > MAX_PORT_PRIO_NUM)
    {
        LOG_ERROR("invalid arguement!\n");
        return -1;
    }
    if(prio == rstp.port[port-1].prio)
    {
        LOG_NORMAL("same arguement,do nothing\n");
        return 0;
    }
    
    /*调用ctl命令*/
    int ret = cmd_set_port_prio(port, prio);
    if(ret){
        LOG_ERROR("set bridge port[%d] prio failed!\n", port);
        return -1; 
    }

    rstp.port[port-1].prio = prio;

    char buf[2];
    struct uci_ptr ptr = {
        .package = "rstpd",
        .section = strdup(port_if[port-1]),
        .option = "portprio",
    };

    sprintf(buf,"%d",prio);
    ubus_collect_config(&ptr,buf,UCI_TYPE_STRING,DO_SET);

    LOG_NORMAL("rstp set port [%d] priority = [%d] OK\n",port,prio);
    return 0;
}

/* *
 * * @brief             get port path cost
 * * @param[port]       port index.
 * * @retval 0:         OK
 * * @retval -1:        ERROR
 * */
static int rstpGetPathCost(struct blob_buf *buf,int port)
{
    char cost_s[16];

    sprintf(cost_s,"%ld",rstp.port[port-1].ph_cost);
    blobmsg_add_string(buf,"port_path_cost",cost_s);
    LOG_NORMAL("rstp get port [%d] path cost = %s OK\n",port,cost_s);

    return 0;
}

/* *
 * * @brief             set port path cost
 * * @param[port]       port index.
 * * @retval 0:         OK
 * * @retval -1:        ERROR
 * */
static int rstpSetPathCost(int port,void *p)
{
    if(rstp.port[port-1].status != FAL_ENABLE)
    {
        LOG_ERROR("the feature of protocol for port %d is disable!\n",port);
        return -1;
    }
    if(rstp.port[port-1].auto_cal == FAL_ENABLE)
    {
        LOG_ERROR("current mode of port %d is auto calculate cost!\n",port);
        return -1;
    }

    int cost = atoi((char *)p);

    if(cost < 1 || cost > MAX_PORT_PATH_COST)
    {
        LOG_ERROR("invalid arguement!\n");
        return -1;
    }
    if(cost == rstp.port[port-1].ph_cost)
    {
        LOG_NORMAL("same arguement,do nothing\n");
        return 0;
    }
    
    /*调用ctl命令*/
    int ret = cmd_set_port_pathcost(port, cost);
    if(ret){
        LOG_ERROR("set bridge port[%d] cost failed!\n", port);
        return -1; 
    }
    rstp.port[port-1].ph_cost = cost;

    char buf[2];
    struct uci_ptr ptr = {
        .package = "rstpd",
        .section = strdup(port_if[port-1]),
        .option = "portpathcost",
    };

    sprintf(buf,"%d",cost);
    ubus_collect_config(&ptr,buf,UCI_TYPE_STRING,DO_SET);

    LOG_NORMAL("rstp set port [%d] path cost = [%d] OK\n",port,cost);
    return 0;
}

/* *
 * * @brief             get port-auto-calculate-cost
 * * @param[port]       port index.
 * * @retval 0:         OK
 * * @retval -1:        ERROR
 * */
static int rstpGetAutoCalculat(struct blob_buf *buf,int port)
{
    char state_s[2];

    sprintf(state_s,"%d",rstp.port[port-1].auto_cal);
    blobmsg_add_string(buf,"port_auto_calculate_cost",state_s);
    LOG_NORMAL("rstp get port [%d] auto calculate cost = %s OK\n",port,state_s);

    return 0;
}

/* *
 * * @brief             set port-auto-calculate-cost
 * * @param[port]       port index.
 * * @retval 0:         OK
 * * @retval -1:        ERROR
 * */
static int rstpSetAutoCalculat(int port,void *p)
{
    if(rstp.port[port-1].status != FAL_ENABLE)
    {
        LOG_ERROR("the feature of protocol for port %d is disable!\n",port);
    }

    int state = atoi((char *)p);

    if(state != FAL_ENABLE && state != FAL_DISABLE)
    {
        LOG_ERROR("invalid arguement!\n");
        return -1;
    }
    if(state == rstp.port[port-1].auto_cal)
    {
        LOG_NORMAL("same arguement,do nothing\n");
        return 0;
    }
    
    rstp.port[port-1].auto_cal = state;

    char buf[2];
    struct uci_ptr ptr = {
        .package = "rstpd",
        .section = strdup(port_if[port-1]),
        .option = "portautocost",
    };

    sprintf(buf,"%d",state);
    ubus_collect_config(&ptr,buf,UCI_TYPE_STRING,DO_SET);

    LOG_NORMAL("rstp set port [%d] auto calculate cost mode = [%d] OK\n",port,state);
    return 0;
}


/* *
 * * @brief             get port-edge
 * * @param[port]       port index.
 * * @retval 0:         OK
 * * @retval -1:        ERROR
 * */
static int rstpGetPortEdge(struct blob_buf *buf,int port)
{
    char state_s[2];

    sprintf(state_s,"%d",rstp.port[port-1].edge);
    blobmsg_add_string(buf,"port_edge",state_s);
    LOG_NORMAL("rstp get port [%d] edge = %s OK\n",port,state_s);

    return 0;
}

/* *
 * * @brief             set port-edge
 * * @param[port]       port index.
 * * @retval 0:         OK
 * * @retval -1:        ERROR
 * */
static int rstpSetPortEdge(int port,void *p)
{
    if(rstp.port[port-1].status != FAL_ENABLE)
    {
        LOG_ERROR("the feature of protocol for port %d is disable!\n",port);
    }

    int state = atoi((char *)p);

    if(state != FAL_ENABLE && state != FAL_DISABLE)
    {
        LOG_ERROR("invalid arguement!\n");
        return -1;
    }
    if(state == rstp.port[port-1].edge)
    {
        LOG_NORMAL("same arguement,do nothing\n");
        return 0;
    }
    
    int value = 0;
    switch(state){
        case FAL_ENABLE:
            value = 1;
            break;
        case FAL_DISABLE:
            value = 0;
            break;
        default:
            break;
    }

    /*调用ctl命令*/
    int ret = cmd_set_port_edge(port, value);
    if(ret){
        LOG_ERROR("set bridge port[%d] edge failed!\n", port);
        return -1; 
    }
    rstp.port[port-1].edge = state;

    char buf[2];
    struct uci_ptr ptr = {
        .package = "rstpd",
        .section = strdup(port_if[port-1]),
        .option = "portedge",
    };

    sprintf(buf,"%d",state);
    ubus_collect_config(&ptr,buf,UCI_TYPE_STRING,DO_SET);

    LOG_NORMAL("rstp set port [%d] edge mode = [%d] OK\n",port,state);
    return 0;
}

static const struct jw_switch_policy rstp_tbl[] = {
    [RSTP_STATUS] = {.name = "status", .get_ext_handler = rstpGetStatus, .set_ext_handler = rstpSetStatus}, 
    [RSTP_ROOT_PRIO] = {.name = "root_priority", .get_ext_handler = rstpGetRootPriority}, 
    [RSTP_ROOT_HELLOTIME] = {.name = "root_hello_time", .get_ext_handler = rstpGetRootHelloTime}, 
    [RSTP_ROOT_MAX_AGE] = {.name = "root_max_age", .get_ext_handler = rstpGetRootMaxAge}, 
    [RSTP_ROOT_FORWARD_DELAY] = {.name = "root_forward_delay", .get_ext_handler = rstpGetRootForwardDelay}, 
    [RSTP_ROOT_MAC_ADDRESS] = {.name = "root_mac_addr", .get_ext_handler = rstpGetRootMacAddr}, 
    [RSTP_PRIO] = {.name = "priority", .get_ext_handler = rstpGetPriority, .set_ext_handler = rstpSetPriority}, 
    [RSTP_HELLOTIME] = {.name = "hello_time", .get_ext_handler = rstpGetHelloTime, .set_ext_handler = rstpSetHelloTime}, 
    [RSTP_MAX_AGE] = {.name = "max_age", .get_ext_handler = rstpGetMaxAge, .set_ext_handler = rstpSetMaxAge}, 
    [RSTP_FORWARD_DELAY] = {.name = "forward_delay", .get_ext_handler = rstpGetForwardDelay, .set_ext_handler = rstpSetForwardDelay}, 
    [RSTP_MAC_ADDRESS] = {.name = "mac_addr", .get_ext_handler = rstpGetMacAddr}, 
    [RSTP_PORT_STATUS] = {.name = "port_status", .get_handler = rstpGetPortStatus, .set_handler = rstpSetPortStatus}, 
    [RSTP_PORT_PRIO] = {.name = "port_priority", .get_handler = rstpGetPortPriority, .set_handler = rstpSetPortPriority}, 
    [RSTP_PORT_PATH_COST] = {.name = "port_path_cost", .get_handler = rstpGetPathCost, .set_handler = rstpSetPathCost}, 
    [RSTP_PORT_ATUO_CAL] = {.name = "port_auto_calculate_cost", .get_handler = rstpGetAutoCalculat, .set_handler = rstpSetAutoCalculat}, 
    [RSTP_GET_PORT_NUM] = {.name = "port_get_num", .get_ext_handler = rstpGetPortNum}, 
    [RSTP_GET_PORT_NAME] = {.name = "port_get_name", .get_handler = rstpGetReadPortName}, 
    [RSTP_GET_PORT_STATE] = {.name = "port_get_state", .get_handler = rstpGetReadPortState}, 
    [RSTP_GET_PORT_PRIO] = {.name = "port_get_prio", .get_handler = rstpGetReadPortPrio}, 
    [RSTP_GET_PORT_ROLE] = {.name = "port_get_role", .get_handler = rstpGetReadPortRole}, 
    [RSTP_GET_PORT_TYPE] = {.name = "port_get_type", .get_handler = rstpGetReadPortType}, 
    [RSTP_GET_PORT_COST] = {.name = "port_get_cost", .get_handler = rstpGetReadPortCost}, 
    [RSTP_PORT_EDGE] = {.name = "port_edge", .get_handler = rstpGetPortEdge, .set_handler = rstpSetPortEdge}, 
};

/* *
 * * @brief         parse port rstp get
 * * @param[lvl3]   level 3 message structure.
 * * @retval 0:     OK
 * * @retval -1:    ERROR
 * */
static int rstpParsePortGet(struct blob_attr *lvl3, struct blob_buf *buf)
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
    foreach_json_array_get(idx,lvl3,buf,rstp_tbl, __RSTP_TBL_MAX);
    blobmsg_close_array(buf, ret_array);	

    return 0;
}

/* *
 * * @brief         parse port rstp set
 * * @param[lvl3]   level 3 message structure.
 * * @retval 0:     OK
 * * @retval -1:    ERROR
 * */
static int rstpParsePortSet(struct blob_attr *lvl3)
{
    const char *name    = blobmsg_name(lvl3);
    int idx     = atoi(name + 5);

    if (idx < 1 || idx > CONFIG_JWS_PORT_TOTAL_NR) 
    {
        LOG_ERROR("invalid port index!\n");
        return -1;
    }

    if(rstp.status != FAL_ENABLE)
    {
        LOG_ERROR("the feature of rstp is disable!\n");
    }

    /**<    foreach array of level 3 message  */
    int ret;
    ret = foreach_json_array_set(idx,lvl3,rstp_tbl,__RSTP_TBL_MAX);
    if(ret)
    {
        LOG_ERROR("foreach index %d error!\n",idx);
        return -1;
    }

    return 0;
}


/* *
 * * @brief         method "rstp_get" handler
 * * @retval 0:     OK
 * * @retval -1:    ERROR
 * */
static int rstpGetHandler(struct ubus_context *ctx, struct ubus_object *obj,
                struct ubus_request_data *req, const char *method,
                struct blob_attr *msg)
{
    struct blob_attr *tb[__RSTP_GET_MAX];    
	struct blob_attr *lvl1 = NULL;  

    blobmsg_parse(rstp_get_policy, __RSTP_GET_MAX, tb, blob_data(msg), blob_len(msg));
	lvl1 = tb[RSTP_GET_KEY];     
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

                policy_p = (struct jw_switch_policy *)jw_switchd_get_context(name, rstp_tbl, __RSTP_TBL_MAX);
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
                    rstpParsePortGet(lvl3,&b);
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
 * * @brief         method "rstp_set" handler
 * * @retval 0:     OK
 * * @retval -1:    ERROR
 * */
static int rstpSetHandler(struct ubus_context *ctx, struct ubus_object *obj,
                struct ubus_request_data *req, const char *method,
                struct blob_attr *msg)
{
    struct blob_attr *tb[__RSTP_SET_MAX];    
	struct blob_attr *lvl1 = NULL;  
    int ret = -1;

    blobmsg_parse(rstp_set_policy, __RSTP_SET_MAX, tb, blob_data(msg), blob_len(msg));
	lvl1 = tb[RSTP_SET_KEY];     
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

                    policy_p = (struct jw_switch_policy *)jw_switchd_get_context(name, rstp_tbl, __RSTP_TBL_MAX);
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
                    ret = rstpParsePortSet(lvl3);
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


static const struct ubus_method rstp_methods[] = {
	UBUS_METHOD("rstp_get", rstpGetHandler, rstp_get_policy),
    UBUS_METHOD("rstp_set", rstpSetHandler, rstp_set_policy),
};


static struct ubus_object_type rstp_object_type =
	UBUS_OBJECT_TYPE("rstp", rstp_methods);


static struct ubus_object rstp_object = {
	.name = "rstp",
	.type = &rstp_object_type,
	.methods = rstp_methods,
	.n_methods = ARRAY_SIZE(rstp_methods),
};

static int rstpLoadState(struct uci_ptr *ptr)
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
    
    rstp.status = state;

    LOG_NORMAL("rstp load status = %d OK!\n",rstp.status);
    return 0;
}

static int rstpLoadPrio(struct uci_ptr *ptr)
{
    int prio = atoi(ptr->o->v.string);

    if(prio < 0 || prio > MAX_PRIO_NUM)
    {
        LOG_ERROR("invalid arguement!\n"); 
        return -1;
    }

    rstp.prio = prio;

    LOG_NORMAL("rstp load priority = %d OK!\n",prio);
    return 0;
}

static int rstpLoadHelloTime(struct uci_ptr *ptr)
{
    int hello = atoi(ptr->o->v.string);

    if(hello < MIN_HELLO_TIME || hello > MAX_HELLO_TIME)
    {
        LOG_ERROR("invalid arguement!\n"); 
        return -1;
    }

    rstp.hello_t = hello;

    LOG_NORMAL("rstp load hello time = %d OK!\n",hello);
    return 0;
}

static int rstpLoadMaxAge(struct uci_ptr *ptr)
{
    int age = atoi(ptr->o->v.string);

    if(age < MIN_AGE_TIME || age > MAX_AGE_TIME)
    {
        LOG_ERROR("invalid arguement!\n"); 
        return -1;
    }

    rstp.max_age = age;

    LOG_NORMAL("rstp load max age = %d OK!\n",age);
    return 0;
}

static int rstpLoadForwardDelay(struct uci_ptr *ptr)
{
    int delay = atoi(ptr->o->v.string);

    if(delay < MIN_FW_DELAY || delay > MAX_FW_DELAY)
    {
        LOG_ERROR("invalid arguement!\n"); 
        return -1;
    }

    rstp.fwd_dly = delay;

    LOG_NORMAL("rstp load forward delay time = %d OK!\n",delay);
    return 0;
}

static int rstpLoadPortState(struct uci_ptr *ptr)
{
    int port = atoi(ptr->section + strlen("gex"));
    int state;

    if(!strcmp(ptr->o->v.string,"no"))
    {
        state = FAL_DISABLE;
    }
    else if(!strcmp(ptr->o->v.string,"yes"))
    {
        state = FAL_ENABLE;
    }
    else
    {
        LOG_ERROR("invalid arguement!\n"); 
        return -1;
    }

    rstp.port[port-1].status = state;

    LOG_NORMAL("rstp load port %d state = %d OK!\n",port,state);
    return 0;
}

static int rstpLoadPortPrio(struct uci_ptr *ptr)
{
    int port = atoi(ptr->section + strlen("gex"));
    int prio = atoi(ptr->o->v.string);

    if(prio < 0 || prio > MAX_PORT_PRIO_NUM)
    {
        LOG_ERROR("invalid arguement!\n");
        return -1;
    }

    rstp.port[port-1].prio = prio;

    LOG_NORMAL("rstp load port %d prio = %d OK!\n",port,prio);
    return 0;
}

static int rstpLoadPortPathCost(struct uci_ptr *ptr)
{
    int port = atoi(ptr->section + strlen("gex"));
    unsigned long cost = atoi(ptr->o->v.string);

    if(cost < 0 || cost > MAX_PORT_PATH_COST)
    {
        LOG_ERROR("invalid arguement!\n");
        return -1;
    }

    rstp.port[port-1].ph_cost = cost;

    LOG_NORMAL("rstp load port %d pathcost = %d OK!\n",port,cost);
    return 0;
}

static int rstpLoadPortAutoCost(struct uci_ptr *ptr)
{
    int port = atoi(ptr->section + strlen("gex"));
    int mode = atoi(ptr->o->v.string);

    if(mode != FAL_ENABLE && mode != FAL_DISABLE)
    {
        LOG_ERROR("invalid arguement!\n");
        return -1;
    }

    fal_port_type_t type;
    fal_port_comb_t comb;
    int res = fal_port_type_get(0,port,&type, &comb);
    if(res){
        LOG_ERROR(" get port %d type fail\n", port);
        return -1;
    }
    int autocost;
    switch(type){
        case FAL_PORT_TYPE_FX:
        case FAL_PORT_TYPE_FE:
            autocost = 200000;
            break;
        case FAL_PORT_TYPE_GX:
        case FAL_PORT_TYPE_GE:
            autocost = 20000;
            break;
        default:
            autocost = 20000;
            break;        
    }


    if(mode == FAL_ENABLE)
    {
        rstp.port[port-1].auto_cal = FAL_ENABLE;
        rstp.port[port-1].ph_cost = autocost;
    }
    else
    {
        rstp.port[port-1].auto_cal = FAL_DISABLE;
    }

    LOG_NORMAL("rstp load port %d autocost = %d OK!\n",port,mode);
    return 0;
}

static int rstpLoadPortEdge(struct uci_ptr *ptr)
{
    int port = atoi(ptr->section + strlen("gex"));
    int edge = atoi(ptr->o->v.string);

    if(edge != FAL_ENABLE && edge != FAL_DISABLE)
    {
        LOG_ERROR("invalid arguement!\n");
        return -1;
    }

    rstp.port[port-1].edge = edge;

    LOG_NORMAL("rstp load port %d edge = %d OK!\n",port,edge);
    return 0;
}

static UciConfig_S rstpUci[] = {
    {.name = "admin.rstp",          .type = UCI_TYPE_STRING,    .ubusGet = rstpLoadState},
    {.name = "bridge.bridgeprio",   .type = UCI_TYPE_STRING,    .ubusGet = rstpLoadPrio},
    {.name = "bridge.hello",        .type = UCI_TYPE_STRING,    .ubusGet = rstpLoadHelloTime},
    {.name = "bridge.maxage",       .type = UCI_TYPE_STRING,    .ubusGet = rstpLoadMaxAge},
    {.name = "bridge.fdelay",       .type = UCI_TYPE_STRING,    .ubusGet = rstpLoadForwardDelay},
    {.name = "portnonstp",          .type = UCI_TYPE_STRING,    .ubusGet = rstpLoadPortState},
    {.name = "portprio",            .type = UCI_TYPE_STRING,    .ubusGet = rstpLoadPortPrio},
    {.name = "portpathcost",        .type = UCI_TYPE_STRING,    .ubusGet = rstpLoadPortPathCost},
    {.name = "portautocost",        .type = UCI_TYPE_STRING,    .ubusGet = rstpLoadPortAutoCost},
    {.name = "portedge",            .type = UCI_TYPE_STRING,    .ubusGet = rstpLoadPortEdge},
};


static int rstpCfgLoad(void)
{
    memset(&rstp,0,sizeof(rstp));

    char name[64]; 
    int ret;
    int i;
    struct uci_ptr ptr;

    struct uci_context *ctx = uci_alloc_context();
    struct uci_package *package = NULL;
    struct uci_element *elt;
    uci_load(ctx, "rstpd", &package);

    if(package == NULL){
        LOG_ERROR("/etc/config have no rstpd!\n");
        return -1;
    }

    uci_foreach_element(&package->sections, elt)
    {
        if((!strncmp(elt->name, "gex", strlen("gex")))||(!strncmp(elt->name, "fex", strlen("fex")))){
            //LOG_NORMAL("elt->name = %s, port = %d, len = %d\n", elt->name, atoi(elt->name + 3),strlen(elt->name));
            port_if[(atoi(elt->name + 3) - 1)] = (char*)calloc(1, strlen(elt->name));
            memcpy(port_if[(atoi(elt->name + 3) - 1)], elt->name, strlen(elt->name));
        }
        for(i = 0;i < sizeof(rstpUci)/sizeof(UciConfig_S);i++)//遍历
        {
            if(((!strncmp(elt->name, "gex", strlen("gex")))||(!strncmp(elt->name, "fex", strlen("fex")))) && (i > 4)){
                sprintf(name,"rstpd.%s.%s",elt->name, rstpUci[i].name);
            }
            else if((!strncmp(elt->name, "admin", strlen("admin"))) && (i == 0)){
                sprintf(name,"rstpd.%s", rstpUci[i].name);
            }
            else if((!strncmp(elt->name, "bridge", strlen("bridge"))) && (i > 0) && (i <= 4) ){
                sprintf(name,"rstpd.%s", rstpUci[i].name);
            }
            else{
                continue;
            }

            ret = load_config(name,rstpUci[i].type,&ptr,ctx);
            if(ret)
            {
                uci_free_context(ctx);

                if(rstpUci[i].type == UCI_TYPE_LIST) continue;

                LOG_ERROR("load config %s fail!\n",name); 
                return -1;
            }

            ret = rstpUci[i].ubusGet(&ptr);
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

void ubus_init_rstp(struct ubus_context *ctx)
{
	int ret;


	_ctx = ctx;

    ret = rstpCfgLoad();
    if(ret)
    {
		LOG_ERROR("rstp load configure from UCI file fail\n");
        return;
    }
    /**< 处理端口状态*/
    rstpProcessPortStatus(rstp.status);

	ret = ubus_add_object(ctx, &rstp_object);
    
	if (ret)
    {
		LOG_ERROR("Failed to add object [rstp]: %s\n", ubus_strerror(ret));
    }
    ctl_rstp_init();
    //cmd_show_port(1);
}


