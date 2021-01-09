#ifndef LAUNCHER_LAUNCHER_H
#define LAUNCHER_LAUNCHER_H

#ifdef CRTS_HAVE_client
#include "client/client.h"
#endif

#ifdef CRTS_HAVE_server
#include "server/server.h"
#endif

#include "shared/platform/sockets/common.h"
#include "shared/sim/world.h"

enum mode {
	mode_server   = 1 << 0,
	mode_client   = 1 << 1,
	mode_online   = 1 << 2,
	mode_terragen = 1 << 3,
};

struct runtime {
#ifdef CRTS_HAVE_server
	struct server *server;
#endif
#ifdef CRTS_HAVE_client
	struct client *client;
#endif
	struct sock_addr *server_addr;
	void ((*tick)(struct runtime*));
	bool *run;
};

struct launcher_opts {
	struct world_loader wl;
	struct {
		const char *ip;
		uint16_t port;
	} net_addr;
	enum mode mode;
};

struct opts {
	struct launcher_opts launcher;
#ifdef CRTS_HAVE_client
	struct client_opts client;
#endif
#ifdef CRTS_HAVE_server
	struct server_opts server;
#endif
};
#endif
