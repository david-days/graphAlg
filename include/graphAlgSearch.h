/**
 * @file graphAlgSearch.h
 * @brief Search / reachability family over graphData: paths, single-source
 * trees, and connected components (undirected / symmetric adjacency).
 *
 * Node identifiers are size_t, matching the installed graphData public API.
 * Result buffers are always-copy: release with the matching *Destroy helper.
 * Algorithm scratch (queue, visited set, parent map) stays inside graphAlg —
 * never on graph_t via impl/ headers.
 *
 * Topic headers may be reshuffled later; this file is the current home for
 * "what can you search / reach" APIs.
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

/**
 * Single-source DFS: all nodes reachable from source.
 *
 * On success, out holds the reachable set in discovery order with parallel
 * parent (DFS tree) and dist.  dist[i] is hops from source along the DFS
 * tree (tree depth), not unweighted shortest-path distance in the graph.
 *
 * Follows outgoing neighbors as exposed by ops->getNeighbors (directed as
 * stored; undirected backends already present both directions).
 *
 * @return GRAPH_OK on success; GRAPH_ERR if bad args, source absent, or OOM
 */
int graphAlgDfsFrom(struct graphops_t *ops, struct graph_t *g,
                    size_t source,
                    graphAlgPathResult *out);

/**
 * Partition of graph nodes into connected components.
 *
 * Jagged always-copy layout:
 * - nodes[c][0 .. sizes[c]) — node ids in component c (DfsFrom discovery order)
 * - sizes[c] — length of nodes[c]
 * - n — component count
 *
 * Component order follows listNodes root order (first unvisited node starts
 * a new component). On GRAPH_OK the caller owns all buffers; release with
 * graphAlgComponentsResultDestroy(). On GRAPH_ERR the implementation leaves
 * *out clean (no partial ownership).
 */
typedef struct {
    size_t **nodes;
    size_t *sizes;
    size_t n;
} graphAlgComponentsResult;

/**
 * Free owned buffers inside @p r and zero the struct.
 * Does not free @p r itself. Safe on NULL.
 */
void graphAlgComponentsResultDestroy(graphAlgComponentsResult *r);

/**
 * Connected components under getNeighbors as stored.
 *
 * Requires undirected semantics via both directions already present in the
 * adjacency the backend exposes. LINKED|UNDIRECTED (and ARRAY undirected) store
 * a single min(u,v)→max(u,v) record, so getNeighbors is not symmetric there —
 * do not rely on that flag alone. Prefer LINKED|DIRECTED with both arcs, or
 * HASHED|UNDIRECTED. Digraph weak-CC and strongly connected components are
 * deferred. Implementation reuses graphAlgDfsFrom from each unvisited
 * listNodes root.
 *
 * @return GRAPH_OK on success; GRAPH_ERR if bad args or OOM
 */
int graphAlgConnectedComponents(struct graphops_t *ops, struct graph_t *g,
                                graphAlgComponentsResult *out);


/**
 * Edge weight for weighted search.
 * @return non-negative finite cost, 0 for free/"must travel",
 *         +INFINITY to treat as non-edge, NaN/-INFINITY invalid
 */
typedef double (*graphAlgEdgeWeightFn)(size_t u, size_t v,
                                      double cap, double flow,
                                      void *ctx);

typedef struct {
    size_t *nodes;    /* length n — see contract */
    double *dist;     /* parallel; 0 at source; +INFINITY if unreachable */
    size_t *parent;   /* predecessor id; source: parent == itself;
                         unreachable: parent == SIZE_MAX (recommend) */
    size_t n;
} graphAlgWeightedPathResult;

void graphAlgWeightedPathResultDestroy(graphAlgWeightedPathResult *r);

/**
 * Weighted shortest path source → target (Dijkstra).
 *
 * weight_fn NULL → use edge cap. ctx may be NULL.
 * Zero weights allowed. Negative weight → GRAPH_ERR.
 * +INFINITY from weight_fn (or cap) → skip that arc.
 * NaN or -INFINITY from weight_fn → GRAPH_ERR.
 *
 * On GRAPH_OK: out holds the path source..target (n >= 1), dist finite.
 * Unreachable target / bad args / OOM → GRAPH_ERR, *out clean.
 */
int graphAlgDijkstraPath(struct graphops_t *ops, struct graph_t *g,
                         size_t source, size_t target,
                         graphAlgEdgeWeightFn weight_fn, void *ctx,
                         graphAlgWeightedPathResult *out);

#ifdef __cplusplus
}
#endif

#endif /* GRAPHALG_SEARCH_H */
