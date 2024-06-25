#ifndef _HRSENSOR_MANAGE_H
#define _HRSENSOR_MANAGE_H

#include "app_config.h"
#include "system/includes.h"

enum {
    HR_SENSOR_ENABLE = 0,
    HR_SENSOR_DISABLE,
    SPO2_SENSOR_ENABLE,
    SPO2_SENSOR_DISABLE,
    HR_SENSOR_READ_DATA,
    SPO2_SENSOR_READ_DATA,
    HR_SEARCH_SENSOR,
};

typedef struct {
    u8   logo[20];
    u8(*heart_rate_sensor_init)(void);
    char (*heart_rate_sensor_check)(void);
    int (*heart_rate_sensor_ctl)(u8 cmd, void *arg);
} HR_SENSOR_INTERFACE;

typedef struct {
    u8   iic_hdl;
    u8   iic_delay;                 //这个延时并非影响iic的时钟频率，而是2Byte数据之间的延时
    int  init_flag;
} HR_SENSOR_INFO;

struct hrsensor_platform_data {
    u8     iic;
    char  hrSensor_name[20];
    int   hrSensor_int_io;
};

u8 hrsensor_write_nbyte(u8 w_chip_id, u8 register_address, u8 *buf, u8 data_len);
u8 hrsensor_read_nbyte(u8 r_chip_id, u8 register_address, u8 *buf, u8 data_len);
int hr_sensor_io_ctl(u8 cmd, void *arg);
int hr_sensor_init(void *_data);

extern HR_SENSOR_INTERFACE  hrsensor_dev_begin[];
extern HR_SENSOR_INTERFACE hrsensor_dev_end[];

#define REGISTER_HR_SENSOR(hrSensor) \
	static HR_SENSOR_INTERFACE hrSensor SEC_USED(.hrsensor_dev)

#define list_for_each_hrsensor(c) \
	for (c=hrsensor_dev_begin; c<hrsensor_dev_end; c++)

#define HRSENSOR_PLATFORM_DATA_BEGIN(data) \
		static const struct hrsensor_platform_data data = {

#define HRSENSOR_PLATFORM_DATA_END() \
};

#endif

