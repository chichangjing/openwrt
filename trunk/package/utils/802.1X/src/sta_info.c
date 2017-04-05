#include "common/wpabuf.h"
#include "conf.h"
#include "iface.h"
#include "driver/driver.h"
#include "radius/radius.h"
#include "radius/radius_client.h"
#include "ieee802_1x.h"
#include "sta_info.h"

static void sta_list_del(struct bss_data *bss, struct sta_info *sta)
{
	struct sta_info *tmp;

	if (bss->sta_list == sta) {
		bss->sta_list = sta->next;
		return;
	}

	tmp = bss->sta_list;
	while (tmp != NULL && tmp->next != sta)
		tmp = tmp->next;
	if (tmp == NULL) {
		LOG_D("Could not remove STA " MACSTR " from "
			   "list.", MAC2STR(sta->addr));
	} else
		tmp->next = sta->next;
}

static void sta_hash_del(struct bss_data *bss, struct sta_info *sta)
{
	struct sta_info *s;

	s = bss->sta_hash[STA_HASH(sta->addr)];
	if (s == NULL) return;
	if (memcmp(s->addr, sta->addr, 6) == 0) {
		bss->sta_hash[STA_HASH(sta->addr)] = s->hnext;
		return;
	}

	while (s->hnext != NULL &&
	       memcmp(s->hnext->addr, sta->addr, ETH_ALEN) != 0)
		s = s->hnext;
	if (s->hnext != NULL)
		s->hnext = s->hnext->hnext;
	else
		LOG_D("AP: could not remove STA " MACSTR
			   " from hash table", MAC2STR(sta->addr));
}

static void sta_remove_in_other_bss(struct bss_data *bss,struct sta_info *sta)
{
    struct interface *iface;
    struct sta_info *sta2;

    list_for_each_my(iface,(bss->iface),node)
    {
        LOG_TEST;
        sta2 = sta_get(iface->bss, sta->addr); 
        if(sta2 == NULL)
            continue;
    
        sta_free(iface->bss,sta2);
    }
}

static void sta_ageing_timer(struct uloop_timeout *timer)
{
	struct sta_info *sta = container_of(timer,struct sta_info,ageing_timer);
    struct bss_data *bss = sta->ctx;

    LOG_TEST;
    if (sta->timeout_next == STA_REMOVE)
    {
        sta_free(bss, sta);
        return;
    }

}

static void sta_session_timer(struct uloop_timeout *timer)
{
	struct sta_info *sta = container_of(timer,struct sta_info,session_timer);
	// struct bss_data *bss = sta->eapol_sm->eapol->conf->ctx;
	uint8_t addr[ETH_ALEN];

	sta->acct_terminate_cause =
		RADIUS_ACCT_TERMINATE_CAUSE_SESSION_TIMEOUT;
	memcpy(addr, sta->addr, ETH_ALEN);
	//sta_free(bss, sta);
}

int sta_for_each(struct bss_data *bss,int (*cb)(struct bss_data *bss, struct sta_info *sta,void *ctx),void *ctx)
{
    struct sta_info *sta;
    for (sta = bss->sta_list; sta; sta = sta->next)
    {
        if (cb(bss, sta, ctx))
            return 1;
    }

    return 0;
}

struct sta_info * sta_get(struct bss_data *bss, const uint8_t *sta)
{
	struct sta_info *s;

	s = bss->sta_hash[STA_HASH(sta)];
	while (s != NULL && memcmp(s->addr, sta, 6) != 0)
		s = s->hnext;
	return s;
}

void sta_hash_add(struct bss_data *bss, struct sta_info *sta)
{
	sta->hnext = bss->sta_hash[STA_HASH(sta->addr)];
	bss->sta_hash[STA_HASH(sta->addr)] = sta;
}

void sta_free(struct bss_data *bss, struct sta_info *sta)
{
	//accounting_sta_stop(bss, sta);

	sta_hash_del(bss, sta);
	sta_list_del(bss, sta);
	bss->num_sta--;

    uloop_timeout_cancel(&sta->ageing_timer);
    uloop_timeout_cancel(&sta->session_timer);

	ieee802_1x_free_station(sta);
#ifndef CONFIG_NO_RADIUS
	radius_client_flush_auth(bss->radius, sta->addr);
#endif /* CONFIG_NO_RADIUS */

	free(sta);
}

void sta_session_timeout(struct bss_data *bss, struct sta_info *sta,
			    uint32_t session_timeout)
{
    uloop_timeout_cancel(&sta->session_timer);
    uloop_timeout_set(&sta->session_timer,session_timeout * 1000);
}

void sta_no_session_timeout(struct bss_data *bss, struct sta_info *sta)
{
    uloop_timeout_cancel(&sta->session_timer);
}

struct sta_info * sta_add(struct bss_data *bss, const uint8_t *addr)
{
	struct sta_info *sta;

	sta = sta_get(bss, addr);
	if (sta)
		return sta;

	LOG_NORMAL("  New STA");
    struct iface_conf *conf = BSS_GET_CONF(bss);
	if (bss->num_sta >= conf->max_num_sta) 
    {
		/* FIX: might try to remove some old STAs first? */
		LOG_D("no more room for new STAs (%d/%d)",
			   bss->num_sta, conf->max_num_sta);
		return NULL;
	}

	sta = calloc(1,sizeof(struct sta_info));
	if (sta == NULL) 
    {
		LOG_ERROR("malloc failed");
		return NULL;
	}

    sta->ctx = bss;
	/* initialize STA info data */
	memcpy(sta->addr, addr, ETH_ALEN);

    /* register 2 timer callback func:
     *      the one for station session
     *      the other one for station ageing
     * 
     * then open ageing-timer here
     * */
    sta->session_timer.cb = sta_session_timer;
    sta->ageing_timer.cb = sta_ageing_timer;
    uloop_timeout_set(&sta->ageing_timer,conf->ap_max_inactivity * 1000);

	sta->next = bss->sta_list;
	bss->sta_list = sta;
	bss->num_sta++;
	sta_hash_add(bss, sta);
    
    sta_remove_in_other_bss(bss,sta);
	return sta;
}

void sta_disconnect(struct bss_data *bss, struct sta_info *sta,
		       const uint8_t *addr, uint16_t reason)
{
	if (sta == NULL)
    {
        LOG_ERROR("station is not exist");
		return;
    }

    //bss->driver->driver_deauth(bss->drv_priv,bss->own_addr,addr,reason);

    uloop_timeout_set(&sta->ageing_timer,0);
	sta->timeout_next = STA_REMOVE;
}
