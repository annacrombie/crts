#include "posix.h"

#include <stdlib.h>

#include "server/sim/ent.h"
#include "server/sim/environment.h"
#include "server/sim/sim.h"
#include "server/sim/update_tile.h"
#include "shared/constants/globals.h"
#include "shared/math/rand.h"
#include "shared/sim/tiles.h"
#include "shared/util/log.h"
#include "tracy.h"

static uint32_t
determine_grow_chance(struct chunk *ck, int32_t x, int32_t y, enum tile t)
{
	struct point p[4] = {
		{ x + 1, y     },
		{ x - 1, y     },
		{ x,     y + 1 },
		{ x,     y - 1 },
	};
	uint8_t adj = 0;
	size_t i;

	enum tile trigger = gcfg.tiles[t].next_to;
	if (!trigger) {
		trigger = t;
	}

	for (i = 0; i < 4; ++i) {
		if (p[i].x < 0 || p[i].x >= CHUNK_SIZE || p[i].y < 0 || p[i].y >= CHUNK_SIZE) {
			continue;
		}

		if (trigger == ck->tiles[p[i].x][p[i].y]) {
			++adj;
		}
	}

	return adj > 0 ? gcfg.misc.terrain_base_adj_grow_chance / adj
		: gcfg.misc.terrain_base_not_adj_grow_chance;
}

static bool
age_chunk(struct chunk *ck)
{
	enum tile t, nt;
	struct point c;
	uint32_t chance;
	bool updated = false;

	for (c.x = 0; c.x < CHUNK_SIZE; ++c.x) {
		for (c.y = 0; c.y < CHUNK_SIZE; ++c.y) {
			t = ck->tiles[c.x][c.y];

			if (!(nt = gcfg.tiles[t].next)) {
				continue;
			}

			chance = determine_grow_chance(ck, c.x, c.y, t);

			if (chance == 0 || !rand_chance(chance)) {
				continue;
			}

			ck->tiles[c.x][c.y] = gcfg.tiles[t].next;
			updated = true;
		}
	}

	return updated;
}


static enum iteration_result
process_chunk(struct chunks *cnks, struct chunk *ck)
{
	if (age_chunk(ck)) {
		touch_chunk(cnks, ck);
	}

	return ir_cont;
}

static void
spawn_random_creature(struct simulation *sim, struct chunk *ck)
{
	struct point c;
	struct ent *spawn;
	int i, amnt;
	enum ent_type et = gcfg.misc.spawnable_ents[rand_uniform(SPAWNABLE_ENTS_LEN)];

	for (c.x = 0; c.x < CHUNK_SIZE; ++c.x) {
		for (c.y = 0; c.y < CHUNK_SIZE; ++c.y) {
			if (ck->tiles[c.x][c.y] == gcfg.ents[et].spawn_tile) {
				if (rand_chance(gcfg.ents[et].spawn_chance)) {
					amnt = gcfg.ents[et].group_size;

					for (i = 0; i < amnt; ++i) {
						spawn = spawn_ent(sim->world);
						spawn->type = et;
						spawn->pos = point_add(&c, &ck->pos);
					}
				}
			}
		}
	}
}

static void
burn_spread(struct world *w, struct point *p)
{
	size_t i;
	struct point c[4] = {
		{ p->x + 1, p->y     },
		{ p->x - 1, p->y     },
		{ p->x,     p->y + 1 },
		{ p->x,     p->y - 1 },
	};

	for (i = 0; i < 4; ++i) {
		if (gcfg.tiles[get_tile_at(&w->chunks, &c[i])].flamable) {
			if (rand_chance(gcfg.misc.fire_spread_ignite_chance)) {
				update_functional_tile(w, &c[i], tile_fire, 0, 0);
			}
		}

	}
}

static void
process_random_chunk(struct simulation *sim)
{
	size_t ri;

	if ((ri = hdarr_len(&sim->world->chunks.hd)) == 0) {
		return;
	}

	ri = rand_uniform(ri);

	struct chunk *ck = hdarr_get_by_i(&sim->world->chunks.hd, ri);

	process_chunk(&sim->world->chunks, ck);

	spawn_random_creature(sim, ck);
}

struct find_food_ctx {
	struct circle *range;
};

static bool
find_food_pred(void *_ctx, struct ent *e)
{
	struct find_food_ctx *ctx = _ctx;

	return (e->type == et_resource_meat || e->type == et_resource_crop)
	       && (ctx->range ? point_in_circle(&e->pos, ctx->range) : true);
}

struct ent *
find_food(struct world *w, struct point *p, struct circle *c)
{
	struct find_food_ctx ctx = { c };

	return find_ent(w, p, &ctx, find_food_pred);
}

static enum iteration_result
process_functional_tiles(void *_sim, void *_p, uint64_t val)
{
	struct point *p = _p /*, q */;
	/* struct circle c; */
	struct simulation *sim = _sim;
	/* struct ent *e; */

	union functional_tile ft = { .val = val };

	switch (ft.ft.type) {
	case tile_fire:
		if (ft.ft.age > gcfg.misc.fire_spread_rate &&
		    rand_chance(gcfg.misc.fire_spread_chance)) {
			burn_spread(sim->world, p);
			update_tile(sim->world, p, tile_ash);
		} else {
			update_functional_tile(sim->world, p,
				tile_fire, 0, ft.ft.age + 1);
		}
		break;
	default:
		break;
	}

	return ir_cont;
}

void
process_environment(struct simulation *sim)
{
	TracyCZoneAutoS;
	process_random_chunk(sim);

	//hdarr_for_each(sim->world->chunks->hd, sim->world->chunks, process_chunk);

	struct hash tmp = sim->world->chunks.functional_tiles;
	/* struct hash buf = sim->world->chunks.functional_tiles_buf; */

	sim->world->chunks.functional_tiles = sim->world->chunks.functional_tiles_buf;

	hash_for_each_with_keys(&tmp, sim, process_functional_tiles);

	hash_clear(&tmp);
	sim->world->chunks.functional_tiles_buf = tmp;
	TracyCZoneAutoE;
}
