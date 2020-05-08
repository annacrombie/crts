#ifndef CLIENT_CFG_CFG_H
#define CLIENT_CFG_CFG_H
#define LOOKUP_TBL_LEN 64

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "client/cfg/ini.h"
#include "client/opts.h"

struct lookup_table {
	struct {
		char *str;
		uint32_t t;
	} e[LOOKUP_TBL_LEN];
};

int32_t cfg_string_lookup(const char *str, struct lookup_table *tbl);
bool parse_cfg_file(const char *filename, void *ctx, ini_handler handler);
#endif