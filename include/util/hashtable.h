//
// Created by david on 11/1/26.
//

#ifndef GRAPHALG_HASHTABLE_H
#define GRAPHALG_HASHTABLE_H

#include <stddef.h>
#include <util/hash.h>

typedef struct ht_entry_t ht_entry_t, *ht_entry;

/**
 * Singly-linked hash-table entry. Key and value pointers are borrowed
 * (not copied or freed by the table).
 */
struct ht_entry_t {
    void *key;
    size_t keylen;
    void *value;
    ht_entry next;
};

typedef struct hashtable_t hashtable_t, *hashtable;

/**
 * Open-ended hash table of opaque keys (void * + length) to borrowed values.
 *
 * Keys are borrowed: the caller must keep key bytes alive and unchanged for as
 * long as the entry remains in the table. Equality is keylen match + memcmp.
 * Values are also borrowed and are not freed by ht_free / ht_remove.
 *
 * Pointer identity (same reference): pass the address of the pointer and
 * sizeof(void *), e.g. ht_put(ht, &node, sizeof(node), val);
 * Value identity (same id/label bytes): pass the address of those bytes and
 * their length.
 */
struct hashtable_t {
    ht_entry *buckets;   /* array of chain heads; length bucket_count */
    size_t bucket_count; /* prime table size */
    size_t size;         /* number of keys stored */
};

/**
 * Creates a new empty hash table.
 * @param hint Approximate starting capacity; resolved to a nearby prime bucket
 *             count. Pass 0 for a small default.
 * @return hashtable handle, or NULL on allocation failure
 */
hashtable ht_create(size_t hint);

/**
 * Frees bucket chains and the handle. Does not free keys or values.
 * @param ht table to free
 * @return true if successful, otherwise false
 */
int ht_free(hashtable ht);

/**
 * @param ht table to query
 * @return number of keys, or 0 if ht is NULL
 */
size_t ht_size(hashtable ht);

/**
 * @param ht table to query
 * @return true if empty (or ht is NULL), otherwise false
 */
int ht_empty(hashtable ht);

/**
 * Insert or replace a key/value pair.
 *
 * Grows and rehashes when load would exceed one key per bucket, choosing the
 * next prime strictly greater than 2 * current size.
 *
 * @param ht table to use
 * @param key borrowed key bytes (must remain valid while member)
 * @param keylen length of key in bytes; must be non-zero
 * @param value borrowed value pointer (may be NULL)
 * @return HASH_INSERTED, HASH_UPDATED (value replaced), or HASH_ERR
 */
hash_result ht_put(hashtable ht, void *key, size_t keylen, void *value);

/**
 * Look up a key and optionally return its value.
 * @param ht table to use
 * @param key key bytes to match
 * @param keylen length of key in bytes
 * @param out_value set to the stored value if found; may be NULL
 * @return true if the key is present, otherwise false
 */
int ht_get(hashtable ht, void *key, size_t keylen, void **out_value);

/**
 * Remove a key if present and optionally return its value.
 * @param ht table to use
 * @param key key bytes to match
 * @param keylen length of key in bytes
 * @param out_value set to the stored value if removed; may be NULL
 * @return true if a key was removed, otherwise false
 */
int ht_remove(hashtable ht, void *key, size_t keylen, void **out_value);

/**
 * @param ht table to query
 * @param key key bytes to match
 * @param keylen length of key in bytes
 * @return true if present, otherwise false
 */
int ht_contains(hashtable ht, void *key, size_t keylen);

#endif //GRAPHALG_HASHTABLE_H
