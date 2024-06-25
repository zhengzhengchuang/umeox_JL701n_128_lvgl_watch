/**
 * @copyright (c) 2003 - 2024, Goodix Co., Ltd. All rights reserved.
 *
 * @file    gh30x_example_comm_pkg.c
 *
 * @brief   example code for gh30x
 *
 */

#include "string.h"
#include "../inc/gh30x_example_common.h"

/***** HBD communication protocol code (V2.2). see below. ******/
//#warning "data packet version 2.2"
#define COMM_VERSION                        (0x22)
#define COMM_HEADER_LENGTH                  (2u)
#define COMM_CRC_LENGTH                     (1u)
#define COMM_RAW_DATA_PACKAGE_LEN_EX        (16u)
#define COMM_RAW_DATA_PACKET_HEADER_LENGTH  (1u)
#define COMM_PACKET_DATA_INDEX              (2u)
#define COMM_SENDDATA_FUNC_REGISTER_MAX     (3)

/* cmd id code */
#define COMM_CMD_NOP                        (0x00) /*<nop operation>*/
#define COMM_CMD_CTRL_ACK                   (0x01) /*<ctrl ack>*/
#define COMM_CMD_CHECK_STATUS               (0x02) /*<get status>*/
#define COMM_CMD_REG_RW                     (0x03) /*<register RW>*/
#define COMM_CMD_LOAD_CONFIG                (0x04) /*<load config array>*/
#define COMM_CMD_ADT_CONFIG_RW              (0x05) /*<adt config RW>*/
#define COMM_CMD_READ_OTP_REG               (0x07) /*<Read Otp Reg>*/
#define COMM_CMD_HB_PACKEGE_DATA_EX         (0x08) /*<raw data package, only output>*/
#define COMM_CMD_HBD_CTRL                   (0x0C) /*<hbd ctrl>*/
#define COMM_CMD_AUTO_LED_CONFIG            (0x0E) /*<soft autoled config>*/

/* macro of reg rw cmd. */
#define COMM_REG_WRITE_MODE                 (0x00)
#define COMM_REG_READ_MODE                  (0x01)
#define COMM_REG_MODE_AND_NUM_INDEX         (0u)
#define COMM_REG_ADDR_H_INDEX               (1u)
#define COMM_REG_ADDR_L_INDEX               (2u)
#define COMM_REG_DATA_INDEX                 (3u)
#define COMM_REG_HEADER_LENGTH              (3u)

/* macro of adt config rw cmd. */
#define COMM_ADT_CONFIG_WRITE_MODE          (0x00)
#define COMM_ADT_CONFIG_MODE                (0x01)

/* macro of app cmd. */
#define COMM_APP_CMD_PACKET_LEN             (17u)
#define COMM_CMD_APP_STATE                  (0x0A) /*<raw data package, only output>*/
#define COMM_CMD_RWDATA_STATE               (0x0B) /*<raw data package, only output>*/

/* comm translate key code. array from "GooidxIOTKeycodeList*/
const GU8 uchCommKeyCode[20] =
{
    0x47, 0x6F, 0x6F, 0x64, 0x69, 0x78, 0x49, 0x4F, 0x54, 0x4B, 0x65, 0x79, 0x63, 0x6F, 0x64, 0x65, 0x4C, 0x69, 0x73, 0x74,
};

const static GU8 g_uchCrc8TabArr[] = {
    0x00, 0x07, 0x0E, 0x09, 0x1C, 0x1B, 0x12, 0x15, 0x38, 0x3F, 0x36, 0x31, 0x24, 0x23, 0x2A, 0x2D,
    0x70, 0x77, 0x7E, 0x79, 0x6C, 0x6B, 0x62, 0x65, 0x48, 0x4F, 0x46, 0x41, 0x54, 0x53, 0x5A, 0x5D,
    0xE0, 0xE7, 0xEE, 0xE9, 0xFC, 0xFB, 0xF2, 0xF5, 0xD8, 0xDF, 0xD6, 0xD1, 0xC4, 0xC3, 0xCA, 0xCD,
    0x90, 0x97, 0x9E, 0x99, 0x8C, 0x8B, 0x82, 0x85, 0xA8, 0xAF, 0xA6, 0xA1, 0xB4, 0xB3, 0xBA, 0xBD,
    0xC7, 0xC0, 0xC9, 0xCE, 0xDB, 0xDC, 0xD5, 0xD2, 0xFF, 0xF8, 0xF1, 0xF6, 0xE3, 0xE4, 0xED, 0xEA,
    0xB7, 0xB0, 0xB9, 0xBE, 0xAB, 0xAC, 0xA5, 0xA2, 0x8F, 0x88, 0x81, 0x86, 0x93, 0x94, 0x9D, 0x9A,
    0x27, 0x20, 0x29, 0x2E, 0x3B, 0x3C, 0x35, 0x32, 0x1F, 0x18, 0x11, 0x16, 0x03, 0x04, 0x0D, 0x0A,
    0x57, 0x50, 0x59, 0x5E, 0x4B, 0x4C, 0x45, 0x42, 0x6F, 0x68, 0x61, 0x66, 0x73, 0x74, 0x7D, 0x7A,
    0x89, 0x8E, 0x87, 0x80, 0x95, 0x92, 0x9B, 0x9C, 0xB1, 0xB6, 0xBF, 0xB8, 0xAD, 0xAA, 0xA3, 0xA4,
    0xF9, 0xFE, 0xF7, 0xF0, 0xE5, 0xE2, 0xEB, 0xEC, 0xC1, 0xC6, 0xCF, 0xC8, 0xDD, 0xDA, 0xD3, 0xD4,
    0x69, 0x6E, 0x67, 0x60, 0x75, 0x72, 0x7B, 0x7C, 0x51, 0x56, 0x5F, 0x58, 0x4D, 0x4A, 0x43, 0x44,
    0x19, 0x1E, 0x17, 0x10, 0x05, 0x02, 0x0B, 0x0C, 0x21, 0x26, 0x2F, 0x28, 0x3D, 0x3A, 0x33, 0x34,
    0x4E, 0x49, 0x40, 0x47, 0x52, 0x55, 0x5C, 0x5B, 0x76, 0x71, 0x78, 0x7F, 0x6A, 0x6D, 0x64, 0x63,
    0x3E, 0x39, 0x30, 0x37, 0x22, 0x25, 0x2C, 0x2B, 0x06, 0x01, 0x08, 0x0F, 0x1A, 0x1D, 0x14, 0x13,
    0xAE, 0xA9, 0xA0, 0xA7, 0xB2, 0xB5, 0xBC, 0xBB, 0x96, 0x91, 0x98, 0x9F, 0x8A, 0x8D, 0x84, 0x83,
    0xDE, 0xD9, 0xD0, 0xD7, 0xC2, 0xC5, 0xCC, 0xCB, 0xE6, 0xE1, 0xE8, 0xEF, 0xFA, 0xFD, 0xF4, 0xF3,
};

const uint8_t comm_key_code_list[20] =
{
    0x47, 0x6F, 0x6F, 0x64, 0x69, 0x78, 0x49, 0x4F, 0x54, 0x4B, 0x65, 0x79, 0x63, 0x6F, 0x64, 0x65, 0x4C, 0x69, 0x73, 0x74,
};

const static uint8_t comm_packet_crc8_tab[] = {
    0x00, 0x07, 0x0E, 0x09, 0x1C, 0x1B, 0x12, 0x15, 0x38, 0x3F, 0x36, 0x31, 0x24, 0x23, 0x2A, 0x2D,
    0x70, 0x77, 0x7E, 0x79, 0x6C, 0x6B, 0x62, 0x65, 0x48, 0x4F, 0x46, 0x41, 0x54, 0x53, 0x5A, 0x5D,
    0xE0, 0xE7, 0xEE, 0xE9, 0xFC, 0xFB, 0xF2, 0xF5, 0xD8, 0xDF, 0xD6, 0xD1, 0xC4, 0xC3, 0xCA, 0xCD,
    0x90, 0x97, 0x9E, 0x99, 0x8C, 0x8B, 0x82, 0x85, 0xA8, 0xAF, 0xA6, 0xA1, 0xB4, 0xB3, 0xBA, 0xBD,
    0xC7, 0xC0, 0xC9, 0xCE, 0xDB, 0xDC, 0xD5, 0xD2, 0xFF, 0xF8, 0xF1, 0xF6, 0xE3, 0xE4, 0xED, 0xEA,
    0xB7, 0xB0, 0xB9, 0xBE, 0xAB, 0xAC, 0xA5, 0xA2, 0x8F, 0x88, 0x81, 0x86, 0x93, 0x94, 0x9D, 0x9A,
    0x27, 0x20, 0x29, 0x2E, 0x3B, 0x3C, 0x35, 0x32, 0x1F, 0x18, 0x11, 0x16, 0x03, 0x04, 0x0D, 0x0A,
    0x57, 0x50, 0x59, 0x5E, 0x4B, 0x4C, 0x45, 0x42, 0x6F, 0x68, 0x61, 0x66, 0x73, 0x74, 0x7D, 0x7A,
    0x89, 0x8E, 0x87, 0x80, 0x95, 0x92, 0x9B, 0x9C, 0xB1, 0xB6, 0xBF, 0xB8, 0xAD, 0xAA, 0xA3, 0xA4,
    0xF9, 0xFE, 0xF7, 0xF0, 0xE5, 0xE2, 0xEB, 0xEC, 0xC1, 0xC6, 0xCF, 0xC8, 0xDD, 0xDA, 0xD3, 0xD4,
    0x69, 0x6E, 0x67, 0x60, 0x75, 0x72, 0x7B, 0x7C, 0x51, 0x56, 0x5F, 0x58, 0x4D, 0x4A, 0x43, 0x44,
    0x19, 0x1E, 0x17, 0x10, 0x05, 0x02, 0x0B, 0x0C, 0x21, 0x26, 0x2F, 0x28, 0x3D, 0x3A, 0x33, 0x34,
    0x4E, 0x49, 0x40, 0x47, 0x52, 0x55, 0x5C, 0x5B, 0x76, 0x71, 0x78, 0x7F, 0x6A, 0x6D, 0x64, 0x63,
    0x3E, 0x39, 0x30, 0x37, 0x22, 0x25, 0x2C, 0x2B, 0x06, 0x01, 0x08, 0x0F, 0x1A, 0x1D, 0x14, 0x13,
    0xAE, 0xA9, 0xA0, 0xA7, 0xB2, 0xB5, 0xBC, 0xBB, 0x96, 0x91, 0x98, 0x9F, 0x8A, 0x8D, 0x84, 0x83,
    0xDE, 0xD9, 0xD0, 0xD7, 0xC2, 0xC5, 0xCC, 0xCB, 0xE6, 0xE1, 0xE8, 0xEF, 0xFA, 0xFD, 0xF4, 0xF3,
};

/* parse state. */
typedef enum
{
    COMM_PARSE_STATE_IDLE = 0,
    COMM_PARSE_STATE_GOT_CMD_AND_STATE = 1,
    COMM_PARSE_STATE_GOT_COMM_VERSION = 2,
    COMM_PARSE_STATE_GOT_RAW_DATA = 3,
    COMM_PARSE_STATE_DONE = 4,
    COMM_PARSE_STATE_ERR = 5,
} EM_COMM_PARSE_STATE;

typedef struct comm_status
{
    GU8 ubPacketCmdId;
    EM_COMM_PARSE_STATE ParseState;
    GU8 ubPacketRawData[20];
    GU8 ubPacketDataIndex;
    GU8 ubPacketDataLength;
} ST_COMM_STATUS;

typedef enum
{
    COMM_STATE_NO_ERR = 0x00,
    COMM_STATE_PROTOCOL_ERR = 0x01,
    COMM_STATE_HB_COMM_ERR = 0x02,
    COMM_STATE_GSENSOR_DATA_ERR = 0x03,
    COMM_STATE_HB_GSENSOR_DATA_ERR = 0x04,
    COMM_STATE_INVALID = 0x05,
} EM_COMM_STATE_CODE;

/* function ptr typedef */
typedef void (*pfnSendData)(GU8 uchDataBuff[], GU8 uchLen); // send data function typde

///  uart parse state
typedef enum 
{
    UART_PARSE_STATE_IDLE = 0,
    UART_PARSE_STATE_GOT_MAGIC_0 = 1,
    UART_PARSE_STATE_GOT_MAGIC_1 = 2,
    UART_PARSE_STATE_GOT_LEN = 3,
    UART_PARSE_STATE_GOT_DATA = 4,
} EM_UART_PARSE_STATE; 

static pfnSendData g_pSendDataFuncArr[COMM_SENDDATA_FUNC_REGISTER_MAX] = {HBD_PTR_NULL};      // send data function ptr array
static GU8 g_uchSendDataFuncRegisterIndex = 0;
static GU8 g_uchCommCurrentIndex = 0;
ST_COMM_STATUS stParseStatus; /* parse status. */

#if (HBD_CTRL_LIB_DEV_VERSION == HBD_CTRL_LIB_FOR_BAND)
static GU8 g_uchDeviceType = DEVICE_BAND; // device type
#elif (HBD_CTRL_LIB_DEV_VERSION == HBD_CTRL_LIB_FOR_EP)
static GU8 g_uchDeviceType = DEVICE_EP; // device type
#elif (HBD_CTRL_LIB_DEV_VERSION == HBD_CTRL_LIB_FOR_IN_EAR_EP)
static GU8 g_uchDeviceType = DEVICE_EP; // device type
#elif (HBD_CTRL_LIB_DEV_VERSION == HBD_CTRL_LIB_FOR_FINGER)
static GU8 g_uchDeviceType = DEVICE_FINGER; // device type
#endif

/// communicate type
#if (__USE_GOODIX_APP__)
int8_t ble_comm_type = COMM_TYPE_INVALID_VAL;
#endif
#if (__UART_WITH_GOODIX_TOOLS__)
int8_t uart_comm_type = COMM_TYPE_INVALID_VAL;
#endif

#if __USE_GOODIX_APP__
static uint8_t mcu_rawdata_buffer[__BLE_MCU_PKG_BUFFER_MAX_LEN__];
static uint16_t mcu_rawdata_buffer_len = 0;
static uint16_t mcu_rawdata_buffer_index = 0;
static uint8_t mcu_rawdata_pkg_index = 0;
static uint8_t mcu_algo_result_type = 0;
static uint8_t mcu_algo_result_len = 0;

GU32 g_unGh30xProtocolSendDataFunc;  //0: disable  >0: GH30X_FUNCTION_HR / GH30X_FUNCTION_HRV / GH30X_FUNCTION_SPO2
GU8 g_uchGh30xProtocolSendAlgoResult[GH30X_PROTOCOL_SEND_ALGO_RESULT_BYTE_NUM];
#endif

#if (__UART_WITH_GOODIX_TOOLS__)
/// buffer & state for uart
uint8_t uart_module_recv_data_buffer[MUTLI_PKG_UART_MAX_LEN];
uint16_t uart_module_recv_index = 0;
uint16_t uart_module_recv_len = 0;
EM_UART_PARSE_STATE uart_module_parse_state = UART_PARSE_STATE_IDLE;
#endif

/// gh30x comm pkg init
void gh30x_comm_pkg_init(void)
{
    #if (__USE_GOODIX_APP__)
    ble_comm_type = HBD_SetSendDataFunc(ble_module_send_pkg); // setup ble send data function
    #endif

    #if (__USE_GOODIX_APP__)
    ble_module_repeat_send_timer_init(); // init repeat send data timer
    #endif

    #if (__UART_WITH_GOODIX_TOOLS__)
    uart_comm_type = HBD_SetSendDataFunc(uart_module_send_pkg);  // setup uart send data function
    #endif
}

#if (__USE_GOODIX_APP__)
/// app rx data handler
void gh30x_app_cmd_parse(uint8_t *buffer, uint8_t length) 
{
    gh30x_communicate_parse_handler(ble_comm_type, buffer, length);
}

/// send app pkg
void ble_module_send_pkg(uint8_t string[], uint8_t length) // send value via through profile
{
#if (__NEW_DATA_MULTI_PKG_NUM__)
    static uint8_t ble_module_send_pkg_index = 0; /// send mutli pkg index
    static uint8_t ble_module_send_pkg_buffer[__BLE_PKG_SIZE_MAX__ + 1]; /// send mutli pkg buffer
	if (MUTLI_PKG_CMD_VERIFY(string[0])) // if pkg cmd equal rawdata cmd
    {
        uint8_t new_pkg_index_of_buffer = ble_module_send_pkg_index * (length + MUTLI_PKG_HEADER_LEN);
        // fix ble_module_send_pkg_buffer
        ble_module_send_pkg_buffer[new_pkg_index_of_buffer++] = MUTLI_PKG_MAGIC_0_VAL;
        ble_module_send_pkg_buffer[new_pkg_index_of_buffer++] = MUTLI_PKG_MAGIC_1_VAL;
        ble_module_send_pkg_buffer[new_pkg_index_of_buffer++] = length;
        memcpy(&ble_module_send_pkg_buffer[new_pkg_index_of_buffer], string, length);
        ble_module_send_pkg_buffer[new_pkg_index_of_buffer + length] = MUTLI_PKG_MAGIC_2_VAL;
        ble_module_send_pkg_index ++;

        if ((((ble_module_send_pkg_index + 1) * (length + MUTLI_PKG_HEADER_LEN)) > __BLE_PKG_SIZE_MAX__)
            || (ble_module_send_pkg_index > __NEW_DATA_MULTI_PKG_NUM__))
        {
            ble_module_send_data_via_gdcs(ble_module_send_pkg_buffer, ble_module_send_pkg_index * (length + MUTLI_PKG_HEADER_LEN));
            ble_module_send_pkg_index = 0;
        }
    }
    else
    {
        ble_module_send_data_via_gdcs(string, length);
        ble_module_send_pkg_index = 0;
    }
#else
    ble_module_send_data_via_gdcs(string, length);
#endif
}

/// send mcu rawdata pkg repeat
void send_mcu_rawdata_packet_repeat(void)
{
    uint16_t nRes;
    uint16_t gdcs_send_data_len = 0;
    uint8_t m_gdcs_repeat_buffer[__BLE_PKG_SIZE_MAX__];

    gdcs_send_data_len = comm_packaging_rawdata_packet(m_gdcs_repeat_buffer, __BLE_PKG_SIZE_MAX__, 
                    mcu_algo_result_type, mcu_rawdata_buffer_len, mcu_rawdata_pkg_index, mcu_algo_result_len,
                    &mcu_rawdata_buffer[mcu_rawdata_buffer_index], mcu_rawdata_buffer_len - mcu_rawdata_buffer_index);
    mcu_rawdata_buffer_index += (gdcs_send_data_len - DBG_MCU_PKG_HEADER_LEN); /*fixed sub packet_header(8)*/

    nRes = ble_module_send_data_via_gdcs(m_gdcs_repeat_buffer, gdcs_send_data_len);

    if (GH30X_EXAMPLE_OK_VAL == nRes)
    {
        mcu_rawdata_pkg_index++;
    }
    else
    {
        EXAMPLE_DEBUG_LOG_L1("ble send fail code: %d\r\n", nRes);
        mcu_rawdata_buffer_len = 0;
    }

    if (mcu_rawdata_buffer_len > mcu_rawdata_buffer_index)
    {
        ble_module_repeat_send_timer_start();
    } 
    else
    {
        ble_module_repeat_send_timer_stop();
        mcu_rawdata_buffer_len = 0;
        mcu_rawdata_buffer_index = 0;
        mcu_rawdata_pkg_index = 0;
        mcu_algo_result_type = 0;
        mcu_algo_result_len = 0;
    }
}

/// send mcu rawdata pkg start
void send_mcu_rawdata_packet_start(uint8_t algo_result_type, uint8_t algo_result[], uint8_t algo_result_len,
                                           GS32 rawdata_out[][DBG_MCU_MODE_PKG_LEN], uint8_t rawdata_out_len)
{
    uint8_t rawdata_out_i = 0;

    ble_module_repeat_send_timer_stop();

    memcpy(mcu_rawdata_buffer, algo_result, algo_result_len);
    for (rawdata_out_i = 0; rawdata_out_i < rawdata_out_len; rawdata_out_i++)
    {
        mcu_rawdata_buffer[algo_result_len + rawdata_out_i * DBG_MCU_PKG_RAW_FRAME_LEN + 0]  = GET_BYTE2_FROM_DWORD(rawdata_out[rawdata_out_i][0]);
        mcu_rawdata_buffer[algo_result_len + rawdata_out_i * DBG_MCU_PKG_RAW_FRAME_LEN + 1]  = GET_BYTE1_FROM_DWORD(rawdata_out[rawdata_out_i][0]);
        mcu_rawdata_buffer[algo_result_len + rawdata_out_i * DBG_MCU_PKG_RAW_FRAME_LEN + 2]  = GET_BYTE0_FROM_DWORD(rawdata_out[rawdata_out_i][0]);
        mcu_rawdata_buffer[algo_result_len + rawdata_out_i * DBG_MCU_PKG_RAW_FRAME_LEN + 3]  = GET_BYTE2_FROM_DWORD(rawdata_out[rawdata_out_i][1]);
        mcu_rawdata_buffer[algo_result_len + rawdata_out_i * DBG_MCU_PKG_RAW_FRAME_LEN + 4]  = GET_BYTE1_FROM_DWORD(rawdata_out[rawdata_out_i][1]);
        mcu_rawdata_buffer[algo_result_len + rawdata_out_i * DBG_MCU_PKG_RAW_FRAME_LEN + 5]  = GET_BYTE0_FROM_DWORD(rawdata_out[rawdata_out_i][1]);
        mcu_rawdata_buffer[algo_result_len + rawdata_out_i * DBG_MCU_PKG_RAW_FRAME_LEN + 6]  = GET_BYTE1_FROM_DWORD(rawdata_out[rawdata_out_i][2]);
        mcu_rawdata_buffer[algo_result_len + rawdata_out_i * DBG_MCU_PKG_RAW_FRAME_LEN + 7]  = GET_BYTE0_FROM_DWORD(rawdata_out[rawdata_out_i][2]);
        mcu_rawdata_buffer[algo_result_len + rawdata_out_i * DBG_MCU_PKG_RAW_FRAME_LEN + 8]  = GET_BYTE1_FROM_DWORD(rawdata_out[rawdata_out_i][3]);
        mcu_rawdata_buffer[algo_result_len + rawdata_out_i * DBG_MCU_PKG_RAW_FRAME_LEN + 9]  = GET_BYTE0_FROM_DWORD(rawdata_out[rawdata_out_i][3]);
        mcu_rawdata_buffer[algo_result_len + rawdata_out_i * DBG_MCU_PKG_RAW_FRAME_LEN + 10] = GET_BYTE1_FROM_DWORD(rawdata_out[rawdata_out_i][4]);
        mcu_rawdata_buffer[algo_result_len + rawdata_out_i * DBG_MCU_PKG_RAW_FRAME_LEN + 11] = GET_BYTE0_FROM_DWORD(rawdata_out[rawdata_out_i][4]);
        mcu_rawdata_buffer[algo_result_len + rawdata_out_i * DBG_MCU_PKG_RAW_FRAME_LEN + 12] = GET_BYTE0_FROM_DWORD(rawdata_out[rawdata_out_i][5]);
    }

    mcu_rawdata_buffer_len = (uint16_t)algo_result_len + (uint16_t)rawdata_out_len * DBG_MCU_PKG_RAW_FRAME_LEN;
    mcu_rawdata_buffer_index = 0;
    mcu_rawdata_pkg_index = 0;
    mcu_algo_result_type = algo_result_type;
    mcu_algo_result_len = algo_result_len;

    send_mcu_rawdata_packet_repeat();
}

/// send mcu hb mode wear status rawdata pkg
void send_mcu_hb_mode_wear_status_pkg(uint8_t wear_status, GS32 rawdata_dbg[][DBG_MCU_MODE_PKG_LEN], uint8_t rawdata_dbg_len)
{
    uint8_t algo_out_array[MCU_PKG_HB_ALGO_RESULT_LEN] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

    algo_out_array[1] = wear_status;
    algo_out_array[5] = GH30X_EXAMPLE_OK_VAL;
    send_mcu_rawdata_packet_start(MCU_PKG_HB_TYPE_VAL, algo_out_array, MCU_PKG_HB_ALGO_RESULT_LEN, rawdata_dbg, rawdata_dbg_len);
}

void Gh30xCopyRawdataToBLeBuf(GU16 usFrameCnt,  const STGh30xFrameInfo *const pstFrameInfo)
{
    GU8 algo_result_offset = 0;

    if (NULL == pstFrameInfo)
    {
        EXAMPLE_DEBUG_LOG_L1("[Gh30xCopyRawdataToBLeBuf] ptr null!");
        return;
    }

    switch (g_unGh30xProtocolSendDataFunc)
    {
        case GH30X_FUNCTION_HR:
        {
            algo_result_offset = MCU_PKG_HB_ALGO_RESULT_LEN;
        }
        break;

        case GH30X_FUNCTION_HRV:
        {
            algo_result_offset = MCU_PKG_HRV_ALGO_RESULT_LEN;
        }
        break;

        case GH30X_FUNCTION_SPO2:
        {
            algo_result_offset = MCU_PKG_SPO2_ALGO_RESULT_LEN;
        }
        break;

        default:
        {
            // EXAMPLE_DEBUG_LOG_L2("[Gh30xCopyRawdataToBLeBuf] SendDataFunc:%d, FuncID:%d\r", g_unGh30xProtocolSendDataFunc, pstFrameInfo->unFunctionID);
        }
        break;
    }

    if ((GH30X_FUNCTION_PROTOCOL == pstFrameInfo->unFunctionID) && (0 != g_unGh30xProtocolSendDataFunc))
    {
        GU32 unRawdata[2];
        GU16 usAccX,usAccY,usAccZ;
        GU8  uchPackId;

        unRawdata[0] = pstFrameInfo->punRawdata[0] >> 7;
        unRawdata[1] = pstFrameInfo->punRawdata[1] >> 7;

        usAccX = pstFrameInfo->pusGsensordata[0];
        usAccY = pstFrameInfo->pusGsensordata[1];
        usAccZ = pstFrameInfo->pusGsensordata[2];

        uchPackId = pstFrameInfo->punFrameCnt[0] & 0x000000FF;

        mcu_rawdata_buffer[algo_result_offset + usFrameCnt * DBG_MCU_PKG_RAW_FRAME_LEN + 0]  = GET_BYTE2_FROM_DWORD(unRawdata[0]);
        mcu_rawdata_buffer[algo_result_offset + usFrameCnt * DBG_MCU_PKG_RAW_FRAME_LEN + 1]  = GET_BYTE1_FROM_DWORD(unRawdata[0]);
        mcu_rawdata_buffer[algo_result_offset + usFrameCnt * DBG_MCU_PKG_RAW_FRAME_LEN + 2]  = GET_BYTE0_FROM_DWORD(unRawdata[0]);
        mcu_rawdata_buffer[algo_result_offset + usFrameCnt * DBG_MCU_PKG_RAW_FRAME_LEN + 3]  = GET_BYTE2_FROM_DWORD(unRawdata[1]);
        mcu_rawdata_buffer[algo_result_offset + usFrameCnt * DBG_MCU_PKG_RAW_FRAME_LEN + 4]  = GET_BYTE1_FROM_DWORD(unRawdata[1]);
        mcu_rawdata_buffer[algo_result_offset + usFrameCnt * DBG_MCU_PKG_RAW_FRAME_LEN + 5]  = GET_BYTE0_FROM_DWORD(unRawdata[1]);
        mcu_rawdata_buffer[algo_result_offset + usFrameCnt * DBG_MCU_PKG_RAW_FRAME_LEN + 6]  = GET_BYTE1_FROM_DWORD(usAccX);
        mcu_rawdata_buffer[algo_result_offset + usFrameCnt * DBG_MCU_PKG_RAW_FRAME_LEN + 7]  = GET_BYTE0_FROM_DWORD(usAccX);
        mcu_rawdata_buffer[algo_result_offset + usFrameCnt * DBG_MCU_PKG_RAW_FRAME_LEN + 8]  = GET_BYTE1_FROM_DWORD(usAccY);
        mcu_rawdata_buffer[algo_result_offset + usFrameCnt * DBG_MCU_PKG_RAW_FRAME_LEN + 9]  = GET_BYTE0_FROM_DWORD(usAccY);
        mcu_rawdata_buffer[algo_result_offset + usFrameCnt * DBG_MCU_PKG_RAW_FRAME_LEN + 10] = GET_BYTE1_FROM_DWORD(usAccZ);
        mcu_rawdata_buffer[algo_result_offset + usFrameCnt * DBG_MCU_PKG_RAW_FRAME_LEN + 11] = GET_BYTE0_FROM_DWORD(usAccZ);
        mcu_rawdata_buffer[algo_result_offset + usFrameCnt * DBG_MCU_PKG_RAW_FRAME_LEN + 12] = uchPackId;
    }
}

void Gh30xUpdateProtocolSendDataFunc(void)
{
    GU32 unLastSendDataFunc = g_unGh30xProtocolSendDataFunc;

    g_unGh30xProtocolSendDataFunc = 0;

    if (g_unGh30xDemoFuncMode & GH30X_FUNCTION_SPO2)
    {
        g_unGh30xProtocolSendDataFunc = GH30X_FUNCTION_SPO2;
        mcu_algo_result_len = MCU_PKG_SPO2_ALGO_RESULT_LEN;
    }
    else if (g_unGh30xDemoFuncMode & GH30X_FUNCTION_HRV)
    {
        g_unGh30xProtocolSendDataFunc = GH30X_FUNCTION_HRV;
        mcu_algo_result_len = MCU_PKG_HRV_ALGO_RESULT_LEN;
    }
    else if (g_unGh30xDemoFuncMode & GH30X_FUNCTION_HR)
    {
        g_unGh30xProtocolSendDataFunc = GH30X_FUNCTION_HR;
        mcu_algo_result_len = MCU_PKG_HB_ALGO_RESULT_LEN;
    }
    if (unLastSendDataFunc != g_unGh30xProtocolSendDataFunc)
    {
        memset(g_uchGh30xProtocolSendAlgoResult, 0, GH30X_PROTOCOL_SEND_ALGO_RESULT_BYTE_NUM);
    }
}

void GH30xRecordAlgoResult(const STGh30xFrameInfo *const pstFrameInfo)
{
    if ((GH30X_FUNCTION_HR == g_unGh30xProtocolSendDataFunc) && (GH30X_FUNCTION_PROTOCOL == pstFrameInfo->unFunctionID))
    {
        g_uchGh30xProtocolSendAlgoResult[1] = 0x01;
        g_uchGh30xProtocolSendAlgoResult[8] = pstFrameInfo->pchDrvCurrentCode[0];
        g_uchGh30xProtocolSendAlgoResult[9] = pstFrameInfo->pchDrvCurrentCode[1];

        return;
    }
    else if ((GH30X_FUNCTION_SPO2  == g_unGh30xProtocolSendDataFunc) && (GH30X_FUNCTION_PROTOCOL == pstFrameInfo->unFunctionID))
    {
        g_uchGh30xProtocolSendAlgoResult[18] = pstFrameInfo->pchDrvCurrentCode[0];
        g_uchGh30xProtocolSendAlgoResult[19] = pstFrameInfo->pchDrvCurrentCode[1];

        return;
    }
    else if((GH30X_FUNCTION_HRV == g_unGh30xProtocolSendDataFunc) && (GH30X_FUNCTION_PROTOCOL == pstFrameInfo->unFunctionID))
    {
        g_uchGh30xProtocolSendAlgoResult[11] = pstFrameInfo->pchDrvCurrentCode[0];
        g_uchGh30xProtocolSendAlgoResult[12] = pstFrameInfo->pchDrvCurrentCode[1];

        return;
    }

    if ((pstFrameInfo->pstAlgoResult->uchUpdateFlag) && (0 != g_unGh30xProtocolSendDataFunc))
    {
        if ((GH30X_FUNCTION_SPO2 == g_unGh30xProtocolSendDataFunc) && (GH30X_FUNCTION_SPO2 == pstFrameInfo->unFunctionID))
        {
            g_uchGh30xProtocolSendAlgoResult[0] = pstFrameInfo->pstAlgoResult->snResult[0];   //(GS32)(GH3x2x_Round((float)stResult.final_spo2/10000)); //还可作为wear 标志 255代表 unwear
            g_uchGh30xProtocolSendAlgoResult[1] = pstFrameInfo->pstAlgoResult->snResult[2];   //(GS32)stResult.final_confi_coeff;
            g_uchGh30xProtocolSendAlgoResult[2] = pstFrameInfo->pstAlgoResult->snResult[4];   //(GS32)stResult.final_hb_mean;
            g_uchGh30xProtocolSendAlgoResult[3] = pstFrameInfo->pstAlgoResult->snResult[6];   //pstSpo2Res->uchHbConfidentLvl;
            g_uchGh30xProtocolSendAlgoResult[14] = GET_BYTE1_FROM_DWORD(pstFrameInfo->pstAlgoResult->snResult[1]);  //(GS32)stResult.final_r_val;
            g_uchGh30xProtocolSendAlgoResult[15] = GET_BYTE0_FROM_DWORD(pstFrameInfo->pstAlgoResult->snResult[1]);  //(GS32)stResult.final_r_val;
            g_uchGh30xProtocolSendAlgoResult[16] = pstFrameInfo->pstAlgoResult->snResult[3];  //(GS32)stResult.final_valid_level;
            g_uchGh30xProtocolSendAlgoResult[17] = 0; // abnormal_state
            g_uchGh30xProtocolSendAlgoResult[18] = pstFrameInfo->pchDrvCurrentCode[0];
            g_uchGh30xProtocolSendAlgoResult[19] = pstFrameInfo->pchDrvCurrentCode[1];
            //g_uchGh30xProtocolSendAlgoResult[20] = current[2];
            handle_algo_result_update(GH30X_FUNCTION_SPO2, (GU16)pstFrameInfo->pstAlgoResult->snResult[0]);
        }
        else if ((GH30X_FUNCTION_SPO2 == g_unGh30xProtocolSendDataFunc) && (GH30X_FUNCTION_HRV == pstFrameInfo->unFunctionID))
        {
            g_uchGh30xProtocolSendAlgoResult[4] = GET_HIGH_BYTE_FROM_WORD(pstFrameInfo->pstAlgoResult->snResult[0]);
            g_uchGh30xProtocolSendAlgoResult[5] = GET_LOW_BYTE_FROM_WORD(pstFrameInfo->pstAlgoResult->snResult[0]);
            g_uchGh30xProtocolSendAlgoResult[6] = GET_HIGH_BYTE_FROM_WORD(pstFrameInfo->pstAlgoResult->snResult[1]);
            g_uchGh30xProtocolSendAlgoResult[7] = GET_LOW_BYTE_FROM_WORD(pstFrameInfo->pstAlgoResult->snResult[1]);
            g_uchGh30xProtocolSendAlgoResult[8] = GET_HIGH_BYTE_FROM_WORD(pstFrameInfo->pstAlgoResult->snResult[2]);
            g_uchGh30xProtocolSendAlgoResult[9] = GET_LOW_BYTE_FROM_WORD(pstFrameInfo->pstAlgoResult->snResult[2]);
            g_uchGh30xProtocolSendAlgoResult[10] = GET_HIGH_BYTE_FROM_WORD(pstFrameInfo->pstAlgoResult->snResult[3]);
            g_uchGh30xProtocolSendAlgoResult[11] = GET_LOW_BYTE_FROM_WORD(pstFrameInfo->pstAlgoResult->snResult[3]);
            g_uchGh30xProtocolSendAlgoResult[12] = pstFrameInfo->pstAlgoResult->snResult[4];
            g_uchGh30xProtocolSendAlgoResult[13] = pstFrameInfo->pstAlgoResult->snResult[5];
        }

        if ((GH30X_FUNCTION_HRV == g_unGh30xProtocolSendDataFunc) && (GH30X_FUNCTION_HRV == pstFrameInfo->unFunctionID))
        {
            g_uchGh30xProtocolSendAlgoResult[0] = GET_BYTE1_FROM_DWORD(pstFrameInfo->pstAlgoResult->snResult[0]);   //stHrvResult.rri[0];
            g_uchGh30xProtocolSendAlgoResult[1] = GET_BYTE0_FROM_DWORD(pstFrameInfo->pstAlgoResult->snResult[0]);
            g_uchGh30xProtocolSendAlgoResult[2] = GET_BYTE1_FROM_DWORD(pstFrameInfo->pstAlgoResult->snResult[1]);   //stHrvResult.rri[1];
            g_uchGh30xProtocolSendAlgoResult[3] = GET_BYTE0_FROM_DWORD(pstFrameInfo->pstAlgoResult->snResult[1]);
            g_uchGh30xProtocolSendAlgoResult[4] = GET_BYTE1_FROM_DWORD(pstFrameInfo->pstAlgoResult->snResult[2]);   //stHrvResult.rri[2];
            g_uchGh30xProtocolSendAlgoResult[5] = GET_BYTE0_FROM_DWORD(pstFrameInfo->pstAlgoResult->snResult[2]);
            g_uchGh30xProtocolSendAlgoResult[6] = GET_BYTE1_FROM_DWORD(pstFrameInfo->pstAlgoResult->snResult[3]);   //stHrvResult.rri[3];
            g_uchGh30xProtocolSendAlgoResult[7] = GET_BYTE0_FROM_DWORD(pstFrameInfo->pstAlgoResult->snResult[3]);
            g_uchGh30xProtocolSendAlgoResult[8] = pstFrameInfo->pstAlgoResult->snResult[4]; //stHrvResult.rri_confidence;
            g_uchGh30xProtocolSendAlgoResult[9] = pstFrameInfo->pstAlgoResult->snResult[5]; //stHrvResult.rri_valid_num;
            //g_uchGh30xProtocolSendAlgoResult[10] = 0; //hb_val;
            g_uchGh30xProtocolSendAlgoResult[11] = pstFrameInfo->pchDrvCurrentCode[0];
            g_uchGh30xProtocolSendAlgoResult[12] = pstFrameInfo->pchDrvCurrentCode[1];
            //g_uchGh30xProtocolSendAlgoResult[13] = current[2];
            handle_algo_result_update(GH30X_FUNCTION_HRV, (GU16)pstFrameInfo->pstAlgoResult->snResult[0]);
        }
        else if ((GH30X_FUNCTION_HRV == g_unGh30xProtocolSendDataFunc) && (GH30X_FUNCTION_HR == pstFrameInfo->unFunctionID))
        {
            g_uchGh30xProtocolSendAlgoResult[10] = pstFrameInfo->pstAlgoResult->snResult[0]; //hb_val;
            handle_algo_result_update(GH30X_FUNCTION_HR, (GU16)pstFrameInfo->pstAlgoResult->snResult[0]);
        }

        if ((GH30X_FUNCTION_HR == g_unGh30xProtocolSendDataFunc) && (GH30X_FUNCTION_HR == pstFrameInfo->unFunctionID))
        {
            g_uchGh30xProtocolSendAlgoResult[0] = pstFrameInfo->pstAlgoResult->snResult[0];    // stResult.hba_out;
            //g_uchGh30xProtocolSendAlgoResult[1] = 1; //仅上报佩戴结果时，作为佩戴结果 WEAR_STATUS_UNWEAR或 WEAR_STATUS_WEAR
            //g_uchGh30xProtocolSendAlgoResult[2] = wearing_q;
            //g_uchGh30xProtocolSendAlgoResult[3] = vb_val;
            g_uchGh30xProtocolSendAlgoResult[4] = pstFrameInfo->pstAlgoResult->snResult[1];
            g_uchGh30xProtocolSendAlgoResult[5] = 1;     //仅上报佩戴结果时，为GH30X_EXAMPLE_OK_VAL
            //g_uchGh30xProtocolSendAlgoResult[6] = GET_HIGH_BYTE_FROM_WORD(rr_val);
            //g_uchGh30xProtocolSendAlgoResult[7] = GET_LOW_BYTE_FROM_WORD(rr_val);
            g_uchGh30xProtocolSendAlgoResult[8] = pstFrameInfo->pchDrvCurrentCode[0];
            g_uchGh30xProtocolSendAlgoResult[9] = pstFrameInfo->pchDrvCurrentCode[1];
            //g_uchGh30xProtocolSendAlgoResult[10] = current[2];
            //g_uchGh30xProtocolSendAlgoResult[11] = hb_scenario;
            //g_uchGh30xProtocolSendAlgoResult[12] = hb_snr;
            //g_uchGh30xProtocolSendAlgoResult[13] = motion_state;
            //g_uchGh30xProtocolSendAlgoResult[14] = sleep_flag;
            handle_algo_result_update(GH30X_FUNCTION_HR, (GU16)pstFrameInfo->pstAlgoResult->snResult[0]);
        }
        else if ((GH30X_FUNCTION_HR == g_unGh30xProtocolSendDataFunc) && (GH30X_FUNCTION_SOFT_ADT == pstFrameInfo->unFunctionID))
        {
            g_uchGh30xProtocolSendAlgoResult[1] = pstFrameInfo->pstAlgoResult->snResult[0];
            g_uchGh30xProtocolSendAlgoResult[2] = pstFrameInfo->pstAlgoResult->snResult[1];
        }
    }
}

/// send mcu rawdata pkg start
void Gh30xSendMcuRawdataPacketStart(uint8_t algo_result_type, uint8_t algo_result[], uint8_t algo_result_len, GU16 usFrameNum)
{
    ble_module_repeat_send_timer_stop();

    memcpy(mcu_rawdata_buffer, algo_result, algo_result_len);

    mcu_rawdata_buffer_len = (uint16_t)algo_result_len + (uint16_t)usFrameNum * DBG_MCU_PKG_RAW_FRAME_LEN;
    mcu_rawdata_buffer_index = 0;
    mcu_rawdata_pkg_index = 0;
    mcu_algo_result_type = algo_result_type;
    mcu_algo_result_len = algo_result_len;

    send_mcu_rawdata_packet_repeat();
}

void send_mcu_result_pkg(GU16 usFrameNum, const STGh30xFrameInfo *const pstFrameInfo)
{
    if ((0 != usFrameNum) && (0 != g_unGh30xProtocolSendDataFunc) && (NULL != pstFrameInfo) && (GH30X_FUNCTION_PROTOCOL == pstFrameInfo->unFunctionID))
    {
        if (GH30X_FUNCTION_HR == g_unGh30xProtocolSendDataFunc)
        {
            Gh30xSendMcuRawdataPacketStart(MCU_PKG_HB_TYPE_VAL, g_uchGh30xProtocolSendAlgoResult, MCU_PKG_HB_ALGO_RESULT_LEN, usFrameNum);
        }
        else if (GH30X_FUNCTION_SPO2 == g_unGh30xProtocolSendDataFunc)
        {
            Gh30xSendMcuRawdataPacketStart(MCU_PKG_SPO2_TYPE_VAL, g_uchGh30xProtocolSendAlgoResult, MCU_PKG_SPO2_ALGO_RESULT_LEN, usFrameNum);
        }
        else if (GH30X_FUNCTION_HRV == g_unGh30xProtocolSendDataFunc)
        {
            Gh30xSendMcuRawdataPacketStart(MCU_PKG_HRV_TYPE_VAL, g_uchGh30xProtocolSendAlgoResult, MCU_PKG_HRV_ALGO_RESULT_LEN, usFrameNum);
        }
    }
}

#ifdef __HBD_API_EX__
/// send mcu hb mode rawdata pkg
void send_mcu_hb_mode_result_pkg(uint8_t hb_val, uint8_t hb_lvl, uint8_t wear_status, uint8_t wearing_q, uint8_t vb_val, uint8_t clac_ret, uint16_t rr_val, uint8_t current[3],
                                        GS32 rawdata_dbg[][DBG_MCU_MODE_PKG_LEN], uint8_t rawdata_dbg_len,uint8_t hb_scenario, uint8_t hb_snr, uint8_t motion_state, uint8_t sleep_flag)
{
    uint8_t  algo_out_array[MCU_PKG_HB_ALGO_RESULT_LEN];
    algo_out_array[0] = hb_val;
    algo_out_array[1] = wear_status;
    algo_out_array[2] = wearing_q;
    algo_out_array[3] = vb_val;
    algo_out_array[4] = hb_lvl;
    algo_out_array[5] = clac_ret;
    algo_out_array[6] = GET_HIGH_BYTE_FROM_WORD(rr_val);
    algo_out_array[7] = GET_LOW_BYTE_FROM_WORD(rr_val);
    algo_out_array[8] = current[0];
    algo_out_array[9] = current[1];
    algo_out_array[10] = current[2];
    algo_out_array[11] = hb_scenario;
    algo_out_array[12] = hb_snr;
    algo_out_array[13] = motion_state;
    algo_out_array[14] = sleep_flag;
    send_mcu_rawdata_packet_start(MCU_PKG_HB_TYPE_VAL, algo_out_array, MCU_PKG_HB_ALGO_RESULT_LEN, rawdata_dbg, rawdata_dbg_len);
} 
#else
void send_mcu_hb_mode_result_pkg(uint8_t hb_val, uint8_t hb_lvl, uint8_t wear_status, uint8_t wearing_q, uint8_t vb_val, uint8_t clac_ret, uint16_t rr_val, uint8_t current[3],
                                        GS32 rawdata_dbg[][DBG_MCU_MODE_PKG_LEN], uint8_t rawdata_dbg_len)
{
    uint8_t  algo_out_array[MCU_PKG_HB_ALGO_RESULT_LEN];
    algo_out_array[0] = hb_val;
    algo_out_array[1] = wear_status;
    algo_out_array[2] = wearing_q;
    algo_out_array[3] = vb_val;
    algo_out_array[4] = hb_lvl;
    algo_out_array[5] = clac_ret;
    algo_out_array[6] = GET_HIGH_BYTE_FROM_WORD(rr_val);
    algo_out_array[7] = GET_LOW_BYTE_FROM_WORD(rr_val);
    algo_out_array[8] = current[0];
    algo_out_array[9] = current[1];
    algo_out_array[10] = current[2];
    algo_out_array[11] = 0;
    algo_out_array[12] = 0;
    algo_out_array[13] = 0;
    algo_out_array[14] = 0;
    send_mcu_rawdata_packet_start(MCU_PKG_HB_TYPE_VAL, algo_out_array, MCU_PKG_HB_ALGO_RESULT_LEN, rawdata_dbg, rawdata_dbg_len);
}
#endif

/// send mcu hrv mode rawdata pkg
void send_mcu_hrv_mode_result_pkg(uint16_t rr_val_arr[], uint8_t rr_cnt, uint8_t rr_lvl, uint8_t hb_val, uint8_t current[3], GS32 rawdata_dbg[][DBG_MCU_MODE_PKG_LEN], uint8_t rawdata_dbg_len)
{
    uint8_t algo_out_array[MCU_PKG_HRV_ALGO_RESULT_LEN];
    algo_out_array[0] = GET_HIGH_BYTE_FROM_WORD(rr_val_arr[0]);
    algo_out_array[1] = GET_LOW_BYTE_FROM_WORD(rr_val_arr[0]);
    algo_out_array[2] = GET_HIGH_BYTE_FROM_WORD(rr_val_arr[1]);
    algo_out_array[3] = GET_LOW_BYTE_FROM_WORD(rr_val_arr[1]);
    algo_out_array[4] = GET_HIGH_BYTE_FROM_WORD(rr_val_arr[2]);
    algo_out_array[5] = GET_LOW_BYTE_FROM_WORD(rr_val_arr[2]);
    algo_out_array[6] = GET_HIGH_BYTE_FROM_WORD(rr_val_arr[3]);
    algo_out_array[7] = GET_LOW_BYTE_FROM_WORD(rr_val_arr[3]);
    algo_out_array[8] = rr_lvl;
    algo_out_array[9] = rr_cnt;
    algo_out_array[10] = hb_val;
    algo_out_array[11] = current[0];
    algo_out_array[12] = current[1];
    algo_out_array[13] = current[2];
    send_mcu_rawdata_packet_start(MCU_PKG_HRV_TYPE_VAL, algo_out_array, MCU_PKG_HRV_ALGO_RESULT_LEN, rawdata_dbg, rawdata_dbg_len);
}

/// send mcu hb mode wear status rawdata pkg
void send_mcu_spo2_mode_unwear_pkg(GS32 rawdata_dbg[][DBG_MCU_MODE_PKG_LEN], uint8_t rawdata_dbg_len)
{
    uint8_t algo_out_array[MCU_PKG_SPO2_WEAR_RESULT_LEN] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    algo_out_array[0] = SPO2_MCU_UNWEAR_VAL;
    send_mcu_rawdata_packet_start(MCU_PKG_SPO2_TYPE_VAL, algo_out_array, MCU_PKG_SPO2_WEAR_RESULT_LEN, rawdata_dbg, rawdata_dbg_len);
}	

#ifdef __HBD_API_EX__
/// send mcu spo2 mode rawdata pkg
void send_mcu_spo2_mode_result_pkg(void *pstRes, uint8_t abnormal_state, uint8_t current[3], GS32 rawdata_dbg[][DBG_MCU_MODE_PKG_LEN], uint16_t rawdata_dbg_len)
{
#if (__SPO2_DET_SUPPORT__)
    ST_SPO2_RES *pstSpo2Res = pstRes;
    uint8_t algo_out_array[MCU_PKG_SPO2_ALGO_RESULT_LEN];
    algo_out_array[0] = pstSpo2Res->uchSpo2;
    algo_out_array[1] = pstSpo2Res->uchSpo2Confidence;
    algo_out_array[2] = pstSpo2Res->uchHbValue; 
    algo_out_array[3] = pstSpo2Res->uchHbConfidentLvl;
    algo_out_array[4] = GET_HIGH_BYTE_FROM_WORD(pstSpo2Res->usHrvRRVal[0]);
    algo_out_array[5] = GET_LOW_BYTE_FROM_WORD(pstSpo2Res->usHrvRRVal[0]);
    algo_out_array[6] = GET_HIGH_BYTE_FROM_WORD(pstSpo2Res->usHrvRRVal[1]);
    algo_out_array[7] = GET_LOW_BYTE_FROM_WORD(pstSpo2Res->usHrvRRVal[1]);
    algo_out_array[8] = GET_HIGH_BYTE_FROM_WORD(pstSpo2Res->usHrvRRVal[2]);
    algo_out_array[9] = GET_LOW_BYTE_FROM_WORD(pstSpo2Res->usHrvRRVal[2]);
    algo_out_array[10] = GET_HIGH_BYTE_FROM_WORD(pstSpo2Res->usHrvRRVal[3]);
    algo_out_array[11] = GET_LOW_BYTE_FROM_WORD(pstSpo2Res->usHrvRRVal[3]);
    algo_out_array[12] = pstSpo2Res->uchHrvConfidentLvl;
    algo_out_array[13] = pstSpo2Res->uchHrvcnt;	
    algo_out_array[14] = GET_HIGH_BYTE_FROM_WORD(pstSpo2Res->usSpo2RVal);
    algo_out_array[15] = GET_LOW_BYTE_FROM_WORD(pstSpo2Res->usSpo2RVal);
    algo_out_array[16] = pstSpo2Res->uchSpo2ValidLvl;
    algo_out_array[17] = abnormal_state;
    algo_out_array[18] = current[0];
    algo_out_array[19] = current[1];
    algo_out_array[20] = current[2];
    send_mcu_rawdata_packet_start(MCU_PKG_SPO2_TYPE_VAL, algo_out_array, MCU_PKG_SPO2_ALGO_RESULT_LEN, rawdata_dbg, rawdata_dbg_len);
#endif
}
#else
/// send mcu spo2 mode rawdata pkg
void send_mcu_spo2_mode_result_pkg(uint8_t spo2_val, uint8_t spo2_lvl_val, uint8_t hb_val, uint8_t hb_lvl_val, uint16_t rr_val[4], uint8_t rr_lvl_val, uint8_t rr_cnt, 
									uint16_t spo2_r_val, uint8_t wearing_state_val, uint8_t valid_lvl_val, uint8_t abnormal_state, uint8_t current[3], GS32 rawdata_dbg[][DBG_MCU_MODE_PKG_LEN], uint16_t rawdata_dbg_len)
{
    uint8_t algo_out_array[MCU_PKG_SPO2_ALGO_RESULT_LEN];
    algo_out_array[0] = spo2_val;
    algo_out_array[1] = spo2_lvl_val;
    algo_out_array[2] = hb_val; 
    algo_out_array[3] = hb_lvl_val;
    algo_out_array[4] = GET_HIGH_BYTE_FROM_WORD(rr_val[0]);
    algo_out_array[5] = GET_LOW_BYTE_FROM_WORD(rr_val[0]);
    algo_out_array[6] = GET_HIGH_BYTE_FROM_WORD(rr_val[1]);
    algo_out_array[7] = GET_LOW_BYTE_FROM_WORD(rr_val[1]);
    algo_out_array[8] = GET_HIGH_BYTE_FROM_WORD(rr_val[2]);
    algo_out_array[9] = GET_LOW_BYTE_FROM_WORD(rr_val[2]);
    algo_out_array[10] = GET_HIGH_BYTE_FROM_WORD(rr_val[3]);
    algo_out_array[11] = GET_LOW_BYTE_FROM_WORD(rr_val[3]);
    algo_out_array[12] = rr_lvl_val;
    algo_out_array[13] = rr_cnt;	
    algo_out_array[14] = GET_HIGH_BYTE_FROM_WORD(spo2_r_val);
    algo_out_array[15] = GET_LOW_BYTE_FROM_WORD(spo2_r_val);
    algo_out_array[16] = valid_lvl_val;
    algo_out_array[17] = abnormal_state;
    algo_out_array[18] = current[0];
    algo_out_array[19] = current[1];
    algo_out_array[20] = current[2];
    send_mcu_rawdata_packet_start(MCU_PKG_SPO2_TYPE_VAL, algo_out_array, MCU_PKG_SPO2_ALGO_RESULT_LEN, rawdata_dbg, rawdata_dbg_len);
}
#endif

#endif //#if (__USE_GOODIX_APP__)

#if (__UART_WITH_GOODIX_TOOLS__)
/// uart rx data handler
void gh30x_uart_cmd_parse(uint8_t *buffer, uint8_t length)
{
    uint8_t buffer_index = 0;

    for (buffer_index = 0; buffer_index < length; buffer_index++)
    {
        uart_module_handle_recv_byte(buffer[buffer_index]);
    }
}

/// send uart pkg
void uart_module_send_pkg(uint8_t string[], uint8_t length) // send value via through profile
{
    uint8_t uart_module_send_pkg_buffer[MUTLI_PKG_UART_MAX_LEN + MUTLI_PKG_HEADER_LEN]; /// send pkg buffer
    uint8_t uart_module_real_len = (length > MUTLI_PKG_UART_MAX_LEN) ? (MUTLI_PKG_UART_MAX_LEN) : (length);

    // fix uart_module_send_pkg_buffer
    uart_module_send_pkg_buffer[0] = MUTLI_PKG_MAGIC_0_VAL;
    uart_module_send_pkg_buffer[1] = MUTLI_PKG_MAGIC_1_VAL;
    uart_module_send_pkg_buffer[2] = uart_module_real_len;
    memcpy(&uart_module_send_pkg_buffer[3], string, uart_module_real_len);
    uart_module_send_pkg_buffer[3 + uart_module_real_len] = MUTLI_PKG_MAGIC_2_VAL;
    uart_module_send_data(uart_module_send_pkg_buffer, (uart_module_real_len + MUTLI_PKG_HEADER_LEN));
}

/// handle one byte from uart
void uart_module_handle_recv_byte(uint8_t recv_byte)
{
    switch (uart_module_parse_state)
    {
        case UART_PARSE_STATE_IDLE:
        {
            if (recv_byte == MUTLI_PKG_MAGIC_0_VAL)
            {
                uart_module_parse_state = UART_PARSE_STATE_GOT_MAGIC_0;
            }
        }
        break;

        case UART_PARSE_STATE_GOT_MAGIC_0:
        {
            if (MUTLI_PKG_MAGIC_1_VAL == recv_byte)
            {
                uart_module_parse_state = UART_PARSE_STATE_GOT_MAGIC_1;
            }
            else
            {
                uart_module_parse_state = UART_PARSE_STATE_IDLE;
            }
        }
        break;

        case UART_PARSE_STATE_GOT_MAGIC_1:
        {
            if ((recv_byte > 0) && (recv_byte <= MUTLI_PKG_UART_MAX_LEN))
            {
                uart_module_parse_state = UART_PARSE_STATE_GOT_LEN;
                uart_module_recv_index = 0;
                uart_module_recv_len = recv_byte;
            }
            else
            {
                uart_module_parse_state = UART_PARSE_STATE_IDLE;
            }
        }
        break;

        case UART_PARSE_STATE_GOT_LEN:
        {
            uart_module_recv_data_buffer[uart_module_recv_index] = recv_byte;
            uart_module_recv_index++;
            if (uart_module_recv_index >= uart_module_recv_len)
            {
                uart_module_parse_state = UART_PARSE_STATE_GOT_DATA;
            }
        }
        break;

        case UART_PARSE_STATE_GOT_DATA:
        {
            if (MUTLI_PKG_MAGIC_2_VAL == recv_byte)
            {
                gh30x_communicate_parse_handler(uart_comm_type, uart_module_recv_data_buffer, uart_module_recv_len);
            }
            uart_module_parse_state = UART_PARSE_STATE_IDLE;
        }
        break;

        default:
        {
            uart_module_parse_state = UART_PARSE_STATE_IDLE;
        }
        break;
    }
}

#endif

/****************************************************************
* Description: get Chip version.
* Output:  None
* Return: chip version
******************************************************************/
GU8 HBD_GetChipVersion(void)
{
    GU16 usRegdata = 0;
    GU8 uchChipVersion = HBD_VER_VAL_GH300_DEFAULT;

    HBD_I2cSendCmd(HBD_CMD_IDLE);
    HBD_DelayUs(HBD_SEND_CMD_DELAY_X_US);
    usRegdata = HBD_I2cReadReg(HBD_VER_REG_ADDR) & 0x000F;
    HBD_I2cSendCmd(HBD_CMD_SLEEP);
    HBD_DelayUs(HBD_SEND_CMD_DELAY_X_US);
    uchChipVersion = (uchChipVersion & 0xF0) + ((GU8)usRegdata);

    return uchChipVersion;
}

/****************************************************************
* Description: get version packet.
* Output:    puchPacketBuffer: data packet buffer ptr,
* Return: None
******************************************************************/
static void HBD_GetVersionPacket(GU8 *puchPacketBuffer)
{
    puchPacketBuffer[0] = g_uchDeviceType;
    puchPacketBuffer[1] = HBD_CTRL_MAJOR_VERSION_NUMBER;
    puchPacketBuffer[2] = HBD_CTRL_MINOR_VERSION_NUMBER;
    puchPacketBuffer[3] = HBD_CTRL_REVISION_VERSION_NUMBER;
}

/****************************************************************
* Description: Get CRC-8 data
* Input:    uchDataArr: data array,
            uchDataLen: data array len,
* Return: CRC-8 value
******************************************************************/
GU8 HBD_GetCRC(GU8 uchDataArr[], GU8 uchDataLen)
{
    GU8 uchCRCTabIndex;
    GU8 uchCrc8 = 0xFF;
    GU8 uchIndex = 0;

    for (uchIndex = 0; uchIndex < uchDataLen; uchIndex++)
    {
        uchCRCTabIndex = (uchCrc8 ^ (uchDataArr[uchIndex])) & 0xFF;
        uchCrc8 = g_uchCrc8TabArr[uchCRCTabIndex];
    }

    return uchCrc8;
}

/****************************************************************
* Description: translate communicate data, only cover < 20bytes
* Input:    puchDataBuff:ptr of packet buffer,
            uchLen: len of data,
* Return: None
******************************************************************/
static void HBD_CommDataTranslate(GU8 *puchDataBuff, GU8 uchFromIndex, GU8 uchLen)
{
    GU8 uchIndex  = uchFromIndex;
    GU8 uchCodeTabIndex = 0;

    for (; uchIndex < (uchLen+uchFromIndex); uchIndex++)
    {
        puchDataBuff[uchIndex] ^= uchCommKeyCode[uchCodeTabIndex++];
    }
}

/****************************************************************
* Description: parse recv buffer data.
* Input:    uchRecvDataBufferArr: recv data array ,
            uchLen: len of recv data array, 
* Return: parse status, see EM_COMM_PARSE_STATE,
******************************************************************/
static GU8 HBD_CommParseBuffer(GU8 uchRecvDataBufferArr[], GU8 uchLen)
{
    GU8 uchIndex = 0;

    if ((COMM_CRC_LENGTH + COMM_HEADER_LENGTH) > uchLen) // fix len err.
    {
        stParseStatus.ParseState = COMM_PARSE_STATE_ERR;
        return COMM_PARSE_STATE_ERR;
    }

    /* reinit parse state. */
    stParseStatus.ubPacketCmdId = COMM_CMD_NOP;
    stParseStatus.ParseState = COMM_PARSE_STATE_IDLE;
    stParseStatus.ubPacketDataLength = uchLen - COMM_CRC_LENGTH - COMM_HEADER_LENGTH;

    for (uchIndex = 0; uchIndex < uchLen; uchIndex++)
    {
        switch (stParseStatus.ParseState)
        {
            case COMM_PARSE_STATE_IDLE:
                if (COMM_STATE_NO_ERR == (uchRecvDataBufferArr[uchIndex] & 0x0F))
                {
                    stParseStatus.ParseState = COMM_PARSE_STATE_GOT_CMD_AND_STATE;
                    stParseStatus.ubPacketCmdId = (uchRecvDataBufferArr[uchIndex] >> 4) & 0x0F;
                    stParseStatus.ubPacketDataIndex = 0;
                }
                else
                {
                    stParseStatus.ParseState = COMM_PARSE_STATE_ERR;
                }
                break;
            case COMM_PARSE_STATE_GOT_CMD_AND_STATE:
                if (COMM_VERSION == uchRecvDataBufferArr[uchIndex])
                {
                    if (0 != stParseStatus.ubPacketDataLength)
                    {
                        stParseStatus.ParseState = COMM_PARSE_STATE_GOT_COMM_VERSION;
                    }
                    else
                    {
                        stParseStatus.ParseState = COMM_PARSE_STATE_GOT_RAW_DATA; // if raw data is empty
                    }
                }
                else
                {
                    stParseStatus.ParseState = COMM_PARSE_STATE_ERR;
                }
                break;
            case COMM_PARSE_STATE_GOT_COMM_VERSION:
                stParseStatus.ubPacketRawData[stParseStatus.ubPacketDataIndex] = uchRecvDataBufferArr[uchIndex];
                stParseStatus.ubPacketDataIndex++;
                if (stParseStatus.ubPacketDataIndex == stParseStatus.ubPacketDataLength)
                {
                    stParseStatus.ParseState = COMM_PARSE_STATE_GOT_RAW_DATA;
                }
                break;
            case COMM_PARSE_STATE_GOT_RAW_DATA:
                if (uchRecvDataBufferArr[uchIndex] == HBD_GetCRC(uchRecvDataBufferArr, uchLen - COMM_CRC_LENGTH))
                {
                    stParseStatus.ParseState = COMM_PARSE_STATE_DONE;
                }
                else
                {
                    stParseStatus.ParseState = COMM_PARSE_STATE_ERR;
                }
                break;
            default:
                stParseStatus.ParseState = COMM_PARSE_STATE_ERR;
                break;
        }
        if ((COMM_PARSE_STATE_ERR == stParseStatus.ParseState) || (COMM_PARSE_STATE_DONE == stParseStatus.ParseState))
        {
            break;
        }
    }

    return stParseStatus.ParseState;
}

/****************************************************************
* Description: format packet to send .
* Input:    uchState: cmd state,
            uchCmd: cmd id, 
            uchDataBufferArr: data section need to send,
            uchDataLength: data section len,
* Output:   puchPacketBuffer: data packet buffer ptr,
* Return: len afer make packet 
******************************************************************/
static GU8 HBD_CommAddPacketFormat(GU8 uchState, GU8 uchCmd, GU8 *puchPacketBuffer, GU8 uchDataBufferArr[], GU8 uchDataLength)
{
    GU8 uchPacketIndex = 0;
    GU8 uchHeaderLength = COMM_RAW_DATA_PACKET_HEADER_LENGTH;

    puchPacketBuffer[uchPacketIndex++] = ((uchCmd << 4) & 0xF0 ) | (uchState & 0x0F);
    if (COMM_CMD_HB_PACKEGE_DATA_EX != uchCmd)
    {
        puchPacketBuffer[uchPacketIndex++] = COMM_VERSION;
        uchHeaderLength = COMM_HEADER_LENGTH; // if not a rawdata package 
    }
    if (0 != uchDataLength)
    {
        memcpy(&puchPacketBuffer[uchPacketIndex], uchDataBufferArr, uchDataLength);
        uchPacketIndex += uchDataLength;
    }
    puchPacketBuffer[uchPacketIndex++] = HBD_GetCRC(puchPacketBuffer, uchDataLength + uchHeaderLength);
    HBD_CommDataTranslate(puchPacketBuffer, uchHeaderLength/* data index. */, uchDataLength);

    return (uchPacketIndex);
}

/****************************************************************
* Description: send data via g_pSendDataFuncArr.
* Input:    uchDataBufferArr: data array need to send,
            uchLen: data section len,
* Return: None
******************************************************************/
static void HBD_CommSendData(GU8 uchDataBufferArr[], GU8 uchLen)
{
    if ((HBD_PTR_NULL != g_pSendDataFuncArr[g_uchCommCurrentIndex]) && (0 != uchLen))
    {
        g_pSendDataFuncArr[g_uchCommCurrentIndex](uchDataBufferArr, uchLen);
    }
}

/****************************************************************
* Description: parse communicate receive data
* Input:    uchCommInterFaceId: this id from HBD_SetSendDataFunc return,
            uchDataBuffArr: data buff ptr of receive data,
            uchLen: data len of receive data,
* Return: see EM_COMM_CMD_TYPE typedef
******************************************************************/
EM_COMM_CMD_TYPE HBD_CommParseHandler(GU8 uchCommInterFaceId, GU8 uchDataBuffArr[], GU8 uchLen)
{
    GU8 uchRetDataBuffArr[20] = {0};
    GU8 uchCommParseDataBuffArr[20] = {0};
    GU8 uchRetLen = 0;
    EM_COMM_CMD_TYPE emCommCmdType = COMM_CMD_INVALID;

    g_uchCommCurrentIndex = uchCommInterFaceId;

    if ((COMM_CRC_LENGTH + COMM_HEADER_LENGTH) > uchLen) // fix len err.
    {
        stParseStatus.ParseState = COMM_PARSE_STATE_ERR;
        return emCommCmdType;
    }

    memcpy(uchCommParseDataBuffArr, uchDataBuffArr, uchLen);
    HBD_CommDataTranslate(uchCommParseDataBuffArr, COMM_PACKET_DATA_INDEX, uchLen - COMM_HEADER_LENGTH - COMM_CRC_LENGTH);
    HBD_CommParseBuffer(uchCommParseDataBuffArr, uchLen);

    if (stParseStatus.ParseState == COMM_PARSE_STATE_DONE)
    {
        switch (stParseStatus.ubPacketCmdId)
        {
        case COMM_CMD_CHECK_STATUS:
            {
                if (stParseStatus.ubPacketRawData[0] == 0x01)
                {
                    HBD_GetVersionPacket(&stParseStatus.ubPacketRawData[1]); // get software version
                    uchRetLen = HBD_CommAddPacketFormat(COMM_STATE_NO_ERR, stParseStatus.ubPacketCmdId, uchRetDataBuffArr, 
                                stParseStatus.ubPacketRawData, 8);
                }
                else if (stParseStatus.ubPacketRawData[0] == 0x02)
                {
                    stParseStatus.ubPacketRawData[1] = 0x00; // get config serial number // fix to 0x00
                    uchRetLen = HBD_CommAddPacketFormat(COMM_STATE_NO_ERR, stParseStatus.ubPacketCmdId, uchRetDataBuffArr, 
                                stParseStatus.ubPacketRawData, 5);
                }
                else if (stParseStatus.ubPacketRawData[0] == 0x03)
                {
                    stParseStatus.ubPacketRawData[1] = HBD_GetChipVersion(); // get chip hardware version
                    uchRetLen = HBD_CommAddPacketFormat(COMM_STATE_NO_ERR, stParseStatus.ubPacketCmdId, uchRetDataBuffArr, 
                                stParseStatus.ubPacketRawData, 2);
                }
            }
            break;

        case COMM_CMD_LOAD_CONFIG:
            {
                static GU8 uchConfigDataBufferArr[HBD_STORAGE_CONFIG_DATA_REG_MAX_NUM * (sizeof(ST_REGISTER)) + HBD_CONFIG_ARR_HEADER_LENGTH] = {0};
                static GU16 usConfigBufferIndex = 0;
                static GU16 usConfigDataLen = 0;
                static GU8 uchLastPacketIndex = 0;
                GU16 usThisPacketLenValue = ((((GU16)stParseStatus.ubPacketRawData[0])<< 8 ) & 0xFF00) | (((GU16)stParseStatus.ubPacketRawData[1]) & 0x00FF);
                GU8 uchCheckOk = 0;
                if (stParseStatus.ubPacketRawData[2] == 0)
                {
                    usConfigBufferIndex = 0;
                    memset(uchConfigDataBufferArr, 0, HBD_STORAGE_CONFIG_DATA_REG_MAX_NUM * (sizeof(ST_REGISTER)) + HBD_CONFIG_ARR_HEADER_LENGTH);
                    usConfigDataLen = usThisPacketLenValue;
                    uchCheckOk = 1;
                }
                else
                {
                    if ((stParseStatus.ubPacketRawData[2] == (uchLastPacketIndex + 1)) && (usConfigDataLen == usThisPacketLenValue))
                    {
                        uchCheckOk = 1;
                    }
                }
                uchLastPacketIndex = stParseStatus.ubPacketRawData[2];
                if (uchCheckOk == 1)
                {
                    memcpy(&uchConfigDataBufferArr[usConfigBufferIndex], &stParseStatus.ubPacketRawData[3], stParseStatus.ubPacketDataLength - 3);
                    usConfigBufferIndex += (stParseStatus.ubPacketDataLength - 3);
                    if (usConfigDataLen == usConfigBufferIndex)
                    {
#ifndef GOODIX_DEMO_PLANFORM
                        HBD_I2cSendCmd(HBD_CMD_IDLE);
                        HBD_DelayUs(HBD_SEND_CMD_DELAY_X_US);
                        HBD_ChangeToHbModeRegVal();
                        HBD_ConfigArrTranslate(&uchConfigDataBufferArr[HBD_CONFIG_ARR_DATA_INDEX], uchConfigDataBufferArr[HBD_CONFIG_ARR_LENGTH_INDEX] * (sizeof(ST_REGISTER)));
                        HBD_LoadConfigWithMode(uchConfigDataBufferArr, CONFIG_ARR_FROM_RUN_LOAD, 1);
                        HBD_StoreHbModeRegVal();
                        HBD_I2cSendCmd(HBD_CMD_SLEEP);
                        HBD_DelayUs(HBD_SEND_CMD_DELAY_X_US);
#endif
                    }

                    uchRetLen = HBD_CommAddPacketFormat(COMM_STATE_NO_ERR, stParseStatus.ubPacketCmdId, uchRetDataBuffArr, &stParseStatus.ubPacketRawData[2], 1);   
                }
                else
                {
                    uchRetLen = HBD_CommAddPacketFormat(COMM_STATE_PROTOCOL_ERR, stParseStatus.ubPacketCmdId, uchRetDataBuffArr, &stParseStatus.ubPacketRawData[2], 1);
                }
            }
            break;

        case COMM_CMD_HBD_CTRL:
            {
                if (stParseStatus.ubPacketDataLength >= 2)
                {
                    g_uchHbaScenario = stParseStatus.ubPacketRawData[1];
                    if (stParseStatus.ubPacketDataLength >= 3)
                    {
                        //EM_HBD_FUNCTIONAL_STATE emConfigMode = (stParseStatus.ubPacketRawData[2] == 0)? (HBD_FUNCTIONAL_STATE_DISABLE) : (HBD_FUNCTIONAL_STATE_ENABLE); 
                        //HBD_HbAlgoOutputFastModeConfig(emConfigMode);
                    }
                    if ((stParseStatus.ubPacketDataLength >= 4) && (stParseStatus.ubPacketRawData[3] != 0))
                    {
                        g_stAutoLedConfig.stAutoLedChannel[0].emAutoledEnable = HBD_FUNCTIONAL_STATE_ENABLE;
                        g_stAutoLedConfig.stAutoLedChannel[1].emAutoledEnable = HBD_FUNCTIONAL_STATE_ENABLE;
                    }
                    else
                    {
                        g_stAutoLedConfig.stAutoLedChannel[0].emAutoledEnable = HBD_FUNCTIONAL_STATE_DISABLE;
                        g_stAutoLedConfig.stAutoLedChannel[1].emAutoledEnable = HBD_FUNCTIONAL_STATE_DISABLE;
                    } 
                    if ((stParseStatus.ubPacketDataLength >= 5))
                    {
                        g_uchHbaTestMode = stParseStatus.ubPacketRawData[4];
                    }
                }
                if (stParseStatus.ubPacketRawData[0] <  COMM_CMD_INVALID)
                {
                    emCommCmdType = (EM_COMM_CMD_TYPE)stParseStatus.ubPacketRawData[0];
                }
                uchRetLen = HBD_CommAddPacketFormat(COMM_STATE_NO_ERR, stParseStatus.ubPacketCmdId, uchRetDataBuffArr,
                                stParseStatus.ubPacketRawData, stParseStatus.ubPacketDataLength);
            }
            break;
       
        case COMM_CMD_REG_RW:
            {
                GU8 uchRegRwLen = stParseStatus.ubPacketRawData[COMM_REG_MODE_AND_NUM_INDEX] & 0x0F;
                GU8 uchRegRwMode = stParseStatus.ubPacketRawData[COMM_REG_MODE_AND_NUM_INDEX] & 0xF0;
                GU16 usRegAddr = ((((GU16)stParseStatus.ubPacketRawData[COMM_REG_ADDR_H_INDEX])<< 8 ) & 0xFF00) | 
                                 (((GU16)stParseStatus.ubPacketRawData[COMM_REG_ADDR_L_INDEX]) & 0x00FF);
  
                if (uchRegRwMode == COMM_REG_WRITE_MODE)
                {
                    if (((uchRegRwLen * 2) == stParseStatus.ubPacketDataLength - COMM_REG_HEADER_LENGTH))
                    {
                        GU8 uchIndex;
                        GU16 usRegDataTmp = 0;
                        HBD_I2cSendCmd(HBD_CMD_IDLE);
                        HBD_DelayUs(HBD_SEND_CMD_DELAY_X_US);
                        HBD_ChangeToHbModeRegVal();
                        for (uchIndex = 0; uchIndex < (uchRegRwLen); uchIndex++)
                        {
                            usRegDataTmp = ((((GU16)stParseStatus.ubPacketRawData[COMM_REG_DATA_INDEX + (uchIndex * 2)] << 8) & 0xFF00) |
                                               ((GU16)stParseStatus.ubPacketRawData[COMM_REG_DATA_INDEX + 1 + (uchIndex * 2)] & 0x00FF));
                            HBD_I2cWriteReg(usRegAddr + (uchIndex * 2), usRegDataTmp);
                        }
                        HBD_StoreHbModeRegVal();
                        HBD_I2cSendCmd(HBD_CMD_SLEEP);
                        HBD_DelayUs(HBD_SEND_CMD_DELAY_X_US);
                        uchRetLen = HBD_CommAddPacketFormat(COMM_STATE_NO_ERR, stParseStatus.ubPacketCmdId, uchRetDataBuffArr,
                                        stParseStatus.ubPacketRawData, stParseStatus.ubPacketDataLength);
                    }
                    else
                    {
                        uchRetLen = HBD_CommAddPacketFormat(COMM_STATE_PROTOCOL_ERR, stParseStatus.ubPacketCmdId, uchRetDataBuffArr,
                                        stParseStatus.ubPacketRawData, stParseStatus.ubPacketDataLength);
                    }
                }
                else 
                {
                    GU8 uchIndex;
                    GU16 usRegDataTmp = 0;
                    HBD_I2cSendCmd(HBD_CMD_IDLE);
                    HBD_DelayUs(HBD_SEND_CMD_DELAY_X_US);
                    HBD_ChangeToHbModeRegVal();
                    for (uchIndex = 0; uchIndex < (uchRegRwLen); uchIndex++)
                    {
                        usRegDataTmp = HBD_I2cReadReg(usRegAddr + (uchIndex * 2));
                        stParseStatus.ubPacketRawData[COMM_REG_DATA_INDEX+(uchIndex * 2)] = (GU8)((usRegDataTmp >> 8) & 0xFF);
                        stParseStatus.ubPacketRawData[COMM_REG_DATA_INDEX+1+(uchIndex * 2)] = (GU8)((usRegDataTmp) & 0xFF);
                    }
                    HBD_I2cSendCmd(HBD_CMD_SLEEP);
                    HBD_DelayUs(HBD_SEND_CMD_DELAY_X_US);
                    uchRetLen = HBD_CommAddPacketFormat(COMM_STATE_NO_ERR, stParseStatus.ubPacketCmdId, uchRetDataBuffArr,
                                    stParseStatus.ubPacketRawData, stParseStatus.ubPacketDataLength + uchRegRwLen * 2);
                }
            }
            break;
        case COMM_CMD_ADT_CONFIG_RW:
            {
                GU8 uchAdtCofigRwMode = stParseStatus.ubPacketRawData[0];
                if (uchAdtCofigRwMode == COMM_ADT_CONFIG_WRITE_MODE)
                {
                    GU16 usRegDataTmp = (GU16)((stParseStatus.ubPacketRawData[1] >> 7) & 0x1);
                    HBD_I2cSendCmd(HBD_CMD_IDLE);
                    HBD_DelayUs(HBD_SEND_CMD_DELAY_X_US);
                    HBD_I2cWriteReg(HBD_ADT_GINT_CTRL0_REG_ADDR, usRegDataTmp);
                    HBD_I2cSendCmd(HBD_CMD_SLEEP);
                    HBD_DelayUs(HBD_SEND_CMD_DELAY_X_US);
                    #if 0
                    g_stAdtRegList.usGainConfigRegVal = 0x0C00 | (((GU16)(stParseStatus.ubPacketRawData[2] >> 4) & 0x7) << 4) | 
                                                                (((GU16)stParseStatus.ubPacketRawData[2] & 0x7) << 7);  // reg 0x136
                    g_stAdtRegList.usTimeSel1RegVal = (((GU16)(stParseStatus.ubPacketRawData[2] >> 7) & 0x1) << 6) | 
                                                    ((GU16)((stParseStatus.ubPacketRawData[2] >> 3) & 0x1) << 7) | 
                                                    ((GU16)stParseStatus.ubPacketRawData[1] & 0x7);  // 
                    g_stAdtRegList.usChannelMapConfigRegVal = ((GU16) 1 << 5)| (((GU16)(stParseStatus.ubPacketRawData[1] >> 4) & 0x7) << 6); // reg 0x84
                    g_stAdtRegList.usTimeSel0RegVal = 0x0005 | (((GU16)stParseStatus.ubPacketRawData[1] & 0x7) << 8) ;  // reg 0x80
                    g_stAdtRegList.usLed01CurrentConfigRegVal = (((GU16)stParseStatus.ubPacketRawData[4] & 0xFF) << 8) | 
                                                                ((GU16)stParseStatus.ubPacketRawData[3] & 0xFF); // reg 0x118
                    #endif
                    uchRetLen = HBD_CommAddPacketFormat(COMM_STATE_NO_ERR, stParseStatus.ubPacketCmdId, uchRetDataBuffArr, stParseStatus.ubPacketRawData, 5);
                }
                else 
                {
                    //GU16 usRegDataTmp = 0;
                    HBD_I2cSendCmd(HBD_CMD_IDLE);
                    HBD_DelayUs(HBD_SEND_CMD_DELAY_X_US);
                    //usRegDataTmp = HBD_I2cReadReg(HBD_ADT_GINT_CTRL0_REG_ADDR);
                    HBD_I2cSendCmd(HBD_CMD_SLEEP);
                    HBD_DelayUs(HBD_SEND_CMD_DELAY_X_US);
                    #if 0
                    stParseStatus.ubPacketRawData[1] = (GU8)(((((g_stAdtRegList.usChannelMapConfigRegVal >> 6) & 0x7) << 4) | 
                                                        (g_stAdtRegList.usTimeSel1RegVal & 0x7) | 
                                                        (((usRegDataTmp & 0x1) << 7))) & 0xFF);
                    stParseStatus.ubPacketRawData[2] = (GU8)(((((g_stAdtRegList.usGainConfigRegVal >> 4) & 0x7) << 4) | 
                                                             ((g_stAdtRegList.usGainConfigRegVal >> 7) & 0x7) | 
                                                             (((g_stAdtRegList.usTimeSel1RegVal >> 6) & 0x1) << 7) | 
                                                             (((g_stAdtRegList.usTimeSel1RegVal >> 7) & 0x1) << 3)) & 0xFF);
                    stParseStatus.ubPacketRawData[3] = (GU8)(g_stAdtRegList.usLed01CurrentConfigRegVal & 0xFF);  // led0 current
                    stParseStatus.ubPacketRawData[4] = (GU8)((g_stAdtRegList.usLed01CurrentConfigRegVal >> 8) & 0xFF); // led1 current
                    #endif
                    uchRetLen = HBD_CommAddPacketFormat(COMM_STATE_NO_ERR, stParseStatus.ubPacketCmdId, uchRetDataBuffArr, stParseStatus.ubPacketRawData, 5);
                }
            }
            break;
        case COMM_CMD_READ_OTP_REG:
            {
                GU8 uchIndex = 0;
                GU8 uchHasReadFlag = 0;
                GU16 usbOtpRegValue = 0x00;

                if (stParseStatus.ubPacketRawData[0] > 15)
                {
                    stParseStatus.ubPacketRawData[0] = 15;
                }
                HBD_I2cSendCmd(HBD_CMD_IDLE);
                HBD_DelayUs(HBD_SEND_CMD_DELAY_X_US);
                for (uchIndex = 0; uchIndex < stParseStatus.ubPacketRawData[0]; uchIndex++)
                {
                    GU16 usOtpAddr = stParseStatus.ubPacketRawData[1] + uchIndex;
                    if (((usOtpAddr % 2) == 0) || (uchHasReadFlag == 0))
                    {
                        GU16 usbOtpRegAddr = usOtpAddr & 0xFFFE;
                        HBD_I2cWriteReg(0x0064, usbOtpRegAddr); // otp addr 
                        HBD_I2cWriteReg(0x006A, 0x0001);
                        HBD_DelayUs(3);
                        HBD_I2cWriteReg(0x006A, 0x0000);
                        usbOtpRegValue = HBD_I2cReadReg(0x006C); 
                        if ((usOtpAddr % 2) == 0)
                        {
                            stParseStatus.ubPacketRawData[2 + uchIndex] = HBD_GET_LOW_BYTE_FROM_WORD(usbOtpRegValue);
                            uchHasReadFlag = 1;
                        }
                        else
                        {
                            stParseStatus.ubPacketRawData[2 + uchIndex] = HBD_GET_HIGH_BYTE_FROM_WORD(usbOtpRegValue);
                        }
                    }
                    else
                    {
                        stParseStatus.ubPacketRawData[2 + uchIndex] = HBD_GET_HIGH_BYTE_FROM_WORD(usbOtpRegValue);
                        uchHasReadFlag = 0;
                    }
                }
                HBD_I2cSendCmd(HBD_CMD_SLEEP);
                HBD_DelayUs(HBD_SEND_CMD_DELAY_X_US);
                uchRetLen = HBD_CommAddPacketFormat(COMM_STATE_NO_ERR, stParseStatus.ubPacketCmdId, uchRetDataBuffArr, \
                                                        stParseStatus.ubPacketRawData, stParseStatus.ubPacketRawData[0] + 2);
            }
            break;
        case COMM_CMD_AUTO_LED_CONFIG:
            {
                GU8 uchConfigMode = stParseStatus.ubPacketRawData[0];
                switch (uchConfigMode)
                {
                case 1:
                case 2:
                    {
                        GU8 uchThrConfigMode = stParseStatus.ubPacketRawData[1];
                        if (uchThrConfigMode == 0)
                        {
                            g_stAutoLedConfig.stAutoLedChannel[uchConfigMode - 1].unTrigerUpperThr = HBD_MAKEUP_DWORD(stParseStatus.ubPacketRawData[2], stParseStatus.ubPacketRawData[3],
                                                                                                                        stParseStatus.ubPacketRawData[4], stParseStatus.ubPacketRawData[5]);
                            g_stAutoLedConfig.stAutoLedChannel[uchConfigMode - 1].unTrigerLowerThr = HBD_MAKEUP_DWORD(stParseStatus.ubPacketRawData[6], stParseStatus.ubPacketRawData[7],
                                                                                                                        stParseStatus.ubPacketRawData[8], stParseStatus.ubPacketRawData[9]);
                        }
                        else if (uchThrConfigMode == 1)
                        {
                            g_stAutoLedConfig.stAutoLedChannel[uchConfigMode - 1].unTargetUpperThr = HBD_MAKEUP_DWORD(stParseStatus.ubPacketRawData[2], stParseStatus.ubPacketRawData[3],
                                                                                                                        stParseStatus.ubPacketRawData[4], stParseStatus.ubPacketRawData[5]);
                            g_stAutoLedConfig.stAutoLedChannel[uchConfigMode - 1].unTargetLowerThr = HBD_MAKEUP_DWORD(stParseStatus.ubPacketRawData[6], stParseStatus.ubPacketRawData[7],
                                                                                                                        stParseStatus.ubPacketRawData[8], stParseStatus.ubPacketRawData[9]);
                        }
                        else if (uchThrConfigMode == 2)
                        {
                            g_stAutoLedConfig.stAutoLedChannel[uchConfigMode - 1].unCompatibleTakeoffThr = HBD_MAKEUP_DWORD(stParseStatus.ubPacketRawData[2], stParseStatus.ubPacketRawData[3],
                                                                                                                        stParseStatus.ubPacketRawData[4], stParseStatus.ubPacketRawData[5]);
                        }
                    }
                    break;
                case 3:
                    {
                        HBD_AutoLedBgThrConfig(stParseStatus.ubPacketRawData[1] & 0x07, stParseStatus.ubPacketRawData[2] & 0x07);
                    }
                    break;
                default:
                    break;
                } 
                uchRetLen = HBD_CommAddPacketFormat(COMM_STATE_NO_ERR, stParseStatus.ubPacketCmdId, uchRetDataBuffArr, \
                                stParseStatus.ubPacketRawData, stParseStatus.ubPacketDataLength);
            }
            break;
        default :
            uchRetLen = HBD_CommAddPacketFormat(COMM_STATE_PROTOCOL_ERR, stParseStatus.ubPacketCmdId, uchRetDataBuffArr, 
                            stParseStatus.ubPacketRawData, 0);
            break;
        }
    }
    else
    {
        uchRetLen = HBD_CommAddPacketFormat(COMM_STATE_PROTOCOL_ERR, stParseStatus.ubPacketCmdId, uchRetDataBuffArr, \
                        stParseStatus.ubPacketRawData, 0);
    }
    HBD_CommSendData(uchRetDataBuffArr, uchRetLen);
    return emCommCmdType;
}

/****************************************************************
* Description: Set send data function
* Input:    pSendDataFunc: send data func ptr,
* Return: val > 0:communication interface id,
          HBD_RET_RESOURCE_ERROR: have no resource,
******************************************************************/
GS8 HBD_SetSendDataFunc(void (*pSendDataFunc)(GU8 uchDataBuffArr[], GU8 uchLen))
{
    GS8 chRet = HBD_RET_RESOURCE_ERROR;

    if (g_uchSendDataFuncRegisterIndex < COMM_SENDDATA_FUNC_REGISTER_MAX)
    {
        g_pSendDataFuncArr[g_uchSendDataFuncRegisterIndex] = pSendDataFunc;
        chRet = g_uchSendDataFuncRegisterIndex;
        g_uchSendDataFuncRegisterIndex++;
    }
    return chRet;
}

/****************************************************************
* Description: send rawdata Package  
* Input:    unCh1Ppg&unCh2Ppg: ppg rawdata
            sAccX&sAccY&sAccZ: gsensor rawdata
            uchCh0Current&uchCh1Current&uchCh2Current: current rawdata
            uchIndex: index of packega
* Return: None
******************************************************************/
void HBD_SendRawdataPackageDbg(GU32 unCh1Ppg, GU32 unCh2Ppg, GS16 sAccX,GS16 sAccY, GS16 sAccZ, 
                               GU8 uchCh0Current, GU8 uchCh1Current, GU8 uchCh2Current, GU8 uchIndex)
{
    GU8 uchPackageState = COMM_STATE_NO_ERR;
    GU8 uchRetLen = 0;
    GU8 uchRetDataBuffArr[20] = {0};
    GU8 uchDataBuffArr[COMM_RAW_DATA_PACKAGE_LEN_EX] = {0};

    uchDataBuffArr[0] = HBD_GET_BYTE2_FROM_DWORD(unCh1Ppg);
    uchDataBuffArr[1] = HBD_GET_BYTE1_FROM_DWORD(unCh1Ppg);
    uchDataBuffArr[2] = HBD_GET_BYTE0_FROM_DWORD(unCh1Ppg);
    uchDataBuffArr[3] = HBD_GET_BYTE2_FROM_DWORD(unCh2Ppg);
    uchDataBuffArr[4] = HBD_GET_BYTE1_FROM_DWORD(unCh2Ppg);
    uchDataBuffArr[5] = HBD_GET_BYTE0_FROM_DWORD(unCh2Ppg);
    uchDataBuffArr[6] = HBD_GET_HIGH_BYTE_FROM_WORD(sAccX);
    uchDataBuffArr[7] = HBD_GET_LOW_BYTE_FROM_WORD(sAccX);
    uchDataBuffArr[8] = HBD_GET_HIGH_BYTE_FROM_WORD(sAccY);
    uchDataBuffArr[9] = HBD_GET_LOW_BYTE_FROM_WORD(sAccY);
    uchDataBuffArr[10] = HBD_GET_HIGH_BYTE_FROM_WORD(sAccZ);
    uchDataBuffArr[11] = HBD_GET_LOW_BYTE_FROM_WORD(sAccZ);
    uchDataBuffArr[12] = (uchIndex & 0xFF);
    uchDataBuffArr[13] = uchCh0Current;
    uchDataBuffArr[14] = uchCh1Current;
    uchDataBuffArr[15] = uchCh2Current;

    if ((0xFFFFFFFFU == unCh1Ppg) || (0xFFFFFFFFU == unCh2Ppg))
    {
        if ((0xFFFF == (GU16)sAccX) && (0xFFFF == (GU16)sAccY) && (0xFFFF == (GU16)sAccZ))
        {
            uchPackageState = COMM_STATE_HB_GSENSOR_DATA_ERR;
        }
        else
        {
            uchPackageState = COMM_STATE_HB_COMM_ERR;
        }
    }
    else if ((0xFFFF == (GU16)sAccX) && (0xFFFF == (GU16)sAccY) && (0xFFFF == (GU16)sAccZ))
    {
        uchPackageState = COMM_STATE_GSENSOR_DATA_ERR;
    }
    uchRetLen = HBD_CommAddPacketFormat(uchPackageState, COMM_CMD_HB_PACKEGE_DATA_EX, uchRetDataBuffArr,  uchDataBuffArr, COMM_RAW_DATA_PACKAGE_LEN_EX);
    HBD_CommSendData(uchRetDataBuffArr, uchRetLen);
}

/****************************************************************
* Description: send rawdata Package by newdata INT 
* Input:    stGsAxisValue: gsensor axis value
            emGsensorSensitivity: G-sensor sensitivity (counts/g), see EM_HBD_GSENSOR_SENSITIVITY
* Return: auto led state flag: 0: success, 1: error
******************************************************************/
GU8 HBD_SendRawdataPackageByNewdataInt(ST_GS_DATA_TYPE *stGsAxisValue, EM_HBD_GSENSOR_SENSITIVITY emGsensorSensitivity)
{
    GU32 unCh1Ppg = 0, unCh2Ppg = 0;
    GS16 sAccX = 0, sAccY = 0, sAccZ = 0;
    GU16 uwRegVal = 0;
    GU8 uchChannelCurrent[3] = {0};
    GS16 sGsensorRightShift = (1 << emGsensorSensitivity);
    GU32 unRawDataArr[5] = {0};//edited by jianxiao 20210107
    GU8 uchRet = 0;

    if ((g_uchHbdStarted == HBD_START_STATUS_HB_MODE) || (g_uchHbdStarted == HBD_START_STATUS_HRV_MODE)|| (g_uchHbdStarted == HBD_START_STATUS_SPO2_MODE))
    {
        unCh1Ppg = HBD_I2cReadRawdataReg(g_usReadRawdataRegList[0]);
        if (g_uchNeedReadRawdataCount == 2)
        {
            unCh2Ppg = HBD_I2cReadRawdataReg(g_usReadRawdataRegList[1]);
        }
        else
        {
            unCh2Ppg = unCh1Ppg;
        }
        unRawDataArr[0] = unCh1Ppg;
        unRawDataArr[1] = unCh2Ppg;

        //Get current 
        uwRegVal = HBD_I2cReadReg(HBD_LED17_REG_ADDR);
        uchChannelCurrent[0] = (GU8)((uwRegVal & 0x00FF) >> 0);
        uchChannelCurrent[1] = (GU8)((uwRegVal & 0xFF00) >> 8);
        uwRegVal = HBD_I2cReadReg(HBD_LED18_REG_ADDR);
        uchChannelCurrent[2] = (GU8)((uwRegVal & 0x00FF) >> 0);
        /* Edited by jianxiao 20210107*/
        if (stGsAxisValue != HBD_PTR_NULL) // fix
        {
            sAccX = stGsAxisValue->sXAxisVal / sGsensorRightShift;
            sAccY = stGsAxisValue->sYAxisVal / sGsensorRightShift;
            sAccZ = stGsAxisValue->sZAxisVal / sGsensorRightShift;
            unRawDataArr[2] = sAccX;
            unRawDataArr[3] = sAccY;
            unRawDataArr[4] = sAccZ;
        }
        /* ending */
        if (0 == HBD_AutoLedbyNewDataInt(unRawDataArr) || g_uchKeepLedWorking)
        {
            HBD_I2cSendCmd(HBD_CMD_RESUME);

            HBD_SendRawdataPackageDbg(unRawDataArr[0], unRawDataArr[1], sAccX, sAccY, sAccZ, uchChannelCurrent[0], uchChannelCurrent[1], uchChannelCurrent[2], g_uchPackageSN);
            g_uchPackageSN++;
        }
        else
        {
            uchRet = 1;
            HBD_I2cSendCmd(HBD_CMD_IDLE);
        }
    }
    return uchRet;
}

static uint8_t comm_packet_calc_crc(uint8_t uchDataArr[], uint16_t usDataLen)
{
    uint8_t uchCRCTabIndex;
    uint8_t uchCrc8 = 0xFF;
    uint16_t usIndex = 0;

    for (usIndex = 0; usIndex < usDataLen; usIndex++)
    {
        uchCRCTabIndex = (uchCrc8 ^ (uchDataArr[usIndex])) & 0xFF;
        uchCrc8 = comm_packet_crc8_tab[uchCRCTabIndex];
    }
    return uchCrc8;
}

static void comm_packet_translate(uint8_t *puchDataBuff, uint16_t usFromIndex, uint16_t usLen)
{
    uint16_t usIndex = usFromIndex;
    uint16_t usCodeTabIndex = 0;

    for (; usIndex < (usLen + usFromIndex); usIndex++)
    {
        puchDataBuff[usIndex] ^= comm_key_code_list[usCodeTabIndex++ % sizeof(comm_key_code_list)];
    }
}

static uint16_t comm_packet_format(uint8_t uchState, uint8_t uchCmd, uint8_t *puchPacketBuffer, uint8_t uchDataBufferArr[], uint16_t usDataLength)
{
    uint16_t usPacketIndex = 0;
    uint16_t usHeaderLength = COMM_HEADER_LENGTH;

    puchPacketBuffer[usPacketIndex++] = ((uchCmd << 4) & 0xF0 ) | (uchState & 0x0F);
    puchPacketBuffer[usPacketIndex++] = COMM_VERSION;
    if (usDataLength != 0)
    {
        memcpy(&puchPacketBuffer[usPacketIndex], uchDataBufferArr, usDataLength);
        usPacketIndex += usDataLength;
    }
    puchPacketBuffer[usPacketIndex++] = comm_packet_calc_crc(puchPacketBuffer, usDataLength+usHeaderLength);
    comm_packet_translate(puchPacketBuffer, usHeaderLength/* data index. */, usDataLength);

    return (usPacketIndex);
}

void comm_send_ext_packet(void (*pSendDataFunc)(uint8_t uchDataBuffArr[], uint8_t uchLen), uint8_t data_array[], uint8_t data_len)
{
    uint8_t uchRetLen = 0;
    uint8_t uchRetDataBuffArr[20] = {0};
    uint8_t uchPacketDataBuffArr[16] = {0};
    uint8_t uchPacketLen = 0;

    if (data_len == 0)
    {
        return;
    }
    uchPacketLen = (data_len > 16) ? (16) : (data_len - 1);

    if ((data_array != HBD_PTR_NULL) && (uchPacketLen != 0))
    {
        memcpy(uchPacketDataBuffArr, &data_array[1], uchPacketLen);
    }
    uchRetLen = (uint8_t)comm_packet_format(COMM_STATE_NO_ERR, data_array[0], uchRetDataBuffArr, uchPacketDataBuffArr, uchPacketLen);

    if (HBD_PTR_NULL != pSendDataFunc)
    {
        pSendDataFunc(uchRetDataBuffArr, uchRetLen);
    }
}

void comm_send_app_cmd(void (*pSendDataFunc)(uint8_t uchDataBuffArr[], uint8_t uchLen), uint8_t dev_state, uint8_t data_array[], uint8_t data_len)
{
    uint8_t uchRetLen = 0;
    uint8_t uchRetDataBuffArr[20] = {0};
    uint8_t uchPacketDataBuffArr[COMM_APP_CMD_PACKET_LEN] = {0};
    uint8_t uchPacketLen = (data_len >= COMM_APP_CMD_PACKET_LEN) ? (COMM_APP_CMD_PACKET_LEN - 1) : (data_len);

    uchPacketDataBuffArr[0] = dev_state;
    if ((HBD_PTR_NULL != data_array) && (0 != data_len))
    {
        memcpy(&uchPacketDataBuffArr[1], data_array, uchPacketLen);
    }

    if (0x04 == dev_state)
    {
        uchRetLen = (uint8_t)comm_packet_format(COMM_STATE_NO_ERR, COMM_CMD_APP_STATE, uchRetDataBuffArr, uchPacketDataBuffArr, COMM_APP_CMD_PACKET_LEN);
    }
    else
    {
        uchRetLen = (uint8_t)comm_packet_format(COMM_STATE_NO_ERR, COMM_CMD_APP_STATE, uchRetDataBuffArr, uchPacketDataBuffArr, data_len + 1);
    }

    if (pSendDataFunc != HBD_PTR_NULL)
    {
        pSendDataFunc(uchRetDataBuffArr, uchRetLen);
    }
}

void comm_send_app_cmd_auto_led_fail(void (*pSendDataFunc)(uint8_t uchDataBuffArr[], uint8_t uchLen))
{
    comm_send_app_cmd(pSendDataFunc, 0x01, HBD_PTR_NULL, 0);
}

void comm_send_app_cmd_reset(void (*pSendDataFunc)(uint8_t uchDataBuffArr[], uint8_t uchLen))
{
    comm_send_app_cmd(pSendDataFunc, 0x02, HBD_PTR_NULL, 0);
}

void comm_send_app_cmd_battery_low(void (*pSendDataFunc)(uint8_t uchDataBuffArr[], uint8_t uchLen))
{
    comm_send_app_cmd(pSendDataFunc, 0x03, HBD_PTR_NULL, 0);
}

void comm_send_app_cmd_hb_algo_val(void (*pSendDataFunc)(uint8_t uchDataBuffArr[], uint8_t uchLen),
                                    uint8_t hb_aval, uint8_t wear_sval, uint8_t wear_qval, uint8_t sc_bval,
                                    uint8_t lvl_lval, uint8_t func_rval, uint16_t rri_rval)
{
    uint8_t buffer[20] = {0};

    buffer[0] = 0x00;
    buffer[1] = hb_aval;
    buffer[2] = wear_sval;
    buffer[3] = wear_qval;
    buffer[4] = sc_bval;
    buffer[5] = lvl_lval;
    buffer[6] = func_rval;
    buffer[7] = (uint8_t)((rri_rval >> 8) & 0xff);
    buffer[8] = (uint8_t)(rri_rval & 0xff);
    comm_send_app_cmd(pSendDataFunc, 0x04, buffer, 9);
}

void comm_send_app_cmd_hrv_algo_val(void (*pSendDataFunc)(uint8_t uchDataBuffArr[], uint8_t uchLen),
                            uint16_t rri1_rval, uint16_t rri2_rval, uint16_t rri3_rval, uint16_t rri4_rval,
                            uint8_t lvl_lval, uint8_t cnt_cval)
{
    uint8_t buffer[20] = {0};

    buffer[0] = 0x01;
    buffer[1] = (uint8_t)((rri1_rval >> 8) & 0xff);
    buffer[2] = (uint8_t)(rri1_rval & 0xff);
    buffer[3] = (uint8_t)((rri2_rval >> 8) & 0xff);
    buffer[4] = (uint8_t)(rri2_rval & 0xff);
    buffer[5] = (uint8_t)((rri3_rval >> 8) & 0xff);
    buffer[6] = (uint8_t)(rri3_rval & 0xff);
    buffer[7] = (uint8_t)((rri4_rval >> 8) & 0xff);
    buffer[8] = (uint8_t)(rri4_rval & 0xff);
    buffer[9] = lvl_lval;
    buffer[10] = cnt_cval;
    comm_send_app_cmd(pSendDataFunc, 0x04, buffer, 11);
}

void comm_send_app_cmd_spo2_algo_val(void (*pSendDataFunc)(uint8_t uchDataBuffArr[], uint8_t uchLen),
                            uint8_t spo2_rval, uint8_t spo2_reliable_rval, uint8_t hb_aval, uint8_t hb_reliable_rval,
							uint16_t rri1_rval, uint16_t rri2_rval, uint16_t rri3_rval, uint16_t rri4_rval,
                            uint8_t lvl_lval, uint8_t cnt_cval, uint16_t spo2R_rval)
{
    uint8_t buffer[20] = {0};

    buffer[0] = 0x02;
    buffer[1] = spo2_rval;
    buffer[2] = spo2_reliable_rval;
    buffer[3] = hb_aval; 
	buffer[4] = hb_reliable_rval;
    buffer[5] = (uint8_t)((rri1_rval >> 8) & 0xff);
    buffer[6] = (uint8_t)(rri1_rval & 0xff);
    buffer[7] = (uint8_t)((rri2_rval >> 8) & 0xff);
    buffer[8] = (uint8_t)(rri2_rval & 0xff);
    buffer[9] = (uint8_t)((rri3_rval >> 8) & 0xff);
    buffer[10] = (uint8_t)(rri3_rval & 0xff);
    buffer[11] = (uint8_t)((rri4_rval >> 8) & 0xff);
    buffer[12] = (uint8_t)(rri4_rval & 0xff);
    buffer[13] = lvl_lval;
    buffer[14] = cnt_cval;	
	buffer[15] = (uint8_t)((spo2R_rval >> 8) & 0xff);
    buffer[16] = (uint8_t)(spo2R_rval & 0xff);
    comm_send_app_cmd(pSendDataFunc, 0x04, buffer, 17);
}

uint16_t comm_packaging_rawdata_packet(uint8_t *packet_buff, uint16_t comm_max_len, uint8_t algo_type, uint16_t total_len,
                                        uint8_t packet_index, uint8_t algo_result_len, uint8_t data_array[], uint16_t data_len)
{
    uint16_t usPacketIndex = 0;
    uint16_t usHeaderLength = COMM_HEADER_LENGTH;
    uint16_t real_data_len = comm_max_len - 8; // 8 = 3(header + crc) + 5(cmd_header)

    if (0 == data_len)
    {
        return 0;
    }

    if (real_data_len > data_len)
    {
        real_data_len = data_len;
    }
    packet_buff[usPacketIndex++] = ((COMM_CMD_RWDATA_STATE << 4) & 0xF0 ) | (COMM_STATE_NO_ERR & 0x0F);
    packet_buff[usPacketIndex++] = COMM_VERSION;
    packet_buff[usPacketIndex++] = algo_type;
    packet_buff[usPacketIndex++] = (uint8_t)((total_len >> 8) & 0xFF);
    packet_buff[usPacketIndex++] = (uint8_t)(total_len & 0xFF);
    packet_buff[usPacketIndex++] = packet_index;
    packet_buff[usPacketIndex++] = algo_result_len;
    memcpy(&packet_buff[usPacketIndex], data_array, real_data_len);
    usPacketIndex += real_data_len;
    real_data_len += 5; // fixed add cmd_header len
    packet_buff[usPacketIndex++] = comm_packet_calc_crc(packet_buff, real_data_len + usHeaderLength);
    comm_packet_translate(packet_buff, usHeaderLength/* data index. */, real_data_len);

    return (usPacketIndex);
}

/********END OF FILE********* Copyright (c) 2003 - 2024, Goodix Co., Ltd. ********/

