/**
 * Tests for graphAlgConnectedComponents / graphAlgComponentsResultDestroy.
 *
 * The API is defined over getNeighbors as stored and expects undirected
 * semantics via both directions already present. LINKED|UNDIRECTED stores
 * edges only on min(u,v), so these fixtures use DIRECTED with explicit
 * reverse edges where symmetry is required.
 */

#include <check.h>
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

static void assertOutClean(const graphAlgComponentsResult *out) {
    ck_assert_ptr_nonnull(out);
    ck_assert_uint_eq(out->n, 0);
    ck_assert_ptr_null(out->nodes);
    ck_assert_ptr_null(out->sizes);
}

/** Index of the component containing @p id, or SIZE_MAX if absent. */
static size_t componentIndexOf(const graphAlgComponentsResult *out, size_t id) {
    for (size_t c = 0; c < out->n; c++) {
        for (size_t i = 0; i < out->sizes[c]; i++) {
            if (out->nodes[c][i] == id) {
                return c;
            }
        }
    }
    return SIZE_MAX;
}

static int componentContains(const graphAlgComponentsResult *out, size_t c, size_t id) {
    if (c >= out->n) {
        return 0;
    }
    for (size_t i = 0; i < out->sizes[c]; i++) {
        if (out->nodes[c][i] == id) {
            return 1;
        }
    }
    return 0;
}

/**
 * Partition contract: every expected id appears exactly once; total size
 * matches; no empty components; no duplicate ids within a component.
 */
static void assertValidPartition(const graphAlgComponentsResult *out,
                                 const size_t *expect_ids, size_t expect_count) {
    ck_assert_ptr_nonnull(out);
    if (expect_count == 0) {
        assertOutClean(out);
        return;
    }
    ck_assert_ptr_nonnull(out->nodes);
    ck_assert_ptr_nonnull(out->sizes);
    ck_assert_uint_gt(out->n, 0);

    size_t total = 0;
    for (size_t c = 0; c < out->n; c++) {
        ck_assert_uint_gt(out->sizes[c], 0);
        ck_assert_ptr_nonnull(out->nodes[c]);
        total += out->sizes[c];
        for (size_t i = 0; i < out->sizes[c]; i++) {
            for (size_t j = i + 1; j < out->sizes[c]; j++) {
                ck_assert_uint_ne(out->nodes[c][i], out->nodes[c][j]);
            }
        }
    }
    ck_assert_uint_eq(total, expect_count);

    for (size_t i = 0; i < expect_count; i++) {
        size_t c = componentIndexOf(out, expect_ids[i]);
        ck_assert_uint_ne(c, SIZE_MAX);
        size_t hits = 0;
        for (size_t cc = 0; cc < out->n; cc++) {
            if (componentContains(out, cc, expect_ids[i])) {
                hits++;
            }
        }
        ck_assert_uint_eq(hits, 1);
    }
}

static struct graph_t *newDirectedLinked(void) {
    return initGraph(LINKED | DIRECTED | GENERIC, 0, NULL);
}

/** Add u→v and v→u so getNeighbors is symmetric (undirected-as-stored). */
static void addUndirectedEdge(struct graphops_t *ops, struct graph_t *g,
                              size_t *u, size_t *v, double *cap) {
    ck_assert(ops->addEdge(u, v, cap, g) == GRAPH_OK);
    ck_assert(ops->addEdge(v, u, cap, g) == GRAPH_OK);
}

START_TEST(ccEmptyGraph) {
    struct graph_t *g = newDirectedLinked();
    struct graphops_t *ops = getOperations(g);
    graphAlgComponentsResult out;

    ck_assert_ptr_nonnull(g);
    ck_assert_ptr_nonnull(ops);

    out.n = 7;
    out.nodes = (size_t **)0x1;
    out.sizes = (size_t *)0x1;
    ck_assert(graphAlgConnectedComponents(ops, g, &out) == GRAPH_OK);
    assertOutClean(&out);

    teardownGraph(&ops, &g);
}
END_TEST

START_TEST(ccSingleton) {
    struct graph_t *g = newDirectedLinked();
    struct graphops_t *ops = getOperations(g);
    graphAlgComponentsResult out;
    size_t a = 42;
    size_t expect[] = {a};

    ck_assert_ptr_nonnull(g);
    ck_assert_ptr_nonnull(ops);
    ck_assert(ops->addNode(&a, g) == GRAPH_OK);

    memset(&out, 0, sizeof(out));
    ck_assert(graphAlgConnectedComponents(ops, g, &out) == GRAPH_OK);
    ck_assert_uint_eq(out.n, 1);
    ck_assert_uint_eq(out.sizes[0], 1);
    ck_assert_uint_eq(out.nodes[0][0], a);
    assertValidPartition(&out, expect, 1);

    graphAlgComponentsResultDestroy(&out);
    teardownGraph(&ops, &g);
}
END_TEST

START_TEST(ccSingleComponentLine) {
    struct graph_t *g = newDirectedLinked();
    struct graphops_t *ops = getOperations(g);
    graphAlgComponentsResult out;
    double cap = 1.0;
    size_t a = 1, b = 2, c = 3, d = 4;
    size_t expect[] = {a, b, c, d};

    ck_assert_ptr_nonnull(g);
    ck_assert_ptr_nonnull(ops);
    ck_assert(ops->addNode(&a, g) == GRAPH_OK);
    ck_assert(ops->addNode(&b, g) == GRAPH_OK);
    ck_assert(ops->addNode(&c, g) == GRAPH_OK);
    ck_assert(ops->addNode(&d, g) == GRAPH_OK);
    addUndirectedEdge(ops, g, &a, &b, &cap);
    addUndirectedEdge(ops, g, &b, &c, &cap);
    addUndirectedEdge(ops, g, &c, &d, &cap);

    memset(&out, 0, sizeof(out));
    ck_assert(graphAlgConnectedComponents(ops, g, &out) == GRAPH_OK);
    ck_assert_uint_eq(out.n, 1);
    ck_assert_uint_eq(out.sizes[0], 4);
    assertValidPartition(&out, expect, 4);

    graphAlgComponentsResultDestroy(&out);
    teardownGraph(&ops, &g);
}
END_TEST

START_TEST(ccTwoComponents) {
    struct graph_t *g = newDirectedLinked();
    struct graphops_t *ops = getOperations(g);
    graphAlgComponentsResult out;
    double cap = 1.0;
    size_t a = 1, b = 2, c = 3, d = 4;
    size_t expect[] = {a, b, c, d};
    size_t ca, cc;

    ck_assert_ptr_nonnull(g);
    ck_assert_ptr_nonnull(ops);
    ck_assert(ops->addNode(&a, g) == GRAPH_OK);
    ck_assert(ops->addNode(&b, g) == GRAPH_OK);
    ck_assert(ops->addNode(&c, g) == GRAPH_OK);
    ck_assert(ops->addNode(&d, g) == GRAPH_OK);
    addUndirectedEdge(ops, g, &a, &b, &cap);
    addUndirectedEdge(ops, g, &c, &d, &cap);

    memset(&out, 0, sizeof(out));
    ck_assert(graphAlgConnectedComponents(ops, g, &out) == GRAPH_OK);
    ck_assert_uint_eq(out.n, 2);
    assertValidPartition(&out, expect, 4);

    ca = componentIndexOf(&out, a);
    cc = componentIndexOf(&out, c);
    ck_assert_uint_ne(ca, SIZE_MAX);
    ck_assert_uint_ne(cc, SIZE_MAX);
    ck_assert_uint_ne(ca, cc);
    ck_assert(componentContains(&out, ca, b));
    ck_assert(componentContains(&out, cc, d));
    ck_assert(!componentContains(&out, ca, c));
    ck_assert(!componentContains(&out, cc, a));

    graphAlgComponentsResultDestroy(&out);
    teardownGraph(&ops, &g);
}
END_TEST

START_TEST(ccThreeIsolates) {
    struct graph_t *g = newDirectedLinked();
    struct graphops_t *ops = getOperations(g);
    graphAlgComponentsResult out;
    size_t a = 10, b = 20, c = 30;
    size_t expect[] = {a, b, c};

    ck_assert_ptr_nonnull(g);
    ck_assert_ptr_nonnull(ops);
    ck_assert(ops->addNode(&a, g) == GRAPH_OK);
    ck_assert(ops->addNode(&b, g) == GRAPH_OK);
    ck_assert(ops->addNode(&c, g) == GRAPH_OK);

    memset(&out, 0, sizeof(out));
    ck_assert(graphAlgConnectedComponents(ops, g, &out) == GRAPH_OK);
    ck_assert_uint_eq(out.n, 3);
    assertValidPartition(&out, expect, 3);
    ck_assert_uint_eq(out.sizes[0], 1);
    ck_assert_uint_eq(out.sizes[1], 1);
    ck_assert_uint_eq(out.sizes[2], 1);

    graphAlgComponentsResultDestroy(&out);
    teardownGraph(&ops, &g);
}
END_TEST

START_TEST(ccRootOrderFollowsListNodes) {
    struct graph_t *g = newDirectedLinked();
    struct graphops_t *ops = getOperations(g);
    graphAlgComponentsResult out;
    double cap = 1.0;
    size_t a = 1, b = 2, c = 3;
    struct node_t *nlist;
    size_t first_id;

    ck_assert_ptr_nonnull(g);
    ck_assert_ptr_nonnull(ops);
    ck_assert(ops->addNode(&a, g) == GRAPH_OK);
    ck_assert(ops->addNode(&b, g) == GRAPH_OK);
    ck_assert(ops->addNode(&c, g) == GRAPH_OK);
    /* only a—b connected; c isolated. First listNodes id starts component 0. */
    addUndirectedEdge(ops, g, &a, &b, &cap);

    nlist = ops->listNodes(g);
    ck_assert_ptr_nonnull(nlist);
    first_id = nlist->nodeid;
    destroyNodes((void **)&nlist);

    memset(&out, 0, sizeof(out));
    ck_assert(graphAlgConnectedComponents(ops, g, &out) == GRAPH_OK);
    ck_assert_uint_eq(out.n, 2);
    ck_assert(componentContains(&out, 0, first_id));

    graphAlgComponentsResultDestroy(&out);
    teardownGraph(&ops, &g);
}
END_TEST

START_TEST(ccSparseIds) {
    struct graph_t *g = newDirectedLinked();
    struct graphops_t *ops = getOperations(g);
    graphAlgComponentsResult out;
    double cap = 1.0;
    size_t a = 10, b = 1000, c = 7;
    size_t expect[] = {a, b, c};

    ck_assert_ptr_nonnull(g);
    ck_assert_ptr_nonnull(ops);
    ck_assert(ops->addNode(&a, g) == GRAPH_OK);
    ck_assert(ops->addNode(&b, g) == GRAPH_OK);
    ck_assert(ops->addNode(&c, g) == GRAPH_OK);
    addUndirectedEdge(ops, g, &a, &b, &cap);
    addUndirectedEdge(ops, g, &b, &c, &cap);

    memset(&out, 0, sizeof(out));
    ck_assert(graphAlgConnectedComponents(ops, g, &out) == GRAPH_OK);
    ck_assert_uint_eq(out.n, 1);
    assertValidPartition(&out, expect, 3);

    graphAlgComponentsResultDestroy(&out);
    teardownGraph(&ops, &g);
}
END_TEST

/**
 * Directed one-way: a→b only. From a, DFS reaches b, so with listNodes order
 * a then b there is a single component {a,b}. Documents "as stored" (not weak-CC).
 */
START_TEST(ccDirectedOneWayFromSource) {
    struct graph_t *g = newDirectedLinked();
    struct graphops_t *ops = getOperations(g);
    graphAlgComponentsResult out;
    double cap = 1.0;
    size_t a = 1, b = 2;
    size_t expect[] = {a, b};

    ck_assert_ptr_nonnull(g);
    ck_assert_ptr_nonnull(ops);
    ck_assert(ops->addNode(&a, g) == GRAPH_OK);
    ck_assert(ops->addNode(&b, g) == GRAPH_OK);
    ck_assert(ops->addEdge(&a, &b, &cap, g) == GRAPH_OK);

    memset(&out, 0, sizeof(out));
    ck_assert(graphAlgConnectedComponents(ops, g, &out) == GRAPH_OK);
    assertValidPartition(&out, expect, 2);
    ck_assert_uint_eq(out.n, 1);
    ck_assert_uint_eq(out.sizes[0], 2);

    graphAlgComponentsResultDestroy(&out);
    teardownGraph(&ops, &g);
}
END_TEST

START_TEST(ccBadArgs) {
    struct graph_t *g = newDirectedLinked();
    struct graphops_t *ops = getOperations(g);
    graphAlgComponentsResult out;
    size_t a = 1;
    funcGetNeighbors saved_nbr;
    funcListNodes saved_list;
    funcNodeCount saved_count;

    ck_assert_ptr_nonnull(g);
    ck_assert_ptr_nonnull(ops);
    ck_assert(ops->addNode(&a, g) == GRAPH_OK);
    memset(&out, 0, sizeof(out));

    ck_assert(graphAlgConnectedComponents(NULL, g, &out) == GRAPH_ERR);
    ck_assert(graphAlgConnectedComponents(ops, NULL, &out) == GRAPH_ERR);
    ck_assert(graphAlgConnectedComponents(ops, g, NULL) == GRAPH_ERR);

    saved_nbr = ops->getNeighbors;
    ops->getNeighbors = NULL;
    ck_assert(graphAlgConnectedComponents(ops, g, &out) == GRAPH_ERR);
    ops->getNeighbors = saved_nbr;

    saved_list = ops->listNodes;
    ops->listNodes = NULL;
    ck_assert(graphAlgConnectedComponents(ops, g, &out) == GRAPH_ERR);
    ops->listNodes = saved_list;

    saved_count = ops->nodeCount;
    ops->nodeCount = NULL;
    ck_assert(graphAlgConnectedComponents(ops, g, &out) == GRAPH_ERR);
    ops->nodeCount = saved_count;

    teardownGraph(&ops, &g);
}
END_TEST

START_TEST(ccDestroyNullAndClean) {
    graphAlgComponentsResult r;

    graphAlgComponentsResultDestroy(NULL);

    memset(&r, 0, sizeof(r));
    graphAlgComponentsResultDestroy(&r);
    assertOutClean(&r);
}
END_TEST

Suite *connectedcomponents_suite(void) {
    Suite *s = suite_create("graphAlg-connectedcomponents");
    TCase *tc = tcase_create("components");
    tcase_add_test(tc, ccEmptyGraph);
    tcase_add_test(tc, ccSingleton);
    tcase_add_test(tc, ccSingleComponentLine);
    tcase_add_test(tc, ccTwoComponents);
    tcase_add_test(tc, ccThreeIsolates);
    tcase_add_test(tc, ccRootOrderFollowsListNodes);
    tcase_add_test(tc, ccSparseIds);
    tcase_add_test(tc, ccDirectedOneWayFromSource);
    tcase_add_test(tc, ccBadArgs);
    tcase_add_test(tc, ccDestroyNullAndClean);
    suite_add_tcase(s, tc);
    return s;
}

int main(void) {
    Suite *s = connectedcomponents_suite();
    SRunner *sr = srunner_create(s);
    srunner_set_fork_status(sr, CK_NOFORK);
    srunner_run_all(sr, CK_NORMAL);
    int failed = srunner_ntests_failed(sr);
    srunner_free(sr);
    return (failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
