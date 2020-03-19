#ifndef __ACTION_H
#define __ACTION_H
#include <stddef.h>
#include <stdint.h>

#include "shared/math/geom.h"

enum action_type {
	at_none,
	at_move,
	at_harvest,
	at_build,
	at_fight,
	action_type_count
};

enum action_harvest_targets {
	aht_forest,
	aht_mountain,
	aht_wood,
	action_harvest_targets_count,
};

struct action {
	enum action_type type;
	struct circle range;
	uint8_t workers_requested;
	uint8_t id;
	uint16_t tgt;

#ifdef CRTS_SERVER
	uint8_t motivator;

	uint8_t workers_assigned;
	uint8_t workers_waiting;

	uint8_t completion;
#endif
};

void action_init(struct action *act);
void action_inspect(struct action *act);
#endif
