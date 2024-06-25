#ifndef _SMART_BOX_EXTRA_FLASH_CMD_H_
#define _SMART_BOX_EXTRA_FLASH_CMD_H_
#include "typedef.h"
#include "system/event.h"

int JL_smartbox_extra_flash_cmd_resp(void *priv, u8 OpCode, u8 OpCode_SN, u8 *data, u16 len);
int JL_smartbox_extra_flash_cmd_no_resp(void *priv, u8 OpCode, u8 *data, u16 len);
u8 JL_smartbox_extra_flash_info_notify(u8 *buf, u16 len);

int smartbox_extra_flash_opt_resp(u8 dire, u8 OpCode, u8 OpCode_SN, u8 *resp_data, u16 data_len);

#endif
