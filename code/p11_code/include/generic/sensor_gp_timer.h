#ifndef _SENSOR_GP_TIMER_H_
#define _SENSOR_GP_TIMER_H_

int timer0_capture_init(u32 gSensor_int_io);//默认下降沿触发
void port_edge_wkup_set_callback(void (*call_back)(void));
void sensor_timer_cap_start();//
void sensor_timer_cap_close();//关闭定时器捕获
int timer1_init();//给hx3603 定时40ms
void timer1_isr_set_callback(void (*call_back)(void));
void timer1_start();
void timer1_close();//关闭定时器

#endif /* #ifndef _SENSOR_GP_TIMER_H_ */
