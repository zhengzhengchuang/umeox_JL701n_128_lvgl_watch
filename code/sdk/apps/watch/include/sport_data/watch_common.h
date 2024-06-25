#ifndef __WATCH_COMMON_H__
#define __WATCH_COMMON_H__
#include "os/os_type.h"
#define WATCH_DATA_TEST (!TCFG_GSENSOR_ENABLE)
#define C_HEART_TEST  1
#define __data_len 128
#define blocknum 10

#define	FOLLOW_EXERCISE_HEART_RATE  0x00
#define	CONTINUE_HEART_RATE_THRESHOLD_MAX  0x01
#define	CONTINUE_HEART_RATE_THRESHOLD_MAX_MIN 0x02

enum {
    NO_SPORT_MODE = 0x00,
    RUNNING_OUTSIDE,
    INDOOR_RUN,
    SPORT_MODE_MAX,
    SPORT_MODE_NOT_TRANS = 0XFF,
};
enum {
    SUCCESS,
    TYPE_ERROR,
    CALLBACK_ERROR,
    SENSOR_WORKBUF_ERROR,
    NOT_SPORT,
    TASK_OPEN_ERROR,
    EXECISE_STATUS_NOT_STOP,

};//ERROR
enum {
    SLEEP_ALL,
    SLEEP_SHALLOW,
    SLEEP_DEEP,
    SLEEP_REM,
    SLEEP_CONTINUE,

    SLEEP_START_TIME,
    SLEEP_STOP_TIME,
};//sleep_type
enum {
    WRIST,
    FALL,
    SEDENTARY,
    SLEEP,
    EXERCISE_HEART_RATE,
    CONTINUE_HEART_RATE,
};//DETECTION
enum {
    SPORT_STATUS_NULL,
    SPORT_STATUS_START,
    SPORT_STATUS_PAUSE,
    SPORT_STATUS_CONTINNUE,
    SPORT_STATUS_STOP
};//SPORT_STATUS
enum {
    REAL_DATA,
    LAST_DATA,
    MAX_DATA,
    MIN_DATA,
    RESTING_DATA,
};//DATA_TYPE
enum {
    SCREEN_LIGHT,
    SHAKE,
    CALL,
};//response_mode
enum {
    SWITCH_OFF,
    SWITCH_ON,
    SWITCH_TIMING,
};//enable

struct detection_callback {
    void (*wrist)(void);
    void (*fall)(void);
    void (*sedentary)(void);
    void (*heart_rate)(void);
    void (*sleep)(void);
};
struct motion_info {
    int step;
    int step_distance;
    int caloric;
    int motion_time;
    int motion_speed;
    int motion_pace;
    int motion_step_freq;
    int motion_step_stride;
    u8 recoverytime_hour;
    u8 recoverytime_min;
    u8 intensity;
    u8 intensity_mode;
    int intensity_time[5];
};

struct personal_info {
    unsigned char height;
    unsigned char age;
    u16 weight: 15;
    u16 gender: 1;
};

//算法
struct watch_algo {
    int (*personal_information_set)(struct personal_info *personal_info);
    int (*sensor_switch_set)(unsigned char type, unsigned char enable); //设置打开、关闭某一传感器
    int (*detection_ioctrl)(int arg_num, int *arg);
    int (*algo_heart_rate_task)(void);					//开启线程
    void(*algo_heart_rate_task_exit)(void);
    int (*algo_blood_oxygen_task)(void);					//开启线程
    void(*algo_blood_oxygen_task_exit)(void);
};

//运动
struct watch_execise {
    int (*execise_ctrl_status_set)(unsigned char execise_mode, unsigned char status);			//设置运动开始/暂停/继续/结束，输入参数为运动类型
    unsigned char (*execise_ctrl_status_get)(void);					//获取当前运动状态，用于APP与UI同步
    int (*execise_ctrl_status_clr)(void);					//清除当前运动状态，结束后调用
    unsigned char (*execise_mode_get)(void);							//获取当前运动类型，户外、室内
    int (*daily_motion_info_get)(struct motion_info *arg);		//全天数据
    int (*execise_info_get)(struct motion_info *arg);		//运动开始到结束累积的数据
    void (*execise_info_clr)(void);							//清除缓存运动数据
};

//睡眠
struct watch_sleep {
    int (*sleep_flag_buf_get)(unsigned char *buf);						//最近一天睡眠标志buf，用于绘图
    int (*sleep_flag_buf_len)(void);						//睡眠标志长度
    int (*sleep_time_analyse)(int *arg, int sleep_type);
    int (*sleep_time_start_stop_time)(int type, struct sys_time *sleep_time);
    void (*sleep_record_clr)(void);									//清除近一次睡眠缓存
};

//心率
struct watch_heart_rate {
    unsigned char (*heart_rate_data_get)(unsigned char type, unsigned char *valid);
    int (*heart_rate_day_buf_get)(unsigned char *buf, int buf_len, struct sys_time *time);
    int (*heart_rate_day_buf_len_get)(struct sys_time *time);		//	获取某一天的缓存数据长度
    void (*clr_hr_info)(void);//清空心率值
};


//血氧
struct watch_blood_oxygen {
    unsigned char (*blood_oxygen_get)(unsigned char type, unsigned char *valid);							//血氧
    int (*blood_oxygen_day_buf_get)(u8 *buf, int buflen, struct sys_time *time);			//获取某一天的缓存数据，参数为日期，NULL为当天
    int (*blood_oxygen_day_buf_len_get)(struct sys_time *time);		//	获取某一天的缓存数据长度
    void (*clr_spo2_info)(void);//清空血氧值
};

struct watch_work {
    int freq ;				//采样频率
    int len;				//数组长度
    u8  init_status;		//初始化标志
    u8  task_init;          //任务
    u8  task_run;          //任务运行标志
    u16 sensor_timer_id;		//定时器
    int flag[7];			//状态
    int motion_HR_type;		//运动心率模式
    char *workbuf;			//算法工作参数
    u8 fall;				//跌倒
};
struct sensor_heart_rate {
    int len;
    int fs;
    int out[2];

    u8 HR;
    u8 hrmax;
    u8 hrmin;
    u8 hrresting;

    u8 read_status: 2;
    u8 init_status: 2;
    u8 work_status: 2;
    u8 exercise_hr_detection_enable;
    u8 exercise_hr_detection_status;
    u8 exercise_hr_max;
    u8 continuous_hr_status;
    u8 continuous_hr_max;
    u8 continuous_hr_min;

    char *workbuf;
    char *wear_workbuf;
};

struct watch_SPO2_data {
    int len;
    int fs;
    int out[2];

    u8 SPO2;
    u8 spo2min;
    u8 spo2max;
    u8 init_status: 2;
    u8 read_status: 2;
    u8 work_status: 2; //0x00 关闭 0x01打开
    char *workbuf;
};

struct data_sec {
    u8 hrdatacnt ;
    uint32_t hrdata[75];
    OS_SEM hrsem;
};
struct sport_temp_data {
    u32 timestamp;
    int step;
    int distance;
    int kcal;
};
struct sleep_analyse {
    int time;
    short scale;
    char evaluate;
};
struct algo_sleep_data {
    struct sleep_analyse sleep_all;
    struct sleep_analyse sleep_deep;
    struct sleep_analyse sleep_shallow;
    struct sleep_analyse sleep_rem;
    char wake_up_times;
    char continuity_score;
    char sleep_status;		//睡眠状态
    char long_short_sleep_status;//长短睡眠
    short sleepdatacnt;		//睡眠长度
};
#pragma pack(1)//不平台对齐编译
typedef struct health_file_head {
    u8 type;
    u16 year;
    u8 month;
    u8 day;
    u16 crc;
    u8 version;
    u8 interval;
    u16 save;
} HEALTH_FILE_HEAD;
#pragma pack()//平台对齐编译
#pragma pack(1)//不平台对齐编译
struct health_file_data_head {
    u8 hour;
    u8 min;
    u16 len;
};
#pragma pack()//平台对齐编译
struct flash_vm_id_table {
    u16 id;
    u16 size;
};
struct health_file_info {
    u8 file_type;
    u16 file_total;
    u16 max_file_size;
    u16 cur_file_index;
    u16 cur_file_id;
    u16 cur_file_size;
    HEALTH_FILE_HEAD cur_file_head;
};
/*********************************************
				功能开关
**********************************************/
//运动健康总初始化开关
int watch_sensor_open(void);
int watch_sensor_close(void);
//计步
u8 step_start(void);
u8 step_stop(void);
//心率开关
int heart_rate_start(void);
int heart_rate_stop(void);
//血氧开关
int  blood_oxygen_start(void);
int  blood_oxygen_stop(void);
void sleep_record_start(void);
void sleep_record_stop(void);
/********************************************
				api--handle
********************************************/
void watch_algo_handle_get(struct watch_algo *p);
void watch_execise_handle_get(struct watch_execise *p);
void watch_blood_oxygen_handle_get(struct watch_blood_oxygen *p);
void watch_heart_rate_handle_get(struct watch_heart_rate *p);
void watch_sleep_handle_get(struct watch_sleep *p);

void algo_deal_sport_start(void);
void algo_deal_sport_end(void);

/********************************************
				运动
********************************************/
//运动控制
int execise_ctrl_status_set(unsigned char sport_mode, unsigned char status);
int set_sport_mode(u8 mode);
u8 execise_mode_get(void);
u8 execise_ctrl_status_get(void);
void execise_info_clr(void);
int execise_ctrl_status_clr(void);
//运动接口
int execise_info_get(struct motion_info *info);
u32 get_sport_start_time(struct sys_time *t);
u32 get_sport_end_time(struct sys_time *t);
int get_sport_pace_sec(void);
//全天数据
void clr_sport_info(void);
/********************************************
				血氧
********************************************/
//数据获取
int read_spo2_data_task(void);
void spo2_data_task_exit(void);
unsigned char blood_oxygen_data_get(unsigned char type, unsigned char *valid);
int get_day_blood_oxygen_data(u8 *buf, int buf_len, struct sys_time *time);
int get_day_blood_oxygen_data_len(struct sys_time *time);
//数据清除
void clr_blood_oxygen(void);

/********************************************
				心率
********************************************/
//数据获取
int read_heart_rate_task(void);
void heart_rate_task_exit(void);
unsigned char heart_rate_data_get(unsigned char type, unsigned char *valid);
int get_day_heart_rate_data(u8 *buf, int buf_len, struct sys_time *time);
int get_day_heart_rate_data_len(struct sys_time *time);
//数据清除
void clr_heart_rate(void);
/********************************************
				睡眠
********************************************/
int sleep_time_analyse(int *arg, int sleep_type);
//睡眠数据接口，用于绘图
// u8 get_sleep_record(struct sleep_record *p);//获取最近一次睡眠数据
void sleep_flag_free(void);//清除最近一次睡眠数据，并释放空间

int sleep_info_trans_buf(u8 *buf);
int sleep_info_trans_buf_len(void);

int sleep_time_start_stop_time(int type, struct sys_time *sleeptime);
void sleep_quality_analyse(void);
void refresh_sleep_record(u8 flag);
int get_watch_sleep_target(void);
int sleep_file_write(void);
/********************************************
				气压
********************************************/


/********************************************
				其他
********************************************/
int sensor_switch_set(unsigned char type, unsigned char enable);
int detection_init(void);
int detection_ioctrl(int arg_num, int *arg) ;
int personal_information_set(struct personal_info *personal_info);
int daily_motion_info_get(struct motion_info *info);
u8 set_watch_motion_switch(u8 watch_type, u8 enable);
u8 get_watch_motion_switch(u8 watch_type);

#endif




