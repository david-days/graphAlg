//
// Created by david on 11/1/26.
//

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <util/hashset.h>

static int keys_equal(const void *a, size_t alen, const void *b, size_t blen) {
    if (alen != blen) {
        return 0;
    }
    return memcmp(a, b, alen) == 0;
}

static hs_entry find_in_chain(hs_entry head, void *key, size_t keylen) {
    for (hs_entry e = head; e != NULL; e = e->next) {
        if (keys_equal(e->key, e->keylen, key, keylen)) {
            return e;
        }
    }
    return NULL;
}

static size_t initial_bucket_count(size_t hint) {
    size_t bound;

    /* maxEratosthenesPrime needs bound >= 3; default ~16 → prime 13 */
    if (hint < 3) {
        bound = 17;
    } else if (hint == SIZE_MAX) {
        bound = SIZE_MAX;
    } else {
        /* largest prime <= hint */
        bound = hint + 1;
    }
    return maxEratosthenesPrime(&bound);
}

static int hs_rehash(hashset hs, size_t new_count) {
    hs_entry *new_buckets;
    hs_entry *old_buckets;
    size_t old_count;

    if (hs == NULL || new_count < 2) {
        return false;
    }

    new_buckets = (hs_entry *)calloc(new_count, sizeof(hs_entry));
    if (new_buckets == NULL) {
        return false;
    }

    old_buckets = hs->buckets;
    old_count = hs->bucket_count;

    for (size_t i = 0; i < old_count; i++) {
        hs_entry e = old_buckets[i];
        while (e != NULL) {
            hs_entry next = e->next;
            size_t idx = hashBucketIndex(e->key, e->keylen, new_count);
            e->next = new_buckets[idx];
            new_buckets[idx] = e;
            e = next;
        }
    }

    free(old_buckets);
    hs->buckets = new_buckets;
    hs->bucket_count = new_count;
    return true;
}

static int hs_maybe_grow(hashset hs) {
    size_t floor;
    size_t new_count;

    if (hs == NULL) {
        return false;
    }
    /* Grow when an insert would push load factor above 1 */
    if (hs->size < hs->bucket_count) {
        return true;
    }

    if (hs->size > SIZE_MAX / 2) {
        return false;
    }
    floor = hs->size * 2;
    new_count = nextEratosthenesPrime(&floor);
    if (new_count == 0) {
        return false;
    }
    return hs_rehash(hs, new_count);
}

hashset hs_create(size_t hint) {
    hashset hs;
    size_t buckets;

    buckets = initial_bucket_count(hint);
    if (buckets == 0) {
        return NULL;
    }

    hs = (hashset)malloc(sizeof(hashset_t));
    if (hs == NULL) {
        return NULL;
    }

    hs->buckets = (hs_entry *)calloc(buckets, sizeof(hs_entry));
    if (hs->buckets == NULL) {
        free(hs);
        return NULL;
    }

    hs->bucket_count = buckets;
    hs->size = 0;
    return hs;
}

int hs_free(hashset hs) {
    if (hs == NULL) {
        return false;
    }

    for (size_t i = 0; i < hs->bucket_count; i++) {
        hs_entry e = hs->buckets[i];
        while (e != NULL) {
            hs_entry next = e->next;
            free(e);
            e = next;
        }
    }

    free(hs->buckets);
    hs->buckets = NULL;
    hs->bucket_count = 0;
    hs->size = 0;
    free(hs);
    return true;
}

size_t hs_size(hashset hs) {
    if (hs == NULL) {
        return 0;
    }
    return hs->size;
}

int hs_empty(hashset hs) {
    return hs == NULL || hs->size == 0;
}

hash_result hs_add(hashset hs, void *key, size_t keylen) {
    size_t idx;
    hs_entry e;

    if (hs == NULL || key == NULL || keylen == 0) {
        return HASH_ERR;
    }

    idx = hashBucketIndex(key, keylen, hs->bucket_count);
    if (find_in_chain(hs->buckets[idx], key, keylen) != NULL) {
        return HASH_UPDATED;
    }

    if (!hs_maybe_grow(hs)) {
        return HASH_ERR;
    }

    /* Bucket index may have changed after rehash */
    idx = hashBucketIndex(key, keylen, hs->bucket_count);

    e = (hs_entry)malloc(sizeof(hs_entry_t));
    if (e == NULL) {
        return HASH_ERR;
    }
    e->key = key;
    e->keylen = keylen;
    e->next = hs->buckets[idx];
    hs->buckets[idx] = e;
    hs->size++;
    return HASH_INSERTED;
}

int hs_remove(hashset hs, void *key, size_t keylen) {
    size_t idx;
    hs_entry e;
    hs_entry prev;

    if (hs == NULL || key == NULL || keylen == 0) {
        return false;
    }

    idx = hashBucketIndex(key, keylen, hs->bucket_count);
    prev = NULL;
    e = hs->buckets[idx];
    while (e != NULL) {
        if (keys_equal(e->key, e->keylen, key, keylen)) {
            if (prev == NULL) {
                hs->buckets[idx] = e->next;
            } else {
                prev->next = e->next;
            }
            free(e);
            hs->size--;
            return true;
        }
        prev = e;
        e = e->next;
    }
    return false;
}

int hs_contains(hashset hs, void *key, size_t keylen) {
    size_t idx;

    if (hs == NULL || key == NULL || keylen == 0) {
        return false;
    }

    idx = hashBucketIndex(key, keylen, hs->bucket_count);
    return find_in_chain(hs->buckets[idx], key, keylen) != NULL;
}
