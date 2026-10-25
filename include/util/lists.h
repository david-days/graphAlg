//
// Created by david on 10/21/26.
//

#ifndef GRAPHALG_LISTS_H
#define GRAPHALG_LISTS_H

#include <stddef.h>

typedef struct list_item_t list_item_t, *list_item;

/**
 * doubly-linked holder for list data
 */
struct list_item_t {
    void *data;
    list_item prev;
    list_item next;
    double weight;
};


typedef struct list_t list_t, *list;

/**
 * List structure holding item chain and count
 */
struct list_t {
    list_item head;
    size_t count;
};

/**
 * Creates a new list_t instance
 * @return pointer to a list_t instance
 */
list list_create();

/**
 *
 * @param lst pointer to list to be freed
 * @return true if successful, false if the list is already NULL;
 */
int list_free(list lst);

/**
 * Add an item to the end of the list chain
 * @param lst List to be used
 * @param data void pointer to data
 * @return true if successful, otherwise false
 */
int list_add(list lst, void *data, double wt);

/**
 * Non-destructive retrieval of an item at a given index
 * @param lst pointer to list to be used
 * @param index index of item to be retrieved
 * @param out_data pointer to void pointer to be set to the data at the given index
 * @return true if successful, otherwise false (NULL list, no items, index out of bounds)
 */
int list_getAt(list lst, size_t index, void **out_data);

/**
 * Insert a new value into the list at a given index. item at that index is moved down the list
 * @param lst List to be used
 * @param index Index in the items where the new item will be inserted
 * @param data value for the new item
 * @return true if the insertion was successful, false if there was an invalid condition or error
 */
int list_insertAt(list lst, size_t index, void *data, double wt);

/**
 * Remove an item at the given index.  Any items after, if they exist, will be moved up
 * @param lst list to be used
 * @param index Index of the item to be removed
 * @param out_data Data pointer to be set to the data at the given index
 * @return true if the item was successfully removed, otherwise false
 */
int list_removeAt(list lst, size_t index, void **out_data);;


/**
 * Returns the last item in the list pointed to by the curr head
 * @param cur Head of the list to be used
 * @return pointer to last list_item_t in the list.
 */
list_item list_getTailItem(list_item cur);

/**
 *
 * @param a list_item_t pointer of the first item whose contents will be swapped
 * @param b list_item_t pointer of the second item whose contents will be swapped
 * @return true if the swap was successful, otherwise false
 */
int list_swap(list_item a, list_item b);

#endif //GRAPHALG_LISTS_H
