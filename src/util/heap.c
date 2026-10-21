//
// Created by david on 10/19/26.
//
#include <stdbool.h>
#include <stdlib.h>
#include <util/heap.h>

static void swap(hitem a, hitem b) {
    hitem_t tmp = *a;
    *a = *b;
    *b = tmp;
}

static void heapify_up(heap h, size_t index) {
    while (index > 1) {
        size_t parent = index / 2;
        if ((h->is_min && h->items[index].priority < h->items[parent].priority) ||
            (!h->is_min && h->items[index].priority > h->items[parent].priority)) {
            swap(&h->items[index], &h->items[parent]);
            index = parent;
        } else {
            break;
        }
    }
}

static void heapify_down(heap h, size_t index) {
    while (2 * index <= h->size) {
        size_t child = 2 * index;
        if (child + 1 <= h->size &&
            ((h->is_min && h->items[child + 1].priority < h->items[child].priority) ||
             (!h->is_min && h->items[child + 1].priority > h->items[child].priority))) {
            child++;
        }
        if ((h->is_min && h->items[index].priority > h->items[child].priority) ||
            (!h->is_min && h->items[index].priority < h->items[child].priority)) {
            swap(&h->items[index], &h->items[child]);
            index = child;
        } else {
            break;
        }
    }
}

heap h_create(size_t capacity, bool is_min) {
    heap h = malloc(sizeof(heap_t));
    if (h == NULL) {
        return NULL;
    }
    h->items = malloc((capacity + 1) * sizeof(hitem_t));
    if (h->items == NULL) {
        free(h);
        return NULL;
    }
    h->size = 0;
    h->capacity = capacity;
    h->is_min = is_min;
    return h;
}

int h_empty(heap h) {
    return h == NULL || h->size == 0;
}

int h_free(heap h) {
    if (h == NULL) {
        return false;
    }
    free(h->items);
    free(h);
    return true;
}

void *h_peek(heap h) {
    if (h_empty(h)) {
        return NULL;
    }
    return h->items[1].data;
}

int h_push(heap h, void *data, size_t priority) {
    if (h == NULL || h->size >= h->capacity) {
        return false;
    }
    h->size++;
    h->items[h->size].data = data;
    h->items[h->size].priority = priority;
    heapify_up(h, h->size);
    return true;
}

int h_pop(heap h, void **val) {
    if (val != NULL) {
        *val = NULL;
    }
    if (h_empty(h)) {
        return false;
    }

    if (val != NULL) {
        *val = h->items[1].data;
    }
    h->items[1] = h->items[h->size];
    h->size--;
    if (h->size >= 1) {
        heapify_down(h, 1);
    }
    return true;
}
