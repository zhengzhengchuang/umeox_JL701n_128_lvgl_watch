#include "../lv_watch.h"

void timer_sec_task_handle(void)
{
    HrTimerSecProcess();
    BoTimerSecProcess();

    return;
}

void utc_day_task_handle(int priv)
{
    struct sys_time *utc_time = \
        (struct sys_time *)priv;

    PTimeInfoParaUpdate();
    HcalendarInfoParaUpdate();

    return;
}

void utc_minute_task_handle(int priv)
{
    struct sys_time *utc_time = \
        (struct sys_time *)priv;

    HrProcess(utc_time);
    BoProcess(utc_time);
    DndProcess(utc_time);
    SedProcess(utc_time);
    SleepProcess(utc_time);
    WeatherProcess(utc_time);
    UserAlarmProcess(utc_time);
    TasbihReminderProcess(utc_time);

    return;
}

void utc_second_task_handle(int priv)
{
    struct sys_time *utc_time = \
        (struct sys_time *)priv;

    PTimeProcess(utc_time);
    HcalendarProcess(utc_time);

    return;
}
