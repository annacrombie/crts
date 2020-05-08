#include <stddef.h>
#include <string.h>
#include <unistd.h>

#include "client/cfg/common.h"
#include "client/cfg/graphics.h"
#include "client/cfg/keymap.h"
#include "shared/util/log.h"

int32_t
cfg_string_lookup(const char *str, struct lookup_table *tbl)
{
	size_t i;

	for (i = 0; i < LOOKUP_TBL_LEN; ++i) {
		if (tbl->e[i].str == NULL) {
			break;
		} else if (strcmp(tbl->e[i].str, str) == 0) {
			return tbl->e[i].t;
		}
	}

	return -1;
}

bool
parse_cfg_file(const char *filename, void *ctx, ini_handler handler)
{
	if (access(filename, R_OK) != 0) {
		L("file '%s' not found", filename);
		return false;
	} else {
		L("parsing '%s'", filename);
	}

	if (ini_parse(filename, handler, ctx) != 0) {
		L("error parsing '%s'", filename);
		return false;
	}

	return true;
}