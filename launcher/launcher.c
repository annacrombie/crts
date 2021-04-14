#include "posix.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "launcher/assets.h"
#include "launcher/launcher.h"
#include "launcher/opts.h"
#include "shared/msgr/transport/basic.h"
#include "shared/msgr/transport/rudp.h"
#include "shared/platform/sockets/common.h"
#include "shared/util/log.h"
#include "shared/util/timer.h"
#include "tracy.h"

#ifdef CRTS_HAVE_client
#include "client/client.h"
#endif

#ifdef CRTS_HAVE_server
#include "server/server.h"
#endif

#ifdef CRTS_HAVE_terragen
#include "terragen/terragen.h"
#endif

const float dt = 1.0f / 30.0f;

static void
main_loop(struct runtime *rt)
{
	struct timer timer;
	timer_init(&timer);

	float client_tick_time = 0, server_tick_time = 0;
	float simtime = 0;

	while (*rt->run) {
		TracyCFrameMark;

		if (rt->server) {
#ifdef CRTS_HAVE_server
			uint32_t ticks = simtime / dt;
			if (ticks) {
				server_tick(rt->server, ticks);
			}
			simtime -= dt * ticks;

			server_tick_time = timer_lap(&timer);
			simtime += server_tick_time;
#endif
		}

		if (rt->client) {
#ifdef CRTS_HAVE_client
			client_tick(rt->client);

			if (rt->server_addr) {
				rudp_connect(rt->client->msgr, rt->server_addr);
			}

			client_tick_time = timer_lap(&timer);
			simtime += client_tick_time;
#ifndef NDEBUG
			timer_sma_push(&rt->client->prof.client_tick, client_tick_time);
			timer_sma_push(&rt->client->prof.server_tick, server_tick_time);
#endif
#endif
		} else {
			/* throttle tick rate if we are only running the server */
			static struct timespec tick = { .tv_nsec = (dt / 2) * 1000000000 };
			nanosleep(&tick, NULL);
		}
	}
}

int
main(int argc, char *const argv[])
{
	log_init();
	launcher_assets_init();

	static bool always_true = true;
	struct runtime rt = { .run = &always_true };
	struct opts opts = { 0 };

	if (!parse_opts(argc, argv, &opts)) {
		return 1;
	}

#if defined(CRTS_HAVE_client) || defined(CRTS_HAVE_server)
	const struct sock_impl *socks = NULL;

	if (opts.launcher.mode & mode_online) {
		socks = get_sock_impl(sock_impl_type_system);
	}
#endif

#ifdef CRTS_HAVE_terragen
	if (opts.launcher.mode & mode_terragen) {
		terragen_main(&opts.terragen);
		return 0;
	}
#endif

#ifdef CRTS_HAVE_server
	if (opts.launcher.mode & mode_server) {
		static struct server server = { 0 };
		rt.server = &server;

		if (!init_server(rt.server, &opts.launcher.wl, &opts.server)) {
			return 1;
		}

		if (opts.launcher.mode & mode_online) {
			struct sock_addr addr;
			socks->addr_init(&addr, opts.launcher.net_addr.port);

			static struct msgr_transport_rudp_ctx server_rudp_ctx = { 0 };
			if (!msgr_transport_init_rudp(&server_rudp_ctx, &rt.server->msgr, socks, &addr)) {
				return 1;
			}
		}
	}
#endif

#ifdef CRTS_HAVE_client
	if (opts.launcher.mode & mode_client) {
		static struct client client = { 0 };
		rt.client = &client;

		if (!init_client(rt.client, &opts.client)) {
			return 1;
		}

		if (opts.launcher.mode & mode_online) {
			static struct sock_addr server_addr = { 0 };
			rt.server_addr = &server_addr;

			socks->addr_init(rt.server_addr, opts.launcher.net_addr.port);
			if (!socks->resolve(rt.server_addr, opts.launcher.net_addr.ip)) {
				return 1;
			}

			struct sock_addr addr = { 0 };
			static struct msgr_transport_rudp_ctx client_rudp_ctx = { 0 };
			if (!msgr_transport_init_rudp(&client_rudp_ctx, rt.client->msgr, socks, &addr)) {
				return 1;
			}
		}

		rt.run = &rt.client->run;
	}
#endif

#if defined(CRTS_HAVE_client) && defined(CRTS_HAVE_server)
	if (!(opts.launcher.mode & mode_online)) {
		static struct msgr_transport_basic_ctx transport_basic_ctx[2] = { 0 };

		msgr_transport_init_basic(rt.client->msgr, &rt.server->msgr,
			&transport_basic_ctx[0]);
		msgr_transport_init_basic(&rt.server->msgr, rt.client->msgr,
			&transport_basic_ctx[1]);
	}
#endif

	main_loop(&rt);

#ifdef CRTS_HAVE_client
	if (opts.launcher.mode & mode_client) {
		deinit_client(rt.client);
	}
#endif
}
