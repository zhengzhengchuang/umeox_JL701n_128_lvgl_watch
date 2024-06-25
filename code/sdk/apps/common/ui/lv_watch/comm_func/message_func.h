#ifndef __MESSAGE_FUNC_H__
#define __MESSAGE_FUNC_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "../include/ui_menu.h"
#include "../comm_nor_vm/nor_vm_message.h"

typedef struct 
{
    uint32_t other       :1;
    uint32_t facebook    :1;
    uint32_t vkontakte   :1;
    uint32_t instagram   :1;
    uint32_t linkedin    :1;
    uint32_t tiktok      :1;
    uint32_t whatsapp    :1;
    uint32_t wechat      :1;
    
    uint32_t snapchat    :1;
    uint32_t telegram    :1;
    uint32_t twitter     :1;
    uint32_t skype       :1;
    uint32_t line        :1;
    uint32_t gmail       :1;
    uint32_t sms         :1;
    uint32_t phone       :1;

    uint32_t reserved    :16;
}ancs_bit_field_t;

typedef union
{
    uint32_t info;
    ancs_bit_field_t bit_field;
}ancs_sw_union_t;

typedef struct
{
    u16 vm_mask;

    ancs_sw_union_t sw_union;
}MsgNotifyInfoPara_t;
extern MsgNotifyInfoPara_t Notify_Info;

uint8_t GetQueryId(void);
void SetQueryId(uint8_t id);

u8 GetMessageNum(void);
void MsgNotifyProcess(void);
bool GetVmFlashMessageCtx(u8 idx);
void MsgNotifyFromAncs(void *name, void *data, u16 len);

void MsgNotifyInfoParaRead(void);
void MsgNotifyInfoParaWrite(void);
void MsgNotifyInfoParaReset(void);
void MsgNotifyInfoParaUpdate(void);
#ifdef __cplusplus
} /* extern "C" */
#endif

#endif 
