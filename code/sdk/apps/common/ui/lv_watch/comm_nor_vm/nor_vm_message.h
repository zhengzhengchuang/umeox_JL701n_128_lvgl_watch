#ifndef __NOR_VM_MESSAGE_H__
#define __NOR_VM_MESSAGE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "../include/ui_menu.h"

#define Msg_Max_Num (10)
#define Msg_Ctx_Len (120)

enum
{
    message_type_other,
    message_type_facebook,
    message_type_vkontakte,
    message_type_instagram,
    message_type_linkedin,
    message_type_tiktok,
    message_type_whatsapp,
    message_type_wechat,
    message_type_snapchat,
    message_type_telegram,
    message_type_twitter,
    message_type_skype,
    message_type_line,
    message_type_gmail,
    message_type_sms,
    message_type_phone,

    message_type_unknown,
};
typedef u8 message_type_t;

typedef struct
{
    u16 check_code; 	

    u32 timestamp;
	u8 message_type;
    char msg_ctx[Msg_Ctx_Len + 1];//后补'\0'
}vm_message_ctx_t;
extern vm_message_ctx_t w_message;
extern vm_message_ctx_t r_message;

void VmMessageCtxClear(void);
u8 VmMessageItemNum(void);
bool VmMessageCtxByIdx(u8 idx);
void VmMessageCtxFlashSave(void *p);
void VmMessageDelByIdx(u8 index);
#ifdef __cplusplus
}
#endif

#endif
