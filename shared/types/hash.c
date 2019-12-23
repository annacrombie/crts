#include <string.h>

#include "util/log.h"
#include "types/hash.h"

struct hash *hash_init(size_t buckets, size_t bdepth, size_t keysize)
{
	struct hash *h;

	h = calloc(1, sizeof(struct hash));

	h->cap = buckets * bdepth;
	h->e = calloc(h->cap, sizeof(struct hash_elem));

	h->keysize = keysize;
#ifdef HASH_STATS
	h->worst_lookup = 0;
	h->collisions = 0;
#endif

	return h;
};

static unsigned hash_1(const struct hash *hash, const void *key)
{
	const unsigned char *p = key;
	unsigned h = 16777619;
	size_t i;

	for (i = 0; i < hash->keysize; i++)
		h ^= (h << 5) + (h >> 2) + p[i];

	return h;
}

static unsigned hash_2(const struct hash *hash, const void *key)
{
	const unsigned char *p = key;
	unsigned h = 0;
	size_t i;

	for (i = 0; i < hash->keysize; i++)
		h = (p[i] ^ h) * 16777619;

	return h;
}

static struct hash_elem *walk_chain(const struct hash *h, const void *key)
{
	struct hash_elem *he;
	size_t i = 0;

	unsigned h1 = hash_1(h, key);
	unsigned h2 = hash_2(h, key);

	for (i = 0; i < h->cap; ++i) {
		he = &h->e[(h1 + (i * h2)) & (h->cap - 1)];

		if (!(he->init & HASH_KEY_SET && memcmp(he->key, key, h->keysize) != 0)) {
#ifdef HASH_STATS
			//L("h1: %d, h2: %d, i: %d", h1, h2, i);
			if (i > 0) {
				((struct hash *)h)->collisions++;
				if (i > h->worst_lookup)
					((struct hash *)h)->worst_lookup = i;
			}

#endif
			return he;
		}
	}

	return NULL;
}

const struct hash_elem *hash_get(const struct hash *h, const void *key)
{
	return walk_chain(h, key);
}

void hash_unset(const struct hash *h, const void *key)
{
	const struct hash_elem *he;

	if ((he = walk_chain(h, key)) != NULL)
		((struct hash_elem *)he)->init ^= HASH_VALUE_SET;
}

void hash_set(struct hash *h, const void *key, unsigned val)
{
	struct hash_elem *he;

	if ((he = walk_chain(h, key)) == NULL) {
		L("Hash full!");
		return;
	}

	if (!(he->init & HASH_KEY_SET)) {
		memcpy(he->key, key, h->keysize);
		he->init |= HASH_KEY_SET;
	}

	he->val = val;
	he->init |= HASH_VALUE_SET;
}

