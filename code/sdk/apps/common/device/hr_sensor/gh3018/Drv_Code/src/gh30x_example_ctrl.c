/**
 * @copyright (c) 2003 - 2024, Goodix Co., Ltd. All rights reserved.
 *
 * @file    gh30x_example_ctrl.c
 *
 * @brief   example code for gh30x
 *
 */

/************************************************************************
*   Include header files
************************************************************************/

#include "../inc/gh30x_example_common.h"
#if (__USE_GOODIX_APP__)
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/// debug log string
#if (__EXAMPLE_DEBUG_LOG_LVL__) // debug level > 0

/// dbg run mode string
const char dbg_rum_mode_string[][27] =
{
    "RUN_MODE_INVALID\0",
    "RUN_MODE_INVALID\0",
    "RUN_MODE_ADT_HB_DET\0",
    "RUN_MODE_HRV_DET\0",
    "RUN_MODE_INVALID\0",
    "RUN_MODE_GETRAWDATA_DET\0",
    "RUN_MODE_INVALID\0",
    "RUN_MODE_SPO2_DET\0",
    "RUN_MODE_HARDWARE_ADT_DET\0",
    "RUN_MODE_INVALID\0",
    "RUN_MODE_HR_SPO2_DET\0",
    "RUN_MODE_FACTORY_DET\0",
};
/// dbg communicate cmd string
const char dbg_comm_cmd_string[][35] =
{
    "COMM_CMD_ALGO_IN_MCU_HB_START\0",
    "COMM_CMD_ALGO_IN_MCU_HB_STOP\0",
    "COMM_CMD_ALGO_IN_APP_HB_START\0",
    "COMM_CMD_ALGO_IN_APP_HB_STOP\0",
    "COMM_CMD_ALGO_IN_MCU_HRV_START\0",
    "COMM_CMD_ALGO_IN_MCU_HRV_STOP\0",
    "COMM_CMD_ALGO_IN_APP_HRV_START\0",
    "COMM_CMD_ALGO_IN_APP_HRV_STOP\0",
    "COMM_CMD_ADT_SINGLE_MODE_START\0",
    "COMM_CMD_ADT_SINGLE_MODE_STOP\0",
    "COMM_CMD_ADT_CONTINUOUS_MODE_START\0",
    "COMM_CMD_ADT_CONTINUOUS_MODE_STOP\0",
    "COMM_CMD_ALGO_IN_MCU_SPO2_START\0",
    "COMM_CMD_ALGO_IN_MCU_SPO2_STOP\0",
    "COMM_CMD_ALGO_IN_APP_SPO2_START\0",
    "COMM_CMD_ALGO_IN_APP_SPO2_STOP\0",
    "COMM_CMD_ALGO_IN_MCU_BP_START\0",
    "COMM_CMD_ALGO_IN_MCU_BP_STOP\0",
    "COMM_CMD_INVALID\0",
};
/// dbg ret val string
const char dbg_ret_val_string[][35] =
{
    "HBD_RET_LED_CONFIG_ALL_OFF_ERROR\0",
    "HBD_RET_NO_INITED_ERROR\0",
    "HBD_RET_RESOURCE_ERROR\0",
    "HBD_RET_COMM_ERROR\0",
    "HBD_RET_COMM_NOT_REGISTERED_ERROR\0",
    "HBD_RET_PARAMETER_ERROR\0",
    "HBD_RET_GENERIC_ERROR\0",
    "HBD_RET_OK\0",
};

#endif

#if (__SYSTEM_TEST_SUPPORT__)
ST_ANALOG_REG_LIST g_stHbRegList;
#endif

#if (__FACTORY_DET_SUPPORT__)
STFactoryModeCfg gstFactoryModeCfg = {
    FACTORY_IR_RED_STATE,
    FACTORY_IR_RED_STATE
};
#endif

#if (__HR_SPO2_DET_SUPPORT__)
///gh30x run method
uint8_t g_uchGh30xRunMethod = 0;
#endif

extern bool goodix_app_start_app_mode;

/// gh30x load new config
GS8 gh30x_Load_new_config(const ST_REGISTER *config_ptr, uint16_t len)
{
    GU8 index = 0;
    for (index = 0; index < __RETRY_MAX_CNT_CONFIG__; index ++) // retry
    {
        if (HBD_LoadNewRegConfigArr(config_ptr, len) == HBD_RET_OK)
        {
            return HBD_RET_OK;
        }
        EXAMPLE_DEBUG_LOG_L1("gh30x load new config error\r\n");
    }
    return HBD_RET_COMM_ERROR;
}

/// gh30x adt func start
void gh30x_adt_wear_detect_start(const ST_REGISTER *config_ptr, GU16 config_len)
{
    GS16 nRes = 0;
    GU8 index = 0;

    HAL_GH30X_FIFO_INT_TIMEOUT_TIMER_STOP();
    if ((config_ptr != NULL) & (config_len != 0))
    {
#if (__HBD_ALGORITHM_USE_DYN_MEM__)
        gh30x_malloc_memory();
#endif
        gh30x_Load_new_config(config_ptr, config_len);
        nRes = HBD_AdtWearDetectStart();
        if (nRes != HBD_RET_OK)  // start
        {
            EXAMPLE_DEBUG_LOG_L1("gh30x adt start error : %d\r\n", nRes);
            for (index = 0; index < __RETRY_MAX_CNT_CONFIG__; index ++) // retry
            {
                if (HBD_SimpleInit(&gh30x_init_config) == HBD_RET_OK)
                {
                    if (gh30x_Load_new_config(config_ptr, config_len) == HBD_RET_OK)
                    {
                        if (HBD_AdtWearDetectStart() == HBD_RET_OK)
                        {
                            EXAMPLE_DEBUG_LOG_L1("gh30x adt start retry success\r\n");
                            break;
                        }
                    }
                }
            }
        }
    }
}

#if (__HB_NEED_ADT_CONFIRM__)
/// gh30x adt confirm func start
void gh30x_adt_confirm_start(void)
{
    GS16 nRes = 0;
    GU8 index = 0;

    HAL_GH30X_FIFO_INT_TIMEOUT_TIMER_STOP();
    HBD_AdtConfirmConfig(__HB_ADT_CONFIRM_GS_AMP__, __HB_ADT_CONFIRM_CHECK_CNT__, __HB_ADT_CONFIRM_THR_CNT__);
#if (__HBD_ALGORITHM_USE_DYN_MEM__)
        gh30x_malloc_memory();
#endif
    nRes = HBD_AdtConfirmStart();
    if (nRes != HBD_RET_OK)  // start
    {
        EXAMPLE_DEBUG_LOG_L1("gh30x adt confirm start error : %d\r\n", nRes);
        for (index = 0; index < __RETRY_MAX_CNT_CONFIG__; index ++) // retry
        {
            if (HBD_SimpleInit(&gh30x_init_config) == HBD_RET_OK)
            {
                if (gh30x_Load_new_config(hb_adt_confirm_reg_config, hb_adt_confirm_reg_config_len) == HBD_RET_OK)
                {
                    if (HBD_AdtConfirmStart() == HBD_RET_OK)
                    {
                        EXAMPLE_DEBUG_LOG_L1("gh30x adt confirm start retry success\r\n");
                        HAL_GH30X_FIFO_INT_TIMEOUT_TIMER_START();
                        break;
                    }
                }
            }
        }
    }
    else
    {
        HAL_GH30X_FIFO_INT_TIMEOUT_TIMER_START();
    }
}
#endif

#if (__HB_DET_SUPPORT__)
/// gh30x hb func start
#if 0
void gh30x_hb_start(void)
{
    GS16 nRes = 0;
    GU8 index = 0;

    HAL_GH30X_FIFO_INT_TIMEOUT_TIMER_STOP();

    #if (__HBA_ENABLE_WEARING__)
    GF32 wearing_config_array[3] = {0, 0, 0};
    HBD_EnableWearing(wearing_config_array);
    #endif

    HBD_SetFifoThrCnt(FIFO_THR_CONFIG_TYPE_HB, __HB_FIFO_THR_CNT_CONFIG__);
    #if (__HBD_ALGORITHM_USE_DYN_MEM__)
    gh30x_malloc_memory();
    #endif
    nRes = HBD_HbDetectStart();
    if (nRes != HBD_RET_OK)  // start
    {
        EXAMPLE_DEBUG_LOG_L1("gh30x hb start error : %d\r\n", nRes);
        for (index = 0; index < __RETRY_MAX_CNT_CONFIG__; index ++) // retry
        {
            if (HBD_SimpleInit(&gh30x_init_config) == HBD_RET_OK)
            {
                if (gh30x_Load_new_config(hb_reg_config_array, hb_reg_config_array_len) == HBD_RET_OK)
                {
                    if (HBD_HbDetectStart() == HBD_RET_OK)
                    {
                        EXAMPLE_DEBUG_LOG_L1("gh30x hb start retry success\r\n");
                        HAL_GH30X_FIFO_INT_TIMEOUT_TIMER_START();
                        break;
                    }
                }
            }
        }
    }
    else
    {
        HAL_GH30X_FIFO_INT_TIMEOUT_TIMER_START();
    }
}
#endif

#endif

#if (__HRV_DET_SUPPORT__)
#if 0
/// gh30x hrv func start
void gh30x_hrv_start(void)
{
    GS16 nRes = 0;
    GU8 index = 0;

    HAL_GH30X_FIFO_INT_TIMEOUT_TIMER_STOP();

    HBD_SetFifoThrCnt(FIFO_THR_CONFIG_TYPE_HRV, __HRV_FIFO_THR_CNT_CONFIG__);
#if (__HBD_ALGORITHM_USE_DYN_MEM__)
    gh30x_malloc_memory();
#endif

#if (__HBA_ENABLE_WEARING__)
    GF32 wearing_config_array[3] = {0, 0, 0};
    HBD_EnableWearing(wearing_config_array);
#endif

    if(g_uchGh30xRunMethod == 1)//call without adt start need load config
    {
        gh30x_Load_new_config(hrv_reg_config_array, hrv_reg_config_array_len);
    }
    // gh30x_Load_new_config(hrv_reg_config_array, hrv_reg_config_array_len);
    nRes = HBD_HbWithHrvDetectStart();

    if(nRes != HBD_RET_OK) // start
    {
        EXAMPLE_DEBUG_LOG_L1("gh30x hrv start error : %d\r\n", nRes);
        for (index = 0; index < __RETRY_MAX_CNT_CONFIG__; index ++) // retry
        {
            if (HBD_SimpleInit(&gh30x_init_config) == HBD_RET_OK)
            {
                if (gh30x_Load_new_config(hrv_reg_config_array, hrv_reg_config_array_len) == HBD_RET_OK)
                {
                    if (HBD_HbWithHrvDetectStart() == HBD_RET_OK)
                    {
                        EXAMPLE_DEBUG_LOG_L1("gh30x hrv start retry success\r\n");
                        HAL_GH30X_FIFO_INT_TIMEOUT_TIMER_START();
                        break;
                    }
                }
            }
        }
    }
    else
    {
        HAL_GH30X_FIFO_INT_TIMEOUT_TIMER_START();
    }
}
#endif

#endif

#if (__SPO2_DET_SUPPORT__)
/// gh30x spo2 func start
#if 0
void gh30x_spo2_start(void)
{
    GS16 nRes = 0;
    GU8 index = 0;

    HAL_GH30X_FIFO_INT_TIMEOUT_TIMER_STOP();

    HBD_SetFifoThrCnt(FIFO_THR_CONFIG_TYPE_SPO2, __SPO2_FIFO_THR_CNT_CONFIG__);
#if (__HBD_ALGORITHM_USE_DYN_MEM__)
    gh30x_malloc_memory();
#endif
    if(g_uchGh30xRunMethod == 1)//call without adt start need load config
    {
        gh30x_Load_new_config(spo2_reg_config_array, spo2_reg_config_array_len);
    }
    nRes = HBD_SpO2DetectStart();
    if (nRes != HBD_RET_OK)  // start
    {
        EXAMPLE_DEBUG_LOG_L1("gh30x spo2 start error : %d\r\n", nRes);
        for (index = 0; index < __RETRY_MAX_CNT_CONFIG__; index ++) // retry
        {
            if (HBD_SimpleInit(&gh30x_init_config) == HBD_RET_OK)
            {
                if (gh30x_Load_new_config(spo2_reg_config_array, spo2_reg_config_array_len) == HBD_RET_OK)
                {
                    if (HBD_SpO2DetectStart() == HBD_RET_OK)
                    {
                        EXAMPLE_DEBUG_LOG_L1("gh30x spo2 start retry success\r\n");
                        HAL_GH30X_FIFO_INT_TIMEOUT_TIMER_START();
                        break;
                    }
                }
            }
        }
    }
    else
    {
        HAL_GH30X_FIFO_INT_TIMEOUT_TIMER_START();
    }
}
#endif
#endif

#if (__GET_RAWDATA_ONLY_SUPPORT__)
/// gh30x get rawdata only func start
void gh30x_getrawdata_start(void)
{
    GU8 index = 0;

    HAL_GH30X_FIFO_INT_TIMEOUT_TIMER_STOP();

    //HBD_SetFifoThrCnt(FIFO_THR_CONFIG_TYPE_GETRAWDATA, __GET_RAWDATA_FIFO_THR_CNT_CONFIG__);
#if (__HBD_ALGORITHM_USE_DYN_MEM__)
    gh30x_malloc_memory();
#endif
    // gh30x_Load_new_config(getrawdata_reg_config_array, rawdata_reg_config_array_len);
    if (HBD_StartHBDOnly(__GET_RAWDATA_SAMPLE_RATE__, 1, __GET_RAWDATA_FIFO_THR_CNT_CONFIG__) != HBD_RET_OK)  // start
    {
        EXAMPLE_DEBUG_LOG_L1("gh30x getrawdata start error\r\n");
        for (index = 0; index < __RETRY_MAX_CNT_CONFIG__; index++) // retry
        {
            if (HBD_SimpleInit(&gh30x_init_config) == HBD_RET_OK)
            {
                if (HBD_LoadNewRegConfigArr(getrawdata_reg_config_array, getrawdata_reg_config_array_len) == HBD_RET_OK)
                {
                    if (HBD_StartHBDOnly(__GET_RAWDATA_SAMPLE_RATE__, 1, __GET_RAWDATA_FIFO_THR_CNT_CONFIG__) == HBD_RET_OK)
                    {
                        HAL_GH30X_FIFO_INT_TIMEOUT_TIMER_START();
                        break;
                    }
                }
            }
        }
    }
    else
    {
        HAL_GH30X_FIFO_INT_TIMEOUT_TIMER_START();
    }
}

#endif


#if (__HARDWARE_ADT_DET_SUPPORT__)
extern EMAdtMode gemHardwareAdtMode;
void gh30x_set_hardware_adt_mode(EMAdtMode emHardwareAdtMode)
{
    gemHardwareAdtMode = emHardwareAdtMode;
}

void gh30x_hardware_adt_start(void)
{
    GS16 nRes = 0;
    GU8 index = 0;

    HAL_GH30X_FIFO_INT_TIMEOUT_TIMER_STOP();

#if (__HBD_ALGORITHM_USE_DYN_MEM__)
    gh30x_malloc_memory();
#endif

    gh30x_Load_new_config(hb_reg_config_array, hb_reg_config_array_len);
    if(gemHardwareAdtMode == WEAR_DETECTION)
    {
        nRes = HBD_AdtWearDetectStart();
    }
    else
    {
        nRes = HBD_AdtSheddingDetectStart();
    }

    if (nRes != HBD_RET_OK)  // start
    {
        EXAMPLE_DEBUG_LOG_L1("gh30x hardware adt start error : %d\r\n", nRes);
        for (index = 0; index < __RETRY_MAX_CNT_CONFIG__; index ++) // retry
        {
            if (HBD_SimpleInit(&gh30x_init_config) == HBD_RET_OK)
            {
                if (gh30x_Load_new_config(hb_reg_config_array, hb_reg_config_array_len) == HBD_RET_OK)
                {
                    if(gemHardwareAdtMode == SHEDDING_DETECTION)
                    {
                        if (HBD_AdtWearDetectStart() == HBD_RET_OK)
                        {
                            EXAMPLE_DEBUG_LOG_L1("gh30x hardware adt start retry success\r\n");
                            break;
                        }
                    }
                    else
                    {
                        if (HBD_AdtSheddingDetectStart() == HBD_RET_OK)
                        {
                            EXAMPLE_DEBUG_LOG_L1("gh30x hardware adt start retry success\r\n");
                            break;
                        }
                    }
                }
            }
        }
    }
    else
    {
        HAL_GH30X_FIFO_INT_TIMEOUT_TIMER_START();
    }
}
#endif

#if (__FACTORY_DET_SUPPORT__)
void gh30x_factory_start(void)
{
    GS16 nRes = HBD_RET_GENERIC_ERROR;
    GU8 index = 0;

    HAL_GH30X_FIFO_INT_TIMEOUT_TIMER_STOP();

    if(gstFactoryModeCfg.now_state == FACTORY_IR_RED_STATE)
    {
        gstFactoryModeCfg.last_state = gstFactoryModeCfg.now_state;
        gstFactoryModeCfg.now_state = FACTORY_GREEN_IR_STATE;

        if (gh30x_Load_new_config(factory_green_ir_reg_config_array, factory_green_ir_reg_config_array_len) == HBD_RET_OK)
        {
            nRes = HBD_StartHBDOnly(FACTORY_GREEN_IR_SAMPLE_RATE, HBD_FUNCTIONAL_STATE_ENABLE, FACTORY_GREEN_IR_FIFO_THR );
        }
        if (nRes != HBD_RET_OK)  // start
        {
            EXAMPLE_DEBUG_LOG_L1("gh30x_factory_start GREEN IR error : %d\r\n", nRes);
            for (index = 0; index < __RETRY_MAX_CNT_CONFIG__; index ++) // retry
            {
                if (HBD_SimpleInit(&gh30x_init_config) == HBD_RET_OK)
                {
                    if (gh30x_Load_new_config(factory_green_ir_reg_config_array, factory_green_ir_reg_config_array_len) == HBD_RET_OK)
                    {
                        if (HBD_StartHBDOnly(FACTORY_GREEN_IR_SAMPLE_RATE, HBD_FUNCTIONAL_STATE_ENABLE, FACTORY_GREEN_IR_FIFO_THR ) == HBD_RET_OK)
                        {
                            EXAMPLE_DEBUG_LOG_L1("gh30x_factory_start GREEN IR retry success\r\n");
                            HAL_GH30X_FIFO_INT_TIMEOUT_TIMER_START();
                            break;
                        }
                    }
                }
            }
        }
        else
        {
            HAL_GH30X_FIFO_INT_TIMEOUT_TIMER_START();
        }
    }
    else if(gstFactoryModeCfg.now_state == FACTORY_GREEN_IR_STATE)
    {
        gstFactoryModeCfg.last_state = gstFactoryModeCfg.now_state;
        gstFactoryModeCfg.now_state = FACTORY_IR_RED_STATE;

        if (gh30x_Load_new_config(factory_ir_red_reg_config_array, factory_ir_red_reg_config_array_len) == HBD_RET_OK)
        {
            nRes = HBD_StartHBDOnly(FACTORY_IR_RED_SAMPLE_RATE, HBD_FUNCTIONAL_STATE_ENABLE, FACTORY_IR_RED_FIFO_THR);
        }
        if (nRes != HBD_RET_OK)  // start
        {
            EXAMPLE_DEBUG_LOG_L1("gh30x_factory_start IR RED error : %d\r\n", nRes);
            for (index = 0; index < __RETRY_MAX_CNT_CONFIG__; index ++) // retry
            {
                if (HBD_SimpleInit(&gh30x_init_config) == HBD_RET_OK)
                {
                    if (gh30x_Load_new_config(factory_ir_red_reg_config_array, factory_ir_red_reg_config_array_len) == HBD_RET_OK)
                    {
                        if (HBD_StartHBDOnly(FACTORY_IR_RED_SAMPLE_RATE, HBD_FUNCTIONAL_STATE_ENABLE, FACTORY_IR_RED_FIFO_THR) == HBD_RET_OK)
                        {
                            EXAMPLE_DEBUG_LOG_L1("gh30x_factory_start IR RED retry success\r\n");
                            HAL_GH30X_FIFO_INT_TIMEOUT_TIMER_START();
                            break;
                        }
                    }
                }
            }
        }
        else
        {
            HAL_GH30X_FIFO_INT_TIMEOUT_TIMER_START();
        }
    }
    else
    {

    }
}
#endif

#if (__HR_SPO2_DET_SUPPORT__)
//HR use ir rawdata
void gh30x_hr_spo2_start(void)
{
    GS16 nRes = HBD_RET_GENERIC_ERROR;
    GU8 index = 0;

    HAL_GH30X_FIFO_INT_TIMEOUT_TIMER_STOP();

    HBD_SetFifoThrCnt(FIFO_THR_CONFIG_TYPE_SPO2, __HR_SPO2_FIFO_THR_CNT_CONFIG__);

#if (__HBA_ENABLE_WEARING__)
    GF32 wearing_config_array[3] = {0, 0, 0};
    HBD_EnableWearing(wearing_config_array);
#endif

#if (__HBD_ALGORITHM_USE_DYN_MEM__)
    gh30x_malloc_memory();
#endif

    if (g_uchGh30xRunMethod == 1)//call without adt start need load config
    {
        gh30x_Load_new_config(spo2_reg_config_array, spo2_reg_config_array_len);
    }
    nRes = HBD_HrSpO2DetectStart();

    if (nRes != HBD_RET_OK)  // start
    {
        EXAMPLE_DEBUG_LOG_L1("gh30x hb spo2 start error : %d\r\n", nRes);
        for (index = 0; index < __RETRY_MAX_CNT_CONFIG__; index ++) // retry
        {
            if (HBD_SimpleInit(&gh30x_init_config) == HBD_RET_OK)
            {
                if (gh30x_Load_new_config(spo2_reg_config_array, spo2_reg_config_array_len) == HBD_RET_OK)
                {
                    if (HBD_HrSpO2DetectStart())
                    {
                        EXAMPLE_DEBUG_LOG_L1("gh30x hb spo2 start retry success\r\n");
                        HAL_GH30X_FIFO_INT_TIMEOUT_TIMER_START();
                        break;
                    }
                }
            }
        }
    }
    else
    {
        HAL_GH30X_FIFO_INT_TIMEOUT_TIMER_START();
    }
}
#endif

#if (__GH30X_COMMUNICATION_INTERFACE__ == GH30X_COMMUNICATION_INTERFACE_SPI)

/// i2c exchange to spi for gh30x wrtie
uint8_t gh30x_i2c_write_exchange_to_spi(uint8_t device_id, const uint8_t write_buffer[], uint16_t length)
{
    uint8_t ret = GH30X_EXAMPLE_OK_VAL;

    if ((length == 3) && (write_buffer[0] == 0xDD) && (write_buffer[1] == 0xDD))
    {
        hal_gh30x_spi_cs_set_low();
        hal_gh30x_spi_write(&write_buffer[2], 1);
        hal_gh30x_spi_cs_set_high();
        HBD_DelayUs(10);
    }
    else
    {
        uint8_t spi_write_buffer[5] = {0};
        uint16_t spi_real_len = length - 2;

        hal_gh30x_spi_cs_set_low();
        spi_write_buffer[0] = 0xF0;
        spi_write_buffer[1] = write_buffer[0];
        spi_write_buffer[2] = write_buffer[1];
        spi_write_buffer[3] = GET_HIGH_BYTE_FROM_WORD(spi_real_len);
        spi_write_buffer[4] = GET_LOW_BYTE_FROM_WORD(spi_real_len);
        hal_gh30x_spi_write(spi_write_buffer, 5);
        hal_gh30x_spi_write(&write_buffer[2], spi_real_len);
        HBD_DelayUs(20);
        hal_gh30x_spi_cs_set_high();
        HBD_DelayUs(10);
    }
    return ret;
}

/// i2c exchange to spi for gh30x read
uint8_t gh30x_i2c_read_exchange_to_spi(uint8_t device_id, const uint8_t write_buffer[], uint16_t write_length, uint8_t read_buffer[], uint16_t read_length)
{
    uint8_t ret = GH30X_EXAMPLE_OK_VAL;

    if (write_length == 2)
    {
        uint8_t spi_write_buffer[3] = {0};
        hal_gh30x_spi_cs_set_low();
        spi_write_buffer[0] = 0xF0;
        spi_write_buffer[1] = write_buffer[0];
        spi_write_buffer[2] = write_buffer[1];
        hal_gh30x_spi_write(spi_write_buffer, 3);
        HBD_DelayUs(20);
        hal_gh30x_spi_cs_set_high();
        HBD_DelayUs(10);
        hal_gh30x_spi_cs_set_low();
        spi_write_buffer[0] = 0xF1;
        hal_gh30x_spi_write(spi_write_buffer, 1);
        hal_gh30x_spi_read(read_buffer, read_length);
        HBD_DelayUs(20);
        hal_gh30x_spi_cs_set_high();
        HBD_DelayUs(10);
    }

    return ret;
}

#endif

#if (__SYSTEM_TEST_SUPPORT__)
/// system test comm check
uint8_t gh30x_systemtest_comm_check(void)
{
    uint8_t ret = 0;
    ROMAHBD_Interfcae gh30x_wr_i;

    gh30x_wr_i.WR_Fun = (Write_fun)HBD_I2cWriteReg;
    gh30x_wr_i.RD_Fun = (Read_fun)HBD_I2cReadReg;

    HBD_I2cSendCmd(0xC0);
    HBD_DelayUs(600);
    ret=HBDTEST_Comm_Check(&gh30x_wr_i);
    HBD_I2cSendCmd(0xC4);
    HBD_DelayUs(600);

    return ret;
}

/// system test otp check
uint8_t gh30x_systemtest_otp_check(void)
{
    uint8_t ret = 0;
    uint8_t systemtest_otp_buffer[64] = {0};
    ROMAHBD_Interfcae gh30x_wr_i;

    gh30x_wr_i.WR_Fun = (Write_fun)HBD_I2cWriteReg;
    gh30x_wr_i.RD_Fun = (Read_fun)HBD_I2cReadReg;

    HBD_I2cSendCmd(0xC0);
    HBD_DelayUs(600);
    ret = HBDTEST_OTP_Check(&gh30x_wr_i, systemtest_otp_buffer);
    HBD_I2cSendCmd(0xC4);
    HBD_DelayUs(600);

    return ret;
}

/// system test os start
void gh30x_systemtest_os_start(GU8 led_num)
{
    const ST_REGISTER *system_test_reg_config_ptr = NULL;
    GU16 system_test_reg_config_len = 0;

    HAL_GH30X_FIFO_INT_TIMEOUT_TIMER_STOP();
    // load config
    if (led_num == 2)
    {
        system_test_reg_config_ptr = systemtest_led2_reg_config_array;
        system_test_reg_config_len = systemtest_led2_reg_config_array_len;
    }
    else if (led_num == 1)
    {
        system_test_reg_config_ptr = systemtest_led1_reg_config_array;
        system_test_reg_config_len = systemtest_led1_reg_config_array_len;
    }
    else // fixed to 0
    {
        system_test_reg_config_ptr = systemtest_led0_reg_config_array;
        system_test_reg_config_len = systemtest_led0_reg_config_array_len;
    }

    gh30x_Load_new_config(system_test_reg_config_ptr, system_test_reg_config_len);
    HBD_FifoConfig(0, HBD_FUNCTIONAL_STATE_DISABLE);
    HBD_FifoConfig(1, HBD_FUNCTIONAL_STATE_DISABLE);
    #if (__HBD_ALGORITHM_USE_DYN_MEM__)
    gh30x_malloc_memory();
    #endif

    Gh30xHBD_SampleStart();

    HBD_FifoConfig(0, HBD_FUNCTIONAL_STATE_ENABLE);
    HBD_FifoConfig(1, HBD_FUNCTIONAL_STATE_ENABLE);
}

/// system test os calc
uint8_t gh30x_systemtest_os_calc(uint8_t led_num)
{
    uint8_t ret = 0xFF;

    static int32_t systemtest_rawdata_buffer[__SYSTEM_TEST_DATA_CNT_CONFIG__] = {0};
    static uint8_t systemtest_rawdata_buffer_index = 0;
    static uint8_t systemtest_last_led_num = 0;

    if (systemtest_last_led_num != led_num)
    {
        systemtest_rawdata_buffer_index = 0;
        systemtest_last_led_num = led_num;
    }

    systemtest_rawdata_buffer[systemtest_rawdata_buffer_index] = HBD_I2cReadRawdataReg(g_usReadRawdataRegList[0]) & 0x0001FFFF;
    systemtest_rawdata_buffer_index++;
    EXAMPLE_DEBUG_LOG_L1("got rawdata %d,the index is %d.\r\n",systemtest_rawdata_buffer[systemtest_rawdata_buffer_index-1],systemtest_rawdata_buffer_index);

    if (systemtest_rawdata_buffer_index >= __SYSTEM_TEST_DATA_CNT_CONFIG__)
    {
        if(goodix_system_test_mode & 0x1)
        {
            EXAMPLE_DEBUG_LOG_L1("check rawdata result\n");
            const HBDTEST_ROMAHBData *hbdatalst[]={&systemtest_led0_os_result,&systemtest_led1_os_result,&systemtest_led2_os_result};
            ret = HBDTEST_Check_Rawdata_Noise((int *)systemtest_rawdata_buffer, __SYSTEM_TEST_DATA_CNT_CONFIG__, hbdatalst[led_num]);
        }
        else if(goodix_system_test_mode & 0x2)
        {
            EXAMPLE_DEBUG_LOG_L1("check ctr result\n");
            HBDTEST_ROMALEDCheckData *hbdatalst[]={&led0std,&led1std,&led2std};
            ret = HBDTEST_Check_CTRandNoise((int *)systemtest_rawdata_buffer,__SYSTEM_TEST_DATA_CNT_CONFIG__,hbdatalst[led_num]);
            hbdatalst[led_num]->_res._flag|=0x1;
            EXAMPLE_DEBUG_LOG_L1("res is %f,%d\n",hbdatalst[led_num]->_res._CTR,hbdatalst[led_num]->_res._flag);
        }
        else if(goodix_system_test_mode & 0x4)
        {
            EXAMPLE_DEBUG_LOG_L1("check leak result\n");
            HBDTEST_ROMALEDCheckData *hbdatalst[]={&led0std,&led1std,&led2std};
            ret = HBDTEST_Check_LeakandRatio((int *)systemtest_rawdata_buffer,__SYSTEM_TEST_DATA_CNT_CONFIG__,hbdatalst[led_num]);
        }
        else
        {
            EXAMPLE_DEBUG_LOG_L1("test error");
            ret = EN_SELFTST_ERROR_ALL;
        }
        systemtest_rawdata_buffer_index = 0;
    }

    return ret;
}

//analysis the config and get config
void gh30x_systemtest_param_set(uint8_t led_num,HBDTEST_ROMAConfigParam* param)
{
    const ST_REGISTER *array_list[3] = {&systemtest_led0_reg_config_array[0], &systemtest_led1_reg_config_array[0], \

        &systemtest_led2_reg_config_array[0]};
    const uint8_t lenth_list[3] = {systemtest_led0_reg_config_array_len, systemtest_led1_reg_config_array_len, \
        systemtest_led2_reg_config_array_len};

    const ST_REGISTER *reg_config_array = array_list[led_num];

    uint8_t lenth = lenth_list[led_num];
    int temp[5]; // 118,11a,84,136,180
    unsigned char flag = 0;

    for (int j = 0; j < lenth; j++)
    {
        if (reg_config_array[j].usRegAddr == 0x118)
        {
            temp[0] =reg_config_array[j].usRegData;
            flag |= 0x1;
        }
        if (reg_config_array[j].usRegAddr == 0x11a)
        {
            temp[1] = reg_config_array[j].usRegData;
            flag |= 0x2;
        }
        if (reg_config_array[j].usRegAddr == 0x84)
        {
            temp[2] = reg_config_array[j].usRegData;
            flag |= 0x4;
        }
        if (reg_config_array[j].usRegAddr == 0x136)
        {
            temp[3] = reg_config_array[j].usRegData;
            flag |= 0x8;
        }
        if (reg_config_array[j].usRegAddr == 0x180)
        {
            temp[4] = reg_config_array[j].usRegData;
            flag |= 0x10;
        }
//      if(flag==31)
//      {
//        break;
//      }
    }

    unsigned index = 0;
    unsigned char map = temp[2] & 0x1c0;
    unsigned char mapobj[][3] = {{0,1,2}, {0,2,1}, {1,0,2}, {1,2,0}, {2,0,1}, {2,1,0}};

    for (int j = 0; j < 3; j++)
    {
        if (temp[2] & (1 << j))
        {
            index = j;
            break;
        }
    }
    index = mapobj[map][index];

    int16_t resismap[8] = {100, 200, 380, 700, 1000, 1480, 1960, 2440};
    float currmap[8] = {20.0, 40, 60, 80, 100, 120, 140, 160};

    switch (index)
    {
        case 0:
            param->_ledResis = resismap[(temp[3] & 0x70) >> 4];
            param->_ledCurr = currmap[(temp[4] & 0xe0) >> 5] * (float)(temp[0] & 0xff) / 255.0f;
            break;
        case 1:
            param->_ledResis = resismap[(temp[3] & 0x380) >> 7];
            param->_ledCurr = currmap[(temp[4] & 0xe0) >> 5] * (float)((temp[0] & 0xff00) >> 8) / 255.0f;
            break;
        case 2:
            param->_ledResis = resismap[(temp[3] & 0x1c00) >> 10];
            param->_ledCurr = currmap[(temp[4] & 0xe0) >> 5] * (float)(temp[1] & 0xff) / 255.0f;
            break;
        default:
            EXAMPLE_DEBUG_LOG_L1("analysis index err %d\n", index);
            break;
    }
    EXAMPLE_DEBUG_LOG_L1("analysis res is %d,%f,%d\n", led_num, param->_ledCurr, param->_ledResis);
}
#endif


//=============================================================================================================================
//drv_lib global variables
//=============================================================================================================================
GU16 g_usIrqPluseWidth = 20; //default irq width 20us
EM_HBD_FUNCTIONAL_STATE g_emHbFifoEnableFlag = HBD_FUNCTIONAL_STATE_ENABLE;
EM_HBD_FUNCTIONAL_STATE g_emHrvFifoEnableFlag = HBD_FUNCTIONAL_STATE_ENABLE;
EM_HBD_FUNCTIONAL_STATE g_emBpfFifoEnableFlag = HBD_FUNCTIONAL_STATE_ENABLE;
EM_HBD_FUNCTIONAL_STATE g_emSpo2FifoEnableFlag = HBD_FUNCTIONAL_STATE_ENABLE;

GU16 g_usReadRawdataRegList[2] = {0};
GU8 g_uchNeedReadRawdataCount = 0;
GU8 g_uchPackageSN = 0;
GU8 g_uchCloseHBAlgo = 0;
GU8 g_uchCloseHBAlgoFlag = 0;
GU8 g_chWearIsEnableFlag = 0;
GU8 g_uchHbdStarted = HBD_START_STATUS_OFF;
GU8 g_uchHbdInited = HBD_INIT_STATUS_NO_INIT;
GU8 g_uchI2cDeviceId = HBD_I2C_DEVICE_ID_BASE;
//ST_ANALOG_REG_LIST g_stHbRegList;
//ST_ANALOG_REG_LIST g_stAdtRegList;
GU8 g_uchAdtHasStarted = 0;
//static GU8 g_uchReadFifoBuffArr[HBD_READ_FIFO_BYTE_NUM] = {0xFF};
static GU32 g_uchLastReadFifoCnt = 0;

//resume state flag,justice could read or write reg
GU8 g_uchWrRegflag = 1;

#if (HBD_CTRL_LIB_DEV_VERSION == HBD_CTRL_LIB_FOR_BAND)
const static GS8 g_szHbdCtrlVersionStrArr[] = {'L','i','b','H','B','C','_','B','a','n','d','a','n','d','E','p','_','v', \
                                        0x30+HBD_CTRL_MAJOR_VERSION_NUMBER, '.', 0x30+HBD_CTRL_MINOR_VERSION_NUMBER, \
                                        '.', 0x30+(HBD_CTRL_REVISION_VERSION_NUMBER/10), '.', 0x30+(HBD_CTRL_REVISION_VERSION_NUMBER%10), '\0'};
#elif (HBD_CTRL_LIB_DEV_VERSION == HBD_CTRL_LIB_FOR_EP)
const static GS8 g_szHbdCtrlVersionStrArr[] = {'L','i','b','H','B','C','_','E','p','_','v', \
                                        0x30+HBD_CTRL_MAJOR_VERSION_NUMBER, '.', 0x30+HBD_CTRL_MINOR_VERSION_NUMBER, '.', \
                                        0x30+(HBD_CTRL_REVISION_VERSION_NUMBER/10), '.', 0x30+(HBD_CTRL_REVISION_VERSION_NUMBER%10), '\0'};
#elif (HBD_CTRL_LIB_DEV_VERSION == HBD_CTRL_LIB_FOR_IN_EAR_EP)
const static GS8 g_szHbdCtrlVersionStrArr[] = {'L','i','b','H','B','C','_','I','n','e','a','r','E','p','_','v', \
                                        0x30+HBD_CTRL_MAJOR_VERSION_NUMBER, '.', 0x30+HBD_CTRL_MINOR_VERSION_NUMBER, '.', \
                                        0x30+(HBD_CTRL_REVISION_VERSION_NUMBER/10), '.', 0x30+(HBD_CTRL_REVISION_VERSION_NUMBER%10), '\0'};
#elif (HBD_CTRL_LIB_DEV_VERSION == HBD_CTRL_LIB_FOR_FINGER)
const static GS8 g_szHbdCtrlVersionStrArr[] = {'L','i','b','H','B','C','_','F','i','n','g','e','r', \
                                        0x30+HBD_CTRL_MAJOR_VERSION_NUMBER, '.', 0x30+HBD_CTRL_MINOR_VERSION_NUMBER, '.', \
                                        0x30+(HBD_CTRL_REVISION_VERSION_NUMBER/10), '.', 0x30+(HBD_CTRL_REVISION_VERSION_NUMBER%10), '\0'};

#endif
#if 0
ST_FIFO_THR_CONFIG g_stFifoThrConfigList[HBD_FIFO_THR_CONFIG_NUM] = {{0, HRV_SAMPLING_RATE_SETTING}, /* hrv */
                                                                     {0, HB_SAMPLING_RATE_SETTING},  /* hb */
                                                                     {0, HSM_SAMPLING_RATE_SETTING}, /* hsm */
                                                                     {0, BPF_SAMPLING_RATE_SETTING}, /* bpf */
                                                                     {0, PFA_SAMPLING_RATE_SETTING},  /* pfa */
                                                                     {0, SPO2_SAMPLING_RATE_SETTING}  /* spo2 */};
#endif
/* otp trim crc8 array. */
static const GU8 g_uchOtpTrimCrc8TabArr[] =
{
    0x00, 0x31, 0x62, 0x53, 0xc4, 0xf5, 0xa6, 0x97, 0xb9, 0x88, 0xdb, 0xea, 0x7d, 0x4c, 0x1f, 0x2e,
    0x43, 0x72, 0x21, 0x10, 0x87, 0xb6, 0xe5, 0xd4, 0xfa, 0xcb, 0x98, 0xa9, 0x3e, 0x0f, 0x5c, 0x6d,
    0x86, 0xb7, 0xe4, 0xd5, 0x42, 0x73, 0x20, 0x11, 0x3f, 0x0e, 0x5d, 0x6c, 0xfb, 0xca, 0x99, 0xa8,
    0xc5, 0xf4, 0xa7, 0x96, 0x01, 0x30, 0x63, 0x52, 0x7c, 0x4d, 0x1e, 0x2f, 0xb8, 0x89, 0xda, 0xeb,
    0x3d, 0x0c, 0x5f, 0x6e, 0xf9, 0xc8, 0x9b, 0xaa, 0x84, 0xb5, 0xe6, 0xd7, 0x40, 0x71, 0x22, 0x13,
    0x7e, 0x4f, 0x1c, 0x2d, 0xba, 0x8b, 0xd8, 0xe9, 0xc7, 0xf6, 0xa5, 0x94, 0x03, 0x32, 0x61, 0x50,
    0xbb, 0x8a, 0xd9, 0xe8, 0x7f, 0x4e, 0x1d, 0x2c, 0x02, 0x33, 0x60, 0x51, 0xc6, 0xf7, 0xa4, 0x95,
    0xf8, 0xc9, 0x9a, 0xab, 0x3c, 0x0d, 0x5e, 0x6f, 0x41, 0x70, 0x23, 0x12, 0x85, 0xb4, 0xe7, 0xd6,
    0x7a, 0x4b, 0x18, 0x29, 0xbe, 0x8f, 0xdc, 0xed, 0xc3, 0xf2, 0xa1, 0x90, 0x07, 0x36, 0x65, 0x54,
    0x39, 0x08, 0x5b, 0x6a, 0xfd, 0xcc, 0x9f, 0xae, 0x80, 0xb1, 0xe2, 0xd3, 0x44, 0x75, 0x26, 0x17,
    0xfc, 0xcd, 0x9e, 0xaf, 0x38, 0x09, 0x5a, 0x6b, 0x45, 0x74, 0x27, 0x16, 0x81, 0xb0, 0xe3, 0xd2,
    0xbf, 0x8e, 0xdd, 0xec, 0x7b, 0x4a, 0x19, 0x28, 0x06, 0x37, 0x64, 0x55, 0xc2, 0xf3, 0xa0, 0x91,
    0x47, 0x76, 0x25, 0x14, 0x83, 0xb2, 0xe1, 0xd0, 0xfe, 0xcf, 0x9c, 0xad, 0x3a, 0x0b, 0x58, 0x69,
    0x04, 0x35, 0x66, 0x57, 0xc0, 0xf1, 0xa2, 0x93, 0xbd, 0x8c, 0xdf, 0xee, 0x79, 0x48, 0x1b, 0x2a,
    0xc1, 0xf0, 0xa3, 0x92, 0x05, 0x34, 0x67, 0x56, 0x78, 0x49, 0x1a, 0x2b, 0xbc, 0x8d, 0xde, 0xef,
    0x82, 0xb3, 0xe0, 0xd1, 0x46, 0x77, 0x24, 0x15, 0x3b, 0x0a, 0x59, 0x68, 0xff, 0xce, 0x9d, 0xac,
};


/* i2c operation api. */
static pfnI2cWrite g_pI2cWriteFunc = HBD_PTR_NULL;
static pfnI2cRead g_pI2cReadFunc = HBD_PTR_NULL;
static pfnDelayUs g_pDelayUsFunc = HBD_PTR_NULL;   // delay us function ptr

//static pfnAlgoMemoryInitHookFunc g_pAlgoMemoryInitHookFunc;
static pfnAlgoMemoryDeinitHookFunc g_pAlgoMemoryDeinitHookFunc;
//static pfnAlgoInitHookFunc g_pAlgoInitHookFunc;
static pfnAlgoCalculateHookFunc g_pAlgoCalculateHookFunc;
static pfnAlgoDeinitHookFunc g_pAlgoDeinitHookFunc;
//=============================================================================================================================
//wear confirm global variables
//=============================================================================================================================
static GU16 g_usWearStateCheckIndex = 0;
static GU32 g_unWearStateLowerThreshold = 0;
static GU32 g_unWearStateUpperThreshold = 0;
static GU32 g_unUnwearStateLowerThreshold = 0;
static GU32 g_unUnwearStateUpperThreshold = 0;

GU32 g_unWearStateDetectMsk = HBD_WEAR_STATE_CHECK_THR_MSK;
GU8  g_uchWearStateDetectchecksum = HBD_WEAR_STATE_CHECK_SUM;
GU8  g_uchWearStateDetectcheckThrsum = HBD_WEAR_STATE_CHECK_THR_SUM;
GU8  g_uchWearStateDetectMode = 0;
//=============================================================================================================================
//autoled global variables
//=============================================================================================================================

GU8 g_uchKeepLedWorking = 0;
ST_AUTO_LED_CONFIG g_stAutoLedConfig = {
                                        { { 60000, 20000, 58000, 25000, 30, HBD_FUNCTIONAL_STATE_DISABLE, 7/*Max Gain*/, 120/*Max 255*/, HBD_FUNCTIONAL_STATE_DISABLE },   /* channel 0 */
                                        { 60000, 20000, 58000, 25000, 30, HBD_FUNCTIONAL_STATE_DISABLE, 7/*Max Gain*/, 50/*Max 255*/, HBD_FUNCTIONAL_STATE_DISABLE } }, /* channel 1 */
                                          5 /* unBgStronglightThr */, 4 /* unBgWeakStronglightThr */, 6 /* unBgUpperThr */, 2/* unBgLowerThr */, };


GU32 g_unIdealTargetValueArr[2] = { 40000, 40000 };   /* Ideal Target Array */
GU32 g_unIdealTargetTrigArr[2] = { 20000, 20000 };

GF32 g_fIdealCalcRatio = 0.8;
GF32 g_fIdealTrigRatio = 0.3;
//GF32 g_fTriggerIdealAdjustRatio[2] = {0.1f, 0.1f};    /* Ideal Target trigger Ratio */
GF32 g_fMotionTrgIdealExtraRatio[2] = {0.1f, 0.1f};  /* Ideal Target trigger Extra Ratio,in motion case: Trigger ratio = g_fTriggerIdealAdjustRatio + g_fMotionTrgIdealExtraRatio*/
ST_IDEAL_ADJUST g_stIdealAdjust = {{{0, 0, 0},/* CH0 */
                                  {0, 0, 0}},15/*15s*/};/* Struct of Idealtaget adjusting, contain:Last trigger state, count, FirsttriggerFlag */

//GF32 g_fMotionIdealTargetRatio[2] = {0.8f, 0.8f};     /* Reduce Ideal Target to g_fMotionIdealTargetRatio * g_unIdealTargetValueArr*/
GU8 g_uchIpdAmpFactor[2] = {4, 5};                    /* Saturation, Calculated Ipd product factor */

//ST_RECURSION_CALC_STD g_stAutoLedAccInfo;                    /* G-sensor Calculate, std, max, min, avg */
ST_RECURSION_CALC_NOSTD g_stTrigCalcInfo[2];          /* For Trigger flag, max, min, avg */
ST_RECURSION_CALC_NOSTD g_stBufferBGInfo[2];          /* BG avg max min diff */

// PPG_saturation judge
ST_AUTO_LED_Sat   g_stPpgSatInfo = { 0 , {0,0} ,0  };
GU32 g_PpgSatThr[2] = {60000, 60000};


ST_BG_ADJUST g_stBGAdj ={{{0,0,0},{0,0,0}}, 3, 7, HBD_FUNCTIONAL_STATE_DISABLE, 64};/* Struct of BG weak strong light Adjust */

/* Motion */
ST_RAWDATA_AVG_IN_MOTIONIN g_CalcAvgRatioInMotion ={0.9f,0.1f}; /* Motion: RawdataAvg=Max*Maxratio + Min*Minratio */
GF32 gfMotionTrigRatio[2] = {0.0f, 0.0f};//In motion extra ratio of trigger target

/* For newdata int */
GU16 g_uchSoftLEDNewdataModeCNT = 0;                  /* For newdata int, Counter to Sample frequncy */

/* Current for NADT */
GU8 g_uchAutoLedCurrentArr[2] = { 0 };                /* Global Current Array for NADT, needing from yufei */

/* Write Register Confirm */
ST_AUTOLED_WRITE_REG_CHECK g_stAutoledWrtRegCheck = {5000/* Write Regsister confirm time*/, {5000, 5000}};

/* ----------global AutoLEDflag--------------*/
GU8 g_uchAutoLedErrorFlag = 0;                        /* AutoLed Result Flag */
GU8 g_uchAutoLedLastFlag = 0;                         /* last time autoled Flag, bit0-->channel0, bit1-->channel1 */

/* Sensor information */
ST_SAMPLE_INFO g_stSampleInfo;                        /* Sample Information */
const GU16 g_usGainArrEnum[8] = {100, 200, 400, 700, 1000, 1480, 1960, 2440};

GU8 g_uchAutoLedGainStartVal = 0x9B;//unused

/*--------Ending---------------*/

//=============================================================================================================================
//algm global variables
//=============================================================================================================================

//GS32 g_nCalibrationParameter[5] = {0};

#if (ENABLE_SPO2_ABNORMAL_STATE)
#define SPO2_RESULT_CHECK_BUF_LEN (10)
static GS32 g_nIRPIarr[SPO2_RESULT_CHECK_BUF_LEN] = {0};
static GS32 g_nRedPIarr[SPO2_RESULT_CHECK_BUF_LEN] = {0};
GU8 g_nSpo2AbnormalState = 0;
#endif

GU8 g_uchMemUseFlag =0;
GU8 g_uchAdtContinuousMode = 0;

GU8  g_uIRErrCnt = 0;
// resampling
GU16 usHbWithHrvSampleRate = HB_SAMPLING_RATE_SETTING;
GU16 usHbWithHrvSampleIndex = 0;
GU16 usHbWithHrvSampleCalcDiv = 1;
GU16 usHbWithHrvSampleReIndex = 0;

//HBA config
//GS32 g_sleep_flg = 0;
//GS32 g_nReferenceHbValue = 0;
GU8 g_uchHbaScenario = 0;
GU8 g_uchHbaTestMode = 0;

const GU8 uchConfigArrKeyCode[] =
{
    0x47, 0x6F, 0x6F, 0x69, 0x64, 0x78, 0x49, 0x4F, 0x54, 0x43, 0x6F, 0x6E, 0x66, 0x69, 0x67, 0x41, 0x72, 0x72, 0x4B, 0x65, 0x79, 0x43, 0x6F, 0x64, 0x65, 
};

/************************************************************************
*   Function Definitions
************************************************************************/

/**
 * @fn     void HBD_AlgoRegisterHookFunc(GS32 (*pAlgoMemoryInitHookFunc)(void* mem_addr, GS32 size),
 *                               void (*pAlgoMemoryDeinitHookFunc)(void),
 *                               GS32 (*pAlgoVersionHookFunc)(EMFunctionID function_id,GU8 version[100]),
 *                               void (*pAlgoInitHookFunc)(EMFunctionID function_id,GU8 buffer_len,GS32 *algo_param_buffer),
 *                               void (*pAlgoCalculateHookFunc)(EMFunctionID function_id,ST_ALGO_CALCULATE_INFO *algo_calc_info,ST_ALGO_RESULT *algo_result),
 *                               void (*pAlgoDeinitHookFunc)(EMFunctionID function_id))
 *
 * @brief  algo register hook function callback
 *
 * @attention   None
 *
 * @param[in]   pAlgoMemoryInitHookFunc      pointer to algo memory init hook function
 * @param[in]   pAlgoMemoryDeinitHookFunc    pointer to algo memory deinit hook function
 * @param[in]   pAlgoInitHookFunc            pointer to algo init hook function
 * @param[in]   pAlgoCalculateHookFunc       pointer to algo calculate hook function
 * @param[in]   pAlgoDeinitHookFunc          pointer to algo deinit hook function
 * @param[out]  None
 *
 * @return  None
 */
#if 0
void HBD_AlgoRegisterHookFunc(GS32 (*pAlgoMemoryInitHookFunc)(GS32* error_code),
                                 void (*pAlgoMemoryDeinitHookFunc)(void),
                                 void (*pAlgoInitHookFunc)(EMFunctionID function_id,GU8 buffer_len,GS32 *algo_param_buffer),
                                 void (*pAlgoCalculateHookFunc)(EMFunctionID function_id,ST_ALGO_CALCULATE_INFO *algo_calc_info,ST_ALGO_RESULT *algo_result),
                                 void (*pAlgoDeinitHookFunc)(EMFunctionID function_id))
{
    //memory
    g_pAlgoMemoryInitHookFunc = pAlgoMemoryInitHookFunc;
    g_pAlgoMemoryDeinitHookFunc = pAlgoMemoryDeinitHookFunc;
    //handle
    g_pAlgoInitHookFunc = pAlgoInitHookFunc;
    g_pAlgoCalculateHookFunc = pAlgoCalculateHookFunc;
    g_pAlgoDeinitHookFunc = pAlgoDeinitHookFunc;

}
#endif

/****************************************************************
* Description: Get CRC-8 data for Otp Trim
* Input:    uchDataArr: data array,
     uchDataLen: data array len,
* Return: CRC-8 value
******************************************************************/
static GU8 HBD_GetOtpTrimCRC(GU8 uchDataArr[], GS32 uchDataLen)
{
    GU8 uchCRCTabIndex;
    GU8 uchCrc8 = 0x00;
    GS32 uchIndex = 0;
    for (uchIndex = 0; uchIndex < uchDataLen; uchIndex++)
    {
        uchCRCTabIndex = (uchCrc8 ^ (uchDataArr[uchIndex])) & 0xFF;
        uchCrc8 = g_uchOtpTrimCrc8TabArr[uchCRCTabIndex];
    }
    return uchCrc8;
}

/**
 * @fn     GU8 HBD_RawdataCheck(GU32 unRawdata)
 *
 * @brief  rawdata check last byte in algo lib
 *
 * @attention   none
 *
 * @param[in]   unRawdata   single rawdata
 * @param[out]  None
 *
 * @return  errcode
 * @retval  0               check success
 * @retval  1               check failed
 */
#if 0
GU8 HBD_RawdataCheck(GU32 unRawdata)
{
    GU32 unRawdataTemp = HBD_RawdataCalc(unRawdata);
    return(unRawdataTemp != unRawdata);
}
#endif

/****************************************************************
* Description: delay n Us
* Input:    usUsCnt: count need to delay(Us),
* Return: None
******************************************************************/
void HBD_DelayUs(GU16 usUsCnt)
{
    if (HBD_PTR_NULL != g_pDelayUsFunc)
    {
        g_pDelayUsFunc(usUsCnt);
    }
    else
    {
        volatile GU8 uchIndex;
        while (usUsCnt--)
        {
            for(uchIndex = 0; uchIndex < HBD_DELAY_ONE_US_CNT; uchIndex++);
        }
    }
}

/****************************************************************
* Description: set delay function callback
* Input:  pDelayUsFunc:delay function callback
* Return: None
******************************************************************/
void HBD_SetDelayUsCallback (void (*pDelayUsFunc)(GU16 usUsec))
{
    g_pDelayUsFunc = pDelayUsFunc;
}

/***************** spi and i2c communication function list **************/

GU8 g_uchGh30xLastCmd;   //real chip cmd
volatile GU8 g_uchNewIntFlag;   //0: no INT   1: chip have output a new INT

/****************************************************************
* Description: send cmd to GH30x via i2c
* Output: uchCmd:i2c cmd
* Return: None
******************************************************************/
void HBD_I2cSendCmd(GU8 uchCmd)
{
    GU8 uchBuffArr[3] = {0};

    uchBuffArr[0] = HBD_GET_HIGH_BYTE_FROM_WORD(HBD_I2C_CMD_ADDR);
    uchBuffArr[1] = HBD_GET_LOW_BYTE_FROM_WORD(HBD_I2C_CMD_ADDR);
    uchBuffArr[2] = uchCmd;
    if (g_pI2cWriteFunc != HBD_PTR_NULL)
    {
        if (HBD_CMD_RESUME == uchCmd)
        {
            if ((HBD_CMD_SCAN == g_uchGh30xLastCmd) || (HBD_CMD_RESUME == g_uchGh30xLastCmd))
            {
                if (g_uchNewIntFlag > 1)  // new INT has came at Gh30x process period, we can not resume chip
                {

                }
                else
                {
                    g_pI2cWriteFunc(g_uchI2cDeviceId, uchBuffArr, 3);
                    //EXAMPLE_DEBUG_LOG_L2("[HBD_I2cSendCmd] 1-Set g_uchNewIntFlag 0, uchCmd:0x%x\r", uchCmd);
                    g_uchNewIntFlag = 0;  // if g_uchNewIntFlag was set when it be clean, next INT will be skil, mcu will respond repeat INT
                    HBD_DelayUs(HBD_SEND_CMD_DELAY_X_US);
                    g_uchGh30xLastCmd = uchCmd;
                }
            }
        }
        else
        {
            //EXAMPLE_DEBUG_LOG_L2("[HBD_I2cSendCmd] 2-Set g_uchNewIntFlag 0, uchCmd:0x%x\r", uchCmd);
            g_uchNewIntFlag = 0;
            g_pI2cWriteFunc(g_uchI2cDeviceId, uchBuffArr, 3);
            HBD_DelayUs(HBD_SEND_CMD_DELAY_X_US);
            g_uchGh30xLastCmd = uchCmd;
        }
    }
}

/****************************************************************
* Description: Check gh30x status, avoid reading registers under incorrect flow
* Output: None
* Return: None
******************************************************************/
void HBD_TryToWakeUp(void)
{
    if (HBD_CMD_IDLE == g_uchGh30xLastCmd)
    {
    }
    else if ((HBD_CMD_SCAN == g_uchGh30xLastCmd) || (HBD_CMD_RESUME == g_uchGh30xLastCmd))
    {
        if (0 == g_uchNewIntFlag)
        {
            EXAMPLE_DEBUG_LOG_L1("fatal error:  mcu is accessing register/fifo at GH30X sleep period!");
            while(1);    //fatal error:  mcu is accessing register/fifo at GH30X sleep period
        }
    }
    else
    {
        HBD_I2cSendCmd(HBD_CMD_IDLE); // stop
    }
}

/****************************************************************
* Description: write GH30x register via i2c
* Output:   usAddr:register addr
            usValue:register data
* Return: None
******************************************************************/
void HBD_I2cWriteReg(GU16 usAddr, GU16 usValue)
{
    GU8 uchBuffArr[4] = {0};

    HBD_TryToWakeUp();

    EXAMPLE_DEBUG_LOG_L2("[%s]usAddr = 0x%x, usValue = 0x%x\r\n", __FUNCTION__, usAddr, usValue);
    uchBuffArr[0] = HBD_GET_HIGH_BYTE_FROM_WORD(usAddr);
    uchBuffArr[1] = HBD_GET_LOW_BYTE_FROM_WORD(usAddr);
    uchBuffArr[2] = HBD_GET_HIGH_BYTE_FROM_WORD(usValue);
    uchBuffArr[3] = HBD_GET_LOW_BYTE_FROM_WORD(usValue);
    if (g_pI2cWriteFunc != HBD_PTR_NULL)
    {
        g_pI2cWriteFunc(g_uchI2cDeviceId, uchBuffArr, 4);
    }
}

/****************************************************************
* Description: write GH30x register via i2c
* Output:   usAddr:register addr
            usValue:register data
* Return: None
******************************************************************/
void HBD_I2cWriteRawdataReg(GU16 usAddr, GU32 unValue)
{
    GU8 uchBuffArr[6] = {0};

    HBD_TryToWakeUp();

    uchBuffArr[0] = HBD_GET_HIGH_BYTE_FROM_WORD(usAddr);
    uchBuffArr[1] = HBD_GET_LOW_BYTE_FROM_WORD(usAddr);
    uchBuffArr[2] = HBD_GET_BYTE1_FROM_DWORD(unValue);
    uchBuffArr[3] = HBD_GET_BYTE0_FROM_DWORD(unValue);
    uchBuffArr[4] = HBD_GET_BYTE3_FROM_DWORD(unValue);
    uchBuffArr[5] = HBD_GET_BYTE2_FROM_DWORD(unValue);

    if (g_pI2cWriteFunc != HBD_PTR_NULL)
    {
        g_pI2cWriteFunc(g_uchI2cDeviceId, uchBuffArr, 6);
    }
}

/****************************************************************
* Description: read GH30x register via i2c
* Output: usAddr:register addr
* Return: register data
******************************************************************/
GU16 HBD_I2cReadReg(GU16 usAddr)
{
    GU8 uchCmdBuffArr[2] = {0};
    GU8 uchReadBuffArr[2] = {0};

    HBD_TryToWakeUp();


    uchCmdBuffArr[0] = HBD_GET_HIGH_BYTE_FROM_WORD(usAddr);
    uchCmdBuffArr[1] = HBD_GET_LOW_BYTE_FROM_WORD(usAddr);
    if (g_pI2cReadFunc != HBD_PTR_NULL)
    {
        g_pI2cReadFunc(g_uchI2cDeviceId, uchCmdBuffArr, 2, uchReadBuffArr, 2);
    }
    return HBD_MAKEUP_WORD(uchReadBuffArr[0], uchReadBuffArr[1]);
}

/****************************************************************
* Description: read GH30x rawdata register via i2c
* Output: usAddr:rawdata register addr
* Return: rawdata register data
******************************************************************/
GU32 HBD_I2cReadRawdataReg(GU16 usAddr)
{
    GU8 uchCmdBuffArr[2] = {0};
    GU8 uchReadBuffArr[4] = {0};

    HBD_TryToWakeUp();

    uchCmdBuffArr[0] = HBD_GET_HIGH_BYTE_FROM_WORD(usAddr);
    uchCmdBuffArr[1] = HBD_GET_LOW_BYTE_FROM_WORD(usAddr);
    if (g_pI2cReadFunc != HBD_PTR_NULL)
    {
        g_pI2cReadFunc(g_uchI2cDeviceId, uchCmdBuffArr, 2, uchReadBuffArr, 4);
    }
    return HBD_MAKEUP_DWORD(0, uchReadBuffArr[3], uchReadBuffArr[0], uchReadBuffArr[1]);
}



void HBD_I2cWriteRegBitField(GU16 usRegAddr, GU8 uchLsb, GU8 uchMsb, GU16 usValue)
{
    GU16 usMakData = ((((GU16)0x0001) << (uchMsb - uchLsb + 1)) - 1) << uchLsb;
    GU16 usRegData = 0;
    HBD_TryToWakeUp();
    usRegData = HBD_I2cReadReg(usRegAddr);
    GH30X_VAL_CLEAR_BIT(usRegData, usMakData);
    GH30X_VAL_SET_BIT(usRegData, (usValue << uchLsb) & usMakData);
    HBD_I2cWriteReg(usRegAddr, usRegData);
}

GU16 HBD_I2cReadRegBitField(GU16 usRegAddr, GU8 uchLsb, GU8 uchMsb)
{
    GU16 usMakData = ((((GU16)0x0001) << (uchMsb - uchLsb + 1)) - 1) << uchLsb;
    GU16 usRegData = 0;
    HBD_TryToWakeUp();
    usRegData = HBD_I2cReadReg(usRegAddr);
    GH30X_VAL_GET_BIT(usRegData, usMakData);
    usRegData >>= uchLsb;
    return usRegData;
}

/****************************************************************
* Description: read GH30x mutli rawdata via i2c
* Output: punDataBuffer: buffer for fifo rawdata
          usLen:fifo len
* Return: None
******************************************************************/
void HBD_I2cReadFifo(GU8 *puchDataBuffer, GU16 usLen)
{
    GU8 uchCmdBuffArr[2] = {0};
    GU8 ucReadTimes = 0;
    GU8 ucTailNum = 0;
    GU8 ucFifoBreakupNum = 240;
    GU16 i;

    HBD_TryToWakeUp();

    uchCmdBuffArr[0] = HBD_GET_HIGH_BYTE_FROM_WORD(HBD_FIFO_REG_ADDR);
    uchCmdBuffArr[1] = HBD_GET_LOW_BYTE_FROM_WORD(HBD_FIFO_REG_ADDR);
    if (g_pI2cReadFunc != HBD_PTR_NULL)
    {
        ucReadTimes = usLen / ucFifoBreakupNum;
        ucTailNum = usLen - (ucFifoBreakupNum * ucReadTimes);
        for (i = 0; i < ucReadTimes; ++i)
        {
            g_pI2cReadFunc(g_uchI2cDeviceId, uchCmdBuffArr, 2, &puchDataBuffer[i * ucFifoBreakupNum], ucFifoBreakupNum);
        }
        if (ucTailNum)
        {
            g_pI2cReadFunc(g_uchI2cDeviceId, uchCmdBuffArr, 2, &puchDataBuffer[i * ucFifoBreakupNum], ucTailNum); // fifo one data len :3 bytes
        }
    }
}

/****************************************************************
* Description: set i2c operation function
* Input:  uchI2cIdLowTwoBitsSelect: i2c low two bits addr selected, see EM_HBD_I2C_ID_SEL
          pI2cWriteFunc: i2c write function pointer
          pI2cReadFunc: i2c read function pointer
* Return: HBD_RET_PARAMETER_ERROR: function pointer parameter set null error,
          HBD_RET_OK: register success
******************************************************************/
GS8 HBD_SetI2cRW (GU8 uchI2cIdLowTwoBitsSelect,
                   GU8 (*pI2cWriteFunc)(GU8 uchDeviceId, const GU8 uchWriteBytesArr[], GU16 usWriteLen),
                   GU8 (*pI2cReadFunc)(GU8 uchDeviceId, const GU8 uchCmddBytesArr[], GU16 usCmddLen, GU8 uchReadBytesArr[], GU16 usMaxReadLen))
{
    GS8 chRet = HBD_RET_PARAMETER_ERROR;
    if ((pI2cWriteFunc != HBD_PTR_NULL) && (pI2cReadFunc != HBD_PTR_NULL) && (uchI2cIdLowTwoBitsSelect < HBD_I2C_ID_INVALID))
    {
        g_uchI2cDeviceId = HBD_I2C_DEVICE_ID_BASE | (uchI2cIdLowTwoBitsSelect << 1);
        g_pI2cWriteFunc = pI2cWriteFunc;
        g_pI2cReadFunc = pI2cReadFunc;
        chRet = HBD_RET_OK;
    }
    return chRet;
}

/*****************end of i2c communication function list **************/


/************************config array function list **************************/

void HBD_VirtualConfig(GU16 usAddr, GU16 usData)
{
//    static GU16 tmph =0;
    switch (usAddr)
    {
        //data for calibration parameters high bits in old setting
        case 0x2000:
        case 0x2002:
        case 0x2004:
        //data for calibration parameters high bits in new setting
        case 0x2030:
        case 0x2032:
        case 0x2034:
        case 0x2036:
        case 0x2038:

        //data for configure parameters high bits
        case 0x2100:
        case 0x2102:
        case 0x2104:
        case 0x2106:
        case 0x2108:
//        case 0x210A:
//        case 0x210C:
//        case 0x210E:
//        case 0x2110:
//        case 0x2112:
//        case 0x2114:
//        case 0x2116:
//        case 0x2118:
//        case 0x211A:
//        case 0x211C:
//        case 0x211E:
//        case 0x2120:  //for 0.7.0.3,spo2:1.8.7.11a
        //data for ACC Parameter
        case 0x2400:
        case 0x2402:
        case 0x2404:
        case 0x2406:

        // data for scene Parameter
        case 0x2500:
            //tmph = usData;
            break;

        //data for calibration parameters low bits in old setting
        case 0x2001:
            //g_nCalibrationParameter[0] = (GS32)HBD_MAKEUP_DWORD2(tmph,usData);
            break;
        case 0x2003:
            //g_nCalibrationParameter[1] = -1*((GS32)HBD_MAKEUP_DWORD2(tmph,usData));
            break;
        case 0x2005:
            //g_nCalibrationParameter[2] = (GS32)HBD_MAKEUP_DWORD2(tmph,usData);
            break;

        //goodix4318 update spo2 1.9.2.6.1a algm
        /*
         case 0x2031:
            g_siCaliCoeffA[0] = (GS32)HBD_MAKEUP_DWORD2(tmph,usData);
            break;
         case 0x2033:
            g_siCaliCoeffA[1] = (GS32)HBD_MAKEUP_DWORD2(tmph,usData);
            break;
         case 0x2035:
            g_siCaliCoeffA[2] = (GS32)HBD_MAKEUP_DWORD2(tmph,usData);
            break;
         case 0x2037:
            g_siCaliCoeffA[3] = (GS32)HBD_MAKEUP_DWORD2(tmph,usData);
            break;
         case 0x2039:
            g_siCaliCoeffB[0] = (GS32)HBD_MAKEUP_DWORD2(tmph,usData);g_uchUseNewSetting = 1;
            break;
         case 0x203B:
            g_siCaliCoeffB[1] = (GS32)HBD_MAKEUP_DWORD2(tmph,usData);
            break;
         case 0x203D:
            g_siCaliCoeffB[2] = (GS32)HBD_MAKEUP_DWORD2(tmph,usData);
            break;
         case 0x203F:
            g_siCaliCoeffB[3] = (GS32)HBD_MAKEUP_DWORD2(tmph,usData);
            break;
         case 0x2041:
            g_siCaliCoeffC[0] = (GS32)HBD_MAKEUP_DWORD2(tmph,usData);
            break;
         case 0x2043:
            g_siCaliCoeffC[1] = (GS32)HBD_MAKEUP_DWORD2(tmph,usData);
            break;
         case 0x2045:
            g_siCaliCoeffC[2] = (GS32)HBD_MAKEUP_DWORD2(tmph,usData);
            break;
         case 0x2047:
            g_siCaliCoeffC[3] = (GS32)HBD_MAKEUP_DWORD2(tmph,usData);
            break;
         case 0x2049:
            g_siCaliLineA[0] = (GS32)HBD_MAKEUP_DWORD2(tmph,usData);
            break;
         case 0x204B:
            g_siCaliLineA[1] = (GS32)HBD_MAKEUP_DWORD2(tmph,usData);
            break;
         case 0x204D:
            g_siCaliLineA[2] = (GS32)HBD_MAKEUP_DWORD2(tmph,usData);
            break;
         case 0x204F:
            g_siCaliLineA[3] = (GS32)HBD_MAKEUP_DWORD2(tmph,usData);
            break;
         case 0x2033:
            g_siCaliLineB[0] = (GS32)HBD_MAKEUP_DWORD2(tmph,usData);
            break;
         case 0x2035:
            g_siCaliLineB[1] = (GS32)HBD_MAKEUP_DWORD2(tmph,usData);
            break;
         case 0x2037:
            g_siCaliLineB[2] = (GS32)HBD_MAKEUP_DWORD2(tmph,usData);
            break;
         case 0x2039:
            g_siCaliLineB[3] = (GS32)HBD_MAKEUP_DWORD2(tmph,usData);
            break;
            */

         //data for calibration parameters low bits in new setting
         case 0x2031:
            //g_nCalibrationParameter[0] = (GS32)HBD_MAKEUP_DWORD2(tmph,usData);
            break;
         case 0x2033:
            //g_nCalibrationParameter[1] = (GS32)HBD_MAKEUP_DWORD2(tmph,usData);
            break;
         case 0x2035:
            //g_nCalibrationParameter[2] = (GS32)HBD_MAKEUP_DWORD2(tmph,usData);
            break;
         case 0x2037:
            //g_nCalibrationParameter[3] = (GS32)HBD_MAKEUP_DWORD2(tmph,usData);
            break;
         case 0x2039:
            //g_nCalibrationParameter[4] = (GS32)HBD_MAKEUP_DWORD2(tmph,usData);
            break;
        // config auto led gain start val
        case 0x2200:
            g_uchAutoLedGainStartVal = HBD_GET_LOW_BYTE_FROM_WORD(usData) & 0x17;//make sure with ruichong
            break;

        // RRI and HB module function enable
        case 0x2300:
           // g_nRRIAndHBModuleEnable[0] = usData;g_uchRRIAndHBSetting = 1;
            break;
        // ACC Parameter
        case 0x2401:
            //g_nACCParameter[0] = (GS32)HBD_MAKEUP_DWORD2(tmph,usData);
            break;
        case 0x2403:
            //g_nACCParameter[1] = (GS32)HBD_MAKEUP_DWORD2(tmph,usData);
            break;
        case 0x2405:
            //g_nACCParameter[2] = (GS32)HBD_MAKEUP_DWORD2(tmph,usData);
            break;
        case 0x2407:
            //g_nACCParameter[3] = (GS32)HBD_MAKEUP_DWORD2(tmph,usData); g_uchACCSetting = 1;
            break;

        // scene Parameter
        case 0x2501:
            //g_nsceneParameter = (GS32)HBD_MAKEUP_DWORD2(tmph,usData); g_nsceneSetting = 1;
            break;
        // short of breath Parameter
        case 0x2600:
            //g_nsbreathParameter[0] = usData;
            break;
        case 0x2601:
            //g_nsbreathParameter[1] = usData;g_nsbreathSetting = 1;
            break;
        case 0x2800:
            //g_HrvNeedlpl = usData;
            break;
        default:
            break;
    }

    if (usAddr >= 0x2A00 && usAddr <= 0x2A40)   //Config for AutoLedConfig
    {
        static GU16 tmph =0;
        switch (usAddr)
        {
            case 0X2A00:
                if (0 == HBD_GET_LOW_BYTE_FROM_WORD(usData))
                {
                    g_stAutoLedConfig.stAutoLedChannel[0].uchCurrentLimit = 0x7F;
                }
                else
                {
                    g_stAutoLedConfig.stAutoLedChannel[0].uchCurrentLimit = HBD_GET_LOW_BYTE_FROM_WORD(usData);
                }
                if (0 == HBD_GET_HIGH_BYTE_FROM_WORD(usData))
                {
                    g_stAutoLedConfig.stAutoLedChannel[1].uchCurrentLimit = 0x7F;
                }
                else
                {
                    g_stAutoLedConfig.stAutoLedChannel[1].uchCurrentLimit = HBD_GET_HIGH_BYTE_FROM_WORD(usData);
                }
                break;
            case 0x2A02:
            case 0x2A04:
            case 0x2A06:
            case 0x2A08:
            case 0x2A0A:
            case 0x2A0C:
            case 0x2A0E:
            case 0x2A10:
            case 0x2A12:
            case 0x2A14:
                        tmph = usData;
                break;
            case 0x2A03:
                g_stAutoLedConfig.stAutoLedChannel[0].unTrigerUpperThr = (GS32)HBD_MAKEUP_DWORD2(tmph,usData);
                break;
            case 0x2A05:
                g_stAutoLedConfig.stAutoLedChannel[0].unTrigerLowerThr = (GS32)HBD_MAKEUP_DWORD2(tmph,usData);
                break;
            case 0x2A07:
                g_stAutoLedConfig.stAutoLedChannel[0].unTargetUpperThr = (GS32)HBD_MAKEUP_DWORD2(tmph,usData);
                break;
            case 0x2A09:
                g_stAutoLedConfig.stAutoLedChannel[0].unTargetLowerThr = (GS32)HBD_MAKEUP_DWORD2(tmph,usData);
                break;
            case 0x2A0B:
                    g_stAutoLedConfig.stAutoLedChannel[0].unCompatibleTakeoffThr = (GS32)HBD_MAKEUP_DWORD2(tmph, usData);
                break;
            case 0x2A0D:
                g_stAutoLedConfig.stAutoLedChannel[1].unTrigerUpperThr = (GS32)HBD_MAKEUP_DWORD2(tmph,usData);
                break;
            case 0x2A0F:
                g_stAutoLedConfig.stAutoLedChannel[1].unTrigerLowerThr = (GS32)HBD_MAKEUP_DWORD2(tmph,usData);
                break;
            case 0x2A11:
                g_stAutoLedConfig.stAutoLedChannel[1].unTargetUpperThr = (GS32)HBD_MAKEUP_DWORD2(tmph,usData);
                break;
            case 0x2A13:
                g_stAutoLedConfig.stAutoLedChannel[1].unTargetLowerThr = (GS32)HBD_MAKEUP_DWORD2(tmph,usData);
                break;
            case 0x2A15:
                    g_stAutoLedConfig.stAutoLedChannel[1].unCompatibleTakeoffThr = (GS32)HBD_MAKEUP_DWORD2(tmph, usData);
                break;

            case 0x2A16:
                if (HBD_GET_LOW_BYTE_FROM_WORD(usData))
                {
                    g_stAutoLedConfig.stAutoLedChannel[0].emAutoledEnable = HBD_FUNCTIONAL_STATE_ENABLE;
                }
                else
                {
                    g_stAutoLedConfig.stAutoLedChannel[0].emAutoledEnable = HBD_FUNCTIONAL_STATE_DISABLE;
                }
                if (HBD_GET_HIGH_BYTE_FROM_WORD(usData))
                {
                    g_stAutoLedConfig.stAutoLedChannel[1].emAutoledEnable = HBD_FUNCTIONAL_STATE_ENABLE;
                }
                else
                {
                    g_stAutoLedConfig.stAutoLedChannel[1].emAutoledEnable = HBD_FUNCTIONAL_STATE_DISABLE;
                }
                break;

            case 0x2A17:
                g_stAutoLedConfig.uchBgUpperThr = HBD_GET_HIGH_BYTE_FROM_WORD(usData);
                g_stAutoLedConfig.uchgLowerThr = HBD_GET_LOW_BYTE_FROM_WORD(usData);
                break;

            case 0x2A18:
                if (HBD_GET_LOW_BYTE_FROM_WORD(usData) > 5)
                {
                   g_stAutoLedConfig.uchBgStrongLightThr = 0x5;
                }
                else
                {
                   g_stAutoLedConfig.uchBgStrongLightThr = HBD_GET_LOW_BYTE_FROM_WORD(usData);
                }
                break;

            case 0x2A19:
                /*
                if (HBD_GET_LOW_BYTE_FROM_WORD(usData))
                {
                    g_stAutoLedConfig.stAutoLedChannel[0].emAutoledSaveEnergyEnable = HBD_FUNCTIONAL_STATE_ENABLE;
                }
                else
                {
                    g_stAutoLedConfig.stAutoLedChannel[0].emAutoledSaveEnergyEnable = HBD_FUNCTIONAL_STATE_DISABLE;
                }
                if (HBD_GET_HIGH_BYTE_FROM_WORD(usData))
                {
                    g_stAutoLedConfig.stAutoLedChannel[1].emAutoledSaveEnergyEnable = HBD_FUNCTIONAL_STATE_ENABLE;
                }
                else
                {
                    g_stAutoLedConfig.stAutoLedChannel[1].emAutoledSaveEnergyEnable = HBD_FUNCTIONAL_STATE_DISABLE;
                }*/
                break;
            case 0x2A30:
            //    if (HBD_GET_LOW_BYTE_FROM_WORD(usData) > 15 || HBD_GET_LOW_BYTE_FROM_WORD(usData) < 5)
            //    {
            //        g_fTriggerIdealAdjustRatio[0] = 0.1f;
            //    }
            //    else{
            //        g_fTriggerIdealAdjustRatio[0] = (GF32)HBD_GET_LOW_BYTE_FROM_WORD(usData) / 100.0f;
            //    }
            //    if (HBD_GET_HIGH_BYTE_FROM_WORD(usData) > 15 || HBD_GET_HIGH_BYTE_FROM_WORD(usData) < 5)
            //    {
            //        g_fTriggerIdealAdjustRatio[1] = 0.1f;
            //    }
            //    else{
            //        g_fTriggerIdealAdjustRatio[1] = (GF32)HBD_GET_HIGH_BYTE_FROM_WORD(usData) / 100.0f;
            //    }
                break;
            case 0x2A31:
                if (HBD_GET_LOW_BYTE_FROM_WORD(usData) < 1 || HBD_GET_LOW_BYTE_FROM_WORD(usData) > 8)
                {
                    g_uchIpdAmpFactor[0] = 0x5; //min 1, max 7, otherwise set to 5
                }
                else
                {
                    g_uchIpdAmpFactor[0] = HBD_GET_LOW_BYTE_FROM_WORD(usData); //Ipd * factor,factor 5
                }
                if (HBD_GET_HIGH_BYTE_FROM_WORD(usData) < 1 || HBD_GET_HIGH_BYTE_FROM_WORD(usData) > 8)
                {
                    g_uchIpdAmpFactor[1] = 0x5; //min 1, max 7, otherwise set to 5
                }
                else
                {
                    g_uchIpdAmpFactor[1] = HBD_GET_HIGH_BYTE_FROM_WORD(usData); //Ipd * factor,factor 5
                }
                break;
            case 0x2A32:
                if (HBD_GET_LOW_BYTE_FROM_WORD(usData) > 10 || HBD_GET_LOW_BYTE_FROM_WORD(usData) < 3)
                {
                    g_fMotionTrgIdealExtraRatio[0] = 0.1f; // min 1, max 10, otherwise set to 5
                }
                else
                {
                    g_fMotionTrgIdealExtraRatio[0] = (GF32)HBD_GET_LOW_BYTE_FROM_WORD(usData) / 100.0f; // 0.05 default 5
                }
                if (HBD_GET_HIGH_BYTE_FROM_WORD(usData) > 10 || HBD_GET_HIGH_BYTE_FROM_WORD(usData) < 3)
                {
                    g_fMotionTrgIdealExtraRatio[1] = 0.1f; // min 1, max 10, otherwise set to 5
                }
                else
                {
                    g_fMotionTrgIdealExtraRatio[1] = (GF32)HBD_GET_HIGH_BYTE_FROM_WORD(usData) / 100.0f; // 0.05 default 5
                }
                break;
            case 0x2A33:
//                if (HBD_GET_LOW_BYTE_FROM_WORD(usData) > 89 || HBD_GET_LOW_BYTE_FROM_WORD(usData) < 60)
//                {
//                    g_fMotionIdealTargetRatio[0] = 0.8f;  // otherwise set to 0.8
//                }
//                else{
//                    g_fMotionIdealTargetRatio[0] = (GF32)HBD_GET_LOW_BYTE_FROM_WORD(usData) / 100.0f;  // 0.8 default 0.8*target
//                }
//                if (HBD_GET_HIGH_BYTE_FROM_WORD(usData) > 89 || HBD_GET_HIGH_BYTE_FROM_WORD(usData) < 59)
//                {
//                    g_fMotionIdealTargetRatio[1] = 0.8f;  // otherwise set to 0.8
//                }
//                else{
//                    g_fMotionIdealTargetRatio[1] = (GF32)HBD_GET_HIGH_BYTE_FROM_WORD(usData) / 100.0f;  // 0.8 default 0.8*target
//                }
                break;
            case 0x2A34:
                if (HBD_GET_LOW_BYTE_FROM_WORD(usData) > 30 || HBD_GET_LOW_BYTE_FROM_WORD(usData) < 7)
                {
                    g_stIdealAdjust.uchTriggerDuration = 0xF; // otherwise set to 15s
                }
                else
                {
                    g_stIdealAdjust.uchTriggerDuration = HBD_GET_LOW_BYTE_FROM_WORD(usData); // 9-25 pulse/min , -->15s
                }
                break;
            case 0x2A3B:
                if (HBD_GET_LOW_BYTE_FROM_WORD(usData) < 6)
                {
                    g_stBGAdj.uchBGRecoverGainDuration = 0x7;// otherwise set to 7
                }
                else
                {
                    g_stBGAdj.uchBGRecoverGainDuration = HBD_GET_LOW_BYTE_FROM_WORD(usData);// Highlight --> Normal,Duration 7s
                }
                if (HBD_GET_HIGH_BYTE_FROM_WORD(usData) < 3)
                {
                    g_stBGAdj.uchBGReduceGainDuration = 0x3;//otherwise set to 3
                }
                else
                {
                    g_stBGAdj.uchBGReduceGainDuration = HBD_GET_HIGH_BYTE_FROM_WORD(usData);// WeakHighlight Reduce Target 3s
                }
                break;
            case 0x2A3C:
                if ((HBD_GET_LOW_BYTE_FROM_WORD(usData)+HBD_GET_HIGH_BYTE_FROM_WORD(usData)) != 100)
                {
                    g_CalcAvgRatioInMotion.fMinRatio = 0.1f;
                    g_CalcAvgRatioInMotion.fMaxRatio = 0.9f;
                }
                else
                {
                    g_CalcAvgRatioInMotion.fMinRatio = (GF32)HBD_GET_LOW_BYTE_FROM_WORD(usData) / 100.0f;// MinRatio 10-->0.1
                    g_CalcAvgRatioInMotion.fMaxRatio = (GF32)HBD_GET_HIGH_BYTE_FROM_WORD(usData) / 100.0f;// MaxRatio 90-->0.9
                }
                break;
            case 0x2A3D:
                if (HBD_GET_LOW_BYTE_FROM_WORD(usData) < 4)
                {
                    g_stAutoledWrtRegCheck.unWrtRegThres = 0x5 * 1000;// otherwise set to 5
                }
                else
                {
                    g_stAutoledWrtRegCheck.unWrtRegThres = HBD_GET_LOW_BYTE_FROM_WORD(usData) * 1000;// write reg confirm time，default 5s
                }
                break;
            case 0x2A3E:
                g_uchKeepLedWorking = (GU8)HBD_GET_LOW_BYTE_FROM_WORD(usData);//Will not close led when value is 1
                break;
            case 0x2A3F:
                if (HBD_GET_LOW_BYTE_FROM_WORD(usData))
                {
                    g_stBGAdj.uchTurnOnNotReturnFail = HBD_FUNCTIONAL_STATE_ENABLE;
                }
                else
                {
                    g_stBGAdj.uchTurnOnNotReturnFail = HBD_FUNCTIONAL_STATE_DISABLE;
                }
                if (HBD_GET_HIGH_BYTE_FROM_WORD(usData))
                {
                    g_stBGAdj.uchTakeoffFactorAtBGhigher = (GU8)HBD_GET_HIGH_BYTE_FROM_WORD(usData);
                }
                else
                {
                    g_stBGAdj.uchTakeoffFactorAtBGhigher = 64;
                }
                break;
            case 0x2A40:
                //if (HBD_GET_LOW_BYTE_FROM_WORD(usData))
                //{
                    g_fIdealCalcRatio = (GU8)HBD_GET_LOW_BYTE_FROM_WORD(usData) * 1.0f / 100.0f;
                    g_fIdealCalcRatio = (g_fIdealCalcRatio > 0.8f ? 0.8f : g_fIdealCalcRatio);
                //}
                //else
                //{
                //    g_fIdealCalcRatio = 0.8;
                //}
                //if (HBD_GET_HIGH_BYTE_FROM_WORD(usData))
                //{
                    g_fIdealTrigRatio = (GU8)HBD_GET_HIGH_BYTE_FROM_WORD(usData) * 1.0f / 100.0f;
                    g_fIdealTrigRatio = (g_fIdealTrigRatio > g_fIdealCalcRatio ? g_fIdealCalcRatio : g_fIdealTrigRatio);
                //}
                //else
                //{
                //    g_fIdealTrigRatio = 0.3;
                //}
                break;
        }
    }

    if (usAddr >= 0x2B00 && usAddr <= 0x2BFF)   //Config for nadt
    {
#if (HBD_CTRL_LIB_DEV_VERSION == HBD_CTRL_LIB_FOR_BAND)
#if 0
        GS32 nNadtConfig[1] = {0};
        switch(usAddr)
        {
            case 0X2B00:
                nNadtConfig[0] = HBD_GET_LOW_BYTE_FROM_WORD(usData);      // g_byContinuousTurnRestartCnt
                //NADT_Control(NADT_CONFIG_TURNLIGHT_TYPE, nNadtConfig);
                break;
            case 0X2B01:
                nNadtConfig[0] = HBD_GET_LOW_BYTE_FROM_WORD(usData);      // g_byUnwearCheckCntDefault
                //NADT_Control(NADT_CONFIG_UNWEAR_TIME_TYPE, nNadtConfig);
                break;
            case 0X2B02:
                nNadtConfig[0] = usData;                                  // g_usUnwearByNoiseCheckTimeoutMax
                //NADT_Control(NADT_CONFIG_DETECT_TIMEOUT_TYPE, nNadtConfig);
                break;
            case 0X2B03:
                nNadtConfig[0] = HBD_GET_LOW_BYTE_FROM_WORD(usData);      // g_byUnWearLevel
                //NADT_Control(NADT_COFIG_UNWEAR_LEVEL_TYPE, nNadtConfig);
                break;
            case 0X2B04:
                nNadtConfig[0] = HBD_GET_LOW_BYTE_FROM_WORD(usData);      // g_byLiveDetectEnable
                //NADT_Control(NADT_CONFIG_LIVE_DETECT_EN_TYPE, nNadtConfig);
                break;
            case 0X2B05:
                nNadtConfig[0] = (GS16)usData;                            // g_nHeartRateLowThres
                //NADT_Control(NADT_CONFIG_HB_LOW_THR_TYPE, nNadtConfig);
                break;
            case 0X2B06:
                nNadtConfig[0] = (GS16)usData;                            // g_nHeartRateUpThres
                //NADT_Control(NADT_CONFIG_HB_HIGH_THR_TYPE, nNadtConfig);
                break;
            case 0X2B07:
                nNadtConfig[0] = (GS16)usData;                            // g_nGreenSignalLowThres
                //NADT_Control(NADT_CONFIG_GREEN_SIG_LOW_THR_TYPE, nNadtConfig);
                break;
            case 0X2B08:
                nNadtConfig[0] = (GS16)usData;                            // g_nGreenSignalUpThres
                //NADT_Control(NADT_CONFIG_GREEN_SIG_HIGH_THR_TYPE, nNadtConfig);
                break;
            case 0X2B09:
                nNadtConfig[0] = HBD_GET_LOW_BYTE_FROM_WORD(usData);      // g_byBaseRatioEnable
                //NADT_Control(NADT_CONFIG_BASE_RATIO_EN_TYPE, nNadtConfig);
                break;
            case 0X2B0A:
                nNadtConfig[0] = (GS16)usData;                            // g_nBaseRatioThres
                //NADT_Control(NADT_CONFIG_BASE_RATIO_THR_TYPE, nNadtConfig);
                break;
            case 0X2B0B:
                nNadtConfig[0] = HBD_GET_LOW_BYTE_FROM_WORD(usData);      // g_byLiveConfirmEnable
                //NADT_Control(NADT_CONFIG_LIVE_CONFIRM_EN_TYPE, nNadtConfig);
                break;
            case 0X2B0C:
                nNadtConfig[0] = (GS16)usData;                            // g_nInRedSignalLowThres
                //NADT_Control(NADT_CONFIG_IR_SIG_LOW_THR_TYPE, nNadtConfig);
                break;
            case 0X2B0D:
                nNadtConfig[0] = (GS16)usData;                            // g_nInRedSignalUpThres
                //NADT_Control(NADT_CONFIG_IR_SIG_HIGH_THR_TYPE, nNadtConfig);
                break;
            case 0X2B0E:
                nNadtConfig[0] = HBD_GET_LOW_BYTE_FROM_WORD(usData);      // g_byInRedCTRJudgeEnable
                //NADT_Control(NADT_CONFIG_CTR_EN_TYPE, nNadtConfig);
                break;
            case 0X2B0F:
                nNadtConfig[0] = (GS16)usData;                            // g_nInRedCTRThres
                //NADT_Control(NADT_CONFIG_CTR_THR_TYPE, nNadtConfig);
                break;
            case 0X2B10:
                nNadtConfig[0] = HBD_GET_LOW_BYTE_FROM_WORD(usData);      // g_byGainJudgeEnable
                //NADT_Control(NADT_CONFIG_GAIN_EN_TYPE, nNadtConfig);
                break;
            case 0X2B11:
                nNadtConfig[0] = (GS16)usData;                            // g_nGainStableTime
                //NADT_Control(NADT_CONFIG_GAIN_STABLE_TIME_TYPE, nNadtConfig);
                break;
            case 0X2B12:
                nNadtConfig[0] = (GS16)usData;                            // g_nGreenGainDiffThres
                //NADT_Control(NADT_CONFIG_GREEN_GAIN_THR_TYPE, nNadtConfig);
                break;
            case 0X2B13:
                nNadtConfig[0] = (GS16)usData;                            // g_nInRedGainDiffThres
                //NADT_Control(NADT_CONFIG_IR_GAIN_THR_TYPE, nNadtConfig);
                break;
            case 0X2B14:
                nNadtConfig[0] = HBD_GET_LOW_BYTE_FROM_WORD(usData);      // g_byCyclicityJudgeEnable
                //NADT_Control(NADT_CONFIG_PERIOD_EN_TYPE, nNadtConfig);
                break;
            case 0X2B15:
                nNadtConfig[0] = (GS16)usData;                           // g_nLightLowThres
                //NADT_Control(NADT_CONFIG_PERIOD_LOW_THR_TYPE, nNadtConfig);
                break;
            case 0X2B16:
                nNadtConfig[0] = (GS16)usData;                           // g_nLightUpThres
                //NADT_Control(NADT_CONFIG_PERIOD_HIGH_THR_TYPE, nNadtConfig);
                break;
            case 0X2B17:
                nNadtConfig[0] = (GS16)usData;                           // g_nCyclicityDiffThres
                //NADT_Control(NADT_CONFIG_PERIOD_DIFF_THR_TYPE, nNadtConfig);
                break;
            case 0X2B18:
                nNadtConfig[0] = (GS16)usData;                           // g_nTotalConfirmCnt
                //NADT_Control(NADT_CONFIG_TOTAL_CNT_TYPE, nNadtConfig);
                break;
            case 0X2B19:
                nNadtConfig[0] = (GS16)usData;                           // g_nUnwearCntThres
                //NADT_Control(NADT_CONFIG_UNWEAR_CNT_TYPE, nNadtConfig);
                break;
            case 0X2B1A:
                nNadtConfig[0] = (GS16)usData;                           // g_nAdtUnwearCnt
                //NADT_Control(NADT_CONFIG_ADT_WEAR_OFF_CNT_TYPE, nNadtConfig);
                break;

        }
#endif
#endif               //  #if (HBD_CTRL_LIB_DEV_VERSION == HBD_CTRL_LIB_FOR_BAND)
    }
}

/****************************************************************
* Description: translate config array
* Input:  uchDataBuffArr: need to translate array
          usLen: len of array
* Return: None
******************************************************************/
void HBD_ConfigArrTranslate(GU8 uchDataBuffArr[], GU16 usLen)
{
    GU16 usIndex  = 0;
    for (usIndex = 0; usIndex < usLen; usIndex++)
    {
        uchDataBuffArr[usIndex] ^=  uchConfigArrKeyCode[(usIndex % sizeof(uchConfigArrKeyCode))];
    }
}

/****************************************************************
* Description: Load config with mode.
* Input:  uchConfigArr: config array data,
          uchStoreMode: store mode, see EM_CONFIG_MODE,
          uchForceUpdateConfig: 1: need force update,0 :don't need,
* Return: HBD_RET_OK:load config success, HBD_RET_COMM_ERROR: load config error,
******************************************************************/
GS8 HBD_LoadConfigWithMode(GU8 uchConfigArr[], GU8 uchStoreMode, GU8 uchForceUpdateConfig)
{
    GS8 chRet = HBD_RET_OK;
    GU16 usIndex = 0;
    ST_REGISTER stConfigReg = {0};
    GU16 uchConfigArrIndexLen = uchConfigArr[HBD_CONFIG_ARR_LENGTH_INDEX] * sizeof(ST_REGISTER);

    for (usIndex = 0; usIndex < uchConfigArrIndexLen; usIndex += sizeof(ST_REGISTER)) // write GH30x reg.
    {
        stConfigReg.usRegAddr = HBD_MAKEUP_WORD(uchConfigArr[HBD_CONFIG_ARR_DATA_INDEX + usIndex], uchConfigArr[HBD_CONFIG_ARR_DATA_INDEX + usIndex + 1]);
        stConfigReg.usRegData = HBD_MAKEUP_WORD(uchConfigArr[HBD_CONFIG_ARR_DATA_INDEX + usIndex + 2], uchConfigArr[HBD_CONFIG_ARR_DATA_INDEX + usIndex + 3]);
        if (stConfigReg.usRegAddr < COMM_ALGORITHM_VIRTUAL_ADDR)
        {
            HBD_I2cWriteReg(HBD_GET_REG_REAL_ADRR(stConfigReg.usRegAddr), stConfigReg.usRegData);
            if ((stConfigReg.usRegAddr & HBD_REG_DONT_NEED_CHECK_BIT) != HBD_REG_DONT_NEED_CHECK_BIT) /* need check val. */
            {
                if (HBD_I2cReadReg(HBD_GET_REG_REAL_ADRR(stConfigReg.usRegAddr)) != stConfigReg.usRegData)
                {
                    chRet = HBD_RET_COMM_ERROR;
                    break;
                }
            }
        }
        else
        {
            HBD_VirtualConfig(stConfigReg.usRegAddr, stConfigReg.usRegData);
        }
    }
    return chRet;
}

/****************************************************************
* Description: load new GH30x config
* Input:    uchNewConfigArr: config array ,
            uchForceUpdateConfig: 1: need force update,0 :don't need,
* Return: HBD_RET_OK:load config success, HBD_RET_COMM_ERROR: load config error,
******************************************************************/
GS8 HBD_LoadNewConfig(GU8 uchNewConfigArr[], GU8 uchForceUpdateConfig)
{
    GS8 chRet = HBD_RET_OK;
    HBD_I2cSendCmd(HBD_CMD_IDLE);
    HBD_DelayUs(HBD_SEND_CMD_DELAY_X_US);
    HBD_ConfigArrTranslate(&uchNewConfigArr[HBD_CONFIG_ARR_DATA_INDEX], uchNewConfigArr[HBD_CONFIG_ARR_LENGTH_INDEX] * (sizeof(ST_REGISTER)));
    chRet = HBD_LoadConfigWithMode(uchNewConfigArr, CONFIG_ARR_FROM_COMPILE_LOAD, uchForceUpdateConfig);
    if (chRet == HBD_RET_OK)
    {
        HBD_StoreHbModeRegVal();
    }
    HBD_I2cSendCmd(HBD_CMD_SLEEP);
    HBD_DelayUs(HBD_SEND_CMD_DELAY_X_US);
    HBD_ConfigArrTranslate(&uchNewConfigArr[HBD_CONFIG_ARR_DATA_INDEX], uchNewConfigArr[HBD_CONFIG_ARR_LENGTH_INDEX] * (sizeof(ST_REGISTER))); // recover reg data
    return chRet;
}

/****************************************************************
* Description: load new GH30x reg config array
* Input:    stRegConfigArr: reg config array ,
            usRegConfigLen: reg config array len,
* Return: HBD_RET_OK:load config success, HBD_RET_COMM_ERROR: load config error,
******************************************************************/
GS8 HBD_LoadNewRegConfigArr(const ST_REGISTER stRegConfigArr[], GU16 usRegConfigLen)
{
    GS8 chRet = HBD_RET_OK;
    GU16 usIndex = 0;

    HBD_I2cSendCmd(HBD_CMD_IDLE);
    HBD_DelayUs(HBD_SEND_CMD_DELAY_X_US);
    for (usIndex = 0; usIndex < usRegConfigLen; usIndex++ ) // write GH30x reg.
    {
        if (stRegConfigArr[usIndex].usRegAddr < COMM_ALGORITHM_VIRTUAL_ADDR)
        {
            HBD_I2cWriteReg(HBD_GET_REG_REAL_ADRR(stRegConfigArr[usIndex].usRegAddr), stRegConfigArr[usIndex].usRegData);
            if ((stRegConfigArr[usIndex].usRegAddr & HBD_REG_DONT_NEED_CHECK_BIT) != HBD_REG_DONT_NEED_CHECK_BIT) /* need check val. */
            {
                if (HBD_I2cReadReg(HBD_GET_REG_REAL_ADRR(stRegConfigArr[usIndex].usRegAddr)) != stRegConfigArr[usIndex].usRegData)
                {
                    LOGD("err reg:0x%04x", stRegConfigArr[usIndex].usRegAddr);
                    chRet = HBD_RET_COMM_ERROR;
                    break;
                }
            }
        }
        else
        {
            HBD_VirtualConfig(stRegConfigArr[usIndex].usRegAddr, stRegConfigArr[usIndex].usRegData);
        }
    }
#if 0
    if (chRet == HBD_RET_OK)
    {
        HBD_StoreHbModeRegVal();
    }
#endif
    HBD_I2cSendCmd(HBD_CMD_SLEEP);
    HBD_DelayUs(HBD_SEND_CMD_DELAY_X_US);
    return chRet;
}

void Gh30xLoadBitFieldRegList(const ST_REGISTER_BIT_FIELD * pstCfg, GU16 usLen)
{
    for (GU16 usRegCnt = 0; usRegCnt < usLen; usRegCnt ++)
    {
        if (pstCfg->usRegAddr < COMM_ALGORITHM_VIRTUAL_ADDR)
        {
            HBD_I2cWriteRegBitField(pstCfg[usRegCnt].usRegAddr, pstCfg[usRegCnt].uchLsb, pstCfg[usRegCnt].uchMsb, pstCfg[usRegCnt].usRegData);
        }
        else
        {
            HBD_VirtualConfig(pstCfg[usRegCnt].usRegAddr, pstCfg[usRegCnt].usRegData);
        }
    }
}


/************************end of config array function list **************************/

/****************************************************************
* Description: Communication operation interface confirm.
* Output:  None
* Return: HBD_RET_OK=GH30x comunicate ok,
          HBD_RET_COMM_ERROR=GH30x comunicate error,
          HBD_RET_COMM_NOT_REGISTERED_ERROR=GH30x i2c/spi interface not registered,
******************************************************************/
GS8 HBD_CommunicationInterfaceConfirm (void)
{
    GS8 chRet = HBD_RET_COMM_ERROR;

    if ((g_pI2cWriteFunc == HBD_PTR_NULL) || (g_pI2cReadFunc == HBD_PTR_NULL))
    {
        return HBD_RET_COMM_NOT_REGISTERED_ERROR;
    }
    HBD_I2cSendCmd(HBD_CMD_IDLE);
    HBD_DelayUs(HBD_SEND_CMD_DELAY_X_US);
    if (HBD_I2cReadReg(HBD_ID1_REG_ADDR) == HBD_GH3XX_ID1_VAL)
    {
        chRet = HBD_RET_OK;
    }
    return chRet;
}

STGh30xChnlInfo g_pstGh30xChnlInfo[2];

const GU8 g_puchLogicChnlMap[][3] =
{
    {0,1,2},
    {0,2,1},
    {1,0,2},
    {1,2,0},
    {2,0,1},
    {2,1,0},
    {0,1,2},
    {0,1,2},
};

const GU8 g_puchPhysicalLedColor[3] = {PYH_LED0_COLOR, PYH_LED1_COLOR, PYH_LED2_COLOR};
#if (__EXAMPLE_DEBUG_LOG_LVL__)
const char *g_pstrLedColor[] = {"Green", "Red", "Ir"};
#endif

void Gh30xSampleRateSet(GU16 usSampleRate)
{
    HBD_I2cWriteReg(HBD_SAMPLE_RATE_REG_ADDR, 32768 / usSampleRate);
}

void Gh30xIntTimeSet(GU8 uchIntTimeRegisterValue)
{
    HBD_I2cWriteRegBitField(HBD_TIME_SEL0_REG_ADDR,8,10,uchIntTimeRegisterValue);
    HBD_I2cWriteRegBitField(HBD_TIME_SEL1_REG_ADDR,0,2,uchIntTimeRegisterValue);
}

void Gh30xSoftAgcSet(GU8 uchChnlId, EM_HBD_FUNCTIONAL_STATE emState)
{
    if (uchChnlId <= 2)
    {
        g_stAutoLedConfig.stAutoLedChannel[uchChnlId].emAutoledEnable = emState;
    }
}
void Gh30xHardAgcDisable(GU8 uchChnlId)
{
    GU8 uchLogicLedIndex = g_pstGh30xChnlInfo[uchChnlId].uchLogicLedIndex;
    if (uchLogicLedIndex <= 2)
    {
        HBD_I2cWriteRegBitField(0x012E, uchLogicLedIndex, uchLogicLedIndex, 0);   //disable driver current auto adjust
        HBD_I2cWriteRegBitField(0x0136, uchLogicLedIndex, uchLogicLedIndex, 0);   //disable gain auto adjust
    }
}

void Gh30xSetDefaultDrvCurrent(GU8 uchChnlId, GU8 uchDrvCurrentCode)
{
    GU8 uchLogicLedIndex = g_pstGh30xChnlInfo[uchChnlId].uchLogicLedIndex;
    if (uchLogicLedIndex <= 1)
    {
        HBD_I2cWriteRegBitField(HBD_LED12_REG_ADDR, 0 + uchLogicLedIndex * 8, 7 + uchLogicLedIndex * 8, uchDrvCurrentCode);
    }
    else if (uchLogicLedIndex == 2)
    {
        HBD_I2cWriteRegBitField(HBD_LED13_REG_ADDR, 0 + uchLogicLedIndex, 7 + uchLogicLedIndex, uchDrvCurrentCode);
    }
}

GU8 Gh30xGetDrvCurrent(GU8 uchChnlId)
{
    GU8 uchLogicLedIndex = g_pstGh30xChnlInfo[uchChnlId].uchLogicLedIndex;
    if (uchLogicLedIndex <= 1)
    {
        return HBD_I2cReadRegBitField(HBD_LED12_REG_ADDR, 0 + uchLogicLedIndex * 8, 7 + uchLogicLedIndex * 8);
    }
    else if (uchLogicLedIndex == 2)
    {
        return HBD_I2cReadRegBitField(HBD_LED13_REG_ADDR, 0 + uchLogicLedIndex, 7 + uchLogicLedIndex);
    }
    return 0;
}

void Gh30xSetDefaultGain(GU8 uchChnlId, GU8 uchGainCode)
{
    GU8 uchLogicLedIndex = g_pstGh30xChnlInfo[uchChnlId].uchLogicLedIndex;
    if (uchLogicLedIndex <= 2)
    {
        HBD_I2cWriteRegBitField(HBD_LED27_REG_ADDR, 4 + uchLogicLedIndex * 3, 6 + uchLogicLedIndex * 3,uchGainCode);
    }
}

GU8 Gh30xGetGain(GU8 uchChnlId)
{
    GU8 uchLogicLedIndex = g_pstGh30xChnlInfo[uchChnlId].uchLogicLedIndex;
    if (uchLogicLedIndex <= 2)
    {
        return HBD_I2cReadRegBitField(HBD_LED27_REG_ADDR, 4 + uchLogicLedIndex * 3, 6 + uchLogicLedIndex * 3);
    }
    return 0;
}

void Gh30xSetFifoWaterline(GU16 usWaterline)
{
    HBD_I2cWriteReg(HBD_FIFO_THR_REG_ADDR, usWaterline);
}

void Gh30xInitChnlDrvAndGainInfo(void)
{
    for (GU8 uchChnlCnt = 0; uchChnlCnt < 2; uchChnlCnt ++)
    {
        g_pstGh30xChnlInfo[uchChnlCnt].uchDrvCurrentCode = Gh30xGetDrvCurrent(uchChnlCnt);
        g_pstGh30xChnlInfo[uchChnlCnt].uchGain = Gh30xGetGain(uchChnlCnt);
        g_pstGh30xChnlInfo[uchChnlCnt].uchDrvCurrentChangeFlag = 0;
        g_pstGh30xChnlInfo[uchChnlCnt].uchGainChangeFlag = 0;
    }
}

void Gh30xUpdateChnlDrvInfo(void)
{
    for (GU8 uchChnlCnt = 0; uchChnlCnt < 2; uchChnlCnt ++)
    {
        GU8 uchCurrentDrv;
        uchCurrentDrv = Gh30xGetDrvCurrent(uchChnlCnt);
        if (uchCurrentDrv != g_pstGh30xChnlInfo[uchChnlCnt].uchDrvCurrentCode)
        {
            g_pstGh30xChnlInfo[uchChnlCnt].uchDrvCurrentChangeFlag = 1;
        }
        else
        {
            g_pstGh30xChnlInfo[uchChnlCnt].uchDrvCurrentChangeFlag = 0;
        }
        g_pstGh30xChnlInfo[uchChnlCnt].uchDrvCurrentCode = uchCurrentDrv;
    }
}

void Gh30xUpdateChnlGainInfo(GU8 uchChnlId, GU8 uchFifoHighByte)
{
    GU8 uchGain = ((uchFifoHighByte >> 1) & 0x07);
    if (g_pstGh30xChnlInfo[uchChnlId].uchGain != uchGain)
    {
        g_pstGh30xChnlInfo[uchChnlId].uchGainChangeFlag = 1;
        g_pstGh30xChnlInfo[uchChnlId].uchGain = uchGain;
    }
    else
    {
        g_pstGh30xChnlInfo[uchChnlId].uchGainChangeFlag = 0;
    }
}

void Gh30xGetChnlInfo(void)
{
    GU8 uchLedEnBit;
    GU8 uchLogicChnlMapIndex;

    uchLedEnBit = HBD_I2cReadRegBitField(HBD_CONFIG_REG_ADDR, 0, 2);
    uchLogicChnlMapIndex = HBD_I2cReadRegBitField(HBD_CONFIG_REG_ADDR, 6, 8);

    g_uchNeedReadRawdataCount = 0;
    memset((GU8*)g_pstGh30xChnlInfo, 0xFF, sizeof(g_pstGh30xChnlInfo));
    for (GU8 uchChnlCnt = 0; uchChnlCnt < 3; uchChnlCnt++)
    {
        if (uchLedEnBit & (1 << uchChnlCnt))
        {
            g_pstGh30xChnlInfo[g_uchNeedReadRawdataCount].uchLogicLedIndex = uchChnlCnt;
            g_pstGh30xChnlInfo[g_uchNeedReadRawdataCount].uchLedColor = g_puchPhysicalLedColor[g_puchLogicChnlMap[uchLogicChnlMapIndex][uchChnlCnt]];

            EXAMPLE_DEBUG_LOG_L1("chnl%d: uchLogicLedIndex = %d, color = %s\r\n",
                                        g_uchNeedReadRawdataCount,
                                        g_pstGh30xChnlInfo[g_uchNeedReadRawdataCount].uchLogicLedIndex,
                                        g_pstrLedColor[g_pstGh30xChnlInfo[g_uchNeedReadRawdataCount].uchLedColor]
                                        );

            g_usReadRawdataRegList[g_uchNeedReadRawdataCount] = HBD_LED0_DATA_L_REG_ADDR + 4 * uchChnlCnt; // g_uchNeedReadRawdataCount;

            g_uchNeedReadRawdataCount++;
            if (g_uchNeedReadRawdataCount >= 2)
            {
                break;
            }
        }
    }
    EXAMPLE_DEBUG_LOG_L1("g_uchNeedReadRawdataCount = %d\r\n", g_uchNeedReadRawdataCount);
}

/****************************************************************
* Description: calc rawdata count.
* Output:  None
* Return: None
******************************************************************/
#if (__SYSTEM_TEST_SUPPORT__)
static void HBD_CalcRawdataCount(GU16 usHbdConfigRegVal)
{
    GU8 uchIndex = 0;

    g_uchNeedReadRawdataCount = 0;
    for (uchIndex = 0; uchIndex < 3; uchIndex ++)
    {
        if (((usHbdConfigRegVal & (1 << uchIndex)) != 0) && (g_uchNeedReadRawdataCount < 2))
        {
            g_usReadRawdataRegList[g_uchNeedReadRawdataCount] = HBD_LED0_DATA_L_REG_ADDR + uchIndex * 4;
            g_uchNeedReadRawdataCount ++;
        }
    }
    if (((usHbdConfigRegVal & 0x0007) == 0x0007))
    {
        g_usReadRawdataRegList[0] = HBD_LED0_DATA_L_REG_ADDR + 4;
        g_usReadRawdataRegList[1] = HBD_LED0_DATA_L_REG_ADDR + 8;
    }
    if ((usHbdConfigRegVal & 0x000E) == 0x000E)
    {
        g_uchNeedReadRawdataCount = 1;
        if ((usHbdConfigRegVal & 0x001E) == 0x001E) //合成打码: led1和led2共享配置，以led1配置为准
        {
            g_usReadRawdataRegList[0] = HBD_LED0_DATA_L_REG_ADDR + 4;  //调节LED1的电流和增益 led1和led2共享配置
        }
    }
}
#endif

/****************************************************************
* Description: Change reg val to hb mode val.
* Output:  None
* Return: None
******************************************************************/
void HBD_ChangeToHbModeRegVal(void)
{
    if (g_uchAdtHasStarted == 1) // if last start adt, refresh analog reg list
    {
        #if 0
        HBD_I2cWriteReg(HBD_CONFIG_REG_ADDR, g_stHbRegList.usChannelMapConfigRegVal); // reg 0x84
        HBD_I2cWriteReg(HBD_LED12_REG_ADDR, g_stHbRegList.usLed01CurrentConfigRegVal); // reg 0x118
        HBD_I2cWriteReg(HBD_LED27_REG_ADDR, g_stHbRegList.usGainConfigRegVal); // reg 0x136
        HBD_I2cWriteReg(HBD_TIME_SEL0_REG_ADDR, g_stHbRegList.usTimeSel0RegVal); // reg 0x80
        HBD_I2cWriteReg(HBD_TIME_SEL1_REG_ADDR, g_stHbRegList.usTimeSel1RegVal); // reg 0x82
        HBD_I2cWriteReg(HBD_AFE_REG_ADDR, g_stHbRegList.usHbdAfeRegVal); // reg 0x186
        #endif
        g_uchAdtHasStarted = 0;
    }
}

/****************************************************************
* Description: store hb mode val.
* Output:  None
* Return: None
******************************************************************/
void HBD_StoreHbModeRegVal(void)
{
#if (__SYSTEM_TEST_SUPPORT__)
    g_stHbRegList.usChannelMapConfigRegVal = HBD_I2cReadReg(HBD_CONFIG_REG_ADDR); // reg 0x84
    EXAMPLE_DEBUG_LOG_L1("[%s]usChannelMapConfigRegVal = 0x%X\r\n", __FUNCTION__,g_stHbRegList.usChannelMapConfigRegVal);
    g_stHbRegList.usLed01CurrentConfigRegVal = HBD_I2cReadReg(HBD_LED12_REG_ADDR); // reg 0x118
    EXAMPLE_DEBUG_LOG_L1("[%s]usLed01CurrentConfigRegVal = 0x%X\r\n", __FUNCTION__,g_stHbRegList.usLed01CurrentConfigRegVal);
    g_stHbRegList.usGainConfigRegVal = HBD_I2cReadReg(HBD_LED27_REG_ADDR); // reg 0x136
    EXAMPLE_DEBUG_LOG_L1("[%s]usGainConfigRegVal = 0x%X\r\n", __FUNCTION__,g_stHbRegList.usGainConfigRegVal);
    g_stHbRegList.usTimeSel0RegVal = HBD_I2cReadReg(HBD_TIME_SEL0_REG_ADDR); // reg 0x80
    EXAMPLE_DEBUG_LOG_L1("[%s]usTimeSel0RegVal = 0x%X\r\n", __FUNCTION__,g_stHbRegList.usTimeSel0RegVal);
    g_stHbRegList.usTimeSel1RegVal = HBD_I2cReadReg(HBD_TIME_SEL1_REG_ADDR); // reg 0x82
    EXAMPLE_DEBUG_LOG_L1("[%s]usTimeSel1RegVal = 0x%X\r\n", __FUNCTION__,g_stHbRegList.usTimeSel1RegVal);
    g_stHbRegList.usHbdAfeRegVal = HBD_I2cReadReg(HBD_AFE_REG_ADDR);         // reg 0x186
    EXAMPLE_DEBUG_LOG_L1("[%s]usHbdAfeRegVal = 0x%X\r\n", __FUNCTION__,g_stHbRegList.usHbdAfeRegVal);
#else
    EXAMPLE_DEBUG_LOG_L2("[%s]usChannelMapConfigRegVal = 0x%X\r\n", __FUNCTION__, HBD_I2cReadReg(HBD_CONFIG_REG_ADDR));
    EXAMPLE_DEBUG_LOG_L2("[%s]usChannelMapConfigRegVal = 0x%X\r\n", __FUNCTION__, HBD_I2cReadReg(HBD_LED12_REG_ADDR));
    EXAMPLE_DEBUG_LOG_L2("[%s]usChannelMapConfigRegVal = 0x%X\r\n", __FUNCTION__, HBD_I2cReadReg(HBD_LED27_REG_ADDR));
    EXAMPLE_DEBUG_LOG_L2("[%s]usChannelMapConfigRegVal = 0x%X\r\n", __FUNCTION__, HBD_I2cReadReg(HBD_TIME_SEL0_REG_ADDR));
    EXAMPLE_DEBUG_LOG_L2("[%s]usChannelMapConfigRegVal = 0x%X\r\n", __FUNCTION__, HBD_I2cReadReg(HBD_TIME_SEL1_REG_ADDR));
    EXAMPLE_DEBUG_LOG_L2("[%s]usChannelMapConfigRegVal = 0x%X\r\n", __FUNCTION__, HBD_I2cReadReg(HBD_AFE_REG_ADDR));

    // EXAMPLE_DEBUG_LOG_L1("[%s]usChannelMapConfigRegVal = 0x%X\r\n", __FUNCTION__, HBD_I2cReadReg(HBD_CONFIG_REG_ADDR));
    // EXAMPLE_DEBUG_LOG_L1("[%s]usChannelMapConfigRegVal = 0x%X\r\n", __FUNCTION__, HBD_I2cReadReg(HBD_LED12_REG_ADDR));
    // EXAMPLE_DEBUG_LOG_L1("[%s]usChannelMapConfigRegVal = 0x%X\r\n", __FUNCTION__, HBD_I2cReadReg(HBD_LED27_REG_ADDR));
    // EXAMPLE_DEBUG_LOG_L1("[%s]usChannelMapConfigRegVal = 0x%X\r\n", __FUNCTION__, HBD_I2cReadReg(HBD_TIME_SEL0_REG_ADDR));
    // EXAMPLE_DEBUG_LOG_L1("[%s]usChannelMapConfigRegVal = 0x%X\r\n", __FUNCTION__, HBD_I2cReadReg(HBD_TIME_SEL1_REG_ADDR));
    // EXAMPLE_DEBUG_LOG_L1("[%s]usChannelMapConfigRegVal = 0x%X\r\n", __FUNCTION__, HBD_I2cReadReg(HBD_AFE_REG_ADDR));
#endif

    g_uchAdtHasStarted = 0;

#if (__SYSTEM_TEST_SUPPORT__)
    HBD_CalcRawdataCount(g_stHbRegList.usChannelMapConfigRegVal);
#endif

    return;
}

/****************************************************************
* Description: Change reg val to adt mode val.
* Output:  None
* Return: None
******************************************************************/
void HBD_ChangeToAdtModeRegVal(void)
{
    if (g_uchAdtHasStarted == 0)
    {
        g_uchAdtHasStarted = 1;
    #if 0
        HBD_I2cWriteReg(HBD_CONFIG_REG_ADDR, g_stAdtRegList.usChannelMapConfigRegVal); // reg 0x84
        EXAMPLE_DEBUG_LOG_L1("[%s]usChannelMapConfigRegVal = 0x%X\r\n", __FUNCTION__,g_stAdtRegList.usChannelMapConfigRegVal);
        HBD_I2cWriteReg(HBD_LED12_REG_ADDR, g_stAdtRegList.usLed01CurrentConfigRegVal); // reg 0x118
        EXAMPLE_DEBUG_LOG_L1("[%s]usLed01CurrentConfigRegVal = 0x%X\r\n", __FUNCTION__,g_stAdtRegList.usLed01CurrentConfigRegVal);
        HBD_I2cWriteReg(HBD_LED27_REG_ADDR, g_stAdtRegList.usGainConfigRegVal); // reg 0x136
        EXAMPLE_DEBUG_LOG_L1("[%s]usGainConfigRegVal = 0x%X\r\n", __FUNCTION__,g_stAdtRegList.usGainConfigRegVal);
        HBD_I2cWriteReg(HBD_TIME_SEL0_REG_ADDR, g_stAdtRegList.usTimeSel0RegVal); // reg 0x80
        EXAMPLE_DEBUG_LOG_L1("[%s]usTimeSel0RegVal = 0x%X\r\n", __FUNCTION__,g_stAdtRegList.usTimeSel0RegVal);
        HBD_I2cWriteReg(HBD_TIME_SEL1_REG_ADDR, g_stAdtRegList.usTimeSel1RegVal); // reg 0x82
        EXAMPLE_DEBUG_LOG_L1("[%s]usTimeSel1RegVal = 0x%X\r\n", __FUNCTION__,g_stAdtRegList.usTimeSel1RegVal);
        HBD_I2cWriteReg(HBD_AFE_REG_ADDR, g_stAdtRegList.usHbdAfeRegVal); // reg 0x186
        EXAMPLE_DEBUG_LOG_L1("[%s]usHbdAfeRegVal = 0x%X\r\n", __FUNCTION__,g_stAdtRegList.usHbdAfeRegVal);
    #endif
    }
}

/****************************************************************
* Description: Communicate confirm.
* Output:  None
* Return: HBD_RET_OK=check ok, HBD_RET_COMM_ERROR=check reg error
******************************************************************/
GS8 HBD_CommunicateConfirm(void)
{
    GS8 chRet = HBD_RET_COMM_ERROR;
    GU8 uchCheckCommunicateCnt = 0;
    /*test communicate, check GH30x communicate ok.*/
    for (uchCheckCommunicateCnt = 0; uchCheckCommunicateCnt < HBD_COMMUNICATE_CHECK_MAX_CNT; uchCheckCommunicateCnt++)
    {
        HBD_I2cSendCmd(HBD_CMD_IDLE);
        HBD_DelayUs(HBD_SEND_CMD_DELAY_X_US);
        if (HBD_I2cReadReg(HBD_ID1_REG_ADDR) == HBD_GH3XX_ID1_VAL)
        {
            chRet = HBD_RET_OK;
            break;
        }
    }
    if (chRet != HBD_RET_OK)
    {
        HBD_I2cSendCmd(HBD_CMD_SLEEP);
    }
    return chRet;
}

/****************************************************************
* Description: simple init hbd configure parameters
* Input:    stHbInitConfigParam: Init Config struct ,see ST_HBD_INIT_CONFIG_TYPE,
* Return: HBD_RET_OK=success,
          HBD_RET_PARAMETER_ERROR=paramters error,
          HBD_RET_COMM_ERROR=GH30x comunicate error,
          HBD_RET_COMM_NOT_REGISTERED_ERROR=GH30x i2c interface not registered
******************************************************************/
GS8 HBD_SimpleInit(ST_HBD_INIT_CONFIG_TYPE *stHbdInitConfigParam)
{
    // GU8 *uchInitRegConfigArrPtr = (GU8 *)g_uchGh300InitRegConfigArr;
    GU16 usRegVal = 0;
    g_uchHbdInited = HBD_INIT_STATUS_NO_INIT;
    g_uchHbdStarted = HBD_START_STATUS_OFF;
    g_uchGh30xLastCmd = HBD_CMD_SLEEP;
    g_uchNewIntFlag = 0;
    //HBD_UnUseVarWarningCancel();
    if (stHbdInitConfigParam == HBD_PTR_NULL)
    {
        return HBD_RET_PARAMETER_ERROR;
    }

    if ((g_pI2cWriteFunc == HBD_PTR_NULL) || (g_pI2cReadFunc == HBD_PTR_NULL))
    {
        return HBD_RET_COMM_NOT_REGISTERED_ERROR;
    }

    if (HBD_RET_OK != HBD_CommunicateConfirm())
    {
        return HBD_RET_COMM_ERROR;
    }

    g_uchPackageSN = 0;
    if (HBD_RET_OK != HBD_AutoLedReset(HBD_START_STATUS_OFF))
    {
        return HBD_RET_COMM_ERROR;
    }

    HBD_I2cWriteRegBitField(0x0012, 0, 0, 0); // diable IDLE time out exit
    HBD_I2cWriteRegBitField(0x0046, 15, 15, 0); // diable RESUME time out enter
    HBD_I2cReadReg(HBD_IRQ_STATUS_REG_ADDR); // clear all INT status
    HBD_I2cWriteReg(0x0182, 0x84DB); // fix vdd18_stb to 1.8V
    HBD_I2cWriteReg(0x0180, 0x008D); // fixed vdd18a & vdd18d

    //  fix cp & ft clock trim error
    {
        usRegVal = HBD_I2cReadReg(0x00E4);
        if ((usRegVal == 0x01) || (usRegVal == 0x02))
        {
            GU16 us32kTrimValue = 0;
            GU16 us12mAndV0p6TrimValue = 0;
            GU16 usCpReTrimVal = 0;
            GU8  uchOtpFtUpdateFlag = 1;

            HBD_I2cWriteReg(0x0064, 0x0020); // trim OSC 32K[8:0], otp addr 0x0020
            HBD_I2cWriteReg(0x006A, 0x0001);
            HBD_DelayUs(3);
            HBD_I2cWriteReg(0x006A, 0x0000);
            us32kTrimValue = HBD_I2cReadReg(0x006C);

            HBD_I2cWriteReg(0x0064, 0x0022); // trim OSC 12M[6:0] & V0P6[11:8], otp addr 0x0022
            HBD_I2cWriteReg(0x006A, 0x0001);
            HBD_DelayUs(3);
            HBD_I2cWriteReg(0x006A, 0x0000);
            us12mAndV0p6TrimValue = HBD_I2cReadReg(0x006C);

            if (usRegVal == 0x02)
            {
                GU8 uchOtpFtTrimVal[5] = {0};
                GU16 usCpTrimCrcValue = 0;
                GU8 uchCpTrimCrc8CalcValue = 0;

                uchOtpFtUpdateFlag = 0;
                HBD_I2cWriteReg(0x0064, 0x0024); // trim CRC8[7:0], otp addr 0x0024
                HBD_I2cWriteReg(0x006A, 0x0001);
                HBD_DelayUs(3);
                HBD_I2cWriteReg(0x006A, 0x0000);
                usCpTrimCrcValue = HBD_I2cReadReg(0x006C);

                uchOtpFtTrimVal[0] = HBD_GET_LOW_BYTE_FROM_WORD(us32kTrimValue);
                uchOtpFtTrimVal[1] = HBD_GET_HIGH_BYTE_FROM_WORD(us32kTrimValue);
                uchOtpFtTrimVal[2] = HBD_GET_LOW_BYTE_FROM_WORD(us12mAndV0p6TrimValue);
                uchOtpFtTrimVal[3] = HBD_GET_HIGH_BYTE_FROM_WORD(us12mAndV0p6TrimValue);
                uchOtpFtTrimVal[4] = HBD_GET_LOW_BYTE_FROM_WORD(usCpTrimCrcValue);
                uchCpTrimCrc8CalcValue = HBD_GetOtpTrimCRC(uchOtpFtTrimVal, 4);
                if (uchOtpFtTrimVal[4] == uchCpTrimCrc8CalcValue)
                {
                    uchOtpFtUpdateFlag = 1;
                    //  V0P6[11:8]
                    usCpReTrimVal = HBD_I2cReadReg(0x0194) & 0xFFF0;
                    usCpReTrimVal = usCpReTrimVal | ((us12mAndV0p6TrimValue >> 8) & 0x000F);
                    HBD_I2cWriteReg(0x0194, usCpReTrimVal);
                }
            }
            if (uchOtpFtUpdateFlag != 0)
            {
                // OSC 32K[8:0]
                usCpReTrimVal = HBD_I2cReadReg(0x018A) & 0xFE00;
                usCpReTrimVal = usCpReTrimVal | (us32kTrimValue & 0x01FF);
                HBD_I2cWriteReg(0x018A, usCpReTrimVal);
                // OSC 12M[6:0]
                usCpReTrimVal = (us12mAndV0p6TrimValue & 0x007F);
                HBD_I2cWriteReg(0x018C, usCpReTrimVal);
            }
        }
    }


    g_emHbFifoEnableFlag = stHbdInitConfigParam->stHbInitConfig.emHbModeFifoEnable;
    g_emHrvFifoEnableFlag = stHbdInitConfigParam->stHbInitConfig.emHrvModeFifoEnable;
    g_emBpfFifoEnableFlag = stHbdInitConfigParam->stHbInitConfig.emBpfModeFifoEnable;
    g_emSpo2FifoEnableFlag = stHbdInitConfigParam->stHbInitConfig.emSpo2ModeFifoEnable;
    HBD_StoreHbModeRegVal();

    /// adt config
    HBD_I2cWriteReg(HBD_ADT_GINT_CTRL0_REG_ADDR, (GU16)stHbdInitConfigParam->stAdtInitConfig.emGINTEnable);

    /// use adt reg1[bit 0] as inited flag
    usRegVal = HBD_I2cReadReg(HBD_ADT_GINT_CTRL1_REG_ADDR);
    usRegVal |= 0x0001;
    HBD_I2cWriteReg(HBD_ADT_GINT_CTRL1_REG_ADDR, usRegVal);

    HBD_I2cSendCmd(HBD_CMD_SLEEP);
    HBD_DelayUs(HBD_SEND_CMD_DELAY_X_US);
    g_uchLastReadFifoCnt = 0;
    g_uchHbdInited = HBD_INIT_STATUS_INITED;
    return HBD_RET_OK;
}

GS8 HBD_GetChipId(GU8 *uchIdData, GU8 *uchIdLen)
{
    GU16 usTempValue = 0;
    GU16 usOtpUidReadStartAddr = 0x0010;
    GU8 uchIndex = 0;
    if (g_uchHbdStarted == HBD_START_STATUS_OFF)
    {
        HBD_I2cSendCmd(HBD_CMD_IDLE);
        HBD_DelayUs(HBD_SEND_CMD_DELAY_X_US);
    }
    if ((g_uchHbdStarted >=  HBD_START_STATUS_ADT_WEAR_MODE) || (0 == g_uchWrRegflag))
    {
        return HBD_RET_GENERIC_ERROR;
    }

    while (usOtpUidReadStartAddr < 0x0020)
    {
        HBD_I2cWriteReg(0x0064, usOtpUidReadStartAddr);
        HBD_I2cWriteReg(0x006A, 0x0001);
        HBD_DelayUs(3);
        HBD_I2cWriteReg(0x006A, 0x0000);
        usTempValue = HBD_I2cReadReg(0x006C);
        uchIdData[uchIndex++] = HBD_GET_LOW_BYTE_FROM_WORD(usTempValue);
        uchIdData[uchIndex++] = HBD_GET_HIGH_BYTE_FROM_WORD(usTempValue);
        usOtpUidReadStartAddr += 0x0002;
    }
    *uchIdLen = uchIndex;
    if (g_uchHbdStarted == HBD_START_STATUS_OFF)
    {
        HBD_I2cSendCmd(HBD_CMD_SLEEP);
        HBD_DelayUs(HBD_SEND_CMD_DELAY_X_US);
    }

    return HBD_RET_OK;
}

/****************************************************************
* Description: check chip inited or not
* Input:  None
* Return: 1=inited; 0+no inited
******************************************************************/
GU8 HBD_CheckChipInited(void)
{
    HBD_I2cSendCmd(HBD_CMD_IDLE); // stop
    HBD_DelayUs(HBD_SEND_CMD_DELAY_X_US);

    return (GU8)(HBD_I2cReadReg(HBD_ADT_GINT_CTRL1_REG_ADDR) & 0x0001);
}

/****************************************************************
* Description: config autoled channel
* Input:  stAutoLedChannelConfig: channal config
          uchChannel: channel index(1 or 2)
* Return: HBD_RET_OK=success,
          HBD_RET_PARAMETER_ERROR=paramters error,
******************************************************************/
GS8 HBD_AutoLedChannelConfig(ST_AUTO_LED_CH_CONFIG stAutoLedChannelConfig, GU8 uchChannel)
{
    GS8 chRet = HBD_RET_PARAMETER_ERROR;
    if ((uchChannel == 1 || uchChannel == 2))
    {
        g_stAutoLedConfig.stAutoLedChannel[uchChannel - 1] = stAutoLedChannelConfig;
        chRet = HBD_RET_OK;
    }
    return chRet;
}

/****************************************************************
* Description: config autoled bg thr
* Input:  uchBgUpperThrConfig: bg upper thr config
          uchgLowerThrConfig: bg lower thr config
* Return: HBD_RET_OK=success
******************************************************************/
GS8 HBD_AutoLedBgThrConfig(GU8 uchBgUpperThrConfig, GU8 uchgLowerThrConfig)
{
    g_stAutoLedConfig.uchBgUpperThr = uchBgUpperThrConfig;
    g_stAutoLedConfig.uchgLowerThr = uchgLowerThrConfig;
    return HBD_RET_OK;
}

/****************************************************************
* Description: config autoled
* Input:  stAutoLedCh1Config: channal 1 config
          stAutoLedCh2Config: channal 2 config
          uchBgUpperThrConfig: bg upper thr config
          uchgLowerThrConfig: bg lower thr config
* Return: HBD_RET_OK=success,
******************************************************************/
GS8 HBD_AutoLedConfig(ST_AUTO_LED_CH_CONFIG stAutoLedCh1Config, ST_AUTO_LED_CH_CONFIG stAutoLedCh2Config,
                      GU8 uchBgUpperThrConfig, GU8 uchgLowerThrConfig)
{
    HBD_AutoLedChannelConfig(stAutoLedCh1Config, 1);
    HBD_AutoLedChannelConfig(stAutoLedCh2Config, 2);
    HBD_AutoLedBgThrConfig(uchBgUpperThrConfig, uchgLowerThrConfig);
    return HBD_RET_OK;
}

/****************************************************************
* Description: config autoled gain start val
* Return: HBD_RET_OK=success,
******************************************************************/
GS8 HBD_AutoLedGainModeConfig(EM_HBD_FUNCTIONAL_STATE emUseConfigValEnable, GU8 uchGainVal)
{
    if (emUseConfigValEnable == HBD_FUNCTIONAL_STATE_ENABLE)
    {
        g_uchAutoLedGainStartVal = 0x80 | ((uchGainVal & 0x07 << 3) | (uchGainVal & 0x07));
    }
    else
    {
        g_uchAutoLedGainStartVal = 0x9B; /* default channel 1/2 Gain start from 0x03*/
    }
    return HBD_RET_OK;
}

/****************************************************************
* Description: config autoled current thr max val
* Input:  uchAutoledCurrentMaxThrVal: current max thr val;
* Return: HBD_RET_OK=success,
******************************************************************/
GS8 HBD_AutoLedCurrentMaxThrConfig(GU8 uchAutoledCurrentMaxThrVal)
{
    g_stAutoLedConfig.stAutoLedChannel[0].uchCurrentLimit = uchAutoledCurrentMaxThrVal;
    g_stAutoLedConfig.stAutoLedChannel[1].uchCurrentLimit = uchAutoledCurrentMaxThrVal;
    return HBD_RET_OK;
}

/****************************************************************
* Description: Calculate Sample Rate
* Input:  None
* Return: Sample rate(Hz)  正常配置采样率应该大于25Hz，且为25的整数倍
******************************************************************/
static GU16 HBD_AutoLedCalcuSampleRate()
{
    GU16 usRegVal = 0;
    GU16 usSampleVal = 0;
    usRegVal = HBD_I2cReadReg(HBD_SAMPLE_RATE_REG_ADDR);
    if (0 == usRegVal)
    {
        usSampleVal = HB_SAMPLING_RATE_SETTING; //默认值
    }
    else
    {
        usSampleVal = (GU16)(HBD_SAMPLE_RATE_CLOCK_FREQ / usRegVal);
        usSampleVal = ((usSampleVal + 12) / 25) * 25;  //取整操作
    }
    return  usSampleVal;
}

/****************************************************************
* Description: soft autoled reset
* Input:  None
* Return: HBD_RET_OK=success
******************************************************************/
GS8 HBD_AutoLedReset(GU8 uchMode)
{
    GU16 usSampleVal = 0;
    GU8 uchIndex = 0;
    GU8 Try_time;

    for (uchIndex = 0; uchIndex < HBD_AUTO_LED_MAX_CHANNEL_NUM; uchIndex++)
    {
        /* 计算理想目标值 Ideal = Low + (High - Low) * 0.8 */
        g_unIdealTargetValueArr[uchIndex] = (GU32)(g_stAutoLedConfig.stAutoLedChannel[uchIndex].unTargetLowerThr * (1 - g_fIdealCalcRatio) + \
            g_stAutoLedConfig.stAutoLedChannel[uchIndex].unTargetUpperThr * g_fIdealCalcRatio);
        /* 限制到 50000 */
        g_unIdealTargetValueArr[uchIndex] = (g_unIdealTargetValueArr[uchIndex] > 50000) ? 50000 : g_unIdealTargetValueArr[uchIndex];

        g_unIdealTargetTrigArr[uchIndex] = (GU32)(g_stAutoLedConfig.stAutoLedChannel[uchIndex].unTargetLowerThr * (1 - g_fIdealTrigRatio) + \
            g_stAutoLedConfig.stAutoLedChannel[uchIndex].unTargetUpperThr * g_fIdealTrigRatio);
        g_unIdealTargetTrigArr[uchIndex] = (g_unIdealTargetTrigArr[uchIndex] > g_stAutoLedConfig.stAutoLedChannel[uchIndex].unTargetLowerThr) ? \
            g_unIdealTargetTrigArr[uchIndex] : g_stAutoLedConfig.stAutoLedChannel[uchIndex].unTargetLowerThr ;
//        g_unIdealTargetTrigArr[uchIndex] = g_unIdealTargetTrigArr[uchIndex];

        /* 初始化理想目标值调节结构体 */
        memset(&g_stIdealAdjust.stIdealAdjustCh[uchIndex], 0, sizeof(ST_IDEAL_ADJUST_CH));
        /* 初始化背景光调节结构体 */
        memset(&g_stBGAdj.stBGAdjCh[uchIndex], 0, sizeof(ST_BG_ADJUST_CH));
        memset(&g_stBufferBGInfo[uchIndex], 0, sizeof(ST_RECURSION_CALC_NOSTD));

        /* 初始化运动相关结构体 */
        //gfMotionTrigRatio[uchIndex] = g_fTriggerIdealAdjustRatio[uchIndex];
        gfMotionTrigRatio[uchIndex] = 0.0f;

        /* 初始化触发调光结构体 */
        memset(&g_stTrigCalcInfo[uchIndex], 0, sizeof(ST_RECURSION_CALC_NOSTD));
        /* 初始化饱和判断阈值 */
        g_PpgSatThr[uchIndex] = g_stAutoLedConfig.stAutoLedChannel[uchIndex].unTargetUpperThr;

        /* 初始化全局电流 */
        g_uchAutoLedCurrentArr[uchIndex] = 0;
        /* 初始化写寄存器确认 */
        g_stAutoledWrtRegCheck.unWrtRegNormDuration[uchIndex] = g_stAutoledWrtRegCheck.unWrtRegThres;
    }

    /* 获取采样频率 */
    usSampleVal = HBD_AutoLedCalcuSampleRate();

    //正常配置采样率应该大于25Hz，且为25的整数倍(当通信错误采样率算错再算3次)
    if (!usSampleVal) //如果采样率为0
    {
        Try_time = 3;
        while ((!usSampleVal) && Try_time)
        {
            --Try_time;
            usSampleVal = HBD_AutoLedCalcuSampleRate();
        }

        if (!usSampleVal) //3次重新计算之后采样率了依然为零 上报错误
        {
            return HBD_RET_COMM_ERROR;
        }
    }

    /* 采样信息 */
    g_stSampleInfo.unSampleFre = usSampleVal;  //fre
    g_stSampleInfo.unSampleTim = 1000 / g_stSampleInfo.unSampleFre; //每帧对应时间ms
    g_stSampleInfo.unSampleFifoDuration = 0; //初始化一次fifo中断对应的时间
    g_stSampleInfo.unAutoLedCalcLen = usSampleVal / 5;  //计算触发调光的长度

    /* 初始化Gsensor计算结构体 */
    //memset(&g_stAutoLedAccInfo,0,sizeof(ST_RECURSION_CALC_STD));

    /****************** 全局变量返回初始化 **********************/
    g_uchAutoLedErrorFlag = 0; //调光失败
    g_uchAutoLedLastFlag = 0; //调光标记

    /*单帧模式 */

    g_uchSoftLEDNewdataModeCNT = 0;//计数器

    /* 单通道 若第二通道开启则关闭 */
    if ((g_uchNeedReadRawdataCount == 1) && (g_stAutoLedConfig.stAutoLedChannel[1].emAutoledEnable == HBD_FUNCTIONAL_STATE_ENABLE))
    {
        g_stAutoLedConfig.stAutoLedChannel[1].emAutoledEnable = HBD_FUNCTIONAL_STATE_DISABLE;
    }

    // LOGY("[AutoLED][RESTFUN][RST]: CalcLen:%d SampleFre:%d IT:%05d %05d", g_stSampleInfo.unAutoLedCalcLen, usSampleVal, (int)g_unIdealTargetValueArr[0], (int)g_unIdealTargetValueArr[1]);
    return HBD_RET_OK;
}

/****************************************************************
* Description: soft autoled Write register
* Input:  Gain current
* Return: None
******************************************************************/
static void HBD_AutoLedWriteReg(GU8 uchGainArr[2], GU8 uchGainShiftArr[2], GU16 usLastGainMsk,
    GU8 uchCurrentArr[2], GU8 uchCurrentShift[2], GU32 unLastCurrentMsk)
{
    HBD_I2cWriteReg(HBD_LED27_REG_ADDR, usLastGainMsk | (GU16)(uchGainArr[0] << uchGainShiftArr[0]) | (GU16)(uchGainArr[1] << uchGainShiftArr[1]));//写Gain
    HBD_I2cWriteRawdataReg(HBD_LED12_REG_ADDR, unLastCurrentMsk | (GU32)(uchCurrentArr[0] << uchCurrentShift[0]) | (GU32)(uchCurrentArr[1] << uchCurrentShift[1]));//写电流
    // LOGY("[AutoLED][Wrt_Reg][CHA]: C0:%d G0:%d C1:%d G1:%d ", uchCurrentArr[0], uchGainArr[0], uchCurrentArr[1], uchGainArr[1]);
}

/****************************************************************
* Description: soft autoled Recursion Calculate
* Input: Luinput- intput data  uNum- Loop times
* Output: fstd    favg  lumin  lumax
* Return: None
******************************************************************/
//static void HBD_AutoLedRecursionCalc(GU32 luinput, GF32 *fstd, GF32 *favg, GU32 *lumin, GU32 *lumax, GU32 *ludiff, GS32 uNum)
//{
//    if (0 == uNum || HBD_PTR_NULL == fstd || HBD_PTR_NULL == favg || HBD_PTR_NULL == lumin || HBD_PTR_NULL == lumax)
//    {
//        return;
//    }
//    else
//    {
//        if (1 == uNum)
//        {
//            *lumax = luinput;
//            *lumin = luinput;
//            *favg = luinput * 1.0f;
//            *fstd = 0;
//        }
//        else{
//            *lumax = luinput > *lumax ? luinput : *lumax;
//            *lumin = luinput < *lumin ? luinput : *lumin;
//            *fstd = *fstd + (GF32)(uNum - 1) / uNum * (luinput - *favg) * (luinput - *favg);//递归计算标准差
//            *favg = *favg + (GF32)(luinput - *favg) / uNum;//递归计算均值
//        }
//        *ludiff = *lumax - *lumin;
//    }
//}

/****************************************************************
* Description: soft autoled Recursion Calculate
* Input: Luinput- intput data  uNum- Loop times
* Output: favg    lumin  lumax
* Return: None
******************************************************************/
static void HBD_AutoLedRecursionCalcNoSTD(GU32 luinput, GF32 *favg, GU32 *lumin, GU32 *lumax, GU32 *ludiff, GU16 uNum)
{
    if (0 == uNum || HBD_PTR_NULL == favg || HBD_PTR_NULL == lumin || HBD_PTR_NULL == lumax || HBD_PTR_NULL == ludiff)
    {
        return;
    }
    else
    {
        if (1 == uNum)
        {
            *lumax = luinput;
            *lumin = luinput;
            *favg = luinput * 1.0f;
        }
        else
        {
            *lumax = luinput > *lumax ? luinput : *lumax;
            *lumin = luinput < *lumin ? luinput : *lumin;
            *favg = *favg + (GF32)(luinput - *favg) / uNum;//递归计算均值
        }
        *ludiff = *lumax - *lumin;
    }
}

/****************************************************************
* Description: soft autoled clac ACC std avg max min
* Input: stGsAxisValue[] lLength sGsensorRightShift
* Return: None
******************************************************************/
//static void HBD_AutoLedCalcAccSTD(ST_GS_DATA_TYPE stGsAxisValue[], GS32 lLength, ST_RECURSION_CALC_STD *pstaccstd, GS16 sGsensorRightShift)
//{

//    GU32 unACC = 0;
//    GS16 sACCX = 0;
//    GS16 sACCY = 0;
//    GS16 sACCZ = 0;
//    if (HBD_PTR_NULL == stGsAxisValue)//空指针 返回0
//    {
//        pstaccstd->fAvg = 0;
//        pstaccstd->uMax = 0;
//        pstaccstd->uMin = 0;
//        pstaccstd->fStd = 0;
//        return;
//    }
//    for (GS32 i = 0; i < lLength; i++)
//    {
//        sACCX = stGsAxisValue[i].sXAxisVal / sGsensorRightShift;//X轴归一化
//        sACCY = stGsAxisValue[i].sYAxisVal / sGsensorRightShift;//Y轴归一化
//        sACCZ = stGsAxisValue[i].sZAxisVal / sGsensorRightShift;//Z轴归一化
//        unACC = (GU32)sqrt(sACCX * sACCX + sACCY * sACCY + sACCZ * sACCZ);//矢量和
//        HBD_AutoLedRecursionCalc(unACC, &pstaccstd->fStd, &pstaccstd->fAvg, &pstaccstd->uMin, &pstaccstd->uMax, &pstaccstd->uDif, i + 1);//计算标准差
//    }

//    if (1 == lLength || 0 == lLength){
//        pstaccstd->fStd = 0;//长度小于1 标准差为0
//    }
//    else{
//        pstaccstd->fStd = sqrtf(pstaccstd->fStd / (lLength - 1));//递归标准差公式 STD = SQRT(STD(t-1)/(NUM-1))
//    }
    //LOGY("");
    // LOGY("[AutoLED][ACCINFO][ACC]: Std:%5.1f Avg:%d Max:%d Min:%d Dif:%d", pstaccstd->fStd, (int)pstaccstd->fAvg,\
    // (int)pstaccstd->uMax, (int)pstaccstd->uMin, (int)pstaccstd->uDif);
//}

/****************************************************************
* Description: Recursion Calc rawdata info
* Input:  rawdata usNum:input number
* Return: none
******************************************************************/
static void HBD_AutoLedTrigRecursionCalc(GU32 uRawdata[2], GU16 usNum)
{
    for (GU8 i = 0; i<HBD_AUTO_LED_MAX_CHANNEL_NUM; i++)//对每个通道单独调用接口计算均值最大值最小值
    {
        //用于计算rawdata的均值
        HBD_AutoLedRecursionCalcNoSTD(uRawdata[i] & 0x1ffff, &g_stTrigCalcInfo[i].fAvg, &g_stTrigCalcInfo[i].uMin, &g_stTrigCalcInfo[i].uMax, &g_stTrigCalcInfo[i].uDif, usNum);
    }
}

/****************************************************************
* Description: Recursion Calc  Rawdata std BG AVG
* Input:  rawdata usNum:input number
* Return: none
******************************************************************/
static void HBD_AutoLedBGBufferRecursionCalc(GU32 uRawdata[2], GU16 usNum)
{
    for (GU8 i = 0; i<HBD_AUTO_LED_MAX_CHANNEL_NUM; i++)//对每个通道单独调用接口计算均值最大值最小值
    {
        //用于计算背景光
        HBD_AutoLedRecursionCalcNoSTD(uRawdata[i] >> 20 & 0x07, &g_stBufferBGInfo[i].fAvg, &g_stBufferBGInfo[i].uMin, &g_stBufferBGInfo[i].uMax, &g_stBufferBGInfo[i].uDif, usNum);
    }
}

/****************************************************************
* Description:  ppg Saturation Judge
* Input:  rawdata
* Return: none
******************************************************************/
//GU8 gs_flag1 = 0;
//GU8 gs_flag2 = 0;
//GU8 gs_flag3 = 0;
//GU8 gs_flag4 = 0;

static void HBD_AutoLedPpgSatJudge(GU32 uRawdata[2])
{
    g_stPpgSatInfo.Sat_CheckCnt++;
    GU32 Upper = 0;
    for (GU8 i = 0; i<HBD_AUTO_LED_MAX_CHANNEL_NUM; i++)//对每个通道单独调用接口计算饱和数据的个数
    {
        Upper = g_stAutoLedConfig.stAutoLedChannel[i].unTrigerUpperThr;//外部配置触发上限

        switch (g_stBGAdj.stBGAdjCh[i].uchUpperlevel)//根据背景光切换触发阈值
        {
            case 0:
            break;
            case 1://对应3-4之间的背景光
            Upper = 49000;
            break;
            case 2://对应背景光4
            Upper = 45000;
            break;
            case 3://对应强光
            Upper = 30000;
            break;
            default:
            break;
        }
        g_PpgSatThr[i]= Upper;  ////修改饱和阈值为触发上限

        GU32 ppgraw = (uRawdata[i] & 0x1ffff);
        if (ppgraw >  g_PpgSatThr[i])
        {
            g_stPpgSatInfo.Sat_cnt[i]++;
        }
        // LOGY("[AutoLED][SAT][CH%d]: ppgraw:%d, cnt:%d\r", i, ppgraw, g_stPpgSatInfo.Sat_cnt[i]);
    }

//    gs_flag1 = g_stPpgSatInfo.Sat_cnt[0];
//    gs_flag2 = g_stPpgSatInfo.Sat_cnt[1];
    GU32 SAT_CHECK_NUM_MAX = g_stSampleInfo.unSampleFre*5;    //5秒时间窗
    GU32 SAT_CNT_THRESHOLD = SAT_CHECK_NUM_MAX/16;    //  饱和判断阈值：1/16的数据

    if (g_stPpgSatInfo.Sat_CheckCnt >= SAT_CHECK_NUM_MAX)
    {
        g_stPpgSatInfo.Sat_flag = 0 ; //一次统计（5秒）之后初始化饱和标志 ，饱和标致置位的话会持续5秒
        for (GU8 j = 0; j<HBD_AUTO_LED_MAX_CHANNEL_NUM; j++)//对每个通道单独调用接口计算饱和数据的个数
        {
            if( g_stPpgSatInfo.Sat_cnt[j] >=  SAT_CNT_THRESHOLD)
            {
                g_stPpgSatInfo.Sat_flag |= (1<<j);
            }
            g_stPpgSatInfo.Sat_cnt[j] = 0;
        }
        g_stPpgSatInfo.Sat_CheckCnt = 0;
        g_stPpgSatInfo.Sat_adj_flag = g_stPpgSatInfo.Sat_flag;
    }

//   gs_flag3 = g_stPpgSatInfo.Sat_flag;
}

static void HBD_AutoLedPpgSatTrigerFlag( GU8 *uchAutoLedFlag, GU32 uRawdataAvg[2] )
{
    for (GU8 j = 0; j< g_uchNeedReadRawdataCount; j++) // 对每个通道分别看是否触发饱和调光
    {
        if (g_stAutoLedConfig.stAutoLedChannel[j].emAutoledEnable == HBD_FUNCTIONAL_STATE_ENABLE)
        {
            // 饱和且调光均值大于理想目标值则触发调光，确保电流下调
            if ((g_stPpgSatInfo.Sat_adj_flag & (1 << j)) && (uRawdataAvg[j] > g_unIdealTargetValueArr[j]))
            {
                *uchAutoLedFlag |= (1 << j);
                g_stPpgSatInfo.Sat_adj_flag &= (~(1 << j)); // 调光之后清零标志位 防止5秒内多次饱和触发调光
//              gs_flag4 |= (1<<j);
            }
        }
    }
}

/****************************************************************
* Description: soft autoled get current and gain
* Input:  uchFlag: flag of HBD_AutoLedCalcTriger function output
puchGainArr: ptr of gain output
puchGainShiftArr: ptr of gain shift output
pusLastGainMsk: ptr of gain mask output
puchCurrentArr: ptr of current output
puchCurrentShift: ptr of current shift output
punLastCurrentMsk: ptr of current mask output
* Return: None
******************************************************************/
GU8 HBD_AutoLedGetCurrentAndGain(GU8 *puchGainArr, GU8 *puchGainShiftArr, GU16 *pusLastGainMsk,
    GU8 *puchCurrentArr, GU8 *puchCurrentShift, GU32 *punLastCurrentMsk)
{
    GU8 uchIndex = 0;
    GU8 nRes = 0;

    *punLastCurrentMsk = HBD_I2cReadRawdataReg(HBD_LED17_REG_ADDR);//读取电流寄存器
    *pusLastGainMsk = HBD_I2cReadReg(HBD_LED27_REG_ADDR);//读取Gain寄存器

    for (uchIndex = 0; uchIndex < g_uchNeedReadRawdataCount; uchIndex++)
    {
        puchCurrentShift[uchIndex] = (GU8)((g_usReadRawdataRegList[uchIndex] - HBD_LED0_DATA_L_REG_ADDR) * 2);//计算通道偏移量
        puchCurrentArr[uchIndex] = (*punLastCurrentMsk >> puchCurrentShift[uchIndex]) & 0xFF;//通道电流值
        g_uchAutoLedCurrentArr[uchIndex] = puchCurrentArr[uchIndex];  //给NADT提供的电流值
        *punLastCurrentMsk &= ~(0x000000FF << puchCurrentShift[uchIndex]);//通道Mask

        puchGainShiftArr[uchIndex] = (GU8)((((g_usReadRawdataRegList[uchIndex] - HBD_LED0_DATA_L_REG_ADDR) / 4) * 3) + 4);//计算通道偏移量
        puchGainArr[uchIndex] = (*pusLastGainMsk >> puchGainShiftArr[uchIndex]) & 0x07;//通道Gain
        *pusLastGainMsk &= ~(0x000000007 << puchGainShiftArr[uchIndex]);//通道Mask
        // LOGY("[AutoLED][REG_G_C][CH%d]: G:%d C:%d", uchIndex, puchGainArr[uchIndex], puchCurrentArr[uchIndex]);
    }
    if (2 == g_uchNeedReadRawdataCount)//有通道的电流为0 返回异常
    {
        if ((0 == puchCurrentArr[0]) || (0 == puchCurrentArr[1]))
        {
            nRes = 1;
        }
    }
    else if (1 == g_uchNeedReadRawdataCount)//电流为0 返回异常
    {
        if ((0 == puchCurrentArr[0]))
        {
            nRes = 1;
        }
    }
    return nRes;
}

/****************************************************************
* Description: soft autoled BG Process
* Input:  uchGainArr: Last Gain Array
* Return: reduce gain flag
        0:No  1:Yes
******************************************************************/
static GU8 HBD_AutoLEDBGProcess(GU8 uchGainArr[2], GU32 unRawdataAvgArr[2])
{
    GF32 BGAvg = 0;
    GU8 BGMin = 0;
    GU8 uchRes = 0;

    for (GU8 uchChannelIndex = 0; uchChannelIndex < g_uchNeedReadRawdataCount; uchChannelIndex++)
    {
        if (g_stAutoLedConfig.stAutoLedChannel[uchChannelIndex].emAutoledEnable == HBD_FUNCTIONAL_STATE_ENABLE)
        {//只在通道开启调光时进行

            BGAvg = g_stBufferBGInfo[uchChannelIndex].fAvg;//均值
            BGMin = (GU8)g_stBufferBGInfo[uchChannelIndex].uMin;//最小值

            if (BGAvg >= g_stAutoLedConfig.uchBgStrongLightThr)//强光(BG5 最大范围:32768-49152)
            {
                g_unIdealTargetValueArr[uchChannelIndex] = 26000;//BG：5, Max:32768 80% -->26214

                g_unIdealTargetTrigArr[uchChannelIndex] = (GU32)(g_unIdealTargetValueArr[uchChannelIndex] / 2);
                g_unIdealTargetTrigArr[uchChannelIndex] = (g_unIdealTargetTrigArr[uchChannelIndex] > g_stAutoLedConfig.stAutoLedChannel[uchChannelIndex].unTargetLowerThr) ? \
                    g_unIdealTargetTrigArr[uchChannelIndex] : g_stAutoLedConfig.stAutoLedChannel[uchChannelIndex].unTargetLowerThr;

                if ((GU32)g_stTrigCalcInfo[uchChannelIndex].fAvg > 30000) //BG：5, Max:32768 90% -->30000
                {
                    if (uchGainArr[uchChannelIndex] >= 2)
                    {
                        uchGainArr[uchChannelIndex] -= 2;//Gain降两档
                        g_stAutoLedConfig.stAutoLedChannel[uchChannelIndex].uchGainLimitThr = uchGainArr[uchChannelIndex]; //限制Gain
                        g_uchAutoLedLastFlag |= 1 << uchChannelIndex; // 全局调光标记
                        uchRes = 1;
                    }
                    else if (0x1 == uchGainArr[uchChannelIndex]){
                        uchGainArr[uchChannelIndex] = 0;//Gain最小
                        g_stAutoLedConfig.stAutoLedChannel[uchChannelIndex].uchGainLimitThr = 0;//限制Gain
                        g_uchAutoLedLastFlag |= 1 << uchChannelIndex;// 全局调光标记
                        uchRes = 1;
                    }
                    else{//最小值 无法继续降
                        g_stAutoLedConfig.stAutoLedChannel[uchChannelIndex].uchGainLimitThr = 0;//限制Gain
                    }
                    g_stBGAdj.stBGAdjCh[uchChannelIndex].uchBGLevelCnt = 0;
                }
                else{
                    //如果上次为强光 则计数器增加
                    (HBD_AUTOLED_BG_HIGHLIGHT == g_stBGAdj.stBGAdjCh[uchChannelIndex].uchLastBGLevel) ? \
                    (g_stBGAdj.stBGAdjCh[uchChannelIndex].uchBGLevelCnt += g_stSampleInfo.unSampleFifoDuration) : \
                        (g_stBGAdj.stBGAdjCh[uchChannelIndex].uchBGLevelCnt = g_stSampleInfo.unSampleFifoDuration);
                    //周期判断
                    if (g_stBGAdj.stBGAdjCh[uchChannelIndex].uchBGLevelCnt / (g_stBGAdj.uchBGReduceGainDuration * 1000))
                    {
                        g_stAutoLedConfig.stAutoLedChannel[uchChannelIndex].uchGainLimitThr = uchGainArr[uchChannelIndex];//限制Gain
                        g_stBGAdj.stBGAdjCh[uchChannelIndex].uchBGLevelCnt = 0;//计数器清0
                        g_stBGAdj.stBGAdjCh[uchChannelIndex].uchUpperlevel = 3;//根据背景光调整触发上限为3
                    }
                }
                g_stBGAdj.stBGAdjCh[uchChannelIndex].uchLastBGLevel = HBD_AUTOLED_BG_HIGHLIGHT;//当前等级
            }
            else if (BGAvg > g_stAutoLedConfig.uchBgWeakStrongLightThr && BGAvg < g_stAutoLedConfig.uchBgStrongLightThr && BGMin >= g_stAutoLedConfig.uchBgWeakStrongLightThr)//弱强光
            {//BG4-5     Max:49151
                g_unIdealTargetValueArr[uchChannelIndex] = 39000;//BG 4-5,Max:49151 80% -->39000

                g_unIdealTargetTrigArr[uchChannelIndex] = (GU32)(g_unIdealTargetValueArr[uchChannelIndex] / 2);
                g_unIdealTargetTrigArr[uchChannelIndex] = (g_unIdealTargetTrigArr[uchChannelIndex] > g_stAutoLedConfig.stAutoLedChannel[uchChannelIndex].unTargetLowerThr) ? \
                    g_unIdealTargetTrigArr[uchChannelIndex] : g_stAutoLedConfig.stAutoLedChannel[uchChannelIndex].unTargetLowerThr;

                if ( (GU32)g_stTrigCalcInfo[uchChannelIndex].fAvg > 46000) //BG 4-5,Max:49151 90% -->46000
                {
                    if (uchGainArr[uchChannelIndex] >= 1)
                    {
                        uchGainArr[uchChannelIndex] = uchGainArr[uchChannelIndex] - 1;
                        g_stAutoLedConfig.stAutoLedChannel[uchChannelIndex].uchGainLimitThr = uchGainArr[uchChannelIndex]; //限制Gain
                        g_uchAutoLedLastFlag |= 1 << uchChannelIndex; //调光标记
                        uchRes = 1;
                    }
                    else{
                        g_stAutoLedConfig.stAutoLedChannel[uchChannelIndex].uchGainLimitThr = 0;
                    }
                    g_stBGAdj.stBGAdjCh[uchChannelIndex].uchBGLevelCnt = 0;
                }
                else{
                    //上一次为弱强光 计数器增加 否则清0
                    (HBD_AUTOLED_BG_WEAKLIGHT == g_stBGAdj.stBGAdjCh[uchChannelIndex].uchLastBGLevel) ? \
                    (g_stBGAdj.stBGAdjCh[uchChannelIndex].uchBGLevelCnt += g_stSampleInfo.unSampleFifoDuration) : \
                        (g_stBGAdj.stBGAdjCh[uchChannelIndex].uchBGLevelCnt = g_stSampleInfo.unSampleFifoDuration);
                    //周期判断
                    if (g_stBGAdj.stBGAdjCh[uchChannelIndex].uchBGLevelCnt / (g_stBGAdj.uchBGReduceGainDuration * 1000))/* 3s*/
                    {
                        g_stAutoLedConfig.stAutoLedChannel[uchChannelIndex].uchGainLimitThr = uchGainArr[uchChannelIndex];
                        g_stBGAdj.stBGAdjCh[uchChannelIndex].uchBGLevelCnt = 0;
                        g_stBGAdj.stBGAdjCh[uchChannelIndex].uchUpperlevel = 2;//根据背景光调整触发上限为2
                    }
                }
                g_stBGAdj.stBGAdjCh[uchChannelIndex].uchLastBGLevel = HBD_AUTOLED_BG_WEAKLIGHT;
            }
            else if (4 == BGAvg){ //BG=4(49152-65536)

                g_unIdealTargetValueArr[uchChannelIndex] = (GU32)((g_stAutoLedConfig.stAutoLedChannel[uchChannelIndex].unTargetLowerThr * (1-g_fIdealCalcRatio) + \
                    g_stAutoLedConfig.stAutoLedChannel[uchChannelIndex].unTargetUpperThr * g_fIdealCalcRatio) * 0.8f);

                /* Calculate Ideal Target Value, Ideal = Low + (High - Low) * 0.8 */
                g_unIdealTargetValueArr[uchChannelIndex] = (GU32)((g_stAutoLedConfig.stAutoLedChannel[uchChannelIndex].unTargetLowerThr * 0.2 + \
                    g_stAutoLedConfig.stAutoLedChannel[uchChannelIndex].unTargetUpperThr * 0.8) * 0.8);
                /* Limit Ideal Target to 40000 */ //BG is 4 MAX Rawdata 49612 - 65535
                g_unIdealTargetValueArr[uchChannelIndex] = (g_unIdealTargetValueArr[uchChannelIndex] > 40000) ? \
                    40000 : g_unIdealTargetValueArr[uchChannelIndex];

                g_unIdealTargetTrigArr[uchChannelIndex] = (GU32)(g_unIdealTargetValueArr[uchChannelIndex] / 2);
                g_unIdealTargetTrigArr[uchChannelIndex] = (g_unIdealTargetTrigArr[uchChannelIndex] > g_stAutoLedConfig.stAutoLedChannel[uchChannelIndex].unTargetLowerThr) ? \
                    g_unIdealTargetTrigArr[uchChannelIndex] : g_stAutoLedConfig.stAutoLedChannel[uchChannelIndex].unTargetLowerThr;

                (HBD_AUTOLED_BG_LEVEL4 == g_stBGAdj.stBGAdjCh[uchChannelIndex].uchLastBGLevel) ? \
                    (g_stBGAdj.stBGAdjCh[uchChannelIndex].uchBGLevelCnt += g_stSampleInfo.unSampleFifoDuration) : \
                        (g_stBGAdj.stBGAdjCh[uchChannelIndex].uchBGLevelCnt = g_stSampleInfo.unSampleFifoDuration);
                // LOGY("LastLevelCNT:%d    %d",g_stBGAdj.stBGAdjCh[uchChannelIndex].uchBGLevelCnt,g_stBGAdj.uchBGReduceGainDuration);
                if (g_stBGAdj.stBGAdjCh[uchChannelIndex].uchBGLevelCnt / (g_stBGAdj.uchBGReduceGainDuration * 1000))/* 3s */
                {
                    g_stAutoLedConfig.stAutoLedChannel[uchChannelIndex].uchGainLimitThr = uchGainArr[uchChannelIndex];
                    g_stBGAdj.stBGAdjCh[uchChannelIndex].uchBGLevelCnt = 0;
                    g_stBGAdj.stBGAdjCh[uchChannelIndex].uchUpperlevel = 1;//根据背景光调整触发上限为1
                }
                g_stBGAdj.stBGAdjCh[uchChannelIndex].uchLastBGLevel = HBD_AUTOLED_BG_LEVEL4;
            }
            else if (4 > BGAvg && 3 < BGAvg){ /* 3s, IdealTarget * 0.9 */

                g_unIdealTargetValueArr[uchChannelIndex] = (GU32)(g_stAutoLedConfig.stAutoLedChannel[uchChannelIndex].unTargetLowerThr * (1-g_fIdealCalcRatio) + \
                    g_stAutoLedConfig.stAutoLedChannel[uchChannelIndex].unTargetUpperThr * g_fIdealCalcRatio);

                /* Calculate Ideal Target Value, Ideal = Low + (High - Low) * 0.8 */
                g_unIdealTargetValueArr[uchChannelIndex] = (GU32)(g_stAutoLedConfig.stAutoLedChannel[uchChannelIndex].unTargetLowerThr * 0.2 + \
                    g_stAutoLedConfig.stAutoLedChannel[uchChannelIndex].unTargetUpperThr * 0.8);
                /* Limit Ideal Target to 50000 */
                g_unIdealTargetValueArr[uchChannelIndex] = (g_unIdealTargetValueArr[uchChannelIndex] > 50000) ? \
                    50000 : g_unIdealTargetValueArr[uchChannelIndex];

                g_unIdealTargetTrigArr[uchChannelIndex] = (GU32)(g_stAutoLedConfig.stAutoLedChannel[uchChannelIndex].unTargetLowerThr * (1 - g_fIdealTrigRatio) + \
                    g_stAutoLedConfig.stAutoLedChannel[uchChannelIndex].unTargetUpperThr * g_fIdealTrigRatio);
                g_unIdealTargetTrigArr[uchChannelIndex] = (g_unIdealTargetTrigArr[uchChannelIndex] > g_stAutoLedConfig.stAutoLedChannel[uchChannelIndex].unTargetLowerThr) ? \
                    g_unIdealTargetTrigArr[uchChannelIndex] : g_stAutoLedConfig.stAutoLedChannel[uchChannelIndex].unTargetLowerThr;

                (HBD_AUTOLED_BG_LEVEL34 == g_stBGAdj.stBGAdjCh[uchChannelIndex].uchLastBGLevel) ? \
                    (g_stBGAdj.stBGAdjCh[uchChannelIndex].uchBGLevelCnt += g_stSampleInfo.unSampleFifoDuration) : \
                    (g_stBGAdj.stBGAdjCh[uchChannelIndex].uchBGLevelCnt = g_stSampleInfo.unSampleFifoDuration);
                if (g_stBGAdj.stBGAdjCh[uchChannelIndex].uchBGLevelCnt / (g_stBGAdj.uchBGReduceGainDuration * 1000))/* 3s */
                {
                    g_stAutoLedConfig.stAutoLedChannel[uchChannelIndex].uchGainLimitThr = uchGainArr[uchChannelIndex];
                    g_stBGAdj.stBGAdjCh[uchChannelIndex].uchBGLevelCnt = 0;
                    g_stBGAdj.stBGAdjCh[uchChannelIndex].uchUpperlevel = 0;//根据背景光调整触发上限为0
                }
                g_stBGAdj.stBGAdjCh[uchChannelIndex].uchLastBGLevel = HBD_AUTOLED_BG_LEVEL34;
            }
            else if (3 >= BGAvg){

                g_unIdealTargetValueArr[uchChannelIndex] = (GU32)(g_stAutoLedConfig.stAutoLedChannel[uchChannelIndex].unTargetLowerThr * (1-g_fIdealCalcRatio) + \
                    g_stAutoLedConfig.stAutoLedChannel[uchChannelIndex].unTargetUpperThr * g_fIdealCalcRatio);

                /* Calculate Ideal Target Value, Ideal = Low + (High - Low) * 0.8 */
                g_unIdealTargetValueArr[uchChannelIndex] = (GU32)(g_stAutoLedConfig.stAutoLedChannel[uchChannelIndex].unTargetLowerThr * 0.2 + \
                    g_stAutoLedConfig.stAutoLedChannel[uchChannelIndex].unTargetUpperThr * 0.8);
                /* Limit Ideal Target to 50000 */
                g_unIdealTargetValueArr[uchChannelIndex] = (g_unIdealTargetValueArr[uchChannelIndex] > 50000) ? \
                    50000 : g_unIdealTargetValueArr[uchChannelIndex];

                g_unIdealTargetTrigArr[uchChannelIndex] = (GU32)(g_stAutoLedConfig.stAutoLedChannel[uchChannelIndex].unTargetLowerThr * (1 - g_fIdealTrigRatio) + \
                    g_stAutoLedConfig.stAutoLedChannel[uchChannelIndex].unTargetUpperThr * g_fIdealTrigRatio);
                g_unIdealTargetTrigArr[uchChannelIndex] = (g_unIdealTargetTrigArr[uchChannelIndex] > g_stAutoLedConfig.stAutoLedChannel[uchChannelIndex].unTargetLowerThr ) ? \
                    g_unIdealTargetTrigArr[uchChannelIndex] : g_stAutoLedConfig.stAutoLedChannel[uchChannelIndex].unTargetLowerThr;

                (HBD_AUTOLED_BG_NORMAL == g_stBGAdj.stBGAdjCh[uchChannelIndex].uchLastBGLevel) ? \
                   (g_stBGAdj.stBGAdjCh[uchChannelIndex].uchBGLevelCnt += g_stSampleInfo.unSampleFifoDuration) : \
                       (g_stBGAdj.stBGAdjCh[uchChannelIndex].uchBGLevelCnt = g_stSampleInfo.unSampleFifoDuration);
                if (g_stBGAdj.stBGAdjCh[uchChannelIndex].uchBGLevelCnt / (g_stBGAdj.uchBGRecoverGainDuration * 1000))
                {
                    if (7 > g_stAutoLedConfig.stAutoLedChannel[uchChannelIndex].uchGainLimitThr)
                    {
                        ++g_stAutoLedConfig.stAutoLedChannel[uchChannelIndex].uchGainLimitThr;
                    }
                    g_stBGAdj.stBGAdjCh[uchChannelIndex].uchBGLevelCnt = 0;
                    g_stBGAdj.stBGAdjCh[uchChannelIndex].uchUpperlevel = 0;//根据背景光调整触发上限为0
                }
                g_stBGAdj.stBGAdjCh[uchChannelIndex].uchLastBGLevel = HBD_AUTOLED_BG_NORMAL;
            }
            // LOGY("[AutoLED][BG_PROC][CH%d]: BG:%f LastL:0x%x CNT:%d GainL:%d Ideal:%d UpperTrig:0x%x", uchChannelIndex, BGAvg, g_stBGAdj.stBGAdjCh[uchChannelIndex].uchLastBGLevel, \
            //  g_stBGAdj.stBGAdjCh[uchChannelIndex].uchBGLevelCnt,g_stAutoLedConfig.stAutoLedChannel[uchChannelIndex].uchGainLimitThr,\
            //  (int)g_unIdealTargetValueArr[uchChannelIndex], g_stBGAdj.stBGAdjCh[uchChannelIndex].uchUpperlevel);
        }
    }
    return uchRes;
}

/****************************************************************
* Description: soft autoled Motion Detection
* Input:  unRawdata rawdata
* Return: uchMotionLevel 0-Nomotion
*         HBD_AUTOLED_MOTION_LEVEL_0                  (1<<0)
*         HBD_AUTOLED_MOTION_LEVEL_1                  (1<<1)
*         HBD_AUTOLED_MOTION_LEVEL_2                  (1<<2)
*         HBD_AUTOLED_MOTION_LEVEL_3                  (1<<3)
*         HBD_AUTOLED_MOTION_LEVEL_4                  (1<<4)
******************************************************************/
//GU8 HBD_AutoLedMotionLevelDetection(void)
//{
//    //ACC信息获取
//    GU16 uACCAvg = (GU16)g_stAutoLedAccInfo.fAvg;
//    GU16 uACCStd = (GU16)g_stAutoLedAccInfo.fStd;
//    GU16 uACCMin = (GU16)g_stAutoLedAccInfo.uMin;
//    GU16 uACCMax = (GU16)g_stAutoLedAccInfo.uMax;
//    GU16 uACCDiff = uACCMax - uACCMin;
//    GU8 uchMotionLevel = 0;

//    if (uACCStd <= g_stMotionThold.uDailyACCStdThr)//日常
//    {
//        if (uACCDiff < g_stMotionThold.uRestACCDiffThr){
//            uchMotionLevel = HBD_AUTOLED_MOTION_LEVEL_0; /* 静息*/
//        }
//        else{
//            uchMotionLevel = HBD_AUTOLED_MOTION_LEVEL_1; /* 日常*/
//        }
//    }
//    else if (g_stMotionThold.uDailyACCStdThr < uACCStd && g_stMotionThold.uWalkACCStdThr >= uACCStd){
//        uchMotionLevel = HBD_AUTOLED_MOTION_LEVEL_2;//步行
//    }
//    else{
//        uchMotionLevel = HBD_AUTOLED_MOTION_LEVEL_4;//运动场景
//    }

//    if (uchMotionLevel == HBD_AUTOLED_MOTION_LEVEL_2)
//    {
//        if (uACCAvg >= g_stMotionThold.uTrotACCAvgThr) /* 快走 */
//        {
//            uchMotionLevel = HBD_AUTOLED_MOTION_LEVEL_3;
//        }
//    }

//    if (uchMotionLevel == HBD_AUTOLED_MOTION_LEVEL_4)
//    {
//        if (uACCMax >= g_stMotionThold.uDashACCMaxThr) /* 剧烈运动 */
//        {
//            uchMotionLevel = HBD_AUTOLED_MOTION_LEVEL_5;
//        }
//    }
//    return uchMotionLevel;//运动等级返回
//}

/****************************************************************
* Description: soft autoled saturation process
* Input:   None
* Return:  None
******************************************************************/

static void HBD_AutoLedSatProcess( )
{
    /*无数据饱和理想目标值提调光触发阈值高*/
    /*  数据饱和理想目标值提调光触发阈值降低*/
    GU8 uchChannelIndex = 0;
    GU8 n = 0;//系数
    GU32 unTargetTemp = 0;

    for (uchChannelIndex = 0; uchChannelIndex < g_uchNeedReadRawdataCount; uchChannelIndex++)
    {
       if (g_stPpgSatInfo.Sat_flag & (1<<uchChannelIndex)) // 如果数据饱和
       {
            n = 2;  // 将减小理想目标调光触发阈值
       }
       else
       {//计数器归0并累加一个fifo时间长度
            n = 0;
       }

       gfMotionTrigRatio[uchChannelIndex] = n * g_fMotionTrgIdealExtraRatio[uchChannelIndex];

       //Motion: Triger threshold = TargetLow + (TargetHigh - TargetLow) * (g_fIdealTrigRatio - n * g_fMotionTrgIdealExtraRatio)
       if (g_fIdealTrigRatio > 0.0f)
       {
           unTargetTemp = (GU32)(g_stAutoLedConfig.stAutoLedChannel[uchChannelIndex].unTargetLowerThr * (1 - g_fIdealTrigRatio + gfMotionTrigRatio[uchChannelIndex]) + \
               g_stAutoLedConfig.stAutoLedChannel[uchChannelIndex].unTargetUpperThr * (g_fIdealTrigRatio - gfMotionTrigRatio[uchChannelIndex]));
           //Motion: Triger threshold < TargetLow    Triger threshold = TargetLow
           unTargetTemp = (unTargetTemp > g_stAutoLedConfig.stAutoLedChannel[uchChannelIndex].unTargetLowerThr) ? \
               unTargetTemp : g_stAutoLedConfig.stAutoLedChannel[uchChannelIndex].unTargetLowerThr;
           //Motion: Triger threshold < BG  g_unIdealTargetTrigArr;
       }
       else {
           unTargetTemp = (GU32)(g_stAutoLedConfig.stAutoLedChannel[uchChannelIndex].unTargetLowerThr);
       }

       if (unTargetTemp < g_unIdealTargetTrigArr[uchChannelIndex])
       {
           g_unIdealTargetTrigArr[uchChannelIndex] = unTargetTemp;
       }
    }

    // LOGY("[AutoLED][MT_PROC][TRG]: TargetTrigArr1:%d TargetTrigArr2:%d ", g_unIdealTargetTrigArr[0], g_unIdealTargetTrigArr[1]);
    // LOGY("[AutoLED][MT_PROC][ACC]: SatFlag:0x%x ", g_stPpgSatInfo.Sat_flag);
    // LOGY("[AutoLED][MT_PROC][CHA]: Channel[0]Ratio:%3.2f Channel[1]Ratio:%3.2f ", gfMotionTrigRatio[0], gfMotionTrigRatio[1]);
}

/****************************************************************
* Description: soft autoled clac  rawdatatrigger
* Input:  g_stTrigCalcInfo
* Return: trigger flag:
        bit0-PPG1 bit1-PPG2
******************************************************************/
static GU8 HBD_AutoLedCalcTrigerFlag(void)
{
    GU8 uchRet = 0;
    GU8 uchChannelIndex = 0;
    GU32 Lower = 0;
    for (uchChannelIndex = 0; uchChannelIndex < g_uchNeedReadRawdataCount; uchChannelIndex++)
    {
        Lower = g_stAutoLedConfig.stAutoLedChannel[uchChannelIndex].unTrigerLowerThr;//外部配置触发下限

        if (g_stAutoLedConfig.stAutoLedChannel[uchChannelIndex].emAutoledEnable == HBD_FUNCTIONAL_STATE_ENABLE)
        {
            //开启调光才计算
            if ((GU32)g_stTrigCalcInfo[uchChannelIndex].fAvg > g_PpgSatThr[uchChannelIndex])//大于上限
            {
                uchRet |= (1 << uchChannelIndex);//上触发
            }
            else if ((GU32)g_stTrigCalcInfo[uchChannelIndex].fAvg < Lower)//小于下限
            {
                uchRet |= (1 << uchChannelIndex);//下触发
            }
        }
        // LOGY("[AutoLED][TRG_R_F][CH%d]: FLAG:%d AVG:%05d MIN:%05d MAX:%05d THR:%05d", uchChannelIndex, uchRet >> uchChannelIndex & 0x1, \
        //      (int)g_stTrigCalcInfo[uchChannelIndex].fAvg, (int)g_stTrigCalcInfo[uchChannelIndex].uMin, (int)g_stTrigCalcInfo[uchChannelIndex].uMax, (int)Lower);
    }
    return uchRet;
}

/****************************************************************
* Description: Buffer Rawdata Avg calculator
* Output: Rawdata Average
* Return: none
******************************************************************/
static GU8 HBD_AutoLedRawdataAvgDeal(GU32 uRawdataAvg[2])
{
    GU8 nRes = 0;
    for (GU8 i = 0; i<g_uchNeedReadRawdataCount; i++)
    {
      if (g_stAutoLedConfig.stAutoLedChannel[i].emAutoledEnable == HBD_FUNCTIONAL_STATE_ENABLE)
      {
        if (g_stTrigCalcInfo[i].fAvg == 0.0f)//Rawdata is 0
        {
            return 1;
        }

        /* Average*/
        //数据饱和时(饱和标志一旦置位5秒切换一次状态)，考虑大值对结果的影响  调光均值计算偏大  一旦数据饱和就生效(优先限制数据饱和)
        if ( (g_stPpgSatInfo.Sat_flag & (1<<i))  /* && (g_stTrigCalcInfo[i].uMax >  (GU32)(0.9f*g_PpgSatThr[i])) */ )
        {
            // LOGY("[AutoLED][Buf_AVG][CH%d]: 1 \n", i);
            uRawdataAvg[i] = (GU32)(g_stTrigCalcInfo[i].uMin * g_CalcAvgRatioInMotion.fMinRatio + g_stTrigCalcInfo[i].uMax * g_CalcAvgRatioInMotion.fMaxRatio);
                   //限制调光均值计算过大
             uRawdataAvg[i] = ( uRawdataAvg[i] > (GU32)(3*(GU32)g_stTrigCalcInfo[i].fAvg)) ? (GU32)(3*(GU32)g_stTrigCalcInfo[i].fAvg) : uRawdataAvg[i];
        }
        //数据没饱和时
        else
        {
            // LOGY("[AutoLED][Buf_AVG][CH%d]: 2 \n", i);
            //除0保护
            if (g_stTrigCalcInfo[i].uDif == 0) {
                uRawdataAvg[i] = (GU32)g_stTrigCalcInfo[i].fAvg;
            }
            //正常分支
            else{
                //Average = (Max-Avg)/Dif*MIN + (MIN-Avg)/Dif*MAX
                uRawdataAvg[i] = (GU32)((g_stTrigCalcInfo[i].uMax - (GU32)g_stTrigCalcInfo[i].fAvg)*g_stTrigCalcInfo[i].uMin / g_stTrigCalcInfo[i].uDif + \
                    ((GU32)g_stTrigCalcInfo[i].fAvg - g_stTrigCalcInfo[i].uMin)*g_stTrigCalcInfo[i].uMax / g_stTrigCalcInfo[i].uDif);
            }
        }
        // LOGY("[AutoLED][Buf_AVG][CH%d]: AVG:%05d", i, (int)uRawdataAvg[i]);
            }
    }
    return nRes;
}

/****************************************************************
* Description: Confirm Write Register Base on Rawdata
* Input:  uchCalcGain  uchCalCurrent: Calculator result
*         uchGainArr  uchCurrentArr: Last situation
*         uchchannel: Channel Index
* Return:0-Not 1-Yes
******************************************************************/
static GU8 HBD_AutoLedWriteRegConfirm(GU8 uchCalcGain, GU8 uchCalCurrent, GU8 uchGainArr[2], GU8 uchCurrentArr[2], GU8 uchchannel)
{
    GF32 fratio = 0.0f;

    if (uchCurrentArr[uchchannel] != 0)//计算提升rawdata还是降低rawdata
    {//Gain*电流 判断向上调节还是向下调节 之前一段时间有触发上限 不能升
        fratio = 1.0f * g_usGainArrEnum[uchCalcGain] * uchCalCurrent / (g_usGainArrEnum[uchGainArr[uchchannel]] * uchCurrentArr[uchchannel]);
    }

    //是否可以提升
    // if ((GU32)g_stTrigCalcInfo[uchchannel].fAvg > (g_unIdealTargetValueArr[uchchannel] / (1-g_fTriggerIdealAdjustRatio[uchchannel])) && fratio >= 1.0f)
    if ((g_stTrigCalcInfo[uchchannel].fAvg > ((GF32)g_unIdealTargetValueArr[uchchannel] / 0.9f)) && fratio >= 1.0f)
    {
        g_stAutoledWrtRegCheck.unWrtRegNormDuration[uchchannel] = 0;
    }
    //是否可以降低 低于下限 不能降
    else if ((GU32)g_stTrigCalcInfo[uchchannel].fAvg < (g_stAutoLedConfig.stAutoLedChannel[uchchannel].unCompatibleTakeoffThr / 0.9) && fratio < 1.0f)
    {
        g_stAutoledWrtRegCheck.unWrtRegNormDuration[uchchannel] = 0;
    }

     // 数据饱和且均值大于下限时，不允许上调
    if((g_stPpgSatInfo.Sat_flag & (1<<uchchannel))&& ((GU32)g_stTrigCalcInfo[uchchannel].fAvg > g_stAutoLedConfig.stAutoLedChannel[uchchannel].unTargetLowerThr))
    {
        if(g_usGainArrEnum[uchGainArr[uchchannel]]*uchCurrentArr[uchchannel] < uchCalCurrent*g_usGainArrEnum[uchCalcGain])
        {
           return 0;
        }
    }

    // LOGY("[AutoLED][REG_CFM][CH%d]: RegCnt:%d RegThr:%d RegG:%d CalG:%d RegC:%d CalC:%d RawAVG:%d Ideal:%d TrigR:%f Ratio:%f", uchchannel,\
    //  g_stAutoledWrtRegCheck.unWrtRegNormDuration[uchchannel], g_stAutoledWrtRegCheck.unWrtRegThres,\
    //  uchGainArr[uchchannel],uchCalcGain,uchCurrentArr[uchchannel],uchCalCurrent,\
    //  (int)g_stTrigCalcInfo[uchchannel].fAvg, (int)g_unIdealTargetValueArr[uchchannel],\
    //  gfMotionTrigRatio[uchchannel],fratio);

    return (GU8)(g_stAutoledWrtRegCheck.unWrtRegNormDuration[uchchannel] / g_stAutoledWrtRegCheck.unWrtRegThres);//返回标记
}

#define AutoLEDABSF(x) ((x)>=0?(x):-(x))
/****************************************************************
* Description: Adjust Rawdata to Ideal Target
* Input:  uchFlag- AutoLEDflag
        LastCurrent&Gain
        RawdataAvg
* Return: uchAdjFlag
        0-No 1-Adjust
******************************************************************/
static GU8 HBD_AutoLedAdjustRawdataToIdeal(GU8 uchFlag, GU8 uchGainArr[2], GU8 uchGainShiftArr[2], GU16 usLastGainMsk,
    GU8 uchCurrentArr[2], GU8 uchCurrentShift[2], GU32 unLastCurrentMsk, GU32 unRawdataAvgArr[2])
{
    GU8 uchTrigFlg = 0;;//触发比例
    GU8 uchChannelIndex = 0;//通道索引
    GU16 uchCalcCurrent = 0;//计算电流
    GU8 uchCalcGain = 0;//计算Gain
    GU8 uchCurrentLimit = 0;//功耗限制
    GU8 uchGainLimit = 0;//gain限制(因为背景光
    GU8 uchAdjFlag = 0;//调节标记
    GU8 uchtrigger = 0;//触发理想目标值调节标记

    for (uchChannelIndex = 0; uchChannelIndex < g_uchNeedReadRawdataCount; uchChannelIndex++)
    {
        uchtrigger = 0;
        if (!(uchFlag >> uchChannelIndex & 0x1))
        {
            if (g_stAutoLedConfig.stAutoLedChannel[uchChannelIndex].emAutoledEnable == HBD_FUNCTIONAL_STATE_ENABLE)
            {
                //限制调光触发下阈值不能太大
                g_unIdealTargetTrigArr[uchChannelIndex] = (g_unIdealTargetTrigArr[uchChannelIndex] > 0.8f *g_unIdealTargetValueArr[uchChannelIndex]) ? \
                    0.8f *g_unIdealTargetValueArr[uchChannelIndex]  : g_unIdealTargetTrigArr[uchChannelIndex];
                uchCurrentLimit = g_stAutoLedConfig.stAutoLedChannel[uchChannelIndex].uchCurrentLimit; //Limit Current Max
                uchGainLimit = g_stAutoLedConfig.stAutoLedChannel[uchChannelIndex].uchGainLimitThr; //Limit Gain Max
                //fRatio = AutoLEDABSF((unRawdataAvgArr[uchChannelIndex] * 1.0f / g_unIdealTargetValueArr[uchChannelIndex]) - 1);
                uchTrigFlg = (unRawdataAvgArr[uchChannelIndex] > g_unIdealTargetTrigArr[uchChannelIndex]) ? 0 : 1;

                //if (fRatio > gfMotionTrigRatio[uchChannelIndex]) /* 大于理想目标值调节比例(此处叠加了运动的影响,所以为Motion) */
                if (uchTrigFlg)
                {
                    /* 计算是否进入调节 */
                    if (!g_stIdealAdjust.stIdealAdjustCh[uchChannelIndex].uchFirstTrigger) /* 是否第一次触发*/
                    {
                        uchtrigger = 1;
                        g_stIdealAdjust.stIdealAdjustCh[uchChannelIndex].uchLastIdealTrigger = 1;
                        g_stIdealAdjust.stIdealAdjustCh[uchChannelIndex].uchFirstTrigger = 1;
                    }
                    else
                    {
                        if (g_stIdealAdjust.stIdealAdjustCh[uchChannelIndex].uchLastIdealTrigger) /* 累加到周期值*/
                        {
                            g_stIdealAdjust.stIdealAdjustCh[uchChannelIndex].uchTriggerCnt += g_stSampleInfo.unSampleFifoDuration;
                            uchtrigger = (GU8)(g_stIdealAdjust.stIdealAdjustCh[uchChannelIndex].uchTriggerCnt / g_stIdealAdjust.uchTriggerDuration / 1000);//是否触发标记
                        }
                        else{
                            uchtrigger = 0;
                        }
                        g_stIdealAdjust.stIdealAdjustCh[uchChannelIndex].uchLastIdealTrigger = 1;
                    }

                    if (uchtrigger)
                    {
                        g_stIdealAdjust.stIdealAdjustCh[uchChannelIndex].uchTriggerCnt = 0; /* Enter adjusting, Counter set to zero */
                        if (uchGainArr[uchChannelIndex] >= uchGainLimit || uchCurrentArr[uchChannelIndex] >= uchCurrentLimit)
                        {
                            //此时为DC量过低时的兼容分支，不进行理想目标值处理
                        }
                        else {
                            if ((unRawdataAvgArr[uchChannelIndex] != 0) && (uchCurrentArr[uchChannelIndex] != 0))
                            {
                                //按数据比例计算电流
                                uchCalcCurrent = (GU16)(uchCurrentArr[uchChannelIndex] * g_unIdealTargetValueArr[uchChannelIndex] * 1.0f / unRawdataAvgArr[uchChannelIndex]);//fixed 210307//div0
                                if (!uchCalcCurrent)//避免算出0
                                {
                                    uchCalcCurrent = (GU16)(uchCurrentArr[uchChannelIndex]);//fixed 210324 !!!CalCurrent will not equals zero
                                }
                            }
                            else {//set to last current
                                uchCalcCurrent = (GU16)(uchCurrentArr[uchChannelIndex]);//fixed 210324 !!!CalCurrent will not equals zero
                            }
                            uchCalcGain = uchGainArr[uchChannelIndex];
                            /* CalcCurrent bigger than limit, Gain++ */
                            if (uchCalcCurrent > uchCurrentLimit)//fixed 21.01.06//fixed 210308
                            {//大于功耗限制，增大Gain，重新计算电流
                                if (uchCalcGain < uchGainLimit)
                                {
                                    ++uchCalcGain;
                                    if (unRawdataAvgArr[uchChannelIndex] != 0)
                                    {
                                        //Gain++后计算电流
                                        uchCalcCurrent = (GU16)(((GF32)g_unIdealTargetValueArr[uchChannelIndex] * g_usGainArrEnum[uchCalcGain] * uchCurrentArr[uchChannelIndex])\
                                            / (g_usGainArrEnum[uchCalcGain + 1] * unRawdataAvgArr[uchChannelIndex]));  //Gain++, calc current
                                        if(uchCalcCurrent > uchCurrentLimit)
                                        {
                                            uchCalcCurrent = uchCurrentLimit;
                                        }
                                    }
                                    else
                                    {
                                        uchCalcCurrent = (GU16)(uchCurrentArr[uchChannelIndex]);
                                    }
                                }
                                else
                                {
                                    uchCalcCurrent = uchCurrentLimit; //电流最大为功耗限制值
                                }
                            }
                            if (uchCalcGain != uchGainArr[uchChannelIndex] || uchCalcCurrent != uchCurrentArr[uchChannelIndex])
                            {
                                if (HBD_AutoLedWriteRegConfirm(uchCalcGain, (GU8)uchCalcCurrent, uchGainArr, uchCurrentArr, uchChannelIndex))
                                {
                                    uchGainArr[uchChannelIndex] = uchCalcGain;
                                    uchCurrentArr[uchChannelIndex] = (GU8)uchCalcCurrent;
                                    uchAdjFlag = 1;
                                    g_uchAutoLedLastFlag |= 1 << uchChannelIndex; /* 全局调光标记*/
                                }
                            }
                            // Gain Current is ok!
                        }
                    }
                }
                else
                {
                    g_stIdealAdjust.stIdealAdjustCh[uchChannelIndex].uchLastIdealTrigger = 0;
                    g_stIdealAdjust.stIdealAdjustCh[uchChannelIndex].uchTriggerCnt = 0;
                    //复位后第一次没触发理想值调节则不再进行第一次触发判断，避免初期频繁调光影响出值时间
                    if (!g_stIdealAdjust.stIdealAdjustCh[uchChannelIndex].uchFirstTrigger)
                    {
                        g_stIdealAdjust.stIdealAdjustCh[uchChannelIndex].uchFirstTrigger = 1; //Only for first ideal adj, after start sample,21.01.06
                    }
                }
            }
            // LOGY("[AutoLED][ID_PROC][CH%d]: Cnt:%d Last:%d Tri:%d G:%d C:%d ", uchChannelIndex, g_stIdealAdjust.stIdealAdjustCh[uchChannelIndex].uchTriggerCnt, \
            //     g_stIdealAdjust.stIdealAdjustCh[uchChannelIndex].uchLastIdealTrigger, uchtrigger, \
            //     (int)uchGainArr[uchChannelIndex], (int)uchCurrentArr[uchChannelIndex]);
        }
    }
    return uchAdjFlag;
}

/****************************************************************
* Description: soft autoled clac fIpd consider saturation
* Input:  Gain Current Factor
* Return: fIpd
******************************************************************/
static float HBD_AutoLedfIpdSaturation(GU8 uchGain, GU8 uchCurrent, GU32 unRawdataAvgArr, GU8 uchAmp)
{
    float fSatRatio = 0.0f;
    float fIpd = 0.0f;
    fSatRatio = ((float)unRawdataAvgArr - 60000.0f) / 60000.0f;//只考虑过饱和的情况
    fSatRatio = fSatRatio > 0.0f ? 1 + uchAmp * fSatRatio : 1.0f; // 只针对过饱和，其它情况不考虑
    if ((uchCurrent != 0) && (unRawdataAvgArr != 0))
    {
        //Ipd = Raw/(Gain*Current)
        fIpd = (GF32)unRawdataAvgArr / g_usGainArrEnum[uchGain] / uchCurrent * fSatRatio;//Ipd计算
    }
    else
    {
        fIpd = 0.0f;
    }
    // LOGY("[AutoLED][Ipd_Val][IPD]: SatRatio:%05.03f fIpd:%05.03f", fSatRatio, fIpd);
    return fIpd;
}

/****************************************************************
* Description: soft autoled Direct calculate current
* Input:  uchchindex- index of channel fIpd- fIpdval uchGain- LastGain
        uchcurrent- LastCurrent      uchlimitcurLimit- Current limit
        unidealtarget- Ideal target  unrawedataavg- last average rawdata
        Ouput: usPrefCurrent- preffered current
* Return: uchCalcFlag 0-ok 1-Gain-- 2-Gain++ 3-cali current > max Gain++
        4-cali current < min Gain-- 5-fail
******************************************************************/
static GU8 HBD_AutoLedDirectCalculate(GU8 uchchindex, GF32 fIpd, GU8 uchGain, GU8 uchcurrent, GU16 *usPrefCurrent, GU8 uchlimitcur,GU32 unidealtarget, GU32 unrawedataavg)
{
    GU16 usCalcCur = 0;
    GU8 unCase = HBD_AUTO_LED_DC_OK;
    if (HBD_PTR_NULL == usPrefCurrent)
    {
        return HBD_AUTO_LED_DC_FAIL; //empty pointer
    }

    if (fIpd != 0)
    {
        usCalcCur = (GU16)(unidealtarget / fIpd / g_usGainArrEnum[uchGain]);//计算电流，注意该处为U16
    }
    else
    {
        usCalcCur = uchcurrent;//error, result equals last value
    }

    if (usCalcCur > uchlimitcur)
    {
        unCase = HBD_AUTO_LED_DC_CALC_G_PLUS;  //cali current > max Gain++
    }
    else if (usCalcCur < 2)
    {
        unCase = HBD_AUTO_LED_DC_CALC_G_MINUS;  //cali current < min Gain--
    }

    *usPrefCurrent = usCalcCur;
    // LOGY("[AutoLED][DC_CALC][RES]: CalcCur:%d Case:0x%x", usCalcCur, unCase);
    return unCase;
}

/****************************************************************
* Description: soft autoled dichotomy calculate current
* Input:  uchchindex- index of channel fIpd- fIpdval  uchGain- LastGain
          uchcurrent- LastCurrent       uchlimitcurLimit- Current limit uchLimitGain- Gain limit
          unidealtarget- Ideal target  unrawedataavg- last average rawdata
  Ouput:  usPrefCurrent- preffered current
* Return: uchCompatibleModeFlag
          0- No need Compatible 1- config < upper
          2-
******************************************************************/
static GU8 HBD_AutoLedDirectCalculateJudge(GU8 uchChannelIndex, GF32 fIpd, GU8 *uchGain, GU8 uchcurrent, GU16 *usPrefCurrent, GU8 uchLimitCurrent, GU8 uchLimitGain, GU32 unidealtarget, GU32 unrawedataavg)
{
    GU8 uchCalcFlag = 0;
    GU8 uchCalcGain = *uchGain;//另计算Gain初值为当前寄存器的值
    GU8 uchCompatibleModeFlag = HBD_AUTO_LED_COMPATIBLE_NONE;
    uchCalcFlag = HBD_AutoLedDirectCalculate(uchChannelIndex, fIpd, uchCalcGain, uchcurrent, usPrefCurrent, \
        uchLimitCurrent, unidealtarget, unrawedataavg);//判断结果
    // LOGY("[AutoLED][DC_CALC][1st]: Flag:0x%x Target:%05d CurL:%d", uchCalcFlag, (int)unidealtarget, uchLimitCurrent);
    while (uchCalcFlag)//当结果不为0 根据结果选择Gain++ 或者Gain-- 后重新计算
    {
        // LOGY("[AutoLED] [Loop uchCalcFlagDC:0x0%x]", uchCalcFlag);
        switch (uchCalcFlag)
        {
            case HBD_AUTO_LED_DC_CALC_G_MINUS:/* current:1 */
                if (uchCalcGain > 0)
                {
                    uchCalcFlag = HBD_AutoLedDirectCalculate(uchChannelIndex, fIpd, --uchCalcGain, uchcurrent, usPrefCurrent,\
                        uchLimitCurrent, unidealtarget, unrawedataavg);
                }
                else{ /* current:1 Gain:0 */
                    /* whether in [Ideal Upper], Yes ->OK, No ->Fail */
                    uchCompatibleModeFlag = HBD_AUTO_LED_COMPATIBLE_CASE1;
                }
                break;
            case HBD_AUTO_LED_DC_CALC_G_PLUS:/* clacCurrent:[Limit     ] */
                if (uchCalcGain < uchLimitGain)/* Gain:[0 Limit] clacCurrent: [Limit  ] */
                {
                    uchCalcFlag = HBD_AutoLedDirectCalculate(uchChannelIndex, fIpd, ++uchCalcGain, uchcurrent, usPrefCurrent,\
                        uchLimitCurrent, unidealtarget, unrawedataavg);
                }
                else{/* Gain:Limit clacCurrent: [Limit]*/
                    /* whether in [Compatibleline Ideal], Yes ->OK, No ->Fail */
                    uchCompatibleModeFlag = HBD_AUTO_LED_COMPATIBLE_CASE2;
                }
                break;
            case HBD_AUTO_LED_DC_FAIL:
                    /* Sensor Error, Soft Reset*/
                    uchCompatibleModeFlag = HBD_AUTO_LED_COMPATIBLE_FAIL;
                break;
            default:
                    /* Sensor Error, Soft Reset*/
                    //uchCompatibleModeFlag = HBD_AUTO_LED_COMPATIBLE_FAIL;
                break;
        }
        *uchGain = uchCalcGain;
        if (uchCompatibleModeFlag)
        {
            break;
        }
    }
    // LOGY("[AutoLED][DC_CALC][FLG]: CompatibleModeFlag:0x%x", uchCompatibleModeFlag);
    return uchCompatibleModeFlag;
}

/****************************************************************
* Description: soft autoled dichotomy calculate current
* Input:
* Return: uchRes： 0-Ok 0xFF-Fail
******************************************************************/
static GU8 HBD_AutoLedCompatibleMode(GU8 uchcmpflag, GU8 uchChannelIndex, GF32 fIpd, GU16 *usPrefCurrent, GU8 *usPrefGain, GU8 uchLimitCurrent, \
    GU8 uchLimitGain, GU32 uIdealRawdata, GU32 uRawdataAvg, GU32 lnTakeoff, GU8 uchGain, GU8 uchCur)
{
    GU8 uchRes = 0;
    GU32 uCalcRawdata = 0;
    GU32 uUpperThrBasedBG = 0;
    //在强光下，修改触发上限
    if (HBD_AUTOLED_BG_LEVEL4 <= g_stBGAdj.stBGAdjCh[uchChannelIndex].uchLastBGLevel)
    {
        uUpperThrBasedBG = (GU32)(g_stAutoLedConfig.stAutoLedChannel[uchChannelIndex].unTargetUpperThr * 0.75);  //49618/65536
    }
    else{
        uUpperThrBasedBG = g_stAutoLedConfig.stAutoLedChannel[uchChannelIndex].unTargetUpperThr;//fix20210113
    }

    switch (uchcmpflag)
    {
        /* current:1 Gain:0, whether in [Ideal Upper], Yes ->OK, No ->Fail*/
        case HBD_AUTO_LED_COMPATIBLE_CASE1: //Enter condition: Current=1 Gain=0
            uCalcRawdata = (GU32)(fIpd * 0x1 * g_usGainArrEnum[0]);
            if (uCalcRawdata < uUpperThrBasedBG)
            {
                *usPrefCurrent = 1;
                *usPrefGain = 0;
            }
            else{
                uchRes = HBD_AUTO_LED_COMPATIBLE_CASE1; // CTR too High, Failed
            }
            break;

        /* Gain:Limit clacCurrent:Limit, whether in [Compatibleline Ideal], Yes ->OK, No ->Fail*/
        case HBD_AUTO_LED_COMPATIBLE_CASE2: //电流达到限制值，Gain等于限制值
            if (uchCur == 0xFF && uchGain == uchLimitGain)//最大电流 最大限制Gain
            {
                if (uRawdataAvg < lnTakeoff)
                {
                    uchRes = HBD_AUTO_LED_COMPATIBLE_CASE2;//最大电流最大限制Gain,小于脱落阈值 失败
                }
                else{//设置为最大电流最大限制Gain
                    *usPrefCurrent = 0xFF;
                    *usPrefGain = uchLimitGain;
                }
            }
            else{
                uCalcRawdata = (GU32)(fIpd * uchLimitCurrent * g_usGainArrEnum[uchLimitGain]);//用限制值计算rawdata
                if (uCalcRawdata >= lnTakeoff)//大于脱落阈值，ok
                {
                    *usPrefCurrent = uchLimitCurrent;
                    *usPrefGain = uchLimitGain;
                }
                else{//小于脱落阈值，提升电流兼容
                    uCalcRawdata = (GU32)(fIpd * 0xFF * g_usGainArrEnum[uchLimitGain]);
                    if (uIdealRawdata < uCalcRawdata) /* find the Gain, make the rawdata >Idealtarget */
                    {
                        *usPrefCurrent = (GU16)(uIdealRawdata / fIpd / g_usGainArrEnum[uchLimitGain]);
                        *usPrefGain = uchLimitGain;
                    }
                    else{//不论是否大于脱落阈值都重新写入最大值，用于二次确认
                        *usPrefCurrent = 0xFF;
                        *usPrefGain = uchLimitGain;
                    }
                }
            }
            break;

            /* sensor error or no mem */
        case HBD_AUTO_LED_COMPATIBLE_FAIL:
            /* code */
            uchRes = HBD_AUTO_LED_COMPATIBLE_FAIL;
            break;
        default:
            uchRes = 0;
            break;
    }
    // LOGY("[AutoLED][COMPATI][CH%d]: Flag:0x%x Result:0x%x TakeoffThold:%d CalcG:%d CalcC:%d", uchChannelIndex, uchcmpflag, uchRes, lnTakeoff, *usPrefGain, *usPrefCurrent);
    return uchRes;
}

/****************************************************************
* Description: soft autoled clac calc gain current
* Input:  None
* Return: trigger flag
******************************************************************/
GU8 HBD_AutoLedCalcGainCurrent(GU8 uchFlag, GU8 uchGainArr[2], GU8 uchGainShiftArr[2], GU16 usLastGainMsk,
    GU8 uchCurrentArr[2], GU8 uchCurrentShift[2], GU32 unLastCurrentMsk, GU32 unRawdataAvgArr[2])
{
    GF32 fIpdVal = 0.0f;
    GU8 uchChannelIndex = 0;
    GU16 usPreferredCalcCurrent = 0;
    GU8 uchCalcGain = 0;
    GU8 uchCurrentLimit = 0;
    GU8 uchGainLimit = 0;
    GU8 uchCompatibleFlag = 0;
    GU8 uchCompatibleResult = 0;
    GU32 lnAutoLEDTakeoffThold = 0;
    g_uchAutoLedErrorFlag = 0;

     /*第一步理想目标值触发判断 当有通道非rawdata触发上下限时进入 */
    if (0x3 != uchFlag)
    {
        if (HBD_AutoLedAdjustRawdataToIdeal(uchFlag, uchGainArr, uchGainShiftArr, usLastGainMsk,
            uchCurrentArr, uchCurrentShift, unLastCurrentMsk, unRawdataAvgArr) && 0 == uchFlag)
        {
            HBD_AutoLedWriteReg(uchGainArr, uchGainShiftArr, usLastGainMsk,
                uchCurrentArr, uchCurrentShift, unLastCurrentMsk);
            return g_uchAutoLedErrorFlag;
        }
    }
    // LOGY("[AutoLED][GAIN][CURRECNT] uchFlag = 0x%x \n", uchFlag);
    /* rawdata触发上下限处理 */
    if ((uchFlag & 0x03) != 0)
    {
        for (uchChannelIndex = 0; uchChannelIndex < g_uchNeedReadRawdataCount; uchChannelIndex++)
        {
            usPreferredCalcCurrent = 0;
            uchCalcGain = 0;
            /* 在特殊应用中，如耳机，修改脱落阈值 */
            if (g_stBGAdj.uchTurnOnNotReturnFail)
            {
                if(g_stBGAdj.stBGAdjCh[uchChannelIndex].uchLastBGLevel >= HBD_AUTOLED_BG_LEVEL4)
                {
                    lnAutoLEDTakeoffThold = (g_stAutoLedConfig.stAutoLedChannel[uchChannelIndex].unCompatibleTakeoffThr *\
                    g_stBGAdj.uchTakeoffFactorAtBGhigher) >> 7 ;
                }
            }
            else//Normal
            {
                lnAutoLEDTakeoffThold = g_stAutoLedConfig.stAutoLedChannel[uchChannelIndex].unCompatibleTakeoffThr;
            }

            if ((uchFlag & (1 << uchChannelIndex)) != 0) // ch1~ch2
            {
                g_stIdealAdjust.stIdealAdjustCh[uchChannelIndex].uchLastIdealTrigger = 0;
                g_stIdealAdjust.stIdealAdjustCh[uchChannelIndex].uchTriggerCnt = 0;
                uchCurrentLimit = g_stAutoLedConfig.stAutoLedChannel[uchChannelIndex].uchCurrentLimit; //Limit Current Max
                uchGainLimit = g_stAutoLedConfig.stAutoLedChannel[uchChannelIndex].uchGainLimitThr; //Limit Gain Max
                /* Step1: Adjust Ipd value by saturation ratio of rawdata , (rawdata-60000)/60000 */
                fIpdVal = HBD_AutoLedfIpdSaturation(uchGainArr[uchChannelIndex], uchCurrentArr[uchChannelIndex], unRawdataAvgArr[uchChannelIndex], g_uchIpdAmpFactor[uchChannelIndex]);
                if (0 == fIpdVal)//Error return AutoLED fail
                {
                    // LOGY("[AutoLED][ERRCODE][ERR]: IPD fail,C:%d G:%d", uchCurrentArr[uchChannelIndex], uchGainArr[uchChannelIndex]);
                    g_uchAutoLedErrorFlag = 1;
                    return g_uchAutoLedErrorFlag;
                }
                /* Step2: Calculate the Current & Gain */
                uchCalcGain = uchGainArr[uchChannelIndex];/* Gain start from Last Gain Array */
                uchCompatibleFlag = HBD_AutoLedDirectCalculateJudge(uchChannelIndex, fIpdVal, &uchCalcGain, uchCurrentArr[uchChannelIndex], &usPreferredCalcCurrent, \
                    uchCurrentLimit, uchGainLimit, g_unIdealTargetValueArr[uchChannelIndex], unRawdataAvgArr[uchChannelIndex]);
                /* Step3: deal with the result of Step2 */
                if (!uchCompatibleFlag)/* Current is OK */
                {
                    if (uchCalcGain!=uchGainArr[uchChannelIndex] || usPreferredCalcCurrent!=uchCurrentArr[uchChannelIndex])
                    {
                        if (HBD_AutoLedWriteRegConfirm(uchCalcGain, (GU8)usPreferredCalcCurrent, uchGainArr, uchCurrentArr, uchChannelIndex))
                        {
                            uchGainArr[uchChannelIndex] = uchCalcGain;
                            uchCurrentArr[uchChannelIndex] = (GU8)usPreferredCalcCurrent;
                            g_uchAutoLedLastFlag |= 1 << uchChannelIndex; /* Channel AutoLed Flag*/
                        }
                    }
                }
                else
                {//进入兼容模式处理
                    uchCompatibleResult = HBD_AutoLedCompatibleMode(uchCompatibleFlag, uchChannelIndex, fIpdVal, \
                        &usPreferredCalcCurrent, &uchCalcGain, uchCurrentLimit, uchGainLimit,  g_unIdealTargetValueArr[uchChannelIndex],\
                        unRawdataAvgArr[uchChannelIndex], lnAutoLEDTakeoffThold, uchGainArr[uchChannelIndex], uchCurrentArr[uchChannelIndex]);
                    if (!uchCompatibleResult) /* Compatiable Mode Sucess */
                    {
                        if (uchCalcGain!=uchGainArr[uchChannelIndex] || usPreferredCalcCurrent!=uchCurrentArr[uchChannelIndex])
                        {
                            if (HBD_AutoLedWriteRegConfirm(uchCalcGain, (GU8)usPreferredCalcCurrent, uchGainArr, uchCurrentArr, uchChannelIndex))
                            {
                                uchGainArr[uchChannelIndex] = uchCalcGain;
                                uchCurrentArr[uchChannelIndex] = (GU8)usPreferredCalcCurrent;
                                g_uchAutoLedLastFlag |= 1 << uchChannelIndex; /* Channel AutoLed Flag*/
                            }
                        }
                    }
                    else /* No solution, return error */
                    {
                        if (g_stBGAdj.uchTurnOnNotReturnFail)//不返回失败开关
                        {
                            if (g_stBGAdj.stBGAdjCh[uchChannelIndex].uchLastBGLevel >= HBD_AUTOLED_BG_WEAKLIGHT)
                            {
                                g_uchAutoLedErrorFlag = 0;//will not return fail flag
                            }
                        }
                        else{
                            g_uchAutoLedErrorFlag = 1;//Fail
                        }
                    }
                }
            }
        }
    }
    if (g_uchAutoLedLastFlag)
    {
        HBD_AutoLedWriteReg(uchGainArr, uchGainShiftArr, usLastGainMsk, uchCurrentArr, uchCurrentShift, unLastCurrentMsk);
    }
    return g_uchAutoLedErrorFlag;
}

/****************************************************************
* Description: soft autoled by new data
* Input:  unRawdata rawdata
* Return: autoled success or error
******************************************************************/
GU8 HBD_AutoLedbyNewDataInt(GU32 unRawdata[10])
{
    GU8 uchAutoLedGainArr[2] = { 0 };
    GU8 uchAutoLedGainShiftArr[2] = { 0 };
    GU16 usAutoLedGainMsk = 0;
    GU8 uchAutoLedCurrentArr[2] = { 0 };
    GU8 uchAutoLedCurrentShift[2] = { 0 };
    GU32 unAutoLedCurrentMsk = 0;
    GU8 uchAutoLedFlag = 0;
    GU32 unAutoLedRawdataAvgArr[2] = { 0 };
    GU8 uAutoLEDCalFlag = 0;
    //GU32 unACC = 0;
    GU8 i = 0;//通道索引
    GU16 usIndex = g_stSampleInfo.unSampleFre - g_stSampleInfo.unAutoLedCalcLen; /*AutoLED Start index*/
    g_stSampleInfo.unSampleFifoDuration = g_stSampleInfo.unSampleFre * g_stSampleInfo.unSampleTim;
    /*增加调光标记 add by Benjamin 200817*/
    if (g_uchAutoLedLastFlag){
        switch (g_uchAutoLedLastFlag & 0x03)
        {
        case 1:
            unRawdata[0] |= 0x800000;
            break;
        case 2:
            unRawdata[1] |= 0x800000;
            break;
        case 3:
            unRawdata[0] |= 0x800000;
            unRawdata[1] |= 0x800000;
            break;
        }
    }
    g_uchAutoLedLastFlag = 0;

    /* Count to g_stSampleInfo.unAutoLedCalcLen,updata AutoLED Flag*/
    g_uchSoftLEDNewdataModeCNT = g_uchSoftLEDNewdataModeCNT % g_stSampleInfo.unSampleFre;

    /* BG calculate */
    HBD_AutoLedBGBufferRecursionCalc(unRawdata, g_uchSoftLEDNewdataModeCNT + 1);
    /* PPG Sat judge   饱和判断需要根据背景光等级改变饱和阈值的*/
    HBD_AutoLedPpgSatJudge(unRawdata);

    //unACC = (GU32)sqrt(unRawdata[2] * unRawdata[2] + unRawdata[3] * unRawdata[3] + unRawdata[4] * unRawdata[4]);
    //LOGD("%d %d %d %d", (int)unRawdata[2], (int)unRawdata[3], (int)unRawdata[4], (int)unACC);
    //HBD_AutoLedRecursionCalc(unACC, &g_stAutoLedAccInfo.fStd, &g_stAutoLedAccInfo.fAvg, &g_stAutoLedAccInfo.uMin, &g_stAutoLedAccInfo.uMax, &g_stAutoLedAccInfo.uDif, (GS32)(g_uchSoftLEDNewdataModeCNT + 1));
    //LOGD("Std:%5.1f Avg:%d Max:%d Min:%d Dif:%d", g_stAutoLedAccInfo.fStd, (int)g_stAutoLedAccInfo.fAvg,\
            //(int)g_stAutoLedAccInfo.uMax, (int)g_stAutoLedAccInfo.uMin, (int)g_stAutoLedAccInfo.uDif);
    /* Get Trigger info */
    if (usIndex <= g_uchSoftLEDNewdataModeCNT)
    {
        HBD_AutoLedTrigRecursionCalc(unRawdata,g_uchSoftLEDNewdataModeCNT - usIndex + 1);
    }

    if (g_uchSoftLEDNewdataModeCNT == (g_stSampleInfo.unSampleFre - 1))
    {
        //g_stAutoLedAccInfo.fStd = sqrtf(g_stAutoLedAccInfo.fStd / g_uchSoftLEDNewdataModeCNT );
        ++g_uchSoftLEDNewdataModeCNT;
        //LOGY("");
        //LOGY("[AutoLED][ACCINFO][ACC]: Std:%5.1f Avg:%d Max:%d Min:%d Dif:%d", g_stAutoLedAccInfo.fStd, (int)g_stAutoLedAccInfo.fAvg,\
        //(int)g_stAutoLedAccInfo.uMax, (int)g_stAutoLedAccInfo.uMin, (int)g_stAutoLedAccInfo.uDif);

        /* Get Gain And Current */
        if (HBD_AutoLedGetCurrentAndGain(uchAutoLedGainArr, uchAutoLedGainShiftArr, &usAutoLedGainMsk, \
            uchAutoLedCurrentArr, uchAutoLedCurrentShift, &unAutoLedCurrentMsk))
        {
            for (i = 0; i < g_uchNeedReadRawdataCount; i++)
            {
                if (!uchAutoLedCurrentArr[i])
                {
                    uchAutoLedCurrentArr[i] = 20;
                    g_uchAutoLedLastFlag |= 1 << i;//调光标记
                }
            }
            HBD_AutoLedWriteReg(uchAutoLedGainArr, uchAutoLedGainShiftArr, usAutoLedGainMsk,
                uchAutoLedCurrentArr, uchAutoLedCurrentShift, unAutoLedCurrentMsk);
            return uAutoLEDCalFlag;//给定电流后返回此次调光
        }

        /* BG Level Process */
        if (HBD_AutoLEDBGProcess(uchAutoLedGainArr, unAutoLedRawdataAvgArr))
        {
            // LOGY("************Enter BG Process****************");
            HBD_AutoLedWriteReg(uchAutoLedGainArr, uchAutoLedGainShiftArr, usAutoLedGainMsk,
                uchAutoLedCurrentArr, uchAutoLedCurrentShift, unAutoLedCurrentMsk);
            return  uAutoLEDCalFlag;
        }

          /* Saturation Process   数据饱和修改理想目标值触发阈值修改*/
        HBD_AutoLedSatProcess( );

        /* Trigger flag */
        uchAutoLedFlag = HBD_AutoLedCalcTrigerFlag();

        if (HBD_AutoLedRawdataAvgDeal(unAutoLedRawdataAvgArr))
        {
            return HBD_AUTOLED_ERROR_CURRENT_RAWDATA;//Rawdata is too small, <100
        }

        HBD_AutoLedPpgSatTrigerFlag(&uchAutoLedFlag, unAutoLedRawdataAvgArr);    //判断是否饱和触发调光
        /* If Get Current and Gain, uchAutoLedGainArr will be updataed ,
           Write Reg need to check Call:
              FUNCTION HBD_AutoLedWriteRegConfirm */
        uAutoLEDCalFlag = HBD_AutoLedCalcGainCurrent(uchAutoLedFlag, uchAutoLedGainArr, uchAutoLedGainShiftArr, usAutoLedGainMsk,
            uchAutoLedCurrentArr, uchAutoLedCurrentShift, unAutoLedCurrentMsk, unAutoLedRawdataAvgArr);
        return uAutoLEDCalFlag;
    }

    ++g_uchSoftLEDNewdataModeCNT;
    return uAutoLEDCalFlag;
}

/****************************************************************
* Description: soft autoled by fifo int
* Input:  puchDataBuffer: fifo buffer
usRawdatabufferCnt: rawdata cnt
* Return: autoled success or error
******************************************************************/
GU8 HBD_AutoLedbyFifoInt(GU32 *punDataBuffer, GU16 usRawdatabufferCnt)
{
    GU8 uchAutoLedGainArr[2] = { 0 };
    GU8 uchAutoLedGainShiftArr[2] = { 0 };
    GU16 usAutoLedGainMsk = 0;
    GU8 uchAutoLedCurrentArr[2] = { 0 };
    GU8 uchAutoLedCurrentShift[2] = { 0 };
    GU32 unAutoLedCurrentMsk = 0;
    GU32 unAutoLedRawdataAvgArr[2] = { 0 };
    GU8 uchAutoLedFlag = 0;
    GU32 unRawdata[2] = { 0 };
    GU16 usCalcStartIndex = (usRawdatabufferCnt > g_stSampleInfo.unAutoLedCalcLen) ? (usRawdatabufferCnt - g_stSampleInfo.unAutoLedCalcLen) : (0);//start index of trigger calculation
    GU8 uAutoLEDCalFlag = 0;//added by Benjamin 200818
    GU8 i = 0;//通道索引

    // LOGY("[AutoLED][FIFO_IN][IDX]: Start:%d Len:%d Calclen:%d LastFlag:0x%x", usCalcStartIndex, usRawdatabufferCnt, g_stSampleInfo.unAutoLedCalcLen, g_uchAutoLedLastFlag);
    g_stSampleInfo.unSampleFifoDuration = usRawdatabufferCnt * g_stSampleInfo.unSampleTim;
    for (int i = 0; i < g_uchNeedReadRawdataCount; i++)
    {
        g_stAutoledWrtRegCheck.unWrtRegNormDuration[i] += g_stSampleInfo.unSampleFifoDuration;
        if (g_stAutoledWrtRegCheck.unWrtRegNormDuration[i] >= g_stAutoledWrtRegCheck.unWrtRegThres)
        {
            g_stAutoledWrtRegCheck.unWrtRegNormDuration[i] = g_stAutoledWrtRegCheck.unWrtRegThres;
        }
    }
    /* Add Autoled Flag */
    g_uchAutoLedLastFlag = 0;
#if 0
    if (g_uchAutoLedLastFlag)
    {
        if (g_uchNeedReadRawdataCount == 2)
        {
            switch (g_uchAutoLedLastFlag & 0x03)
            {
            case 1:
                punDataBuffer[0 * 6] |= 0x80;
                break;
            case 2:
                punDataBuffer[0 * 6 + 3] |= 0x80;
                break;
            case 3:
                punDataBuffer[0 * 6] |= 0x80;
                punDataBuffer[0 * 6 + 3] |= 0x80;
                break;
            }
        }
        else
        {
            punDataBuffer[0 * 3] |= 0x80;
        }
        g_uchAutoLedLastFlag = 0;
    }
#endif

    for (GU16 cnt = 0; cnt < usRawdatabufferCnt; cnt++)
    {
        /* combine 24bits rawdata*/
        switch (g_uchNeedReadRawdataCount)
        {
            case 2: //2 channel
                unRawdata[0] = punDataBuffer[cnt * g_uchNeedReadRawdataCount] >> 7;
                unRawdata[1] = punDataBuffer[cnt * g_uchNeedReadRawdataCount + 1] >> 7;
            break;
            case 1: //single channel
                unRawdata[0] = punDataBuffer[cnt * g_uchNeedReadRawdataCount] >> 7;
                unRawdata[1] = unRawdata[0];
            break;
            default:
            break;
        }
        /* AutoLED Flag */
        if (cnt >= usCalcStartIndex)   //只取中断最近的5帧数据处理
        {
            HBD_AutoLedTrigRecursionCalc(unRawdata,cnt - usCalcStartIndex + 1);
        }
        /*  BG Recursion Calculator */
        HBD_AutoLedBGBufferRecursionCalc(unRawdata, cnt + 1);

        /* PPG Sat judge   饱和判断需要根据背景光等级改变饱和阈值的*/
        HBD_AutoLedPpgSatJudge(unRawdata);
    }

    /* Get Gain And Current */
    if (HBD_AutoLedGetCurrentAndGain(uchAutoLedGainArr, uchAutoLedGainShiftArr, &usAutoLedGainMsk, \
         uchAutoLedCurrentArr, uchAutoLedCurrentShift, &unAutoLedCurrentMsk))
    {
        for (i = 0; i < g_uchNeedReadRawdataCount; i++)
        {
            if (!uchAutoLedCurrentArr[i])
            {
                uchAutoLedCurrentArr[i] = 20;
                g_uchAutoLedLastFlag |= 1 << i;//调光标记
            }
        }
        HBD_AutoLedWriteReg(uchAutoLedGainArr, uchAutoLedGainShiftArr, usAutoLedGainMsk,
            uchAutoLedCurrentArr, uchAutoLedCurrentShift, unAutoLedCurrentMsk);
        return uAutoLEDCalFlag;//给定电流后返回此次调光
    }
#if (HBD_CTRL_LIB_DEV_VERSION == HBD_CTRL_LIB_FOR_FINGER)
    else
    { //// 没有寄存器的电流值为0，计算CTR过大，则判断指夹仪的调光失败
        GU16 usMaxDriveCur = 0;

        usMaxDriveCur = (HBD_I2cReadReg(HBD_LED_MAXDRIVECUR_REG_ADDR) >> 5) & 0x7;
        usMaxDriveCur = 20 + usMaxDriveCur * 20;
        for (i = 0; i < g_uchNeedReadRawdataCount; i++)
        {
            GU32 nCuurent = uchAutoLedCurrentArr[i] * usMaxDriveCur; // nCuurent/255为电流(单位: uA);nCuurent/255=0(整形相除),计算CTR时把除以255改为乘以255
            GU16 usGainVal = g_usGainArrEnum[uchAutoLedGainArr[i]];
            GU32 nCtr = ((GU64)(g_stTrigCalcInfo[i].fAvg) * 800 * 255000) / (usGainVal * 65536) / nCuurent; // CTR单位: nA/mA

            if (nCtr > __HBD_CTR_WEAROFF_THRESHOLD__)
            {
                g_uchAutoLedErrorFlag = 1; // Fail
                return g_uchAutoLedErrorFlag; // 调光失败返回
            }
        }
    }
#endif

    /* BG Level Process */
    if (HBD_AutoLEDBGProcess(uchAutoLedGainArr, unAutoLedRawdataAvgArr))
    {
        // LOGY("************Enter BG Process****************");
        HBD_AutoLedWriteReg(uchAutoLedGainArr, uchAutoLedGainShiftArr, usAutoLedGainMsk,
            uchAutoLedCurrentArr, uchAutoLedCurrentShift, unAutoLedCurrentMsk);
        return  uAutoLEDCalFlag;
    }

    /* Saturation Process 数据饱和修改理想目标值触发阈值修改*/
    HBD_AutoLedSatProcess();

    /* Rawdata Trigger 调光触发上阈值会根据背景光等级修改 */
    uchAutoLedFlag = HBD_AutoLedCalcTrigerFlag();

    if (HBD_AutoLedRawdataAvgDeal(unAutoLedRawdataAvgArr))
    {
        g_uchAutoLedErrorFlag = 1;
        return HBD_AUTOLED_ERROR_CURRENT_RAWDATA;//Rawdata is too small, <100
    }

    HBD_AutoLedPpgSatTrigerFlag(&uchAutoLedFlag, unAutoLedRawdataAvgArr);    //判断是否饱和触发调光

    /* If Get Current and Gain, uchAutoLedGainArr will be updataed,
       Write Reg need to check Call:
          FUNCTION HBD_AutoLedWriteRegConfirm */
    uAutoLEDCalFlag = HBD_AutoLedCalcGainCurrent(uchAutoLedFlag, uchAutoLedGainArr, uchAutoLedGainShiftArr, usAutoLedGainMsk,
        uchAutoLedCurrentArr, uchAutoLedCurrentShift, unAutoLedCurrentMsk, unAutoLedRawdataAvgArr);
    return uAutoLEDCalFlag;
}

/****************************************************************
* Description: memory free
* Input:  cmdtype
* Return: None
******************************************************************/
void HBD_Mem_Free()
{
    //EXAMPLE_DEBUG_LOG_L1("HBD_Mem_Free g_uchMemUseFlag = %d\n\r",g_uchMemUseFlag);
    if(SPO2_MEM_USE_CMD == g_uchMemUseFlag)
    {
        // HOOK_FUNC_CALL(g_pAlgoDeinitHookFunc, (GH3011_FUNC_OFFSET_SPO2));
        // HOOK_FUNC_CALL(g_pAlgoDeinitHookFunc, (GH3011_FUNC_OFFSET_HRV));
        // HOOK_FUNC_CALL(g_pAlgoMemoryDeinitHookFunc, ());
        if(g_pAlgoDeinitHookFunc)
            g_pAlgoDeinitHookFunc(GH3011_FUNC_OFFSET_SPO2);
        if(g_pAlgoDeinitHookFunc)
            g_pAlgoDeinitHookFunc(GH3011_FUNC_OFFSET_HRV);
        if(g_pAlgoMemoryDeinitHookFunc)
            g_pAlgoMemoryDeinitHookFunc();
    }
    else if(HB_MEM_USE_CMD == g_uchMemUseFlag)
    {

        if (g_chWearIsEnableFlag == 1)
        {
            //NADT_ProcDeInit();       // add nadt deinit
            //HOOK_FUNC_CALL(g_pAlgoDeinitHookFunc, (GH3011_FUNC_OFFSET_NADT));
            if(g_pAlgoDeinitHookFunc)
                g_pAlgoDeinitHookFunc(GH3011_FUNC_OFFSET_NADT);
        }
        if(0 == g_uchCloseHBAlgo)
        {
            //HBADeInit();
            //HOOK_FUNC_CALL(g_pAlgoDeinitHookFunc, (GH3011_FUNC_OFFSET_HR));
            if(g_pAlgoDeinitHookFunc)
                g_pAlgoDeinitHookFunc(GH3011_FUNC_OFFSET_HR);
        }
        //HOOK_FUNC_CALL(g_pAlgoMemoryDeinitHookFunc, ());
        if(g_pAlgoMemoryDeinitHookFunc)
            g_pAlgoMemoryDeinitHookFunc();
    }
    else if(HRV_MEM_USE_CMD == g_uchMemUseFlag)
    {
        // HOOK_FUNC_CALL(g_pAlgoDeinitHookFunc, (GH3011_FUNC_OFFSET_HRV));
        // HOOK_FUNC_CALL(g_pAlgoMemoryDeinitHookFunc, ());
        if(g_pAlgoDeinitHookFunc)
            g_pAlgoDeinitHookFunc(GH3011_FUNC_OFFSET_HRV);
        if(g_pAlgoMemoryDeinitHookFunc)
            g_pAlgoMemoryDeinitHookFunc();
    }
    else if(HB_HRV_MEM_USE_CMD == g_uchMemUseFlag)
    {
        if (g_chWearIsEnableFlag == 1)
        {
            //NADT_ProcDeInit();       // add nadt deinit
            //HOOK_FUNC_CALL(g_pAlgoDeinitHookFunc, (GH3011_FUNC_OFFSET_NADT));
            if(g_pAlgoDeinitHookFunc)
                g_pAlgoDeinitHookFunc(GH3011_FUNC_OFFSET_NADT);
        }
        // HOOK_FUNC_CALL(g_pAlgoDeinitHookFunc, (GH3011_FUNC_OFFSET_HR));
        // HOOK_FUNC_CALL(g_pAlgoDeinitHookFunc, (GH3011_FUNC_OFFSET_HRV));
        // HOOK_FUNC_CALL(g_pAlgoMemoryDeinitHookFunc, ());
        if(g_pAlgoDeinitHookFunc)
            g_pAlgoDeinitHookFunc(GH3011_FUNC_OFFSET_HR);
        if(g_pAlgoDeinitHookFunc)
            g_pAlgoDeinitHookFunc(GH3011_FUNC_OFFSET_HRV);
        if(g_pAlgoMemoryDeinitHookFunc)
          g_pAlgoMemoryDeinitHookFunc();  
    }
    g_uchMemUseFlag = NO_MEM_USE_CMD;
}

/****************************************************************
* Description: Control use HB algo or not
* Input:  emState
* Return: None
******************************************************************/
void HBD_EnableHBAlgo(EM_HBD_FUNCTIONAL_STATE emState)
{
    if(HBD_FUNCTIONAL_STATE_DISABLE == emState)
    {
        g_uchCloseHBAlgoFlag = 1;
    }
    else
    {
        g_uchCloseHBAlgoFlag = 0;
    }
}

/****************************************************************
* Description: start hbd
* Input:  None
* Return: HBD_RET_OK=success,
          HBD_RET_LED_CONFIG_ALL_OFF_ERROR=all Led disable error,
          HBD_RET_NO_INITED_ERROR=not inited,
******************************************************************/
#if 1
GS8 HBD_HbDetectStart (void)
{
    GS8 chRet = HBD_RET_NO_INITED_ERROR;
    GU16 usRegVal = 0;
    //GS32 nMemInitRes = 0;
    if (g_uchNeedReadRawdataCount < 1)
    {
        return HBD_RET_LED_CONFIG_ALL_OFF_ERROR;
    }
    if (HBD_INIT_STATUS_INITED == g_uchHbdInited)
    {
        if (HBD_CheckChipInited() != 0) // check inited
        {
            GU16 usFifoEnableRegVal = 0x00;
            GU16 usIrqCtrlRegVal = ((g_usIrqPluseWidth - 1) << 8) | (1 << 5) /* idle state*/ | HBD_IRQ_MSK_ADC_VALID | HBD_IRQ_MSK_AUTO_LED_FAIL;
            //GS32 NADTSampleRate = HB_SAMPLING_RATE_SETTING;///HB algo only suppost 25Hz, don't suppost other value for 20200803
            HBD_ChangeToHbModeRegVal();
            g_uIRErrCnt = 0;

            /* HBA algo reinit */
            #if 0
            if (g_uchHbdStarted != HBD_START_STATUS_BAND_WEAR_CONFIRM_MODE) // fixed if last start mode is wear confirm, don't init hba algo
            {
                HBD_Mem_Free ();
                HOOK_FUNC_CALL(g_pAlgoMemoryInitHookFunc, (&nMemInitRes));
                if (0 != nMemInitRes)
                {
                    chRet = nMemInitRes;// + HBD_RET_ALG_MEM_INIT_OK;
                    return chRet;
                }
                g_uchCloseHBAlgo = g_uchCloseHBAlgoFlag;

                g_uchMemUseFlag = HB_MEM_USE_CMD;
                if(0 == g_uchCloseHBAlgo)
                {
                    GS32 nHbAlgoParamBuffer[3] = {0};
                    nHbAlgoParamBuffer[0] = HB_SAMPLING_RATE_SETTING;
                    nHbAlgoParamBuffer[1] = g_uchHbaScenario;
                    nHbAlgoParamBuffer[2] = g_uchHbaTestMode;
                    HOOK_FUNC_CALL(g_pAlgoInitHookFunc, (GH3011_FUNC_OFFSET_HR, 3, nHbAlgoParamBuffer));
                }

                if(g_chWearIsEnableFlag == 1)
                {
                    GS32 nNadtAlgoParamBuffer[4] = {0};
                    nNadtAlgoParamBuffer[0] = g_stAutoLedConfig.stAutoLedChannel[0].emAutoledEnable;
                    nNadtAlgoParamBuffer[1] = g_stAutoLedConfig.stAutoLedChannel[0].unTrigerLowerThr;
                    nNadtAlgoParamBuffer[2] = NADTSampleRate;
                    nNadtAlgoParamBuffer[3] = HBD_I2cReadReg(HBD_ADT_THR3_REG_ADDR);
                    HOOK_FUNC_CALL(g_pAlgoInitHookFunc, (GH3011_FUNC_OFFSET_NADT, 4, nNadtAlgoParamBuffer));
                }
            }
            #endif
            HBD_AutoLedReset(HBD_START_STATUS_HB_MODE);
            g_uchPackageSN = 0;  // index clear

            if (g_emHbFifoEnableFlag == HBD_FUNCTIONAL_STATE_ENABLE)
            {
                usFifoEnableRegVal = 0x01;
                usIrqCtrlRegVal = (usIrqCtrlRegVal & HBD_IRQ_REG_IRQ_UNMSK)| HBD_IRQ_MSK_FIFO_THR_FULL | HBD_IRQ_MSK_FIFO_FULL | HBD_IRQ_MSK_AUTO_LED_FAIL;
            }

            usRegVal = HBD_I2cReadReg(HBD_SAMPLE_RATE_REG_ADDR);
            HBD_I2cWriteReg(HBD_SAMPLE_RATE_REG_ADDR,usRegVal);
         //   HBD_I2cWriteReg(HBD_SAMPLE_RATE_REG_ADDR, (GU16)(HBD_SAMPLE_RATE_CLOCK_FREQ/HB_SAMPLING_RATE_SETTING));
            HBD_I2cWriteReg(HBD_FIFO_EN_REG_ADDR, usFifoEnableRegVal);
            //HBD_I2cWriteReg(HBD_FIFO_THR_REG_ADDR, g_stFifoThrConfigList[HBD_FIFO_THR_CONFIG_HB_INDEX].usFifoThrCntConfigVal * g_uchNeedReadRawdataCount);
            HBD_I2cWriteReg(HBD_IRQ_CTRL_REG_ADDR, usIrqCtrlRegVal);
            HBD_I2cSendCmd(HBD_CMD_SCAN);
            g_uchLastReadFifoCnt = 0;
            g_uchHbdStarted = HBD_START_STATUS_HB_MODE;
            chRet = HBD_RET_OK;
        }
        else
        {
            HBD_I2cSendCmd(HBD_CMD_SLEEP);
            HBD_DelayUs(HBD_SEND_CMD_DELAY_X_US);
            g_uchHbdInited = HBD_INIT_STATUS_NO_INIT;
        }
    }
    return chRet;
}

#endif
/****************************************************************
* Description: start hbd with hrv
* Input:  None
* Return: HBD_RET_OK=success,
          HBD_RET_LED_CONFIG_ALL_OFF_ERROR=all Led disable error,
          HBD_RET_NO_INITED_ERROR=not inited,
******************************************************************/
#if 0
GS8 HBD_HbWithHrvDetectStart (void)
{
    GS8 chRet = HBD_RET_NO_INITED_ERROR;
    GU16 usRegVal = 0;
    if (g_uchNeedReadRawdataCount < 1)
    {
        return HBD_RET_LED_CONFIG_ALL_OFF_ERROR;
    }
    if (HBD_INIT_STATUS_INITED == g_uchHbdInited)
    {
        if (HBD_CheckChipInited() != 0) // check inited
        {
            GU16 usFifoEnableRegVal = 0x00;
            GU16 usIrqCtrlRegVal = ((g_usIrqPluseWidth - 1) << 8) | (1 << 5) /* idle state*/ | HBD_IRQ_MSK_ADC_VALID | HBD_IRQ_MSK_AUTO_LED_FAIL;
            GS32 NADTSampleRate = HB_SAMPLING_RATE_SETTING;
            HBD_ChangeToHbModeRegVal();

            usRegVal = HBD_I2cReadReg(HBD_SAMPLE_RATE_REG_ADDR);
            HBD_I2cWriteReg(HBD_SAMPLE_RATE_REG_ADDR,usRegVal);

            /* HBA algo reinit */
            if (g_uchHbdStarted != HBD_START_STATUS_BAND_WEAR_CONFIRM_MODE) // fixed if last start mode is wear confirm, don't init hba algo
            {
                GS32 nMemInitRes = 0;
                HBD_Mem_Free ();
                HOOK_FUNC_CALL(g_pAlgoMemoryInitHookFunc, (&nMemInitRes));
                if (0 != nMemInitRes)
                {
                    chRet = nMemInitRes;// + HBD_RET_ALG_MEM_INIT_OK;
                    return chRet;
                }
                g_uchMemUseFlag = HB_HRV_MEM_USE_CMD;

                GS32 nHbAlgoParamBuffer[3] = {0};
                nHbAlgoParamBuffer[0] = HB_SAMPLING_RATE_SETTING;
                nHbAlgoParamBuffer[1] = g_uchHbaScenario;
                nHbAlgoParamBuffer[2] = g_uchHbaTestMode;
                HOOK_FUNC_CALL(g_pAlgoInitHookFunc, (GH3011_FUNC_OFFSET_HR, 3, nHbAlgoParamBuffer));

                if(g_chWearIsEnableFlag == 1)
                {
                    GS32 nNadtAlgoParamBuffer[4] = {0};
                    nNadtAlgoParamBuffer[0] = g_stAutoLedConfig.stAutoLedChannel[0].emAutoledEnable;
                    nNadtAlgoParamBuffer[1] = g_stAutoLedConfig.stAutoLedChannel[0].unTrigerLowerThr;
                    nNadtAlgoParamBuffer[2] = NADTSampleRate;
                    nNadtAlgoParamBuffer[3] = HBD_I2cReadReg(HBD_ADT_THR3_REG_ADDR);
                    HOOK_FUNC_CALL(g_pAlgoInitHookFunc, (GH3011_FUNC_OFFSET_NADT, 4, nNadtAlgoParamBuffer));
                }

                usHbWithHrvSampleRate = (GU16)(HBD_SAMPLE_RATE_CLOCK_FREQ/usRegVal);
                usHbWithHrvSampleRate = (usHbWithHrvSampleRate / HB_SAMPLING_RATE_SETTING) * HB_SAMPLING_RATE_SETTING;
                usHbWithHrvSampleCalcDiv = usHbWithHrvSampleRate / HB_SAMPLING_RATE_SETTING;

                GS32 nHrvAlgoParamBuffer[1] = {usHbWithHrvSampleRate};
                HOOK_FUNC_CALL(g_pAlgoInitHookFunc, (GH3011_FUNC_OFFSET_HRV, 1, nHrvAlgoParamBuffer));
            }
            usHbWithHrvSampleIndex = 0;
            usHbWithHrvSampleReIndex = 0;
            HBD_AutoLedReset(HBD_START_STATUS_HB_MODE);
            g_uchPackageSN = 0;  // index clear

            if (g_emHbFifoEnableFlag == HBD_FUNCTIONAL_STATE_ENABLE)
            {
                usFifoEnableRegVal = 0x01;
                usIrqCtrlRegVal = (usIrqCtrlRegVal & HBD_IRQ_REG_IRQ_UNMSK)| HBD_IRQ_MSK_FIFO_THR_FULL | HBD_IRQ_MSK_FIFO_FULL | HBD_IRQ_MSK_AUTO_LED_FAIL;
            }

         //   HBD_I2cWriteReg(HBD_SAMPLE_RATE_REG_ADDR, (GU16)(HBD_SAMPLE_RATE_CLOCK_FREQ/HB_SAMPLING_RATE_SETTING));
            HBD_I2cWriteReg(HBD_FIFO_EN_REG_ADDR, usFifoEnableRegVal);
            HBD_I2cWriteReg(HBD_FIFO_THR_REG_ADDR, g_stFifoThrConfigList[HBD_FIFO_THR_CONFIG_HRV_INDEX].usFifoThrCntConfigVal * g_uchNeedReadRawdataCount);
            HBD_I2cWriteReg(HBD_IRQ_CTRL_REG_ADDR, usIrqCtrlRegVal);

            HBD_I2cSendCmd(HBD_CMD_SCAN);
            g_uchLastReadFifoCnt = 0;
            g_uchHbdStarted = HBD_START_STATUS_HB_MODE;
            chRet = HBD_RET_OK;
        }
        else
        {
            HBD_I2cSendCmd(HBD_CMD_SLEEP);
            HBD_DelayUs(HBD_SEND_CMD_DELAY_X_US);
            g_uchHbdInited = HBD_INIT_STATUS_NO_INIT;
        }
    }
    return chRet;
}
#endif

/****************************************************************
* Description: start Wear state confirm.
* Input:  None,
* Return: HBD_RET_OK=success,
          HBD_RET_NO_INITED_ERROR=not inited,
          HBD_RET_LED_CONFIG_ALL_OFF_ERROR=all Led disable error,
          HBD_RET_GENERIC_ERROR=don't need check wear state,
******************************************************************/
GS8 HBD_WearStateConfirmStart (void)
{
    GS8 chRet = HBD_RET_NO_INITED_ERROR;
    if (g_uchNeedReadRawdataCount < 1)
    {
        return HBD_RET_LED_CONFIG_ALL_OFF_ERROR;
    }
    if (g_uchHbdStarted != HBD_START_STATUS_HB_MODE)
    {
        return HBD_RET_GENERIC_ERROR;
    }
    if (HBD_INIT_STATUS_INITED == g_uchHbdInited)
    {
        GU16 usRegVal = 0;
        if (HBD_CheckChipInited() != 0) // check inited
        {
            g_uchPackageSN = 0; // index reset
            HBD_AutoLedReset(HBD_START_STATUS_BAND_WEAR_CONFIRM_MODE);
            // Threshold val from reg
            {
                // wear possible
                usRegVal = HBD_I2cReadReg(0x00C2);
                g_unWearStateUpperThreshold = (GU32) usRegVal;
                usRegVal = HBD_I2cReadReg(0x00C4);
                g_unWearStateLowerThreshold = (GU32) usRegVal;
                usRegVal = HBD_I2cReadReg(0x00C6);
                g_unWearStateUpperThreshold += (GU32) usRegVal;
                usRegVal = HBD_I2cReadReg(0x00C8);
                g_unWearStateLowerThreshold += (GU32) usRegVal;

                // unwear
                usRegVal = HBD_I2cReadReg(0x00CE);
                g_unUnwearStateUpperThreshold = (GU32) usRegVal;
                usRegVal = HBD_I2cReadReg(0x00D0);
                g_unUnwearStateLowerThreshold = (GU32) usRegVal;
                usRegVal = HBD_I2cReadReg(0x00D4);
                g_unUnwearStateUpperThreshold += (GU32) usRegVal;
                usRegVal = HBD_I2cReadReg(0x00D6);
                g_unUnwearStateLowerThreshold += (GU32) usRegVal;
            }

            HBD_I2cSendCmd(HBD_CMD_SCAN);
            g_uchLastReadFifoCnt = 0;
            g_usWearStateCheckIndex = 0;
            g_uchHbdStarted = HBD_START_STATUS_BAND_WEAR_CONFIRM_MODE;
            chRet = HBD_RET_OK;
        }
        else
        {
            HBD_I2cSendCmd(HBD_CMD_SLEEP);
            HBD_DelayUs(HBD_SEND_CMD_DELAY_X_US);
            g_uchHbdInited = HBD_INIT_STATUS_NO_INIT;
        }
    }
    return chRet;
}

/****************************************************************
* Description: start in-ear ep wear detect, only use with hb detect
* Input:  uchAlgoInitFlag: algo need init flag
* Return: HBD_RET_OK=success,
          HBD_RET_NO_INITED_ERROR=not inited,
          HBD_RET_LED_CONFIG_ALL_OFF_ERROR=all Led disable error,
          HBD_RET_GENERIC_ERROR=wear detect function is disabled.
******************************************************************/
#if 0
GS8 HBD_SoftWearDetectStartOrRecover (GU8 uchAlgoInitFlag)
{
    GS8 chRet = HBD_RET_NO_INITED_ERROR;

    if (g_uchNeedReadRawdataCount < 1)
    {
        return HBD_RET_LED_CONFIG_ALL_OFF_ERROR;
    }
    if (g_chWearIsEnableFlag == 0)
    {
        return HBD_RET_GENERIC_ERROR;
    }
    if (HBD_INIT_STATUS_INITED == g_uchHbdInited)
    {
        if (HBD_CheckChipInited() != 0) // check inited
        {
            GU16 usFifoEnableRegVal = 0x00;
            GU16 usIrqCtrlRegVal = ((g_usIrqPluseWidth - 1) << 8) | (1 << 5) /* idle state*/ | HBD_IRQ_MSK_ADC_VALID | HBD_IRQ_MSK_AUTO_LED_FAIL;

            HBD_ChangeToHbModeRegVal();

            if (uchAlgoInitFlag != 0)/* HBA algo reinit */
            {
                GS32 nMemInitRes = 0;
                HBD_Mem_Free ();
                HOOK_FUNC_CALL(g_pAlgoMemoryInitHookFunc, (&nMemInitRes));
                if (0 != nMemInitRes)
                {
                    chRet = nMemInitRes;// + HBD_RET_ALG_MEM_INIT_OK;
                    return chRet;
                }
                g_uchMemUseFlag = HB_MEM_USE_CMD;
                GS32 nHbAlgoParamBuffer[3] = {0};
                nHbAlgoParamBuffer[0] = HB_SAMPLING_RATE_SETTING;
                nHbAlgoParamBuffer[1] = g_uchHbaScenario;
                nHbAlgoParamBuffer[2] = g_uchHbaTestMode;
                HOOK_FUNC_CALL(g_pAlgoInitHookFunc, (GH3011_FUNC_OFFSET_HR, 3, nHbAlgoParamBuffer));

                GS32 nNadtAlgoParamBuffer[4] = {0};
                nNadtAlgoParamBuffer[0] = g_stAutoLedConfig.stAutoLedChannel[0].emAutoledEnable;
                nNadtAlgoParamBuffer[1] = g_stAutoLedConfig.stAutoLedChannel[0].unTrigerLowerThr;
                nNadtAlgoParamBuffer[2] = HB_SAMPLING_RATE_SETTING;
                nNadtAlgoParamBuffer[3] = HBD_I2cReadReg(HBD_ADT_THR3_REG_ADDR);
                HOOK_FUNC_CALL(g_pAlgoInitHookFunc, (GH3011_FUNC_OFFSET_NADT, 4, nNadtAlgoParamBuffer));
            }
            else
            {
                //HBAStopLostPackageProcess(); // stop lost package for 1 point if recover mode
            }
            HBD_AutoLedReset(HBD_START_STATUS_IN_EAR_EP_WEAR_DETECT_MODE);
            g_uchPackageSN = 0; // index reset

            if (g_emHbFifoEnableFlag == HBD_FUNCTIONAL_STATE_ENABLE)
            {
                usFifoEnableRegVal = 0x01;
                usIrqCtrlRegVal = (usIrqCtrlRegVal & HBD_IRQ_REG_IRQ_UNMSK)| HBD_IRQ_MSK_FIFO_THR_FULL | HBD_IRQ_MSK_FIFO_FULL | HBD_IRQ_MSK_AUTO_LED_FAIL;
            }

            HBD_I2cWriteReg(HBD_SAMPLE_RATE_REG_ADDR, (GU16)(HBD_SAMPLE_RATE_CLOCK_FREQ/HB_SAMPLING_RATE_SETTING));
            HBD_I2cWriteReg(HBD_FIFO_EN_REG_ADDR, usFifoEnableRegVal);
            HBD_I2cWriteReg(HBD_FIFO_THR_REG_ADDR, g_stFifoThrConfigList[HBD_FIFO_THR_CONFIG_HB_INDEX].usFifoThrCntConfigVal * g_uchNeedReadRawdataCount);
            HBD_I2cWriteReg(HBD_IRQ_CTRL_REG_ADDR, usIrqCtrlRegVal);

            HBD_I2cSendCmd(HBD_CMD_SCAN); /* start */
            g_uchLastReadFifoCnt = 0;
            g_uchHbdStarted = HBD_START_STATUS_IN_EAR_EP_WEAR_DETECT_MODE;
            chRet = HBD_RET_OK;
        }
        else
        {
            HBD_I2cSendCmd(HBD_CMD_SLEEP);
            HBD_DelayUs(HBD_SEND_CMD_DELAY_X_US);
            g_uchHbdInited = HBD_INIT_STATUS_NO_INIT;
        }
    }
    return chRet;
}
#endif

/****************************************************************
* Description: start in-ear ep wear detect, only use with hb detect
* Input:  None,
* Return: HBD_RET_OK=success,
          HBD_RET_NO_INITED_ERROR=not inited,
          HBD_RET_LED_CONFIG_ALL_OFF_ERROR=all Led disable error,
          HBD_RET_GENERIC_ERROR=wear detect function is disabled.
******************************************************************/
#if 0
GS8 HBD_InearEpWearDetectStart (void)
{
    return HBD_SoftWearDetectStartOrRecover(1);
}
#endif

/****************************************************************
* Description: recover in-ear ep detect, only use in autoled int
* Input:  None,
* Return: HBD_RET_OK=success,
          HBD_RET_NO_INITED_ERROR=not inited,
          HBD_RET_LED_CONFIG_ALL_OFF_ERROR=all Led disable error,
          HBD_RET_GENERIC_ERROR=wear detect function is disabled.
******************************************************************/
#if 0
GS8 HBD_InearEpDetectRecover (void)
{
    return HBD_SoftWearDetectStartOrRecover(0);
}
#endif

/****************************************************************
* Description: start wear detect, only use with hb detect
* Input:  None,
* Return: HBD_RET_OK=success,
          HBD_RET_NO_INITED_ERROR=not inited,
          HBD_RET_LED_CONFIG_ALL_OFF_ERROR=all Led disable error,
          HBD_RET_GENERIC_ERROR=wear detect function is disabled.
******************************************************************/
#if 0
GS8 HBD_SoftWearDetectStart (void)
{
    return HBD_SoftWearDetectStartOrRecover(1);
}
#endif

/****************************************************************
* Description: recover detect, only use in autoled int
* Input:  None,
* Return: HBD_RET_OK=success,
          HBD_RET_NO_INITED_ERROR=not inited,
          HBD_RET_LED_CONFIG_ALL_OFF_ERROR=all Led disable error,
          HBD_RET_GENERIC_ERROR=wear detect function is disabled.
******************************************************************/
#if 0
GS8 HBD_SoftWearDetectRecover (void)
{
    return HBD_SoftWearDetectStartOrRecover(0);
}
#endif

/****************************************************************
* Description: start hrv
* Input:  None
* Return: HBD_RET_OK=success,
          HBD_RET_LED_CONFIG_ALL_OFF_ERROR=all Led disable error,
          HBD_RET_NO_INITED_ERROR=not inited,
******************************************************************/
#if 0
GS8 HBD_HrvDetectStart (void)
{
    GS8 chRet = HBD_RET_NO_INITED_ERROR;
    if (g_uchNeedReadRawdataCount < 1)
    {
        return HBD_RET_LED_CONFIG_ALL_OFF_ERROR;
    }
    if (HBD_INIT_STATUS_INITED == g_uchHbdInited)
    {
        if (HBD_CheckChipInited() != 0) // check inited
        {
            GS32 nMemInitRes = 0;
            GU16 usFifoEnableRegVal = 0x00;
            GU16 usIrqCtrlRegVal = ((g_usIrqPluseWidth - 1) << 8) | (1 << 5) /* idle state*/ | HBD_IRQ_MSK_ADC_VALID | HBD_IRQ_MSK_AUTO_LED_FAIL;
            GS32 nHRVSampleRate = HRV_SAMPLING_RATE_SETTING;
            GU16 usRegVal = HBD_I2cReadReg(HBD_SAMPLE_RATE_REG_ADDR);
            nHRVSampleRate = (GU16)(HBD_SAMPLE_RATE_CLOCK_FREQ/usRegVal);
            if(256 != nHRVSampleRate)
            {
                nHRVSampleRate = ((nHRVSampleRate + 12)/25) * 25;
            }
            HBD_ChangeToHbModeRegVal();

            HBD_Mem_Free ();
            HOOK_FUNC_CALL(g_pAlgoMemoryInitHookFunc, (&nMemInitRes));
            if (0 != nMemInitRes)
            {
                chRet = nMemInitRes;// + HBD_RET_ALG_MEM_INIT_OK;
                return chRet;
            }
            g_uchMemUseFlag = HRV_MEM_USE_CMD;
            GS32 nHrvAlgoParamBuffer[1] = {nHRVSampleRate};
            HOOK_FUNC_CALL(g_pAlgoInitHookFunc, (GH3011_FUNC_OFFSET_HRV, 1, nHrvAlgoParamBuffer));

            HBD_AutoLedReset(HBD_START_STATUS_HRV_MODE);
            g_uchPackageSN = 0;  // index clear

            if (g_emHrvFifoEnableFlag == HBD_FUNCTIONAL_STATE_ENABLE)
            {
                usFifoEnableRegVal = 0x01;
                usIrqCtrlRegVal = (usIrqCtrlRegVal & HBD_IRQ_REG_IRQ_UNMSK)| HBD_IRQ_MSK_FIFO_THR_FULL | HBD_IRQ_MSK_FIFO_FULL | HBD_IRQ_MSK_AUTO_LED_FAIL;
            }

            //HBD_I2cWriteReg(HBD_SAMPLE_RATE_REG_ADDR, (GU16)(HBD_SAMPLE_RATE_CLOCK_FREQ/HRV_SAMPLING_RATE_SETTING));
            HBD_I2cWriteReg(HBD_FIFO_EN_REG_ADDR, usFifoEnableRegVal);
            HBD_I2cWriteReg(HBD_FIFO_THR_REG_ADDR, g_stFifoThrConfigList[HBD_FIFO_THR_CONFIG_HRV_INDEX].usFifoThrCntConfigVal * g_uchNeedReadRawdataCount);
            HBD_I2cWriteReg(HBD_IRQ_CTRL_REG_ADDR, usIrqCtrlRegVal);

            HBD_I2cSendCmd(HBD_CMD_SCAN);
            g_uchLastReadFifoCnt = 0;
            g_uchHbdStarted = HBD_START_STATUS_HRV_MODE;
            chRet = HBD_RET_OK;
        }
        else
        {
            HBD_I2cSendCmd(HBD_CMD_SLEEP);
            HBD_DelayUs(HBD_SEND_CMD_DELAY_X_US);
            g_uchHbdInited = HBD_INIT_STATUS_NO_INIT;
        }
    }
    return chRet;
}
#endif
/****************************************************************
* Description: start SpO2 and Hr
* Input:  None
* Return: HBD_RET_OK=success,
          HBD_RET_LED_CONFIG_ALL_OFF_ERROR=all Led disable error,
          HBD_RET_NO_INITED_ERROR=not inited,
******************************************************************/
#if 0
GS8 HBD_HrSpO2DetectStart (void)
{
    GS8 chRet = HBD_RET_NO_INITED_ERROR;
    GU16 usSpo2SampleRate = SPO2_SAMPLING_RATE_SETTING;

    if (g_uchNeedReadRawdataCount < 1)
    {
        return HBD_RET_LED_CONFIG_ALL_OFF_ERROR;
    }
    if (HBD_INIT_STATUS_INITED == g_uchHbdInited)
    {
        if (HBD_CheckChipInited() != 0) // check inited
        {
            GU16 usFifoEnableRegVal = 0x00;
            GU16 usRegVal = 0;
            GU16 usIrqCtrlRegVal = ((g_usIrqPluseWidth - 1) << 8) | (1 << 5) /* idle state*/ | HBD_IRQ_MSK_ADC_VALID | HBD_IRQ_MSK_AUTO_LED_FAIL;
            HBD_ChangeToHbModeRegVal();

            usRegVal = HBD_I2cReadReg(HBD_SAMPLE_RATE_REG_ADDR);

            usSpo2SampleRate = (GU16)(HBD_SAMPLE_RATE_CLOCK_FREQ/usRegVal);

             /* HBA algo reinit */
            if (g_uchHbdStarted != HBD_START_STATUS_BAND_WEAR_CONFIRM_MODE) // fixed if last start mode is wear confirm, don't init hba algo
            {
                GS32 nMemInitRes = 0;
                HBD_Mem_Free ();
                HOOK_FUNC_CALL(g_pAlgoMemoryInitHookFunc, (&nMemInitRes));
                if (0 != nMemInitRes)
                {
                    chRet = nMemInitRes;// + HBD_RET_ALG_MEM_INIT_OK;
                    return chRet;
                }
                g_uchMemUseFlag = SPO2_MEM_USE_CMD;

                GS32 nAlgoParamBuffer[6] = {usSpo2SampleRate,
                                            g_nCalibrationParameter[0],
                                            g_nCalibrationParameter[1],
                                            g_nCalibrationParameter[2],
                                            g_nCalibrationParameter[3],
                                            g_nCalibrationParameter[4]};
                HOOK_FUNC_CALL(g_pAlgoInitHookFunc, (GH3011_FUNC_OFFSET_SPO2, 6, nAlgoParamBuffer));
                //SpO2Init();

                GS32 nHbAlgoParamBuffer[3] = {0};
                nHbAlgoParamBuffer[0] = HB_SAMPLING_RATE_SETTING;
                nHbAlgoParamBuffer[1] = g_uchHbaScenario;
                nHbAlgoParamBuffer[2] = g_uchHbaTestMode;
                HOOK_FUNC_CALL(g_pAlgoInitHookFunc, (GH3011_FUNC_OFFSET_HR, 3, nHbAlgoParamBuffer));
            }

            HBD_AutoLedReset(HBD_START_STATUS_SPO2_MODE);
            g_uchPackageSN = 0;  // index clear

            if (g_emSpo2FifoEnableFlag == HBD_FUNCTIONAL_STATE_ENABLE)
            {
                usFifoEnableRegVal = 0x01;
                usIrqCtrlRegVal = (usIrqCtrlRegVal & HBD_IRQ_REG_IRQ_UNMSK)| HBD_IRQ_MSK_FIFO_THR_FULL | HBD_IRQ_MSK_FIFO_FULL | HBD_IRQ_MSK_AUTO_LED_FAIL;
            }

            HBD_I2cWriteReg(HBD_SAMPLE_RATE_REG_ADDR, usRegVal);
            HBD_I2cWriteReg(HBD_FIFO_EN_REG_ADDR, usFifoEnableRegVal);
            HBD_I2cWriteReg(HBD_FIFO_THR_REG_ADDR, g_stFifoThrConfigList[HBD_FIFO_THR_CONFIG_SPO2_INDEX].usFifoThrCntConfigVal * g_uchNeedReadRawdataCount);
            HBD_I2cWriteReg(HBD_IRQ_CTRL_REG_ADDR, usIrqCtrlRegVal);


            HBD_I2cSendCmd(HBD_CMD_SCAN);
            g_uchLastReadFifoCnt = 0;
            g_uchHbdStarted = HBD_START_STATUS_SPO2_MODE;
            chRet = HBD_RET_OK;
        }
        else
        {
            HBD_I2cSendCmd(HBD_CMD_SLEEP);
            HBD_DelayUs(HBD_SEND_CMD_DELAY_X_US);
            g_uchHbdInited = HBD_INIT_STATUS_NO_INIT;
        }
    }
    return chRet;
}
#endif

void Gh30xGetAdtDownThrdRegValue(void)
{
    GU16 usWearOffDownThrRegValue;

    g_usGh30xAdtDownThrdRegValue = HBD_I2cReadReg(0x00C4);

    /***patch: set wear off thrd according wear on thrd, becasue PC tool do not output correct wear off register value*/
    usWearOffDownThrRegValue = g_usGh30xAdtDownThrdRegValue - g_usGh30xAdtDownThrdRegValue / 8;   // 7/8
    HBD_I2cWriteReg(0x00D0, usWearOffDownThrRegValue);  //down thrd
    HBD_I2cWriteReg(0x00CE, 0xFFFF);  //up thrd

#if (HBD_CTRL_LIB_DEV_VERSION == HBD_CTRL_LIB_FOR_FINGER)
    GU16 usWearUp, usWearDown, usUnwearUp, usUnwearDown;

    usWearUp = HBD_I2cReadReg(0x00C2);
    usWearDown = HBD_I2cReadReg(0x00C4);
    usUnwearUp = HBD_I2cReadReg(0x00CE);
    usUnwearDown = HBD_I2cReadReg(0x00D0);

    HBD_I2cWriteReg(0x00C2, (usWearDown - usWearDown / 8));
    HBD_I2cWriteReg(0x00C4, 0);
    HBD_I2cWriteReg(0x00CE, usWearDown);
    HBD_I2cWriteReg(0x00D0, 0);

    usWearUp = HBD_I2cReadReg(0x00C2);
    usWearDown = HBD_I2cReadReg(0x00C4);
    usUnwearUp = HBD_I2cReadReg(0x00CE);
    usUnwearDown = HBD_I2cReadReg(0x00D0);

    EXAMPLE_DEBUG_LOG_L1("[Gh30xGetAdtDownThrdRegValue for finger] wear_up:0x%x, wear_down:0x%x, unwear_up:0x%x, unwear_down:0x%x\r", \
        usWearUp, usWearDown, usUnwearUp, usUnwearDown);
#endif
}

GS8 Gh30xHBD_SampleStart(void)
{
    GS8 chRet = HBD_RET_NO_INITED_ERROR;

    if (HBD_CheckChipInited() != 0) // check inited
    {
        GU16 usIrgEnAndState = 0;
        HBD_StoreHbModeRegVal();

#if (__SYSTEM_TEST_SUPPORT__)
        if (false == goodix_app_start_app_mode && 0 == goodix_system_test_mode)
#else
        if (false == goodix_app_start_app_mode)
#endif
        {
            usIrgEnAndState = (((g_usIrqPluseWidth - 1) << 8) | (1 << 5)/* idle state*/| HBD_IRQ_MSK_FIFO_THR_FULL | HBD_IRQ_MSK_FIFO_FULL | HBD_IRQ_MSK_AUTO_LED_FAIL);
        }
        else
        {
            usIrgEnAndState = (((g_usIrqPluseWidth - 1) << 8) | (1 << 5) /* idle state*/ | HBD_IRQ_MSK_ADC_VALID | HBD_IRQ_MSK_AUTO_LED_FAIL);
        }

        HBD_ChangeToHbModeRegVal();
        HBD_AutoLedReset(0);

        GU16 usRegVal = HBD_I2cReadReg(HBD_SAMPLE_RATE_REG_ADDR);
        HBD_I2cWriteReg(HBD_SAMPLE_RATE_REG_ADDR, usRegVal);
        EXAMPLE_DEBUG_LOG_L1("[%s] usIrgEnAndState:0x%x, RATE_REG:%d, plusWidh:%d\r\n", __FUNCTION__, usIrgEnAndState, usRegVal, g_usIrqPluseWidth);

#if (__SYSTEM_TEST_SUPPORT__)
        if (false == goodix_app_start_app_mode && 0 == goodix_system_test_mode)
#else
        if (false == goodix_app_start_app_mode)
#endif
        {
            HBD_I2cWriteReg(HBD_FIFO_EN_REG_ADDR, 0x01);
        }
        else
        {
            HBD_I2cWriteReg(HBD_FIFO_EN_REG_ADDR, 0x00);
        }

        HBD_I2cWriteRegBitField(HBD_IRQ_CTRL_REG_ADDR, 0, 15, usIrgEnAndState);
        HBD_I2cSendCmd(HBD_CMD_SCAN);

        g_uchLastReadFifoCnt = 0;
        g_uchHbdStarted = HBD_START_STATUS_HB_MODE;
        chRet = HBD_RET_OK;
    }

    return chRet;
}



/****************************************************************
* Description: start SpO2
* Input:  None
* Return: HBD_RET_OK=success,
          HBD_RET_LED_CONFIG_ALL_OFF_ERROR=all Led disable error,
          HBD_RET_NO_INITED_ERROR=not inited,
******************************************************************/
#if 0
GS8 HBD_SpO2DetectStart (void)
{
    GS8 chRet = HBD_RET_NO_INITED_ERROR;
    GU16 usSpo2SampleRate = SPO2_SAMPLING_RATE_SETTING;

    if (g_uchNeedReadRawdataCount < 1)
    {
        return HBD_RET_LED_CONFIG_ALL_OFF_ERROR;
    }
    if (HBD_INIT_STATUS_INITED == g_uchHbdInited)
    {
        if (HBD_CheckChipInited() != 0) // check inited
        {
            GU16 usFifoEnableRegVal = 0x00;
            GU16 usRegVal = 0;
            GU16 usIrqCtrlRegVal = ((g_usIrqPluseWidth - 1) << 8) | (1 << 5) /* idle state*/ | HBD_IRQ_MSK_ADC_VALID | HBD_IRQ_MSK_AUTO_LED_FAIL;
            HBD_ChangeToHbModeRegVal();

            usRegVal = HBD_I2cReadReg(HBD_SAMPLE_RATE_REG_ADDR);

            usSpo2SampleRate = (GU16)(HBD_SAMPLE_RATE_CLOCK_FREQ/usRegVal);

             /* HBA algo reinit */
            if (g_uchHbdStarted != HBD_START_STATUS_BAND_WEAR_CONFIRM_MODE) // fixed if last start mode is wear confirm, don't init hba algo
            {
                GS32 nMemInitRes = 0;
                HBD_Mem_Free ();
                HOOK_FUNC_CALL(g_pAlgoMemoryInitHookFunc, (&nMemInitRes));
                if (0 != nMemInitRes)
                {
                    chRet = nMemInitRes;// + HBD_RET_ALG_MEM_INIT_OK;
                    return chRet;
                }
                g_uchMemUseFlag = SPO2_MEM_USE_CMD;

                GS32 nAlgoParamBuffer[6] = {usSpo2SampleRate,
                                            g_nCalibrationParameter[0],
                                            g_nCalibrationParameter[1],
                                            g_nCalibrationParameter[2],
                                            g_nCalibrationParameter[3],
                                            g_nCalibrationParameter[4]};
                HOOK_FUNC_CALL(g_pAlgoInitHookFunc, (GH3011_FUNC_OFFSET_SPO2, 6, nAlgoParamBuffer));
                nAlgoParamBuffer[0] = usSpo2SampleRate;
                HOOK_FUNC_CALL(g_pAlgoInitHookFunc, (GH3011_FUNC_OFFSET_HRV, 1, nAlgoParamBuffer));
            }

            HBD_AutoLedReset(HBD_START_STATUS_SPO2_MODE);
            g_uchPackageSN = 0;  // index clear

            if (g_emSpo2FifoEnableFlag == HBD_FUNCTIONAL_STATE_ENABLE)
            {
                usFifoEnableRegVal = 0x01;
                usIrqCtrlRegVal = (usIrqCtrlRegVal & HBD_IRQ_REG_IRQ_UNMSK)| HBD_IRQ_MSK_FIFO_THR_FULL | HBD_IRQ_MSK_FIFO_FULL | HBD_IRQ_MSK_AUTO_LED_FAIL;
            }

            HBD_I2cWriteReg(HBD_SAMPLE_RATE_REG_ADDR, usRegVal);
            HBD_I2cWriteReg(HBD_FIFO_EN_REG_ADDR, usFifoEnableRegVal);
            HBD_I2cWriteReg(HBD_FIFO_THR_REG_ADDR, g_stFifoThrConfigList[HBD_FIFO_THR_CONFIG_SPO2_INDEX].usFifoThrCntConfigVal * g_uchNeedReadRawdataCount);
            HBD_I2cWriteReg(HBD_IRQ_CTRL_REG_ADDR, usIrqCtrlRegVal);

            HBD_I2cSendCmd(HBD_CMD_SCAN);
            g_uchLastReadFifoCnt = 0;
            g_uchHbdStarted = HBD_START_STATUS_SPO2_MODE;
            chRet = HBD_RET_OK;
        }
        else
        {
            HBD_I2cSendCmd(HBD_CMD_SLEEP);
            HBD_DelayUs(HBD_SEND_CMD_DELAY_X_US);
            g_uchHbdInited = HBD_INIT_STATUS_NO_INIT;
        }
    }
    return chRet;
}
#endif

/****************************************************************
* Description: stop hbd
* Input:  None
* Return: HBD_RET_OK=success,
          HBD_RET_NO_INITED_ERROR=fail:don't init success
******************************************************************/
GS8 HBD_Stop (void)
{
    GS8 chRet = HBD_RET_NO_INITED_ERROR;
    if (HBD_INIT_STATUS_INITED == g_uchHbdInited)
    {
        HBD_Mem_Free ();
        g_uchHbdStarted = HBD_START_STATUS_OFF;
        HBD_I2cSendCmd(HBD_CMD_IDLE);
        HBD_DelayUs(HBD_SEND_CMD_DELAY_X_US);
        HBD_AutoLedReset(HBD_START_STATUS_OFF);
        HBD_I2cSendCmd(HBD_CMD_SLEEP);
        HBD_DelayUs(HBD_SEND_CMD_DELAY_X_US);
        g_uchLastReadFifoCnt = 0;
        g_uchPackageSN = 0;
        chRet = HBD_RET_OK;
    }
    return chRet;
}


/****************************************************************
* Description: start adt wear on / off  detect
* Input:  uchDetWearOnOrOff
          @ GH30X_ADT_MODE_DET_WEAR_ON: wear on detectiong
          @ GH30X_ADT_MODE_DET_WEAR_OFF: wear off detectiong
* Return: HBD_RET_OK=success,
          HBD_RET_NO_INITED_ERROR=fail:don't init success
******************************************************************/
#if (__EXAMPLE_DEBUG_LOG_LVL__)
const char *g_pstrAdtMode[] = {"on", "off"};
#endif
GS8 Gh30xAdtWearDetectStart (GU8 uchDetWearOnOrOff)
{
    GS8 chRet = HBD_RET_NO_INITED_ERROR;
    EXAMPLE_DEBUG_LOG_L1("[%s] wear %s detecting.\r\n", __FUNCTION__, g_pstrAdtMode[uchDetWearOnOrOff]);
    if (HBD_INIT_STATUS_INITED == g_uchHbdInited)
    {
        if (HBD_CheckChipInited() != 0) // check inited
        {
            GU16 usAdtCtrlRegVal = 0;

            HBD_ChangeToAdtModeRegVal();

            usAdtCtrlRegVal = HBD_I2cReadReg(HBD_ADT_CTRL_REG_ADDR);
            if(GH30X_ADT_MODE_DET_WEAR_ON == uchDetWearOnOrOff)
            {
                usAdtCtrlRegVal &= ~(HBD_ADT_CTRL_MSK_UNWEAR_EN);
            }
            else
            {
                usAdtCtrlRegVal |= (HBD_ADT_CTRL_MSK_UNWEAR_EN);
            }
            HBD_I2cWriteReg(HBD_ADT_CTRL_REG_ADDR, usAdtCtrlRegVal);
            HBD_I2cWriteReg(HBD_IRQ_CTRL_REG_ADDR, ((((GU16)g_usIrqPluseWidth - 1) << 8) | (1 << 5) /* idle state*/ | HBD_IRQ_MSK_ADT_SUCCESS));

            g_uchAdtContinuousMode = 0;

            HBD_I2cSendCmd(HBD_CMD_ADT);
            g_uchLastReadFifoCnt = 0;
            g_uchHbdStarted = HBD_START_STATUS_ADT_WEAR_MODE;
            chRet = HBD_RET_OK;
        }
        else
        {
            HBD_I2cSendCmd(HBD_CMD_SLEEP);
            HBD_DelayUs(HBD_SEND_CMD_DELAY_X_US);
            g_uchHbdInited = HBD_INIT_STATUS_NO_INIT;
            chRet = HBD_RET_NO_INITED_ERROR;
        }
    }
    return chRet;
}

/****************************************************************
* Description: start adt wear detect
* Input:  None
* Return: HBD_RET_OK=success,
          HBD_RET_NO_INITED_ERROR=fail:don't init success
******************************************************************/
GS8 HBD_AdtWearDetectStart (void)
{
    GS8 chRet = HBD_RET_NO_INITED_ERROR;
    EXAMPLE_DEBUG_LOG_L1("[%s] exe.\r\n", __FUNCTION__);
    if (HBD_INIT_STATUS_INITED == g_uchHbdInited)
    {
        if (HBD_CheckChipInited() != 0) // check inited
        {
            GU16 usAdtCtrlRegVal = 0;

            HBD_ChangeToAdtModeRegVal();

            usAdtCtrlRegVal = HBD_I2cReadReg(HBD_ADT_CTRL_REG_ADDR);
            usAdtCtrlRegVal &= ~(HBD_ADT_CTRL_MSK_UNWEAR_EN);
            HBD_I2cWriteReg(HBD_ADT_CTRL_REG_ADDR, usAdtCtrlRegVal);
            HBD_I2cWriteReg(HBD_IRQ_CTRL_REG_ADDR, ((((GU16)g_usIrqPluseWidth - 1) << 8) | (1 << 5) /* idle state*/ | HBD_IRQ_MSK_ADT_SUCCESS));

            g_uchAdtContinuousMode = 0;

            HBD_I2cSendCmd(HBD_CMD_ADT);
            g_uchLastReadFifoCnt = 0;
            g_uchHbdStarted = HBD_START_STATUS_ADT_WEAR_MODE;
            chRet = HBD_RET_OK;
        }
        else
        {
            HBD_I2cSendCmd(HBD_CMD_SLEEP);
            HBD_DelayUs(HBD_SEND_CMD_DELAY_X_US);
            g_uchHbdInited = HBD_INIT_STATUS_NO_INIT;
            chRet = HBD_RET_NO_INITED_ERROR;
        }
    }
    return chRet;
}

/****************************************************************
* Description: start adt shedding detect
* Input:  None
* Return: HBD_RET_OK=success,
          HBD_RET_NO_INITED_ERROR=fail:don't init success
******************************************************************/
GS8 HBD_AdtSheddingDetectStart (void)
{
    GS8 chRet = HBD_RET_NO_INITED_ERROR;
    EXAMPLE_DEBUG_LOG_L1("[%s] exe.\r\n", __FUNCTION__);
    if (HBD_INIT_STATUS_INITED == g_uchHbdInited)
    {
        if (HBD_CheckChipInited() != 0) // check inited
        {
            GU16 usAdtCtrlRegVal = 0;

            HBD_ChangeToAdtModeRegVal();

            usAdtCtrlRegVal = HBD_I2cReadReg(HBD_ADT_CTRL_REG_ADDR);
            usAdtCtrlRegVal |= (HBD_ADT_CTRL_MSK_UNWEAR_EN);
            HBD_I2cWriteReg(HBD_ADT_CTRL_REG_ADDR, usAdtCtrlRegVal);
            HBD_I2cWriteReg(HBD_IRQ_CTRL_REG_ADDR, ((((GU16)g_usIrqPluseWidth - 1) << 8) | (1 << 5) /* idle state*/ | HBD_IRQ_MSK_ADT_SUCCESS));

            g_uchAdtContinuousMode = 0;

            HBD_I2cSendCmd(HBD_CMD_ADT);
            g_uchLastReadFifoCnt = 0;
            g_uchHbdStarted = HBD_START_STATUS_ADT_WEAR_MODE;
            chRet = HBD_RET_OK;
        }
        else
        {
            HBD_I2cSendCmd(HBD_CMD_SLEEP);
            HBD_DelayUs(HBD_SEND_CMD_DELAY_X_US);
            g_uchHbdInited = HBD_INIT_STATUS_NO_INIT;
            chRet = HBD_RET_NO_INITED_ERROR;
        }
    }
    return chRet;
}

/****************************************************************
* Description: start adt wear detect continuous
* Input:  None
* Return: HBD_RET_OK=success,
          HBD_RET_NO_INITED_ERROR=fail:don't init success
******************************************************************/
GS8 HBD_AdtWearContinuousDetectStart(void)
{
    GS8 chRet = HBD_AdtWearDetectStart();
    g_uchAdtContinuousMode = 1;
    return chRet;
}

/****************************************************************
* Description: check adt has started
* Input:  None
* Return: 1= has started,
          0= has not started
******************************************************************/
GU8 HBD_IsAdtWearDetectHasStarted (void)
{
    GU8 uchRet = 0;
    if ((g_uchHbdStarted == HBD_START_STATUS_ADT_WEAR_MODE) || (g_uchHbdStarted == HBD_START_STATUS_ADT_UNWEAR_MODE))
    {
        uchRet = 1;
    }
    return uchRet;
}

/****************************************************************
* Description: get Int status
* Input:  None
* Return: Status, see EM_INT_STATUS
******************************************************************/
GU8 HBD_GetIntStatus (void)
{
    GU8 uchIntStatus = INT_STATUS_INVALID;
    GU16 usIrqRegVal = 0;

    //if (g_uchHbdStarted >= HBD_START_STATUS_ADT_WEAR_MODE)   //wanpeng 20230518
    //{
    //    HBD_I2cSendCmd(HBD_CMD_IDLE);
    //    HBD_DelayUs(HBD_SEND_CMD_DELAY_X_US);
    //}
    usIrqRegVal = (GU8)(HBD_I2cReadReg(HBD_IRQ_STATUS_REG_ADDR) & HBD_IRQ_REG_IRQ_MSK);

    if (usIrqRegVal == HBD_IRQ_MSK_RESET)
    {
        uchIntStatus = INT_STATUS_CHIP_RESET;
        g_uchHbdInited = HBD_INIT_STATUS_NO_INIT;
    }
    else if (usIrqRegVal & HBD_IRQ_MSK_AUTO_LED_FAIL)
    {
        uchIntStatus = INT_STATUS_UNWEAR_DETECTED;
    }
    else if (usIrqRegVal & HBD_IRQ_MSK_FIFO_THR_FULL)
    {
        uchIntStatus = INT_STATUS_FIFO_WATERMARK;
        g_uchWrRegflag = 1;
    }
    else if (usIrqRegVal & HBD_IRQ_MSK_FIFO_FULL)
    {
        uchIntStatus = INT_STATUS_FIFO_FULL;
    }
    else if (usIrqRegVal & HBD_IRQ_MSK_ADT_SUCCESS)
    {
        if (g_uchHbdStarted == HBD_START_STATUS_ADT_WEAR_MODE)
        {
            GU16 usAdtStatus = HBD_I2cReadReg(HBD_ADT_CTRL_REG_ADDR) & HBD_ADT_CTRL_MSK_UNWEAR_FLAG;
            if (usAdtStatus != 0)
            {
                uchIntStatus = INT_STATUS_UNWEAR_DETECTED;
            }
            else
            {
                uchIntStatus = INT_STATUS_WEAR_DETECTED;
            }
            if (g_uchAdtContinuousMode == 1)
            {
                HBD_I2cSendCmd(HBD_CMD_ADT);
            }
            else
            {
                HBD_I2cSendCmd(HBD_CMD_SLEEP);
                HBD_DelayUs(HBD_SEND_CMD_DELAY_X_US);
            }
        }
    }
    else if (usIrqRegVal & HBD_IRQ_MSK_ADC_VALID)
    {
        uchIntStatus = INT_STATUS_NEW_DATA;
    }

    return uchIntStatus;
}

/****************************************************************
* Description: get reg Rawdata by newdata int.
* Input:  None,
* Output:   pnRawDataArr[0]:ptr of ppg1 output
            pnRawDataArr[1]:ptr of ppg2 output
            pnRawDataArr[2]:ptr of index output
* Return: None.
******************************************************************/
void HBD_GetNewdataIntData(GS32 *pnRawDataArr)
{
    pnRawDataArr[0] = HBD_I2cReadRawdataReg(g_usReadRawdataRegList[0]);
    if (g_uchNeedReadRawdataCount == 2)
    {
        pnRawDataArr[1] = HBD_I2cReadRawdataReg(g_usReadRawdataRegList[1]);
    }
    else
    {
        pnRawDataArr[1] = pnRawDataArr[0];
    }

    if (0 == HBD_AutoLedbyNewDataInt((GU32 *)pnRawDataArr) || g_uchKeepLedWorking)
    {
        HBD_I2cSendCmd(HBD_CMD_RESUME);// AutoLED Sucess Or Do not need to AutoLED,set to sleep
    }
    else{
        HBD_I2cSendCmd(HBD_CMD_IDLE);// AutoLED Fail,set to idle
    }

    pnRawDataArr[0] = (pnRawDataArr[0] << 7) & 0xFFFFFF80;
    pnRawDataArr[1] = (pnRawDataArr[1] << 7) & 0xFFFFFF80;

    GS32 nRawDataTransform = pnRawDataArr[0] & 0xffffff;
    //HBD_RawdataTransform((GU32*)&nRawDataTransform);
    pnRawDataArr[0] = ((pnRawDataArr[0] & (~0xffffff)) | nRawDataTransform);

    nRawDataTransform = pnRawDataArr[1] & 0xffffff;
    //HBD_RawdataTransform((GU32*)&nRawDataTransform);
    pnRawDataArr[1] = ((pnRawDataArr[1] & (~0xffffff)) | nRawDataTransform);

    pnRawDataArr[5] = (g_uchPackageSN & 0xFF);
    g_uchPackageSN++;
}




void Gh30xSetAdjFlagForSoftAgc(GU8 *puchRawdataFifo, GU16 usSamplePointNum)
{
    for(GU8 uchChnlCnt = 0; uchChnlCnt < g_uchNeedReadRawdataCount; uchChnlCnt ++)
    {
        if(g_stAutoLedConfig.stAutoLedChannel[uchChnlCnt].emAutoledEnable)
        {
            for(GU16 usSampleCnt = 0; usSampleCnt < usSamplePointNum; usSampleCnt += g_uchNeedReadRawdataCount)
            {
                GU16 usFifoHighByteIndex = (uchChnlCnt + usSampleCnt)*3;
                puchRawdataFifo[usFifoHighByteIndex] &= (~(1 << 7));  //clear adj flag
                Gh30xUpdateChnlGainInfo(uchChnlCnt,puchRawdataFifo[usFifoHighByteIndex]);
                if(g_pstGh30xChnlInfo[uchChnlCnt].uchGainChangeFlag)
                {
                    puchRawdataFifo[usFifoHighByteIndex] |= (1 << 7);
                }
            }
            if(g_pstGh30xChnlInfo[uchChnlCnt].uchDrvCurrentChangeFlag)
            {
                puchRawdataFifo[uchChnlCnt*3] |= (1 << 7);
            }
        }
    }

    if(g_pstGh30xChnlInfo[0].uchDrvCurrentChangeFlag)
    {
        puchRawdataFifo[0] |= (1 << 7);
    }
    if(2 == g_uchNeedReadRawdataCount)
    {
        if(g_pstGh30xChnlInfo[1].uchDrvCurrentChangeFlag)
        {
            puchRawdataFifo[3] |= (1 << 7);
        }
    }
}






/****************************************************************
* Description: get reg Rawdata by fifo int .
* Input:  None,
* Output:   puchRawdataFifo:ptr of fifo output
* Return: fifo point num
******************************************************************/
GU16 Gh30xGetFifoRawdata(GU8 *puchRawdataFifo, GU8 *puchNeedForceReadAgain)
{
    GU16 usRawdataFifoRealCnt = HBD_I2cReadReg(HBD_FIFO_DATA_CNT_ADDR);
    GU16 usIrqFifoFullRegValue = 0; //add to deal xiaoxun bug   20220208_goodix4318

    puchNeedForceReadAgain[0] = 0;
    EXAMPLE_DEBUG_LOG_L1("[Gh30xGetFifoRawdata] usRawdataFifoRealCnt = %d, %d.\r", (int)usRawdataFifoRealCnt, g_uchNeedReadRawdataCount);

    if (usRawdataFifoRealCnt > GH30X_RAWDATA_FIFO_BUF_SAMPLE_POINT_NUM_MAX)
    {
        usRawdataFifoRealCnt = GH30X_RAWDATA_FIFO_BUF_SAMPLE_POINT_NUM_MAX;
        puchNeedForceReadAgain[0] = 1;  //rawdata buffer is not enough big, we need force read again
        EXAMPLE_DEBUG_LOG_L1("[Gh30xGetFifoRawdata]rawdata buffer is not enough big, we need force read again.\r\n");
    }

    if ((usRawdataFifoRealCnt % g_uchNeedReadRawdataCount) == 1)
    {
        usRawdataFifoRealCnt -= 1;
    }

    if (usRawdataFifoRealCnt > 0)
    {
        usIrqFifoFullRegValue = HBD_I2cReadReg(HBD_GET_REG_REAL_ADRR(HBD_IRQ_CTRL_REG_ADDR)); // add to deal xiaoxun bug 20220208_goodix4318
        if (usIrqFifoFullRegValue & HBD_IRQ_MSK_FIFO_THR_FULL)
        {
            HBD_I2cWriteReg(HBD_GET_REG_REAL_ADRR(HBD_IRQ_CTRL_REG_ADDR), usIrqFifoFullRegValue & (~HBD_IRQ_MSK_FIFO_THR_FULL));
        }
        HBD_I2cReadFifo(puchRawdataFifo, usRawdataFifoRealCnt * 3);
        if (usIrqFifoFullRegValue & HBD_IRQ_MSK_FIFO_THR_FULL) // add to deal xiaoxun bug   20220208_goodix4318
        {
            HBD_I2cWriteReg(HBD_GET_REG_REAL_ADRR(HBD_IRQ_CTRL_REG_ADDR), usIrqFifoFullRegValue);
        }

        // add adj flag
        Gh30xSetAdjFlagForSoftAgc(puchRawdataFifo, usRawdataFifoRealCnt);
        Gh30xRawdata24BitTo32Bit(puchRawdataFifo, usRawdataFifoRealCnt, GH30X_RAWDATA_FIFO_BUF_SAMPLE_POINT_NUM_MAX);
    }
    g_uchLastReadFifoCnt = usRawdataFifoRealCnt;

    return usRawdataFifoRealCnt;   //fifo point num
}

/****************************************************************
* Description: calc gsensor float index .
* Input:  None,
* Output:   usReadRawdataFifoCnt: read rawdata fifo cnt
            usReadGsensorArrCnt: read gsensor array cnt
* Return: fGsensorFloatIndex: calc output .
******************************************************************/
#if 0
static GF32 HBD_CalcGsensorFloatIndex(GU16 usReadRawdataFifoCnt, GU16 usReadGsensorArrCnt)
{
    GF32 fGsensorFloatIndex = 0.0f;
    if ((usReadRawdataFifoCnt > 1) && (usReadGsensorArrCnt > 0))
    {
        fGsensorFloatIndex = ((GF32)(usReadGsensorArrCnt - 1)) / (usReadRawdataFifoCnt - 1);
    }
    return fGsensorFloatIndex;
}
#endif

#if __DEBUG_ADT_CONFIRM__
/****************************************************************
* Description: pack Rawdata with gsensor by fifo int.
* Input:  None,
* Output:   puchRawdataFifo:ptr of fifo data
usRawdataFifoRealCnt: fifo cnt
usRawdataIndex: index will get rawdata packet
stGsensorDataArr: gsensor data array
stGsensorDataRealCnt: gsensor data cnt
fGsensorDataIndex: gensor data index calc val
usGsensorDataRightShift: gsensor right shift val
nRawDataArrForAlgo: ptr of rawdata packet
nRawDataArrForDbg: ptr of rawdata packet for debug
pusRawdataOutLenForDbg: ptr of rawdata packet for debug len
* Return: None.
******************************************************************/
static void HBD_PackFifoRawdataWithGsensorAll(GU8 *puchRawdataFifo, GU16 usRawdataFifoRealCnt, GU16 usRawdataIndex,
    ST_GS_DATA_TYPE stGsensorDataArr[], GU16 stGsensorDataRealCnt, GF32 fGsensorDataIndex, GS16 usGsensorDataRightShift,
    GS32 nRawDataArrForAlgo[10], GS32 nRawDataArrForDbg[][6], GU16 *pusRawdataOutLenForDbg, GU16 usDbgMaxLen)
{
    GU16 usPPGIndex = (GU16)(usRawdataIndex/fGsensorDataIndex);
    if (usRawdataFifoRealCnt <= usPPGIndex)
    {
        usPPGIndex = usRawdataFifoRealCnt;
    }
    if (g_uchNeedReadRawdataCount == 2)
    {
        nRawDataArrForAlgo[0] = HBD_MAKEUP_DWORD(0x00, puchRawdataFifo[usPPGIndex * 6],
            puchRawdataFifo[usPPGIndex * 6 + 1], puchRawdataFifo[usPPGIndex * 6 + 2]);
        nRawDataArrForAlgo[1] = HBD_MAKEUP_DWORD(0x00, puchRawdataFifo[usPPGIndex * 6 + 3],
            puchRawdataFifo[usPPGIndex * 6 + 4], puchRawdataFifo[usPPGIndex * 6 + 5]);
    }
    else
    {
        nRawDataArrForAlgo[0] = HBD_MAKEUP_DWORD(0x00, puchRawdataFifo[usPPGIndex * 3],
            puchRawdataFifo[usPPGIndex * 3 + 1], puchRawdataFifo[usPPGIndex * 3 + 2]);
        nRawDataArrForAlgo[1] = nRawDataArrForAlgo[0];
    }

    nRawDataArrForAlgo[2] = (GS32)(stGsensorDataArr[usRawdataIndex].sXAxisVal / usGsensorDataRightShift);
    nRawDataArrForAlgo[3] = (GS32)(stGsensorDataArr[usRawdataIndex].sYAxisVal / usGsensorDataRightShift);
    nRawDataArrForAlgo[4] = (GS32)(stGsensorDataArr[usRawdataIndex].sZAxisVal / usGsensorDataRightShift);

    nRawDataArrForAlgo[5] = (usPPGIndex & 0xFF);

    if ((nRawDataArrForDbg != HBD_PTR_NULL) && (pusRawdataOutLenForDbg != HBD_PTR_NULL))
    { // fixed debug output
        if (usRawdataIndex < usDbgMaxLen)
        {
            nRawDataArrForDbg[usRawdataIndex][0] = nRawDataArrForAlgo[0];//& 0x0001FFFF; fixed 24bits
            nRawDataArrForDbg[usRawdataIndex][1] = nRawDataArrForAlgo[1];//& 0x0001FFFF; fixed 24bits
            nRawDataArrForDbg[usRawdataIndex][2] = nRawDataArrForAlgo[2];
            nRawDataArrForDbg[usRawdataIndex][3] = nRawDataArrForAlgo[3];
            nRawDataArrForDbg[usRawdataIndex][4] = nRawDataArrForAlgo[4];
            nRawDataArrForDbg[usRawdataIndex][5] = nRawDataArrForAlgo[5];
            *pusRawdataOutLenForDbg = usRawdataIndex + 1;
        }
        else
        {
            *pusRawdataOutLenForDbg = 0;
        }
    }
}
#endif

/****************************************************************
* Description: Resample G-sensor data from less to more
* Input:  const ST_GS_DATA_TYPE stGsensorDataArr[]  source G-sensor data
*         GU16 usGsensorDataRealCnt  source G-sensor data frames
*         GU16 usRawdataFifoRealCnt  destination G-sensor data frames
*         GU16 usRawdataIndex   current destination G-sensor data index
* Return: destination G-sensor data
******************************************************************/
#if 0
static ST_GS_DATA_TYPE HBD_ResampleGsensorData(const ST_GS_DATA_TYPE stGsensorDataArr[], GU16 usGsensorDataRealCnt, GU16 usRawdataFifoRealCnt, GU16 usRawdataIndex)
{
    GU16 usLastPos = usGsensorDataRealCnt - 1;
    GF32 fIndex = ((GF32)usRawdataIndex * usGsensorDataRealCnt) / usRawdataFifoRealCnt;
    GU16 usP1 = (GU16)fIndex;
    GU16 usP2 = (usP1 == usLastPos) ? usLastPos : usP1 + 1;
    GF32 fCoe = fIndex - usP1;
    ST_GS_DATA_TYPE stGsensorNewData;
    stGsensorNewData.sXAxisVal = (GS16)((1.0f - fCoe) * stGsensorDataArr[usP1].sXAxisVal + fCoe * stGsensorDataArr[usP2].sXAxisVal);
    stGsensorNewData.sYAxisVal = (GS16)((1.0f - fCoe) * stGsensorDataArr[usP1].sYAxisVal + fCoe * stGsensorDataArr[usP2].sYAxisVal);
    stGsensorNewData.sZAxisVal = (GS16)((1.0f - fCoe) * stGsensorDataArr[usP1].sZAxisVal + fCoe * stGsensorDataArr[usP2].sZAxisVal);
    return stGsensorNewData;
}
#endif

/****************************************************************
* Description: pack Rawdata with gsensor by fifo int.
* Input:  None,
* Output:   puchRawdataFifo:ptr of fifo data
            usRawdataFifoRealCnt: fifo cnt
            usRawdataIndex: index will get rawdata packet
            stGsensorDataArr: gsensor data array
            stGsensorDataRealCnt: gsensor data cnt
            fGsensorDataIndex: gensor data index calc val
            usGsensorDataRightShift: gsensor right shift val
            nRawDataArrForAlgo: ptr of rawdata packet
            nRawDataArrForDbg: ptr of rawdata packet for debug
            pusRawdataOutLenForDbg: ptr of rawdata packet for debug len
* Return: None.
******************************************************************/
#if 0
static void HBD_PackFifoRawdataWithGsensor(GU8 *puchRawdataFifo, GU16 usRawdataFifoRealCnt, GU16 usRawdataIndex,
                                           ST_GS_DATA_TYPE stGsensorDataArr[], GU16 stGsensorDataRealCnt, GF32 fGsensorDataIndex, GS16 usGsensorDataRightShift,
                                           GS32 nRawDataArrForAlgo[10], GS32 nRawDataArrForDbg[][6], GU16 *pusRawdataOutLenForDbg, GU16 usDbgMaxLen)
{
    if (g_uchNeedReadRawdataCount == 2)
    {
        nRawDataArrForAlgo[0] = HBD_MAKEUP_DWORD(0x00, puchRawdataFifo[usRawdataIndex * 6],
                                    puchRawdataFifo[usRawdataIndex * 6 + 1], puchRawdataFifo[usRawdataIndex * 6 + 2]);
        nRawDataArrForAlgo[1] = HBD_MAKEUP_DWORD(0x00, puchRawdataFifo[usRawdataIndex * 6 + 3],
                                    puchRawdataFifo[usRawdataIndex * 6 + 4], puchRawdataFifo[usRawdataIndex * 6 + 5]);
    }
    else
    {
        nRawDataArrForAlgo[0] = HBD_MAKEUP_DWORD(0x00, puchRawdataFifo[usRawdataIndex * 3],
                                    puchRawdataFifo[usRawdataIndex * 3 + 1], puchRawdataFifo[usRawdataIndex * 3 + 2]);
        nRawDataArrForAlgo[1] = nRawDataArrForAlgo[0];
    }

    GS32 sliRawdataArrForStore[2] = {0};
    sliRawdataArrForStore[0] = nRawDataArrForAlgo[0];
    sliRawdataArrForStore[1] = nRawDataArrForAlgo[1];

    nRawDataArrForAlgo[0] = (nRawDataArrForAlgo[0] << 7) & 0xFFFFFF80;
    nRawDataArrForAlgo[1] = (nRawDataArrForAlgo[1] << 7) & 0xFFFFFF80;

    //EXAMPLE_DEBUG_LOG_L1("sliRawdataArrForStore %x, %x\n\r nRawDataArrForAlgo = %x, %x\n\r",sliRawdataArrForStore[0],sliRawdataArrForStore[1],\
                                                             nRawDataArrForAlgo[0],nRawDataArrForAlgo[1]);

    GS32 nRawDataTransform = 0;
    nRawDataTransform = nRawDataArrForAlgo[0] & 0xffffff;
    HBD_RawdataTransform((GU32*)&nRawDataTransform);
    nRawDataArrForAlgo[0] = ((nRawDataArrForAlgo[0] & (~0xffffff)) | nRawDataTransform);

    nRawDataTransform = nRawDataArrForAlgo[1] & 0xffffff;
    HBD_RawdataTransform((GU32*)&nRawDataTransform);
    nRawDataArrForAlgo[1] = ((nRawDataArrForAlgo[1] & (~0xffffff)) | nRawDataTransform);
    //EXAMPLE_DEBUG_LOG_L1("HBD_RawdataTransform %x, %x\n\r",nRawDataArrForAlgo[0],nRawDataArrForAlgo[1]);

    if (stGsensorDataArr != HBD_PTR_NULL)
    {
        if(usRawdataFifoRealCnt <= stGsensorDataRealCnt)
        {
            GU16 usGsIndex = (GU16)(usRawdataIndex*fGsensorDataIndex);
            nRawDataArrForAlgo[2] = (GS32)(stGsensorDataArr[usGsIndex].sXAxisVal / usGsensorDataRightShift);
            nRawDataArrForAlgo[3] = (GS32)(stGsensorDataArr[usGsIndex].sYAxisVal / usGsensorDataRightShift);
            nRawDataArrForAlgo[4] = (GS32)(stGsensorDataArr[usGsIndex].sZAxisVal / usGsensorDataRightShift);
        }
        else
        {
            //Gsensor数据比PPG数据少的话就进行差值和重采样处理补齐
            ST_GS_DATA_TYPE stGsensorNewData = HBD_ResampleGsensorData(stGsensorDataArr, stGsensorDataRealCnt, usRawdataFifoRealCnt, usRawdataIndex);
            nRawDataArrForAlgo[2] = (GS32)(stGsensorNewData.sXAxisVal / usGsensorDataRightShift);
            nRawDataArrForAlgo[3] = (GS32)(stGsensorNewData.sYAxisVal / usGsensorDataRightShift);
            nRawDataArrForAlgo[4] = (GS32)(stGsensorNewData.sZAxisVal / usGsensorDataRightShift);
        }
    }
    nRawDataArrForAlgo[5] = (g_uchPackageSN & 0xFF);
    g_uchPackageSN++;
    if ((nRawDataArrForDbg != HBD_PTR_NULL) && (pusRawdataOutLenForDbg != HBD_PTR_NULL))
    { // fixed debug output
        if (usRawdataIndex < usDbgMaxLen)
        {
            nRawDataArrForDbg[usRawdataIndex][0] = sliRawdataArrForStore[0];//& 0x0001FFFF; fixed 24bits
            nRawDataArrForDbg[usRawdataIndex][1] = sliRawdataArrForStore[1];//& 0x0001FFFF; fixed 24bits
            nRawDataArrForDbg[usRawdataIndex][2] = nRawDataArrForAlgo[2];
            nRawDataArrForDbg[usRawdataIndex][3] = nRawDataArrForAlgo[3];
            nRawDataArrForDbg[usRawdataIndex][4] = nRawDataArrForAlgo[4];
            nRawDataArrForDbg[usRawdataIndex][5] = nRawDataArrForAlgo[5];
            *pusRawdataOutLenForDbg = usRawdataIndex + 1;
        }
        else
        {
            *pusRawdataOutLenForDbg = 0;
        }
    }
}
#endif

/****************************************************************
* Description: pack Rawdata with gsensor by fifo int.output U8 rawdata
* Input:  None,
* Output:   puchRawdataFifo:ptr of fifo data
            usRawdataFifoRealCnt: fifo cnt
            usRawdataIndex: index will get rawdata packet
            stGsensorDataArr: gsensor data array
            stGsensorDataRealCnt: gsensor data cnt
            fGsensorDataIndex: gensor data index calc val
            usGsensorDataRightShift: gsensor right shift val
            nRawDataArrForAlgo: ptr of rawdata packet
            uchRawDataArrForDbg: ptr of rawdata packet for debug
            pusRawdataOutLenForDbg: ptr of rawdata packet for debug len
            uchDbgRawdataPkgLen: one pkg len
* Return: None.
******************************************************************/
#if 0
static void HBD_PackFifoRawdataWithGsensorDbgU8(GU8 *puchRawdataFifo, GU16 usRawdataFifoRealCnt, GU16 usRawdataIndex,
                                           ST_GS_DATA_TYPE stGsensorDataArr[], GU16 stGsensorDataRealCnt, GF32 fGsensorDataIndex, GS16 usGsensorDataRightShift,
                                           GS32 nRawDataArrForAlgo[10], GU8 uchRawDataArrForDbg[], GU16 *pusRawdataOutLenForDbg, GU8 uchDbgRawdataPkgLen, GU16 usDbgMaxLen)
{
    if (g_uchNeedReadRawdataCount == 2)
    {
        nRawDataArrForAlgo[0] = HBD_MAKEUP_DWORD(0x00, puchRawdataFifo[usRawdataIndex * 6],
                                    puchRawdataFifo[usRawdataIndex * 6 + 1], puchRawdataFifo[usRawdataIndex * 6 + 2]);
        nRawDataArrForAlgo[1] = HBD_MAKEUP_DWORD(0x00, puchRawdataFifo[usRawdataIndex * 6 + 3],
                                    puchRawdataFifo[usRawdataIndex * 6 + 4], puchRawdataFifo[usRawdataIndex * 6 + 5]);
    }
    else
    {
        nRawDataArrForAlgo[0] = HBD_MAKEUP_DWORD(0x00, puchRawdataFifo[usRawdataIndex * 3],
                                    puchRawdataFifo[usRawdataIndex * 3 + 1], puchRawdataFifo[usRawdataIndex * 3 + 2]);
        nRawDataArrForAlgo[1] = nRawDataArrForAlgo[0];
    }

    GS32 sliRawdataArrForStore[2] = {0};
    sliRawdataArrForStore[0] = nRawDataArrForAlgo[0];
    sliRawdataArrForStore[1] = nRawDataArrForAlgo[1];

    nRawDataArrForAlgo[0] = (nRawDataArrForAlgo[0] << 7) & 0xFFFFFF80;
    nRawDataArrForAlgo[1] = (nRawDataArrForAlgo[1] << 7) & 0xFFFFFF80;
    if (stGsensorDataArr != HBD_PTR_NULL)
    {
        if(usRawdataFifoRealCnt <= stGsensorDataRealCnt)
        {
            GU16 usGsIndex = (GU16)(usRawdataIndex*fGsensorDataIndex);
            nRawDataArrForAlgo[2] = (GS32)(stGsensorDataArr[usGsIndex].sXAxisVal / usGsensorDataRightShift);
            nRawDataArrForAlgo[3] = (GS32)(stGsensorDataArr[usGsIndex].sYAxisVal / usGsensorDataRightShift);
            nRawDataArrForAlgo[4] = (GS32)(stGsensorDataArr[usGsIndex].sZAxisVal / usGsensorDataRightShift);
        }
        else
        {
            //Gsensor数据比PPG数据少的话就进行差值和重采样处理补齐
            ST_GS_DATA_TYPE stGsensorNewData = HBD_ResampleGsensorData(stGsensorDataArr, stGsensorDataRealCnt, usRawdataFifoRealCnt, usRawdataIndex);
            nRawDataArrForAlgo[2] = (GS32)(stGsensorNewData.sXAxisVal / usGsensorDataRightShift);
            nRawDataArrForAlgo[3] = (GS32)(stGsensorNewData.sYAxisVal / usGsensorDataRightShift);
            nRawDataArrForAlgo[4] = (GS32)(stGsensorNewData.sZAxisVal / usGsensorDataRightShift);
        }
    }
    nRawDataArrForAlgo[5] = (g_uchPackageSN & 0xFF);
    g_uchPackageSN++;
    if ((uchRawDataArrForDbg != HBD_PTR_NULL) && (pusRawdataOutLenForDbg != HBD_PTR_NULL))
    { // fixed debug output
        if (usRawdataIndex < usDbgMaxLen)
        {
            // get 24bits data
            uchRawDataArrForDbg[usRawdataIndex * uchDbgRawdataPkgLen + 0] = HBD_GET_BYTE0_FROM_DWORD(sliRawdataArrForStore[0]);//& 0x0001FFFF; fixed 24bits
            uchRawDataArrForDbg[usRawdataIndex * uchDbgRawdataPkgLen + 1] = HBD_GET_BYTE1_FROM_DWORD(sliRawdataArrForStore[0]);//& 0x0001FFFF; fixed 24bits
            uchRawDataArrForDbg[usRawdataIndex * uchDbgRawdataPkgLen + 2] = HBD_GET_BYTE2_FROM_DWORD(sliRawdataArrForStore[0]);//& 0x0001FFFF; fixed 24bits
            // get 24bits data
            uchRawDataArrForDbg[usRawdataIndex * uchDbgRawdataPkgLen + 3] = HBD_GET_BYTE0_FROM_DWORD(sliRawdataArrForStore[1]);//& 0x0001FFFF; fixed 24bits
            uchRawDataArrForDbg[usRawdataIndex * uchDbgRawdataPkgLen + 4] = HBD_GET_BYTE1_FROM_DWORD(sliRawdataArrForStore[1]);//& 0x0001FFFF; fixed 24bits
            uchRawDataArrForDbg[usRawdataIndex * uchDbgRawdataPkgLen + 5] = HBD_GET_BYTE2_FROM_DWORD(sliRawdataArrForStore[1]);//& 0x0001FFFF; fixed 24bits
            if (uchDbgRawdataPkgLen == 13) // need output gsensor data
            {
                uchRawDataArrForDbg[usRawdataIndex * uchDbgRawdataPkgLen + 6] = HBD_GET_BYTE0_FROM_DWORD(nRawDataArrForAlgo[2]);
                uchRawDataArrForDbg[usRawdataIndex * uchDbgRawdataPkgLen + 7] = HBD_GET_BYTE1_FROM_DWORD(nRawDataArrForAlgo[2]);
                uchRawDataArrForDbg[usRawdataIndex * uchDbgRawdataPkgLen + 8] = HBD_GET_BYTE0_FROM_DWORD(nRawDataArrForAlgo[3]);
                uchRawDataArrForDbg[usRawdataIndex * uchDbgRawdataPkgLen + 9] = HBD_GET_BYTE1_FROM_DWORD(nRawDataArrForAlgo[3]);
                uchRawDataArrForDbg[usRawdataIndex * uchDbgRawdataPkgLen + 10] = HBD_GET_BYTE0_FROM_DWORD(nRawDataArrForAlgo[4]);
                uchRawDataArrForDbg[usRawdataIndex * uchDbgRawdataPkgLen + 11] = HBD_GET_BYTE1_FROM_DWORD(nRawDataArrForAlgo[4]);
                uchRawDataArrForDbg[usRawdataIndex * uchDbgRawdataPkgLen + 12] = HBD_GET_BYTE0_FROM_DWORD(nRawDataArrForAlgo[5]);
            }
            else
            {
                uchRawDataArrForDbg[usRawdataIndex * uchDbgRawdataPkgLen + 6] = HBD_GET_BYTE0_FROM_DWORD(nRawDataArrForAlgo[5]);
            }
            *pusRawdataOutLenForDbg = usRawdataIndex + 1;
        }
        else
        {
            *pusRawdataOutLenForDbg = 0;
        }
    }
}


static GU16 HBD_GetFifoRawdata(GU8 *puchRawdataFifo)
{
	return 0;
}


static GU16 HBD_GetFifoRawdataWithoutAutoled(GU8 *puchRawdataFifo)
{
    GU16 usRawdataFifoRealCnt = HBD_I2cReadReg(HBD_FIFO_DATA_CNT_ADDR);
    GU16 usIrqFifoFullRegValue = 0; //add to deal xiaoxun bug   20220208_goodix4318
    usRawdataFifoRealCnt = (usRawdataFifoRealCnt > HBD_READ_FIFO_MAX_CNT)? (HBD_READ_FIFO_MAX_CNT): (usRawdataFifoRealCnt);
    if((usRawdataFifoRealCnt % g_uchNeedReadRawdataCount) == 1)
    {
        usRawdataFifoRealCnt -= 1;
    }
//
    if (usRawdataFifoRealCnt > 0)
    {
        usIrqFifoFullRegValue = HBD_I2cReadReg(HBD_GET_REG_REAL_ADRR(HBD_IRQ_CTRL_REG_ADDR));//add to deal xiaoxun bug   20220208_goodix4318
        if(usIrqFifoFullRegValue & HBD_IRQ_MSK_FIFO_THR_FULL)
        {
            HBD_I2cWriteReg(HBD_GET_REG_REAL_ADRR(HBD_IRQ_CTRL_REG_ADDR), usIrqFifoFullRegValue & (~HBD_IRQ_MSK_FIFO_THR_FULL));
        }
        HBD_I2cReadFifo(puchRawdataFifo, usRawdataFifoRealCnt * 3);
        if(usIrqFifoFullRegValue & HBD_IRQ_MSK_FIFO_THR_FULL)//add to deal xiaoxun bug   20220208_goodix4318
        {
            HBD_I2cWriteReg(HBD_GET_REG_REAL_ADRR(HBD_IRQ_CTRL_REG_ADDR), usIrqFifoFullRegValue);
        }
        usRawdataFifoRealCnt = usRawdataFifoRealCnt / g_uchNeedReadRawdataCount;
    }
    g_uchLastReadFifoCnt = usRawdataFifoRealCnt;

    return usRawdataFifoRealCnt;
}

/****************************************************************
* Description: pack Rawdata with gsensor by fifo int.
* Input:  None,
* Output:   puchRawdataFifo:ptr of fifo data
            usRawdataFifoRealCnt: fifo cnt
            usRawdataIndex: index will get rawdata packet
            stGsensorDataArr: gsensor data array
            stGsensorDataRealCnt: gsensor data cnt
            fGsensorDataIndex: gensor data index calc val
            usGsensorDataRightShift: gsensor right shift val
            nRawDataArrForAlgo: ptr of rawdata packet
            nRawDataArrForDbg: ptr of rawdata packet for debug
            pusRawdataOutLenForDbg: ptr of rawdata packet for debug len
* Return: None.
******************************************************************/
static void HBD_PackFifoRawdataWithoutGsensor(GU8 *puchRawdataFifo, GU16 usRawdataFifoRealCnt,
                                           GU16 usRawdataIndex, GS32 nRawDataArrForAlgo[10])
{
    if (g_uchNeedReadRawdataCount == 2)
    {
        nRawDataArrForAlgo[0] = HBD_MAKEUP_DWORD(0x00, puchRawdataFifo[usRawdataIndex * 6],
                                    puchRawdataFifo[usRawdataIndex * 6 + 1], puchRawdataFifo[usRawdataIndex * 6 + 2]);
        nRawDataArrForAlgo[1] = HBD_MAKEUP_DWORD(0x00, puchRawdataFifo[usRawdataIndex * 6 + 3],
                                    puchRawdataFifo[usRawdataIndex * 6 + 4], puchRawdataFifo[usRawdataIndex * 6 + 5]);
    }
    else
    {
        nRawDataArrForAlgo[0] = HBD_MAKEUP_DWORD(0x00, puchRawdataFifo[usRawdataIndex * 3],
                                    puchRawdataFifo[usRawdataIndex * 3 + 1], puchRawdataFifo[usRawdataIndex * 3 + 2]);
        nRawDataArrForAlgo[1] = nRawDataArrForAlgo[0];
    }

    nRawDataArrForAlgo[0] = (nRawDataArrForAlgo[0] << 7) & 0xFFFFFF80;
    nRawDataArrForAlgo[1] = (nRawDataArrForAlgo[1] << 7) & 0xFFFFFF80;

    GS32 nRawDataTransform = 0;
    nRawDataTransform = nRawDataArrForAlgo[0] & 0xffffff;
    HBD_RawdataTransform((GU32*)&nRawDataTransform);
    nRawDataArrForAlgo[0] = ((nRawDataArrForAlgo[0] & (~0xffffff)) | nRawDataTransform);

    nRawDataTransform = nRawDataArrForAlgo[1] & 0xffffff;
    HBD_RawdataTransform((GU32*)&nRawDataTransform);
    nRawDataArrForAlgo[1] = ((nRawDataArrForAlgo[1] & (~0xffffff)) | nRawDataTransform);


    nRawDataArrForAlgo[5] = (g_uchPackageSN & 0xFF);
    g_uchPackageSN++;
}
#endif

/****************************************************************
* Description: calculate hbd_value and wearing detect by newdata int.
* Input:  stGsAxisValue: gsensor data
          emGsensorSensitivity: G-sensor sensitivity (counts/g), see EM_HBD_GSENSOR_SENSITIVITY
* Output:   puchHbValue:ptr of heartbeat value output
            puchWearingState:ptr of wearing state output
            puchWearingQuality: ptr of wearing quality output
            puchVoiceBroadcast: ptr of voice broadcast output
            pusRRvalue: ptr of RR value output
* Return: refresh flag, 1:heartbeat refresh, 2:wearingstate refresh,3: heartbeat&wearingstate refresh.
******************************************************************/
GS8 HBD_HbCalculateByNewdataIntDbg(ST_GS_DATA_TYPE *stGsAxisValue, EM_HBD_GSENSOR_SENSITIVITY emGsensorSensitivity, GU8 *puchHbValue, GU8 *puchWearingState, GU8 *puchWearingQuality, GU8 *puchVoiceBroadcast, GU16 *pusRRvalue)
{
    GU8 uchRet = 0;
    if ((g_uchHbdStarted == HBD_START_STATUS_HB_MODE) || (g_uchHbdStarted == HBD_START_STATUS_IN_EAR_EP_WEAR_DETECT_MODE))
    {
        GS32 nRawDataArr[10] = {0};
        //GS32 nHeartBeatArr[7] = {0};
        GS16 sGsensorRightShift = (1 << emGsensorSensitivity);
        *puchWearingState = 1;
        *puchWearingQuality = 0;

        if (stGsAxisValue != HBD_PTR_NULL) // fix
        {
            nRawDataArr[2] = (GS32)(stGsAxisValue->sXAxisVal / sGsensorRightShift);
            nRawDataArr[3] = (GS32)(stGsAxisValue->sYAxisVal / sGsensorRightShift);
            nRawDataArr[4] = (GS32)(stGsAxisValue->sZAxisVal / sGsensorRightShift);
        }
        HBD_GetNewdataIntData(nRawDataArr);

        ST_ALGO_CALCULATE_INFO algo_calc_info = {0};
        ST_ALGO_RESULT algo_calc_result = {0};

        #ifndef HBD_TEST_NO_ALGO
        if(0 == g_uchCloseHBAlgo)
        {
            nRawDataArr[7] = 0;//g_nReferenceHbValue
            algo_calc_info.uchFrameID = nRawDataArr[5];
            algo_calc_info.nRawdata[0] = nRawDataArr[0] & 0xffffff;
            memcpy(algo_calc_info.sAccData, &nRawDataArr[2], 3 * sizeof(GS32));
            algo_calc_info.unSleepFlag = 0;//g_sleep_flg
        
            //HOOK_FUNC_CALL(g_pAlgoCalculateHookFunc, (GH3011_FUNC_OFFSET_HR, &algo_calc_info, &algo_calc_result));
            if(g_pAlgoCalculateHookFunc)
                g_pAlgoCalculateHookFunc(GH3011_FUNC_OFFSET_HR, &algo_calc_info, &algo_calc_result);
            uchRet = algo_calc_result.uchUpdateFlag;
        }
        #endif

        if(g_chWearIsEnableFlag == 1)
        {
            algo_calc_info.led_current[1] = g_uchAutoLedCurrentArr[1];
            algo_calc_info.nRawdata[0] = nRawDataArr[0] & 0xffffffff;
            algo_calc_info.nRawdata[1] = nRawDataArr[1] & 0xffffffff;
           
            //HOOK_FUNC_CALL(g_pAlgoCalculateHookFunc, (GH3011_FUNC_OFFSET_NADT, &algo_calc_info, &algo_calc_result));
            if(g_pAlgoCalculateHookFunc)
                g_pAlgoCalculateHookFunc(GH3011_FUNC_OFFSET_NADT, &algo_calc_info, &algo_calc_result);
        }

        if (g_uchHbdStarted == HBD_START_STATUS_HB_MODE)
        {
            *puchHbValue = (GU8)algo_calc_result.snResult[0];
            *puchWearingQuality = 0;
            *puchVoiceBroadcast = 0;
            *pusRRvalue = 0;
        }

        if(g_chWearIsEnableFlag == 1)
        {
            *puchWearingState = algo_calc_result.snResult[12];
            *puchWearingQuality = algo_calc_result.snResult[13];
        }
        else
        {
            *puchWearingState = 1;
            *puchWearingQuality = 100;
        }

        if (g_uchAutoLedErrorFlag == 1)
        {
            *puchWearingState = 2;
        }
    }
    return uchRet;
}

/****************************************************************
* Description: calculate hbd_value and wearing detect by newdata int.
* Input:  stGsAxisValue: gsensor data
          emGsensorSensitivity: G-sensor sensitivity (counts/g), see EM_HBD_GSENSOR_SENSITIVITY
* Output: *pstHbRes:ptr of hb calculate result,see ST_HB_RES
* Return: refresh flag, 1:heartbeat refresh, 2:wearingstate refresh,3: heartbeat&wearingstate refresh.
******************************************************************/
GS8 HBD_HbCalculateByNewdataIntDbgEx(ST_GS_DATA_TYPE *stGsAxisValue, EM_HBD_GSENSOR_SENSITIVITY emGsensorSensitivity, ST_HB_RES *pstHbRes)
{
    GU8 uchRet = 0;
    if ((g_uchHbdStarted == HBD_START_STATUS_HB_MODE) || (g_uchHbdStarted == HBD_START_STATUS_IN_EAR_EP_WEAR_DETECT_MODE))
    {
        GS32 nRawDataArr[10] = {0};
        //GS32 nHeartBeatArr[7] = {0};
        GS16 sGsensorRightShift = (1 << emGsensorSensitivity);
        pstHbRes->uchWearingState = 1;
        pstHbRes->uchWearingQuality = 0;

        if (stGsAxisValue != HBD_PTR_NULL) // fix
        {
            nRawDataArr[2] = (GS32)(stGsAxisValue->sXAxisVal / sGsensorRightShift);
            nRawDataArr[3] = (GS32)(stGsAxisValue->sYAxisVal / sGsensorRightShift);
            nRawDataArr[4] = (GS32)(stGsAxisValue->sZAxisVal / sGsensorRightShift);
        }
        HBD_GetNewdataIntData(nRawDataArr);
        ST_ALGO_CALCULATE_INFO algo_calc_info = {0};
        ST_ALGO_RESULT algo_calc_result = {0};
        #ifndef HBD_TEST_NO_ALGO
        if(0 == g_uchCloseHBAlgo)
        {
            nRawDataArr[7] = 0;//g_nReferenceHbValue
            //uchRet = HBACalculate(nRawDataArr, nHeartBeatArr, &g_stHbaInfo); // calc hb
            algo_calc_info.uchFrameID = nRawDataArr[5];
            algo_calc_info.nRawdata[0] = nRawDataArr[0] & 0xffffff;
            memcpy(algo_calc_info.sAccData, &nRawDataArr[2], 3 * sizeof(GS32));
            algo_calc_info.unSleepFlag = 0;//g_sleep_flg
            
            //HOOK_FUNC_CALL(g_pAlgoCalculateHookFunc, (GH3011_FUNC_OFFSET_HR, &algo_calc_info, &algo_calc_result));
            if(g_pAlgoCalculateHookFunc)
                g_pAlgoCalculateHookFunc(GH3011_FUNC_OFFSET_HR, &algo_calc_info, &algo_calc_result);
            uchRet = algo_calc_result.uchUpdateFlag;
        }
        #endif

        if(g_chWearIsEnableFlag == 1)
        {
            algo_calc_info.led_current[1] = g_uchAutoLedCurrentArr[1];
            algo_calc_info.nRawdata[0] = nRawDataArr[0] & 0xffffffff;
            algo_calc_info.nRawdata[1] = nRawDataArr[1] & 0xffffffff;
           
            //HOOK_FUNC_CALL(g_pAlgoCalculateHookFunc, (GH3011_FUNC_OFFSET_NADT, &algo_calc_info, &algo_calc_result));
            if(g_pAlgoCalculateHookFunc)
                g_pAlgoCalculateHookFunc(GH3011_FUNC_OFFSET_NADT, &algo_calc_info, &algo_calc_result);
        }
        if (g_uchHbdStarted == HBD_START_STATUS_HB_MODE)
        {
            pstHbRes->uchHbValue = (GU8)algo_calc_result.snResult[0];
            pstHbRes->uchScene = (GU8)algo_calc_result.snResult[2];
            pstHbRes->uchSNR = (GU8)algo_calc_result.snResult[3];
            pstHbRes->uchAccuracyLevel = (GU8)algo_calc_result.snResult[4];
            pstHbRes->uchMotionState = (GU8)algo_calc_result.snResult[5];
            pstHbRes->uchSleepFlag = 0;//g_sleep_flg
        }

        if(g_chWearIsEnableFlag == 1)
        {
            pstHbRes->uchWearingState = algo_calc_result.snResult[12];
            pstHbRes->uchWearingQuality = algo_calc_result.snResult[13];
        }
        else
        {
            pstHbRes->uchWearingState = 1;
            pstHbRes->uchWearingQuality = 100;
        }

        if (g_uchAutoLedErrorFlag == 1)
        {
            pstHbRes->uchWearingState = 2;
        }
    }
    return uchRet;
}

/****************************************************************
* Description: calculate hbd_value and wearing detect by fifo int.
* Input:  stGsAxisValue: gsensor data buffer
             usGsDataNum: gsensor data count
             emGsensorSensitivity: G-sensor sensitivity (counts/g), see EM_HBD_GSENSOR_SENSITIVITY
* Output:   puchHbValue:ptr of heartbeat value output
            puchWearingState:ptr of wearing state output
            puchWearingQuality: ptr of wearing quality output
            puchVoiceBroadcast: ptr of voice broadcast output
            pusRRvalue: ptr of RR value output
* Return: refresh flag, if heartbeat value have refresh that return 1.
******************************************************************/
#if 0
GU8 HBD_HbCalculateByFifoInt(ST_GS_DATA_TYPE stGsAxisValue[], GU16 usGsDataNum, EM_HBD_GSENSOR_SENSITIVITY emGsensorSensitivity, GU8 *puchHbValue, GU8 *puchWearingState, GU8 *puchWearingQuality, GU8 *puchVoiceBroadcast, GU16 *pusRRvalue)
{
    GU8 uchAccuracyLevel;
    GU16 usRawdataOutLen = 0;
    return HBD_HbCalculateWithLvlByFifoIntDebugOutputData(stGsAxisValue, usGsDataNum, emGsensorSensitivity,
                                                          puchHbValue, &uchAccuracyLevel, puchWearingState, puchWearingQuality, puchVoiceBroadcast, pusRRvalue,
                                                           HBD_PTR_NULL, &usRawdataOutLen);

}
#endif

/****************************************************************
* Description: calculate hbd_value and wearing detect by fifo int.
* Input:  stGsAxisValue: gsensor data buffer
             usGsDataNum: gsensor data count
             emGsensorSensitivity: G-sensor sensitivity (counts/g), see EM_HBD_GSENSOR_SENSITIVITY
* Output:   puchHbValue:ptr of heartbeat value output
            puchWearingState:ptr of wearing state output
            puchWearingQuality: ptr of wearing quality output
            puchVoiceBroadcast: ptr of voice broadcast output
            pusRRvalue: ptr of RR value output
            nRawdataOut: ptr of rawdata array output (ppg1, ppg2, gs-x, gs-y, gs-z, index)
            pusRawdataOutLen: ptr of rawdata len output
* Return: refresh flag, if heartbeat value have refresh that return 1.
******************************************************************/
#if 0
GU8 HBD_HbCalculateByFifoIntDebugOutputData(ST_GS_DATA_TYPE stGsAxisValue[], GU16 usGsDataNum, EM_HBD_GSENSOR_SENSITIVITY emGsensorSensitivity,
                                            GU8 *puchHbValue, GU8 *puchWearingState, GU8 *puchWearingQuality, GU8 *puchVoiceBroadcast, GU16 *pusRRvalue,
                                            GS32 nRawdataOut[][6], GU16 *pusRawdataOutLen)
{
    GU8 uchAccuracyLevel;
    return HBD_HbCalculateWithLvlByFifoIntDebugOutputData(stGsAxisValue, usGsDataNum, emGsensorSensitivity,
                                                          puchHbValue, &uchAccuracyLevel, puchWearingState, puchWearingQuality, puchVoiceBroadcast, pusRRvalue,
                                                           nRawdataOut, pusRawdataOutLen);

}
#endif

/****************************************************************
* Description: calculate hbd_value and wearing detect by fifo int.
* Input:  stGsAxisValue: gsensor data buffer
             usGsDataNum: gsensor data count
             emGsensorSensitivity: G-sensor sensitivity (counts/g), see EM_HBD_GSENSOR_SENSITIVITY
* Output:   puchHbValue:ptr of heartbeat value output
            puchAccuracyLevel: ptr of heartbeat accuracy level(1-3), max 3
            puchWearingState:ptr of wearing state output
            puchWearingQuality: ptr of wearing quality output
            puchVoiceBroadcast: ptr of voice broadcast output
            pusRRvalue: ptr of RR value output
* Return: refresh flag, if heartbeat value have refresh that return 1.
******************************************************************/
#if 0
GU8 HBD_HbCalculateWithLvlByFifoInt(ST_GS_DATA_TYPE stGsAxisValue[], GU16 usGsDataNum, EM_HBD_GSENSOR_SENSITIVITY emGsensorSensitivity,
                                    GU8 *puchHbValue, GU8 *puchAccuracyLevel, GU8 *puchWearingState, GU8 *puchWearingQuality, GU8 *puchVoiceBroadcast, GU16 *pusRRvalue)
{
    GU16 usRawdataOutLen = 0;
    return HBD_HbCalculateWithLvlByFifoIntDebugOutputData(stGsAxisValue, usGsDataNum, emGsensorSensitivity,
                                                          puchHbValue, puchAccuracyLevel, puchWearingState, puchWearingQuality, puchVoiceBroadcast, pusRRvalue,
                                                           HBD_PTR_NULL, &usRawdataOutLen);

}
#endif

/****************************************************************
* Description: calculate hbd_value and wearing detect by fifo int.
* Input:  stGsAxisValue: gsensor data buffer
             usGsDataNum: gsensor data count
             emGsensorSensitivity: G-sensor sensitivity (counts/g), see EM_HBD_GSENSOR_SENSITIVITY
* Output:   puchHbValue:ptr of heartbeat value output
            puchAccuracyLevel: ptr of heartbeat accuracy level(1-3), max 3
            puchWearingState:ptr of wearing state output
            puchWearingQuality: ptr of wearing quality output
            puchVoiceBroadcast: ptr of voice broadcast output
            pusRRvalue: ptr of RR value output
            nRawdataOut: ptr of rawdata array output (ppg1, ppg2, gs-x, gs-y, gs-z, index)
            pusRawdataOutLen:ptr of rawdata len output
* Return: refresh flag, if heartbeat value have refresh that return 1.
******************************************************************/
#if 0
GS8 HBD_HbCalculateWithLvlByFifoIntDebugOutputData(ST_GS_DATA_TYPE stGsAxisValue[], GU16 usGsDataNum, EM_HBD_GSENSOR_SENSITIVITY emGsensorSensitivity,
                                            GU8 *puchHbValue, GU8 *puchAccuracyLevel, GU8 *puchWearingState, GU8 *puchWearingQuality, GU8 *puchVoiceBroadcast, GU16 *pusRRvalue,
                                            GS32 nRawdataOut[][6], GU16 *pusRawdataOutLen)
{
    GU8 uchRet = 0;
    //GU8 uchAlgoRet = 0;
    GU8 uchAlgoHasRefreshFlag = 0;
    GU16 usDbgRawdataMaxLen = (*pusRawdataOutLen);
    *pusRawdataOutLen = 0;
    if ((g_uchHbdStarted == HBD_START_STATUS_HB_MODE) || (g_uchHbdStarted == HBD_START_STATUS_IN_EAR_EP_WEAR_DETECT_MODE))
    {
        GU16 usRawdataFifoCnt = 0;
        GF32 fIndexInc = 0.0f;
        GU16 usIndex = 0;
        GS32 nRawDataArr[10] = {0};
        //GS32 nHeartBeatArr[7] = {0};
        GS16 sGsensorRightShift = (1 << emGsensorSensitivity);
        //GU8 uchNadtRes[2] = {0};
        *puchWearingState = 1;
        *puchWearingQuality = 0;
        if (usDbgRawdataMaxLen == 0)
        {
            usDbgRawdataMaxLen = 0xFFFF;
        }
        //HBD_AutoLedCalcAccSTD(stGsAxisValue, usGsDataNum, &g_stAutoLedAccInfo, sGsensorRightShift);
        usRawdataFifoCnt = HBD_GetFifoRawdata(g_uchReadFifoBuffArr);
        fIndexInc = HBD_CalcGsensorFloatIndex(usRawdataFifoCnt, usGsDataNum);
        if (usRawdataFifoCnt > 0)
        {
            ST_ALGO_CALCULATE_INFO algo_calc_info = {0};
            ST_ALGO_RESULT algo_calc_result = {0};
            for (usIndex = 0; usIndex < usRawdataFifoCnt; usIndex++)
            {
                HBD_PackFifoRawdataWithGsensor(g_uchReadFifoBuffArr, usRawdataFifoCnt, usIndex,
                                           stGsAxisValue, usGsDataNum, fIndexInc, sGsensorRightShift,
                                           nRawDataArr, nRawdataOut, pusRawdataOutLen, usDbgRawdataMaxLen);

                #ifndef HBD_TEST_NO_ALGO
                if(0 == g_uchCloseHBAlgo)
                {
                    nRawDataArr[7] = 0;//g_nReferenceHbValue
                    //uchAlgoRet = HBACalculate(nRawDataArr, nHeartBeatArr, &g_stHbaInfo); // calc hb
                    algo_calc_info.uchFrameID = nRawDataArr[5];
                    algo_calc_info.nRawdata[0] = nRawDataArr[0] & 0xffffff;
                    memcpy(algo_calc_info.sAccData, &nRawDataArr[2], 3 * sizeof(GS32));
                    algo_calc_info.unSleepFlag = 0;//g_sleep_flg
                    HOOK_FUNC_CALL(g_pAlgoCalculateHookFunc, (GH3011_FUNC_OFFSET_HR, &algo_calc_info, &algo_calc_result));
                }
                #endif
                if(g_chWearIsEnableFlag == 1)
                {
                    algo_calc_info.nRawdata[0] = nRawDataArr[0] & 0xffffffff;
                    algo_calc_info.nRawdata[1] = nRawDataArr[1] & 0xffffffff;
                    algo_calc_info.led_current[1] = g_uchAutoLedCurrentArr[1];
                    HOOK_FUNC_CALL(g_pAlgoCalculateHookFunc, (GH3011_FUNC_OFFSET_NADT, &algo_calc_info, &algo_calc_result));
                }
                if ((algo_calc_result.uchUpdateFlag & 0x01) != 0) // hb refresh
                {
                    if (g_uchHbdStarted == HBD_START_STATUS_HB_MODE)
                    {
                        uchRet = 1;
                        uchAlgoHasRefreshFlag |= 0x01;
                        *puchHbValue = (GU8)algo_calc_result.snResult[0];
                        *puchWearingQuality = 0;
                        *puchVoiceBroadcast = 0;
                        *puchAccuracyLevel = (GU8)algo_calc_result.snResult[1];
                        *pusRRvalue = 0;
                    }
                }
            }
            if (uchAlgoHasRefreshFlag == 0x00)
            {
                if (g_uchHbdStarted == HBD_START_STATUS_HB_MODE)
                {
                    *puchHbValue = (GU8)algo_calc_result.snResult[0];
                    *puchWearingQuality = 0;
                    *puchVoiceBroadcast = 0;
                    *puchAccuracyLevel = (GU8)algo_calc_result.snResult[1];
                    *pusRRvalue = 0;
                }
            }

            if(g_chWearIsEnableFlag == 1)
            {
                *puchWearingState = algo_calc_result.snResult[12];
                *puchWearingQuality = algo_calc_result.snResult[13];

            }
            else
            {
                *puchWearingState = 1;
                *puchWearingQuality = 100;
            }


            if (g_uchAutoLedErrorFlag == 1)
            {
                *puchWearingState = 2;
            }
        }
    }
    return uchRet;
}
#endif
/****************************************************************
* Description: calculate hbd_value and wearing detect by fifo int.
* Input:  stGsAxisValue: gsensor data buffer
             usGsDataNum: gsensor data count
             emGsensorSensitivity: G-sensor sensitivity (counts/g), see EM_HBD_GSENSOR_SENSITIVITY
* Output:   *pstHbRes: ptr of hb calculate result ,seeST_HB_RES
            nRawdataOut: ptr of rawdata array output (ppg1, ppg2, gs-x, gs-y, gs-z, index)
            pusRawdataOutLen:ptr of rawdata len output
* Return: refresh flag, if heartbeat value have refresh that return 1.
******************************************************************/
#if 0
GS8 HBD_HbCalculateByFifoIntEx(ST_GS_DATA_TYPE stGsAxisValue[], GU16 usGsDataNum, EM_HBD_GSENSOR_SENSITIVITY emGsensorSensitivity,
                                            GS32 nRawdataOut[][6], GU16 *pusRawdataOutLen, ST_HB_RES *pstHbRes)
{
    GU8 uchRet = 0;
    //GU8 uchAlgoRet = 0;
    GU8 uchAlgoHasRefreshFlag = 0;
    GU16 usDbgRawdataMaxLen = (*pusRawdataOutLen);
    *pusRawdataOutLen = 0;
    if ((g_uchHbdStarted == HBD_START_STATUS_HB_MODE) || (g_uchHbdStarted == HBD_START_STATUS_IN_EAR_EP_WEAR_DETECT_MODE))
    {
        GU16 usRawdataFifoCnt = 0;
        GF32 fIndexInc = 0.0f;
        GU16 usIndex = 0;
        GS32 nRawDataArr[10] = {0};
        //GS32 nHeartBeatArr[7] = {0};
        GS16 sGsensorRightShift = (1 << emGsensorSensitivity);
        //GU8 uchNadtRes[2] = {0};
        pstHbRes->uchWearingState = 1;
        pstHbRes->uchWearingQuality = 0;
        if (usDbgRawdataMaxLen == 0)
        {
            usDbgRawdataMaxLen = 0xFFFF;
        }
        //HBD_AutoLedCalcAccSTD(stGsAxisValue, usGsDataNum, &g_stAutoLedAccInfo, sGsensorRightShift);
        usRawdataFifoCnt = HBD_GetFifoRawdata(g_uchReadFifoBuffArr);
        fIndexInc = HBD_CalcGsensorFloatIndex(usRawdataFifoCnt, usGsDataNum);
        if (usRawdataFifoCnt > 0)
        {
            ST_ALGO_CALCULATE_INFO algo_calc_info = {0};
            ST_ALGO_RESULT algo_calc_result = {0};
            for (usIndex = 0; usIndex < usRawdataFifoCnt; usIndex++)
            {
                HBD_PackFifoRawdataWithGsensor(g_uchReadFifoBuffArr, usRawdataFifoCnt, usIndex,
                                           stGsAxisValue, usGsDataNum, fIndexInc, sGsensorRightShift,
                                           nRawDataArr, nRawdataOut, pusRawdataOutLen, usDbgRawdataMaxLen);

                #ifndef HBD_TEST_NO_ALGO
                if(0 == g_uchCloseHBAlgo)
                {
                    nRawDataArr[7] = 0;//g_nReferenceHbValue
                    //uchAlgoRet = HBACalculate(nRawDataArr, nHeartBeatArr, &g_stHbaInfo); // calc hb
                    algo_calc_info.uchFrameID = nRawDataArr[5];
                    algo_calc_info.nRawdata[0] = nRawDataArr[0] & 0xffffff;
                    memcpy(algo_calc_info.sAccData, &nRawDataArr[2], 3 * sizeof(GS32));
                    algo_calc_info.unSleepFlag = 0;//g_sleep_flg
                    HOOK_FUNC_CALL(g_pAlgoCalculateHookFunc, (GH3011_FUNC_OFFSET_HR, &algo_calc_info, &algo_calc_result));
                    uchRet = algo_calc_result.uchUpdateFlag;
                }
                #endif

                if(g_chWearIsEnableFlag == 1)
                {
                    algo_calc_info.led_current[1] = g_uchAutoLedCurrentArr[1];
                    algo_calc_info.nRawdata[0] = nRawDataArr[0] & 0xffffffff;
                    algo_calc_info.nRawdata[1] = nRawDataArr[1] & 0xffffffff;
                    HOOK_FUNC_CALL(g_pAlgoCalculateHookFunc, (GH3011_FUNC_OFFSET_NADT, &algo_calc_info, &algo_calc_result));
                }

                if ((algo_calc_result.uchUpdateFlag & 0x01) != 0) // hb refresh
                {
                    if (g_uchHbdStarted == HBD_START_STATUS_HB_MODE)
                    {
                        uchRet = 1;
                        uchAlgoHasRefreshFlag |= 0x01;
                        pstHbRes->uchHbValue = (GU8)algo_calc_result.snResult[0];
                        pstHbRes->uchScene = (GU8)algo_calc_result.snResult[2];
                        pstHbRes->uchSNR = (GU8)algo_calc_result.snResult[3];
                        pstHbRes->uchAccuracyLevel = (GU8)algo_calc_result.snResult[4];
                        pstHbRes->uchMotionState = (GU8)algo_calc_result.snResult[5];
                        pstHbRes->uchSleepFlag = 0;//g_sleep_flg
                    }
                }

            }
            if (uchAlgoHasRefreshFlag == 0x00)
            {
                if (g_uchHbdStarted == HBD_START_STATUS_HB_MODE)
                {
                    pstHbRes->uchHbValue = (GU8)algo_calc_result.snResult[0];
                    pstHbRes->uchScene = (GU8)algo_calc_result.snResult[2];
                    pstHbRes->uchSNR = (GU8)algo_calc_result.snResult[3];
                    pstHbRes->uchAccuracyLevel = (GU8)algo_calc_result.snResult[4];
                    pstHbRes->uchMotionState = (GU8)algo_calc_result.snResult[5];
                    pstHbRes->uchSleepFlag = 0;//g_sleep_flg
                }
            }

            if(g_chWearIsEnableFlag == 1)
            {
                pstHbRes->uchWearingState = algo_calc_result.snResult[12];
                pstHbRes->uchWearingQuality = algo_calc_result.snResult[13];
            }
            else
            {
                pstHbRes->uchWearingState = 1;
                pstHbRes->uchWearingQuality = 100;
            }

            if (g_uchAutoLedErrorFlag == 1)
            {
                pstHbRes->uchWearingState = 2;
            }
        }
    }
    return uchRet;
}
#endif

/****************************************************************
* Description: calculate hbd_value and wearing detect by newdata int. must started with HBD_HbWithHrvDetectStart
* Input:  stGsAxisValue: gsensor data
          emGsensorSensitivity: G-sensor sensitivity (counts/g), see EM_HBD_GSENSOR_SENSITIVITY
* Output:   puchHbValue:ptr of heartbeat value output
            puchWearingState:ptr of wearing state output
            puchWearingQuality: ptr of wearing quality output
            puchVoiceBroadcast: ptr of voice broadcast output
            pusRRvalue: ptr of RR value output
            usRRvalueArr: ptr of RR value output
            puchRRvalueCnt: ptr of RR value count output
            puchHrvConfidentLvl: ptr of RR value confident
* Return: refresh flag, 1:heartbeat refresh, 2:wearingstate refresh,3: heartbeat&wearingstate refresh.
******************************************************************/
GS8 HBD_HbWithHrvCalculateByNewdataInt(ST_GS_DATA_TYPE *stGsAxisValue, EM_HBD_GSENSOR_SENSITIVITY emGsensorSensitivity,
                    GU8 *puchHbValue, GU8 *puchWearingState, GU8 *puchWearingQuality, GU8 *puchVoiceBroadcast,
                    GU16 usRRvalueArr[4], GU8 *puchRRvalueCnt, GU8 *puchHrvConfidentLvl)
{
    GU8 uchRet = 0;
    GU8 uchHrvAlgoRet = 0;
    if ((g_uchHbdStarted == HBD_START_STATUS_HB_MODE) || (g_uchHbdStarted == HBD_START_STATUS_IN_EAR_EP_WEAR_DETECT_MODE))
    {
        GS32 nRawDataArr[10] = {0};
        //GS32 nHeartBeatArr[7] = {0};

        //GU8 uchNadtRes[2] = {0};
        *puchWearingState = 1;
        *puchWearingQuality = 0;
        GS16 sGsensorRightShift = (1 << emGsensorSensitivity);

        if (stGsAxisValue != HBD_PTR_NULL) // fix
        {
            nRawDataArr[2] = (GS32)(stGsAxisValue->sXAxisVal / sGsensorRightShift);
            nRawDataArr[3] = (GS32)(stGsAxisValue->sYAxisVal / sGsensorRightShift);
            nRawDataArr[4] = (GS32)(stGsAxisValue->sZAxisVal / sGsensorRightShift);
        }
        HBD_GetNewdataIntData(nRawDataArr);

        #ifndef HBD_TEST_NO_ALGO

        ST_ALGO_CALCULATE_INFO algo_calc_info = {0};
        ST_ALGO_RESULT algo_calc_result = {0};

        algo_calc_info.uchFrameID = nRawDataArr[5];
        algo_calc_info.nRawdata[0] = nRawDataArr[0];
        memcpy(algo_calc_info.sAccData, &nRawDataArr[2], 3 * sizeof(GS32));
       
        //HOOK_FUNC_CALL(g_pAlgoCalculateHookFunc, (GH3011_FUNC_OFFSET_HRV, &algo_calc_info, &algo_calc_result));
        if(g_pAlgoCalculateHookFunc)
            g_pAlgoCalculateHookFunc(GH3011_FUNC_OFFSET_HRV, &algo_calc_info, &algo_calc_result);
        uchHrvAlgoRet = algo_calc_result.uchUpdateFlag;

        if (uchHrvAlgoRet != 0)
        {
            usRRvalueArr[0]         = (GU16)algo_calc_result.snResult[0];
            usRRvalueArr[1]         = (GU16)algo_calc_result.snResult[1];
            usRRvalueArr[2]         = (GU16)algo_calc_result.snResult[2];
            usRRvalueArr[3]         = (GU16)algo_calc_result.snResult[3];
            *puchHrvConfidentLvl    = (GU8)algo_calc_result.snResult[4];
            *puchRRvalueCnt         = (GU8)algo_calc_result.snResult[5];
        }

        if (usHbWithHrvSampleIndex == 0)
        {
            nRawDataArr[5] = usHbWithHrvSampleReIndex;
            nRawDataArr[7] = 0;//g_nReferenceHbValue
            algo_calc_info.uchFrameID = nRawDataArr[5];
            algo_calc_info.nRawdata[0] = nRawDataArr[0] & 0xffffff;
            memcpy(algo_calc_info.sAccData, &nRawDataArr[2], 3 * sizeof(GS32));
            algo_calc_info.unSleepFlag = 0;//g_sleep_flg
            
            //HOOK_FUNC_CALL(g_pAlgoCalculateHookFunc, (GH3011_FUNC_OFFSET_HR, &algo_calc_info, &algo_calc_result));
            if(g_pAlgoCalculateHookFunc)
                g_pAlgoCalculateHookFunc(GH3011_FUNC_OFFSET_HR, &algo_calc_info, &algo_calc_result);
            uchRet = algo_calc_result.uchUpdateFlag;
            algo_calc_info.led_current[1] = g_uchAutoLedCurrentArr[1];
            algo_calc_info.nRawdata[0] = nRawDataArr[0] & 0xffffffff;
            algo_calc_info.nRawdata[1] = nRawDataArr[1] & 0xffffffff;
            
            //HOOK_FUNC_CALL(g_pAlgoCalculateHookFunc, (GH3011_FUNC_OFFSET_NADT, &algo_calc_info, &algo_calc_result));
            if(g_pAlgoCalculateHookFunc)
                g_pAlgoCalculateHookFunc(GH3011_FUNC_OFFSET_NADT, &algo_calc_info, &algo_calc_result);
            usHbWithHrvSampleReIndex++;
            usHbWithHrvSampleReIndex &= 0x00FF;
        }
        usHbWithHrvSampleIndex++;
        usHbWithHrvSampleIndex %= usHbWithHrvSampleCalcDiv;
        #endif
        if (g_uchHbdStarted == HBD_START_STATUS_HB_MODE)
        {
            *puchHbValue = (GU8)algo_calc_result.snResult[0];
            *puchWearingQuality = 0;
            *puchVoiceBroadcast = 0;
        }

        *puchWearingState =  algo_calc_result.snResult[12];
        *puchWearingQuality = algo_calc_result.snResult[13];
        if (g_uchAutoLedErrorFlag == 1)
        {
            *puchWearingState = 2;
        }
    }
    return uchRet;
}

/****************************************************************
* Description: calculate hbd_value and wearing detect by newdata int. must started with HBD_HbWithHrvDetectStart
* Input:  stGsAxisValue: gsensor data
          emGsensorSensitivity: G-sensor sensitivity (counts/g), see EM_HBD_GSENSOR_SENSITIVITY
* Output:   pstHbRes:ptr of hb calculate result output,see ST_HB_RES
            pstHrvRes:ptr of hrv calculate result output,see ST_HRV_RES
* Return: refresh flag, 1:heartbeat refresh, 2:wearingstate refresh,3: heartbeat&wearingstate refresh.
******************************************************************/
GS8 HBD_HbWithHrvCalculateByNewdataIntEx(ST_GS_DATA_TYPE *stGsAxisValue, EM_HBD_GSENSOR_SENSITIVITY emGsensorSensitivity,
                                          ST_HB_RES *pstHbRes, ST_HRV_RES *pstHrvRes)
{
    GU8 uchRet = 0;
    GU8 uchHrvAlgoRet = 0;
    if ((g_uchHbdStarted == HBD_START_STATUS_HB_MODE) || (g_uchHbdStarted == HBD_START_STATUS_IN_EAR_EP_WEAR_DETECT_MODE))
    {
        GS32 nRawDataArr[10] = {0};
        //GS32 nHeartBeatArr[7] = {0};

        //GU8 uchNadtRes[2] = {0};
        pstHbRes->uchWearingState = 1;
        pstHbRes->uchWearingQuality = 0;
        GS16 sGsensorRightShift = (1 << emGsensorSensitivity);

        if (stGsAxisValue != HBD_PTR_NULL) // fix
        {
            nRawDataArr[2] = (GS32)(stGsAxisValue->sXAxisVal / sGsensorRightShift);
            nRawDataArr[3] = (GS32)(stGsAxisValue->sYAxisVal / sGsensorRightShift);
            nRawDataArr[4] = (GS32)(stGsAxisValue->sZAxisVal / sGsensorRightShift);
        }
        HBD_GetNewdataIntData(nRawDataArr);
        #ifndef HBD_TEST_NO_ALGO

        ST_ALGO_CALCULATE_INFO algo_calc_info = {0};
        ST_ALGO_RESULT algo_calc_result = {0};

        algo_calc_info.uchFrameID = nRawDataArr[5];
        algo_calc_info.nRawdata[0] = nRawDataArr[0];
        memcpy(algo_calc_info.sAccData, &nRawDataArr[2], 3 * sizeof(GS32));
        //HOOK_FUNC_CALL(g_pAlgoCalculateHookFunc, (GH3011_FUNC_OFFSET_HRV, &algo_calc_info, &algo_calc_result));
        if(g_pAlgoCalculateHookFunc)
            g_pAlgoCalculateHookFunc(GH3011_FUNC_OFFSET_HRV, &algo_calc_info, &algo_calc_result);
        uchHrvAlgoRet = algo_calc_result.uchUpdateFlag;

        if (uchHrvAlgoRet != 0)
        {
            pstHrvRes->usRRvalueArr[0]     = (GU16)algo_calc_result.snResult[0];
            pstHrvRes->usRRvalueArr[1]     = (GU16)algo_calc_result.snResult[1];
            pstHrvRes->usRRvalueArr[2]     = (GU16)algo_calc_result.snResult[2];
            pstHrvRes->usRRvalueArr[3]     = (GU16)algo_calc_result.snResult[3];
            pstHrvRes->uchHrvConfidentLvl  = (GU8)algo_calc_result.snResult[4];
            pstHrvRes->uchRRvalueCnt       = (GU8)algo_calc_result.snResult[5];
        }

        if (usHbWithHrvSampleIndex == 0)
        {
            nRawDataArr[5] = usHbWithHrvSampleReIndex;
            nRawDataArr[7] = 0;//g_nReferenceHbValue
            //uchRet = HBACalculate(nRawDataArr, nHeartBeatArr, &g_stHbaInfo); // calc hb
            algo_calc_info.uchFrameID = nRawDataArr[5];
            algo_calc_info.nRawdata[0] = nRawDataArr[0] & 0xffffff;
            memcpy(algo_calc_info.sAccData, &nRawDataArr[2], 3 * sizeof(GS32));
            algo_calc_info.unSleepFlag = 0;//g_sleep_flg
            //HOOK_FUNC_CALL(g_pAlgoCalculateHookFunc, (GH3011_FUNC_OFFSET_HR, &algo_calc_info, &algo_calc_result));
            if(g_pAlgoCalculateHookFunc)
                g_pAlgoCalculateHookFunc(GH3011_FUNC_OFFSET_HR, &algo_calc_info, &algo_calc_result);
            uchRet = algo_calc_result.uchUpdateFlag;
            algo_calc_info.led_current[1] = g_uchAutoLedCurrentArr[1];
            algo_calc_info.nRawdata[0] = nRawDataArr[0] & 0xffffffff;
            algo_calc_info.nRawdata[1] = nRawDataArr[1] & 0xffffffff;
            //HOOK_FUNC_CALL(g_pAlgoCalculateHookFunc, (GH3011_FUNC_OFFSET_NADT, &algo_calc_info, &algo_calc_result));
            if(g_pAlgoCalculateHookFunc)
               g_pAlgoCalculateHookFunc(GH3011_FUNC_OFFSET_NADT, &algo_calc_info, &algo_calc_result);

            usHbWithHrvSampleReIndex++;
            usHbWithHrvSampleReIndex &= 0x00FF;
        }
        usHbWithHrvSampleIndex++;
        usHbWithHrvSampleIndex %= usHbWithHrvSampleCalcDiv;
        #endif
        if (g_uchHbdStarted == HBD_START_STATUS_HB_MODE)
        {
            pstHbRes->uchHbValue = (GU8)algo_calc_result.snResult[0];
            pstHbRes->uchScene = (GU8)algo_calc_result.snResult[2];
            pstHbRes->uchSNR = (GU8)algo_calc_result.snResult[3];
            pstHbRes->uchAccuracyLevel = (GU8)algo_calc_result.snResult[4];
            pstHbRes->uchMotionState = (GU8)algo_calc_result.snResult[5];
            pstHbRes->uchSleepFlag = 0;//g_sleep_flg
        }

        pstHbRes->uchWearingState = algo_calc_result.snResult[12];
        pstHbRes->uchWearingQuality = algo_calc_result.snResult[13];
        if (g_uchAutoLedErrorFlag == 1)
        {
            pstHbRes->uchWearingState = 2;
        }
    }
    return uchRet;
}

/****************************************************************
* Description: calculate hbd_value,wearing detect and hrv by fifo int. must started with HBD_HbWithHrvDetectStart
* Input:  stGsAxisValue: gsensor data buffer
             usGsDataNum: gsensor data count
             emGsensorSensitivity: G-sensor sensitivity (counts/g), see EM_HBD_GSENSOR_SENSITIVITY
* Output:   puchHbValue:ptr of heartbeat value output
            puchAccuracyLevel: ptr of heartbeat accuracy level(1-3), max 3
            puchWearingState:ptr of wearing state output
            puchWearingQuality: ptr of wearing quality output
            puchVoiceBroadcast: ptr of voice broadcast output
            usRRvalueArr: ptr of RR value output
            puchRRvalueCnt: ptr of RR value count output
            puchHrvConfidentLvl: ptr of RR value confident
            nRawdataOut: ptr of rawdata array output (ppg1, ppg2, gs-x, gs-y, gs-z, index)
            pusRawdataOutLen:ptr of rawdata len output
* Return: refresh flag, if heartbeat value have refresh that return 1.
******************************************************************/
#if 0
GS8 HBD_HbWithHrvCalculateByFifoIntDbgData(ST_GS_DATA_TYPE stGsAxisValue[], GU16 usGsDataNum, EM_HBD_GSENSOR_SENSITIVITY emGsensorSensitivity, 
                            GU8 *puchHbValue, GU8 *puchAccuracyLevel, GU8 *puchWearingState, GU8 *puchWearingQuality, GU8 *puchVoiceBroadcast, 
                            GU16 usRRvalueArr[4], GU8 *puchRRvalueCnt, GU8 *puchHrvConfidentLvl,
                            GS32 nRawdataOut[][6], GU16 *pusRawdataOutLen)
{
    GU8 uchRet = 0;
    //GU8 uchAlgoRet = 0;
    GU8 uchAlgoHasRefreshFlag = 0;
    GU8 uchHrvAlgoRet = 0;
    GU16 usDbgRawdataMaxLen = (*pusRawdataOutLen);
    *pusRawdataOutLen = 0;
    if ((g_uchHbdStarted == HBD_START_STATUS_HB_MODE) || (g_uchHbdStarted == HBD_START_STATUS_IN_EAR_EP_WEAR_DETECT_MODE))
    {
        GU16 usRawdataFifoCnt = 0;
        GF32 fIndexInc = 0.0f;
        GU16 usIndex = 0;
        GS32 nRawDataArr[10] = {0};
        //GS32 nHeartBeatArr[7] = {0};

        //GU8 uchNadtRes[2] = {0};
        *puchWearingState = 1;
        *puchWearingQuality = 0;
        GS16 sGsensorRightShift = (1 << emGsensorSensitivity);
        if (usDbgRawdataMaxLen == 0)
        {
            usDbgRawdataMaxLen = 0xFFFF;
        }
        //HBD_AutoLedCalcAccSTD(stGsAxisValue, usGsDataNum, &g_stAutoLedAccInfo, sGsensorRightShift);
        usRawdataFifoCnt = HBD_GetFifoRawdata(g_uchReadFifoBuffArr);
        fIndexInc = HBD_CalcGsensorFloatIndex(usRawdataFifoCnt, usGsDataNum);
        if (usRawdataFifoCnt > 0)
        {
            ST_ALGO_CALCULATE_INFO algo_calc_info = {0};
            ST_ALGO_RESULT algo_calc_result = {0};
            for (usIndex = 0; usIndex < usRawdataFifoCnt; usIndex++)
            {
                HBD_PackFifoRawdataWithGsensor(g_uchReadFifoBuffArr, usRawdataFifoCnt, usIndex,
                                           stGsAxisValue, usGsDataNum, fIndexInc, sGsensorRightShift,
                                           nRawDataArr, nRawdataOut, pusRawdataOutLen, usDbgRawdataMaxLen);

                #ifndef HBD_TEST_NO_ALGO

                algo_calc_info.uchFrameID = nRawDataArr[5];
                algo_calc_info.nRawdata[0] = nRawDataArr[0];
                memcpy(algo_calc_info.sAccData, &nRawDataArr[2], 3 * sizeof(GS32));
                HOOK_FUNC_CALL(g_pAlgoCalculateHookFunc, (GH3011_FUNC_OFFSET_HRV, &algo_calc_info, &algo_calc_result));
                uchHrvAlgoRet = algo_calc_result.uchUpdateFlag;

                if (uchHrvAlgoRet != 0)
                {
                    usRRvalueArr[0]      = (GU16)algo_calc_result.snResult[0];
                    usRRvalueArr[1]      = (GU16)algo_calc_result.snResult[1];
                    usRRvalueArr[2]      = (GU16)algo_calc_result.snResult[2];
                    usRRvalueArr[3]      = (GU16)algo_calc_result.snResult[3];
                    *puchHrvConfidentLvl = (GU8)algo_calc_result.snResult[4];
                    *puchRRvalueCnt      = (GU8)algo_calc_result.snResult[5];
                }
                //uchAlgoRet = 0;
                if (usHbWithHrvSampleIndex == 0)
                {
                    nRawDataArr[5] = usHbWithHrvSampleReIndex;
                    nRawDataArr[7] = 0;//g_nReferenceHbValue
                    //uchAlgoRet = HBACalculate(nRawDataArr, nHeartBeatArr, &g_stHbaInfo); // calc hb
                    algo_calc_info.uchFrameID = nRawDataArr[5];
                    algo_calc_info.nRawdata[0] = nRawDataArr[0] & 0xffffff;
                    memcpy(algo_calc_info.sAccData, &nRawDataArr[2], 3 * sizeof(GS32));
                    algo_calc_info.unSleepFlag = 0;//g_sleep_flg
                    HOOK_FUNC_CALL(g_pAlgoCalculateHookFunc, (GH3011_FUNC_OFFSET_HR, &algo_calc_info, &algo_calc_result));
                    uchRet = algo_calc_result.uchUpdateFlag;

                    algo_calc_info.led_current[1] = g_uchAutoLedCurrentArr[1];
                    algo_calc_info.nRawdata[0] = nRawDataArr[0] & 0xffffffff;
                    algo_calc_info.nRawdata[1] = nRawDataArr[1] & 0xffffffff;
                    HOOK_FUNC_CALL(g_pAlgoCalculateHookFunc, (GH3011_FUNC_OFFSET_NADT, &algo_calc_info, &algo_calc_result));
                    usHbWithHrvSampleReIndex++;
                    usHbWithHrvSampleReIndex &= 0x00FF;
                }
                usHbWithHrvSampleIndex++;
                usHbWithHrvSampleIndex %= usHbWithHrvSampleCalcDiv;
                #endif
                if ((algo_calc_result.uchUpdateFlag & 0x01) != 0) // hb refresh
                {
                    if (g_uchHbdStarted == HBD_START_STATUS_HB_MODE)
                    {
                        uchAlgoHasRefreshFlag |= 0x01;
                        *puchHbValue = (GU8)algo_calc_result.snResult[0];
                        *puchWearingQuality = 0;
                        *puchVoiceBroadcast = 0;
                        *puchAccuracyLevel = (GU8)algo_calc_result.snResult[1];
                    }
                }
            }
            if (uchAlgoHasRefreshFlag == 0x00)
            {
                if (g_uchHbdStarted == HBD_START_STATUS_HB_MODE)
                {
                    *puchHbValue = (GU8)algo_calc_result.snResult[0];
                    *puchWearingQuality = 0;
                    *puchVoiceBroadcast = 0;
                    *puchAccuracyLevel = (GU8)algo_calc_result.snResult[1];
                }
            }
            *puchWearingState = algo_calc_result.snResult[12];
            *puchWearingQuality = algo_calc_result.snResult[13];
            if (g_uchAutoLedErrorFlag == 1)
            {
                *puchWearingState = 2;
            }
        }
    }
    return uchRet;
}
#endif

/****************************************************************
* Description: calculate hbd_value,wearing detect and hrv by fifo int. must started with HBD_HbWithHrvDetectStart
* Input:  stGsAxisValue: gsensor data buffer
             usGsDataNum: gsensor data count
             emGsensorSensitivity: G-sensor sensitivity (counts/g), see EM_HBD_GSENSOR_SENSITIVITY
* Output:   *pstHbRes:ptr of hb calculate result,see ST_HB_RES
            *pstHrvRes:ptr of hrv calculate result,see ST_HRV_RES
            nRawdataOut: ptr of rawdata array output (ppg1, ppg2, gs-x, gs-y, gs-z, index)
            pusRawdataOutLen:ptr of rawdata len output
* Return: refresh flag, if heartbeat value have refresh that return 1.
******************************************************************/
#if 0
GS8 HBD_HbWithHrvCalculateByFifoIntDbgDataEx(ST_GS_DATA_TYPE stGsAxisValue[], GU16 usGsDataNum, EM_HBD_GSENSOR_SENSITIVITY emGsensorSensitivity, 
                            ST_HB_RES *pstHbRes, ST_HRV_RES *pstHrvRes, GS32 nRawdataOut[][6], GU16 *pusRawdataOutLen)
{
    GU8 uchRet = 0;
    //GU8 uchAlgoRet = 0;
    GU8 uchAlgoHasRefreshFlag = 0;
    GU8 uchHrvAlgoRet = 0;
    GU16 usDbgRawdataMaxLen = (*pusRawdataOutLen);
    *pusRawdataOutLen = 0;
    if ((g_uchHbdStarted == HBD_START_STATUS_HB_MODE) || (g_uchHbdStarted == HBD_START_STATUS_IN_EAR_EP_WEAR_DETECT_MODE))
    {
        GU16 usRawdataFifoCnt = 0;
        GF32 fIndexInc = 0.0f;
        GU16 usIndex = 0;
        GS32 nRawDataArr[10] = {0};

        //GU8 uchNadtRes[2] = {0};
        pstHbRes->uchWearingState = 1;
        pstHbRes->uchWearingQuality = 0;
        GS16 sGsensorRightShift = (1 << emGsensorSensitivity);
        if (usDbgRawdataMaxLen == 0)
        {
            usDbgRawdataMaxLen = 0xFFFF;
        }
        //HBD_AutoLedCalcAccSTD(stGsAxisValue, usGsDataNum, &g_stAutoLedAccInfo, sGsensorRightShift);
        usRawdataFifoCnt = HBD_GetFifoRawdata(g_uchReadFifoBuffArr);
        fIndexInc = HBD_CalcGsensorFloatIndex(usRawdataFifoCnt, usGsDataNum);
        if (usRawdataFifoCnt > 0)
        {
            ST_ALGO_CALCULATE_INFO algo_calc_info = {0};
            ST_ALGO_RESULT algo_calc_result = {0};
            for (usIndex = 0; usIndex < usRawdataFifoCnt; usIndex++)
            {
                HBD_PackFifoRawdataWithGsensor(g_uchReadFifoBuffArr, usRawdataFifoCnt, usIndex,
                                           stGsAxisValue, usGsDataNum, fIndexInc, sGsensorRightShift,
                                           nRawDataArr, nRawdataOut, pusRawdataOutLen, usDbgRawdataMaxLen);

                #ifndef HBD_TEST_NO_ALGO
                algo_calc_info.uchFrameID = nRawDataArr[5];
                algo_calc_info.nRawdata[0] = nRawDataArr[0];
                algo_calc_info.nRawdata[1] = nRawDataArr[1] & 0xffffff;
                memcpy(algo_calc_info.sAccData, &nRawDataArr[2], 3 * sizeof(GS32));
                HOOK_FUNC_CALL(g_pAlgoCalculateHookFunc, (GH3011_FUNC_OFFSET_HRV, &algo_calc_info, &algo_calc_result));
                uchHrvAlgoRet = algo_calc_result.uchUpdateFlag;

                if (uchHrvAlgoRet != 0)
                {
                    pstHrvRes->usRRvalueArr[0]    = (GU16)algo_calc_result.snResult[0];
                    pstHrvRes->usRRvalueArr[1]    = (GU16)algo_calc_result.snResult[1];
                    pstHrvRes->usRRvalueArr[2]    = (GU16)algo_calc_result.snResult[2];
                    pstHrvRes->usRRvalueArr[3]    = (GU16)algo_calc_result.snResult[3];
                    pstHrvRes->uchHrvConfidentLvl = (GU8)algo_calc_result.snResult[4];
                    pstHrvRes->uchRRvalueCnt      = (GU8)algo_calc_result.snResult[5];
                }
                if (usHbWithHrvSampleIndex == 0)
                {
                    nRawDataArr[5] = usHbWithHrvSampleReIndex;
                    nRawDataArr[7] = 0;//g_nReferenceHbValue
                    //uchAlgoRet = HBACalculate(nRawDataArr, nHeartBeatArr, &g_stHbaInfo); // calc hb
                    algo_calc_info.uchFrameID = nRawDataArr[5];
                    algo_calc_info.nRawdata[0] = nRawDataArr[0] & 0xffffff;
                    memcpy(algo_calc_info.sAccData, &nRawDataArr[2], 3 * sizeof(GS32));
                    algo_calc_info.unSleepFlag = 0;//g_sleep_flg
                    HOOK_FUNC_CALL(g_pAlgoCalculateHookFunc, (GH3011_FUNC_OFFSET_HR, &algo_calc_info, &algo_calc_result));
                    uchRet = algo_calc_result.uchUpdateFlag;

                    if(g_chWearIsEnableFlag == 1)
                    {
                        algo_calc_info.nRawdata[0] = nRawDataArr[0] & 0xffffffff;
                        algo_calc_info.nRawdata[1] = nRawDataArr[1] & 0xffffffff;
                        algo_calc_info.led_current[1] = g_uchAutoLedCurrentArr[1];
                        HOOK_FUNC_CALL(g_pAlgoCalculateHookFunc, (GH3011_FUNC_OFFSET_NADT, &algo_calc_info, &algo_calc_result));
                    }

                    if ((algo_calc_result.uchUpdateFlag & 0x01) != 0) // hb refresh
                    {
                        if (g_uchHbdStarted == HBD_START_STATUS_HB_MODE)
                        {
                            uchAlgoHasRefreshFlag |= 0x01;
                            pstHbRes->uchHbValue = (GU8)algo_calc_result.snResult[0];
                            pstHbRes->uchScene = (GU8)algo_calc_result.snResult[2];
                            pstHbRes->uchSNR = (GU8)algo_calc_result.snResult[3];
                            pstHbRes->uchAccuracyLevel = (GU8)algo_calc_result.snResult[4];
                            pstHbRes->uchMotionState = (GU8)algo_calc_result.snResult[5];
                            pstHbRes->uchSleepFlag = 0;//g_sleep_flg
                            uchRet = 1;
                        }
                    }
                    usHbWithHrvSampleReIndex++;
                    usHbWithHrvSampleReIndex &= 0x00FF;
                }
                usHbWithHrvSampleIndex++;
                usHbWithHrvSampleIndex %= usHbWithHrvSampleCalcDiv;
                #endif
            }

            if(g_chWearIsEnableFlag == 1)
            {
                pstHbRes->uchWearingState = algo_calc_result.snResult[12];
                pstHbRes->uchWearingQuality = algo_calc_result.snResult[13];
            }
            else
            {
                pstHbRes->uchWearingState = 1;
                pstHbRes->uchWearingQuality = 100;
            }

            if (g_uchAutoLedErrorFlag == 1)
            {
                pstHbRes->uchWearingState = 2;
            }
        }
    }

    return uchRet;
}
#endif

/****************************************************************
* Description: Wear state confirm.
* Output:   None,
* Return: wear state, 0: during wear state detect;
                      1: state output wear
                      2: state output unwaer
******************************************************************/
#if 0
GU8 HBD_WearStateConfirm (void)
{
    GU8 uchRet = 0;
    GU16 usIndex = 0;
    GU32 unPpgRawdata = 0;
    GU16 usHbCalcCnt = 0;
    static GU32 unWearStateCheckBitsSet = 0;
    GU32 unWearStateCheckBitsSetTmp = 0;
    GU8 unCheckBitsSetIndex = 0;
    GU8 unWearStateCheckCnt = 0;

    static GU32 unUnwearStateCheckBitsSet = 0;
    GU32 unUnwearStateCheckBitsSetTmp = 0;
    GU8 unUnwearStateCheckCnt = 0;

    if (g_uchHbdStarted == HBD_START_STATUS_BAND_WEAR_CONFIRM_MODE)
    {
        usHbCalcCnt = HBD_GetFifoRawdata(g_uchReadFifoBuffArr);
        if (usHbCalcCnt > 0)
        {
            if (g_usWearStateCheckIndex == 0)
            {
                unWearStateCheckBitsSet = 0;
                unUnwearStateCheckBitsSet = 0;
            }

            for (usIndex = 0; usIndex < usHbCalcCnt; usIndex++ )
            {
                if (g_uchNeedReadRawdataCount == 2)
                {
                    unPpgRawdata = HBD_MAKEUP_DWORD(0x00, g_uchReadFifoBuffArr[usIndex * 6 + 3],
                                                g_uchReadFifoBuffArr[usIndex * 6 + 4], g_uchReadFifoBuffArr[usIndex * 6 + 5]);
                }
                else
                {
                    unPpgRawdata = HBD_MAKEUP_DWORD(0x00, g_uchReadFifoBuffArr[usIndex * 3],
                                                g_uchReadFifoBuffArr[usIndex * 3 + 1], g_uchReadFifoBuffArr[usIndex * 3 + 2]);
                }
                unPpgRawdata = unPpgRawdata & 0x0001FFFF;

                // cehck wear
                unWearStateCheckBitsSet <<= 1;
                if ((unPpgRawdata >= g_unWearStateLowerThreshold) && (unPpgRawdata <= g_unWearStateUpperThreshold))
                {
                    unWearStateCheckBitsSet |= 0x01;
                }
                unWearStateCheckBitsSetTmp = unWearStateCheckBitsSet & HBD_WEAR_STATE_CHECK_THR_MSK;
                // check unwear
                unUnwearStateCheckBitsSet <<= 1;
                if ((unPpgRawdata >= g_unUnwearStateLowerThreshold) && (unPpgRawdata <= g_unUnwearStateUpperThreshold))
                {
                    unUnwearStateCheckBitsSet |= 0x01;
                }
                unUnwearStateCheckBitsSetTmp = unUnwearStateCheckBitsSet & HBD_WEAR_STATE_CHECK_THR_MSK;

                unWearStateCheckCnt = 0;
                unUnwearStateCheckCnt = 0;
                for (unCheckBitsSetIndex = 0; unCheckBitsSetIndex < HBD_WEAR_STATE_CHECK_SUM; unCheckBitsSetIndex++ )
                {
                    // check wear
                    if ((unWearStateCheckBitsSetTmp & (1 << unCheckBitsSetIndex)) != 0)
                    {
                        unWearStateCheckCnt ++;
                        if (unWearStateCheckCnt >= HBD_WEAR_STATE_CHECK_THR_SUM)
                        {
                            uchRet = 1;
                            break;
                        }
                    }
                    // check unwear
                    if ((unUnwearStateCheckBitsSetTmp & (1 << unCheckBitsSetIndex)) != 0)
                    {
                        unUnwearStateCheckCnt ++;
                        if (unUnwearStateCheckCnt >= HBD_WEAR_STATE_CHECK_THR_SUM)
                        {
                            uchRet = 2;
                            break;
                        }
                    }
                }

                if (uchRet == 0)
                {
                    if (g_usWearStateCheckIndex < HBD_WEAR_STATE_CHECK_MAX_COUNT)
                    {
                        g_usWearStateCheckIndex ++;
                    }
                    else
                    {
                        uchRet = 2;
                        unWearStateCheckBitsSet = 0;
                        unUnwearStateCheckBitsSet = 0;
                        g_usWearStateCheckIndex = 0;
                    }
                }
                else
                {
                    unWearStateCheckBitsSet = 0;
                    unUnwearStateCheckBitsSet = 0;
                    g_usWearStateCheckIndex = 0;
                    break;
                }
            }
        }
    }

    return uchRet;
}
#endif
/****************************************************************
* Description: Wear state Detect.
* Output:   None,
* Return: wear state, 0: during wear state detect;
                      1: state output wear
                      2: state output unwaer
******************************************************************/
#if 0
GU8 HBD_WearStateConfirmEx (void)
{
    GU8 uchRet = 0;
    GU16 usIndex = 0;
    GU32 unPpgRawdata = 0;
    GU16 usHbCalcCnt = 0;
    static GU32 unWearStateCheckBitsSet = 0;
    GU32 unWearStateCheckBitsSetTmp = 0;
    GU8 unCheckBitsSetIndex = 0;
    GU8 unWearStateCheckCnt = 0;

    static GU32 unUnwearStateCheckBitsSet = 0;
    GU32 unUnwearStateCheckBitsSetTmp = 0;
    GU8 unUnwearStateCheckCnt = 0;

    if (g_uchHbdStarted == HBD_START_STATUS_BAND_WEAR_CONFIRM_MODE)
    {
        usHbCalcCnt = HBD_GetFifoRawdata(g_uchReadFifoBuffArr);

        if (usHbCalcCnt > 0)
        {
            if (g_usWearStateCheckIndex == 0)
            {
                unWearStateCheckBitsSet = 0;
                unUnwearStateCheckBitsSet = 0;
            }

            for (usIndex = 0; usIndex < usHbCalcCnt; usIndex++ )
            {
                if (g_uchNeedReadRawdataCount == 2)
                {
                    unPpgRawdata = HBD_MAKEUP_DWORD(0x00, g_uchReadFifoBuffArr[usIndex * 6 + 3],
                                                g_uchReadFifoBuffArr[usIndex * 6 + 4], g_uchReadFifoBuffArr[usIndex * 6 + 5]);
                }
                else
                {
                    unPpgRawdata = HBD_MAKEUP_DWORD(0x00, g_uchReadFifoBuffArr[usIndex * 3],
                                                g_uchReadFifoBuffArr[usIndex * 3 + 1], g_uchReadFifoBuffArr[usIndex * 3 + 2]);
                }
                unPpgRawdata = unPpgRawdata & 0x0001FFFF;

                if (g_uchWearStateDetectMode == 0)
                {
                    // cehck wear
                    unWearStateCheckBitsSet <<= 1;
                    if ((unPpgRawdata >= g_unWearStateLowerThreshold) && (unPpgRawdata <= g_unWearStateUpperThreshold))
                    {
                        unWearStateCheckBitsSet |= 0x01;
                    }
                    unWearStateCheckBitsSetTmp = unWearStateCheckBitsSet & g_unWearStateDetectMsk;
                }
                else
                {
                    // check unwear
                    unUnwearStateCheckBitsSet <<= 1;
                    if ((unPpgRawdata >= g_unUnwearStateLowerThreshold) && (unPpgRawdata <= g_unUnwearStateUpperThreshold))
                    {
                        unUnwearStateCheckBitsSet |= 0x01;
                    }
                    unUnwearStateCheckBitsSetTmp = unUnwearStateCheckBitsSet & g_unWearStateDetectMsk;
                }
            }
            unWearStateCheckCnt = 0;
            unUnwearStateCheckCnt = 0;
            for (unCheckBitsSetIndex = 0; unCheckBitsSetIndex < g_uchWearStateDetectchecksum; unCheckBitsSetIndex++ )
            {
                if (g_uchWearStateDetectMode == 0)
                {
                    // check wear
                    if ((unWearStateCheckBitsSetTmp & (1 << unCheckBitsSetIndex)) != 0)
                    {
                        unWearStateCheckCnt ++;
                        if (unWearStateCheckCnt >= g_uchWearStateDetectcheckThrsum)
                        {
                            uchRet = 1;
                            break;
                        }
                    }
                }
                else
                {
                    // check unwear
                    if ((unUnwearStateCheckBitsSetTmp & (1 << unCheckBitsSetIndex)) != 0)
                    {
                        unUnwearStateCheckCnt ++;
                        if (unUnwearStateCheckCnt >= g_uchWearStateDetectcheckThrsum)
                        {
                            uchRet = 2;
                            break;
                        }
                    }
                }
            }
            if (g_uchWearStateDetectMode == 0)
            {
                if (uchRet == 0)
                {
                    uchRet = 2;
                }
                else if (uchRet == 1)
                {
                    unWearStateCheckBitsSet = 0;
                    unUnwearStateCheckBitsSet = 0;
                    g_usWearStateCheckIndex = 0;
                    g_uchWearStateDetectMode = 1;
                }
            }
            else
            {
                if (uchRet == 0)
                {
                    uchRet = 1;
                }
                else if (uchRet == 2)
                {
                    unWearStateCheckBitsSet = 0;
                    unUnwearStateCheckBitsSet = 0;
                    g_usWearStateCheckIndex = 0;
                    g_uchWearStateDetectMode = 0;
                }
            }
        }
    }

    return uchRet;
}
#endif
/****************************************************************
* Description: Wear state confirm by new int.
* Output:   None,
* Return: wear state, 0: during wear state detect;
                      1: state output wear
                      2: state output unwaer
******************************************************************/
GU8 HBD_WearStateConfirmByNewdataInt (void)
{
    GU8 uchRet = 0;
    GU32 unPpgRawdata = 0;
    static GU32 unWearStateCheckBitsSet = 0;
    GU32 unWearStateCheckBitsSetTmp = 0;
    GU8 unCheckBitsSetIndex = 0;
    GU8 unWearStateCheckCnt = 0;

    static GU32 unUnwearStateCheckBitsSet = 0;
    GU32 unUnwearStateCheckBitsSetTmp = 0;
    GU8 unUnwearStateCheckCnt = 0;

    if (g_uchHbdStarted == HBD_START_STATUS_BAND_WEAR_CONFIRM_MODE)
    {
        GS32 nPpgRawDataArr[3] = {0};

        HBD_GetNewdataIntData(nPpgRawDataArr);

        if (g_usWearStateCheckIndex == 0)
        {
            unWearStateCheckBitsSet = 0;
            unUnwearStateCheckBitsSet = 0;
        }

        if (g_uchNeedReadRawdataCount == 2)
        {
            unPpgRawdata = nPpgRawDataArr[1];
        }
        else
        {
            unPpgRawdata = nPpgRawDataArr[0];
        }
        unPpgRawdata = unPpgRawdata & 0x0001FFFF;

        // cehck wear
        unWearStateCheckBitsSet <<= 1;
        if ((unPpgRawdata >= g_unWearStateLowerThreshold) && (unPpgRawdata <= g_unWearStateUpperThreshold))
        {
            unWearStateCheckBitsSet |= 0x01;
        }
        unWearStateCheckBitsSetTmp = unWearStateCheckBitsSet & HBD_WEAR_STATE_CHECK_THR_MSK;
        // check unwear
        unUnwearStateCheckBitsSet <<= 1;
        if ((unPpgRawdata >= g_unUnwearStateLowerThreshold) && (unPpgRawdata <= g_unUnwearStateUpperThreshold))
        {
            unUnwearStateCheckBitsSet |= 0x01;
        }
        unUnwearStateCheckBitsSetTmp = unUnwearStateCheckBitsSet & HBD_WEAR_STATE_CHECK_THR_MSK;

        unWearStateCheckCnt = 0;
        unUnwearStateCheckCnt = 0;
        for (unCheckBitsSetIndex = 0; unCheckBitsSetIndex < HBD_WEAR_STATE_CHECK_SUM; unCheckBitsSetIndex++ )
        {
            // check wear
            if ((unWearStateCheckBitsSetTmp & (1 << unCheckBitsSetIndex)) != 0)
            {
                unWearStateCheckCnt ++;
                if (unWearStateCheckCnt >= HBD_WEAR_STATE_CHECK_THR_SUM)
                {
                    uchRet = 1;
                    break;
                }
            }
            // check unwear
            if ((unUnwearStateCheckBitsSetTmp & (1 << unCheckBitsSetIndex)) != 0)
            {
                unUnwearStateCheckCnt ++;
                if (unUnwearStateCheckCnt >= HBD_WEAR_STATE_CHECK_THR_SUM)
                {
                    uchRet = 2;
                    break;
                }
            }
        }

        if (uchRet == 0)
        {
            if (g_usWearStateCheckIndex < HBD_WEAR_STATE_CHECK_MAX_COUNT)
            {
                g_usWearStateCheckIndex ++;
            }
            else
            {
                uchRet = 2;
                unWearStateCheckBitsSet = 0;
                unUnwearStateCheckBitsSet = 0;
                g_usWearStateCheckIndex = 0;
            }
        }
        else
        {
            unWearStateCheckBitsSet = 0;
            unUnwearStateCheckBitsSet = 0;
            g_usWearStateCheckIndex = 0;
        }
    }

    return uchRet;
}

/****************************************************************
* Description: Wear state confirm by new int.
* Output:   None,
* Return: wear state, 0: during wear state detect;
                      1: state output wear
                      2: state output unwaer
******************************************************************/
#if 0
GU8 HBD_WearStateConfirmByFifoInt (void)
{
    return HBD_WearStateConfirm();
}
#endif

/****************************************************************
* Description: enable wearing and setting direction array
* Input:    fDirectionArr: gsensor direction
* Return: None
******************************************************************/
void HBD_EnableWearing (GF32 fDirectionArr[3])
{
    g_chWearIsEnableFlag = 1;
    //HBAConfigGsensor(fDirectionArr, 6, uchScaleArr);
}

/****************************************************************
* Description: calculate hrv_value by newdata int.
* Input:  stGsAxisValue: gsensor data, if want get algo static state value, could set NULL.
          emGsensorSensitivity: G-sensor sensitivity (counts/g), see EM_HBD_GSENSOR_SENSITIVITY
* Output: pusHrvRRValueArr:ptr of hrv RR value output array
* Return: refresh cnt, all count of RR value have refreshed.
******************************************************************/
GS8 HBD_HrvCalculateByNewdataInt(ST_GS_DATA_TYPE *stGsAxisValue, EM_HBD_GSENSOR_SENSITIVITY emGsensorSensitivity, GU16 *pusHrvRRValueArr)
{
    GU8 uchHrvRefreshedCnt = 0;
    if (g_uchHbdStarted == HBD_START_STATUS_HRV_MODE)
    {
        GU8 uchOutputIndex = 0;
        GS32 nRawDataArr[10] = {0};

        GS16 sGsensorRightShift = (1 << emGsensorSensitivity);

        if (stGsAxisValue != HBD_PTR_NULL) // fix
        {
            nRawDataArr[2] = (GS32)(stGsAxisValue->sXAxisVal / sGsensorRightShift);
            nRawDataArr[3] = (GS32)(stGsAxisValue->sYAxisVal / sGsensorRightShift);
            nRawDataArr[4] = (GS32)(stGsAxisValue->sZAxisVal / sGsensorRightShift);
        }
        HBD_GetNewdataIntData(nRawDataArr);
        ST_ALGO_CALCULATE_INFO algo_calc_info = {0};
        ST_ALGO_RESULT algo_calc_result = {0};
        #ifndef HBD_TEST_NO_ALGO
        algo_calc_info.uchFrameID = nRawDataArr[5];
        algo_calc_info.nRawdata[0] = nRawDataArr[0];
        memcpy(algo_calc_info.sAccData, &nRawDataArr[2], 3 * sizeof(GS32));
        //HOOK_FUNC_CALL(g_pAlgoCalculateHookFunc, (GH3011_FUNC_OFFSET_HRV, &algo_calc_info, &algo_calc_result));
        if(g_pAlgoCalculateHookFunc)
            g_pAlgoCalculateHookFunc(GH3011_FUNC_OFFSET_HRV, &algo_calc_info, &algo_calc_result);
        uchHrvRefreshedCnt = algo_calc_result.uchUpdateFlag;
        #endif
        if (uchHrvRefreshedCnt != 0)
        {
            uchHrvRefreshedCnt  = (GU8)algo_calc_result.snResult[5];
            for ( uchOutputIndex = 0; uchOutputIndex < uchHrvRefreshedCnt; uchOutputIndex ++)
            {
                pusHrvRRValueArr[uchOutputIndex] = (GU16)algo_calc_result.snResult[uchOutputIndex];
            }
        }
    }
    return uchHrvRefreshedCnt;
}

/****************************************************************
* Description: calculate hrv_value by fifo int.
* Input:  stGsAxisValue: gsensor data buffer, if want get algo static state value, could set NULL.
          usGsDataNum: gsensor data count, if want get algo static state value, could set less thah 100.
          emGsensorSensitivity: G-sensor sensitivity (counts/g), see EM_HBD_GSENSOR_SENSITIVITY
* Output: pusHrvRRValueArr:ptr of hrv RR value output array
* Return: refresh cnt, all count of RR value have refreshed.
******************************************************************/
#if 0
GS8 HBD_HrvCalculateByFifoInt(ST_GS_DATA_TYPE stGsAxisValue[], GU16 usGsDataNum, EM_HBD_GSENSOR_SENSITIVITY emGsensorSensitivity, GU16 *pusHrvRRValueArr)
{
    GU8 uchHrvRefreshedCnt = 0;
    if (g_uchHbdStarted == HBD_START_STATUS_HRV_MODE)
    {
        GU8 uchHrvAlgoRet = 0;
        GU8 uchOutputIndex = 0;
        GU16 usRawdataFifoCnt = 0;
        GU16 usIndex = 0;
        GF32 fIndexInc = 0.0f;
        GS32 nRawDataArr[10] = {0};

        GS16 sGsensorRightShift = (1 << emGsensorSensitivity);

        ST_ALGO_CALCULATE_INFO algo_calc_info = {0};
        ST_ALGO_RESULT algo_calc_result = {0};

        //HBD_AutoLedCalcAccSTD(stGsAxisValue, usGsDataNum, &g_stAutoLedAccInfo, sGsensorRightShift);
        usRawdataFifoCnt = HBD_GetFifoRawdata(g_uchReadFifoBuffArr);
        fIndexInc = HBD_CalcGsensorFloatIndex(usRawdataFifoCnt, usGsDataNum);
        if (usRawdataFifoCnt > 0)
        {
            for (usIndex = 0; usIndex < usRawdataFifoCnt; usIndex++)
            {
                HBD_PackFifoRawdataWithGsensor(g_uchReadFifoBuffArr, usRawdataFifoCnt, usIndex,
                                           stGsAxisValue, usGsDataNum, fIndexInc, sGsensorRightShift,
                                           nRawDataArr, HBD_PTR_NULL, HBD_PTR_NULL, 0xFFFF);

                #ifndef HBD_TEST_NO_ALGO
                algo_calc_info.uchFrameID = nRawDataArr[5];
                algo_calc_info.nRawdata[0] = nRawDataArr[0];
                memcpy(algo_calc_info.sAccData, &nRawDataArr[2], 3 * sizeof(GS32));
                HOOK_FUNC_CALL(g_pAlgoCalculateHookFunc, (GH3011_FUNC_OFFSET_HRV, &algo_calc_info, &algo_calc_result));
                uchHrvAlgoRet = algo_calc_result.uchUpdateFlag;
                #endif
                if (uchHrvAlgoRet != 0)
                {
                    uchHrvAlgoRet = (GU8)algo_calc_result.snResult[5];
                    for ( uchOutputIndex = 0; uchOutputIndex < uchHrvAlgoRet; uchOutputIndex ++)
                    {
                        pusHrvRRValueArr[uchHrvRefreshedCnt] = (GU16)algo_calc_result.snResult[uchOutputIndex];
                        uchHrvRefreshedCnt ++;
                    }
                }
            }
        }
    }
    return uchHrvRefreshedCnt;
}
#endif
/****************************************************************
* Description: calculate hrv_value by fifo int.
* Input:  stGsAxisValue: gsensor data buffer, if want get algo static state value, could set NULL.
          usGsDataNum: gsensor data count, if want get algo static state value, could set less thah 100.
          emGsensorSensitivity: G-sensor sensitivity (counts/g), see EM_HBD_GSENSOR_SENSITIVITY
* Output: pusHrvRRValueArr:ptr of hrv RR value output array
          nRawdataOut: ptr of rawdata array output (ppg1, ppg2, gs-x, gs-y, gs-z, index)
          pusRawdataOutLen:ptr of rawdata len output
* Return: refresh cnt, all count of RR value have refreshed.
******************************************************************/
#if 0
GS8 HBD_HrvCalculateByFifoIntDbgRawdata(ST_GS_DATA_TYPE stGsAxisValue[], GU16 usGsDataNum, EM_HBD_GSENSOR_SENSITIVITY emGsensorSensitivity, GU16 *pusHrvRRValueArr, GS32 nRawdataOut[][6], GU16 *pusRawdataOutLen)
{
    GU8 uchHrvRefreshedCnt = 0;
    GU16 usDbgRawdataMaxLen = (*pusRawdataOutLen);
    *pusRawdataOutLen = 0;
    if (g_uchHbdStarted == HBD_START_STATUS_HRV_MODE)
    {
        GU8 uchHrvAlgoRet = 0;
        GU8 uchOutputIndex = 0;
        GU16 usRawdataFifoCnt = 0;
        GU16 usIndex = 0;
        GF32 fIndexInc = 0.0f;
        GS32 nRawDataArr[10] = {0};

        GS16 sGsensorRightShift = (1 << emGsensorSensitivity);
        if (usDbgRawdataMaxLen == 0)
        {
            usDbgRawdataMaxLen = 0xFFFF;
        }

        ST_ALGO_CALCULATE_INFO algo_calc_info = {0};
        ST_ALGO_RESULT algo_calc_result = {0};

        //HBD_AutoLedCalcAccSTD(stGsAxisValue, usGsDataNum, &g_stAutoLedAccInfo, sGsensorRightShift);
        usRawdataFifoCnt = HBD_GetFifoRawdata(g_uchReadFifoBuffArr);
        fIndexInc = HBD_CalcGsensorFloatIndex(usRawdataFifoCnt, usGsDataNum);
        if (usRawdataFifoCnt > 0)
        {
            for (usIndex = 0; usIndex < usRawdataFifoCnt; usIndex++)
            {
                HBD_PackFifoRawdataWithGsensor(g_uchReadFifoBuffArr, usRawdataFifoCnt, usIndex,
                                           stGsAxisValue, usGsDataNum, fIndexInc, sGsensorRightShift,
                                           nRawDataArr, nRawdataOut, pusRawdataOutLen, usDbgRawdataMaxLen);

                #ifndef HBD_TEST_NO_ALGO
                algo_calc_info.uchFrameID = nRawDataArr[5];
                algo_calc_info.nRawdata[0] = nRawDataArr[0];
                memcpy(algo_calc_info.sAccData, &nRawDataArr[2], 3 * sizeof(GS32));
                HOOK_FUNC_CALL(g_pAlgoCalculateHookFunc, (GH3011_FUNC_OFFSET_HRV, &algo_calc_info, &algo_calc_result));
                uchHrvAlgoRet = algo_calc_result.uchUpdateFlag;
                #endif
                if (uchHrvAlgoRet != 0)
                {
                    uchHrvAlgoRet = (GU8)algo_calc_result.snResult[5];
                    for ( uchOutputIndex = 0; uchOutputIndex < uchHrvAlgoRet; uchOutputIndex ++)
                    {
                        pusHrvRRValueArr[uchHrvRefreshedCnt] = (GU16)algo_calc_result.snResult[uchOutputIndex];
                        uchHrvRefreshedCnt ++;
                    }
                }
            }
        }
    }
    return uchHrvRefreshedCnt;
}
#endif

/****************************************************************
* Description: calculate hrv_value by newdata int.
* Input:  stGsAxisValue: gsensor data, if want get algo static state value, could set NULL.
          emGsensorSensitivity: G-sensor sensitivity (counts/g), see EM_HBD_GSENSOR_SENSITIVITY
* Output: pusHrvRRValueArr:ptr of hrv RR value output array
          puchConfidentLvl:ptr of confident level, 0:no confidence, 1:confidence
* Return: refresh cnt, all count of RR value have refreshed.
******************************************************************/
GS8 HBD_HrvCalculateWithLvlByNewdataInt(ST_GS_DATA_TYPE *stGsAxisValue, EM_HBD_GSENSOR_SENSITIVITY emGsensorSensitivity, GU16 *pusHrvRRValueArr, GU8 *puchConfidentLvl)
{
    GU8 uchHrvRefreshedCnt = 0;
    if (g_uchHbdStarted == HBD_START_STATUS_HRV_MODE)
    {
        GU8 uchOutputIndex = 0;
        GS32 nRawDataArr[10] = {0};

        GS16 sGsensorRightShift = (1 << emGsensorSensitivity);

        if (stGsAxisValue != HBD_PTR_NULL) // fix
        {
            nRawDataArr[2] = (GS32)(stGsAxisValue->sXAxisVal / sGsensorRightShift);
            nRawDataArr[3] = (GS32)(stGsAxisValue->sYAxisVal / sGsensorRightShift);
            nRawDataArr[4] = (GS32)(stGsAxisValue->sZAxisVal / sGsensorRightShift);
        }
        HBD_GetNewdataIntData(nRawDataArr);
        ST_ALGO_CALCULATE_INFO algo_calc_info = {0};
        ST_ALGO_RESULT algo_calc_result = {0};
        #ifndef HBD_TEST_NO_ALGO
        algo_calc_info.uchFrameID = nRawDataArr[5];
        algo_calc_info.nRawdata[0] = nRawDataArr[0];
        memcpy(algo_calc_info.sAccData, &nRawDataArr[2], 3 * sizeof(GS32));
        //HOOK_FUNC_CALL(g_pAlgoCalculateHookFunc, (GH3011_FUNC_OFFSET_HRV, &algo_calc_info, &algo_calc_result));
        if(g_pAlgoCalculateHookFunc)
            g_pAlgoCalculateHookFunc(GH3011_FUNC_OFFSET_HRV, &algo_calc_info, &algo_calc_result);
        uchHrvRefreshedCnt = algo_calc_result.uchUpdateFlag;
        #endif
        if (uchHrvRefreshedCnt != 0)
        {
            uchHrvRefreshedCnt = (GU8)algo_calc_result.snResult[5];
            for ( uchOutputIndex = 0; uchOutputIndex < uchHrvRefreshedCnt; uchOutputIndex ++)
            {
                pusHrvRRValueArr[uchOutputIndex] = (GU16)algo_calc_result.snResult[uchOutputIndex];
            }
            if (uchHrvRefreshedCnt > 0)
            {
                *puchConfidentLvl = (GU8)algo_calc_result.snResult[4];
            }
        }
    }
    return uchHrvRefreshedCnt;
}

/****************************************************************
* Description: calculate hrv_value by fifo int.
* Input:  stGsAxisValue: gsensor data buffer, if want get algo static state value, could set NULL.
          usGsDataNum: gsensor data count, if want get algo static state value, could set less thah 100.
          emGsensorSensitivity: G-sensor sensitivity (counts/g), see EM_HBD_GSENSOR_SENSITIVITY
* Output: pusHrvRRValueArr:ptr of hrv RR value output array
          puchConfidentLvl:ptr of confident level, 0:no confidence, 1:confidence
* Return: refresh cnt, all count of RR value have refreshed.
******************************************************************/
#if 0
GS8 HBD_HrvCalculateWithLvlByFifoInt(ST_GS_DATA_TYPE stGsAxisValue[], GU16 usGsDataNum, EM_HBD_GSENSOR_SENSITIVITY emGsensorSensitivity, GU16 *pusHrvRRValueArr, GU8 *puchConfidentLvl)
{
    GU8 uchHrvRefreshedCnt = 0;
    if (g_uchHbdStarted == HBD_START_STATUS_HRV_MODE)
    {
        GU8 uchHrvAlgoRet = 0;
        GU8 uchOutputIndex = 0;
        GU16 usRawdataFifoCnt = 0;
        GU16 usIndex = 0;
        GF32 fIndexInc = 0.0f;
        GS32 nRawDataArr[10] = {0};

        GS16 sGsensorRightShift = (1 << emGsensorSensitivity);

        //HBD_AutoLedCalcAccSTD(stGsAxisValue, usGsDataNum, &g_stAutoLedAccInfo, sGsensorRightShift);
        usRawdataFifoCnt = HBD_GetFifoRawdata(g_uchReadFifoBuffArr);
        fIndexInc = HBD_CalcGsensorFloatIndex(usRawdataFifoCnt, usGsDataNum);
        if (usRawdataFifoCnt > 0)
        {
            for (usIndex = 0; usIndex < usRawdataFifoCnt; usIndex++)
            {
                HBD_PackFifoRawdataWithGsensor(g_uchReadFifoBuffArr, usRawdataFifoCnt, usIndex,
                                           stGsAxisValue, usGsDataNum, fIndexInc, sGsensorRightShift,
                                           nRawDataArr, HBD_PTR_NULL, HBD_PTR_NULL, 0xFFFF);

                ST_ALGO_CALCULATE_INFO algo_calc_info = {0};
                ST_ALGO_RESULT algo_calc_result = {0};
                #ifndef HBD_TEST_NO_ALGO
                algo_calc_info.uchFrameID = nRawDataArr[5];
                algo_calc_info.nRawdata[0] = nRawDataArr[0];
                memcpy(algo_calc_info.sAccData, &nRawDataArr[2], 3 * sizeof(GS32));
                HOOK_FUNC_CALL(g_pAlgoCalculateHookFunc, (GH3011_FUNC_OFFSET_HRV, &algo_calc_info, &algo_calc_result));
                uchHrvRefreshedCnt = algo_calc_result.uchUpdateFlag;
                #endif
                if (uchHrvAlgoRet != 0)
                {
                    uchHrvAlgoRet = (GU8)algo_calc_result.snResult[5];
                    for ( uchOutputIndex = 0; uchOutputIndex < uchHrvAlgoRet; uchOutputIndex ++)
                    {
                        pusHrvRRValueArr[uchHrvRefreshedCnt] = (GU16)algo_calc_result.snResult[uchOutputIndex];
                        uchHrvRefreshedCnt ++;
                    }
                    if (uchHrvAlgoRet > 0)
                    {
                        *puchConfidentLvl = (GU8)algo_calc_result.snResult[4];
                    }
                }
            }
        }
    }
    return uchHrvRefreshedCnt;
}
#endif

/****************************************************************
* Description: calculate hrv_value by fifo int.
* Input:  stGsAxisValue: gsensor data buffer, if want get algo static state value, could set NULL.
          usGsDataNum: gsensor data count, if want get algo static state value, could set less thah 100.
          emGsensorSensitivity: G-sensor sensitivity (counts/g), see EM_HBD_GSENSOR_SENSITIVITY
* Output: pusHrvRRValueArr:ptr of hrv RR value output array
          puchConfidentLvl:ptr of confident level, 0:no confidence, 1:confidence
          nRawdataOut: ptr of rawdata array output (ppg1, ppg2, gs-x, gs-y, gs-z, index)
          pusRawdataOutLen:ptr of rawdata len output
* Return: refresh cnt, all count of RR value have refreshed.
******************************************************************/
#if 0
GS8 HBD_HrvCalculateWithLvlByFifoIntDbgRawdata(ST_GS_DATA_TYPE stGsAxisValue[], GU16 usGsDataNum, EM_HBD_GSENSOR_SENSITIVITY emGsensorSensitivity, GU16 *pusHrvRRValueArr, GU8 *puchConfidentLvl, GS32 nRawdataOut[][6], GU16 *pusRawdataOutLen)
{
    GU8 uchHrvRefreshedCnt = 0;
    GU16 usDbgRawdataMaxLen = (*pusRawdataOutLen);
    *pusRawdataOutLen = 0;
    if (g_uchHbdStarted == HBD_START_STATUS_HRV_MODE)
    {
        GU8 uchHrvAlgoRet = 0;
        GU8 uchOutputIndex = 0;
        GU16 usRawdataFifoCnt = 0;
        GU16 usIndex = 0;
        GF32 fIndexInc = 0.0f;
        GS32 nRawDataArr[10] = {0};

        GS16 sGsensorRightShift = (1 << emGsensorSensitivity);
        if (usDbgRawdataMaxLen == 0)
        {
            usDbgRawdataMaxLen = 0xFFFF;
        }

        //HBD_AutoLedCalcAccSTD(stGsAxisValue, usGsDataNum, &g_stAutoLedAccInfo, sGsensorRightShift);
        usRawdataFifoCnt = HBD_GetFifoRawdata(g_uchReadFifoBuffArr);
        fIndexInc = HBD_CalcGsensorFloatIndex(usRawdataFifoCnt, usGsDataNum);
        if (usRawdataFifoCnt > 0)
        {
            for (usIndex = 0; usIndex < usRawdataFifoCnt; usIndex++)
            {
                HBD_PackFifoRawdataWithGsensor(g_uchReadFifoBuffArr, usRawdataFifoCnt, usIndex,
                                           stGsAxisValue, usGsDataNum, fIndexInc, sGsensorRightShift,
                                           nRawDataArr, nRawdataOut, pusRawdataOutLen, usDbgRawdataMaxLen);

                ST_ALGO_CALCULATE_INFO algo_calc_info = {0};
                ST_ALGO_RESULT algo_calc_result = {0};
                #ifndef HBD_TEST_NO_ALGO
                algo_calc_info.uchFrameID = nRawDataArr[5];
                algo_calc_info.nRawdata[0] = nRawDataArr[0];
                memcpy(algo_calc_info.sAccData, &nRawDataArr[2], 3 * sizeof(GS32));
                HOOK_FUNC_CALL(g_pAlgoCalculateHookFunc, (GH3011_FUNC_OFFSET_HRV, &algo_calc_info, &algo_calc_result));
                uchHrvAlgoRet = algo_calc_result.uchUpdateFlag; // hrv calc
                #endif
                if (uchHrvAlgoRet != 0)
                {
                    uchHrvAlgoRet = (GU8)algo_calc_result.snResult[5];
                    for ( uchOutputIndex = 0; uchOutputIndex < uchHrvAlgoRet; uchOutputIndex ++)
                    {
                        pusHrvRRValueArr[uchHrvRefreshedCnt] = (GU16)algo_calc_result.snResult[uchOutputIndex];
                        uchHrvRefreshedCnt ++;
                    }
                    if (uchHrvAlgoRet > 0)
                    {
                        *puchConfidentLvl = (GU8)algo_calc_result.snResult[4];
                    }
                }
            }
        }
    }
    return uchHrvRefreshedCnt;
}
#endif

/****************************************************************
* Description: calculate hrv_value by fifo int.
* Input:  stGsAxisValue: gsensor data buffer, if want get algo static state value, could set NULL.
          usGsDataNum: gsensor data count, if want get algo static state value, could set less thah 100.
          emGsensorSensitivity: G-sensor sensitivity (counts/g), see EM_HBD_GSENSOR_SENSITIVITY
* Output: pstHrvRes:ptr of hrv result
          nRawdataOut: ptr of rawdata array output (ppg1, ppg2, gs-x, gs-y, gs-z, index)
          pusRawdataOutLen:ptr of rawdata len output
* Return: refresh cnt, all count of RR value have refreshed.
******************************************************************/
#if 0
GS8 HBD_HrvCalculateByFifoIntEx(ST_GS_DATA_TYPE stGsAxisValue[], GU16 usGsDataNum, EM_HBD_GSENSOR_SENSITIVITY emGsensorSensitivity, ST_HRV_RES *pstHrvRes, GS32 nRawdataOut[][6], GU16 *pusRawdataOutLen)
{
    GU16 usDbgRawdataMaxLen = (*pusRawdataOutLen);
    *pusRawdataOutLen = 0;
    if (g_uchHbdStarted == HBD_START_STATUS_HRV_MODE)
    {
        GU8 uchHrvAlgoRet = 0;
        GU16 usRawdataFifoCnt = 0;
        GU16 usIndex = 0;
        GF32 fIndexInc = 0.0f;
        GS32 nRawDataArr[10] = {0};

        GS16 sGsensorRightShift = (1 << emGsensorSensitivity);
        if (usDbgRawdataMaxLen == 0)
        {
            usDbgRawdataMaxLen = 0xFFFF;
        }

        //HBD_AutoLedCalcAccSTD(stGsAxisValue, usGsDataNum, &g_stAutoLedAccInfo, sGsensorRightShift);
        usRawdataFifoCnt = HBD_GetFifoRawdata(g_uchReadFifoBuffArr);
        fIndexInc = HBD_CalcGsensorFloatIndex(usRawdataFifoCnt, usGsDataNum);
        if (usRawdataFifoCnt > 0)
        {
            for (usIndex = 0; usIndex < usRawdataFifoCnt; usIndex++)
            {
                HBD_PackFifoRawdataWithGsensor(g_uchReadFifoBuffArr, usRawdataFifoCnt, usIndex,
                                           stGsAxisValue, usGsDataNum, fIndexInc, sGsensorRightShift,
                                           nRawDataArr, nRawdataOut, pusRawdataOutLen, usDbgRawdataMaxLen);

                #ifndef HBD_TEST_NO_ALGO
                ST_ALGO_CALCULATE_INFO algo_calc_info = {0};
                ST_ALGO_RESULT algo_calc_result = {0};
                algo_calc_info.uchFrameID = nRawDataArr[5];
                algo_calc_info.nRawdata[0] = nRawDataArr[0];
                memcpy(algo_calc_info.sAccData, &nRawDataArr[2], 3 * sizeof(GS32));
                HOOK_FUNC_CALL(g_pAlgoCalculateHookFunc, (GH3011_FUNC_OFFSET_HRV, &algo_calc_info, &algo_calc_result));
                uchHrvAlgoRet = algo_calc_result.uchUpdateFlag; // hrv calc
                #endif
                if (uchHrvAlgoRet != 0)
                {
                    //uchHrvAlgoRet = (GU8)algo_calc_result.snResult[5];
                    pstHrvRes->usRRvalueArr[0]         = (GU16)algo_calc_result.snResult[0];
                    pstHrvRes->usRRvalueArr[1]         = (GU16)algo_calc_result.snResult[1];
                    pstHrvRes->usRRvalueArr[2]         = (GU16)algo_calc_result.snResult[2];
                    pstHrvRes->usRRvalueArr[3]         = (GU16)algo_calc_result.snResult[3];
                    pstHrvRes->uchHrvConfidentLvl    = (GU8)algo_calc_result.snResult[4];
                    pstHrvRes->uchRRvalueCnt         = (GU8)algo_calc_result.snResult[5];
                    // for ( uchOutputIndex = 0; uchOutputIndex < uchHrvAlgoRet; uchOutputIndex ++)
                    // {
                        // pstHrvRes->usRRvalueArr[uchHrvRefreshedCnt] = (GU16)algo_calc_result.snResult[uchOutputIndex];
                        // uchHrvRefreshedCnt ++;
                    // }

                    // if (uchHrvAlgoRet > 0)
                    // {
                        // pstHrvRes->uchHrvConfidentLvl = (GU8)algo_calc_result.snResult[4];
                    // }
                }
            }
        }
    }
    return pstHrvRes->uchRRvalueCnt;
}
#endif


/****************************************************************
* Description: calculate wear state by newdata int.
* Input:  stGsAxisValue: gsensor data, if want get algo static state value, could set NULL.
          emGsensorSensitivity: G-sensor sensitivity (counts/g), see EM_HBD_GSENSOR_SENSITIVITY
* Output: puchWearState:ptr of wear state value output, 0:default, 1:wear, 2:unwear,
* Return: refresh flag, if wear state value have refresh that return 1.
******************************************************************/
GU8 HBD_InearEpWearDetectCalculateByNewdataInt(ST_GS_DATA_TYPE *stGsAxisValue, EM_HBD_GSENSOR_SENSITIVITY emGsensorSensitivity, GU8 *puchWearState)
{
    GU8 uchRet = 0;
    if (g_uchHbdStarted == HBD_START_STATUS_IN_EAR_EP_WEAR_DETECT_MODE)
    {
        GS32 nRawDataArr[10] = {0};
        //GS8 nWdArr[1] = {0};
        //GU8 nWdArr[1] = {0};//goodix4318 change data type
        GS16 sGsensorRightShift = (1 << emGsensorSensitivity);

        if (stGsAxisValue != HBD_PTR_NULL) // fix
        {
            nRawDataArr[2] = (GS32)(stGsAxisValue->sXAxisVal / sGsensorRightShift);
            nRawDataArr[3] = (GS32)(stGsAxisValue->sYAxisVal / sGsensorRightShift);
            nRawDataArr[4] = (GS32)(stGsAxisValue->sZAxisVal / sGsensorRightShift);
        }
        HBD_GetNewdataIntData(nRawDataArr);
        ST_ALGO_CALCULATE_INFO algo_calc_info = {0};
        ST_ALGO_RESULT algo_calc_result = {0};
        algo_calc_info.uchFrameID = nRawDataArr[5];
        algo_calc_info.nRawdata[0] = nRawDataArr[0] & 0xffffffff;
        memcpy(algo_calc_info.sAccData, &nRawDataArr[2], 3 * sizeof(GS32));
        algo_calc_info.led_current[1] = g_uchAutoLedCurrentArr[1];
        algo_calc_info.nRawdata[1] = nRawDataArr[1] & 0xffffffff;

        #ifndef HBD_TEST_NO_ALGO
        //HOOK_FUNC_CALL(g_pAlgoCalculateHookFunc, (GH3011_FUNC_OFFSET_NADT, &algo_calc_info, &algo_calc_result));
        if(g_pAlgoCalculateHookFunc)
            g_pAlgoCalculateHookFunc(GH3011_FUNC_OFFSET_NADT, &algo_calc_info, &algo_calc_result);
        #endif
        *puchWearState = (GU8)algo_calc_result.snResult[12];
    }

    return uchRet;
}

/****************************************************************
* Description: calculate wear state by fifo int.
* Input:  stGsAxisValue: gsensor data buffer, if want get algo static state value, could set NULL.
          usGsDataNum: gsensor data count, if want get algo static state value, could set less thah 25.
          emGsensorSensitivity: G-sensor sensitivity (counts/g), see EM_HBD_GSENSOR_SENSITIVITY
* Output: puchWearState:ptr of wear state value output, 0:default, 1:wear, 2:unwear,
* Return: refresh flag, if wear state value have refresh that return 1.
******************************************************************/
#if 0
GU16 HBD_InearEpWearDetectCalculateByFifoInt(ST_GS_DATA_TYPE stGsAxisValue[], GU16 usGsDataNum, EM_HBD_GSENSOR_SENSITIVITY emGsensorSensitivity, GU8 *puchWearState)
{
    GU8 uchRet = 0;
    if (g_uchHbdStarted == HBD_START_STATUS_IN_EAR_EP_WEAR_DETECT_MODE)
    {
        GU16 usRawdataFifoCnt = 0;
        GU16 usIndex = 0;
        GF32 fIndexInc = 0.0f;
        GS32 nRawDataArr[10] = {0};
        //GS8 nWdArr[1] = {0};
        //GU8 nWdArr[1] = {0};//goodix4318 change data type
        GS16 sGsensorRightShift = (1 << emGsensorSensitivity);
        ST_ALGO_CALCULATE_INFO algo_calc_info = {0};
        ST_ALGO_RESULT algo_calc_result = {0};
        //HBD_AutoLedCalcAccSTD(stGsAxisValue, usGsDataNum, &g_stAutoLedAccInfo, sGsensorRightShift);
        usRawdataFifoCnt = HBD_GetFifoRawdata(g_uchReadFifoBuffArr);
        fIndexInc = HBD_CalcGsensorFloatIndex(usRawdataFifoCnt, usGsDataNum);
        if (usRawdataFifoCnt > 0)
        {
            for (usIndex = 0; usIndex < usRawdataFifoCnt; usIndex++)
            {
                HBD_PackFifoRawdataWithGsensor(g_uchReadFifoBuffArr, usRawdataFifoCnt, usIndex,
                                           stGsAxisValue, usGsDataNum, fIndexInc, sGsensorRightShift,
                                           nRawDataArr, HBD_PTR_NULL, HBD_PTR_NULL, 0xFFFF);

                algo_calc_info.uchFrameID = nRawDataArr[5];
                algo_calc_info.nRawdata[0] = nRawDataArr[0] & 0xffffffff;
                memcpy(algo_calc_info.sAccData, &nRawDataArr[2], 3 * sizeof(GS32));
                algo_calc_info.led_current[1] = g_uchAutoLedCurrentArr[1];
                algo_calc_info.nRawdata[1] = nRawDataArr[1] & 0xffffffff;

                #ifndef HBD_TEST_NO_ALGO
                HOOK_FUNC_CALL(g_pAlgoCalculateHookFunc, (GH3011_FUNC_OFFSET_NADT, &algo_calc_info, &algo_calc_result));
                #endif
                *puchWearState = (GU8)algo_calc_result.snResult[12];
            }
        }
    }

    return uchRet;
}
#endif

/****************************************************************
* Description: calculate wear state by fifo int.
* Input:  stGsAxisValue: gsensor data buffer, if want get algo static state value, could set NULL.
          usGsDataNum: gsensor data count, if want get algo static state value, could set less thah 25.
          emGsensorSensitivity: G-sensor sensitivity (counts/g), see EM_HBD_GSENSOR_SENSITIVITY
* Output: puchWearState:ptr of wear state value output, 0:default, 1:wear, 2:unwear,
          nRawdataOut: ptr of rawdata array output (ppg1, ppg2, gs-x, gs-y, gs-z, index)
          pusRawdataOutLen：ptr of rawdata len output
* Return: refresh flag, if wear state value have refresh that return 1.
******************************************************************/
#if 0
GU16 HBD_InearEpWearDetectByFifoIntDbgRawdata(ST_GS_DATA_TYPE stGsAxisValue[], GU16 usGsDataNum, EM_HBD_GSENSOR_SENSITIVITY emGsensorSensitivity, GU8 *puchWearState, GS32 nRawdataOut[][6], GU16 *pusRawdataOutLen)
{
    GU8 uchRet = 0;
    GU16 usDbgRawdataMaxLen = (*pusRawdataOutLen);
    *pusRawdataOutLen = 0;
    if (g_uchHbdStarted == HBD_START_STATUS_IN_EAR_EP_WEAR_DETECT_MODE)
    {
        GU16 usRawdataFifoCnt = 0;
        GU16 usIndex = 0;
        GF32 fIndexInc = 0.0f;
        GS32 nRawDataArr[10] = {0};
        //GS8 nWdArr[1] = {0};
        //GU8 nWdArr[1] = {0};//goodix4318 change data type
        GS16 sGsensorRightShift = (1 << emGsensorSensitivity);
        ST_ALGO_CALCULATE_INFO algo_calc_info = {0};
        ST_ALGO_RESULT algo_calc_result = {0};
        if (usDbgRawdataMaxLen == 0)
        {
            usDbgRawdataMaxLen = 0xFFFF;
        }

        //HBD_AutoLedCalcAccSTD(stGsAxisValue, usGsDataNum, &g_stAutoLedAccInfo, sGsensorRightShift);
        usRawdataFifoCnt = HBD_GetFifoRawdata(g_uchReadFifoBuffArr);
        fIndexInc = HBD_CalcGsensorFloatIndex(usRawdataFifoCnt, usGsDataNum);
        if (usRawdataFifoCnt > 0)
        {
            for (usIndex = 0; usIndex < usRawdataFifoCnt; usIndex++)
            {
                HBD_PackFifoRawdataWithGsensor(g_uchReadFifoBuffArr, usRawdataFifoCnt, usIndex,
                                           stGsAxisValue, usGsDataNum, fIndexInc, sGsensorRightShift,
                                           nRawDataArr, nRawdataOut, pusRawdataOutLen, usDbgRawdataMaxLen);

                algo_calc_info.uchFrameID = nRawDataArr[5];
                algo_calc_info.nRawdata[0] = nRawDataArr[0] & 0xffffffff;
                memcpy(algo_calc_info.sAccData, &nRawDataArr[2], 3 * sizeof(GS32));
                algo_calc_info.led_current[1] = g_uchAutoLedCurrentArr[1];
                algo_calc_info.nRawdata[1] = nRawDataArr[1] & 0xffffffff;

                #ifndef HBD_TEST_NO_ALGO
                HOOK_FUNC_CALL(g_pAlgoCalculateHookFunc, (GH3011_FUNC_OFFSET_NADT, &algo_calc_info, &algo_calc_result));
                #endif
                *puchWearState = (GU8)algo_calc_result.snResult[12];
            }
        }
    }

    return uchRet;
}
#endif

/****************************************************************
* Description: get fifo count has read
* Input:  None,
* Return: fifo count has read,
******************************************************************/
GU8 HBD_GetFifoCntHasRead (void)
{
    GU8 uchFifoCount = g_uchLastReadFifoCnt;
    g_uchLastReadFifoCnt = 0;
    return uchFifoCount;
}

/****************************************************************
* Description: Reset chip
* Input:  None
* Return: HBD_RET_OK=success,
          HBD_RET_COMM_NOT_REGISTERED_ERROR=GH30x i2c interface not registered
******************************************************************/
GS8 HBD_ChipReset (void)
{
    if ((g_pI2cWriteFunc == HBD_PTR_NULL) || (g_pI2cReadFunc == HBD_PTR_NULL) )
    {
        return HBD_RET_COMM_NOT_REGISTERED_ERROR;
    }
    HBD_I2cSendCmd(HBD_CMD_RESET);
    HBD_DelayUs(HBD_SEND_RST_CMD_DELAY_X_US);
    g_uchHbdInited = HBD_INIT_STATUS_NO_INIT;
    return HBD_RET_OK;
}

/****************************************************************
* Description: set irq pluse width
* Input:  uchIrqPluseWidth: irq width(us) setting ,
                            if set 0: fixed 1us and return HBD_RET_PARAMETER_ERROR
* Return: HBD_RET_OK=success,
          HBD_RET_PARAMETER_ERROR=fail:parameter error
******************************************************************/
GS8 HBD_SetIrqPluseWidth(GU8 uchIrqPluseWidth)
{
    if (uchIrqPluseWidth == 0)
    {
        g_usIrqPluseWidth = 1;
        return HBD_RET_PARAMETER_ERROR;
    }
    g_usIrqPluseWidth = uchIrqPluseWidth;
    return HBD_RET_OK;
}

/****************************************************************
* Description: change Hb config
* Input:  uchMode: 0:Hrv reconfig, else Hb reconfig
*         emFifoEnable: see EM_HBD_FUNCTIONAL_STATE
* Return: HBD_RET_OK=success,
          HBD_RET_PARAMETER_ERROR=fail:parameter error,
******************************************************************/
GS8 HBD_FifoConfig(GU8 uchMode, EM_HBD_FUNCTIONAL_STATE emFifoEnable)
{
    if (emFifoEnable > HBD_FUNCTIONAL_STATE_ENABLE)
    {
        return HBD_RET_PARAMETER_ERROR;
    }
    if (uchMode == 0)
    {
        g_emHrvFifoEnableFlag = emFifoEnable;
    }
    else
    {
        g_emHbFifoEnableFlag = emFifoEnable;
        g_emSpo2FifoEnableFlag = emFifoEnable;
        g_emBpfFifoEnableFlag = emFifoEnable;
    }

    return HBD_RET_OK;
}

/****************************************************************
* Description: set irq pluse width
* Input:  uchMode: 0:Hrv config,
                   1:Hb config
                   2:hsm config
                   3:bpd config
                   4:pfa config
            usFifoCnt: Fifo thr setting (if enable two channel led,thr will auto * 2)
* Return: HBD_RET_OK=success,
          HBD_RET_PARAMETER_ERROR=fail:parameter error, usFifoCnt not allow 0, uchMode > 4
******************************************************************/
#if 0
GS8 HBD_SetFifoThrCnt (GU8 uchMode, GU16 usFifoCnt)
{
    GS8 chRet = HBD_RET_PARAMETER_ERROR;
    if ((usFifoCnt > 0) && (uchMode < HBD_FIFO_THR_CONFIG_NUM))
    {
        g_stFifoThrConfigList[uchMode].usFifoThrCntConfigVal = usFifoCnt;
        //g_stFifoThrConfigList[uchMode].uchFifoThrCntConfigByFunc = 1;
        chRet = HBD_RET_OK;
    }

    return chRet;
}
#endif

/****************************************************************
* Description: Get version
* Input:    None,
* Return: library Hbd ctrl version
******************************************************************/
GS8 * HBD_GetHbdVersion (void)
{
    return (GS8 *)g_szHbdCtrlVersionStrArr;
}

#if (ENABLE_SPO2_ABNORMAL_STATE)
/****************************************************************
* Description: Get SpO2_value good or not
* Return : 0-data good(signal good), 1-Detected motion, 2-PI too big, 3-data bad(signal bad)
******************************************************************/
GU8 HBD_GetSpo2AbnormalState (void)
{
    return g_nSpo2AbnormalState;
}

/****************************************************************
* Description: calculate SpO2_value good or not
* Input:nSpO2Arr[0]：血氧值，返回为70-100；
        nSpO2Arr[1]：血氧置信等级；
        nSpO2Arr[2]：血氧实时置信度；
        nSpO2Arr[3]：血氧平均R值*10000；
        nSpO2Arr[4]：红外心率值；
        nSpO2Arr[5]：红外心率置信度；
        nSpO2Arr[6]：数据无效标记，无效（调光或运动）时为1
        nSpO2Arr[7]：红外PI值*100000
        nSpO2Arr[8]：红光PI值*100000
        nSpO2Arr[9]：红外SNR值*10
        nSpO2Arr[10]：红光SNR值*10
        nSpO2Arr[11]：血氧实时R值*10000
******************************************************************/
static void HBD_SpO2AbnormalStateCalc(GS32 uchRet, ST_ALGO_RESULT *stSpo2Result)
{
    GU8 i = 0;
    static GU16 warningcnt = 0;
    GS32 IRPIsum = 0;
    GS32 RedPIsum = 0;
    GS32 IRPImean = 0;
    GS32 RedPImean = 0;
    static GS32 IRPIcnt = 0;
    static GS32 RedPIcnt = 0;
    g_nSpo2AbnormalState = 0;
    if(1==uchRet)
    {
        if(stSpo2Result->snResult[0] > 0 && stSpo2Result->snResult[0]<70)
        {
            stSpo2Result->snResult[0] = 70;
        }
        else if(stSpo2Result->snResult[0]>100)
        {
            stSpo2Result->snResult[0] = 100;
        }
        if(0 == stSpo2Result->snResult[6] && stSpo2Result->snResult[1] <= 0)
        {
            for(i=0;i<SPO2_RESULT_CHECK_BUF_LEN-1;i++)
            {
                g_nIRPIarr[i+1] = g_nIRPIarr[i];
                g_nRedPIarr[i+1] = g_nRedPIarr[i];
                IRPIsum  = IRPIsum+g_nIRPIarr[i];
                RedPIsum = RedPIsum+g_nRedPIarr[i];
            }
            g_nIRPIarr[0]  = stSpo2Result->snResult[7];
            g_nRedPIarr[0] = stSpo2Result->snResult[8];
            IRPIsum  = IRPIsum+g_nIRPIarr[0];
            RedPIsum = RedPIsum+g_nRedPIarr[0];
            IRPImean = IRPIsum/SPO2_RESULT_CHECK_BUF_LEN;
            RedPImean = RedPIsum/SPO2_RESULT_CHECK_BUF_LEN;
            if (IRPImean>30)
            {
                IRPIcnt = IRPIcnt +1;
            }
            else
            {
                IRPIcnt = 0;
            }
            if (RedPImean>30)
            {
                RedPIcnt = RedPIcnt +1;
            }
            else
            {
                RedPIcnt = 0;
            }
        }
        else
        {
            IRPIcnt = 0;
            RedPIcnt = 0;
        }
        if(1 == stSpo2Result->snResult[6])
        {
            g_nSpo2AbnormalState = 1;
        }
        else
        {
            if(stSpo2Result->snResult[1]<=0)
            {
                warningcnt = warningcnt +1;
                if(warningcnt>20)
                {
                    if (IRPIcnt >20 || RedPIcnt >20)
                    {
                        g_nSpo2AbnormalState = 2;
                    }
                    else
                    {
                        g_nSpo2AbnormalState = 3;
                    }
                }
            }
            else
            {
                warningcnt = 0;
            }
        }
    }
}
#endif

#define SPO2MAPSELECT   0  //0: no map  1: map for lexin   2:map for aiwei
//const static GS32 Spo2MapTtable1[31] ={90,90,91,91,91,91,92,92,92,92,93,93,93,93,94,94,94,94,95,95,95,95,96,96,97,97,98,98,99,99,100}; //for lexin
//const static GS32 Spo2MapTtable2[31] ={80,81,81,82,83,83,84,85,85,86,87,87,88,89,89,90,91,91,92,93,93,94,95,95,96,97,97,98,99,99,100};  //for aiwei

/****************************************************************
* Description: calculate SpO2_value
* Input:  pstParam: pointer of result struct
* Return:  refresh flag, if spo2 value have refresh that return 1.
******************************************************************/
#if 0
static GU8 HBD_Spo2CalculateCore(ST_SPO2_CAL_PARAM *pstParam)
{
    GU8 uchRet = 0;
    GU8 uchHrvAlgoRet = 0;
    //GS32 nSpO2Arr[13] = {0};

    if(pstParam->uchFifoMode)
    {
        if(pstParam->uchU8Mode)
        {
            HBD_PackFifoRawdataWithGsensorDbgU8(g_uchReadFifoBuffArr, pstParam->usRawdataFifoCnt, pstParam->usIndex,
                                           pstParam->stGsAxisValueArr, pstParam->usGsDataNum, pstParam->fIndexInc, pstParam->sGsensorRightShift,
                                           pstParam->nRawDataArr, (GU8 *)pstParam->pnRawdataOut, pstParam->pusRawdataOutLen, pstParam->uchDbgPkgLen, pstParam->usDbgRawdataMaxLen);
        }
        else
        {
            HBD_PackFifoRawdataWithGsensor(g_uchReadFifoBuffArr, pstParam->usRawdataFifoCnt, pstParam->usIndex,
                                   pstParam->stGsAxisValueArr, pstParam->usGsDataNum, pstParam->fIndexInc, pstParam->sGsensorRightShift,
                                   pstParam->nRawDataArr, pstParam->pnRawdataOut, pstParam->pusRawdataOutLen, pstParam->usDbgRawdataMaxLen);
        }
    }

    ST_ALGO_CALCULATE_INFO algo_calc_info = {0};
    ST_ALGO_RESULT algo_calc_result = {0};

#ifndef HBD_TEST_NO_ALGO
    algo_calc_info.uchFrameID = pstParam->nRawDataArr[5];
    algo_calc_info.nRawdata[0] = pstParam->nRawDataArr[0] & 0xffffff;
    algo_calc_info.nRawdata[1] = pstParam->nRawDataArr[1] & 0xffffff;
    algo_calc_info.nCurAdjFlag[0] = (pstParam->nRawDataArr[0] >> 30) & 0x1;
    algo_calc_info.nCurAdjFlag[1] = (pstParam->nRawDataArr[1] >> 30) & 0x1;
    memcpy(algo_calc_info.sAccData, &pstParam->nRawDataArr[2], 3 * sizeof(GS32));
    algo_calc_info.unSleepFlag = 0;//g_sleep_flg
    HOOK_FUNC_CALL(g_pAlgoCalculateHookFunc, (GH3011_FUNC_OFFSET_HR, &algo_calc_info, &algo_calc_result));
    *pstParam->pliSpo2InvalidFlag = (GS32)algo_calc_result.snResult[6];

    //uchRet = SpO2Caluate(pstParam->nRawDataArr, nSpO2Arr);

#endif
    #if (ENABLE_SPO2_ABNORMAL_STATE)
    HBD_SpO2AbnormalStateCalc(uchRet, &algo_calc_result);
    #endif
    if(1)
    {
        if(algo_calc_result.snResult[0] > 0 && algo_calc_result.snResult[0] < 70)
        {
            algo_calc_result.snResult[0] = 70;
        }
        if (1==SPO2MAPSELECT)
        {
            *(pstParam->puchSpo2Value) = Spo2MapTtable1[(GS32)algo_calc_result.snResult[0]-70];
        }
        else if(2==SPO2MAPSELECT)
        {
            *(pstParam->puchSpo2Value) = Spo2MapTtable2[(GS32)algo_calc_result.snResult[0]-70];
        }
        else
        {
            *(pstParam->puchSpo2Value) = algo_calc_result.snResult[0];
        }
        *(pstParam->puchValidLvl)         = (GS32)algo_calc_result.snResult[1];
        *(pstParam->puchSpo2ConfidentLvl) = (GU8)algo_calc_result.snResult[2];
        *(pstParam->pusSpo2RVal)          = (GU16)algo_calc_result.snResult[11];
        *(pstParam->puchHbValue)          = (GU8)algo_calc_result.snResult[4];
        *(pstParam->puchHbConfidentLvl)   = (GU8)algo_calc_result.snResult[5];
    }

    algo_calc_info.uchFrameID = pstParam->nRawDataArr[5];
    algo_calc_info.nRawdata[0] = pstParam->nRawDataArr[0];
    memcpy(algo_calc_info.sAccData, &pstParam->nRawDataArr[2], 3 * sizeof(GS32));
    HOOK_FUNC_CALL(g_pAlgoCalculateHookFunc, (GH3011_FUNC_OFFSET_HRV, &algo_calc_info, &algo_calc_result));
    uchHrvAlgoRet = algo_calc_result.uchUpdateFlag;
    if(1==uchHrvAlgoRet){
        *(pstParam->pusHrvRRVal1) = (GU16)algo_calc_result.snResult[0];
        *(pstParam->pusHrvRRVal2) = (GU16)algo_calc_result.snResult[1];
        *(pstParam->pusHrvRRVal3) = (GU16)algo_calc_result.snResult[2];
        *(pstParam->pusHrvRRVal4) = (GU16)algo_calc_result.snResult[3];
        *(pstParam->puchHrvConfidentLvl)  = (GU8)algo_calc_result.snResult[4];
        *(pstParam->puchHrvcnt)           = (GU8)algo_calc_result.snResult[5];
    }
    return uchRet;
}
#endif


#if 0
static void HBD_ConnectResult(ST_SPO2_CAL_PARAM *pstParam, ST_GS_DATA_TYPE stGsAxisValue[], GU16 usGsDataNum, EM_HBD_GSENSOR_SENSITIVITY emGsensorSensitivity, 
                              GU8 *pusSpo2Value, GU8 *puchSpo2ConfidentLvl, GU8 *puchHbValue, GU8 *puchHbConfidentLvl,
                              GU16 *pusHrvRRVal1,GU16 *pusHrvRRVal2,GU16 *pusHrvRRVal3,GU16 *pusHrvRRVal4, GU8 *puchHrvConfidentLvl, GU8 *puchHrvcnt,
                              GU16 *pusSpo2RVal, GS32 *pliSpo2InvalidFlag, void *pnRawdataOut, GU16 *pusRawdataOutLen, GU8 *puchValidLvl)
{
    pstParam->stGsAxisValueArr = stGsAxisValue;
    pstParam->usGsDataNum = usGsDataNum;
    pstParam->sGsensorRightShift = (1 << emGsensorSensitivity);
    pstParam->puchSpo2Value = pusSpo2Value;
    pstParam->puchSpo2ConfidentLvl = puchSpo2ConfidentLvl;
    pstParam->pusSpo2RVal = pusSpo2RVal;
    pstParam->puchValidLvl = puchValidLvl;
    pstParam->puchHbValue = puchHbValue;
    pstParam->puchHbConfidentLvl = puchHbConfidentLvl;
    pstParam->pusHrvRRVal1 = pusHrvRRVal1;
    pstParam->pusHrvRRVal2 = pusHrvRRVal2;
    pstParam->pusHrvRRVal3 = pusHrvRRVal3;
    pstParam->pusHrvRRVal4 = pusHrvRRVal4;
    pstParam->puchHrvcnt = puchHrvcnt;
    pstParam->puchHrvConfidentLvl = puchHrvConfidentLvl;
    pstParam->pnRawdataOut = pnRawdataOut;
    pstParam->usDbgRawdataMaxLen = (*pusRawdataOutLen);
    pstParam->pusRawdataOutLen = pusRawdataOutLen;
    pstParam->pliSpo2InvalidFlag = pliSpo2InvalidFlag;
}

#endif
/****************************************************************
* Description: calculate SpO2_value by fifo int.
* Input:  stGsAxisValue: gsensor data buffer, if want get algo static state value, could set NULL.
          usGsDataNum: gsensor data count, if want get algo static state value, could set less thah 100.
          emGsensorSensitivity: G-sensor sensitivity (counts/g), see EM_HBD_GSENSOR_SENSITIVITY
* Output:     pusSpo2Value : Spo2 Value
            puchSpo2ConfidentLvl: Spo2 Confident Level
            puchHbValue  : HB Value
            puchHbConfidentLvl: HB Confident Level
            pusHrvRRVal1 : HRV RRI value 1
            pusHrvRRVal2 : HRV RRI value 2
            pusHrvRRVal3 : HRV RRI value 3
            pusHrvRRVal4 : HRV RRI value 4
            puchHrvConfidentLvl: HRV Confident Level
            puchHrvcnt : HRV valid cnt
            pusSpo2RVal :spo2 R value
            pliSpo2InvalidFlag:InvalidFlag
            puchWearingState : Wearing State
* Return:  refresh flag, if spo2 value have refresh that return 1.
******************************************************************/
#if 0
GU8 HBD_Spo2CalculateByFifoInt(ST_GS_DATA_TYPE stGsAxisValue[], GU16 usGsDataNum, EM_HBD_GSENSOR_SENSITIVITY emGsensorSensitivity, GU8 *pusSpo2Value, GU8 *puchSpo2ConfidentLvl, GU8 *puchHbValue, GU8 *puchHbConfidentLvl,
                                     GU16 *pusHrvRRVal1,GU16 *pusHrvRRVal2,GU16 *pusHrvRRVal3,GU16 *pusHrvRRVal4, GU8 *puchHrvConfidentLvl, GU8 *puchHrvcnt,GU16 *pusSpo2RVal, GS32 *pliSpo2InvalidFlag, GU8 *puchWearingState)                       
{
    GU8 uchValidLvl;
    GU16 usRawdataOutLen = 0;
    return HBD_Spo2CalculateByFifoIntDbgRawdataInnerUse(stGsAxisValue, usGsDataNum, emGsensorSensitivity, \
                                                        pusSpo2Value, puchSpo2ConfidentLvl, puchHbValue, puchHbConfidentLvl, \
                                                        pusHrvRRVal1, pusHrvRRVal2, pusHrvRRVal3, pusHrvRRVal4, puchHrvConfidentLvl, \
                                                        puchHrvcnt, pusSpo2RVal, pliSpo2InvalidFlag, puchWearingState, HBD_PTR_NULL, &usRawdataOutLen, &uchValidLvl);
}
#endif

/****************************************************************
* Description: calculate SpO2_value by fifo int.
* Input:  stGsAxisValue: gsensor data buffer, if want get algo static state value, could set NULL.
          usGsDataNum: gsensor data count, if want get algo static state value, could set less thah 100.
          emGsensorSensitivity: G-sensor sensitivity (counts/g), see EM_HBD_GSENSOR_SENSITIVITY
* Output:     pusSpo2Value : Spo2 Value
            puchSpo2ConfidentLvl: Spo2 Confident Level
            puchHbValue  : HB Value
            puchHbConfidentLvl: HB Confident Level
            pusHrvRRVal1 : HRV RRI value 1
            pusHrvRRVal2 : HRV RRI value 2
            pusHrvRRVal3 : HRV RRI value 3
            pusHrvRRVal4 : HRV RRI value 4
            puchHrvConfidentLvl: HRV Confident Level
            puchHrvcnt : HRV valid cnt
            pusSpo2RVal :spo2 R value
            pliSpo2InvalidFlag:InvalidFlag
            puchWearingState : Wearing State
* Return:  refresh flag, if spo2 value have refresh that return 1.
******************************************************************/
#if 0
GU8 HBD_Spo2CalculateByFifoIntDbgRawdata(ST_GS_DATA_TYPE stGsAxisValue[], GU16 usGsDataNum, EM_HBD_GSENSOR_SENSITIVITY emGsensorSensitivity,
                                        GU8 *pusSpo2Value, GU8 *puchSpo2ConfidentLvl, GU8 *puchHbValue, GU8 *puchHbConfidentLvl,
                                        GU16 *pusHrvRRVal1,GU16 *pusHrvRRVal2,GU16 *pusHrvRRVal3,GU16 *pusHrvRRVal4, GU8 *puchHrvConfidentLvl,
                                        GU8 *puchHrvcnt,GU16 *pusSpo2RVal, GS32 *pliSpo2InvalidFlag, GU8 *puchWearingState, GS32 nRawdataOut[][6], GU16 *pusRawdataOutLen)
{
    GU8 uchValidLvl;
    return HBD_Spo2CalculateByFifoIntDbgRawdataInnerUse(stGsAxisValue, usGsDataNum, emGsensorSensitivity, \
                                                        pusSpo2Value, puchSpo2ConfidentLvl, puchHbValue, puchHbConfidentLvl, \
                                                        pusHrvRRVal1, pusHrvRRVal2, pusHrvRRVal3, pusHrvRRVal4, puchHrvConfidentLvl, \
                                                        puchHrvcnt, pusSpo2RVal,pliSpo2InvalidFlag, puchWearingState, nRawdataOut, pusRawdataOutLen, &uchValidLvl);
}
#endif

/****************************************************************
* Description: calculate SpO2_value by fifo int.
* Input:  stGsAxisValue: gsensor data buffer, if want get algo static state value, could set NULL.
          usGsDataNum: gsensor data count, if want get algo static state value, could set less thah 100.
          emGsensorSensitivity: G-sensor sensitivity (counts/g), see EM_HBD_GSENSOR_SENSITIVITY
* Output:     pusSpo2Value : Spo2 Value
            puchSpo2ConfidentLvl: Spo2 Confident Level
            puchHbValue  : HB Value
            puchHbConfidentLvl: HB Confident Level
            pusHrvRRVal1 : HRV RRI value 1
            pusHrvRRVal2 : HRV RRI value 2
            pusHrvRRVal3 : HRV RRI value 3
            pusHrvRRVal4 : HRV RRI value 4
            puchHrvConfidentLvl: HRV Confident Level
            puchHrvcnt : HRV valid cnt
            pusSpo2RVal :spo2 R value
            pliSpo2InvalidFlag:InvalidFlag
            puchWearingState : Wearing State
* Return:  refresh flag, if spo2 value have refresh that return 1.
******************************************************************/
#if 0
GS8 HBD_Spo2CalculateByFifoIntDbgU8Rawdata(ST_GS_DATA_TYPE stGsAxisValue[], GU16 usGsDataNum, EM_HBD_GSENSOR_SENSITIVITY emGsensorSensitivity,
                                        GU8 *pusSpo2Value, GU8 *puchSpo2ConfidentLvl, GU8 *puchHbValue, GU8 *puchHbConfidentLvl,
                                        GU16 *pusHrvRRVal1,GU16 *pusHrvRRVal2,GU16 *pusHrvRRVal3,GU16 *pusHrvRRVal4, GU8 *puchHrvConfidentLvl,
                                        GU8 *puchHrvcnt,GU16 *pusSpo2RVal, GS32* pliSpo2InvalidFlag, GU8 *puchWearingState, GU8 uchRawdataOut[], GU16 *pusRawdataOutLen, GU8 uchNeedDbgGsData)
{
    GU8 uchRet = 0;
    if (g_uchHbdStarted == HBD_START_STATUS_SPO2_MODE)
    {
        GU8 uchValidLvl;
        ST_SPO2_CAL_PARAM stParam;
        memset(&stParam, 0, sizeof(ST_SPO2_CAL_PARAM));
        stParam.uchFifoMode = 1;
        stParam.uchU8Mode = 1;
        HBD_ConnectResult(&stParam, stGsAxisValue, usGsDataNum, emGsensorSensitivity, pusSpo2Value, puchSpo2ConfidentLvl, puchHbValue, puchHbConfidentLvl, \
                          pusHrvRRVal1, pusHrvRRVal2, pusHrvRRVal3, pusHrvRRVal4, puchHrvConfidentLvl, puchHrvcnt, pusSpo2RVal, pliSpo2InvalidFlag, uchRawdataOut, pusRawdataOutLen, &uchValidLvl);
        stParam.pnRawdataOut = uchRawdataOut;
        stParam.uchDbgPkgLen = 13;
        if (uchNeedDbgGsData == 0)
        {
            stParam.uchDbgPkgLen = 7;
        }

        //HBD_AutoLedCalcAccSTD(stGsAxisValue, usGsDataNum, &g_stAutoLedAccInfo, stParam.sGsensorRightShift);
        stParam.usRawdataFifoCnt = HBD_GetFifoRawdata(g_uchReadFifoBuffArr);
        stParam.fIndexInc = HBD_CalcGsensorFloatIndex(stParam.usRawdataFifoCnt, usGsDataNum);
        if (stParam.usRawdataFifoCnt > 0)
        {
            for (stParam.usIndex = 0; stParam.usIndex < stParam.usRawdataFifoCnt; stParam.usIndex++)
            {
                uchRet |= HBD_Spo2CalculateCore(&stParam);
            }

            if (g_uchAutoLedErrorFlag == 1)
            {
                *puchWearingState = 2;
            }
            else
            {
                *puchWearingState = 1;
            }
        }
    }
    return uchRet;
}
#endif

/****************************************************************
* Description: calculate SpO2_value by fifo int.
* Input:  stGsAxisValue: gsensor data buffer, if want get algo static state value, could set NULL.
          usGsDataNum: gsensor data count, if want get algo static state value, could set less thah 100.
          emGsensorSensitivity: G-sensor sensitivity (counts/g), see EM_HBD_GSENSOR_SENSITIVITY
* Output:     pusSpo2Value : Spo2 Value
            puchSpo2ConfidentLvl: Spo2 Confident Level
            puchHbValue  : HB Value
            puchHbConfidentLvl: HB Confident Level
            pusHrvRRVal1 : HRV RRI value 1
            pusHrvRRVal2 : HRV RRI value 2
            pusHrvRRVal3 : HRV RRI value 3
            pusHrvRRVal4 : HRV RRI value 4
            puchHrvConfidentLvl: HRV Confident Level
            puchHrvcnt : HRV valid cnt
            pusSpo2RVal :spo2 R value
            pliSpo2InvalidFlag:InvalidFlag
            puchWearingState : Wearing State
* Return:  refresh flag, if spo2 value have refresh that return 1.
******************************************************************/
#if 0
GS8 HBD_Spo2CalculateByFifoIntDbgRawdataInnerUse(ST_GS_DATA_TYPE stGsAxisValue[], GU16 usGsDataNum, EM_HBD_GSENSOR_SENSITIVITY emGsensorSensitivity,
                                                GU8 *pusSpo2Value, GU8 *puchSpo2ConfidentLvl, GU8 *puchHbValue, GU8 *puchHbConfidentLvl,
                                                GU16 *pusHrvRRVal1,GU16 *pusHrvRRVal2,GU16 *pusHrvRRVal3,GU16 *pusHrvRRVal4, GU8 *puchHrvConfidentLvl,
                                                GU8 *puchHrvcnt,GU16 *pusSpo2RVal, GS32 *pliSpo2InvalidFlag, GU8 *puchWearingState, GS32 nRawdataOut[][6], GU16 *pusRawdataOutLen, GU8 *puchValidLvl)
{
    GU8 uchRet = 0;
    if (g_uchHbdStarted == HBD_START_STATUS_SPO2_MODE)
    {
        ST_SPO2_CAL_PARAM stParam = {0};
        stParam.uchFifoMode = 1;
        HBD_ConnectResult(&stParam, stGsAxisValue, usGsDataNum, emGsensorSensitivity, pusSpo2Value, puchSpo2ConfidentLvl, puchHbValue, puchHbConfidentLvl, \
                          pusHrvRRVal1, pusHrvRRVal2, pusHrvRRVal3, pusHrvRRVal4, puchHrvConfidentLvl, puchHrvcnt, pusSpo2RVal, pliSpo2InvalidFlag, nRawdataOut, pusRawdataOutLen, puchValidLvl);

        //HBD_AutoLedCalcAccSTD(stGsAxisValue, usGsDataNum, &g_stAutoLedAccInfo, stParam.sGsensorRightShift);
        stParam.usRawdataFifoCnt = HBD_GetFifoRawdata(g_uchReadFifoBuffArr);
        stParam.fIndexInc = HBD_CalcGsensorFloatIndex(stParam.usRawdataFifoCnt, usGsDataNum);
        if (stParam.usRawdataFifoCnt > 0)
        {
            for (stParam.usIndex = 0; stParam.usIndex < stParam.usRawdataFifoCnt; stParam.usIndex++)
            {
                uchRet |= HBD_Spo2CalculateCore(&stParam);
            }

            if (g_uchAutoLedErrorFlag == 1)
            {
                *puchWearingState = 2;
            }
            else
            {
                *puchWearingState = 1;
            }
        }
    }
    return uchRet;
}
#endif
#if 0
GS8 HBD_HrSpo2CalculateByFifoIntEx(ST_GS_DATA_TYPE stGsAxisValue[], GU16 usGsDataNum, EM_HBD_GSENSOR_SENSITIVITY emGsensorSensitivity,
                                                GS32 nRawdataOut[][6], GU16 *pusRawdataOutLen, ST_SPO2_RES *pstSpo2Res)
{
    GU8 uchRet = 0;
    GU16 usDbgRawdataMaxLen = (*pusRawdataOutLen);
    *pusRawdataOutLen = 0;
    GU16 current = HBD_I2cReadReg(0x122);  //get current
    GU16 current0 = (GU16)((GF32)(current & 0x00FF)*100/255);
    GU16 current1 = (GU16)((GF32)((current & 0xFF00) >> 8)*100/255);
    if (g_uchHbdStarted == HBD_START_STATUS_SPO2_MODE)
    {
        GU16 usRawdataFifoCnt = 0;
        GU16 usIndex = 0;
        GF32 fIndexInc = 0.0f;
        GS32 nRawDataArr[10] = {0};
        //GS32 nSpO2Arr[13] = {0};

        GS16 sGsensorRightShift = (1 << emGsensorSensitivity);
        if (usDbgRawdataMaxLen == 0)
        {
            usDbgRawdataMaxLen = 0xFFFF;
        }

        //HBD_AutoLedCalcAccSTD(stGsAxisValue, usGsDataNum, &g_stAutoLedAccInfo, sGsensorRightShift);
        usRawdataFifoCnt = HBD_GetFifoRawdata(g_uchReadFifoBuffArr);
        fIndexInc = HBD_CalcGsensorFloatIndex(usRawdataFifoCnt, usGsDataNum);
        if (usRawdataFifoCnt > 0)
        {
            ST_ALGO_CALCULATE_INFO algo_calc_info = {0};
            ST_ALGO_RESULT algo_calc_result = {0};
            for (usIndex = 0; usIndex < usRawdataFifoCnt; usIndex++)
            {
                HBD_PackFifoRawdataWithGsensor(g_uchReadFifoBuffArr, usRawdataFifoCnt, usIndex,
                                           stGsAxisValue, usGsDataNum, fIndexInc, sGsensorRightShift,
                                           nRawDataArr, nRawdataOut, pusRawdataOutLen, usDbgRawdataMaxLen);
#ifndef HBD_TEST_NO_ALGO
                algo_calc_info.uchFrameID = nRawDataArr[5];
                algo_calc_info.nRawdata[0] = nRawDataArr[0] & 0xffffff;
                algo_calc_info.nRawdata[1] = nRawDataArr[1] & 0xffffff;
                algo_calc_info.nCurAdjFlag[0] = (nRawDataArr[0] >> 30) & 0x1;
                algo_calc_info.nCurAdjFlag[1] = (nRawDataArr[1] >> 30) & 0x1;
                memcpy(algo_calc_info.sAccData, &nRawDataArr[2], 3 * sizeof(GS32));
                algo_calc_info.unSleepFlag = 0;//g_sleep_flg
                algo_calc_info.led_current[0] = current0;
                algo_calc_info.led_current[1] = current1;
                HOOK_FUNC_CALL(g_pAlgoCalculateHookFunc, (GH3011_FUNC_OFFSET_SPO2, &algo_calc_info, &algo_calc_result));
#endif
                #if (ENABLE_SPO2_ABNORMAL_STATE)
                HBD_SpO2AbnormalStateCalc(uchRet, &algo_calc_result);
                #endif
                if(pstSpo2Res->uchSpo2 > 0 && pstSpo2Res->uchSpo2 < 70)
                {
                    pstSpo2Res->uchSpo2 = 70;
                    //nSpO2Arr[12] = 70*10000;
                }

                if(algo_calc_result.uchUpdateFlag == 1)
                {
                    uchRet = 1;
                    pstSpo2Res->uchSpo2 = (GS32)algo_calc_result.snResult[0];
                    pstSpo2Res->uchSpo2ValidLvl = (GS32)algo_calc_result.snResult[1];
                    pstSpo2Res->uchSpo2Confidence = (GU8)algo_calc_result.snResult[2];
                    pstSpo2Res->usSpo2RVal = (GU16)algo_calc_result.snResult[11];
                    pstSpo2Res->uchHbValue = (GU8)algo_calc_result.snResult[4];
                    pstSpo2Res->uchHbConfidentLvl = (GU8)algo_calc_result.snResult[5];
                    //EXAMPLE_DEBUG_LOG_L1("algo_calc_result.snResult[0] = %d %d\n\r",algo_calc_result.snResult[0], pstSpo2Res->uchSpo2);
                }
                pstSpo2Res->uliSpo2InvlaidFlag = (GS32)algo_calc_result.snResult[6];

                //EXAMPLE_DEBUG_LOG_L1("HBD_HrSpo2CalculateByFifoIntEx uchSpo2 = %d rawdata = %d, %d , %d\n\r", pstSpo2Res->uchSpo2,algo_calc_info.nRawdata[0],algo_calc_info.nRawdata[1],algo_calc_result.snResult[4]);

                /*led 0 rawdata handle */
                if(0 == g_uchCloseHBAlgo)
                {
                    nRawDataArr[7] = 0;//g_nReferenceHbValue
                    algo_calc_info.uchFrameID = nRawDataArr[5];
                    algo_calc_info.nRawdata[0] = nRawDataArr[0] & 0xffffff;
                    //memcpy(algo_calc_info.sAccData, &nRawDataArr[2], 3 * sizeof(GS32));
                    algo_calc_info.unSleepFlag = 0;//g_sleep_flg
                    algo_calc_info.led_current[0] = current0;
                    HOOK_FUNC_CALL(g_pAlgoCalculateHookFunc, (GH3011_FUNC_OFFSET_HR, &algo_calc_info, &algo_calc_result));
                    if((algo_calc_result.uchUpdateFlag & 0x01) != 0)
                    {
                        pstSpo2Res->uchHbValue = (GU8)algo_calc_result.snResult[0];
                        pstSpo2Res->uchHbConfidentLvl = (GU8)algo_calc_result.snResult[1];
                        //EXAMPLE_DEBUG_LOG_L1("HBD_HrSpo2CalculateByFifoIntEx uchHbValue = %d , uchHbConfidentLvl = %d [%d]\n\r", pstSpo2Res->uchSpo2, pstSpo2Res->uchHbConfidentLvl,algo_calc_result.snResult[0]);
                    }
                }

            }

            if (g_uchAutoLedErrorFlag == 1)
            {
                //EXAMPLE_DEBUG_LOG_L1("pstSpo2Res->uchWearingState = 2 /n/r");
                pstSpo2Res->uchWearingState = 2;
            }
            else
            {
                pstSpo2Res->uchWearingState = 1;
            }
        }
    }
    return uchRet;
}

#endif

/****************************************************************
* Description: calculate SpO2_value by fifo int.(new api)
* Input:    stGsAxisValue: gsensor data buffer, if want get algo static state value, could set NULL.
            usGsDataNum: gsensor data count, if want get algo static state value, could set less thah 100.
            emGsensorSensitivity: G-sensor sensitivity (counts/g), see EM_HBD_GSENSOR_SENSITIVITY
            pusRawdataOutLen : nRawdataOut length
* Output:   pstSpo2Res : spo2 result struct
            nRawdataOut : rawdata buf
            pusRawdataOutLen : RawdataOut real length
* Return:  refresh flag, if spo2 value have refresh that return 1.
******************************************************************/
#if 0
GS8 HBD_Spo2CalculateByFifoIntEx(ST_GS_DATA_TYPE stGsAxisValue[], GU16 usGsDataNum, EM_HBD_GSENSOR_SENSITIVITY emGsensorSensitivity,
                                                GS32 nRawdataOut[][6], GU16 *pusRawdataOutLen, ST_SPO2_RES *pstSpo2Res)
{
    GU8 uchRet = 0;
    GU16 usDbgRawdataMaxLen = (*pusRawdataOutLen);
    *pusRawdataOutLen = 0;
    if (g_uchHbdStarted == HBD_START_STATUS_SPO2_MODE)
    {
        GU16 usRawdataFifoCnt = 0;
        GU16 usIndex = 0;
        GF32 fIndexInc = 0.0f;
        GS32 nRawDataArr[10] = {0};
        //GS32 nSpO2Arr[13] = {0};

        GS16 sGsensorRightShift = (1 << emGsensorSensitivity);
        if (usDbgRawdataMaxLen == 0)
        {
            usDbgRawdataMaxLen = 0xFFFF;
        }

        //HBD_AutoLedCalcAccSTD(stGsAxisValue, usGsDataNum, &g_stAutoLedAccInfo, sGsensorRightShift);
        usRawdataFifoCnt = HBD_GetFifoRawdata(g_uchReadFifoBuffArr);
        fIndexInc = HBD_CalcGsensorFloatIndex(usRawdataFifoCnt, usGsDataNum);
        if (usRawdataFifoCnt > 0)
        {
            ST_ALGO_CALCULATE_INFO algo_calc_info = {0};
            ST_ALGO_RESULT algo_calc_result = {0};
            for (usIndex = 0; usIndex < usRawdataFifoCnt; usIndex++)
            {
                HBD_PackFifoRawdataWithGsensor(g_uchReadFifoBuffArr, usRawdataFifoCnt, usIndex,
                                           stGsAxisValue, usGsDataNum, fIndexInc, sGsensorRightShift,
                                           nRawDataArr, nRawdataOut, pusRawdataOutLen, usDbgRawdataMaxLen);
                #ifndef HBD_TEST_NO_ALGO
                algo_calc_info.uchFrameID = nRawDataArr[5];
                algo_calc_info.nRawdata[0] = nRawDataArr[0] & 0xffffff;
                algo_calc_info.nRawdata[1] = nRawDataArr[1] & 0xffffff;
                algo_calc_info.nCurAdjFlag[0] = (nRawDataArr[0] >> 30) & 0x1;
                algo_calc_info.nCurAdjFlag[1] = (nRawDataArr[1] >> 30) & 0x1;
                memcpy(algo_calc_info.sAccData, &nRawDataArr[2], 3 * sizeof(GS32));
                algo_calc_info.unSleepFlag = 0;//g_sleep_flg
                HOOK_FUNC_CALL(g_pAlgoCalculateHookFunc, (GH3011_FUNC_OFFSET_SPO2, &algo_calc_info, &algo_calc_result));
                #endif
                #if (ENABLE_SPO2_ABNORMAL_STATE)
                HBD_SpO2AbnormalStateCalc(uchRet, &algo_calc_result);
                #endif
                if(pstSpo2Res->uchSpo2 > 0 && pstSpo2Res->uchSpo2 < 70)
                {
                    pstSpo2Res->uchSpo2 = 70;
                    //nSpO2Arr[12] = 70*10000;
                }
                if(algo_calc_result.uchUpdateFlag == 1)
                {
                    pstSpo2Res->uchSpo2 = (GS32)algo_calc_result.snResult[0];
                    pstSpo2Res->uchSpo2ValidLvl = (GS32)algo_calc_result.snResult[1];
                    pstSpo2Res->uchSpo2Confidence = (GU8)algo_calc_result.snResult[2];
                    pstSpo2Res->usSpo2RVal = (GU16)algo_calc_result.snResult[11];
                    pstSpo2Res->uchHbValue = (GU8)algo_calc_result.snResult[4];
                    pstSpo2Res->uchHbConfidentLvl = (GU8)algo_calc_result.snResult[5];
                    //EXAMPLE_DEBUG_LOG_L1("algo_calc_result.snResult[0] = %d %d uchSpo2ValidLvl = %d\n\r",algo_calc_result.snResult[0], pstSpo2Res->uchSpo2,pstSpo2Res->uchSpo2ValidLvl);
                }
                pstSpo2Res->uliSpo2InvlaidFlag = (GS32)algo_calc_result.snResult[6];

                algo_calc_info.uchFrameID = nRawDataArr[5];
                algo_calc_info.nRawdata[0] = nRawDataArr[0];
                memcpy(algo_calc_info.sAccData, &nRawDataArr[2], 3 * sizeof(GS32));
                HOOK_FUNC_CALL(g_pAlgoCalculateHookFunc, (GH3011_FUNC_OFFSET_HRV, &algo_calc_info, &algo_calc_result));
                GU8 uchHrvAlgoRet = algo_calc_result.uchUpdateFlag;

                if (uchHrvAlgoRet)
                {
                    pstSpo2Res->usHrvRRVal[0] = (GU16)algo_calc_result.snResult[0];
                    pstSpo2Res->usHrvRRVal[1] = (GU16)algo_calc_result.snResult[1];
                    pstSpo2Res->usHrvRRVal[2] = (GU16)algo_calc_result.snResult[2];
                    pstSpo2Res->usHrvRRVal[3] = (GU16)algo_calc_result.snResult[3];
                    pstSpo2Res->uchHrvConfidentLvl = (GU8)algo_calc_result.snResult[4];
                    pstSpo2Res->uchHrvcnt = (GU8)algo_calc_result.snResult[5];
                }
            }

            if (g_uchAutoLedErrorFlag == 1)
            {
                pstSpo2Res->uchWearingState = 2;
            }
            else
            {
                pstSpo2Res->uchWearingState = 1;
            }
        }
    }
    return uchRet;
}
#endif
/****************************************************************
* Description: calculate SpO2_value by newdata int.
* Input:  stGsAxisValue: gsensor data, if want get algo static state value, could set NULL.
          emGsensorSensitivity: G-sensor sensitivity (counts/g), see EM_HBD_GSENSOR_SENSITIVITY
* Output:     pusSpo2Value : Spo2 Value
            puchSpo2ConfidentLvl: Spo2 Confident Level
            puchHbValue  : HB Value
            puchHbConfidentLvl: HB Confident Level
            pusHrvRRVal1 : HRV RRI value 1
            pusHrvRRVal2 : HRV RRI value 2
            pusHrvRRVal3 : HRV RRI value 3
            pusHrvRRVal4 : HRV RRI value 4
            puchHrvConfidentLvl: HRV Confident Level
            puchHrvcnt : HRV valid cnt
            pusSpo2RVal :spo2 R value
            puchWearingState : Wearing State
* Return:  refresh flag, if spo2 value have refresh that return 1.
******************************************************************/
#if 0
GS8 HBD_Spo2CalculateByNewdataInt(ST_GS_DATA_TYPE *stGsAxisValue, EM_HBD_GSENSOR_SENSITIVITY emGsensorSensitivity,  GU8 *pusSpo2Value, GU8 *puchSpo2ConfidentLvl, GU8 *puchHbValue, GU8 *puchHbConfidentLvl,
                                           GU16 *pusHrvRRVal1,GU16 *pusHrvRRVal2,GU16 *pusHrvRRVal3,GU16 *pusHrvRRVal4, GU8 *puchHrvConfidentLvl, GU8 *puchHrvcnt,GU16 *pusSpo2RVal, GS32 *pliSpo2InvalidFlag, GU8 *puchWearingState)
{
    GU8 uchRet = 0;
    if (g_uchHbdStarted == HBD_START_STATUS_SPO2_MODE)
    {
        GU8 uchValidLvl;
        GU16 usRawdataOutLen = 0;
        ST_SPO2_CAL_PARAM stParam = {0};

        HBD_ConnectResult(&stParam, HBD_PTR_NULL, 0, emGsensorSensitivity, pusSpo2Value, puchSpo2ConfidentLvl, puchHbValue, puchHbConfidentLvl, \
                          pusHrvRRVal1, pusHrvRRVal2, pusHrvRRVal3, pusHrvRRVal4, puchHrvConfidentLvl, puchHrvcnt, pusSpo2RVal, pliSpo2InvalidFlag, HBD_PTR_NULL, &usRawdataOutLen, &uchValidLvl);

        if (stGsAxisValue != HBD_PTR_NULL) // fix
        {
            stParam.nRawDataArr[2] = (GS32)(stGsAxisValue->sXAxisVal / stParam.sGsensorRightShift);
            stParam.nRawDataArr[3] = (GS32)(stGsAxisValue->sYAxisVal / stParam.sGsensorRightShift);
            stParam.nRawDataArr[4] = (GS32)(stGsAxisValue->sZAxisVal / stParam.sGsensorRightShift);
        }
        HBD_GetNewdataIntData(stParam.nRawDataArr);

        uchRet |= HBD_Spo2CalculateCore(&stParam);

        if (g_uchAutoLedErrorFlag == 1)
        {
            *puchWearingState = 2;
        }
        else
        {
            *puchWearingState = 1;
        }
    }
    return uchRet;
}
#endif

/****************************************************************
* Description: start adt confrim
* Input:  None
* Return: HBD_RET_OK=success,
          HBD_RET_LED_CONFIG_ALL_OFF_ERROR=all Led disable error,
          HBD_RET_NO_INITED_ERROR=not inited,
******************************************************************/
#if 0
GS8 HBD_AdtConfirmStart(void)
{
    GS8 chRet = HBD_RET_NO_INITED_ERROR;
    GU16 usRegVal = 0;
    if (g_uchNeedReadRawdataCount < 1)
    {
        return HBD_RET_LED_CONFIG_ALL_OFF_ERROR;
    }
    if (HBD_INIT_STATUS_INITED == g_uchHbdInited)
    {
        if (HBD_CheckChipInited() != 0) // check inited
        {
            GU16 usFifoEnableRegVal = 0x00;
            GU16 usIrqCtrlRegVal = ((g_usIrqPluseWidth - 1) << 8) | (1 << 5) /* idle state*/ | HBD_IRQ_MSK_ADC_VALID | HBD_IRQ_MSK_AUTO_LED_FAIL;

            HBD_ChangeToHbModeRegVal();

            usRegVal = HBD_I2cReadReg(0x00C4) * 2; // need * 2
            /* adt confrim algo reinit */
            GS32 nAdtConfirmAlgoParamBuffer[1] = {0};
            nAdtConfirmAlgoParamBuffer[0] = usRegVal;
            HOOK_FUNC_CALL(g_pAlgoInitHookFunc, (GH3011_FUNC_OFFSET_ADT_CONFIRM, 1, nAdtConfirmAlgoParamBuffer));

            HBD_AutoLedReset(HBD_START_STATUS_HB_MODE);
            g_uchPackageSN = 0;  // index clear

            if (g_emHbFifoEnableFlag == HBD_FUNCTIONAL_STATE_ENABLE)
            {
                usFifoEnableRegVal = 0x01;
                usIrqCtrlRegVal = (usIrqCtrlRegVal & HBD_IRQ_REG_IRQ_UNMSK)| HBD_IRQ_MSK_FIFO_THR_FULL | HBD_IRQ_MSK_FIFO_FULL | HBD_IRQ_MSK_AUTO_LED_FAIL;
            }

            HBD_I2cWriteReg(HBD_FIFO_EN_REG_ADDR, usFifoEnableRegVal);
            HBD_I2cWriteReg(HBD_FIFO_THR_REG_ADDR, g_stFifoThrConfigList[HBD_FIFO_THR_CONFIG_HB_INDEX].usFifoThrCntConfigVal * g_uchNeedReadRawdataCount);
            HBD_I2cWriteReg(HBD_IRQ_CTRL_REG_ADDR, usIrqCtrlRegVal);

            HBD_I2cSendCmd(HBD_CMD_SCAN);
            g_uchLastReadFifoCnt = 0;
            g_uchHbdStarted = HBD_START_STATUS_HB_MODE;
            chRet = HBD_RET_OK;
        }
        else
        {
            HBD_I2cSendCmd(HBD_CMD_SLEEP);
            HBD_DelayUs(HBD_SEND_CMD_DELAY_X_US);
            g_uchHbdInited = HBD_INIT_STATUS_NO_INIT;
        }
    }
    return chRet;
}
#endif

/****************************************************************
* Description: adt confirm detect by newdata int.
* Input:  stGsAxisValue: gsensor data
          emGsensorSensitivity: G-sensor sensitivity (counts/g), see EM_HBD_GSENSOR_SENSITIVITY
* Output:  None
* Return: wear flag, 0x00: calc ing 0x11:wear, 0x12:unwear
******************************************************************/
GU8 HBD_AdtConfirmCalculateByNewdataInt(ST_GS_DATA_TYPE *stGsAxisValue, EM_HBD_GSENSOR_SENSITIVITY emGsensorSensitivity)
{
    GU8 uchRet = 0;
    if (g_uchHbdStarted == HBD_START_STATUS_HB_MODE)
    {
        GS32 nRawDataArr[10] = {0};
        GS16 sGsensorRightShift = (1 << emGsensorSensitivity);

        if (stGsAxisValue != HBD_PTR_NULL) // fix
        {
            nRawDataArr[2] = (GS32)(stGsAxisValue->sXAxisVal / sGsensorRightShift);
            nRawDataArr[3] = (GS32)(stGsAxisValue->sYAxisVal / sGsensorRightShift);
            nRawDataArr[4] = (GS32)(stGsAxisValue->sZAxisVal / sGsensorRightShift);
        }
        HBD_GetNewdataIntData(nRawDataArr);
        ST_ALGO_CALCULATE_INFO algo_calc_info = {0};
        ST_ALGO_RESULT algo_calc_result = {0};
        #ifndef HBD_TEST_NO_ALGO
        algo_calc_info.nRawdata[0] = nRawDataArr[0] & 0xffffff;
        memcpy(algo_calc_info.sAccData, &nRawDataArr[2], 3 * sizeof(GS32));
        //HOOK_FUNC_CALL(g_pAlgoCalculateHookFunc, (GH3011_FUNC_OFFSET_ADT_CONFIRM, &algo_calc_info, &algo_calc_result));
        if(g_pAlgoCalculateHookFunc)
            g_pAlgoCalculateHookFunc(GH3011_FUNC_OFFSET_ADT_CONFIRM, &algo_calc_info, &algo_calc_result);
        //uchRet = ADTConfirmAlgoCalc(nRawDataArr); /// calc adt
        uchRet = algo_calc_result.snResult[0];
        #endif
    }

    return uchRet;
}

/****************************************************************
* Description: adt confirm detect by fifo int.
* Input:  stGsAxisValue: gsensor data buffer
             usGsDataNum: gsensor data count
             emGsensorSensitivity: G-sensor sensitivity (counts/g), see EM_HBD_GSENSOR_SENSITIVITY
* Output:   None
* Return: wear flag, 0x00: calc ing 0x11:wear, 0x12:unwear
******************************************************************/
#if 0
GU8 HBD_AdtConfirmCalculateByFifoInt(ST_GS_DATA_TYPE stGsAxisValue[], GU16 usGsDataNum, EM_HBD_GSENSOR_SENSITIVITY emGsensorSensitivity)
{
    GU8 uchRet = 0;
    if (g_uchHbdStarted == HBD_START_STATUS_HB_MODE)
    {
        GU16 usRawdataFifoCnt = 0;
        GF32 fIndexInc = 0.0f;
        GU16 usIndex = 0;
        GS32 nRawDataArr[10] = {0};
        GS16 sGsensorRightShift = (1 << emGsensorSensitivity);
        ST_ALGO_CALCULATE_INFO algo_calc_info = {0};
        ST_ALGO_RESULT algo_calc_result = {0};

        //HBD_AutoLedCalcAccSTD(stGsAxisValue, usGsDataNum, &g_stAutoLedAccInfo, sGsensorRightShift);
        usRawdataFifoCnt = HBD_GetFifoRawdata(g_uchReadFifoBuffArr);
        fIndexInc = HBD_CalcGsensorFloatIndex(usRawdataFifoCnt, usGsDataNum);
        if (usRawdataFifoCnt > 0)
        {
            for (usIndex = 0; usIndex < usRawdataFifoCnt; usIndex++)
            {
                HBD_PackFifoRawdataWithGsensor(g_uchReadFifoBuffArr, usRawdataFifoCnt, usIndex,
                                           stGsAxisValue, usGsDataNum, fIndexInc, sGsensorRightShift,
                                           nRawDataArr, HBD_PTR_NULL, HBD_PTR_NULL, 0xFFFF);

#ifndef HBD_TEST_NO_ALGO
               algo_calc_info.nRawdata[0] = nRawDataArr[0] & 0xffffff;
               memcpy(algo_calc_info.sAccData, &nRawDataArr[2], 3 * sizeof(GS32));
               HOOK_FUNC_CALL(g_pAlgoCalculateHookFunc, (GH3011_FUNC_OFFSET_ADT_CONFIRM, &algo_calc_info, &algo_calc_result));
               //uchRet = ADTConfirmAlgoCalc(nRawDataArr); /// calc adt
               uchRet = algo_calc_result.snResult[0];
#endif
                if ((uchRet == 0x11) || (uchRet == 0x12))
                {
                    break;
                }
            }
        }
    }

    return uchRet;
}
#endif
/****************************************************************
* Description: adt confirm detect by fifo int.
* Input:  stGsAxisValue: gsensor data buffer
             usGsDataNum: gsensor data count
             emGsensorSensitivity: G-sensor sensitivity (counts/g), see EM_HBD_GSENSOR_SENSITIVITY
* Output:   nRawdataOut: ptr of rawdata array output (ppg1, ppg2, gs-x, gs-y, gs-z, index)
            pusRawdataOutLen：ptr of rawdata len output
* Return: wear flag, 0x00: calc ing 0x11:wear, 0x12:unwear
******************************************************************/
#if 0
GU8 HBD_AdtConfirmCalculateByFifoIntDbgOutputData(ST_GS_DATA_TYPE stGsAxisValue[], GU16 usGsDataNum, EM_HBD_GSENSOR_SENSITIVITY emGsensorSensitivity,
                                                        GS32 nRawdataOut[][6], GU16 *pusRawdataOutLen)
{
    GU8 uchRet = 0;
    GU16 usDbgRawdataMaxLen = (*pusRawdataOutLen);
    *pusRawdataOutLen = 0;
    if (g_uchHbdStarted == HBD_START_STATUS_HB_MODE)
    {
        GU16 usRawdataFifoCnt = 0;
        GF32 fIndexInc = 0.0f;
        GU16 usIndex = 0;
        GS32 nRawDataArr[10] = {0};
        GS16 sGsensorRightShift = (1 << emGsensorSensitivity);
        ST_ALGO_CALCULATE_INFO algo_calc_info = {0};
        ST_ALGO_RESULT algo_calc_result = {0};
        if (usDbgRawdataMaxLen == 0)
        {
            usDbgRawdataMaxLen = 0xFFFF;
        }

        //HBD_AutoLedCalcAccSTD(stGsAxisValue, usGsDataNum, &g_stAutoLedAccInfo, sGsensorRightShift);
        usRawdataFifoCnt = HBD_GetFifoRawdata(g_uchReadFifoBuffArr);
        fIndexInc = HBD_CalcGsensorFloatIndex(usRawdataFifoCnt, usGsDataNum);
        if (usRawdataFifoCnt > 0)
        {
#if __DEBUG_ADT_CONFIRM__
            for (usIndex = 0; usIndex < usGsDataNum; usIndex++)
            {
                HBD_PackFifoRawdataWithGsensorAll(g_uchReadFifoBuffArr, usRawdataFifoCnt, usIndex,
                    stGsAxisValue, usGsDataNum, fIndexInc, sGsensorRightShift,
                    nRawDataArr, nRawdataOut, pusRawdataOutLen, usDbgRawdataMaxLen);
            }
#endif
            for (usIndex = 0; usIndex < usRawdataFifoCnt; usIndex++)
            {
#if __DEBUG_ADT_CONFIRM__
                HBD_PackFifoRawdataWithGsensor(g_uchReadFifoBuffArr, usRawdataFifoCnt, usIndex,
                                           stGsAxisValue, usGsDataNum, fIndexInc, sGsensorRightShift,
                                           nRawDataArr, HBD_PTR_NULL, 0, usDbgRawdataMaxLen);
#else
                HBD_PackFifoRawdataWithGsensor(g_uchReadFifoBuffArr, usRawdataFifoCnt, usIndex,
                                           stGsAxisValue, usGsDataNum, fIndexInc, sGsensorRightShift,
                                           nRawDataArr, nRawdataOut, pusRawdataOutLen, usDbgRawdataMaxLen);
#endif
#ifndef HBD_TEST_NO_ALGO
               algo_calc_info.nRawdata[0] = nRawDataArr[0] & 0xffffff;
               memcpy(algo_calc_info.sAccData, &nRawDataArr[2], 3 * sizeof(GS32));
               HOOK_FUNC_CALL(g_pAlgoCalculateHookFunc, (GH3011_FUNC_OFFSET_ADT_CONFIRM, &algo_calc_info, &algo_calc_result));
               //uchRet = ADTConfirmAlgoCalc(nRawDataArr); /// calc adt
               uchRet = algo_calc_result.snResult[0];
#endif

                if ((uchRet == 0x11) || (uchRet == 0x12))
                {
                    break;
                }
            }
        }
    }

    return uchRet;
}
#endif
/*****************************************************************************************
* Description: set LED current，can not set in adt mode and sleep mode
* Input:  fLed0Current:LED0 current
          fLed1Current:LED1 current
* Return: HBD_RET_PARAMETER_ERROR: out of range,and set current to the closest limit value
          HBD_RET_GENERIC_ERROR: can't set current in this mode
          HBD_RET_OK: set current success
******************************************************************************************/
GS8 HBD_SetLedCurrent(GF32 fLed0Current, GF32 fLed1Current)
{
    GU16 usLed0Current = 0;
    GU16 usLed1Current = 0;
    GU16 usMaxDriveCur = 0;
    GU16 usRegVal = 0;
    GS8 chRet = HBD_RET_OK;
    if (g_uchHbdStarted == HBD_START_STATUS_OFF)
    {
        HBD_I2cSendCmd(HBD_CMD_IDLE);
        HBD_DelayUs(HBD_SEND_CMD_DELAY_X_US);
    }
    if ((g_uchHbdStarted >=  HBD_START_STATUS_ADT_WEAR_MODE) || (0 == g_uchWrRegflag))
    {
        return HBD_RET_GENERIC_ERROR;
    }
    usMaxDriveCur = (HBD_I2cReadReg(HBD_LED_MAXDRIVECUR_REG_ADDR) >> 5) & 0x7;
    usMaxDriveCur = 20 + usMaxDriveCur * 20;
    if(fLed0Current > usMaxDriveCur || fLed0Current < 0.0f)
    {
        chRet = HBD_RET_PARAMETER_ERROR;
        fLed0Current = fLed0Current < 0.0f ? 0.0f : fLed0Current;
        fLed0Current = fLed0Current > usMaxDriveCur ? usMaxDriveCur : fLed0Current;
    }
    if(fLed1Current > usMaxDriveCur || fLed1Current < 0.0f)
    {
        chRet = HBD_RET_PARAMETER_ERROR;
        fLed1Current = fLed1Current < 0.0f ? 0.0f : fLed1Current;
        fLed1Current = fLed1Current > usMaxDriveCur ? usMaxDriveCur : fLed1Current;
    }
    usLed0Current = (GU16) fLed0Current * 255 / usMaxDriveCur;
    usLed1Current = (GU16) fLed1Current * 255 / usMaxDriveCur;
    usRegVal = (usLed1Current << 8) | usLed0Current;
    HBD_I2cWriteReg(HBD_LED12_REG_ADDR, usRegVal);
    return chRet;
}

/****************************************************************
* Description: get LED current
* output:  pLed0Current:ptr of LED0 current
          pLed1Current:ptr of LED1 current
* Return: HBD_RET_GENERIC_ERROR: can't get current in this mode
          HBD_RET_OK: get current success
******************************************************************/
GS8 HBD_GetLedCurrrent(GF32 *pLed0Current, GF32 *pLed1Current)
{
    GU16 usRegVal = 0;
    GU16 usMaxDriveCur = 0;
    if (g_uchHbdStarted == HBD_START_STATUS_OFF)
    {
        HBD_I2cSendCmd(HBD_CMD_IDLE);
        HBD_DelayUs(HBD_SEND_CMD_DELAY_X_US);
    }
    if ((g_uchHbdStarted >=  HBD_START_STATUS_ADT_WEAR_MODE) || (0 == g_uchWrRegflag))
    {
        return HBD_RET_GENERIC_ERROR;
    }
    usMaxDriveCur = (HBD_I2cReadReg(HBD_LED_MAXDRIVECUR_REG_ADDR) >> 5) & 0x7;
    usMaxDriveCur = 20 + usMaxDriveCur * 20;
    usRegVal = HBD_I2cReadReg(HBD_LED17_REG_ADDR);
    *pLed0Current = (GF32) (usRegVal & 0xFF) * usMaxDriveCur / 255;
    *pLed1Current = (GF32) ((usRegVal >> 8) & 0xFF) * usMaxDriveCur / 255;
    return HBD_RET_OK;
}

/****************************************************************
* Description: Start HBD mode for get rawdata
* Input:  GU16 usSampleRate : sample rate
*         GU8 ucEnableFifo : 1 enable fifo, 0 disable fifo
*         GU8 ucFifoTHR : Fifo int generate threshold
* Return: 0 success, others fail
******************************************************************/
GS8 HBD_StartHBDOnly(GU16 usSampleRate, GU8 ucEnableFifo, GU16 ucFifoTHR)
{
    GS8 chRet = HBD_RET_NO_INITED_ERROR;

    if (g_uchNeedReadRawdataCount < 1)
    {
        return HBD_RET_LED_CONFIG_ALL_OFF_ERROR;
    }
    if (HBD_INIT_STATUS_INITED == g_uchHbdInited)
    {
        if (HBD_CheckChipInited() != 0) // check inited
        {
            GU16 usFifoEnableRegVal = 0x00;
            GU16 usIrqCtrlRegVal = ((g_usIrqPluseWidth - 1) << 8) | (1 << 5) /* idle state*/ | HBD_IRQ_MSK_ADC_VALID | HBD_IRQ_MSK_AUTO_LED_FAIL;

            HBD_ChangeToHbModeRegVal();

            HBD_Mem_Free ();
            HBD_AutoLedReset(HBD_START_STATUS_GET_DATA_MODE);
            g_uchPackageSN = 0;  // index clear

            if (ucEnableFifo == HBD_FUNCTIONAL_STATE_ENABLE)
            {
                usFifoEnableRegVal = 0x01;
                usIrqCtrlRegVal = (usIrqCtrlRegVal & HBD_IRQ_REG_IRQ_UNMSK)| HBD_IRQ_MSK_FIFO_THR_FULL | HBD_IRQ_MSK_FIFO_FULL | HBD_IRQ_MSK_AUTO_LED_FAIL;
            }

            HBD_I2cWriteReg(HBD_SAMPLE_RATE_REG_ADDR, (GU16)(HBD_SAMPLE_RATE_CLOCK_FREQ/usSampleRate));
            HBD_I2cWriteReg(HBD_FIFO_EN_REG_ADDR, usFifoEnableRegVal);
            HBD_I2cWriteReg(HBD_FIFO_THR_REG_ADDR, ucFifoTHR * g_uchNeedReadRawdataCount);
            HBD_I2cWriteReg(HBD_IRQ_CTRL_REG_ADDR, usIrqCtrlRegVal);

            HBD_I2cSendCmd(HBD_CMD_SCAN);
            g_uchLastReadFifoCnt = 0;
            g_uchHbdStarted = HBD_START_STATUS_GET_DATA_MODE;
            chRet = HBD_RET_OK;
        }
        else
        {
            HBD_I2cSendCmd(HBD_CMD_SLEEP);
            HBD_DelayUs(HBD_SEND_CMD_DELAY_X_US);
            g_uchHbdInited = HBD_INIT_STATUS_NO_INIT;
        }
    }
    return chRet;
}

/****************************************************************
* Description: Get rawdata in fifo int
* Input:  GU8 ucBufLen : rawdata array length(frames), if ucBufLen < *pucRealDataLen value, data will be lost
* Output: GS32 nRawdataOut: ptr of rawdata array output
* Return: 0 OK, 1 soft autoled error
******************************************************************/
#if 0
GU8 HBD_GetRawdataByFifoInt(GU16 ucBufLen, GS32 nRawdataOut[][2], GU16 *pucRealDataLen)
{
    GU8 index = 0;
    GU16 ucRealDataLen = 0;

    ucRealDataLen = HBD_GetFifoRawdata(g_uchReadFifoBuffArr);
    for(;index<ucRealDataLen && index<ucBufLen; ++index)
    {
        if (g_uchNeedReadRawdataCount == 2)
        {
            nRawdataOut[index][0] = HBD_MAKEUP_DWORD(0x00, g_uchReadFifoBuffArr[index * 6], g_uchReadFifoBuffArr[index * 6 + 1], g_uchReadFifoBuffArr[index * 6 + 2]);
            nRawdataOut[index][1] = HBD_MAKEUP_DWORD(0x00, g_uchReadFifoBuffArr[index * 6 + 3], g_uchReadFifoBuffArr[index * 6 + 4], g_uchReadFifoBuffArr[index * 6 + 5]);      
        }
        else
        {
            nRawdataOut[index][0] = HBD_MAKEUP_DWORD(0x00, g_uchReadFifoBuffArr[index * 3], g_uchReadFifoBuffArr[index * 3 + 1], g_uchReadFifoBuffArr[index * 3 + 2]);
            nRawdataOut[index][1] = nRawdataOut[index][0];
        }
        nRawdataOut[index][0] = (nRawdataOut[index][0] << 7) & 0xFFFFFF80;
        nRawdataOut[index][1] = (nRawdataOut[index][1] << 7) & 0xFFFFFF80;

        GS32 nRawDataTransform = 0;
        nRawDataTransform = nRawdataOut[index][0] & 0xffffff;
        HBD_RawdataTransform((GU32*)&nRawDataTransform);
        nRawdataOut[index][0] = ((nRawdataOut[index][0] & (~0xffffff)) | nRawDataTransform);

        nRawDataTransform = nRawdataOut[index][1] & 0xffffff;
        HBD_RawdataTransform((GU32*)&nRawDataTransform);
        nRawdataOut[index][1] = ((nRawdataOut[index][1] & (~0xffffff)) | nRawDataTransform);
    }
    *pucRealDataLen = ucRealDataLen;

    return g_uchAutoLedErrorFlag;
}
#endif
/****************************************************************
* Description: Get rawdata in fifo int ,do not auto led
* Input:  GU8 ucBufLen : rawdata array length(frames), if ucBufLen < *pucRealDataLen value, data will be lost
* Output: GS32 nRawdataOut: ptr of rawdata array output
* Return: 0 OK
******************************************************************/
#if 0
GU8 HBD_GetRawdataByFifoIntWithoutAutoled(GU16 ucBufLen, GS32 nRawdataOut[][2], GU16 *pucRealDataLen)
{
    GU8 index = 0;
    GU16 ucRealDataLen = 0;

    ucRealDataLen = HBD_GetFifoRawdataWithoutAutoled(g_uchReadFifoBuffArr);
    for(;index<ucRealDataLen && index<ucBufLen; ++index)
    {
        if (g_uchNeedReadRawdataCount == 2)
        {
            nRawdataOut[index][0] = HBD_MAKEUP_DWORD(0x00, g_uchReadFifoBuffArr[index * 6], g_uchReadFifoBuffArr[index * 6 + 1], g_uchReadFifoBuffArr[index * 6 + 2]);
            nRawdataOut[index][1] = HBD_MAKEUP_DWORD(0x00, g_uchReadFifoBuffArr[index * 6 + 3], g_uchReadFifoBuffArr[index * 6 + 4], g_uchReadFifoBuffArr[index * 6 + 5]);      
        }
        else
        {
            nRawdataOut[index][0] = HBD_MAKEUP_DWORD(0x00, g_uchReadFifoBuffArr[index * 3], g_uchReadFifoBuffArr[index * 3 + 1], g_uchReadFifoBuffArr[index * 3 + 2]);
            nRawdataOut[index][1] = nRawdataOut[index][0];
        }
        nRawdataOut[index][0] = (nRawdataOut[index][0] << 7) & 0xFFFFFF80;
        nRawdataOut[index][1] = (nRawdataOut[index][1] << 7) & 0xFFFFFF80;

        GS32 nRawDataTransform = 0;
        nRawDataTransform = nRawdataOut[index][0] & 0xffffff;
        HBD_RawdataTransform((GU32*)&nRawDataTransform);
        nRawdataOut[index][0] = ((nRawdataOut[index][0] & (~0xffffff)) | nRawDataTransform);

        nRawDataTransform = nRawdataOut[index][1] & 0xffffff;
        HBD_RawdataTransform((GU32*)&nRawDataTransform);
        nRawdataOut[index][1] = ((nRawdataOut[index][1] & (~0xffffff)) | nRawDataTransform);
    }
    *pucRealDataLen = ucRealDataLen;

    return 0;
}
#endif
/****************************************************************
* Description: Get rawdata in new data int
* Output: GU32 *ppg1
*         GU32 *ppg2
* Return: 0 OK, 1 soft autoled error
******************************************************************/
GU8 HBD_GetRawdataByNewDataInt(GU32 *ppg1, GU32 *ppg2)
{
    GU32 unRawDataArr[2];

    *ppg1 = HBD_I2cReadRawdataReg(g_usReadRawdataRegList[0]);
    if (g_uchNeedReadRawdataCount == 2)
    {
        *ppg2 = HBD_I2cReadRawdataReg(g_usReadRawdataRegList[1]);
    }
    else
    {
        *ppg2 = *ppg1;
    }
    unRawDataArr[0] = *ppg1;
    unRawDataArr[1] = *ppg2;

    if (0 == HBD_AutoLedbyNewDataInt(unRawDataArr) || g_uchKeepLedWorking)
    {
        HBD_I2cSendCmd(HBD_CMD_RESUME);
    }
    else
    {
        HBD_I2cSendCmd(HBD_CMD_IDLE);// AutoLED Fail,set to idle
    }
    return g_uchAutoLedErrorFlag;
}

/*****************************************************************************************
* Description: must be called after HBD_GetRawdataByFifoInt or HBD_GetRawdataByNewDataInt
*
*****************************************************************************************/

void HBD_GetRawdataHasDone(void)
{
    if (g_uchAutoLedErrorFlag == 0  || g_uchKeepLedWorking)
    {
        HBD_I2cSendCmd(HBD_CMD_RESUME);
        g_uchWrRegflag = 0;
    }
    else
    {
        HBD_I2cSendCmd(HBD_CMD_IDLE);
        g_uchWrRegflag = 1;
    }
}

/****************************************************************
* Description: Use IR ppg to do a simple check wear state
* Input:  plRawdataBuff: rawdata
* Return: wear flag, 0x00: calc ing 0x11:wear, 0x12:unwear
******************************************************************/
#if 0
static GU8 HBD_IRConfirmAlgoCalc(GS32 *plRawdataBuff)
{
    GU8 uchRet = 0x00;
    GU32 ulPpgRawdataVal = (GU32)(plRawdataBuff[0] & 0x0001FFFF); // just use 17bits

    if (ulPpgRawdataVal > g_stAutoLedConfig.stAutoLedChannel[0].unTargetUpperThr || ulPpgRawdataVal < g_stAutoLedConfig.stAutoLedChannel[0].unTargetLowerThr)
    {
        g_uIRErrCnt++;
    }
    else
    {
        g_uIRErrCnt=0;
    }

    if (g_uIRErrCnt < 250)
    {
        uchRet = 0x11;
    }
    else
    {
        uchRet = 0x12;
    }
    return uchRet;
}
#endif
/****************************************************************
* Description: Use IR ppg to do a simple check wear state by fifo int.
* Input:  stGsAxisValue: gsensor data buffer
             usGsDataNum: gsensor data count
             emGsensorSensitivity: G-sensor sensitivity (counts/g), see EM_HBD_GSENSOR_SENSITIVITY
* Output:   nRawdataOut: ptr of rawdata array output (ppg1, ppg2, gs-x, gs-y, gs-z, index)
            pusRawdataOutLen：ptr of rawdata len output
* Return: wear flag, 0x00: calc ing 0x11:wear, 0x12:unwear
******************************************************************/
#if 0
GU8 HBD_IRSimpleWearCheckByFifoIntDbgOutputData(ST_GS_DATA_TYPE stGsAxisValue[], GU16 usGsDataNum, EM_HBD_GSENSOR_SENSITIVITY emGsensorSensitivity,
                                                    GS32 nRawdataOut[][6], GU16 *pusRawdataOutLen)
{
    GU8 uchRet = 0;
    GU16 usDbgRawdataMaxLen = (*pusRawdataOutLen);
    *pusRawdataOutLen = 0;
    if (g_uchHbdStarted == HBD_START_STATUS_HB_MODE)
    {
        GU16 usRawdataFifoCnt = 0;
        GF32 fIndexInc = 0.0f;
        GU16 usIndex = 0;
        GS32 nRawDataArr[10] = {0};
        GS16 sGsensorRightShift = (1 << emGsensorSensitivity);
        if (usDbgRawdataMaxLen == 0)
        {
            usDbgRawdataMaxLen = 0xFFFF;
        }

        //HBD_AutoLedCalcAccSTD(stGsAxisValue, usGsDataNum, &g_stAutoLedAccInfo, sGsensorRightShift);
        usRawdataFifoCnt = HBD_GetFifoRawdata(g_uchReadFifoBuffArr);
        fIndexInc = HBD_CalcGsensorFloatIndex(usRawdataFifoCnt, usGsDataNum);
        if (usRawdataFifoCnt > 0)
        {
            for (usIndex = 0; usIndex < usRawdataFifoCnt; usIndex++)
            {
                HBD_PackFifoRawdataWithGsensor(g_uchReadFifoBuffArr, usRawdataFifoCnt, usIndex,
                                           stGsAxisValue, usGsDataNum, fIndexInc, sGsensorRightShift,
                                           nRawDataArr, nRawdataOut, pusRawdataOutLen, usDbgRawdataMaxLen);

                #ifndef HBD_TEST_NO_ALGO
                uchRet = HBD_IRConfirmAlgoCalc(nRawDataArr);
                #endif
            }
            if(g_uchAutoLedErrorFlag == 1)
            {
                uchRet = 0x12;
            }
        }
    }

    return uchRet;
}
#endif
void Gh3011PoolIsNotEnough(void)
{
    //
}

// void HBAGetVersionEx(GU8 ver[])
// {
    // //
// }

/********END OF FILE********* Copyright (c) 2003 - 2020, Goodix Co., Ltd. ********/
