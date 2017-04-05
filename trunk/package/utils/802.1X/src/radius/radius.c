#include "common/wpabuf.h"
#include "crypto/md5.h"
#include "crypto/crypto.h"
#include "radius.h"


/**
 * struct radius_msg - RADIUS message structure for new and parsed messages
 */
struct radius_msg {
	/**
	 * buf - Allocated buffer for RADIUS message
	 */
	struct wpabuf *buf;

	/**
	 * hdr - Pointer to the RADIUS header in buf
	 */
	struct radius_hdr *hdr;

	/**
	 * attr_pos - Array of indexes to attributes
	 *
	 * The values are number of bytes from buf to the beginning of
	 * struct radius_attr_hdr.
	 */
	size_t *attr_pos;

	/**
	 * attr_size - Total size of the attribute pointer array
	 */
	size_t attr_size;

	/**
	 * attr_used - Total number of attributes in the array
	 */
	size_t attr_used;
};


struct radius_hdr * radius_msg_get_hdr(struct radius_msg *msg)
{
	return msg->hdr;
}


struct wpabuf * radius_msg_get_buf(struct radius_msg *msg)
{
	return msg->buf;
}


static struct radius_attr_hdr *
radius_get_attr_hdr(struct radius_msg *msg, int idx)
{
	return (struct radius_attr_hdr *)
		(wpabuf_mhead_uint8_t(msg->buf) + msg->attr_pos[idx]);
}


static void radius_msg_set_hdr(struct radius_msg *msg, uint8_t code, uint8_t identifier)
{
	msg->hdr->code = code;
	msg->hdr->identifier = identifier;
}


static int radius_msg_initialize(struct radius_msg *msg)
{
	msg->attr_pos =
		calloc(1,RADIUS_DEFAULT_ATTR_COUNT * sizeof(*msg->attr_pos));
	if (msg->attr_pos == NULL)
		return -1;

	msg->attr_size = RADIUS_DEFAULT_ATTR_COUNT;
	msg->attr_used = 0;

	return 0;
}


/**
 * radius_msg_new - Create a new RADIUS message
 * @code: Code for RADIUS header
 * @identifier: Identifier for RADIUS header
 * Returns: Context for RADIUS message or %NULL on failure
 *
 * The caller is responsible for freeing the returned data with
 * radius_msg_free().
 */
struct radius_msg * radius_msg_new(uint8_t code, uint8_t identifier)
{
	struct radius_msg *msg;

	msg = calloc(1,sizeof(*msg));
	if (msg == NULL)
		return NULL;

	msg->buf = wpabuf_alloc(RADIUS_DEFAULT_MSG_SIZE);
	if (msg->buf == NULL || radius_msg_initialize(msg)) {
		radius_msg_free(msg);
		return NULL;
	}
	msg->hdr = wpabuf_put(msg->buf, sizeof(struct radius_hdr));

	radius_msg_set_hdr(msg, code, identifier);

	return msg;
}


/**
 * radius_msg_free - Free a RADIUS message
 * @msg: RADIUS message from radius_msg_new() or radius_msg_parse()
 */
void radius_msg_free(struct radius_msg *msg)
{
	if (msg == NULL)
		return;

	wpabuf_free(msg->buf);
	free(msg->attr_pos);
	free(msg);
}


static const char *radius_code_string(uint8_t code)
{
	switch (code) {
	case RADIUS_CODE_ACCESS_REQUEST: return "Access-Request";
	case RADIUS_CODE_ACCESS_ACCEPT: return "Access-Accept";
	case RADIUS_CODE_ACCESS_REJECT: return "Access-Reject";
	case RADIUS_CODE_ACCOUNTING_REQUEST: return "Accounting-Request";
	case RADIUS_CODE_ACCOUNTING_RESPONSE: return "Accounting-Response";
	case RADIUS_CODE_ACCESS_CHALLENGE: return "Access-Challenge";
	case RADIUS_CODE_STATUS_SERVER: return "Status-Server";
	case RADIUS_CODE_STATUS_CLIENT: return "Status-Client";
	case RADIUS_CODE_RESERVED: return "Reserved";
	default: return "?Unknown?";
	}
}


struct radius_attr_type {
	uint8_t type;
	char *name;
	enum {
		RADIUS_ATTR_UNDIST, RADIUS_ATTR_TEXT, RADIUS_ATTR_IP,
		RADIUS_ATTR_HEXDUMP, RADIUS_ATTR_INT32, RADIUS_ATTR_IPV6
	} data_type;
};

static struct radius_attr_type radius_attrs[] =
{
	{ RADIUS_ATTR_USER_NAME, "User-Name", RADIUS_ATTR_TEXT },
	{ RADIUS_ATTR_USER_PASSWORD, "User-Password", RADIUS_ATTR_UNDIST },
	{ RADIUS_ATTR_NAS_IP_ADDRESS, "NAS-IP-Address", RADIUS_ATTR_IP },
	{ RADIUS_ATTR_NAS_PORT, "NAS-Port", RADIUS_ATTR_INT32 },
	{ RADIUS_ATTR_FRAMED_MTU, "Framed-MTU", RADIUS_ATTR_INT32 },
	{ RADIUS_ATTR_REPLY_MESSAGE, "Reply-Message", RADIUS_ATTR_TEXT },
	{ RADIUS_ATTR_STATE, "State", RADIUS_ATTR_UNDIST },
	{ RADIUS_ATTR_CLASS, "Class", RADIUS_ATTR_UNDIST },
	{ RADIUS_ATTR_VENDOR_SPECIFIC, "Vendor-Specific", RADIUS_ATTR_UNDIST },
	{ RADIUS_ATTR_SESSION_TIMEOUT, "Session-Timeout", RADIUS_ATTR_INT32 },
	{ RADIUS_ATTR_IDLE_TIMEOUT, "Idle-Timeout", RADIUS_ATTR_INT32 },
	{ RADIUS_ATTR_TERMINATION_ACTION, "Termination-Action",
	  RADIUS_ATTR_INT32 },
	{ RADIUS_ATTR_CALLED_STATION_ID, "Called-Station-Id",
	  RADIUS_ATTR_TEXT },
	{ RADIUS_ATTR_CALLING_STATION_ID, "Calling-Station-Id",
	  RADIUS_ATTR_TEXT },
	{ RADIUS_ATTR_NAS_IDENTIFIER, "NAS-Identifier", RADIUS_ATTR_TEXT },
	{ RADIUS_ATTR_PROXY_STATE, "Proxy-State", RADIUS_ATTR_UNDIST },
	{ RADIUS_ATTR_ACCT_STATUS_TYPE, "Acct-Status-Type",
	  RADIUS_ATTR_INT32 },
	{ RADIUS_ATTR_ACCT_DELAY_TIME, "Acct-Delay-Time", RADIUS_ATTR_INT32 },
	{ RADIUS_ATTR_ACCT_INPUT_OCTETS, "Acct-Input-Octets",
	  RADIUS_ATTR_INT32 },
	{ RADIUS_ATTR_ACCT_OUTPUT_OCTETS, "Acct-Output-Octets",
	  RADIUS_ATTR_INT32 },
	{ RADIUS_ATTR_ACCT_SESSION_ID, "Acct-Session-Id", RADIUS_ATTR_TEXT },
	{ RADIUS_ATTR_ACCT_AUTHENTIC, "Acct-Authentic", RADIUS_ATTR_INT32 },
	{ RADIUS_ATTR_ACCT_SESSION_TIME, "Acct-Session-Time",
	  RADIUS_ATTR_INT32 },
	{ RADIUS_ATTR_ACCT_INPUT_PACKETS, "Acct-Input-Packets",
	  RADIUS_ATTR_INT32 },
	{ RADIUS_ATTR_ACCT_OUTPUT_PACKETS, "Acct-Output-Packets",
	  RADIUS_ATTR_INT32 },
	{ RADIUS_ATTR_ACCT_TERMINATE_CAUSE, "Acct-Terminate-Cause",
	  RADIUS_ATTR_INT32 },
	{ RADIUS_ATTR_ACCT_MULTI_SESSION_ID, "Acct-Multi-Session-Id",
	  RADIUS_ATTR_TEXT },
	{ RADIUS_ATTR_ACCT_LINK_COUNT, "Acct-Link-Count", RADIUS_ATTR_INT32 },
	{ RADIUS_ATTR_ACCT_INPUT_GIGAWORDS, "Acct-Input-Gigawords", 
	  RADIUS_ATTR_INT32 },
	{ RADIUS_ATTR_ACCT_OUTPUT_GIGAWORDS, "Acct-Output-Gigawords",
	  RADIUS_ATTR_INT32 },
	{ RADIUS_ATTR_EVENT_TIMESTAMP, "Event-Timestamp",
	  RADIUS_ATTR_INT32 },
	{ RADIUS_ATTR_NAS_PORT_TYPE, "NAS-Port-Type", RADIUS_ATTR_INT32 },
	{ RADIUS_ATTR_TUNNEL_TYPE, "Tunnel-Type", RADIUS_ATTR_HEXDUMP },
	{ RADIUS_ATTR_TUNNEL_MEDIUM_TYPE, "Tunnel-Medium-Type",
	  RADIUS_ATTR_HEXDUMP },
	{ RADIUS_ATTR_CONNECT_INFO, "Connect-Info", RADIUS_ATTR_TEXT },
	{ RADIUS_ATTR_EAP_MESSAGE, "EAP-Message", RADIUS_ATTR_UNDIST },
	{ RADIUS_ATTR_MESSAGE_AUTHENTICATOR, "Message-Authenticator",
	  RADIUS_ATTR_UNDIST },
	{ RADIUS_ATTR_TUNNEL_PRIVATE_GROUP_ID, "Tunnel-Private-Group-Id",
	  RADIUS_ATTR_HEXDUMP },
	{ RADIUS_ATTR_ACCT_INTERIM_INTERVAL, "Acct-Interim-Interval",
	  RADIUS_ATTR_INT32 },
	{ RADIUS_ATTR_CHARGEABLE_USER_IDENTITY, "Chargable-User-Identity",
	  RADIUS_ATTR_TEXT },
	{ RADIUS_ATTR_NAS_IPV6_ADDRESS, "NAS-IPv6-Address", RADIUS_ATTR_IPV6 },
};
#define RADIUS_ATTRS (sizeof(radius_attrs) / sizeof(radius_attrs[0]))


static struct radius_attr_type *radius_get_attr_type(uint8_t type)
{
	size_t i;

	for (i = 0; i < RADIUS_ATTRS; i++) {
		if (type == radius_attrs[i].type)
			return &radius_attrs[i];
	}

	return NULL;
}


static void print_char(char c)
{
	if (c >= 32 && c < 127)
		printf("%c", c);
	else
		printf("<%02x>", c);
}


static void radius_msg_dump_attr(struct radius_attr_hdr *hdr)
{
	struct radius_attr_type *attr;
	int i, len;
	unsigned char *pos;

	attr = radius_get_attr_type(hdr->type);

	printf("   Attribute %d (%s) length=%d\n",
	       hdr->type, attr ? attr->name : "?Unknown?", hdr->length);

	if (attr == NULL)
		return;

	len = hdr->length - sizeof(struct radius_attr_hdr);
	pos = (unsigned char *) (hdr + 1);

	switch (attr->data_type) {
	case RADIUS_ATTR_TEXT:
		printf("      Value: '");
		for (i = 0; i < len; i++)
			print_char(pos[i]);
		printf("'\n");
		break;

	case RADIUS_ATTR_IP:
		if (len == 4) {
			struct in_addr addr;
			memcpy(&addr, pos, 4);
			printf("      Value: %s\n", inet_ntoa(addr));
		} else
			printf("      Invalid IP address length %d\n", len);
		break;

#ifdef CONFIG_IPV6
	case RADIUS_ATTR_IPV6:
		if (len == 16) {
			char buf[128];
			const char *atxt;
			struct in6_addr *addr = (struct in6_addr *) pos;
			atxt = inet_ntop(AF_INET6, addr, buf, sizeof(buf));
			printf("      Value: %s\n", atxt ? atxt : "?");
		} else
			printf("      Invalid IPv6 address length %d\n", len);
		break;
#endif /* CONFIG_IPV6 */

	case RADIUS_ATTR_HEXDUMP:
	case RADIUS_ATTR_UNDIST:
		printf("      Value:");
		for (i = 0; i < len; i++)
			printf(" %02x", pos[i]);
		printf("\n");
		break;

	case RADIUS_ATTR_INT32:
		if (len == 4)
			printf("      Value: %u\n", WPA_GET_BE32(pos));
		else
			printf("      Invalid INT32 length %d\n", len);
		break;

	default:
		break;
	}
}


void radius_msg_dump(struct radius_msg *msg)
{
	size_t i;

	LOG_D("RADIUS message: code=%d (%s) identifier=%d length=%d\n",
	       msg->hdr->code, radius_code_string(msg->hdr->code),
	       msg->hdr->identifier, ntohs(msg->hdr->length));

	for (i = 0; i < msg->attr_used; i++) {
		struct radius_attr_hdr *attr = radius_get_attr_hdr(msg, i);
		radius_msg_dump_attr(attr);
	}
}


int radius_msg_finish(struct radius_msg *msg, const uint8_t *secret,
		      size_t secret_len)
{
	if (secret) {
		uint8_t auth[MD5_MAC_LEN];
		struct radius_attr_hdr *attr;

		memset(auth, 0, MD5_MAC_LEN);
		attr = radius_msg_add_attr(msg,
					   RADIUS_ATTR_MESSAGE_AUTHENTICATOR,
					   auth, MD5_MAC_LEN);
		if (attr == NULL) {
			LOG_W("RADIUS: Could not add "
				   "Message-Authenticator");
			return -1;
		}
		msg->hdr->length = htons(wpabuf_len(msg->buf));
		hmac_md5(secret, secret_len, wpabuf_head(msg->buf),
			 wpabuf_len(msg->buf), (uint8_t *) (attr + 1));
	} else
		msg->hdr->length = htons(wpabuf_len(msg->buf));

	if (wpabuf_len(msg->buf) > 0xffff) {
		LOG_W("RADIUS: Too long message (%lu)",
			   (unsigned long) wpabuf_len(msg->buf));
		return -1;
	}
	return 0;
}


int radius_msg_finish_srv(struct radius_msg *msg, const uint8_t *secret,
			  size_t secret_len, const uint8_t *req_authenticator)
{
	uint8_t auth[MD5_MAC_LEN];
	struct radius_attr_hdr *attr;
	const uint8_t *addr[4];
	size_t len[4];

	memset(auth, 0, MD5_MAC_LEN);
	attr = radius_msg_add_attr(msg, RADIUS_ATTR_MESSAGE_AUTHENTICATOR,
				   auth, MD5_MAC_LEN);
	if (attr == NULL) {
		printf("WARNING: Could not add Message-Authenticator\n");
		return -1;
	}
	msg->hdr->length = htons(wpabuf_len(msg->buf));
	memcpy(msg->hdr->authenticator, req_authenticator,
		  sizeof(msg->hdr->authenticator));
	hmac_md5(secret, secret_len, wpabuf_head(msg->buf),
		 wpabuf_len(msg->buf), (uint8_t *) (attr + 1));

	/* ResponseAuth = MD5(Code+ID+Length+RequestAuth+Attributes+Secret) */
	addr[0] = (uint8_t *) msg->hdr;
	len[0] = 1 + 1 + 2;
	addr[1] = req_authenticator;
	len[1] = MD5_MAC_LEN;
	addr[2] = wpabuf_head_uint8_t(msg->buf) + sizeof(struct radius_hdr);
	len[2] = wpabuf_len(msg->buf) - sizeof(struct radius_hdr);
	addr[3] = secret;
	len[3] = secret_len;
	md5_vector(4, addr, len, msg->hdr->authenticator);

	if (wpabuf_len(msg->buf) > 0xffff) {
		LOG_W("RADIUS: Too long message (%lu)",
			   (unsigned long) wpabuf_len(msg->buf));
		return -1;
	}
	return 0;
}


void radius_msg_finish_acct(struct radius_msg *msg, const uint8_t *secret,
			    size_t secret_len)
{
	const uint8_t *addr[2];
	size_t len[2];

	msg->hdr->length = htons(wpabuf_len(msg->buf));
	memset(msg->hdr->authenticator, 0, MD5_MAC_LEN);
	addr[0] = wpabuf_head(msg->buf);
	len[0] = wpabuf_len(msg->buf);
	addr[1] = secret;
	len[1] = secret_len;
	md5_vector(2, addr, len, msg->hdr->authenticator);

	if (wpabuf_len(msg->buf) > 0xffff) {
		LOG_W("RADIUS: Too long messages (%lu)",
			   (unsigned long) wpabuf_len(msg->buf));
	}
}


static int radius_msg_add_attr_to_array(struct radius_msg *msg,
					struct radius_attr_hdr *attr)
{
	if (msg->attr_used >= msg->attr_size) {
		size_t *nattr_pos;
		int nlen = msg->attr_size * 2;

		nattr_pos = realloc(msg->attr_pos,
				       nlen * sizeof(*msg->attr_pos));
		if (nattr_pos == NULL)
			return -1;

		msg->attr_pos = nattr_pos;
		msg->attr_size = nlen;
	}

	msg->attr_pos[msg->attr_used++] =
		(unsigned char *) attr - wpabuf_head_uint8_t(msg->buf);

	return 0;
}


struct radius_attr_hdr *radius_msg_add_attr(struct radius_msg *msg, uint8_t type,
					    const uint8_t *data, size_t data_len)
{
	size_t buf_needed;
	struct radius_attr_hdr *attr;

	if (data_len > RADIUS_MAX_ATTR_LEN) {
		printf("radius_msg_add_attr: too long attribute (%lu bytes)\n",
		       (unsigned long) data_len);
		return NULL;
	}

	buf_needed = sizeof(*attr) + data_len;

	if (wpabuf_tailroom(msg->buf) < buf_needed) {
		/* allocate more space for message buffer */
		if (wpabuf_resize(&msg->buf, buf_needed) < 0)
			return NULL;
		msg->hdr = wpabuf_mhead(msg->buf);
	}

	attr = wpabuf_put(msg->buf, sizeof(struct radius_attr_hdr));
	attr->type = type;
	attr->length = sizeof(*attr) + data_len;
	wpabuf_put_data(msg->buf, data, data_len);

	if (radius_msg_add_attr_to_array(msg, attr))
		return NULL;

	return attr;
}


/**
 * radius_msg_parse - Parse a RADIUS message
 * @data: RADIUS message to be parsed
 * @len: Length of data buffer in octets
 * Returns: Parsed RADIUS message or %NULL on failure
 *
 * This parses a RADIUS message and makes a copy of its data. The caller is
 * responsible for freeing the returned data with radius_msg_free().
 */
struct radius_msg * radius_msg_parse(const uint8_t *data, size_t len)
{
	struct radius_msg *msg;
	struct radius_hdr *hdr;
	struct radius_attr_hdr *attr;
	size_t msg_len;
	unsigned char *pos, *end;

	if (data == NULL || len < sizeof(*hdr))
		return NULL;

	hdr = (struct radius_hdr *) data;

	msg_len = ntohs(hdr->length);
	if (msg_len < sizeof(*hdr) || msg_len > len) 
    {
		LOG_ERROR("RADIUS: Invalid message length");
		return NULL;
	}

	if (msg_len < len) 
		LOG_W("RADIUS: Ignored %lu extra bytes after RADIUS message", (unsigned long) len - msg_len);

	msg = calloc(1,sizeof(*msg));
	if (msg == NULL)
		return NULL;

	msg->buf = wpabuf_alloc_copy(data, msg_len);
	if (msg->buf == NULL || radius_msg_initialize(msg)) 
    {
		radius_msg_free(msg);
		return NULL;
	}
	msg->hdr = wpabuf_mhead(msg->buf);

	/* parse attributes */
	pos = wpabuf_mhead_uint8_t(msg->buf) + sizeof(struct radius_hdr);
	end = wpabuf_mhead_uint8_t(msg->buf) + wpabuf_len(msg->buf);
	while (pos < end) 
    {
		if ((size_t) (end - pos) < sizeof(*attr))
			goto fail;

		attr = (struct radius_attr_hdr *) pos;

		if (pos + attr->length > end || attr->length < sizeof(*attr))
			goto fail;

		/* TODO: check that attr->length is suitable for attr->type */

		if (radius_msg_add_attr_to_array(msg, attr))
			goto fail;

		pos += attr->length;
	}

	return msg;

 fail:
	radius_msg_free(msg);
	return NULL;
}


int radius_msg_add_eap(struct radius_msg *msg, const uint8_t *data, size_t data_len)
{
	const uint8_t *pos = data;
	size_t left = data_len;

	while (left > 0) {
		int len;
		if (left > RADIUS_MAX_ATTR_LEN)
			len = RADIUS_MAX_ATTR_LEN;
		else
			len = left;

		if (!radius_msg_add_attr(msg, RADIUS_ATTR_EAP_MESSAGE,
					 pos, len))
			return 0;

		pos += len;
		left -= len;
	}

	return 1;
}


uint8_t *radius_msg_get_eap(struct radius_msg *msg, size_t *eap_len)
{
	uint8_t *eap, *pos;
	size_t len, i;
	struct radius_attr_hdr *attr;

	if (msg == NULL)
		return NULL;

	len = 0;
	for (i = 0; i < msg->attr_used; i++) {
		attr = radius_get_attr_hdr(msg, i);
		if (attr->type == RADIUS_ATTR_EAP_MESSAGE)
			len += attr->length - sizeof(struct radius_attr_hdr);
	}

	if (len == 0)
		return NULL;

	eap = malloc(len);
	if (eap == NULL)
		return NULL;

	pos = eap;
	for (i = 0; i < msg->attr_used; i++) {
		attr = radius_get_attr_hdr(msg, i);
		if (attr->type == RADIUS_ATTR_EAP_MESSAGE) {
			int flen = attr->length - sizeof(*attr);
			memcpy(pos, attr + 1, flen);
			pos += flen;
		}
	}

	if (eap_len)
		*eap_len = len;

	return eap;
}


int radius_msg_verify_msg_auth(struct radius_msg *msg, const uint8_t *secret,
			       size_t secret_len, const uint8_t *req_auth)
{
	uint8_t auth[MD5_MAC_LEN], orig[MD5_MAC_LEN];
	uint8_t orig_authenticator[16];
	struct radius_attr_hdr *attr = NULL, *tmp;
	size_t i;

	for (i = 0; i < msg->attr_used; i++) {
		tmp = radius_get_attr_hdr(msg, i);
		if (tmp->type == RADIUS_ATTR_MESSAGE_AUTHENTICATOR) {
			if (attr != NULL) {
				printf("Multiple Message-Authenticator "
				       "attributes in RADIUS message\n");
				return 1;
			}
			attr = tmp;
		}
	}

	if (attr == NULL) {
		printf("No Message-Authenticator attribute found\n");
		return 1;
	}

	memcpy(orig, attr + 1, MD5_MAC_LEN);
	memset(attr + 1, 0, MD5_MAC_LEN);
	if (req_auth) {
		memcpy(orig_authenticator, msg->hdr->authenticator,
			  sizeof(orig_authenticator));
		memcpy(msg->hdr->authenticator, req_auth,
			  sizeof(msg->hdr->authenticator));
	}
	hmac_md5(secret, secret_len, wpabuf_head(msg->buf),
		 wpabuf_len(msg->buf), auth);
	memcpy(attr + 1, orig, MD5_MAC_LEN);
	if (req_auth) {
		memcpy(msg->hdr->authenticator, orig_authenticator,
			  sizeof(orig_authenticator));
	}

	if (memcmp(orig, auth, MD5_MAC_LEN) != 0) {
		printf("Invalid Message-Authenticator!\n");
		return 1;
	}

	return 0;
}


int radius_msg_verify(struct radius_msg *msg, const uint8_t *secret,
		      size_t secret_len, struct radius_msg *sent_msg, int auth)
{
	const uint8_t *addr[4];
	size_t len[4];
	uint8_t hash[MD5_MAC_LEN];

	if (sent_msg == NULL) 
    {
		LOG_ERROR("No matching Access-Request message found\n");
		return 1;
	}

	if (auth &&
	    radius_msg_verify_msg_auth(msg, secret, secret_len,
				       sent_msg->hdr->authenticator)) 
		return 1;
	

	/* ResponseAuth = MD5(Code+ID+Length+RequestAuth+Attributes+Secret) */
	addr[0] = (uint8_t *) msg->hdr;
	len[0] = 1 + 1 + 2;
	addr[1] = sent_msg->hdr->authenticator;
	len[1] = MD5_MAC_LEN;
	addr[2] = wpabuf_head_uint8_t(msg->buf) + sizeof(struct radius_hdr);
	len[2] = wpabuf_len(msg->buf) - sizeof(struct radius_hdr);
	addr[3] = secret;
	len[3] = secret_len;
	md5_vector(4, addr, len, hash);
	if (memcmp(hash, msg->hdr->authenticator, MD5_MAC_LEN) != 0) 
    {
		LOG_ERROR("Response Authenticator invalid!\n");
		return 1;
	}

	return 0;
}


int radius_msg_copy_attr(struct radius_msg *dst, struct radius_msg *src,
			 uint8_t type)
{
	struct radius_attr_hdr *attr;
	size_t i;
	int count = 0;

	for (i = 0; i < src->attr_used; i++) {
		attr = radius_get_attr_hdr(src, i);
		if (attr->type == type) {
			if (!radius_msg_add_attr(dst, type, (uint8_t *) (attr + 1),
						 attr->length - sizeof(*attr)))
				return -1;
			count++;
		}
	}

	return count;
}


/* Create Request Authenticator. The value should be unique over the lifetime
 * of the shared secret between authenticator and authentication server.
 * Use one-way MD5 hash calculated from current timestamp and some data given
 * by the caller. */
void radius_msg_make_authenticator(struct radius_msg *msg,
				   const uint8_t *data, size_t len)
{
	struct timeval tv;
	long int l;
	const uint8_t *addr[3];
	size_t elen[3];

	gettimeofday(&tv,NULL);
	l = random();
	addr[0] = (uint8_t *) &tv;
	elen[0] = sizeof(tv);
	addr[1] = data;
	elen[1] = len;
	addr[2] = (uint8_t *) &l;
	elen[2] = sizeof(l);
	md5_vector(3, addr, elen, msg->hdr->authenticator);
}


/* Get Vendor-specific RADIUS Attribute from a parsed RADIUS message.
 * Returns the Attribute payload and sets alen to indicate the length of the
 * payload if a vendor attribute with subtype is found, otherwise returns NULL.
 * The returned payload is allocated with malloc() and caller must free it
 * by calling free().
 */
static uint8_t *radius_msg_get_vendor_attr(struct radius_msg *msg, uint32_t vendor,
				      uint8_t subtype, size_t *alen)
{
	uint8_t *data, *pos;
	size_t i, len;

	if (msg == NULL)
		return NULL;

	for (i = 0; i < msg->attr_used; i++) {
		struct radius_attr_hdr *attr = radius_get_attr_hdr(msg, i);
		size_t left;
		uint32_t vendor_id;
		struct radius_attr_vendor *vhdr;

		if (attr->type != RADIUS_ATTR_VENDOR_SPECIFIC)
			continue;

		left = attr->length - sizeof(*attr);
		if (left < 4)
			continue;

		pos = (uint8_t *) (attr + 1);

		memcpy(&vendor_id, pos, 4);
		pos += 4;
		left -= 4;

		if (ntohl(vendor_id) != vendor)
			continue;

		while (left >= sizeof(*vhdr)) {
			vhdr = (struct radius_attr_vendor *) pos;
			if (vhdr->vendor_length > left ||
			    vhdr->vendor_length < sizeof(*vhdr)) {
				left = 0;
				break;
			}
			if (vhdr->vendor_type != subtype) {
				pos += vhdr->vendor_length;
				left -= vhdr->vendor_length;
				continue;
			}

			len = vhdr->vendor_length - sizeof(*vhdr);
			data = malloc(len);
			if (data == NULL)
				return NULL;
			memcpy(data, pos + sizeof(*vhdr), len);
			if (alen)
				*alen = len;
			return data;
		}
	}

	return NULL;
}


static uint8_t * decrypt_ms_key(const uint8_t *key, size_t len,
			   const uint8_t *req_authenticator,
			   const uint8_t *secret, size_t secret_len, size_t *reslen)
{
	uint8_t *plain, *ppos, *res;
	const uint8_t *pos;
	size_t left, plen;
	uint8_t hash[MD5_MAC_LEN];
	int i, first = 1;
	const uint8_t *addr[3];
	size_t elen[3];

	/* key: 16-bit salt followed by encrypted key info */

	if (len < 2 + 16)
		return NULL;

	pos = key + 2;
	left = len - 2;
	if (left % 16) {
		printf("Invalid ms key len %lu\n", (unsigned long) left);
		return NULL;
	}

	plen = left;
	ppos = plain = malloc(plen);
	if (plain == NULL)
		return NULL;
	plain[0] = 0;

	while (left > 0) {
		/* b(1) = MD5(Secret + Request-Authenticator + Salt)
		 * b(i) = MD5(Secret + c(i - 1)) for i > 1 */

		addr[0] = secret;
		elen[0] = secret_len;
		if (first) {
			addr[1] = req_authenticator;
			elen[1] = MD5_MAC_LEN;
			addr[2] = key;
			elen[2] = 2; /* Salt */
		} else {
			addr[1] = pos - MD5_MAC_LEN;
			elen[1] = MD5_MAC_LEN;
		}
		md5_vector(first ? 3 : 2, addr, elen, hash);
		first = 0;

		for (i = 0; i < MD5_MAC_LEN; i++)
			*ppos++ = *pos++ ^ hash[i];
		left -= MD5_MAC_LEN;
	}

	if (plain[0] == 0 || plain[0] > plen - 1) {
		printf("Failed to decrypt MPPE key\n");
		free(plain);
		return NULL;
	}

	res = malloc(plain[0]);
	if (res == NULL) {
		free(plain);
		return NULL;
	}
	memcpy(res, plain + 1, plain[0]);
	if (reslen)
		*reslen = plain[0];
	free(plain);
	return res;
}


static void encrypt_ms_key(const uint8_t *key, size_t key_len, uint16_t salt,
			   const uint8_t *req_authenticator,
			   const uint8_t *secret, size_t secret_len,
			   uint8_t *ebuf, size_t *elen)
{
	int i, len, first = 1;
	uint8_t hash[MD5_MAC_LEN], saltbuf[2], *pos;
	const uint8_t *addr[3];
	size_t _len[3];

	WPA_PUT_BE16(saltbuf, salt);

	len = 1 + key_len;
	if (len & 0x0f) {
		len = (len & 0xf0) + 16;
	}
	memset(ebuf, 0, len);
	ebuf[0] = key_len;
	memcpy(ebuf + 1, key, key_len);

	*elen = len;

	pos = ebuf;
	while (len > 0) {
		/* b(1) = MD5(Secret + Request-Authenticator + Salt)
		 * b(i) = MD5(Secret + c(i - 1)) for i > 1 */
		addr[0] = secret;
		_len[0] = secret_len;
		if (first) {
			addr[1] = req_authenticator;
			_len[1] = MD5_MAC_LEN;
			addr[2] = saltbuf;
			_len[2] = sizeof(saltbuf);
		} else {
			addr[1] = pos - MD5_MAC_LEN;
			_len[1] = MD5_MAC_LEN;
		}
		md5_vector(first ? 3 : 2, addr, _len, hash);
		first = 0;

		for (i = 0; i < MD5_MAC_LEN; i++)
			*pos++ ^= hash[i];

		len -= MD5_MAC_LEN;
	}
}


struct radius_ms_mppe_keys *
radius_msg_get_ms_keys(struct radius_msg *msg, struct radius_msg *sent_msg,
		       const uint8_t *secret, size_t secret_len)
{
	uint8_t *key;
	size_t keylen;
	struct radius_ms_mppe_keys *keys;

	if (msg == NULL || sent_msg == NULL)
		return NULL;

	keys = calloc(1,sizeof(*keys));
	if (keys == NULL)
		return NULL;

	key = radius_msg_get_vendor_attr(msg, RADIUS_VENDOR_ID_MICROSOFT,
					 RADIUS_VENDOR_ATTR_MS_MPPE_SEND_KEY,
					 &keylen);
	if (key) {
		keys->send = decrypt_ms_key(key, keylen,
					    sent_msg->hdr->authenticator,
					    secret, secret_len,
					    &keys->send_len);
		free(key);
	}

	key = radius_msg_get_vendor_attr(msg, RADIUS_VENDOR_ID_MICROSOFT,
					 RADIUS_VENDOR_ATTR_MS_MPPE_RECV_KEY,
					 &keylen);
	if (key) {
		keys->recv = decrypt_ms_key(key, keylen,
					    sent_msg->hdr->authenticator,
					    secret, secret_len,
					    &keys->recv_len);
		free(key);
	}

	return keys;
}


struct radius_ms_mppe_keys *
radius_msg_get_cisco_keys(struct radius_msg *msg, struct radius_msg *sent_msg,
			  const uint8_t *secret, size_t secret_len)
{
	uint8_t *key;
	size_t keylen;
	struct radius_ms_mppe_keys *keys;

	if (msg == NULL || sent_msg == NULL)
		return NULL;

	keys = calloc(1,sizeof(*keys));
	if (keys == NULL)
		return NULL;

	key = radius_msg_get_vendor_attr(msg, RADIUS_VENDOR_ID_CISCO,
					 RADIUS_CISCO_AV_PAIR, &keylen);
	if (key && keylen == 51 &&
	    memcmp(key, "leap:session-key=", 17) == 0) {
		keys->recv = decrypt_ms_key(key + 17, keylen - 17,
					    sent_msg->hdr->authenticator,
					    secret, secret_len,
					    &keys->recv_len);
	}
	free(key);

	return keys;
}


int radius_msg_add_mppe_keys(struct radius_msg *msg,
			     const uint8_t *req_authenticator,
			     const uint8_t *secret, size_t secret_len,
			     const uint8_t *send_key, size_t send_key_len,
			     const uint8_t *recv_key, size_t recv_key_len)
{
	struct radius_attr_hdr *attr;
	uint32_t vendor_id = htonl(RADIUS_VENDOR_ID_MICROSOFT);
	uint8_t *buf;
	struct radius_attr_vendor *vhdr;
	uint8_t *pos;
	size_t elen;
	int hlen;
	uint16_t salt;

	hlen = sizeof(vendor_id) + sizeof(*vhdr) + 2;

	/* MS-MPPE-Send-Key */
	buf = malloc(hlen + send_key_len + 16);
	if (buf == NULL) {
		return 0;
	}
	pos = buf;
	memcpy(pos, &vendor_id, sizeof(vendor_id));
	pos += sizeof(vendor_id);
	vhdr = (struct radius_attr_vendor *) pos;
	vhdr->vendor_type = RADIUS_VENDOR_ATTR_MS_MPPE_SEND_KEY;
	pos = (uint8_t *) (vhdr + 1);
	salt = random() | 0x8000;
	WPA_PUT_BE16(pos, salt);
	pos += 2;
	encrypt_ms_key(send_key, send_key_len, salt, req_authenticator, secret,
		       secret_len, pos, &elen);
	vhdr->vendor_length = hlen + elen - sizeof(vendor_id);

	attr = radius_msg_add_attr(msg, RADIUS_ATTR_VENDOR_SPECIFIC,
				   buf, hlen + elen);
	free(buf);
	if (attr == NULL) {
		return 0;
	}

	/* MS-MPPE-Recv-Key */
	buf = malloc(hlen + send_key_len + 16);
	if (buf == NULL) {
		return 0;
	}
	pos = buf;
	memcpy(pos, &vendor_id, sizeof(vendor_id));
	pos += sizeof(vendor_id);
	vhdr = (struct radius_attr_vendor *) pos;
	vhdr->vendor_type = RADIUS_VENDOR_ATTR_MS_MPPE_RECV_KEY;
	pos = (uint8_t *) (vhdr + 1);
	salt ^= 1;
	WPA_PUT_BE16(pos, salt);
	pos += 2;
	encrypt_ms_key(recv_key, recv_key_len, salt, req_authenticator, secret,
		       secret_len, pos, &elen);
	vhdr->vendor_length = hlen + elen - sizeof(vendor_id);

	attr = radius_msg_add_attr(msg, RADIUS_ATTR_VENDOR_SPECIFIC,
				   buf, hlen + elen);
	free(buf);
	if (attr == NULL) {
		return 0;
	}

	return 1;
}


/* Add User-Password attribute to a RADIUS message and encrypt it as specified
 * in RFC 2865, Chap. 5.2 */
struct radius_attr_hdr *
radius_msg_add_attr_user_password(struct radius_msg *msg,
				  const uint8_t *data, size_t data_len,
				  const uint8_t *secret, size_t secret_len)
{
	uint8_t buf[128];
	int padlen, i;
	size_t buf_len, pos;
	const uint8_t *addr[2];
	size_t len[2];
	uint8_t hash[16];

	if (data_len > 128)
		return NULL;

	memcpy(buf, data, data_len);
	buf_len = data_len;

	padlen = data_len % 16;
	if (padlen) {
		padlen = 16 - padlen;
		memset(buf + data_len, 0, padlen);
		buf_len += padlen;
	}

	addr[0] = secret;
	len[0] = secret_len;
	addr[1] = msg->hdr->authenticator;
	len[1] = 16;
	md5_vector(2, addr, len, hash);

	for (i = 0; i < 16; i++)
		buf[i] ^= hash[i];
	pos = 16;

	while (pos < buf_len) {
		addr[0] = secret;
		len[0] = secret_len;
		addr[1] = &buf[pos - 16];
		len[1] = 16;
		md5_vector(2, addr, len, hash);

		for (i = 0; i < 16; i++)
			buf[pos + i] ^= hash[i];

		pos += 16;
	}

	return radius_msg_add_attr(msg, RADIUS_ATTR_USER_PASSWORD,
				   buf, buf_len);
}


int radius_msg_get_attr(struct radius_msg *msg, uint8_t type, uint8_t *buf, size_t len)
{
	struct radius_attr_hdr *attr = NULL, *tmp;
	size_t i, dlen;

	for (i = 0; i < msg->attr_used; i++) {
		tmp = radius_get_attr_hdr(msg, i);
		if (tmp->type == type) {
			attr = tmp;
			break;
		}
	}

	if (!attr)
		return -1;

	dlen = attr->length - sizeof(*attr);
	if (buf)
		memcpy(buf, (attr + 1), dlen > len ? len : dlen);
	return dlen;
}


int radius_msg_get_attr_ptr(struct radius_msg *msg, uint8_t type, uint8_t **buf,
			    size_t *len, const uint8_t *start)
{
	size_t i;
	struct radius_attr_hdr *attr = NULL, *tmp;

	for (i = 0; i < msg->attr_used; i++) {
		tmp = radius_get_attr_hdr(msg, i);
		if (tmp->type == type &&
		    (start == NULL || (uint8_t *) tmp > start)) {
			attr = tmp;
			break;
		}
	}

	if (!attr)
		return -1;

	*buf = (uint8_t *) (attr + 1);
	*len = attr->length - sizeof(*attr);
	return 0;
}


int radius_msg_count_attr(struct radius_msg *msg, uint8_t type, int min_len)
{
	size_t i;
	int count;

	for (count = 0, i = 0; i < msg->attr_used; i++) {
		struct radius_attr_hdr *attr = radius_get_attr_hdr(msg, i);
		if (attr->type == type &&
		    attr->length >= sizeof(struct radius_attr_hdr) + min_len)
			count++;
	}

	return count;
}


struct radius_tunnel_attrs {
	int tag_used;
	int type; /* Tunnel-Type */
	int medium_type; /* Tunnel-Medium-Type */
	int vlanid;
};


/**
 * radius_msg_get_vlanid - Parse RADIUS attributes for VLAN tunnel information
 * @msg: RADIUS message
 * Returns: VLAN ID for the first tunnel configuration of -1 if none is found
 */
int radius_msg_get_vlanid(struct radius_msg *msg)
{
	struct radius_tunnel_attrs tunnel[RADIUS_TUNNEL_TAGS], *tun;
	size_t i;
	struct radius_attr_hdr *attr = NULL;
	const uint8_t *data;
	char buf[10];
	size_t dlen;

	memset(&tunnel, 0, sizeof(tunnel));

	for (i = 0; i < msg->attr_used; i++) {
		attr = radius_get_attr_hdr(msg, i);
		data = (const uint8_t *) (attr + 1);
		dlen = attr->length - sizeof(*attr);
		if (attr->length < 3)
			continue;
		if (data[0] >= RADIUS_TUNNEL_TAGS)
			tun = &tunnel[0];
		else
			tun = &tunnel[data[0]];

		switch (attr->type) {
		case RADIUS_ATTR_TUNNEL_TYPE:
			if (attr->length != 6)
				break;
			tun->tag_used++;
			tun->type = WPA_GET_BE24(data + 1);
			break;
		case RADIUS_ATTR_TUNNEL_MEDIUM_TYPE:
			if (attr->length != 6)
				break;
			tun->tag_used++;
			tun->medium_type = WPA_GET_BE24(data + 1);
			break;
		case RADIUS_ATTR_TUNNEL_PRIVATE_GROUP_ID:
			if (data[0] < RADIUS_TUNNEL_TAGS) {
				data++;
				dlen--;
			}
			if (dlen >= sizeof(buf))
				break;
			memcpy(buf, data, dlen);
			buf[dlen] = '\0';
			tun->tag_used++;
			tun->vlanid = atoi(buf);
			break;
		}
	}

	for (i = 0; i < RADIUS_TUNNEL_TAGS; i++) {
		tun = &tunnel[i];
		if (tun->tag_used &&
		    tun->type == RADIUS_TUNNEL_TYPE_VLAN &&
		    tun->medium_type == RADIUS_TUNNEL_MEDIUM_TYPE_802 &&
		    tun->vlanid > 0)
			return tun->vlanid;
	}

	return -1;
}


void radius_free_class(struct radius_class_data *c)
{
	size_t i;
	if (c == NULL)
		return;
	for (i = 0; i < c->count; i++)
		free(c->attr[i].data);
	free(c->attr);
	c->attr = NULL;
	c->count = 0;
}


int radius_copy_class(struct radius_class_data *dst,
		      const struct radius_class_data *src)
{
	size_t i;

	if (src->attr == NULL)
		return 0;

	dst->attr = calloc(1,src->count * sizeof(struct radius_attr_data));
	if (dst->attr == NULL)
		return -1;

	dst->count = 0;

	for (i = 0; i < src->count; i++) {
		dst->attr[i].data = malloc(src->attr[i].len);
		if (dst->attr[i].data == NULL)
			break;
		dst->count++;
		memcpy(dst->attr[i].data, src->attr[i].data,
			  src->attr[i].len);
		dst->attr[i].len = src->attr[i].len;
	}

	return 0;
}
