#ifndef __LVGL_MAIN_H__
#define __LVGL_MAIN_H__

#ifdef __cplusplus
extern "C" {
#endif

#define UI_TASK_NAME "ui"

/*自定义UI资源起始地址*/
#define RES_BASE_ADDR (0x4FE000)
#define EX_RES_BASE_ADDR (0x000000)

enum
{
    /*ui相关操作*/
    ui_msg_menu_jump = 0x00,
    ui_msg_menu_refresh,
    ui_msg_menu_offscreen,
    ui_msg_clp_refresh,
    ui_msg_widget_refresh,
    ui_msg_key_handle,
    ui_msg_rdec_handle,

    /*nor flash数据写*/
    ui_msg_nor_hr_wirte,
    ui_msg_nor_bo_write,
    ui_msg_nor_pedo_write,
    ui_msg_nor_sleep_write,
    ui_msg_nor_weather_write,
    ui_msg_nor_message_write,
    ui_msg_nor_contacts_write,
    ui_msg_nor_call_log_write,

    /*nor flash数据擦除*/
    ui_msg_nor_data_clear,
    
    /*nor flash数据读*/
    //ui_msg_nor_pedo_send,
    
    /*设备绑定操作*/
    ui_msg_unbond_handle,
    ui_msg_ori_bond_handle,
    ui_msg_new_bond_handle,
    ui_msg_first_bond_handle,
};

int lvgl_test_init(void *param);
int post_ui_msg(int *msg, u8 len);
void ui_msg_handle(int *msg, u8 len);
void ui_key_msg_post(int key_value, int key_event);
#ifdef __cplusplus
}
#endif

#endif
