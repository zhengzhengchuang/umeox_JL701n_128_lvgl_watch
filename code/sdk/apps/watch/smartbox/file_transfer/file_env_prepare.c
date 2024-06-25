#include "file_env_prepare.h"
#include "smartbox/smartbox.h"
#include "system/includes.h"
#include "common/dev_status.h"
#include "clock_cfg.h"
#include "file_operate/file_manager.h"

#if (SMART_BOX_EN && TCFG_DEV_MANAGER_ENABLE)

#define PHONE_BOOK_PATH      "storage/sd1/C/DOWNLOAD/call.txt"

void file_delete_one_file(u8 OpCode_SN, u8 *data, u16 len)
{
    FILE *file = fopen(PHONE_BOOK_PATH, "r");
    if (file) {
        fdelete(file);
    }
    JL_CMD_response_send(JL_OPCODE_ONE_FILE_DELETE, JL_PRO_STATUS_SUCCESS, OpCode_SN, NULL, 0);
}
#else
void file_delete_one_file(u8 OpCode_SN, u8 *data, u16 len)
{
}
#endif
