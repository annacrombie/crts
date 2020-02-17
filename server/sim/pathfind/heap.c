#include "server/sim/pathfind/heap.h"
#include "server/sim/pathfind/pg_node.h"
#include "shared/util/mem.h"

static int
heap_compare(const void *const ctx, const void *const a, const void *const b)
{
	const struct pgraph *g = ctx;
	const struct pg_node *na, *nb;

	na = g->nodes.e + *(uint16_t *)a;
	nb = g->nodes.e + *(uint16_t *)b;

	return na->h_dist < nb->h_dist;
}

static void
heap_move(void *const dst, const void *const src)
{
	uint16_t tmp = *(uint16_t *)dst;

	*(uint16_t *)dst = *(uint16_t *)src;
	*(uint16_t *)src = tmp;
}

void
heap_sort(struct pgraph *pg)
{
	gheap_make_heap(&pg->heap.ctx, pg->heap.e, pg->heap.len);
	gheap_sort_heap(&pg->heap.ctx, pg->heap.e, pg->heap.len);
}

uint16_t
heap_push(struct pgraph *pg, const struct pg_node *n)
{
	union {
		void **vp;
		uint16_t **ip;
	} ints = { .ip = &pg->heap.e };

	uint16_t off = get_mem(ints.vp, sizeof(uint16_t), &pg->heap.len, &pg->heap.cap);
	uint16_t *ip = off + pg->heap.e;

	*ip = n - pg->nodes.e;

	return *ip;
}

uint16_t
heap_pop(struct pgraph *pg)
{
	if (pg->heap.len <= 0) {
		return 0;
	}

	pg->heap.len--;
	pg->heap.e[0] = pg->heap.e[pg->heap.len];

	return 0;
}

struct pg_node *
heap_peek(const struct pgraph *pg)
{
	if (pg->heap.len <= 0) {
		return NULL;
	}

	return pg->nodes.e + pg->heap.e[0];
}

void
heap_init(struct pgraph *pg)
{
	pg->heap.ctx.fanout = 2;
	pg->heap.ctx.page_chunks = 1;
	pg->heap.ctx.item_size = sizeof(uint16_t);
	pg->heap.ctx.less_comparer = &heap_compare;
	pg->heap.ctx.less_comparer_ctx = pg;
	pg->heap.ctx.item_mover = &heap_move;
}