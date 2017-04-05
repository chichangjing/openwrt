#include "common/wpabuf.h"
#include "radius.h"
#include "radius_client.h"

/* Defaults for RADIUS retransmit values (exponential backoff) */

/**
 * RADIUS_CLIENT_FIRST_WAIT - RADIUS client timeout for first retry in seconds
 */
#define RADIUS_CLIENT_FIRST_WAIT 3

/**
 * RADIUS_CLIENT_MAX_WAIT - RADIUS client maximum retry timeout in seconds
 */
#define RADIUS_CLIENT_MAX_WAIT 120

/**
 * RADIUS_CLIENT_MAX_RETRIES - RADIUS client maximum retries
 *
 * Maximum number of retransmit attempts before the entry is removed from
 * retransmit list.
 */
#define RADIUS_CLIENT_MAX_RETRIES 10

/**
 * RADIUS_CLIENT_MAX_ENTRIES - RADIUS client maximum pending messages
 *
 * Maximum number of entries in retransmit list (oldest entries will be
 * removed, if this limit is exceeded).
 */
#define RADIUS_CLIENT_MAX_ENTRIES 30

/**
 * RADIUS_CLIENT_NUM_FAILOVER - RADIUS client failover point
 *
 * The number of failed retry attempts after which the RADIUS server will be
 * changed (if one of more backup servers are configured).
 */
#define RADIUS_CLIENT_NUM_FAILOVER 4


/**
 * struct radius_rx_handler - RADIUS client RX handler
 *
 * This data structure is used internally inside the RADIUS client module to
 * store registered RX handlers. These handlers are registered by calls to
 * radius_client_register() and unregistered when the RADIUS client is
 * deinitialized with a call to radius_client_deinit().
 */
struct radius_rx_handler {
	/**
	 * handler - Received RADIUS message handler
	 */
	RadiusRxResult (*handler)(struct radius_msg *msg,
				  struct radius_msg *req,
				  const uint8_t *shared_secret,
				  size_t shared_secret_len,
				  void *data);

	/**
	 * data - Context data for the handler
	 */
	void *data;
};


/**
 * struct radius_msg_list - RADIUS client message retransmit list
 *
 * This data structure is used internally inside the RADIUS client module to
 * store pending RADIUS requests that may still need to be retransmitted.
 */
struct radius_msg_list {
	/**
	 * addr - STA/client address
	 *
	 * This is used to find RADIUS messages for the same STA.
	 */
	uint8_t addr[ETH_ALEN];

	/**
	 * msg - RADIUS message
	 */
	struct radius_msg *msg;

	/**
	 * msg_type - Message type
	 */
	RadiusType msg_type;

	/**
	 * first_try - Time of the first transmission attempt
	 */
	long first_try;

	/**
	 * next_try - Time for the next transmission attempt
	 */
	long next_try;

	/**
	 * attempts - Number of transmission attempts
	 */
	int attempts;

	/**
	 * next_wait - Next retransmission wait time in seconds
	 */
	int next_wait;

	/**
	 * last_attempt - Time of the last transmission attempt
	 */
	struct timeval last_attempt;

	/**
	 * shared_secret - Shared secret with the target RADIUS server
	 */
	const uint8_t *shared_secret;

	/**
	 * shared_secret_len - shared_secret length in octets
	 */
	size_t shared_secret_len;

	/* TODO: server config with failover to backup server(s) */

	/**
	 * next - Next message in the list
	 */
	struct radius_msg_list *next;
};


/**
 * struct radius_client_data - Internal RADIUS client data
 *
 * This data structure is used internally inside the RADIUS client module.
 * External users allocate this by calling radius_client_init() and free it by
 * calling radius_client_deinit(). The pointer to this opaque data is used in
 * calls to other functions as an identifier for the RADIUS client instance.
 */
struct radius_client_data {
	/**
	 * ctx - Context pointer for hostapd_logger() callbacks
	 */
	void *ctx;

	/**
	 * conf - RADIUS client configuration (list of RADIUS servers to use)
	 */
	struct radius_servers *conf;

    struct uloop_fd auth_fd;
    struct uloop_fd acct_fd;

    struct uloop_timeout primary_interval_timer;
    struct uloop_timeout retransmit_timer;

	/**
	 * auth_handlers - Authentication message handlers
	 */
	struct radius_rx_handler *auth_handlers;

	/**
	 * num_auth_handlers - Number of handlers in auth_handlers
	 */
	size_t num_auth_handlers;

	/**
	 * acct_handlers - Accounting message handlers
	 */
	struct radius_rx_handler *acct_handlers;

	/**
	 * num_acct_handlers - Number of handlers in acct_handlers
	 */
	size_t num_acct_handlers;

	/**
	 * msgs - Pending outgoing RADIUS messages
	 */
	struct radius_msg_list *msgs;

	/**
	 * num_msgs - Number of pending messages in the msgs list
	 */
	size_t num_msgs;

	/**
	 * next_radius_identifier - Next RADIUS message identifier to use
	 */
	uint8_t next_radius_identifier;
};


static int radius_client_init_acct(struct radius_client_data *radius);
static int radius_client_init_auth(struct radius_client_data *radius);


static void radius_client_msg_free(struct radius_msg_list *req)
{
	radius_msg_free(req->msg);
	free(req);
}

static void radius_client_update_acct_msgs(struct radius_client_data *radius,
					   const uint8_t *shared_secret,
					   size_t shared_secret_len)
{
	struct radius_msg_list *entry;

	if (!radius)
		return;

	for (entry = radius->msgs; entry; entry = entry->next) {
		if (entry->msg_type == RADIUS_ACCT) {
			entry->shared_secret = shared_secret;
			entry->shared_secret_len = shared_secret_len;
			radius_msg_finish_acct(entry->msg, shared_secret,
					       shared_secret_len);
		}
	}
}

static int radius_change_server(struct radius_client_data *radius,
		     struct radius_server *nserv,
		     struct radius_server *oserv,
		     int sock,int auth)
{
	struct sockaddr_in serv, claddr;
	struct sockaddr *addr, *cl_addr;
	socklen_t addrlen, claddrlen;
	struct radius_msg_list *entry;
	struct radius_servers *conf = radius->conf;

	if (!oserv || nserv->shared_secret_len != oserv->shared_secret_len ||
	    memcmp(nserv->shared_secret, oserv->shared_secret,
		      nserv->shared_secret_len) != 0) 
    {
		/* Pending RADIUS packets used different shared secret, so
		 * they need to be modified. Update accounting message
		 * authenticators here. Authentication messages are removed
		 * since they would require more changes and the new RADIUS
		 * server may not be prepared to receive them anyway due to
		 * missing state information. Client will likely retry
		 * authentication, so this should not be an issue. */
		if (auth)
			radius_client_flush(radius, 1);
		else 
			radius_client_update_acct_msgs(radius, nserv->shared_secret,nserv->shared_secret_len);
	}

	/* Reset retry counters for the new server */
	for (entry = radius->msgs; entry; entry = entry->next) 
    {
		if ((auth && entry->msg_type != RADIUS_AUTH) ||
		    (!auth && entry->msg_type != RADIUS_ACCT))
			continue;
		entry->next_try = entry->first_try + RADIUS_CLIENT_FIRST_WAIT;
		entry->attempts = 0;
		entry->next_wait = RADIUS_CLIENT_FIRST_WAIT * 2;
	}

	if (radius->msgs) 
    {
	    uloop_timeout_cancel(&radius->retransmit_timer);
        uloop_timeout_set(&radius->retransmit_timer,1000 * RADIUS_CLIENT_FIRST_WAIT);
	}

	switch (nserv->addr.af) 
    {
	case AF_INET:
		memset(&serv, 0, sizeof(serv));
		serv.sin_family = AF_INET;
		serv.sin_addr.s_addr = nserv->addr.u.v4.s_addr;
		serv.sin_port = htons(nserv->port);
		addr = (struct sockaddr *) &serv;
		addrlen = sizeof(serv);
		break;
	default:
		return -1;
	}

	if (conf->force_client_addr) 
    {
		switch (conf->client_addr.af) 
        {
		case AF_INET:
			memset(&claddr, 0, sizeof(claddr));
			claddr.sin_family = AF_INET;
			claddr.sin_addr.s_addr = conf->client_addr.u.v4.s_addr;
			claddr.sin_port = htons(0);
			cl_addr = (struct sockaddr *) &claddr;
			claddrlen = sizeof(claddr);
			break;
		default:
			return -1;
		}

		if (bind(sock, cl_addr, claddrlen) < 0) 
        {
			LOG_ERROR("bind[radius] error");
			return -1;
		}
	}

	if (connect(sock, addr, addrlen) < 0) 
    {
		LOG_ERROR("connect[radius] error");
		return -1;
	}

#ifndef CONFIG_NATIVE_WINDOWS
	switch (nserv->addr.af) 
    {
	case AF_INET:
		claddrlen = sizeof(claddr);
		getsockname(sock, (struct sockaddr *) &claddr, &claddrlen);
		LOG_D("RADIUS local address: %s:%u",
			   inet_ntoa(claddr.sin_addr), ntohs(claddr.sin_port));
		break;
	}
#endif /* CONFIG_NATIVE_WINDOWS */

	return 0;
}



/**
 * radius_client_register - Register a RADIUS client RX handler
 * @radius: RADIUS client context from radius_client_init()
 * @msg_type: RADIUS client type (RADIUS_AUTH or RADIUS_ACCT)
 * @handler: Handler for received RADIUS messages
 * @data: Context pointer for handler callbacks
 * Returns: 0 on success, -1 on failure
 *
 * This function is used to register a handler for processing received RADIUS
 * authentication and accounting messages. The handler() callback function will
 * be called whenever a RADIUS message is received from the active server.
 *
 * There can be multiple registered RADIUS message handlers. The handlers will
 * be called in order until one of them indicates that it has processed or
 * queued the message.
 */
int radius_client_register(struct radius_client_data *radius,
			   RadiusType msg_type,
			   RadiusRxResult (*handler)(struct radius_msg *msg,
						     struct radius_msg *req,
						     const uint8_t *shared_secret,
						     size_t shared_secret_len,
						     void *data),
			   void *data)
{
	struct radius_rx_handler **handlers, *newh;
	size_t *num;

	if (msg_type == RADIUS_ACCT) {
		handlers = &radius->acct_handlers;
		num = &radius->num_acct_handlers;
	} else {
		handlers = &radius->auth_handlers;
		num = &radius->num_auth_handlers;
	}

	newh = realloc(*handlers,
			  (*num + 1) * sizeof(struct radius_rx_handler));
	if (newh == NULL)
		return -1;

	newh[*num].handler = handler;
	newh[*num].data = data;
	(*num)++;
	*handlers = newh;

	return 0;
}


static void radius_client_handle_send_error(struct radius_client_data *radius,
					    struct uloop_fd *rets_fd, RadiusType msg_type)
{
#ifndef CONFIG_NATIVE_WINDOWS
	int _errno = errno;
	LOG_ERROR("[RADIUS] errno = %d",_errno);
	if (_errno == ENOTCONN || _errno == EDESTADDRREQ || _errno == EINVAL ||
	    _errno == EBADF) 
    {
		uloop_fd_delete(rets_fd);
        close(rets_fd->fd);
		if (msg_type == RADIUS_ACCT || msg_type == RADIUS_ACCT_INTERIM)
			radius_client_init_acct(radius);
		else
			radius_client_init_auth(radius);
	}
#endif /* CONFIG_NATIVE_WINDOWS */
}


static int radius_client_retransmit(struct radius_client_data *radius,
				    struct radius_msg_list *entry,
				    long now)
{
	struct radius_servers *conf = radius->conf;
	struct wpabuf *buf;
    struct uloop_fd *rets_fd;

	if (entry->msg_type == RADIUS_ACCT ||
	    entry->msg_type == RADIUS_ACCT_INTERIM) {
		rets_fd = &radius->acct_fd;
		if (entry->attempts == 0)
			conf->acct_server->requests++;
		else {
			conf->acct_server->timeouts++;
			conf->acct_server->retransmissions++;
		}
	} else {
		rets_fd = &radius->auth_fd;
		if (entry->attempts == 0)
			conf->auth_server->requests++;
		else {
			conf->auth_server->timeouts++;
			conf->auth_server->retransmissions++;
		}
	}

	/* retransmit; remove entry if too many attempts */
	entry->attempts++;

	gettimeofday(&entry->last_attempt,NULL);
	buf = radius_msg_get_buf(entry->msg);
	if (send(rets_fd->fd, wpabuf_head(buf), wpabuf_len(buf), 0) < 0)
		radius_client_handle_send_error(radius, rets_fd, entry->msg_type);

	entry->next_try = now + entry->next_wait;
	entry->next_wait *= 2;
	if (entry->next_wait > RADIUS_CLIENT_MAX_WAIT)
		entry->next_wait = RADIUS_CLIENT_MAX_WAIT;
	if (entry->attempts >= RADIUS_CLIENT_MAX_RETRIES) {
		LOG_ERROR("Removing un-ACKed RADIUS message due to too many "
		       "failed retransmit attempts\n");
		return 1;
	}

	return 0;
}

static void radius_client_timer(struct uloop_timeout *timer)
{
	struct radius_client_data *radius = container_of(timer,struct radius_client_data,retransmit_timer);
	struct radius_servers *conf = radius->conf;
	struct timeval now;
	long first;
	struct radius_msg_list *entry, *prev, *tmp;
	int auth_failover = 0, acct_failover = 0;

	entry = radius->msgs;
	if (!entry)
		return;

	gettimeofday(&now,NULL);
	first = 0;

	prev = NULL;
	while (entry) {
		if (now.tv_sec >= entry->next_try &&
		    radius_client_retransmit(radius, entry, now.tv_sec)) {
			if (prev)
				prev->next = entry->next;
			else
				radius->msgs = entry->next;

			tmp = entry;
			entry = entry->next;
			radius_client_msg_free(tmp);
			radius->num_msgs--;
			continue;
		}

		if (entry->attempts > RADIUS_CLIENT_NUM_FAILOVER) {
			if (entry->msg_type == RADIUS_ACCT ||
			    entry->msg_type == RADIUS_ACCT_INTERIM)
				acct_failover++;
			else
				auth_failover++;
		}

		if (first == 0 || entry->next_try < first)
			first = entry->next_try;

		prev = entry;
		entry = entry->next;
	}

	if (radius->msgs) {
		if (first < now.tv_sec)
			first = now.tv_sec;
        uloop_timeout_set(&radius->retransmit_timer,1000 * (first - now.tv_sec));
	}

	if (auth_failover && conf->num_auth_servers > 1) {
		struct radius_server *next, *old;
		old = conf->auth_server;

		for (entry = radius->msgs; entry; entry = entry->next) {
			if (entry->msg_type == RADIUS_AUTH)
				old->timeouts++;
		}

		next = old + 1;
		if (next > &(conf->auth_servers[conf->num_auth_servers - 1]))
			next = conf->auth_servers;
		conf->auth_server = next;
		if(radius_change_server(radius, next, old,radius->auth_fd.fd,1))
        {
            LOG_ERROR("radius change auth server error");
            return;
        }
	}

	if (acct_failover && conf->num_acct_servers > 1) {
		struct radius_server *next, *old;
		old = conf->acct_server;

		for (entry = radius->msgs; entry; entry = entry->next) {
			if (entry->msg_type == RADIUS_ACCT ||
			    entry->msg_type == RADIUS_ACCT_INTERIM)
				old->timeouts++;
		}

		next = old + 1;
		if (next > &conf->acct_servers[conf->num_acct_servers - 1])
			next = conf->acct_servers;
		conf->acct_server = next;
		if(radius_change_server(radius, next, old,radius->acct_fd.fd,0))
        {
            LOG_ERROR("radius change auth server error");
            return;
        }
	}
}


static void radius_client_update_timeout(struct radius_client_data *radius)
{
	struct timeval now;
	long first;
	struct radius_msg_list *entry;

	uloop_timeout_cancel(&radius->retransmit_timer);

	if (radius->msgs == NULL) {
		return;
	}

	first = 0;
	for (entry = radius->msgs; entry; entry = entry->next) {
		if (first == 0 || entry->next_try < first)
			first = entry->next_try;
	}

	gettimeofday(&now,NULL);
	if (first < now.tv_sec)
		first = now.tv_sec;
    uloop_timeout_set(&radius->retransmit_timer,1000 * (first - now.tv_sec));
}


static void radius_client_list_add(struct radius_client_data *radius,
				   struct radius_msg *msg,
				   RadiusType msg_type,
				   const uint8_t *shared_secret,
				   size_t shared_secret_len, const uint8_t *addr)
{
	struct radius_msg_list *entry, *prev;

	entry = calloc(1,sizeof(*entry));
	if (entry == NULL) {
		LOG_ERROR("Failed to add RADIUS packet into retransmit list\n");
		radius_msg_free(msg);
		return;
	}

	if (addr)
		memcpy(entry->addr, addr, ETH_ALEN);
	entry->msg = msg;
	entry->msg_type = msg_type;
	entry->shared_secret = shared_secret;
	entry->shared_secret_len = shared_secret_len;
	gettimeofday(&entry->last_attempt,NULL);
	entry->first_try = entry->last_attempt.tv_sec;
	entry->next_try = entry->first_try + RADIUS_CLIENT_FIRST_WAIT;
	entry->attempts = 1;
	entry->next_wait = RADIUS_CLIENT_FIRST_WAIT * 2;
	entry->next = radius->msgs;
	radius->msgs = entry;
	radius_client_update_timeout(radius);

	if (radius->num_msgs >= RADIUS_CLIENT_MAX_ENTRIES) {
		LOG_ERROR("Removing the oldest un-ACKed RADIUS packet due to "
		       "retransmit list limits.\n");
		prev = NULL;
		while (entry->next) {
			prev = entry;
			entry = entry->next;
		}
		if (prev) {
			prev->next = NULL;
			radius_client_msg_free(entry);
		}
	} else
		radius->num_msgs++;
}


static void radius_client_list_del(struct radius_client_data *radius,
				   RadiusType msg_type, const uint8_t *addr)
{
	struct radius_msg_list *entry, *prev, *tmp;

	if (addr == NULL)
		return;

	entry = radius->msgs;
	prev = NULL;
	while (entry) {
		if (entry->msg_type == msg_type &&
		    memcmp(entry->addr, addr, ETH_ALEN) == 0) {
			if (prev)
				prev->next = entry->next;
			else
				radius->msgs = entry->next;
			tmp = entry;
			entry = entry->next;
			radius_client_msg_free(tmp);
			radius->num_msgs--;
			continue;
		}
		prev = entry;
		entry = entry->next;
	}
}


/**
 * radius_client_send - Send a RADIUS request
 * @radius: RADIUS client context from radius_client_init()
 * @msg: RADIUS message to be sent
 * @msg_type: Message type (RADIUS_AUTH, RADIUS_ACCT, RADIUS_ACCT_INTERIM)
 * @addr: MAC address of the device related to this message or %NULL
 * Returns: 0 on success, -1 on failure
 *
 * This function is used to transmit a RADIUS authentication (RADIUS_AUTH) or
 * accounting request (RADIUS_ACCT or RADIUS_ACCT_INTERIM). The only difference
 * between accounting and interim accounting messages is that the interim
 * message will override any pending interim accounting updates while a new
 * accounting message does not remove any pending messages.
 *
 * The message is added on the retransmission queue and will be retransmitted
 * automatically until a response is received or maximum number of retries
 * (RADIUS_CLIENT_MAX_RETRIES) is reached.
 *
 * The related device MAC address can be used to identify pending messages that
 * can be removed with radius_client_flush_auth() or with interim accounting
 * updates.
 */
int radius_client_send(struct radius_client_data *radius,
		       struct radius_msg *msg, RadiusType msg_type,
		       const uint8_t *addr)
{
	struct radius_servers *conf = radius->conf;
	const uint8_t *shared_secret;
	size_t shared_secret_len;
	int res;
    struct uloop_fd *send_fd;
	struct wpabuf *buf;

	if (msg_type == RADIUS_ACCT_INTERIM) 
    {
		/* Remove any pending interim acct update for the same STA. */
		radius_client_list_del(radius, msg_type, addr);
	}

	if (msg_type == RADIUS_ACCT || msg_type == RADIUS_ACCT_INTERIM) 
    {
		if (conf->acct_server == NULL) 
			return -1;
		
		shared_secret = conf->acct_server->shared_secret;
		shared_secret_len = conf->acct_server->shared_secret_len;
		radius_msg_finish_acct(msg, shared_secret, shared_secret_len);
		send_fd = &radius->acct_fd;
		conf->acct_server->requests++;
	} 
    else 
    {
		if (conf->auth_server == NULL) 
			return -1;
		
		shared_secret = conf->auth_server->shared_secret;
		shared_secret_len = conf->auth_server->shared_secret_len;
		radius_msg_finish(msg, shared_secret, shared_secret_len);
		send_fd = &radius->auth_fd;
		conf->auth_server->requests++;
	}

	if (conf->msg_dumps)
		radius_msg_dump(msg);

	buf = radius_msg_get_buf(msg);
	res = send(send_fd->fd, wpabuf_head(buf), wpabuf_len(buf), 0);
	if (res < 0)
		radius_client_handle_send_error(radius, send_fd, msg_type);

	radius_client_list_add(radius, msg, msg_type, shared_secret,
			       shared_secret_len, addr);

	return 0;
}

static void radius_client_receive(int sock, struct radius_client_data *radius,RadiusType msg_type)
{
	struct radius_servers *conf = radius->conf;
	int len, roundtrip;
	unsigned char buf[3000];
	struct radius_msg *msg;
	struct radius_hdr *hdr;
	struct radius_rx_handler *handlers;
	size_t num_handlers, i;
	struct radius_msg_list *req, *prev_req;
	struct timeval now;
	struct radius_server *rconf;
	int invalid_authenticator = 0;

	if (msg_type == RADIUS_ACCT) 
    {
		handlers = radius->acct_handlers;
		num_handlers = radius->num_acct_handlers;
		rconf = conf->acct_server;
	} 
    else 
    {
		handlers = radius->auth_handlers;
		num_handlers = radius->num_auth_handlers;
		rconf = conf->auth_server;
	}

	len = recv(sock, buf, sizeof(buf), MSG_DONTWAIT);
	if (len < 0) 
    {
		LOG_ERROR("recv[RADIUS] error");
		return;
	}
	if (len == sizeof(buf)) 
    {
		LOG_ERROR("Possibly too long UDP frame for our buffer - dropping it\n");
		return;
	}

	msg = radius_msg_parse(buf, len);
	if (msg == NULL) 
    {
		LOG_ERROR("Parsing incoming RADIUS frame failed\n");
		rconf->malformed_responses++;
		return;
	}
	hdr = radius_msg_get_hdr(msg);

	if (conf->msg_dumps)
		radius_msg_dump(msg);

	switch (hdr->code) {
	case RADIUS_CODE_ACCESS_ACCEPT:
		rconf->access_accepts++;
		break;
	case RADIUS_CODE_ACCESS_REJECT:
		rconf->access_rejects++;
		break;
	case RADIUS_CODE_ACCESS_CHALLENGE:
		rconf->access_challenges++;
		break;
	case RADIUS_CODE_ACCOUNTING_RESPONSE:
		rconf->responses++;
		break;
	}

	prev_req = NULL;
	req = radius->msgs;
	while (req) 
    {
		/* TODO: also match by src addr:port of the packet when using
		 * alternative RADIUS servers (?) */
		if ((req->msg_type == msg_type ||
		     (req->msg_type == RADIUS_ACCT_INTERIM &&
		      msg_type == RADIUS_ACCT)) &&
		    radius_msg_get_hdr(req->msg)->identifier ==
		    hdr->identifier)
			break;

		prev_req = req;
		req = req->next;
	}
	if (req == NULL) 
		goto fail;

	gettimeofday(&now,NULL);
	roundtrip = (now.tv_sec - req->last_attempt.tv_sec) * 100 +
		(now.tv_usec - req->last_attempt.tv_usec) / 10000;
	rconf->round_trip_time = roundtrip;

	/* Remove ACKed RADIUS packet from retransmit list */
	if (prev_req)
		prev_req->next = req->next;
	else
		radius->msgs = req->next;
	radius->num_msgs--;

	for (i = 0; i < num_handlers; i++) 
    {
		RadiusRxResult res;
		res = handlers[i].handler(msg, req->msg, req->shared_secret,
					  req->shared_secret_len,
					  handlers[i].data);
		switch (res) 
        {
		case RADIUS_RX_PROCESSED:
			radius_msg_free(msg);
			/* continue */
		case RADIUS_RX_QUEUED:
			radius_client_msg_free(req);
			return;
		case RADIUS_RX_INVALID_AUTHENTICATOR:
			invalid_authenticator++;
			/* continue */
		case RADIUS_RX_UNKNOWN:
			/* continue with next handler */
			break;
		}
	}

	if (invalid_authenticator)
		rconf->bad_authenticators++;
	else
		rconf->unknown_types++;
	radius_client_msg_free(req);

 fail:
	radius_msg_free(msg);
}

static void radius_client_receive_auth(struct uloop_fd *auth_fd,uint32_t events) 
{
    struct radius_client_data *radius = container_of(auth_fd,struct radius_client_data,auth_fd);    

    radius_client_receive(auth_fd->fd,radius,RADIUS_AUTH);
}

static void radius_client_receive_acct(struct uloop_fd *acct_fd,uint32_t events) 
{
    struct radius_client_data *radius = container_of(acct_fd,struct radius_client_data,acct_fd);    

    radius_client_receive(acct_fd->fd,radius,RADIUS_ACCT);
}


/**
 * radius_client_get_id - Get an identifier for a new RADIUS message
 * @radius: RADIUS client context from radius_client_init()
 * Returns: Allocated identifier
 *
 * This function is used to fetch a unique (among pending requests) identifier
 * for a new RADIUS message.
 */
uint8_t radius_client_get_id(struct radius_client_data *radius)
{
	struct radius_msg_list *entry, *prev, *_remove;
	uint8_t id = radius->next_radius_identifier++;

	/* remove entries with matching id from retransmit list to avoid
	 * using new reply from the RADIUS server with an old request */
	entry = radius->msgs;
	prev = NULL;
	while (entry) {
		if (radius_msg_get_hdr(entry->msg)->identifier == id) {
			if (prev)
				prev->next = entry->next;
			else
				radius->msgs = entry->next;
			_remove = entry;
		} else {
			_remove = NULL;
			prev = entry;
		}
		entry = entry->next;

		if (_remove)
			radius_client_msg_free(_remove);
	}

	return id;
}


/**
 * radius_client_flush - Flush all pending RADIUS client messages
 * @radius: RADIUS client context from radius_client_init()
 * @only_auth: Whether only authentication messages are removed
 */
void radius_client_flush(struct radius_client_data *radius, int only_auth)
{
	struct radius_msg_list *entry, *prev, *tmp;

	if (!radius)
		return;

	prev = NULL;
	entry = radius->msgs;

	while (entry) {
		if (!only_auth || entry->msg_type == RADIUS_AUTH) {
			if (prev)
				prev->next = entry->next;
			else
				radius->msgs = entry->next;

			tmp = entry;
			entry = entry->next;
			radius_client_msg_free(tmp);
			radius->num_msgs--;
		} else {
			prev = entry;
			entry = entry->next;
		}
	}

	if (radius->msgs == NULL)
	    uloop_timeout_cancel(&radius->retransmit_timer);
}



static void radius_retry_primary_timer(struct uloop_timeout *timer)
{
	struct radius_client_data *radius = container_of(timer,struct radius_client_data,primary_interval_timer);
	struct radius_servers *conf = radius->conf;
	struct radius_server *oserv;

	if (radius->auth_fd.fd && conf->auth_servers &&
	    conf->auth_server != conf->auth_servers) {
		oserv = conf->auth_server;
		conf->auth_server = conf->auth_servers;
		if(radius_change_server(radius, conf->auth_server, oserv,radius->auth_fd.fd,1))
        {
            LOG_ERROR("radius change auth server error");
            return;
        }
	}

	if (radius->acct_fd.fd && conf->acct_servers &&
	    conf->acct_server != conf->acct_servers) {
		oserv = conf->acct_server;
		conf->acct_server = conf->acct_servers;
		if(radius_change_server(radius, conf->acct_server, oserv,radius->acct_fd.fd,0))
        {
            LOG_ERROR("radius change auth server error");
            return;
        }
	}

	if (conf->retry_primary_interval)
        uloop_timeout_set(&radius->primary_interval_timer,1000 * conf->retry_primary_interval);
}


static int radius_client_disable_pmtu_discovery(int s)
{
	int r = -1;
#if defined(IP_MTU_DISCOVER) && defined(IP_PMTUDISC_DONT)
	/* Turn off Path MTU discovery on IPv4/UDP sockets. */
	int action = IP_PMTUDISC_DONT;
	r = setsockopt(s, IPPROTO_IP, IP_MTU_DISCOVER, &action,
		       sizeof(action));
	if (r == -1)
		LOG_ERROR("Failed to set IP_MTU_DISCOVER: "
			   "%s", strerror(errno));
#endif
	return r;
}

/* init radius auth client if configured
 * create a UDP socket for radius auth client,and register a callback func for receiving radius auth data
 * */
static int radius_client_init_auth(struct radius_client_data *radius)
{
	struct radius_servers *conf = radius->conf;
	int fd;

	fd = socket(PF_INET, SOCK_DGRAM, 0);
	if (fd < 0)
    {
		LOG_ERROR("socket[PF_INET,SOCK_DGRAM] error");
		return -1;
    }
	else {
		radius_client_disable_pmtu_discovery(fd);
	}

	if(radius_change_server(radius, conf->auth_server, NULL,fd,1))
    {
        LOG_ERROR("radius change auth server error");
        return -1;
    }

    radius->auth_fd.fd = fd;
    radius->auth_fd.cb = radius_client_receive_auth;
    if(uloop_fd_add(&radius->auth_fd,ULOOP_READ) < 0)
    {
		LOG_ERROR("Could not register read socket for authentication "
		       "server\n");
		return -1;
	}

	return 0;
}


static int radius_client_init_acct(struct radius_client_data *radius)
{
	struct radius_servers *conf = radius->conf;
	int fd;

	fd = socket(PF_INET, SOCK_DGRAM, 0);
	if (fd < 0)
    {
		LOG_ERROR("socket[PF_INET,SOCK_DGRAM] error");
		return -1;
    }
	else {
		radius_client_disable_pmtu_discovery(fd);
	}


	if(radius_change_server(radius, conf->acct_server, NULL,fd, 0))
    {
        LOG_ERROR("radius change acct server error");
        return -1;
    }

    radius->acct_fd.fd = fd;
    radius->acct_fd.cb = radius_client_receive_acct;
    if(uloop_fd_add(&radius->auth_fd,ULOOP_READ) < 0)
    {
		LOG_ERROR("Could not register read socket for accounting "
		       "server\n");
		return -1;
	}

	return 0;
}


/**
 * radius_client_init - Initialize RADIUS client
 * @ctx: Callback context to be used in hostapd_logger() calls
 * @conf: RADIUS client configuration (RADIUS servers)
 * Returns: Pointer to private RADIUS client context or %NULL on failure
 *
 * The caller is responsible for keeping the configuration data available for
 * the lifetime of the RADIUS client, i.e., until radius_client_deinit() is
 * called for the returned context pointer.
 */
struct radius_client_data *
radius_client_init(void *ctx, struct radius_servers *conf)
{
	struct radius_client_data *radius;

	radius = calloc(1,sizeof(struct radius_client_data));
	if (radius == NULL)
		return NULL;

	radius->ctx = ctx;
	radius->conf = conf;

	if (conf->auth_server && radius_client_init_auth(radius)) {
        LOG_ERROR("init radius auth client error");
		radius_client_deinit(radius);
		return NULL;
	}

	if (conf->acct_server && radius_client_init_acct(radius)) {
        LOG_ERROR("init radius acct client error");
		radius_client_deinit(radius);
		return NULL;
	}

    /* register 2 timer:
     *      the one for primary interval 
     *      the other one for retransmit
     */
    radius->primary_interval_timer.cb = radius_retry_primary_timer;
	if (conf->retry_primary_interval)
        uloop_timeout_set(&radius->primary_interval_timer,1000 * conf->retry_primary_interval);

    radius->retransmit_timer.cb = radius_client_timer;

	return radius;
}


/**
 * radius_client_deinit - Deinitialize RADIUS client
 * @radius: RADIUS client context from radius_client_init()
 */
void radius_client_deinit(struct radius_client_data *radius)
{
	if (!radius)
		return;

	if (radius->auth_fd.fd)
    {
		uloop_fd_delete(&radius->auth_fd);
        close(radius->auth_fd.fd);
    }
	if (radius->acct_fd.fd)
    {
		uloop_fd_delete(&radius->acct_fd);
        close(radius->acct_fd.fd);
    }

    uloop_timeout_cancel(&radius->primary_interval_timer);

	radius_client_flush(radius, 0);
	free(radius->auth_handlers);
	free(radius->acct_handlers);
	free(radius);
}


/**
 * radius_client_flush_auth - Flush pending RADIUS messages for an address
 * @radius: RADIUS client context from radius_client_init()
 * @addr: MAC address of the related device
 * 删除radius发送等待队列中指定mac的消息
 *
 * This function can be used to remove pending RADIUS authentication messages
 * that are related to a specific device. The addr parameter is matched with
 * the one used in radius_client_send() call that was used to transmit the
 * authentication request.
 */
void radius_client_flush_auth(struct radius_client_data *radius,
			      const uint8_t *addr)
{
	struct radius_msg_list *entry, *prev, *tmp;

	prev = NULL;
	entry = radius->msgs;
	while (entry) {
		if (entry->msg_type == RADIUS_AUTH &&
		    memcmp(entry->addr, addr, ETH_ALEN) == 0) {

			if (prev)
				prev->next = entry->next;
			else
				radius->msgs = entry->next;

			tmp = entry;
			entry = entry->next;
			radius_client_msg_free(tmp);
			radius->num_msgs--;
			continue;
		}

		prev = entry;
		entry = entry->next;
	}
}


static int radius_client_dump_auth_server(char *buf, size_t buflen,
					  struct radius_server *serv,
					  struct radius_client_data *cli)
{
	int pending = 0;
	struct radius_msg_list *msg;
	char abuf[50];

	if (cli) {
		for (msg = cli->msgs; msg; msg = msg->next) {
			if (msg->msg_type == RADIUS_AUTH)
				pending++;
		}
	}

	return snprintf(buf, buflen,
			   "radiusAuthServerIndex=%d\n"
			   "radiusAuthServerAddress=%s\n"
			   "radiusAuthClientServerPortNumber=%d\n"
			   "radiusAuthClientRoundTripTime=%d\n"
			   "radiusAuthClientAccessRequests=%u\n"
			   "radiusAuthClientAccessRetransmissions=%u\n"
			   "radiusAuthClientAccessAccepts=%u\n"
			   "radiusAuthClientAccessRejects=%u\n"
			   "radiusAuthClientAccessChallenges=%u\n"
			   "radiusAuthClientMalformedAccessResponses=%u\n"
			   "radiusAuthClientBadAuthenticators=%u\n"
			   "radiusAuthClientPendingRequests=%u\n"
			   "radiusAuthClientTimeouts=%u\n"
			   "radiusAuthClientUnknownTypes=%u\n"
			   "radiusAuthClientPacketsDropped=%u\n",
			   serv->index,
			   ip_txt(&serv->addr, abuf, sizeof(abuf)),
			   serv->port,
			   serv->round_trip_time,
			   serv->requests,
			   serv->retransmissions,
			   serv->access_accepts,
			   serv->access_rejects,
			   serv->access_challenges,
			   serv->malformed_responses,
			   serv->bad_authenticators,
			   pending,
			   serv->timeouts,
			   serv->unknown_types,
			   serv->packets_dropped);
}


static int radius_client_dump_acct_server(char *buf, size_t buflen,
					  struct radius_server *serv,
					  struct radius_client_data *cli)
{
	int pending = 0;
	struct radius_msg_list *msg;
	char abuf[50];

	if (cli) {
		for (msg = cli->msgs; msg; msg = msg->next) {
			if (msg->msg_type == RADIUS_ACCT ||
			    msg->msg_type == RADIUS_ACCT_INTERIM)
				pending++;
		}
	}

	return snprintf(buf, buflen,
			   "radiusAccServerIndex=%d\n"
			   "radiusAccServerAddress=%s\n"
			   "radiusAccClientServerPortNumber=%d\n"
			   "radiusAccClientRoundTripTime=%d\n"
			   "radiusAccClientRequests=%u\n"
			   "radiusAccClientRetransmissions=%u\n"
			   "radiusAccClientResponses=%u\n"
			   "radiusAccClientMalformedResponses=%u\n"
			   "radiusAccClientBadAuthenticators=%u\n"
			   "radiusAccClientPendingRequests=%u\n"
			   "radiusAccClientTimeouts=%u\n"
			   "radiusAccClientUnknownTypes=%u\n"
			   "radiusAccClientPacketsDropped=%u\n",
			   serv->index,
			   ip_txt(&serv->addr, abuf, sizeof(abuf)),
			   serv->port,
			   serv->round_trip_time,
			   serv->requests,
			   serv->retransmissions,
			   serv->responses,
			   serv->malformed_responses,
			   serv->bad_authenticators,
			   pending,
			   serv->timeouts,
			   serv->unknown_types,
			   serv->packets_dropped);
}


/**
 * radius_client_get_mib - Get RADIUS client MIB information
 * @radius: RADIUS client context from radius_client_init()
 * @buf: Buffer for returning MIB data in text format
 * @buflen: Maximum buf length in octets
 * Returns: Number of octets written into the buffer
 */
int radius_client_get_mib(struct radius_client_data *radius, char *buf,
			  size_t buflen)
{
	struct radius_servers *conf = radius->conf;
	int i;
	struct radius_server *serv;
	int count = 0;

	if (conf->auth_servers) {
		for (i = 0; i < conf->num_auth_servers; i++) {
			serv = &conf->auth_servers[i];
			count += radius_client_dump_auth_server(
				buf + count, buflen - count, serv,
				serv == conf->auth_server ?
				radius : NULL);
		}
	}

	if (conf->acct_servers) {
		for (i = 0; i < conf->num_acct_servers; i++) {
			serv = &conf->acct_servers[i];
			count += radius_client_dump_acct_server(
				buf + count, buflen - count, serv,
				serv == conf->acct_server ?
				radius : NULL);
		}
	}

	return count;
}
