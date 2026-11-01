//
// Created by david on 10/25/26.
//

#include <util/hash.h>
#include <limits.h>
#include <stdint.h>
#include <stdlib.h>


#undef get16bits
#if (defined(__GNUC__) && defined(__i386__)) || defined(__WATCOMC__) \
  || defined(_MSC_VER) || defined (__BORLANDC__) || defined (__TURBOC__)
#define get16bits(d) (*((const uint16_t *) (d)))
#endif

#if !defined (get16bits)
#define get16bits(d) ((((size_t)(((const uint8_t *)(d))[1])) << 8)\
                       +(size_t)(((const uint8_t *)(d))[0]) )
#endif

size_t superFastHash(const void * data, int len) {
    const char *bytes;
    size_t hash = len, tmp;
    int rem;

    if (len <= 0 || data == NULL) return 0;

    bytes = (const char *)data;
    rem = len & 3;
    len >>= 2;

    /* Main loop */
    for (;len > 0; len--) {
        hash  += get16bits (bytes);
        tmp    = (get16bits (bytes+2) << 11) ^ hash;
        hash   = (hash << 16) ^ tmp;
        bytes  += 2*sizeof (uint16_t);
        hash  += hash >> 11;
    }

    /* Handle end cases */
    switch (rem) {
        case 3: hash += get16bits (bytes);
            hash ^= hash << 16;
            hash ^= ((signed char)bytes[sizeof (uint16_t)]) << 18;
            hash += hash >> 11;
            break;
        case 2: hash += get16bits (bytes);
            hash ^= hash << 11;
            hash += hash >> 17;
            break;
        case 1: hash += (signed char)*bytes;
            hash ^= hash << 10;
            hash += hash >> 1;
    }

    /* Force "avalanching" of final 127 bits */
    hash ^= hash << 3;
    hash += hash >> 5;
    hash ^= hash << 4;
    hash += hash >> 17;
    hash ^= hash << 25;
    hash += hash >> 6;

    return hash;
}

/**
 * @brief Using the Sieve of Eratosthenes, find the largest prime strictly below bin_count
 *
 * Implementation based off example code at https://www.programminglogic.com/the-sieve-of-eratosthenes-implemented-in-c/
 *
 * @param bin_count Upper bound (exclusive); largest prime returned is < *bin_count
 * @return Largest prime strictly below *bin_count, or 0 on bad input / allocation failure
 */
size_t maxEratosthenesPrime(size_t *bin_count) {
    size_t mprime = 0;
    size_t plen;
    char *sieve;

    if (bin_count == NULL || *bin_count < 3) {
        return 0;
    }

    plen = *bin_count;
    /* One flag byte per candidate integer in [0, plen) */
    sieve = (char *)calloc(plen, 1);
    if (sieve != NULL) {
        for (size_t i = 2; i < plen; i++) {
            sieve[i] = 1;
        }

        for (size_t i = 2; i < plen; i++) {
            if (sieve[i]) {
                for (size_t j = i; i * j < plen; j++) {
                    sieve[i * j] = 0;
                }
            }
        }

        for (size_t i = 2; i < plen; i++) {
            if (sieve[i]) {
                mprime = i;
            }
        }
    }
    free(sieve);
    return mprime;
}

/**
 * @brief Using the Sieve of Eratosthenes, find the smallest prime strictly above floor
 *
 * Expands the sieve window until a prime > *floor is found.
 *
 * @param floor Lower bound (exclusive); smallest prime returned is > *floor
 * @return Smallest prime strictly above *floor, or 0 on bad input / allocation failure
 */
size_t nextEratosthenesPrime(size_t *floor) {
    size_t lo;
    size_t bound;
    char *sieve;

    if (floor == NULL) {
        return 0;
    }

    lo = *floor;
    /* Need room for at least one candidate above lo; sieve indexes [0, bound) */
    if (lo >= SIZE_MAX - 2) {
        return 0;
    }
    bound = lo + 2;
    if (bound < 3) {
        bound = 3;
    }

    for (;;) {
        sieve = (char *)calloc(bound, 1);
        if (sieve == NULL) {
            return 0;
        }

        for (size_t i = 2; i < bound; i++) {
            sieve[i] = 1;
        }

        for (size_t i = 2; i < bound; i++) {
            if (sieve[i]) {
                for (size_t j = i; i * j < bound; j++) {
                    sieve[i * j] = 0;
                }
            }
        }

        for (size_t i = lo + 1; i < bound; i++) {
            if (sieve[i]) {
                free(sieve);
                return i;
            }
        }

        free(sieve);

        /* No prime in (lo, bound); expand the search window */
        if (bound > SIZE_MAX / 2) {
            if (bound == SIZE_MAX) {
                return 0;
            }
            bound = SIZE_MAX;
        } else {
            bound *= 2;
        }
    }
}

size_t hashBucketIndex(void *idval, size_t datalen, size_t bucketcount) {
    int len;

    if (idval == NULL || datalen == 0 || bucketcount == 0) {
        return 0;
    }

    /* superFastHash takes int length; clamp rather than truncate silently */
    if (datalen > (size_t)INT_MAX) {
        len = INT_MAX;
    } else {
        len = (int)datalen;
    }

    return superFastHash(idval, len) % bucketcount;
}
