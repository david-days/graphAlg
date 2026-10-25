/**
 * @file graphAlgCut.h
 * @brief Public cut / max-flow result types for graphAlg.
 *
 * Binary s–t partition plus the cut-set in graph coordinates (node id pairs
 * and capacities). Algorithm entry points (Edmonds–Karp, BK, …) will populate
 * these structs later; multilabel / move-making belongs in a separate header.
 *
 * Node identifiers are size_t, matching the installed graphData public API.
 * Result buffers are always-copy: release with graphAlgCutResultDestroy().
 */

#ifndef GRAPHALG_CUT_H
#define GRAPHALG_CUT_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Side of an s–t cut for a node assignment. */
typedef enum {
    GRAPHALG_CUT_SOURCE = 0,
    GRAPHALG_CUT_SINK = 1
} graphAlgCutSide;

/**
 * One edge in the cut-set, in graph coordinates.
 *
 * For a certified min-cut, these are the edges with u on the source side and
 * v on the sink side (or one record per undirected link — solver documents
 * which). capacity is the original link capacity that contributes to the cut
 * value, not residual capacity.
 */
typedef struct {
    size_t u;
    size_t v;
    double capacity;
} graphAlgCutEdge;

/**
 * Result of a binary s–t cut / max-flow solve.
 *
 * assign[i] corresponds to node index i in the solve's node ordering (to be
 * documented by each algorithm entry point). cutEdges lists boundary edges for
 * spatial / navigation translation.
 *
 * If certifiedMaxFlow is 0, the partition and cutEdges describe a feasible
 * cut from an anytime / early exit, not a proved minimum cut.
 *
 * scratch is reserved for warm-start (residual / search trees). Until that
 * API exists it must be NULL; destroy does not free a non-NULL scratch.
 */
typedef struct {
    double flowValue;
    int certifiedMaxFlow;
    graphAlgCutSide *assign;
    size_t n;
    graphAlgCutEdge *cutEdges;
    size_t nCutEdges;
    void *scratch;
} graphAlgCutResult;

/**
 * Free owned buffers inside @p r and zero the struct.
 * Does not free @p r itself (stack or caller-owned heap). Safe on NULL.
 * Does not free scratch; leave scratch NULL until warm-start is defined.
 */
void graphAlgCutResultDestroy(graphAlgCutResult *r);

#ifdef __cplusplus
}
#endif

#endif /* GRAPHALG_CUT_H */
