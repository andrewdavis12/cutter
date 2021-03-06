#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif
#include <cutter.h>
#include <cutter/cut-test-runner.h>
#include <cutter/cut-test-case.h>
#include <cutter/cut-test-suite.h>
#include <cutter/cut-loader.h>

#include "../lib/cuttest-utils.h"

void test_run (void);
void test_run_test_case (void);
void test_run_test_case_with_regex (void);
void test_run_test (void);
void test_run_test_with_regex (void);
void test_run_test_in_test_case (void);
void test_run_test_with_regex_in_test_case (void);
void test_run_test_in_test_case_with_regex (void);
void test_run_test_with_regex_in_test_case_with_regex (void);
void test_run_test_in_test_case_with_null (void);
void test_run_test_with_filter_with_null (void);

static CutRunContext *run_context;
static CutTestSuite *test_object;
static CutLoader *loader;

static gint n_run_stub_run_test_function = 0;
static gint n_run_stub_test_function1 = 0;
static gint n_run_stub_test_function2 = 0;

static gint n_run_stock_run_test_function = 0;
static gint n_run_stock_test_function1 = 0;
static gint n_run_stock_test_function2 = 0;

static void
stub_test_function1 (void)
{
    cut_assert_equal_int(1, 1);
    cut_assert_equal_int(1, 1);
    cut_assert_equal_int(1, 1);
    cut_assert_equal_int(1, 1);
    n_run_stub_test_function1++;
}

static void
stub_test_function2 (void)
{
    n_run_stub_test_function2++;
}

static void
stub_run_test_function (void)
{
    n_run_stub_run_test_function++;
}

static void
stock_test_function1 (void)
{
    n_run_stock_test_function1++;
}

static void
stock_test_function2 (void)
{
    n_run_stock_test_function2++;
}

static void
stock_run_test_function (void)
{
    n_run_stock_run_test_function++;
}

void
cut_setup (void)
{
    CutTestCase *test_case;
    gchar *test_path;

    n_run_stub_test_function1 = 0;
    n_run_stub_test_function2 = 0;
    n_run_stub_run_test_function = 0;
    n_run_stock_test_function1 = 0;
    n_run_stock_test_function2 = 0;
    n_run_stock_run_test_function = 0;

    run_context = CUT_RUN_CONTEXT(cut_test_runner_new());

    test_object = cut_test_suite_new_empty();

    test_path = g_build_filename(cuttest_get_base_dir(),
                                 "fixtures",
                                 "loader",
                                 "test",
#ifndef G_OS_WIN32
                                 ".libs",
#endif
                                 "stub-test-functions." G_MODULE_SUFFIX,
                                 NULL);
    loader = cut_loader_new(test_path);
    g_free(test_path);
    test_case = cut_loader_load_test_case(loader);
    cut_test_suite_add_test_case(test_object, test_case);
    g_object_unref(test_case);

    test_case = cut_test_case_new("stub-test-case", NULL, NULL,
                                  NULL, NULL);
    cuttest_add_test(test_case, "stub_test_1", stub_test_function1);
    cuttest_add_test(test_case, "stub_test_2", stub_test_function2);
    cuttest_add_test(test_case, "run_test_function", stub_run_test_function);
    cut_test_suite_add_test_case(test_object, test_case);
    g_object_unref(test_case);

    test_case = cut_test_case_new("stock-test-case", NULL, NULL,
                                  NULL, NULL);
    cuttest_add_test(test_case, "stock_test_1", stock_test_function1);
    cuttest_add_test(test_case, "stock_test_2", stock_test_function2);
    cuttest_add_test(test_case, "stock_run_test_function",
                     stock_run_test_function);
    cut_test_suite_add_test_case(test_object, test_case);
    g_object_unref(test_case);
}

void
cut_teardown (void)
{
    g_object_unref(loader);
    g_object_unref(test_object);
    g_object_unref(run_context);
}

static gboolean
run_test_case (gchar *test_case_name)
{
    return cut_test_suite_run_test_in_test_case(test_object,
                                                run_context,
                                                "/.*/",
                                                test_case_name);
}

static gboolean
run_test (gchar *test_name)
{
    return cut_test_suite_run_test(test_object, run_context, test_name);
}

static gboolean
run_test_in_test_case (gchar *test_name, gchar *test_case_name)
{
    return cut_test_suite_run_test_in_test_case(test_object,
                                                run_context,
                                                test_name,
                                                test_case_name);
}

static gboolean
run_test_with_filter (const gchar **test_case_names, const gchar **test_names)
{
    return cut_test_suite_run_with_filter(test_object,
                                          run_context,
                                          test_case_names,
                                          test_names);
}

void
test_run (void)
{
    cut_assert(cut_test_suite_run(test_object, run_context));
    cut_assert_equal_int(1, n_run_stub_test_function1);
    cut_assert_equal_int(1, n_run_stub_test_function2);
    cut_assert_equal_int(1, n_run_stub_run_test_function);
    cut_assert_equal_int(1, n_run_stock_test_function1);
    cut_assert_equal_int(1, n_run_stock_test_function2);
    cut_assert_equal_int(1, n_run_stock_run_test_function);
}

void
test_run_test_case (void)
{
    cut_assert(cut_test_suite_run_test_case(test_object, run_context,
                                            "stub-test-case"));
    cut_assert_equal_int(1, n_run_stub_test_function1);
    cut_assert_equal_int(1, n_run_stub_test_function2);
    cut_assert_equal_int(1, n_run_stub_run_test_function);
    cut_assert_equal_int(0, n_run_stock_test_function1);
    cut_assert_equal_int(0, n_run_stock_test_function2);
    cut_assert_equal_int(0, n_run_stock_run_test_function);
}

void
test_run_test_case_with_regex (void)
{
    cut_assert(run_test_case("/stub/"));
    cut_assert_equal_int(1, n_run_stub_test_function1);
    cut_assert_equal_int(1, n_run_stub_test_function2);
    cut_assert_equal_int(1, n_run_stub_run_test_function);
    cut_assert_equal_int(0, n_run_stock_test_function1);
    cut_assert_equal_int(0, n_run_stock_test_function2);
    cut_assert_equal_int(0, n_run_stock_run_test_function);
}

void
test_run_test (void)
{
    cut_assert(run_test("run_test_function"));
    cut_assert_equal_int(0, n_run_stub_test_function1);
    cut_assert_equal_int(0, n_run_stub_test_function2);
    cut_assert_equal_int(1, n_run_stub_run_test_function);
    cut_assert_equal_int(0, n_run_stock_test_function1);
    cut_assert_equal_int(0, n_run_stock_test_function2);
    cut_assert_equal_int(0, n_run_stock_run_test_function);
}

void
test_run_test_with_regex (void)
{
    cut_assert(run_test("/stub/"));
    cut_assert_equal_int(1, n_run_stub_test_function1);
    cut_assert_equal_int(1, n_run_stub_test_function2);
    cut_assert_equal_int(0, n_run_stub_run_test_function);
    cut_assert_equal_int(0, n_run_stock_test_function1);
    cut_assert_equal_int(0, n_run_stock_test_function2);
    cut_assert_equal_int(0, n_run_stock_run_test_function);
}

void
test_run_test_in_test_case (void)
{
    cut_assert(run_test_in_test_case("run_test_function", "stub-test-case"));
    cut_assert_equal_int(0, n_run_stub_test_function1);
    cut_assert_equal_int(0, n_run_stub_test_function2);
    cut_assert_equal_int(1, n_run_stub_run_test_function);
    cut_assert_equal_int(0, n_run_stock_test_function1);
    cut_assert_equal_int(0, n_run_stock_test_function2);
    cut_assert_equal_int(0, n_run_stock_run_test_function);
}

void
test_run_test_with_regex_in_test_case (void)
{
    cut_assert(run_test_in_test_case("/stub/", "stub-test-case"));
    cut_assert_equal_int(1, n_run_stub_test_function1);
    cut_assert_equal_int(1, n_run_stub_test_function2);
    cut_assert_equal_int(0, n_run_stub_run_test_function);
    cut_assert_equal_int(0, n_run_stock_test_function1);
    cut_assert_equal_int(0, n_run_stock_test_function2);
    cut_assert_equal_int(0, n_run_stock_run_test_function);
}

void
test_run_test_in_test_case_with_regex (void)
{
    cut_assert(run_test_in_test_case("run_test_function", "/stub/"));
    cut_assert_equal_int(0, n_run_stub_test_function1);
    cut_assert_equal_int(0, n_run_stub_test_function2);
    cut_assert_equal_int(1, n_run_stub_run_test_function);
    cut_assert_equal_int(0, n_run_stock_test_function1);
    cut_assert_equal_int(0, n_run_stock_test_function2);
    cut_assert_equal_int(0, n_run_stock_run_test_function);
}

void
test_run_test_with_regex_in_test_case_with_regex (void)
{
    cut_assert(run_test_in_test_case("/stub/", "/stub/"));
    cut_assert_equal_int(1, n_run_stub_test_function1);
    cut_assert_equal_int(1, n_run_stub_test_function2);
    cut_assert_equal_int(0, n_run_stub_run_test_function);
    cut_assert_equal_int(0, n_run_stock_test_function1);
    cut_assert_equal_int(0, n_run_stock_test_function2);
    cut_assert_equal_int(0, n_run_stock_run_test_function);
}

void
test_run_test_in_test_case_with_null (void)
{
    cut_assert(run_test(NULL));
    cut_assert_equal_int(1, n_run_stub_test_function1);
    cut_assert_equal_int(1, n_run_stub_test_function2);
    cut_assert_equal_int(1, n_run_stub_run_test_function);
    cut_assert_equal_int(1, n_run_stock_test_function1);
    cut_assert_equal_int(1, n_run_stock_test_function2);
    cut_assert_equal_int(1, n_run_stock_run_test_function);
}

void
test_run_test_with_filter_with_null (void)
{
    cut_assert(run_test_with_filter(NULL, NULL));
    cut_assert_equal_int(1, n_run_stub_test_function1);
    cut_assert_equal_int(1, n_run_stub_test_function2);
    cut_assert_equal_int(1, n_run_stub_run_test_function);
    cut_assert_equal_int(1, n_run_stock_test_function1);
    cut_assert_equal_int(1, n_run_stock_test_function2);
    cut_assert_equal_int(1, n_run_stock_run_test_function);
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/

