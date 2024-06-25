#include "ui/ui.h"
#include "app_config.h"
#include "ui/ui_api.h"
#include "system/timer.h"
#include "key_event_deal.h"
#include "audio_config.h"
#include "jiffies.h"
#include "app_power_manage.h"
#include "asm/charge.h"
#include "audio_dec_file.h"
#include "device/device.h"
#include "sport/sport_api.h"
#include "sport_data/watch_common.h"
#include "sport_data/sport.h"

#if (TCFG_UI_ENABLE || (SMART_BOX_EN && JL_SMART_BOX_SENSORS_DATA_OPT))
static struct motion_info __motion_info;
static struct motion_info __execise_info;
static struct watch_execise *__execise_hd;
static const u16 leap_month_table[] = {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
static void __syst2utime(struct sys_time *time, struct utime *t)
{
    t->year = time->year;
    t->month = time->month;
    t->day = time->day;
    t->hour = time->hour;
    t->min = time->min;
    t->sec = time->sec;
}

static int  __day2sec(struct sys_time *time)
{
    int sec = 0;
    sec += time->month * 60 * 60 * 24 * leap_month_table[(time->month - 1) % 12];
    sec += time->day * 60 * 60 * 24;
    sec += time->hour * 60 * 60;
    sec += time->min * 60;
    sec += time->sec;
    return sec;
}


static void  __sec2day(int sec, struct sys_time *time)
{
    time->day = sec / (60 * 60 * 24);
    time->hour = sec % (60 * 60 * 24) / (60 * 60);
    time->min = sec % (60 * 60 * 24) % (60 * 60) / 60;
    time->sec = sec % (60 * 60 * 24) % (60 * 60) % 60;
}

static int __get_rtc_time(struct sys_time *sys_time)
{
    void *fd = dev_open("rtc", NULL);
    if (!fd) {
        memset(sys_time, 0, sizeof(struct sys_time));
        return -1;
    }
    dev_ioctl(fd, IOCTL_GET_SYS_TIME, (u32)sys_time);
    /* printf("get_sys_time : %d-%d-%d,%d:%d:%d\n", time->year, time->month, time->day, time->hour, time->min, time->sec); */
    dev_close(fd);
    return 0;
}



//运动总时间
int ui_sport_get_total_time(struct utime *t)
{
    struct sys_time time;
    get_sport_start_time(&time);
    u32 s_time = __day2sec(&time);
    /* printf("get_sport_time : %d-%d-%d,%d:%d:%d %d\n", time.year, time.month, time.day, time.hour, time.min, time.sec,s_time); */
    __get_rtc_time(&time);
    u32 c_time = __day2sec(&time);
    /* printf("get_sys_time : %d-%d-%d,%d:%d:%d %d\n", time.year, time.month, time.day, time.hour, time.min, time.sec,c_time); */
    __sec2day(c_time - s_time, &time);
    /* printf("set_sys_time : %d-%d-%d,%d:%d:%d %d\n", time.year, time.month, time.day, time.hour, time.min, time.sec,c_time); */
    /* __syst2utime(&time, t); */
    /* printf("set_sys_time : %d-%d-%d,%d:%d:%d %d\n", time.year, time.month, time.day, time.hour, time.min, time.sec,c_time); */
    /* return 0; */
    if (__execise_hd == NULL) {
        __execise_hd = zalloc(sizeof(struct watch_execise));
        watch_execise_handle_get(__execise_hd);
    }
    u8 run_status = __execise_hd->execise_ctrl_status_get();
    static u32 r_time = 0;
    static u32 p_time = 0;
    static u32 p_sec = 0;

    if (run_status == 1) {
        r_time = c_time - s_time;
        p_time = c_time;
        if (p_sec != 0) {
            p_sec = 0;
        }
    } else if (run_status == 2) {
        p_sec += c_time - p_time;
    } else if (run_status == 3) {
        p_time = c_time;
        r_time = c_time - s_time - p_sec;
    } else if (run_status == 4) { //停止

    }
    printf("%s=rtime=%d,ptime=%d,ctime=%d,psec=%d,runstatus=%d", __func__, r_time, p_time, c_time, p_sec, run_status);
    if (t == NULL) {
        return r_time;
    }
    __sec2day(r_time, &time);
    /* printf("set_sys_time : %d-%d-%d,%d:%d:%d %d\n", time.year, time.month, time.day, time.hour, time.min, time.sec,c_time); */

    __syst2utime(&time, t);
    /* printf("set_sys_time : %d-%d-%d,%d:%d:%d %d\n", time.year, time.month, time.day, time.hour, time.min, time.sec,c_time); */
    return 0;
}


//获取运动实时步数
int ui_sport_num_run_step(struct unumber *num)
{
    num->type = TYPE_NUM;
    num->numbs =  1;
    num->number[0] = __execise_info.step;
    return 0;
}
//获取运动步频
int ui_sport_num_run_step_freq(struct unumber *num)
{
    num->type = TYPE_NUM;
    num->numbs =  1;
    num->number[0] = __execise_info.motion_step_freq;
    return 0;
}

//获取运动卡路里
int ui_sport_num_run_kcall(struct unumber *num)
{
    num->type = TYPE_NUM;
    num->numbs =  1;
    num->number[0] = __execise_info.caloric;
    return 0;
}

//获取运动距离
int ui_sport_num_run_distance(struct unumber *num)
{
    num->type = TYPE_NUM;
    num->numbs =  1;
    num->number[0] = __execise_info.step_distance;
    return 0;
}

//获取配速
int ui_sport_num_run_speed(struct unumber *num)
{
    u32 speed = __motion_info.motion_speed;//10米/小时
    num->type = TYPE_NUM;
    num->numbs =  2;
    num->number[0] = speed / 100;
    num->number[1] = speed % 100;
    return 0;
}



//开始运动时间
int ui_sportrecord_get_start_time(struct utime *t)
{
    struct sys_time time;
    sportrecord_get_start_time(&time);
    __syst2utime(&time, t);
    return 0;
}


//运动时间结束
int ui_sportrecord_get_end_time(struct utime *t)
{
    struct sys_time time;
    sportrecord_get_end_time(&time);
    __syst2utime(&time, t);
    return 0;
}


//运动时间
int ui_sportrecord_get_run_time(struct utime *t)
{
    struct sys_time time;
    sportrecord_get_run_time(&time);
    __syst2utime(&time, t);
    return 0;
}



//运动热量
int ui_sportrecord_get_heat(struct unumber *num)
{
    int heat =  sportrecord_get_heat();
    num->type = TYPE_NUM;
    num->numbs =  1;
    num->number[0] = heat;
    return 0;
}





//运动最高心率
int ui_sportrecord_get_Hheart(struct unumber *num)
{


    int heat =  sportrecord_get_Hheart();
    num->type = TYPE_NUM;
    num->numbs =  1;
    num->number[0] = heat;
    return 0;
}



//运动最低心率
int ui_sportrecord_get_Lheart(struct unumber *num)
{


    int heat =  sportrecord_get_Lheart();
    num->type = TYPE_NUM;
    num->numbs =  1;
    num->number[0] = heat;
    return 0;
}




//运动平均心率
int ui_sportrecord_get_meanheart(struct unumber *num)
{

    int heat =  sportrecord_get_meanheart();
    num->type = TYPE_NUM;
    num->numbs =  1;
    num->number[0] = heat;
    return 0;
}



//运动距离
int ui_sportrecord_get_distance(struct unumber *num)
{
    u8 valid = 0;
    int digit = 1;
    int distance =  sportrecord_get_distance(&valid);
    num->type = TYPE_NUM;
    num->numbs = valid ? 2 : 1;
    switch (valid) {
    case 0:
        digit = 1;
        break;
    case 1:
        digit = 10;
        break;
    case 2:
        digit = 100;
        break;
    case 3:
        digit = 1000;
        break;
    case 4:
        digit = 10000;
        break;
    }
    num->number[0] = distance / digit;
    num->number[1] = distance % digit;
    return 0;
}





//运动恢复时间
int ui_sportrecord_get_recovertime(struct utime *t)
{
    struct sys_time time;

    time.min = __execise_info.recoverytime_min;
    time.hour = __execise_info.recoverytime_hour;
    __syst2utime(&time, t);
    return 0;
}



//运动有氧时间
int ui_sportrecord_num_aerobic(struct unumber *num)
{
    struct sys_time time = {0};
    sportrecord_get_aerobic_time(&time);
    int cur = __day2sec(&time);
    num->type = TYPE_NUM;
    num->numbs =  1;
    num->number[0] = cur / 60;
    return 0;
}



//运动有氧时间进度条
int ui_sportrecord_slider_get_aerobic(int *persent)
{
    struct sys_time time = {0};
    int total = 0;
    int cur = 0;
    sportrecord_get_aerobic_time(&time);
    cur = __day2sec(&time);
    sportrecord_get_run_time(&time);
    total = __day2sec(&time);
    *persent = cur * 100 / total;
    return 0;
}



//运动无氧时间
int ui_sportrecord_num_anaerobic(struct unumber *num)
{
    struct sys_time time = {0};
    sportrecord_get_anaerobic_time(&time);
    int cur = __day2sec(&time);
    num->type = TYPE_NUM;
    num->numbs =  1;
    num->number[0] = cur / 60;
    return 0;
}



//运动无氧进度条
int ui_sportrecord_slider_get_anaerobic(int *persent)
{
    struct sys_time time = {0};
    int total = 0;
    int cur = 0;
    sportrecord_get_anaerobic_time(&time);
    cur = __day2sec(&time);
    sportrecord_get_run_time(&time);
    total = __day2sec(&time);
    *persent = cur * 100 / total;
    return 0;
}


//运动燃烧脂肪进度
int ui_sportrecord_slider_get_fat(int *persent)
{
    struct sys_time time = {0};
    int total = 0;
    int cur = 0;
    sportrecord_get_fat_time(&time);
    cur = __day2sec(&time);
    sportrecord_get_run_time(&time);
    total = __day2sec(&time);
    *persent = cur * 100 / total;
    return 0;
}


//运动燃烧脂肪时间
int ui_sportrecord_num_fat_time(struct unumber *num)
{
    struct sys_time time = {0};
    sportrecord_get_fat_time(&time);
    int cur = __day2sec(&time);
    num->type = TYPE_NUM;
    num->numbs =  1;
    num->number[0] = cur / 60;
    return 0;
}



//运动极限时间
int ui_sportrecord_slider_get_limit(int *persent)
{
    struct sys_time time = {0};
    int total = 0;
    int cur = 0;
    sportrecord_get_limit_time(&time);
    cur = __day2sec(&time);
    sportrecord_get_run_time(&time);
    total = __day2sec(&time);
    *persent = cur * 100 / total;
    return 0;
}


//运动极限时间进度条
int ui_sportrecord_num_limit_time(struct unumber *num)
{
    struct sys_time time = {0};
    sportrecord_get_limit_time(&time);
    int cur = __day2sec(&time);
    num->type = TYPE_NUM;
    num->numbs =  1;
    num->number[0] = cur / 60;
    return 0;
}



//运动热身时间进度条
int ui_sportrecord_slider_get_warm(int *persent)
{
    struct sys_time time = {0};
    int total = 0;
    int cur = 0;
    sportrecord_get_warm_time(&time);
    cur = __day2sec(&time);
    sportrecord_get_run_time(&time);
    total = __day2sec(&time);
    *persent = cur * 100 / total;
    return 0;
}


//运动热身时间
int ui_sportrecord_num_warm_time(struct unumber *num)
{
    struct sys_time time = {0};
    sportrecord_get_warm_time(&time);
    int cur = __day2sec(&time);
    num->type = TYPE_NUM;
    num->numbs =  1;
    num->number[0] = cur / 60;
    return 0;
}



//全天步数
int ui_get_number_daily_step(struct unumber *num)
{
    num->type = TYPE_NUM;
    num->numbs =  1;
    num->number[0] = __motion_info.step;
    return 0;
}
//全天热量
int ui_get_number_daily_kcal(struct unumber *num)
{
    num->type = TYPE_NUM;
    num->numbs =  1;
    num->number[0] = __motion_info.caloric;
    return 0;
}
//全天距离
int ui_get_number_daily_distance(struct unumber *num)
{
    num->type = TYPE_NUM;
    num->numbs =  1;
    num->number[0] = __motion_info.step_distance;
    return 0;
}


//30分钟强度时间
int ui_get_number_strength_time(struct unumber *num)
{
    extern int get_sport_strength_time(void);
    int strength = get_sport_strength_time();
    num->type = TYPE_NUM;
    num->numbs =  2;
    num->number[0] = strength;
    num->number[1] = 30;
    return 0;
}




//12小时活动时间
int ui_get_number_active_time(struct unumber *num)
{
    int time = get_sport_active_time();
    num->type = TYPE_NUM;
    num->numbs =  2;
    num->number[0] = time;
    num->number[1] = 12;
    return 0;
}



//活动记录进度条
int ui_get_multprogress_active_record(int *persent)
{
    int step =  __motion_info.step;
    persent[0] = step * 100 / 10000;
    extern int get_sport_strength_time(void);
    extern int get_sport_active_time(void);
    persent[1] = get_sport_strength_time() * 100 / 30;
    persent[2] = get_sport_active_time() * 100 / 12;

    return 0;
}


//睡眠进度条
int ui_get_progress_sleep(int *persent)
{
    struct watch_sleep __sleep_hd;
    watch_sleep_handle_get(&__sleep_hd);
    int arg[3];
    __sleep_hd.sleep_time_analyse(arg, SLEEP_ALL);
    int time = arg[0];
    int target = get_watch_sleep_target();
    if (time < 0) {
        time  = 0 ;
    }
    persent[0] = time * 100 / target;
    if (persent[0] > 100) {
        persent[0] = 100;
    }

    return 0;
}



//睡眠时间
int ui_get_number_sleep_time(struct unumber *num)
{
    struct watch_sleep __sleep_hd;
    watch_sleep_handle_get(&__sleep_hd);
    int arg[3];
    __sleep_hd.sleep_time_analyse(arg, SLEEP_ALL);
    int time = arg[0];
    if (time < 0) {
        time  = 0 ;
    }
    num->type = TYPE_NUM;
    num->numbs =  1;
    num->number[0] = time ;
    return 0;
}

//睡眠小时
int ui_get_number_sleep_hour_time(struct unumber *num)
{
    struct watch_sleep __sleep_hd;
    watch_sleep_handle_get(&__sleep_hd);
    int arg[3];
    __sleep_hd.sleep_time_analyse(arg, SLEEP_ALL);
    int time = arg[0];
    if (time < 0) {
        time  = 0 ;
    }
    num->type = TYPE_NUM;
    num->numbs =  1;
    num->number[0] = (int) time / 60;
    return 0;
}



//睡眠分钟
int ui_get_number_sleep_minute_time(struct unumber *num)
{
    struct watch_sleep __sleep_hd;
    watch_sleep_handle_get(&__sleep_hd);
    int arg[3];
    __sleep_hd.sleep_time_analyse(arg, SLEEP_ALL);
    int time = arg[0];
    num->type = TYPE_NUM;
    num->numbs =  1;
    num->number[0] = time % 60;
    return 0;
}



//睡眠目标
int ui_get_number_sleep_target(struct unumber *num)
{
    int target = get_watch_sleep_target();
    num->type = TYPE_NUM;
    num->numbs =  1;
    num->number[0] = target;
    return 0;
}



//实时心率
int ui_get_number_heart(struct unumber *num)
{
    struct watch_heart_rate __heartrate_hd;
    watch_heart_rate_handle_get(&__heartrate_hd);
    int heart = __heartrate_hd.heart_rate_data_get(LAST_DATA, NULL);
    num->type = TYPE_NUM;
    num->numbs =  1;
    num->number[0] = heart;
    return 0;
}


//最大心率
int ui_get_number_Hheart(struct unumber *num)
{
    struct watch_heart_rate __heartrate_hd;
    watch_heart_rate_handle_get(&__heartrate_hd);
    num->type = TYPE_NUM;
    num->numbs =  1;
    num->number[0] = __heartrate_hd.heart_rate_data_get(MAX_DATA, NULL);
    return 0;
}


//最低心率
int ui_get_number_Lheart(struct unumber *num)
{
    struct watch_heart_rate __heartrate_hd;
    watch_heart_rate_handle_get(&__heartrate_hd);
    num->type = TYPE_NUM;
    num->numbs =  1;
    num->number[0] = __heartrate_hd.heart_rate_data_get(MIN_DATA, NULL);
    return 0;
}


//静息心率
int ui_get_number_Resting_heart(struct unumber *num)
{
    struct watch_heart_rate __heartrate_hd;
    watch_heart_rate_handle_get(&__heartrate_hd);

    num->type = TYPE_NUM;
    num->numbs =  1;
    num->number[0] = __heartrate_hd.heart_rate_data_get(RESTING_DATA, NULL);
    return 0;
}

//血氧浓度
int ui_get_number_blood_oxygen(struct unumber *num)
{
    struct watch_blood_oxygen __bloodoxygen_hd;
    watch_blood_oxygen_handle_get(&__bloodoxygen_hd);
    num->type = TYPE_NUM;
    num->numbs =  1;
    num->number[0] = __bloodoxygen_hd.blood_oxygen_get(LAST_DATA, NULL);
    return 0;
}
//运动
int ui_get_number_spdist(struct unumber *num)
{
    num->type = TYPE_NUM;
    num->numbs =  1;
    num->number[0] = __execise_info.step_distance;
    return 0;
}
int ui_get_number_spstep(struct unumber *num)
{
    num->type = TYPE_NUM;
    num->numbs =  1;
    num->number[0] = __execise_info.step;
    return 0;
}
int ui_get_number_pace(struct unumber *num)
{
    num->type = TYPE_NUM;
    num->numbs =  1;
    num->number[0] = __execise_info.motion_pace;
    return 0;
}
int ui_get_number_stepfreq(struct unumber *num)
{
    num->type = TYPE_NUM;
    num->numbs =  1;
    num->number[0] = __execise_info.motion_step_freq;
    return 0;
}
int ui_get_number_kcal(struct unumber *num)
{
    num->type = TYPE_NUM;
    num->numbs =  1;
    num->number[0] = __execise_info.caloric;
    return 0;
}
int ui_get_number_stamina(struct unumber *num)
{
    printf("%s", __func__);
    num->type = TYPE_NUM;
    num->numbs = 1;
    num->number[0] = 90;
    return 0;
}
int ui_get_number_load(struct unumber *num)
{
    printf("%s", __func__);
    num->type = TYPE_NUM;
    num->numbs = 1;
    num->number[0] = 85;
    return 0;
}
/*
//全天数据
Rstep   获取全天步数
Rkcal   获取全天热量
Rdist	获取全天距离

Ractive 获取活动时间
Rsleep  获得睡眠时间
RsleepH 获得睡眠时间小时
RsleepM 获得睡眠时间分钟
RsleeTG 睡眠目标
Rheart  实时心率
RheartS 静息心率
RheartH 最高心率
RheartL 最低心率
Roxygen 血氧浓度
Soxygen 血氧浓度测量状态

Rstreng 获取运动强度
Spdist  运动距离
Rspeed  实时配速
Rsfreq  实时步频率
Sptime  运动时间
Spkcall 运动卡路里
Spstep  运动步数

 *
 *
 *
 *
 *
 */



const struct MANAGE_SOURCE sport_record_table[] = {
    {-1, "Sptime", CTRL_TYPE_TIME, ui_sport_get_total_time},
    {-1, "Spstep", CTRL_TYPE_NUMBER, ui_sport_num_run_step},
    {-1, "Rsfreq", CTRL_TYPE_NUMBER, ui_sport_num_run_step_freq},
    {-1, "Spkcall", CTRL_TYPE_NUMBER, ui_sport_num_run_kcall},
    {-1, "Spdist", CTRL_TYPE_NUMBER, ui_sport_num_run_distance},
    {-1, "Rspeed", CTRL_TYPE_NUMBER, ui_sport_num_run_speed},

    {-1, "stime", CTRL_TYPE_TIME, ui_sportrecord_get_start_time},
    {-1, "etime", CTRL_TYPE_TIME, ui_sportrecord_get_end_time},
    {-1, "rtime", CTRL_TYPE_TIME, ui_sportrecord_get_run_time},
    {-1, "recoT", CTRL_TYPE_TIME, ui_sportrecord_get_recovertime},
    {-1, "sheat", CTRL_TYPE_NUMBER, ui_sportrecord_get_heat},
    {-1, "sdist", CTRL_TYPE_NUMBER, ui_sportrecord_get_distance},
    {-1, "Hheart", CTRL_TYPE_NUMBER, ui_sportrecord_get_Hheart},
    {-1, "Lheart", CTRL_TYPE_NUMBER, ui_sportrecord_get_Lheart},
    {-1, "Mheart", CTRL_TYPE_NUMBER, ui_sportrecord_get_meanheart},



    {-1, "Eartime", CTRL_TYPE_NUMBER, ui_sportrecord_num_aerobic},
    {-1, "Anatime", CTRL_TYPE_NUMBER, ui_sportrecord_num_anaerobic},
    {-1, "Fattime", CTRL_TYPE_NUMBER, ui_sportrecord_num_fat_time},
    {-1, "Limtime", CTRL_TYPE_NUMBER, ui_sportrecord_num_limit_time},
    {-1, "Wtime", CTRL_TYPE_NUMBER, ui_sportrecord_num_warm_time},

    {-1, "Rstep", CTRL_TYPE_NUMBER, ui_get_number_daily_step},
    {-1, "Rkcal", CTRL_TYPE_NUMBER, ui_get_number_daily_kcal},
    {-1, "Rdist", CTRL_TYPE_NUMBER, ui_get_number_daily_distance},
    {-1, "Rstreng", CTRL_TYPE_NUMBER, ui_get_number_strength_time},
    {-1, "Ractive", CTRL_TYPE_NUMBER, ui_get_number_active_time},
    {-1, "Rsleep", CTRL_TYPE_NUMBER, ui_get_number_sleep_time},
    {-1, "RsleepH", CTRL_TYPE_NUMBER, ui_get_number_sleep_hour_time},
    {-1, "RsleepM", CTRL_TYPE_NUMBER, ui_get_number_sleep_minute_time},
    {-1, "RsleeTG", CTRL_TYPE_NUMBER, ui_get_number_sleep_target},
    {MULTI_PROGRESS, "none", CTRL_TYPE_MULTIPROGRESS, ui_get_multprogress_active_record},
    {PROGRESS_SLEEP, "none", CTRL_TYPE_PROGRESS, ui_get_progress_sleep},


    {-1, "Rheart", CTRL_TYPE_NUMBER, ui_get_number_heart},
    {-1, "RheartS", CTRL_TYPE_NUMBER, ui_get_number_Resting_heart},
    {-1, "RheartH", CTRL_TYPE_NUMBER, ui_get_number_Hheart},
    {-1, "RheartL", CTRL_TYPE_NUMBER, ui_get_number_Lheart},

    {-1, "Rspeed", CTRL_TYPE_NUMBER, ui_get_number_pace},
    {-1, "Spdist", CTRL_TYPE_NUMBER, ui_get_number_spdist},
    {-1, "Spstep", CTRL_TYPE_NUMBER, ui_get_number_spstep},
    {-1, "Rsfreq", CTRL_TYPE_NUMBER, ui_get_number_stepfreq},
    {-1, "Spkcall", CTRL_TYPE_NUMBER, ui_get_number_kcal},
    {-1, "Roxygen", CTRL_TYPE_NUMBER, ui_get_number_blood_oxygen },
    {-1, "Rstam", CTRL_TYPE_NUMBER, ui_get_number_stamina},
    {-1, "Rload", CTRL_TYPE_NUMBER, ui_get_number_load},


#if(CONFIG_UI_STYLE == STYLE_JL_WTACH)
    {SPORT_SLIDER_AEROBIC,  "none", CTRL_TYPE_SLIDER, ui_sportrecord_slider_get_aerobic},
    {SPORT_SLIDER_ANAEROBIC, "none", CTRL_TYPE_SLIDER, ui_sportrecord_slider_get_anaerobic},
    {SPORT_SLIDER_FAT,      "none", CTRL_TYPE_SLIDER, ui_sportrecord_slider_get_fat},
    {SPORT_SLIDER_LIMIT,    "none", CTRL_TYPE_SLIDER, ui_sportrecord_slider_get_limit},
    {SPORT_SLIDER_WARM,     "none", CTRL_TYPE_SLIDER, ui_sportrecord_slider_get_warm},
#endif
    {0, NULL, 0, NULL},
};

static struct MANAGE_SOURCE *source_info_table = (struct MANAGE_SOURCE *)sport_record_table;

void ui_update_source_table_set(const struct MANAGE_SOURCE *table)
{
    source_info_table = table;
}


void ui_update_source_by_elm(struct element *elm, u8 redraw)
{
    if (__execise_hd == NULL) {
        __execise_hd = zalloc(sizeof(struct watch_execise));
        watch_execise_handle_get(__execise_hd);
    }
    __execise_hd->execise_info_get(&__execise_info);
    __execise_hd->daily_motion_info_get(&__motion_info);
    struct MANAGE_SOURCE *p;
    struct element *k;
    u8 type = 0;
    u8 find = 0;

    if (!elm) {
        return;
    }

    if (!source_info_table) {
        return;
    }

    type = ui_id2type(elm->id);

    if (type == CTRL_TYPE_LAYOUT) {
        find = 1;//布局特殊处理
        list_for_each_child_element(k, elm) {
            ui_update_source_by_elm(k, 0);
        }
    }

    if (type == CTRL_TYPE_MULTIPROGRESS) {
        list_for_each_child_element(k, elm) {
            ui_update_source_by_elm(k, 0);
        }
    }

    if (type == CTRL_TYPE_PROGRESS) {
        list_for_each_child_element(k, elm) {
            ui_update_source_by_elm(k, 0);
        }
    }

    if (type == CTRL_TYPE_GRID) {
        struct ui_grid *grid = (struct ui_grid *)elm;
        find = 1;//布局特殊处理
        for (int i = 0; i < grid->avail_item_num; i++) {
            list_for_each_child_element(k, &grid->item[i].elm) {
                ui_update_source_by_elm(k, 0);
            }
        }
    }

    for (p = source_info_table; (p->source != NULL || p->id) && !find; p++) {
        if (p->type == type) {
            switch (type) {
            case CTRL_TYPE_TEXT:
                struct ui_text *text = (struct ui_text *)elm;
                if (p->source && !strcmp(text->source, p->source)) {
                    find = 1;
                } else if (p->id && p->id == elm->id) {
                    find = 1;
                }
                break;
            case CTRL_TYPE_NUMBER:
                struct ui_number *number = (struct ui_number *)elm;

                if (p->source && !strcmp(number->source, p->source) && p->func) {
                    find = 1;
                } else if (p->id && p->id == elm->id) {
                    find = 1;
                }

                if (find) {
                    struct unumber n = {0};
                    int (*number_cb)(struct unumber *) = (int (*)(struct unumber *))(p->func);
                    number_cb(&n);
                    ui_number_update(elm, &n);
                }

                break;
            case CTRL_TYPE_TIME:
                struct ui_time *time = (struct ui_time *)elm;
                if (p->source && !strcmp(time->source, p->source)) {
                    find = 1;
                } else if (p->id && p->id == elm->id) {
                    find = 1;
                }

                if (find) {
                    struct utime t = {0};
                    int (*time_cb)(struct utime *) = (int (*)(struct utime *))(p->func);
                    time_cb(&t);
                    ui_time_update(elm, &t);
                }
                break;
            case CTRL_TYPE_SLIDER:

                if (p->id && p->id == elm->id) {
                    find = 1;
                }

                if (find) {
                    int persent = 0;
                    int (*slider_cb)(int *) = (int (*)(int *))(p->func);
                    slider_cb(&persent);
                    ui_slider_set_persent(elm, persent);
                }
                break;

            case  CTRL_TYPE_PROGRESS:
                if (p->id && p->id == elm->id) {
                    find = 1;
                }

                if (find) {
                    int persent = 0;
                    int (*progress_slider_cb)(int *) = (int (*)(int *))(p->func);
                    progress_slider_cb(&persent);
                    ui_progress_set_persent(elm, persent);
                }
                break;

            case  CTRL_TYPE_MULTIPROGRESS:
                if (p->id && p->id == elm->id) {
                    find = 1;
                }

                if (find) {
                    int persent[3] = {0};
                    int (*mult_progress_slider_cb)(int *) = (int (*)(int *))(p->func);
                    mult_progress_slider_cb(persent);
                    ui_multiprogress_set_persent(elm, persent[0]);
                    ui_multiprogress_set_second_persent(elm, persent[1]);
                    ui_multiprogress_set_third_persent(elm, persent[2]);
                }
                break;
            }
        }
    }
    if (__execise_hd != NULL) {
        free(__execise_hd);
        __execise_hd = NULL;
    }
    if (find && redraw) {
        ui_core_redraw(elm);
    }
}


#endif /* #if TCFG_UI_ENABLE */



