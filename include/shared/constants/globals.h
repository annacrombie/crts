#ifndef CRTS_GLOBALS_H
#define CRTS_GLOBALS_H

#include "shared/sim/action.h"
#include "shared/sim/ent.h"

struct global_cfg_t {
	const struct {
		const char *name;
		const uint16_t max_workers;
		const uint16_t min_workers;
		const uint16_t completed_at;
		const uint16_t satisfaction;
	} actions[action_type_count];

	const struct {
		const char *name;
		const bool animate;
	} ents[ent_type_count];
};

extern const struct global_cfg_t gcfg;
#endif