#include "posix.h"

#ifndef CRTS_SERVER
#define CRTS_SERVER
#endif

#include <assert.h>
#include <math.h>
#include <stdint.h>
#include <stdlib.h>

#include "server/sim/ent.h"
#include "server/sim/terrain.h"
#include "shared/constants/globals.h"
#include "shared/math/geom.h"
#include "shared/math/perlin.h"
#include "shared/math/rand.h"
#include "shared/sim/chunk.h"
#include "shared/sim/world.h"
#include "shared/types/hash.h"
#include "shared/types/hdarr.h"
#include "shared/util/log.h"
#include "shared/util/mem.h"

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

bool
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

bool
find_tile(enum tile t, struct chunks *cnks, const struct rectangle *rect,
	const struct point *start, struct point *p, struct hash *skip)
{
	struct point q, r, c = { 0, 0 };
	uint32_t dist, cdist = UINT32_MAX;
	bool found = false;

	for (p->x = rect->pos.x; p->x < rect->pos.x + (int64_t)rect->width; ++p->x) {
		for (p->y = rect->pos.y; p->y < rect->pos.y + (int64_t)rect->height; ++p->y) {
			q = nearest_chunk(p);
			r = point_sub(p, &q);

			if (get_chunk(cnks, &q)->tiles[r.x][r.y] == t) {
				if (skip != NULL && hash_get(skip, p) != NULL) {
					continue;
				}

				found = true;
				dist = square_dist(start, p);
				if (dist < cdist) {
					cdist = dist;
					c = *p;
				}
			}
		}
	}

	*p = c;
	return found;
}

enum tile
get_tile_at(struct chunks *cnks, const struct point *p)
{
	struct chunk *ck = get_chunk_at(cnks, p);
	struct point rp = point_sub(p, &ck->pos);

	return ck->tiles[rp.x][rp.y];
}

bool
find_adj_tile(struct chunks *cnks, struct point *s, struct point *rp,
	struct rectangle *r, enum tile t, uint8_t et, uint8_t reject[4],
	bool (*pred)(enum tile t, uint8_t et))
{
	enum tile tt;
	struct point p[4] = {
		{ s->x + 1, s->y     },
		{ s->x - 1, s->y     },
		{ s->x,     s->y + 1 },
		{ s->x,     s->y - 1 },
	};
	size_t i;

	for (i = 0; i < 4; ++i) {
		if (r && !point_in_rect(&p[i], r)) {
			continue;
		} else if (reject && reject[i]) {
			continue;
		}

		tt = get_tile_at(cnks, &p[i]);

		if (tt == t || (pred && pred(tt, et))) {
			if (rp) {
				*rp = p[i];
			}
			return true;
		}
	}

	return false;
}


bool
tile_is_traversable(enum tile t, uint8_t trav)
{
	return gcfg.tiles[t].trav_type & trav;
}

bool
is_traversable(struct chunks *cnks, const struct point *p, uint8_t t)
{
	return tile_is_traversable(get_tile_at(cnks, p), t);
}

void
touch_chunk(struct chunks *cnks, struct chunk *ck)
{
	ck->last_touched = ++cnks->chunk_date;
	ck->touched_this_tick |= true;
}

static void
commit_tile(struct chunks *cnks, const struct point *p, enum tile t)
{
	struct chunk *ck = get_chunk_at(cnks, p);
	struct point rp = point_sub(p, &ck->pos);

	assert(rp.x >= 0 && rp.x < CHUNK_SIZE);
	assert(rp.y >= 0 && rp.y < CHUNK_SIZE);

	if (t == ck->tiles[rp.x][rp.y]) {
		return;
	}

	if (ck->tiles[rp.x][rp.y] == tile_mountain) {
		ck->heights[rp.x][rp.y] -= 2.0;
	}

	if (gcfg.tiles[ck->tiles[rp.x][rp.y]].functional) {
		hash_unset(cnks->functional_tiles, p);
	}

	ck->tiles[rp.x][rp.y] = t;
	ck->harvested[rp.x][rp.y] = 0;

	touch_chunk(cnks, ck);
}

void
update_tile(struct chunks *cnks, const struct point *p, enum tile t)
{
	assert(!gcfg.tiles[t].functional);

	commit_tile(cnks, p, t);

}

void
update_functional_tile(struct chunks *cnks, const struct point *p, enum tile t,
	uint16_t mot, uint32_t age)
{
	assert(gcfg.tiles[t].functional);

	commit_tile(cnks, p, t);

	union functional_tile ft = { .ft = { .type = t, .motivator = mot,
					     .age = age } };

	hash_set(cnks->functional_tiles, p, ft.val);
}
