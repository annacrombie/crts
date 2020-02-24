#ifndef __NET_CONNECTION_H
#define __NET_CONNECTION_H
#include <stdlib.h>
#include <arpa/inet.h>

struct connection {
	union {
		struct sockaddr_in ia;
		struct sockaddr sa;
	} addr;

	uint32_t stale;
	uint16_t motivator;
};

struct connection_pool {
	size_t len;
	size_t cap;
	int seq;
	struct connection *l;
};

void cx_inspect(const struct connection *c);
void cx_init(struct connection *c, const struct sockaddr_in *addr);
#endif
