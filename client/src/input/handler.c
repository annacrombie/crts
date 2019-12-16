#include <stdlib.h>
#include <curses.h>

#include "keycodes.h"
#include "util/log.h"
#include "handler.h"
#include "move_handler.h"
#include "action_handler.h"

static void do_nothing(void *_)
{
}

static void (*const kc_func[KEY_COMMANDS])(void *) = {
	[kc_none]                 = do_nothing,
	[kc_invalid]              = do_nothing,
	[kc_view_up]              = view_up,
	[kc_view_down]            = view_down,
	[kc_view_left]            = view_left,
	[kc_view_right]           = view_right,
	[kc_enter_selection_mode] = set_input_mode_select,
	[kc_enter_normal_mode]    = set_input_mode_normal,
	[kc_quit]                 = end_simulation,
	[kc_cursor_up]            = cursor_up,
	[kc_cursor_down]          = cursor_down,
	[kc_cursor_left]          = cursor_left,
	[kc_cursor_right]         = cursor_right,
	[kc_create_move_action]   = create_move_action,
};

static unsigned transform_key(unsigned k)
{
	switch (k) {
	case KEY_UP:
		return skc_up;
	case KEY_DOWN:
		return skc_down;
	case KEY_LEFT:
		return skc_left;
	case KEY_RIGHT:
		return skc_right;
	default:
		return k;
	}
}

struct keymap *handle_input(struct keymap *km, unsigned k, struct display *sim)
{
	k = transform_key(k);

	if (k > ASCII_RANGE)
		return NULL;

	km = &km->map[k];

	if (km->map == NULL) {
		kc_func[km->cmd](sim);
		km = NULL;
	}

	return km;
}
