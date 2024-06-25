#include "app_config.h"
#include "ui/ui_style.h"
#include "ui/ui.h"
#include "ui/ui_api.h"
#include "ui/res_config.h"

#include "app_task.h"
#include "system/timer.h"
#include "key_event_deal.h"
#include "device/device.h"


#define LOG_TAG_CONST       UI
#define LOG_TAG     		"[UI-ACTION]"
#define LOG_ERROR_ENABLE
#define LOG_DEBUG_ENABLE
#define LOG_INFO_ENABLE
/* #define LOG_DUMP_ENABLE */
#define LOG_CLI_ENABLE
#include "debug.h"



#if TCFG_UI_ENABLE && (!TCFG_LUA_ENABLE)
#if TCFG_UI_ENABLE_FINDPHONE

#define STYLE_NAME  JL

extern u8 *get_cur_connect_phone_mac_addr(void);
extern u8 get_rcsp_connect_status(void);

static u16 timer = 0;

static u8 number = 0;
static u8 counter = 0;

static void refresh()
{
    if (!timer) {
        return;
    }
    ui_pic_show_image_by_id(FIND_PHONE_PIC, counter++);
    if (counter >= number) {
        counter = 1;
    }

}

static int FIND_PHONE_LAYOUT_onchane(void *ctr, enum element_change_event e, void *arg)
{
    switch (e) {
    case ON_CHANGE_INIT:
        ui_auto_shut_down_disable();
        break;
    case ON_CHANGE_RELEASE:
        ui_auto_shut_down_enable();
        if (timer) {
            sys_timer_del(timer);
            timer = 0;
        }
#if RCSP_ADV_FIND_DEVICE_ENABLE
        extern void smartbox_stop_find_device(void *priv);
        smartbox_stop_find_device(NULL);
        extern void smartbox_find_phone_reset(void);
        smartbox_find_phone_reset();

#endif
        break;
    case ON_CHANGE_FIRST_SHOW:
#if RCSP_ADV_FIND_DEVICE_ENABLE
        if (get_cur_connect_phone_mac_addr() && get_rcsp_connect_status())
#endif
        {

#if RCSP_ADV_FIND_DEVICE_ENABLE
            extern void smartbox_find_device(void);
            smartbox_find_device();
#endif


            ui_text_set_index((struct ui_text *)ui_core_get_element_by_id(FIND_PHONE_TEXT), 1);
            if (!timer) {
                ui_pic_set_image_index((struct ui_pic *)ui_core_get_element_by_id(FIND_PHONE_PIC), 1);
                number = ui_pic_get_normal_image_number_by_id(FIND_PHONE_PIC);
                timer = sys_timer_add(NULL, refresh, 200);
                counter = 1;
            }
        }

        break;
    default:
        return false;
    }
    return false;
}


REGISTER_UI_EVENT_HANDLER(FIND_PHONE_LAYOUT)
.ontouch = NULL,
 .onkey = NULL,
  .onchange  = FIND_PHONE_LAYOUT_onchane,
};


#endif /* #if TCFG_UI_ENABLE_FINDPHONE */
#endif /* #if (!TCFG_LUA_ENABLE) */


