/**
 * Unit tests for util/hashset.
 */

#include <check.h>
#include <stdlib.h>
#include <string.h>
#include <util/hashset.h>

START_TEST(createEmptyFree) {
    hashset hs = hs_create(0);
    ck_assert_ptr_nonnull(hs);
    ck_assert(hs_empty(hs));
    ck_assert_uint_eq(hs_size(hs), 0);
    ck_assert(hs_free(hs));
}
END_TEST

START_TEST(nullHandleOps) {
    int x = 1;
    ck_assert(hs_empty(NULL));
    ck_assert_uint_eq(hs_size(NULL), 0);
    ck_assert(!hs_free(NULL));
    ck_assert_int_eq(hs_add(NULL, &x, sizeof(x)), HASH_ERR);
    ck_assert(!hs_contains(NULL, &x, sizeof(x)));
    ck_assert(!hs_remove(NULL, &x, sizeof(x)));
}
END_TEST

START_TEST(addContainsRemove) {
    int a = 10, b = 20, c = 30;
    hashset hs = hs_create(8);
    ck_assert_ptr_nonnull(hs);

    ck_assert_int_eq(hs_add(hs, &a, sizeof(a)), HASH_INSERTED);
    ck_assert_int_eq(hs_add(hs, &b, sizeof(b)), HASH_INSERTED);
    ck_assert_int_eq(hs_add(hs, &c, sizeof(c)), HASH_INSERTED);
    ck_assert_uint_eq(hs_size(hs), 3);
    ck_assert(!hs_empty(hs));

    ck_assert(hs_contains(hs, &a, sizeof(a)));
    ck_assert(hs_contains(hs, &b, sizeof(b)));
    ck_assert(hs_contains(hs, &c, sizeof(c)));

    ck_assert_int_eq(hs_add(hs, &a, sizeof(a)), HASH_UPDATED);
    ck_assert_uint_eq(hs_size(hs), 3);

    ck_assert(hs_remove(hs, &b, sizeof(b)));
    ck_assert(!hs_contains(hs, &b, sizeof(b)));
    ck_assert_uint_eq(hs_size(hs), 2);
    ck_assert(!hs_remove(hs, &b, sizeof(b)));

    ck_assert(hs_free(hs));
}
END_TEST

START_TEST(rejectBadKeys) {
    int a = 1;
    hashset hs = hs_create(0);
    ck_assert_ptr_nonnull(hs);

    ck_assert_int_eq(hs_add(hs, NULL, sizeof(a)), HASH_ERR);
    ck_assert_int_eq(hs_add(hs, &a, 0), HASH_ERR);
    ck_assert(!hs_contains(hs, NULL, sizeof(a)));
    ck_assert(!hs_contains(hs, &a, 0));
    ck_assert(!hs_remove(hs, NULL, sizeof(a)));
    ck_assert(!hs_remove(hs, &a, 0));

    ck_assert(hs_free(hs));
}
END_TEST

START_TEST(pointerIdentity) {
    /* Same reference: key bytes are the pointer value itself */
    int node_a = 1, node_b = 1;
    int *pa = &node_a;
    int *pb = &node_b;
    int *pa2 = &node_a;

    hashset hs = hs_create(0);
    ck_assert_ptr_nonnull(hs);

    ck_assert_int_eq(hs_add(hs, &pa, sizeof(pa)), HASH_INSERTED);
    ck_assert(hs_contains(hs, &pa2, sizeof(pa2)));
    ck_assert(!hs_contains(hs, &pb, sizeof(pb)));
    ck_assert_int_eq(hs_add(hs, &pa2, sizeof(pa2)), HASH_UPDATED);
    ck_assert_uint_eq(hs_size(hs), 1);

    ck_assert(hs_free(hs));
}
END_TEST

START_TEST(valueIdentityStrings) {
    const char *l1 = "vertex-7";
    char l2[] = "vertex-7";
    const char *other = "vertex-8";

    hashset hs = hs_create(0);
    ck_assert_ptr_nonnull(hs);

    ck_assert_int_eq(hs_add(hs, (void *)l1, strlen(l1)), HASH_INSERTED);
    ck_assert(hs_contains(hs, l2, strlen(l2)));
    ck_assert_int_eq(hs_add(hs, l2, strlen(l2)), HASH_UPDATED);
    ck_assert(!hs_contains(hs, (void *)other, strlen(other)));
    ck_assert_uint_eq(hs_size(hs), 1);

    ck_assert(hs_free(hs));
}
END_TEST

START_TEST(growsAndKeepsMembership) {
    hashset hs = hs_create(3); /* small start so growth triggers */
    int values[64];
    size_t start_buckets;

    ck_assert_ptr_nonnull(hs);
    start_buckets = hs->bucket_count;

    for (int i = 0; i < 64; i++) {
        values[i] = i;
        ck_assert_int_eq(hs_add(hs, &values[i], sizeof(values[i])), HASH_INSERTED);
    }

    ck_assert_uint_eq(hs_size(hs), 64);
    ck_assert_uint_gt(hs->bucket_count, start_buckets);

    for (int i = 0; i < 64; i++) {
        ck_assert(hs_contains(hs, &values[i], sizeof(values[i])));
    }

    /* Re-add should not grow size */
    ck_assert_int_eq(hs_add(hs, &values[0], sizeof(values[0])), HASH_UPDATED);
    ck_assert_uint_eq(hs_size(hs), 64);

    ck_assert(hs_free(hs));
}
END_TEST

Suite *hashset_suite(void) {
    Suite *s = suite_create("hashset");
    TCase *tc = tcase_create("core");
    tcase_add_test(tc, createEmptyFree);
    tcase_add_test(tc, nullHandleOps);
    tcase_add_test(tc, addContainsRemove);
    tcase_add_test(tc, rejectBadKeys);
    tcase_add_test(tc, pointerIdentity);
    tcase_add_test(tc, valueIdentityStrings);
    tcase_add_test(tc, growsAndKeepsMembership);
    suite_add_tcase(s, tc);
    return s;
}

int main(void) {
    Suite *s = hashset_suite();
    SRunner *sr = srunner_create(s);
    srunner_set_fork_status(sr, CK_NOFORK);
    srunner_run_all(sr, CK_NORMAL);
    int failed = srunner_ntests_failed(sr);
    srunner_free(sr);
    return (failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
