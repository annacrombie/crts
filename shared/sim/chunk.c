#include <stdlib.h>
#include <string.h>
#include "util/log.h"
#include "sim/chunk.h"

void
chunks_init(struct chunks **cnks)
{
	if (*cnks == NULL) {
		*cnks = malloc(sizeof(struct chunks));
	}

	memset(*cnks, 0, sizeof(struct chunks));

	(*cnks)->h = hash_init(2048, 6, sizeof(struct point));
}

void
chunk_init(struct chunk **c)
{
	if (*c == NULL) {
		*c = malloc(sizeof(struct chunks));
	}

	memset(*c, 0, sizeof(struct chunk));

	(*c)->empty = 1;
}

static int
roundto(int i, int nearest)
{
	int m = i % nearest;

	return m >= 0 ? i - m : i - (nearest + m);
}

struct point
nearest_chunk(const struct point *p)
{
	struct point q = {
		roundto(p->x, CHUNK_SIZE),
		roundto(p->y, CHUNK_SIZE)
	};

	return q;
}
