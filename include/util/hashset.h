//
// Created by david on 11/1/26.
//

#ifndef GRAPHALG_HASHSET_H
#define GRAPHALG_HASHSET_H

#include <stddef.h>
#include <util/hash.h>

typedef struct hs_entry_t hs_entry_t, *hs_entry;

/**
 * Singly-linked hash-set entry. Key bytes are borrowed (not copied or freed).
 */
struct hs_entry_t {
    void *key;
    size_t keylen;
    hs_entry next;
};

typedef struct hashset_t hashset_t, *hashset;

/**
 * Open-ended hash set of opaque keys (void * + length).
 *
 * Keys are borrowed: the caller must keep key bytes alive and unchanged for as
 * long as the entry remains in the set. Equality is keylen match + memcmp.
 *
 * Pointer identity (same reference): pass the address of the pointer and
 * sizeof(void *), e.g. hs_add(hs, &node, sizeof(node));
 * Value identity (same id/label bytes): pass the address of those bytes and
 * their length.
 */
struct hashset_t {
    hs_entry *buckets;   /* array of chain heads; length bucket_count */
    size_t bucket_count; /* prime table size */
    size_t size;         /* number of keys stored */
};

/**
 * Creates a new empty hash set.
 * @param hint Approximate starting capacity; resolved to a nearby prime bucket
 *             count. Pass 0 for a small default.
 * @return hashset handle, or NULL on allocation failure
 */
hashset hs_create(size_t hint);

/**
 * Frees bucket chains and the handle. Does not free key payloads.
 * @param hs set to free
 * @return true if successful, otherwise false
 */
int hs_free(hashset hs);

/**
 * @param hs set to query
 * @return number of keys, or 0 if hs is NULL
 */
size_t hs_size(hashset hs);

/**
 * @param hs set to query
 * @return true if empty (or hs is NULL), otherwise false
 */
int hs_empty(hashset hs);

/**
 * Insert a key if not already present.
 *
 * Grows and rehashes when load would exceed one key per bucket, choosing the
 * next prime strictly greater than 2 * current size.
 *
 * @param hs set to use
 * @param key borrowed key bytes (must remain valid while member)
 * @param keylen length of key in bytes; must be non-zero
 * @return HASH_INSERTED, HASH_UPDATED (already present), or HASH_ERR
 */
hash_result hs_add(hashset hs, void *key, size_t keylen);

/**
 * Remove a key if present.
 * @param hs set to use
 * @param key key bytes to match
 * @param keylen length of key in bytes
 * @return true if a key was removed, otherwise false
 */
int hs_remove(hashset hs, void *key, size_t keylen);

/**
 * @param hs set to query
 * @param key key bytes to match
 * @param keylen length of key in bytes
 * @return true if present, otherwise false
 */
int hs_contains(hashset hs, void *key, size_t keylen);

#endif //GRAPHALG_HASHSET_H
