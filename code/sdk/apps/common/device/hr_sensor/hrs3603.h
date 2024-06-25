#ifndef HRS3603_H_INCLUDED
#define HRS3603_H_INCLUDED

#include "typedef.h"
#include "asm/iic_hw.h"
#include "asm/iic_soft.h"
#include "os/os_api.h"
#include "system/includes.h"
#include "media/includes.h"
#include "asm/timer.h"

#define TCFG_HX3603_DEV_ENABLE   1
#define TCFG_HX3603_USE_IIC_TYPE 0
struct _hx3603_dev_platform_data {
    u8 iic_hdl;
    u8 iic_delay;          //这个延时并非影响iic的时钟频率，而是2Byte数据之间的延时
};
#define HX3603_ADDR        0x44
#define HX3603_RADDR       0x89
#define HX3603_WADDR       0x88


u8 hx3603_test();
u8 hx3603_chip_init(void *priv);





#define TIMER_READ_MODE

typedef struct {
    int32_t phase[3];
} hrs3603_data_t;

typedef enum {
    MSG_ALG_NOT_OPEN = 0x01,
    MSG_NO_TOUCH = 0x02,
    MSG_PPG_LEN_TOO_SHORT = 0x03,
    MSG_HR_READY = 0x04,
    MSG_ALG_TIMEOUT = 0x05,
    MSG_SETTLE = 0x06
} hrs3603_msg_code_t;

typedef enum {
    MSG_BP_ALG_NOT_OPEN = 0x01,
    MSG_BP_NO_TOUCH = 0x02,
    MSG_BP_PPG_LEN_TOO_SHORT = 0x03,
    MSG_BP_READY = 0x04,
    MSG_BP_ALG_TIMEOUT = 0x05,
    MSG_BP_SETTLE = 0x06
} hrs3603_bp_msg_code_t;

//add ericy 20180426
typedef enum {
    MSG_SPO2_ALG_NOT_OPEN = 0x01,
    MSG_SPO2_NO_TOUCH = 0x02,
    MSG_SPO2_PPG_LEN_TOO_SHORT = 0x03,
    MSG_SPO2_READY = 0x04,
    MSG_SPO2_ALG_TIMEOUT = 0x05,
    MSG_SPO2_SETTLE = 0x06
} hrs3603_spo2_msg_code_t;

typedef struct {
    hrs3603_spo2_msg_code_t  spo2_alg_status;
    uint32_t                 data_cnt;
    uint16_t                 spo2_result;
    uint8_t                  hr_result;

} hrs3603_spo2_results_t;

typedef struct {
    uint8_t                 red_offset_idac;
    uint8_t                 ir_offset_idac;
    uint16_t                spo2_start_cnt;

} hrs3603_spo2_agcpara_t;

typedef enum {
    MSG_BFR_ALG_NOT_OPEN = 0x01,
    MSG_BFR_NO_TOUCH = 0x02,
    MSG_BFR_PPG_LEN_TOO_SHORT = 0x03,
    MSG_BFR_READY = 0x04,
    MSG_BFR_ALG_TIMEOUT = 0x05,
    MSG_BFR_SETTLE = 0x06
} hrs3603_bfr_msg_code_t;

//add ericy 20180428
typedef enum {
    DEFAULT_MOTION = 0,
    STATIC_MOTION = 1,
    WALKING_MOTION = 2,
    RUNING_MOTION = 3,
    CYCLING_MOTION = 4,
    SWINMING_MOTION = 5,
    TENNIS_MOTION = 6,
    HRM_MONITOR_MODE = 7
} MOIION_STATUS;

typedef struct {

    hrs3603_bfr_msg_code_t   bfr_alg_status;
    uint32_t                 data_cnt;
    uint16_t                 bfr_result;

} hrs3603_bfr_results_t;

//add ericy 20180428

typedef struct {
    hrs3603_msg_code_t alg_status;
    uint32_t           data_cnt;
    uint8_t            hr_result;
    uint8_t            hr_result_qual; // ericy add20170111
    u8               object_flg;
} hrs3603_results_t;


typedef struct {
    hrs3603_bp_msg_code_t bp_alg_status;
    uint8_t            sbp;
    uint8_t            dbp;
    uint32_t           data_cnt;
    uint8_t            hr_result; //20170614 ericy
    u8               object_flg;
} hrs3603_bp_results_t;



typedef struct {
    uint8_t  reg_value_id;
    uint8_t  reg_value_osc;
    uint8_t  reg_value_ldo;
    uint8_t  reg_value_led;
} hx3603_efuse_value;



#ifdef HRS_BLE_APP
typedef struct {
    int8_t vector_flag;
    uint8_t  hr_result;
    uint8_t alg_status;
    uint32_t data_cnt;
    uint16_t hrm_raw_data;
    uint16_t als_raw_data;
    uint16_t gsensor_x;
    uint16_t gsensor_y;
    uint16_t gsensor_z;
} rawdata_vector_t ;
#endif

//extern volatile oled_display_t oled_dis;
void Hrs3603_hr_alg_config(void);
void Hrs3603_spo2_alg_config(void);
bool Hrs3603_chip_check_id();
bool hx3603_chip_init(void *priv);
u8 hx3603_test();

void Hrs3603_chip_disable(void);
void hrs3603_disable();
void hrs3603_hr_enable(void);
void hrs3603_spo2_enable(void);

bool Hrs3603_write_reg(u8 addr, u8 data);
u8 Hrs3603_read_reg(u8 addr);
bool Hrs3603_read_hrs(u32 *hrm_data, u32 *als_data);
bool Hrs3603_read_ps1(u32 *infrared_data);
u8 Hrs3603_read_red_phase(u32 *red_raw_data, u32 *als_inred_data);
u8 Hrs3603_read_ir_phase(u32 *ir_raw_data, u32 *als_inir_data);
void Hrs3603_Int_handle(void);
int read_hrs3603_result(void *arg);
void Hrs3603_spo2_Int_handle(void);
int read_hrs3603_spo2_result(void *arg);
int Hrs3603_io_ctl(u8 cmd, void *arg);
#endif // HRS3603_H_INCLUDED

