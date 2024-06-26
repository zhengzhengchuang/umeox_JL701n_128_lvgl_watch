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

    /*外部flash数据操作*/
    ui_msg_nor_vm_hr,
    ui_msg_nor_vm_bo,
    ui_msg_nor_vm_clear,
    ui_msg_nor_vm_sleep,
    ui_msg_nor_vm_weather,
    ui_msg_nor_vm_message,
    ui_msg_nor_vm_call_log,
    ui_msg_nor_vm_contacts,
    
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
