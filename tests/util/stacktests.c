/**
 * Unit tests for util/stack.
 */

#include <check.h>
#include <stdlib.h>
#include <util/stack.h>

START_TEST(createEmptyFree) {
    stack s = s_create();
    ck_assert_ptr_nonnull(s);
    ck_assert(s_empty(s));
    ck_assert_ptr_null(s_peek(s));
    ck_assert(s_free(s));
}
END_TEST

START_TEST(nullHandleOps) {
    void *val = (void *)0x1;
    ck_assert(s_empty(NULL));
    ck_assert(!s_free(NULL));
    ck_assert(!s_push(NULL, &val));
    ck_assert(!s_pop(NULL, &val));
    ck_assert_ptr_null(val);
    ck_assert_ptr_null(s_peek(NULL));
}
END_TEST

START_TEST(lifoOrder) {
    int a = 1, b = 2, c = 3;
    void *out = NULL;
    stack s = s_create();
    ck_assert_ptr_nonnull(s);

    ck_assert(s_push(s, &a));
    ck_assert(s_push(s, &b));
    ck_assert(s_push(s, &c));
    ck_assert(!s_empty(s));

    ck_assert(s_pop(s, &out));
    ck_assert_ptr_eq(out, &c);
    ck_assert(s_pop(s, &out));
    ck_assert_ptr_eq(out, &b);
    ck_assert(s_pop(s, &out));
    ck_assert_ptr_eq(out, &a);

    ck_assert(s_empty(s));
    ck_assert(s_free(s));
}
END_TEST

START_TEST(peekDoesNotRemove) {
    int a = 10, b = 20;
    void *out = NULL;
    stack s = s_create();
    ck_assert_ptr_nonnull(s);

    ck_assert(s_push(s, &a));
    ck_assert(s_push(s, &b));
    ck_assert_ptr_eq(s_peek(s), &b);
    ck_assert_ptr_eq(s_peek(s), &b);

    ck_assert(s_pop(s, &out));
    ck_assert_ptr_eq(out, &b);
    ck_assert_ptr_eq(s_peek(s), &a);

    ck_assert(s_free(s));
}
END_TEST

START_TEST(popEmpty) {
    void *out = (void *)0x1;
    stack s = s_create();
    ck_assert_ptr_nonnull(s);

    ck_assert(!s_pop(s, &out));
    ck_assert_ptr_null(out);
    ck_assert(!s_pop(s, NULL));

    ck_assert(s_free(s));
}
END_TEST

START_TEST(nullPayloadAllowed) {
    void *out = (void *)0x1;
    stack s = s_create();
    ck_assert_ptr_nonnull(s);

    ck_assert(s_push(s, NULL));
    ck_assert(!s_empty(s));
    ck_assert_ptr_null(s_peek(s));
    ck_assert(s_pop(s, &out));
    ck_assert_ptr_null(out);
    ck_assert(s_empty(s));

    ck_assert(s_free(s));
}
END_TEST

START_TEST(drainSingleThenReuse) {
    int a = 7, b = 8;
    void *out = NULL;
    stack s = s_create();
    ck_assert_ptr_nonnull(s);

    ck_assert(s_push(s, &a));
    ck_assert(s_pop(s, &out));
    ck_assert_ptr_eq(out, &a);
    ck_assert(s_empty(s));

    ck_assert(s_push(s, &b));
    ck_assert_ptr_eq(s_peek(s), &b);
    ck_assert(s_pop(s, &out));
    ck_assert_ptr_eq(out, &b);
    ck_assert(s_empty(s));

    ck_assert(s_free(s));
}
END_TEST

Suite *stack_suite(void) {
    Suite *s = suite_create("stack");
    TCase *tc = tcase_create("core");
    tcase_add_test(tc, createEmptyFree);
    tcase_add_test(tc, nullHandleOps);
    tcase_add_test(tc, lifoOrder);
    tcase_add_test(tc, peekDoesNotRemove);
    tcase_add_test(tc, popEmpty);
    tcase_add_test(tc, nullPayloadAllowed);
    tcase_add_test(tc, drainSingleThenReuse);
    suite_add_tcase(s, tc);
    return s;
}

int main(void) {
    Suite *suite = stack_suite();
    SRunner *sr = srunner_create(suite);
    srunner_set_fork_status(sr, CK_NOFORK);
    srunner_run_all(sr, CK_NORMAL);
    int failed = srunner_ntests_failed(sr);
    srunner_free(sr);
    return (failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
