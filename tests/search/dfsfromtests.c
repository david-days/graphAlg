/**
 * Tests for graphAlgDfsFrom.
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

static void assertOutClean(const graphAlgPathResult *out) {
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

static int resultContains(const graphAlgPathResult *out, size_t id) {
    for (size_t i = 0; i < out->n; i++) {
        if (out->nodes[i] == id) {
            return 1;
        }
    }
    return 0;
}

/** Contract checks for a successful single-source DFS tree. */
static void assertValidTree(struct graphops_t *ops, struct graph_t *g,
                            size_t source,
                            const graphAlgPathResult *out,
                            size_t expect_n) {
    ck_assert_ptr_nonnull(out);
    ck_assert_uint_eq(out->n, expect_n);
    ck_assert_ptr_nonnull(out->nodes);
    ck_assert_ptr_nonnull(out->dist);
    ck_assert_ptr_nonnull(out->parent);

    ck_assert_uint_eq(out->nodes[0], source);
    ck_assert_uint_eq(out->parent[0], source);
    ck_assert_uint_eq(out->dist[0], 0);

    for (size_t i = 0; i < out->n; i++) {
        /* no duplicate ids */
        for (size_t j = i + 1; j < out->n; j++) {
            ck_assert_uint_ne(out->nodes[i], out->nodes[j]);
        }
    }

    for (size_t i = 1; i < out->n; i++) {
        size_t p = out->parent[i];
        size_t ppos = out->n; /* sentinel */
        for (size_t j = 0; j < i; j++) {
            if (out->nodes[j] == p) {
                ppos = j;
                break;
            }
        }
        ck_assert_uint_lt(ppos, i); /* parent discovered earlier */
        ck_assert_uint_eq(out->dist[i], out->dist[ppos] + 1);
        ck_assert(neighborContains(ops, g, p, out->nodes[i]));
    }
}

static struct graph_t *newDirectedLinked(void) {
    return initGraph(LINKED | DIRECTED | GENERIC, 0, NULL);
}

START_TEST(dfsFromSingleton) {
    struct graph_t *g = newDirectedLinked();
    struct graphops_t *ops = getOperations(g);
    graphAlgPathResult out;
    size_t a = 42;

    ck_assert_ptr_nonnull(g);
    ck_assert_ptr_nonnull(ops);
    ck_assert(ops->addNode(&a, g) == GRAPH_OK);

    memset(&out, 0, sizeof(out));
    ck_assert(graphAlgDfsFrom(ops, g, a, &out) == GRAPH_OK);
    assertValidTree(ops, g, a, &out, 1);

    graphAlgPathResultDestroy(&out);
    teardownGraph(&ops, &g);
}
END_TEST

START_TEST(dfsFromLine) {
    struct graph_t *g = newDirectedLinked();
    struct graphops_t *ops = getOperations(g);
    graphAlgPathResult out;
    double cap = 1.0;
    size_t a = 1, b = 2, c = 3, d = 4;

    ck_assert_ptr_nonnull(g);
    ck_assert_ptr_nonnull(ops);
    ck_assert(ops->addNode(&a, g) == GRAPH_OK);
    ck_assert(ops->addNode(&b, g) == GRAPH_OK);
    ck_assert(ops->addNode(&c, g) == GRAPH_OK);
    ck_assert(ops->addNode(&d, g) == GRAPH_OK);
    ck_assert(ops->addEdge(&a, &b, &cap, g) == GRAPH_OK);
    ck_assert(ops->addEdge(&b, &c, &cap, g) == GRAPH_OK);
    ck_assert(ops->addEdge(&c, &d, &cap, g) == GRAPH_OK);

    memset(&out, 0, sizeof(out));
    ck_assert(graphAlgDfsFrom(ops, g, a, &out) == GRAPH_OK);
    assertValidTree(ops, g, a, &out, 4);
    ck_assert_uint_eq(out.nodes[1], b);
    ck_assert_uint_eq(out.nodes[2], c);
    ck_assert_uint_eq(out.nodes[3], d);
    ck_assert_uint_eq(out.dist[3], 3);

    graphAlgPathResultDestroy(&out);
    teardownGraph(&ops, &g);
}
END_TEST

START_TEST(dfsFromExcludesOtherComponent) {
    struct graph_t *g = newDirectedLinked();
    struct graphops_t *ops = getOperations(g);
    graphAlgPathResult out;
    double cap = 1.0;
    size_t a = 1, b = 2, c = 3;

    ck_assert_ptr_nonnull(g);
    ck_assert_ptr_nonnull(ops);
    ck_assert(ops->addNode(&a, g) == GRAPH_OK);
    ck_assert(ops->addNode(&b, g) == GRAPH_OK);
    ck_assert(ops->addNode(&c, g) == GRAPH_OK);
    ck_assert(ops->addEdge(&a, &b, &cap, g) == GRAPH_OK);
    /* c isolated */

    memset(&out, 0, sizeof(out));
    ck_assert(graphAlgDfsFrom(ops, g, a, &out) == GRAPH_OK);
    assertValidTree(ops, g, a, &out, 2);
    ck_assert(resultContains(&out, a));
    ck_assert(resultContains(&out, b));
    ck_assert(!resultContains(&out, c));

    graphAlgPathResultDestroy(&out);
    teardownGraph(&ops, &g);
}
END_TEST

START_TEST(dfsFromSourceAbsent) {
    struct graph_t *g = newDirectedLinked();
    struct graphops_t *ops = getOperations(g);
    graphAlgPathResult out;
    size_t a = 1, missing = 99;

    ck_assert_ptr_nonnull(g);
    ck_assert_ptr_nonnull(ops);
    ck_assert(ops->addNode(&a, g) == GRAPH_OK);

    out.n = 7;
    out.nodes = (size_t *)0x1;
    out.dist = (size_t *)0x1;
    out.parent = (size_t *)0x1;
    ck_assert(graphAlgDfsFrom(ops, g, missing, &out) == GRAPH_ERR);
    assertOutClean(&out);

    teardownGraph(&ops, &g);
}
END_TEST

START_TEST(dfsFromBadArgs) {
    struct graph_t *g = newDirectedLinked();
    struct graphops_t *ops = getOperations(g);
    graphAlgPathResult out;
    size_t a = 1;
    funcGetNeighbors saved_nbr;
    funcListNodes saved_list;

    ck_assert_ptr_nonnull(g);
    ck_assert_ptr_nonnull(ops);
    ck_assert(ops->addNode(&a, g) == GRAPH_OK);
    memset(&out, 0, sizeof(out));

    ck_assert(graphAlgDfsFrom(NULL, g, a, &out) == GRAPH_ERR);
    ck_assert(graphAlgDfsFrom(ops, NULL, a, &out) == GRAPH_ERR);
    ck_assert(graphAlgDfsFrom(ops, g, a, NULL) == GRAPH_ERR);

    saved_nbr = ops->getNeighbors;
    ops->getNeighbors = NULL;
    ck_assert(graphAlgDfsFrom(ops, g, a, &out) == GRAPH_ERR);
    ops->getNeighbors = saved_nbr;

    saved_list = ops->listNodes;
    ops->listNodes = NULL;
    ck_assert(graphAlgDfsFrom(ops, g, a, &out) == GRAPH_ERR);
    ops->listNodes = saved_list;

    teardownGraph(&ops, &g);
}
END_TEST

START_TEST(dfsFromSparseIds) {
    struct graph_t *g = newDirectedLinked();
    struct graphops_t *ops = getOperations(g);
    graphAlgPathResult out;
    double cap = 1.0;
    size_t a = 10, b = 1000, c = 7;

    ck_assert_ptr_nonnull(g);
    ck_assert_ptr_nonnull(ops);
    ck_assert(ops->addNode(&a, g) == GRAPH_OK);
    ck_assert(ops->addNode(&b, g) == GRAPH_OK);
    ck_assert(ops->addNode(&c, g) == GRAPH_OK);
    ck_assert(ops->addEdge(&a, &b, &cap, g) == GRAPH_OK);
    ck_assert(ops->addEdge(&b, &c, &cap, g) == GRAPH_OK);

    memset(&out, 0, sizeof(out));
    ck_assert(graphAlgDfsFrom(ops, g, a, &out) == GRAPH_OK);
    assertValidTree(ops, g, a, &out, 3);
    ck_assert_uint_eq(out.nodes[1], b);
    ck_assert_uint_eq(out.nodes[2], c);

    graphAlgPathResultDestroy(&out);
    teardownGraph(&ops, &g);
}
END_TEST

START_TEST(dfsFromDirectedOneWay) {
    struct graph_t *g = newDirectedLinked();
    struct graphops_t *ops = getOperations(g);
    graphAlgPathResult out;
    double cap = 1.0;
    size_t a = 1, b = 2;

    ck_assert_ptr_nonnull(g);
    ck_assert_ptr_nonnull(ops);
    ck_assert(ops->addNode(&a, g) == GRAPH_OK);
    ck_assert(ops->addNode(&b, g) == GRAPH_OK);
    ck_assert(ops->addEdge(&a, &b, &cap, g) == GRAPH_OK);

    memset(&out, 0, sizeof(out));
    ck_assert(graphAlgDfsFrom(ops, g, b, &out) == GRAPH_OK);
    assertValidTree(ops, g, b, &out, 1);
    ck_assert(!resultContains(&out, a));

    graphAlgPathResultDestroy(&out);
    teardownGraph(&ops, &g);
}
END_TEST

START_TEST(dfsFromDiamond) {
    struct graph_t *g = newDirectedLinked();
    struct graphops_t *ops = getOperations(g);
    graphAlgPathResult out;
    double cap = 1.0;
    size_t s = 1, mid_a = 2, mid_b = 3, t = 4;

    ck_assert_ptr_nonnull(g);
    ck_assert_ptr_nonnull(ops);
    ck_assert(ops->addNode(&s, g) == GRAPH_OK);
    ck_assert(ops->addNode(&mid_a, g) == GRAPH_OK);
    ck_assert(ops->addNode(&mid_b, g) == GRAPH_OK);
    ck_assert(ops->addNode(&t, g) == GRAPH_OK);
    ck_assert(ops->addEdge(&s, &mid_a, &cap, g) == GRAPH_OK);
    ck_assert(ops->addEdge(&s, &mid_b, &cap, g) == GRAPH_OK);
    ck_assert(ops->addEdge(&mid_a, &t, &cap, g) == GRAPH_OK);
    ck_assert(ops->addEdge(&mid_b, &t, &cap, g) == GRAPH_OK);

    memset(&out, 0, sizeof(out));
    ck_assert(graphAlgDfsFrom(ops, g, s, &out) == GRAPH_OK);
    assertValidTree(ops, g, s, &out, 4);
    ck_assert(resultContains(&out, mid_a));
    ck_assert(resultContains(&out, mid_b));
    ck_assert(resultContains(&out, t));
    /* t at hop distance 2 regardless of which mid parent BFS chose */
    for (size_t i = 0; i < out.n; i++) {
        if (out.nodes[i] == t) {
            ck_assert_uint_eq(out.dist[i], 2);
        }
    }

    graphAlgPathResultDestroy(&out);
    teardownGraph(&ops, &g);
}
END_TEST

/**
 * Fixture where BFS shortest-hop dist(t) differs from DFS tree-depth(t).
 *
 *     s ──► c ──► t     (short: 2 hops)
 *     │
 *     └────► a ──► b ──► t   (long: 3 hops)
 *
 * Edge order from s is intentional: add s→c before s→a so getNeighbors(s)
 * lists c then a.  Iterative DFS (mark-on-push, LIFO) pushes c then a and
 * therefore expands a first, discovering t via a→b→t at depth 3.  BFS still
 * reaches t via c at hop distance 2.
 *
 * Also: compare dist by looking up t in each result separately — discovery
 * order differs, so the same array index is not the same node.
 */
START_TEST(dfsResultDifferent) {
    struct graph_t *g = newDirectedLinked();
    struct graphops_t *ops = getOperations(g);
    graphAlgPathResult outDfs, outBfs;
    double cap = 1.0;
    size_t s = 1, a = 2, b = 3, c = 4, t = 5;
    size_t dfs_dist_t = SIZE_MAX, bfs_dist_t = SIZE_MAX;

    ck_assert_ptr_nonnull(g);
    ck_assert_ptr_nonnull(ops);
    ck_assert(ops->addNode(&s, g) == GRAPH_OK);
    ck_assert(ops->addNode(&a, g) == GRAPH_OK);
    ck_assert(ops->addNode(&b, g) == GRAPH_OK);
    ck_assert(ops->addNode(&c, g) == GRAPH_OK);
    ck_assert(ops->addNode(&t, g) == GRAPH_OK);
    /* short branch first in adjacency, long branch second → DFS expands long first */
    ck_assert(ops->addEdge(&s, &c, &cap, g) == GRAPH_OK);
    ck_assert(ops->addEdge(&s, &a, &cap, g) == GRAPH_OK);
    ck_assert(ops->addEdge(&a, &b, &cap, g) == GRAPH_OK);
    ck_assert(ops->addEdge(&b, &t, &cap, g) == GRAPH_OK);
    ck_assert(ops->addEdge(&c, &t, &cap, g) == GRAPH_OK);

    memset(&outDfs, 0, sizeof(outDfs));
    memset(&outBfs, 0, sizeof(outBfs));
    ck_assert(graphAlgDfsFrom(ops, g, s, &outDfs) == GRAPH_OK);
    ck_assert(graphAlgBfsFrom(ops, g, s, &outBfs) == GRAPH_OK);
    assertValidTree(ops, g, s, &outDfs, 5);
    assertValidTree(ops, g, s, &outBfs, 5);

    for (size_t i = 0; i < outDfs.n; i++) {
        if (outDfs.nodes[i] == t) {
            dfs_dist_t = outDfs.dist[i];
        }
    }
    for (size_t i = 0; i < outBfs.n; i++) {
        if (outBfs.nodes[i] == t) {
            bfs_dist_t = outBfs.dist[i];
        }
    }
    ck_assert_uint_eq(bfs_dist_t, 2); /* shortest hops via c */
    ck_assert_uint_eq(dfs_dist_t, 3); /* tree depth via a→b */
    ck_assert_uint_ne(dfs_dist_t, bfs_dist_t);

    graphAlgPathResultDestroy(&outDfs);
    graphAlgPathResultDestroy(&outBfs);
    teardownGraph(&ops, &g);
}
END_TEST

Suite *dfsfrom_suite(void) {
    Suite *s = suite_create("graphAlg-dfsfrom");
    TCase *tc = tcase_create("from");
    tcase_add_test(tc, dfsFromSingleton);
    tcase_add_test(tc, dfsFromLine);
    tcase_add_test(tc, dfsFromExcludesOtherComponent);
    tcase_add_test(tc, dfsFromSourceAbsent);
    tcase_add_test(tc, dfsFromBadArgs);
    tcase_add_test(tc, dfsFromSparseIds);
    tcase_add_test(tc, dfsFromDirectedOneWay);
    tcase_add_test(tc, dfsFromDiamond);
    tcase_add_test(tc, dfsResultDifferent);
    suite_add_tcase(s, tc);
    return s;
}

int main(void) {
    Suite *s = dfsfrom_suite();
    SRunner *sr = srunner_create(s);
    srunner_set_fork_status(sr, CK_NOFORK);
    srunner_run_all(sr, CK_NORMAL);
    int failed = srunner_ntests_failed(sr);
    srunner_free(sr);
    return (failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
