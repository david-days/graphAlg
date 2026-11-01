//
// Created by david on 10/25/26.
//

#ifndef GRAPHALG_HASH_H
#define GRAPHALG_HASH_H

#include <stddef.h>

/**
 * Result codes for hash container insert/add operations.
 * Negative values are errors (room to split HASH_ERR later); positive are success paths.
 */
typedef enum {
    HASH_ERR = -1,       /* bad args, allocation failure, or other error */
    HASH_INSERTED = 1,   /* new key placed in the container */
    HASH_UPDATED = 2     /* key already present (put: value replaced; add: no-op) */
} hash_result;

/**
 * Calculate a unique hash for the char data.
 * SuperFastHash used from http://www.azillionmonkeys.com/qed/hash.html under LGPL 2.1 license
 * @param data character data (string value) to be hashed
 * @param len Length of the character data
 * @return hash value of the character data, if not null; otherwise, zero.
 */
size_t superFastHash (const void * data, int len);

/**
 * @brief Using the Sieve of Eratosthenes, find the largest prime strictly below bin_count
 *
 * Quick and dirty (not necessarily computationally quick) way to choose a prime
 * table size for hashing — typically just under a desired bucket / allocation count.
 *
 * @param bin_count Upper bound (exclusive); largest prime returned is < *bin_count
 * @return Largest prime strictly below *bin_count, or 0 if bin_count is NULL,
 *         *bin_count < 3, or allocation fails
 */
size_t maxEratosthenesPrime(size_t *bin_count);

/**
 * @brief Using the Sieve of Eratosthenes, find the smallest prime strictly above floor
 *
 * Used when growing a hash table: next prime > 2 * current size.
 *
 * @param floor Lower bound (exclusive); smallest prime returned is > *floor
 * @return Smallest prime strictly above *floor, or 0 if floor is NULL or allocation fails
 */
size_t nextEratosthenesPrime(size_t *floor);

/**
 * Performs a hash on the id value passed in and return the index of the bucket that data should be placed into or found
 * @param idval void pointer to value to be hashed
 * @param datalen Length of the data pointed to by idval
 * @param bucketcount Number of buckets to hash into
 * @return Index of the bucket to place the value into, or 0 if idval is NULL,
 *         datalen is 0, or bucketcount is 0
 */
size_t hashBucketIndex(void *idval, size_t datalen, size_t bucketcount);

#endif //GRAPHALG_HASH_H
