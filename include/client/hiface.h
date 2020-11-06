#ifndef CLIENT_HIFACE_H
#define CLIENT_HIFACE_H

#include <stdint.h>

#include "client/input/cmdline.h"
#include "client/input/keymap.h"
#include "client/sim.h"
#include "shared/net/net_ctx.h"
#include "shared/sim/action.h"
#include "shared/types/geom.h"

#ifdef CRTS_PATHFINDING
#include "shared/pathfind/api.h"
#endif

#define HF_BUF_LEN 32

struct hiface_buf {
	char buf[HF_BUF_LEN];
	size_t len;
};

struct hiface {
	/* input related buffers */
	struct hiface_buf num;
	struct {
		bool override;
		long val;
	} num_override;
	struct hiface_buf cmd;
	struct cmdline cmdline;

	struct point cursor;
	struct point view;
	enum input_mode im;
	struct keymap km[input_mode_count];
	uint32_t redrew_world;

	struct action next_act;
	bool next_act_changed;
	uint8_t action_seq;

	bool keymap_describe;
	char description[KEYMAP_DESC_LEN];
	size_t desc_len;
	bool input_changed;

	bool center_cursor;
	bool display_help;

	/* big pointers */
	struct c_simulation *sim;
	struct net_ctx *nx;
	struct ui_ctx *ui_ctx;

	/* debugging */
#ifdef CRTS_PATHFINDING
	struct {
		bool on;
		uint32_t path;
		struct point goal;
		struct darr *path_points;
	} debug_path;
#endif
};

struct hiface *hiface_init(struct c_simulation *sim);
long hiface_get_num(struct hiface *hif, long def);
void commit_action(struct hiface *hif);
void undo_action(struct hiface *hif);
void override_num_arg(struct hiface *hf, long num);
void hf_describe(struct hiface *hf, enum keymap_category cat, char *desc, ...);
void hiface_reset_input(struct hiface *hf);
void hifb_append_char(struct hiface_buf *hbf, unsigned c);
#endif
