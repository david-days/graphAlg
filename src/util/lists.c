//
// Created by david on 10/22/26.
//

#include <stdbool.h>
#include <stdlib.h>
#include <util/lists.h>

static list_item list_createItem(void *data) {
    list_item newNode = malloc(sizeof(list_item_t));
    if (newNode == NULL) return NULL;
    newNode->data = data;
    newNode->prev = NULL;
    newNode->next = NULL;
    newNode->weight = 0.0;
    return newNode;
}

static int list_appendItem(list lst, list_item newNode) {
    if (lst == NULL || newNode == NULL) return false;
    list_item tail = list_getTailItem(lst->head);
    if (tail == NULL) {
        // empty list--adding first item
        newNode->prev = NULL;
        newNode->next = NULL;
        lst->head = newNode;
        return true;
    }
    newNode->prev = tail;
    newNode->next = NULL;
    tail->next = newNode;
    return true;
}

static void list_freeItem(list_item item) {
    item->data = NULL;
    item->prev = NULL;
    item->next = NULL;
    free(item);
}

list list_create() {
    list lst = malloc(sizeof(list_t));
    if (lst == NULL)
        return NULL;
    lst->head = NULL;
    lst->count = 0;
    return lst;
}

int list_free(list lst) {
    if (lst == NULL) return false;
    list_item cur = lst->head;
    while (cur != NULL) {
        list_item next = cur->next;
        list_freeItem(cur);
        cur = next;
    }
    lst->head = NULL;
    free(lst);
    return true;
}

int list_add(list lst, void *data, double wt) {
    if (lst == NULL) return false;
    list_item newNode = list_createItem(data);
    if (newNode == NULL) return false;
    newNode->weight = wt;
    if (list_appendItem(lst, newNode)) {
        lst->count++;
        return true;
    }
    return false;
}

int list_getAt(list lst, size_t index, void **out_data) {
    // check invalid conditions
    if (out_data == NULL) return false;
    *out_data = NULL;
    if (lst == NULL) return false;
    if (lst->head == NULL) return false;
    if (index >= lst->count) return false;
    //all good--start the real work
    list_item cur = lst->head;
    for (size_t i = 0; i < index && cur != NULL; i++) {
        cur = cur->next;
    }
    if (cur == NULL) return false;
    *out_data = cur->data;
    return true;
}

int list_insertAt(list lst, size_t index, void *data, double wt) {
    if (lst == NULL) return false;
    if (index > lst->count) return false;
    if (lst->count != 0 && lst->head == NULL) return false;
    //ready to do the insertion
    list_item newItem = list_createItem(data);
    if (newItem == NULL) return false;
    newItem->weight = wt;
    //empty list degenerate case
    if (lst->count == 0 && lst->head == NULL) {
        newItem->prev = NULL;
        newItem->next = NULL;
        lst->head = newItem;
        lst->count++;
        return true;
    }

    //handle index being at the end
    if (index == lst->count) {
        if (list_appendItem(lst, newItem)) {
            lst->count++;
            return true;
        }
        list_freeItem(newItem);
        return false;
    }
    list_item cur = lst->head;
    for (size_t i = 0; i < index && cur != NULL; i++) {
        cur = cur->next;
    }
    if (cur == NULL) return false;
    newItem->next = cur;
    newItem->prev = cur->prev;
    if (cur->prev != NULL) {
        cur->prev->next = newItem;
    } else {
        lst->head = newItem;
    }
    cur->prev = newItem;
    lst->count++;
    return true;
}

int list_removeAt(list lst, size_t index, void **out_data) {
    if (out_data != NULL) *out_data = NULL;
    if (lst == NULL) return false;
    if (index >= lst->count) return false;
    if (lst->count != 0 && lst->head == NULL) return false;
    //from this point, assume valid
    list_item cur = lst->head;
    for (size_t i=0; i<index && cur != NULL; i++) {
        cur = cur->next;
    }
    if (cur == NULL) return false;
    if (out_data != NULL) *out_data = cur->data;
    list_item prior = cur->prev;
    list_item desc = cur->next;
    if (prior != NULL) {
        prior->next = desc;
    } else {
        lst->head = desc;
    }
    if (desc != NULL) {
        desc->prev = prior;
    }
    cur->data = NULL;
    cur->prev = NULL;
    cur->next = NULL;
    free(cur);
    lst->count--;
    return true;
}

list_item list_getTailItem(list_item cur) {
    while (cur != NULL && cur->next != NULL)
        cur = cur->next;
    return cur;
}

int list_swap(list_item a, list_item b) {
    if (a == NULL || b == NULL) return false;
    void *tmpData = a->data;
    double tmpWeight = a->weight;

    a->data = b->data;
    a->weight = b->weight;

    b->data = tmpData;
    b->weight = tmpWeight;
    return true;
}