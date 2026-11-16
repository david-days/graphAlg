/**
 * Tests for graphAlgDijkstraPath / graphAlgWeightedPathResultDestroy.
 */

#include <check.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#include <graphAlg.h>
#include <graphAlgSearch.h>
#include <graphInit.h>
#include <graphCrud.h>
#include <graphData.h>

static void teardownGraph(struct graphops_t **ops, struct graph_t **g) {
    if (ops != NULL && *ops != NULL) {
        destroyGraphops((void **)ops);
    }
    if (g != NULL && *g != NULL) {
        clearGraph(*g);
        destroyGraph((void **)g);
    }
}

static void assertOutClean(const graphAlgWeightedPathResult *out) {
    ck_assert_ptr_nonnull(out);
    ck_assert_uint_eq(out->n, 0);
    ck_assert_ptr_null(out->nodes);
    ck_assert_ptr_null(out->dist);
    ck_assert_ptr_null(out->parent);
}

static int neighborContains(struct graphops_t *ops, struct graph_t *g,
                            size_t u, size_t v) {
    struct node_t *nbrs = ops->getNeighbors(&u, g);
    int found = 0;
    for (struct node_t *n = nbrs; n != NULL; n = n->next) {
        if (n->nodeid == v) {
            found = 1;
            break;
        }
    }
    destroyNodes((void **)&nbrs);
    return found;
}

/** Contract checks for a successful weighted path (finite, nondecreasing dist). */
static void assertValidWeightedPath(struct graphops_t *ops, struct graph_t *g,
                                    size_t source, size_t target,
                                    const graphAlgWeightedPathResult *out,
                                    size_t expect_hops) {
    ck_assert_ptr_nonnull(out);
    ck_assert_uint_eq(out->n, expect_hops + 1);
    ck_assert_ptr_nonnull(out->nodes);
    ck_assert_ptr_nonnull(out->dist);
    ck_assert_ptr_nonnull(out->parent);

    ck_assert_uint_eq(out->nodes[0], source);
    ck_assert_uint_eq(out->nodes[out->n - 1], target);
    ck_assert_uint_eq(out->parent[0], source);
    ck_assert_double_eq_tol(out->dist[0], 0.0, 1e-12);

    for (size_t i = 1; i < out->n; i++) {
        ck_assert_uint_eq(out->parent[i], out->nodes[i - 1]);
        ck_assert(neighborContains(ops, g, out->nodes[i - 1], out->nodes[i]));
        ck_assert(isfinite(out->dist[i]));
        ck_assert(out->dist[i] >= out->dist[i - 1] - 1e-12);
    }
}

static struct graph_t *newDirectedLinked(void) {
    return initGraph(LINKED | DIRECTED | GENERIC, 0, NULL);
}

static double weightDoubleCap(size_t u, size_t v, double cap, double flow, void *ctx) {
    (void)u;
    (void)v;
    (void)flow;
    (void)ctx;
    return 2.0 * cap;
}

static double weightNegative(size_t u, size_t v, double cap, double flow, void *ctx) {
    (void)u;
    (void)v;
    (void)cap;
    (void)flow;
    (void)ctx;
    return -1.0;
}

typedef struct {
    size_t u;
    size_t v;
} blocked_arc_t;

/** Return +inf only for the single blocked arc in ctx; else 1.0. */
static double weightInfArc(size_t u, size_t v, double cap, double flow, void *ctx) {
    (void)cap;
    (void)flow;
    const blocked_arc_t *block = (const blocked_arc_t *)ctx;
    if (block != NULL && u == block->u && v == block->v) {
        return INFINITY;
    }
    return 1.0;
}

START_TEST(dijkstraPathMultiHop) {
    struct graph_t *g = newDirectedLinked();
    struct graphops_t *ops = getOperations(g);
    graphAlgWeightedPathResult out;
    double cap = 1.0;
    size_t a = 1, b = 2, c = 3;

    ck_assert_ptr_nonnull(g);
    ck_assert_ptr_nonnull(ops);
    ck_assert(ops->addNode(&a, g) == GRAPH_OK);
    ck_assert(ops->addNode(&b, g) == GRAPH_OK);
    ck_assert(ops->addNode(&c, g) == GRAPH_OK);
    ck_assert(ops->addEdge(&a, &b, &cap, g) == GRAPH_OK);
    ck_assert(ops->addEdge(&b, &c, &cap, g) == GRAPH_OK);

    memset(&out, 0, sizeof(out));
    ck_assert(graphAlgDijkstraPath(ops, g, a, c, NULL, NULL, &out) == GRAPH_OK);
    assertValidWeightedPath(ops, g, a, c, &out, 2);
    ck_assert_uint_eq(out.nodes[1], b);
    ck_assert_double_eq_tol(out.dist[1], 1.0, 1e-12);
    ck_assert_double_eq_tol(out.dist[2], 2.0, 1e-12);

    graphAlgWeightedPathResultDestroy(&out);
    teardownGraph(&ops, &g);
}
END_TEST

START_TEST(dijkstraPathSourceEqualsTarget) {
    struct graph_t *g = newDirectedLinked();
    struct graphops_t *ops = getOperations(g);
    graphAlgWeightedPathResult out;
    size_t a = 42;

    ck_assert_ptr_nonnull(g);
    ck_assert_ptr_nonnull(ops);
    ck_assert(ops->addNode(&a, g) == GRAPH_OK);

    memset(&out, 0, sizeof(out));
    ck_assert(graphAlgDijkstraPath(ops, g, a, a, NULL, NULL, &out) == GRAPH_OK);
    assertValidWeightedPath(ops, g, a, a, &out, 0);

    graphAlgWeightedPathResultDestroy(&out);
    teardownGraph(&ops, &g);
}
END_TEST

START_TEST(dijkstraPathPrefersCheaperLonger) {
    /* Direct a→c costs 10; a→b→c costs 1+1=2. Dijkstra must take the long path. */
    struct graph_t *g = newDirectedLinked();
    struct graphops_t *ops = getOperations(g);
    graphAlgWeightedPathResult out;
    double cheap = 1.0, expensive = 10.0;
    size_t a = 1, b = 2, c = 3;

    ck_assert_ptr_nonnull(g);
    ck_assert_ptr_nonnull(ops);
    ck_assert(ops->addNode(&a, g) == GRAPH_OK);
    ck_assert(ops->addNode(&b, g) == GRAPH_OK);
    ck_assert(ops->addNode(&c, g) == GRAPH_OK);
    ck_assert(ops->addEdge(&a, &c, &expensive, g) == GRAPH_OK);
    ck_assert(ops->addEdge(&a, &b, &cheap, g) == GRAPH_OK);
    ck_assert(ops->addEdge(&b, &c, &cheap, g) == GRAPH_OK);

    memset(&out, 0, sizeof(out));
    ck_assert(graphAlgDijkstraPath(ops, g, a, c, NULL, NULL, &out) == GRAPH_OK);
    assertValidWeightedPath(ops, g, a, c, &out, 2);
    ck_assert_uint_eq(out.nodes[1], b);
    ck_assert_double_eq_tol(out.dist[2], 2.0, 1e-12);

    graphAlgWeightedPathResultDestroy(&out);
    teardownGraph(&ops, &g);
}
END_TEST

START_TEST(dijkstraPathZeroWeight) {
    struct graph_t *g = newDirectedLinked();
    struct graphops_t *ops = getOperations(g);
    graphAlgWeightedPathResult out;
    double zero = 0.0, one = 1.0;
    size_t a = 1, b = 2, c = 3;

    ck_assert_ptr_nonnull(g);
    ck_assert_ptr_nonnull(ops);
    ck_assert(ops->addNode(&a, g) == GRAPH_OK);
    ck_assert(ops->addNode(&b, g) == GRAPH_OK);
    ck_assert(ops->addNode(&c, g) == GRAPH_OK);
    ck_assert(ops->addEdge(&a, &b, &zero, g) == GRAPH_OK);
    ck_assert(ops->addEdge(&b, &c, &one, g) == GRAPH_OK);

    memset(&out, 0, sizeof(out));
    ck_assert(graphAlgDijkstraPath(ops, g, a, c, NULL, NULL, &out) == GRAPH_OK);
    assertValidWeightedPath(ops, g, a, c, &out, 2);
    ck_assert_double_eq_tol(out.dist[1], 0.0, 1e-12);
    ck_assert_double_eq_tol(out.dist[2], 1.0, 1e-12);

    graphAlgWeightedPathResultDestroy(&out);
    teardownGraph(&ops, &g);
}
END_TEST

START_TEST(dijkstraPathWeightFn) {
    struct graph_t *g = newDirectedLinked();
    struct graphops_t *ops = getOperations(g);
    graphAlgWeightedPathResult out;
    double cap = 1.0;
    size_t a = 1, b = 2;

    ck_assert_ptr_nonnull(g);
    ck_assert_ptr_nonnull(ops);
    ck_assert(ops->addNode(&a, g) == GRAPH_OK);
    ck_assert(ops->addNode(&b, g) == GRAPH_OK);
    ck_assert(ops->addEdge(&a, &b, &cap, g) == GRAPH_OK);

    memset(&out, 0, sizeof(out));
    ck_assert(graphAlgDijkstraPath(ops, g, a, b, weightDoubleCap, NULL, &out)
              == GRAPH_OK);
    assertValidWeightedPath(ops, g, a, b, &out, 1);
    ck_assert_double_eq_tol(out.dist[1], 2.0, 1e-12);

    graphAlgWeightedPathResultDestroy(&out);
    teardownGraph(&ops, &g);
}
END_TEST

START_TEST(dijkstraPathInfSkip) {
    /* a→c blocked (+inf); a→b→c open. */
    struct graph_t *g = newDirectedLinked();
    struct graphops_t *ops = getOperations(g);
    graphAlgWeightedPathResult out;
    double cap = 1.0;
    size_t a = 1, b = 2, c = 3;
    blocked_arc_t block;

    ck_assert_ptr_nonnull(g);
    ck_assert_ptr_nonnull(ops);
    ck_assert(ops->addNode(&a, g) == GRAPH_OK);
    ck_assert(ops->addNode(&b, g) == GRAPH_OK);
    ck_assert(ops->addNode(&c, g) == GRAPH_OK);
    ck_assert(ops->addEdge(&a, &c, &cap, g) == GRAPH_OK);
    ck_assert(ops->addEdge(&a, &b, &cap, g) == GRAPH_OK);
    ck_assert(ops->addEdge(&b, &c, &cap, g) == GRAPH_OK);

    block.u = a;
    block.v = c;
    memset(&out, 0, sizeof(out));
    ck_assert(graphAlgDijkstraPath(ops, g, a, c, weightInfArc, &block, &out)
              == GRAPH_OK);
    assertValidWeightedPath(ops, g, a, c, &out, 2);
    ck_assert_uint_eq(out.nodes[1], b);

    graphAlgWeightedPathResultDestroy(&out);
    teardownGraph(&ops, &g);
}
END_TEST

START_TEST(dijkstraPathNegativeWeight) {
    struct graph_t *g = newDirectedLinked();
    struct graphops_t *ops = getOperations(g);
    graphAlgWeightedPathResult out;
    double cap = 1.0;
    size_t a = 1, b = 2;

    ck_assert_ptr_nonnull(g);
    ck_assert_ptr_nonnull(ops);
    ck_assert(ops->addNode(&a, g) == GRAPH_OK);
    ck_assert(ops->addNode(&b, g) == GRAPH_OK);
    ck_assert(ops->addEdge(&a, &b, &cap, g) == GRAPH_OK);

    out.n = 3;
    out.nodes = (size_t *)0x1;
    out.dist = (double *)0x1;
    out.parent = (size_t *)0x1;
    ck_assert(graphAlgDijkstraPath(ops, g, a, b, weightNegative, NULL, &out)
              == GRAPH_ERR);
    assertOutClean(&out);

    teardownGraph(&ops, &g);
}
END_TEST

START_TEST(dijkstraPathUnreachable) {
    struct graph_t *g = newDirectedLinked();
    struct graphops_t *ops = getOperations(g);
    graphAlgWeightedPathResult out;
    double cap = 1.0;
    size_t a = 1, b = 2, c = 3;

    ck_assert_ptr_nonnull(g);
    ck_assert_ptr_nonnull(ops);
    ck_assert(ops->addNode(&a, g) == GRAPH_OK);
    ck_assert(ops->addNode(&b, g) == GRAPH_OK);
    ck_assert(ops->addNode(&c, g) == GRAPH_OK);
    ck_assert(ops->addEdge(&a, &b, &cap, g) == GRAPH_OK);

    out.n = 99;
    out.nodes = (size_t *)0x1;
    out.dist = (double *)0x1;
    out.parent = (size_t *)0x1;
    ck_assert(graphAlgDijkstraPath(ops, g, a, c, NULL, NULL, &out) == GRAPH_ERR);
    assertOutClean(&out);

    teardownGraph(&ops, &g);
}
END_TEST

START_TEST(dijkstraPathBadArgs) {
    struct graph_t *g = newDirectedLinked();
    struct graphops_t *ops = getOperations(g);
    graphAlgWeightedPathResult out;
    size_t a = 1;
    funcGetEdges saved_edges;
    funcListNodes saved_list;

    ck_assert_ptr_nonnull(g);
    ck_assert_ptr_nonnull(ops);
    ck_assert(ops->addNode(&a, g) == GRAPH_OK);
    memset(&out, 0, sizeof(out));

    ck_assert(graphAlgDijkstraPath(NULL, g, a, a, NULL, NULL, &out) == GRAPH_ERR);
    ck_assert(graphAlgDijkstraPath(ops, NULL, a, a, NULL, NULL, &out) == GRAPH_ERR);
    ck_assert(graphAlgDijkstraPath(ops, g, a, a, NULL, NULL, NULL) == GRAPH_ERR);

    saved_edges = ops->getEdges;
    ops->getEdges = NULL;
    ck_assert(graphAlgDijkstraPath(ops, g, a, a, NULL, NULL, &out) == GRAPH_ERR);
    ops->getEdges = saved_edges;

    saved_list = ops->listNodes;
    ops->listNodes = NULL;
    ck_assert(graphAlgDijkstraPath(ops, g, a, a, NULL, NULL, &out) == GRAPH_ERR);
    ops->listNodes = saved_list;

    teardownGraph(&ops, &g);
}
END_TEST

START_TEST(weightedPathResultDestroyFreesOwnedBuffers) {
    graphAlgWeightedPathResult r;
    memset(&r, 0, sizeof(r));
    r.n = 2;
    r.nodes = calloc(2, sizeof(size_t));
    r.dist = calloc(2, sizeof(double));
    r.parent = calloc(2, sizeof(size_t));
    ck_assert_ptr_nonnull(r.nodes);
    ck_assert_ptr_nonnull(r.dist);
    ck_assert_ptr_nonnull(r.parent);

    graphAlgWeightedPathResultDestroy(NULL);
    graphAlgWeightedPathResultDestroy(&r);
    assertOutClean(&r);
}
END_TEST

Suite *dijkstrapath_suite(void) {
    Suite *s = suite_create("graphAlg-dijkstrapath");
    TCase *tc = tcase_create("path");
    tcase_add_test(tc, dijkstraPathMultiHop);
    tcase_add_test(tc, dijkstraPathSourceEqualsTarget);
    tcase_add_test(tc, dijkstraPathPrefersCheaperLonger);
    tcase_add_test(tc, dijkstraPathZeroWeight);
    tcase_add_test(tc, dijkstraPathWeightFn);
    tcase_add_test(tc, dijkstraPathInfSkip);
    tcase_add_test(tc, dijkstraPathNegativeWeight);
    tcase_add_test(tc, dijkstraPathUnreachable);
    tcase_add_test(tc, dijkstraPathBadArgs);
    tcase_add_test(tc, weightedPathResultDestroyFreesOwnedBuffers);
    suite_add_tcase(s, tc);
    return s;
}

int main(void) {
    Suite *s = dijkstrapath_suite();
    SRunner *sr = srunner_create(s);
    srunner_set_fork_status(sr, CK_NOFORK);
    srunner_run_all(sr, CK_NORMAL);
    int failed = srunner_ntests_failed(sr);
    srunner_free(sr);
    return (failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
