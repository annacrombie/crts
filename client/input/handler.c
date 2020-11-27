#include "posix.h"

#include <stdlib.h>
#include <string.h>

#include "client/cfg/keymap.h"
#include "client/input/action_handler.h"
#include "client/input/cmdline.h"
#include "client/input/handler.h"
#include "client/input/keymap.h"
#include "client/input/move_handler.h"
#include "shared/sim/action.h"
#include "shared/util/log.h"

#ifndef NDEBUG
#include "client/input/debug.h"
#endif

static void
do_nothing(struct hiface *_)
{
}

static void
end_simulation(struct hiface *d)
{
	if (d->keymap_describe) {
		hf_describe(d, kmc_sys, "end program");
		return;
	}

	d->sim->run = 0;
}

static void
set_input_mode(struct hiface *d)
{
	enum input_mode im = hiface_get_num(d, 0) % input_mode_count;

	if (d->keymap_describe) {
		hf_describe(d, kmc_sys, "enter %s mode", input_mode_names[im]);
	}

	d->im = im;
}

static void
toggle_help(struct hiface *d)
{
	if (d->keymap_describe) {
		hf_describe(d, kmc_sys, "toggle help");
		return;
	}

	d->display_help = !d->display_help;
}

static kc_func kc_funcs[key_command_count] = {
	[kc_none]                 = do_nothing,
	[kc_invalid]              = do_nothing,
	[kc_macro]                = do_nothing,
	[kc_center_cursor]        = center_cursor,
	[kc_view_up]              = view_up,
	[kc_view_down]            = view_down,
	[kc_view_left]            = view_left,
	[kc_view_right]           = view_right,
	[kc_find]                 = find,
	[kc_set_input_mode]       = set_input_mode,
	[kc_quit]                 = end_simulation,
	[kc_cursor_up]            = cursor_up,
	[kc_cursor_down]          = cursor_down,
	[kc_cursor_left]          = cursor_left,
	[kc_cursor_right]         = cursor_right,
	[kc_set_action_type]      = set_action_type,
	[kc_set_action_target]    = set_action_target,
	[kc_undo_action]          = undo_last_action,
	[kc_resize_selection]     = resize_selection,
	[kc_exec_action]          = exec_action,
	[kc_toggle_help]          = toggle_help,

#ifndef NDEBUG
	[kc_debug_pathfind_toggle] = debug_pathfind_toggle,
	[kc_debug_pathfind_place_point] = debug_pathfind_place_point,
#else
	[kc_debug_pathfind_toggle] = do_nothing,
	[kc_debug_pathfind_place_point] = do_nothing,
#endif
};

static void
hifb_clear(struct hiface_buf *buf)
{
	buf->len = 0;
	buf->buf[0] = '\0';
}

static void
do_macro(struct hiface *hif, char *macro)
{
	size_t i, len = strlen(macro);
	struct keymap *mkm = &hif->km[hif->im];

	//hifb_clear(&hif->num);
	//hifb_clear(&hif->cmd);

	for (i = 0; i < len; i++) {
		if ((mkm = handle_input(mkm, macro[i], hif)) == NULL) {
			mkm = &hif->km[hif->im];
		}
	}
}

void
trigger_cmd_with_num(enum key_command kc, struct hiface *hf, int32_t val)
{
	override_num_arg(hf, val);
	trigger_cmd(kc, hf);
}

void
trigger_cmd(enum key_command kc, struct hiface *hf)
{
	if (hf->resize.b) {
		hf->resize.oldcurs = hf->cursor;
		hf->cursor = hf->resize.tmpcurs;
	}

	kc_funcs[kc](hf);

	if (hf->resize.b) {
		hf->resize.tmpcurs = hf->cursor;
		hf->cursor = hf->resize.oldcurs;
		check_selection_resize(hf);
	}

	hifb_clear(&hf->num);

	hf->num_override.override = false;
	hf->num_override.val = 0;
}

static void exec_node(struct hiface *hf, struct keymap **mkm, struct kc_node *node);

static void
exec_macro(struct hiface *hf, struct kc_macro *macro)
{
	struct keymap *mkm = &hf->km[hf->im];
	uint8_t i;
	for (i = 0; i < macro->nodes; ++i) {
		exec_node(hf, &mkm, &macro->node[i]);
	}

}

static void
exec_node(struct hiface *hf, struct keymap **mkm, struct kc_node *node)
{
	switch (node->type) {
	case kcmnt_expr:
		/* L("node:expr:%d", node->val.expr.kc); */
		if (node->val.expr.argc) {
			trigger_cmd_with_num(node->val.expr.kc, hf, node->val.expr.argv[0]);
		} else {
			trigger_cmd(node->val.expr.kc, hf);
		}
		*mkm = &hf->km[hf->im];
		break;
	case kcmnt_char:
		/* L("node:char:%d", node->val.c); */
		if ((*mkm)->map[(uint8_t)node->val.c].map) {
			*mkm = &(*mkm)->map[(uint8_t)node->val.c];
		} else {
			exec_macro(hf, &(*mkm)->map[(uint8_t)node->val.c].cmd);
			*mkm = &hf->km[hf->im];
		}
		break;
	}
}

struct keymap *
handle_input(struct keymap *km, unsigned k, struct hiface *hif)
{
	if (k > ASCII_RANGE) {
		return NULL;
	} else if (hif->im == im_cmd) {
		parse_cmd_input(hif, k);
		return NULL;
	}

	hif->input_changed = true;

	if (k >= '0' && k <= '9') {
		hifb_append_char(&hif->num, k);
		return km;
	} else if (!hif->keymap_describe) {
		hifb_append_char(&hif->cmd, k);
	}

	if (!km) {
		LOG_W("invalid macro");
		return NULL;
	}

	if (km->map[k].map) {
		return &km->map[k];
	} else if (km->map[k].cmd.nodes) {
		exec_macro(hif, &km->map[k].cmd);
		//hifb_clear(&hif->num);
		//hifb_clear(&hif->cmd);

		/* if (km->map[k].cmd == kc_macro) { */
		/* 	hifb_clear(&hif->num); */
		/* 	do_macro(hif, km->map[k].strcmd); */
		/* } else { */
		/* 	trigger_cmd(km->map[k].cmd, hif); */
		/* } */
	}

	hifb_clear(&hif->cmd);
	return NULL;
}

void
for_each_completion(struct keymap *km, void *ctx, for_each_completion_cb cb)
{
	unsigned k;

	for (k = 0; k < ASCII_RANGE; ++k) {
		if (km->map[k].map) {
			for_each_completion(&km->map[k], ctx, cb);
		} else if (km->map[k].cmd.nodes) {
			cb(ctx, &km->map[k]);
		}
	}
}

struct describe_completions_ctx {
	struct hiface *hf;
	struct hiface_buf *num;
	void *ctx;
	for_each_completion_cb cb;
};

static void
describe_completion(void *_ctx, struct keymap *km)
{
	struct describe_completions_ctx *ctx = _ctx;
	enum input_mode oim = ctx->hf->im;

	memset(ctx->hf->description, 0, KEYMAP_DESC_LEN);
	ctx->hf->desc_len = 0;

	do_macro(ctx->hf, km->trigger);

	strncpy(km->desc, ctx->hf->description, KEYMAP_DESC_LEN);
	ctx->cb(ctx->ctx, km);

	hiface_reset_input(ctx->hf);
	ctx->hf->im = oim;
	ctx->hf->num = *ctx->num;
}

void
describe_completions(struct hiface *hf, struct keymap *km,
	void *usr_ctx, for_each_completion_cb cb)
{

	struct hiface_buf nbuf = hf->num;
	struct hiface_buf cbuf = hf->cmd;
	struct action act = hf->next_act;

	struct describe_completions_ctx ctx = {
		.hf = hf,
		.ctx = usr_ctx,
		.cb = cb,
		.num = &nbuf,
	};

	hf->keymap_describe = true;

	for_each_completion(km, &ctx, describe_completion);

	hf->keymap_describe = false;

	hf->cmd = cbuf;
	hf->next_act = act;
}
