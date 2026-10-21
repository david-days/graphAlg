/**
 * Unit tests for util/heap.
 */

#include <check.h>
#include <stdbool.h>
#include <stdlib.h>
#include <util/heap.h>

START_TEST(createEmptyFree) {
    heap h = h_create(8, true);
    ck_assert_ptr_nonnull(h);
    ck_assert(h_empty(h));
    ck_assert_ptr_null(h_peek(h));
    ck_assert(h_free(h));
}
END_TEST

START_TEST(nullHandleOps) {
    void *val = (void *)0x1;
    ck_assert(h_empty(NULL));
    ck_assert(!h_free(NULL));
    ck_assert(!h_push(NULL, &val, 1));
    ck_assert(!h_pop(NULL, &val));
    ck_assert_ptr_null(val);
    ck_assert_ptr_null(h_peek(NULL));
}
END_TEST

START_TEST(minHeapOrder) {
    int a = 1, b = 2, c = 3, d = 4;
    void *out = NULL;
    heap h = h_create(8, true);
    ck_assert_ptr_nonnull(h);

    ck_assert(h_push(h, &c, 30));
    ck_assert(h_push(h, &a, 10));
    ck_assert(h_push(h, &d, 40));
    ck_assert(h_push(h, &b, 20));

    ck_assert(h_pop(h, &out));
    ck_assert_ptr_eq(out, &a);
    ck_assert(h_pop(h, &out));
    ck_assert_ptr_eq(out, &b);
    ck_assert(h_pop(h, &out));
    ck_assert_ptr_eq(out, &c);
    ck_assert(h_pop(h, &out));
    ck_assert_ptr_eq(out, &d);

    ck_assert(h_empty(h));
    ck_assert(h_free(h));
}
END_TEST

START_TEST(maxHeapOrder) {
    int a = 1, b = 2, c = 3;
    void *out = NULL;
    heap h = h_create(8, false);
    ck_assert_ptr_nonnull(h);

    ck_assert(h_push(h, &a, 10));
    ck_assert(h_push(h, &c, 30));
    ck_assert(h_push(h, &b, 20));

    ck_assert(h_pop(h, &out));
    ck_assert_ptr_eq(out, &c);
    ck_assert(h_pop(h, &out));
    ck_assert_ptr_eq(out, &b);
    ck_assert(h_pop(h, &out));
    ck_assert_ptr_eq(out, &a);

    ck_assert(h_free(h));
}
END_TEST

START_TEST(peekDoesNotRemove) {
    int a = 10, b = 20;
    void *out = NULL;
    heap h = h_create(4, true);
    ck_assert_ptr_nonnull(h);

    ck_assert(h_push(h, &b, 20));
    ck_assert(h_push(h, &a, 10));
    ck_assert_ptr_eq(h_peek(h), &a);
    ck_assert_ptr_eq(h_peek(h), &a);

    ck_assert(h_pop(h, &out));
    ck_assert_ptr_eq(out, &a);
    ck_assert_ptr_eq(h_peek(h), &b);

    ck_assert(h_free(h));
}
END_TEST

START_TEST(rejectWhenFull) {
    int a = 1, b = 2, c = 3;
    heap h = h_create(2, true);
    ck_assert_ptr_nonnull(h);

    ck_assert(h_push(h, &a, 1));
    ck_assert(h_push(h, &b, 2));
    ck_assert(!h_push(h, &c, 3));

    ck_assert(h_free(h));
}
END_TEST

START_TEST(popEmpty) {
    void *out = (void *)0x1;
    heap h = h_create(4, true);
    ck_assert_ptr_nonnull(h);

    ck_assert(!h_pop(h, &out));
    ck_assert_ptr_null(out);
    ck_assert(!h_pop(h, NULL));

    ck_assert(h_free(h));
}
END_TEST

START_TEST(nullPayloadAllowed) {
    void *out = (void *)0x1;
    heap h = h_create(4, true);
    ck_assert_ptr_nonnull(h);

    ck_assert(h_push(h, NULL, 5));
    ck_assert(!h_empty(h));
    ck_assert_ptr_null(h_peek(h));
    ck_assert(h_pop(h, &out));
    ck_assert_ptr_null(out);
    ck_assert(h_empty(h));

    ck_assert(h_free(h));
}
END_TEST

Suite *heap_suite(void) {
    Suite *s = suite_create("heap");
    TCase *tc = tcase_create("core");
    tcase_add_test(tc, createEmptyFree);
    tcase_add_test(tc, nullHandleOps);
    tcase_add_test(tc, minHeapOrder);
    tcase_add_test(tc, maxHeapOrder);
    tcase_add_test(tc, peekDoesNotRemove);
    tcase_add_test(tc, rejectWhenFull);
    tcase_add_test(tc, popEmpty);
    tcase_add_test(tc, nullPayloadAllowed);
    suite_add_tcase(s, tc);
    return s;
}

int main(void) {
    Suite *suite = heap_suite();
    SRunner *sr = srunner_create(suite);
    srunner_set_fork_status(sr, CK_NOFORK);
    srunner_run_all(sr, CK_NORMAL);
    int failed = srunner_ntests_failed(sr);
    srunner_free(sr);
    return (failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
