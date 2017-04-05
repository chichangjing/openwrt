/* *< 中威文件编码格式为gb2312,cp936 */
/* * $Id$ $DateTime$
 * * @file vlan_config.c
 * * @brief This is a table of the switch's vlan configure.
 * * @version 0.0.1
 * * @since 0.0.1
 * * @author code<songyaofei@joyware.com>
 * * @date 2016-05-24
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

#include "jwf-tool.h"

static struct blob_buf b;
static int notify;
static struct ubus_context *_ctx;

#define MIN_SYS_NAME_LEN 3
#define MAX_SYS_NAME_LEN 64
#define MIN_SYS_LOCATE_LEN MIN_SYS_NAME_LEN
#define MAX_SYS_LOCATE_LEN MAX_SYS_NAME_LEN
static struct{
    char mode_name[64];
    char descpt[512];
    char sys_name[MAX_SYS_NAME_LEN];
    char sys_locat[MAX_SYS_LOCATE_LEN];
    char hw_ver[64];
    char bl_ver[64];
    char sw_ver[64];
    int save;
    int clear;
}sysCfg;

enum {
    SYSINFO_GET_KEY,
    __SYSINFO_GET_MAX,
};

enum {
    SYSINFO_SET_KEY,
    __SYSINFO_SET_MAX
};

static const struct blobmsg_policy sysinfo_get_policy[] = {
    [SYSINFO_GET_KEY] = {.name = "get_args", .type = BLOBMSG_TYPE_ARRAY},
};

static const struct blobmsg_policy sysinfo_set_policy[] = {
    [SYSINFO_SET_KEY] = {.name = "set_args", .type = BLOBMSG_TYPE_ARRAY},
};

enum {
    SYSINFO_MODEL_NAME,
    SYSINFO_UNIQUE_ID,
    SYSINFO_MAC,
    SYSINFO_DESCRIPTION,
    SYSINFO_SYS_NAME,
    SYSINFO_SYS_LOCATION,
    SYSINFO_SYS_UPTIME,
    SYSINFO_HARDWARE_VERSION,
    SYSINFO_BOOTLOADER_VERSION,
    SYSINFO_SOFTWARE_VERSION,
    SYSINFO_SAVE_CFG,
    SYSINFO_CLEAR_CFG,
    SYSINFO_PORT_NUM,
    __SYSINFO_TBL_MAX
};

/* *
 * * @brief         system get model name
 * * @retval 0:     OK
 * * @retval -1:    ERROR
 * */
static int sysGetModelName(struct blob_buf *buf)
{
    blobmsg_add_string(buf, "model_name", CONFIG_JWS_PRODUCT_TYPE);
    LOG_NORMAL("get model name OK\n");

    return 0;
}

/* *
 * * @brief         system get unique id
 * * @retval 0:     OK
 * * @retval -1:    ERROR
 * */
static int sysGetUniqueId(struct blob_buf *buf)
{
    blobmsg_add_string(buf, "unique_id", CONFIG_JWS_SERIAL_NUMBER);
    LOG_NORMAL("get unique id OK\n");

    return 0;
}

/* *
 * * @brief         system get port num
 * * @retval 0:     OK
 * * @retval -1:    ERROR
 * */
static int sysGetPortNum(struct blob_buf *buf)
{
    char num_s[2];
    sprintf(num_s, "%d", CONFIG_JWS_PORT_TOTAL_NR);
    blobmsg_add_string(buf, "port_num", num_s);
    LOG_NORMAL("get port num OK\n");

    return 0;
}

/* *
 * * @brief         system get mac
 * * @retval 0:     OK
 * * @retval -1:    ERROR
 * */
static int sysGetMac(struct blob_buf *buf)
{
    int ret;
    unsigned char mac[6];
    char mac_s[18];

    ret = jwf_mac_get(mac);
    if(ret)
    {
        blobmsg_add_string(buf, "mac_addr", "-1");
        LOG_ERROR("device general get mac address fail\n");
        return -1;
    }

    sprintf(mac_s,"%02x-%02x-%02x-%02x-%02x-%02x",mac[0],mac[1],mac[2],mac[3],mac[4],mac[5]);
    blobmsg_add_string(buf, "mac_addr", mac_s);
    LOG_NORMAL("get mac address OK\n");

    return 0;
}

/* *
 * * @brief         system get description
 * * @retval 0:     OK
 * * @retval -1:    ERROR
 * */
static int sysGetDescription(struct blob_buf *buf)
{
    blobmsg_add_string(buf, "description", "joyware industry switch, all rights reserved");
    LOG_NORMAL("get system description OK\n");

    return 0;
}

/* *
 * * @brief         system get system name
 * * @retval 0:     OK
 * * @retval -1:    ERROR
 * */
static int sysGetName(struct blob_buf *buf)
{
    blobmsg_add_string(buf, "sys_name", sysCfg.sys_name);
    LOG_NORMAL("get system name OK\n");

    return 0;
}

/* *
 * * @brief         system set system name
 * * @param *p:     buffer store name
 * * @retval 0:     OK
 * * @retval -1:    ERROR
 * */
static int sysSetName(void *p)
{
    char *name = (char *)p;
    int len = strlen(name);

    if(len < MIN_SYS_NAME_LEN || len > MAX_SYS_NAME_LEN)
    {
        LOG_ERROR("invalid arguement!\n"); 
        return -1;
    }
    
    if(!strcmp(name,sysCfg.sys_name))
    {
        LOG_NORMAL("same system name,do nothing\n");
        return 0;
    }
    
    strcpy(sysCfg.sys_name,name);
    struct uci_ptr ptr = {
        .package = "switchd",
        .section = "device_general",
        .option = "system_name",
    };
    ubus_collect_config(&ptr,name,UCI_TYPE_STRING,DO_SET);

    LOG_NORMAL("set system name = [%s] OK\n",name);
    
    return 0;
}

/* *
 * * @brief         system get location
 * * @retval 0:     OK
 * * @retval -1:    ERROR
 * */
static int sysGetLocation(struct blob_buf *buf)
{
    blobmsg_add_string(buf, "sys_location", sysCfg.sys_locat);
    LOG_NORMAL("get system loacation OK\n");

    return 0;
}

/* *
 * * @brief         system set location
 * * @param *p:     buffer store location
 * * @retval 0:     OK
 * * @retval -1:    ERROR
 * */
static int sysSetLocation(void *p)
{
    char *locate = (char *)p;
    int len = strlen(locate);

    if(len < MIN_SYS_LOCATE_LEN || len > MAX_SYS_LOCATE_LEN)
    {
        LOG_ERROR("invalid arguement!\n"); 
        return -1;
    }

    if(!strcmp(locate,sysCfg.sys_locat))
    {
        LOG_NORMAL("same system location,do nothing\n");
        return 0;
    }

    strcpy(sysCfg.sys_locat,locate);
    struct uci_ptr ptr = {
        .package = "switchd",
        .section = "device_general",
        .option = "system_location",
    };
    ubus_collect_config(&ptr,locate,UCI_TYPE_STRING,DO_SET);

    LOG_NORMAL("set system locate = [%s] OK\n",locate);

    return 0;
}

/* *
 * * @brief         system get uptime
 * * @retval 0:     OK
 * * @retval -1:    ERROR
 * */
static int sysGetUptime(struct blob_buf *buf)
{
    struct sysinfo info;
    char uptime_s[20];
    int ret;

    ret = sysinfo(&info);
    if(ret)
    {
        LOG_ERROR("call function - sysinfo error!\n"); 
        return -1;
    }

    sprintf(uptime_s,"%lu",info.uptime*100);
    blobmsg_add_string(buf, "sys_uptime",uptime_s);
    LOG_NORMAL("get system uptime OK\n");

    return 0;
}

/* *
 * * @brief         system get hardware version
 * * @retval 0:     OK
 * * @retval -1:    ERROR
 * */
static int sysGetHardwareVersion(struct blob_buf *buf)
{
    blobmsg_add_string(buf, "hardware_version", CONFIG_JWS_HARDWARE_VERSION);
    LOG_NORMAL("get hardware version OK\n");

    return 0;
}

/* *
 * * @brief         system get bootloader version
 * * @retval 0:     OK
 * * @retval -1:    ERROR
 * */
static int sysGetBootloaderVersion(struct blob_buf *buf)
{
    blobmsg_add_string(buf, "bootloader_version", "u-boot-2015.04");
    LOG_NORMAL("get bootloader version OK\n");

    return 0;
}

/* *
 * * @brief         system get software version
 * * @retval 0:     OK
 * * @retval -1:    ERROR
 * */
static int sysGetSoftwareVersion(struct blob_buf *buf)
{
    FILE *file = fopen("/etc/openwrt_version","r");
    if(file == NULL)
    {
        LOG_ERROR("open file :/etc/openwrt_version fail!\n");
        return -1;
    }

    char ver_s[32];
    if(fgets(ver_s,sizeof(ver_s),file) == NULL)
    {
        LOG_ERROR("read /etc/openwrt_version error!\n");
        return -1;
    }
    ver_s[strlen(ver_s)-1] = 0;

    blobmsg_add_string(buf, "software_version", ver_s);
    LOG_NORMAL("get software version OK\n");

    return 0;
}

/* *
 * * @brief         system get result of save 
 * * @retval 0:     OK
 * * @retval -1:    ERROR
 * */
static int sysGetSaveCfg(struct blob_buf *buf)
{
    char save_s[2];

    sprintf(save_s,"%d",sysCfg.save);
    blobmsg_add_string(buf, "save_config",save_s);
    LOG_NORMAL("get result of save configure OK\n");

    return 0; 
}

/* *
 * * @brief         system set save action
 * * @param *p      2 - save, 1 - do nothing
 * * @retval 0:     OK
 * * @retval -1:    ERROR
 * */
static int sysSetSaveCfg(void *p)
{
    int save = atoi((char *)p);

    if(save == 2)
    {
        int ret;

        ret = ubus_commit_config();            
        if(ret)
        {
            LOG_ERROR("system configure save to UCI file error!\n");
            return -1;
        }
    }
    else if(save == 1)
    {
        LOG_WARN("save cmd may be incorrectly,do nothing!\n");
    }
    else
    {
        LOG_ERROR("invalid arguement!\n");
        return -1;
    }

    LOG_NORMAL("set save configure cmd OK\n");

    return 0;
}

/* *
 * * @brief         system get result of clear
 * * @retval 0:     OK
 * * @retval -1:    ERROR
 * */
static int sysGetClearCfg(struct blob_buf *buf)
{
    char clear_s[2];

    sprintf(clear_s,"%d",sysCfg.clear);
    blobmsg_add_string(buf, "clear_config",clear_s);
    LOG_NORMAL("get result of clear configure OK\n");

    return 0; 
}

/* *
 * * @brief         system set clear action
 * * @param *p      2 - clear, 1 - do nothing
 * * @retval 0:     OK
 * * @retval -1:    ERROR
 * */
static int sysSetClearCfg(void *p)
{
    int clear = atoi((char *)p);

    if(clear == 2)
    {
        int ret;

        ret = ubus_reset_config();
        if(ret)
        {
            LOG_ERROR("reset UCI file error!\n");
            return -1;
        }
    }
    else if(clear == 1)
    {
        LOG_WARN("clear cmd may be incorrectly,do nothing!\n");
    }
    else
    {
        LOG_ERROR("invalid arguement!\n");
        return -1;
    }

    LOG_NORMAL("set clear configure cmd OK\n");

    return 0;
}


static const struct jw_switch_policy sysinfo_tbl[] = {
    [SYSINFO_MODEL_NAME] = {.name = "model_name", .get_ext_handler = sysGetModelName}, 
    [SYSINFO_UNIQUE_ID] = {.name = "unique_id", .get_ext_handler = sysGetUniqueId},
    [SYSINFO_MAC] = {.name = "mac_addr", .get_ext_handler = sysGetMac},
    [SYSINFO_DESCRIPTION] = {.name = "description", .get_ext_handler = sysGetDescription},
    [SYSINFO_SYS_NAME] = {.name = "sys_name", .get_ext_handler = sysGetName, .set_ext_handler = sysSetName},
    [SYSINFO_SYS_LOCATION] = {.name = "sys_location", .get_ext_handler = sysGetLocation, .set_ext_handler = sysSetLocation},
    [SYSINFO_SYS_UPTIME] = {.name = "sys_uptime", .get_ext_handler = sysGetUptime},
    [SYSINFO_HARDWARE_VERSION] = {.name = "hardware_version", .get_ext_handler = sysGetHardwareVersion},
    [SYSINFO_BOOTLOADER_VERSION] = {.name = "bootloader_version", .get_ext_handler = sysGetBootloaderVersion},
    [SYSINFO_SOFTWARE_VERSION] = {.name = "software_version", .get_ext_handler = sysGetSoftwareVersion},
    [SYSINFO_SAVE_CFG] = {.name = "save_config", .get_ext_handler = sysGetSaveCfg, .set_ext_handler = sysSetSaveCfg},
    [SYSINFO_CLEAR_CFG] = {.name = "clear_config", .get_ext_handler = sysGetClearCfg, .set_ext_handler = sysSetClearCfg},
    [SYSINFO_PORT_NUM] = {.name = "port_num", .get_ext_handler = sysGetPortNum},
};


/* *
 * * @brief         handle function for the method of "sysinfo_get"
 * * @retval 0:     OK
 * * @retval -1:    ERROR
 * */
static int sysGetHandler(struct ubus_context *ctx, struct ubus_object *obj,
                struct ubus_request_data *req, const char *method,
                struct blob_attr *msg)
{
    struct blob_attr *tb[__SYSINFO_GET_MAX];
	struct blob_attr *lvl1 = NULL;  

    blobmsg_parse(sysinfo_get_policy, __SYSINFO_GET_MAX, tb, blob_data(msg), blob_len(msg));
	lvl1 = tb[SYSINFO_GET_KEY];     
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

            policy_p = (struct jw_switch_policy *)jw_switchd_get_context(name, sysinfo_tbl, __SYSINFO_TBL_MAX);
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
 * * @brief         handle function for the method of "sysinfo_set"
 * * @retval 0:     OK
 * * @retval -1:    ERROR
 * */
static int sysSetHandler(struct ubus_context *ctx, struct ubus_object *obj,
                struct ubus_request_data *req, const char *method,
                struct blob_attr *msg)
{
    struct blob_attr *tb[__SYSINFO_SET_MAX];    
	struct blob_attr *lvl1 = NULL;  
    int ret = -1;

    blobmsg_parse(sysinfo_set_policy, __SYSINFO_SET_MAX, tb, blob_data(msg), blob_len(msg));
	lvl1 = tb[SYSINFO_SET_KEY];     
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
                
                policy_p = (struct jw_switch_policy *)jw_switchd_get_context(name, sysinfo_tbl, __SYSINFO_TBL_MAX);
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


static const struct ubus_method system_methods[] = {
	UBUS_METHOD("sysinfo_get", sysGetHandler, sysinfo_get_policy),
    UBUS_METHOD("sysinfo_set", sysSetHandler, sysinfo_set_policy),
};


static struct ubus_object_type system_object_type =
	UBUS_OBJECT_TYPE("sysinfo", system_methods);


static struct ubus_object system_object = {
	.name = "sysinfo",
	.type = &system_object_type,
	.methods = system_methods,
	.n_methods = ARRAY_SIZE(system_methods),
};

static int sysLoadModelname(struct uci_ptr *ptr)
{
    char *name = ptr->o->v.string;

    if(strlen(name) > 64)
    {
        LOG_ERROR("invalid arguement!\n");
        return -1; 
    }
    
    strcpy(sysCfg.mode_name,name);
    LOG_NORMAL("device general set mode name = [%s] OK\n",name);

    return 0;
}

static int sysLoadSysName(struct uci_ptr *ptr)
{
    char *name = ptr->o->v.string;

    if(strlen(name) > MAX_SYS_NAME_LEN)
    {
        LOG_ERROR("invalid arguement!\n");
        return -1; 
    }
    
    strcpy(sysCfg.sys_name,name);
    LOG_NORMAL("device general set system name = [%s] OK\n",name);

    return 0;
}

static int sysLoadSysLocation(struct uci_ptr *ptr)
{
    char *location = ptr->o->v.string;
    
    if(strlen(location) > MAX_SYS_LOCATE_LEN)
    {
        LOG_ERROR("invalid arguement!\n");
        return -1; 
    }
    
    strcpy(sysCfg.sys_locat,location);
    LOG_NORMAL("device general set system location = [%s] OK\n",location);

    return 0;
}

static int sysLoadHardwareVersion(struct uci_ptr *ptr)
{
    char *hw = ptr->o->v.string;

    if(strlen(hw) > 64)
    {
        LOG_ERROR("invalid arguement!\n");
        return -1; 
    }
    
    strcpy(sysCfg.hw_ver,hw);
    LOG_NORMAL("device general set hardware version = [%s] OK\n",hw);

    return 0;
}

static UciConfig_S devUci[] = {
    {.name = "model_name",      .type = UCI_TYPE_STRING,    .ubusGet = sysLoadModelname},
    {.name = "system_name",     .type = UCI_TYPE_STRING,    .ubusGet = sysLoadSysName},
    {.name = "hardware_version",.type = UCI_TYPE_STRING,    .ubusGet = sysLoadHardwareVersion},
    {.name = "system_location", .type = UCI_TYPE_STRING,    .ubusGet = sysLoadSysLocation},
};

int sysCfgLoad(void)
{
    struct uci_context *ctx = uci_alloc_context();

    char name[64]; 
    int ret;
    int i;
    struct uci_ptr ptr;

    for(i = 0;i < sizeof(devUci)/sizeof(UciConfig_S);i++)
    {
        sprintf(name,"switchd.device_general.%s",devUci[i].name);
        ret = load_config(name,devUci[i].type,&ptr,ctx);
        if(ret)
        {
            uci_free_context(ctx);

            if(devUci[i].type == UCI_TYPE_LIST) continue;

            LOG_ERROR("load config %s fail!\n",name); 
            return -1;
        }
        
        ret = devUci[i].ubusGet(&ptr);
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

void switchd_bcast_event(char *event, struct blob_attr *msg)
{
	int ret;

	if (!notify)
		return;

	ret = ubus_notify(_ctx, &system_object, event, msg, -1);
	if (ret)
		fprintf(stderr, "Failed to notify log: %s\n", ubus_strerror(ret));
}

static int sysCfgInit(void)
{
    int ret;

    memset(&sysCfg,0,sizeof(sysCfg));

    sysCfg.save = 2;
    sysCfg.clear = 2;

    ret = sysCfgLoad();
    if(ret)
    {
		LOG_ERROR("system configure load fail!\n");
        return -1;
    }

    return 0;
}

void ubus_init_sysinfo(struct ubus_context *ctx)
{
	int ret;

	_ctx = ctx;

    ret = sysCfgInit();
    if(ret)
    {
		LOG_ERROR("Init system configuration fail!\n");
        return;
    }

	ret = ubus_add_object(ctx, &system_object);
    
	if (ret)
    {
		LOG_ERROR("Failed to add object [sysinfo]: %s\n", ubus_strerror(ret));
    }
}
