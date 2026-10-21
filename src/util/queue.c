#include <stdbool.h>
#include <stdlib.h>
#include <util/queue.h>

queue q_create(void) {
    queue q = malloc(sizeof(queue_t));
    if (q == NULL) {
        return NULL;
    }
    q->front = NULL;
    q->back = NULL;
    return q;
}

int q_free(queue q) {
    if (q == NULL) {
        return false;
    }
    item curr = q->front;
    while (curr != NULL) {
        item next = curr->next;
        free(curr);
        curr = next;
    }
    free(q);
    return true;
}

int q_empty(queue q) {
    return q == NULL || q->front == NULL;
}

int q_enqueue(queue q, void *v) {
    if (q == NULL) {
        return false;
    }
    item nd = malloc(sizeof(item_t));
    if (nd == NULL) {
        return false;
    }
    nd->data = v;
    nd->prev = q->back;
    nd->next = NULL;
    if (q->back != NULL) {
        q->back->next = nd;
    } else {
        q->front = nd;
    }
    q->back = nd;
    return true;
}

int q_dequeue(queue q, void **val) {
    if (val != NULL) {
        *val = NULL;
    }
    if (q_empty(q)) {
        return false;
    }

    item front = q->front;
    if (val != NULL) {
        *val = front->data;
    }
    q->front = front->next;
    if (q->front != NULL) {
        q->front->prev = NULL;
    } else {
        q->back = NULL;
    }
    free(front);
    return true;
}

void *q_peek(queue q) {
    if (q_empty(q)) {
        return NULL;
    }
    return q->front->data;
}
