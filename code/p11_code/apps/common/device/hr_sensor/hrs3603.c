#include "app_config.h"
#include "includes.h"
#include "gpio.h"
#include "hrs3603.h"
#include "hrSensor_manage.h"
#include "iic_api.h"
#include "sensor_gp_timer.h"
#include "timer.h"
#include "sensor_api.h"
#include "msg.h"
#include "bank_switch.h"
#include "sensor_api.h"

/* #pragma bss_seg(".sport_dev_bss") */
/* #pragma const_seg(".sport_dev_const") */
/* #pragma code_seg(".sport_dev_code") */
/*  */

#if defined(TCFG_HRS3603_DEV_ENABLE) && TCFG_HRS3603_DEV_ENABLE


#define BANK_NAME  BANK_SENSOR_TAG


#define log_i(...)

//hr3 通道 spo2 4通道,buf 复用25 hz
#define HR_SPO2_DATALEN ((25+5)*4)//需与大于单次传入算法的数据量

static u8 mode = 0;
static u8 timer_start = 0;
static u8 sensor_online = 0;
static cbuffer_t *data_w_cbuf[2];
static u32 hrdata[HR_SPO2_DATALEN];


void hrs3603_timers_start()//定时器轮询模式：开始血氧采集（初始化成功后调用）
{
    timer_start = 1;
}

void hrs3603_timers_stop()//定时器轮询模式：开始血氧采集（初始化成功后调用）
{
    timer_start = 0;
}

/****************************iic*****************************/
bool Hrs3603_write_reg(u8 addr, u8 data)
{
    // 根据不同平台实现i2c, sucess: return 0; fail: retrun 1;
    if (hrsensor_write_nbyte(HX3603_WADDR, addr, &data, 1) == 1) {
        return 0;
    } else {
        return 1;
    }
}
u8 Hrs3603_read_reg(u8 register_addr)
{
    uint8_t data_buf = 0;

    if (hrsensor_read_nbyte(HX3603_RADDR, register_addr, &data_buf, 1)) {
        return data_buf;
    } else {
        return 0;
    }
}

/****************************hx3603*****************************/
bool Hrs3603_chip_check_id()
{
    int i = 0 ;
    uint8_t chip_id = 0;

    chip_id = Hrs3603_read_reg(0x00) ;
    log_i("--hrs3603 id!0x%x\n", chip_id);
    if (chip_id != 0x23) {
        log_i("--------read hx3603 id error!");
        return false;
    }
    log_i("--------read hx3603 id ok!");

    return true;
}

void Hrs3603_chip_disable(void)
{
    /* printf("%s\n",__func__); */
    Hrs3603_write_reg(0x01, 0x01);
    Hrs3603_write_reg(0x02, 0x01);
    mdelay(50);
    Hrs3603_write_reg(0x1a, 0x13);
    mdelay(50);
}

void hrs3603_disable(void)//关闭定时器
{
    /* printf("%s",__func__); */
    hrs3603_timers_stop();//关闭定时器
    Hrs3603_chip_disable(); // 此时功耗在 1ua 以下
}
void hrs3603_hr_enable(void) //从低功耗恢复
{
    /* printf("%s\n",__func__);  */
    mode = 0;
    Hrs3603_chip_disable(); // 此时功耗在 1ua 以下
    Hrs3603_hr_alg_config();//重置寄存器
    hrs3603_timers_start();//开定时器
}
void hrs3603_spo2_enable(void)
{
    /* printf("%s\n",__func__); */
    mode = 1;
    Hrs3603_chip_disable(); // 此时功耗在 1ua 以下
    Hrs3603_spo2_alg_config();
    hrs3603_timers_start();
}


REGISTER_CODE_SEG(BANK_CODE_SENSOR_INIT_BANK_0)
u8 hx3603_init(cbuffer_t  *hrcbuf, cbuffer_t  *spo2cbuf)
{
    /* printf("%s",__func__); */
    mdelay(40);//芯片上电请延时5ms

    if (!hrcbuf || !spo2cbuf) {
        ASSERT(0);
    }
    data_w_cbuf[0] = hrcbuf;
    data_w_cbuf[1] = spo2cbuf;

    cbuf_init(data_w_cbuf[0], hrdata, sizeof(hrdata));
    cbuf_init(data_w_cbuf[1], hrdata, sizeof(hrdata));

    if (Hrs3603_chip_check_id()) {
        /* hx3603_timer_init(); */
        log_i("%s1", __func__);
        //hrs3603_spo2_enable();
        /* Hrs3603_hr_alg_config(); */
        hrs3603_timers_stop();
        /* hrs3603_disable();//先关灯； */
        //
        //
        sensor_online = 1;
        return 1;
    }
    log_i("%s2", __func__);
    return 0;
}
int hrs3603_alg_send_data_test(u32 als_raw_data, u32 infrared_data)
{
    /* printf("%s %d %d \n",__func__,als_raw_data,infrared_data);	 */
    static u8 send_data = 0x04;
    static u8 hx_close = 0, hx_open = 0;
    u8 wcnt = 0;
    if (infrared_data > 30000) {
        hx_open++;
        if (hx_open == 2) {
            hx_open--;
            if ((als_raw_data > 450000) || (hx_close == 2)) {
                send_data += 2;
                if (send_data > 0x7a) {
                    send_data = 0x7a;
                    return true;
                }
                if (send_data == 0x06) {
                    /* printf("\n%s1\n",__func__); */
                    wcnt += Hrs3603_write_reg(0x0d, 0x63);
                    wcnt += Hrs3603_write_reg(0x06, 0x0a);
                    wcnt += Hrs3603_write_reg(0x0b, 0x73);
                    wcnt += Hrs3603_write_reg(0x03, 0x0f);
                } else {
                    wcnt += Hrs3603_write_reg(0x06, send_data);
                }
            } else if (als_raw_data < 200000) {
                send_data -= 2;
                if (send_data < 0x06) {
                    send_data = 0x06;
                    return true;
                }
                wcnt += Hrs3603_write_reg(0x06, send_data);
            }
            hx_close = 0;
        }
    } else {
        hx_close++;
        if (hx_close == 2) {
            hx_open = 0;
            send_data = 0x04;
            /* printf("\n%s2  %d\n",__func__,infrared_data); */
            wcnt += Hrs3603_write_reg(0x03, 0x07);
        } else if (hx_close == 3) {
            hx_close--;
        }
    }
    if (wcnt) {
        return false;
    } else {
        return true;
    }
}
hrs3603_spo2_msg_code_t hx3603_state = MSG_SPO2_ALG_NOT_OPEN;
int hrs3603_spo2_agc_test(u32 red_new_raw_data, u32 als_in_red_raw_data, u32 ir_new_raw_data, u32 als_in_ir_raw_data)
{
    static u8 red_on_state = 2, ir_on_state = 2; //默认刚开机
    static s16 red06_data = 0;
    static s16 ir05_data = 0;
    u8 wcnt = 0;
    //先加减
    if (red_new_raw_data >= 450000) {
        red06_data++;
        if (red_on_state == 2) {
            red_on_state = 1;
            red06_data = 0x1f;
        }
        Hrs3603_read_reg(0x0d);
        if (red06_data <= 0x7f) {
            wcnt += Hrs3603_write_reg(0x06, red06_data);
        } else {
            red06_data = 0x7f;
        }
    } else if (red_new_raw_data <= 250000) {
        red06_data--;
        if (red_on_state == 2) {
            red_on_state = 1;
            red06_data = 0x1d;
        }
        Hrs3603_read_reg(0x0d);
        if (red06_data >= 0x00) {
            wcnt += Hrs3603_write_reg(0x06, red06_data);
        } else {
            red06_data = 0x00;
        }
    }
    if (ir_new_raw_data >= 450000) {
        ir05_data++;
        if (ir_on_state == 2) {
            ir_on_state = 1;
            ir05_data = 0x1f;
        }
        Hrs3603_read_reg(0x0d);
        if (ir05_data <= 0x7f) {
            wcnt += Hrs3603_write_reg(0x05, ir05_data);
        } else {
            ir05_data = 0x7f;
        }
    } else if (ir_new_raw_data <= 250000) {
        ir05_data--;
        if (ir_on_state == 2) {
            ir_on_state = 1;
            ir05_data = 0x1d;
        }
        Hrs3603_read_reg(0x0d);
        if (ir05_data >= 0x00) {
            wcnt += Hrs3603_write_reg(0x05, ir05_data);
        } else {
            ir05_data = 0x00;
        }
    }
    if (hx3603_state == MSG_SPO2_ALG_NOT_OPEN) {
        hx3603_state = MSG_SPO2_PPG_LEN_TOO_SHORT; //3
    }
    //开关机
    if (ir_new_raw_data - als_in_ir_raw_data < 100000) { //关机
        if (ir_on_state) {
            wcnt += Hrs3603_write_reg(0x03, 0x07);
            wcnt += Hrs3603_write_reg(0x05, 0x00);
            wcnt += Hrs3603_write_reg(0x06, 0x00);
            ir_on_state = 0;
            red_on_state = 0;
            red06_data = 0x00;
            ir05_data = 0x00;
            hx3603_state = MSG_SPO2_NO_TOUCH; //2
        }
    } else {
        if (ir_on_state == 0) {
            wcnt += Hrs3603_write_reg(0x03, 0x0f);
            ir_on_state = 2; //刚开机
            red_on_state = 2; //刚开机
            hx3603_state = MSG_SPO2_ALG_NOT_OPEN; //1
        }
    }
    if (wcnt) {
        return false;
    } else {
        return true;
    }
}



/**********************数据处理*****************************/

bool Hrs3603_brust_read_reg(uint8_t register_addr, uint8_t *buf, uint8_t length)
{
    if (hrsensor_read_nbyte(HX3603_RADDR, register_addr, buf, length)) {
        return true;
    } else {
        printf("\n%s\n", __func__);
        return false;
    }
}
bool Hrs3603_read_hrs(u32 *hrm_data, u32 *als_data)
{
//	log_i("%s",__func__);
    uint8_t  databuf[6] = {0};
    int32_t P0 = 0, P1 = 0;

    if (Hrs3603_brust_read_reg(0xa0, databuf, 6) == false) {
        printf("\n%s\n", __func__);
        return false;
    }

    P0 = ((databuf[0]) | (databuf[1] << 8) | (databuf[2] << 16));
    P1 = ((databuf[3]) | (databuf[4] << 8) | (databuf[5] << 16));

    if (P0 > P1) {
        *hrm_data = P0 - P1;
    } else {
        *hrm_data = 0;
    }
    *als_data = P0;

    return true;
}
bool Hrs3603_read_ps1(u32 *infrared_data)
{
//   log_i("%s",__func__);
    uint8_t  databuf[6] = {0};
    int32_t P0 = 0, P1 = 0;
    if (Hrs3603_brust_read_reg(0xa6, databuf, 6) == false) {
        printf("\n%s\n", __func__);
        return false;
    }
    P0 = ((databuf[0]) | (databuf[1] << 8) | (databuf[2] << 16));
    P1 = ((databuf[3]) | (databuf[4] << 8) | (databuf[5] << 16));
    if (P0 > P1) {
        *infrared_data = P0 - P1;
    } else {
        *infrared_data = 0;
    }
    return true;
}
u8 Hrs3603_read_red_phase(u32 *red_raw_data, u32 *als_inred_data)
{
//	log_i("%s",__func__);
    uint8_t  databuf[6] = {0};
    u32 P0 = 0, P1 = 0;
    if (Hrs3603_brust_read_reg(0xa0, databuf, 6) == false) {
        printf("\n%s\n", __func__);
        return false;
    }

    P0 = ((databuf[0]) | (databuf[1] << 8) | (databuf[2] << 16));
    P1 = ((databuf[3]) | (databuf[4] << 8) | (databuf[5] << 16));

    *red_raw_data = P0;
    *als_inred_data = P1;
    if ((P0 > 0x7FFFF) | (P1 > 0x7FFFF)) {
        printf("P0=%d,%d %d %d", P0, databuf[0], databuf[1], databuf[2]);
        printf("P1=%d,%d %d %d", P0, databuf[3], databuf[4], databuf[5]);
    }
    return true;
}

u8 Hrs3603_read_ir_phase(u32 *ir_raw_data, u32 *als_inir_data)
{
//	log_i("%s",__func__);
    uint8_t  databuf[6] = {0};
    u32 P0 = 0, P1 = 0;
    if (Hrs3603_brust_read_reg(0xa6, databuf, 6) == false) {
        printf("\n%s\n", __func__);
        return false;
    }
    P0 = ((databuf[0]) | (databuf[1] << 8) | (databuf[2] << 16));
    P1 = ((databuf[3]) | (databuf[4] << 8) | (databuf[5] << 16));

    *ir_raw_data = P0;
    *als_inir_data = P1;
    if ((P0 > 0x7FFFF) | (P1 > 0x7FFFF)) {
        printf("P0=%d,%d %d %d", P0, databuf[0], databuf[1], databuf[2]);
        printf("P1=%d,%d %d %d", P0, databuf[3], databuf[4], databuf[5]);
    }

    return true;
}

void Hrs3603_hr_alg_config(void)
{
    /* printf("%s\n",__func__); */
    uint16_t sample_rate = 25;                     /*config the data rate of chip frog3,uint is Hz,GREEN-25,IR-800*/
    uint32_t prf_clk_num = 32000 / sample_rate;    /*period in clk num, num=Fclk/fs*/
    uint16_t ps1_interval_i2c = 25;                 /*PS1 PRF间隔0-255* 0x1c, 25 mean 1s */
    /*********phase enable********/
    uint8_t hrs_enable = 1;                        /*heart rhythm society enable,1:enable  0:disable*/
    uint8_t ps0_enable = 0;                        /*ALS enable, 1:enable  0:disable*/
    uint8_t ps1_enable = 1;                        /*proximity sensor enable, 1:enable  0:disable*/
    uint8_t ts_enable = 0;
    /********osr config********/
    uint16_t hrs_ps0_ts_osr = 3;                   /*TS/PS0/HRS OSR:  0:128 1:256 2:512 3:1023*/
    uint16_t ps1_osr = 3;                          /*PS1 OSR:  0:128 1:256 2:512 3:1023*/
    /***********cap *********/
    uint8_t hrs_ps0_ts_int_cap = 15;                /*INTCAPSEL HRS/TS/PS0  0000 3.125pF, 1111 50pF  liner*/
    uint8_t ps1_int_cap = 15;                       /*INTCAPSEL PS1  0000 3.125pF, 1111 50pF  liner*/

    /***********led open enable***********/
    uint8_t hrs_led_en = 1;                         /*PS0/HRS LED  1:enable  0:disable */
    uint8_t ps1_led_en = 1;                         /*PS1 LED  1:enable  0:disable */

    uint8_t hrs_ckafe_en = 1;                       /*CKAFEINT  1:enable  0:disable */
    uint8_t ps1_ckafe_en = 1;

    uint8_t dccancel_hrs_idac = 0;                    /*offset DCCANCEL HRS/TS/PS0*/
    uint8_t dccancel_ps1_idac = 0;                    /*offset DCCANCEL PS1*/

    uint8_t hrs_leddr_msb = 3;                      /* 0~7 , 1= 25ma , step = 25ma*/
    uint8_t ps1_leddr_msb = 7;                      /* 0~7 , 1= 25ma , step = 25ma*/
    uint8_t leddr_lsb = 7;                          /* 0~7 , 1= 3.125ma , step = 3.125ma*/

    /**********led drive selsct**********/
    uint8_t force_ledsel_sel = 0;                   /*force_ledsel_sel  0:hrs  1:ps1*/
    uint8_t ledsel_ps1 = 2;                         /*LDR SEL 001:LDR0(green)   010:LDR1(IR)   100:LDR2(RED)*/
    uint8_t ledsel_hrs = 1;                         /*LDR SEL 001:LDR0(green)   010:LDR1(IR)   100:LDR2(RED)*/
    /***************************/
//    uint8_t pd_drive_sel=1;                         /*force_pddrive_sel*/
//    uint8_t pd_drive_ps1=1;

    uint8_t rfsel_hrs = 6;           /* 0~7 , 0=10K, 1=20k, 2=40k, 3=80k, 4=160k, 5=320k, 6=640k, 7=1280K*/
    uint8_t rfsel_ps1 = 1;           /* 0~7 , 0=10K, 1=20k, 2=40k, 3=80k, 4=160k, 5=320k, 6=640k, 7=1280K*/

    uint8_t hrs_ckafe_cycle = 50;           /*hrs ledontime, 0~255,  led on time = 4*hrs_ckafe_cycle*0.382us */
    uint8_t ps1_ckafe_cycle = 50;           /*ps ledontime, 0~255,  led on time = 4*hrs_ckafe_cycle*0.382us */

    /********tia or cap mode*******/
    uint8_t tia_hrs_en = 1;                         /*tia_hrs_en*/
    uint8_t tia_ps1_en = 1;                         /*tia_ps1_en*/
    /**********pd config*********/
    uint8_t ext_pd_sel = 1;                         /*EXT_PD_DSEL 1 VIP_EXT to PD negative pin; 0 VIN_ECT to PD negative pin*/
    uint8_t extpd_ps1_en = 1;                       /*extpd_ps1_en*/
    uint8_t extpd_hrs_en = 1;                       /*extpd_hrs_en*/

    uint8_t reset_cycle = 5;                        /*3bit reset_cycle_sel  (2^(reset_cycle+1))-1*/

//   uint8_t en2rst_delay=128;                       /*6bit en2rst_delay*/

    /************self test mode***********/
    uint8_t selftest_adc_afe = 0;                   /*self-test for AFE+ADC*/
    uint8_t selftest_adc = 0;                       /*self-test for ADC*/

    uint8_t adc_data_clk_pol = 3;                   /*adc_data_clk_pol  1:neg  0:pos */

    Hrs3603_write_reg(0x1e, 0x00);

    Hrs3603_write_reg(0x10, (uint8_t)(prf_clk_num & 0xff));
    Hrs3603_write_reg(0x11, (uint8_t)((prf_clk_num & 0x0f00) >> 8));

    Hrs3603_write_reg(0x01, (ts_enable << 4) | (ps0_enable << 3) | (hrs_enable << 2) | hrs_ps0_ts_osr);
    Hrs3603_write_reg(0x02, (ps1_enable << 2) | ps1_osr);
    Hrs3603_write_reg(0x03, (hrs_led_en << 3) | (ps1_led_en << 2) | (hrs_ckafe_en << 1) | ps1_ckafe_en);
    Hrs3603_write_reg(0x04, reset_cycle);
    Hrs3603_write_reg(0x08, hrs_ckafe_cycle);
    Hrs3603_write_reg(0x09, ps1_ckafe_cycle);
    Hrs3603_write_reg(0x1c,  ps1_interval_i2c);
    Hrs3603_write_reg(0x05, dccancel_ps1_idac);
    Hrs3603_write_reg(0x06, dccancel_hrs_idac);
    Hrs3603_write_reg(0x07, (ledsel_ps1 << 4) | (force_ledsel_sel << 3) | ledsel_hrs);
    Hrs3603_write_reg(0x0a, (ps1_int_cap << 4) | hrs_ps0_ts_int_cap);

    //Hrs3603_write_reg(0x0b,(pd_drive_sel<<3)|(pd_drive_ps1<<4)|((led_drive&0x38)>>3));
    Hrs3603_write_reg(0x0b, ((ps1_leddr_msb << 4) | hrs_leddr_msb) & 0x77);

    Hrs3603_write_reg(0xc4, (ext_pd_sel << 7) | ((leddr_lsb & 0x07) << 4) | 0x0f);
    Hrs3603_write_reg(0x0d, (rfsel_hrs << 4) | (rfsel_ps1));
    Hrs3603_write_reg(0x0c, rfsel_ps1);
    Hrs3603_write_reg(0x0f, 0x00);
    Hrs3603_write_reg(0x0e, (extpd_ps1_en << 3) | (extpd_hrs_en << 2) | (tia_ps1_en << 1) | tia_hrs_en);
    Hrs3603_write_reg(0x1b, 0x7f);
    Hrs3603_write_reg(0xc2, 0x10 | (selftest_adc_afe << 7) | (selftest_adc << 6)); // test mode
    Hrs3603_write_reg(0xc3, 0xff);
    Hrs3603_write_reg(0x18, adc_data_clk_pol);
    Hrs3603_write_reg(0x1a, 0x12);
    Hrs3603_write_reg(0x13, 0x02);  //enable hrs int
    Hrs3603_write_reg(0x12, 0x50);  //
    Hrs3603_write_reg(0x20, 0x20);  //close fifo int
}
void Hrs3603_spo2_alg_config(void)
{
    /* printf("%s\n",__func__); */
//	log_i("%s",__func__);
    uint16_t sample_rate = 25;                     /*config the data rate of chip frog3,uint is Hz,GREEN-25,IR-800*/
    uint32_t prf_clk_num = 32000 / sample_rate;    /*period in clk num, num=Fclk/fs*/
    uint16_t ps1_interval_i2c = 0;                 /*PS1 PRF间隔0-255* 0x1c, 25 mean 1s */

    /*********phase enable********/
    uint8_t hrs_enable = 1;                        /*heart rhythm society enable,1:enable  0:disable*/
    uint8_t ps0_enable = 0;                        /*ALS enable, 1:enable  0:disable*/
    uint8_t ps1_enable = 1;                        /*proximity sensor enable, 1:enable  0:disable*/
    uint8_t ts_enable = 0;
    /********osr config********/
    uint16_t hrs_ps0_ts_osr = 3;                   /*TS/PS0/HRS OSR:  0:128 1:256 2:512 3:1023*/
    uint16_t ps1_osr = 3;                          /*PS1 OSR:  0:128 1:256 2:512 3:1023*/
    /***********cap *********/
    uint8_t hrs_ps0_ts_int_cap = 15;                /*INTCAPSEL HRS/TS/PS0  0000 3.125pF, 1111 50pF  liner*/
    uint8_t ps1_int_cap = 15;                       /*INTCAPSEL PS1  0000 3.125pF, 1111 50pF  liner*/

    /***********led open enable***********/
    uint8_t hrs_led_en = 1;                         /*PS0/HRS LED  1:enable  0:disable */
    uint8_t ps1_led_en = 1;                         /*PS1 LED  1:enable  0:disable */

    uint8_t hrs_ckafe_en = 1;                       /*CKAFEINT  1:enable  0:disable */
    uint8_t ps1_ckafe_en = 1;

    uint8_t dccancel_hrs_idac = 0;                    /*offset DCCANCEL HRS/TS/PS0*/
    uint8_t dccancel_ps1_idac = 0;                    /*offset DCCANCEL PS1*/

    uint8_t hrs_leddr_msb = 6;                      /* 0~7 , 1= 25ma , step = 25ma*/
    uint8_t ps1_leddr_msb = 6;                      /* 0~7 , 1= 25ma , step = 25ma*/
    uint8_t leddr_lsb = 7;                          /* 0~7 , 1= 3.125ma , step = 3.125ma*/

    /**********led drive selsct**********/
    uint8_t force_ledsel_sel = 0;                   /*force_ledsel_sel  0:hrs  1:ps1*/
    uint8_t ledsel_ps1 = 2;                         /*LDR SEL 001:LDR0(green)   010:LDR1(red)   100:LDR2(IR)*/
    uint8_t ledsel_hrs = 4;                         /*LDR SEL 001:LDR0(green)   010:LDR1(red)   100:LDR2(IR)*/
    /***************************/
//    uint8_t pd_drive_sel=1;                         /*force_pddrive_sel*/
//    uint8_t pd_drive_ps1=1;

    uint8_t rfsel_hrs = 7;           /* 0~7 , 0=10K, 1=20k, 2=40k, 3=80k, 4=160k, 5=320k, 6=640k, 7=1280K*/
    uint8_t rfsel_ps1 = 7;           /* 0~7 , 0=10K, 1=20k, 2=40k, 3=80k, 4=160k, 5=320k, 6=640k, 7=1280K*/

    uint8_t hrs_ckafe_cycle = 255;           /*hrs ledontime, 0~255,  led on time = 4*hrs_ckafe_cycle*0.382us */
    uint8_t ps1_ckafe_cycle = 255;           /*ps ledontime, 0~255,  led on time = 4*hrs_ckafe_cycle*0.382us */

    /********tia or cap mode*******/
    uint8_t tia_hrs_en = 1;                         /*tia_hrs_en*/
    uint8_t tia_ps1_en = 1;                         /*tia_ps1_en*/
    /**********pd config*********/
    uint8_t ext_pd_sel = 1;                         /*EXT_PD_DSEL 1 VIP_EXT to PD negative pin; 0 VIN_ECT to PD negative pin*/
    uint8_t extpd_ps1_en = 1;                       /*extpd_ps1_en*/
    uint8_t extpd_hrs_en = 1;                       /*extpd_hrs_en*/

    uint8_t reset_cycle = 7;                        /*3bit reset_cycle_sel  (2^(reset_cycle+1))-1*/

//   uint8_t en2rst_delay=128;                       /*6bit en2rst_delay*/

    /************self test mode***********/
    uint8_t selftest_adc_afe = 0;                   /*self-test for AFE+ADC*/
    uint8_t selftest_adc = 0;                       /*self-test for ADC*/

    uint8_t adc_data_clk_pol = 3;                   /*adc_data_clk_pol  1:neg  0:pos */
    Hrs3603_write_reg(0x1e, 0x00);
    Hrs3603_write_reg(0x10, (uint8_t)(prf_clk_num & 0xff));
    Hrs3603_write_reg(0x11, (uint8_t)((prf_clk_num & 0x0f00) >> 8));
    Hrs3603_write_reg(0x01, (ts_enable << 4) | (ps0_enable << 3) | (hrs_enable << 2) | hrs_ps0_ts_osr);
    Hrs3603_write_reg(0x02, (ps1_enable << 2) | ps1_osr);
    Hrs3603_write_reg(0x03, (hrs_led_en << 3) | (ps1_led_en << 2) | (hrs_ckafe_en << 1) | ps1_ckafe_en);
    Hrs3603_write_reg(0x04, reset_cycle);
    Hrs3603_write_reg(0x08, hrs_ckafe_cycle);
    Hrs3603_write_reg(0x09, ps1_ckafe_cycle);
    Hrs3603_write_reg(0x1c,  ps1_interval_i2c);
    Hrs3603_write_reg(0x05, dccancel_ps1_idac);
    Hrs3603_write_reg(0x06, dccancel_hrs_idac);
    Hrs3603_write_reg(0x07, (ledsel_ps1 << 4) | (force_ledsel_sel << 3) | ledsel_hrs);
    Hrs3603_write_reg(0x0a, (ps1_int_cap << 4) | hrs_ps0_ts_int_cap);
    //Hrs3603_write_reg(0x0b,(pd_drive_sel<<3)|(pd_drive_ps1<<4)|((led_drive&0x38)>>3));
    Hrs3603_write_reg(0x0b, ((ps1_leddr_msb << 4) | hrs_leddr_msb) & 0x77);
    Hrs3603_write_reg(0xc4, (ext_pd_sel << 7) | ((leddr_lsb & 0x07) << 4) | 0x0f);
    Hrs3603_write_reg(0x0d, (rfsel_hrs << 4) | (rfsel_ps1));
    Hrs3603_write_reg(0x0c, rfsel_ps1);
    Hrs3603_write_reg(0x0f, 0x00);
    Hrs3603_write_reg(0x0e, (extpd_ps1_en << 3) | (extpd_hrs_en << 2) | (tia_ps1_en << 1) | tia_hrs_en);
    Hrs3603_write_reg(0x1b, 0x7f);
    Hrs3603_write_reg(0xc2, 0x10 | (selftest_adc_afe << 7) | (selftest_adc << 6)); // test mode
    Hrs3603_write_reg(0xc3, 0xff);
    Hrs3603_write_reg(0x18, adc_data_clk_pol);
    Hrs3603_write_reg(0x1a, 0x12);
    Hrs3603_write_reg(0x0f, 0x00); //  add ericy 20201106
    Hrs3603_write_reg(0x13, 0x01);  //enable ps1 int
    Hrs3603_write_reg(0x12, 0x50);  //
    Hrs3603_write_reg(0x20, 0x20);  //close fifo int
}


/* static void mebug_io_1(u8 i) */
/* { */
/*     P11_PORT->PB_SEL |= BIT(i); 	//P11 Control */
/*     P11_PORT->PB_DIR &= ~BIT(i); 	//dir output */
/*     P11_PORT->PB_OUT |= BIT(i); 	//output 1 */
/* } */
/*  */
/*  */
/* static void mebug_io_0(u8 i) */
/* { */
/*     P11_PORT->PB_SEL |= BIT(i); 	//P11 Control */
/*     P11_PORT->PB_DIR &= ~BIT(i); 	//dir output */
/*     P11_PORT->PB_OUT &= ~BIT(i); 	//output 0 */
/* } */
/*  */

static void Hrs3603_40ms_timer_Cb()
{
    /* printf("%s timer=%d mode=%d\n",__func__,timer_start,mode); */
    if (!timer_start) {
        return;
    }
    /* mebug_io_1(9); */
    if (!mode) {
        sensorhub_irq_add(IRQ_HRSENSOR_INDEX);
    } else {
        sensorhub_irq_add(IRQ_SPO2SENSOR_INDEX);
    }
    user_main_post_to_p11_system(P11_SYS_KICK, 0);
    /* mebug_io_0(9); */
}


void Hrs3603_Int_handle()
{
    if (!sensor_online) {
        return;
    }

    if (mode) {
        return;
    }

    u32 temp[3] = {0};

    if ((Hrs3603_read_hrs(&temp[0], &temp[1]) == false) | (Hrs3603_read_ps1(&temp[2]) == false)) {
        putchar('R');
        return ;
    }

    if (hrs3603_alg_send_data_test(temp[1], temp[2]) == false) {
        putchar('S');
        return;
    }

    if (!cbuf_is_write_able(data_w_cbuf[0], sizeof(temp))) {
        putchar('f');
        return;
    }

    int wlen = cbuf_write(data_w_cbuf[0], temp, sizeof(temp));

    if (wlen == 0) {
        printf("hr data_w_cbuf_full\n");
    }

}

LOOP_DETECT_REGISTER(NULL, 40, Hrs3603_40ms_timer_Cb);

int read_hrs3603_result(void *arg, u16 len)
{
//	log_i("%s",__func__);

    if (!sensor_online) {
        return 0;
    }

    if (mode) {
        return 0;
    }

    if (arg) {
        return cbuf_read(data_w_cbuf[0], arg, len);
    }
    return cbuf_get_data_len(data_w_cbuf[0]);
}

int Hrs3603_data_check_len()
{
    if (!sensor_online) {
        return 0;
    }
    if (mode) {
        return 0;
    }
    return  cbuf_get_data_len(data_w_cbuf[0]);
}


int Hrs3603_spo2_data_check_len()
{
    if (!sensor_online) {
        return 0;
    }
    if (!mode) {
        return 0;
    }
    return  cbuf_get_data_len(data_w_cbuf[1]);
}
void Hrs3603_spo2_Int_handle(void)
{
//	log_i("%s",__func__);
    if (!sensor_online) {
        return;
    }

    if (!mode) {
        return;
    }



    u32 temp[4] = {0};

    if ((Hrs3603_read_red_phase(&temp[0], &temp[1]) == false) | (Hrs3603_read_ir_phase(&temp[2], &temp[3]) == false)) {
        putchar('r');
        return;
    }
    if (hrs3603_spo2_agc_test(temp[0], temp[1], temp[2], temp[3]) == false) {
        putchar('t');
        return;
    }

    if ((temp[0] > 0x7FFFF) | (temp[1] > 0x7FFFF) | (temp[2] > 0x7FFFF) | (temp[3] > 0x7FFFF)) { //19bit
        return;
    }
    if (!cbuf_is_write_able(data_w_cbuf[1], sizeof(temp))) {
        putchar('g');
        return;
    }

    int wlen = cbuf_write(data_w_cbuf[1], temp, sizeof(temp));
    if (wlen == 0) {
        printf("spo2 data_w_cbuf_full\n");
    }
}

int read_hrs3603_spo2_result(void *arg, u16 len)
{
    /* log_i("%s",__func__); */
    if (!sensor_online) {
        return 0;
    }
    if (!mode) {
        return 0;
    }
    if (arg) {
        return cbuf_read(data_w_cbuf[1], arg, len);
    }
    return cbuf_get_data_len(data_w_cbuf[1]);

}



REGISTER_CODE_SEG(BANK_CODE_SENSOR_NORMAL_BANK_1)
int Hrs3603_io_ctl(u8 cmd, void *arg)
{
    /* printf("%s cmd=%d\n",__func__,cmd); */
//	log_i("%s",__func__);

    int *data;
    int *msg = (int *)arg;
    int len;
    int ret = 0;

    /* if (!sensor_online) { */
    /* return -1; */
    /* } */

    switch (cmd) {
    case HR_SENSOR_ENABLE:
        hrs3603_hr_enable();
        break;
    case HR_SENSOR_DISABLE:
        hrs3603_disable();
        cbuf_clear(data_w_cbuf[0]);
        cbuf_clear(data_w_cbuf[1]);
        break;
    case SPO2_SENSOR_ENABLE:
        hrs3603_spo2_enable();
        break;
    case SPO2_SENSOR_DISABLE:
        hrs3603_disable();
        cbuf_clear(data_w_cbuf[0]);
        cbuf_clear(data_w_cbuf[1]);
        break;
    case HR_SENSOR_CHECK_DATA:
        return Hrs3603_data_check_len();
        break;
    case HR_SENSOR_READ_DATA:
        //返回心率传感器数据
        data = (void *)(msg[0]);
        len  = (int)(msg[1]);
        ret = read_hrs3603_result(data, len);
        break;
    case SPO2_SENSOR_CHECK_DATA:
        return Hrs3603_spo2_data_check_len();
        break;
    case SPO2_SENSOR_READ_DATA:
        //返回血氧传感器数据
        data = (void *)(msg[0]);
        len  = (int)(msg[1]);
        ret = read_hrs3603_spo2_result(data, len);
        break;
    case HR_SEARCH_SENSOR:
        if (Hrs3603_chip_check_id() == true) {
            int valid = 1;
            memcpy(arg, &valid, 4);
        } else {
            int valid = 0;
            memcpy(arg, &valid, 4);
        }
        break;
    case HR_SENSOR_IRQ:
        Hrs3603_Int_handle();
        break;
    case SPO2_SENSOR_IRQ:
        Hrs3603_spo2_Int_handle();
        break;

    default:
        break;
    }
    return ret;
}
REGISTER_HR_SENSOR(hrSensor) = {
    .logo = "hrs3603",
    .heart_rate_sensor_init = hx3603_init,
    .heart_rate_sensor_check = NULL,
    .heart_rate_sensor_ctl = Hrs3603_io_ctl,
} ;

static u8 hrs3603_idle_query(void)
{
    return 1;
}

/* REGISTER_LP_TARGET(hrs3603_lp_target) = { */
/*     .name = "hrs3603", */
/*     .is_idle = hrs3603_idle_query, */
/* }; */
#endif

