#ifndef IP_ADDR_H
#define IP_ADDR_H

#include "common/common.h"

struct ip_addr {
	int af; /* AF_INET / AF_INET6 */
	union {
		struct in_addr v4;
#ifdef CONFIG_IPV6
		struct in6_addr v6;
#endif /* CONFIG_IPV6 */
		uint8_t max_len[16];
	} u;
};

const char * ip_txt(const struct ip_addr *addr, char *buf,
			    size_t buflen);
int ip_diff(struct ip_addr *a, struct ip_addr *b);
int parse_ip_addr(const char *txt, struct ip_addr *addr);

#endif /* IP_ADDR_H */
