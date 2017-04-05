#include "common/os.h"
#include "common/eapol_common.h"
#include "driver/driver.h"
#include "conf.h"
#include "iface.h"
#include "crypto/md5.h"
#include "crypto/crypto.h"
#include "radius/radius.h"
#include "radius/radius_client.h"
#include "eap_server/eap.h"
#include "eapol_auth/eapol_auth_sm.h"
#include "eapol_auth/eapol_auth_sm_i.h"
#include "sta_info.h"
#include "ieee802_1x.h"


#ifndef CONFIG_NO_RADIUS
static void ieee802_1x_learn_identity(struct bss_data *bss,
				      struct eapol_state_machine *sm,
				      const uint8_t *eap, size_t len)
{
	const uint8_t *identity;
	size_t identity_len;

	if (len <= sizeof(struct eap_hdr) ||
	    eap[sizeof(struct eap_hdr)] != EAP_TYPE_IDENTITY)
		return;

	identity = eap_get_identity(sm->eap, &identity_len);
	if (identity == NULL)
		return;

	/* Save station identity for future RADIUS packets */
	free(sm->identity);
	sm->identity = malloc(identity_len + 1);
	if (sm->identity == NULL) {
		sm->identity_len = 0;
		return;
	}

	memcpy(sm->identity, identity, identity_len);
	sm->identity_len = identity_len;
	sm->identity[identity_len] = '\0';
	sm->dot1xAuthEapolRespIdFramesRx++;
}

static void ieee802_1x_encapsulate_radius(struct bss_data *bss,
					  struct sta_info *sta,
					  const uint8_t *eap, size_t len)
{
	struct radius_msg *msg;
	char buf[128];
	struct eapol_state_machine *sm = sta->eapol_sm;

	if (sm == NULL)
		return;

	ieee802_1x_learn_identity(bss, sm, eap, len);

	LOG_D("Encapsulating EAP message into a RADIUS packet");

	sm->radius_identifier = radius_client_get_id(bss->radius);
	msg = radius_msg_new(RADIUS_CODE_ACCESS_REQUEST,
			     sm->radius_identifier);
	if (msg == NULL) 
    {
		LOG_ERROR("Could not create net RADIUS packet\n");
		return;
	}

	radius_msg_make_authenticator(msg, (uint8_t *) sta, sizeof(*sta));

	if (sm->identity &&
	    !radius_msg_add_attr(msg, RADIUS_ATTR_USER_NAME,
				 sm->identity, sm->identity_len)) 
    {
		LOG_ERROR("Could not add User-Name\n");
		goto fail;
	}

    struct iface_conf *conf = BSS_GET_CONF(bss);
	if (conf->own_ip_addr.af == AF_INET &&
	    !radius_msg_add_attr(msg, RADIUS_ATTR_NAS_IP_ADDRESS,
				 (uint8_t *) &conf->own_ip_addr.u.v4, 4)) 
    {
		LOG_ERROR("Could not add NAS-IP-Address\n");
		goto fail;
	}


	if (conf->nas_identifier &&
	    !radius_msg_add_attr(msg, RADIUS_ATTR_NAS_IDENTIFIER,
				 (uint8_t *) conf->nas_identifier,
				 strlen(conf->nas_identifier))) 
    {
		LOG_ERROR("Could not add NAS-Identifier\n");
		goto fail;
	}


	snprintf(buf, sizeof(buf), RADIUS_802_1X_ADDR_FORMAT,
		    MAC2STR(sta->addr));
	buf[sizeof(buf) - 1] = '\0';
	if (!radius_msg_add_attr(msg, RADIUS_ATTR_CALLING_STATION_ID,
				 (uint8_t *) buf, strlen(buf))) 
    {
		LOG_ERROR("Could not add Calling-Station-Id\n");
		goto fail;
	}

	/* TODO: should probably check MTU from driver config; 2304 is max for
	 * IEEE 802.11, but use 1400 to avoid problems with too large packets
	 */
	if (!radius_msg_add_attr_int32(msg, RADIUS_ATTR_FRAMED_MTU, 1400)) 
    {
		LOG_ERROR("Could not add Framed-MTU\n");
		goto fail;
	}

	if (!radius_msg_add_attr_int32(msg, RADIUS_ATTR_NAS_PORT_TYPE,
				       RADIUS_NAS_PORT_TYPE_IEEE_802_11)) 
    {
		LOG_ERROR("Could not add NAS-Port-Type\n");
		goto fail;
	}

	if (eap && !radius_msg_add_eap(msg, eap, len)) 
    {
		LOG_ERROR("Could not add EAP-Message\n");
		goto fail;
	}

	/* State attribute must be copied if and only if this packet is
	 * Access-Request reply to the previous Access-Challenge */
	if (sm->last_recv_radius &&
	    radius_msg_get_hdr(sm->last_recv_radius)->code ==
	    RADIUS_CODE_ACCESS_CHALLENGE) 
    {
		int res = radius_msg_copy_attr(msg, sm->last_recv_radius,
					       RADIUS_ATTR_STATE);
		if (res < 0) 
        {
			LOG_ERROR("Could not copy State attribute from previous "
			       "Access-Challenge\n");
			goto fail;
		}
		if (res > 0) 
        {
			LOG_D("Copied RADIUS State Attribute");
		}
	}

	if(radius_client_send(bss->radius, msg, RADIUS_AUTH, sta->addr))
    {
        LOG_ERROR("client send radius packet error");
        goto fail;
    }

	return;

 fail:
	radius_msg_free(msg);
}
#endif /* CONFIG_NO_RADIUS */

static void handle_eap_response(struct bss_data *bss,
				struct sta_info *sta, struct eap_hdr *eap,
				size_t len)
{
	uint8_t type, *data;
	struct eapol_state_machine *sm = sta->eapol_sm;

	data = (uint8_t *) (eap + 1);

	if (len < sizeof(struct eap_hdr) + 1) 
    {
		LOG_ERROR("handle_eap_response: too short response data\n");
		return;
	}

	sm->eap_type_supp = type = data[0];
    LOG_D("received EAP packet (code=%d id=%d len=%d): EAP Response-%s (%d)",
          eap->code, eap->identifier, be_to_host16(eap->length),eap_server_get_name(0, type), type);

	sm->dot1xAuthEapolRespFramesRx++;

	wpabuf_free(sm->eap_if->eapRespData);
	sm->eap_if->eapRespData = wpabuf_alloc_copy(eap, len);
    if(sm->eap_if->eapRespData == NULL)
    {
        LOG_ERROR("alloc wpabuf for eap-resp data error");
        return;
    }
	    
    sm->eapolEap = TRUE;
}


/* Process incoming EAP packet from Supplicant */
static void handle_eap(struct bss_data *bss, struct sta_info *sta,
		       uint8_t *buf, size_t len)
{
	struct eap_hdr *eap;
	uint16_t eap_len;

	if (len < sizeof(struct eap_hdr)) 
    {
		LOG_ERROR("too short EAP packet\n");
		return;
	}

	eap = (struct eap_hdr *) buf;

	eap_len = be_to_host16(eap->length);
	if (eap_len < sizeof(struct eap_hdr)) 
    {
		LOG_ERROR("Invalid EAP length");
		return;
	} 
    else if (eap_len > len) 
    {
		LOG_ERROR("Too short frame to contain this EAP packet");
		return;
	} 
    else if (eap_len < len) 
		LOG_D("Ignoring %lu extra bytes after EAP packet", (unsigned long) len - eap_len);

	switch (eap->code) 
    {
	case EAP_CODE_REQUEST:
		LOG_ERROR(" EAP-request");
		return;
	case EAP_CODE_RESPONSE:
		LOG_NORMAL(" EAP-response");
		handle_eap_response(bss, sta, eap, eap_len);
		break;
	case EAP_CODE_SUCCESS:
		LOG_ERROR(" EAP-success");
		return;
	case EAP_CODE_FAILURE:
		LOG_ERROR(" EAP-failure");
		return;
	default:
		LOG_ERROR(" (unknown code)");
		return;
	}
}

/**
 * ieee802_1x_receive - Process the EAPOL frames from the Supplicant
 * @buf: EAPOL frame
 * @len: Length of buf in octets
 *
 * This function is called for each incoming EAPOL frame from the interface
 */
void ieee802_1x_receive(struct bss_data *bss, struct sta_info *sta, const uint8_t *buf,
			size_t len)
{
	struct ieee802_1x_hdr *hdr;
	uint16_t datalen;

	LOG_D("IEEE 802.1X: %lu bytes from " MACSTR,(unsigned long) len, MAC2STR(sta->addr));

	if (len < sizeof(struct ieee802_1x_hdr)) 
    {
		LOG_ERROR("too short IEEE 802.1X packet\n");
		return;
	}

	hdr = (struct ieee802_1x_hdr *) buf;
	datalen = be_to_host16(hdr->length);
	LOG_D("IEEE 802.1X: version=%d type=%d length=%d",hdr->version, hdr->type, datalen);

	if (len - sizeof(struct ieee802_1x_hdr) < datalen) 
    {
		LOG_ERROR("frame too short for this IEEE 802.1X packet\n");
		if (sta->eapol_sm)
			sta->eapol_sm->dot1xAuthEapLengthErrorFramesRx++;
		return;
	}
	if (len - sizeof(*hdr) > datalen) 
		LOG_D("ignoring %lu extra octets after IEEE 802.1X packet",
			   (unsigned long) len - sizeof(*hdr) - datalen);

	if (sta->eapol_sm) 
    {
		sta->eapol_sm->dot1xAuthLastEapolFrameVersion = hdr->version;
		sta->eapol_sm->dot1xAuthEapolFramesRx++;
	}
    else 
    {
		sta->eapol_sm = eapol_auth_alloc(bss->eapol_auth, sta->addr, sta);
		if (sta->eapol_sm == NULL)
        {
            LOG_ERROR("alloc state machine manage structure error");
			return;
        }

		sta->eapol_sm->eap_if->portEnabled = TRUE;
	}

	switch (hdr->type) 
    {
	case IEEE802_1X_TYPE_EAP_PACKET:
		handle_eap(bss, sta, (uint8_t *) (hdr + 1), datalen);
		break;

	case IEEE802_1X_TYPE_EAPOL_START:
		LOG_NORMAL("EAPOL-Start packet");
		sta->eapol_sm->flags &= ~EAPOL_SM_WAIT_START;
		sta->eapol_sm->eapolStart = TRUE;
		sta->eapol_sm->dot1xAuthEapolStartFramesRx++;
		break;

	case IEEE802_1X_TYPE_EAPOL_LOGOFF:
		sta->acct_terminate_cause =
			RADIUS_ACCT_TERMINATE_CAUSE_USER_REQUEST;
		// accounting_sta_stop(bss, sta);
		sta->eapol_sm->eapolLogoff = TRUE;
		sta->eapol_sm->dot1xAuthEapolLogoffFramesRx++;
		break;

	case IEEE802_1X_TYPE_EAPOL_KEY:
		LOG_W("EAPOL-Key packet");
		break;

	case IEEE802_1X_TYPE_EAPOL_ENCAPSULATED_ASF_ALERT:
		LOG_W("EAPOL-Encapsulated-ASF-Alert packet");
		/* TODO: implement support for this; show data */
		break;

	default:
		LOG_ERROR("unknown IEEE 802.1X packet type");
		sta->eapol_sm->dot1xAuthInvalidEapolFramesRx++;
		break;
	}

	eapol_auth_step(sta->eapol_sm);
}

void ieee802_1x_free_station(struct sta_info *sta)
{
	struct eapol_state_machine *sm = sta->eapol_sm;

	if (sm == NULL)
		return;

	sta->eapol_sm = NULL;

#ifndef CONFIG_NO_RADIUS
	radius_msg_free(sm->last_recv_radius);
	radius_free_class(&sm->radius_class);
#endif /* CONFIG_NO_RADIUS */

	free(sm->identity);
	eapol_auth_free(sm);
}


#ifndef CONFIG_NO_RADIUS
static void ieee802_1x_decapsulate_radius(struct bss_data *bss,
					  struct sta_info *sta)
{
	uint8_t *eap;
	size_t len;
	struct eap_hdr *hdr;
	int eap_type = -1;
	char buf[64];
	struct radius_msg *msg;
	struct eapol_state_machine *sm = sta->eapol_sm;

	if (sm == NULL || sm->last_recv_radius == NULL) 
    {
        LOG_ERROR("SM == NULL or last recv radius == NULL");
		if (sm)
			sm->eap_if->aaaEapNoReq = TRUE;
		return;
	}

	msg = sm->last_recv_radius;

	eap = radius_msg_get_eap(msg, &len);
	if (eap == NULL) 
    {
        LOG_ERROR("no EAP-Message in this radius packet");
		/* RFC 3579, Chap. 2.6.3:
		 * RADIUS server SHOULD NOT send Access-Reject/no EAP-Message
		 * attribute */
		sm->eap_if->aaaEapNoReq = TRUE;
		return;
	}

	if (len < sizeof(*hdr)) 
    {
        LOG_ERROR("too short for this radius packet");
		free(eap);
		sm->eap_if->aaaEapNoReq = TRUE;
		return;
	}

	if (len > sizeof(*hdr))
		eap_type = eap[sizeof(*hdr)];

	hdr = (struct eap_hdr *) eap;
	switch (hdr->code) 
    {
	case EAP_CODE_REQUEST:
		if (eap_type >= 0)
			sm->eap_type_authsrv = eap_type;
		snprintf(buf, sizeof(buf), "EAP-Request-%s (%d)",
			    eap_type >= 0 ? eap_server_get_name(0, eap_type) :
			    "??",
			    eap_type);
		break;
	case EAP_CODE_RESPONSE:
		snprintf(buf, sizeof(buf), "EAP Response-%s (%d)",
			    eap_type >= 0 ? eap_server_get_name(0, eap_type) :
			    "??",
			    eap_type);
		break;
	case EAP_CODE_SUCCESS:
		strlcpy(buf, "EAP Success", sizeof(buf));
		break;
	case EAP_CODE_FAILURE:
		strlcpy(buf, "EAP Failure", sizeof(buf));
		break;
	default:
		strlcpy(buf, "unknown EAP code", sizeof(buf));
		break;
	}
	buf[sizeof(buf) - 1] = '\0';
    LOG_D("decapsulated EAP packet (code=%d id=%d len=%d) from RADIUS server: %s",hdr->code, hdr->identifier, be_to_host16(hdr->length),buf);
	sm->eap_if->aaaEapReq = TRUE;

	wpabuf_free(sm->eap_if->aaaEapReqData);
	sm->eap_if->aaaEapReqData = wpabuf_alloc_ext_data(eap, len);
}


static void ieee802_1x_store_radius_class(struct bss_data *bss,
					  struct sta_info *sta,
					  struct radius_msg *msg)
{
	uint8_t *class;
	size_t class_len;
	struct eapol_state_machine *sm = sta->eapol_sm;
	int count, i;
	struct radius_attr_data *nclass;
	size_t nclass_count;

	if (bss->radius == NULL ||
	    sm == NULL)
		return;

	radius_free_class(&sm->radius_class);
	count = radius_msg_count_attr(msg, RADIUS_ATTR_CLASS, 1);
	if (count <= 0)
		return;

	nclass = calloc(1,count * sizeof(struct radius_attr_data));
	if (nclass == NULL)
		return;

	nclass_count = 0;

	class = NULL;
	for (i = 0; i < count; i++) {
		do {
			if (radius_msg_get_attr_ptr(msg, RADIUS_ATTR_CLASS,
						    &class, &class_len,
						    class) < 0) {
				i = count;
				break;
			}
		} while (class_len < 1);

		nclass[nclass_count].data = malloc(class_len);
		if (nclass[nclass_count].data == NULL)
			break;

		memcpy(nclass[nclass_count].data, class, class_len);
		nclass[nclass_count].len = class_len;
		nclass_count++;
	}

	sm->radius_class.attr = nclass;
	sm->radius_class.count = nclass_count;
	LOG_D("IEEE 802.1X: Stored %lu RADIUS Class "
		   "attributes for " MACSTR,
		   (unsigned long) sm->radius_class.count,
		   MAC2STR(sta->addr));
}

/* Update sta->identity based on User-Name attribute in Access-Accept */
static void ieee802_1x_update_sta_identity(struct bss_data *bss,
					   struct sta_info *sta,
					   struct radius_msg *msg)
{
	uint8_t *buf, *identity;
	size_t len;
	struct eapol_state_machine *sm = sta->eapol_sm;

	if (sm == NULL)
		return;

	if (radius_msg_get_attr_ptr(msg, RADIUS_ATTR_USER_NAME, &buf, &len,
				    NULL) < 0)
		return;

	identity = malloc(len + 1);
	if (identity == NULL)
		return;

	memcpy(identity, buf, len);
	identity[len] = '\0';

    LOG_D("old identity '%s' updated with User-Name from Access-Accept '%s'",sm->identity ? (char *) sm->identity : "N/A",(char *) identity);
	free(sm->identity);
	sm->identity = identity;
	sm->identity_len = len;
}

struct sta_id_search {
	uint8_t identifier;
	struct eapol_state_machine *sm;
};

static int ieee802_1x_select_radius_identifier(struct bss_data *bss,
					       struct sta_info *sta,
					       void *ctx)
{
	struct sta_id_search *id_search = ctx;
	struct eapol_state_machine *sm = sta->eapol_sm;

	if (sm && sm->radius_identifier >= 0 &&
	    sm->radius_identifier == id_search->identifier) {
		id_search->sm = sm;
		return 1;
	}
	return 0;
}


static struct eapol_state_machine *
ieee802_1x_search_radius_identifier(struct bss_data *bss, uint8_t identifier)
{
	struct sta_id_search id_search;
	id_search.identifier = identifier;
	id_search.sm = NULL;
	sta_for_each(bss, ieee802_1x_select_radius_identifier, &id_search);
	return id_search.sm;
}


/**
 * receive RADIUS frames from Authentication Server
 * @msg: RADIUS response message
 * @req: RADIUS request message
 * @shared_secret: RADIUS shared secret
 * @shared_secret_len: Length of shared_secret in octets
 * @data: Context data (struct bss_data *)
 * Returns: Processing status
 */
static RadiusRxResult
ieee802_1x_receive_auth(struct radius_msg *msg, struct radius_msg *req,
			const uint8_t *shared_secret, size_t shared_secret_len,
			void *data)
{
	struct bss_data *bss = data;
	struct sta_info *sta;
	uint32_t session_timeout = 0, termination_action;
	int session_timeout_set;
	struct eapol_state_machine *sm;
	int override_eapReq = 0;
	struct radius_hdr *hdr = radius_msg_get_hdr(msg);

	sm = ieee802_1x_search_radius_identifier(bss, hdr->identifier);
	if (sm == NULL) 
    {
		LOG_ERROR("IEEE 802.1X: Could not find matching station for this RADIUS message");
		return RADIUS_RX_UNKNOWN;
	}
	sta = sm->sta;

	/* RFC 2869, Ch. 5.13: valid Message-Authenticator attribute MUST be
	 * present when packet contains an EAP-Message attribute */
	if (hdr->code == RADIUS_CODE_ACCESS_REJECT &&
	    radius_msg_get_attr(msg, RADIUS_ATTR_MESSAGE_AUTHENTICATOR, NULL,0) < 0 &&
	    radius_msg_get_attr(msg, RADIUS_ATTR_EAP_MESSAGE, NULL, 0) < 0) 
    {
		LOG_ERROR("Allowing RADIUS Access-Reject without Message-Authenticator since it does not include EAP-Message");
	} 
    else if (radius_msg_verify(msg, shared_secret, shared_secret_len,req, 1)) 
    {
		LOG_ERROR("Incoming RADIUS packet did not have correct Message-Authenticator - dropped\n");
		return RADIUS_RX_INVALID_AUTHENTICATOR;
	}

	if (hdr->code != RADIUS_CODE_ACCESS_ACCEPT &&
	    hdr->code != RADIUS_CODE_ACCESS_REJECT &&
	    hdr->code != RADIUS_CODE_ACCESS_CHALLENGE) {
		LOG_ERROR("Unknown RADIUS message code\n");
		return RADIUS_RX_UNKNOWN;
	}

	sm->radius_identifier = -1;
	LOG_D("RADIUS packet matching with station " MACSTR,MAC2STR(sta->addr));

	radius_msg_free(sm->last_recv_radius);
	sm->last_recv_radius = msg;

	session_timeout_set = !radius_msg_get_attr_int32(msg, RADIUS_ATTR_SESSION_TIMEOUT,&session_timeout);
	if (radius_msg_get_attr_int32(msg, RADIUS_ATTR_TERMINATION_ACTION,&termination_action))
		termination_action = RADIUS_TERMINATION_ACTION_DEFAULT;

	switch (hdr->code) 
    {
	case RADIUS_CODE_ACCESS_ACCEPT:
		/* RFC 3580, Ch. 3.17 */
		if (session_timeout_set && termination_action == RADIUS_TERMINATION_ACTION_RADIUS_REQUEST) 
			sm->reAuthPeriod = session_timeout;
		else if (session_timeout_set)
			sta_session_timeout(bss, sta, session_timeout);

		sm->eap_if->aaaSuccess = TRUE;
		override_eapReq = 1;
		ieee802_1x_store_radius_class(bss, sta, msg);
		ieee802_1x_update_sta_identity(bss, sta, msg);
		break;
	case RADIUS_CODE_ACCESS_REJECT:
		sm->eap_if->aaaFail = TRUE;
		override_eapReq = 1;
		break;
	case RADIUS_CODE_ACCESS_CHALLENGE:
		sm->eap_if->aaaEapReq = TRUE;
		if (session_timeout_set) 
			/* RFC 2869, Ch. 2.3.2; RFC 3580, Ch. 3.17 */
			sm->eap_if->aaaMethodTimeout = session_timeout;
        else 
        {
			/*
			 * Use dynamic retransmission behavior per EAP
			 * specification.
			 */
			sm->eap_if->aaaMethodTimeout = 0;
		}
		break;
	}

	ieee802_1x_decapsulate_radius(bss, sta);
	if (override_eapReq)
		sm->eap_if->aaaEapReq = FALSE;

	eapol_auth_step(sm);

	return RADIUS_RX_QUEUED;
}
#endif /* CONFIG_NO_RADIUS */

/* ieee802.1x send EAPOL packet 
 * @type    - MUST BE IEEE802_1X_TYPE_EAP_PACKET
 * @data    - EAP data
 * @datalen - EAP data len
 * */
static void ieee802_1x_eapol_send(void *ctx, void *sta_ctx, uint8_t type,
				  const uint8_t *data, size_t datalen)
{
    struct bss_data *bss = ctx;
    struct sta_info *sta = sta_ctx;
	uint8_t *buf;
	struct ieee802_1x_hdr *xhdr;
	size_t len;

	len = sizeof(struct ieee802_1x_hdr) + datalen;
	buf = calloc(1,len);
	if (buf == NULL) 
    {
		LOG_ERROR("alloc failed");
		return;
	}

	xhdr = (struct ieee802_1x_hdr *) buf;
	xhdr->version = BSS_GET_CONF(bss)->eapol_version;
	xhdr->type = type;
	xhdr->length = host_to_be16(datalen);

	if (datalen > 0 && data != NULL)
		memcpy(xhdr + 1, data, datalen);

    bss->driver->driver_send_eapol(bss->drv_priv,sta->addr,buf,len,bss->own_addr);

	free(buf);
}

/* ieee802.1x send radius packet base on AAA
 * @data    - EAP data
 * @datalen - EAP data len
 * */
static void ieee802_1x_aaa_send(void *ctx, void *sta_ctx,
				const uint8_t *data, size_t datalen)
{
	struct bss_data *bss = ctx;
	struct sta_info *sta = sta_ctx;

	ieee802_1x_encapsulate_radius(bss, sta, data, datalen);
}

/* ieee802.1x end authentication
 * 
 * note: auth success do nothing; auth failure remove sta
 * */
static void ieee802_1x_finished(void *ctx, void *sta_ctx, int success)
{
	struct bss_data *bss = ctx;
	struct sta_info *sta = sta_ctx;

    if(success)
    {
    }
    else
        sta_disconnect(bss, sta, sta->addr,23); 
}

#if 0
static int ieee802_1x_get_eap_user(void *ctx, const uint8_t *identity,
				   size_t identity_len, int phase2,
				   struct eap_user *user)
{
	struct bss_data *bss = ctx;
	const struct hostapd_eap_user *eap_user;
	int i, count;

	eap_user = hostapd_get_eap_user(bss->conf, identity,
					identity_len, phase2);
	if (eap_user == NULL)
		return -1;

	memset(user, 0, sizeof(*user));
	user->phase2 = phase2;
	count = EAP_USER_MAX_METHODS;
	if (count > EAP_MAX_METHODS)
		count = EAP_MAX_METHODS;
	for (i = 0; i < count; i++) {
		user->methods[i].vendor = eap_user->methods[i].vendor;
		user->methods[i].method = eap_user->methods[i].method;
	}

	if (eap_user->password) {
		user->password = malloc(eap_user->password_len);
		if (user->password == NULL)
			return -1;
		memcpy(user->password, eap_user->password,
			  eap_user->password_len);
		user->password_len = eap_user->password_len;
	}
	user->force_version = eap_user->force_version;
	user->ttls_auth = eap_user->ttls_auth;

	return 0;
}
#endif

/* ieee802.1x check if sta is alive
 * @addr - to be indexed
 * */
static int ieee802_1x_sta_entry_alive(void *ctx, const uint8_t *addr)
{
	struct bss_data *bss = ctx;
	struct sta_info *sta;
	sta = sta_get(bss, addr);

	return (sta == NULL || sta->eapol_sm == NULL)?0:1;
}

/* ieee802.1x set sta authorize/unauthorize
 * @authorized - 1:authorize; 0-unauthorize
 *
 * note: TODO accounting start if authorize
 */
static void ieee802_1x_set_port_authorized(void *ctx, void *sta_ctx,
					   int authorized)
{
	struct bss_data *bss = ctx;
	struct sta_info *sta = sta_ctx;
	int res;

    res = bss->driver->driver_set_authorized(bss->drv_priv,sta->addr,authorized);

	if (res && errno != ENOENT) 
    {
		LOG_ERROR("Could not set station " MACSTR " flags for kernel "
		       "driver (errno=%d).\n", MAC2STR(sta->addr), errno);
	}

	//if (authorized)
	//	accounting_sta_start(bss, sta);
}

/* ieee802.1x abort authenticate for a sta when authenticating
 * 
 * note: abort not means finish only get eapol-logoff
 */
static void ieee802_1x_abort_auth(void *ctx, void *sta_ctx)
{
	struct bss_data *bss = ctx;
	struct sta_info *sta = sta_ctx;
	struct eapol_state_machine *sm = sta->eapol_sm;
	if (sm == NULL)
		return;

#ifndef CONFIG_NO_RADIUS
	radius_msg_free(sm->last_recv_radius);
	sm->last_recv_radius = NULL;
#endif /* CONFIG_NO_RADIUS */

	if (sm->eap_if->eapTimeout) 
    {
		/*
		 * Disconnect the STA since it did not reply to the last EAP
		 * request and we cannot continue EAP processing (EAP-Failure
		 * could only be sent if the EAP peer actually replied).
		 */
		sm->eap_if->portEnabled = FALSE;
		sta_disconnect(bss, sta, sta->addr,23);
	}
}

/* 802.1x feature initialize for the interface
 * */
int ieee802_1x_init(struct bss_data *bss)
{
	struct eapol_auth_config eapol_conf;
	struct eapol_auth_cb cb;
    struct iface_conf *conf = BSS_GET_CONF(bss);

	memset(&eapol_conf, 0, sizeof(eapol_conf));
	eapol_conf.ctx = bss;
	eapol_conf.eap_reauth_period = conf->eap_reauth_period;
	eapol_conf.eap_server = conf->eap_server;
	eapol_conf.eap_req_id_text = conf->eap_req_id_text;
	eapol_conf.eap_req_id_text_len = conf->eap_req_id_text_len;

	memset(&cb, 0, sizeof(cb));
	cb.eapol_send = ieee802_1x_eapol_send;
	cb.aaa_send = ieee802_1x_aaa_send;
    cb.finished = ieee802_1x_finished;
	//cb.get_eap_user = ieee802_1x_get_eap_user;
	cb.sta_entry_alive = ieee802_1x_sta_entry_alive;
	cb.set_port_authorized = ieee802_1x_set_port_authorized;
	cb.abort_auth = ieee802_1x_abort_auth;

	bss->eapol_auth = eapol_auth_init(&eapol_conf, &cb);
	if (bss->eapol_auth == NULL)
		return -1;

	if (radius_client_register(bss->radius, RADIUS_AUTH,
				   ieee802_1x_receive_auth, bss))
		return -1;

	return 0;
}

/* disable ieee802.1x feature for the interface
 * 
 */
void ieee802_1x_deinit(struct bss_data *bss)
{
	eapol_auth_deinit(bss->eapol_auth);
	bss->eapol_auth = NULL;
}

/*  get user name info
 *
 * */
uint8_t * ieee802_1x_get_identity(struct eapol_state_machine *sm, size_t *len)
{
	if (sm == NULL || sm->identity == NULL)
		return NULL;

	*len = sm->identity_len;
	return sm->identity;
}

uint8_t * ieee802_1x_get_radius_class(struct eapol_state_machine *sm, size_t *len,
				 int idx)
{
	if (sm == NULL || sm->radius_class.attr == NULL ||
	    idx >= (int) sm->radius_class.count)
		return NULL;

	*len = sm->radius_class.attr[idx].len;
	return sm->radius_class.attr[idx].data;
}

