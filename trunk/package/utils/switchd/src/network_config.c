/* *< 中威文件编码格式为gb2312,cp936 */
/* * $Id$ $DateTime$
 * * @file vlan_config.c
 * * @brief This is a table of the switch's network configure.
 * * @version 0.0.1
 * * @since 0.0.1
 * * @author code<songyaofei@joyware.com>
 * * @date 2016-06-21
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
#include <sys/socket.h>

#include <net/if.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <ctype.h>

#include "switchd.h"

static struct blob_buf b;
static struct ubus_context *_ctx;

//static int networkCfgLoadOnePara(int i);

enum {
    NETWORK_GET_KEY,
    __NETWORK_GET_MAX,
};

enum {
    NETWORK_SET_KEY,
    __NETWORK_SET_MAX
};

static const struct blobmsg_policy network_get_policy[] = {
    [NETWORK_GET_KEY] = {.name = "get_args", .type = BLOBMSG_TYPE_ARRAY},
};

static const struct blobmsg_policy network_set_policy[] = {
    [NETWORK_SET_KEY] = {.name = "set_args", .type = BLOBMSG_TYPE_ARRAY},
};

enum {
    NETWORK_IP,
    NETWORK_MASK,
    NETWORK_GATEWAY,
    NETWORK_PROTO,
    __NETWORK_TBL_MAX
};

#define STATIC_S "static"
#define DHCP_S "dhcp"
static struct{
    char ip[16];
    char mask[16];
    char gw[16];
    int proto;
    char *ifname;
}networkCfg;


static int networkGetIp(struct blob_buf *buf)
{ 
    int inet_sock;
    struct ifreq ifr;
    inet_sock = socket(AF_INET, SOCK_DGRAM, 0);

    strcpy(ifr.ifr_name, "br-lan");
    //SIOCGIFADDR标志代表获取接口地址
    if (ioctl(inet_sock, SIOCGIFADDR, &ifr) <  0)
        perror("ioctl");
    LOG_NORMAL("ip: %s\n", inet_ntoa(((struct sockaddr_in*)&(ifr.ifr_addr))->sin_addr));

    blobmsg_add_string(buf, "ipaddr",inet_ntoa(((struct sockaddr_in*)&(ifr.ifr_addr))->sin_addr));
    LOG_NORMAL("network get ip address OK\n");

    return 0;
}

static int networkSetIp(void *p)
{
    char *ip = (char *)p;
    int ret;
    ret = if_valid_ip(ip);
    if(ret)
    {
        LOG_ERROR("invalid arguement!\n"); 
        return -1;
    }

    if(!strcmp(ip,networkCfg.ip))
    {
        LOG_NORMAL("same arguement,do nothing\n");
        return 0;
    }
    
    strcpy(networkCfg.ip,ip);

    struct uci_ptr ptr = {
        .package = "network",
        .section = "lan",
        .option = "ipaddr",
    };
    ubus_collect_config(&ptr,ip,UCI_TYPE_STRING,DO_SET);

    LOG_NORMAL("network set ip address = [%s] OK\n",ip);
    
    return 0;
}

static int networkGetMask(struct blob_buf *buf)
{
    int inet_sock;
    struct ifreq ifr;
    inet_sock = socket(AF_INET, SOCK_DGRAM, 0);

    strcpy(ifr.ifr_name, "br-lan");
    //SIOCGIFADDR标志代表获取接口地址
    if (ioctl(inet_sock, SIOCGIFNETMASK, &ifr) <  0)
        perror("ioctl");
    LOG_NORMAL("mask: %s\n", inet_ntoa(((struct sockaddr_in*)&(ifr.ifr_netmask))->sin_addr));

    blobmsg_add_string(buf, "netmask",inet_ntoa(((struct sockaddr_in*)&(ifr.ifr_netmask))->sin_addr));
    LOG_NORMAL("network get ip address OK\n");

    return 0;
}

static int networkSetMask(void *p)
{
    char *mask = (char *)p;
    int ret;
    ret = if_valid_ip(mask);
    if(ret)
    {
        LOG_ERROR("invalid arguement!\n"); 
        return -1;
    }

    if(!strcmp(mask,networkCfg.mask))
    {
        LOG_NORMAL("same arguement,do nothing\n");
        return 0;
    }
    
    strcpy(networkCfg.mask,mask);

    struct uci_ptr ptr = {
        .package = "network",
        .section = "lan",
        .option = "netmask",
    };
    ubus_collect_config(&ptr,mask,UCI_TYPE_STRING,DO_SET);

    LOG_NORMAL("network set netmask = [%s] OK\n",mask);
    
    return 0;
}


static int networkGetGw(struct blob_buf *buf)
{
    FILE *fp;    
    char gwbuf[512];    
    char cmd[128];    
    char gateway[30];    
    char *tmp;    
    
    strcpy(cmd, "ip route");    
    fp = popen(cmd, "r");    
    if(NULL == fp)    
    {    
        perror("popen error");    
        return -1;    
    }    
    while(fgets(gwbuf, sizeof(gwbuf), fp) != NULL)    
    {    
        tmp =gwbuf;    
        while(*tmp && isspace(*tmp))    
            ++ tmp;    
        if(strncmp(tmp, "default", strlen("default")) == 0)    
            break;    
    }    
    sscanf(gwbuf, "%*s%*s%s", gateway);           
    LOG_NORMAL("default gateway:%s \n", gateway);    
    pclose(fp);

    blobmsg_add_string(buf, "gateway", gateway);
    LOG_NORMAL("network get gateway OK\n");

    return 0;
}

static int networkSetGw(void *p)
{
    char *gw = (char *)p;
    int ret;
    ret = if_valid_ip(gw);
    if(ret)
    {
        LOG_ERROR("invalid arguement!\n"); 
        return -1;
    }

    if(!strcmp(gw,networkCfg.gw))
    {
        LOG_NORMAL("same arguement,do nothing\n");
        return 0;
    }
    
    strcpy(networkCfg.gw,gw);

    struct uci_ptr ptr = {
        .package = "network",
        .section = "lan",
        .option = "gateway",
    };
    ubus_collect_config(&ptr,gw,UCI_TYPE_STRING,DO_SET);

    LOG_NORMAL("network set gateway = [%s] OK\n",gw);
    
    return 0;
}


static int networkGetProto(struct blob_buf *buf)
{
/*    int ret;
    ret = networkCfgLoadOnePara(3);
    if(ret)
    {
		LOG_ERROR("network protocol load configure from UCI file fail\n");
        return -1;
    }
*/
    char proto_s[2];

    sprintf(proto_s,"%d",networkCfg.proto);
    blobmsg_add_string(buf, "protocol", proto_s);
    LOG_NORMAL("network get protocol OK\n");

    return 0;
}

static int networkSetProto(void *p)
{
    int proto = atoi((char *)p);

    if(proto != FAL_DISABLE && proto != FAL_ENABLE)
    {
        LOG_ERROR("invalid arguement!\n"); 
        return -1;
    }

    if(proto == networkCfg.proto)
    {
        LOG_NORMAL("same arguement,do nothing\n");
        return 0;
    }
    
    networkCfg.proto = proto;

    char buf[2];
    struct uci_ptr ptr = {
        .package = "network",
        .section = "lan",
        .option = "proto",
    };
    sprintf(buf,"%d",proto);
    ubus_collect_config(&ptr,buf,UCI_TYPE_STRING,DO_SET);

    LOG_NORMAL("network set protocol = [%d] OK\n",proto);
    
    return 0;
}

static const struct jw_switch_policy network_tbl[] = {
    [NETWORK_IP] = {.name = "ipaddr", .get_ext_handler = networkGetIp, .set_ext_handler = networkSetIp}, 
    [NETWORK_MASK] = {.name = "netmask", .get_ext_handler = networkGetMask, .set_ext_handler = networkSetMask},
    [NETWORK_GATEWAY] = {.name = "gateway", .get_ext_handler = networkGetGw, .set_ext_handler = networkSetGw},
    [NETWORK_PROTO] = {.name = "protocol", .get_ext_handler = networkGetProto, .set_ext_handler = networkSetProto},
};

/* *
 * * @brief         handle function for the method of "network_get"
 * * @retval 0:     OK
 * * @retval -1:    ERROR
 * */
static int networkGetHandler(struct ubus_context *ctx, struct ubus_object *obj,
                struct ubus_request_data *req, const char *method,
                struct blob_attr *msg)
{
    struct blob_attr *tb[__NETWORK_GET_MAX];
	struct blob_attr *lvl1 = NULL;  

    blobmsg_parse(network_get_policy, __NETWORK_GET_MAX, tb, blob_data(msg), blob_len(msg));
	lvl1 = tb[NETWORK_GET_KEY];     
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

            policy_p = (struct jw_switch_policy *)jw_switchd_get_context(name, network_tbl, __NETWORK_TBL_MAX);
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
 * * @brief         handle function for the method of "network_set"
 * * @retval 0:     OK
 * * @retval -1:    ERROR
 * */
static int networkSetHandler(struct ubus_context *ctx, struct ubus_object *obj,
                struct ubus_request_data *req, const char *method,
                struct blob_attr *msg)
{
    struct blob_attr *tb[__NETWORK_SET_MAX];    
	struct blob_attr *lvl1 = NULL;  
    int ret = -1;

    blobmsg_parse(network_set_policy, __NETWORK_SET_MAX, tb, blob_data(msg), blob_len(msg));
	lvl1 = tb[NETWORK_SET_KEY];     
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
                
                policy_p = (struct jw_switch_policy *)jw_switchd_get_context(name, network_tbl, __NETWORK_TBL_MAX);
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
static const struct ubus_method network_methods[] = {
	UBUS_METHOD("network2_get", networkGetHandler, network_get_policy),
    UBUS_METHOD("network2_set", networkSetHandler, network_set_policy),
};


static struct ubus_object_type network_object_type =
	UBUS_OBJECT_TYPE("network2", network_methods);


static struct ubus_object network_object = {
	.name = "network2",
	.type = &network_object_type,
	.methods = network_methods,
	.n_methods = ARRAY_SIZE(network_methods),
};

/* *
 * * @brief             load network arguement - IP from UCI file
 * * @param[*p]         buffer to be parse 
 * * @retval 0:         OK
 * * @retval -1:        ERROR
 * */
static int networkCfgLoadIp(struct uci_ptr *ptr)
{
    int ret;
    char *ip = ptr->o->v.string;

    ret = if_valid_ip(ip);
    if(ret)
    {
        LOG_ERROR("[%s] is an invalid ipv4 address!\n",ip); 
        return -1;
    }

    strcpy(networkCfg.ip,ip);
    LOG_NORMAL("network load ip OK\n");
    
    return 0;
}

/* *
 * * @brief             load network arguement - gateway from UCI file
 * * @param[*p]         buffer to be parse 
 * * @retval 0:         OK
 * * @retval -1:        ERROR
 * */
static int networkCfgLoadGw(struct uci_ptr *ptr)
{
    int ret;
    char *gw = ptr->o->v.string;

    ret = if_valid_ip(gw);
    if(ret)
    {
        LOG_ERROR("[%s] is an invalid ipv4 address!\n",gw); 
        return -1;
    }

    strcpy(networkCfg.gw,gw);
    LOG_NORMAL("network load gateway OK\n");
    
    return 0;
}

/* *
 * * @brief             load network arguement - netmask from UCI file
 * * @param[*p]         buffer to be parse 
 * * @retval 0:         OK
 * * @retval -1:        ERROR
 * */
static int networkCfgLoadMask(struct uci_ptr *ptr)
{
    int ret;
    char *mask = ptr->o->v.string;

    ret = if_valid_ip(mask);
    if(ret)
    {
        LOG_ERROR("[%s] is an invalid ipv4 address!\n",mask); 
        return -1;
    }

    strcpy(networkCfg.mask,mask);
    LOG_NORMAL("network load netmask OK\n");
    
    return 0;
}

/* *
 * * @brief             load network arguement - protocol from UCI file
 * * @param[*p]         buffer to be parse 
 * * @retval 0:         OK
 * * @retval -1:        ERROR
 * */
static int networkCfgLoadProto(struct uci_ptr *ptr)
{
    char *proto = ptr->o->v.string;

    if(!strcasecmp(STATIC_S,proto))
    {
        networkCfg.proto = 1;
    }
    else if(!strcasecmp(DHCP_S,proto))
    {
        networkCfg.proto = 2;
    }
    else
    {
        LOG_ERROR("invalid arguement!\n"); 
        return -1;
    }

    LOG_NORMAL("network load protocal OK\n");
    
    return 0;
}

static int networkCfgLoadIfname(struct uci_ptr *ptr)
{
    networkCfg.ifname = strdup(ptr->o->v.string);
    LOG_NORMAL("network load ifname  = %s OK\n",networkCfg.ifname);
    return 0;
}

int ubus_is_ifname(char *ifname)
{
    return strstr(networkCfg.ifname,ifname)?1:0;
}

static UciConfig_S netUci[] = {
    {.name = "ipaddr",  .type = UCI_TYPE_STRING,    .ubusGet = networkCfgLoadIp},
    {.name = "netmask", .type = UCI_TYPE_STRING,    .ubusGet = networkCfgLoadMask},
    {.name = "gateway", .type = UCI_TYPE_STRING,    .ubusGet = networkCfgLoadGw},
    {.name = "proto",   .type = UCI_TYPE_STRING,    .ubusGet = networkCfgLoadProto},
    {.name = "ifname",  .type = UCI_TYPE_STRING,    .ubusGet = networkCfgLoadIfname},
};


/* *
 * * @brief             load network configure
 * * @retval 0:         OK
 * * @retval -1:        ERROR
 * */
static int networkCfgLoad(void)
{
    struct uci_context *ctx;

    ctx = uci_alloc_context();

    char name[64]; 
    int ret;
    int i;
    struct uci_ptr ptr;

    for(i = 0;i < sizeof(netUci)/sizeof(UciConfig_S);i++)
    {
        sprintf(name,"network.lan.%s",netUci[i].name);
        ret = load_config(name,netUci[i].type,&ptr,ctx);
        if(ret)
        {
            uci_free_context(ctx);

            if(netUci[i].type == UCI_TYPE_LIST) continue;

            LOG_ERROR("load config %s fail!\n",name); 
            return -1;
        }
        
        ret = netUci[i].ubusGet(&ptr);
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

#if 0
/* *
 * * @brief             load network One Para configure
 * * @retval 0:         OK
 * * @retval -1:        ERROR
 * */
static int networkCfgLoadOnePara(int i)
{
    struct uci_context *ctx;

    ctx = uci_alloc_context();

    char name[64]; 
    int ret;
    struct uci_ptr ptr;

    sprintf(name,"network.lan.%s",netUci[i].name);
    ret = load_config(name,netUci[i].type,&ptr,ctx);
    if(ret)
    {
        uci_free_context(ctx);

        LOG_ERROR("load config %s fail!\n",name); 
        return -1;
    }

    ret = netUci[i].ubusGet(&ptr);
    if(ret)
    {
        uci_free_context(ctx);

        LOG_ERROR("ubus get arguement %s fail!\n",name); 
        return -1;
    }

    uci_free_context(ctx);

    return 0;
}
#endif

void ubus_init_network(struct ubus_context *ctx)
{
	int ret;

	_ctx = ctx;

    ret = networkCfgLoad();
    if(ret)
    {
		LOG_ERROR("network load configure from UCI file fail\n");
        return;
    }

	ret = ubus_add_object(ctx, &network_object);
    
	if (ret)
    {
		LOG_ERROR("Failed to add object [network]: %s\n", ubus_strerror(ret));
    }
}
