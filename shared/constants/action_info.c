#include "sim/action.h"
#include "constants/action_info.h"

const struct action_info ACTIONS[] = {
	/*                  name         maxw   minw   diff.  satis.  */
	[at_none]       = { "nothing",      0,     0,     0,      0 },
	[at_move]       = { "move",        20,    20,     1,    100 }
};