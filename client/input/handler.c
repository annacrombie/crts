#include <stdlib.h>
#include <curses.h>

#include "client/cfg/keymap.h"
#include "client/input/action_handler.h"
#include "client/input/handler.h"
#include "client/input/keycmd.h"
#include "client/input/keycodes.h"
#include "client/input/move_handler.h"
#include "shared/util/log.h"

static void
do_nothing(struct hiface *_)
{
}

static void(*const kc_func[KEY_COMMANDS])(struct hiface *) = {
	[kc_none]                 = do_nothing,
	[kc_invalid]              = do_nothing,
	[kc_center]               = center,
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
	[kc_action_move]          = action_move,
	[kc_action_harvest]       = action_harvest,
};

static unsigned
transform_key(unsigned k)
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

static void
hifb_clear(struct hiface_buf *buf)
{
	buf->len = 0;
	buf->buf[0] = '\0';
}

static void
hifb_append_char(struct hiface_buf *hbf, unsigned c)
{
	if (hbf->len >= sizeof(hbf->buf) - 1) {
		return;
	}

	hbf->buf[hbf->len] = c;
	hbf->buf[hbf->len + 1] = '\0';
	hbf->len++;
}

struct keymap *
handle_input(struct keymap *km, unsigned k, struct hiface *hif)
{
	k = transform_key(k);

	if (k > ASCII_RANGE) {
		return NULL;
	}

	if (k >= '0' && k <= '9') {
		hifb_append_char(&hif->num, k);
		return km;
	} else {
		hifb_append_char(&hif->cmd, k);
	}

	km = &km->map[k];

	if (km->map == NULL) {
		kc_func[km->cmd](hif);
		km = NULL;
		hifb_clear(&hif->num);
		hifb_clear(&hif->cmd);
	}

	return km;
}