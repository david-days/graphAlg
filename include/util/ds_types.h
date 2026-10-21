//
// Created by david on 10/18/26.
//

#ifndef GRAPHALG_DS_TYPES_H
#define GRAPHALG_DS_TYPES_H

/**
 * Doubly-linked list node (used by queue).
 */
typedef struct item_t item_t, *item;

struct item_t {
    void *data;
    item prev;
    item next;
};

#endif //GRAPHALG_DS_TYPES_H
