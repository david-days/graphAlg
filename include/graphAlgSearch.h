/**
 * @file graphAlgSearch.h
 * @brief BFS / DFS family: path and reachability results over graphData.
 *
 * Node identifiers are size_t, matching the installed graphData public API.
 * Result buffers are always-copy: release with graphAlgPathResultDestroy().
 * Algorithm scratch (queue, visited set, parent map) stays inside graphAlg —
 * never on graph_t via impl/ headers.
 */

#ifndef GRAPHALG_SEARCH_H
#define GRAPHALG_SEARCH_H

#include <stddef.h>
#include <graphData.h>
#include <graphOps.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Path or single-source search tree in graph coordinates.
 *
 * Parallel arrays of length n:
 * - nodes[i]  — node id (discovery order for BfsFrom; source..target for BfsPath)
 * - dist[i]   — hop distance from source
 * - parent[i] — predecessor id; for the source entry, parent[i] == nodes[i]
 *
 * On GRAPH_OK the caller owns the buffers; release with
 * graphAlgPathResultDestroy(). On GRAPH_ERR the implementation leaves *out
 * clean (no partial ownership).
 */
typedef struct {
    size_t *nodes;
    size_t *dist;
    size_t *parent;
    size_t n;
} graphAlgPathResult;

/**
 * Free owned buffers inside @p r and zero the struct.
 * Does not free @p r itself. Safe on NULL.
 */
void graphAlgPathResultDestroy(graphAlgPathResult *r);

/**
 * Unweighted shortest path from source to target via BFS.
 *
 * Follows outgoing neighbors as exposed by ops->getNeighbors (directed as
 * stored; undirected backends already present both directions). Stops when
 * target is dequeued. On success, out holds the path with parallel dist/parent.
 *
 * @return GRAPH_OK on success; GRAPH_ERR if unreachable, bad args, or OOM
 */
int graphAlgBfsPath(struct graphops_t *ops, struct graph_t *g,
                    size_t source, size_t target,
                    graphAlgPathResult *out);

/**
 * Single-source BFS: all nodes reachable from source.
 *
 * On success, out holds the reachable set in discovery order with parallel
 * dist/parent (full BFS tree).
 *
 * @return GRAPH_OK on success; GRAPH_ERR if bad args, source absent, or OOM
 */
int graphAlgBfsFrom(struct graphops_t *ops, struct graph_t *g,
                    size_t source,
                    graphAlgPathResult *out);

#ifdef __cplusplus
}
#endif

#endif /* GRAPHALG_SEARCH_H */
