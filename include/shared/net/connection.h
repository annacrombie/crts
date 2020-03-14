#ifndef SHARED_NET_CONNECTION_H
#define SHARED_NET_CONNECTION_H

#include <stdbool.h>

#include "shared/net/defs.h"
#include "shared/net/ack.h"

struct connection {
	union {
		struct sockaddr_in ia;
		struct sockaddr sa;
	} addr;

	uint32_t stale;
	uint16_t motivator;
	msg_ack_t bit;
	bool new;

	struct acks acks;
};

void cx_inspect(const struct connection *c);
void cx_init(struct connection *c, const struct sockaddr_in *addr);
#endif