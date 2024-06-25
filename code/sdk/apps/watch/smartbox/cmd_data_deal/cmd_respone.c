#include "smartbox/config.h"
#include "smartbox/event.h"
#include "smartbox_update.h"
#include "file_transfer.h"
#if	(defined CONFIG_DEBUG_RECORD_ENABLE && CONFIG_DEBUG_RECORD_ENABLE)
#include "asm/debug_record.h"
#endif

///>>>>>>>>>>>>>>>设备接收到上报给APP端命令的回复
void cmd_respone(void *priv, u8 OpCode, u8 status, u8 *data, u16 len)
{
    printf("cmd_respone:%x\n", OpCode);
#if RCSP_UPDATE_EN
    if (0 == JL_rcsp_update_cmd_receive_resp(priv, OpCode, status, data, len)) {
        return;
    }
#endif

    switch (OpCode) {
    case JL_OPCODE_FILE_TRANSFER_END:
        file_transfer_download_end(status, data, len);
        break;
    case JL_OPCODE_FILE_TRANSFER_CANCEL:
        file_transfer_download_active_cancel_response(status, data, len);
        break;
    case JL_OPCODE_FILE_RENAME:
        file_transfer_file_rename(status, data, len);
        break;
    case JL_OPCODE_REQUEST_EXCEPTION_INFO:
#if	(defined CONFIG_DEBUG_RECORD_ENABLE && CONFIG_DEBUG_RECORD_ENABLE)
        if (status == JL_PRO_STATUS_SUCCESS) {
            user_debug_record_info_clear();
        }
#endif
        break;
    case JL_OPCODE_MASS_DATA:
        extern void mass_data_respone(void *priv, u8 OpCode, u8 status, u8 * data, u16 len);
        mass_data_respone(priv, OpCode, status, data, len);
        break;
    default:
        break;
    }
}


