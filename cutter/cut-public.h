/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2007-2008  Kouhei Sutou <kou@cozmixng.org>
 *
 *  This library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef __CUT_PUBLIC_H__
#define __CUT_PUBLIC_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <cutter/cut-types.h>

#if __GNUC__ > 2 || (__GNUC__ == 2 && __GNUC_MINOR__ > 4)
#  define CUT_GNUC_PRINTF(format_index, arg_index)                      \
    __attribute__((__format__ (__printf__, format_index, arg_index)))
#  define CUT_GNUC_NORETURN                     \
    __attribute__((__noreturn__))
#else
#  define CUT_GNUC_PRINTF(format_index, arg_index)
#  define CUT_GNUC_NORETURN
#endif

#if __GNUC__ >= 4
#  define CUT_GNUC_NULL_TERMINATED __attribute__((__sentinel__))
#else
#  define CUT_GNUC_NULL_TERMINATED
#endif

typedef int cut_boolean;

#ifndef CUT_FALSE
#  define CUT_FALSE (0)
#endif

#ifndef CUT_TRUE
#  define CUT_TRUE (!CUT_FALSE)
#endif

typedef struct _CutTestContext     CutTestContext;
typedef struct _CutSubProcess      CutSubProcess;

typedef enum {
    CUT_TEST_RESULT_INVALID = -1,
    CUT_TEST_RESULT_SUCCESS,
    CUT_TEST_RESULT_NOTIFICATION,
    CUT_TEST_RESULT_OMISSION,
    CUT_TEST_RESULT_PENDING,
    CUT_TEST_RESULT_FAILURE,
    CUT_TEST_RESULT_ERROR,
    CUT_TEST_RESULT_LAST
} CutTestResultStatus;

typedef struct _CutTestContextKey CutTestContextKey;
struct _CutTestContextKey {
    int unused;
};

void            cut_test_context_current_set(CutTestContextKey *key,
                                             CutTestContext *context);
CutTestContext *cut_test_context_current_get(CutTestContextKey *key);

void  cut_test_context_pass_assertion       (CutTestContext *context);
void  cut_test_context_register_result      (CutTestContext *context,
                                             CutTestResultStatus status,
                                             const char *function_name,
                                             const char *filename,
                                             unsigned int line,
                                             const char *message,
                                             ...) /* CUT_GNUC_PRINTF(7, 8) */;
void  cut_test_context_long_jump            (CutTestContext *context) CUT_GNUC_NORETURN;

const char *cut_test_context_take_string    (CutTestContext *context,
                                             char           *string);
const char *cut_test_context_take_printf    (CutTestContext *context,
                                             const char     *format,
                                             ...) CUT_GNUC_PRINTF(2, 3);
const char **cut_test_context_take_string_array
                                            (CutTestContext *context,
                                             char          **strings);


int   cut_utils_compare_string_array        (char **strings1,
                                             char **strings2);
char *cut_utils_inspect_string_array        (char **strings);
const char *cut_utils_inspect_string        (const char *string);

int   cut_utils_file_exist                  (const char *path);
char *cut_utils_build_path                  (const char *path,
                                             ...) CUT_GNUC_NULL_TERMINATED;
void  cut_utils_remove_path_recursive_force (const char *path);

int   cut_utils_regex_match                 (const char *pattern,
                                             const char *string);
char *cut_utils_fold                        (const char *string);
char *cut_utils_append_diff                 (const char *message,
                                             const char *from,
                                             const char *to);

char *cut_test_context_build_fixture_data_path
                                            (CutTestContext *context,
                                             const char     *path,
                                             ...) CUT_GNUC_NULL_TERMINATED;

const char *cut_utils_get_fixture_data_string(CutTestContext *context,
                                              const char *function,
                                              const char *file,
                                              unsigned int line,
                                              const char *path,
                                              ...) CUT_GNUC_NULL_TERMINATED;

void  cut_utils_get_fixture_data_string_and_path(CutTestContext *context,
                                                 const char *function,
                                                 const char *file,
                                                 unsigned int line,
                                                 char **data,
                                                 char **fixture_data_path,
                                                 const char *path,
                                                 ...) CUT_GNUC_NULL_TERMINATED;

void  cut_test_context_add_data             (CutTestContext *context,
                                             const char     *first_data_name,
                                             ...) CUT_GNUC_NULL_TERMINATED;

void  cut_test_context_set_attributes       (CutTestContext *context,
                                             const char     *first_attribute_name,
                                             ...) CUT_GNUC_NULL_TERMINATED;

int   cut_test_context_trap_fork            (CutTestContext *context,
                                             const char     *function_name,
                                             const char     *filename,
                                             unsigned int    line);
int   cut_test_context_wait_process         (CutTestContext *context,
                                             int             pid,
                                             unsigned int    usec_timeout);
const char *cut_test_context_get_forked_stdout_message
                                            (CutTestContext *context,
                                             int             pid);
const char *cut_test_context_get_forked_stderr_message
                                            (CutTestContext *context,
                                             int             pid);

void  cut_test_context_set_fixture_data_dir (CutTestContext *context,
                                             const char     *path,
                                             ...) CUT_GNUC_NULL_TERMINATED;

char       *cut_diff_readable               (const char     *from,
                                             const char     *to);
char       *cut_diff_folded_readable        (const char     *from,
                                             const char     *to);

CutSubProcess *cut_utils_create_taken_sub_process (const char     *test_directory,
                                                   CutTestContext *test_context);

CutSubProcess *cut_sub_process_new_with_test_context
                                                  (const char     *test_directory,
                                                   CutTestContext *test_context);

cut_boolean    cut_sub_process_run                (CutSubProcess  *sub_process);
void           cut_sub_process_run_async          (CutSubProcess  *sub_process);
cut_boolean    cut_sub_process_wait               (CutSubProcess  *sub_process);
cut_boolean    cut_sub_process_is_success         (CutSubProcess  *sub_process);
cut_boolean    cut_sub_process_is_running         (CutSubProcess  *sub_process);

const char    *cut_sub_process_get_test_directory (CutSubProcess  *sub_process);
void           cut_sub_process_set_test_directory (CutSubProcess  *sub_process,
                                                   const char     *test_directory);
const char    *cut_sub_process_get_source_directory
                                                  (CutSubProcess  *sub_process);
void           cut_sub_process_set_source_directory
                                                  (CutSubProcess  *sub_process,
                                                   const char     *source_directory);

cut_boolean    cut_sub_process_is_multi_thread    (CutSubProcess  *sub_process);
cut_boolean    cut_sub_process_get_multi_thread   (CutSubProcess  *sub_process);
void           cut_sub_process_set_multi_thread   (CutSubProcess  *sub_process,
                                                   cut_boolean     multi_thread);

const char   **cut_sub_process_get_exclude_files  (CutSubProcess  *sub_process);
void           cut_sub_process_set_exclude_files  (CutSubProcess  *sub_process,
                                                   const char    **files);

const char   **cut_sub_process_get_exclude_directories
                                                  (CutSubProcess  *sub_process);
void           cut_sub_process_set_exclude_directories
                                                  (CutSubProcess  *sub_process,
                                                   const char    **directories);

const char   **cut_sub_process_get_target_test_case_names
                                                  (CutSubProcess  *sub_process);
void           cut_sub_process_set_target_test_case_names
                                                  (CutSubProcess  *sub_process,
                                                   const char    **names);

const char   **cut_sub_process_get_target_test_names
                                                  (CutSubProcess  *sub_process);
void           cut_sub_process_set_target_test_names
                                                  (CutSubProcess  *sub_process,
                                                   const char    **names);

double         cut_sub_process_get_elapsed        (CutSubProcess  *sub_process);
double         cut_sub_process_get_total_elapsed  (CutSubProcess  *sub_process);

cut_boolean    cut_sub_process_is_crashed         (CutSubProcess  *sub_process);

const char    *cut_sub_process_get_backtrace      (CutSubProcess  *sub_process);


#ifdef __cplusplus
}
#endif

#endif /* __CUT_PUBLIC_H__ */

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
