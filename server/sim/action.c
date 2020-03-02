#ifndef CRTS_SERVER
#define CRTS_SERVER
#endif

#define _XOPEN_SOURCE 500

#include <stdbool.h>
#include <string.h>

#include "server/sim/action.h"
#include "server/sim/sim.h"
#include "server/sim/pathfind/pgraph.h"
#include "shared/messaging/server_message.h"
#include "shared/util/log.h"
#include "shared/util/mem.h"

bool
action_index(const struct simulation *sim, uint8_t id, size_t *i)
{
	for (*i = 0; *i < sim->actions.len; (*i)++) {
		if (sim->actions.e[*i].act.id == id) {
			return true;
		}
	}

	return false;
}

struct sim_action *
action_get(const struct simulation *sim, uint8_t id)
{
	size_t i;

	if (action_index(sim, id, &i)) {
		return &sim->actions.e[i];
	} else {
		return NULL;
	}
}

struct sim_action *
action_add(struct simulation *sim, const struct action *act)
{
	struct sim_action *nact;
	size_t i;
	union {
		void **vp;
		struct sim_action **sp;
	} sa = { .sp = &sim->actions.e };

	get_mem(sa.vp, sizeof(struct sim_action), &sim->actions.len,
		&sim->actions.cap);

	i = sim->actions.len - 1;
	nact = &sim->actions.e[i];
	memset(nact, 0, sizeof(struct sim_action));

	if (act != NULL) {
		memcpy(&nact->act, act, sizeof(struct action));
	} else {
		action_init(&nact->act);
	}

	nact->act.id = sim->seq++;

	return nact;
}

void
action_del(struct simulation *sim, uint8_t id)
{
	size_t index;

	if (!action_index(sim, id, &index)) {
		return;
	}

	queue_push(sim->outbound,
		sm_create(server_message_rem_action, &sim->actions.e[index].act.id));

	pgraph_destroy(sim->actions.e[index].global);
	pgraph_destroy(sim->actions.e[index].local);

	size_t tail = sim->actions.len - 1;

	memmove(&sim->actions.e[index], &sim->actions.e[tail], sizeof(struct sim_action));
	memset(&sim->actions.e[tail], 0, sizeof(struct sim_action));
	sim->actions.len--;
}
