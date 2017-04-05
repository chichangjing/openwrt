#ifndef EAPOL_AUTH_SM_H
#define EAPOL_AUTH_SM_H

#define EAPOL_SM_PREAUTH BIT(0)
#define EAPOL_SM_WAIT_START BIT(1)
#define EAPOL_SM_USES_WPA BIT(2)
#define EAPOL_SM_FROM_PMKSA_CACHE BIT(3)

struct eapol_auth_config {
	int eap_reauth_period;
	int eap_server;
	char *eap_req_id_text; /* a copy of this will be allocated */
	size_t eap_req_id_text_len;

	void *ctx;  // point to struct bss_data
};

struct eap_user;

typedef enum {
	EAPOL_LOGGER_DEBUG, EAPOL_LOGGER_INFO, EAPOL_LOGGER_WARNING
} eapol_logger_level;

enum eapol_event {
	EAPOL_AUTH_SM_CHANGE,
	EAPOL_AUTH_REAUTHENTICATE
};

struct eapol_auth_cb {
	void (*eapol_send)(void *ctx, void *sta_ctx, uint8_t type, const uint8_t *data,
			   size_t datalen);
	void (*aaa_send)(void *ctx, void *sta_ctx, const uint8_t *data,
			 size_t datalen);
    void (*finished)(void *ctx, void *sta_ctx, int success);
	int (*get_eap_user)(void *ctx, const uint8_t *identity, size_t identity_len,
			    int phase2, struct eap_user *user);
	int (*sta_entry_alive)(void *ctx, const uint8_t *addr);
	void (*set_port_authorized)(void *ctx, void *sta_ctx, int authorized);
	void (*abort_auth)(void *ctx, void *sta_ctx);
};


struct eapol_authenticator * eapol_auth_init(struct eapol_auth_config *conf,
					     struct eapol_auth_cb *cb);
void eapol_auth_deinit(struct eapol_authenticator *eapol);
struct eapol_state_machine *
eapol_auth_alloc(struct eapol_authenticator *eapol, const uint8_t *addr,void *sta_ctx);
void eapol_auth_free(struct eapol_state_machine *sm);
void eapol_auth_step(struct eapol_state_machine *sm);
void eapol_auth_dump_state(FILE *f, const char *prefix,
			   struct eapol_state_machine *sm);
int eapol_auth_eap_pending_cb(struct eapol_state_machine *sm, void *ctx);

#endif /* EAPOL_AUTH_SM_H */
