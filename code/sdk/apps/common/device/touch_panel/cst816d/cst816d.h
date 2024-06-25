#ifndef __CST816D_H__
#define __CST816D_H__

#ifdef __cplusplus
extern "C" {
#endif


#include "app_config.h"
#include "system/includes.h"
#include "asm/iic_hw.h"
#include "asm/iic_soft.h"
#include "asm/gpio.h"
#include "asm/mcpwm.h"
#include "ui/ui.h"
#include "ui/ui_api.h"

#if TCFG_TOUCH_PANEL_ENABLE

#if TCFG_TP_CST816D_ENABLE

bool get_dev_pressed_state(void);
void set_dev_pressed_state(bool state);

int16_t get_dev_touch_x(void);
void set_dev_touch_x(int16_t x);

int16_t get_dev_touch_y(void);
void set_dev_touch_y(int16_t y);
#endif

#endif
#ifdef __cplusplus
}
#endif

#endif
