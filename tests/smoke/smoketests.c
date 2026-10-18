/**
 * Smoke test: graphAlg links the installed graphData public API only.
 */

#include <check.h>
#include <stdlib.h>
#include <string.h>

#include <graphAlg.h>
#include <graphInit.h>
#include <graphCrud.h>
#include <graphData.h>

START_TEST(versionIsSet) {
    const char *v = graphAlgVersion();
    ck_assert_ptr_nonnull(v);
    ck_assert(strlen(v) > 0);
}
END_TEST

START_TEST(usesInstalledGraphData) {
    struct graph_t *g = initGraph(LINKED | DIRECTED | GENERIC, 0, NULL);
    struct graphops_t *ops = NULL;
    size_t id = 1;

    ck_assert_ptr_nonnull(g);
    ops = getOperations(g);
    ck_assert_ptr_nonnull(ops);
    ck_assert(ops->addNode(&id, g) == GRAPH_OK);
    ck_assert(ops->nodeCount(g) == 1);

    destroyGraphops((void **)&ops);
    clearGraph(g);
    destroyGraph((void **)&g);
}
END_TEST

Suite *smoke_suite(void) {
    Suite *s = suite_create("graphAlg-smoke");
    TCase *tc = tcase_create("core");
    tcase_add_test(tc, versionIsSet);
    tcase_add_test(tc, usesInstalledGraphData);
    suite_add_tcase(s, tc);
    return s;
}

int main(void) {
    Suite *s = smoke_suite();
    SRunner *sr = srunner_create(s);
    srunner_set_fork_status(sr, CK_NOFORK);
    srunner_run_all(sr, CK_NORMAL);
    int failed = srunner_ntests_failed(sr);
    srunner_free(sr);
    return (failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
