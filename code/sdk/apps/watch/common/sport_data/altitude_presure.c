#include "event.h"
#include "system/includes.h"
#include "app_config.h"
#include "sport_data/altitude_presure.h"
#include "sport_data/watch_common.h"
#include "sport_data/watch_time_common.h"
#include "bmp280.h"
#include "math.h"
#include "dev_manager.h"
#include "stdlib.h"
#include "fs/fs.h"
#include "sys_time.h"
#include "time.h"
#if defined(TCFG_BMP280_ENABLE) && TCFG_BMP280_ENABLE

#define AP_TIME_INTERVAL 5//时间5-10min
#define alti_airpresure_file_enable 0//气压海拔文件写读开关
#define alti_airpresure_day_buf_enable 0//全天数据缓存,不记录文件且需要缓存全天数据画图时打开
#define AL_PATH "storage/sd1/C/sport/altitude/AL"
#define AP_PATH "storage/sd1/C/sport/altitude/AP"

extern void printf_buf(u8 *buf, u32 len);


//********************************************
//             气压海拔数据参数
//********************************************
static struct air_presure {
    int systimeid;
    u16 air_presure_value;
    u16 min_value;
    u16 max_value;
    u16 day_buf_cnt;
    u8 work_status;
    u8 day_buf_enable;

    u8 Tcount;
    u16 *day_buf;
    u8 air_presure_buf[2 * 60 / AP_TIME_INTERVAL + 1];

} air_presure_data = {
    .work_status = 0,
    .day_buf_enable = alti_airpresure_day_buf_enable
};

static struct altitude {
    u32 altitude_value;
    u32 min_value;
    u32 max_value;
    u32 day_buf_cnt;
    u8  altitude_buf[4 * 60 / AP_TIME_INTERVAL + 1];
    u32 *day_buf;
} altitude_data ;

#if alti_airpresure_file_enable

static struct ap_file_data {
    u8 *presure_databuf; //1data=2byte
    u8 presure_datalen;
    u8 *altitude_databuf; //1data=4byte 高2BYTE整数 低2BYTR小数
    u8 altitude_datalen;
    struct sys_time filetime;
    u8 apfile_status;
} apfiledata;

//***************************************************
//   将海拔气压数据写入文件
//***************************************************
int altitude_presure_write_file(int type)
{
    printf("%s %d", __func__, __LINE__);

    struct sys_time ptime;
    watch_file_get_sys_time(&ptime);

    char path[45];
    if (type == 0) {
        sprintf(path, "%s%02d%02d%02d", AL_PATH, (ptime.year) % 100, ptime.month, ptime.day);
    } else {
        sprintf(path, "%s%02d%02d%02d", AP_PATH, (ptime.year) % 100, ptime.month, ptime.day);
    }
    printf("%s", path);
    FILE *fp = fopen(path, "w+");
    int file_len = flen(fp);
    if (fp == NULL) {
        printf("open_file_error!!!");
        free(fp);
        return 0;
    }
    u8 timebuf[8];
    //写时间
    if (file_len == 0) { //新建文件时首行写入时间
        sprintf(timebuf, "%c%c%c%c%c%c%c", (ptime.year / 100), (ptime.year % 100), \
                ptime.month, ptime.day, ptime.hour, ptime.min, ptime.sec);
        printf("%d%d-%d-%d", ptime.year / 100, ptime.year % 100, ptime.month, ptime.day);
        fwrite(fp, timebuf, strlen(timebuf));

    } else {
        fseek(fp, file_len, 0); //调整文件位置
    }
    //写入一小时数据
    if (type == 0) {
        fwrite(fp, altitude_data.altitude_buf, 4 * 60 / AP_TIME_INTERVAL);
    } else {
        fwrite(fp, air_presure_data.air_presure_buf, 2 * 60 / AP_TIME_INTERVAL);
    }
    fclose(fp);
    return 1;
}


//  解析文件时间内容,用于文件校验
int altitude_presure_file_time(u8 *buf, struct sys_time *ptime)
{
    struct sys_time filetime;
    filetime.year = buf[0] * 100 + buf[1];
    filetime.month = buf[2];
    filetime.day = buf[3];
    filetime.hour = buf[4];
    filetime.min = buf[5];
    filetime.sec = 0; //精度只到分钟
    if ((filetime.year == ptime->year) & (filetime.month == ptime->month) & (filetime.day == ptime->day)) {
        apfiledata.apfile_status = 1;
        apfiledata.filetime = filetime;
        return 1;
    } else {
        apfiledata.apfile_status = 0;
        return 0;
    }
}
int altitude_presure_read_file(struct sys_time *ptime, int type)
{
    printf("%s,%d", __func__, __LINE__);
    int mode = 0;
    if (ptime == NULL) {
        mode = 1;
        ptime = zalloc(sizeof(struct sys_time));
        watch_file_get_sys_time(ptime);//传入空指针默认当天
        if (ptime == NULL) {
            printf("get_sys_time_error!!!");
            return 0;
        }
    }
    char path[45];

    if (type == 0) {
        sprintf(path, "%s%02d%02d%02d", AL_PATH, (ptime->year) % 100, ptime->month, ptime->day);
    } else {
        sprintf(path, "%s%02d%02d%02d", AP_PATH, (ptime->year) % 100, ptime->month, ptime->day);
    }
    FILE *fp = fopen(path, "w+");
    if (fp == NULL) {
        printf("open_file_error!!!");
        return 0;
    }
    int file_len = flen(fp);
    //校验文件安全
    u8 tbuf[8];
    fread(fp, tbuf, 7);
    //printf("day=%d%d-%d-%d\0", tbuf[0], tbuf[1], tbuf[2], tbuf[3]);
    if (altitude_presure_file_time(tbuf, ptime) == 0) {
        printf("file_time_error!\n");//文件时间错误
        return 0;
    }
    //定位到第一个数据,开始读取数据
    fseek(fp, 7, 0);
    if (type == 0) {
        fread(fp, apfiledata.altitude_databuf, file_len - 7);
        apfiledata.altitude_datalen = file_len - 7;
        if (mode == 1) {
            sprintf(apfiledata.altitude_databuf, "%s%s", apfiledata.altitude_databuf, altitude_data.altitude_buf);
            apfiledata.altitude_datalen += strlen(air_presure_data.air_presure_buf);
        }
    } else {
        fread(fp, apfiledata.presure_databuf, file_len - 7);
        apfiledata.altitude_datalen = file_len - 7;
        if (mode == 1) {
            sprintf(apfiledata.presure_databuf, "%s%s", apfiledata.presure_databuf, air_presure_data.air_presure_buf);
            apfiledata.altitude_datalen += strlen(air_presure_data.air_presure_buf);
        }
    }
    fseek(fp, file_len, 0);
    fclose(fp);
    return 1;
}

#endif
//***************************************************
//   获取气压数据
//***************************************************
void altitude_airpresure_refresh_data(void)
{
//    printf("%s %d", __func__, __LINE__);
    if (air_presure_data.work_status != 1) {
        return ;
    }

    bmp280_set_work_mode(BMP280_NORMAL_MODE);
    bmp_pressure_data bmp_pressure;
    bmp_temperature_data bmp_temperature;
    float bmp280_altitude = 0;
    bmp280_get_temperature_and_pressure(&bmp_temperature, &bmp_pressure);
    bmp280_altitude = bmp280_get_altitude(bmp_temperature, bmp_pressure);
    bmp280_set_work_mode(BMP280_SLEEP_MODE);
    air_presure_data.air_presure_value = (u16)((bmp_pressure / 10000) & 0xffff);


    //气压
    if (air_presure_data.min_value == 0) {
        air_presure_data.min_value = air_presure_data.air_presure_value;
    }
    if ((air_presure_data.air_presure_value < air_presure_data.min_value) & (air_presure_data.air_presure_value != 0)) {
        air_presure_data.min_value = air_presure_data.air_presure_value;
    }
    if (air_presure_data.air_presure_value > air_presure_data.max_value) {
        air_presure_data.max_value = air_presure_data.air_presure_value;
    }
    printf("airpresure real=%d,min=%d,max=%d", air_presure_data.air_presure_value, \
           air_presure_data.min_value, air_presure_data.max_value);


    //海拔2byte+2byte定点数 整数<<16+小数*10000//小数部分精度有限

    altitude_data.altitude_value = (((int)bmp280_altitude) << 16) | ((int)((bmp280_altitude - (int)bmp280_altitude) * 10000));

    if (altitude_data.min_value == 0) {
        altitude_data.min_value = altitude_data.altitude_value;
    }
    if ((altitude_data.altitude_value < altitude_data.min_value) & (altitude_data.altitude_value != 0)) {
        altitude_data.min_value = altitude_data.altitude_value;
    }
    if (altitude_data.altitude_value > altitude_data.max_value) {
        altitude_data.max_value = altitude_data.altitude_value;
    }
    printf("altitude real=%d.%5d,%d,min=%d,max=%d", altitude_data.altitude_value >> 16, altitude_data.altitude_value & 0x3fff, altitude_data.altitude_value, \
           altitude_data.min_value, altitude_data.max_value);

    if (air_presure_data.day_buf_enable == 1) {
        air_presure_data.day_buf = zalloc(2 * (24 * 60 / AP_TIME_INTERVAL + 1));
        altitude_data.day_buf = zalloc(4 * (24 * 60 / AP_TIME_INTERVAL + 1));
        air_presure_data.day_buf_cnt = 0;
        altitude_data.day_buf_cnt = 0;
        air_presure_data.day_buf_enable = 2;
    }
    if (air_presure_data.day_buf_enable == 2) {
        air_presure_data.day_buf[air_presure_data.day_buf_cnt] = air_presure_data.air_presure_value;
        air_presure_data.day_buf_cnt++;
        altitude_data.day_buf[altitude_data.day_buf_cnt] = altitude_data.altitude_value;
        altitude_data.day_buf_cnt++;
    }

#if alti_airpresure_file_enable

    sprintf(air_presure_data.air_presure_buf, "%s%c%c", air_presure_data.air_presure_buf, (air_presure_data.air_presure_value >> 8) & 0xff, \
            air_presure_data.air_presure_value & 0xff);

    sprintf(altitude_data.altitude_buf, "%s%c%c%c%c", altitude_data.altitude_buf, (altitude_data.altitude_value >> 24) & 0xff, \
            (altitude_data.altitude_value >> 16) & 0xff, (altitude_data.altitude_value >> 8) & 0xff, altitude_data.altitude_value & 0xff);


    air_presure_data.Tcount++;
    if (air_presure_data.Tcount == (60 / AP_TIME_INTERVAL)) {
        altitude_presure_write_file(0);
        altitude_presure_write_file(1);
        air_presure_data.Tcount = 0;
    }
#endif

}
//********************************************
//           气压数据处理开关
//********************************************
static struct _bmp280_dev_platform_data bmp280_iic_info = {
    .iic_hdl = 1,
    .iic_delay = 0
};

int altitude_airpresure_start(void)
{
    printf("%s ", __func__);
    if (air_presure_data.work_status == 1) {
        printf("Repeated call%s", __func__);
        return 0;
    }
    if (bmp280_init(&bmp280_iic_info)) {
        printf("bmp280 init succ");
    } else {
        printf("bmp280 init error");
        return 0;
    }
#if alti_airpresure_file_enable
    apfiledata.altitude_databuf = zalloc(4 * 24 * 60 / AP_TIME_INTERVAL);
    apfiledata.presure_databuf = zalloc(2 * 24 * 60 / AP_TIME_INTERVAL);
    if ((apfiledata.altitude_databuf == NULL) | (apfiledata.presure_databuf == NULL)) {
        printf("apfiledata.databuf zalloc error");
        return 0;
    }
#endif

    air_presure_data.systimeid = sys_timer_add(NULL, altitude_airpresure_refresh_data,  60 * 1000 * AP_TIME_INTERVAL); //TIME_INTERVAL分钟进行一次读数
    air_presure_data.work_status = 1;
    altitude_airpresure_refresh_data();
    printf("%s succ", __func__);
    return 1;
}
int altitude_airpresure_stop(void)
{
    printf("%s", __func__);
    if (air_presure_data.work_status == 0) {
        printf("Repeated call%s", __func__);
        return 0;
    }
#if alti_airpresure_file_enable
    if ((apfiledata.altitude_databuf != NULL) | (apfiledata.presure_databuf != NULL)) {
        free(apfiledata.altitude_databuf);
        free(apfiledata.presure_databuf);
    }
#endif
    if (air_presure_data.systimeid != 0) {
        sys_timer_del(air_presure_data.systimeid);//注销定时器
        //sleep模式
    }
    air_presure_data.work_status = 0;
    printf("%s succ", __func__);
    return 1;
}


/* 设置气压海拔数据缓存 */
int altitude_airpresure_day_buf_enable(void)
{
    printf("%s", __func__);
    if (air_presure_data.day_buf_enable != 0) {

        printf("Repeated call%s", __func__);
        return 0;
    }
    air_presure_data.day_buf_enable = 1;
    printf("%s succ", __func__);
    return 0;
}
int altitude_airpresure_day_buf_disable(void)
{
    printf("%s", __func__);

    if (air_presure_data.day_buf_enable == 0) {
        printf("Repeated call%s", __func__);
        return 0;
    }
    if (air_presure_data.day_buf == NULL) {
        return 0;
    }
    free(air_presure_data.day_buf);
    air_presure_data.day_buf = NULL;
    free(altitude_data.day_buf);
    altitude_data.day_buf = NULL;
    air_presure_data.day_buf_enable = 0;
    printf("%s succ", __func__);
    return 0;
}
//********************************************
//     		对外数据接口
//********************************************

u16 get_data_air_presure(void)
{
    printf("%s=%d", __func__, air_presure_data.air_presure_value);
    return air_presure_data.air_presure_value;
}

u16 get_min_air_presure(void)
{
    printf("%s=%d", __func__, air_presure_data.min_value);
    return air_presure_data.min_value;
}

u16  get_max_air_presure(void)
{
    printf("%s=%d", __func__, air_presure_data.max_value);
    return air_presure_data.max_value;
}

u32 get_data_altitude(void)
{
    printf("%s=%d", __func__, altitude_data.altitude_value);
    return altitude_data.altitude_value;
}

u32 get_min_altitude(void)
{
    printf("%s=%d", __func__, altitude_data.min_value);
    return altitude_data.min_value;
}

u32  get_max_altitude(void)
{
    printf("%s=%d", __func__, altitude_data.max_value);
    return altitude_data.max_value;
}

u16 get_day_air_presure_data(u8 *buf) //1data=2BYTE
{
    printf("%s", __func__);
#if alti_airpresure_file_enable
    altitude_presure_read_file(NULL, 1);
    memcpy(buf, apfiledata.presure_databuf, apfile_data.presure_datalen);
    return apfiledata.presure_datalen;
#else
    if (air_presure_data.day_buf_enable == 2) {
        memcpy(buf, air_presure_data.day_buf, air_presure_data.day_buf_cnt * 2);
        return air_presure_data.day_buf_cnt * 2;
    } else {
        return 0;
    }
#endif
}


u32 get_day_altitude_data(u8 *buf)//1data=2BYTE 高2BYTE整数 低2BYTR小数
{
    printf("%s", __func__);
#if alti_airpresure_file_enable
    altitude_presure_read_file(NULL, 0);
    memcpy(buf, apfiledata.altitude_databuf, apfile_data.altitude_datalen);
    return apfiledata.altitude_datalen;
#else
    if (air_presure_data.day_buf_enable == 2) {
        memcpy(buf, altitude_data.day_buf, altitude_data.day_buf_cnt * 4);
        return altitude_data.day_buf_cnt * 4;
    } else {
        return 0;
    }
#endif
}

void clr_air_presure_altitude(void) //清除数据
{
    if (air_presure_data.work_status == 1) {
        air_presure_data.air_presure_buf[0] = 0;
        air_presure_data.air_presure_value = 0;
        air_presure_data.min_value = 0;
        air_presure_data.max_value = 0;
        air_presure_data.Tcount = 0;
        if (air_presure_data.day_buf_enable) {
            air_presure_data.day_buf[0] = 0;
            air_presure_data.day_buf_cnt = 0;
            altitude_data.altitude_buf[0] = 0;
            altitude_data.altitude_value = 0;
        }
        altitude_data.min_value = 0;
        altitude_data.max_value = 0;
        altitude_data.day_buf[0] = 0;
        altitude_data.day_buf_cnt = 0;


    }
}
#else
u16 get_data_air_presure(void)
{
    return 0;
}
u16 get_min_air_presure(void)
{
    return 0;
}
u16  get_max_air_presure(void)
{
    return 0;
}

u32 get_data_altitude(void)
{
    return 0;
}
u32 get_min_altitude(void)
{
    return 0;
}
u32  get_max_altitude(void)
{
    return 0;
}

u16 get_day_air_presure_data(u8 *buf) //1data=2BYTE
{
    return 0;
}


u32 get_day_altitude_data(u8 *buf)//1data=2BYTE 高2BYTE整数 低2BYTR小数
{
    return 0;
}

void clr_air_presure_altitude(void) //清除数据
{}
#endif
