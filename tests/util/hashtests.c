/**
 * Unit tests for util/hash.
 */

#include <check.h>
#include <stdlib.h>
#include <string.h>
#include <util/hash.h>

START_TEST(primeNumberTest) {
    size_t expPrime = 43;
    size_t bin_count = 45;
    size_t foundPrime = maxEratosthenesPrime(&bin_count);
    ck_assert_uint_eq(foundPrime, expPrime);

    bin_count = 50;
    expPrime = 47;
    foundPrime = maxEratosthenesPrime(&bin_count);
    ck_assert_uint_eq(foundPrime, expPrime);

    bin_count = 60;
    expPrime = 59;
    foundPrime = maxEratosthenesPrime(&bin_count);
    ck_assert_uint_eq(foundPrime, expPrime);

    bin_count = 102;
    expPrime = 101;
    foundPrime = maxEratosthenesPrime(&bin_count);
    ck_assert_uint_eq(foundPrime, expPrime);

    bin_count = 795;
    expPrime = 787;
    foundPrime = maxEratosthenesPrime(&bin_count);
    ck_assert_uint_eq(foundPrime, expPrime);

    bin_count = 275;
    expPrime = 271;
    foundPrime = maxEratosthenesPrime(&bin_count);
    ck_assert_uint_eq(foundPrime, expPrime);

    bin_count = 580;
    expPrime = 577;
    foundPrime = maxEratosthenesPrime(&bin_count);
    ck_assert_uint_eq(foundPrime, expPrime);

    bin_count = 700;
    expPrime = 691;
    foundPrime = maxEratosthenesPrime(&bin_count);
    ck_assert_uint_eq(foundPrime, expPrime);

    /* Guards: no prime exists below 3; NULL pointer */
    bin_count = 0;
    ck_assert_uint_eq(maxEratosthenesPrime(&bin_count), 0);
    bin_count = 1;
    ck_assert_uint_eq(maxEratosthenesPrime(&bin_count), 0);
    bin_count = 2;
    ck_assert_uint_eq(maxEratosthenesPrime(&bin_count), 0);
    ck_assert_uint_eq(maxEratosthenesPrime(NULL), 0);

    bin_count = 3;
    ck_assert_uint_eq(maxEratosthenesPrime(&bin_count), 2);
}
END_TEST

START_TEST(superFastHashTest) {
    ck_assert_uint_eq(superFastHash(NULL, 5), 0);
    ck_assert_uint_eq(superFastHash("abc", 0), 0);
    ck_assert_uint_eq(superFastHash("abc", -1), 0);

    const char *s = "graphalg-label";
    size_t h1 = superFastHash(s, (int)strlen(s));
    size_t h2 = superFastHash(s, (int)strlen(s));
    ck_assert_uint_ne(h1, 0);
    ck_assert_uint_eq(h1, h2);

    const char *t = "graphalg-label!";
    size_t h3 = superFastHash(t, (int)strlen(t));
    ck_assert_uint_ne(h3, h1);

    /* Binary payload (not a C string) */
    unsigned char bytes[] = {0x00, 0x01, 0xff, 0x7e};
    size_t hb = superFastHash(bytes, (int)sizeof(bytes));
    ck_assert_uint_ne(hb, 0);
    ck_assert_uint_eq(hb, superFastHash(bytes, (int)sizeof(bytes)));
}
END_TEST

START_TEST(nextPrimeTest) {
    size_t floor;
    size_t p;

    floor = 0;
    p = nextEratosthenesPrime(&floor);
    ck_assert_uint_eq(p, 2);

    floor = 1;
    p = nextEratosthenesPrime(&floor);
    ck_assert_uint_eq(p, 2);

    floor = 2;
    p = nextEratosthenesPrime(&floor);
    ck_assert_uint_eq(p, 3);

    floor = 10;
    p = nextEratosthenesPrime(&floor);
    ck_assert_uint_eq(p, 11);

    floor = 100;
    p = nextEratosthenesPrime(&floor);
    ck_assert_uint_eq(p, 101);

    floor = 787;
    p = nextEratosthenesPrime(&floor);
    ck_assert_uint_eq(p, 797);

    /* Growth-style: next prime > 2 * size */
    floor = 20; /* 2 * 10 */
    p = nextEratosthenesPrime(&floor);
    ck_assert_uint_eq(p, 23);

    ck_assert_uint_eq(nextEratosthenesPrime(NULL), 0);
}
END_TEST

START_TEST(hashBucketIndexTest) {
    const char *key = "bucket-key";
    size_t len = strlen(key);
    size_t buckets = 17;

    size_t idx = hashBucketIndex((void *)key, len, buckets);
    ck_assert_uint_lt(idx, buckets);
    ck_assert_uint_eq(idx, hashBucketIndex((void *)key, len, buckets));

    ck_assert_uint_eq(hashBucketIndex(NULL, len, buckets), 0);
    ck_assert_uint_eq(hashBucketIndex((void *)key, 0, buckets), 0);
    ck_assert_uint_eq(hashBucketIndex((void *)key, len, 0), 0);

    /* Index matches modular reduction of the raw hash */
    size_t expected = superFastHash(key, (int)len) % buckets;
    ck_assert_uint_eq(idx, expected);

    /* Another key stays in range (may collide; that is allowed) */
    const char *other = "bucket-key!";
    size_t idx2 = hashBucketIndex((void *)other, strlen(other), buckets);
    ck_assert_uint_lt(idx2, buckets);
}
END_TEST

Suite *hash_suite(void) {
    Suite *s = suite_create("hash");
    TCase *tc = tcase_create("core");
    tcase_add_test(tc, primeNumberTest);
    tcase_add_test(tc, nextPrimeTest);
    tcase_add_test(tc, superFastHashTest);
    tcase_add_test(tc, hashBucketIndexTest);
    suite_add_tcase(s, tc);
    return s;
}

int main(void) {
    Suite *s = hash_suite();
    SRunner *sr = srunner_create(s);
    srunner_set_fork_status(sr, CK_NOFORK);
    srunner_run_all(sr, CK_NORMAL);
    int failed = srunner_ntests_failed(sr);
    srunner_free(sr);
    return (failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
