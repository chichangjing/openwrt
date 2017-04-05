#include <libubox/blobmsg_json.h>
#include <libubus.h>

#include "ubus.h"
#include "conf.h"
#include "iface.h"
#include "radius/radius_client.h"

#define MAX_SHARE_SECRET_LEN 64

static struct ubus_context *g_8021_ctx;
static struct ubus_context *g_other_ctx;
#ifdef UBUS_SUBSCRIBER
static struct ubus_subscriber link_event;
#endif

static const struct blobmsg_policy ret_policy = {
    .name = "ret",
    .type = BLOBMSG_TYPE_STRING
};

static const struct blobmsg_policy ieee8021x_policy = {
    .name = "ret",
    .type = BLOBMSG_TYPE_ARRAY
};

enum {
    EVENT_PORT,
    EVENT_STATUS,
    _EVENT_MAX
};

static const struct blobmsg_policy link_policy[_EVENT_MAX] = {
    [EVENT_PORT]    = { .name = "port", .type = BLOBMSG_TYPE_INT32 },
    [EVENT_STATUS]  = { .name = "link", .type = BLOBMSG_TYPE_INT32 },
};

static int ubus_operation(struct bss_data *bss,int cmd,void *data);

static void ubus_cb_ret(struct ubus_request *req,int type, struct blob_attr *msg)
{
    struct blob_attr *tb[1];
    blobmsg_parse(&ret_policy, 1, tb, blob_data(msg), blob_len(msg));
    if (tb[0] == NULL)
    {
        LOG_ERROR("invalid arguement");
        return;
    }
    char *ret = blobmsg_get_string(tb[0]);
    if(ret == NULL)
        LOG_ERROR("invalid arguement");
    else
        LOG_D("ubus set operation ret = %s",ret);
}

static void ubus_cb_8021x(struct ubus_request *req,int type, struct blob_attr *msg)
{
    if(req->priv == NULL)
    {
        LOG_ERROR("priv = NULL");
        return;
    }

    struct blob_attr *tb[1];
    struct blob_attr *lvl1 = NULL;
    blobmsg_parse(&ieee8021x_policy, 1, tb, blob_data(msg), blob_len(msg));
    lvl1 = tb[0];
    if (lvl1 == NULL)
    {
        LOG_ERROR("invalid arguement");
        return;
    }

    if (blobmsg_type(lvl1) != BLOBMSG_TYPE_ARRAY)
    {
        LOG_ERROR("level 1 message type is not array");
        return;
    }

    struct blob_attr *lvl2 = NULL;
    int rem1;
    blobmsg_for_each_attr(lvl2, lvl1, rem1)
    {
        if (blobmsg_type(lvl2) == BLOBMSG_TYPE_TABLE)
        {
            struct ieee802_1x_conf *conf = req->priv; 
            struct blob_attr *lvl3 = NULL;
            int rem2;
            blobmsg_for_each_attr(lvl3, lvl2, rem2)
            {
                switch(blobmsg_type(lvl3))
                {
                    case BLOBMSG_TYPE_STRING:
                    {
                        const char *name  = blobmsg_name(lvl3);
                        char *value = blobmsg_get_string(lvl3);

                        LOG_D("802.1x response : name = %s,value = %s",name,value);
                        if(!strcmp(name,"globle_state"))
                            *(int *)req->priv = atoi(value);
                        else if(!strcmp(name,"eap_mode"))
                            conf->eap_mode = atoi(value);
                        else if(!strcmp(name,"eap_model"))
                            conf->eap_model = atoi(value);
                        else if(!strcmp(name,"iface_num"))
                            conf->iface_num = atoi(value);
                        break;
                    }
                    case BLOBMSG_TYPE_ARRAY:
                    {
                        const char *name  = blobmsg_name(lvl3);

                        if(!strcmp(name,"primary_radius_server") || !strcmp(name,"secondary_radius_server"))
                        {
                            if(conf->radius_conf == NULL && (conf->radius_conf = calloc(1,sizeof(struct radius_servers))) == NULL)
                            {
                                LOG_ERROR("create radius servers configuration error");
                                return; 
                            }                            
                            struct radius_servers *radius = conf->radius_conf;

                            struct blob_attr *lvl4 = NULL;
                            int rem3;
                            blobmsg_for_each_attr(lvl4, lvl3, rem3)
                            {
                                if(blobmsg_type(lvl4) == BLOBMSG_TYPE_TABLE) 
                                {
                                    struct blob_attr *lvl5 = NULL;
                                    int rem4;
                                    blobmsg_for_each_attr(lvl5, lvl4, rem4)
                                    {
                                        if(blobmsg_type(lvl5) == BLOBMSG_TYPE_STRING) 
                                        {
                                            char *value = blobmsg_get_string(lvl5);
                                            name = blobmsg_name(lvl5);
                                            LOG_D("802.1x response : name = %s,value = %s",name,value);
                                            if(!strcmp(name,"auth_server_addr"))
                                            {
                                                if(conf_radius_server(&radius->auth_servers,&radius->num_auth_servers,value,1812,&radius->auth_server))
                                                {
                                                    LOG_ERROR("create a new configuration for radius auth-server error");
                                                    return; 
                                                }
                                            }
                                            else if(radius->auth_server && !strcmp(name,"auth_server_port"))
                                            {
                                                radius->auth_server->port = atoi(value);
                                            }
                                            else if(radius->auth_server && !strcmp(name,"auth_server_shared_secret"))
                                            {
                                                int len = strlen(value);
                                                if(len == 0 || len > MAX_SHARE_SECRET_LEN)
                                                {
                                                    LOG_ERROR("invalid auth-server-shared-secret!");
                                                    return; 
                                                }

                                                radius->auth_server->shared_secret = (uint8_t *)strdup(value);
                                                radius->auth_server->shared_secret_len = len;
                                            }
                                            else if(!strcmp(name,"acct_server_addr"))
                                            {
                                                if(conf_radius_server(&radius->acct_servers,&radius->num_acct_servers,value,1813,&radius->acct_server))
                                                {
                                                    LOG_ERROR("create a new configuration for radius acct-server error");
                                                    return; 
                                                }
                                            }
                                            else if(radius->acct_server && !strcmp(name,"acct_server_port"))
                                            {
                                                radius->acct_server->port = atoi(value);
                                            }
                                            else if(radius->acct_server && !strcmp(name,"acct_server_shared_secret"))
                                            {
                                                int len = strlen(value);
                                                if(len == 0 || len > MAX_SHARE_SECRET_LEN)
                                                {
                                                    LOG_ERROR("invalid acct-server-shared-secret!");
                                                    return; 
                                                }

                                                radius->acct_server->shared_secret = (uint8_t *)strdup(value);
                                                radius->acct_server->shared_secret_len = len;
                                            }
                                            else
                                            {
                                                LOG_ERROR("invalid radius arguement name = %s",name);
                                                return;
                                            }
                                        }
                                        else
                                        {
                                            LOG_ERROR("level 5 message type is not available!");
                                            return; 
                                        }    
                                    }
                                }
                                else
                                {
                                    LOG_ERROR("level 4 message type is not available!");
                                    return; 
                                }
                            }
                        }
                        else
                        {
                            char *p = strstr(name,"iface-");
                            if(p == NULL)
                            {
                                LOG_ERROR("802.1x response : undefine name = %s",name);
                                return;
                            }

                            int idx = atoi(p + strlen("iface-"));
                            if(idx < 1 || idx > CONFIG_JWS_PORT_TOTAL_NR)
                            {
                                LOG_ERROR("invalid iface index = %d!",idx);
                                return;
                            }
                             
                            struct ubus_iface_param param;
                            memset(&param,0,sizeof(struct ubus_iface_param));
                            param.ifindex = idx;

                            struct blob_attr *lvl4 = NULL;
                            int rem3;
                            blobmsg_for_each_attr(lvl4, lvl3, rem3)
                            {
                                if(blobmsg_type(lvl4) == BLOBMSG_TYPE_TABLE) 
                                {
                                    struct blob_attr *lvl5 = NULL;
                                    int rem4;
                                    blobmsg_for_each_attr(lvl5, lvl4, rem4)
                                    {
                                        if(blobmsg_type(lvl5) == BLOBMSG_TYPE_STRING) 
                                        {
                                            char *value = blobmsg_get_string(lvl5);
                                            name = blobmsg_name(lvl5);
                                            LOG_D("802.1x response : name = %s,value = %s",name,value);

                                            if(!strcmp(name,"ifname"))
                                                param.ifname = value;
                                            else if(!strcmp(name,"reauth"))
                                                param.reauth = atoi(value);
                                            else if(!strcmp(name,"access_mode"))
                                                param.access_mode = atoi(value);
                                            else
                                            {
                                                LOG_ERROR("invalid interface arguement name = %s",name);
                                                return;
                                            }
                                        }
                                        else
                                        {
                                            LOG_ERROR("level 5 message type is not available!");
                                            return; 
                                        }
                                    }
                                }
                                else
                                {
                                    LOG_ERROR("level 4 message type is not available!");
                                    return; 
                                }
                            }

                            param.ubus_ops = ubus_operation;
                            if(iface_init(conf,&param))
                            {
                                LOG_ERROR("init iface-%d error!",idx);
                                return;
                            }
                            LOG_NORMAL("interface<%s> init OK",param.ifname);
                        }

                        break;
                    }
                    default:
                        LOG_ERROR("level 3 message type is not available!");
                        break;
                }
            }
        }
        else
        {
            LOG_ERROR("level 2 message type is not available!");
            return;
        }
    }
}

#ifdef UBUS_SUBSCRIBER
static void ubus_cb_notify_remove(struct ubus_context *ctx, struct ubus_subscriber *s,uint32_t id)
{
    LOG_ERROR("object 0x%08x went away",id);
}

static int ubus_cb_notify_info(struct ubus_context *ctx, struct ubus_object *obj,
                                     struct ubus_request_data *req, const char *method,struct blob_attr *msg)
{
    struct blob_attr *tb[_EVENT_MAX];
    blobmsg_parse(link_policy, _EVENT_MAX, tb, blob_data(msg), blob_len(msg));
    if (tb[EVENT_PORT] == NULL || tb[EVENT_STATUS] == NULL)
    {
        LOG_ERROR("invalid arguement");
        return -1;
    }
    

    int lport   = blobmsg_get_u32(tb[EVENT_PORT]);
    int link    = blobmsg_get_u32(tb[EVENT_STATUS]);

#if 0
    char *str;

    str = blobmsg_format_json(msg, true);
    log_info("Received notification '%s': %s\n", method, str);
#endif
    LOG_D("Received notification '%s': port = %d,link = %d", method, lport,link);

    return 0;
}
#endif

int ubus_init(void)
{
    g_8021_ctx = ubus_connect("/var/run/ubus.sock");
    if(g_8021_ctx == NULL)
    {
        LOG_ERROR("ubus connect to server fail");
        return -1;
    }
    ubus_add_uloop(g_8021_ctx);

    g_other_ctx = ubus_connect("/var/run/ubus.sock");
    if(g_other_ctx == NULL)
    {
        LOG_ERROR("ubus connect to server fail");
        return -1;
    }
    ubus_add_uloop(g_other_ctx);
#ifdef UBUS_SUBSCRIBER
    if(ubus_register_subscriber(g_other_ctx, &link_event) < 0)
    {
        LOG_ERROR("ubus regist subscriber for link event error");
        return -1;
    }

    link_event.remove_cb = ubus_cb_notify_remove;
    link_event.cb        = ubus_cb_notify_info;

    uint32_t id;
    if(ubus_lookup_id(g_other_ctx, "link_event", &id))
    {
        LOG_ERROR("ubus object \"link_event\" cannot find");
        return -1;
    }
    if(ubus_subscribe(g_other_ctx, &link_event, id))
    {
        LOG_ERROR("ubus subscribe link event error");
        return -1;
    }
#endif

    return 0;
}

static int ubus_set_ifstp(int lport,int state)
{
    uint32_t id;

    if (ubus_lookup_id(g_other_ctx, "port_config", &id))
    {
        LOG_ERROR("ubus object \"port_config\" cannot find");
        return -1;
    }

    void *ret_array1,*ret_array2;
    void *ret_table1,*ret_table2;
    struct blob_buf b;
    memset(&b,0,sizeof(struct blob_buf));
    blob_buf_init(&b, 0); 
    ret_array1 = blobmsg_open_array(&b, "set_args");
    ret_table1 = blobmsg_open_table(&b, NULL);
    char buf[8];
    sprintf(buf,"port-%d",lport);
    ret_array2 = blobmsg_open_array(&b, buf);
    ret_table2 = blobmsg_open_table(&b, NULL);

    sprintf(buf,"%d",state);
    blobmsg_add_string(&b, "dot1d_state", buf);

    blobmsg_close_table(&b, ret_table2);
    blobmsg_close_array(&b, ret_array2);
    blobmsg_close_table(&b, ret_table1);
    blobmsg_close_array(&b, ret_array1);
    ubus_invoke(g_other_ctx, id, "port_config_set", b.head, ubus_cb_ret, 0, 3000);
    LOG_NORMAL("ubus set port %d %s OK",lport,(state == UBUS_PORT_FORWARDING)?"forwarding":"blocking");

    return 0;
}

static int ubus_set_8021x_mac(int ifindex,uint8_t *mac,int op)
{
    uint32_t id;

    if (ubus_lookup_id(g_8021_ctx, "802.1x", &id))
    {
        LOG_ERROR("ubus object \"802.1x\" cannot find");
        return -1;
    }

    void *ret_array1,*ret_array2;
    void *ret_table1,*ret_table2;
    struct blob_buf b;
    memset(&b,0,sizeof(struct blob_buf));
    blob_buf_init(&b, 0); 
    ret_array1 = blobmsg_open_array(&b, "set_args");
    ret_table1 = blobmsg_open_table(&b, NULL);
    char buf[32];
    sprintf(buf,"iface-%d",ifindex);
    ret_array2 = blobmsg_open_array(&b, buf);
    ret_table2 = blobmsg_open_table(&b, NULL);
    
    sprintf(buf,"%02x-%02x-%02x-%02x-%02x-%02x",MAC2STR(mac));
    blobmsg_add_string(&b, op?"bundle_mac":"unbundle_mac", buf);

    blobmsg_close_table(&b, ret_table2);
    blobmsg_close_array(&b, ret_array2);
    blobmsg_close_table(&b, ret_table1);
    blobmsg_close_array(&b, ret_array1);
    ubus_invoke(g_8021_ctx, id, "802.1x_set", b.head, ubus_cb_ret, 0, 3000);
    LOG_NORMAL("ubus bundle mac[" MACSTR "] to iface-%d OK",MAC2STR(mac),ifindex);
    return 0;
}

int ubus_get_8021x_status(void *state)
{
    uint32_t id;

    if (ubus_lookup_id(g_8021_ctx, "802.1x", &id))
    {
        LOG_ERROR("ubus object \"802.1x\" cannot find");
        return -1;
    }

    void *ret_array;
    struct blob_buf b;
    memset(&b,0,sizeof(struct blob_buf));
    blob_buf_init(&b, 0); 
    ret_array = blobmsg_open_array(&b, "get_args");

    blobmsg_add_string(&b, NULL, "globle_state");

    blobmsg_close_array(&b, ret_array);
    return ubus_invoke(g_8021_ctx, id, "802.1x_get", b.head, ubus_cb_8021x, state, 1000);
}

int ubus_get_8021x_conf(struct ieee802_1x_conf *conf)
{
    uint32_t id;

    if (ubus_lookup_id(g_8021_ctx, "802.1x", &id))
    {
        LOG_ERROR("ubus object \"802.1x\" cannot find");
        return -1;
    }

    void *ret_array1;
    struct blob_buf b;
    memset(&b,0,sizeof(struct blob_buf));
    blob_buf_init(&b, 0); 
    ret_array1 = blobmsg_open_array(&b, "get_args");

    blobmsg_add_string(&b, NULL, "eap_mode");
    blobmsg_add_string(&b, NULL, "eap_model");
    blobmsg_add_string(&b, NULL, "primary_radius_server");
//    blobmsg_add_string(&b, NULL, "secondary_radius_server");
    blobmsg_add_string(&b, NULL, "iface_num");

    blobmsg_close_array(&b, ret_array1);
    ubus_invoke(g_8021_ctx, id, "802.1x_get", b.head, ubus_cb_8021x, conf, 1000);
    
    LOG_NORMAL("iface_num = %d",conf->iface_num);
    if(conf->iface_num == 0)
        return 0;

    int idx;
    char buf[16];
    void *ret_table;
    void *ret_array2;
    blob_buf_init(&b, 0); 
    ret_array1 = blobmsg_open_array(&b, "get_args");
    ret_table = blobmsg_open_table(&b,NULL);
    ret_array2 = blobmsg_open_array(&b, "iface_show");

    for(idx = 1;idx <= conf->iface_num;idx++)
    {
        sprintf(buf,"iface-%d",idx);
        blobmsg_add_string(&b, NULL, buf);
    }
    
    blobmsg_close_array(&b, ret_array2);
    blobmsg_close_table(&b, ret_table);
    blobmsg_close_array(&b, ret_array1);

    return ubus_invoke(g_8021_ctx, id, "802.1x_get", b.head, ubus_cb_8021x, conf, 1000);
}

static int ubus_operation(struct bss_data *bss,int cmd,void *data)
{
    switch(cmd)
    {
        case UBUS_CMD_PORT_STATE:
            return ubus_set_ifstp(atoi(BSS_GET_CONF(bss)->ifname + 4),*(int *)data);
        case UBUS_CMD_IFACE_BUNDLE_MAC:
            return ubus_set_8021x_mac(BSS_GET_CONF(bss)->ifindex,(uint8_t *)data,1);
        case UBUS_CMD_IFACE_UNBUNDLE_MAC:
            return ubus_set_8021x_mac(BSS_GET_CONF(bss)->ifindex,(uint8_t *)data,0);
        default:
            LOG_ERROR("undefine cmd = %d for ubus",cmd);
            break;
    }

    return 0;
}

