/**
 * @copyright (c) 2003 - 2024, Goodix Co., Ltd. All rights reserved.
 *
 * @file    gh30x_example.h
 *
 * @brief   example code for gh30x
 *
 */

#ifndef _GH30X_EXAMPLE_H_
#define _GH30X_EXAMPLE_H_

#include "gh30x_example_common.h" 

/**
 * @brief start run mode enum
 */
typedef enum
{
    GH30X_RUN_MODE_HB = RUN_MODE_ADT_HB_DET,   
    GH30X_RUN_MODE_HRV = RUN_MODE_HRV_DET,
    GH30X_RUN_MODE_SPO2 = RUN_MODE_SPO2_DET,
    GH30X_RUN_MODE_GETRAW = RUN_MODE_GETRAWDATA_DET,
    GH30X_RUN_MODE_HARDWARE_ADT = RUN_MODE_HARDWARE_ADT_DET,
    GH30X_RUN_MODE_HR_SPO2 = RUN_MODE_HR_SPO2_DET,
    GH30X_RUN_MODE_FACTORY = RUN_MODE_FACTORY_DET,
} EMGh30xRunMode;

/**
 * @brief function state
 */
typedef enum
{
    GH30X_FUNCTIONAL_STATE_DISABLE = 0, 
    GH30X_FUNCTIONAL_STATE_ENABLE = 1,
} EMGh30xFunctionState;

/**
 * @brief system test options
 */
typedef enum
{
    HBDTEST_TESTIEM_COMM = 0x1,
    HBDTEST_TESTIEM_OTP = 0x2,
    HBDTEST_TESTIEM_CTR = 0x4,
    HBDTEST_TESTIEM_LEAK = 0x8,
    HBDTEST_ONLYTEST_CTR_LEAK =0x1c,
    HBDTEST_TEST_ALL_TEST = 0x1f
} EMGh30xTestItem;

#define HR_WORK_MODE (GH30X_FUNCTION_HR|GH30X_FUNCTION_ADT) 
#define BO_WORK_MODE (GH30X_FUNCTION_SPO2|GH30X_FUNCTION_ADT)

/// gh30x module init
int gh30x_module_init(void);

/// gh30x module start
void gh30x_module_start(GU32 unFunctionMode);

/// gh30x module stop
void gh30x_module_stop(void);

/// gh30x interrupt top handler
void hal_gh30x_int_handler_top_half(void);

/// gh30x interrupt bottom handler
void hal_gh30x_int_handler_bottom_half(void);

/// ble recv data handler
void ble_module_recv_data_via_gdcs(uint8_t *data, uint8_t length);

/// uart recv data handler
void uart_module_recv_data(uint8_t *data, uint8_t length);

/// gh30x get mcu start mode
GU8 gh30x_mcu_start_mode_get(void);

/// gh30x set mcu start mode
void gh30x_mcu_start_mode_set(GU8 mcu_start_mode);

/// gh30x system test start
void gh30x_systemtest_start(EMGh30xTestItem mode);

#endif /* _GH30X_EXAMPLE_H_ */

/********END OF FILE********* Copyright (c) 2003 - 2024, Goodix Co., Ltd. ********/
