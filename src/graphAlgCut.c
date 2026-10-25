#include <graphAlgCut.h>

#include <stdlib.h>
#include <string.h>

void graphAlgCutResultDestroy(graphAlgCutResult *r) {
    if (r == NULL) {
        return;
    }

    free(r->assign);
    free(r->cutEdges);
    /* scratch: reserved; freer TBD when warm-start lands */

    memset(r, 0, sizeof(*r));
}
