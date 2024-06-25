#include "includes.h"
#include "sensor_api.h"
#include "gSensor_manage.h"
#include "hrSensor_manage.h"
#include "msg.h"
#include "gpio.h"
#include "circular_buf.h"
#include "bank_switch.h"
#include "usr_timer.h"

#ifndef log_info
#define log_info(...) // printf
#endif
#ifndef log_e
#define log_e(...) //printf
#endif
/*  */
/* #pragma bss_seg(".sensor_api_bss") */
/* #pragma const_seg(".sensor_api_const") */
/* #pragma code_seg(".sensor_api_code") */
/*  */


#if CONFIG_SENSOR_ALG_ENABLE
static volatile u8 sensor_irq[3];

static struct SENSOR_SHARE_PORT sensor_port;

int p33_wkup_takeover(u8 index, u8 gpio)
{
    /* printf("index %x,%x\n", index, gpio); */

#if 0
    if (gpio == IO_PORTB_03) {//gsensor
        write_gsensor_data_handle();
        return true;
    }
#endif /* #if */
#if TCFG_HR_SENSOR_READ_BY_INT
#if TCFG_VCHR02_DEV_ENABLE || TCFG_VCHR11S_DEV_ENABLE
    if (gpio == IO_PORTB_03) {//vchr02
        write_gsensor_data_handle();
        hr_sensor_req_irq();
        return true;
    }
#endif /* #if */
#endif
    return false;
}





//==========================================================//
//                 		gSensor                   	//
//==========================================================//
#if TCFG_GSENSOR_ENABLE
#if TCFG_SC7A20_EN
GSENSOR_PLATFORM_DATA_BEGIN(gSensor_data)
.iic = 0,
 .gSensor_name = "sc7a20",
  /* .gSensor_int_io = (-1),//IO_PORTB_03, */
  .gSensor_int_io = IO_PORTB_03,
   GSENSOR_PLATFORM_DATA_END();
#endif      //end if TCFG_SC7A20_EN

#if TCFG_SC7A20E_EN
GSENSOR_PLATFORM_DATA_BEGIN(gSensor_data)
.iic = 0,
 .gSensor_name = "sc7a20e",
  /* .gSensor_int_io = (-1),//IO_PORTB_03, */
  .gSensor_int_io = IO_PORTB_03,
   GSENSOR_PLATFORM_DATA_END();
#endif      //end if TCFG_SC7A20E_EN

#if TCFG_SC7A20H_EN
GSENSOR_PLATFORM_DATA_BEGIN(gSensor_data)
.iic = 0,
 .gSensor_name = "sc7a20h",
  /* .gSensor_int_io = (-1),//IO_PORTB_03, */
  .gSensor_int_io = IO_PORTB_03,
   GSENSOR_PLATFORM_DATA_END();
#endif      //end if TCFG_SC7A20H_EN

#if TCFG_MSA310_EN
GSENSOR_PLATFORM_DATA_BEGIN(gSensor_data)
.iic = 0,
 .gSensor_name = "msa310",
  .gSensor_int_io = IO_PORTB_03,
   GSENSOR_PLATFORM_DATA_END();
#endif      //end if TCFG_MSA310_EN

#endif      //end if CONFIG_GSENSOR_ENABLE



//==========================================================//
//                 		hr_sensor                   	//
//==========================================================//
#if (TCFG_HR_SENSOR_ENABLE||TCFG_SPO2_SENSOR_ENABLE)

#if TCFG_HRS3300_EN
HRSENSOR_PLATFORM_DATA_BEGIN(hrSensor_data)
.iic = 0,
 .hrSensor_name = "hrs3300",
  HRSENSOR_PLATFORM_DATA_END()
#endif

#if TCFG_HRS3603_DEV_ENABLE
  HRSENSOR_PLATFORM_DATA_BEGIN(hrSensor_data)
  .iic = 0,
   .hrSensor_name = "hrs3603",
    .hrSensor_int_io = -1,//IO_PORTB_00,
     HRSENSOR_PLATFORM_DATA_END()
#endif//TCFG_HRS3603_EN

#if TCFG_VCHR02_DEV_ENABLE
     HRSENSOR_PLATFORM_DATA_BEGIN(hrSensor_data)
     .iic = 0,
      .hrSensor_name = "vchr02",
       .hrSensor_int_io = IO_PORTB_03,//与gsensor分开,IO_PORTB_00,
        HRSENSOR_PLATFORM_DATA_END()
#endif//TCFG_HRS3603_EN

#if TCFG_VCHR11S_DEV_ENABLE
        HRSENSOR_PLATFORM_DATA_BEGIN(hrSensor_data)
        .iic = 0,
         .hrSensor_name = "vchr11s",
          .hrSensor_int_io = IO_PORTB_03,//与gsensor分开,IO_PORTB_00,
           HRSENSOR_PLATFORM_DATA_END()
#endif//TCFG_HRS3603_EN

#endif//TCFG_HR_SENSOR_ENABLE


           void sensor_init()
{
    bank_code_switch(BANK_SENSOR_TAG, BANK_CODE_SENSOR_INIT_BANK_0);
#if TCFG_GSENSOR_ENABLE

    gravity_sensor_init((struct gsensor_platform_data *)&gSensor_data, (cbuffer_t *)sensor_port.sensor_hd[N_GSENSOR]);
#endif      //end if CONFIG_GSENSOR_ENABLE

#if (TCFG_HR_SENSOR_ENABLE||TCFG_SPO2_SENSOR_ENABLE)
    hr_sensor_init((struct hrsensor_platform_data *)&hrSensor_data, (cbuffer_t *)sensor_port.sensor_hd[N_HR_SENSOR], (cbuffer_t *)sensor_port.sensor_hd[N_SPO2_SENSOR]);
#endif

    bank_code_switch(BANK_SENSOR_TAG, BANK_CODE_SENSOR_NORMAL_BANK_1);
    int size = ((25 + 10) * 3 * 2); //设置超时唤醒
    //point 200 *3 *u16
    sensor_hub_ioctl(G_SENSOR_SET_IRQ_SIZE, sizeof(size), (u8 *)(&size));

    int msg[2];
    msg[0] = sizeof(sensor_port.sensor_hd);
    msg[1] = sensor_port.sensor_hd;
    user_p11_post_to_main_system(1, sizeof(msg), (u8 *)msg, 1); //配置主核共享空间

    sensor_hub_ioctl(HR_SENSORHUB_DISABLE, 0, NULL);
    sensor_hub_gsensor_enable(0);
    sensor_hub_spo2sensor_enable(0);
}





void sensorhub_irq_add(u8 index)
{
    sensor_irq[index]++;
}

void sensorhub_irq_del(u8 index)
{
    if (sensor_irq[index]) {
        sensor_irq[index]--;
    }
}

u8 sensorhub_irq_check(u8 index)
{
    return sensor_irq[index];
}

void iic_host_check()
{
#if CONFIG_SENSOR_ALG_ENABLE
    /* 计算心率需要gSensor数据，所以应先读出gSensor数据 */
    if (sensorhub_irq_check(IRQ_GSENSOR_INDEX)) {
        sensorhub_irq_del(IRQ_GSENSOR_INDEX);
        gsensor_data_irq_handle();
        if (sensorhub_irq_check(IRQ_GSENSOR_INDEX)) {
            printf("lost\n");
        }
    }

    if (sensorhub_irq_check(IRQ_HRSENSOR_INDEX)) {
        sensorhub_irq_del(IRQ_HRSENSOR_INDEX);
        hr_sensor_io_ctl(HR_SENSOR_IRQ, 0);
        if (sensorhub_irq_check(IRQ_HRSENSOR_INDEX)) {
            printf("lost\n");
        }
    }

    if (sensorhub_irq_check(IRQ_SPO2SENSOR_INDEX)) {
        sensorhub_irq_del(IRQ_SPO2SENSOR_INDEX);
        hr_sensor_io_ctl(SPO2_SENSOR_IRQ, 0);
        if (sensorhub_irq_check(IRQ_SPO2SENSOR_INDEX)) {
            printf("lost\n");
        }
    }

#endif
}



#if 0


#if 1
#define iic_init(iic)                       hw_iic_init()
#define iic_close(iic)                      hw_iic_close()
#define iic_start(iic)                      hw_iic_start()
#define iic_stop(iic)                       hw_iic_stop()
#define iic_tx_byte(iic, byte)              hw_iic_master_tx_byte(byte)
#define iic_rx_byte(iic, ack)               hw_iic_master_rx_byte(ack)
#define iic_read_buf(iic, buf, len)         hw_iic_master_read_buf(buf, len)
#define iic_write_buf(iic, buf, len)        hw_iic_master_write_buf(buf, len)
/* #define iic_suspend(iic)                    hw_iic_suspend() */
/* #define iic_resume(iic)                     hw_iic_resume() */
#else
#define iic_init(iic)                       soft_iic_init(iic)
#define iic_close(iic)                     soft_iic_uninit(iic)
#define iic_start(iic)                      soft_iic_start(iic)
#define iic_stop(iic)                       soft_iic_stop(iic)
#define iic_tx_byte(iic, byte)              soft_iic_tx_byte(iic, byte)
#define iic_rx_byte(iic, ack)               soft_iic_rx_byte(iic, ack)
#define iic_read_buf(iic, buf, len)         soft_iic_read_buf(iic, buf, len)
#define iic_write_buf(iic, buf, len)        soft_iic_write_buf(iic, buf, len)
/* #define iic_suspend(iic)                    soft_iic_suspend(iic) */
/* #define iic_resume(iic)                     soft_iic_resume(iic) */
#endif

#define DELAY_CNT (10)

u8 sensor_iic0_write(u8 w_chip_id, u8 register_address, u8 function_command)
{
    u8 ret = 1;
    iic_start(0);
    if (0 == iic_tx_byte(0, w_chip_id)) {
        ret = 0;
        log_e("\n gsen iic wr err 0\n");
        goto __gcend;
    }
    delay(DELAY_CNT);
    if (0 == iic_tx_byte(0, register_address)) {
        ret = 0;
        log_e("\n gsen iic wr err 1\n");
        goto __gcend;
    }
    delay(DELAY_CNT);
    if (0 == iic_tx_byte(0, function_command)) {
        ret = 0;
        log_e("\n gsen iic wr err 2\n");
        goto __gcend;
    }
__gcend:
    iic_stop(0);
    return ret;
}

u16 sensor_iic0_read(u8 r_chip_id, u8 register_address, u8 *buf, u16 data_len)
{
    u16 read_len = 0;

    iic_start(0);
    if (0 == iic_tx_byte(0, r_chip_id - 1)) {
        log_e("\n gsen iic rd err 0\n");
        read_len = 0;
        goto __gdend;
    }

    delay(DELAY_CNT);

    if (0 == iic_tx_byte(0, register_address)) {
        log_e("\n gsen iic rd err 1\n");
        read_len = 0;
        goto __gdend;
    }

    iic_start(0);
    if (0 == iic_tx_byte(0, r_chip_id)) {
        log_e("\n gsen iic rd err 2\n");
        read_len = 0;
        goto __gdend;
    }

    delay(DELAY_CNT);

    for (; data_len > 1; data_len--) {
        *buf++ = iic_rx_byte(0, 1);
        read_len ++;
    }

    *buf = iic_rx_byte(0, 0);
    read_len ++;

__gdend:
    iic_stop(0);
    delay(DELAY_CNT);
    return read_len;
}


int sensor_iic0_init()
{
    iic_init(0);
    return 0;
}

#endif

static int case_gsensor_cmd(u8 means, u16 priv, u16 len, u8 *data)
{
    int ret = 0;
    int size;
    switch (means) {
    case  SENSOR_READ:
        ret = read_gsensor_nbuf(data, len);
        break;
    case SENSOR_CHECK_READ_BUF:
        ret = read_gsensor_buf_check_len();
        break;
    case SENSOR_SET_IRQ_BUF_SIZE:
        memcpy(&size, data, len);
        ret = gsensor_set_soft_irq_size(size);
        break;
    }
    return ret;
}



static int case_hr_sensor_cmd(u8 means, u16 priv, u16 len, u8 *data)
{
    int ret = 0;
    int size;
    int msg[3];
    switch (means) {
    case SENSOR_ENABLE:
        hr_sensor_io_ctl(HR_SENSOR_ENABLE, 0);
        break;
    case SENSOR_DIASBLE:
        hr_sensor_io_ctl(HR_SENSOR_DISABLE, 0);
        break;
    case SENSOR_CHECK_READ_BUF:
        ret = hr_sensor_io_ctl(HR_SENSOR_CHECK_DATA, 0);
        break;
    case  SENSOR_READ:
        msg[0] = data;
        msg[1] = len;
        ret = hr_sensor_io_ctl(HR_SENSOR_READ_DATA, msg);
        break;
    }
    return ret;
}

int sensor_hub_ioctl(int cmd, u16 len, u8 *data)
{
    u8 type  = CMD2TYPE(cmd);
    u8 means = CMD2MEANS(cmd);
    u16 user = CMD2USER(cmd);
    switch (type) {
    case SPO2_CMD:

        break;
    case HR_SENSOR_CMD:
        return case_hr_sensor_cmd(means, user, len, data);
        break;
    case G_SENSOR_CMD:
        return case_gsensor_cmd(means, user, len, data);
        break;
    }
    return len;
}





int sensor_hub_gsensor_enable(u8 on)
{
    int ret = 0;

    if (on) {
        ret =  gsensor_enable();
    } else {
        ret =  gsensor_disable();
    }

    if (ret == 0) {
        sensor_hub_timer_enable(on);
    }
    return ret;
}


int sensor_hub_gsensor_check_len()
{
    return read_gsensor_buf_check_len();
}

int sensor_hub_gsensor_read(u8 *data, u16 len)
{
    return read_gsensor_nbuf(data, len);
}

int sensor_hub_hrsensor_low_power_detection(void)
{
    return  hr_sensor_io_ctl(HR_SENSOR_LP_DETECTION, 0);
}

int sensor_hub_hrsensor_factory_test(void)
{
    return  hr_sensor_io_ctl(HR_SENSOR_FACTORY_TEST, 0);
}

int sensor_hub_hrsensor_check_len()
{
    return hr_sensor_io_ctl(HR_SENSOR_CHECK_DATA, 0);
}

int sensor_hub_hrsensor_read(u8 *data, u16 len)
{
    int msg[2];
    msg[0] = data;
    msg[1] = len;
    return  hr_sensor_io_ctl(HR_SENSOR_READ_DATA, msg);
}

int sensor_hub_hrsensor_enable(u8 on)
{
    if (on) {
        return  hr_sensor_io_ctl(HR_SENSOR_ENABLE, 0);
    } else {
        return  hr_sensor_io_ctl(HR_SENSOR_DISABLE, 0);
    }
}


int sensor_hub_spo2sensor_check_len()
{
    return 0;
}

int sensor_hub_spo2ensor_read(u8 *data, u16 len)
{
    return 0;
}

int sensor_hub_spo2sensor_enable(u8 on)
{
    if (on) {
        return  hr_sensor_io_ctl(SPO2_SENSOR_ENABLE, 0);
    } else {
        return  hr_sensor_io_ctl(SPO2_SENSOR_DISABLE, 0);
    }
}

int sensor_hub_gsensor_search(u32 *valid)
{
    gsensor_io_ctl(SEARCH_SENSOR, valid);
    return 0;
}
int sensor_hub_hrsensor_search(u32 *valid)
{
    hr_sensor_io_ctl(HR_SEARCH_SENSOR, valid);
    return 0;

}


static void sensor_hub_timer_handle(void *priv)
{
    write_gsensor_data_handle();

#if !TCFG_HR_SENSOR_READ_BY_INT
    hr_sensor_req_irq();
#endif
}

int sensor_hub_timer_enable(u8 enable)
{
    static u16 sensor_hub_timer = 0;

    if (enable && sensor_hub_timer == 0) {
        sensor_hub_timer = usr_timer_add(NULL, sensor_hub_timer_handle, 200, 0);
    } else if ((!enable) &&  sensor_hub_timer > 0) {
        usr_timer_del(sensor_hub_timer);
        sensor_hub_timer = 0;
    }

    return 0;
}
#endif /* #if TCFG_GSENSOR_ENABLE */
