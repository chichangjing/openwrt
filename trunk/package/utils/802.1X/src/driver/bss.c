#include "driver/driver.h"
#include "iface.h"
#include "conf.h"
#include "ubus.h"
#include "sta_info.h"
#include "ieee802_1x.h"

static int bss_new_assoc_sta(struct bss_data *bss,const uint8_t *addr)
{
    struct sta_info *sta = sta_get(bss, addr);
    if(sta)
        return 0;

    LOG_D("Data frame from unknown STA " MACSTR " - adding a new STA",MAC2STR(addr));
    sta = sta_add(bss, addr);
    if(sta == NULL)
    {
        LOG_ERROR("Failed to add STA entry for " MACSTR,MAC2STR(addr));
        return -1;
    }

    //ieee802_1x_new_station(bss, sta);

    return 0;
}

static int bss_eapol_rx(struct bss_data *bss,struct eapol_rx *event)
{
    struct sta_info *sta = sta_get(bss, event->src);
    if(sta == NULL)
    {
        LOG_D("Data frame from unknown STA " MACSTR " - adding a new STA",MAC2STR(event->src));
        sta = sta_add(bss, event->src);
        if(sta == NULL)
        {
            LOG_ERROR("Failed to add STA entry for " MACSTR,MAC2STR(event->src));
            return -1;
        }
    }

    ieee802_1x_receive(bss,sta,event->data,event->data_len);

    return 0;
}

static int bss_link_notify(struct bss_data *bss,struct link_notify *event)
{
    struct iface_conf *conf = BSS_GET_CONF(bss);

    if(atoi(conf->ifname + 4) != event->lport)
        return 0;

    if(event->link != bss->link)
    {
        bss->link = event->link;
        LOG_NORMAL("port %d link %s",event->lport,(bss->link == LINK_UP)?"up":"down");

        struct sta_info *sta;
        if(conf->access_mode == BO_PORT)
        {
            if(bss->link == LINK_DOWN && (sta = bss->sta_list) != NULL)
            {
                LOG_D("Removing station " MACSTR,MAC2STR(sta->addr));
                sta_free(bss, sta);
            }
        }
        else
        {
            if(bss->link == LINK_DOWN)
            {
                struct sta_info *prev;

                sta = bss->sta_list;
                while (sta) 
                {
                    prev = sta;
                    sta = sta->next;
                    LOG_D("Removing station " MACSTR,MAC2STR(prev->addr));
                    bss->driver->driver_set_authorized(bss->drv_priv,prev->addr,0);
                    sta_free(bss, prev);
                }

                return 0;
            }
        }
        
        bss->driver->driver_set_port_state(bss->drv_priv,conf->access_mode);
    }

    return 0;
}

void bss_event_handle(void *ctx,enum event_type type,union event_data *event)
{
    struct bss_data *bss = ctx;
    switch(type)
    {
        case EVENT_NEW_STA:
            bss_new_assoc_sta(bss,event->new_sta.addr);
            break;
        case EVENT_EAPOL_RX:
            if(bss->later)
            {
                free(bss->later);
                bss->later = NULL;
            }
            if(bss->link == LINK_DOWN)
            {
                bss->later = calloc(1,sizeof(struct eapol_rx) + ETH_ALEN + event->eapol_rx.data_len);
                if(bss->later == NULL)
                {
                    LOG_ERROR("calloc rx eapol later structure error");
                    return;
                }

                bss->later->src = (uint8_t *)(bss->later + 1);
                bss->later->data = bss->later->src + ETH_ALEN;
                memcpy(bss->later + 1,event->eapol_rx.src,ETH_ALEN);
                memcpy((uint8_t *)(bss->later + 1) + ETH_ALEN,event->eapol_rx.data,event->eapol_rx.data_len);
                bss->later->data_len = event->eapol_rx.data_len;
                
                LOG_D("parse this eapol packet later because the interface is in link down status");
                return;
            }
            bss_eapol_rx(bss,&event->eapol_rx);
            break;
        case EVENT_LINK:
            bss_link_notify(bss,&event->link_notify);
            if(bss->later)
            {
                if(bss->link == LINK_UP)
                    bss_eapol_rx(bss,bss->later);
                free(bss->later);
                bss->later = NULL;
            }
            break;
        default:
            break;
    }
}
