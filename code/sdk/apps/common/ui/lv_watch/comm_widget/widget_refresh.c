#include "../lv_watch.h"
#include "widget_refresh.h"

/*********************************************************************************
                                  常用控件元素在这里更新                                       
*********************************************************************************/
static uint16_t widget_refresh_timer_id = 0;

static void widget_refresh_timer_cb(void *priv)
{
    int widget_refresh_img[1];
    widget_refresh_img[0] = \
        ui_msg_widget_refresh;
    post_ui_msg(widget_refresh_img, 1);

    return;
}

void widget_refresh_init(void)
{
    if(widget_refresh_timer_id)
        sys_timer_del(widget_refresh_timer_id);

    widget_refresh_timer_id = 0;

    return;
}

void widget_refresh_handle(void)
{
    common_date_widget_refresh();
    common_time_widget_refresh();
    common_week_widget_refresh();

    common_data_refresh();

    return;
}

void widget_refresh_timer_create(void)
{
    if(!widget_refresh_timer_id)
        widget_refresh_timer_id = \
            sys_timer_add(NULL, widget_refresh_timer_cb, 100);

    return;
}