/**
 * @copyright (c) 2003 - 2020, Goodix Co., Ltd. All rights reserved.
 *
 * @file    gh3011_example_port.c
 *
 * @brief   example code for gh3011 (condensed  hbd_ctrl lib)
 *
 */
// #include "gpio.h"
// #include "asm/timer.h"
#include "../../sensor_iic/sensor_iic.h"
#include "../inc/gh30x_example_common.h"
#include "../inc/gh30x_example.h"
#include "../../Algo_Code/common/inc/gh30x_demo_algo_config.h"
#include "../../../../../ui/lv_watch/comm_sensor/sensor_hr_task.h"

static const int interrupt_io = \
	IO_PORTB_02;

#ifdef GOODIX_DEMO_PLANFORM
#include "low_power_manage.h"
#include "chip_cmd_pro.h"
#include "cmd_process.h"
#include "chip_cmd_pro.h"
#include "FreeRTOS.h"
#include "Semphr.h"
#include "cardiff.h"
#include "demo_app.h"
#include "cardiff.h"
#include "g_sensor_manager.h"
#include "slaver_log.h"
#include "iap_control.h"
#include "ui_control.h"
#include "gh30x_demo_goodix_planfrom.h"
#endif

#ifdef GOODIX_DEMO_PLANFORM
extern uint8_t gh30x_run_mode;
extern bool goodix_app_start_app_mode;
extern uint8_t gh30x_mcu_start_mode_get(void);
#endif

/* gh30x i2c interface */

/// i2c for gh30x init
void hal_gh30x_i2c_init(void)
{
#ifdef GOODIX_DEMO_PLANFORM
    // code implement by user
    Cardiff_I2C_init();
#endif
}

/// i2c for gh30x wrtie
uint8_t hal_gh30x_i2c_write(uint8_t device_id, const uint8_t write_buffer[], uint16_t length)
{
    uint8_t ret = GH30X_EXAMPLE_ERR_VAL;

    // code implement by user
#ifdef GOODIX_DEMO_PLANFORM
    CardiffI2cWriteFunc(device_id, write_buffer,length);
#endif

    u8 addr_len = 2;
    u8 data_len = length - 2;

    uint8_t slave_addr = (device_id >> 1);
    u16 addr_16bit = (u16)((write_buffer[0] << 8)|write_buffer[1]);

    u8 retry = 5;
    bool op_succ = \
        sensor_iic_u16addr_tx_buf(slave_addr, addr_16bit, &write_buffer[addr_len], data_len);
    while(retry && !op_succ)
    {
        op_succ = \
            sensor_iic_u16addr_tx_buf(slave_addr, addr_16bit, &write_buffer[addr_len], data_len);

        retry--;
    }

    if(op_succ)
        ret = GH30X_EXAMPLE_OK_VAL;

    return ret;
}

/// i2c for gh30x read
uint8_t hal_gh30x_i2c_read(uint8_t device_id, const uint8_t write_buffer[], uint16_t write_length, uint8_t read_buffer[], uint16_t read_length)
{
    uint8_t ret = GH30X_EXAMPLE_ERR_VAL;

    // code implement by user
#ifdef GOODIX_DEMO_PLANFORM
    CardiffI2cReadFunc(device_id, write_buffer, write_length, read_buffer, read_length);
#endif
    u8 data_len = read_length;
    u8 slave_addr = (device_id >> 1);
    u16 addr_16bit = (u16)((write_buffer[0] << 8)|write_buffer[1]);

    u8 retry = 5;
    bool op_succ = \
        sensor_iic_u16addr_rx_buf(slave_addr, addr_16bit, read_buffer, data_len);

    while(retry && !op_succ)
    {
        op_succ = \
            sensor_iic_u16addr_rx_buf(slave_addr, addr_16bit, read_buffer, data_len);

        retry--;
    }

    if(op_succ)
        ret = GH30X_EXAMPLE_OK_VAL;

    return ret;
}

/* gh30x spi interface */

/// spi for gh30x init
void hal_gh30x_spi_init(void)
{
    // code implement by user
#ifdef GOODIX_DEMO_PLANFORM
    CardiffSpiInit();
    CardiffSpiCsPinInit();
#endif
}

/// spi for gh30x wrtie
uint8_t hal_gh30x_spi_write(const uint8_t write_buffer[], uint16_t length)
{
    uint8_t ret = 1;

    // code implement by user
#ifdef GOODIX_DEMO_PLANFORM
    SpiWriteFunc((u8 *)write_buffer, length);
#endif

    return ret;
}

/// spi for gh30x read
uint8_t hal_gh30x_spi_read(uint8_t read_buffer[], uint16_t length)
{
    uint8_t ret = 1;

    // code implement by user
#ifdef GOODIX_DEMO_PLANFORM
    SpiReadFunc(read_buffer, length);
#endif

    return ret;
}

/// spi cs set low for gh30x
void hal_gh30x_spi_cs_set_low(void)
{
    // code implement by user
#ifdef GOODIX_DEMO_PLANFORM
    SpiCsCtrlFunc(0);
#endif
}

/// spi cs set high for gh30x
void hal_gh30x_spi_cs_set_high(void)
{
    // code implement by user
#ifdef GOODIX_DEMO_PLANFORM
    SpiCsCtrlFunc(1);
#endif
}

/* delay */

/// delay us
void hal_gh30x_delay_us(uint16_t us_cnt)
{
    // code implement by user
#ifdef GOODIX_DEMO_PLANFORM
    PlatformDelayUs(us_cnt);
#endif

    udelay(us_cnt);
}

/* gsensor driver */

/// gsensor motion detect mode flag
bool gsensor_drv_motion_det_mode = false;

/// gsensor init
int8_t gsensor_drv_init(void)
{
    int8_t ret = GH30X_EXAMPLE_OK_VAL;

    gsensor_drv_motion_det_mode = false;

    // code implement by user
    /* if enable all func equal 25Hz, should config > 25Hz;
    but if enable have 100hz, should config to > 100hz. if not, feeback to GOODIX!!!
    */
    //ret = gs_drv_init();

    return ret;
}

/// gsensor enter normal mode
void gsensor_drv_enter_normal_mode(void)
{
    // code implement by user
    gsensor_drv_motion_det_mode = false;
}

/// gsensor enter fifo mode
void gsensor_drv_enter_fifo_mode(void)
{
    // code implement by user
    gsensor_drv_motion_det_mode = false;
}

/// gsensor enter motion det mode
void gsensor_drv_enter_motion_det_mode(void)
{
    // code implement by user
    /* if enable motion det mode that call @ref hal_gsensor_drv_int1_handler when motion generate irq
        e.g. 1. (hardware) use gsensor motion detect module with reg config
             2. (software) gsensor enter normal mode, then define 30ms timer get gsensor rawdata,
                if now total acceleration(sqrt(x*x+y*y+z*z)) - last total acceleration >= 30 (0.05g @512Lsb/g) as motion
                generate that call @ref hal_gsensor_drv_int1_handler
    */
    //gs_drv_enter_motion_det_mode();
    gsensor_drv_motion_det_mode = true;
}

/// gsensor get fifo data
void gsensor_drv_get_fifo_data(ST_GS_DATA_TYPE gsensor_buffer[], uint16_t *gsensor_buffer_index, uint16_t gsensor_max_len)
{
    // code implement by user
#ifdef GOODIX_DEMO_PLANFORM
    GS_GetGsensorData((ST_GS_DATA_TYPE*)gsensor_buffer, gsensor_buffer_index);
#endif

    HrGsDataFifoRead(gsensor_buffer, gsensor_buffer_index);
    // for(u8 i = 0; i < *gsensor_buffer_index; i++)
    // {
    //     printf("%d:x = %d, y = %d, z = %d\n", i, gsensor_buffer[i].sXAxisVal, \
    //         gsensor_buffer[i].sYAxisVal, gsensor_buffer[i].sZAxisVal);
    // }

    /**************************** WARNNING: DO NOT REMOVE OR MODIFY THIS CODE   ---START***************************************************/
    if ((*gsensor_buffer_index) > (gsensor_max_len))
    {
        EXAMPLE_DEBUG_LOG_L1("[%s]: Fatal error! Gsensor buffer is accessed overrun !", __FUNCTION__);
        while(1);   // Fatal error !!!
    }
    /**************************** WARNNING: DO NOT REMOVE OR MODIFY THIS CODE   ---END***************************************************/
}

/// gsensor clear buffer 
void gsensor_drv_clear_buffer(ST_GS_DATA_TYPE gsensor_buffer[], uint16_t *gsensor_buffer_index, uint16_t gsensor_buffer_len)
{
    if ((gsensor_buffer != NULL) && (gsensor_buffer_index != NULL))
    {
        memset(gsensor_buffer, 0, sizeof(ST_GS_DATA_TYPE) * gsensor_buffer_len);
        *gsensor_buffer_index = 0;
    }
}

/// gsensor get data
void gsensor_drv_get_data(ST_GS_DATA_TYPE *gsensor_data_ptr)
{
	/* This interface is only used in newdata mode,
	    and only one XYZ axis array is obtained each time
	*/
	// code implement by user
}

/* interrupt handler */
void hal_gh30x_int_handler_top_half(void)
{
    g_uchNewIntFlag++;
}

/* thread handler */
void hal_gh30x_int_handler_bottom_half(void)
{
    gh30x_int_msg_handler();
}

/// gh30x int pin init, should config as rise edge trigger
static void hal_gh30x_int_handle(void)
{
    hal_gh30x_int_handler_top_half();

	int SensorHrMsg[1];
	SensorHrMsg[0] = \
		SensorHrProcess;
	PostSensorHrTaskMsg(SensorHrMsg, 1);

	return;
}

void hal_gh30x_int_init(void)
{
    // code implement by user
    // must register func hal_gh30x_int_handler as callback
#ifdef GOODIX_DEMO_PLANFORM
    CardiffIntPinInit();
    EnableIntInterrupt();
#endif

    /*配置中断*/ 
	gpio_set_pull_up(interrupt_io, 0);
	gpio_set_pull_down(interrupt_io, 1);
	gpio_set_direction(interrupt_io, 1);
	gpio_set_die(interrupt_io, 1);

	port_edge_wkup_set_callback_by_index(6, \
		hal_gh30x_int_handle);

    printf("%s\n", __func__);
}

/// gsensor int handler
void hal_gsensor_drv_int1_handler(void)
{
    // code implement by user
    if (gsensor_drv_motion_det_mode)
    {
        gsensor_motion_has_detect();
    }
    else
    {
        /* if using gsensor fifo mode, should get data by fifo int
         * e.g. gsensor_read_fifo_data();
         */
        gsensor_read_fifo_data();
    }
}

/// gsensor int1 init, should config as rise edge trigger
void hal_gsensor_int1_init(void)
{
    // code implement by user
    // must register func hal_gsensor_drv_int1_handler as callback

    /* if using gsensor fifo mode,
    and gsensor fifo depth is not enough to store 1 second data,
    please connect gsensor fifo interrupt to the host,
    or if using gsensor motion detect mode(e.g  motion interrupt response by 0.5G * 5counts),
    and implement this function to receive gsensor interrupt.
    */
    //hal_gsensor_int1_init2();
}

#if (__USE_GOODIX_APP__)
/// handle algo status update
void handle_algo_result_update(GU32 function_id, GU16 value)
{
    switch(function_id)
    {
        case GH30X_FUNCTION_HR:
        break;

        case GH30X_FUNCTION_HRV:
        break;

        case GH30X_FUNCTION_SPO2:
        break;

        default:
        break;
    }
}
#endif

/// handle wear status result
GU8 g_firstflag = 1;
GU32 g_MyselfGh30xDemoFuncMode = 0;
void handle_wear_status_result(uint8_t wearing_state_val, GU8 uchLivingFlag)
{
#if 0
    // code implement by user
    // if(wearing_state_val == WEAR_STATUS_WEAR)
    //     SetHrSensorWearStatus(true);
    // else if(wearing_state_val == WEAR_STATUS_UNWEAR)
    //     SetHrSensorWearStatus(false);
    // else
    //     SetHrSensorWearStatus(true);
    // if(uchLivingFlag == 1)
    // {
    //     if(wearing_state_val == WEAR_STATUS_WEAR)
    //         SetHrSensorWearStatus(true);
    //     else
    //         SetHrSensorWearStatus(false);
    // }else
    //     SetHrSensorWearStatus(false);

    // printf("%s:%d  %d\n", __func__, \
    //     uchLivingFlag, wearing_state_val);
#endif

    printf("wearing_state_val = %d\n", \
        wearing_state_val);

#if 0
    u8 SensorMode = GetHrSensorMode();
    if(wearing_state_val == WEAR_STATUS_UNWEAR)
    {
        SetHrSensorWearStatus(false);

        if(g_firstflag)			
		{
			g_firstflag = 0;
			g_MyselfGh30xDemoFuncMode = \
                g_unGh30xDemoFuncMode;		
		}
        
        if(SensorMode == SensorModeAuto)
        {
            switch (g_unGh30xDemoFuncMode)
            {
                case HR_WORK_MODE:
                {
                    gh30x_module_stop();
                    gh30x_module_start(GH30X_FUNCTION_ADT);
                }
                    break;

                case BO_WORK_MODE:
                {
                    gh30x_module_stop();
                    gh30x_module_start(GH30X_FUNCTION_ADT);
                }
                    break;

                default:
                    break;
            }
        }
    }else if(wearing_state_val == WEAR_STATUS_WEAR)
    {
        SetHrSensorWearStatus(true);

        if(g_firstflag)			
		{
			g_firstflag = 0;
			g_MyselfGh30xDemoFuncMode = \
                g_unGh30xDemoFuncMode;		
		}

        if(SensorMode == SensorModeAuto)
        {
            switch (g_MyselfGh30xDemoFuncMode)
            {
                case HR_WORK_MODE:
                {
                    gh30x_module_stop();
                    gh30x_module_start(HR_WORK_MODE);
                }
                    break;

                case BO_WORK_MODE:
                {
                    gh30x_module_stop();
                    gh30x_module_start(BO_WORK_MODE);
                }
                    break;

                default:
                    break;
            }
        } 
    }
#endif

#ifdef GOODIX_DEMO_PLANFORM
    if (WEAR_STATUS_UNWEAR == wearing_state_val)
    {
        EXAMPLE_DEBUG_LOG_L1("[handle_wear_status_result] wear off! 0x%x\r", g_unGh30xDemoFuncMode);
        if (g_unGh30xDemoFuncMode & (~GH30X_FUNCTION_ADT)) // if have some function excluding ADT, stop current function and restart adt
        {
            /********************* START: the code is optional, system will force sample without the code**************/
            gh30x_module_stop();

            if (false == goodix_app_start_app_mode)
            {
                gh30x_module_start(GH30X_FUNCTION_ADT);
            }
            else
            {
                gh30x_module_start(GH30X_FUNCTION_HR);
            }
            /********************* END: the code is optional, system will force sample without the code**************/
        }
    }
    else if (WEAR_STATUS_WEAR == wearing_state_val)
    {
        EXAMPLE_DEBUG_LOG_L1("[handle_wear_status_result] wear on! 0x%x\r", g_unGh30xDemoFuncMode);
        if (GH30X_FUNCTION_ADT == g_unGh30xDemoFuncMode)
        {
            if (RUN_MODE_HARDWARE_ADT_DET != gh30x_mcu_start_mode_get())
            {
                gh30x_module_stop();
            }

            switch (gh30x_mcu_start_mode_get())
            {
                case RUN_MODE_ADT_HB_DET:
                {
                    gh30x_module_start(GH30X_FUNCTION_SOFT_ADT | GH30X_FUNCTION_HR | GH30X_FUNCTION_ADT);
                }
                break;

                case RUN_MODE_HRV_DET:
                {
                    gh30x_module_start(GH30X_FUNCTION_SOFT_ADT | GH30X_FUNCTION_HRV | GH30X_FUNCTION_HR | GH30X_FUNCTION_ADT);
                }
                break;

                case RUN_MODE_SPO2_DET:
                {
                    gh30x_module_start(GH30X_FUNCTION_SPO2 | GH30X_FUNCTION_HRV | GH30X_FUNCTION_ADT);
                }
                break;

                default:
                break;
            }
        }
    }
#endif
}

#if (__SYSTEM_TEST_SUPPORT__)
/// handle wear status result: {0-11}, led_num: {0-2};
///test_res: <0=> ok , <1=> init err , <2=> order err , <3=> comm read err,<4=> comm write err,<5=> otp read err,<6=> ctr not pass
///<7=> rawdata not pass , <8=> noise not pass , <9=> leak not pass, <10=> leakratio not pass,<11=> resource error
void handle_system_test_result(uint8_t test_res, uint8_t led_num)
{
    // code implement by user
    EXAMPLE_DEBUG_LOG_L1("system test has complete,the result is %d,led is %d\n",test_res,led_num);
}

void handle_before_system_os_test(void)
{
    //code implement by user
    EXAMPLE_DEBUG_LOG_L1("begin new term test");
#ifdef GOODIX_DEMO_PLANFORM
    vTaskDelay(4000);
#endif
}
#endif

#if (__FACTORY_DET_SUPPORT__)
void handle_factory_mode_result(STGh30xFactoryRawdata* pstGh30xFactoryRawdata)
{
    EXAMPLE_DEBUG_LOG_L1("current 0x%x 0x%x 0x%x\r\n",pstGh30xFactoryRawdata->gre1_led_curr, pstGh30xFactoryRawdata->ir_led_curr, pstGh30xFactoryRawdata->red_led_curr);
    EXAMPLE_DEBUG_LOG_L1("green num %d, rawdata = ",pstGh30xFactoryRawdata->gre1_num);

    for (GU8 i = 0; i < pstGh30xFactoryRawdata->gre1_num; i++)
    {
        EXAMPLE_DEBUG_LOG_L1("%d,", pstGh30xFactoryRawdata->gre1Buf[i]);
    }
    EXAMPLE_DEBUG_LOG_L1("\r\n");

    EXAMPLE_DEBUG_LOG_L1("ir num %d, rawdata = ", pstGh30xFactoryRawdata->ir_num);
    for (GU8 i = 0; i < pstGh30xFactoryRawdata->ir_num; i++)
    {
        printf("%d,", pstGh30xFactoryRawdata->IrBuf[i]);
    }
    EXAMPLE_DEBUG_LOG_L1("\r\n");

    EXAMPLE_DEBUG_LOG_L1("red num %d, rawdata = ", pstGh30xFactoryRawdata->red_num);
    for (GU8 i = 0; i < pstGh30xFactoryRawdata->red_num; i++)
    {
        EXAMPLE_DEBUG_LOG_L1("%d,", pstGh30xFactoryRawdata->redBuf[i]);
    }
    EXAMPLE_DEBUG_LOG_L1("\r\n");

    //code implement by user
}
#endif

/* ble */
#if (__USE_GOODIX_APP__)
/// send string via through profile
uint8_t ble_module_send_data_via_gdcs(uint8_t string[], uint8_t length)
{
    uint8_t ret = GH30X_EXAMPLE_OK_VAL;

#ifdef GOODIX_DEMO_PLANFORM
    uint8_t buffer[0xFF] = {0};
    uint8_t *ble_ptr = &buffer[MUTLI_PKG_HEADER_LEN - 1];
    uint8_t len = length <= __BLE_PKG_SIZE_MAX__ ? length : __BLE_PKG_SIZE_MAX__;

    ble_ptr[0] = 0xAA;
    ble_ptr[1] = 0x11;
    ble_ptr[2] = 0xA2;
    ble_ptr[3] = len;
    memcpy(&ble_ptr[4], string, len);
    buffer[0] = MUTLI_PKG_MAGIC_0_VAL;
    buffer[1] = MUTLI_PKG_MAGIC_1_VAL;
    buffer[2] = len + 4;
    buffer[4 + len + MUTLI_PKG_HEADER_LEN - 1] = MUTLI_PKG_MAGIC_2_VAL;

    ret = UartDataTransfer(buffer, (len + MUTLI_PKG_HEADER_LEN + 4));
#endif

    return ret;
}

/// Recv data via through profile
void ble_module_recv_data_via_gdcs(uint8_t *data, uint8_t length)
{
    gh30x_app_cmd_parse(data, length);
}

/// Determine whether the APP command is a reset command
static bool gdcs_parse_is_reset_cmd(uint8_t *gdcs_rx_data, uint8_t gdcs_rx_len)
{
    bool cmd_flag = false;
    if ((gdcs_rx_len == 5) && (gdcs_rx_data[0] == 0xDA) && (gdcs_rx_data[1] == 0xAD) &&
        (gdcs_rx_data[2] == 0xEF) && (gdcs_rx_data[3] == 0xFE) && (gdcs_rx_data[4] == 0xA5))
    {
        cmd_flag = true;
    }
    return cmd_flag;
}

/// Determine whether an APP command is a custom command
static bool gdcs_parse_is_custom_cmd(uint8_t *gdcs_rx_data, uint8_t gdcs_rx_len)
{
    bool cmd_flag = false;
    if ((gdcs_rx_len == 5) && (gdcs_rx_data[0] == 0xDA) && (gdcs_rx_data[1] == 0xAD) &&
        (gdcs_rx_data[2] == 0xEF) && (gdcs_rx_data[3] == 0xFE) && (gdcs_rx_data[4] == 0xA6))
    {
        cmd_flag = true;
    }
    return cmd_flag;
}

/// Data processing sent by APP
static void gdcs_rx_data_msg_handler(uint8_t *p_event_data, uint16_t event_size)
{
#ifdef GOODIX_DEMO_PLANFORM
    if (gdcs_parse_is_reset_cmd(p_event_data, event_size))
    {
        NVIC_SystemReset();
    }
    else if(gdcs_parse_is_custom_cmd(p_event_data, event_size))
    {
    }
    else
    {
        ble_module_recv_data_via_gdcs(p_event_data, event_size);
    }
#else
    ble_module_recv_data_via_gdcs(p_event_data, event_size);
#endif
}

/// Analysis of data sent by APP
void ble_module_gdcs_rx_parse(uint8_t *rx_data, uint16_t rx_length)
{
    gdcs_rx_data_msg_handler(rx_data, rx_length);
}

//APP_TIMER_DEF(m_gdcs_repeat_timer_id);
extern uint32_t m_ble_gdcs_repeat_timer_tick;

/// ble repeat send data timer init
void ble_module_repeat_send_timer_init(void)
{
    // code implement by user
    // must register func ble_module_repeat_send_timer_handler as callback
	/* should setup 50ms timer and ble connect interval should < 100ms
	*/
#ifdef GOODIX_DEMO_PLANFORM
    GOODIX_PLANFROM_CREAT_ADT_CONFIRM_ENTITY();
#endif
}

/// ble repeat send data timer start
void ble_module_repeat_send_timer_start(void)
{
    // code implement by user
#ifdef GOODIX_DEMO_PLANFORM
	GOODIX_PLANFROM_START_TIMER_ENTITY();
#endif
}

/// ble repeat send data timer stop
void ble_module_repeat_send_timer_stop(void)
{
    // code implement by user
#ifdef GOODIX_DEMO_PLANFORM
    GOODIX_PLANFROM_STOP_TIMER_ENTITY();
#endif
}

/// ble repeat send data timer handler
void ble_module_repeat_send_timer_handler(void)
{
    send_mcu_rawdata_packet_repeat();
}

void gdcs_repeat_msg_handler(void *p_event_data, uint16_t event_size)
{
    ble_module_repeat_send_timer_handler();
}

void gdcs_repeat_timeout_handler(void * p_context)
{
    // code implement by user
    /* must call gdcs_repeat_msg_handler */
}

#endif

/* timer */

/// gh30x fifo int timeout timer handler
void hal_gh30x_fifo_int_timeout_timer_handler(void)
{
	gh30x_fifo_int_timeout_msg_handler();
}

/// gh30x fifo int timeout timer start
void hal_gh30x_fifo_int_timeout_timer_start(void)
{
    // code implement by user
}

/// gh30x fifo int timeout timer stop
void hal_gh30x_fifo_int_timeout_timer_stop(void)
{
    // code implement by user
}

/// gh30x fifo int timeout timer init
void hal_gh30x_fifo_int_timeout_timer_init(void)
{
	// code implement by user
	// must register func gh30x_fifo_int_timeout_timer_handler as callback
	/* should setup timer interval with fifo int freq(e.g. 1s fifo int setup 1080ms timer)
	*/
}

/* uart */
#if (__UART_WITH_GOODIX_TOOLS__)
/// recv data via uart
void uart_module_recv_data(uint8_t *data, uint8_t length)
{
    gh30x_uart_cmd_parse(data, length);
}

/// send value via uart
uint8_t uart_module_send_data(uint8_t string[], uint8_t length)
{
    uint8_t ret = GH30X_EXAMPLE_OK_VAL;

    // code implement by user
    return ret;
}
#endif

/* handle cmd with all ctrl cmd @ref EM_COMM_CMD_TYPE, cmd come from goodix APP */
void handle_goodix_communicate_cmd(EM_COMM_CMD_TYPE cmd_type)
{
}

/// print dbg log
void example_dbg_log(char *log_string)
{
    // code implement by user
#ifdef GOODIX_DEMO_PLANFORM
    SLAVER_LOG("%s",log_string);
#endif
    printf("%s\n", log_string);
}

#if (__USER_DYNAMIC_ALGO_MEM_EN__)
void *hal_gh30x_memory_malloc(GU32 size)
{
    printf("malloc %d Btyes\n", size);
#ifdef GOODIX_DEMO_PLANFORM
    return malloc(size);
#else
    // code implement by user
    //return NULL;
    return malloc(size);
#endif
}

void hal_gh30x_memory_free(void *pmem)
{
    printf("free mem\n");
#ifdef GOODIX_DEMO_PLANFORM
    free(pmem);
#else
    free(pmem);
    // code implement by user
#endif
}
#endif

/********END OF FILE********* Copyright (c) 2003 - 2020, Goodix Co., Ltd. ********/
