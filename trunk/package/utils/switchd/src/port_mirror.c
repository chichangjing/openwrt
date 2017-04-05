/* *< 中威文件编码格式为gb2312,cp936 */
/* * $Id$ $DateTime$
 * * @file port_mirror.c
 * * @brief This is a table of the switch's port port mirror configuration.
 * * @version 0.0.1
 * * @since 0.0.1
 * * @author code<songyaofei@josware.com>
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
    SWITCH_PORT_MIRROR_GET_ARRAY,
    __SWITCH_PORT_MIRROR_GET_MAX,
};

enum {
    SWITCH_PORT_MIRROR_SET_ARRAY,
    __SWITCH_PORT_MIRROR_SET_MAX,
};

static const struct blobmsg_policy port_mirror_get_policy[] = {
    [SWITCH_PORT_MIRROR_GET_ARRAY] = {.name = "get_args", .type = BLOBMSG_TYPE_ARRAY},
};

static const struct blobmsg_policy port_mirror_set_policy[] = {
    [SWITCH_PORT_MIRROR_SET_ARRAY] = {.name = "set_args", .type = BLOBMSG_TYPE_ARRAY},
};

enum {
    MIRROR_DESTINATION_PORT,
    MIRROR_ADMIN_MODE,
    __SWITCH_PORT_MIRROR_TBL_MAX,
};

static struct{
    int dest_port;
    fal_mirror_mode_t mode[CONFIG_JWS_PORT_TOTAL_NR];
}portMirror;

/* *
 * * @brief         get destination port
 * * @retval 0:     OK
 * * @retval -1:    ERROR
 * */
static int mirrorGetDestPort(struct blob_buf *buf)
{
    char dest_port_s[2];
    
    sprintf(dest_port_s,"%d",portMirror.dest_port);
    blobmsg_add_string(buf, "destination_port", dest_port_s);
    LOG_NORMAL("mirror get destination port [%s] OK\n",dest_port_s);


    return 0;
}

/* *
 * * @brief             get source port mode
 * * @param[port_idx]   port index.
 * * @retval 0:         OK
 * * @retval -1:        ERROR
 * */
static int mirrorGetSourcePortMode(struct blob_buf *buf,int port_idx)
{
    char mode_s[2];

    sprintf(mode_s,"%d",portMirror.mode[port_idx-1]);
    blobmsg_add_string(buf,"mode",mode_s);
    LOG_NORMAL("mirror get source port [%d] mode = %s OK\n",port_idx,mode_s);

    return 0;
}

/* *
 * * @brief             set source port mirror mode
 * * @param[port_idx]   port index.
 * * @retval 0:         OK
 * * @retval -1:        ERROR
 * */
static int mirrorSetSourcePortMode(int port_idx,void *p)
{
    sw_error_t res;
    fal_mirror_mode_t mode = atoi((char *)p);

    if(mode < FAL_MIRROR_MODE_TX || mode > FAL_MIRROR_MODE_DISABLE)
    {
        LOG_ERROR("invalid arguement!\n");
        return -1;
    }

    if(port_idx == portMirror.dest_port && mode != FAL_MIRROR_MODE_DISABLE)
    {
        LOG_ERROR("conflicting! port %d is destination port!\n",port_idx);
        return -1;
    }
 //   int port_status = get_port_status(port_idx);
 //   if(mode != FAL_MIRROR_MODE_DISABLE){
 //       if((port_status != PORT_STATUS_NORMAL) && (port_status != PORT_STATUS_MIRROR)){
 //           LOG_ERROR(" port status invalid!\n");
 //           return -2; 
 //       }
 //   }

    res = fal_mirror_source_port_mode_set(0,port_idx,mode);
    if(res)
    {
        LOG_ERROR("port mirror set source port [%d] mode = [%d] fail!\n",port_idx,mode);
        return -1;
    }

    portMirror.mode[port_idx-1] = mode;

 //   if((mode == FAL_MIRROR_MODE_DISABLE)&&(port_status == PORT_STATUS_MIRROR)&&(port_idx != portMirror.dest_port)){
 //       set_port_status(port_idx, PORT_STATUS_NORMAL);
 //   }
 //   if(mode != FAL_MIRROR_MODE_DISABLE){
 //       set_port_status(port_idx, PORT_STATUS_MIRROR); 
 //   }

    char buf[128];
    struct uci_ptr ptr = {
        .package = "switchd",
        .section = "mirror",
        .option = "source_port_mode",
    };

    sprintf(buf,"port1:%d port2:%d port3:%d port4:%d port5:%d port6:%d port7:%d port8:%d port9:%d port10:%d",\
            portMirror.mode[0],portMirror.mode[1],portMirror.mode[2],portMirror.mode[3],portMirror.mode[4],\
            portMirror.mode[5],portMirror.mode[6],portMirror.mode[7],portMirror.mode[8],portMirror.mode[9]);
    ubus_collect_config(&ptr,buf,UCI_TYPE_STRING,DO_SET);

    LOG_NORMAL("port mirror set source port [%d] mode = [%d] OK\n",port_idx,mode);
    return 0;
}

/* *
 * * @brief         set destination port
 * * @retval 0:     OK
 * * @retval -1:    ERROR
 * */
static int mirrorSetDestPort(void *p)
{
    fal_port_t dest_port = atoi((char *)p);
    sw_error_t ret;

    if(dest_port < 0 || dest_port > CONFIG_JWS_PORT_TOTAL_NR)
    {
        LOG_ERROR("invalid arguement!\n");
        return -1;
    }
    if(dest_port){
        int port_status = get_port_status(dest_port);
        if((port_status != PORT_STATUS_NORMAL) && (port_status != PORT_STATUS_MIRROR)){
            LOG_ERROR(" port status invalid!\n");
            return -2; 
        }
    }

    int i = 0;
    for(i = 1; i<= CONFIG_JWS_PORT_TOTAL_NR; i++){
        if(i == dest_port)
            continue;
        if(get_port_status(i) == PORT_STATUS_MIRROR)
            set_port_status(i, PORT_STATUS_NORMAL);
    }

    if(dest_port == portMirror.dest_port)
    {
        LOG_NORMAL("same arguement,do nothing\n");
        return 0;
    }
    
    ret = fal_mirror_dest_port_set(0,dest_port);
    if(ret)
    {
        LOG_ERROR("set destination port %d error = %d\n", dest_port, ret);
        return -1;
    }

    portMirror.dest_port = dest_port;
    if(dest_port)
        set_port_status(dest_port, PORT_STATUS_MIRROR);

    char buf[2];
    struct uci_ptr ptr = {
        .package = "switchd",
        .section = "mirror",
        .option = "dest_port",
    };

    sprintf(buf,"%d",dest_port);
    ubus_collect_config(&ptr,buf,UCI_TYPE_STRING,DO_SET);

    /**<    disable corresponding source port mode if one dest port be chose  */
    /**<    disable all source port mode if no dest port be chose  */
    if(!dest_port)
    {
        int port;
        for(port = 1;port < CONFIG_JWS_PORT_TOTAL_NR;port++)
        {
            if(portMirror.mode[port-1] != FAL_MIRROR_MODE_DISABLE)
            {
                ret = mirrorSetSourcePortMode(port,"4");
                if(ret)
                {
                    LOG_ERROR("mirror set source port %d mode disable fail!\n", port);
                    return -1;
                }
            }
        }
    }
    else if(dest_port && portMirror.mode[dest_port-1] != FAL_MIRROR_MODE_DISABLE)
    {
        ret = mirrorSetSourcePortMode(dest_port,"4");
        if(ret)
        {
            LOG_ERROR("mirror set source port %d mode fail!\n", dest_port);
            return -1;
        }
    }

    LOG_NORMAL("port mirror set destination port = [%d] OK\n", dest_port);

    return 0;
}


static const struct jw_switch_policy port_mirror_tbl[] = {
    [MIRROR_DESTINATION_PORT] = {.name = "destination_port", .get_ext_handler = mirrorGetDestPort, .set_ext_handler = mirrorSetDestPort}, 
    [MIRROR_ADMIN_MODE] = {.name = "mode", .get_handler = mirrorGetSourcePortMode, .set_handler = mirrorSetSourcePortMode},
};

/* *
 * * @brief         parse source port config get
 * * @param[lvl3]   level 3 message.
 * * @retval 0:     OK
 * * @retval -1:    ERROR
 * */
static int mirrorParseSoursePortCfgGet(struct blob_attr *lvl3, struct blob_buf *buf)
{
    const char *name    = blobmsg_name(lvl3);
    int idx             = atoi(name + strlen(PORT_IDX));

    if (idx < 1 || idx > CONFIG_JWS_PORT_TOTAL_NR) 
    {
        LOG_ERROR("invalid index %d\n", idx);
        return -1;
    }
 
    void *ret_array;

    ret_array= blobmsg_open_array(buf, name);
    foreach_json_array_get(idx,lvl3,buf,port_mirror_tbl, __SWITCH_PORT_MIRROR_TBL_MAX);
    blobmsg_close_array(buf, ret_array);	

    return 0;
}


/* *
 * * @brief         parse source port config set
 * * @param[lvl3]   level 3 message.
 * * @retval 0:     OK
 * * @retval -1:    ERROR
 * */
static int mirrorParseSourcePortCfgSet(struct blob_attr *lvl3, struct blob_buf *buf)
{
    const char *name    = blobmsg_name(lvl3);
    int idx             = atoi(name + strlen(PORT_IDX));

    if (idx < 1 || idx > CONFIG_JWS_PORT_TOTAL_NR) 
    {
        LOG_ERROR(" port index invalid!\n");
        return -1;
    }

    /**<    foreach array of level 3 message  */
    int ret;
    ret = foreach_json_array_set(idx,lvl3,port_mirror_tbl, __SWITCH_PORT_MIRROR_TBL_MAX);
    if(ret)
    {
        LOG_ERROR("foreach index %d error!\n",idx);
        return -1;
    }

    return 0;
}

/* *
 * * @brief         method "port_mirror_get" handler
 * * @retval 0:     OK
 * * @retval -1:    ERROR
 * */
static int mirrorGetHandler(struct ubus_context *ctx, struct ubus_object *obj,
                struct ubus_request_data *req, const char *method,
                struct blob_attr *msg)
{
    struct blob_attr *tb[__SWITCH_PORT_MIRROR_GET_MAX];    
	struct blob_attr *lvl1 = NULL;  

    blobmsg_parse(port_mirror_get_policy, __SWITCH_PORT_MIRROR_GET_MAX, tb, blob_data(msg), blob_len(msg));
	lvl1 = tb[SWITCH_PORT_MIRROR_GET_ARRAY];     
    if (!lvl1) 
	{
        LOG_ERROR("invalid params in %s\n");
        return -1;
    }

	if (blobmsg_type(lvl1) != BLOBMSG_TYPE_ARRAY) 
	{
        LOG_ERROR("blobmsg type is not array\n");
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
        switch(blobmsg_type(lvl2))
        {
            case BLOBMSG_TYPE_STRING:
            {
                struct jw_switch_policy *policy_p;
                char *name = blobmsg_get_string(lvl2);
                
                policy_p = (struct jw_switch_policy *)jw_switchd_get_context(name, port_mirror_tbl, __SWITCH_PORT_MIRROR_TBL_MAX);
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
                
                if (blobmsg_type(lvl3) ==  BLOBMSG_TYPE_ARRAY) 
                {
                    ret_table = blobmsg_open_table(&b, NULL);
                    mirrorParseSoursePortCfgGet(lvl3, &b);
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
 * * @brief         method "port_mirror_set" handler
 * * @retval 0:     OK
 * * @retval -1:    ERROR
 * */
static int mirrorSetHandler(struct ubus_context *ctx, struct ubus_object *obj,
                struct ubus_request_data *req, const char *method,
                struct blob_attr *msg)
{
    struct blob_attr *tb[__SWITCH_PORT_MIRROR_SET_MAX];    
	struct blob_attr *lvl1 = NULL;  
    int ret = -1;

    blobmsg_parse(port_mirror_set_policy, __SWITCH_PORT_MIRROR_SET_MAX, tb, blob_data(msg), blob_len(msg));
	lvl1 = tb[SWITCH_PORT_MIRROR_SET_ARRAY];     
    if (!lvl1) 
	{
        LOG_ERROR("invalid params in %s\n");
        goto end;
    }

	if (blobmsg_type(lvl1) != BLOBMSG_TYPE_ARRAY) 
	{
        LOG_ERROR("blobmsg type is not array\n");
        goto end;
    }

    struct blob_attr *lvl2 = NULL; 
    int rem = 0;
    struct jw_switch_policy *policy_p = NULL;
		
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
                    const char *name = blobmsg_name(lvl3);

                    policy_p = (struct jw_switch_policy *)jw_switchd_get_context(name, port_mirror_tbl, __SWITCH_PORT_MIRROR_TBL_MAX);
                    if (policy_p && policy_p->set_ext_handler) 
                    {
                        char *val = blobmsg_get_string(lvl3);

                        ret = policy_p->set_ext_handler(val);
                        
                    } 
                    else 
                    {
                        LOG_ERROR("undefined keys\n");
                    }
                    break;
                }
                case BLOBMSG_TYPE_ARRAY:
                    ret = mirrorParseSourcePortCfgSet(lvl3, &b);
                    break;
                default:
                    LOG_ERROR("%s[%d]: invalid level 3 message type\n");
                    break;
            }   
        } 
        else 
        {
            LOG_ERROR("level 2 message type not define\n");
        }

        if(ret)
            goto end;
    }

end:
    blobmsg_add_string(&b, "ret", ret?"-1":"0");
	ubus_send_reply(ctx, req, b.head);

	return UBUS_STATUS_OK;
}


static const struct ubus_method port_mirror_methods[] = {
	UBUS_METHOD("port_mirror_get", mirrorGetHandler, port_mirror_get_policy),
	UBUS_METHOD("port_mirror_set", mirrorSetHandler, port_mirror_set_policy),
};


static struct ubus_object_type port_mirror_object_type =
	UBUS_OBJECT_TYPE("port_mirror", port_mirror_methods);


static struct ubus_object port_mirror_object = {
	.name = "port_mirror",
	.type = &port_mirror_object_type,
	.methods = port_mirror_methods,
	.n_methods = ARRAY_SIZE(port_mirror_methods),
};

/* *
 * * @brief             load mirror arguement - dest_port from UCI file
 * * @param[*p]         buffer to be parse 
 * * @retval 0:         OK
 * * @retval -1:        ERROR
 * */
static int mirrorLoadDestPort(struct uci_ptr *ptr)
{
    int dest_port = atoi(ptr->o->v.string);
    int ret;

    if(dest_port < 0 || dest_port > CONFIG_JWS_PORT_TOTAL_NR)
    {
        LOG_ERROR("destination port arguement is invalid!\n"); 
        return -1;
    }

    if(dest_port)
    {
        if((get_port_status(dest_port) != PORT_STATUS_NORMAL) &&(get_port_status(dest_port) != PORT_STATUS_MIRROR)){//check port status
            LOG_ERROR("set destination port %d  status = %d\n", dest_port, get_port_status(dest_port));
            return -1; 
        }
        ret = fal_mirror_dest_port_set(0,dest_port);
        if(ret)
        {
            LOG_ERROR("set destination port %d error = %d\n", dest_port, ret);
            return -1;
        }
        set_port_status(dest_port, PORT_STATUS_MIRROR);//set port status
    }

    portMirror.dest_port = dest_port;
    LOG_NORMAL("mirror load destination port = [%d] OK\n", dest_port);
    
    return 0;
}


/* *
 * * @brief             load mirror arguement - source_port_mode from UCI file
 * * @param[*p]         buffer to be parse 
 * * @retval 0:         OK
 * * @retval -1:        ERROR
 * */
static int mirrorLoadSourcePortMode(struct uci_ptr *ptr)
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
            LOG_ERROR("source port mode format in UCI file error!\n"); 
            return -1;
        }

        port = atoi(pstr + 4);
        if(port < 1 || port > CONFIG_JWS_PORT_TOTAL_NR)
        {
            LOG_ERROR("source port mode arguement - port index in UCI file error!\n"); 
            return -1;
        }

        char *pdelim = strchr(pstr,':');
        if(pdelim == NULL)
        {
            LOG_ERROR("source port mode format in UCI file error!\n"); 
            return -1;
        }

        mode = atoi(pdelim + 1);
        if(mode < FAL_MIRROR_MODE_TX || mode > FAL_MIRROR_MODE_DISABLE)
        {
            LOG_ERROR("source port mode arguement is invalid!\n");
            return -1;
        }

        if(port == portMirror.dest_port && mode != FAL_MIRROR_MODE_DISABLE)
        {
            LOG_ERROR("conflicting! port %d is destination port!\n",port);
            return -1;
        }
 //       if(mode != FAL_MIRROR_MODE_DISABLE){//mirror status
 //           if((get_port_status(port) != PORT_STATUS_NORMAL) &&(get_port_status(port) != PORT_STATUS_MIRROR)){//check port status
 //               LOG_ERROR("port mirror set source port %d  status = %d\n", port, get_port_status(port));
 //               return -1; 
 //           }
 //       }

        ret = fal_mirror_source_port_mode_set(0,port,mode);
        if(ret)
        {
            LOG_ERROR("port mirror set source port [%d] mode = [%d] fail!\n",port,mode);
            return -1;
        }
//        if(mode != FAL_MIRROR_MODE_DISABLE){//set port status
//            set_port_status(port, PORT_STATUS_MIRROR);
//        }

        portMirror.mode[port-1] = mode;
        
    }
    free(string);

    LOG_NORMAL("mirror load source port mode arguement OK\n");
    
    return 0;
}

static UciConfig_S mirrUci[] = {
    {.name = "dest_port",       .type = UCI_TYPE_STRING,    .ubusGet = mirrorLoadDestPort},
    {.name = "source_port_mode",.type = UCI_TYPE_STRING,    .ubusGet = mirrorLoadSourcePortMode},
};

/* *
 * * @brief             load mirror configure
 * * @retval 0:         OK
 * * @retval -1:        ERROR
 * */
static int mirrorCfgLoad(void)
{
    struct uci_context *ctx;

    ctx = uci_alloc_context();

    char name[64]; 
    int ret;
    int i;
    struct uci_ptr ptr;

    for(i = 0;i < sizeof(mirrUci)/sizeof(UciConfig_S);i++)
    {
        sprintf(name,"switchd.mirror.%s",mirrUci[i].name);
        ret = load_config(name,mirrUci[i].type,&ptr,ctx);
        if(ret)
        {
            uci_free_context(ctx);

            if(mirrUci[i].type == UCI_TYPE_LIST) continue;

            LOG_ERROR("load config %s fail!\n",name); 
            return -1;
        }
        
        ret = mirrUci[i].ubusGet(&ptr);
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


void ubus_init_port_mirror(struct ubus_context *ctx)
{
	int ret;

	_ctx = ctx;
    ret = mirrorCfgLoad();
    if(ret)
    {
		LOG_ERROR("mirror load configure from UCI file fail\n");
        return;
    }

	ret = ubus_add_object(ctx, &port_mirror_object);
	if (ret)
		LOG_ERROR("Failed to add object [port_mirror]: %s\n", ubus_strerror(ret));
}
