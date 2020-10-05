#ifndef CLIENT_NET_H
#define CLIENT_NET_H

#include <stdint.h>

struct c_simulation;

struct net_ctx *net_init(struct c_simulation *sim);
void set_server_address(const char *host);
void net_set_outbound_id(long id);
void check_add_server_cx(struct net_ctx *nx);
#endif
