#include <limits.h>

#include "mapping.h"
#include "heap.h"
#include "pgraph.h"
#include "types/hash.h"
#include "util/mem.h"

struct node *pgraph_lookup(const struct path_graph *g, const struct point *p)
{
	const int *i;

	if ((i = hash_get(g->hash.h, p)) != NULL)
		return g->nodes.e + *i;
	else
		return NULL;
}

int find_or_create_node(struct path_graph *pg, const struct point *p)
{
	struct node *n;
	int *i;

	if ((n = pgraph_lookup(pg, p)) == NULL) {
		n = get_mem((void**)&pg->nodes.e, sizeof(struct node), &pg->nodes.len, &pg->nodes.cap);

		n->p = *p;
		n->path_dist = INT_MAX;
		n->h_dist = INT_MAX;
		n->visited = 0;
		n->flow_calcd = 0;
		n->trav = pg->trav_getter(pg, n);

		i = get_mem((void**)&pg->hash.e, sizeof(int), &pg->hash.len, &pg->hash.cap);
		hash_set(pg->hash.h, p, i);
	}

	return n - pg->nodes.e;
}

void pgraph_create(struct path_graph *pg,
		   struct hash *cnks,
		   const struct point *goal,
		   int (*trav_getter)(struct path_graph *g, struct node *n),
		   int res)
{
	struct node *n;
	int i;

	pg->chunks = cnks;
	pg->trav_getter = trav_getter;
	pg->res = res;

	pg->hash.h = hash_init(sizeof(struct point));
	pg->hash.cap = pg->hash.cap;
	pg->hash.e = calloc(pg->hash.cap, sizeof(int));

	heap_init(pg);

	n = pg->nodes.e + find_or_create_node(pg, goal);
	n->path_dist = 0;
	heap_push(pg, n);

	get_adjacent(pg, n);
	for (i = 0; i < 4; i++) {
		if (n->adj[i] == NULL_NODE)
			continue;
		n = pg->nodes.e + n->adj[i];

		n->path_dist = 0;
		heap_push(pg, n);
	}
}

