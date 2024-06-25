#include "app_config.h"
#include "timer.h"

#if (TCFG_UI_ENABLE &&TCFG_SPI_LCD_ENABLE && (!TCFG_SIMPLE_LCD_ENABLE))

#include "ui/res_config.h"
#include "ui/ui_api.h"

static int standby_before_watch_style = -1;
static int auto_goto_dial_timer;
static int auto_goto_dial_enable = 1;
static u8 screen_saver_status = 0;

extern int standby_watch_get_style();

__attribute__((weak)) u8 get_standby_watch_en()
{
    return 0;
}

__attribute__((weak)) void set_bright_screen_on_off(u8 on_off)
{

}

u8 get_screen_saver_status()
{
    return screen_saver_status;
}

void set_screen_saver_status(u8 status)
{
    screen_saver_status = status;
}

void auto_goto_dial_timer_del()
{
    if (auto_goto_dial_timer) {
        sys_timeout_del(auto_goto_dial_timer);
        auto_goto_dial_timer = 0;
    }
}

static void standby_watch_show()
{
    if (!get_ui_init_status()) {
        return;
    }

    set_ui_open_flag(0);

    if (!get_screen_saver_status()) {
        standby_before_watch_style = watch_get_style();
        if (true != watch_set_style(standby_watch_get_style())) {
            printf("standby_watch_show set watch style fail");
        }
#if UI_WATCH_RES_ENABLE
        UI_HIDE_CURR_WINDOW();//关闭当前页面
#endif
        UI_SHOW_WINDOW(ID_WINDOW_BT);
        ui_auto_shut_down_disable();
        ui_touch_timer_delete();
    }
}

static void standby_watch_hide(u8 recover_cur_page)
{
    if (!get_ui_init_status()) {
        return;
    }

    if (get_ui_open_flag()) {
        return;
    }

    if (get_screen_saver_status()) {
        if (true != watch_set_style(standby_before_watch_style)) {
            printf("standby_watch_hide set watch style fail");
        }

        set_ui_open_flag(1);

        ui_auto_shut_down_enable();
#if UI_WATCH_RES_ENABLE
        if (recover_cur_page) {
            UI_SHOW_WINDOW(0);//恢复当前ui
        }
#endif
        ui_touch_timer_start();
    }
}

void auto_goto_dial_page(void *p)
{
    u8 i;
    u32 *mem_id = get_ui_mem_id();

    if (auto_goto_dial_timer) {
        auto_goto_dial_timer = 0;
        if (mem_id[get_ui_mem_id_size() - 1] != ID_WINDOW_BT) {
            for (i = 1; i <=  get_ui_mem_id_size() - 1; i++) {
                mem_id[i - 1] = mem_id[i];
            }
            mem_id[get_ui_mem_id_size() - 1] = ID_WINDOW_BT;
        }
    }
}


extern int ui_auto_shut_down_timer ;
void ui_screen_saver(void *p)
{
    if (!ui_auto_shut_down_timer) {
        return;
    }

    if (get_standby_watch_en()) {
        standby_watch_show();
    } else {
        ui_backlight_close();
    }
    if (auto_goto_dial_enable) {
        if (auto_goto_dial_timer == 0) {
            auto_goto_dial_timer = sys_timeout_add(NULL, auto_goto_dial_page, 5000);
        }
    }
}

void ui_auto_goto_dial_enable(void)
{
    auto_goto_dial_enable = 1;
}

void ui_auto_goto_dial_disable(void)
{
    auto_goto_dial_enable = 0;
}
//退出屏保
void ui_screen_recover(u8 recover_cur_page)
{
    set_bright_screen_on_off(0);
    if (get_standby_watch_en()) {
        standby_watch_hide(recover_cur_page);
    } else {
        ui_backlight_open(recover_cur_page);
    }
}

#else

void ui_auto_goto_dial_disable(void)
{

}


void ui_auto_goto_dial_enable(void)
{

}
#endif
