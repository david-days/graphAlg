/**
 * Unit tests for util/hashtable.
 */

#include <check.h>
#include <stdlib.h>
#include <string.h>
#include <util/hashtable.h>

START_TEST(createEmptyFree) {
    hashtable ht = ht_create(0);
    ck_assert_ptr_nonnull(ht);
    ck_assert(ht_empty(ht));
    ck_assert_uint_eq(ht_size(ht), 0);
    ck_assert(ht_free(ht));
}
END_TEST

START_TEST(nullHandleOps) {
    int k = 1;
    void *val = (void *)0x1;
    ck_assert(ht_empty(NULL));
    ck_assert_uint_eq(ht_size(NULL), 0);
    ck_assert(!ht_free(NULL));
    ck_assert_int_eq(ht_put(NULL, &k, sizeof(k), &val), HASH_ERR);
    ck_assert(!ht_contains(NULL, &k, sizeof(k)));
    ck_assert(!ht_get(NULL, &k, sizeof(k), &val));
    ck_assert_ptr_null(val);
    ck_assert(!ht_remove(NULL, &k, sizeof(k), &val));
    ck_assert_ptr_null(val);
}
END_TEST

START_TEST(putGetRemove) {
    int ka = 10, kb = 20, kc = 30;
    const char *va = "alpha";
    const char *vb = "bravo";
    const char *vc = "charlie";
    const char *va2 = "ALPHA";
    void *out = NULL;

    hashtable ht = ht_create(8);
    ck_assert_ptr_nonnull(ht);

    ck_assert_int_eq(ht_put(ht, &ka, sizeof(ka), (void *)va), HASH_INSERTED);
    ck_assert_int_eq(ht_put(ht, &kb, sizeof(kb), (void *)vb), HASH_INSERTED);
    ck_assert_int_eq(ht_put(ht, &kc, sizeof(kc), (void *)vc), HASH_INSERTED);
    ck_assert_uint_eq(ht_size(ht), 3);
    ck_assert(!ht_empty(ht));

    ck_assert(ht_get(ht, &ka, sizeof(ka), &out));
    ck_assert_ptr_eq(out, va);
    ck_assert(ht_contains(ht, &kb, sizeof(kb)));

    ck_assert_int_eq(ht_put(ht, &ka, sizeof(ka), (void *)va2), HASH_UPDATED);
    ck_assert_uint_eq(ht_size(ht), 3);
    ck_assert(ht_get(ht, &ka, sizeof(ka), &out));
    ck_assert_ptr_eq(out, va2);

    ck_assert(ht_remove(ht, &kb, sizeof(kb), &out));
    ck_assert_ptr_eq(out, vb);
    ck_assert(!ht_contains(ht, &kb, sizeof(kb)));
    ck_assert_uint_eq(ht_size(ht), 2);
    ck_assert(!ht_remove(ht, &kb, sizeof(kb), &out));
    ck_assert_ptr_null(out);

    ck_assert(ht_free(ht));
}
END_TEST

START_TEST(nullValueAllowed) {
    int k = 42;
    void *out = (void *)0x1;

    hashtable ht = ht_create(0);
    ck_assert_ptr_nonnull(ht);

    ck_assert_int_eq(ht_put(ht, &k, sizeof(k), NULL), HASH_INSERTED);
    ck_assert(ht_contains(ht, &k, sizeof(k)));
    ck_assert(ht_get(ht, &k, sizeof(k), &out));
    ck_assert_ptr_null(out);

    ck_assert(ht_free(ht));
}
END_TEST

START_TEST(rejectBadKeys) {
    int k = 1;
    void *v = &k;
    hashtable ht = ht_create(0);
    ck_assert_ptr_nonnull(ht);

    ck_assert_int_eq(ht_put(ht, NULL, sizeof(k), v), HASH_ERR);
    ck_assert_int_eq(ht_put(ht, &k, 0, v), HASH_ERR);
    ck_assert(!ht_get(ht, NULL, sizeof(k), NULL));
    ck_assert(!ht_get(ht, &k, 0, NULL));
    ck_assert(!ht_remove(ht, NULL, sizeof(k), NULL));
    ck_assert(!ht_remove(ht, &k, 0, NULL));

    ck_assert(ht_free(ht));
}
END_TEST

START_TEST(pointerIdentity) {
    int node_a = 1, node_b = 1;
    int *pa = &node_a;
    int *pb = &node_b;
    int *pa2 = &node_a;
    int val = 99;
    void *out = NULL;

    hashtable ht = ht_create(0);
    ck_assert_ptr_nonnull(ht);

    ck_assert_int_eq(ht_put(ht, &pa, sizeof(pa), &val), HASH_INSERTED);
    ck_assert(ht_get(ht, &pa2, sizeof(pa2), &out));
    ck_assert_ptr_eq(out, &val);
    ck_assert(!ht_contains(ht, &pb, sizeof(pb)));
    ck_assert_int_eq(ht_put(ht, &pa2, sizeof(pa2), &val), HASH_UPDATED);
    ck_assert_uint_eq(ht_size(ht), 1);

    ck_assert(ht_free(ht));
}
END_TEST

START_TEST(stringKeys) {
    const char *l1 = "vertex-7";
    char l2[] = "vertex-7";
    int weight = 5;
    int weight2 = 9;
    void *out = NULL;

    hashtable ht = ht_create(0);
    ck_assert_ptr_nonnull(ht);

    ck_assert_int_eq(ht_put(ht, (void *)l1, strlen(l1), &weight), HASH_INSERTED);
    ck_assert(ht_get(ht, l2, strlen(l2), &out));
    ck_assert_ptr_eq(out, &weight);
    ck_assert_int_eq(ht_put(ht, l2, strlen(l2), &weight2), HASH_UPDATED);
    ck_assert(ht_get(ht, (void *)l1, strlen(l1), &out));
    ck_assert_ptr_eq(out, &weight2);
    ck_assert_uint_eq(ht_size(ht), 1);

    ck_assert(ht_free(ht));
}
END_TEST

START_TEST(growsAndKeepsMappings) {
    hashtable ht = ht_create(3);
    int keys[64];
    int vals[64];
    size_t start_buckets;
    void *out = NULL;

    ck_assert_ptr_nonnull(ht);
    start_buckets = ht->bucket_count;

    for (int i = 0; i < 64; i++) {
        keys[i] = i;
        vals[i] = i * 10;
        ck_assert_int_eq(ht_put(ht, &keys[i], sizeof(keys[i]), &vals[i]), HASH_INSERTED);
    }

    ck_assert_uint_eq(ht_size(ht), 64);
    ck_assert_uint_gt(ht->bucket_count, start_buckets);

    for (int i = 0; i < 64; i++) {
        ck_assert(ht_get(ht, &keys[i], sizeof(keys[i]), &out));
        ck_assert_ptr_eq(out, &vals[i]);
    }

    ck_assert_int_eq(ht_put(ht, &keys[0], sizeof(keys[0]), &vals[0]), HASH_UPDATED);
    ck_assert_uint_eq(ht_size(ht), 64);

    ck_assert(ht_free(ht));
}
END_TEST

Suite *hashtable_suite(void) {
    Suite *s = suite_create("hashtable");
    TCase *tc = tcase_create("core");
    tcase_add_test(tc, createEmptyFree);
    tcase_add_test(tc, nullHandleOps);
    tcase_add_test(tc, putGetRemove);
    tcase_add_test(tc, nullValueAllowed);
    tcase_add_test(tc, rejectBadKeys);
    tcase_add_test(tc, pointerIdentity);
    tcase_add_test(tc, stringKeys);
    tcase_add_test(tc, growsAndKeepsMappings);
    suite_add_tcase(s, tc);
    return s;
}

int main(void) {
    Suite *s = hashtable_suite();
    SRunner *sr = srunner_create(s);
    srunner_set_fork_status(sr, CK_NOFORK);
    srunner_run_all(sr, CK_NORMAL);
    int failed = srunner_ntests_failed(sr);
    srunner_free(sr);
    return (failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
