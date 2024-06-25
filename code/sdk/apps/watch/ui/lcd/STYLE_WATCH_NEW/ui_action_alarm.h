
#ifndef  __UI_ACTION_ALARM_H_
#define  __UI_ACTION_ALARM_H_
#include "app_task.h"
#include "rtc/alarm.h"
#include "app_task.h"
#include "system/timer.h"
#include "device/device.h"
#include "key_event_deal.h"
#include "res/resfile.h"
#include "ui/style_JL.h"

//通过闹钟index选择显示的控件
#define ITEM_NUMB(x)  ALARM_NUMB_PRE_ITEM##x
#define ITEM_WEEM(x)  ALARM_TEXT_WEEK_ITEM##x
#define TIME_PAM(x)   ALARM_TEXT_AM_PM_INTM##x
#define TIME_PIC(x)   ALARM_BTN_ITEM##x
#define FATHER_ID(x)  V_ITEM_##x
#define DE_WEEK_PIC(x)   WEEK_PIC_##x

#define ALARM_PARM_SIZE    64
#define MAX_ALARM_ITEM      5


static const int ALARM_WEEK_TEXT_LIST[] = {
    ALARM_TEXT_WEEK_ITEM1,
    ALARM_TEXT_WEEK_ITEM2,
    ALARM_TEXT_WEEK_ITEM3,
    ALARM_TEXT_WEEK_ITEM4,
    ALARM_TEXT_WEEK_ITEM5,
    ALARM_TEXT_WEEK_ITEM6,
};

static const int ALARM_APM_TEXT_LIST[] = {
    ALARM_TEXT_AM_PM_INTM1,
    ALARM_TEXT_AM_PM_INTM2,
    ALARM_TEXT_AM_PM_INTM3,
    ALARM_TEXT_AM_PM_INTM4,
    ALARM_TEXT_AM_PM_INTM5,
    ALARM_TEXT_AM_PM_INTM6,
};


int get_vm_alarm_map();
extern u8 alarm_get_total(void);


extern void alarm_delete(u8 index);
extern void alarm_delete_all(void);
extern u8 get_alarm_number2table(u8 num, u8 *table);
u8 get_alarm_timer__by_item(u8 item, PT_ALARM p);

#endif
