#include "../lv_watch.h"
#include "../../../../../include_lib/btstack/le/le_user.h"

void umeox_common_le_reply_fail(u8 *buf, u8 len)
{
    u8 reply_buf[Cmd_Pkt_Len];
    memset(reply_buf, 0x00, Cmd_Pkt_Len);

    u8 cmd_idx = 0;
    le_cmd_t cmd = buf[0];
    reply_buf[cmd_idx] = cmd|(0x80);

    u32 verify_crc = 0;
    u8 crc_idx = len - 1;
    for(u8 i = 0; i < crc_idx; i++)
        verify_crc += reply_buf[i];

    reply_buf[crc_idx] = \
        (u8)(verify_crc&(0xff));

    umeox_common_le_notify_data(reply_buf, len);
 
    return;
}

bool umeox_common_le_notify_data(u8 *buf, u8 len)
{
    int ret;

    if(buf == NULL || len == 0)
        return false;

    if(len > Cmd_Pkt_Len)
        len = Cmd_Pkt_Len;

    u8 att_service = BLE_USER_SERVICE;
    struct ble_server_operation_t *ble_server_operation;
    ble_get_server_operation_table(&ble_server_operation);
    ret = ble_server_operation->send_data(&att_service, buf, len);
    if(!ret) return true;

    printf("notify fail!!!!!!!!!!!!!!!!!!!!!\n");

    return false;
}
