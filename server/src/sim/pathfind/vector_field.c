#include <limits.h>
#include "mapping.h"
#include "vector_field.h"

void calculate_path_vector(struct path_graph *pg, struct node *n)
{
	int i, min, mini;
	struct node *c;

	n->flow.x = n->flow.y = 0;
	min = INT_MAX;

	get_adjacent(pg, n);

	for (i = 0; i < 4; i++) {
		if (n->adj[i] < 0)
			continue;

		c = pg->nodes.e + n->adj[i];

		if (c->path_dist < min) {
			mini = i;
			min = c->path_dist;
		}
	}

	if (min <= 0)
		return;

	switch (mini) {
	case 0: n->flow.x =  1; break;
	case 1: n->flow.x = -1; break;
	case 2: n->flow.y =  1; break;
	case 3: n->flow.y = -1; break;
	}

	n->flow_calcd = 1;
}
