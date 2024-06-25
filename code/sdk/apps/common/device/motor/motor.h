#ifndef __MOTOR_H__
#define __MOTOR_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "timer.h"
#include "asm/gpio.h"
#include "asm/mcpwm.h"

/**/
enum
{
    motor_duty_0 =  0,
    motor_duty_1 =  10,
    motor_duty_2 =  20,
    motor_duty_3 =  30,
    motor_duty_4 =  40,
    motor_duty_5 =  50,
    motor_duty_6 =  60,
    motor_duty_7 =  70,
    motor_duty_8 =  80,
    motor_duty_9 =  90,
    motor_duty_10 = 100,
};

#define sdw_motor_duty  (motor_duty_8) //关机震动强度
#define def_motor_duty  (motor_duty_10)//默认震动强度

void motor_init(void);
void motor_disable(void);
void motor_run(u8 cnt, u8 duty);
#ifdef __cplusplus
} /* extern "C" */
#endif

#endif 
