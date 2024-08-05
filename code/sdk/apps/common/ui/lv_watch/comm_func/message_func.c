#include "../lv_watch.h"

static const char ancs_facebook[] = "com.facebook.Facebook";
static const char ancs_vkontakte[] = "com.vkontakte.android";
static const char ancs_instagram[] = "com.burbn.instagram";
static const char ancs_linkedin[] = "com.linkedin.LinkedIn";
static const char ancs_tiktok[] = "com.zhiliaoapp.musically";
static const char ancs_whatsapp[] = "net.whatsapp.WhatsApp";
static const char ancs_wechat[] = "com.tencent.xin";
static const char ancs_snapchat[] = "com.toyopagroup.picaboo";
static const char ancs_telegram[] = "ph.telegra.Telegraph";
static const char ancs_twitter[] = "com.atebits.Tweetie2";
static const char ancs_skype[] = "com.skype.skype";
static const char ancs_line[] = "jp.naver.line";
static const char ancs_gmail[] = "com.google.Gmail";
static const char ancs_sms[] = "com.apple.MobileSMS";
static const char ancs_phone[] = "com.apple.mobilephone";

#define VM_MASK (0x55af)
MsgNotifyInfoPara_t Notify_Info;

static const MsgNotifyInfoPara_t InitInfo = {
    .sw_union.info = 0xFFFFFFFF,
};

static uint8_t QueryId;
uint8_t GetQueryId(void)
{
    return QueryId;
}

void SetQueryId(uint8_t id)
{
    QueryId = id;
    return;
}

u8 GetMessageNum(void)
{
    u8 num = \
        VmMessageItemNum();

    return num;
}

bool GetVmFlashMessageCtx(u8 idx)
{
    bool ret = \
        VmMessageCtxByIdx(idx);
    
    return ret;
}

static void VmFlashMessageCtxWrite(\
    vm_message_ctx_t *w_ctx)
{
    if(w_ctx->check_code != 0)
        VmMessageCtxFlashSave(w_ctx);

    memset(w_ctx, 0, sizeof(vm_message_ctx_t));
    
    return;
}

static void MessageNotifyIsOnHandle(void)
{
    int dnd_state = \
        GetVmParaCacheByLabel(vm_label_dnd_state);
    if(dnd_state == dnd_state_enable)
        return;

    //当前菜单是否支持弹窗
    if(!MenuSupportPopup())
        return;

    u8 num = GetMessageNum();
    if(num == 0)
        return;

    //震动
    motor_run(1, def_motor_duty);

    SetQueryId(0);//最新一条索引是:0
    ui_menu_jump(ui_act_id_msg_detail);

    return;
}

void MsgNotifyProcess(void)
{
    bool BondFlag = GetDevBondFlag();
    if(BondFlag == false)
        return;

    vm_message_ctx_t *w_ctx = \
        &w_message;

    if(w_ctx->check_code == 0)
        return;

    u8 type = w_ctx->message_type;
    
    /*暂时屏蔽掉Ble通话*/
    if(type == message_type_phone)
        return;

    if(type >= message_type_unknown)
        return;

    /*消息推送开关*/
    u32 info = Notify_Info.sw_union.info;
    if(!((info >> type) & 0x1)) return;

    VmFlashMessageCtxWrite(w_ctx);

    MessageNotifyIsOnHandle();

    return;
}

void MsgNotifyFromAncs(void *name, void *data, u16 len)
{
    static u16 total_len = 0;

    const u16 max_len = Msg_Ctx_Len;
    u8 *type = &(w_message.message_type);
    char *w_ctx = w_message.msg_ctx;
    u16 *check_code = &(w_message.check_code);

    if(strcmp((char *)name, "AppIdentifier") == 0) 
    {
        memset(w_ctx, 0, max_len + 1);

        if(strcmp((char *)data, ancs_facebook) == 0)
            *type  = message_type_facebook;
        else if(strcmp((char *)data, ancs_vkontakte) == 0)
            *type  = message_type_vkontakte;
        else if(strcmp((char *)data, ancs_instagram) == 0)
            *type  = message_type_instagram;
        else if(strcmp((char *)data, ancs_linkedin) == 0)
            *type  = message_type_linkedin;
        else if(strcmp((char *)data, ancs_tiktok) == 0)
            *type  = message_type_tiktok;
        else if(strcmp((char *)data, ancs_whatsapp) == 0)
            *type  = message_type_whatsapp;
        else if(strcmp((char *)data, ancs_wechat) == 0)
            *type  = message_type_wechat;
        else if(strcmp((char *)data, ancs_snapchat) == 0)
            *type  = message_type_snapchat;
        else if(strcmp((char *)data, ancs_telegram) == 0)
            *type  = message_type_telegram;
        else if(strcmp((char *)data, ancs_twitter) == 0)
            *type  = message_type_twitter;
        else if(strcmp((char *)data, ancs_skype) == 0)
            *type  = message_type_skype;
        else if(strcmp((char *)data, ancs_line) == 0)
            *type  = message_type_line;
        else if(strcmp((char *)data, ancs_gmail) == 0)
            *type  = message_type_gmail;
        else if(strcmp((char *)data, ancs_sms) == 0)
            *type  = message_type_sms;
        else if(strcmp((char *)data, ancs_phone) == 0)
            *type  = message_type_phone;
        else
            *type  = message_type_other;
    }else if(strcmp((char *)name, "IDTitle") == 0) 
    {
        total_len = 0;

        u16 len_tmp = len;
        if(len_tmp <= max_len - 1)
            memcpy(w_ctx, data, len_tmp);
        else
        {
            len_tmp = max_len - 1;
            memcpy(w_ctx, data, len_tmp);
        }
        w_ctx[len_tmp] = ':';
        total_len += (len_tmp + 1);
    }else if(strcmp((char *)name, "IDMessage") == 0)
    {
        u16 len_tmp = len;
        if(total_len + len_tmp <= max_len)
            memcpy(&w_ctx[total_len], data, len_tmp);
        else
        {
            len_tmp = max_len - total_len;
            if(len_tmp > 0)
                memcpy(&w_ctx[total_len], data, len_tmp);
        }

        struct sys_time time;
        GetUtcTime(&time);
        w_message.timestamp = UtcTimeToSec(&time);
        *check_code = Nor_Vm_Check_Code;

        int ui_msg_post[1];
        ui_msg_post[0] = ui_msg_nor_message_write;
        post_ui_msg(ui_msg_post, 1);
    }

    return;
}

void MsgNotifyInfoParaRead(void)
{
    int vm_op_len = \
        sizeof(MsgNotifyInfoPara_t);

    int ret = syscfg_read(CFG_MSG_NOTIFY_PARA_INFO, \
        &Notify_Info, vm_op_len);
    if(ret != vm_op_len || Notify_Info.vm_mask != VM_MASK)
        MsgNotifyInfoParaReset();
    
    return;
}

void MsgNotifyInfoParaWrite(void)
{
    int vm_op_len = \
        sizeof(MsgNotifyInfoPara_t);
    
    for(u8 i = 0; i < 3; i++)
    {
        int ret = syscfg_write(CFG_MSG_NOTIFY_PARA_INFO, \
            &Notify_Info, vm_op_len);
        if(ret == vm_op_len)
            break;
    }

    return;
}

void MsgNotifyInfoParaReset(void)
{
    int vm_op_len = \
        sizeof(MsgNotifyInfoPara_t);

    memcpy(&Notify_Info, &InitInfo, vm_op_len);

    Notify_Info.vm_mask = VM_MASK;

    MsgNotifyInfoParaUpdate();

    return;
}

void MsgNotifyInfoParaUpdate(void)
{
    MsgNotifyInfoParaWrite();

    return;
}

