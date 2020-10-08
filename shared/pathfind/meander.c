#include "posix.h"

#include <stdlib.h>

#include "shared/math/rand.h"
#include "shared/pathfind/meander.h"
#include "shared/sim/tiles.h"
#include "shared/util/log.h"

void
meander(struct chunks *cnks, struct point *pos, uint8_t trav)
{
	uint8_t choice = rand_uniform(4);
	struct point np = *pos;

	switch (choice) {
	case 0:
		np.x += 1;
		break;
	case 1:
		np.x -= 1;
		break;
	case 2:
		np.y += 1;
		break;
	case 3:
		np.y -= 1;
		break;
	}

	if (is_traversable(cnks, &np, trav)) {
		*pos = np;
	}
}