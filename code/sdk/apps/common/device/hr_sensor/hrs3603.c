


#include "app_config.h"
#include "os/os_api.h"
#include "asm/clock.h"
#include "system/timer.h"
#include "asm/cpu.h"
#include "generic/typedef.h"
#include "generic/gpio.h"
#include "debug.h"
#include "hrs3603.h"
#include "hrSensor_manage.h"
#include "system/os/os_api.h"
#include "printf.h"
#include <stdint.h>
#include <stdlib.h>

spinlock_t sensor_iic;
u8 sensor_iic_init_status = 0;

#if TCFG_HRS3603_EN
#if defined(TCFG_HX3603_DEV_ENABLE) && TCFG_HX3603_DEV_ENABLE

u8 hrsensortimes = 0;
int hrsensorcbuf[36];
cbuffer_t hrsensor_cbuf;


extern void delay_2ms(int cnt);//fm
static int mode = 0;
struct data_sec {
    u8 hrdatacnt;
    u32 *hrdata;
    OS_SEM hrsem;
} *datasec;
struct spo2_data {
    u8 cnt;
    u32 *spo2data;
    OS_SEM spo2sem;
} *spo2secdata;
/****************************timer3*****************************/
#define timer_x_hx3603 JL_TIMER3  //timer3
#define IRQ_TIMEx_IDX (3+4)       //timer3
___interrupt
static void timer3_isr()
{
    static u32 cnt1 = 0;
    timer_x_hx3603->CON |= BIT(14);
    ++cnt1;
    if (mode == 0) {
        Hrs3603_Int_handle();
    } else {
        Hrs3603_spo2_Int_handle();
    }
    if (cnt1 >= 43) {
        cnt1 = 0;
    }
}
#define APP_TIMER_CLK           clk_get("timer")
#define MAX_TIME_CNT            0x7fff
#define MIN_TIME_CNT            0x100
#define TIMER_UNIT_MS           1 //1ms起一次中断
int timer3_init()
{
    u32 prd_cnt = 1500;
    u8 index = 2;
    u32 timer_div[] = {
        /*0000*/    1,
        /*0001*/    4,
        /*0010*/    16,
        /*0011*/    64,
        /*0100*/    2,
        /*0101*/    8,
        /*0110*/    32,
        /*0111*/    128,
        /*1000*/    256,
        /*1001*/    4 * 256,
        /*1010*/    16 * 256,
        /*1011*/    64 * 256,
        /*1100*/    2 * 256,
        /*1101*/    8 * 256,
        /*1110*/    32 * 256,
        /*1111*/    128 * 256,
    };

//    printf("%s :%d", __func__, __LINE__);
    for (index = 0; index < (sizeof(timer_div) / sizeof(timer_div[0])); index++) {
        prd_cnt = 40 * TIMER_UNIT_MS * (clk_get("timer") / 1000) / timer_div[index];
        if (prd_cnt > MIN_TIME_CNT && prd_cnt < MAX_TIME_CNT) {
            break;
        }
    }
    timer_x_hx3603->CNT = 0;
    timer_x_hx3603->PRD = prd_cnt; //1ms*40
    request_irq(IRQ_TIMEx_IDX, 4, timer3_isr, 0);
    timer_x_hx3603->CON = (index << 4)  | BIT(3);
    printf("   PRD:0x%x / lsb:%d iosc:%d,div:%d\n", timer_x_hx3603->PRD, clk_get("lsb"), clk_get("timer"), index);
    return 0;
}
void hrs3603_timers_start()//定时器轮询模式：开始血氧采集（初始化成功后调用）
{
    timer_x_hx3603->CON |= BIT(0);
}
void hrs3603_timers_close()//关闭定时器
{
    timer_x_hx3603->CON &= ~BIT(0);
}
/****************************iic*****************************/
#if TCFG_HX3603_USE_IIC_TYPE
#define iic_init(iic)                       hw_iic_init(iic)
#define iic_uninit(iic)                     hw_iic_uninit(iic)
#define iic_start(iic)                      hw_iic_start(iic)
#define iic_stop(iic)                       hw_iic_stop(iic)
#define iic_tx_byte(iic, byte)              hw_iic_tx_byte(iic, byte)
#define iic_rx_byte(iic, ack)               hw_iic_rx_byte(iic, ack)
#define iic_read_buf(iic, buf, len)         hw_iic_read_buf(iic, buf, len)
#define iic_write_buf(iic, buf, len)        hw_iic_write_buf(iic, buf, len)
#define iic_suspend(iic)                    hw_iic_suspend(iic)
#define iic_resume(iic)                     hw_iic_resume(iic)
#else
#define iic_init(iic)                       soft_iic_init(iic)
#define iic_uninit(iic)                     soft_iic_uninit(iic)
#define iic_start(iic)                      soft_iic_start(iic)
#define iic_stop(iic)                       soft_iic_stop(iic)
#define iic_tx_byte(iic, byte)              soft_iic_tx_byte(iic, byte)
#define iic_rx_byte(iic, ack)               soft_iic_rx_byte(iic, ack)
#define iic_read_buf(iic, buf, len)         soft_iic_read_buf(iic, buf, len)
#define iic_write_buf(iic, buf, len)        soft_iic_write_buf(iic, buf, len)
#define iic_suspend(iic)                    soft_iic_suspend(iic)
#define iic_resume(iic)                     soft_iic_resume(iic)
#endif
static struct _hx3603_dev_platform_data hx3603_iic_info_test = {
    .iic_hdl = 1,
    .iic_delay = 0
};
static struct _hx3603_dev_platform_data *hx3603_iic_info;

static u8 hx3603_write_reg(u8 hx3603_reg_addr, u8 data)
{
    /* os_mutex_pend(&SENSOR_IIC_MUTEX,0); */
    /* printf("%s",__func__); */
    spin_lock(&sensor_iic);
    iic_start(hx3603_iic_info->iic_hdl);
    if (0 == iic_tx_byte(hx3603_iic_info->iic_hdl, HX3603_WADDR)) {
        iic_stop(hx3603_iic_info->iic_hdl);
        spin_unlock(&sensor_iic);
        log_e("hx3603 write fail1!\n");
        return false;
    }
    delay_2ms(hx3603_iic_info->iic_delay);
    if (0 == iic_tx_byte(hx3603_iic_info->iic_hdl, hx3603_reg_addr)) {
        iic_stop(hx3603_iic_info->iic_hdl);
        spin_unlock(&sensor_iic);
        log_e("hx3603 write fail2!\n");
        return false;
    }
    delay_2ms(hx3603_iic_info->iic_delay);
    if (0 == iic_tx_byte(hx3603_iic_info->iic_hdl, data)) {
        iic_stop(hx3603_iic_info->iic_hdl);
        spin_unlock(&sensor_iic);
        log_e("hx3603 write fail3!\n");
        return false;
    }
    iic_stop(hx3603_iic_info->iic_hdl);
    delay_2ms(hx3603_iic_info->iic_delay);
    /* os_mutex_post(&SENSOR_IIC_MUTEX); */
    spin_unlock(&sensor_iic);
    return true;
}
AT(.volatile_ram_code)
static u8 hx3603_read_reg(u8 hx3603_reg_addr, u8 *read_data, u8 len)
{
    /* printf("%s",__func__); */
    /* os_mutex_pend(&SENSOR_IIC_MUTEX,0); */
    spin_lock(&sensor_iic);
    iic_start(hx3603_iic_info->iic_hdl);
    if (0 == iic_tx_byte(hx3603_iic_info->iic_hdl, HX3603_WADDR)) {
        iic_stop(hx3603_iic_info->iic_hdl);
        spin_unlock(&sensor_iic);
        log_e("hx3603 read fail1!\n");
        return false;
    }
    delay_2ms(hx3603_iic_info->iic_delay);
    if (0 == iic_tx_byte(hx3603_iic_info->iic_hdl, hx3603_reg_addr)) {
        iic_stop(hx3603_iic_info->iic_hdl);
        spin_unlock(&sensor_iic);
        log_e("hx3603 read fail2!\n");
        return false;
    }
    delay_2ms(hx3603_iic_info->iic_delay);
    iic_start(hx3603_iic_info->iic_hdl);
    if (0 == iic_tx_byte(hx3603_iic_info->iic_hdl, HX3603_RADDR)) {
        iic_stop(hx3603_iic_info->iic_hdl);
        spin_unlock(&sensor_iic);
        log_e("hx3603 read fail3!\n");
        return false;
    }

    for (u8 i = 0; i < len - 1; i++) {
        read_data[i] = iic_rx_byte(hx3603_iic_info->iic_hdl, 1);
        delay_2ms(hx3603_iic_info->iic_delay);
    }
    read_data[len - 1] = iic_rx_byte(hx3603_iic_info->iic_hdl, 0);
    iic_stop(hx3603_iic_info->iic_hdl);
    delay_2ms(hx3603_iic_info->iic_delay);
    /* os_mutex_post(&SENSOR_IIC_MUTEX); */
    spin_unlock(&sensor_iic);
    return true;
}

/****************************hx3603*****************************/
bool Hrs3603_chip_check_id()
{
    int i = 0 ;
    uint8_t chip_id = 0;

    chip_id = Hrs3603_read_reg(0x00) ;
    if (chip_id != 0x23) {
        return false;
    }

    return true;
}


bool hx3603_chip_init(void *priv)
{

    if (priv == NULL) {
        printf("hx3603 init fail(no priv)\n");
        return false;
    }
    printf("%s", __func__);
    hx3603_iic_info = &hx3603_iic_info_test;
//    printf("%s", __func__);

    printf("--------read id!%x", Hrs3603_read_reg(0x00));
    if (Hrs3603_chip_check_id() == 1) {
        printf("--------read hx3603 id ok!");
//       Hrs3603_hr_alg_config();

        return 1;//Hrs3603_chip_init ();
    } else {
        log_e("--------read hx3603 id error!");

        //      ASSERT(Hrs3603_chip_check_id());//检查board配置 ，如果没有hrsensor请置0
        return 0;//Hrs3603_chip_init ();
    }
}
void Hrs3603_chip_disable(void)
{
    Hrs3603_write_reg(0x01, 0x01);
    Hrs3603_write_reg(0x02, 0x01);
    os_time_dly(5);
//    nrf_delay_ms(50); //need delay 50ms
    Hrs3603_write_reg(0x1a, 0x13);
}
void hrs3603_disable(void)//关闭定时器
{
    hrs3603_timers_close();
    Hrs3603_chip_disable(); // 此时功耗在 1ua 以下
}
void hrs3603_hr_enable(void) //从低功耗恢复
{
    /* printf("%s",__func__); */
    mode = 0;
    Hrs3603_hr_alg_config();//重置寄存器
    hrs3603_timers_start();//开定时器
}
void hrs3603_spo2_enable(void)
{
    /* printf("%s",__func__); */
    mode = 1;
    Hrs3603_spo2_alg_config();
    hrs3603_timers_start();
}

u8 hx3603_init()
{
    if (sensor_iic_init_status == 0) {
        spin_lock_init(&sensor_iic);
        sensor_iic_init_status = 1;
    }
    iic_init(hx3603_iic_info_test.iic_hdl);// initial i2c
    os_time_dly(5);//芯片上电请延时

    if (hx3603_chip_init(&hx3603_iic_info_test)) {
        timer3_init();
        printf("%s1", __func__);
        //hrs3603_spo2_enable();
        //Hrs3603_hr_alg_config();
        //hrs3603_timers_start();
        //  hrs3603_disable();//先关灯；
        return 1;
    }
    printf("%s2", __func__);
    return 0;
}
void hrs3603_alg_send_data_test(u32 als_raw_data, u32 infrared_data)
{
    static u8 send_data = 0x04;
    static u8 hx_close = 0, hx_open = 0;
    if (infrared_data > 30000) {
        hx_open++;
        if (hx_open == 2) {
            hx_open--;
            if ((als_raw_data > 450000) || (hx_close == 2)) {
                send_data += 2;
                if (send_data > 0x7a) {
                    send_data = 0x7a;
                    return;
                }
                if (send_data == 0x06) {
                    Hrs3603_write_reg(0x0d, 0x63);
                    Hrs3603_write_reg(0x06, 0x0a);
                    Hrs3603_write_reg(0x0b, 0x73);
                    Hrs3603_write_reg(0x03, 0x0f);
                    //            send_data +=2;
                    //            Hrs3603_write_reg(0x06,send_data);
                } else {
                    Hrs3603_write_reg(0x06, send_data);
                }
            } else if (als_raw_data < 200000) {
                send_data -= 2;
                if (send_data < 0x06) {
                    send_data = 0x06;
                    return;
                }
                Hrs3603_write_reg(0x06, send_data);
            }
            hx_close = 0;
        }
    } else {
        hx_close++;
        if (hx_close == 2) {
            hx_open = 0;
            send_data = 0x04;
            Hrs3603_write_reg(0x03, 0x07);
        } else if (hx_close == 3) {
            hx_close--;
        }
    }
}
hrs3603_spo2_msg_code_t hx3603_state = MSG_SPO2_ALG_NOT_OPEN;
void hrs3603_spo2_agc_test(u32 red_new_raw_data, u32 als_in_red_raw_data, u32 ir_new_raw_data, u32 als_in_ir_raw_data)
{
    static u8 red_on_state = 2, ir_on_state = 2; //默认刚开机
    static s16 red06_data = 0;
    static s16 ir05_data = 0;
    //先加减
    if (red_new_raw_data >= 450000) {
        red06_data++;
        if (red_on_state == 2) {
            red_on_state = 1;
            red06_data = 0x1f;
        }
        Hrs3603_read_reg(0x0d);
        if (red06_data <= 0x7f) {
            Hrs3603_write_reg(0x06, red06_data);
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
            Hrs3603_write_reg(0x06, red06_data);
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
            Hrs3603_write_reg(0x05, ir05_data);
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
            Hrs3603_write_reg(0x05, ir05_data);
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
            Hrs3603_write_reg(0x03, 0x07);
            Hrs3603_write_reg(0x05, 0x00);
            Hrs3603_write_reg(0x06, 0x00);
            ir_on_state = 0;
            red_on_state = 0;
            red06_data = 0x00;
            ir05_data = 0x00;
            hx3603_state = MSG_SPO2_NO_TOUCH; //2
        }
    } else {
        if (ir_on_state == 0) {
            Hrs3603_write_reg(0x03, 0x0f);
            ir_on_state = 2; //刚开机
            red_on_state = 2; //刚开机
            hx3603_state = MSG_SPO2_ALG_NOT_OPEN; //1
        }
    }
}



/**********************数据处理*****************************/

bool Hrs3603_write_reg(u8 addr, u8 data)
{
    // 根据不同平台实现i2c, sucess: return 0; fail: retrun 1;
    hx3603_write_reg(addr, data);
    return 0;
}
u8 Hrs3603_read_reg(u8 addr)
{
    uint8_t data_buf = 0;
    if (hx3603_read_reg(addr, &data_buf, 1)) {
        return data_buf;
    } else {
        return 0;
    }
}
AT(.volatile_ram_code)
bool Hrs3603_brust_read_reg(uint8_t addr, uint8_t *buf, uint8_t length)
{
    if (hx3603_read_reg(addr, buf, length)) {
        return true;
    } else {
        return false;
    }
}
AT(.volatile_ram_code)
bool Hrs3603_read_hrs(u32 *hrm_data, u32 *als_data)
{
//	printf("%s",__func__);
    uint8_t  databuf[6] = {0};
    int32_t P0 = 0, P1 = 0;

    Hrs3603_brust_read_reg(0xa0, databuf, 6);

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
AT(.volatile_ram_code)
bool Hrs3603_read_ps1(u32 *infrared_data)
{
//   printf("%s",__func__);
    uint8_t  databuf[6] = {0};
    int32_t P0 = 0, P1 = 0;
    Hrs3603_brust_read_reg(0xa6, databuf, 6);
    P0 = ((databuf[0]) | (databuf[1] << 8) | (databuf[2] << 16));
    P1 = ((databuf[3]) | (databuf[4] << 8) | (databuf[5] << 16));
    if (P0 > P1) {
        *infrared_data = P0 - P1;
    } else {
        *infrared_data = 0;
    }
    return true;
}
AT(.volatile_ram_code)
u8 Hrs3603_read_red_phase(u32 *red_raw_data, u32 *als_inred_data)
{
//	printf("%s",__func__);
    uint8_t  databuf[6] = {0};
    int32_t P0 = 0, P1 = 0;
    Hrs3603_brust_read_reg(0xa0, databuf, 6);

    P0 = ((databuf[0]) | (databuf[1] << 8) | (databuf[2] << 16));
    P1 = ((databuf[3]) | (databuf[4] << 8) | (databuf[5] << 16));

    *red_raw_data = P0;
    *als_inred_data = P1;

    return true;
}
AT(.volatile_ram_code)

u8 Hrs3603_read_ir_phase(u32 *ir_raw_data, u32 *als_inir_data)
{
//	printf("%s",__func__);
    uint8_t  databuf[6] = {0};
    int32_t P0 = 0, P1 = 0;
    Hrs3603_brust_read_reg(0xa6, databuf, 6);

    P0 = ((databuf[0]) | (databuf[1] << 8) | (databuf[2] << 16));
    P1 = ((databuf[3]) | (databuf[4] << 8) | (databuf[5] << 16));

    *ir_raw_data = P0;
    *als_inir_data = P1;

    return true;
}

void Hrs3603_hr_alg_config(void)
{
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
    Hrs3603_write_reg(0x13, 0x02);    //enable hrs int
    Hrs3603_write_reg(0x12, 0x50);    //
    Hrs3603_write_reg(0x20, 0x20);    //close fifo int
}
void Hrs3603_spo2_alg_config(void)
{
//	printf("%s",__func__);
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
    Hrs3603_write_reg(0x13, 0x01);    //enable ps1 int

    Hrs3603_write_reg(0x12, 0x50);    //
    Hrs3603_write_reg(0x20, 0x20);    //close fifo int
}



void Hrs3603_Int_handle(void)
{
    /* printf("%s",__func__); */
    if ((hrsensor_cbuf.data_len + 12) > hrsensor_cbuf.total_len) { //不写入，防止错位
        printf("hrsensor cbuf full");
        return ;
    }
    u32 hrm_raw_data;
    u32 als_raw_data;
    u32 infrared_data;
    Hrs3603_read_hrs(&hrm_raw_data, &als_raw_data);
    Hrs3603_read_ps1(&infrared_data);
    hrs3603_alg_send_data_test(als_raw_data, infrared_data);
    cbuf_write(&hrsensor_cbuf, &hrm_raw_data, 4);
    cbuf_write(&hrsensor_cbuf, &als_raw_data, 4);
    cbuf_write(&hrsensor_cbuf, &infrared_data, 4);
    /* printf("%s=%d", __func__, hrsensortimes); */
    /* if (datasec == NULL) { */
    /* return; */
    /* } */
    /* if (datasec->hrdatacnt >= hrsensortimes) { */
    /* return; */
    /* } */

    /* datasec->hrdata[datasec->hrdatacnt * 3] = hrm_raw_data; */
    /* datasec->hrdata[datasec->hrdatacnt * 3 + 1] = als_raw_data; */
    /* datasec->hrdata[datasec->hrdatacnt * 3 + 2] = infrared_data; */
    /* datasec->hrdatacnt++; */
    /* printf("%s", __func__); */
}
int read_hrs3603_result(void *arg)
{
    /* printf("%s %d %d ",__func__,hrsensor_cbuf.data_len,(hrsensortimes*sizeof(int))); */
    if (hrsensor_cbuf.data_len >= (hrsensortimes * sizeof(int))) {
        cbuf_read(&hrsensor_cbuf, arg, hrsensortimes * sizeof(int));
        return hrsensortimes * sizeof(int);
    } else {
        /* printf("hrsensor_cbuf.data_len not enough"); */

        return 0;
    }
    //	printf("%s",__func__);
    /* if (spo2secdata != NULL) { */
    /* return ; */
    /* } */
    /* datasec = zalloc(sizeof(struct data_sec)); */
    /* if (datasec == NULL) { */
    /* printf("_hrs3603_zalloc_error"); */
    /* return ; */
    /* } */
    /* datasec->hrdata = (u32 *)arg; */
    /* printf("%x===%x",datasec->hrdata,arg); */
    /* while (1) { */
    /* printf("cnt=%d",datasec->hrdatacnt); */
    /* if (datasec->hrdatacnt >= hrsensortimes) { */
    /* break; */
    /* } */
    /* os_time_dly(hrsensortimes * 4); */
    /* } */
    //memcpy(arg, datasec->hrdata, hrsensortimes * 3 * 4);
    /* free(datasec); */
    /* datasec = NULL; */
}
void Hrs3603_spo2_Int_handle(void)
{
    /* printf("%s",__func__); */
    if ((hrsensor_cbuf.data_len + 16) > hrsensor_cbuf.total_len) { //不写入，防止错位
        printf("spo2sensor cbuf full");
        return ;
    }
    u32 red_new_raw_data;
    u32 als_in_red_raw_data;
    u32 ir_new_raw_data;
    u32 als_in_ir_raw_data;
    Hrs3603_read_red_phase(&red_new_raw_data, &als_in_red_raw_data);
    Hrs3603_read_ir_phase(&ir_new_raw_data, &als_in_ir_raw_data);
    hrs3603_spo2_agc_test(red_new_raw_data, als_in_red_raw_data, \
                          ir_new_raw_data, als_in_ir_raw_data);
    cbuf_write(&hrsensor_cbuf, &red_new_raw_data, 4);
    cbuf_write(&hrsensor_cbuf, &als_in_red_raw_data, 4);
    cbuf_write(&hrsensor_cbuf, &ir_new_raw_data, 4);
    cbuf_write(&hrsensor_cbuf, &als_in_ir_raw_data, 4);
    /* printf("%s",__func__); */

    /* if (spo2secdata == NULL) { */
    /* return; */
    /* } */
    /* if (spo2secdata->cnt >= hrsensortimes) { */
    /* return; */
    /* } */


    /* u32 red_new_raw_data; */
    /* u32 als_in_red_raw_data; */
    /* u32 ir_new_raw_data; */
    /* u32 als_in_ir_raw_data; */

    /* Hrs3603_read_red_phase(&red_new_raw_data, &als_in_red_raw_data); */
    /* Hrs3603_read_ir_phase(&ir_new_raw_data, &als_in_ir_raw_data); */

    /* hrs3603_spo2_agc_test(red_new_raw_data, als_in_red_raw_data, \ */
    /* ir_new_raw_data, als_in_ir_raw_data); */
    /* spo2secdata->spo2data[spo2secdata->cnt * 4] = red_new_raw_data; */
    /* spo2secdata->spo2data[spo2secdata->cnt * 4 + 1] = als_in_red_raw_data; */
    /* spo2secdata->spo2data[spo2secdata->cnt * 4 + 2] = ir_new_raw_data; */
    /* spo2secdata->spo2data[spo2secdata->cnt * 4 + 3] = als_in_ir_raw_data; */
    /* spo2secdata->cnt++; */

}
int read_hrs3603_spo2_result(void *arg)
{

    /* printf("%s %d %d ",__func__,hrsensor_cbuf.data_len,(hrsensortimes*sizeof(int))); */
    if (hrsensor_cbuf.data_len >= (hrsensortimes * sizeof(int))) {
        cbuf_read(&hrsensor_cbuf, arg, hrsensortimes * sizeof(int));
        return hrsensortimes * sizeof(int);
    } else {
        /* printf("spo2sensor_cbuf.data_len not enough"); */

        return 0;
    }

    printf("%s", __func__);
    /* if (datasec != NULL) { */
    /* return ; */
    /* } */
    /* spo2secdata = zalloc(sizeof(struct spo2_data)); */
    /* if (spo2secdata == NULL) { */
    /* printf("spo2secdata zalloc error"); */
    /* return; */
    /* } */
    /* spo2secdata->spo2data = (u32 *)arg; */
    /* printf("%x===%x",spo2secdata->spo2data,arg); */
    /* while (1) { */
    /* printf("cnt=%d",spo2secdata->cnt); */
    /* if (spo2secdata->cnt >= hrsensortimes) { */
    /* break; */
    /* } */
    /* os_time_dly(hrsensortimes * 4); */
    /* } */

    /* memcpy(arg, spo2secdata->spo2data, hrsensortimes * 4 * 4); */

    /* free(spo2secdata); */
    /* spo2secdata = NULL; */
}

int Hrs3603_io_ctl(u8 cmd, void *arg)
{
//	printf("%s",__func__);
    switch (cmd) {
    case HR_SENSOR_ENABLE:
        hrsensortimes = arg;
        cbuf_init(&hrsensor_cbuf, hrsensorcbuf, 24 * sizeof(int));
        hrs3603_hr_enable();
        break;
    case HR_SENSOR_DISABLE:
        hrsensortimes = arg;
        cbuf_clear(&hrsensor_cbuf);
        hrs3603_disable();
        break;
    case SPO2_SENSOR_ENABLE:
        hrsensortimes = arg;
        cbuf_init(&hrsensor_cbuf, hrsensorcbuf, 24 * sizeof(int));
        hrs3603_spo2_enable();
        break;
    case SPO2_SENSOR_DISABLE:
        hrsensortimes = arg;
        cbuf_clear(&hrsensor_cbuf);
        hrs3603_disable();
        break;
    case HR_SENSOR_READ_DATA:
        //返回心率传感器数据
        return read_hrs3603_result(arg);
        break;
    case SPO2_SENSOR_READ_DATA:
        //返回血氧传感器数据
        return read_hrs3603_spo2_result(arg);
        break;
    case HR_SEARCH_SENSOR:
        if (Hrs3603_chip_check_id() == true) {
            char res = 1;
            memcpy(arg, &res, 1);
        } else {
            char res = 0;
            memcpy(arg, &res, 1);
        }
        break;
    default:
        break;
    }
    return 0;
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

REGISTER_LP_TARGET(hrs3603_lp_target) = {
    .name = "hrs3603",
    .is_idle = hrs3603_idle_query,
};
#endif
#endif
