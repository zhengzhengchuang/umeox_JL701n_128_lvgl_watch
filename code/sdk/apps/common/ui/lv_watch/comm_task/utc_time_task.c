#include "../lv_watch.h"

void timer_sec_task_handle(void)
{
    /* 定时器秒任务 */
    HrTimerSecProcess();
    BoTimerSecProcess();

    return;
}

void utc_day_task_handle(int priv)
{
    struct sys_time *ptime = (struct sys_time *)priv;

    /* 新的一天，朝拜时间更新 */
    PTimeDayProcess(ptime);

    /* 新的一天，回历更新 */
    HcalendarInfoParaUpdate();

    return;
}

void utc_minute_task_handle(int priv)
{
    struct sys_time *ptime = (struct sys_time *)priv;

    /* 隔一分钟，轮询睡眠状态 */
    SleepUtcMinProcess(ptime);
    
    /* 隔一分钟，轮询自动心率开启 */
    HrUtcMinProcess(ptime);

    /* 隔一分钟，轮询自动血氧开启 */
    BoUtcMinProcess(ptime);

    /* 隔一分钟，轮询勿扰状态 */
    DndUtcMinProcess(ptime);

    /* 隔一分钟，轮询久坐提醒 */
    SedUtcMinProcess(ptime);

    /* 隔一分钟，轮询天气数据 */
    WeatherUtcMinProcess(ptime);

    /* 隔一分钟，轮询闹钟 */
    UserUtcMinAlarmProcess(ptime);

    /* 隔一分钟，轮询赞念提醒 */
    TasbihReminderProcess(ptime);

    /* 隔一分钟，pedo数据处理 */
    PedoDataMinProcess(ptime);

    return;
}

void utc_second_task_handle(int priv)
{
    struct sys_time *ptime = (struct sys_time *)priv;

    /* 隔秒，判断朝拜时间是否到 */
    PTimeSecProcess(ptime);

    /* 隔秒，判断回历节日是否到 */
    HcalendarProcess(ptime);

    /* 隔秒，gomore算法process */
    GoMoreAlgoProcess(ptime);

    /* 隔秒，低电提醒process */
    LPRemindProcess(ptime);

    /* 隔秒，低电关机process */
    LPShutdownProcess(ptime);

    return;
}
