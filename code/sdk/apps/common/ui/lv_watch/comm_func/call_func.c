#include "app_main.h"
#include "../lv_watch.h"
#include "../../../../../include_lib/btstack/avctp_user.h"

extern BT_USER_PRIV_VAR bt_user_priv_var;

static char name_cache[Contacts_Max_Num][Call_Name_Max_Len];
static char number_cache[Contacts_Max_Num][Call_Number_Max_Len];

char *GetContactsName(u8 idx)
{
    if(idx >= Contacts_Max_Num)
        return NULL;

    return name_cache[idx];
}

char *GetContactsNumber(u8 idx)
{
    if(idx >= Contacts_Max_Num)
        return NULL;

    return number_cache[idx];
}

void CtrlCallOutByNum(char *num, u8 len)
{
    if(num == NULL || len == 0)
        return;

    uint8_t ble_bt_connect = \
        GetDevBleBtConnectStatus();
    if(ble_bt_connect == 0 || \
        ble_bt_connect == 1)
    {
        ui_menu_jump(ui_act_id_call_disc);
        
        return;
    }

    user_send_cmd_prepare(USER_CTRL_DIAL_NUMBER, \
        len, (uint8_t *)num);

    return;
}

char *GetCallNumName(void)
{
    static char *call_name;
    static uint8_t *call_num;

    uint8_t len = \
        bt_user_priv_var.income_phone_len;

    if(len == 0)
        return NULL;

    call_num = \
        bt_user_priv_var.income_phone_num;

    call_name = \
        GetContactsNameByNumber((char *)call_num);

    if(call_name) 
        return call_name;

    return call_num;
}

void CtrlCallAnswer(void)
{
    user_send_cmd_prepare(USER_CTRL_HFP_CALL_ANSWER, \
        0, NULL);

    return;
}

void CtrlCallHangUp(void)
{
    user_send_cmd_prepare(USER_CTRL_HFP_CALL_HANGUP, \
        0, NULL);

    return;
}

static u8 OutOrIn = 0;//0:未知 1:拨出 2:拨入
u8 GetCallOutOrIn(void)
{
    return OutOrIn;
}
void SetCallOutOrIn(u8 val)
{
    OutOrIn = val;
    return;
}

static bool AnswerState = false;
bool GetCallAnswerState(void)
{
    return AnswerState;
}
void SetCallAnswerState(bool state)
{
    AnswerState = state;
    return;
}

void ClearThisCallProcess(void)
{
    SetCallOutOrIn(0);
    SetCallAnswerState(false);
}

void UpdateCallLogVmFlash(void)
{
    u8 OutOrIn = \
        GetCallOutOrIn();
    if(OutOrIn == 0)
        return;
    
    bool AnswerState = \
       GetCallAnswerState(); 

    u8 state = call_log_unknown;
    if(OutOrIn == 1)
    {
        if(AnswerState == true)
            state = call_log_state_out;
        else
            state = call_log_state_hangup;
    }else if(OutOrIn == 2)
    {
        if(AnswerState == true)
            state = call_log_state_in;
        else
            state = call_log_state_hangup;
    }

    u8 *call_state = &(w_call_log.state);
    u16 *check_code = &(w_call_log.check_code);
    struct sys_time *time = &(w_call_log.time);
    char *name_str = w_call_log.name_str;
    char *number_str = w_call_log.number_str;

    *call_state = state;
    *check_code = Nor_Vm_Check_Code;
    

    u8 len = \
        bt_user_priv_var.income_phone_len;
    u8 *call_num = \
        bt_user_priv_var.income_phone_num;
    if(len == 0)
        return;
    
    memcpy(number_str, call_num, len);

    char *call_name = \
        GetContactsNameByNumber((char *)call_num);
    if(call_name)
        memcpy(name_str, call_name, Call_Name_Max_Len);
    GetUtcTime(time);

    int ui_msg_post[1];
    ui_msg_post[0] = \
        ui_msg_nor_call_log_write;
    post_ui_msg(ui_msg_post, 1);

    return;
}

void VmCallLogCtxFlashWrite(void)
{
    if(w_call_log.check_code != 0)
        VmCallLogCtxFlashSave(&w_call_log);

    memset(&w_call_log, 0, sizeof(vm_call_log_ctx_t));

    printf("call update falsh success\n");

    return;
}

static char *contacts_name;
char *GetContactsNameByNumber(char *number)
{
    contacts_name = NULL;

    if(!number) 
        return contacts_name;

    uint8_t num = \
        VmContactsItemNum();

    if(num == 0)
        return contacts_name;

    for(uint8_t idx = 0; idx < num; idx++)
    {
        bool ret = \
            VmContactsCtxByIdx(idx);
        if(ret == false)
            continue;

        if(strcmp(number, r_contacts.number_str) == 0)
        {
            contacts_name = \
                r_contacts.name_str;

            break;
        }
    }

    return contacts_name;
}

void UpdateContactsVmFlash(void)
{
    VmContactsCtxClear();

    char *name;
    char *number;
    for(u8 i = 0; i < Contacts_Max_Num; i++)
    {
        name = GetContactsName(i);
        number = GetContactsNumber(i);

        w_contacts.check_code = Nor_Vm_Check_Code;
        memcpy(w_contacts.name_str, name, Call_Name_Max_Len);
        memcpy(w_contacts.number_str, number, Call_Number_Max_Len);
        VmContactsCtxFlashSave(i, &w_contacts);
        
        memset(&w_contacts, 0, sizeof(vm_contacts_ctx_t));
    }

    return;
}
 
static bool ProcessValid = \
    false;
bool GetProcessValid(void)
{
    return ProcessValid;
}
void SetProcessValid(bool valid)
{
    ProcessValid = valid;
    return;
}

void CallOutOrInProcess(void)
{
    bool valid = false;
    SetProcessValid(valid);

    bool BondFlag = GetDevBondFlag();
    if(BondFlag == false)
        return;

    //判断当前是否符合弹出的条件
    if(!MenuSupportPopup())
        return;

    /*勿扰模开启*/
    int dnd_state = \
        GetVmParaCacheByLabel(vm_label_dnd_state);
    if(dnd_state == dnd_state_enable) return;

    u8 OutOrIn = \
        GetCallOutOrIn();
    
    /*来电马达震动*/
    if(OutOrIn == 2)
        motor_run(3, def_motor_duty);

    /*响铃*/
    //...

    if(OutOrIn == 1)
        ui_menu_jump(ui_act_id_call_out); 
    else if(OutOrIn == 2)
        ui_menu_jump(ui_act_id_call_in); 

    valid = true;
    SetProcessValid(valid);  

    return;
}

void CallHangUpAfterHandle(void)
{
    bool valid = \
        GetProcessValid();
    if(valid == false)
        return;

    CallOnlineDurationTimerDelete();

    ui_act_id_t act_id = \
        read_menu_return_level_id();

    bool state = \
        GetCallAnswerState();
    if(state == true)
        act_id = ui_act_id_call_end;

    ui_menu_jump(act_id);

    return;
}

void CallAnswerAfterHandle(void)
{
    bool valid = \
        GetProcessValid();
    if(valid == false)
        return;

    ui_menu_jump(ui_act_id_call_online);

    return;
}

static u16 online_timer_id = 0;
static u32 online_duration = 0;
u16 GetCallOnlineTimerId(void)
{
    return online_timer_id;
}
u32 GetCallOnlineDuration(void)
{
    return online_duration;
}

static void timer_cb(void *priv)
{
    if(online_duration < 0xffffffff)
        online_duration++;

    return;
}
void CallOnlineDurationTimerCreate(void)
{
    if(online_timer_id)
        return;

    online_timer_id = \
        sys_hi_timer_add(NULL, timer_cb, 1000);

    online_duration = 0;

    return;
}
void CallOnlineDurationTimerDelete(void)
{
    if(online_timer_id)
        sys_hi_timer_del(\
            online_timer_id);

    online_timer_id = 0;

    return;
}