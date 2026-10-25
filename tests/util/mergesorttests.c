/**
 * Unit tests for util/mergesort.
 */

#include <check.h>
#include <stdlib.h>
#include <util/lists.h>
#include <util/mergesort.h>

static void assert_sorted_by_weight(list_item head) {
    list_item cur = head;

    while (cur != NULL && cur->next != NULL) {
        ck_assert(cur->weight <= cur->next->weight);
        ck_assert_ptr_eq(cur->next->prev, cur);
        cur = cur->next;
    }
}

static size_t count_items(list_item head) {
    size_t count = 0;
    list_item cur = head;

    while (cur != NULL) {
        count++;
        cur = cur->next;
    }

    return count;
}

START_TEST(sortNullHead) {
    ck_assert_ptr_null(mergeSort(NULL));
}
END_TEST

START_TEST(sortSingleItem) {
    int a = 1;
    list lst = list_create();
    ck_assert_ptr_nonnull(lst);

    ck_assert(list_add(lst, &a, 1.0));

    list sorted = mergeSort(lst);

    ck_assert_ptr_eq(sorted, lst);
    ck_assert_uint_eq(count_items(sorted->head), 1);
    ck_assert_ptr_eq(sorted->head->data, &a);
    ck_assert_double_eq(sorted->head->weight, 1.0);
    ck_assert_ptr_null(sorted->head->prev);
    ck_assert_ptr_null(sorted->head->next);

    ck_assert(list_free(lst));
}
END_TEST

START_TEST(sortTwoAlreadySorted) {
    int a = 1, b = 2;
    list lst = list_create();
    ck_assert_ptr_nonnull(lst);

    ck_assert(list_add(lst, &a, 1.0));
    ck_assert(list_add(lst, &b, 2.0));

    list sorted = mergeSort(lst);

    ck_assert_ptr_eq(sorted, lst);
    ck_assert_uint_eq(count_items(sorted->head), 2);
    assert_sorted_by_weight(sorted->head);
    ck_assert_ptr_eq(sorted->head->data, &a);
    ck_assert_ptr_eq(sorted->head->next->data, &b);

    ck_assert(list_free(lst));
}
END_TEST

START_TEST(sortTwoReversed) {
    int a = 1, b = 2;
    list lst = list_create();
    ck_assert_ptr_nonnull(lst);

    ck_assert(list_add(lst, &b, 2.0));
    ck_assert(list_add(lst, &a, 1.0));

    list sorted = mergeSort(lst);

    ck_assert_ptr_eq(sorted, lst);
    ck_assert_uint_eq(count_items(sorted->head), 2);
    assert_sorted_by_weight(sorted->head);
    ck_assert_ptr_eq(sorted->head->data, &a);
    ck_assert_ptr_eq(sorted->head->next->data, &b);

    ck_assert(list_free(lst));
}
END_TEST

START_TEST(sortSeveralUnsorted) {
    int a = 1, b = 2, c = 3, d = 4, e = 5;
    list lst = list_create();
    ck_assert_ptr_nonnull(lst);

    ck_assert(list_add(lst, &e, 5.0));
    ck_assert(list_add(lst, &b, 2.0));
    ck_assert(list_add(lst, &d, 4.0));
    ck_assert(list_add(lst, &a, 1.0));
    ck_assert(list_add(lst, &c, 3.0));

    list sorted = mergeSort(lst);

    ck_assert_ptr_eq(sorted, lst);
    ck_assert_uint_eq(count_items(sorted->head), 5);
    assert_sorted_by_weight(sorted->head);

    ck_assert_ptr_eq(sorted->head->data, &a);
    ck_assert_ptr_eq(sorted->head->next->data, &b);
    ck_assert_ptr_eq(sorted->head->next->next->data, &c);
    ck_assert_ptr_eq(sorted->head->next->next->next->data, &d);
    ck_assert_ptr_eq(sorted->head->next->next->next->next->data, &e);

    ck_assert(list_free(lst));
}
END_TEST

START_TEST(sortAlreadySorted) {
    int a = 1, b = 2, c = 3, d = 4, e = 5;
    list lst = list_create();
    ck_assert_ptr_nonnull(lst);

    ck_assert(list_add(lst, &a, 1.0));
    ck_assert(list_add(lst, &b, 2.0));
    ck_assert(list_add(lst, &c, 3.0));
    ck_assert(list_add(lst, &d, 4.0));
    ck_assert(list_add(lst, &e, 5.0));

    list sorted = mergeSort(lst);

    ck_assert_ptr_eq(sorted, lst);
    ck_assert_uint_eq(count_items(sorted->head), 5);
    assert_sorted_by_weight(sorted->head);

    ck_assert_ptr_eq(sorted->head->data, &a);
    ck_assert_ptr_eq(sorted->head->next->data, &b);
    ck_assert_ptr_eq(sorted->head->next->next->data, &c);
    ck_assert_ptr_eq(sorted->head->next->next->next->data, &d);
    ck_assert_ptr_eq(sorted->head->next->next->next->next->data, &e);

    ck_assert(list_free(lst));
}
END_TEST

START_TEST(sortReverseSorted) {
    int a = 1, b = 2, c = 3, d = 4, e = 5;
    list lst = list_create();
    ck_assert_ptr_nonnull(lst);

    ck_assert(list_add(lst, &e, 5.0));
    ck_assert(list_add(lst, &d, 4.0));
    ck_assert(list_add(lst, &c, 3.0));
    ck_assert(list_add(lst, &b, 2.0));
    ck_assert(list_add(lst, &a, 1.0));

    list sorted = mergeSort(lst);

    ck_assert_ptr_eq(sorted, lst);
    ck_assert_uint_eq(count_items(sorted->head), 5);
    assert_sorted_by_weight(sorted->head);

    ck_assert_ptr_eq(sorted->head->data, &a);
    ck_assert_ptr_eq(sorted->head->next->data, &b);
    ck_assert_ptr_eq(sorted->head->next->next->data, &c);
    ck_assert_ptr_eq(sorted->head->next->next->next->data, &d);
    ck_assert_ptr_eq(sorted->head->next->next->next->next->data, &e);

    ck_assert(list_free(lst));
}
END_TEST

START_TEST(sortDuplicateWeights) {
    int a = 1, b = 2, c = 3, d = 4;
    list lst = list_create();
    ck_assert_ptr_nonnull(lst);

    ck_assert(list_add(lst, &a, 2.0));
    ck_assert(list_add(lst, &b, 1.0));
    ck_assert(list_add(lst, &c, 2.0));
    ck_assert(list_add(lst, &d, 1.0));

    list sorted = mergeSort(lst);

    ck_assert_ptr_eq(sorted, lst);
    ck_assert_uint_eq(count_items(sorted->head), 4);
    assert_sorted_by_weight(sorted->head);

    ck_assert_double_eq(sorted->head->weight, 1.0);
    ck_assert_double_eq(sorted->head->next->weight, 1.0);
    ck_assert_double_eq(sorted->head->next->next->weight, 2.0);
    ck_assert_double_eq(sorted->head->next->next->next->weight, 2.0);

    ck_assert(list_free(lst));
}
END_TEST

START_TEST(sortNegativeAndFractionalWeights) {
    int a = 1, b = 2, c = 3, d = 4;
    list lst = list_create();
    ck_assert_ptr_nonnull(lst);

    ck_assert(list_add(lst, &a, 3.5));
    ck_assert(list_add(lst, &b, -1.0));
    ck_assert(list_add(lst, &c, 0.25));
    ck_assert(list_add(lst, &d, -2.5));

    list sorted = mergeSort(lst);

    ck_assert_ptr_eq(sorted, lst);
    ck_assert_uint_eq(count_items(sorted->head), 4);
    assert_sorted_by_weight(sorted->head);

    ck_assert_ptr_eq(sorted->head->data, &d);
    ck_assert_ptr_eq(sorted->head->next->data, &b);
    ck_assert_ptr_eq(sorted->head->next->next->data, &c);
    ck_assert_ptr_eq(sorted->head->next->next->next->data, &a);

    ck_assert(list_free(lst));
}
END_TEST

Suite *mergesort_suite(void) {
    Suite *s = suite_create("mergesort");
    TCase *tc = tcase_create("core");

    tcase_add_test(tc, sortNullHead);
    tcase_add_test(tc, sortSingleItem);
    tcase_add_test(tc, sortTwoAlreadySorted);
    tcase_add_test(tc, sortTwoReversed);
    tcase_add_test(tc, sortSeveralUnsorted);
    tcase_add_test(tc, sortAlreadySorted);
    tcase_add_test(tc, sortReverseSorted);
    tcase_add_test(tc, sortDuplicateWeights);
    tcase_add_test(tc, sortNegativeAndFractionalWeights);

    suite_add_tcase(s, tc);
    return s;
}

int main(void) {
    Suite *suite = mergesort_suite();
    SRunner *sr = srunner_create(suite);
    srunner_set_fork_status(sr, CK_NOFORK);
    srunner_run_all(sr, CK_NORMAL);
    int failed = srunner_ntests_failed(sr);
    srunner_free(sr);
    return (failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}