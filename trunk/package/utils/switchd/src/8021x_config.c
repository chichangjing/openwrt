/* *< 中威文件编码格式为gb2312,cp936 */
/* * $Id$ $DateTime$
 * * @file ieee8021x_config.c
 * * @brief This is a table of the switch's ieee8021x configure.
 * * @version 0.0.1
 * * @since 0.0.1
 * * @author code<songyaofei@josware.com>
 * * @date 2017-03-22
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
#include <linux/if_ether.h>
#include "switchd.h"

static struct blob_buf b;
static struct ubus_context *_ctx;

struct radius_server {
    char *auth_server_addr;
    int  auth_server_port;
    char *auth_server_share_srcret;
    char *acct_server_addr;
    int  acct_server_port;
    char *acct_server_share_srcret;
};

enum {
    EAP_RELAY,
    EAP_TERMINAL
}eEepMode;

enum {
    EAP_PASSTHROUGH,
    EAP_LOCAL
}eEeapModel;

enum {
    BASE_ON_PORT,
    BASE_ON_MAC
}eAccessMode;

struct mac_list {
    struct list_head inode;
    struct list_head hnode;
    unsigned char mac[ETH_ALEN];
    int lport;
};

struct iface {
    int idx;
    char *ifname;
    int reauth;
    int access_mode;
    struct list_head node;
    struct list_head bind_mac;
    int bind_num;
};

#define HASH_MAC_LEN 256
#define HASH_MAC(mac) (mac[5])
static struct {
    int state;
    int eap_mode;
    int eap_model;
    int iface_num;
    struct list_head iface;
    struct list_head *hash_mac;
    struct radius_server *primary;
    struct radius_server *secondary;
}ieee8021x;

enum {
    IEEE8021X_GET_KEY,
    __IEEE8021X_GET_MAX,
};

enum {
    IEEE8021X_SET_KEY,
    __IEEE8021X_SET_MAX
};

static const struct blobmsg_policy ieee8021x_get_policy[] = {
    [IEEE8021X_GET_KEY] = {.name = "get_args", .type = BLOBMSG_TYPE_ARRAY},
};

static const struct blobmsg_policy ieee8021x_set_policy[] = {
    [IEEE8021X_SET_KEY] = {.name = "set_args", .type = BLOBMSG_TYPE_ARRAY},
};

enum {
    IEEE8021X_STATE,
    IEEE8021X_EAP_MODE,
    IEEE8021X_EAP_MODEL,
    IEEE8021X_IFACE_NUM,
    IEEE8021X_IFACE_SHOW,
    IEEE8021X_IFACE_ADD,
    IEEE8021X_IFACE_DEL,
    IEEE8021X_IFACE_NAME,
    IEEE8021X_IFACE_BUNDLE,
    IEEE8021X_IFACE_UNBUNDLE,
    __IEEE8021X_TBL_MAX
};

static int ieee8021xUnbundleMac(fal_fdb_mac_t *jssdk_mac,struct iface *ife)
{
    int cur_lport = atoi(ife->ifname + 4);

    if(ieee8021x.hash_mac == NULL)
    {
        LOG_ERROR("no anyone mac has been bundled!\n");
        return -1;
    }
    struct list_head *hhead = ieee8021x.hash_mac + HASH_MAC(jssdk_mac->mac);
    if(list_empty(hhead))
    {
        LOG_ERROR("mac[%02x-%02x-%02x-%02x-%02x-%02x] has not been bundled!\n",
                  jssdk_mac->mac[0],jssdk_mac->mac[1],jssdk_mac->mac[2],jssdk_mac->mac[3],jssdk_mac->mac[4],jssdk_mac->mac[5]);
        return -1;
    }

    struct mac_list *bd,*bd_bak;
    list_for_each_entry_safe(bd,bd_bak,hhead,hnode)
    {
        if(!memcmp(bd->mac,jssdk_mac->mac,ETH_ALEN))   
        {
            if(bd->lport != cur_lport)
            {
                LOG_ERROR("mac[%02x-%02x-%02x-%02x-%02x-%02x] has been bundled,but not on port %d\n",
                          jssdk_mac->mac[0],jssdk_mac->mac[1],jssdk_mac->mac[2],jssdk_mac->mac[3],jssdk_mac->mac[4],jssdk_mac->mac[5],cur_lport);
                return -1;
            }

            if(fal_mac_row_destroy(0,jssdk_mac,bd->lport))
            {
                LOG_ERROR("jssdk destory mac[%02x-%02x-%02x-%02x-%02x-%02x] on port %d error\n",\
                          jssdk_mac->mac[0],jssdk_mac->mac[1],jssdk_mac->mac[2],jssdk_mac->mac[3],jssdk_mac->mac[4],jssdk_mac->mac[5],bd->lport);
                return -1;
            }
            list_del(&bd->inode);
            list_del(&bd->hnode);
            free(bd);

            return 0;
        }
    }

    LOG_ERROR("mac[%02x-%02x-%02x-%02x-%02x-%02x] has not been bundled on any iface\n",
              jssdk_mac->mac[0],jssdk_mac->mac[1],jssdk_mac->mac[2],jssdk_mac->mac[3],jssdk_mac->mac[4],jssdk_mac->mac[5]);
    return -1;
}

static int ieee8021xBundleMac(fal_fdb_mac_t *jssdk_mac,struct iface *ife)
{
    int cur_lport = atoi(ife->ifname + 4);

    if(ieee8021x.hash_mac == NULL)
    {
        ieee8021x.hash_mac = calloc(HASH_MAC_LEN,sizeof(struct list_head));
        if(ieee8021x.hash_mac == NULL)
        {
            LOG_ERROR("calloc hash mac error\n");
            return -1;
        }
        int idx;
        for(idx = 0;idx < HASH_MAC_LEN;idx++)
            INIT_LIST_HEAD(ieee8021x.hash_mac + idx);

create:
        {
        struct mac_list *bind = calloc(1,sizeof(struct mac_list));
        if(bind == NULL)
        {
            LOG_ERROR("calloc bind-mac memery error\n");
            return -1;
        }
        memcpy(bind->mac,jssdk_mac->mac,ETH_ALEN);
        bind->lport = cur_lport;
        list_add_tail(&bind->inode,&ife->bind_mac);
        list_add_tail(&bind->hnode,ieee8021x.hash_mac + HASH_MAC(jssdk_mac->mac));
        }
    }
    else
    {
        struct list_head *hhead = ieee8021x.hash_mac + HASH_MAC(jssdk_mac->mac);
        if(list_empty(hhead))
            goto create;

        struct mac_list *bd,*bd_bak;
        list_for_each_entry_safe(bd,bd_bak,hhead,hnode)
        {
            if(!memcmp(bd->mac,jssdk_mac->mac,ETH_ALEN))   
            {
                if(bd->lport == cur_lport)
                {
                    LOG_WARN("mac[%02x-%02x-%02x-%02x-%02x-%02x] has already bound on port %d,do nothing\n",\
                             jssdk_mac->mac[0],jssdk_mac->mac[1],jssdk_mac->mac[2],jssdk_mac->mac[3],jssdk_mac->mac[4],jssdk_mac->mac[5],cur_lport);
                    return 0;
                }
                else
                {
                    LOG_WARN("mac[%02x-%02x-%02x-%02x-%02x-%02x] has been bound,unbundling from old port %d and binding on new port %d\n",\
                             jssdk_mac->mac[0],jssdk_mac->mac[1],jssdk_mac->mac[2],jssdk_mac->mac[3],jssdk_mac->mac[4],jssdk_mac->mac[5],bd->lport,cur_lport);
                    if(fal_mac_row_destroy(0,jssdk_mac,bd->lport))
                    {
                        LOG_ERROR("jssdk destory mac[%02x-%02x-%02x-%02x-%02x-%02x] on port %d error\n",\
                                  jssdk_mac->mac[0],jssdk_mac->mac[1],jssdk_mac->mac[2],jssdk_mac->mac[3],jssdk_mac->mac[4],jssdk_mac->mac[5],bd->lport);
                        return -1;
                    }
                    list_del(&bd->inode);
                    bd->lport = cur_lport;
                    list_add_tail(&bd->inode,&ife->bind_mac);
                }
            }
        }

        goto create;
    }

    if(fal_8021x_mac_bind(0,cur_lport,FAL_MAC_STATIC,jssdk_mac))
    {
        LOG_ERROR("jssdk bind mac [%02x-%02x-%02x-%02x-%02x-%02x] to port %d error!\n",
                  jssdk_mac->mac[0],jssdk_mac->mac[1],jssdk_mac->mac[2],jssdk_mac->mac[3],jssdk_mac->mac[4],jssdk_mac->mac[5],cur_lport);
        return -1;
    }

    return 0;
}

static int ieee8021xGetState(struct blob_buf *buf)
{
    char state_s[2];
    
    sprintf(state_s,"%d",ieee8021x.state);
    blobmsg_add_string(buf, "globle_state", state_s);
    LOG_NORMAL("get ieee802.1x globle state = [%s] OK\n",state_s);

    return 0;
}

static int ieee8021xSetState(void *p)
{

    return 0;
}

static int ieee8021xGetEapMode(struct blob_buf *buf)
{
    char mode_s[2];
    
    sprintf(mode_s,"%d",ieee8021x.eap_mode);
    blobmsg_add_string(buf, "eap_mode", mode_s);
    LOG_NORMAL("get ieee802.1x eap mode = [%s] OK\n",mode_s);

    return 0;
}

static int ieee8021xSetEapMode(void *p)
{

    return 0;
}

static int ieee8021xGetEapModel(struct blob_buf *buf)
{
    char model_s[2];
    
    sprintf(model_s,"%d",ieee8021x.eap_model);
    blobmsg_add_string(buf, "eap_model", model_s);
    LOG_NORMAL("get ieee802.1x eap model = [%s] OK\n",model_s);

    return 0;
}

static int ieee8021xSetEapModel(void *p)
{

    return 0;
}

static int ieee8021xGetIfaceNum(struct blob_buf *buf)
{
    char num_s[2];
    
    sprintf(num_s,"%d",ieee8021x.iface_num);
    blobmsg_add_string(buf, "iface_num", num_s);
    LOG_NORMAL("get ieee802.1x iface num = [%s] OK\n",num_s);

    return 0;
}

static int ieee8021xGetIfaceMac(struct blob_buf *buf,int idx)
{
    return 0;
}

static int ieee8021xIfaceBundleMac(int idx,void *p)
{
    fal_fdb_mac_t mac_new;
    sw_error_t ret;
    
    /**<    check arguement  */
    ret = parse_mac(mac_new.mac,(char *)p);
    if(ret)
    {
        LOG_ERROR("analyse fdb mac address fail!\n");
        return -1;
    }

    struct iface *ife,*ife_bak;
    list_for_each_entry_safe(ife,ife_bak,&ieee8021x.iface,node)
    {
        if(ife->idx == idx)
        {
            if(ife->ifname == NULL)
            {
                LOG_ERROR("iface-%d has not set ifname!\n",idx);
                return -1;           
            }
            
            if(ife->access_mode == BASE_ON_PORT)
            {
                LOG_ERROR("iface-%d access mode is <base on port>,should not bundle mac!\n",idx);
                return -1;           
            }

            if(ieee8021xBundleMac(&mac_new,ife))
            {
                LOG_ERROR("iface-%d bind mac error!\n",idx);
                return -1;           
            }

            LOG_NORMAL("jssdk bind mac [%02x-%02x-%02x-%02x-%02x-%02x] to iface-%d OK!\n",
                      mac_new.mac[0],mac_new.mac[1],mac_new.mac[2],mac_new.mac[3],mac_new.mac[4],mac_new.mac[5],idx);
            return 0;
        }
    }

    LOG_ERROR("no matching iface index(%d)!\n",idx);
    
    return -1;
}

static int ieee8021xIfaceUnbundleMac(int idx,void *p)
{
    fal_fdb_mac_t mac_exist;
    sw_error_t ret;
    
    /**<    check arguement  */
    ret = parse_mac(mac_exist.mac,(char *)p);
    if(ret)
    {
        LOG_ERROR("analyse fdb mac address fail!\n");
        return -1;
    }

    struct iface *ife,*ife_bak;
    list_for_each_entry_safe(ife,ife_bak,&ieee8021x.iface,node)
    {
        if(ife->idx == idx)
        {
            if(ife->ifname == NULL)
            {
                LOG_ERROR("iface-%d has not set ifname!\n",idx);
                return -1;           
            }

            if(ieee8021xUnbundleMac(&mac_exist,ife))
            {
                LOG_ERROR("iface-%d bind mac error!\n",idx);
                return -1;           
            }

            LOG_NORMAL("jssdk unbundle mac [%02x-%02x-%02x-%02x-%02x-%02x] from iface-%d OK!\n",
                      mac_exist.mac[0],mac_exist.mac[1],mac_exist.mac[2],mac_exist.mac[3],mac_exist.mac[4],mac_exist.mac[5],idx);
            return 0;
        }
    }

    LOG_ERROR("no matching iface index(%d)!\n",idx);
    
    return -1;
}

static int ieee8021xGetRadiusServer(struct blob_buf *b,const char *role)
{
    struct radius_server *radius = (!strcmp(role,"primary_radius_server"))?ieee8021x.primary:ieee8021x.secondary;
    if(radius == NULL)
    {
        LOG_ERROR("%s is no exist\n",role);
        return -1;
    }

    void *ret_array;
    void *ret_table;

    ret_array = blobmsg_open_array(b, role); 

    if(radius->auth_server_addr != NULL)
    {
        ret_table = blobmsg_open_table(b, NULL);
        blobmsg_add_string(b, "auth_server_addr", radius->auth_server_addr);
        blobmsg_close_table(b, ret_table);	
    }
    if(radius->auth_server_port)
    {
        ret_table = blobmsg_open_table(b, NULL);
        char buf[16];
        sprintf(buf,"%d",radius->auth_server_port);
        blobmsg_add_string(b, "auth_server_port", buf);
        blobmsg_close_table(b, ret_table);	
    }
    if(radius->auth_server_share_srcret != NULL)
    {
        ret_table = blobmsg_open_table(b, NULL);
        blobmsg_add_string(b, "auth_server_shared_secret", radius->auth_server_share_srcret);
        blobmsg_close_table(b, ret_table);	
    }
    if(radius->acct_server_addr != NULL)
    {
        ret_table = blobmsg_open_table(b, NULL);
        blobmsg_add_string(b, "acct_server_addr", radius->acct_server_addr);
        blobmsg_close_table(b, ret_table);	
    }
    if(radius->acct_server_port)
    {
        ret_table = blobmsg_open_table(b, NULL);
        char buf[16];
        sprintf(buf,"%d",radius->acct_server_port);
        blobmsg_add_string(b, "acct_server_port", buf);
        blobmsg_close_table(b, ret_table);	
    }
    if(radius->acct_server_share_srcret != NULL)
    {
        ret_table = blobmsg_open_table(b, NULL);
        blobmsg_add_string(b, "acct_server_shared_secret", radius->acct_server_share_srcret);
        blobmsg_close_table(b, ret_table);	
    }
            
    blobmsg_close_array(b, ret_array);	
    LOG_NORMAL("get ieee802.1x %s OK\n",role);

    return 0;
}

static int ieee8021xSetRadiusServer(int role,void *p)
{

    return 0;
}

static int ieee8021xIfaceShow(struct blob_buf *b,int idx)
{
    void *ret_array;
    void *ret_table;

    struct iface *ife,*ife_bak;
    list_for_each_entry_safe(ife,ife_bak,&ieee8021x.iface,node)
    {
        if(ife->idx == idx)
        {
            char buf[16];
            sprintf(buf,"iface-%d",idx);
            ret_array = blobmsg_open_array(b, buf); 

            if(ife->ifname != NULL)
            {
                ret_table = blobmsg_open_table(b, NULL);
                blobmsg_add_string(b, "ifname", ife->ifname);
                blobmsg_close_table(b, ret_table);	
            }

            ret_table = blobmsg_open_table(b, NULL);
            sprintf(buf,"%d",ife->reauth);
            blobmsg_add_string(b, "reauth", buf);
            blobmsg_close_table(b, ret_table);	

            ret_table = blobmsg_open_table(b, NULL);
            sprintf(buf,"%d",ife->access_mode);
            blobmsg_add_string(b, "access_mode", buf);
            blobmsg_close_table(b, ret_table);	

            blobmsg_close_array(b, ret_array);	
            
            LOG_NORMAL("get ieee802.1x iface-%d<%s> OK\n",idx,ife->ifname);
            return 0;
        }
    }

    LOG_ERROR("no matching idx = %d\n",idx);
    return -1;
}

static int ieee8021xIfaceAdd(void *p)
{
    int idx = atoi((char *)p + strlen("iface-"));

    if(idx != (ieee8021x.iface_num + 1))
    {
        LOG_ERROR("invalid arguement!\n");
        return -1;
    }

    ieee8021x.iface_num++;
    struct iface *ife = calloc(1,sizeof(struct iface));
    list_add_tail(&ife->node,&ieee8021x.iface);
    ife->idx = ieee8021x.iface_num;
    INIT_LIST_HEAD(&ife->bind_mac);

    return 0;
}

static int ieee8021xIfaceDel(void *p)
{
    int idx = atoi((char *)p + strlen("iface-"));

    if(idx < 1 || idx > ieee8021x.iface_num)
    {
        LOG_ERROR("invalid arguement!\n");
        return -1;
    }

    /**< TODO  */

    return 0;
}

static int ieee8021xIfaceGetName(struct blob_buf *b,int idx)
{

    return 0;
}

static int ieee8021xIfaceSetName(int idx,void *p)
{

    return 0;
}

static const struct jw_switch_policy ieee8021x_tbl[] = {
    [IEEE8021X_STATE] = {.name = "globle_state", .get_ext_handler = ieee8021xGetState, .set_ext_handler = ieee8021xSetState}, 
    [IEEE8021X_EAP_MODE] = {.name = "eap_mode", .get_ext_handler = ieee8021xGetEapMode, .set_ext_handler = ieee8021xSetEapMode}, 
    [IEEE8021X_EAP_MODEL] = {.name = "eap_model", .get_ext_handler = ieee8021xGetEapModel, .set_ext_handler = ieee8021xSetEapModel},
    [IEEE8021X_IFACE_NUM] = {.name = "iface_num", .get_ext_handler = ieee8021xGetIfaceNum}, 
    [IEEE8021X_IFACE_SHOW] = {.name = "iface_show", .get_handler = ieee8021xIfaceShow}, 
    [IEEE8021X_IFACE_ADD] = {.name = "iface_add", .set_ext_handler = ieee8021xIfaceAdd}, 
    [IEEE8021X_IFACE_DEL] = {.name = "iface_del", .set_ext_handler = ieee8021xIfaceDel}, 
    [IEEE8021X_IFACE_NAME] = {.name = "ifname", .get_handler = ieee8021xIfaceGetName, .set_handler = ieee8021xIfaceSetName}, 
    [IEEE8021X_IFACE_BUNDLE] = {.name = "bundle_mac", .set_handler = ieee8021xIfaceBundleMac}, 
    [IEEE8021X_IFACE_UNBUNDLE] = {.name = "unbundle_mac", .set_handler = ieee8021xIfaceUnbundleMac}, 
};

static int ieee8021xParseIfaceGet(struct blob_attr *lvl3, struct blob_buf *buf)
{
    const char *name = blobmsg_name(lvl3);
    
    if(strcmp("iface_show",name))
    {
        LOG_ERROR("invalid arguement,expect to get \"iface_show\"!\n");
        return -1;
    }

    struct blob_attr *lvl4 = NULL;
    int rem;
    blobmsg_for_each_attr(lvl4,lvl3, rem) 
    {
        if(blobmsg_type(lvl4) == BLOBMSG_TYPE_STRING)
        {
            char *name = blobmsg_get_string(lvl4);
            if(strncmp(name,"iface-",strlen("iface-")))
            {
                LOG_ERROR("policy check not pass!\n");
                return -1;
            }

            int idx = atoi(name + strlen("iface-"));
            if(idx < 1 || idx > ieee8021x.iface_num)
            {
                LOG_ERROR("iface idx in invalid\n");
                return -1;
            }

            ieee8021xIfaceShow(buf,idx);    
        }
        else
        {
            LOG_ERROR("level 4 message type is not available!\n");
            return -1;
        }
    }

    return 0;
}

static int ieee8021xParseIfaceSet(struct blob_attr *lvl3)
{
    const char *name    = blobmsg_name(lvl3);
    int idx     = atoi(name + strlen("iface-"));

    if (idx < 1 || idx > ieee8021x.iface_num) 
    {
        LOG_ERROR("invalid iface index!\n");
        return -1;
    }

    /**<    foreach array of level 3 message  */
    int ret;
    ret = foreach_json_array_set(idx,lvl3,ieee8021x_tbl,__IEEE8021X_TBL_MAX);
    if(ret)
    {
        LOG_ERROR("foreach index %d error!\n",idx);
        return -1;
    }

    return 0;
}

static int ieee8021xGetHandler(struct ubus_context *ctx, struct ubus_object *obj,
                struct ubus_request_data *req, const char *method,
                struct blob_attr *msg)
{
    struct blob_attr *tb[__IEEE8021X_GET_MAX];    
	struct blob_attr *lvl1 = NULL;  

    blobmsg_parse(ieee8021x_get_policy, __IEEE8021X_GET_MAX, tb, blob_data(msg), blob_len(msg));
	lvl1 = tb[IEEE8021X_GET_KEY];     
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

                ret_table = blobmsg_open_table(&b, NULL);
                policy_p = (struct jw_switch_policy *)jw_switchd_get_context(name, ieee8021x_tbl, __IEEE8021X_TBL_MAX);
                if (policy_p && policy_p->get_ext_handler) 
                    policy_p->get_ext_handler(&b);
                else if(!strcmp(name,"primary_radius_server"))
                    ieee8021xGetRadiusServer(&b,name);
                else if(!strcmp(name,"secondary_radius_server"))
                    ieee8021xGetRadiusServer(&b,name);
                else
                {
                    LOG_ERROR("policy check not pass!\n");
                }
                blobmsg_close_table(&b, ret_table);	
            }
                break;
            case BLOBMSG_TYPE_TABLE:
            {
                struct blob_attr *lvl3 = blobmsg_data(lvl2);

                if(blobmsg_type(lvl3) == BLOBMSG_TYPE_ARRAY)
                {
                    ret_table = blobmsg_open_table(&b, NULL);
                    ieee8021xParseIfaceGet(lvl3,&b);
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

static int ieee8021xSetHandler(struct ubus_context *ctx, struct ubus_object *obj,
                struct ubus_request_data *req, const char *method,
                struct blob_attr *msg)
{
    struct blob_attr *tb[__IEEE8021X_SET_MAX];    
	struct blob_attr *lvl1 = NULL;  
    int ret = -1;

    blobmsg_parse(ieee8021x_set_policy, __IEEE8021X_SET_MAX, tb, blob_data(msg), blob_len(msg));
	lvl1 = tb[IEEE8021X_SET_KEY];     
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

                    policy_p = (struct jw_switch_policy *)jw_switchd_get_context(name, ieee8021x_tbl, __IEEE8021X_TBL_MAX);
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
                    ret = ieee8021xParseIfaceSet(lvl3);
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

static const struct ubus_method ieee8021x_methods[] = {
	UBUS_METHOD("802.1x_get", ieee8021xGetHandler, ieee8021x_get_policy),
    UBUS_METHOD("802.1x_set", ieee8021xSetHandler, ieee8021x_set_policy),
};


static struct ubus_object_type ieee8021x_object_type =
	UBUS_OBJECT_TYPE("802.1x", ieee8021x_methods);


static struct ubus_object ieee8021x_object = {
	.name = "802.1x",
	.type = &ieee8021x_object_type,
	.methods = ieee8021x_methods,
	.n_methods = ARRAY_SIZE(ieee8021x_methods),
};

int ieee8021xLoadAdmin(struct uci_section *sct)
{
    struct uci_option *opt;
    struct uci_element *elt;

    uci_foreach_element(&sct->options,elt)
    {
        opt = uci_to_option(elt);
        if(!strcmp(elt->name,"802_1x"))
            ieee8021x.state = strcmp(opt->v.string,"on")?FAL_DISABLE:FAL_ENABLE;
        else if(!strcmp(elt->name,"eap_mode"))
            ieee8021x.eap_mode = strcmp(opt->v.string,"relay")?EAP_TERMINAL:EAP_RELAY;
        else if(!strcmp(elt->name,"eap_model"))
            ieee8021x.eap_mode = strcmp(opt->v.string,"pass_through")?EAP_LOCAL:EAP_PASSTHROUGH;
        else
        {
            LOG_ERROR("option name = %s,while not define\n",elt->name);
            // return -1;
        }
    }

    LOG_NORMAL("ieee802.1x load globle configure OK\n");

    return 0;
}

int ieee8021xLoadRadius(struct uci_section *sct)
{
    struct uci_option *opt;
    struct uci_element *elt;
    struct radius_server *radius = calloc(1,sizeof(struct radius_server));

    uci_foreach_element(&sct->options,elt)
    {
        opt = uci_to_option(elt);
        if(!strcmp(elt->name,"auth_server_addr"))
        {
            if(if_valid_ip(opt->v.string))
                goto fail;
            radius->auth_server_addr = strdup(opt->v.string);
        }
        else if(!strcmp(elt->name,"acct_server_addr"))
        {
            if(if_valid_ip(opt->v.string))
                goto fail;
            radius->acct_server_addr = strdup(opt->v.string);
        }
        else if(!strcmp(elt->name,"auth_server_port"))
        {
            int port = atoi(opt->v.string);
            if(port <= 0 || port > 0xffff)
                goto fail;
            if(radius->acct_server_port == port)
                goto fail;
            radius->auth_server_port = port;
        }
        else if(!strcmp(elt->name,"acct_server_port"))
        {
            int port = atoi(opt->v.string);
            if(port <= 0 || port > 0xffff)
                goto fail;
            if(radius->auth_server_port == port)
                goto fail;
            radius->acct_server_port = port;
        }
        else if(!strcmp(elt->name,"auth_server_shared_secret"))
        {
            if(strlen(opt->v.string) > 256)
                goto fail;
            radius->auth_server_share_srcret = strdup(opt->v.string);
        }
        else if(!strcmp(elt->name,"acct_server_shared_secret"))
        {
            if(strlen(opt->v.string) > 256)
                goto fail;
            radius->acct_server_share_srcret = strdup(opt->v.string);
        }
        else
        {
            LOG_ERROR("option name = %s,while not define\n",elt->name);
            // goto fail;
        }
    }

    if(!strcmp(sct->e.name,"primary"))
        ieee8021x.primary = radius;
    else if(!strcmp(sct->e.name,"secondary"))
        ieee8021x.secondary = radius;

    LOG_NORMAL("ieee802.1x load %s radius server configure ok\n",sct->e.name);

    return 0;

fail:
    if(radius->auth_server_share_srcret)
        free(radius->auth_server_share_srcret);
    if(radius->acct_server_share_srcret)
        free(radius->acct_server_share_srcret);
    free(radius);
    return -1;
}

static int ieee8021xLoadIface(struct uci_section *sct)
{
    struct uci_option *opt;
    struct uci_element *elt;
    struct iface *iface = calloc(1,sizeof(struct iface));

    uci_foreach_element(&sct->options,elt)
    {
        opt = uci_to_option(elt);
        if(!strcmp(elt->name,"ifname"))
        {
            if(ubus_is_ifname(opt->v.string))
            {
                iface->ifname = strdup(opt->v.string);

#if 0 
                // may be fdb has not completed initialize in this time
                if(ubus_is_fdb_port(lport))
                {
                    LOG_ERROR("port %d has been already used as fdb port!\n",lport);
                    return -1;
                }
#endif
            }
            else
            {
                LOG_ERROR("this is in %s-%d\n",__func__,__LINE__);
                goto fail;
            }
        }
        else if(!strcmp(elt->name,"reauth"))
        {
            int reauth = atoi(opt->v.string);
            if(reauth < 0 || reauth > 0xffff)
            {
                LOG_ERROR("this is in %s-%d\n",__func__,__LINE__);
                goto fail;
            }

            iface->reauth = reauth;
        }
        else if(!strcmp(elt->name,"access_mode"))
        {
            iface->access_mode = strcmp(opt->v.string,"base_on_port")?BASE_ON_MAC:BASE_ON_PORT;

            int lport = atoi(iface->ifname + 4);
            if(iface->access_mode == BASE_ON_MAC && fal_8021x_port_auth_set(0,lport,FAL_ENABLE))
            {
                LOG_ERROR("jssdk set 802.1x enable for port %d error!\n",lport);
                return -1;
            }
        }
        else
        {
            LOG_ERROR("option name = %s,while not define\n",elt->name);
            // goto fail;
        }
    }

    list_add_tail(&iface->node,&ieee8021x.iface);
    ieee8021x.iface_num++;
    iface->idx = ieee8021x.iface_num;
    INIT_LIST_HEAD(&iface->bind_mac);

    LOG_NORMAL("ieee802.1x load iface<%s> configure OK\n",iface->ifname?iface->ifname:"null");

    return 0;

fail:
    if(iface->ifname)
        free(iface->ifname);
    free(iface);
    return -1;
}

static struct uciConfig {
    const char *type;
    int (*func)(struct uci_section *sct);
}ieee8021xUci[] = {
    {.type = "admin",   .func = ieee8021xLoadAdmin},
    {.type = "radius",  .func = ieee8021xLoadRadius},
    {.type = "iface",   .func = ieee8021xLoadIface}
};

static int ieee8021xCfgLoad(void)
{
    struct uci_context *ctx;
    struct uci_package *pkg;

    memset(&ieee8021x, 0, sizeof(ieee8021x));
    INIT_LIST_HEAD(&ieee8021x.iface);

    ctx = uci_alloc_context();

    if(UCI_OK != uci_load(ctx,"/etc/config/nas",&pkg))
        return -1;

    struct uci_section *sct;
    struct uci_element *elt;
    uci_foreach_element(&pkg->sections,elt)
    {
        sct = uci_to_section(elt);
        int i;
        for(i = 0;i < sizeof(ieee8021xUci)/sizeof(struct uciConfig);i++)
        {
            if(!strcmp(sct->type,ieee8021xUci[i].type))
            {
                if(ieee8021xUci[i].func(sct))
                {
                    LOG_ERROR("load config %s fail!\n",sct->e.name);
                    return -1;
                }
                break;
            }
        }
    }

    uci_free_context(ctx);
    LOG_NORMAL("ieee802.1x load configure OK!\n");

    return 0;
}

void ubus_init_8021x(struct ubus_context *ctx)
{
    int ret;

	_ctx = ctx;

    ret = ieee8021xCfgLoad();
    if(ret)
    {
        LOG_ERROR("Init ieee802.1x configure fail!\n"); 
        return;
    }

	ret = ubus_add_object(ctx, &ieee8021x_object);
	if (ret)
		LOG_ERROR("Failed to add object [ieee8021x]: %s\n", ubus_strerror(ret));
}
