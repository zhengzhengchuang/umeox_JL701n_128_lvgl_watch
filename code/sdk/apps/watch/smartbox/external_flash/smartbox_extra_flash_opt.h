#ifndef _SMART_BOX_EXTRA_FLASH_OPT_H_
#define _SMART_BOX_EXTRA_FLASH_OPT_H_
#include "typedef.h"
#include "system/event.h"

int smartbox_extra_flash_opt(u8 *data, u16 len, u8 OpCode, u8 OpCode_SN);
int get_extra_flash_info(void *priv, u8 *resp_data);
int smartbox_extra_flash_opt_start(void);
void smartbox_extra_flash_opt_stop(void);

// 其他地方会调用到
void smartbox_extra_flash_opt_dial_nodify(void);
void smartbox_eflash_flag_set(u8 eflash_state_type);
u8 smartbox_eflash_flag_get(void);
void smartbox_eflash_update_flag_set(u8 eflash_state_type);
u8 smartbox_eflash_update_flag_get(void);

void smartbox_extra_flash_init(void);
int smartbox_extra_flash_event_deal(struct sys_event *event);
void smartbox_extra_flash_close(void);

void smartbox_extra_flash_disconnect_tips(u32 sec);
#endif
