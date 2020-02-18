#ifndef SERVER_SIM_SIM_H
#define SERVER_SIM_SIM_H

#include "shared/sim/world.h"
#include "shared/types/queue.h"

struct simulation {
	struct world *world;
	struct queue *inbound;
	struct queue *outbound;
	struct {
		struct sim_action *e;
		size_t len;
		size_t cap;
	} actions;

	struct pgraph *meander;

	size_t seq;
};

void populate(struct simulation *sim);
void simulate(struct simulation *sim);
struct simulation *sim_init(struct world *w);
enum pathfind_result pathfind_and_update(struct simulation *sim,
	struct pgraph *pg, struct ent *e);
#endif
