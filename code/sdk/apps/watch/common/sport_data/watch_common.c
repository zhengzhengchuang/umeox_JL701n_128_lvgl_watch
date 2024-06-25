#include "event.h"
#include "system/includes.h"
#include "app_config.h"
#include "sport_data/watch_api.h"
#include "sport_data/HeartRate_api.h"
#include "sport_data/SPO2_api.h"
#include "sport_data/watch_common.h"
#include "sport_data/watch_time_common.h"
#include "sport_data/detection_response.h"
#include "gSensor/gSensor_manage.h"
#include "hr_sensor/hrSensor_manage.h"
#include "sport_data/sport.h"
#include "sport_data/sleep.h"
#include "sport_info_opt.h"
#include "sport_info_exercise_heart_rate.h"
#include "sport_info_personal_info.h"
#include "sport_info_sedentary.h"
#include "sport_info_fall_detection.h"
#include "sport_info_raise_wrist.h"
#include "sport_info_sleep_detection.h"
#include "sys_time.h"
#include "clock_cfg.h"
#include "motion_module.h"
#include "user_p11_cmd.h"
#include "timestamp/timestamp.h"
#include "ui/ui_api.h"
#define USED_TIMER         (1)
#define USED_TASK          (2)
#define GSENSOR_TASK_USED  (USED_TASK)
//**************************************************
//                      配置
//**************************************************
#define WATCHLEN	600				//GSENSOR数据量；8的倍数，范围64-600
#define WATCHFREQ	200				//GSENSOR频率
#define	WATCHPOINT	3				//通道数量3
#define HRFREQ		25				//HRSENSOR频率
#define	HRPOINT		3				//通道数量3
#define HRLEN 		((WATCHLEN*HRFREQ*HRPOINT)/(WATCHFREQ*WATCHPOINT))		//HRSENSOR数据量
#define SPO2FREQ	HRFREQ			//SPO2频率,与HRSENSOR同一传感器
#define	SPO2POINT	4				//通道数量4
#define SPO2LEN 	((WATCHLEN*SPO2FREQ*SPO2POINT)/(WATCHFREQ*WATCHPOINT))//SPO2数据量
#define	SENSOR_INTERVAL ((1000*WATCHLEN)/(WATCHFREQ*WATCHPOINT))
#define HRSENSOR_READ_FILT_TIME 20	//过滤不稳定数据的时间，建议为12-20s
#define HRSENSOR_READ_SINGLE_PASS_TIME 40 //单次读取的最长时间，建议为30-40s
#define CONTINUOUS_HR_WARN_TIME 10*60//连续心率模式下，心率连续10MIN超过阈值触发心率预警
#define CONTINUE_HEART_RATE_MODE FOLLOW_EXERCISE_HEART_RATE//连续心率阈值设置，跟随运动心率大小,需要APP支持

//默认的个人参数配置，可以通过APP修改
#define	HEIGHT		175		//cm
#define WEIGHT		55		//kg
#define	AGE			18		//
#define GENDER		1		//女0男1
#define INTENSITY	0		//0最大心率百分比 1储备心率百分比
//计步使能，抬腕使能，睡眠使能，跌倒检测使能, 久坐提醒使能 ,运动使能采样率，数据长度，贴片类型//其中贴片类型为传感器芯片上的贴片方位选择，共有0~7，8种方位，在最终手表硬件焊接后确定方位此参数可确定
WATCH_parameter WP = { 1, 1, 1, 1, 1, 1, WATCHFREQ, WATCHLEN, 1};
Personal_inf PI = {HEIGHT, WEIGHT, AGE, GENDER}; //身高cm;体重kg;年龄;性别(男1)//默认值，根据个人需要调用函数设置
//在sport_data_func.c中将 宏SPORT_DATA_TIMED_NOFIFYCATION_TIME 配置为相应的时间可以实现定时向手机推送数据，默认关闭
int gsensorlen = WATCHLEN;
struct watch_work watch;
struct watch_SPO2_data wspo2;
struct sensor_heart_rate whr;
struct sport_temp_data wtemp;
struct algo_sleep_data algo_sleep;
static struct motion_info __motion_info;
static u8 search_sensor_cnt = 0;
OS_MUTEX SENSOR_MUTEX;
OS_SEM SPO2_SEM;
OS_SEM HR_SEM;
static int data_temp_storage(void);
extern void mem_stats(void);
//***************************************************************
//							api
//***************************************************************
struct watch_algo watch_algo_hd = {
    //设置个人信息
    .personal_information_set = personal_information_set,
    //设置传感器开关
    .sensor_switch_set = sensor_switch_set,
    //设置健康检测功能
    .detection_ioctrl = detection_ioctrl,
    //心率线程
    .algo_heart_rate_task = read_heart_rate_task,
    .algo_heart_rate_task_exit = heart_rate_task_exit,
    //血氧线程
    .algo_blood_oxygen_task = read_spo2_data_task,
    .algo_blood_oxygen_task_exit = spo2_data_task_exit,
};
struct watch_execise watch_execise_hd = {
    //设置运动开始/暂停/继续/结束，输入参数为运动类型
    .execise_ctrl_status_set = execise_ctrl_status_set,
    //获取当前运动状态，用于APP与UI同步
    .execise_ctrl_status_get = execise_ctrl_status_get,
    //清除当前运动状态，结束后调用
    .execise_ctrl_status_clr = execise_ctrl_status_clr,
    //获取当前运动类型，户外、室内
    .execise_mode_get = execise_mode_get,
    //全天运动数据获取
    .daily_motion_info_get = daily_motion_info_get,
    //运动开始到结束累积的数据
    .execise_info_get = execise_info_get,
    //清除缓存运动数据
    .execise_info_clr = execise_info_clr,
};
struct watch_blood_oxygen blood_oxygen_hd = {
    //获取血氧数据
    .blood_oxygen_get = blood_oxygen_data_get,
    //获取全天血氧数据
    .blood_oxygen_day_buf_get = get_day_blood_oxygen_data,
    //获取全体血氧数据长度
    .blood_oxygen_day_buf_len_get = get_day_blood_oxygen_data_len,
};
struct watch_heart_rate heart_rate_hd = {
    //获取心率数据
    .heart_rate_data_get = heart_rate_data_get,
    //获取全天心率数据
    .heart_rate_day_buf_get = get_day_heart_rate_data,
    //获取全体心率数据长度
    .heart_rate_day_buf_len_get = get_day_heart_rate_data_len,
};
struct watch_sleep sleep_hd = {
    //最近一天睡眠标志buf，用于绘图
    .sleep_flag_buf_get = sleep_info_trans_buf,
    //睡眠标志长度
    .sleep_flag_buf_len = sleep_info_trans_buf_len,
    //不同类型睡眠的时长、比例、评估
    .sleep_time_analyse = sleep_time_analyse,
    .sleep_time_start_stop_time = sleep_time_start_stop_time,
    //清除近一次睡眠缓存
    .sleep_record_clr = sleep_flag_free,
};
struct detection_callback detection_hd = {
    .wrist = wrist_detection_response,
    .fall = fall_detection_response,
    .sleep = sleep_detecion_response,
    .sedentary = sedentary_detecion_response,
    .heart_rate = sport_hr_detection_response,
};
void watch_algo_handle_get(struct watch_algo *p)
{
    memcpy(p, &watch_algo_hd, sizeof(struct watch_algo));
}
void watch_execise_handle_get(struct watch_execise *p)
{
    memcpy(p, &watch_execise_hd, sizeof(struct watch_execise));
}
void watch_blood_oxygen_handle_get(struct watch_blood_oxygen *p)
{
    memcpy(p, &blood_oxygen_hd, sizeof(struct watch_blood_oxygen));
}
void watch_heart_rate_handle_get(struct watch_heart_rate *p)
{
    memcpy(p, &heart_rate_hd, sizeof(struct watch_heart_rate));
}
void watch_sleep_handle_get(struct watch_sleep *p)
{
    memcpy(p, &sleep_hd, sizeof(struct watch_sleep));
}
/****************************************************************
  						血氧
*****************************************************************/
static int spo2_algo_enable(void)
{
    u8 search_spo2sensor;
    hr_sensor_io_ctl(HR_SEARCH_SENSOR, &search_spo2sensor);
    if (search_spo2sensor == 0) {
        log_e("spo2sensor_not_find\n");
        return false;
    }
    if (wspo2.workbuf != NULL) {
        log_e("wspo2.workbuf not NULL\n");
        return false;
    }
    int bufsize = get_SPO2_buf(SPO2LEN, SPO2FREQ);
    wspo2.workbuf = (char *)malloc(bufsize);
    if (wspo2.workbuf == NULL) {
        log_e("spo2_workbuf_malloc_error\n");
        return false;
    }
    init_SPO2(wspo2.workbuf, SPO2FREQ, SPO2LEN);
    return true;
}
static int spo2_algo_disable(void)
{
    printf("%s", __func__);
    free(wspo2.workbuf);
    wspo2.workbuf = NULL;
    return 0;
}
static void refresh_spo2_data(void *p)
{
    printf("%s %s", __func__, os_current_task());
    int cnt = 0;
    int spo2sensorbuf[SPO2LEN] = {0};
    int spo2len = SPO2LEN;
    u8  SPO2_DATA = 0;
    int spo2dlytime = (SPO2LEN * 100) / (SPO2POINT * SPO2FREQ);
    while (1) {
        os_sem_pend(&SPO2_SEM, 0);
        printf("%s %s", __func__, os_current_task());
        os_mutex_pend(&SENSOR_MUTEX, 0);//防止心率血氧冲突
        /* 配置传感器 */
        hr_sensor_io_ctl(SPO2_SENSOR_DISABLE, NULL);//关闭传感器
        hr_sensor_io_ctl(SPO2_SENSOR_ENABLE, &spo2len);//打开传感器
        /* 延时等待第一组数据 */
        os_time_dly(spo2dlytime);
        for (cnt = 0; cnt <= (HRSENSOR_READ_SINGLE_PASS_TIME * (u8)(SPO2FREQ * SPO2POINT / spo2len));) {
            if (hr_sensor_io_ctl(SPO2_SENSOR_READ_DATA, spo2sensorbuf) == 0) {//拿不到数据
                os_time_dly(spo2dlytime);//延时
                search_sensor_cnt++;//计数
                if (search_sensor_cnt == 10) {//连续十次读数异常、结束读数
                    log_e("sensor_dont_online\n");
                    search_sensor_cnt = 0;
                    break;
                }
                continue;
            }
            search_sensor_cnt = 0;
            SPO2_run(wspo2.workbuf, spo2len, spo2sensorbuf, wspo2.out);
            SPO2_DATA = getCurrentSPO2(wspo2.workbuf);
            /* 数据处理 */
            if (cnt % ((u8)(SPO2FREQ * SPO2POINT / spo2len)) == 0) {//约每秒打印一次
                printf("TASK SPO2=%d wear=%d cnt=%d\n", wspo2.SPO2, wspo2.out[0], cnt);
            }
            if ((SPO2_DATA != 0) & (cnt > (HRSENSOR_READ_FILT_TIME * (u8)(SPO2FREQ * SPO2POINT / spo2len)))) { //(SPO2FREQ*4/SPO2LEN)约1s
                if (SPO2_DATA != 0) {
                    wspo2.SPO2 = SPO2_DATA;
                }
                if (wspo2.spo2min == 0) {
                    wspo2.spo2min = wspo2.SPO2;
                }
                if ((wspo2.SPO2 < wspo2.spo2min) & (wspo2.SPO2 != 0)) {
                    wspo2.spo2min = wspo2.SPO2;
                }
                if (wspo2.SPO2 > wspo2.spo2max) {
                    wspo2.spo2max = wspo2.SPO2;
                }
            }
            if (wspo2.read_status == 0) {//用于退出当前线程
                break;
            }
            cnt++;
        }
        hr_sensor_io_ctl(SPO2_SENSOR_DISABLE, NULL);//关闭传感器
        wspo2.read_status = 0;
        os_mutex_post(&SENSOR_MUTEX);//防止心率血氧冲突
        if ((whr.continuous_hr_status == 1) | (whr.exercise_hr_detection_status == 1))  {
            watch_algo_hd.algo_heart_rate_task();
        }
    }
}

int read_spo2_data_task(void)
{
    printf("%s readstatus=%d", __func__, wspo2.read_status);
    if (wspo2.read_status == 0) {
        wspo2.read_status = 1;
        wspo2.SPO2 = 0;
        os_sem_post(&SPO2_SEM);
        return SUCCESS;
    }
    return TASK_OPEN_ERROR;
}
void spo2_data_task_exit(void)
{
    wspo2.read_status = 0;
}
static u8 get_blood_oxygen()
{
    printf("%s ", __func__);
#if TCFG_SPO2_SENSOR_ENABLE
    if (wspo2.work_status == 1) {
        printf("spo2=%d  wear=%d", wspo2.SPO2, wspo2.out[0]);
        return wspo2.SPO2;
    }
#else
    wspo2.SPO2 = 99;
    return wspo2.SPO2;

#endif
    return 0;
}

static u8 get_blood_oxygen_wear(void)
{
    return (u8)wspo2.out[0];
}
static u8 get_max_blood_oxygen(void)
{
    return wspo2.spo2max;
}
static u8 get_min_blood_oxygen(void)
{
    return wspo2.spo2min;
}
static void clr_spo2_info(void) //重置血氧
{
    if (watch.init_status == 1) {
        wspo2.SPO2 = 0;
        wspo2.spo2min = 0;
        wspo2.spo2max = 0;
    }
}
unsigned char blood_oxygen_data_get(unsigned char type, unsigned char *valid)
{
    if (valid != NULL) {
        *valid = get_blood_oxygen_wear();
    }
    switch (type) {
    case REAL_DATA:
        read_spo2_data_task();
        return get_blood_oxygen();
    case LAST_DATA:
        return get_blood_oxygen();
        break;
    case MAX_DATA:
        return get_max_blood_oxygen();
    case MIN_DATA:
        return get_min_blood_oxygen();
    }
    return 0;
}
//**************************************************
//				心率
//**************************************************
static int hr_algo_enable(void)
{
    char search_hrsensor;
    hr_sensor_io_ctl(HR_SEARCH_SENSOR, &search_hrsensor);
    if (search_hrsensor == 0) {
        log_e("watch_hrsensor_not_find\n");
        return false;
    }
    if (whr.workbuf != NULL) {
        log_e("whr.workbuf not NULL\n");
        return false;
    }
    if (whr.wear_workbuf != NULL) {
        log_e("whr.wearbuf not null \n");
        return false;
    }
    int bufsize = get_HeartRate_buf(HRLEN, HRFREQ);
    whr.workbuf = (char *)malloc(bufsize);
    if (whr.workbuf == NULL) {
        log_e("HR_workbuf_malloc_error");
        return false;
    }
    init_HeartRate(whr.workbuf, HRFREQ, HRLEN);
    int wear_bufsize = get_weardect_buf(HRLEN);
    whr.wear_workbuf = (char *)malloc(wear_bufsize);
    if (whr.wear_workbuf == NULL) {
        log_e("wear_workbuf malloc error");
        return false;
    }
    init_WearDect(whr.wear_workbuf, HRFREQ, HRLEN);
    return true;
}
static int hr_algo_disable(void)
{
    free(whr.wear_workbuf);
    whr.wear_workbuf = NULL;
    free(whr.workbuf);
    whr.workbuf = NULL;
    return true;
}
static int continous_hr_warn_cnt = 0;
static u8 test_switch = 0;
static void refresh_heart_rate_data(void *p)
{
    printf("%s %s", __func__, os_current_task());
    int cnt = 0;
    int hrsensorbuf[HRLEN] = {0};
    int hrlen = HRLEN;
    u8 HR_DATA = 0;
    int hrdlytime = ((HRLEN * 100) / (HRPOINT * HRFREQ));
    while (1) {
        os_sem_pend(&HR_SEM, 0);
        printf("%s %s", __func__, os_current_task());
        os_mutex_pend(&SENSOR_MUTEX, 0);//防止心率血氧冲突
        hr_sensor_io_ctl(HR_SENSOR_DISABLE, NULL);
        hr_sensor_io_ctl(HR_SENSOR_ENABLE, &hrlen);
        /* 延时等待第一组数据 */
        os_time_dly(hrdlytime);
        for (cnt = 0; cnt <= ((u8)(HRSENSOR_READ_SINGLE_PASS_TIME * HRFREQ * HRPOINT / hrlen));) {
            if (hr_sensor_io_ctl(HR_SENSOR_READ_DATA, hrsensorbuf) == 0) {
                os_time_dly(hrdlytime);
                search_sensor_cnt++;
                if (search_sensor_cnt == 5) {
                    log_e("sensor_dont_online\n");
                    search_sensor_cnt = 0;
                    break;
                }
                continue;
            }
            search_sensor_cnt = 0;
            wear_detection(whr.wear_workbuf, hrlen, hrsensorbuf, whr.out);
            HeartRate_run(whr.workbuf, whr.wear_workbuf, HRLEN,  whr.out); //运行
            HR_DATA = getCurrentHR(whr.workbuf);//实时调取心率
#if C_HEART_TEST//测试用
            if (test_switch) {
                HR_DATA = 140;
            }
#endif
            /* 辅助性处理 */
            if (cnt % ((u8)(HRFREQ * HRPOINT / hrlen)) == 0) {
                printf("REFRESH_HEART_DATA=%d,wear=%d HR=%d CNT=%d ", whr.HR, whr.out[0], HR_DATA, cnt);
            }
            if ((HR_DATA != 0) & (cnt > ((u8)(HRSENSOR_READ_FILT_TIME * HRFREQ * HRPOINT / hrlen)))) {
                if (HR_DATA != 0) {
                    whr.HR = HR_DATA;
                }
                if (whr.hrmin == 0) {
                    whr.hrmin = whr.HR;
                }
                if ((whr.HR < whr.hrmin) & (whr.HR != 0)) {
                    whr.hrmin = whr.HR;
                }
                if (whr.HR > whr.hrmax) {
                    whr.hrmax = whr.HR;
                }
            }

            if (whr.continuous_hr_status == 1) {
                if (wspo2.read_status == 1) { //检测到血氧线程挂起，则优先释放hrsensor资源给血氧
                    break;
                }
                whr.HR = HR_DATA;
                if (whr.HR >= whr.continuous_hr_max) {
                    continous_hr_warn_cnt++;
                    printf("hr_warn_time=%ds target_time=%ds real_hr=%d dete_hr_max%d\n", continous_hr_warn_cnt, CONTINUOUS_HR_WARN_TIME, whr.HR, whr.continuous_hr_max);
                    if (continous_hr_warn_cnt > CONTINUOUS_HR_WARN_TIME) {
                        continous_hr_warn_cnt = 0;
                        detection_hd.heart_rate();
                    }
                }
#if (CONTINUE_HEART_RATE_MODE==CONTINUE_HEART_RATE_THRESHOLD_MAX_MIN)
                else if (whr.HR < whr.continuous_hr_min) {
                    printf("hr_warn_time=%ds target_time=%ds real_hr=%d dete_hr_min%d\n", continous_hr_warn_cnt, CONTINUOUS_HR_WARN_TIME, whr.HR, whr.continuous_hr_min);
                    if (continous_hr_warn_cnt > CONTINUOUS_HR_WARN_TIME) {
                        continous_hr_warn_cnt = 0;
                        /* detection_hd.heart_rate(); */
                    }
                }
#endif
                else {
                    continous_hr_warn_cnt = 0;
                }
                continue;
            } else if (whr.exercise_hr_detection_status == 1) {
                whr.HR = HR_DATA;
                if (wspo2.read_status == 1) { //检测到血氧线程挂起，则优先释放hrsensor资源给血氧
                    break;
                }
                if (whr.HR >= whr.exercise_hr_max) {
                    printf("real_hr=%d dete_hr_max\n", whr.HR, whr.exercise_hr_max);
                    detection_hd.heart_rate();
                }
                continue;
            }
            if (whr.read_status == 0) {
                break;
            }
            cnt++;
        }
        //关闭传感器
        hr_sensor_io_ctl(HR_SENSOR_DISABLE, NULL);
        whr.read_status = 0;
        os_mutex_post(&SENSOR_MUTEX);//防止心率血氧冲突
    }
}

static u8 save_exercise_heart_rate(u8 enable, u8 hr_threshold)
{
    /* printf("%s %d",__func__,hr_threshold); */

    whr.exercise_hr_detection_enable = enable;
    whr.exercise_hr_max = hr_threshold;
#if (CONTINUE_HEART_RATE_MODE==FOLLOW_EXERCISE_HEART_RATE)
    whr.continuous_hr_max = hr_threshold; //使用同一阈值
#endif
    return enable;
}
static u8 save_continue_heart_rate(u8 enable, u8 hr_threshold_max, u8 hr_threshold_min)
{
    printf("%s %d %d", __func__, hr_threshold_max, hr_threshold_min);
    whr.continuous_hr_status = enable;
#if (CONTINUE_HEART_RATE_MODE==CONTINUE_HEART_RATE_THRESHOLD_MAX)
    whr.continuous_hr_max = hr_threshold_max;//使用同一阈值
#elif (CONTINUE_HEART_RATE_MODE==CONTINUE_HEART_RATE_THRESHOLD_MAX_MIN)
    whr.exercise_hr_min = min;
    whr.continuous_hr_max = hr_threshold_max;//使用同一阈值
#endif
    return enable;
}
int get_heart_rate_threshold(void)
{
    /* printf("%s c%d e%d",__func__,whr.continuous_hr_max,whr.exercise_hr_max); */
    if (whr.continuous_hr_status) {
        return whr.continuous_hr_max;
    } else if (whr.exercise_hr_detection_enable) {
        return whr.exercise_hr_max;
    }
    return 0;
}
int save_exercise_heart_rate_mode(u8 enable, u8 motion_HR_type)
{
    if (watch.workbuf == NULL) {
        return 	SENSOR_WORKBUF_ERROR;
    }
    HeartRate_par HP = {0, 0};
    watch.motion_HR_type = motion_HR_type;
    init_HR(watch.workbuf, HP, watch.motion_HR_type); //初始化心率模式
    init_MotionHR(watch.workbuf, watch.motion_HR_type); //初始化运动心率
    resetMotion(watch.workbuf);//重置运动模式
    return SUCCESS;
}
int read_heart_rate_task(void)
{
    printf("%s %d", __func__, whr.read_status);
    if (whr.read_status == 0) {
        whr.read_status = 1;
        whr.HR = 0;
        os_sem_post(&HR_SEM);
        return SUCCESS;
    }
    return TASK_OPEN_ERROR;
}
void heart_rate_task_exit(void)
{
    whr.read_status = 0;
}
#if C_HEART_TEST
void c_heart_rate(u8 enable)
{
    test_switch = enable;
}
#endif
static u8 get_heart_rate(void)
{
    printf("%s", __func__);
#if TCFG_HR_SENSOR_ENABLE
    printf("HR=%d", whr.HR);
    return whr.HR;//如果设备不在线，返回值为0
#else
    whr.HR = 120;
    return whr.HR;
#endif
}
static u8 get_heart_rate_wear(void)
{
    return whr.out[0];
}

static u8 get_resting_heart_rate(void)
{
#if TCFG_HR_SENSOR_ENABLE
    return whr.hrresting;
#else
    return 70;
#endif
}
static u8 get_min_heart_rate(void)
{
#if TCFG_HR_SENSOR_ENABLE
    return whr.hrmin;
#else
    return 80;
#endif
}
u8 get_max_heart_rate(void)
{
#if TCFG_HR_SENSOR_ENABLE
    return whr.hrmax;
#else
    return 150;
#endif
}

void clr_hr_info(void) //重置心率值
{
    if (whr.work_status == 1) {
        whr.HR = 0;
        whr.hrmin = 0;
        whr.hrmax = 0;
        whr.hrresting = 0;
    }
}

static unsigned char heart_rate_data_get(unsigned char type, unsigned char *valid)
{
    if (valid != NULL) {
        *valid = get_heart_rate_wear();
    }
    switch (type) {
    case REAL_DATA:
        read_heart_rate_task();
        return get_heart_rate();
    case LAST_DATA:
        return get_heart_rate();
        break;
    case MAX_DATA:
        return get_max_heart_rate();
    case MIN_DATA:
        return get_min_heart_rate();
    case RESTING_DATA:
        return get_resting_heart_rate();
    }
    return 0;
}
//**************************************************
//                 更新手表数据
//**************************************************
struct sys_time *ptime;
int adjusting_step;
static int adjusting_data(void)
{
    if (ptime == NULL) {
        ptime = zalloc(sizeof(struct sys_time));
        watch_file_get_sys_time(ptime);
        adjusting_step = __motion_info.step;
    }
    struct sys_time ntime;
    watch_file_get_sys_time(&ntime);
    if (((ptime->hour * 60 + ptime->min) - (ntime.hour * 60 + ntime.min)) > 20) {
        if ((__motion_info.step - adjusting_step) < 15) {
            data_temp_storage();
            resetSteps(watch.workbuf);
            resetMotion(watch.workbuf);
            free(ptime);
            ptime = NULL;
        }
    }
    return 0;
}
static int data_temp_storage(void)
{
    struct sys_time time;
    watch_file_get_sys_time(&time);
    wtemp.timestamp = timestamp_mytime_2_utc_sec(&time);
    wtemp.step = __motion_info.step;//步数
    wtemp.distance = __motion_info.step_distance;//距离
    wtemp.kcal = __motion_info.caloric;//热量
    return 0;
}
static int data_temp_storage_set_vm()
{
    data_temp_storage();
    int ret = syscfg_write(VM_SPORT_INFO_DAILY_DATA, &wtemp, sizeof(struct sport_temp_data));
    return ret;
}
static int data_temp_storage_get_vm()
{
    struct sport_temp_data tmp;
    int ret = syscfg_read(VM_SPORT_INFO_DAILY_DATA, &tmp, sizeof(struct sport_temp_data));
    if (ret != sizeof(struct sport_temp_data)) {
        return false;
    }
    struct sys_time cur_time, save_time;
    watch_file_get_sys_time(&cur_time);
    timestamp_utc_sec_2_mytime(tmp.timestamp, &save_time);
    if ((save_time.year == cur_time.year) && (save_time.month == cur_time.month) && (save_time.day == cur_time.day)) {
        wtemp.step = __motion_info.step;//步数
        wtemp.distance = __motion_info.step_distance;//距离
        wtemp.kcal = __motion_info.caloric;//热量
    }
    return true;
}
static int algo_motion_info_refresh(void)
{
    if (watch.init_status == 0) {
        return 0;
    }
    //运动信息
    //运动模块需要1分钟才能识别状态改变
    //	motionstatus	0	 1	  2	   3
    //	运动状态		静止 慢走 走路 跑步
    __motion_info.step 				= getCurrentStep(watch.workbuf) + wtemp.step;			//步数
    __motion_info.step_distance 	= (getStepDistance(watch.workbuf) / 10) + wtemp.distance;	//距离	单位：10米
    __motion_info.caloric			= getStepKCal(watch.workbuf) + wtemp.kcal;		//热量	单位：大卡，心率达到运动要求才会计算卡路里
//    __motion_info.motion_status		= getMotionStatus(watch.workbuf);	//状态	见上表
    __motion_info.motion_time  		= getMotion_time(watch.workbuf);	//时间	单位：秒
    __motion_info.motion_speed 		= getStep_Per_min(watch.workbuf);	//速度	单位：10米/小时
    __motion_info.motion_pace  		= getSpeed_distribution(watch.workbuf); //配速 单位：秒/公里
    __motion_info.motion_step_freq	= getStep_frequency(watch.workbuf);	//步频	单位：步/分钟
    __motion_info.motion_step_stride = getStride(watch.workbuf);			//步幅	单位：厘米
    __motion_info.intensity 		= getIntensity(watch.workbuf);//运动强度0	1 2 3 4 5
    /* mem_stats(); */
    printf("step=%d,distance=%d,kcal=%d,motiontime=%d,speed=%d,pace=%d", \
           __motion_info.step, __motion_info.step_distance, __motion_info.caloric, __motion_info.motion_time, \
           __motion_info.motion_speed, __motion_info.motion_pace);
    return __motion_info.step ;
}
#if (!TCFG_GSENSOR_ENABLE)
static void algo_motion_info_refresh_test(void *param)
{
    __motion_info.step 				++;			//步数
    __motion_info.step_distance 	= (int)(__motion_info.step / 10);	//距离	单位：10米
    __motion_info.caloric			= 30 * (int)(__motion_info.step_distance / 100);		//热量	单位：大卡，心率达到运动要求才会计算卡路里
//    __motion_info.motion_status		= getMotionStatus(watch.workbuf);	//状态	见上表
    __motion_info.motion_time  		++ ;	//时间	单位：秒
    __motion_info.motion_speed 		= 500;	//速度	单位：10米/小时
    __motion_info.motion_pace  		= 1000; //配速 单位：秒/公里
    __motion_info.motion_step_freq	= 60;	//步频	单位：步/分钟
    __motion_info.motion_step_stride = 100;			//步幅	单位：厘米
    __motion_info.intensity 		= 1;//运动强度0	1 2 3 4 5
    /* printf("step=%d,distance=%d,kcal=%d,motiontime=%d,speed=%d,pace=%d", \ */
    /*        __motion_info.step, __motion_info.step_distance, __motion_info.caloric, __motion_info.motion_time, \ */
    /*        __motion_info.motion_speed, __motion_info.motion_pace); */
}
static int algo_motion_info_refresh_cb(int param)
{
    algo_motion_info_refresh_test(NULL);
    return 0;
}
#endif
static int algo_hr_resting_get(void)
{
    if (watch.init_status == 0) {
        return 0;
    }
    if (watch.flag[6] == 1) {
        whr.hrresting = getRestingHeartRate(watch.workbuf); //静息心率
        return whr.hrresting;
    }
    return 0;
}
static int algo_motion_clr(void)
{
    if (watch.init_status == 0) {
        return 0;
    }
    resetSteps(watch.workbuf);
    resetMotion(watch.workbuf);
    clr_sport_info();
    clr_hr_info();
    clr_heart_rate();
    clr_spo2_info();
    clr_blood_oxygen();
    // clr_air_presure_altitude();
    return 0;
}
static int algo_detection_response(void)
{
    if (watch.init_status == 0) {
        return 0;
    }

    /* printf("wrist=%d,wear=%d,sleep=%d,fall=%d,sedentary=%d", watch.flag[0], watch.flag[1], \ */
    /* watch.flag[2], watch.flag[3], watch.flag[4]); */
    //记录标志
    //flag[0]中0没有抬腕,1抬腕；
    //flag[1]中0佩戴，1没有佩戴；
    //flag[2]中0清醒，1浅睡，2深睡,3 REM（该标志1分钟更新一次）4零星小睡。睡眠小于2小时只有0和4
    //flag[3]中0未跌倒，1跌倒//需要手动清除跌倒记录
    //flag[4]中0未久坐，1久坐//建议久坐提醒调用的检测时间段为：上午8：00~12：00 下午2：00~8：00 每1小时久坐提醒一次
    //flag[5]中0清醒或小睡，1睡眠时间大于2小时
    //flag[6]中0未读到静息心率，1读到静息心率
    if (watch.flag[0] == 1) {
        printf("W R I S T");
        if (detection_hd.wrist != NULL) {
            detection_hd.wrist();
        }
    }
    if (watch.flag[2] != 0) {
        if (algo_sleep.sleep_status == 0) {
            sleep_record_start();
            algo_sleep.sleep_status = 1;
        }
        refresh_sleep_record(watch.flag[2]);
    } else {
        if (algo_sleep.sleep_status != 0) {
            if (getSleepTimeAll(watch.workbuf) >= 10) { //低于10分钟睡眠,睡眠无效数据丢弃

                sleep_record_stop();//结束睡眠，正常保存文件
            } else {
                sleep_flag_free();//清除最近一次睡眠数据
            }
        }
    }
    if (watch.flag[3] == 1) {
        watch.fall = 1;//记录跌倒状态
        printf("F A L L");
        if (detection_hd.fall != NULL) {
            detection_hd.fall();
        }
    }
    if (watch.flag[4] == 1) {
        printf("S E D E N T A R Y");
        if (detection_hd.sedentary != NULL) {
            detection_hd.sedentary();
        }
    }
    return 0;
}
void algo_deal_sport_start(void)
{
    if (watch.init_status == 0) {
        return;
    }
    getMotionSwitch(watch.workbuf, 1);
    printf("%s%d", __func__, whr.exercise_hr_detection_enable);
    if (whr.exercise_hr_detection_enable) {
        whr.exercise_hr_detection_status = 1;
        watch_algo_hd.algo_heart_rate_task();
    }
}
void algo_deal_sport_end(void)
{
    if (watch.init_status == 0) {
        return;
    }
    if (whr.exercise_hr_detection_enable) {
        whr.exercise_hr_detection_status = 0;
        watch_algo_hd.algo_heart_rate_task_exit();
    }

#if TCFG_GSENSOR_ENABLE
    /* printf("%s %d", __func__, __LINE__); */
    __motion_info.intensity_time[0] = getIntensity1_time(watch.workbuf);
    __motion_info.intensity_time[1] = getIntensity2_time(watch.workbuf);
    __motion_info.intensity_time[2] = getIntensity3_time(watch.workbuf);
    __motion_info.intensity_time[3] = getIntensity4_time(watch.workbuf);
    __motion_info.intensity_time[4] = getIntensity5_time(watch.workbuf);
    __motion_info.motion_time = 0;
    int recovery = restore_time(watch.workbuf, 720, 0);
    __motion_info.recoverytime_hour = recovery / 60;
    __motion_info.recoverytime_min = recovery % 60;
#else
    /* printf("%s %d", __func__, __LINE__); */
    memset(__motion_info.intensity_time, 0, 20);
    __motion_info.intensity_time[0] = __motion_info.motion_time;
    __motion_info.recoverytime_hour = 24;
    __motion_info.recoverytime_min = 10;
#endif
    printf("recovery_time=%d", __motion_info.recoverytime_hour * 60 + __motion_info.recoverytime_min);
    getMotionSwitch(watch.workbuf, 0);
    resetMotion(watch.workbuf);
}
int daily_motion_info_get(struct motion_info *info)
{
#if TCFG_GSENSOR_ENABLE
    /* printf("%s %d", __func__, __LINE__); */
    if (watch.init_status == 1) {
        memcpy(info, &__motion_info, sizeof(struct motion_info));
        return SUCCESS;
    } else {
        memset(info, 0, sizeof(struct motion_info));
        return SENSOR_WORKBUF_ERROR;
    }
#else
    /* printf("%s %d", __func__, __LINE__); */
    memcpy(info, &__motion_info, sizeof(struct motion_info));
    return SUCCESS;
#endif
}
static u8 search_gsensor_cnt = 0;
static u8 algo_cnt = 0;
short gsensorbuf[WATCHLEN];

#if 0
static void refresh_watch_data(void *param)
{
    printf("%s %s", __func__, os_current_task());
    if (watch.init_status != 1) {
        log_e("watch_not_init");
        return ;
    }
    int buflen = read_gsensor_nbuf(gsensorbuf, sizeof(short) * WATCHLEN);				//读取传感器函数
    if (buflen == 0) {
        search_gsensor_cnt++;
        if ((search_gsensor_cnt >= 5) & (!whr.read_status) & (!wspo2.read_status)) {
            gsensor_disable();
            gsensor_enable();
            search_gsensor_cnt = 0;
        }
        log_e("buflen=0");
        return ;
    }
    search_gsensor_cnt = 0;
    //算法	如果没有心率传感器，则把whr.HR改成一个定值,0只计步，(60,90)可以做简单的运动判断
    if (whr.work_status == 0) {
        whr.HR = 90; //
        run_watch(watch.workbuf, gsensorbuf, WATCHLEN, watch.flag, WP, 1, whr.HR);
    } else {
        run_watch(watch.workbuf, gsensorbuf, WATCHLEN, watch.flag, WP, 1, whr.HR);
    }
    algo_detection_response();
    algo_cnt++;
    if (algo_cnt >= (int)(WATCHFREQ * WATCHPOINT / WATCHLEN)) {
        algo_motion_info_refresh();
        refresh_detection_time();
        /* printf("switch wrist=%d,sleep=%d ,fall=%d,sendentary=%d", WP.wrist_enable, WP.sleep_enable, WP.fall_enable, WP.Sedentary_enable); */
        algo_cnt = 0;
    }
    //时间处理
    struct sys_time ntime;
    watch_file_get_sys_time(&ntime);
    if ((ntime.hour == 0) & (ntime.min == 0) & (ntime.sec <= 1)) {
        algo_motion_clr();
        //step_distance_kcal_file_write_stop();
    }
    if ((ntime.min == 59) & (ntime.sec >= 59)) { //每个小时结束写入文件
        //step_distance_kcal_file_write();
    }
}
#endif

#if(GSENSOR_TASK_USED == USED_TASK)

extern u8 p11_algo_event;

typedef enum {
    ALGO_NOTHING,           //没有相关动作
    ALGO_WRIST_UP,          //抬腕动作
    ALGO_WRIST_DOWN,        //落腕动作
    ALGO_DOUBLE_CLICK,      //双击屏幕
    ALGO_HITTING,           //击球动作
    ALGO_ON_DESK,           //放置桌面
} p11_algo_out;

void p11_algo_event_handle(void)
{
    switch (p11_algo_event) {
    case ALGO_WRIST_UP:
        ui_screen_recover(1);//亮屏
        break;

    case ALGO_WRIST_DOWN:
        ui_screen_saver(NULL);

        break;

    default:
        break;
    }
}


static void  gSensor_data_process(void)
{
    int dlen = master_p11cbuf_get_data_len(0);

    if (dlen > 0) {
        master_p11cbuf_read(0, gsensorbuf, dlen);
        motion_module_run(gsensorbuf, dlen / 6);
    }
}


typedef struct {
    OS_SEM g_sem;
    OS_SEM hr_sem;
    OS_SEM spo2_sem;
} SENSOR_KICK;


static SENSOR_KICK sensor_kick;
static int refresh_watch_data_check_cb(int *parm)
{
    /* printf(">>>>>>"); */
    extern void sensor_kick_start();
    sensor_kick_start();
    /* int argv[3]; */
    /* argv[0] = (int)sensor_data_process; */
    /* argv[1] = 1; */
    /* argv[2] = (int)parm; */
    /* os_taskq_post_type("app_core", Q_CALLBACK, 3, argv); */
    return 0;
}


static int reflash_motion_info_refresh(void)
{
    //运动信息
    __motion_info.step 				= wtemp.step + motion_module_get_steps();
    __motion_info.step_distance 	= wtemp.distance + motion_module_get_distance();
    __motion_info.caloric			= wtemp.kcal + motion_module_get_calories();
    __motion_info.motion_time  		= 0;
    __motion_info.motion_speed 		= 0;
    __motion_info.motion_pace  		= 0;
    __motion_info.motion_step_freq	= motion_module_get_step_frequency();
    __motion_info.motion_step_stride = 0;
    __motion_info.intensity 		= 0;
    printf("step=%d,distance=%d,kcal=%d,motiontime=%d,speed=%d,pace=%d", \
           __motion_info.step, __motion_info.step_distance, __motion_info.caloric, __motion_info.motion_time, \
           __motion_info.motion_speed, __motion_info.motion_pace);
    return __motion_info.step ;
}
static int motion_info_clear(void)
{
    struct sys_time time;
    watch_file_get_sys_time(&time);
    if ((time.hour == 0) && (time.min == 0) && (time.sec == 0)) {
        motion_module_clear_steps();
        motion_module_clear_distance();
        motion_module_clear_calories();
        clr_sport_info();
    }
    return 0;
}


static void refresh_watch_Task(void *p)
{
    motion_module_init();
    sys_s_hi_timer_add(NULL, refresh_watch_data_check_cb, 1000); //1s 跟随系统节奏

    while (1) {
        os_sem_pend(&sensor_kick.g_sem, 0);
        watch.task_run = 1;
        p11_algo_event_handle();
        // refresh_watch_data(NULL);
        motion_info_clear();
        gSensor_data_process();
        reflash_motion_info_refresh();
//        hr_sensor_data_process();
        watch.task_run = 0;
    }
}

#endif

void sensor_kick_start()
{
#if(GSENSOR_TASK_USED == USED_TASK)
    if (!watch.init_status) {
        //printf("%s %d\n",__FUNCTION__,__LINE__);
        return;
    }
    //printf("%s %d\n",__FUNCTION__,__LINE__);
    os_sem_set(&sensor_kick.g_sem, 0);
    os_sem_post(&sensor_kick.g_sem);
#endif
}
//**************************************************
//             算法接口初始化
//**************************************************
static int watch_api_init(void)
{
    printf("%s", __func__);
    char search_gsensor;
    //gsensor_io_ctl(SEARCH_SENSOR, &search_gsensor);
    if (search_gsensor == 0) {
        log_e("watch_gsensor_not_find");
        return 0;
    }

    watch.init_status = 0;


#if(GSENSOR_TASK_USED == USED_TASK)
    if (strcmp(os_current_task(), "gsensor")) {
        while (watch.task_run) {
            os_time_dly(1);
        }
    }
#endif

    PED_parameter PP = { 250, 1000, 600, 300, 900};//PP计步参数，参数分别为：幅度阈值1，幅度阈值2，幅度阈值3，最短步时长，跑步阈值
    BRI_parameter BP =  { -600, 150, -200, 500, -500, -630, 1400 };//BP抬腕参数，参数分别为：阈值X，阈值Z，阈值UP，y1阈值，y2阈值，x阈值，增量阈值
    SLEEP_parameter SP = { 10, 1, 120, 100000 };//SP睡眠参数，参数分别为：时间阈值，调节因子，腕动阈值，活动阈值
    FALL_detection FD = {3000, 300, 1000};//FD摔倒检测参数，分别为：合速度，阈值，补偿阈值
    Sedentary_r SR = {400000, 100000}; //SR活动量阈值
    HeartRate_par HP = {0, 0};//最大心率和静息心率参数，调用 init_HR()修改
    //初始化工作空间
    int watchfreq = WATCHFREQ;
    int warchlen = WATCHLEN;
    if (watch.workbuf == NULL) {
        int bufsize = get_watch_buf(watchfreq, warchlen);
        watch.workbuf = (char *)malloc(bufsize);
        printf("workbuf=%lu", sizeof(watch.workbuf));
        if (watch.workbuf == NULL) {
            log_e("watch_workbuf_malloc_error");
            return 0;
        }
    }
    printf("PI.HEIGHT=%d %d %d %d", PI.height, PI.weight, PI.age, PI.gender);
    printf("switch wrist=%d,sleep=%d ,fall=%d,sendentary=%d", WP.wrist_enable, WP.sleep_enable, WP.fall_enable, WP.Sedentary_enable);
    init_watch(watch.workbuf, WP, PP, BP, SP, FD, PI, SR); //初始化计步算法
    init_motion(watch.workbuf);//运动模块初始化
    init_HR(watch.workbuf, HP, watch.motion_HR_type);//静息心率参数初始化，HP为上面提到的最大和静息参数配置，0为最大心率百分比，1为储备心率百分比
    init_MotionHR(watch.workbuf, watch.motion_HR_type);//运动心率设置,与心率参数初始化一致。0为最大心率百分比，1为储备心率百分比
#if(GSENSOR_TASK_USED == USED_TIMER)
    watch.sensor_timer_id = sys_s_hi_timer_add(NULL, refresh_watch_data_check_cb, (int)SENSOR_INTERVAL / 2); //算法传入的数值
#else

    if (!watch.task_init) {
        os_sem_create(&sensor_kick.g_sem, 0);
        os_sem_create(&sensor_kick.hr_sem, 0);
        os_sem_create(&sensor_kick.spo2_sem, 0);
        task_create(refresh_watch_Task, NULL, "gsensor");
        watch.task_init = 1;
    }

#endif
    clock_add_set(WATCH_SPORT_CLK);
    watch.init_status = 1;
    return 1;
}

/* 计步开关 */
static u8 step_start(void)
{
    printf("%s", __func__);
    if (watch.init_status == 1) {
        printf("Repeated call %s", __func__);
        return 0;
    }
#if TCFG_GSENSOR_ENABLE
    printf("%s %d", __func__, __LINE__);
    if (gsensor_enable() == 0) {
        printf("%s %d", __func__, __LINE__);

        watch.motion_HR_type = 0; //默认参数
        watch.init_status = 0;//默认参数
        if (watch_api_init() == 1) {
            printf("watch_sport_init_succ");
        } else {
            printf("watch_sport_init_error");
            return 0;
        }
        printf("%s succ", __func__);
        return 1;
    }
#endif
    return 0;
}


static u8 step_stop(void)
{
    printf("%s", __func__);
    if (watch.init_status == 0) {
        printf("Repeated call %s", __func__);
        return -1;
    }
#if TCFG_GSENSOR_ENABLE
    if (gsensor_disable() == 0) {
        if (watch.sensor_timer_id != 0) {
            sys_s_hi_timer_del(watch.sensor_timer_id);
            watch.sensor_timer_id = 0;
        }

#if(GSENSOR_TASK_USED != USED_TASK)
        if (watch.init_status != 0) {
            free(watch.workbuf);
            watch.workbuf = NULL;
        }
#endif
        data_temp_storage();//保存连续性数据
        //step_distance_kcal_file_write_break();//存入文件
        watch.init_status = 0;

        clock_remove_set(WATCH_SPORT_CLK);
        printf("%s succ", __func__);
        return 1;
    }
#endif
    return 0;
}


static u8 set_watch_sport_initial_value_before(void)   //初始化前的参数、状态的配置
{
    printf("%s", __func__);
    detection_init();
    //个人信息
    extern personal_information info;
    memset(&info, 0, sizeof(personal_information));
    sport_personal_info_get(&info);
    u8 age = watch_time_age(info.birth_y, info.birth_m, info.birth_d);
    /* printf("%d %d %d %d", info.height, info.weight, info.gender, age); */
    if (!info.height) {
        PI.height = HEIGHT;
    } else {
        PI.height = info.height;
    }
    if (!info.weight) {
        PI.weight = WEIGHT;
    } else {
        PI.weight = info.weight;
    }
    if (age == 227) {
        PI.age = AGE;
    } else {
        PI.age = age;
    }
    PI.gender = info.gender;
    /* printf("%s %d %d %d %d", __func__, PI.height, PI.weight, PI.gender, PI.age); */
    return 0;
}
static u8 set_watch_sport_initial_value_back(void)   //初始化后的一些功能开关,
{
    printf("%s", __func__);

    int watch_switch = sport_info_swtich_record_get(SPORT_INFO_SWTICH_TYPE_MAX) | BIT(SPORT_INFO_SWTICH_TYPE_SENSOR_PEDOMETER);
    //心率模式
    //set_continuous_heart_rate_enable((watch_switch & BIT(SPORT_INFO_SWTICH_TYPE_CONTINUOUS_HEART_RATE)));
    e_heart_rate heart_rate;
    sport_exercise_heart_rate_get(&heart_rate);
    //set_exercise_heart_rate_detection((watch_switch & BIT(SPORT_INFO_SWTICH_TYPE_CONTINUOUS_HEART_RATE)), heart_rate.max_heart_rate);
    //传感器功能开关
    for (u8 i = 1; i <= (1 + (SPORT_INFO_SWTICH_TYPE_SENSOR_ALTITUDE_RECORD - SPORT_INFO_SWTICH_TYPE_SENSOR_PEDOMETER)); i++) {
        switch ((i)) {
        case SPORT_INFO_SWTICH_TYPE_SENSOR_PEDOMETER:
#if TCFG_GSENSOR_ENABLE
            if ((watch_switch & BIT(i)) == 0) {
                step_stop();
            } else {
                step_start();
            }
#endif
            break;
        case SPORT_INFO_SWTICH_TYPE_SENSOR_PEDOMETER_RECORD	:
            if ((watch_switch & BIT(i)) == 0) { //预留功能
            } else {
            }
            break;
        case SPORT_INFO_SWTICH_TYPE_SENSOR_HEART_RATE:
#if TCFG_HR_SENSOR_ENABLE
            if ((watch_switch & BIT(i)) == 0) {
                heart_rate_stop();
            } else {
                heart_rate_start();
            }
#endif
            break;
        case SPORT_INFO_SWTICH_TYPE_SENSOR_HEART_RATE_RECORD:
            if ((watch_switch & BIT(i)) == 0) {
            } else {
            }
            break;
        case SPORT_INFO_SWTICH_TYPE_SENSOR_BLOOD_OXYGEN:
#if TCFG_SPO2_SENSOR_ENABLE
            if ((watch_switch & BIT(i)) == 0) {
                blood_oxygen_stop();
            } else {
                blood_oxygen_start();
            }
#endif
            break;
        case SPORT_INFO_SWTICH_TYPE_SENSOR_BLOOD_OXYGEN_RECORD:
            if ((watch_switch & BIT(i)) == 0) {
            } else {
            }
            break;
        case SPORT_INFO_SWTICH_TYPE_SENSOR_ALTITUDE:
#if TCFG_BMP280_ENABLE
            if ((watch_switch & BIT(i)) == 0) {
                altitude_airpresure_stop();
            } else {
                altitude_airpresure_start();
            }
#endif
            break;
        case SPORT_INFO_SWTICH_TYPE_SENSOR_ALTITUDE_RECORD:
            if ((watch_switch & BIT(i)) == 0) {
            } else {
            }
            break;
        }
    }
    return 0;
}

/************************************************************************
					    	总开关
************************************************************************/
int watch_sensor_open(void)
{
    printf("%s", __func__);

    /* mem_stats(); */
    if ((watch.init_status == 1) | (whr.work_status == 1) | (wspo2.work_status == 1)) {
        return 0;
    }
    //初始化结构体
    memset(&wspo2, 0, sizeof(struct watch_SPO2_data));
    memset(&whr, 0, sizeof(struct sensor_heart_rate));
    memset(&__motion_info, 0, sizeof(struct motion_info));
    memset(&watch.flag, 0, 4 * 5);
    set_watch_sport_initial_value_before();//与算法初始化配置相关的内容
    os_mutex_create(&SENSOR_MUTEX);
    //算法初始化和功能开启
#if TCFG_GSENSOR_ENABLE
    step_start();
    data_temp_storage_get_vm();
    daily_active_record_start();
#endif
#if TCFG_HR_SENSOR_ENABLE
    char search_hrsensor;
    hr_sensor_io_ctl(HR_SEARCH_SENSOR, &search_hrsensor);
    if (search_hrsensor == 0) {
        log_e("watch_hrsensor_not_find\n");
    } else {
        whr.work_status = 1;
        if (hr_algo_enable() == false) {
            log_e("hr_algo_enable_error\n");
            whr.read_status = 0;
            whr.work_status = 0;
        } else {
            os_sem_create(&HR_SEM, 1);
            task_create(refresh_heart_rate_data, NULL, "heartrate");
            printf("hrsensor_online_work\n");
        }
    }
#endif
#if TCFG_SPO2_SENSOR_ENABLE
    u8 search_spo2sensor;
    hr_sensor_io_ctl(HR_SEARCH_SENSOR, &search_spo2sensor);
    if (search_spo2sensor == 0) {
        log_e("spo2sensor_not_find\n");
    } else {
        wspo2.work_status = 1;
        if (spo2_algo_enable() == false) {//算法空间初始化
            log_e("spo2_algo_enable_error \n");
            wspo2.read_status = 0;
            wspo2.work_status = 0;
        } else {
            os_sem_create(&SPO2_SEM, 0);
            task_create(refresh_spo2_data, NULL, "spo2read");
            printf("spo2sensor_online_work\n");
        }
    }
#endif
#if TCFG_BMP280_ENABLE
    if (altitude_airpresure_start()) {
        printf("watch_bmp280_init_succ");
    }
#endif
    set_watch_sport_initial_value_back();//与算法初始化配置无关的内容
    /* mem_stats(); */
    return 1;
}
#if (!TCFG_GSENSOR_ENABLE)
static int test_timer_id;
void watch_sensor_test(void)
{
    printf("%s", __func__);
    test_timer_id = sys_timer_add(NULL, algo_motion_info_refresh_test, 1000);
}
#endif
int watch_sensor_close(void)
{
#if (!TCFG_GSENSOR_ENABLE)
    sys_timer_del(test_timer_id);
#endif
    step_stop();
    data_temp_storage_set_vm();
    daily_active_record_stop();
    if (whr.work_status != 0) {
        heart_rate_stop();
        task_kill("heartrate");
        whr.read_status = 0; //结束线程
        whr.work_status = 0;
    }
    if (wspo2.work_status != 0) {
        blood_oxygen_stop();
        task_kill("spo2read");//结束线程
        spo2_algo_disable();
        wspo2.read_status = 0;
        wspo2.work_status = 0;
    }
#if TCFG_BMP280_ENABLE
    altitude_airpresure_stop();
#endif
    printf("watch_sensor_stop!\n");
    /* mem_stats(); */
    return 1;
}

/**************************************************
 			 		设置接口
***************************************************/
int personal_information_set(struct personal_info *personal_info)
{
    if (watch.sensor_timer_id != 0) {
        sys_s_hi_timer_del(watch.sensor_timer_id);
        watch.sensor_timer_id = 0;
    }

#if(GSENSOR_TASK_USED != USED_TASK)
    if (watch.init_status != 0) {
        free(watch.workbuf);
        watch.workbuf = NULL;
    }
#endif
    watch.init_status = 0;
    data_temp_storage();//保存连续性数据
    PI.height	= personal_info->height;
    PI.weight	= personal_info->weight;
    PI.age		= personal_info->age;
    PI.gender	= personal_info->gender;
    printf("%s %d %d %d %d", __func__, PI.height, PI.weight, PI.age, PI.gender);
    //重置算法
    watch_api_init();
    return SUCCESS;
}

int sensor_switch_set(unsigned char type, unsigned char enable)
{
    printf("%s %d %d", __func__, type, enable);
    switch (type) {
    case SPORT_INFO_SWTICH_TYPE_SENSOR_PEDOMETER:
        if (enable) {
            step_start();
        } else {
            step_stop();
        }
        break;
    case SPORT_INFO_SWTICH_TYPE_SENSOR_HEART_RATE:
        if (enable) {
            heart_rate_start();
        } else {
            heart_rate_stop();
        }
        break;
    case SPORT_INFO_SWTICH_TYPE_SENSOR_BLOOD_OXYGEN:
        if (enable) {
            blood_oxygen_start();
        } else {
            blood_oxygen_stop();
        }
        break;
    default:
        return	TYPE_ERROR;
        break;
    }
    return SUCCESS;
}
int detection_init(void)
{
    u8 *mode_data = NULL;
    u16 mode_len = 0;
    u8 enable;
    u8 buf[20];
//WRIST
    if (sport_info_swtich_record_get(SPORT_INFO_SWTICH_TYPE_RAISE_WRIST)) {
        enable = SWITCH_ON;
    } else if (sport_info_swtich_record_get(SPORT_INFO_SWTICH_TYPE_RAISE_WRIST_CUSTOMIZE)) {
        enable = SWITCH_TIMING;
    } else {
        enable = SWITCH_OFF;
    }
    sport_raise_wrist_get((raise_wrist_t *)buf);
    mode_len = sport_info_record_get(SPORT_INFO_MODE_TYPE_RAISE_WRIST, &mode_data);
    if (mode_data && mode_len) {
        save_detection_set(WRIST, enable, mode_data[0], buf);
    }
//SLEEP
    if (sport_info_swtich_record_get(SPORT_INFO_SWTICH_TYPE_SLEEP_DETECTION)) {
        enable = SWITCH_ON;
    } else if (sport_info_swtich_record_get(SPORT_INFO_SWTICH_TYPE_SLEEP_DETECTION_CUSTOMIZE)) {
        enable = SWITCH_TIMING;
    } else {
        enable = SWITCH_OFF;
    }
    sport_sleep_detection_get((sleep_detection_t *)buf);
    if (mode_data && mode_len) {
        save_detection_set(SLEEP, enable, 0, buf);
    }
//sendentary
    if (sport_info_swtich_record_get(SPORT_INFO_SWTICH_TYPE_SEDENTARY)) {
        enable = SWITCH_TIMING;
    } else {
        enable = SWITCH_OFF;
    }
    sport_sedentary_get((sedentary_t *)buf);//sedentary_buf[0]为午休免打扰
    mode_len = sport_info_record_get(SPORT_INFO_MODE_TYPE_SEDENTARY, &mode_data);
    if (mode_data && mode_len) {
        save_detection_set(SEDENTARY, enable, mode_data[0], buf + 1);
    }
//FALL
    if (sport_info_swtich_record_get(SPORT_INFO_SWTICH_TYPE_FALL_DETECTION)) {
        enable = SWITCH_ON;
    } else {
        enable = SWITCH_OFF;
    }
    /* printf("%s", __func__); */
    memset(buf, 0, 20);
    sport_fall_detection_get((fall_detection_t *)buf);
    if (buf[0] != 0) {
        set_emergency_contact_number(buf + 1, buf[0]);
    }
    mode_len = sport_info_record_get(SPORT_INFO_MODE_TYPE_FALL_DETECTION, &mode_data);
    if (mode_data && mode_len) {
        memset(buf, 0, 20);
        save_detection_set(FALL, enable, mode_data[0], buf);
    }
//EXERCISE_HEART_RATE
    if (sport_info_swtich_record_get(SPORT_INFO_SWTICH_TYPE_EXERCISE_HEART_RATE)) {
        enable = SWITCH_ON;
    } else {
        enable = SWITCH_OFF;
    }
    sport_exercise_heart_rate_get((e_heart_rate *)buf);
    /* printf("%s %d",__func__,buf[0]); */
    if (buf[0] == 0) {
        buf[0] = 120; //不设置数值时，默认120触发
    }
    save_exercise_heart_rate(enable, buf[0]);
    save_exercise_heart_rate_mode(enable, buf[1]);
    memset(buf, 0, 20);
    save_detection_set(CONTINUE_HEART_RATE, enable, SCREEN_LIGHT, buf);
    e_heart_rate heart_rate;
    heart_rate.max_heart_rate = 120;
    heart_rate.heart_rate_type = 1;
    sport_info_write_vm(VM_SPORT_INFO_EXERCISE_HEART_RATE, (u8 *)&heart_rate, sizeof(e_heart_rate));

//CONTINUE_HEART_RATE
    if (sport_info_swtich_record_get(SPORT_INFO_SWTICH_TYPE_CONTINUOUS_HEART_RATE)) {
        enable = SWITCH_ON;
    } else {
        enable = SWITCH_OFF;
    }
    mode_len = sport_info_record_get(SPORT_INFO_MODE_TYPE_CONTINUOUS_HEART_RATE, &mode_data);
    if (mode_data && mode_len) {
        sport_exercise_heart_rate_get((e_heart_rate *)buf);
        /* printf("%s %d",__func__,buf[0]); */
        if (buf[0] == 0) {
            buf[0] = 120; //不设置数值时，默认120触发
        }
        save_continue_heart_rate(enable, buf[0], 0);
        memset(buf, 0, 20);
        save_detection_set(CONTINUE_HEART_RATE, enable, mode_data[0], buf);
    }
    printf("sendentary=%d,fall=%d,wrist=%d,sleep=%d", WP.Sedentary_enable, WP.fall_enable, WP.wrist_enable, WP.sleep_enable);
    return 0;
}
int detection_ioctrl(int arg_num, int *arg) //打开检测功能，注册回调函数
{
    int type = -1;
    int enable = -1;
    unsigned char time[4] = {0, 0, 0, 0};
    u8 hr_threshold = -1;
    int response_mode = 0;

    switch (arg_num) {
    case 5:
        hr_threshold = arg[4]; //心率阈值
    case 4:
        time[0] = (arg[3] >> 24) & 0xff;
        time[1] = (arg[3] >> 16) & 0xff;
        time[2] = (arg[3] >> 8) & 0xff;
        time[3] = (arg[3]) & 0xff;
    case 3:
        response_mode = arg[2];
    case 2:
        enable = arg[1];
    case 1:
        type = arg[0];

        break;
    }
    printf("%s arg_num=%d \n type=%d,enable=%d,response_mode=%d,start_time=%d:%d endtime=%d:%d", \
           __func__, arg_num,		type,	enable,	response_mode,	time[0], time[1], time[2], time[3]);
    switch (type) {
    case WRIST:
    case FALL:
    case SEDENTARY:
    case SLEEP:
        save_detection_set(type, enable, response_mode, time);
        break;
    case EXERCISE_HEART_RATE://仅运动时有效
        printf("%s,%d", __func__, hr_threshold);
        save_exercise_heart_rate(enable, hr_threshold);
        save_exercise_heart_rate_mode(enable, arg[3]);
        save_detection_set(type, enable, response_mode, time);
        break;
    case CONTINUE_HEART_RATE://全天有效
        printf("%s,%d", __func__, hr_threshold);
        e_heart_rate heart_rate;
        sport_exercise_heart_rate_get(&heart_rate);
        save_continue_heart_rate(enable, heart_rate.max_heart_rate, 0);
        if (enable) {
            watch_algo_hd.algo_heart_rate_task();
        } else {
            watch_algo_hd.algo_heart_rate_task_exit();
        }
        save_detection_set(type, enable, response_mode, time);
        break;
    }

    return SUCCESS;
}


#if 0
//计步、抬腕、睡眠、跌倒、久坐检测功能开关，用于打开或关闭某一功能
u8 set_watch_motion_switch(u8 watch_type, u8 enable)
{
    printf("%s %d=%d", __func__, watch_type, enable);
    if (watch.sensor_timer_id != 0) {
        sys_s_hi_timer_del(watch.sensor_timer_id);
        watch.sensor_timer_id = 0;
    }
    if (watch.init_status != 0) {
        free(watch.workbuf);
        watch.workbuf = NULL;
    }
    switch (watch_type) {
    case WRIST://抬腕使能
        WP.wrist_enable = enable;
        break;
    case SLEEP://睡眠检测使能
        WP.sleep_enable = enable;
        break;
    case FALL://跌倒监测使能
        WP.fall_enable = enable;
        break;
    case SEDENTARY://久坐提醒使能
        WP.Sedentary_enable = enable;
        break;
    default:
        break;
    }
    if (watch.init_status == 1) {
        watch_api_init();
    }
    return 1;
}

//计步、抬腕、睡眠、跌倒、久坐检测功能查询
u8 get_watch_motion_switch(u8 watch_type)
{
    /* printf("%s %d", __func__, watch_type); */
    switch (watch_type) {
    case WRIST://抬腕使能
        return WP.wrist_enable ;
        break;
    case SLEEP://睡眠检测使能
        return WP.sleep_enable ;
        break;
    case FALL://跌倒监测使能
        return WP.fall_enable ;
        break;
    case SEDENTARY://久坐提醒使能
        return WP.Sedentary_enable ;
        break;
    default:
        break;
    }
    //重置！

    return -1;
}
#endif

/***************************************************
 			 清除数据
****************************************************/
void clr_sport_info(void)   //重置运动数据
{
    if (watch.init_status == 1) {
        memset(&__motion_info, 0, sizeof(struct motion_info));
        memset(&wtemp, 0, sizeof(struct sport_temp_data));
    }
}
/****************************************************
  			睡眠时间
****************************************************/

/***************************************************
  				睡眠记录开关
***************************************************/
void refresh_sleep_record(u8 flag)
{
    if (algo_sleep.sleepdatacnt < 120) {
        algo_sleep.sleepdatacnt = record_short_sleep(flag);
    }
    if (algo_sleep.sleepdatacnt == 120) {
        if (algo_sleep.long_short_sleep_status == 0) { //长短睡眠切换

//			watch.sleepdatacnt=record_2hour_sleep(get_2HourStatus(watch.workbuf));
            algo_sleep.long_short_sleep_status = 1;
        } else {
            algo_sleep.sleepdatacnt = record_long_sleep(flag); //return 121
        }
    }
    if (algo_sleep.sleepdatacnt > 120) {
        algo_sleep.sleepdatacnt = record_long_sleep(flag);
    }

}

void sleep_record_start(void)
{
    algo_sleep.long_short_sleep_status = 0;
    algo_sleep.sleepdatacnt = 0;
    algo_sleep.sleep_status = 0;
}


void sleep_record_stop(void)
{
    sleep_quality_analyse();//睡眠分析
    algo_sleep.sleep_status = 0;
    algo_sleep.long_short_sleep_status = 0;
    algo_sleep.sleepdatacnt = 0;
    sleep_file_write();
}

void sleep_quality_analyse(void)
{
    if (watch.init_status == 0) {
        log_e("%sinit_status=0", __func__);
        return ;
    }
    Sleep_quality_evaluation(watch.workbuf);
    algo_sleep.sleep_all.time = getSleepTimeAll(watch.workbuf);
    algo_sleep.sleep_all.scale = 100;
    algo_sleep.sleep_all.evaluate = get_SleepScore(watch.workbuf);

    algo_sleep.sleep_deep.time = getDeepSleepTime(watch.workbuf);
    algo_sleep.sleep_deep.scale = get_scale_deep(watch.workbuf);
    algo_sleep.sleep_deep.evaluate = scale_deep_evaluate(watch.workbuf);

    algo_sleep.sleep_shallow.time = getShallowSleepTime(watch.workbuf);
    algo_sleep.sleep_shallow.scale = get_scale_shallow(watch.workbuf);
    algo_sleep.sleep_shallow.evaluate = scale_shallow_evaluate(watch.workbuf);

    algo_sleep.sleep_rem.time = getREMSleepTime(watch.workbuf);
    algo_sleep.sleep_rem.scale = get_scale_REM(watch.workbuf);
    algo_sleep.sleep_rem.evaluate = scale_REM_evaluate(watch.workbuf);

    algo_sleep.wake_up_times = get_wakeup_times(watch.workbuf);
    algo_sleep.continuity_score = DeepSleep_continuity_score(watch.workbuf);
}
int sleep_time_analyse(int *arg, int sleep_type)
{
    switch (sleep_type) {
    case SLEEP_ALL:
        arg[0] = algo_sleep.sleep_all.time;
        arg[1] = algo_sleep.sleep_all.scale;
        arg[2] = algo_sleep.sleep_all.evaluate;
        return 3;
    case SLEEP_SHALLOW:
        arg[0] = algo_sleep.sleep_shallow.time;
        arg[1] = algo_sleep.sleep_shallow.scale;
        arg[2] = algo_sleep.sleep_shallow.evaluate;
        return 3;
    case SLEEP_DEEP:
        arg[0] = algo_sleep.sleep_deep.time;
        arg[1] = algo_sleep.sleep_deep.scale;
        arg[2] = algo_sleep.sleep_deep.evaluate;
        return 3;
    case SLEEP_REM:
        arg[0] = algo_sleep.sleep_rem.time;
        arg[1] = algo_sleep.sleep_rem.scale;
        arg[2] = algo_sleep.sleep_rem.evaluate;
        return 3;
    case SLEEP_CONTINUE:
        arg[0] = algo_sleep.wake_up_times;
        arg[1] = algo_sleep.continuity_score;
        arg[2] = 0;
        return 2;
    }
    return 0;
}
