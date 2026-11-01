//
// Created by david on 11/1/26.
//

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <util/hashtable.h>

static int keys_equal(const void *a, size_t alen, const void *b, size_t blen) {
    if (alen != blen) {
        return 0;
    }
    return memcmp(a, b, alen) == 0;
}

static ht_entry find_in_chain(ht_entry head, void *key, size_t keylen) {
    for (ht_entry e = head; e != NULL; e = e->next) {
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

static int ht_rehash(hashtable ht, size_t new_count) {
    ht_entry *new_buckets;
    ht_entry *old_buckets;
    size_t old_count;

    if (ht == NULL || new_count < 2) {
        return false;
    }

    new_buckets = (ht_entry *)calloc(new_count, sizeof(ht_entry));
    if (new_buckets == NULL) {
        return false;
    }

    old_buckets = ht->buckets;
    old_count = ht->bucket_count;

    for (size_t i = 0; i < old_count; i++) {
        ht_entry e = old_buckets[i];
        while (e != NULL) {
            ht_entry next = e->next;
            size_t idx = hashBucketIndex(e->key, e->keylen, new_count);
            e->next = new_buckets[idx];
            new_buckets[idx] = e;
            e = next;
        }
    }

    free(old_buckets);
    ht->buckets = new_buckets;
    ht->bucket_count = new_count;
    return true;
}

static int ht_maybe_grow(hashtable ht) {
    size_t floor;
    size_t new_count;

    if (ht == NULL) {
        return false;
    }
    /* Grow when an insert would push load factor above 1 */
    if (ht->size < ht->bucket_count) {
        return true;
    }

    if (ht->size > SIZE_MAX / 2) {
        return false;
    }
    floor = ht->size * 2;
    new_count = nextEratosthenesPrime(&floor);
    if (new_count == 0) {
        return false;
    }
    return ht_rehash(ht, new_count);
}

hashtable ht_create(size_t hint) {
    hashtable ht;
    size_t buckets;

    buckets = initial_bucket_count(hint);
    if (buckets == 0) {
        return NULL;
    }

    ht = (hashtable)malloc(sizeof(hashtable_t));
    if (ht == NULL) {
        return NULL;
    }

    ht->buckets = (ht_entry *)calloc(buckets, sizeof(ht_entry));
    if (ht->buckets == NULL) {
        free(ht);
        return NULL;
    }

    ht->bucket_count = buckets;
    ht->size = 0;
    return ht;
}

int ht_free(hashtable ht) {
    if (ht == NULL) {
        return false;
    }

    for (size_t i = 0; i < ht->bucket_count; i++) {
        ht_entry e = ht->buckets[i];
        while (e != NULL) {
            ht_entry next = e->next;
            free(e);
            e = next;
        }
    }

    free(ht->buckets);
    ht->buckets = NULL;
    ht->bucket_count = 0;
    ht->size = 0;
    free(ht);
    return true;
}

size_t ht_size(hashtable ht) {
    if (ht == NULL) {
        return 0;
    }
    return ht->size;
}

int ht_empty(hashtable ht) {
    return ht == NULL || ht->size == 0;
}

hash_result ht_put(hashtable ht, void *key, size_t keylen, void *value) {
    size_t idx;
    ht_entry e;

    if (ht == NULL || key == NULL || keylen == 0) {
        return HASH_ERR;
    }

    idx = hashBucketIndex(key, keylen, ht->bucket_count);
    e = find_in_chain(ht->buckets[idx], key, keylen);
    if (e != NULL) {
        e->value = value;
        return HASH_UPDATED;
    }

    if (!ht_maybe_grow(ht)) {
        return HASH_ERR;
    }

    /* Bucket index may have changed after rehash */
    idx = hashBucketIndex(key, keylen, ht->bucket_count);

    e = (ht_entry)malloc(sizeof(ht_entry_t));
    if (e == NULL) {
        return HASH_ERR;
    }
    e->key = key;
    e->keylen = keylen;
    e->value = value;
    e->next = ht->buckets[idx];
    ht->buckets[idx] = e;
    ht->size++;
    return HASH_INSERTED;
}

int ht_get(hashtable ht, void *key, size_t keylen, void **out_value) {
    size_t idx;
    ht_entry e;

    if (out_value != NULL) {
        *out_value = NULL;
    }
    if (ht == NULL || key == NULL || keylen == 0) {
        return false;
    }

    idx = hashBucketIndex(key, keylen, ht->bucket_count);
    e = find_in_chain(ht->buckets[idx], key, keylen);
    if (e == NULL) {
        return false;
    }
    if (out_value != NULL) {
        *out_value = e->value;
    }
    return true;
}

int ht_remove(hashtable ht, void *key, size_t keylen, void **out_value) {
    size_t idx;
    ht_entry e;
    ht_entry prev;

    if (out_value != NULL) {
        *out_value = NULL;
    }
    if (ht == NULL || key == NULL || keylen == 0) {
        return false;
    }

    idx = hashBucketIndex(key, keylen, ht->bucket_count);
    prev = NULL;
    e = ht->buckets[idx];
    while (e != NULL) {
        if (keys_equal(e->key, e->keylen, key, keylen)) {
            if (prev == NULL) {
                ht->buckets[idx] = e->next;
            } else {
                prev->next = e->next;
            }
            if (out_value != NULL) {
                *out_value = e->value;
            }
            free(e);
            ht->size--;
            return true;
        }
        prev = e;
        e = e->next;
    }
    return false;
}

int ht_contains(hashtable ht, void *key, size_t keylen) {
    return ht_get(ht, key, keylen, NULL);
}
