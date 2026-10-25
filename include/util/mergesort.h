//
// Created by david on 10/25/26.
//

#ifndef GRAPHALG_MERGESORT_H
#define GRAPHALG_MERGESORT_H

#include <util/lists.h>

/**
 * Sorts a list using the merge sort algorithm.
 *
 * This algorithm does _not_ preserve the link order between list_items themselves, unlike the quicksort and selection sort
 * implementations.  The links will be reshuffled in sort * order with their contents.
 * @param lst The list to sort.
 * @return The sorted list.
 */
list mergeSort(list lst);

#endif //GRAPHALG_MERGESORT_H
