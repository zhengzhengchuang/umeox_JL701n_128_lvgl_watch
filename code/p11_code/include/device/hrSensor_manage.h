#ifndef _HRSENSOR_MANAGE_H
#define _HRSENSOR_MANAGE_H

#include "app_config.h"
#include "includes.h"
#include "circular_buf.h"

#define dev_log(fmt,...)  printf("[dev_hr] %s " fmt "\n",__func__, ##__VA_ARGS__)

enum {
    HR_SENSOR_ENABLE = 0,
    HR_SENSOR_DISABLE,
    SPO2_SENSOR_ENABLE,
    SPO2_SENSOR_DISABLE,
    HR_SENSOR_READ_DATA,
    HR_SENSOR_CHECK_DATA,
    SPO2_SENSOR_READ_DATA,
    SPO2_SENSOR_CHECK_DATA,
    HR_SEARCH_SENSOR,
    HR_SENSOR_IRQ,
    SPO2_SENSOR_IRQ,
    HR_SENSOR_LP_DETECTION,  //低功耗检测：一般检测佩戴状态
    HR_SENSOR_FACTORY_TEST,  //工厂测试模式：一般包含漏光和灰卡测试
};


typedef struct {
    u8   logo[20];
    u8(*heart_rate_sensor_init)(cbuffer_t  *hrcbuf, cbuffer_t  *spo2cbuf);
    char (*heart_rate_sensor_check)(void);
    int (*heart_rate_sensor_ctl)(u8 cmd, void *arg);
    void (*accel_data_input)(void *accel, u8 point);
} HR_SENSOR_INTERFACE;

typedef struct {
    u8   iic_hdl;
    u8   iic_delay;                 //这个延时并非影响iic的时钟频率，而是2Byte数据之间的延时
    int  init_flag;
} HR_SENSOR_INFO;

struct hrsensor_platform_data {
    u8    iic;
    char  hrSensor_name[20];
    int   hrSensor_int_io;
};

u8 hrsensor_write_nbyte(u8 w_chip_id, u8 register_address, u8 *buf, u8 data_len);
u8 hrsensor_read_nbyte(u8 r_chip_id, u8 register_address, u8 *buf, u8 data_len);
int hr_sensor_io_ctl(u8 cmd, void *arg);
int hr_sensor_init(void *_data, cbuffer_t *hr_cbuf, cbuffer_t *spo2_cbuf);
void hr_sensor_req_irq(void);

extern HR_SENSOR_INTERFACE hrsensor_dev_begin[];
extern HR_SENSOR_INTERFACE hrsensor_dev_end[];

#define REGISTER_HR_SENSOR(hrSensor) \
	HR_SENSOR_INTERFACE hrSensor SEC_USED(.hrsensor_dev)

#define list_for_each_hrsensor(c) \
	for (c=hrsensor_dev_begin; c<hrsensor_dev_end; c++)

#define HRSENSOR_PLATFORM_DATA_BEGIN(data) \
		static const struct hrsensor_platform_data data = {

#define HRSENSOR_PLATFORM_DATA_END() \
};

#endif

