/*
 * @Description:
 * @Author: HJY
 * @Date: 2022-11-16 20:33:10
 * @LastEditTime: 2022-11-23 17:14:36
 * @LastEditors: HJY
 */
/*
 * Copyright (C) 2019-2030 Alibaba Group Holding Limited
 */

#undef _DEBUG_H_
#define LOG_TAG_CONST       YUNIT_TEST          //修改成文件名
#define LOG_TAG             "[YUNIT_TEST]"
#include "debug.h"
#define LOG_v(t)  log_tag_const_v_ ## t
#define LOG_i(t)  log_tag_const_i_ ## t
#define LOG_d(t)  log_tag_const_d_ ## t
#define LOG_w(t)  log_tag_const_w_ ## t
#define LOG_e(t)  log_tag_const_e_ ## t
#define LOG_c(t)  log_tag_const_c_ ## t
#define LOG_tag(tag, n) n(tag)
const char LOG_tag(LOG_TAG_CONST, LOG_v) AT(.LOG_TAG_CONST) = 1;
const char LOG_tag(LOG_TAG_CONST, LOG_i) AT(.LOG_TAG_CONST) = 1; //log_info
const char LOG_tag(LOG_TAG_CONST, LOG_d) AT(.LOG_TAG_CONST) = 1; //log_debug
const char LOG_tag(LOG_TAG_CONST, LOG_w) AT(.LOG_TAG_CONST) = 1;
const char LOG_tag(LOG_TAG_CONST, LOG_e) AT(.LOG_TAG_CONST) = 1;
const char LOG_tag(LOG_TAG_CONST, LOG_c) AT(.LOG_TAG_CONST) = 1;

#include "yunit.h"
#include "hs_private.h"
//#include "yc_debug.h"
#include "app_config.h"
#if TCFG_PAY_ALIOS_ENABLE && (TCFG_PAY_ALIOS_WAY_SEL==TCFG_PAY_ALIOS_WAY_T_HEAD)
extern void yunit_test_init(void);
extern void yunit_test_api(void);

void yunit_test_main(int count, char **name)
{
    int item_count = count;


    yunit_test_init();
    HS_IIC_Init();


    yunit_test_api();


    int ret = 0;

    if (item_count > 1) {
        int i;



        for (i = 1; i < item_count; i++) {


            yunit_test_suite_t *test_suite = yunit_get_test_suite(name[i]);

            if (test_suite != NULL) {
                ret = yunit_run_test_suite(test_suite);
                log_debug("suite %s completed with %d\n", name[i], ret);

                continue;
            } else {
                log_debug("suite name err\n");
                return;
            }

            const char *suite_case = name[i];
            char *test = strrchr(suite_case, ':');

            if (test != NULL) {
                *test++ = '\0';

                test_suite = yunit_get_test_suite(suite_case);

                if (test_suite != NULL) {
                    yunit_test_case_t *test_case = yunit_get_test_case(test_suite, test);

                    if (test_case != NULL) {
                        ret = yunit_run_test_case(test_suite, test_case);
                        log_debug("suite %s completed with %d\n", name[i], ret);
                    } else {
                        log_debug("test case %s not found\n", test);
                    }
                } else {
                    log_debug("suite %s not found\n", suite_case);
                }
            }
        }
    } else {


        ret = yunit_test_run();
        log_debug("\nTests completed with return value %d\n", ret);
    }

    yunit_test_print_result();

    yunit_test_deinit();
}

void yunit_sdk_test(void)
{
    log_info("%s\n", __func__);
    char *name[15] = {"ALIPAY_HAL"};
    yunit_test_main(1, name);
}

#endif
