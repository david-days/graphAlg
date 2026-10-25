//
// Created by david on 10/21/26.
//

#include <util/quicksort.h>


// Partitions the list taking the first element as the pivot
static list_item partition(list_item head, list_item tail) {
    list_item pivot = head;
    list_item pre = head;
    list_item curr = head->next;

    // Traverse the list until you reach the node after the tail
    while (curr != tail->next) {
        if (curr->weight < pivot->weight) {
            pre = pre->next;
            list_swap(pre, curr);
        }
        // Move 'curr' to the next node
        curr = curr->next;
    }
    // Swap the pivot's data with 'pre' data
    list_swap(pivot, pre);
    // Return 'pre' as the new pivot
    return pre;
}

// Helper function for quick sort
static void quickSortHelper(list_item head, list_item tail) {
    // Base case: if the list is empty or consists of a single node
    if (head == NULL || tail == NULL || head == tail || head == tail->next) {
        return;
    }

    // Call partition to find the pivot node
    list_item pivot = partition(head, tail);

    // Recursive call for the left part
    // of the list (before the pivot)
    quickSortHelper(head, pivot->prev);

    // Recursive call for the right part
    // of the list (after the pivot)
    quickSortHelper(pivot->next, tail);
}

// The main function for quick sort. This is a
// wrapper over quickSortHelper
list quickSort(list lst) {
    if (lst == NULL) return NULL;
    if (lst->count < 2) return lst;

    // Find the tail of the list
    list_item tail = list_getTailItem(lst->head);

    // Call the helper function to sort the list
    quickSortHelper(lst->head, tail);
    return lst;
}
