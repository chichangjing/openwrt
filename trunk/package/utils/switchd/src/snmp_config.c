/* *< 中威文件编码格式为gb2312,cp936 */
/* * $Id$ $DateTime$
 * * @file snmp_config.c
 * * @brief This is a table of the switch's snmp configure.
 * * @version 0.0.1
 * * @since 0.0.1
 * * @author code<songyaofei@josware.com>
 * * @date 2016-06-28
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
    SNMP_GET_KEY,
    __SNMP_GET_MAX,
};

enum {
    SNMP_SET_KEY,
    __SNMP_SET_MAX
};

static const struct blobmsg_policy snmp_get_policy[] = {
    [SNMP_GET_KEY] = {.name = "get_args", .type = BLOBMSG_TYPE_ARRAY},
};

static const struct blobmsg_policy snmp_set_policy[] = {
    [SNMP_SET_KEY] = {.name = "set_args", .type = BLOBMSG_TYPE_ARRAY},
};

enum {
    SNMP_STATUS,
    SNMP_READONLY,
    SNMP_READWRITE,
    SNMP_PORT,
    SNMP_TRAP_STATUS,
    SNMP_TRAP_PORT,
    SNMP_TRAP_SERVER_IP,
    __SNMP_TBL_MAX
};

#define SNMP_READONLY_NAME 16
#define SNMP_READWRITE_NAME 16  
static struct{
    int status;
    char readonly[SNMP_READONLY_NAME];
    char readwrite[SNMP_READWRITE_NAME];
    int port;
    int trap_status;
    int trap_port;
    char trap_server_ip[16];
}snmp;

/* *
 * * @brief         get snmp status 
 * * @retval 0:     OK
 * * @retval -1:    ERROR
 * */
static int snmpGetStatus(struct blob_buf *buf)
{
    char status_s[2];

    sprintf(status_s,"%d",snmp.status);
    blobmsg_add_string(buf, "status", status_s);
    LOG_NORMAL("snmp get status OK\n");

    return 0;
}

/* *
 * * @brief         set snmp status 
 * * @retval 0:     OK
 * * @retval -1:    ERROR
 * */
static int snmpSetStatus(void *p)
{
    int status = atoi((char *)p);

    if(status != FAL_DISABLE && status != FAL_ENABLE)
    {
        LOG_ERROR("invalid arguement!\n");
        return -1;
    }

    if(status == snmp.status)
    {
        LOG_NORMAL("same arguement,do nothing\n");
        return 0;
    }

    snmp.status = status;

    struct uci_ptr ptr = {
        .package = "snmpd",
        .section = "snmpd",
        .option = "snmpdenable",
    };
    char buf[2];
    sprintf(buf, "%d", status);
    ubus_collect_config(&ptr,buf,UCI_TYPE_STRING,DO_SET);
    

    LOG_NORMAL("snmp set status = [%d] OK\n", status);

    return 0;
}

/* *
 * * @brief         get snmp readonly name
 * * @retval 0:     OK
 * * @retval -1:    ERROR
 * */
static int snmpGetReadonlyName(struct blob_buf *buf)
{
    blobmsg_add_string(buf, "readonly_name", snmp.readonly);
    LOG_NORMAL("snmp get readonly community name OK\n");

    return 0;
}

/* *
 * * @brief         set snmp readonly name
 * * @param *p:     buffer store readonly name
 * * @retval 0:     OK
 * * @retval -1:    ERROR
 * */
static int snmpSetReadonlyName(void *p)
{
    if(snmp.status != FAL_ENABLE)
    {
        LOG_ERROR("the feature of snmp is disable!\n"); 
        return -1;
    }

    char *name = (char *)p;
    int len = strlen(name);

    if(len < 1 || len > SNMP_READONLY_NAME)
    {
        LOG_ERROR("invalid arguement!\n"); 
        return -1;
    }
    
    if(!strcmp(name,snmp.readonly))
    {
        LOG_NORMAL("same arguement,do nothing\n");
        return 0;
    }
    
    strcpy(snmp.readonly,name);

    struct uci_ptr ptr = {
        .package = "snmpd",
        .section = "public",
        .option = "community",
    };
    ubus_collect_config(&ptr,name,UCI_TYPE_STRING,DO_SET);

    LOG_NORMAL("snmp set readonly community name = [%s] OK\n",name);
    
    return 0;
}

/* *
 * * @brief         get snmp readwrite name
 * * @retval 0:     OK
 * * @retval -1:    ERROR
 * */
static int snmpGetReadwriteName(struct blob_buf *buf)
{
    blobmsg_add_string(buf, "readwrite_name", snmp.readwrite);
    LOG_NORMAL("snmp get readwrite community name OK\n");

    return 0;
}

/* *
 * * @brief         set snmp readwrite name
 * * @param *p:     buffer store readwrite name
 * * @retval 0:     OK
 * * @retval -1:    ERROR
 * */
static int snmpSetReadwriteName(void *p)
{
    if(snmp.status != FAL_ENABLE)
    {
        LOG_ERROR("the feature of snmp is disable!\n"); 
        return -1;
    }

    if(snmp.status != FAL_ENABLE)
    {
        LOG_ERROR("the feature of snmp is disable!\n"); 
        return -1;
    }

    char *name = (char *)p;
    int len = strlen(name);

    if(len < 1 || len > SNMP_READWRITE_NAME)
    {
        LOG_ERROR("invalid arguement!\n"); 
        return -1;
    }
    
    if(!strcmp(name,snmp.readwrite))
    {
        LOG_NORMAL("same arguement,do nothing\n");
        return 0;
    }
    
    strcpy(snmp.readwrite,name);

    struct uci_ptr ptr = {
        .package = "snmpd",
        .section = "private",
        .option = "community",
    };
    ubus_collect_config(&ptr,name,UCI_TYPE_STRING,DO_SET);

    LOG_NORMAL("snmp set readwrite community name = [%s] OK\n",name);
    
    return 0;
}

/* *
 * * @brief         get snmp port number
 * * @retval 0:     OK
 * * @retval -1:    ERROR
 * */
static int snmpGetPort(struct blob_buf *buf)
{
    char port_s[8];

    sprintf(port_s,"%d",snmp.port);
    blobmsg_add_string(buf, "port", port_s);
    LOG_NORMAL("snmp get port OK\n");

    return 0;
}

/* *
 * * @brief         set snmp port
 * * @retval 0:     OK
 * * @retval -1:    ERROR
 * */
static int snmpSetPort(void *p)
{
    if(snmp.status != FAL_ENABLE)
    {
        LOG_ERROR("the feature of snmp is disable!\n"); 
        return -1;
    }

    int port = atoi((char *)p);

    if(port < 1 || port > 65535)
    {
        LOG_ERROR("invalid arguement!\n");
        return -1;
    }

    if(port == snmp.port)
    {
        LOG_NORMAL("same arguement,do nothing\n");
        return 0;
    }

    snmp.port = port;
    struct uci_ptr ptr = {
        .package = "snmpd",
        .section = "agent",
        .option = "agentaddress",
    };
    char buf[16];
    sprintf(buf,"UDP:%d",port);
    ubus_collect_config(&ptr,buf,UCI_TYPE_STRING,DO_SET);


    LOG_NORMAL("snmp set port = [%d] OK\n", port);

    return 0;
}

/* *
 * * @brief         get snmp trap status 
 * * @retval 0:     OK
 * * @retval -1:    ERROR
 * */
static int snmpGetTrapStatus(struct blob_buf *buf)
{
    char status_s[2];

    sprintf(status_s,"%d",snmp.trap_status);
    blobmsg_add_string(buf, "trap_status", status_s);
    LOG_NORMAL("snmp get trap status OK\n");

    return 0;
}

/* *
 * * @brief         set snmp trap status 
 * * @param *p:     buffer store trap status
 * * @retval 0:     OK
 * * @retval -1:    ERROR
 * */
static int snmpSetTrapStatus(void *p)
{
    int status = atoi((char *)p);

    if(status != FAL_DISABLE && status != FAL_ENABLE)
    {
        LOG_ERROR("invalid arguement!\n");
        return -1;
    }

    if(status == snmp.trap_status)
    {
        LOG_NORMAL("same arguement,do nothing\n");
        return 0;
    }

    snmp.trap_status = status;
    
    struct uci_ptr ptr = {
        .package = "snmpd",
        .section = "trap",
        .option = "authtrapenable",
    };
    
    char buf[2];
    sprintf(buf, "%d", status);
    ubus_collect_config(&ptr,buf,UCI_TYPE_STRING,DO_SET);

    LOG_NORMAL("snmp set status = [%d] OK\n", status);

    return 0;
}


static int snmpGetTrapPort(struct blob_buf *buf)
{
    char port_s[8];

    sprintf(port_s,"%d",snmp.trap_port);
    blobmsg_add_string(buf, "trap_port", port_s);
    LOG_NORMAL("snmp get trap port OK\n");

    return 0;
}

/* *
 * * @brief         set snmp trap port
 * * @retval 0:     OK
 * * @retval -1:    ERROR
 * */
static int snmpSetTrapPort(void *p)
{
    if(snmp.trap_status != FAL_ENABLE)
    {
        LOG_ERROR("the feature of snmp trap is disable!\n"); 
        return -1;
    }

    int port = atoi((char *)p);

    if(port < 1 || port > 65535)
    {
        LOG_ERROR("invalid arguement!\n");
        return -1;
    }

    if(port == snmp.trap_port)
    {
        LOG_NORMAL("same arguement,do nothing\n");
        return 0;
    }
    
    snmp.trap_port = port;
    struct uci_ptr ptr = {
        .package = "snmpd",
        .section = "trap",
        .option = "serverAddr",
    };
    char buf[32];
    sprintf(buf,"%s:%d",snmp.trap_server_ip,port);
    ubus_collect_config(&ptr,buf,UCI_TYPE_STRING,DO_SET);


    LOG_NORMAL("snmp set port = [%d] OK\n", port);

    return 0;
}

static int snmpGetTrapServerIp(struct blob_buf *buf)
{
    blobmsg_add_string(buf, "trap_server_ip", snmp.trap_server_ip);
    LOG_NORMAL("snmp get trap server ip address OK\n");

    return 0;
}

static int snmpSetTrapServerIp(void *p)
{
    if(snmp.trap_status != FAL_ENABLE)
    {
        LOG_ERROR("the feature of snmp trap is disable!\n"); 
        return -1;
    }

    char *ip = (char *)p;
    int ret;
    ret = if_valid_ip(ip);
    if(ret)
    {
        LOG_ERROR("invalid arguement!\n"); 
        return -1;
    }

    if(!strcmp(ip,snmp.trap_server_ip))
    {
        LOG_NORMAL("same arguement,do nothing\n");
        return 0;
    }
    
    strcpy(snmp.trap_server_ip,ip);
    struct uci_ptr ptr = {
        .package = "snmpd",
        .section = "trap",
        .option = "serverAddr",
    };
    char buf[32];
    sprintf(buf,"%s:%d",ip,snmp.trap_port);
    ubus_collect_config(&ptr,buf,UCI_TYPE_STRING,DO_SET);


    LOG_NORMAL("snmp set trap server ip address = [%s] OK\n",ip);
    
    return 0;
}

static const struct jw_switch_policy snmp_tbl[] = {
    [SNMP_STATUS] = {.name = "status", .get_ext_handler = snmpGetStatus, .set_ext_handler = snmpSetStatus}, 
    [SNMP_READONLY] = {.name = "readonly_name", .get_ext_handler = snmpGetReadonlyName, .set_ext_handler = snmpSetReadonlyName}, 
    [SNMP_READWRITE] = {.name = "readwrite_name", .get_ext_handler = snmpGetReadwriteName, .set_ext_handler = snmpSetReadwriteName}, 
    [SNMP_PORT] = {.name = "port", .get_ext_handler = snmpGetPort, .set_ext_handler = snmpSetPort}, 
    [SNMP_TRAP_STATUS] = {.name = "trap_status", .get_ext_handler = snmpGetTrapStatus, .set_ext_handler = snmpSetTrapStatus}, 
    [SNMP_TRAP_PORT] = {.name = "trap_port", .get_ext_handler = snmpGetTrapPort, .set_ext_handler = snmpSetTrapPort}, 
    [SNMP_TRAP_SERVER_IP] = {.name = "trap_server_ip", .get_ext_handler = snmpGetTrapServerIp, .set_ext_handler = snmpSetTrapServerIp}, 
};


/* *
 * * @brief         handle function for the method of "snmp_get"
 * * @retval 0:     OK
 * * @retval -1:    ERROR
 * */
static int snmpGetHandler(struct ubus_context *ctx, struct ubus_object *obj,
                struct ubus_request_data *req, const char *method,
                struct blob_attr *msg)
{
    struct blob_attr *tb[__SNMP_GET_MAX];
	struct blob_attr *lvl1 = NULL;  

    blobmsg_parse(snmp_get_policy, __SNMP_GET_MAX, tb, blob_data(msg), blob_len(msg));
	lvl1 = tb[SNMP_GET_KEY];     
    if (!lvl1) 
    {
        LOG_ERROR("invalid params in %s\n",__func__);
        return -1;
    }
    if (blobmsg_type(lvl1) != BLOBMSG_TYPE_ARRAY) 
    {
        LOG_ERROR("blobmsg type is not array\n");
        return -1;
    }

    struct blob_attr *lvl2 = NULL; 
    int rem = 0;
    void *ret_array;
    void *ret_table;

	blob_buf_init(&b, 0);

    ret_array = blobmsg_open_array(&b, "ret");

    blobmsg_for_each_attr(lvl2, lvl1, rem) 
    {
        if (blobmsg_type(lvl2) == BLOBMSG_TYPE_STRING) 
        {
            struct jw_switch_policy *policy_p;
            char *name = blobmsg_get_string(lvl2);

            policy_p = (struct jw_switch_policy *)jw_switchd_get_context(name, snmp_tbl, __SNMP_TBL_MAX);
            if (policy_p && policy_p->get_ext_handler) 
            {
                ret_table = blobmsg_open_table(&b, NULL);
                policy_p->get_ext_handler(&b);
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
 * * @brief         handle function for the method of "snmp_set"
 * * @retval 0:     OK
 * * @retval -1:    ERROR
 * */
static int snmpSetHandler(struct ubus_context *ctx, struct ubus_object *obj,
                struct ubus_request_data *req, const char *method,
                struct blob_attr *msg)
{
    struct blob_attr *tb[__SNMP_SET_MAX];    
	struct blob_attr *lvl1 = NULL;  
    int ret = -1;

    blobmsg_parse(snmp_set_policy, __SNMP_SET_MAX, tb, blob_data(msg), blob_len(msg));
	lvl1 = tb[SNMP_SET_KEY];     
    if (!lvl1) 
	{
        LOG_ERROR("invalid params in %s\n",__func__);
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

    blobmsg_for_each_attr(lvl2,lvl1, rem) 
    {
        if (blobmsg_type(lvl2) == BLOBMSG_TYPE_TABLE) 
        {
            struct blob_attr *lvl3 = blobmsg_data(lvl2);
            
            if(blobmsg_type(lvl3) == BLOBMSG_TYPE_STRING)
            {
                struct jw_switch_policy *policy_p = NULL;
                const char *name  = blobmsg_name(lvl3);
                char *value = blobmsg_get_string(lvl3);
                
                policy_p = (struct jw_switch_policy *)jw_switchd_get_context(name, snmp_tbl, __SNMP_TBL_MAX);
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

end:
    blobmsg_add_string(&b, "ret", ret?"-1":"0");
	ubus_send_reply(ctx, req, b.head);

	return 0;
}



static const struct ubus_method snmp_methods[] = {
	UBUS_METHOD("snmp_get", snmpGetHandler, snmp_get_policy),
    UBUS_METHOD("snmp_set", snmpSetHandler, snmp_set_policy),
};


static struct ubus_object_type snmp_object_type =
	UBUS_OBJECT_TYPE("snmp", snmp_methods);


static struct ubus_object snmp_object = {
	.name = "snmp",
	.type = &snmp_object_type,
	.methods = snmp_methods,
	.n_methods = ARRAY_SIZE(snmp_methods),
};

/* *
 * * @brief             load snmp arguement - state from UCI file
 * * @param[*p]         buffer to be parse 
 * * @retval 0:         OK
 * * @retval -1:        ERROR
 * */
static int snmpLoadState(struct uci_ptr *ptr)
{
    int state = atoi(ptr->o->v.string);

    if(state != FAL_DISABLE && state != FAL_ENABLE)
    {
        LOG_ERROR("invalid arguement!\n"); 
        return -1;
    }

    snmp.status = state;
    LOG_NORMAL("snmp load status = [%d] OK\n", state);
    
    return 0;
}

/* *
 * * @brief             load snmp arguement - readonly community name from UCI file
 * * @param[*p]         buffer to be parse 
 * * @retval 0:         OK
 * * @retval -1:        ERROR
 * */
static int snmpLoadReadonlyName(struct uci_ptr *ptr)
{
    char *name = ptr->o->v.string;

    if(strlen(name) >= SNMP_READONLY_NAME)
    {
        LOG_ERROR("invalid arguement!\n");
        return -1; 
    }
    
    strcpy(snmp.readonly,name);
    LOG_NORMAL("snmp load readonly community name = [%s] OK\n",name);

    return 0;
}

/* *
 * * @brief             load snmp arguement - readwrite community name from UCI file
 * * @param[*p]         buffer to be parse 
 * * @retval 0:         OK
 * * @retval -1:        ERROR
 * */
static int snmpLoadReadwriteName(struct uci_ptr *ptr)
{
    char *name = ptr->o->v.string;

    if(strlen(name) >= SNMP_READWRITE_NAME)
    {
        LOG_ERROR("invalid arguement!\n");
        return -1; 
    }
    
    strcpy(snmp.readwrite,name);
    LOG_NORMAL("snmp load readwrite community name = [%s] OK\n",name);

    return 0;
}

/* *
 * * @brief             load snmp arguement - port receive message from UCI file
 * * @param[*p]         buffer to be parse 
 * * @retval 0:         OK
 * * @retval -1:        ERROR
 * */
static int snmpLoadPort(struct uci_ptr *ptr)
{
    if(strncmp(ptr->o->v.string,"UDP:",strlen("UDP:")))
    {
        LOG_ERROR("error format of snmp port in UCI file!\n"); 
        return -1;
    }

    int port = atoi(ptr->o->v.string + strlen("UDP:"));

    if(port < 1 || port > 65535)
    {
        LOG_ERROR("invalid arguement!\n"); 
        return -1;
    }

    snmp.port = port;
    LOG_NORMAL("snmp load port = [%d] OK\n", port);
    
    return 0;
}

/* *
 * * @brief             load snmp arguement - trap status from UCI file
 * * @param[*p]         buffer to be parse 
 * * @retval 0:         OK
 * * @retval -1:        ERROR
 * */
static int snmpLoadTrapStatus(struct uci_ptr *ptr)
{
    LOG_NORMAL("trap status: %s\n", ptr->o->v.string);
    int status = atoi(ptr->o->v.string);

    if(status != FAL_DISABLE && status != FAL_ENABLE)
    {
        LOG_ERROR("invalid arguement!\n"); 
        return -1;
    }

    snmp.trap_status = status;
    LOG_NORMAL("snmp load trap status = [%d] OK\n", status);
    
    return 0;
}

/* *
 * * @brief             load snmp arguement - trap server IP from UCI file
 * * @param[*p]         buffer to be parse 
 * * @retval 0:         OK
 * * @retval -1:        ERROR
 * */
static int snmpLoadTrapServer(struct uci_ptr *ptr)
{
    int ret;
    char *ip = ptr->o->v.string;
    char offset = strcspn(ip,":");
    
    *(ip + offset) = 0;
    ret = if_valid_ip(ip);
    if(ret)
    {
        LOG_ERROR("[%s] is an invalid ipv4 address!\n",ip); 
        return -1;
    }
    strcpy(snmp.trap_server_ip,ip);

    int port = atoi(ip + offset + 1);
    if(port < 1 || port > 65535)
    {
        LOG_ERROR("invalid arguement!\n"); 
        return -1;
    }
    snmp.trap_port = port;

    LOG_NORMAL("snmp load server ip = %s, port = %d OK\n",ip,port);
    
    return 0;
}

static UciConfig_S snmpUci[] = {
    {.name = "snmpd.snmpdenable",   .type = UCI_TYPE_STRING,    .ubusGet = snmpLoadState},
    {.name = "public.community",    .type = UCI_TYPE_STRING,    .ubusGet = snmpLoadReadonlyName},
    {.name = "private.community",   .type = UCI_TYPE_STRING,    .ubusGet = snmpLoadReadwriteName},
    {.name = "agent.agentaddress",  .type = UCI_TYPE_STRING,    .ubusGet = snmpLoadPort},
    {.name = "trap.authtrapenable", .type = UCI_TYPE_STRING,    .ubusGet = snmpLoadTrapStatus},
    {.name = "trap.serverAddr",     .type = UCI_TYPE_STRING,    .ubusGet = snmpLoadTrapServer},
};

int snmpCfgLoad(void)
{
    struct uci_context *ctx = uci_alloc_context();

    char name[64]; 
    int ret;
    int i;
    struct uci_ptr ptr;

    for(i = 0;i < sizeof(snmpUci)/sizeof(UciConfig_S);i++)
    {
        sprintf(name,"snmpd.%s",snmpUci[i].name);
        ret = load_config(name,snmpUci[i].type,&ptr,ctx);
        if(ret)
        {
            uci_free_context(ctx);

            if(snmpUci[i].type == UCI_TYPE_LIST) continue;

            LOG_ERROR("load config %s fail!\n",name); 
            return -1;
        }
        
        ret = snmpUci[i].ubusGet(&ptr);
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

void ubus_init_snmp(struct ubus_context *ctx)
{
	int ret;

	_ctx = ctx;

    ret = snmpCfgLoad();
    if(ret)
    {
		LOG_ERROR("Init snmp configuration fail!\n");
        return;
    }

	ret = ubus_add_object(ctx, &snmp_object);
    
	if (ret)
    {
		LOG_ERROR("Failed to add object [snmp]: %s\n", ubus_strerror(ret));
    }
}
