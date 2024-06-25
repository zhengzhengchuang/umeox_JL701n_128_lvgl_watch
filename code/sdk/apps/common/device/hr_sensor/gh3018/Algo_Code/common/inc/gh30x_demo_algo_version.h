/**
 * @copyright (c) 2003 - 2024, Goodix Co., Ltd. All rights reserved.
 *
 * @file    gh30x_demo_version.h
 *
 * @brief   gh30x algo call demo code version
 *
 * @version ref gh30x_example_common.h
 *
 */

#ifndef _GH30X_DEMO_ALGO_CALL_VERSION_H_
#define _GH30X_DEMO_ALGO_CALL_VERSION_H_

#define GOODIX_DEMO_MAJOR_VERSION_NUMBER        0      /**< major version number */
#define GOODIX_DEMO_MINOR_VERSION_NUMBER        5      /**< minor version number */

#define GOODIX_TO_STRING(x)                     #x     /**< number to char */
#define GOODIX_STR(x)                           GOODIX_TO_STRING(x) /**< number to char */

/// makeup version string
#define GOODIX_ALGO_CALL_VERSION_STRING         "Goodix_Algocall_" \
                                                    "v"GOODIX_STR(GOODIX_DEMO_MAJOR_VERSION_NUMBER) \
                                                    "."GOODIX_STR(GOODIX_DEMO_MINOR_VERSION_NUMBER)
#endif  /* _GH30X_DEMO_ALGO_CALL_VERSION_H_ */
