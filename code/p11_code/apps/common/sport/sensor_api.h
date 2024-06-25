#ifndef __SENSOR_API_H__
#define __SENSOR_API_H__


#define IRQ_GSENSOR_INDEX    (0)
#define IRQ_HRSENSOR_INDEX   (1)
#define IRQ_SPO2SENSOR_INDEX (2)


//类型和办法转变为 对应的cmd
#define SENSOR_TYPE(x)        ((x) << 24)
#define SENSOR_MEANS(x)       ((x) << 16)
#define SENSOR_USER(x)        ((x))


//cmd 转变为 对应的类型和办法
#define CMD2TYPE(x)  (((x)>>24)&0xff)
#define CMD2MEANS(x) (((x)>>16)&0xff)
#define CMD2USER(x)  (((x))&0xffff)


//传感器类型,最大支持255
#define SPO2_CMD      (1)
#define HR_SENSOR_CMD (2)
#define G_SENSOR_CMD  (3)



//传感器办法,最大支持255
#define SENSOR_DIASBLE (0)
#define SENSOR_ENABLE  (1)
#define SENSOR_READ    (2)
#define SENSOR_WRITE   (3)
#define SENSOR_INIT    (4)
#define SENSOR_CHECK_READ_BUF    (5)
#define SENSOR_SET_IRQ_BUF_SIZE      (6)


//用户需要自定义携带信息,16位,sdk暂时没有使用
#define USER_PRIV_NULL 0


//spo2 命令和办法
#define  SPO2_SENSORHUB_DISABLE    (SENSOR_TYPE(SPO2_CMD)|SENSOR_MEANS(SENSOR_DIASBLE)|SENSOR_USER(0))
#define  SPO2_SENSORHUB_ENABLE     (SENSOR_TYPE(SPO2_CMD)|SENSOR_MEANS(SENSOR_ENABLE)|SENSOR_USER(0))
#define  SPO2_SENSORHUB_READ_DATA  (SENSOR_TYPE(SPO2_CMD)|SENSOR_MEANS(SENSOR_READ)|SENSOR_USER(0))
#define  SPO2_SENSORHUB_INIT       (SENSOR_TYPE(SPO2_CMD)|SENSOR_MEANS(SENSOR_INIT)|SENSOR_USER(0))


//心率模块 命令和办法
#define  HR_SENSORHUB_DISABLE    (SENSOR_TYPE(HR_SENSOR_CMD)|SENSOR_MEANS(SENSOR_DIASBLE)|SENSOR_USER(0))
#define  HR_SENSORHUB_ENABLE     (SENSOR_TYPE(HR_SENSOR_CMD)|SENSOR_MEANS(SENSOR_ENABLE)|SENSOR_USER(0))
#define  HR_SENSORHUB_READ_DATA  (SENSOR_TYPE(HR_SENSOR_CMD)|SENSOR_MEANS(SENSOR_READ)|SENSOR_USER(0))
#define  HR_SENSORHUB_CHECK_LEN  (SENSOR_TYPE(HR_SENSOR_CMD)|SENSOR_MEANS(SENSOR_CHECK_READ_BUF)|SENSOR_USER(0))


//gsensor 模块 命令和办法
#define  G_SENSOR_DISABLE    (SENSOR_TYPE(G_SENSOR_CMD)|SENSOR_MEANS(SENSOR_DIASBLE)|SENSOR_USER(0))
#define  G_SENSOR_ENABLE     (SENSOR_TYPE(G_SENSOR_CMD)|SENSOR_MEANS(SENSOR_ENABLE)|SENSOR_USER(0))
#define  G_SENSOR_READ_DATA  (SENSOR_TYPE(G_SENSOR_CMD)|SENSOR_MEANS(SENSOR_READ)|SENSOR_USER(0))
#define  G_SENSOR_READ_CHECK_LEN  (SENSOR_TYPE(G_SENSOR_CMD)|SENSOR_MEANS(SENSOR_CHECK_READ_BUF)|SENSOR_USER(0))
#define  G_SENSOR_SET_IRQ_SIZE   (SENSOR_TYPE(G_SENSOR_CMD)|SENSOR_MEANS(SENSOR_SET_IRQ_BUF_SIZE)|SENSOR_USER(0))


extern int sensor_hub_timer_enable(u8 enable);

extern int sensor_hub_ioctl(int cmd, u16 len, u8 *data);

extern void sensorhub_irq_add(u8 index);

extern void sensorhub_irq_del(u8 index);

extern u8 sensorhub_irq_check(u8 index);


extern int sensor_hub_gsensor_enable(u8 on);

extern int sensor_hub_gsensor_check_len();

extern int sensor_hub_gsensor_read(u8 *data, u16 len);

extern int sensor_hub_hrsensor_low_power_detection(void);

extern int sensor_hub_hrsensor_factory_test(void);

extern int sensor_hub_hrsensor_check_len();

extern int sensor_hub_hrsensor_read(u8 *data, u16 len);

extern int sensor_hub_hrsensor_enable(u8 on);

extern int sensor_hub_spo2sensor_enable(u8 on);

extern int sensor_hub_gsensor_search(u32 *valid);

extern int sensor_hub_hrsensor_search(u32 *valid);

extern void sensor_init();


#endif
