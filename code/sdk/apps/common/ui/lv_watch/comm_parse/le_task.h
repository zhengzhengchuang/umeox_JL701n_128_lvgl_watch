#ifndef __LE_TASK_H__
#define __LE_TASK_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "../include/ui_menu.h"

#define Le_Task_Name "le_task"

enum
{
    Le_Set_UtcTime,
    Le_Set_Shutdown,
    Le_Set_Restart,
    Le_Set_Reset,
    Le_Req_Exit_Camera,
    Le_Dnd_Update,
    Le_Find_Dev,
    Le_User_Info_Update,
    Le_GalgoKey_Update,
    Le_First_Dev_Bond,
    Le_Ori_Dev_Bond,
    Le_New_Dev_Bond,
    Le_Dev_UnBond,
    Le_Notify_Push,
    Le_ANCSSw_Update,
    Le_LL_Info_Update,
    Le_Weather_Update,
    Le_Sed_Info_Update,
    Le_Remote_Enter_Camera,
    Le_Remote_Exit_Camera,
    Le_PTime_Cfg_Para_Update,
    Le_Call_Contacts_Clear,
    Le_Call_Contacts_Update,

    Le_History_Hr_Data,
    Le_History_Bo_Data,
    Le_History_Pedo_Data,
};

void le_task_create(void);
int post_le_task_msg(int *msg, u8 len);
void le_task_msg_handle(int *msg, u8 len);

void le_task_post(int type, int *para, int len);
#ifdef __cplusplus
} /* extern "C" */
#endif

#endif 
