/**
 * Unit tests for util/queue.
 */

#include <check.h>
#include <stdlib.h>
#include <util/queue.h>

START_TEST(createEmptyFree) {
    queue q = q_create();
    ck_assert_ptr_nonnull(q);
    ck_assert(q_empty(q));
    ck_assert_ptr_null(q_peek(q));
    ck_assert(q_free(q));
}
END_TEST

START_TEST(nullHandleOps) {
    void *val = (void *)0x1;
    ck_assert(q_empty(NULL));
    ck_assert(!q_free(NULL));
    ck_assert(!q_enqueue(NULL, &val));
    ck_assert(!q_dequeue(NULL, &val));
    ck_assert_ptr_null(val);
    ck_assert_ptr_null(q_peek(NULL));
}
END_TEST

START_TEST(fifoOrder) {
    int a = 1, b = 2, c = 3;
    void *out = NULL;
    queue q = q_create();
    ck_assert_ptr_nonnull(q);

    ck_assert(q_enqueue(q, &a));
    ck_assert(q_enqueue(q, &b));
    ck_assert(q_enqueue(q, &c));
    ck_assert(!q_empty(q));

    ck_assert(q_dequeue(q, &out));
    ck_assert_ptr_eq(out, &a);
    ck_assert(q_dequeue(q, &out));
    ck_assert_ptr_eq(out, &b);
    ck_assert(q_dequeue(q, &out));
    ck_assert_ptr_eq(out, &c);

    ck_assert(q_empty(q));
    ck_assert(q_free(q));
}
END_TEST

START_TEST(peekDoesNotRemove) {
    int a = 10, b = 20;
    void *out = NULL;
    queue q = q_create();
    ck_assert_ptr_nonnull(q);

    ck_assert(q_enqueue(q, &a));
    ck_assert(q_enqueue(q, &b));
    ck_assert_ptr_eq(q_peek(q), &a);
    ck_assert_ptr_eq(q_peek(q), &a);

    ck_assert(q_dequeue(q, &out));
    ck_assert_ptr_eq(out, &a);
    ck_assert_ptr_eq(q_peek(q), &b);

    ck_assert(q_free(q));
}
END_TEST

START_TEST(dequeueEmpty) {
    void *out = (void *)0x1;
    queue q = q_create();
    ck_assert_ptr_nonnull(q);

    ck_assert(!q_dequeue(q, &out));
    ck_assert_ptr_null(out);
    ck_assert(!q_dequeue(q, NULL));

    ck_assert(q_free(q));
}
END_TEST

START_TEST(nullPayloadAllowed) {
    void *out = (void *)0x1;
    queue q = q_create();
    ck_assert_ptr_nonnull(q);

    ck_assert(q_enqueue(q, NULL));
    ck_assert(!q_empty(q));
    ck_assert_ptr_null(q_peek(q));
    ck_assert(q_dequeue(q, &out));
    ck_assert_ptr_null(out);
    ck_assert(q_empty(q));

    ck_assert(q_free(q));
}
END_TEST

START_TEST(drainSingleThenReuse) {
    int a = 7, b = 8;
    void *out = NULL;
    queue q = q_create();
    ck_assert_ptr_nonnull(q);

    ck_assert(q_enqueue(q, &a));
    ck_assert(q_dequeue(q, &out));
    ck_assert_ptr_eq(out, &a);
    ck_assert(q_empty(q));

    ck_assert(q_enqueue(q, &b));
    ck_assert_ptr_eq(q_peek(q), &b);
    ck_assert(q_dequeue(q, &out));
    ck_assert_ptr_eq(out, &b);
    ck_assert(q_empty(q));

    ck_assert(q_free(q));
}
END_TEST

Suite *queue_suite(void) {
    Suite *s = suite_create("queue");
    TCase *tc = tcase_create("core");
    tcase_add_test(tc, createEmptyFree);
    tcase_add_test(tc, nullHandleOps);
    tcase_add_test(tc, fifoOrder);
    tcase_add_test(tc, peekDoesNotRemove);
    tcase_add_test(tc, dequeueEmpty);
    tcase_add_test(tc, nullPayloadAllowed);
    tcase_add_test(tc, drainSingleThenReuse);
    suite_add_tcase(s, tc);
    return s;
}

int main(void) {
    Suite *s = queue_suite();
    SRunner *sr = srunner_create(s);
    srunner_set_fork_status(sr, CK_NOFORK);
    srunner_run_all(sr, CK_NORMAL);
    int failed = srunner_ntests_failed(sr);
    srunner_free(sr);
    return (failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
