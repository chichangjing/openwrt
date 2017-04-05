/*
 * SHA1 internal definitions
 * Copyright (c) 2003-2005, Jouni Malinen <j@w1.fi>
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

#ifndef SHA1_I_H
#define SHA1_I_H

struct SHA1Context {
	uint32_t state[5];
	uint32_t count[2];
	unsigned char buffer[64];
};

void SHA1Init(struct SHA1Context *context);
void SHA1Update(struct SHA1Context *context, const void *data, uint32_t len);
void SHA1Final(unsigned char digest[20], struct SHA1Context *context);
void SHA1Transform(uint32_t state[5], const unsigned char buffer[64]);

#endif /* SHA1_I_H */
