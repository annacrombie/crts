#include "posix.h"

#include <stdlib.h>
#include <string.h>

#include "shared/util/inih.h"
#include "shared/util/log.h"
#include "shared/util/text.h"
#include "terragen/gen/gen.h"
#include "terragen/gen/opts.h"

const struct terragen_opt_data terragen_opt_info[tg_opt_count] = {
	[tg_seed]           = { "seed",           dt_int,   { .u = 123456 } },
	[tg_radius]         = { "radius",         dt_float, { .f = 0.4f   }, .min = 0, .max = 1 },
	[tg_dim]            = { "dim",            dt_int,   { .u = 384    }, .min = 128, .max = 4096, .step = 128 },
	[tg_points]         = { "points",         dt_int,   { .u = 2500   }, .min = 3, .max = 10000, .step = 1 },

	[tg_mountains]      = { "mountains",      dt_int,   { .u = 20     }, .min = 0, .max = 10000, .step = 1 },
	[tg_valleys]        = { "valleys",        dt_int,   { .u = 4      }, .min = 0, .max = 10000, .step = 1 },
	[tg_fault_radius]   = { "fault_radius",   dt_float, { .f = 8.0f   }, .min = 0, .max = 40, },
	[tg_fault_curve]    = { "fault_curve",    dt_float, { .f = 0.75f  }, .min = 0, .max = 30, },
	[tg_height_mod]     = { "height_mod",     dt_float, { .f = 8.0f   }, .min = 0, .max = 30, },

	[tg_erosion_cycles] = { "erosion_cycles", dt_int,   { .u = 10 }, .min = 0, .max = 100000, .step = 1 },
	[tg_capacity]       = { "capacity",       dt_float, { .f = 0.002f }, .min = 0.0001, .max = 0.005 },
	[tg_solubility]     = { "solubility",     dt_float, { .f = 0.0001f }, .min = 0.0, .max = 0.0005 },
	[tg_deposition]     = { "deposition",     dt_float, { .f = 0.001f }, .min = 0.0, .max = 0.005 },

	[tg_upscale]        = { "upscale",        dt_int,   { .u = 2      }, .min = 1, .max = 8, .step = 1 },
};

void
tg_opts_set_defaults(terragen_opts opts)
{
	uint32_t i;
	for (i = 0; i < tg_opt_count; ++i) {
		opts[i] = terragen_opt_info[i].def;
	}
}

static enum terragen_opt
lookup_str_opt(const char *str)
{
	uint32_t i;
	for (i = 0; i < tg_opt_count; ++i) {
		if (strcmp(terragen_opt_info[i].name, str) == 0) {
			return i;
		}
	}

	return tg_opt_count;
}

static bool
parse_option(terragen_opts opts, const char *k, const char *v)
{
	enum terragen_opt opt;

	if ((opt = lookup_str_opt(k)) < tg_opt_count) {
		switch (terragen_opt_info[opt].t) {
		case dt_float:
			opts[opt].f = strtof(v, NULL);
			break;
		case dt_int:
			opts[opt].u = strtoul(v, NULL, 10);
			break;
		case dt_none:
			assert(false);
			break;
		}
	} else {
		return false;
	}

	return true;
}

static bool
parse_ini_cfg_handler(void *vp, char *err, const char *sec, const char *k,
	const char *v, uint32_t line)
{
	if (!parse_option(vp, k, v)) {
		INIH_ERR("invalid option: %s", k);
		return false;
	}

	return true;
}

static bool
tg_parse_optstring_cb(void *ctx, const char *k, const char *v)
{
	return parse_option(ctx, k, v);
}

void
tg_parse_optstring(char *s, terragen_opts opts)
{
	parse_optstring(s, opts, tg_parse_optstring_cb);
}

void
tg_parse_optfile(const char *f, terragen_opts opts)
{
	parse_cfg_file(f, opts, parse_ini_cfg_handler);
}
