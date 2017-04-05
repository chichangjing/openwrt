/*
 * CHAP-MD5 (RFC 1994)
 * Copyright (c) 2007-2009, Jouni Malinen <j@w1.fi>
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
#include "crypto/crypto.h"
#include "chap.h"

int chap_md5(uint8_t id, const uint8_t *secret, size_t secret_len, const uint8_t *challenge,
	      size_t challenge_len, uint8_t *response)
{
	const uint8_t *addr[3];
	size_t len[3];

	addr[0] = &id;
	len[0] = 1;
	addr[1] = secret;
	len[1] = secret_len;
	addr[2] = challenge;
	len[2] = challenge_len;
	return md5_vector(3, addr, len, response);
}
