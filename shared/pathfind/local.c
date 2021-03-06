#include "posix.h"

#include <string.h>

#include "shared/pathfind/abstract.h"
#include "shared/pathfind/local.h"
#include "shared/pathfind/macros.h"
#include "shared/util/log.h"
#include "tracy.h"

#define CHUNK_SIZE 16

struct ag_local_heap_e { uint32_t d; uint8_t i; };

struct ag_mini_g {
	uint8_t prev[CHUNK_SIZE * CHUNK_SIZE];
	uint8_t d[CHUNK_SIZE * CHUNK_SIZE];
	uint8_t visited[(CHUNK_SIZE * CHUNK_SIZE) / 8];
	struct ag_local_heap_e heap[CHUNK_PERIM];
	struct { uint8_t x; uint8_t y; } goal;
	uint8_t heap_len;
};

static void
check_neighbour(struct ag_mini_g *g, uint8_t c, uint8_t n)
{
	if (g->d[c] <= g->d[n]) {
		g->d[n] = g->d[c] + 1;
		g->prev[n] = c;

		if (!SB1_GET(g->visited, n)) {
			int32_t dx = g->goal.x - (n >> 4),
				dy = g->goal.y - (n & 15);

			++g->heap_len;
			assert(g->heap_len < CHUNK_PERIM);

			g->heap[g->heap_len - 1] = (struct ag_local_heap_e){
				.d = g->d[n] + (dx * dx) + (dy * dy),
				.i = n
			};

			SB1_SET(g->visited, n, 1);
		}
	}
}

#ifndef NDEBUG
void
print_astar_local_path(struct ag_mini_g *g, uint8_t s, uint8_t goal,
	uint8_t path[MAXPATH_LOCAL], uint8_t plen)
{
	char c, to_print[CHUNK_SIZE][CHUNK_SIZE + 1] = { 0 };
	uint8_t v;
	uint32_t i, j, found;

	for (i = 0; i < CHUNK_SIZE * CHUNK_SIZE; ++i) {
		found = false;

		for (j = 0; j < plen; ++j) {
			if (path[j] == i) {
				found = true;
				break;
			}
		}

		v = SB1_GET(g->visited, i);

		if (i == s) {
			c = 's';
		} else if (i == goal) {
			c = 'g';
		} else {
			c = found ? '$' : (v ? '+' : '_');
		}

		to_print[i / 16][i % 16] = c;
	}


	L(log_pathfind, "path:");
	for (i = 0; i < CHUNK_SIZE; ++i) {
		L(log_pathfind, "%s", to_print[i]);
	}
}
#endif

bool
astar_local_possible(const struct ag_component *agc, uint8_t s, uint8_t goal)
{
	uint8_t region;

	return (region = SB4_GET(agc->region_map, s)) == SB4_GET(agc->region_map, goal)
	       && region != NULL_REGION;
}

bool
astar_local(const struct ag_component *agc, uint8_t s, uint8_t goal,
	uint8_t path[MAXPATH_LOCAL], uint8_t *pathlen)
{
	TracyCZoneAutoS;

	if (s == goal) {
		path[0] = goal;
		*pathlen = 1;
		TracyCZoneAutoE;
		return true;
	} else if (!astar_local_possible(agc, s, goal)) {
		return false;
	}

	struct ag_mini_g g = { .heap = { { .i = s } }, .heap_len = 1 };

	/* L(log_pathfind, "connecting (%d, %d), and (%d, %d)", s % 16, s / 16, goal % 16, goal / 16); */

	/* NOTE: wont work if the type of g.d is changed */
	memset(g.d, UINT8_MAX, CHUNK_SIZE * CHUNK_SIZE);
	g.d[s] = 0;
	SB1_SET(g.visited, s, 1);

	g.goal.x = goal >> 4;
	g.goal.y = goal & 15;

	uint8_t cur, plen = 0;

	uint8_t i;
	uint16_t min = -1;
	uint32_t minv = UINT32_MAX;

	while (g.heap_len) {
		for (i = 0; i < g.heap_len; ++i) {
			if (g.heap[i].d < minv) {
				minv = g.heap[i].d;
				min = i;
			}
		}

		assert(min < 256);

		if ((cur = g.heap[min].i) == goal) {
			goto found;
		}

		--g.heap_len;

		if (min != g.heap_len) {
			g.heap[min] = g.heap[g.heap_len];
		}

		min = -1;
		minv = UINT32_MAX;

/* 		uint8_t v = EDGE_GET(g.edges, cur); */
/* 		L(log_pathfind, "%d | %d, %d %c%c%c%c | %d | %d", cur, cur % 16, cur / 16, */
/* 			v & (1 << 0) ? '1' : '0', */
/* 			v & (1 << 1) ? '1' : '0', */
/* 			v & (1 << 2) ? '1' : '0', */
/* 			v & (1 << 3) ? '1' : '0', */
/* 			g.d[cur], */
/* 			g.heap_len */
/* 			); */

		if ((SB4_GET(agc->adj_map, cur) & (1 << 0))) {
			check_neighbour(&g, cur, LEFT_OF(cur));
		}

		if ((SB4_GET(agc->adj_map, cur) & (1 << 1))) {
			check_neighbour(&g, cur, BELOW(cur));
		}

		if ((SB4_GET(agc->adj_map, cur) & (1 << 2))) {
			check_neighbour(&g, cur, RIGHT_OF(cur));
		}

		if ((SB4_GET(agc->adj_map, cur) & (1 << 3))) {
			check_neighbour(&g, cur, ABOVE(cur));
		}
	}

	/* print_astar_local_path(&g, s, goal, path, plen); */

	TracyCZoneAutoE;
	return false;

found:
	path[plen] = goal;
	cur = g.prev[goal];
	++plen;

	while (cur != s) {
		path[plen] = cur;
		cur = g.prev[cur];
		++plen;
		assert(plen < MAXPATH_LOCAL);
	}

	path[plen] = s;
	++plen;

	*pathlen = plen;

	/* print_astar_local_path(&g, s, goal, path, plen); */

	TracyCZoneAutoE;
	return true;
}
