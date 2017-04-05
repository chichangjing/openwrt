#ifndef _CTL_H_
#define _CTL_H_

#include "common/ip_addr.h"
#include "common/eapol_common.h"
#include "ubus.h"
#include "sta_info.h"

#define MAX_STA_COUNT 2007



struct iface_conf {
    struct ieee802_1x_conf *conf_g;
    int ifindex;                    // interface index from ubus
    char ifname[IFNAMSIZ + 1];      // interface name
    char bridge[IFNAMSIZ + 1];

    int ieee802_1x;                 // if enable 802.1x feature on this interface
    int eapol_version;              // default use <2 : IEEE 802.1X-2004>，while support <1 : IEEE 802.1X-2001>
    int eap_server;                 // if use local eap server


    int eap_reauth_period;          // if not 0 means enable reauth feature and reauth period for reauth timer
    int access_mode;                // base on port or base on mac

    int max_num_sta;                // max number of station

    char *eap_req_id_text;          // (optional)additional information for eapol-eap-identifier
    size_t eap_req_id_text_len;     // (optional)

    struct ip_addr own_ip_addr;
    char *nas_identifier;

    const struct driver_ops *driver;// point to a set of driver
    int use_pae_group_addr;         // if use group addr when sending eapol packet

    int ap_max_inactivity;          // station ageing time
};

struct radius_servers;
struct ieee802_1x_conf {
    struct radius_servers *radius_conf;
    int eap_mode;
    int eap_model;
    int iface_num;
    struct list_head *lnode;    // point to the last interface
};

#define EAP_USER_MAX_METHODS 8
struct hostapd_eap_user {
    struct hostapd_eap_user *next;
    uint8_t *identity;
    size_t identity_len;
    struct {
        int vendor;
        uint32_t method;
    }methods[EAP_USER_MAX_METHODS];
    uint8_t *password;
    size_t password_len;
    int phase2;
    int force_version;
    unsigned int wildcard_prefix:1;
    unsigned int password_hash:1; /*  whether password is hashed with* nt_password_hash()  */
    int ttls_auth; /*  EAP_TTLS_AUTH_* bitfield  */
};

struct ubus_iface_param;
struct radius_server;

extern struct driver_ops driver_ops;
#define DEFAULT_REAUTH_PERIOD 3600
static inline void conf_defaults_iface(struct iface_conf *conf)
{
    conf->eap_reauth_period = DEFAULT_REAUTH_PERIOD;
    conf->eapol_version = EAPOL_VERSION;
    conf->ieee802_1x = 1;
    conf->access_mode = BO_PORT;
    conf->max_num_sta = MAX_STA_COUNT;
    conf->ap_max_inactivity = AP_MAX_INACTIVITY;
}

static inline void conf_iface(struct iface_conf *conf,struct ieee802_1x_conf *conf_g,struct ubus_iface_param *param)
{
    conf_defaults_iface(conf);

    conf->driver = &driver_ops;

    strlcpy(conf->ifname,param->ifname,IFNAMSIZ + 1);
    conf->ifindex = param->ifindex;
    conf->eap_reauth_period = param->reauth;
    conf->access_mode = param->access_mode;
    conf->conf_g = conf_g;
}

int conf_radius_server(struct radius_server **servers,int *num_server,const char *ip,int def_port,struct radius_server **curr_serv);
#endif
