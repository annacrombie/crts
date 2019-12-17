#ifndef __SERVER_CX_H
#define __SERVER_CX_H
#include <arpa/inet.h>
#include "types/queue.h"

struct server_cx {
	union {
		struct sockaddr_in ia;
		struct sockaddr sa;
	} server_addr;

	int sock;

	struct queue *inbound;
	struct queue *outbound;
};

extern socklen_t socklen;
void server_cx_init(struct server_cx *s, const char *ipv4addr);
#endif
