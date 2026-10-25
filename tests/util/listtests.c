/**
 * Unit tests for util/lists.
 */

#include <check.h>
#include <stdlib.h>
#include <util/lists.h>
START_TEST(createEmptyFree) {
    list lst = list_create();
    ck_assert_ptr_nonnull(lst);
    ck_assert_ptr_null(lst->head);
    ck_assert_uint_eq(lst->count, 0);
    ck_assert(list_free(lst));
}
END_TEST

START_TEST(nullHandleOps) {
    int value = 42;
    void *out = (void *)0x1;

    ck_assert(!list_free(NULL));
    ck_assert(!list_add(NULL, &value, 1.0));
    ck_assert(!list_getAt(NULL, 0, &out));
    ck_assert_ptr_null(out);
    ck_assert(!list_insertAt(NULL, 0, &value, 1.0));

    out = (void *)0x1;
    ck_assert(!list_removeAt(NULL, 0, &out));
    ck_assert_ptr_null(out);
}
END_TEST

START_TEST(addSingleItem) {
    int value = 10;
    void *out = NULL;
    list lst = list_create();
    ck_assert_ptr_nonnull(lst);

    ck_assert(list_add(lst, &value, 2.5));
    ck_assert_uint_eq(lst->count, 1);
    ck_assert_ptr_nonnull(lst->head);
    ck_assert_ptr_eq(lst->head->data, &value);
    ck_assert_double_eq(lst->head->weight, 2.5);
    ck_assert_ptr_null(lst->head->prev);
    ck_assert_ptr_null(lst->head->next);

    ck_assert(list_getAt(lst, 0, &out));
    ck_assert_ptr_eq(out, &value);

    ck_assert(list_free(lst));
}
END_TEST

START_TEST(addMultipleItemsMaintainsOrder) {
    int a = 1, b = 2, c = 3;
    void *out = NULL;
    list lst = list_create();
    ck_assert_ptr_nonnull(lst);

    ck_assert(list_add(lst, &a, 1.0));
    ck_assert(list_add(lst, &b, 2.0));
    ck_assert(list_add(lst, &c, 3.0));

    ck_assert_uint_eq(lst->count, 3);

    ck_assert(list_getAt(lst, 0, &out));
    ck_assert_ptr_eq(out, &a);

    ck_assert(list_getAt(lst, 1, &out));
    ck_assert_ptr_eq(out, &b);

    ck_assert(list_getAt(lst, 2, &out));
    ck_assert_ptr_eq(out, &c);

    ck_assert_ptr_null(lst->head->prev);
    ck_assert_ptr_eq(lst->head->next->prev, lst->head);
    ck_assert_ptr_eq(lst->head->next->next->prev, lst->head->next);
    ck_assert_ptr_null(lst->head->next->next->next);

    ck_assert(list_free(lst));
}
END_TEST

START_TEST(getAtRejectsBadIndexesAndNullOutParam) {
    int a = 1;
    void *out = (void *)0x1;
    list lst = list_create();
    ck_assert_ptr_nonnull(lst);

    ck_assert(!list_getAt(lst, 0, &out));
    ck_assert_ptr_null(out);

    ck_assert(list_add(lst, &a, 1.0));

    ck_assert(!list_getAt(lst, 0, NULL));

    out = (void *)0x1;
    ck_assert(!list_getAt(lst, 1, &out));
    ck_assert_ptr_null(out);

    ck_assert(list_free(lst));
}
END_TEST

START_TEST(insertIntoEmptyAtZero) {
    int a = 1;
    void *out = NULL;
    list lst = list_create();
    ck_assert_ptr_nonnull(lst);

    ck_assert(list_insertAt(lst, 0, &a, 10.0));

    ck_assert_uint_eq(lst->count, 1);
    ck_assert_ptr_nonnull(lst->head);
    ck_assert_ptr_eq(lst->head->data, &a);
    ck_assert_double_eq(lst->head->weight, 10.0);

    ck_assert(list_getAt(lst, 0, &out));
    ck_assert_ptr_eq(out, &a);

    ck_assert(list_free(lst));
}
END_TEST

START_TEST(insertAtHead) {
    int a = 1, b = 2;
    void *out = NULL;
    list lst = list_create();
    ck_assert_ptr_nonnull(lst);

    ck_assert(list_add(lst, &b, 2.0));
    ck_assert(list_insertAt(lst, 0, &a, 1.0));

    ck_assert_uint_eq(lst->count, 2);
    ck_assert_ptr_eq(lst->head->data, &a);
    ck_assert_ptr_null(lst->head->prev);
    ck_assert_ptr_nonnull(lst->head->next);
    ck_assert_ptr_eq(lst->head->next->prev, lst->head);

    ck_assert(list_getAt(lst, 0, &out));
    ck_assert_ptr_eq(out, &a);

    ck_assert(list_getAt(lst, 1, &out));
    ck_assert_ptr_eq(out, &b);

    ck_assert(list_free(lst));
}
END_TEST

START_TEST(insertInMiddle) {
    int a = 1, b = 2, c = 3;
    void *out = NULL;
    list lst = list_create();
    ck_assert_ptr_nonnull(lst);

    ck_assert(list_add(lst, &a, 1.0));
    ck_assert(list_add(lst, &c, 3.0));
    ck_assert(list_insertAt(lst, 1, &b, 2.0));

    ck_assert_uint_eq(lst->count, 3);

    ck_assert(list_getAt(lst, 0, &out));
    ck_assert_ptr_eq(out, &a);

    ck_assert(list_getAt(lst, 1, &out));
    ck_assert_ptr_eq(out, &b);

    ck_assert(list_getAt(lst, 2, &out));
    ck_assert_ptr_eq(out, &c);

    ck_assert_ptr_eq(lst->head->next->prev, lst->head);
    ck_assert_ptr_eq(lst->head->next->next->prev, lst->head->next);

    ck_assert(list_free(lst));
}
END_TEST

START_TEST(insertAtTail) {
    int a = 1, b = 2, c = 3;
    void *out = NULL;
    list lst = list_create();
    ck_assert_ptr_nonnull(lst);

    ck_assert(list_add(lst, &a, 1.0));
    ck_assert(list_add(lst, &b, 2.0));
    ck_assert(list_insertAt(lst, 2, &c, 3.0));

    ck_assert_uint_eq(lst->count, 3);

    ck_assert(list_getAt(lst, 2, &out));
    ck_assert_ptr_eq(out, &c);

    ck_assert_ptr_eq(list_getTailItem(lst->head)->data, &c);
    ck_assert_ptr_null(list_getTailItem(lst->head)->next);

    ck_assert(list_free(lst));
}
END_TEST

START_TEST(insertRejectsIndexPastEnd) {
    int a = 1;
    list lst = list_create();
    ck_assert_ptr_nonnull(lst);

    ck_assert(!list_insertAt(lst, 1, &a, 1.0));
    ck_assert_uint_eq(lst->count, 0);
    ck_assert_ptr_null(lst->head);

    ck_assert(list_free(lst));
}
END_TEST

START_TEST(removeOnlyItem) {
    int a = 1;
    void *out = NULL;
    list lst = list_create();
    ck_assert_ptr_nonnull(lst);

    ck_assert(list_add(lst, &a, 1.0));
    ck_assert(list_removeAt(lst, 0, &out));

    ck_assert_ptr_eq(out, &a);
    ck_assert_uint_eq(lst->count, 0);
    ck_assert_ptr_null(lst->head);

    ck_assert(list_free(lst));
}
END_TEST

START_TEST(removeHead) {
    int a = 1, b = 2, c = 3;
    void *out = NULL;
    list lst = list_create();
    ck_assert_ptr_nonnull(lst);

    ck_assert(list_add(lst, &a, 1.0));
    ck_assert(list_add(lst, &b, 2.0));
    ck_assert(list_add(lst, &c, 3.0));

    ck_assert(list_removeAt(lst, 0, &out));
    ck_assert_ptr_eq(out, &a);
    ck_assert_uint_eq(lst->count, 2);
    ck_assert_ptr_eq(lst->head->data, &b);
    ck_assert_ptr_null(lst->head->prev);

    ck_assert(list_getAt(lst, 0, &out));
    ck_assert_ptr_eq(out, &b);

    ck_assert(list_getAt(lst, 1, &out));
    ck_assert_ptr_eq(out, &c);

    ck_assert(list_free(lst));
}
END_TEST

START_TEST(removeMiddle) {
    int a = 1, b = 2, c = 3;
    void *out = NULL;
    list lst = list_create();
    ck_assert_ptr_nonnull(lst);

    ck_assert(list_add(lst, &a, 1.0));
    ck_assert(list_add(lst, &b, 2.0));
    ck_assert(list_add(lst, &c, 3.0));

    ck_assert(list_removeAt(lst, 1, &out));
    ck_assert_ptr_eq(out, &b);
    ck_assert_uint_eq(lst->count, 2);

    ck_assert(list_getAt(lst, 0, &out));
    ck_assert_ptr_eq(out, &a);

    ck_assert(list_getAt(lst, 1, &out));
    ck_assert_ptr_eq(out, &c);

    ck_assert_ptr_eq(lst->head->next->prev, lst->head);

    ck_assert(list_free(lst));
}
END_TEST

START_TEST(removeTail) {
    int a = 1, b = 2, c = 3;
    void *out = NULL;
    list lst = list_create();
    ck_assert_ptr_nonnull(lst);

    ck_assert(list_add(lst, &a, 1.0));
    ck_assert(list_add(lst, &b, 2.0));
    ck_assert(list_add(lst, &c, 3.0));

    ck_assert(list_removeAt(lst, 2, &out));
    ck_assert_ptr_eq(out, &c);
    ck_assert_uint_eq(lst->count, 2);

    ck_assert_ptr_eq(list_getTailItem(lst->head)->data, &b);
    ck_assert_ptr_null(list_getTailItem(lst->head)->next);

    ck_assert(list_free(lst));
}
END_TEST

START_TEST(removeRejectsBadIndexAndAllowsNullOutParam) {
    int a = 1;
    void *out = (void *)0x1;
    list lst = list_create();
    ck_assert_ptr_nonnull(lst);

    ck_assert(!list_removeAt(lst, 0, &out));
    ck_assert_ptr_null(out);

    ck_assert(list_add(lst, &a, 1.0));

    out = (void *)0x1;
    ck_assert(!list_removeAt(lst, 1, &out));
    ck_assert_ptr_null(out);

    ck_assert(list_removeAt(lst, 0, NULL));
    ck_assert_uint_eq(lst->count, 0);
    ck_assert_ptr_null(lst->head);

    ck_assert(list_free(lst));
}
END_TEST

START_TEST(nullPayloadAllowed) {
    void *out = (void *)0x1;
    list lst = list_create();
    ck_assert_ptr_nonnull(lst);

    ck_assert(list_add(lst, NULL, 1.0));
    ck_assert_uint_eq(lst->count, 1);

    ck_assert(list_getAt(lst, 0, &out));
    ck_assert_ptr_null(out);

    out = (void *)0x1;
    ck_assert(list_removeAt(lst, 0, &out));
    ck_assert_ptr_null(out);
    ck_assert_uint_eq(lst->count, 0);

    ck_assert(list_free(lst));
}
END_TEST

START_TEST(getTailItemCases) {
    int a = 1, b = 2;
    list lst = list_create();
    ck_assert_ptr_nonnull(lst);

    ck_assert_ptr_null(list_getTailItem(NULL));

    ck_assert(list_add(lst, &a, 1.0));
    ck_assert_ptr_eq(list_getTailItem(lst->head), lst->head);

    ck_assert(list_add(lst, &b, 2.0));
    ck_assert_ptr_eq(list_getTailItem(lst->head), lst->head->next);
    ck_assert_ptr_eq(list_getTailItem(lst->head)->data, &b);

    ck_assert(list_free(lst));
}
END_TEST

Suite *list_suite(void) {
    Suite *s = suite_create("list");
    TCase *tc = tcase_create("core");

    tcase_add_test(tc, createEmptyFree);
    tcase_add_test(tc, nullHandleOps);
    tcase_add_test(tc, addSingleItem);
    tcase_add_test(tc, addMultipleItemsMaintainsOrder);
    tcase_add_test(tc, getAtRejectsBadIndexesAndNullOutParam);
    tcase_add_test(tc, insertIntoEmptyAtZero);
    tcase_add_test(tc, insertAtHead);
    tcase_add_test(tc, insertInMiddle);
    tcase_add_test(tc, insertAtTail);
    tcase_add_test(tc, insertRejectsIndexPastEnd);
    tcase_add_test(tc, removeOnlyItem);
    tcase_add_test(tc, removeHead);
    tcase_add_test(tc, removeMiddle);
    tcase_add_test(tc, removeTail);
    tcase_add_test(tc, removeRejectsBadIndexAndAllowsNullOutParam);
    tcase_add_test(tc, nullPayloadAllowed);
    tcase_add_test(tc, getTailItemCases);

    suite_add_tcase(s, tc);
    return s;
}

int main(void) {
    Suite *suite = list_suite();
    SRunner *sr = srunner_create(suite);
    srunner_set_fork_status(sr, CK_NOFORK);
    srunner_run_all(sr, CK_NORMAL);
    int failed = srunner_ntests_failed(sr);
    srunner_free(sr);
    return (failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
