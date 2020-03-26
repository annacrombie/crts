#include <assert.h>

#include "client/display/window.h"
#include "client/graphics.h"
#include "shared/util/log.h"

struct graphics_t graphics = { .color_i = 1, .trans_bg = { .fgi = 1, .bgi = 1 } };

#define TRANS_BG_MAP_I(fg_i, bg_i) (((bg_i - 1) * (graphics.trans_bg.fgi - 1)) + (fg_i - 1))

uint16_t
get_bg_pair(int16_t fg, int16_t bg)
{
	fg = graphics.trans_bg.fg_map[fg + TRANS_COLOR_BUF];
	bg = graphics.trans_bg.bg_map[bg + TRANS_COLOR_BUF];

	assert(fg != 0);
	assert(bg != 0);

	return graphics.trans_bg.pairs[TRANS_BG_MAP_I(fg, bg)];
}

void
init_tile_curs(void)
{
	size_t i, j, bgi, fgi;
	int16_t fg, bg, pair;

	for (i = 0; i < tile_count; ++i) {
		graphics.tile_curs[i] = graphics.tiles[i];
		graphics.tile_curs[i].pix.attr = graphics.cursor[ct_harvest].pix.attr;
		graphics.tile_curs[i].zi = zi_3;

		/* initialize transparent background colors */
		bg = graphics.tiles[i].pix.bg;
		if (!(bgi = graphics.trans_bg.bg_map[bg + TRANS_COLOR_BUF])) {
			bgi = graphics.trans_bg.bgi++;
			graphics.trans_bg.bg_map[bg + TRANS_COLOR_BUF] = bgi;

			for (j = 0; j < TRANS_COLORS; ++j) {
				if (!(fgi = graphics.trans_bg.fg_map[j])) {
					continue;
				}
				fg = j - TRANS_COLOR_BUF;

				pair = setup_color_pair(&graphics, fg, bg);

				/* too many colors! */
				assert(TRANS_BG_MAP_I(fgi, bgi) < TRANS_COLORS);

				graphics.trans_bg.pairs[TRANS_BG_MAP_I(fgi, bgi)] = pair;
			}
		}

	}

	for (i = 0; i < extended_ent_type_count; ++i) {
		graphics.ent_curs[i] = graphics.entities[i];
		graphics.ent_curs[i].pix.attr = graphics.cursor[ct_harvest].pix.attr;
		graphics.ent_curs[i].zi = zi_3;
	}
}
