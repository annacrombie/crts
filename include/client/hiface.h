#ifndef __DISPLAY_H
#define __DISPLAY_H

#include <stdint.h>

#include "client/input/modes.h"
#include "client/sim.h"
#include "shared/net/net_ctx.h"
#include "shared/sim/action.h"
#include "shared/types/geom.h"

struct hiface_buf {
	char buf[16];
	size_t len;
};

struct hiface {
	struct hiface_buf num;
	struct hiface_buf cmd;
	struct simulation *sim;
	struct net_ctx *nx;
	struct point cursor;
	struct point view;
	enum input_mode im;
	struct keymap *km;
	uint32_t redrew_world;
	uint64_t server_timeout;

	struct action next_act;
	bool next_act_changed;
};

struct hiface *hiface_init(struct simulation *sim);
long hiface_get_num(struct hiface *hif, long def);
#endif
