//
// Created by david on 10/21/26.
//

#ifndef GRAPHALG_QUICKSORT_H
#define GRAPHALG_QUICKSORT_H

#include <util/lists.h>

/**
 * Performs quicksort on the given list.
 *
 * This algorithm preserves the links between list_items themselves--the internal data and weights are sorted.
 * @param lst List to be sorted
 * @return Link to the sorted list (should be the same as lst)
 */
list quickSort(list lst);

#endif //GRAPHALG_QUICKSORT_H
