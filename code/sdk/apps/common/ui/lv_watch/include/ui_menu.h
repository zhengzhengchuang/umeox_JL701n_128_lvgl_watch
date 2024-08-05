#ifndef __UI_MENU_H__
#define __UI_MENU_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "ui_act_id.h"
#include "../../lvgl/lvgl.h"


#define Always_OnScreen (0x0fffffff)

enum
{
    menu_align_left,
    menu_align_right,
};
typedef uint8_t menu_align_t;

typedef void (*ui_menu_create_func_cb)(lv_obj_t *);
typedef void (*ui_menu_refresh_func_cb)(lv_obj_t *);
typedef void (*ui_menu_destory_func_cb)(lv_obj_t *);
typedef void (*ui_menu_display_func_cb)(lv_obj_t *);
typedef void (*ui_menu_key_func_cb)(lv_obj_t *, int, int);
typedef void (*ui_menu_rdec_key_func_cb)(lv_obj_t *, int);

typedef struct 
{  
    void *menu_arg;
    bool lock_flag;//当前页面是否上锁。
    bool return_flag;  //当前页面是否返回上一级的机制。
    bool disable_te; //关掉te(提高帧率)
    ui_act_id_t menu_id; 
    uint32_t user_offscreen_time;//0系统设置熄屏时间，>0用户自定义熄屏时间，Always_OnScreen常亮
    uint32_t user_refresh_time_inv;  //0默认刷新时间间隔，>0用户自定义刷新时间间隔
    ui_menu_key_func_cb key_func_cb;//页面按键回调函数
    ui_menu_create_func_cb create_func_cb;
    ui_menu_refresh_func_cb refresh_func_cb;
    ui_menu_destory_func_cb destory_func_cb;
    ui_menu_display_func_cb display_func_cb;
    ui_menu_rdec_key_func_cb rdec_func_cb;
}ui_menu_load_info_t;

extern ui_menu_load_info_t ui_menu_load_end[];
extern ui_menu_load_info_t ui_menu_load_begin[];

#define list_for_ui_menu_load(member) \
	for(member = ui_menu_load_begin; member < ui_menu_load_end; member++)

#define register_ui_menu_load_info(name) \
    const ui_menu_load_info_t name SEC_USED(.ui_menu_load)

#define extern_ui_menu_load_info(name) \
    extern const ui_menu_load_info_t name

/*多个表盘共用同个id，需要extern出来*/
extern_ui_menu_load_info(menu_load_watchface_00);
extern_ui_menu_load_info(menu_load_watchface_01);
extern_ui_menu_load_info(menu_load_watchface_02);
extern_ui_menu_load_info(menu_load_watchface_03);
extern_ui_menu_load_info(menu_load_watchface_04);
extern_ui_menu_load_info(menu_load_watchface_05);
extern_ui_menu_load_info(menu_load_watchface_06);
extern_ui_menu_load_info(menu_load_watchface_07);
extern_ui_menu_load_info(menu_load_watchface_08);
extern_ui_menu_load_info(menu_load_watchface_09);
extern_ui_menu_load_info(menu_load_watchface_10);
extern_ui_menu_load_info(menu_load_watchface_11);

/*多个菜单共用同个id，需要extern出来*/
extern_ui_menu_load_info(menu_load_style_00);
extern_ui_menu_load_info(menu_load_style_01);
extern_ui_menu_load_info(menu_load_style_02);

extern_ui_menu_load_info(menu_load_al_name_list0);
extern_ui_menu_load_info(menu_load_al_name_list1);
#ifdef __cplusplus
}
#endif

#endif
