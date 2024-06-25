#ifndef _BL6133_H_
#define _BL6133_H_


#include <string.h>
#include "bl6133_custom.h"


#if TCFG_TP_BL6133_ENABLE

/*************Betterlife ic rst int pin select***********/
#ifdef RESET_PIN_WAKEUP
// #define	CTP_RESET_PIN		        IO_PORTA_05
#define	CTP_RESET_PIN		        TCFG_TP_RESET_IO
#define CTP_SET_RESET_PIN_OUTPUT    gpio_write(CTP_RESET_PIN, 1);gpio_set_direction(CTP_RESET_PIN,0);gpio_set_die(CTP_RESET_PIN,1);gpio_set_hd0(CTP_RESET_PIN, 1)
#define CTP_SET_RESET_PIN_HIGH		gpio_write(CTP_RESET_PIN, 1);gpio_set_hd0(CTP_RESET_PIN, 1)
#define CTP_SET_RESET_PIN_LOW		gpio_write(CTP_RESET_PIN, 0);gpio_set_hd0(CTP_RESET_PIN, 1)
#endif

#ifdef   GPIO_EINT
#define CTP_EINT_PIN			    TCFG_TP_INT_IO
// #define  CTP_EINT_PIN			    IO_PORTA_04
#define CTP_SET_I2C_EINT_OUTPUT		gpio_write(CTP_EINT_PIN, 1);gpio_set_direction(CTP_EINT_PIN,0);gpio_set_die(CTP_EINT_PIN,1)
#define CTP_SET_I2C_EINT_INPUT		gpio_set_direction(CTP_EINT_PIN,1);gpio_set_pull_up(CTP_EINT_PIN,1);gpio_set_pull_down(CTP_EINT_PIN,0);gpio_set_die(CTP_EINT_PIN,1)
#define CTP_SET_I2C_EINT_HIGH		gpio_write(CTP_EINT_PIN, 1)
#define CTP_SET_I2C_EINT_LOW		gpio_write(CTP_EINT_PIN, 0)
#endif


// enum {
// 	ELM_EVENT_TOUCH_DOWN,
// 	ELM_EVENT_TOUCH_MOVE,
// 	ELM_EVENT_TOUCH_R_MOVE,
// 	ELM_EVENT_TOUCH_L_MOVE,
// 	ELM_EVENT_TOUCH_D_MOVE,
// 	ELM_EVENT_TOUCH_U_MOVE,
// 	ELM_EVENT_TOUCH_HOLD,
// 	ELM_EVENT_TOUCH_UP,
// };



typedef struct {
    u8 iic_hdl;
    u8 iic_delay;    //这个延时并非影响iic的时钟频率，而是2Byte数据之间的延时
} bl6133_param;

bool ctp_init_bl6133();
void ctp_sleep_bl6133(void);
void ctp_wakeup_bl6133(void);
void ctp_pad_enter_dlps_bl6133(void);
void ctp_pad_exit_dlps_bl6133(void);
void ctp_handle_interrupt_bl6133(void);
void bl_ts_set_intup(char level);
void BI6133_evt_handle(uint8_t status, uint8_t move, uint16_t x, uint16_t y);
void ctp_enter_low_power(void);
void ctp_exit_low_power(void);
int16_t get_dev_touch_x(void);
int16_t get_dev_touch_y(void);
u8 get_dev_pressed_state(void);
#endif
#endif

