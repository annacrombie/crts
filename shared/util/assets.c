#include "posix.h"

#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "shared/types/hash.h"
#include "shared/util/assets.h"
#include "shared/util/log.h"

#ifdef INCLUDE_EMBEDDED_DATA
#include "embedded_data.h"
#else
struct file_data embedded_files[] = { 0 };
size_t embedded_files_len = 0;
#endif

#define CHUNK_SIZE BUFSIZ

uint8_t *buffer;
size_t buffer_size = CHUNK_SIZE;

#define ASSET_PATHS_LEN 16
#define PATH_MAX 256
static struct {
	const char *path;
	size_t len;
} asset_paths[ASSET_PATHS_LEN] = { 0 };

void
asset_path_init(char *asset_path)
{
	size_t i = 0;

	char *sep;

	while ((sep = strchr(asset_path, ':'))) {
		*sep = '\0';
		asset_paths[i].path = asset_path;
		asset_paths[i].len = strlen(asset_path);
		asset_path = sep + 1;
		++i;
	}

	asset_paths[i].path = asset_path;
	asset_paths[i].len = strlen(asset_path);

	/* TODO rename this funciton */
	buffer = malloc(sizeof(uint8_t) * buffer_size);
}

static struct file_data *
lookup_embedded_asset(const char *path)
{
	size_t i;
	for (i = 0; i < embedded_files_len; ++i) {
		if (strcmp(path, embedded_files[i].path) == 0) {
			return &embedded_files[i];
		}
	}

	return NULL;
}

static struct file_data *
read_raw_asset(FILE *f, const char *path)
{
	static struct file_data fd;

	memset(buffer, 0, buffer_size);

	fd.path = path;
	fd.len = 0;

	size_t b = 1;
	while (b > 0) {
		if (buffer_size - fd.len < CHUNK_SIZE) {
			buffer_size *= 2;
			buffer = realloc(buffer,
				sizeof(uint8_t) * buffer_size);
		}

		b = fread(&buffer[fd.len], 1, CHUNK_SIZE, f);
		fd.len += b;
	}
	fd.data = buffer;

	fclose(f);

	return &fd;
}

struct file_data*
asset(const char *path)
{
	char pathbuf[PATH_MAX + 1];
	struct file_data *fdat;
	FILE *f;
	size_t i;

	L("loading asset @ '%s'", path);

	if ((fdat = lookup_embedded_asset(path))) {
		L("  [embedded]");
		return fdat;
	}

	if (*path == '/') {
		if (access(path, R_OK) == 0 && (f = fopen(path, "r"))) {
			L("  '%s'", path);
			return read_raw_asset(f, path);
		} else {
			return NULL;
		}
	}

	for (i = 0; i < ASSET_PATHS_LEN; ++i) {
		if (asset_paths[i].path == NULL) {
			continue;
		}

		snprintf(pathbuf, PATH_MAX, "%s/%s", asset_paths[i].path, path);

		if (access(pathbuf, R_OK) == 0 && (f = fopen(pathbuf, "r"))) {
			L("  '%s'", pathbuf);
			return read_raw_asset(f, pathbuf);
		}
	}

	LOG_W("failed to load asset '%s'", path);

	return NULL;
}
