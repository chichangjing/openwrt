/*
 * hostapd / EAP-MD5 server
 * Copyright (c) 2004-2007, Jouni Malinen <j@w1.fi>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * Alternatively, this software may be distributed under the terms of BSD
 * license.
 *
 * See README and COPYING for more details.
 */

#include "common/common.h"
#include "eap_i.h"
#include "eap_common/chap.h"
#include "common/os.h"


#define CHALLENGE_LEN 16

// 定义了一个MD5数据块
struct eap_md5_data {
	uint8_t challenge[CHALLENGE_LEN];    // 16字节的挑战字
	enum { CONTINUE, SUCCESS, FAILURE } state;  // MD5状态
};


static void * eap_md5_init(struct eap_sm *sm)
{
	struct eap_md5_data *data;

	data = calloc(1,sizeof(*data));
	if (data == NULL)
		return NULL;
	data->state = CONTINUE;

	return data;
}

// 复位md5操作,实际就是释放MD5数据块空间
static void eap_md5_reset(struct eap_sm *sm, void *priv)
{
	struct eap_md5_data *data = priv;
	free(data);
}


static struct wpabuf * eap_md5_buildReq(struct eap_sm *sm, void *priv, uint8_t id)
{
	struct eap_md5_data *data = priv;
	struct wpabuf *req;

	if (os_get_random(data->challenge, CHALLENGE_LEN)) {
		LOG_ERROR("EAP-MD5: Failed to get random data");
		data->state = FAILURE;
		return NULL;
	}

	req = eap_msg_alloc(EAP_VENDOR_IETF, EAP_TYPE_MD5, 1 + CHALLENGE_LEN,
			    EAP_CODE_REQUEST, id);
	if (req == NULL) {
		LOG_ERROR("EAP-MD5: Failed to allocate memory for "
			   "request");
		data->state = FAILURE;
		return NULL;
	}

	wpabuf_put_uint8_t(req, CHALLENGE_LEN);
	wpabuf_put_data(req, data->challenge, CHALLENGE_LEN);

	data->state = CONTINUE;

	return req;
}


static Boolean eap_md5_check(struct eap_sm *sm, void *priv,
			     struct wpabuf *respData)
{
	const uint8_t *pos;
	size_t len;

	pos = eap_hdr_validate(EAP_VENDOR_IETF, EAP_TYPE_MD5, respData, &len);
	if (pos == NULL || len < 1) {
		LOG_NORMAL("EAP-MD5: Invalid frame");
		return TRUE;
	}
	if (*pos != CHAP_MD5_LEN || 1 + CHAP_MD5_LEN > len) {
		LOG_NORMAL("EAP-MD5: Invalid response "
			   "(response_len=%d payload_len=%lu",
			   *pos, (unsigned long) len);
		return TRUE;
	}

	return FALSE;
}


static void eap_md5_process(struct eap_sm *sm, void *priv,
			    struct wpabuf *respData)
{
	struct eap_md5_data *data = priv;
	const uint8_t *pos;
	size_t plen;
	uint8_t hash[CHAP_MD5_LEN], id;

	if (sm->user == NULL || sm->user->password == NULL ||
	    sm->user->password_hash) {
		LOG_NORMAL("EAP-MD5: Plaintext password not "
			   "configured");
		data->state = FAILURE;
		return;
	}

	pos = eap_hdr_validate(EAP_VENDOR_IETF, EAP_TYPE_MD5, respData, &plen);
	if (pos == NULL || *pos != CHAP_MD5_LEN || plen < 1 + CHAP_MD5_LEN)
		return; /* Should not happen - frame already validated */

	pos++; /* Skip response len */

	id = eap_get_id(respData);
	chap_md5(id, sm->user->password, sm->user->password_len,
		 data->challenge, CHALLENGE_LEN, hash);

	if (memcmp(hash, pos, CHAP_MD5_LEN) == 0) {
		LOG_NORMAL("EAP-MD5: Done - Success");
		data->state = SUCCESS;
	} else {
		LOG_NORMAL("EAP-MD5: Done - Failure");
		data->state = FAILURE;
	}
}

// 判断md5类型的eap方法当前是否不处于continue状态
static Boolean eap_md5_isDone(struct eap_sm *sm, void *priv)
{
	struct eap_md5_data *data = priv;
	return data->state != CONTINUE;
}

// 判断md5类型的eap方法当前是否处于success状态
static Boolean eap_md5_isSuccess(struct eap_sm *sm, void *priv)
{
	struct eap_md5_data *data = priv;
	return data->state == SUCCESS;
}

// 注册md5类型的eap方法
int eap_server_md5_register(void)
{
	struct eap_method *eap;
	int ret;

	eap = eap_server_method_alloc(EAP_SERVER_METHOD_INTERFACE_VERSION,
				      EAP_VENDOR_IETF, EAP_TYPE_MD5, "MD5");
	if (eap == NULL)
		return -1;

	eap->init = eap_md5_init;
	eap->reset = eap_md5_reset;
	eap->buildReq = eap_md5_buildReq;
	eap->check = eap_md5_check;
	eap->process = eap_md5_process;
	eap->isDone = eap_md5_isDone;
	eap->isSuccess = eap_md5_isSuccess;

    // 注册md5类型的eap方法
	ret = eap_server_method_register(eap);
	if (ret)
		eap_server_method_free(eap);
	return ret;
}
