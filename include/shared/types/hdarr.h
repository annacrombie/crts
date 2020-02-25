#ifndef UTIL_HDARR_H
#define UTIL_HDARR_H

#include <stddef.h>
#include "shared/types/iterator.h"

struct hdarr;

struct hdarr *hdarr_init(size_t size, size_t keysize, size_t item_size);
void *hdarr_get(struct hdarr *hd, const void *key);
const size_t *hdarr_get_i(struct hdarr *hd, const void *key);
void *hdarr_get_by_i(struct hdarr *hd, size_t i);
void hdarr_destroy(struct hdarr *hd);
void hdarr_for_each(struct hdarr *hd, void *ctx, iterator_func ifnc);
size_t hdarr_set(struct hdarr *hd, const void *key, const void *value);
#endif
