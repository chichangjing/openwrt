#ifndef STA_INFO_H
#define STA_INFO_H

/* STA flags */
#define WLAN_STA_AUTH BIT(0)
#define WLAN_STA_ASSOC BIT(1)
#define WLAN_STA_PS BIT(2)
#define WLAN_STA_TIM BIT(3)
#define WLAN_STA_PERM BIT(4)
#define WLAN_STA_AUTHORIZED BIT(5)
#define WLAN_STA_PENDING_POLL BIT(6) /* pending activity poll not ACKed */
#define WLAN_STA_SHORT_PREAMBLE BIT(7)
#define WLAN_STA_PREAUTH BIT(8)
#define WLAN_STA_WMM BIT(9)
#define WLAN_STA_MFP BIT(10)
#define WLAN_STA_HT BIT(11)
#define WLAN_STA_WPS BIT(12)
#define WLAN_STA_MAYBE_WPS BIT(13)
#define WLAN_STA_WDS BIT(14)
#define WLAN_STA_NONERP BIT(31)

/* Maximum number of supported rates (from both Supported Rates and Extended
 * Supported Rates IEs). */
#define WLAN_SUPP_RATES_MAX 32


struct sta_info {
    void *ctx;
	struct sta_info *next; /* next entry in sta list */
	struct sta_info *hnext; /* next entry in hash table list */
	uint8_t addr[6];
	uint16_t capability;
	uint16_t listen_interval; /* or beacon_int for APs */

	uint16_t auth_alg;
	uint8_t previous_ap[6];

	enum {
		STA_NULLFUNC = 0, STA_DISASSOC, STA_DEAUTH, STA_REMOVE
	} timeout_next;

	/* IEEE 802.1X related data */
	struct eapol_state_machine *eapol_sm;

	uint32_t acct_session_id_hi;
	uint32_t acct_session_id_lo;
	time_t acct_session_start;
	int acct_session_started;
	int acct_terminate_cause; /* Acct-Terminate-Cause */

	unsigned long last_rx_bytes;
	unsigned long last_tx_bytes;
	uint32_t acct_input_gigawords; /* Acct-Input-Gigawords */
	uint32_t acct_output_gigawords; /* Acct-Output-Gigawords */

	uint8_t *challenge; /* IEEE 802.11 Shared Key Authentication Challenge */

    struct uloop_timeout session_timer;
    struct uloop_timeout ageing_timer;
};


/* Default value for maximum station inactivity. After AP_MAX_INACTIVITY has
 * passed since last received frame from the station, a nullfunc data frame is
 * sent to the station. If this frame is not acknowledged and no other frames
 * have been received, the station will be disassociated after
 * AP_DISASSOC_DELAY seconds. Similarily, the station will be deauthenticated
 * after AP_DEAUTH_DELAY seconds has passed after disassociation. */
#define AP_MAX_INACTIVITY (5 * 60)
#define AP_DISASSOC_DELAY (1)
#define AP_DEAUTH_DELAY (1)
/* Number of seconds to keep STA entry with Authenticated flag after it has
 * been disassociated. */
#define AP_MAX_INACTIVITY_AFTER_DISASSOC (1 * 30)
/* Number of seconds to keep STA entry after it has been deauthenticated. */
#define AP_MAX_INACTIVITY_AFTER_DEAUTH (1 * 5)


struct bss_data;

int sta_for_each(struct bss_data *bss,
		    int (*cb)(struct bss_data *bss, struct sta_info *sta,
			      void *ctx),
		    void *ctx);
struct sta_info * sta_get(struct bss_data *bss, const uint8_t *sta);
void sta_hash_add(struct bss_data *bss, struct sta_info *sta);
void sta_free(struct bss_data *bss, struct sta_info *sta);
void sta_session_timeout(struct bss_data *bss, struct sta_info *sta,
			    uint32_t session_timeout);
void sta_no_session_timeout(struct bss_data *bss,
			       struct sta_info *sta);
struct sta_info * sta_add(struct bss_data *bss, const uint8_t *addr);
void sta_disassociate(struct bss_data *bss, struct sta_info *sta,
			 uint16_t reason);
void sta_deauthenticate(struct bss_data *bss, struct sta_info *sta,
			   uint16_t reason);
int sta_bind_vlan(struct bss_data *bss, struct sta_info *sta,
		     int old_vlanid);
void sta_disconnect(struct bss_data *bss, struct sta_info *sta,
		       const uint8_t *addr, uint16_t reason);

#endif /* STA_INFO_H */
