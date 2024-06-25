#include "includes.h"
#include "../lv_watch.h"

static u16 utc_timer_id = 0;
static bool day_handle = false;
static bool minute_handle = false;
static bool second_handle = false;

static const u16 comm_timer_inv = 500;

extern void sys_enter_soft_poweroff(void *priv);

static void utc_day_handle(void *priv)
{
    struct sys_time *utc_time = \
        (struct sys_time *)priv;

    if(utc_time->hour >= 23 && utc_time->min >= 59 && \
        utc_time->sec >= 58)
        day_handle = true;

    if(utc_time->hour != 0 || utc_time->min != 0 || \
        utc_time->sec != 0)
        return;

    if(!day_handle)
        return;

    int day_handle_msg[2];
    day_handle_msg[0] = \
        comm_msg_utcday_handle;
    day_handle_msg[1] = \
        (int)priv;
    post_comm_task_msg(\
        day_handle_msg, 2);

    day_handle = false;

    return;
}

static void utc_minute_handle(void *priv)
{
    struct sys_time *utc_time = \
        (struct sys_time *)priv;

    if(utc_time->sec >= 58)
        minute_handle = true;

    if(utc_time->sec != 0)
        return;

    if(!minute_handle)
        return;

    int minute_handle_msg[2];
    minute_handle_msg[0] = \
        comm_msg_utcmin_handle;
    minute_handle_msg[1] = \
        (int)priv;
    post_comm_task_msg(\
        minute_handle_msg, 2);

    minute_handle = false;

    return;
}

static void utc_second_handle(void *priv)
{
    struct sys_time *utc_time = \
        (struct sys_time *)priv;

    static u8 last_sec = 0xff;

    if(last_sec != utc_time->sec)
    {
        last_sec = utc_time->sec;
        second_handle = true;
    }else
        second_handle = false;

    if(!second_handle)
        return;

    int second_handle_msg[2];
    second_handle_msg[0] = \
        comm_msg_utcsec_handle;
    second_handle_msg[1] = \
        (int)priv;
    post_comm_task_msg(\
        second_handle_msg, 2);

    second_handle = false;

    return;
}

static void timer_second_handle(void *priv)
{
    int comm_handle_msg[1];
    comm_handle_msg[0] = \
        comm_msg_timersec_handle;
    post_comm_task_msg(\
        comm_handle_msg, 1);

    return;
}

static u8 SecCnt = 0;
static void utc_timerout_cb(void *priv)
{  
    SecCnt += 1;
    SecCnt %= (1000/comm_timer_inv);
    if(SecCnt == 0)
        timer_second_handle(NULL);

    static struct sys_time \
        utc_time;
    GetUtcTime(&utc_time);

    utc_day_handle(&utc_time);

    utc_minute_handle(&utc_time);

    utc_second_handle(&utc_time);

    return;
}

static void comm_task_handle(void *p)
{
    int rev_ret;
    int rev_msg[8] = {0};

    /*注意：这里看看有没有影响到功耗*/
    if(!utc_timer_id)
        utc_timer_id = sys_timer_add(NULL, \
            utc_timerout_cb, comm_timer_inv);

    while(1)
    {
        rev_ret = \
            os_taskq_pend(NULL, rev_msg, \
                ARRAY_SIZE(rev_msg)); 

        if(rev_ret == OS_TASKQ)
            comm_task_msg_handle(rev_msg, \
                ARRAY_SIZE(rev_msg));
    }
    
    return;
}

extern void user_write_p11_sys_time_by_timer(void *priv);
void comm_task_msg_handle(int *rev_msg, u8 len)
{
    if(!rev_msg || len == 0)
        return;

    int msg_cmd = rev_msg[0];

    switch(msg_cmd)
    {
        case comm_msg_utcday_handle:
            utc_day_task_handle(\
                rev_msg[1]);
            break;

        case comm_msg_utcmin_handle:
            utc_minute_task_handle(\
                rev_msg[1]);
            break;

        case comm_msg_utcsec_handle:
            utc_second_task_handle(\
                rev_msg[1]);
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
            cpu_reset();
            break;

        case comm_msg_dev_reset:
            cpu_reset();
            break;

        default:
            printf("comm task not found\n");
            break;
    }

    return;
}

void comm_task_create(void)
{
    int err = task_create(comm_task_handle, \
        NULL, Comm_Task_Name);
    if(err) 
        r_printf("comm task create err!!!!!!!:%d \n", err);

    return;
}

int post_comm_task_msg(int *post_msg, u8 len)
{
    int err = 0;
    int count = 0;

    if(!post_msg || len == 0)
        return -1;

__retry:
    err = os_taskq_post_type(Comm_Task_Name, \
        post_msg[0], len - 1, &post_msg[1]);

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
