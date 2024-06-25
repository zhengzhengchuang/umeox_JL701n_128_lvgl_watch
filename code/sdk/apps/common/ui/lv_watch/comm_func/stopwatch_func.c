#include "../lv_watch.h"

/*
    注意：倒计时进行时，系统不会进入低功耗，只会灭屏。
*/

StopwatchInfoPara_t Stopwatch_Info = {
    .ctx = {0},
    .exist = false,
    .state = stopwatch_state_reset,
};

static u16 timer_id = 0;
static const u32 timer_inv = 10;

static void StopwatchOverPost(void)
{
    int over_post[1];
    over_post[0] = \
        comm_msg_stopwatch_over;
    post_comm_task_msg(over_post, 1);

    return;
}

static void StopwatchTimerCb(void *priv)
{
    stopwatch_ctx_t *s_ctx = \
        &(Stopwatch_Info.ctx);

    uint8_t sec_cnt = \
        1000/timer_inv;
    s_ctx->m_second += 1;
    s_ctx->m_second %= sec_cnt;
    if(s_ctx->m_second == 0)
    {
        s_ctx->second += 1;
        s_ctx->second %= 60;

        if(s_ctx->second == 0)
        {
            s_ctx->minute += 1;

            if(s_ctx->minute >= 60)
            {
                UserStopwatchReset();
                StopwatchOverPost();
            }
        }
    }

    return;
}

void UserStopwatchStart(void)
{
    bool *exist = \
        &(Stopwatch_Info.exist);
    *exist = true;

    if(!timer_id)
        timer_id = sys_hi_timer_add(NULL, \
            StopwatchTimerCb, timer_inv);

    return;
}

void UserStopwatchReset(void)
{
    stopwatch_ctx_t *s_ctx = \
        &(Stopwatch_Info.ctx);
    memset(s_ctx, 0, \
        sizeof(stopwatch_ctx_t));

    stopwatch_state_t *state = \
        &(Stopwatch_Info.state);
    *state = stopwatch_state_reset;

    bool *exist = \
        &(Stopwatch_Info.exist);
    *exist = false;

    if(timer_id)
        sys_hi_timer_del(timer_id);
    timer_id = 0;

    return;
}

void UserStopwatchPause(void)
{
    stopwatch_state_t *state = \
        &(Stopwatch_Info.state);
    *state = stopwatch_state_pause;

    if(timer_id)
        sys_hi_timer_del(timer_id);
    timer_id = 0;

    return;
}

void UserStopwatchResume(void)
{
    stopwatch_state_t *state = \
        &(Stopwatch_Info.state);
    *state = stopwatch_state_start;

    if(!timer_id)
        timer_id = sys_hi_timer_add(NULL, \
            StopwatchTimerCb, timer_inv);

    return;
}

static void UserStopwatchOverIsOn(void)
{
    //当前菜单是否支持弹窗
    if(!MenuSupportPopup())
        return;

    //震动、播放

    ui_menu_jump(\
        ui_act_id_stopwatch_over);

    return;
}

void UserStopwatchOverHandle(void)
{
    UserStopwatchOverIsOn();

    return;
}

void UserStopwatchCreate(void)
{
    bool exist = \
        Stopwatch_Info.exist;
    if(exist == true) 
        return;

    stopwatch_ctx_t *s_ctx = \
        &(Stopwatch_Info.ctx);
    memset(s_ctx, 0, \
        sizeof(stopwatch_ctx_t));

    stopwatch_state_t *state = \
        &(Stopwatch_Info.state);
    *state = stopwatch_state_start;

    return;
}