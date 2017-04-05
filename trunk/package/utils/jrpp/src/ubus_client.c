#include <libubox/blobmsg_json.h>
#include <libubus.h>

#include "utils.h"
#include "ubus_client.h"
#include "jrppd.h"
#include "jrpp_if.h"

static struct ubus_context *ctx;
static struct blob_buf b;
static struct ubus_subscriber link_event;

static const struct blobmsg_policy ret_policy = {
    .name = "ret",
    .type = BLOBMSG_TYPE_STRING
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

static void ubus_callback_ret(struct ubus_request *req,int type, struct blob_attr *msg)
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
#if 0
    else
        log_info("ret = %s",ret);
#endif
}

static void ubus_callback_notify_remove(struct ubus_context *ctx, struct ubus_subscriber *s,uint32_t id)
{
    LOG_ERROR("object 0x%08x went away",id);
}

static int ubus_callback_notify_info(struct ubus_context *ctx, struct ubus_object *obj,
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
    log_info("Received notification '%s': port %d link %s\n", method, lport,(link == 1)?"up":"down");

    br_notify(lport,link);

    return 0;
}

int ubus_client_init(void)
{
    ctx = ubus_connect("/var/run/ubus.sock");
    if(ctx == NULL)
    {
        LOG_ERROR("ubus connect to server fail");
        return -1;
    }

    ubus_add_uloop(ctx);

#if 1
    if(ubus_register_subscriber(ctx, &link_event) < 0)
    {
        LOG_ERROR("ubus regist subscriber for link event error");
        return -1;
    }

    link_event.remove_cb = ubus_callback_notify_remove;
    link_event.cb        = ubus_callback_notify_info;

    uint32_t id;
    if(ubus_lookup_id(ctx, "link_event", &id))
    {
        LOG_ERROR("ubus object \"link_event\" cannot find");
        return -1;
    }
    if(ubus_subscribe(ctx, &link_event, id))
    {
        LOG_ERROR("ubus subscribe link event error");
        return -1;
    }
#endif

    return 0;
}

int ubus_set_ifstp(int lport,int state)
{
    uint32_t id;

    if (ubus_lookup_id(ctx, "port_config", &id))
    {
        LOG_ERROR("ubus object \"port_config\" cannot find");
        return -1;
    }

    void *ret_array1,*ret_array2;
    void *ret_table1,*ret_table2;
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
    ubus_invoke(ctx, id, "port_config_set", b.head, ubus_callback_ret, 0, 3000);

    return 0;
}

