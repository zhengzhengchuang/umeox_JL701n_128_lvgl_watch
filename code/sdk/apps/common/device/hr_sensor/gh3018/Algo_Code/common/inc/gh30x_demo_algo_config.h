/**
 * @copyright (c) 2003 - 2024, Goodix Co., Ltd. All rights reserved.
 *
 * @file    gh30x_demo_algo_config.h
 *
 * @brief   gh30x algo config
 *
 * @version ref gh30x_example_common.h
 *
 */
#ifndef _GH30X_DEMO_ALGO_CONFIG_H_
#define _GH30X_DEMO_ALGO_CONFIG_H_

#ifdef GOODIX_DEMO_PLANFORM
//#define __HBD_ALGORITHM_VERTION__    __HBD_ALGORITHM_VERTION_BASIC__
#else
#define __HBD_ALGORITHM_VERTION__    __HBD_ALGORITHM_VERTION_PRO__
#endif
#ifndef __HBD_ALGORITHM_VERTION__
#error __HBD_ALGORITHM_VERTION__ is undefine !!!
#endif

#ifdef GOODIX_DEMO_PLANFORM
#include "gh30x_demo_goodix_platform.h"
#else
#define GH30X_ALGO_FUNCTION_GROUP_0            (GH30X_FUNCTION_HR | GH30X_FUNCTION_SOFT_ADT)
#define GH30X_ALGO_FUNCTION_GROUP_1            (GH30X_FUNCTION_SPO2)
#define GH30X_ALGO_FUNCTION_GROUP_2            (GH30X_FUNCTION_HRV)
#define GH30X_ALGO_FUNCTION_GROUP_3            (GH30X_NO_FUNCTION)
#define GH30X_ALGO_FUNCTION_GROUP_4            (GH30X_NO_FUNCTION)
#endif

/* 0: global buffer for goodix algorithm mem pool
 * 1: user dynamic buffer for goodix algorithm mem pool */
#define __USER_DYNAMIC_ALGO_MEM_EN__           (1)

/* function algo enable config */
#define __USE_GOODIX_ADT_ALGORITHM__           (1)
#define __USE_GOODIX_HR_ALGORITHM__            (1)
#define __USE_GOODIX_HRV_ALGORITHM__           (1)
#define __USE_GOODIX_SPO2_ALGORITHM__          (1)
#define __USE_GOODIX_SOFT_ADT_ALGORITHM__      (1)

/* algorithm log */
#ifdef GOODIX_DEMO_PLANFORM
#define __ALGO_LOG_CONFIG__                    (1)
#else
#define __ALGO_LOG_CONFIG__                    (0)
#endif

/* algorithm config max number */
#define __GOODIX_ALGO_CFG_LIST_MAX_NUM__       (1)

/*algorithm output value process strategy config*/
#define __GH30X_HR_OUTPUT_VALUE_STRATEGY_EN__  (0)

/* algoritm channel nunber config*/
#define __HR_ALGORITHM_SUPPORT_CHNL_NUM__      (1)
#define __SPO2_ALGORITHM_SUPPORT_CHNL_NUM__    (1)

/* algo and drv interactivity*/
#define __GH_MSG_WTIH_DRV_LAYER_EN__           (0)
#define __GH_SEPARATE_ALGO_EN__                (0)

#if (__USE_GOODIX_SOFT_ADT_ALGORITHM__)
#define __SOFT_ADT_IR_DETECT_TIMEOUT__         (30) /** (recomended value = 30 seconds) ir detect timeout **/
#endif

#define __GS_TIMESTAMP_READ_EN__               (0)
#if __GS_TIMESTAMP_READ_EN__
#define __SYNC_ACC_SAMPLERATE__                (25)
#define __SYNC_PPG_SAMPLERATE__                (25)
#define __SYNC_ACC_BUFFER_SECOND__             (2)
#define __SYNC_PPG_BUFFER_SECOND__             (1)
#define __SYNC_PPG_FRAME_NUM__                 (10)
#define __SYNC_ACC_FRAME_NUM__                 (20)
#define __SYNC_RAWDATA_MAX_NUM__               (3)
#define __SYNC_AGCINFO_MAX_NUM__               (3)
#define __SYNC_FLAG_MAX_NUM__                  (3)
#endif

#define GH_SEND_MSG_WEAR_EVENT(event)   //add your code to send message to drv layer

/***********************************  DO NOT MODIFY FOLLOWING CODE *******************************/
#if (\
    __USE_GOODIX_HR_ALGORITHM__      ||\
    __USE_GOODIX_HRV_ALGORITHM__     ||\
    __USE_GOODIX_HSM_ALGORITHM__     ||\
    __USE_GOODIX_SPO2_ALGORITHM__    ||\
    __USE_GOODIX_ECG_ALGORITHM__     ||\
    __USE_GOODIX_BT_ALGORITHM__      ||\
    __USE_GOODIX_RESP_ALGORITHM__    ||\
    __USE_GOODIX_AF_ALGORITHM__      ||\
    __USE_GOODIX_BP_ALGORITHM__      ||\
    __USE_GOODIX_SOFT_ADT_ALGORITHM__\
    )
#define __GOODIX_ALGO_CALL_MODE__           (1)
#else
#define __GOODIX_ALGO_CALL_MODE__           (0)
#endif

#endif /* _GH30X_DEMO_ALGO_CONFIG_H_ */

/********END OF FILE********* Copyright (c) 2003 - 2024, Goodix Co., Ltd. ********/
