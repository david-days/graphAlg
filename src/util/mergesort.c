//
// Created by david on 10/25/26.
//

#include <util/lists.h>
#include <util/mergesort.h>

static list_item split(list_item head) {
    list_item fast = head;
    list_item slow = head;

    // Move fast pointer two steps and slow pointer
    // one step until fast reaches the end
    while (fast != NULL && fast->next != NULL && fast->next->next != NULL) {
        fast = fast->next->next;
        slow = slow->next;
    }

    // Split the list into two halves
    list_item temp = slow->next;
    slow->next = NULL;
    if (temp != NULL) {
        temp->prev = NULL;
    }
    return temp;
}

static list_item merge(list_item first, list_item second) {
    // If either list is empty, return the other list
    if (first == NULL) return second;
    if (second == NULL) return first;

    // Pick the smaller value between first and second nodes
    if (first->weight < second->weight) {

        // Recursively merge the rest of the lists and
        // link the result to the current node
        first->next = merge(first->next, second);
        if (first->next != NULL) {
            first->next->prev = first;
        }
        first->prev = NULL;
        return first;
    } else {
        // Recursively merge the rest of the lists
        // and link the result to the current node
        second->next = merge(first, second->next);
        if (second->next != NULL) {
            second->next->prev = second;
        }
        second->prev = NULL;
        return second;
    }
}

static list_item mergeSortInternal(list_item head) {
    // Base case:  we've reach the end of the (sub) list
    if (head == NULL || head->next == NULL) {
        return head;
    }

    // Split the list into two halves
    list_item second = split(head);

    // Recursively sort each half
    head = mergeSortInternal(head);
    second = mergeSortInternal(second);

    // Merge the two sorted halves
    return merge(head, second);
}

list mergeSort(list lst) {
    if (lst == NULL) return lst;

    // Base case: if the list is empty or has only one node,
    // it's already sorted
    if (lst->head == NULL || lst->head->next == NULL)
        return lst;

    lst->head =mergeSortInternal(lst->head);
    return lst;

}