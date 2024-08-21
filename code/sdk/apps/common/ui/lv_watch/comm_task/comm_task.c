#include "includes.h"
#include "../lv_watch.h"

static u16 utc_timer = 0;

static u8 last_sec = 0xff;
static u8 last_min = 0xff;
static u8 last_hour = 0xff;

static struct sys_time utc_time;

extern void sys_enter_soft_poweroff(void *priv);

static void UtcDayHandle(void *priv)
{
    struct sys_time *ptime = \
        (struct sys_time *)priv;

    bool day_update = false;

    if(ptime->hour == 0)
    {
        if(last_hour != ptime->hour)
        {
            last_hour = ptime->hour;
            day_update = true;
        }
    }else
    {
        last_hour = 0xff;
    }

    if(day_update == false)
        return;

    int CommMsg[2];
    CommMsg[0] = comm_msg_utcday_handle;
    CommMsg[1] = (int)priv;
    PostCommTaskMsg(CommMsg, 2);

    return;
}

static void UtcMinuteHandle(void *priv)
{
    struct sys_time *ptime = \
        (struct sys_time *)priv;

    bool min_update = false;

    if(ptime->sec == 0)
    {
        if(last_min != ptime->min)
        {
            last_min = ptime->min;
            min_update = true;
        }
    }else
    {
        last_min = 0xff;
    }

    if(min_update == false)
        return;

    int CommMsg[2];
    CommMsg[0] = comm_msg_utcmin_handle;
    CommMsg[1] = (int)priv;
    PostCommTaskMsg(CommMsg, 2);

    return;
}

static void UtcSecondHandle(void *priv)
{
    struct sys_time *ptime = \
        (struct sys_time *)priv;

    if(last_sec == ptime->sec)
        return;

    last_sec = ptime->sec;

    int CommMsg[2];
    CommMsg[0] = comm_msg_utcsec_handle;
    CommMsg[1] = (int)priv;
    PostCommTaskMsg(CommMsg, 2);

    return;
}

static void TimerSecondHandle(void *priv)
{
    int CommMsg[2];
    CommMsg[0] = comm_msg_timersec_handle;
    PostCommTaskMsg(CommMsg, 1);

    return;
}

static u8 SecCnt = 0;
static void TimerCb(void *priv)
{  
    SecCnt += 1;
    SecCnt %= 2;
    if(SecCnt == 0)
        TimerSecondHandle(NULL);

    GetUtcTime(&utc_time);
    UtcDayHandle(&utc_time);
    UtcMinuteHandle(&utc_time);
    UtcSecondHandle(&utc_time);

    // u8 bt_status = get_bt_connect_status();
    // printf("bt_status = %d\n", bt_status);

    return;
}

static void CommTaskHandle(void *p)
{
    int ret;
    int msg[16] = {0};

    /*注意：这里看看有没有影响到功耗*/
    if(utc_timer == 0)
        utc_timer = sys_timer_add(NULL, TimerCb, 500);

    while(1)
    {
        ret = os_taskq_pend(NULL, msg, ARRAY_SIZE(msg)); 

        if(ret == OS_TASKQ)
            CommTaskMsgHandle(msg, ARRAY_SIZE(msg));
    }
    
    return;
}

extern void user_dev_reset_handle(void *priv);
extern void user_dev_restart_handle(void *priv);
void CommTaskMsgHandle(int *msg, u8 len)
{
    if(msg == NULL || len == 0)
        return;

    int cmd = msg[0];

    switch(cmd)
    {
        case comm_msg_gomore_init:
            GoMoreAlgoInit();
            break;

        case comm_msg_utcday_handle:
            utc_day_task_handle(msg[1]);
            break;

        case comm_msg_utcmin_handle:
            utc_minute_task_handle(msg[1]);
            break;

        case comm_msg_utcsec_handle:
            utc_second_task_handle(msg[1]);
            break;

        case comm_msg_timersec_handle:
            timer_sec_task_handle();
            break;

        case comm_msg_countdown_timeout:
            UserCountdownTimeoutHandle();
            break;

        case comm_msg_stopwatch_over:
            UserStopwatchOverHandle();
            break;

        case comm_msg_dev_shutdown:
            sys_enter_soft_poweroff(NULL);
            break;

        case comm_msg_dev_restart:
            user_dev_restart_handle(NULL);
            break;

        case comm_msg_dev_reset:
            user_dev_reset_handle(NULL);
            break;

        case comm_msg_power_update:
            BatPowerUpdateHandle();
            break;

        default:
            break;
    }

    return;
}

void CommTaskCreate(void)
{
    int err = task_create(CommTaskHandle, NULL, Comm_Task_Name);
    if(err) printf("comm task create err!!!!!!!:%d \n", err);

    return;
}

int PostCommTaskMsg(int *msg, u8 len)
{
    int err = 0;
    int count = 0;

    if(!msg || len == 0)
        return -1;

__retry:
    err = os_taskq_post_type(Comm_Task_Name, \
        msg[0], len - 1, &msg[1]);

    if(cpu_in_irq() || cpu_irq_disabled())
        return err;

    if(err) 
    {
        if(!strcmp(os_current_task(), Comm_Task_Name)) 
            return err;

        if(count > 20)
            return -1;
        
        count++;
        os_time_dly(1);
        goto __retry;
    }

    return err;
}
