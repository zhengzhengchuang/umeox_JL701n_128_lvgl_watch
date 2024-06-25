#include "motor.h"

static u8 run_cnt;
static u8 motor_duty;
static const u8 motor_pwm_ch = pwm_ch1;
static struct pwm_platform_data motor_pwm;

static u8 vib_cnt;
static u8 inv_cnt;
static u16 motor_timer;

static void pwm_ch_sleep(void)
{
    mcpwm_close(motor_pwm_ch);
    JL_MCPWM->CH1_CON0 = 0x00;

    return;
}

static void pwm_ch_awaken(void)
{
    mcpwm_open(motor_pwm_ch);
    JL_MCPWM->CH1_CON0 = 0x24;

    return;
}

static void motor_stop(void)
{
    mcpwm_set_duty(motor_pwm_ch, 0);
    pwm_ch_sleep();

    return;
}

static void motor_start(void)
{
    pwm_ch_awaken();
    mcpwm_set_duty(motor_pwm_ch, motor_duty*100);

    return;
}

static void motor_timer_del(void)
{
    if(motor_timer)
        sys_timer_del(motor_timer);
    motor_timer = 0;

    return;
}

static const u8 vib_max = 10;//10*20ms
static const u8 inv_max = 10;//10*20ms

static void timer_cb(void *priv)
{
    if(vib_cnt == 0)
    {
        motor_stop();
        motor_start();
    }else if(vib_cnt == vib_max)
    {
        motor_stop();
        run_cnt--;
    }

    if(vib_cnt < vib_max)
    {
        vib_cnt++;
    }else
    {
        if(run_cnt == 0)
        {
            motor_timer_del();

            return;
        }

        vib_cnt = 0xff;

        inv_cnt++;
        if(inv_cnt >= inv_max)
        {
            inv_cnt = 0;
            vib_cnt = 0;
        }
    }

    return;
}

static void motor_timer_create(void)
{
    motor_timer_del();

    motor_timer = \
        sys_timer_add(NULL, timer_cb, 20);

    return;
}

//duty强度:0~100
void motor_run(u8 cnt, u8 duty)
{
    run_cnt = cnt;
    motor_duty = duty;

    if(cnt == 0 || \
        motor_duty == 0) 
        return;

    motor_disable();

    vib_cnt = 0;
    inv_cnt = 0;
    motor_timer_create();

    return;
}

void motor_disable(void)
{
    motor_timer_del();
    motor_stop();

    return;
}

void motor_init(void)
{
    motor_pwm.pwm_aligned_mode = pwm_edge_aligned;         //边沿对齐
    motor_pwm.pwm_ch_num = motor_pwm_ch;                   //通道
    motor_pwm.frequency = 10000;                           //Hz
    motor_pwm.duty = 10000;                                //占空比
    motor_pwm.h_pin = IO_PORTB_11;                         //任意引脚
    motor_pwm.l_pin = -1;                                  //任意引脚,不需要就填-1
    motor_pwm.complementary_en = 1;                        //两个引脚的波形, 0: 同步,  1: 互补，互补波形的占空比体现在H引脚上
    mcpwm_init(&motor_pwm);

    motor_stop();

    return;
}

static u8 motor_idle_query(void)
{
	return 1;
}

REGISTER_LP_TARGET(motor_lp_target) = {
    .name = "motor",
    .is_idle = motor_idle_query,
};
