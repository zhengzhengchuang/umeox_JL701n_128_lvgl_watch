#include "../lv_watch.h"

/*
    注意：倒计时进行时，系统不会进入低功耗，只会灭屏。
*/

CountdownInfoPara_t Countdown_Info = {
    .ctx = {0},
    .exist = false,
    .state = countdown_state_reset,
};

static uint32_t timeout_cnt = 0;
static const int16_t percent_max = 10000;

static u16 timer_id = 0;
static const uint32_t timer_inv = 100;

static void CountdownTimeoutPost(void)
{
    int timeout_post[1];
    timeout_post[0] = \
        comm_msg_countdown_timeout;
    post_comm_task_msg(timeout_post, 1);

    return;
}

static void CountdownTimerCb(void *priv)
{
    countdown_ctx_t *c_ctx = \
        &(Countdown_Info.ctx);

    uint8_t sec_cnt = 1000/timer_inv;

    timeout_cnt += 1;
    uint8_t second = timeout_cnt%sec_cnt;
    if(second == 0)
    {
        if(c_ctx->cur_cnt > 0)
        {
            c_ctx->cur_cnt -= 1;

            if(c_ctx->cur_cnt == 0)
            {
                UserCountdownReset();
                CountdownTimeoutPost();
            }  
        }
    }

    if(c_ctx->cur_cnt > 0)
    {
        uint32_t percent_timeout_cnt = \
            (c_ctx->total_cnt*sec_cnt) - timeout_cnt;
        
        c_ctx->percent = \
            (percent_timeout_cnt*percent_max) / \
                (c_ctx->total_cnt*sec_cnt);

        if(c_ctx->percent > percent_max)
            c_ctx->percent = percent_max;

        if(c_ctx->percent < 0)
            c_ctx->percent = 0;
    }

    return;
}

int16_t GetPercentMax(void)
{
    return percent_max;
}

void UserCountdownReset(void)
{
    countdown_ctx_t *c_ctx = \
        &(Countdown_Info.ctx);
    memset(c_ctx, 0, \
        sizeof(countdown_ctx_t));

    countdown_state_t *state = \
        &(Countdown_Info.state);
    *state = countdown_state_reset;

    bool *exist = \
        &(Countdown_Info.exist);
    *exist = false;

    if(timer_id)
        sys_hi_timer_del(timer_id);
    timer_id = 0;

    timeout_cnt = 0;

    return;
}

void UserCountdownPause(void)
{
    countdown_state_t *state = \
        &(Countdown_Info.state);
    *state = countdown_state_puase;

    if(timer_id)
        sys_hi_timer_del(timer_id);

    timer_id = 0;

    return;
}

void UserCountdownResume(void)
{
    countdown_state_t *state = \
        &(Countdown_Info.state);
    *state = countdown_state_start;

    if(!timer_id)
        timer_id = sys_hi_timer_add(NULL, \
            CountdownTimerCb, timer_inv);

    return;
}

void UserCountdownStart(void)
{
    bool *exist = \
        &(Countdown_Info.exist);
    *exist = true;

    if(!timer_id)
        timer_id = sys_hi_timer_add(NULL, \
            CountdownTimerCb, timer_inv);

    return;
}

static void UserCountdownOverIsOn(void)
{
    //当前菜单是否支持弹窗
    if(!MenuSupportPopup())
        return;

    //震动、播放

    ui_menu_jump(\
        ui_act_id_countdown_over);

    return;
}

void UserCountdownTimeoutHandle(void)
{
    UserCountdownOverIsOn();

    return;
}

void UserCountdownCreate(uint32_t cnt)
{
    bool exist = \
        Countdown_Info.exist;
    if(exist == true) 
        return;

    if(cnt == 0) return;

    countdown_ctx_t *c_ctx = \
        &(Countdown_Info.ctx);
    c_ctx->cur_cnt = cnt;
    c_ctx->total_cnt = cnt;
    c_ctx->percent = percent_max;

    countdown_state_t *state = \
        &(Countdown_Info.state);
    *state = countdown_state_start;

    timeout_cnt = 0;
    
    return;
}
