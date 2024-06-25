#include "stdint.h"
#include "asm/iic_hw.h"
#include "asm/iic_soft.h"
#include "app_config.h"
#include "system/includes.h"

#if TCFG_HRS3300_EN
//////////////////////////////
#include "hrs3300.h"
#include "hrSensor_manage.h"
//#include "hrs3300_alg.h"
//////////////////////////////
//#define GSENSER_DATA

#define I2C_ADDR_HRS3300_W      0x88
#define I2C_ADDR_HRS3300_R      0x89

const uint8_t init_register_array[][2] = {
//{0x01, 0xd0}, //11010000  bit[7]=1,HRS enable;bit[6:4]=101,wait time=50ms,bit[3]=0,LED DRIVE=22 mA
//{0x01, 0xf0},   //11010000  bit[7]=1,HRS enable;bit[6:4]=101,wait time=50ms,bit[3]=0,LED DRIVE=22 mA v13.05
    {0x01, 0x08},
    {0x0c, 0x4e},   //00001110  bit[6]=0,LED DRIVE=22mA;bit[5]=0,sleep mode;p_pulse=1110,duty=50%
    {0x16, 0x78},   //01111000  bits[7:4]=0111,HRS 15bits
    {0x17, 0x0d},   //00001101  bits[7:5]=011,HRS gain 16*;bit[1]=0,HRS data=AD0
    {0x02, 0x00},
    {0x03, 0x00},
    {0x04, 0x00},
    {0x05, 0x00},
    {0x06, 0x00},
    {0x07, 0x00},
    {0x0b, 0x00},
    {0x0c, 0x6e},
};
#define INIT_ARRAY_SIZE (sizeof(init_register_array)/sizeof(init_register_array[0]))


// hrs3300 customer config
const uint8_t  hrs3300_bp_timeout_grade = 0;  // max 15
const uint8_t  hrs3300_agc_init_stage = 0x04;  // init AGC state
const uint8_t  hrs3300_bp_power_grade = 0;
const uint8_t  hrs3300_accurate_first_shot = 0;
const uint8_t  hrs3300_up_factor = 3;
const uint8_t  hrs3300_up_shift = 2;
const uint16_t hrs3300_AMP_LTH = 120;
const uint16_t hrs3300_hr_AMP_LTH = 150;
const uint16_t hrs3300_hr_PVAR_LTH = 10;
// hrs3300 customer config end

static bool hrs3300_power_up_flg = 0 ;
uint8_t reg_0x7f ;
uint8_t reg_0x80 ;
uint8_t reg_0x81 ;
uint8_t reg_0x82 ;

static int get_data_timer = 0;
static u8 hrs3300_is_idle = 1;
void heart_rate_meas_timeout_handler(void *p_context);

bool Hrs3300_write_reg(uint8_t addr, uint8_t data)
{
    // I2c custom  return  0(success), 1(fail)
    uint8_t data_buf = data;
    uint8_t write_len;

    y_printf("addr:0x%x , data:0x%x", addr, data_buf);
    write_len = hrsensor_write_nbyte(I2C_ADDR_HRS3300_W, addr, &data_buf, 1);

    if (write_len) {
        return 0;
    } else {
        return 1;
    }
}

uint8_t Hrs3300_read_reg(uint8_t addr)
{
    uint8_t data_buf = 0;
    hrsensor_read_nbyte(I2C_ADDR_HRS3300_R, addr, &data_buf, 1);
    return data_buf;
}

#ifdef MALLOC_MEMORY
void *hr_malloc(size_t size)
{
    return (void *)malloc(size);
}

void hr_free(void *ptr)
{
    free(ptr);
}
#endif

uint16_t Hrs3300_read_hrs(void)
{
    uint8_t  databuf[3];
    uint16_t data;

    databuf[0] = Hrs3300_read_reg(0x09);	// addr09, bit
    databuf[1] = Hrs3300_read_reg(0x0a);	// addr0a, bit
    databuf[2] = Hrs3300_read_reg(0x0f);	// addr0f, bit

    data = ((databuf[0] << 8) | ((databuf[1] & 0x0F) << 4) | (databuf[2] & 0x0F));

    return data;
}

uint16_t Hrs3300_read_als(void)
{
    uint8_t  databuf[3];
    uint16_t data;

    databuf[0] = Hrs3300_read_reg(0x08);	// addr09, bit [10:3]
    databuf[1] = Hrs3300_read_reg(0x0d);	// addr0a, bit [17:11]
    databuf[2] = Hrs3300_read_reg(0x0e);	// addr0f, bit [2:0]

    data = ((databuf[0] << 3) | ((databuf[1] & 0x3F) << 11) | (databuf[2] & 0x07));

    if (data > 32767) {
        data = 32767;    // prevent overflow of other function
    }

    return data;
}



uint8_t Hrs3300_chip_init()
{
    int i = 0 ;
    uint8_t id = 0;

    for (i = 0; i < 5; i++) {
        id = Hrs3300_read_reg(0x00);
        y_printf("id:0x%x", id);
        if (0x21 == id) {
            break;
        } else {
            os_time_dly(1);
        }
    }

    if (id != 0x21) {
        return false;
    }

    for (i = 0; i < INIT_ARRAY_SIZE; i++) {
        if (Hrs3300_write_reg(init_register_array[i][0],
                              init_register_array[i][1]) != 0) {
            return false;
        }
    }


    if (hrs3300_power_up_flg == 0) {
        reg_0x7f = Hrs3300_read_reg(0x7f) ;
        reg_0x80 = Hrs3300_read_reg(0x80) ;
        reg_0x81 = Hrs3300_read_reg(0x81) ;
        reg_0x82 = Hrs3300_read_reg(0x82) ;
        hrs3300_power_up_flg =  1;
    }

    return true;
}

void Hrs3300_chip_enable()
{
    hrs3300_is_idle = 0;

    Hrs3300_write_reg(0x16, 0x78);
    Hrs3300_write_reg(0x01, 0xd0);
    Hrs3300_write_reg(0x0c, 0x2e);

    Hrs3300_alg_open();

    get_data_timer = sys_timer_add(NULL, heart_rate_meas_timeout_handler, 40);
    return ;
}

void Hrs3300_chip_disable()
{
    if (get_data_timer) {
        sys_timer_del(get_data_timer);
        get_data_timer = 0;
        os_time_dly(2);
    }

    Hrs3300_alg_close();

    Hrs3300_write_reg(0x01, 0x08);
    Hrs3300_write_reg(0x0c, 0x4e);

    hrs3300_is_idle = 1;
    return ;
}

void heart_rate_meas_timeout_handler(void *p_context)
{
    uint32_t err_code;
    uint16_t heart_rate;
    uint16_t hrm_raw_data;
    uint16_t als_raw_data;
    uint8_t gsen_data;
    hrs3300_results_t alg_results;
#ifdef BP_CUSTDOWN_ALG_LIB
    hrs3300_bp_results_t	bp_alg_results ;
#endif
    static uint16_t timer_index = 0;
#ifdef GSENSER_DATA
    AxesRaw_t gsen_buf;
#endif

    //    UNUSED_PARAMETER(p_context);
#ifdef GSENSER_DATA
    LIS3DH_GetAccAxesRaw(&gsen_buf);
#endif
    hrm_raw_data = Hrs3300_read_hrs();
    als_raw_data = Hrs3300_read_als();  // 20170430
    /* r_printf("h:%d a:%d", hrm_raw_data, als_raw_data); */

#ifdef GSENSER_DATA
    Hrs3300_alg_send_data(hrm_raw_data, als_raw_data, gsen_buf.AXIS_X, gsen_buf.AXIS_Y, gsen_buf.AXIS_Z, 0);
#else
    Hrs3300_alg_send_data(hrm_raw_data, als_raw_data, 0, 0, 0, 0);
#endif
#if 0
    alg_results = Hrs3300_alg_get_results();
    SEGGER_RTT_printf(0, "%d %d %d %d %d %d %d\n", hrm_raw_data, als_raw_data, gsen_buf.AXIS_X, gsen_buf.AXIS_Y, gsen_buf.AXIS_Z, alg_results.hr_result, alg_results.alg_status);
#endif

    timer_index ++;
    if (timer_index >= 25)  {    // get result per second
        timer_index = 0;
        alg_results = Hrs3300_alg_get_results();

        if (alg_results.alg_status == MSG_NO_TOUCH) {
            r_printf("MSG_NO_TOUCH---");
        } else if (alg_results.alg_status == MSG_PPG_LEN_TOO_SHORT) {
            y_printf("MSG_PPG_LEN_TOO_SHORT----");
        } else {
#ifdef BP_CUSTDOWN_ALG_LIB
            bp_alg_results = Hrs3300_alg_get_bp_results();
            if (bp_alg_results.sbp != 0) {
                //opr_display_bp(bp_alg_results.sbp, bp_alg_results.dbp);
            }
#endif
            g_printf("alg_results.hr_result:%d", alg_results.hr_result);
        }
    }
}

void Hrs3300_io_ctl(u8 cmd, void *arg)
{
    switch (cmd) {
    case HR_SENSOR_ENABLE:
        Hrs3300_chip_enable();
        break;
    case HR_SENSOR_DISABLE:
        Hrs3300_chip_disable();
        break;
    }
}

REGISTER_HR_SENSOR(hrSensor) = {
    .logo = "hrs3300",
    .heart_rate_sensor_init  = Hrs3300_chip_init,
    .heart_rate_sensor_check = NULL,
    .heart_rate_sensor_ctl   = Hrs3300_io_ctl,
};

static u8 hrs3300_idle_query(void)
{
    return hrs3300_is_idle;
}

REGISTER_LP_TARGET(hrs3300_lp_target) = {
    .name = "hrs3300",
    .is_idle = hrs3300_idle_query,
};

#endif // TCFG_HRS3300_EN

