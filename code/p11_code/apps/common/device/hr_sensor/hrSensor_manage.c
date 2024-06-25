#include "hrSensor_manage.h"
/* #include "hrs3300.h" */
#include "iic_api.h"
#include "gpio.h"
#include "iic_soft.h"
#include "bank_switch.h"
#include "sensor_api.h"
#include "msg.h"


/* #pragma bss_seg(".sport_dev_bss") */
/* #pragma const_seg(".sport_dev_const") */
/* #pragma code_seg(".sport_dev_code") */
/*  */


#define BANK_NAME  BANK_SENSOR_TAG

static const struct hrsensor_platform_data *platform_data;
HR_SENSOR_INTERFACE *hrSensor_hdl = NULL;
HR_SENSOR_INFO  __hrsensor_info = {.iic_delay = 0};
#define hrSensor_info (&__hrsensor_info)


#define t_printf printf

/* OS_MUTEX SENSOR_IIC_MUTEX; */
/* spinlock_t sensor_iic; */
#if TCFG_HR_SENSOR_USE_IIC_TYPE
#define iic_init(iic)                       hw_iic_init()
#define iic_uninit(iic)                     hw_iic_close()
#define iic_start(iic)                      hw_iic_start()
#define iic_stop(iic)                       hw_iic_stop()
#define iic_tx_byte(iic, byte)              hw_iic_master_tx_byte(byte)
#define iic_rx_byte(iic, ack)               hw_iic_master_rx_byte(ack)
#define iic_read_buf(iic, buf, len)         hw_iic_master_read_buf(buf, len)
#define iic_write_buf(iic, buf, len)        hw_iic_master_write_buf(buf, len)
/* #define iic_suspend(iic)                    hw_iic_suspend(iic) */
/* #define iic_resume(iic)                     hw_iic_resume(iic) */
#else
#define iic_init(iic)                       soft_iic_init(iic)
#define iic_uninit(iic)                     soft_iic_uninit(iic)
#define iic_start(iic)                      soft_iic_start(iic)
#define iic_stop(iic)                       soft_iic_stop(iic)
#define iic_tx_byte(iic, byte)              soft_iic_tx_byte(iic, byte)
#define iic_rx_byte(iic, ack)               soft_iic_rx_byte(iic, ack)
#define iic_read_buf(iic, buf, len)         soft_iic_read_buf(iic, buf, len)
#define iic_write_buf(iic, buf, len)        soft_iic_write_buf(iic, buf, len)
/* #define iic_suspend(iic)                    soft_iic_suspend(iic) */
/* #define iic_resume(iic)                     soft_iic_resume(iic) */
#endif

extern void delay(u32 cnt);//fm
u8 hrsensor_write_nbyte(u8 w_chip_id, u8 register_address, u8 *buf, u8 data_len)
{
    /* printf("%s %d",__func__,__LINE__); */
    /* putchar('('); */

    u8 write_len = 0;
    u8 i;

    iic_start(hrSensor_info->iic_hdl);
    delay(hrSensor_info->iic_delay);
    if (0 == iic_tx_byte(hrSensor_info->iic_hdl, w_chip_id)) {
        write_len = 0;
        t_printf("\n hrSensor iic w err 0\n");
        goto __wend;
    }

    delay(hrSensor_info->iic_delay);

    if (0 == iic_tx_byte(hrSensor_info->iic_hdl, register_address)) {
        write_len = 0;
        t_printf("\n hrSensor iic w err 1\n");
        goto __wend;
    }

    for (i = 0; i < data_len; i++) {
        delay(hrSensor_info->iic_delay);
        if (0 == iic_tx_byte(hrSensor_info->iic_hdl, buf[i])) {
            write_len = 0;
            t_printf("\n hrSensor iic w err 2\n");
            goto __wend;
        }
        write_len++;
    }
    delay(hrSensor_info->iic_delay);
__wend:
    iic_stop(hrSensor_info->iic_hdl);
    /* putchar(')'); */
    return write_len;
}

u8 hrsensor_read_nbyte(u8 r_chip_id, u8 register_address, u8 *buf, u8 data_len)
{
    /* printf("%s %d",__func__,__LINE__); */
    u8 read_len = 0;
    /* putchar('<'); */

    iic_start(hrSensor_info->iic_hdl);
    delay(hrSensor_info->iic_delay);
    if (0 == iic_tx_byte(hrSensor_info->iic_hdl, r_chip_id - 1)) {
        t_printf("\n hrSensor iic r err 0\n");
        read_len = 0;
        goto __rend;
    }
    delay(hrSensor_info->iic_delay);

    if (0 == iic_tx_byte(hrSensor_info->iic_hdl, register_address)) {
        t_printf("\n hrSensor iic r err 1\n");
        read_len = 0;
        goto __rend;
    }
    delay(hrSensor_info->iic_delay);
    iic_start(hrSensor_info->iic_hdl);
    if (0 == iic_tx_byte(hrSensor_info->iic_hdl, r_chip_id)) {
        t_printf("\n hrSensor iic r err 2\n");
        read_len = 0;
        goto __rend;
    }

    for (; data_len > 1; data_len--) {
        delay(hrSensor_info->iic_delay);
        *buf++ = iic_rx_byte(hrSensor_info->iic_hdl, 1);
        read_len ++;
    }
    *buf = iic_rx_byte(hrSensor_info->iic_hdl, 0);
    read_len ++;
    delay(hrSensor_info->iic_delay);

__rend:
    iic_stop(hrSensor_info->iic_hdl);
    /* putchar('>'); */
    return read_len;
}


REGISTER_CODE_SEG(BANK_CODE_SENSOR_NORMAL_BANK_1)
int hr_sensor_io_ctl(u8 cmd, void *arg)
{
    /* printf("%s %d",__func__,__LINE__); */
    if (!hrSensor_hdl) {
        return 0;
    }
    return  hrSensor_hdl->heart_rate_sensor_ctl(cmd, arg);
}

void hr_sensor_input_accel_data(void *data, u8 point)
{
    /* printf("%s %d",__func__,__LINE__); */
    if (!hrSensor_hdl) {
        return;
    }
    hrSensor_hdl->accel_data_input(data, point);
}


REGISTER_CODE_SEG(BANK_CODE_SENSOR_INIT_BANK_0)
int hr_sensor_init(void *_data, cbuffer_t  *hrcbuf, cbuffer_t  *spo2cbuf)
{
    /* printf("%s %d\n",__func__,__LINE__); */
    HR_SENSOR_INTERFACE *hd = NULL;
    int retval = 0;
    if (_data == NULL) {
        t_printf("hr init fail(no _data)\n");
        return false;
    }
    platform_data = (const struct hrsensor_platform_data *)_data;
    hrSensor_info->iic_hdl = platform_data->iic;
    retval = iic_init(hrSensor_info->iic_hdl);
    if (retval < 0) {
        t_printf("\n  open iic for hrSensor err\n");
        return retval;
    } else {
        t_printf("\n iic open succ\n");
    }

    retval = -EINVAL;
    list_for_each_hrsensor(hd) {
        t_printf("%s==%s", hd->logo, platform_data->hrSensor_name);
        if (!memcmp(hd->logo, platform_data->hrSensor_name, strlen(platform_data->hrSensor_name))) {
            retval = 0;
            break;
        }
    }

    if (retval < 0) {
        t_printf(">>>hrSensor_hdl logo err\n");
        return retval;
    }

    if (hd->heart_rate_sensor_init(hrcbuf, spo2cbuf)) {
        t_printf(">>>>hrSensor_Init SUCC");
        hrSensor_hdl = hd;
        hrSensor_info->init_flag = 1;
        /* if (platform_data->hrSensor_int_io != -1) {//sdk init */
        /* } */

    } else {
        t_printf(">>>>hrSensor_Init ERROR");
    }

    return 0;
}

void hr_sensor_req_irq(void)
{
    //printf("%s", __func__);
    sensorhub_irq_add(IRQ_HRSENSOR_INDEX);
    user_main_post_to_p11_system(P11_SYS_KICK, 0);
}

/***********************hrs3603 hr test********************/
#if 0
int temp_data[100];
void hrs3603_hr_test()
{
//hr
    hr_sensor_io_ctl(HR_SENSOR_ENABLE, NULL);
    mdelay(100);
    while (1) {
        hr_sensor_io_ctl(HR_SENSOR_READ_DATA, temp_data);
        for (u8 i = 0; i < 3 * 25; i++) {
            t_printf("hr: %d ", temp_data[i]);
            if (i % 6 == 0) {
                putchar('\n');
            }
        }
        delay(10000);
    }
    /* hr_sensor_io_ctl(HR_SENSOR_DISABLE, NULL); */
    /* delay(10000); */

//spo2
    /* hr_sensor_io_ctl(SPO2_SENSOR_ENABLE, NULL); */
    /* delay(100); */
    /* hr_sensor_io_ctl(SPO2_SENSOR_READ_DATA, temp_data); */
    /* for(u8 i=0;i<4*25;i++)t_printf("hr: %d ",temp_data[i]); */
}
#endif
