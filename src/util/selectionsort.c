//
// Created by david on 10/24/26.
//
#include <util/selectionsort.h>

/* Minimum-weight node in the suffix starting at `start`. Caller guarantees start != NULL. */
static list_item minItem(list_item start) {
    list_item min = start;
    for (list_item cur = start->next; cur != NULL; cur = cur->next) {
        if (cur->weight < min->weight) {
            min = cur;
        }
    }
    return min;
}

list selectionSort(list lst) {
    if (lst == NULL) return NULL;
    if (lst->count < 2) return lst;

    for (list_item curr = lst->head; curr != NULL && curr->next != NULL; curr = curr->next) {
        list_item min = minItem(curr);
        if (curr != min) {
            list_swap(curr, min);
        }
    }
    return lst;
}
