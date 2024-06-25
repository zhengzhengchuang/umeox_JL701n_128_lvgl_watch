#ifndef _UI_API_H_
#define _UI_API_H_

#include "app_config.h"
#include "ui/lcd_spi/lcd_drive.h"
#include "ui/ui.h"
#include "ui/ui_style.h"


#define GRT_CUR_MENU    (0)
#define GET_MAIN_MENU   (1)

enum ui_devices_type {
    LED_7,
    LCD_SEG3X9,
    TFT_LCD,//彩屏
    DOT_LCD,//点阵屏
};

//板级配置数据结构
struct ui_devices_cfg {
    enum ui_devices_type type;
    void *private_data;
};


struct touch_event {
    int event;
    int x;
    int y;
    int has_energy;
};


//led7 api
#if (TCFG_UI_ENABLE&&(CONFIG_UI_STYLE == STYLE_JL_LED7))
extern int  led7_ui_init(const struct ui_devices_cfg *ui_cfg);
extern void ui_set_main_menu(enum ui_menu_main menu);
extern void ui_menu_reflash(u8 break_in);//break_in 是否打断显示,例如显示设置过程中需要刷新新界面。是是否打断设置界面显示
extern u8   ui_get_app_menu(u8);
extern void ui_set_led(u8 app_menu, u8 on, u8 phase, u16 highlight, u16 period);
extern void ui_set_auto_reflash(u32 msec);//自动刷新主页
extern void ui_close_main_menu();
extern void ui_set_tmp_menu(u8 app_menu, u16 ret_time, s32 arg, void (*timeout_cb)(u8 menu));
extern void ui_common(void *hd, void *private, u8 menu, u32 arg);//公共显示
#endif // #if (TCFG_UI_ENABLE&&(CONFIG_UI_STYLE == STYLE_JL_LED7))


//lcd api
//
extern int lcd_ui_init(void *arg);
extern int ui_hide_main(int id);
extern int ui_show_main(int id);

extern u8 get_screen_saver_status();
extern void ui_screen_recover(u8 recover_cur_page);
extern void ui_screen_saver(void *p);

extern u8 get_return_index();
extern u8 ui_return_page_pop(u8 dir);
extern int ui_return_page_pop_spec(u32 page_id); // 跳转到指定页面，清除中间的page记录
extern u8 ui_return_page_sub();
extern u32 ui_return_page_id();
extern void ui_return_page_push(int page_id);
extern void ui_return_page_clear();
extern void ui_return_page_del(int page_id);
/*
   int (*entry)(int); // 返回true，entry回调里面自定义处理；返回false或者回调为NULL，默认显示page
   int (*exit)(int); // 返回true，exit回调里面自定义处理；返回false或者回调为NULL，默认清除记录
 */
extern int ui_preemption_page_push(int page_id, int (*entry)(int), int (*exit)(int), u8 priority);
extern int ui_preemption_page_pop(int page_id);
extern int ui_preemption_page_del(int page_id);
extern int ui_preemption_page_check(void);

extern int ui_auto_sw_page_effect_post(int curr_win, int next_win, u16 effect_mode, void *user_effect, void *effect_priv);


typedef enum {
    SLIDE_MODE_OFF,
    SLIDE_MODE_NOT_RIGHT_ALL,
    SLIDE_MODE_NOT_RIGHT_FIRST,
    SLIDE_MODE_NOT_LOOP,
} CARD_SLIDE_MODE;
#define SIDEBAR_LEFT_IN_FIRST_PAGE 1
#define SIDEBAR_LEFT_IN_ALL_PAGE 2
extern int ui_page_manager_mode_set(u8 mode);
extern int ui_page_manager_mode_get(void);

extern void ui_card_enable(void);
extern void ui_card_disable(void);
extern int ui_show_window_slide(u32 page_id, int mode, int step);

extern int ui_server_msg_post(const char *msg, ...);
extern int ui_hide_curr_main();
extern int ui_touch_msg_post(struct touch_event *event);
extern int ui_touch_msg_post_withcallback(struct touch_event *event, void (*cb)(u8 finish));
//extern int ui_key_msg_post(int msg);
extern void key_ui_takeover(u8 on);
extern int key_is_ui_takeover();

extern void ui_touch_timer_delete();
extern void ui_touch_timer_start();
extern void ui_auto_shut_down_modify(void);
extern void ui_auto_shut_down_re_run(void);
extern void ui_auto_shut_down_enable(void);
extern u8 ui_auto_shut_down_disable(void);
extern void ui_set_shut_down_time(u16 time);
extern int ui_get_shut_down_time();
extern void ui_set_shut_down_time_ms(u32 time_ms);
extern int ui_get_shut_down_time_ms();


extern u32 *get_ui_mem_id();
extern u32  get_ui_mem_id_size();
extern void set_ui_open_flag(u8 flag);
extern u8 get_ui_open_flag();
extern u8 get_ui_init_status();
extern void ui_backlight_close(void);
extern void ui_backlight_open(u8 recover_cur_page);

extern int watch_get_style();
extern int watch_set_style(int style);

extern void ui_auto_goto_dial_enable(void);
extern void ui_auto_goto_dial_disable(void);

extern void ui_send_event(u16 event, u32 val);


struct lcd_sleep_headler {
    char *name;
    void (*enter)(void);
    void (*exit)(void);
};

#define REGISTER_LCD_SLEEP_HEADLER(target) \
        const struct lcd_sleep_headler target sec(.lcd_sleep_headler)

extern const struct lcd_sleep_headler lcd_sleep_ctrl_headler_begin[];
extern const struct lcd_sleep_headler lcd_sleep_ctrl_headler_end[];

#define list_for_each_lcd_sleep_headler(p) \
    for (p = lcd_sleep_ctrl_headler_begin; p < lcd_sleep_ctrl_headler_end; p++)


//=================================================================================//
//                        			UI API                    			   		   //
//=================================================================================//
#if (TCFG_UI_ENABLE)
#if(CONFIG_UI_STYLE == STYLE_JL_LED7)//led7 显示

#define UI_INIT(a)             led7_ui_init(a)
#define UI_SHOW_WINDOW(a)      ui_set_main_menu(a)
#define UI_HIDE_WINDOW(...)    ui_close_main_menu()
#define UI_HIDE_CURR_WINDOW()  ui_close_main_menu()
#define UI_GET_WINDOW_ID()     ui_get_app_menu(GET_MAIN_MENU)
#define UI_GET_CURR_MENU()     ui_get_app_menu(GRT_CUR_MENU)
#define UI_REFLASH_WINDOW(a)   ui_menu_reflash(a)
#define UI_SHOW_MENU           ui_set_tmp_menu

#define UI_MSG_POST(...)

#else

#define UI_INIT(a)            lcd_ui_init(a)
#define UI_SHOW_WINDOW(a)     ui_show_main(a)
#define UI_HIDE_WINDOW(a)     ui_hide_main(a)
#define UI_HIDE_CURR_WINDOW() ui_hide_curr_main()
#define UI_GET_WINDOW_ID()    ui_get_current_window_id()
#define UI_MSG_POST           ui_server_msg_post


#define UI_SHOW_MENU(...)
#define UI_GET_CURR_MENU()
#define UI_REFLASH_WINDOW(a)
#define UI_MOTO_RUN(a)       ui_moto_run(a)

#define UI_WINDOW_BACK_PUSH(a)			ui_return_page_push(a)
#define UI_WINDOW_BACK_SHOW(a)			ui_return_page_pop(a)
#define UI_WINDOW_BACK_SPEC_SHOW(a)		ui_return_page_pop_spec(a)
#define UI_WINDOW_BACK_CLEAN()  		ui_return_page_clear()
#define UI_WINDOW_BACK_SUB()  			ui_return_page_sub()
#define UI_WINDOW_BACK_INDEX()  		get_return_index()
#define UI_WINDOW_BACK_DEL(a)			ui_return_page_del(a)

#define UI_WINDOW_PREEMPTION_POSH(a,b,c,d) 		ui_preemption_page_push(a,b,c,d)
#define UI_WINDOW_PREEMPTION_POP(a) 			ui_preemption_page_pop(a)
#define UI_WINDOW_PREEMPTION_DEL(a) 			ui_preemption_page_del(a)
#define UI_WINDOW_PREEMPTION_CHECK() 			ui_preemption_page_check()

#endif

#else

//common api  lcd屏和led7 通用api
#define UI_INIT(...)
#define UI_SHOW_WINDOW(...)
#define UI_HIDE_WINDOW(...)
#define UI_GET_WINDOW_ID()
#define UI_HIDE_CURR_WINDOW()
#define UI_SHOW_MENU(...)
#define UI_MSG_POST(...)
#define UI_REFLASH_WINDOW(a)
#define UI_MOTO_RUN(...)

#define UI_WINDOW_BACK_PUSH(a)
#define UI_WINDOW_BACK_SHOW(a)
#define UI_WINDOW_BACK_SPEC_SHOW(a)
#define UI_WINDOW_BACK_CLEAN()
#define UI_WINDOW_BACK_SUB()
#define UI_WINDOW_BACK_INDEX()
#define UI_WINDOW_BACK_DEL(a)

#define UI_WINDOW_PREEMPTION_POSH(a,b,c,d)
#define UI_WINDOW_PREEMPTION_DEL(a)
#define UI_WINDOW_PREEMPTION_POP(a)

#define UI_WINDOW_PREEMPTION_CHECK()		(false)

#endif /* #if TCFG_UI_ENABLE */

// UI抢占显示，最大PAGE_PREEMPTION_MAX-1
#define UI_WINDOW_PREEMPTION_TYPE_TIMER_COUNTDOWN	1 // 计时器
#define UI_WINDOW_PREEMPTION_TYPE_ALARM				2 // 闹钟
#define UI_WINDOW_PREEMPTION_TYPE_PHONE				3 // 电话
#define UI_WINDOW_PREEMPTION_TYPE_CHARGE			3 // 充电
#define UI_WINDOW_PREEMPTION_TYPE_SPORT				4 // 锻炼
#define UI_WINDOW_PREEMPTION_TYPE_DETECTION			4 // 久坐提醒
#define UI_WINDOW_PREEMPTION_TYPE_FALL				4 // 跌倒检测
#define UI_WINDOW_PREEMPTION_TYPE_UPGRADE			5 // 升级
#define UI_WINDOW_PREEMPTION_TYPE_POWEROFF			6 // 关机


#endif
