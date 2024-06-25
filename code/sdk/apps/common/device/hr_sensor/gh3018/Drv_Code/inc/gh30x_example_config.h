/**
 * @copyright (c) 2003 - 2024, Goodix Co., Ltd. All rights reserved.
 *
 * @file    gh30x_example_config.h
 *
 * @brief   example code for gh30x
 *
 */

#ifndef _GH30X_EXAMPLE_CONFIG_H_
#define _GH30X_EXAMPLE_CONFIG_H_


// application config

/* common */

#define PYH_LED0_COLOR  LED_COLOR_IR
#define PYH_LED1_COLOR  LED_COLOR_RED
#define PYH_LED2_COLOR  LED_COLOR_GREEN

/// gh30x communicate interface type: <1=> i2c, <2=> spi
#define __GH30X_COMMUNICATION_INTERFACE__   (GH30X_COMMUNICATION_INTERFACE_I2C)

#ifndef __GH30X_COMMUNICATION_INTERFACE__
#error __GH30X_COMMUNICATION_INTERFACE__ is undefine !!!
#endif

/// set irq pluse width to 255us
#define __GH30X_IRQ_PLUSE_WIDTH_CONFIG__    (1)

/// platform delay us config
#define __PLATFORM_DELAY_US_CONFIG__        (1)//(0)

/// gsensor sensitivity normalized
#define __GS_SENSITIVITY_CONFIG__          	(HBD_GSENSOR_SENSITIVITY_512_COUNTS_PER_G)

/// retry max cnt
#define __RETRY_MAX_CNT_CONFIG__            (3)

/// reinit max cnt
#define __RESET_REINIT_CNT_CONFIG__         (5)

/// frame data max cnt
#define GH30X_RAWDATA_FIFO_BUF_SAMPLE_POINT_NUM_MAX  60 // typical: 1.2s * 2 channel *25Hz


/*********************function config**************************/
#define __FUNC_TYPE_ADT_ENABLE__            (1)    /**< adt function tye */
#define __FUNC_TYPE_HR_ENABLE__             (1)    /**< hb function tye */
#define __FUNC_TYPE_HRV_ENABLE__            (1)    /**< hb function tye */
#define __FUNC_TYPE_SPO2_ENABLE__           (1)    /**< spo2 function tye */
#define __FUNC_TYPE_SOFT_ADT_ENABLE__       (1)    /**< support soft adt config */
#define __FUNC_TYPE_PROTOCOL_ENABLE__       (1)    /**< protocol function tye */
#define __FUNC_TYPE_AGC_ENABLE__            (1)    /**< AGC function tye */

/*********************function channel number config**************************/
#define GH30X_ADT_CHNL_NUM                  (1)    /**< ADT function channel num */
#define GH30X_HR_CHNL_NUM                   (1)    /**< HR function channel num */
#define GH30X_HRV_CHNL_NUM                  (1)    /**< HRV function channel num */
#define GH30X_SPO2_CHNL_NUM                 (2)    /**< SPO2 function channel num */
#define GH30X_SOFT_ADT_CHNL_NUM             (2)    /**< SOFT_ADT function channel num */
#define GH30X_PROTOCOL_CHNL_NUM             (2)    /**< PROTOCOL function channel num */

/*get rawdata only*/

/// get rawdata only support
#define __GET_RAWDATA_ONLY_SUPPORT__        (0)

/// get rawdata fifo thr cnt config
#define __GET_RAWDATA_FIFO_THR_CNT_CONFIG__ (25)

/// get rawdata only mode sample rate (Hz)
#define __GET_RAWDATA_SAMPLE_RATE__         (25)

/// get rawdata buffer length, max 255 (use heap space)
#define __GET_RAWDATA_BUF_LEN__             (__GET_RAWDATA_FIFO_THR_CNT_CONFIG__ + 5)

/// get rawdata but do not handle algo in hr_only and hr_spo2 mode
#define __GET_RAWDATA_WITHOUT_ALGO_HANDLE   (1)


/* ble */

/// if need use GOODIX app debug
#ifdef GOODIX_DEMO_PLANFORM
#define __USE_GOODIX_APP__                  (1)
#else
#define __USE_GOODIX_APP__                  (0)
#endif

/// if need mutli pkg, = (__NEW_DATA_MULTI_PKG_NUM__ + 1), so set 1 that send with two rawdata cmd
#define __NEW_DATA_MULTI_PKG_NUM__          (4) // mean 5

/// ble pkg size max suport, should < (mtu - 3)
#define __BLE_PKG_SIZE_MAX__                (180)


/* uart */

/// uart connect to goodix dongle or production test tools 
#define __UART_WITH_GOODIX_TOOLS__          (0)


/* system test */

/// system test moudle support
#ifdef GOODIX_DEMO_PLANFORM
#define __SYSTEM_TEST_SUPPORT__             (1)
#else
#define __SYSTEM_TEST_SUPPORT__             (0)
#endif

/// system test moudle cnt check noise, must <= 100
#define __SYSTEM_TEST_DATA_CNT_CONFIG__     (100)


/* dbg log lvl */

/// log debug lvl: <0=> off , <1=> normal info ,  <2=> with data info
#ifdef GOODIX_DEMO_PLANFORM
#define __EXAMPLE_DEBUG_LOG_LVL__           (1)
#else
#define __EXAMPLE_DEBUG_LOG_LVL__           (0)
#endif

/// log support len
#define __EXAMPLE_LOG_DEBUG_SUP_LEN__       (128)

/* double core enable */
#define __HBD_DOUBLE_CORE_ENABLE__          (0)

#define __HBD_API_EX__

/// factory detect support
#define __FACTORY_DET_SUPPORT__             (0)

#define FACTORY_GREEN_IR_SAMPLE_RATE        (25)
#define FACTORY_IR_RED_SAMPLE_RATE          (25)
#define FACTORY_GREEN_IR_FIFO_THR           (25)
#define FACTORY_IR_RED_FIFO_THR             (25)

/// adt detect support
#define __HARDWARE_ADT_DET_SUPPORT__        (1)

/* function algo enable config */
#define __HBD_HB_ALGORITHM_ENABLE__         (1)
#define __HBD_HRV_ALGORITHM_ENABLE__        (1)
#define __HBD_SPO2_ALGORITHM_ENABLE__       (1)
#define __HBD_NADT_ALGORITHM_ENABLE__       (1)

///external algo config
#define __HBD_ALGORITHM_EXTERNANL_CONFIG_ENABLE__    (0)

/// ble mcu mode pkg buffer len , minimum len = (DBG_MCU_PKG_RAW_FRAME_LEN * __ALGO_CALC_DBG_BUFFER_LEN__) + MCU_PKG_SPO2_ALGO_RESULT_LEN
#define __BLE_MCU_PKG_BUFFER_MAX_LEN__      (3000)

#if (HBD_CTRL_LIB_DEV_VERSION == HBD_CTRL_LIB_FOR_FINGER)
#define __HBD_CTR_WEAROFF_THRESHOLD__       (200)
#endif

#ifdef GH30X_FUNC_CHNL_NUM_MAX
#undef GH30X_FUNC_CHNL_NUM_MAX
#endif

#define GH30X_FUNC_CHNL_NUM_MAX 1

#if __FUNC_TYPE_ADT_ENABLE__
#if GH30X_ADT_CHNL_NUM > GH30X_FUNC_CHNL_NUM_MAX
#undef GH30X_FUNC_CHNL_NUM_MAX
#define GH30X_FUNC_CHNL_NUM_MAX GH30X_ADT_CHNL_NUM
#endif
#endif

#if __FUNC_TYPE_HR_ENABLE__
#if GH30X_HR_CHNL_NUM > GH30X_FUNC_CHNL_NUM_MAX
#undef GH30X_FUNC_CHNL_NUM_MAX
#define GH30X_FUNC_CHNL_NUM_MAX GH30X_HR_CHNL_NUM
#endif
#endif

#if __FUNC_TYPE_HRV_ENABLE__
#if GH30X_HRV_CHNL_NUM > GH30X_FUNC_CHNL_NUM_MAX
#undef GH30X_FUNC_CHNL_NUM_MAX
#define GH30X_FUNC_CHNL_NUM_MAX GH30X_HRV_CHNL_NUM
#endif
#endif

#if __FUNC_TYPE_SPO2_ENABLE__
#if GH30X_SPO2_CHNL_NUM > GH30X_FUNC_CHNL_NUM_MAX
#undef GH30X_FUNC_CHNL_NUM_MAX
#define GH30X_FUNC_CHNL_NUM_MAX GH30X_SPO2_CHNL_NUM
#endif
#endif

#if __FUNC_TYPE_SOFT_ADT_ENABLE__
#if GH30X_SOFT_ADT_CHNL_NUM > GH30X_FUNC_CHNL_NUM_MAX
#undef GH30X_FUNC_CHNL_NUM_MAX
#define GH30X_FUNC_CHNL_NUM_MAX GH30X_SOFT_ADT_CHNL_NUM
#endif
#endif

#if __FUNC_TYPE_PROTOCOL_ENABLE__
#if GH30X_PROTOCOL_CHNL_NUM > GH30X_FUNC_CHNL_NUM_MAX
#undef GH30X_FUNC_CHNL_NUM_MAX
#define GH30X_FUNC_CHNL_NUM_MAX GH30X_PROTOCOL_CHNL_NUM
#endif
#endif

#endif /* _GH30X_EXAMPLE_CONFIG_H_ */

/********END OF FILE********* Copyright (c) 2003 - 2024, Goodix Co., Ltd. ********/
