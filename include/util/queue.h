//
// Created by david on 10/18/26.
//

#ifndef GRAPHALG_QUEUE_H
#define GRAPHALG_QUEUE_H

#include <util/ds_types.h>

typedef struct {
    item front; /* dequeue end */
    item back;  /* enqueue end */
} queue_t;

typedef queue_t *queue;

/**
 * Creates a new empty queue.
 * @return queue handle, or NULL on allocation failure
 */
queue q_create(void);

/**
 * Checks if the queue is empty.
 * @param q Queue to be checked
 * @return true if empty (or q is NULL), otherwise false
 */
int q_empty(queue q);

/**
 * Frees the queue nodes and handle. Does not free enqueued payloads.
 * @param q Queue to be freed
 * @return true if successful, otherwise false
 */
int q_free(queue q);

/**
 * Returns the next value in the queue without removing it.
 * @param q queue to be used
 * @return front payload, or NULL if empty
 */
void *q_peek(queue q);

/**
 * Add an item to the back of the queue.
 * @param q queue to be used
 * @param v value to be enqueued (caller retains ownership)
 * @return true if successful, otherwise false
 */
int q_enqueue(queue q, void *v);

/**
 * Remove the front item and optionally return its value.
 * @param q queue to be used
 * @param val out-param for the payload; set to NULL on failure. May be NULL.
 * @return true if an item was dequeued, false if empty/invalid
 */
int q_dequeue(queue q, void **val);

#endif //GRAPHALG_QUEUE_H
