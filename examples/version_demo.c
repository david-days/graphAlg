/**
 * Tiny demo against the in-tree graphAlg build.
 *
 * Against an install (once packaged):
 *   gcc version_demo.c $(pkg-config --cflags --libs graphalg)
 */

#include <graphAlg.h>
#include <stdio.h>
#include <stdlib.h>

int main(void) {
    printf("graphAlg %s\n", graphAlgVersion());
    return EXIT_SUCCESS;
}
