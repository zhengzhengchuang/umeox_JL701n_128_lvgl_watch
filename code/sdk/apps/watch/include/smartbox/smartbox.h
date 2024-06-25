#ifndef __SMARTBOX_H__
#define __SMARTBOX_H__

#include "typedef.h"
#include "app_config.h"

#include "smartbox/config.h"
#include "smartbox/function.h"
#include "browser/browser.h"
#include "smartbox/cmd_user.h"
#include "smartbox/smartbox_task.h"

void smartbox_init(void);

void smartbox_timer_contrl(u8 status);

#endif//__SMARTBOX_H__
