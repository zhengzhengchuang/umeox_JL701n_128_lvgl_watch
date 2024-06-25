/*
 * Copyright (C) 2019-2030 Alibaba Group Holding Limited
 */

#undef _DEBUG_H_
#define LOG_TAG_CONST       YUNIT          //修改成文件名
#define LOG_TAG             "[YUNIT]"
#include "debug.h"
#define LOG_v(t)  log_tag_const_v_ ## t
#define LOG_i(t)  log_tag_const_i_ ## t
#define LOG_d(t)  log_tag_const_d_ ## t
#define LOG_w(t)  log_tag_const_w_ ## t
#define LOG_e(t)  log_tag_const_e_ ## t
#define LOG_c(t)  log_tag_const_c_ ## t
#define LOG_tag(tag, n) n(tag)
const char LOG_tag(LOG_TAG_CONST, LOG_v) AT(.LOG_TAG_CONST) = 0;
const char LOG_tag(LOG_TAG_CONST, LOG_i) AT(.LOG_TAG_CONST) = 1; //log_info
const char LOG_tag(LOG_TAG_CONST, LOG_d) AT(.LOG_TAG_CONST) = 1; //log_debug
const char LOG_tag(LOG_TAG_CONST, LOG_w) AT(.LOG_TAG_CONST) = 1;
const char LOG_tag(LOG_TAG_CONST, LOG_e) AT(.LOG_TAG_CONST) = 1;
const char LOG_tag(LOG_TAG_CONST, LOG_c) AT(.LOG_TAG_CONST) = 1;

#include <stdarg.h>
#include "yunit.h"
#include "alipay_iot.h"
//#include "yc_debug.h"
#include "app_config.h"
#if TCFG_PAY_ALIOS_ENABLE && (TCFG_PAY_ALIOS_WAY_SEL==TCFG_PAY_ALIOS_WAY_T_HEAD)
#define TAG "yunit"

typedef struct yunit_test_case_node_s {
    struct yunit_test_case_node_s *next;
    const char                    *name;
    yunit_test_case_proc           test_proc;
} yunit_test_case_node_t;

typedef struct yunit_test_suite_node_s {
    struct yunit_test_suite_node_s *next;
    const char                     *name;
    yunit_test_suit_init            init;
    yunit_test_suit_deinit          deinit;
    yunit_test_case_setup           setup;
    yunit_test_case_teardown        teardown;
    yunit_test_case_node_t          test_case_list_header;
} yunit_test_suite_node_t;

#define TEST_RESULT_MESSAGE_LENGTH    128
typedef struct yunit_test_case_result_s {
    struct yunit_test_case_result_s *next;
    const char                      *file;
    size_t                           line;
    char                             msg[TEST_RESULT_MESSAGE_LENGTH];
} yunit_test_case_result_t;

typedef struct {
    yunit_test_suite_node_t  test_suite_list_header;
    int                      all_test_cases_count;
    int                      failed_test_cases_count;
    int                      fatal_test_cases_count;
    yunit_test_case_result_t failed_test_case_result;
} yunit_test_context_t;

static int yunit_run_test_case_list(
    yunit_test_suite_node_t *suite,
    yunit_test_case_node_t *test_case_list_header);

yunit_test_context_t g_test_context;
void *my_suite_malloc(int count)
{
    return hal_malloc(count);
}

void *my_case_malloc(int count)
{
    return hal_malloc(count);
}

void *my_result_malloc(int count)
{
    return hal_malloc(count);
}

void my_free(void *data)
{
    if (data != NULL) {
        hal_free(data);
    }
}


void yunit_test_init(void)
{
    log_info("%s\n", __func__);
    memset(&g_test_context, 0, sizeof(yunit_test_context_t));
}

void yunit_test_deinit(void)
{
    yunit_test_suite_node_t *test_suite_node = g_test_context.test_suite_list_header.next;
    while (test_suite_node != NULL) {
        yunit_test_case_node_t *test_case_node = test_suite_node->test_case_list_header.next;
        while (test_case_node != NULL) {
            yunit_test_case_node_t *test_case_node_next = test_case_node->next;
            my_free(test_case_node);
            test_case_node = test_case_node_next;
        }

        yunit_test_suite_node_t *test_suite_node_next = test_suite_node->next;
        my_free(test_suite_node);
        test_suite_node = test_suite_node_next;
    }

    yunit_test_case_result_t *test_case_result = g_test_context.failed_test_case_result.next;
    while (test_case_result != NULL) {
        yunit_test_case_result_t *test_case_result_next = test_case_result->next;
        my_free(test_case_result);
        test_case_result = test_case_result_next;
    }
}

void log_test(int flag, char *name)
{
    if (flag != 1) {
        log_debug("%s-FAIL", name);
    }

}


void *yunit_add_test_suite(
    const char *name,
    yunit_test_suit_init init,
    yunit_test_suit_deinit deinit,
    yunit_test_case_setup setup,
    yunit_test_case_teardown teardown)
{
    log_info("%s\n", __func__);
    yunit_test_suite_node_t *node = my_suite_malloc(sizeof(yunit_test_suite_node_t));
    if (node == NULL) {
        log_debug("%s\n", "out of memory");
        return NULL;
    }

    node->next = NULL;
    node->name = name;
    node->init = init;
    node->deinit = deinit;
    node->setup = setup;
    node->teardown = teardown;

    yunit_test_suite_node_t *prev = &g_test_context.test_suite_list_header;
    while (prev->next != NULL) {
        prev = prev->next;
    }

    prev->next = node;

    return node;
}

int yunit_add_test_case(void *test_suite, const char *name, yunit_test_case_proc proc)
{

    yunit_test_suite_node_t *test_suite_node = test_suite;
    yunit_test_case_node_t *prev = &test_suite_node->test_case_list_header;


    while (prev->next != NULL) {
        prev = prev->next;
    }




    yunit_test_case_node_t *node = my_case_malloc(sizeof(yunit_test_case_node_t));


    if (node == NULL) {
        log_debug("%s\n", "out of memory");
        return -1;
    }
    node->name = name;
    node->test_proc = proc;

    prev->next = node;



    return 0;
}

int yunit_add_test_suites(yunit_test_suite_t *test_suit_array)
{
    int i = 0;
    while (test_suit_array[i].name != NULL) {
        yunit_test_suite_t *test_suite = &test_suit_array[i++];
        void *test_suite_node = yunit_add_test_suite(
                                    test_suite->name,
                                    test_suite->init, test_suite->deinit,
                                    test_suite->setup, test_suite->teardown);

        if (test_suite_node == NULL) {
            return -1;
        }

        int j = 0;
        yunit_test_case_t *test_case_array = test_suite->test_case_array;
        while (test_case_array[j].name != NULL) {
            yunit_test_case_t *test_case = &test_case_array[j++];
            int ret = yunit_add_test_case(
                          test_suite_node,
                          test_case->name, test_case->test_proc);
            if (ret == -1) {
                return ret;
            }
        }
    }

    return 0;
}

void *yunit_get_test_suite(const char *name)
{
    yunit_test_suite_node_t *node = g_test_context.test_suite_list_header.next;


    while (node != NULL) {
        if (strcmp(node->name, name) == 0) {
            return node;
        }
        node = node->next;
    }

    return NULL;
}

void *yunit_get_test_case(void *test_suite, const char *name)
{
    yunit_test_suite_node_t *test_suite_node = test_suite;

    yunit_test_case_node_t *node = test_suite_node->test_case_list_header.next;
    while (node != NULL) {
        if (strcmp(node->name, name) == 0) {
            return node;
        }
        node = node->next;
    }

    return NULL;
}

int yunit_test_run(void)
{
    int ret = 0;

    yunit_test_suite_node_t *test_suite = g_test_context.test_suite_list_header.next;
    while (test_suite != NULL) {
        ret = yunit_run_test_suite(test_suite);
        if (ret != 0) {
            return ret;
        }
        test_suite = test_suite->next;
    }

    return ret;
}

int yunit_run_test_suite(void *test_suite)
{
    int ret = 0;

    yunit_test_suite_node_t *node = test_suite;

    log_debug("run test suites %s\n", node->name);

    if (node->init != NULL) {
        node->init();
    }

    ret = yunit_run_test_case_list(node, &node->test_case_list_header);

    if (node->deinit != NULL) {
        node->deinit();
    }

    if (ret != 0) {
        log_debug("run test suite %s error: ret = %d", node->name, ret);
        return ret;
    }

    return 0;
}

static int _run_test_case(void *test_suite, void *test_case)
{
    yunit_test_suite_node_t *test_suite_node = test_suite;
    yunit_test_case_node_t *test_case_node = test_case;
    int cnt1 = g_test_context.failed_test_cases_count + g_test_context.fatal_test_cases_count;
    int cnt2;
    //long long now = csi_tick_get_ms();
    int delta = 0;

    if (test_suite_node->setup != NULL) {
        test_suite_node->setup();
    }

    //fprintf(stderr, "run test case %s\n", test_case_node->name);
    log_debug("run test case %s\n", test_case_node->name);

    test_case_node->test_proc();

    if (test_suite_node->teardown != NULL) {
        test_suite_node->teardown();
    }

    cnt2 = g_test_context.failed_test_cases_count + g_test_context.fatal_test_cases_count;
    //delta = csi_tick_get_ms() - now;
    //fprintf(stderr, "test case %s finished %d failed %d ms\n", test_case_node->name, cnt2-cnt1, delta);
    log_debug("test case %s finished %d failed %d ms\n", test_case_node->name, cnt2 - cnt1, delta);

    return 0;
}

int yunit_run_test_case(void *test_suite, void *test_case)
{
    int ret = 0;

    yunit_test_suite_node_t *tnode = test_suite;
    yunit_test_case_node_t *tcase = test_case;

    log_debug("run test suites %s\n", tnode->name);

    if (tnode->init != NULL) {
        tnode->init();
    }

    ret = _run_test_case(tnode, tcase);

    if (tnode->deinit != NULL) {
        tnode->deinit();
    }

    if (ret != 0) {
        log_debug("run test suite %s error: ret = %d", tnode->name, ret);
        return ret;
    }

    return 0;
}

int yunit_run_test_case_list(yunit_test_suite_node_t *test_suite,
                             yunit_test_case_node_t *test_case_list_header)
{
    int ret = 0;

    yunit_test_case_node_t *node = test_case_list_header->next;

    while (node != NULL) {
        ret = _run_test_case(test_suite, node);
        if (ret != 0) {
            return ret;
        }

        node = node->next;
    }

    return ret;
}

void yunit_add_test_case_result(int type, const char *file, size_t line, const char *fmt, ...)
{
    log_info("%s[line:%d]\n", __func__, __LINE__);
    g_test_context.all_test_cases_count++;


    if (type == TEST_RESULT_SUCCESS) {
        return;
    }
    if (type == TEST_RESULT_FAILURE) {
        g_test_context.failed_test_cases_count++;
    } else if (type == TEST_RESULT_FATAL) {
        g_test_context.fatal_test_cases_count++;
    } else {

    }
    log_info("%s[line:%d]\n", __func__, __LINE__);
    yunit_test_case_result_t *result = my_result_malloc(sizeof(yunit_test_case_result_t));
    if (result == NULL) {
        log_debug("%s\n", "out of memory");
        return;
    }
    log_info("%s[line:%d]\n", __func__, __LINE__);

    va_list args;
    va_start(args, fmt);
    vsnprintf(result->msg, TEST_RESULT_MESSAGE_LENGTH, fmt, args);
    va_end(args);

    result->file = file;
    result->line = line;
    log_info("%s[line:%d]\n", __func__, __LINE__);
    yunit_test_case_result_t *prev = &g_test_context.failed_test_case_result;
    while (prev != NULL && prev->next != NULL) {
        prev = prev->next;
    }

    prev->next = result;
}

void yunit_test_print_result(void)
{
    log_debug("\n--------------------------------\n");

    log_debug("%d test cases, %d failed\n\n",
              g_test_context.all_test_cases_count,
              g_test_context.failed_test_cases_count + g_test_context.fatal_test_cases_count);

    yunit_test_case_result_t *result = g_test_context.failed_test_case_result.next;
    while (result != NULL) {
        log_debug("failed at %s(#%d)  %s\n", result->file, result->line, result->msg);
        result = result->next;
    }

    log_debug("\n--------------------------------\n");
}

#endif
