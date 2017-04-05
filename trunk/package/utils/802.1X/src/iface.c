#include <sys/ioctl.h>
#include <dirent.h>
#include <linux/rtnetlink.h>

#include "driver/driver.h"
#include "ubus.h"
#include "iface.h"
#include "radius/radius_client.h"
#include "ieee802_1x.h"

static int iface_driver_init(struct interface *iface,int (*ubus_ops)(struct bss_data *bss,int cmd,void *data))
{
    struct bss_data *bss = iface->bss;
    struct driver_init_params param;
    memset(&param,0,sizeof(struct driver_init_params));

    if(bss->driver == NULL || bss->driver->driver_init == NULL)
        return -1;

    param.ifname = iface->conf.ifname;
    param.use_pae_group_addr = iface->conf.use_pae_group_addr;       
    param.access_mode = iface->conf.access_mode;
    param.own_addr = bss->own_addr;
    param.ifindex = iface->conf.ifindex;
    param.ubus_ops = ubus_ops;
    param.rtnl_group = RTMGRP_LINK;
    bss->drv_priv = bss->driver->driver_init(bss,&param);
    if(bss->drv_priv == NULL)
        return -1;

    return 0;
}

/* free one interface
 * TODO add deinit 802.1x features on bss
 */
void iface_free(struct interface *iface)
{
    struct bss_data *bss = iface->bss;

    if(bss->driver && bss->driver->driver_deinit)
        bss->driver->driver_deinit(bss->drv_priv);

    free(bss);
    free(iface);
}

static struct bss_data *iface_bss_init(struct interface *iface,struct iface_conf *conf)
{
    struct bss_data *bss;

    bss = calloc(1,sizeof(struct bss_data));
    if(bss == NULL)
        return NULL;

    bss->iface = iface;
    bss->driver = conf->driver;
    bss->link = LINK_DOWN;
    //bss->new_assoc_sta_cb = iface_new_assoc_sta;

    return bss;
}

/* setup services for every interface according to configuration
 *
 * */
int iface_bss_setup(struct interface *iface,struct ieee802_1x_conf *conf)
{
    struct bss_data *bss;

    bss = iface->bss;

    bss->radius = radius_client_init(bss, conf->radius_conf);
    if(bss->radius == NULL)
    {
        LOG_ERROR("radius client init error");
        return -1;
    }
    LOG_D("radius client init OK");

    if(ieee802_1x_init(bss))
    {
        LOG_ERROR("IEEE 802.1X init error");
        return -1;
    }
    LOG_D("IEEE-802.1x init OK");

#if 0
    if(accounting_init(hapd))
    {
        LOG_ERROR("accounting init error");
        return -1;
    }
#endif

    return 0;
}

/* interface initialize
 * create a manage structure for interface , then init and setup
 * */
int iface_init(struct ieee802_1x_conf *conf_g,struct ubus_iface_param *param)
{
    struct interface *iface;

    iface = calloc(1,sizeof(struct interface));
    if(iface == NULL)
        return -1;

    conf_iface(&iface->conf,conf_g,param);

    iface->bss = iface_bss_init(iface,&iface->conf);
    if(iface->bss == NULL)
    {
        iface_free(iface);
        return -1;
    }

    if(iface_driver_init(iface,param->ubus_ops) || iface_bss_setup(iface,conf_g))
    {
        iface_free(iface);
        return -1;
    }

    if(conf_g->lnode == NULL)
        INIT_LIST_HEAD(&iface->node);
    else
        list_add(&iface->node,conf_g->lnode);

    conf_g->lnode = &iface->node;

    return 0;
}

