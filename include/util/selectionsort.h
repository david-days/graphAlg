//
// Created by david on 10/24/26.
//

#ifndef GRAPHALG_SELECTIONSORT_H
#define GRAPHALG_SELECTIONSORT_H

#include <util/lists.h>

/**
 * Performs selection sort on the given list.
 *
 * This algorithm preserves the links between list_items themselves--the internal data and weights are sorted.
 *
 * @param lst List to be sorted
 * @return Link to the sorted list (should be the same as lst)
 */

list selectionSort(list lst);

#endif //GRAPHALG_SELECTIONSORT_H
