//
// Created by david on 10/19/26.
//

#ifndef GRAPHALG_HEAP_H
#define GRAPHALG_HEAP_H

#include <stddef.h>
#include <stdbool.h>

typedef struct {
    void *data;
    size_t priority;
} hitem_t;

typedef hitem_t *hitem;

typedef struct {
    hitem_t *items; /* array; index 0 unused */
    size_t size;
    size_t capacity;
    bool is_min;
} heap_t;

typedef heap_t *heap;

/**
 * Creates a new empty heap.
 * @param capacity max number of elements (fixed)
 * @param is_min true for min-heap, false for max-heap
 * @return heap handle, or NULL on allocation failure
 */
heap h_create(size_t capacity, bool is_min);

/**
 * Checks if the heap is empty.
 * @param h heap to be checked
 * @return true if empty (or h is NULL), otherwise false
 */
int h_empty(heap h);

/**
 * Frees the items array and handle. Does not free pushed payloads.
 * @param h heap to be freed
 * @return true if successful, otherwise false
 */
int h_free(heap h);

/**
 * Returns the root payload without removing it.
 * @param h heap to be used
 * @return root payload, or NULL if empty
 */
void *h_peek(heap h);

/**
 * Push a value with the given priority.
 * @param h heap to be used
 * @param data payload (caller retains ownership)
 * @param priority ordering key
 * @return true if successful, otherwise false (NULL handle or full)
 */
int h_push(heap h, void *data, size_t priority);

/**
 * Remove the root item and optionally return its value.
 * @param h heap to be used
 * @param val out-param for the payload; set to NULL on failure. May be NULL.
 * @return true if an item was popped, false if empty/invalid
 */
int h_pop(heap h, void **val);

#endif //GRAPHALG_HEAP_H
