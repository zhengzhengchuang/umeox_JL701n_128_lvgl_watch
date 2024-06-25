#include "smartbox/config.h"
#include "smartbox/command.h"
#include "btstack/avctp_user.h"
#include "smartbox/event.h"
#include "system/timer.h"
#include "le_smartbox_module.h"
#include "smartbox_rcsp_manage.h"

#if (SMART_BOX_EN)

void smartbox_command_send_bt_scan_result(char *name, u8 name_len, u8 *addr, u32 dev_class, char rssi)
{
    u8 send_len = sizeof(dev_class) + 6 + sizeof(rssi) + sizeof(name_len) + name_len;
    u8 *send_buf = (u8 *)zalloc(send_len);
    if (send_buf == NULL) {
        return ;
    }
    WRITE_BIG_U32(send_buf, dev_class);
    memcpy(send_buf + sizeof(dev_class), addr, 6);
    memcpy(send_buf + sizeof(dev_class) + 6, &rssi, 1);
    memcpy(send_buf + sizeof(dev_class) + 6 + sizeof(rssi), &name_len, 1);
    memcpy(send_buf + sizeof(dev_class) + 6 + sizeof(rssi) + sizeof(name_len), name, name_len);
    JL_CMD_send(JL_OPCODE_SYS_UPDATE_BT_STATUS, send_buf, send_len, JL_NOT_NEED_RESPOND);
    free(send_buf);
    printf("bt name = %s\n", name);
}

void smartbox_command_send_conncecting_bt_status(u8 *addr, u8 status)
{
    u8 send_buf[7] = {0};
    send_buf[0] = status;
    memcpy(send_buf, addr, 6);
    JL_CMD_send(JL_OPCODE_SYS_EMITTER_BT_CONNECT_STATUS, send_buf, sizeof(send_buf), JL_NOT_NEED_RESPOND);
}


#if RCSP_ADV_FIND_DEVICE_ENABLE
static u16 find_device_timer = 0;
static u8 find_device_key_flag = 0;
static void smartbox_command_send_find_device(void *priv)
{
    struct smartbox *smart = (struct smartbox *) priv;
    if (smart == NULL) {
        return ;
    }
    if (!smart->find_dev_en) {
        return;
    }
    //				 查找手机	播放铃声	超时时间(默认10s)
    u8 send_buf[4] = {0x00, 	0x01, 		0x00, 0x0A};
    JL_CMD_send(JL_OPCODE_SYS_FIND_DEVICE, send_buf, sizeof(send_buf), JL_NOT_NEED_RESPOND);
}

u8 smartbox_find_device_key_flag_get(void)
{
    return find_device_key_flag;
}

void smartbox_send_find_device_stop(void)
{
    struct smartbox *smart = smartbox_handle_get();
    if (smart == NULL || 0 == get_rcsp_connect_status()) {
        return ;
    }
    if (!smart->find_dev_en) {
        return ;
    }
    //				 查找手机	关闭铃声	超时时间(不限制)
    u8 send_buf[4] = {0x00, 	0x00, 		0x00, 0x00};
    JL_CMD_send(JL_OPCODE_SYS_FIND_DEVICE, send_buf, sizeof(send_buf), JL_NOT_NEED_RESPOND);
}

void smartbox_find_device_reset(void)
{
    find_device_key_flag = 0;
    if (find_device_timer) {
        sys_timeout_del(find_device_timer);
        find_device_timer = 0;
    }
}

void smartbox_stop_find_device(void *priv)
{
    if (!find_device_key_flag) {
        return ;
    }
    find_device_key_flag = 0;
    smartbox_find_device_reset();
    smartbox_send_find_device_stop();

    extern void smartbox_set_vol_for_find_device(u8 vol_flag);
    smartbox_set_vol_for_find_device(find_device_key_flag);
    if (!(get_curr_channel_state() & A2DP_CH)) {
        puts("start to connect a2dp ");
        user_send_cmd_prepare(USER_CTRL_CONN_A2DP, 0, NULL);
    }
}

void find_device_timeout_handle(u32 sec)
{
    find_device_key_flag = 1;
    if (sec && (0 == find_device_timer)) {
        find_device_timer = sys_timeout_add(NULL, smartbox_stop_find_device, sec * 1000);
    }

    extern void smartbox_set_vol_for_find_device(u8 vol_flag);
    smartbox_set_vol_for_find_device(find_device_key_flag);
}

void smartbox_find_device(void *priv)
{
    struct smartbox *smart = smartbox_handle_get();
    if (smart == NULL || 0 == get_rcsp_connect_status()) {
        return ;
    }
    if (!smart->find_dev_en) {
        return ;
    }

    if (find_device_key_flag) {
        smartbox_stop_find_device(NULL);
    } else {
        find_device_key_flag = 1;
        /* if ((1 == smart->A_platform) && (0 != get_curr_channel_state())) { */
        /*     // IOS平台 */
        /*     user_send_cmd_prepare(USER_CTRL_DISCONNECTION_HCI, 0, NULL); */
        /* } */
        if (get_curr_channel_state() & A2DP_CH) {
            puts("start to disconnect a2dp ");
            user_send_cmd_prepare(USER_CTRL_DISCONN_A2DP, 0, NULL);
        }
        smartbox_command_send_find_device(smart);
    }
    extern void smart_adv_find_phone_handler(void);
    smart_adv_find_phone_handler();
}




int find_phone_timer_flag = 0;

void smartbox_find_phone_reset(void)
{
    r_printf("%s\n", __func__);
    if (find_phone_timer_flag) {
        sys_timeout_del(find_phone_timer_flag);
        find_phone_timer_flag = 0;
    }
}

void smart_adv_find_phone_handler(void)
{

    r_printf("%s\n", __func__);
    smartbox_find_phone_reset();
    if (!find_phone_timer_flag) {
        r_printf("time scan play find phone music\n");
        find_phone_timer_flag = sys_timeout_add(NULL, smartbox_find_device, 10 * 1000);
    }

}
#endif


#endif//SMART_BOX_EN


