/*
 * FIPS 186-2 PRF for internal crypto implementation
 * Copyright (c) 2006-2007, Jouni Malinen <j@w1.fi>
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
#include "sha1.h"
#include "sha1_i.h"
#include "crypto.h"


int fips186_2_prf(const uint8_t *seed, size_t seed_len, uint8_t *x, size_t xlen)
{
	uint8_t xkey[64];
	uint32_t t[5], _t[5];
	int i, j, m, k;
	uint8_t *xpos = x;
	uint32_t carry;

	if (seed_len > sizeof(xkey))
		seed_len = sizeof(xkey);

	/* FIPS 186-2 + change notice 1 */

	memcpy(xkey, seed, seed_len);
	memset(xkey + seed_len, 0, 64 - seed_len);
	t[0] = 0x67452301;
	t[1] = 0xEFCDAB89;
	t[2] = 0x98BADCFE;
	t[3] = 0x10325476;
	t[4] = 0xC3D2E1F0;

	m = xlen / 40;
	for (j = 0; j < m; j++) {
		/* XSEED_j = 0 */
		for (i = 0; i < 2; i++) {
			/* XVAL = (XKEY + XSEED_j) mod 2^b */

			/* w_i = G(t, XVAL) */
			memcpy(_t, t, 20);
			SHA1Transform(_t, xkey);
			_t[0] = host_to_be32(_t[0]);
			_t[1] = host_to_be32(_t[1]);
			_t[2] = host_to_be32(_t[2]);
			_t[3] = host_to_be32(_t[3]);
			_t[4] = host_to_be32(_t[4]);
			memcpy(xpos, _t, 20);

			/* XKEY = (1 + XKEY + w_i) mod 2^b */
			carry = 1;
			for (k = 19; k >= 0; k--) {
				carry += xkey[k] + xpos[k];
				xkey[k] = carry & 0xff;
				carry >>= 8;
			}

			xpos += SHA1_MAC_LEN;
		}
		/* x_j = w_0|w_1 */
	}

	return 0;
}
