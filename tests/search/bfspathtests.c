/**
 * Tests for graphAlgBfsPath / graphAlgPathResultDestroy.
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

/** Contract checks for a successful path result. */
static void assertValidPath(struct graphops_t *ops, struct graph_t *g,
                            size_t source, size_t target,
                            const graphAlgPathResult *out,
                            size_t expect_hops) {
    ck_assert_ptr_nonnull(out);
    ck_assert_uint_eq(out->n, expect_hops + 1);
    ck_assert_ptr_nonnull(out->nodes);
    ck_assert_ptr_nonnull(out->dist);
    ck_assert_ptr_nonnull(out->parent);

    ck_assert_uint_eq(out->nodes[0], source);
    ck_assert_uint_eq(out->nodes[out->n - 1], target);
    ck_assert_uint_eq(out->parent[0], source);
    ck_assert_uint_eq(out->dist[0], 0);

    for (size_t i = 0; i < out->n; i++) {
        ck_assert_uint_eq(out->dist[i], i);
    }
    for (size_t i = 1; i < out->n; i++) {
        ck_assert_uint_eq(out->parent[i], out->nodes[i - 1]);
        ck_assert(neighborContains(ops, g, out->nodes[i - 1], out->nodes[i]));
    }
}

static struct graph_t *newDirectedLinked(void) {
    return initGraph(LINKED | DIRECTED | GENERIC, 0, NULL);
}

START_TEST(bfsPathMultiHop) {
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
    ck_assert(graphAlgBfsPath(ops, g, a, d, &out) == GRAPH_OK);
    assertValidPath(ops, g, a, d, &out, 3);
    ck_assert_uint_eq(out.nodes[1], b);
    ck_assert_uint_eq(out.nodes[2], c);

    graphAlgPathResultDestroy(&out);
    teardownGraph(&ops, &g);
}
END_TEST

START_TEST(bfsPathSourceEqualsTarget) {
    struct graph_t *g = newDirectedLinked();
    struct graphops_t *ops = getOperations(g);
    graphAlgPathResult out;
    size_t a = 42;

    ck_assert_ptr_nonnull(g);
    ck_assert_ptr_nonnull(ops);
    ck_assert(ops->addNode(&a, g) == GRAPH_OK);

    memset(&out, 0, sizeof(out));
    ck_assert(graphAlgBfsPath(ops, g, a, a, &out) == GRAPH_OK);
    assertValidPath(ops, g, a, a, &out, 0);

    graphAlgPathResultDestroy(&out);
    teardownGraph(&ops, &g);
}
END_TEST

START_TEST(bfsPathOneEdge) {
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
    ck_assert(graphAlgBfsPath(ops, g, a, b, &out) == GRAPH_OK);
    assertValidPath(ops, g, a, b, &out, 1);

    graphAlgPathResultDestroy(&out);
    teardownGraph(&ops, &g);
}
END_TEST

START_TEST(bfsPathUnreachable) {
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

    out.n = 99;
    out.nodes = (size_t *)0x1;
    out.dist = (size_t *)0x1;
    out.parent = (size_t *)0x1;
    ck_assert(graphAlgBfsPath(ops, g, a, c, &out) == GRAPH_ERR);
    assertOutClean(&out);

    teardownGraph(&ops, &g);
}
END_TEST

START_TEST(bfsPathSourceAbsent) {
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
    ck_assert(graphAlgBfsPath(ops, g, missing, a, &out) == GRAPH_ERR);
    assertOutClean(&out);

    teardownGraph(&ops, &g);
}
END_TEST

START_TEST(bfsPathBadArgs) {
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

    ck_assert(graphAlgBfsPath(NULL, g, a, a, &out) == GRAPH_ERR);
    ck_assert(graphAlgBfsPath(ops, NULL, a, a, &out) == GRAPH_ERR);
    ck_assert(graphAlgBfsPath(ops, g, a, a, NULL) == GRAPH_ERR);

    saved_nbr = ops->getNeighbors;
    ops->getNeighbors = NULL;
    ck_assert(graphAlgBfsPath(ops, g, a, a, &out) == GRAPH_ERR);
    ops->getNeighbors = saved_nbr;

    saved_list = ops->listNodes;
    ops->listNodes = NULL;
    ck_assert(graphAlgBfsPath(ops, g, a, a, &out) == GRAPH_ERR);
    ops->listNodes = saved_list;

    teardownGraph(&ops, &g);
}
END_TEST

START_TEST(bfsPathSparseIds) {
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
    ck_assert(graphAlgBfsPath(ops, g, a, c, &out) == GRAPH_OK);
    assertValidPath(ops, g, a, c, &out, 2);
    ck_assert_uint_eq(out.nodes[1], b);

    graphAlgPathResultDestroy(&out);
    teardownGraph(&ops, &g);
}
END_TEST

START_TEST(bfsPathDirectedOneWay) {
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

    out.n = 3;
    out.nodes = (size_t *)0x1;
    out.dist = (size_t *)0x1;
    out.parent = (size_t *)0x1;
    ck_assert(graphAlgBfsPath(ops, g, b, a, &out) == GRAPH_ERR);
    assertOutClean(&out);

    teardownGraph(&ops, &g);
}
END_TEST

START_TEST(bfsPathDiamond) {
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
    ck_assert(graphAlgBfsPath(ops, g, s, t, &out) == GRAPH_OK);
    /* Either S→A→T or S→B→T is fine; length must be shortest. */
    assertValidPath(ops, g, s, t, &out, 2);
    ck_assert(out.nodes[1] == mid_a || out.nodes[1] == mid_b);

    graphAlgPathResultDestroy(&out);
    teardownGraph(&ops, &g);
}
END_TEST

START_TEST(pathResultDestroyFreesOwnedBuffers) {
    graphAlgPathResult r;
    memset(&r, 0, sizeof(r));
    r.n = 2;
    r.nodes = calloc(2, sizeof(size_t));
    r.dist = calloc(2, sizeof(size_t));
    r.parent = calloc(2, sizeof(size_t));
    ck_assert_ptr_nonnull(r.nodes);
    ck_assert_ptr_nonnull(r.dist);
    ck_assert_ptr_nonnull(r.parent);
    r.nodes[0] = 1;
    r.nodes[1] = 2;
    r.dist[0] = 0;
    r.dist[1] = 1;
    r.parent[0] = 1;
    r.parent[1] = 1;

    graphAlgPathResultDestroy(NULL);
    graphAlgPathResultDestroy(&r);
    assertOutClean(&r);
}
END_TEST

Suite *bfspath_suite(void) {
    Suite *s = suite_create("graphAlg-bfspath");
    TCase *tc = tcase_create("path");
    tcase_add_test(tc, bfsPathMultiHop);
    tcase_add_test(tc, bfsPathSourceEqualsTarget);
    tcase_add_test(tc, bfsPathOneEdge);
    tcase_add_test(tc, bfsPathUnreachable);
    tcase_add_test(tc, bfsPathSourceAbsent);
    tcase_add_test(tc, bfsPathBadArgs);
    tcase_add_test(tc, bfsPathSparseIds);
    tcase_add_test(tc, bfsPathDirectedOneWay);
    tcase_add_test(tc, bfsPathDiamond);
    tcase_add_test(tc, pathResultDestroyFreesOwnedBuffers);
    suite_add_tcase(s, tc);
    return s;
}

int main(void) {
    Suite *s = bfspath_suite();
    SRunner *sr = srunner_create(s);
    srunner_set_fork_status(sr, CK_NOFORK);
    srunner_run_all(sr, CK_NORMAL);
    int failed = srunner_ntests_failed(sr);
    srunner_free(sr);
    return (failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
