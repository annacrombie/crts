#ifndef SHARED_SERIALIZE_LIMITS_H
#define SHARED_SERIALIZE_LIMITS_H

#include "shared/sim/action.h"

/* terrain height limits */
#define MAX_HEIGHT 64
#define MIN_HEIGHT -16
/* terrain height resolution */
#define STEPS ((MAX_HEIGHT - MIN_HEIGHT) * 10)
#define MAX_COORD  4096
#define MIN_COORD -4096

#define ACTION_TGT_LIM 64
#define ACTION_RANGE_MAXL (ACTION_RANGE_MAX_H + 1)

#endif
